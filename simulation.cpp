#include <global_root.h>
#include "simulation.h"
#include "scene.h"
#include "entity.h"
#include "statistics.h"
#include <future>
#include "experiment.h"




void Simulation::Reset(const GridSize&size, bool useControl, int64_t simplexSeed, count_t numLayers,  const ArrayRef<Entity>&initialState)
{
	this->simplexNoise.Recreate(simplexSeed);
	this->random = RNG<std::mt19937_64>(simplexSeed);
	this->gridSize = size;
	this->useControl = useControl;
	this->forceRecovery = forceRecovery;

	DB::Reset(size);

	simulated.Reset(size,numLayers,initialState);
	if (useControl)
	{
		control.Reset(size,1,initialState);
		control.lockAutoTrim = true;

		#ifdef D3
			foreach (simulated.layers,layer)
				foreach (layer->shardGrid.Horizontal(),x)
					foreach (layer->shardGrid.Vertical(),y)
						foreach (layer->shardGrid.Deep(),z)
							layer->shardGrid.Get(x,y,z).consistentMatch = &control.layers.First().shardGrid.Get(x,y,z);
		#else
			foreach (simulated.layers,layer)
				foreach (layer->shardGrid.Horizontal(),x)
					foreach (layer->shardGrid.Vertical(),y)
						layer->shardGrid.Get(x,y).consistentMatch = &control.layers.First().shardGrid.Get(x,y);
		#endif
	}

	DB::AssertIsWellDefined();


	timeSpentSimulating = 0;
	numIterations = 0;
	sceneOutdated = true;
	lastSceneChange = timer.Now();
	deadNodes = 0;
	recoveringNodes = 0;
	nodeDeathGeneration = 0;
	revivalDepth = 0;
	revivalCount = 0;

	totalRecoveryChances = 0;
	totalRecoveryStats = Grid::TCStat();






	#if defined DBG_SHARD_HISTORY && defined RECOVERY_TEST
		FILE*f = fopen("history.bin","rb");
		if (f)
		{
			size_t t = sizeof(Shard::HistoryElement);
			foreach (simulated.layers.First().shardGrid,s)
			{
				index_t len;
				size_t at = ftell(f);
				fread(&len,sizeof(len),1,f);
				s->recordedHistory.AppendRow(len);

				for (index_t i = 0; i < len; i++)
				{
					index_t len2;
					fread(&len2,sizeof(len2),1,f);
					s->recordedHistory[i].setLength(len2);
					fread(s->recordedHistory[i].mutablePointer(),len2,1,f);
				}
			}
			fclose(f);
		}
	#endif
}

#if defined DBG_SHARD_HISTORY && defined RECOVERY_TEST
void WriteHistory(const Grid&grid)
{
	FILE*f = fopen("history.bin","w+b");
	foreach (grid.layers.First().shardGrid,s)
	{
		const index_t len = s->newHistory.Count();
		fwrite(&len,sizeof(len),1,f);
		for (index_t i = 0; i < len; i++)
		{
			const index_t len2 = s->newHistory[i].length();
			fwrite(&len2,sizeof(len2),1,f);
			fwrite(s->newHistory[i].c_str(),len2,1,f);
		}
	}
	fclose(f);
}
#endif

void	Simulation::AssertIsRecovering()
{
	this->simulated.AssertIsRecovering();
}

count_t Simulation::Recover(Shard&s)
{
	ASSERT__(s.liveliness.state == Shard::TLiveliness::Recovering);

	bool any = false;

	s.VerifyIntegrity();

	count_t rs = 0;


	const index_t currentGen = this->simulated.topGeneration;

	volatile bool isNew = false;
	
	s.user2 ++;

	if (s.sds.IsEmpty())
	{
		isNew = true;
		bool found = false,couldFind=false;
		index_t gen;
		for (gen = currentGen; gen <= currentGen; gen--)
		{
			#ifdef DBG_SHARD_HISTORY
				s.LogEvent("Recovery: Checking generation "+String(gen));
			#endif
			if (s.client.SDSExists(gen) /*|| s.client.SDSIsUploading(gen)*/)
			{
				#ifdef DBG_SHARD_HISTORY
					s.LogEvent("Recovery: Generation exists in DB: "+String(gen));
				#endif
				found = true;
				couldFind = true;
				break;
			}
		}
//		ASSERT__(couldFind);
		if (!found)
		{
//			FATAL__("unwanted");
			volatile count_t newest = DB::NewestKnownSDSGeneration(s.gridCoords);
			ASSERT__(s.client.SDSExists(newest));
			s.VerifyIntegrity();
			return 0;
		}
		s.user2 = 0;
		auto&sds = s.sds.Append();
		sds.InitGeneration(gen,currentGen,false,CLOCATION,CLOCATION);
		sds.SetOpenEdgeRCS(s);
		ASSERT__(s.client.SDSExists(s.sds.First().GetGeneration()));

		rs = currentGen - gen + 1;

		for (index_t g = gen+1; g <= currentGen; g++)
		{
			s.sds.Append().InitGeneration(g,currentGen,false,CLOCATION,CLOCATION);
			s.sds.Last().SetOpenEdgeRCS(s);
		}
		
	}
	if (this->simulated.layers.Count() == 1 && !s.sds.First().GetOutput())
		s.client.AssertSDSExists(s.sds.First().GetGeneration());

	while (s.sds.Last().GetGeneration() < simulated.topGeneration)
	{
		index_t gen = s.sds.Last().GetGeneration()+1;
		s.sds.Append().InitGeneration(gen,currentGen,false,CLOCATION,CLOCATION);
		s.sds.Last().SetOpenEdgeRCS(s);
	}

	bool canRecover = true;
	for (;;)
	{
		bool repeat = false;
		canRecover = false;
		foreach (s.sds,sds)
		{
			if (!sds->GetOutput() || !sds->GetOutput()->IsFullyConsistent())
			{
				auto rs = sds->DownloadOutput(s);
				if (rs == DB::Result::CannotDownload)
				{
					//outdated
					if (!sds->GetOutput())
					{
						ASSERT__(s.recoveryAt == InvalidIndex);
						s.sds.Erase(0,sds - s.sds.pointer()+1);
						ASSERT__(s.sds.IsNotEmpty());
						repeat = true;
						break;
					}
				}
				elif (rs == DB::Result::Downloaded)
				{
					canRecover = true;
					ASSERT__(s.recoveryAt == InvalidIndex);
					s.sds.Erase(0,sds - s.sds.pointer());
					ASSERT__(s.sds.First().GetOutput()->IsFullyConsistent());
					repeat = true;
					break;
				}
				elif (rs == DB::Result::Requested)
				{
					if (s.client.SDSExists(sds->GetGeneration()))
						canRecover = true;
				}
			}
			else
				canRecover = true;
		}
		if (!repeat)
			break;
	}
	ASSERT__(canRecover);

	if (s.sds.Count() > 1)
		for (index_t n = 0; n < NumNeighbors; n++)
		{
			const index_t inbound = s.neighbors[n].inboundIndex;
			Shard*other = s.neighbors[n].shard;
			if (other)
			{
				for (index_t i = 1; i < s.sds.Count(); i++)
				{
					auto&rcs = s.sds[i].inboundRCS[inbound];
					if (rcs)
						continue;
					ASSERT__(rcs != edgeInboundRCS);
					s.client.DownloadInbound(other,s.sds[i].GetGeneration(),rcs);

				}
			}
		}
	s.VerifyIntegrity();


	if (this->simulated.layers.Count() == 1 && !s.sds.First().GetOutput())
		s.client.AssertSDSExists(s.sds.First().GetGeneration());


	if (!s.sds.First().GetOutput())
	{
		return rs;
	}



	
	if (s.sds.Last().GetOutput() &&  s.sds.Last().GetGeneration() == simulated.topGeneration)
	{
		s.liveliness.state = Shard::TLiveliness::Alive;
		s.VerifyIntegrity();
		return rs;
	}


	for (index_t i = 0; i < 2; i++)	//must be more than one
	{
		index_t offset = s.NewestDefinedSDSIndex();
		ASSERT__(offset != InvalidIndex);
		if (offset+1 >= s.sds.Count())
			s.sds.Append();
		SDS&next = s.sds[offset+1];
		s.sds[offset].PrecomputeSuccessor(s,next,simulated.GetBoundaries(),currentGen,CLOCATION);
		next.FinalizeComputation(s,CLOCATION);
		ASSERT__(next.GetOutput());

		if (next.GetGeneration() == simulated.topGeneration)
		{
			s.liveliness.state = Shard::TLiveliness::Alive;
			s.VerifyIntegrity();
			return rs;
		}

	}
	s.VerifyIntegrity();

	return rs;
}

bool Simulation::IsAsync() const
{
	return future.valid();
}


count_t		Simulation::CountNonFullyAvailableNodes()
{
	count_t rs = 0;
	simulated.ShardIterative([&rs](const Grid::Layer&,const Shard&s)
	{
		if (!s.IsFullyAvailable())
			rs ++;

	});
	return rs;
}


Simulation::Result		Simulation::RunIterationAsync(const TExperiment&exp, bool correctionIterationsAffectFaultProbability)
{
	if (future.valid())
	{
		auto rs = future.wait_for(std::chrono::seconds(0));
		if (rs != std::future_status::ready)
			return Result::Delayed;
		bool b = future.get();
		future = std::future<bool>();


		#if defined DBG_SHARD_HISTORY && defined RECOVERY_TEST
			if (GetTopGeneration() >= 60)
			{
				WriteHistory(simulated);
				exit(0);
			}
		#endif
		#ifdef PERFORMANCE_TEST_TIMESTEPS
			if (GetTopGeneration() >= PERFORMANCE_TEST_TIMESTEPS)
			{
				TerminateProcess(GetCurrentProcess(),0);	//self-terminate
			}
		#endif




		return b ? Result::Okay : Result::Failed;
	}
	if (!sceneOutdated)
		lastSceneChange = timer.Now();
	sceneOutdated = true;
	future = std::async(std::launch::async, [=]() { return RunIteration(exp,correctionIterationsAffectFaultProbability); });
	return Result::Delayed;
}


void Simulation::ProcessNodeDeaths(float reliability)
{
	//if (reliability < 1)
	{
		recovering.Clear();
		simulated.ShardIterative([reliability,this](Grid::Layer&, Shard&s)
		{
			const float x = float(s.gridCoords.x)+0.5f;
			const float y = float(s.gridCoords.y)+0.5f;
			const float z = float(simulated.topGeneration)+0.5f;
			#if defined ERRORS_IN_CENTRAL_NODE_ONLY
				const float s0 = s.gridCoords == gridSize/2;
				const float s1 = 0;

			#else
				const volatile float s0 = this->random.NextFloat();
				const volatile float s1 = this->random.NextFloat();
			#endif
			if (s0 > reliability)
			{
				s.Die(simulated.topGeneration);
				s.liveliness.userValue = nodeDeathGeneration;
			}
			elif (s.IsDead())
			{
				index_t age = nodeDeathGeneration - s.liveliness.userValue;
				float pRecovering = 1.f - exp(-0.125f * float(age));		//exponential CDF (lambda=0.125)
				if (s1 <= pRecovering)
				{
					//assume infinite nodes here
					/*ptrdiff_t cnt = this->extraNodes--;
					if (cnt >= 0)*/
					{
						//have allocated extra node

						s.client.SignalStartup();
						s.liveliness.state = Shard::TLiveliness::Recovering;
					}
				}
				deadNodes++;
			}
			elif (s.liveliness.state == Shard::TLiveliness::Recovering)
			{
				recovering << &s;
				recoveringNodes++;
			}
		});




		Concurrency::parallel_for(index_t(0),recovering.Count(),[this](index_t i)
		{
			count_t d = Recover(*recovering[i]);
			if (d)
			{
				revivalDepth += d; 
				revivalCount++;
			}
		});




		nodeDeathGeneration++;

	}

	Database::ProcessDistribution();


}

void Simulation::RecoverOne(const TExperiment&exp, bool correctionIterationsAffectFaultProbability)
{
	//ASSERT__(!IsAsync());
	lastRecoveryStats = simulated.LockstepCorrectSome(exp,
		correctionIterationsAffectFaultProbability ? 
		[exp,this]()
		{	
			ProcessNodeDeaths(exp.GetReliability());
		} : std::function<void()>()
		,4
	);

	totalRecoveryChances ++;
	totalRecoveryStats += lastRecoveryStats;



	if (!sceneOutdated)
	{
		sceneOutdated = true;
		lastSceneChange = timer.Now();
	}
}


bool Simulation::RunIteration(const TExperiment&exp, bool correctionIterationsAffectFaultProbability)
{

	Timer::Time t0 = timer.Now();

	if (this->extraNodes < 0)
		this->extraNodes = 0;

	if (useControl)
	{
		doNotVerify = true;
		Database::Lock();
		control.EvolveTop();
		Database::Unlock();
		doNotVerify = false;
	}
	simulated.EvolveTop();
	ProcessNodeDeaths(exp.GetReliability());

	if (useControl)
	{
		const index_t gen = GetTopGeneration();


		#if 0
		Array2D<count_t>	inconsistencyCounter(IC::Resolution,IC::Resolution);
		Array2D<count_t>	totalCounter(IC::Resolution,IC::Resolution);
		foreach (simulated.layers,layer)
		{
			for (index_t x = 0; x < width; x++)
				for (index_t y = 0; y < height; y++)
				{
					inconsistencyCounter.Fill(0);
					totalCounter.Fill(0);
					const SDS&approx = layer->shardGrid.Get(x,y).sds.Last();
					if (approx.GetOutput()->coverage.IsFullyConsistent())
						continue;
					const SDS&correct = control.layers.First().shardGrid.Get(x,y).sds.Last();
					foreach (approx.GetOutput()->entities,e)
					{
						int2 at = IC::ToPixels(Frac(e->coordinates));
						totalCounter.Get(at.x,at.y)++;
						
						const auto*e2 = correct.GetOutput()->entities.FindEntity(e->guid);
						if (e2 && (*e2) == *e)
						{
							totalCounter.Get(at.x, at.y)++;	//count fully consistent twice
							continue;
						}
						ASSERT1__(approx.GetOutput()->coverage.GetPixelInconsistency(at.x,at.y)>0,at.ToString());
						inconsistencyCounter.Get(at.x,at.y)++;
						if (e2)
						{
							int2 at = IC::ToPixels(Frac(e2->coordinates));
							ASSERT1__(approx.GetOutput()->coverage.GetPixelInconsistency(at.x,at.y)>0,at.ToString());
							inconsistencyCounter.Get(at.x,at.y)++;
							totalCounter.Get(at.x,at.y)++;
						}
					}
					foreach (correct.GetOutput()->entities,e)
					{
						const auto*e2 = approx.GetOutput()->entities.FindEntity(e->guid);
						if (!e2)
						{
							int2 at = IC::ToPixels(Frac(e->coordinates));
							ASSERT1__(approx.GetOutput()->coverage.GetPixelInconsistency(at.x,at.y)>0,at.ToString());
							inconsistencyCounter.Get(at.x,at.y)++;
							totalCounter.Get(at.x,at.y)++;
							if (!layer->FindFirstEntity(e->guid,approx.GetGeneration()))
							{
								//lost entity: count twice
								inconsistencyCounter.Get(at.x, at.y)++;
								totalCounter.Get(at.x, at.y)++;
							}
						}
					}

					for (UINT32 x= 0; x < IC::Resolution; x++)
						for (UINT32 y = 0; y < IC::Resolution; y++)
						{
							count_t inconsistent = inconsistencyCounter.Get(x,y);
							count_t total = totalCounter.Get(x,y);
							const IC::TSample&pixelInc = approx.GetOutput()->coverage.GetSample(x,y);
							if (pixelInc.IsInvalid())
								continue;
							if (pixelInc.IsConsistent())
							{
								ASSERT_EQUAL__(inconsistent,0);
								continue;
							}
							Statistics::Include(pixelInc, inconsistent,total);

						}

				}

		}
		#endif
	}


	RecoverOne(exp, correctionIterationsAffectFaultProbability);

		//std::numeric_limits<count_t>::max());
	//simulated.DeliverAll();
	if (useControl)
		control.TrimTo(simulated);
	timeSpentSimulating += timer.Now() - t0;
	numIterations++;
	if (useControl)
	{
		simulated.CompareConsistent(control);
		//Input and GetOutput() declared consistent.
		//Input-hash mismatch in Verify, shard 0,0, layer 0, generation 705, oldest SDS in both.
		//Happens in OptimizedDebug, not in Debug
		Verify();
	}
	//else
	//	simulated.TrimTo(simulated.topGeneration);

	Database::AdvanceTimestep(simulated.topGeneration);
//	#ifdef D3
	bool cont = (simulated.topGeneration - simulated.oldestLivingGeneration < 20);
	//#else
	//	bool cont = (simulated.topGeneration - simulated.oldestLivingGeneration < 50);
	//#endif

	//if (!cont && simulated.layers.Count() == 2)
	//{
	//	bool any = false;
	//	foreach (simulated.layers,layer)
	//		foreach (layer->shardGrid,shard)
	//		if (shard->sds.IsNotEmpty() && shard->sds.First().GetGeneration() == simulated.oldestLivingGeneration)
	//		{
	//			any = true;
	//			__debugbreak();
	//		}
	//	ASSERT__(any);
	//}

	Statistics::ExportMergeResults();

	return cont;
/*
	bool brk = false;
	simulated.ShardParallel([&brk](Grid::Layer&, Shard&s)
	{
		if (s.IsFullyAvailable() && s.sds.Last().GetOutput()->ic.GetHighestInconsistency() == IC::MaxDepth)
			brk = true;
	});

	count_t inconsistent=0;
	simulated.ShardIterative([&inconsistent](Grid::Layer&, Shard&s)
	{
		if (!s.IsFullyAvailable() || !s.sds.Last().GetOutput()->IsFullyConsistent())
			inconsistent++;
		else
		{
			volatile bool brk = true;
		}
	});
	brk = inconsistent == simulated.layers.First().shardGrid.Count() * simulated.layers.Count();


	if (forceRecovery)
	{
		simulated.ShardParallel([](Grid::Layer&, Shard&s)
		{
			if (!s.IsFullyAvailable() || s.sds.Last().GetOutput()->ic.GetHighestInconsistency() == 255)
				s.faultFree = true;
			elif (s.sds.Last().GetOutput()->ic.GetHighestInconsistency() == 0)
				s.faultFree = false;
		});
	}



	return !brk;*/
}



index_t Simulation::GetTopGeneration() const
{
	return simulated.topGeneration;
}

index_t Simulation::GetOldestGeneration() const
{
	return simulated.oldestLivingGeneration;
}

count_t Simulation::CountCurrentEntities() const
{
	return useControl ? control.layers.First().numCurrentEntities : simulated.layers.First().numCurrentEntities;
}

count_t Simulation::CountInconsistentSDS() const
{
	count_t rs = 0;
	foreach (simulated.layers.First().shardGrid,s)
	{
		foreach (s->sds,sds)
			if (!sds->GetOutput()->IsFullyConsistent())
				rs++;
	}
	return rs;
}

count_t Simulation::CountConsistentTopGenerationSDS() const
{
	count_t rs = 0;
	foreach (simulated.layers.First().shardGrid,s)
	{
		if (s->sds.Last().GetOutput()->IsFullyConsistent())
		{
			rs ++;
		}
	}
	return rs;
}

count_t Simulation::CountUnconfirmedTopLevelUpdates() const
{
	count_t rs = 0;
	foreach (simulated.layers.First().shardGrid,s)
	{
		foreach (s->sds.Last().outboundRCS,ref)
			if (!ref->confirmed)
				rs ++;
	}
	return rs;
}


namespace Build
{


	#ifdef D3

		void	BuildShard(const Shard&shard, const index_t layer, const count_t highestSDSCount)
		{

			if (shard.IsFullyAvailable() && shard.sds.Last().GetOutput()->IsFullyConsistent())
				return;	//don't care

			//apart from entities two metrics are relevant here: SDS count, and highest inconsistency
			//this stage should also visualize connectivity to neighbor SDS's

			
			
			count_t delta = highestSDSCount - shard.sds.Count();
			float m = std::min<float>(highestSDSCount,10);
			float rel = std::max(0.f,float(shard.sds.Count())+m-highestSDSCount-1)/ 10;

			static const float TotalScale = 0.95f;
			const float myScale = TotalScale;
			
			float scale = 1.f;// (0.125f + (0.75f * rel));
			//ASSERT_LESS_OR_EQUAL__(scale, 1.f);

			float offset = (0.5f) - 0.5f * TotalScale;
			float3 center = float3(shard.gridCoords)+float3(offset,0,0);

			M::Box<> outer,inner,comm;
			outer.SetCenter(center,0.5f*myScale);
			inner.SetCenter(center,scale*0.5f*myScale);
			comm.SetCenter(center,0.45f*myScale);

			Scene3D::SetColor(1,0,0);

			if (shard.IsDead())
				Scene3D::PutWireframe(outer);

			Scene3D::SetColor(4.f * rel,rel,rel*0.2f);
			Scene3D::PutSolid(inner);

			Scene3D::SetColor(1, 0, 0);
			foreach (shard.neighbors,n)
			{
				if (shard.sds.IsEmpty() || !shard.sds.Last().outboundRCS[n-shard.neighbors.begin()].confirmed)
				{
					M::Box<> comm2 = comm;
					comm2.Translate(comm.GetExtent()&float3(n->delta));
					comm2.ConstrainBy(outer);
					Scene3D::PutSolid(comm2);
				}
			}
		}

		void	BuildGrid(const Grid&grid)
		{
			Scene3D::Clear();

			count_t highest = 0;
			foreach (grid.layers,layer)
				foreach (layer->shardGrid,s)
				{
					highest = M::Max(highest,s->sds.Count());
				}

			for (index_t i = 0; i < grid.layers.Count(); i++)
			{
				const auto&layer = grid.layers[i];

				const count_t w = layer.shardGrid.GetWidth();
				const count_t h = layer.shardGrid.GetHeight();
				const count_t d = layer.shardGrid.GetDepth();

				float hOffset = (float(i) + float(grid.layers.Count())*-0.5f) * w * 1.3f;

				Scene3D::SetOffset(/*-0.5f*w - */hOffset, -0.5f*h, -0.5f*d);
				Scene3D::SetColor(0.5f,0.5f,0.5f);

				Scene3D::PutLine(float3(-0.5f, -0.5f, -0.5f), float3(-0.5f, -0.5f, d - 0.5f));
				Scene3D::PutLine(float3(-0.5f, -0.5f, -0.5f), float3(-0.5f, h - 0.5f, -0.5f));
				Scene3D::PutLine(float3(-0.5f, -0.5f, -0.5f), float3(w - 0.5f, -0.5f, -0.5f));
			
				Scene3D::PutLine(float3(w - 0.5f, -0.5f, -0.5f), float3(w - 0.5f, -0.5f, d - 0.5f));
				Scene3D::PutLine(float3(w - 0.5f, -0.5f, -0.5f), float3(w - 0.5f, h - 0.5f, -0.5f));

				Scene3D::PutLine(float3(-0.5f, h - 0.5f, -0.5f), float3(-0.5f, h - 0.5f, d - 0.5f));
				Scene3D::PutLine(float3(-0.5f, h - 0.5f, -0.5f), float3(w - 0.5f, h - 0.5f, -0.5f));

				Scene3D::PutLine(float3(-0.5f, -0.5f, d-0.5f), float3(-0.5f, h - 0.5f, d-0.5f));
				Scene3D::PutLine(float3(-0.5f, -0.5f, d-0.5f), float3(w - 0.5f, -0.5f, d-0.5f));

				Scene3D::PutLine(float3(w-0.5f, h-0.5f, -0.5f), float3(w-0.5f, h-0.5f, d - 0.5f));
				Scene3D::PutLine(float3(-0.5f, h-0.5f, d-0.5f), float3(w-0.5f, h - 0.5f, d-0.5f));
				Scene3D::PutLine(float3(w-0.5f, -0.5f, d-0.5f), float3(w - 0.5f, h-0.5f, d-0.5f));




				//index_t layer = (l - grid.layers.pointer());
				foreach (layer.shardGrid,s)
					BuildShard(*s,i,highest);
			}
		}	
		


	#else


	void SetEntityColor(const Entity&e)
	{
		Scene::SetColor(e.color.r,e.color.g,e.color.b,1.f);
	}

	void RenderEntitySphere(const M::TVec2<>&coords, float scale)
	{
		Scene::PushOffset();
		Scene::AlterOffset(coords);
		Scene::PutSphere(scale * 0.02f);
		Scene::PopOffset();
	}
	void RenderEntityShape(const M::TVec2<>&offset, const EntityAppearance&app,float scale, bool renderRanges, const M::TVec4<>&color, bool colorRanges)
	{
		Scene::PushOffset();
		Scene::AlterOffset(offset);
		Scene::AlterZOffset(app.zOffset);
		if (renderRanges)
		{
			if (colorRanges)
				Scene::SetColor(color);
			else
				Scene::SetColor(0.5f,0.5f,0,0.25f*color.a);
			#ifdef NO_SENSORY
				Scene::PutCircle(Entity::MaxInfluenceRadius);
			#else
				Scene::PutCircle(Entity::MaxAdvertisementRadius);
				if (Entity::MaxAdvertisementRadius != Entity::MaxMotionDistance)
				{
					Scene::SetColor(0,0.5f,0,0.25f);
					Scene::PutCircle(Entity::MaxMotionDistance);
				}
			#endif
		}
		Scene::SetColor(color);
		switch (app.shape)
		{
			case Entity::Shape::LineCube:
//				if (app.scale.z == 0)
					Scene::PutRotatedOutlineRect(app.orientation.GetAngle(),app.scale*scale);
				//else
				//	Scene::PutRotatedOutlineCube(app.orientation,app.scale*scale);
			break;
			case Entity::Shape::SolidCube:
				Scene::PutRotatedFilledRect(app.orientation.GetAngle(),app.scale*scale);
			break;
			case Entity::Shape::None:
			{
				float2 d;
				app.orientation.ToVector(d);
				Scene::PutLineTo(d*0.025f);
				Scene::PutLineTo(d.Normal()*0.0125f);
				Scene::PutLineTo(d.Normal()*-0.0125f);
			}
			break;
			case Entity::Shape::Sphere:
				Scene::PutSphere(scale*0.5f * 0.02f);
			break;
		}
		Scene::PopOffset();
	}


	void RenderEntity(const Entity&e, float scale, bool overrideColor=false, const M::TVec4<>&color = M::Vector4<>::zero)
	{
		M::TVec2<> coords;
		coords.x = M::Frac(e.coordinates.x);
		coords.y = M::Frac(e.coordinates.y);
		RenderEntityShape(coords, e,scale, /*e.logic.IsNotEmpty()*/ overrideColor, overrideColor ? color : float4(e.color,1.f),overrideColor);
	}



	void RenderEntityDelta(const Entity&e, const Grid::Layer&correct, index_t generation)
	{
		const Entity*e2=nullptr;
		foreach (correct.shardGrid,s)
		{
			const auto*gen = s->FindGeneration(generation);
			if (!gen)
				continue;
			e2 =gen->GetOutput()->entities.FindEntity(e.guid);
			if (e2)
				break;
		}
		if (e2 && Vec::equal(e2->coordinates,e.coordinates))
			return;
		{
			//approx.VerifyIsInconsistent(e.coordinates,generation,ToString(e.guid)+ ", actual location");
			RenderEntity(e,e2==nullptr? 2.f:1.5f,true,float4(0.5,0,0,1));
		}

		//glBegin(GL_LINES);
		if (e2)
		{
			//approx.VerifyIsInconsistent((*f)->coordinates,generation,ToString(e.guid)+ ", approximate location");
			M::TVec2<> p = {M::Frac(e.coordinates.x), M::Frac(e.coordinates.y)};
			M::TVec2<> p2 = {(e2)->coordinates.x - e.coordinates.x,
						(e2)->coordinates.y - e.coordinates.y};
			Scene::PushOffset();
			Scene::AlterOffset(p);
			Scene::PutLineTo(p2);
			Scene::PopOffset();
		}
	}

	
	void RenderGenerationDelta(const Shard&approx, const Grid::Layer&correct, index_t generation)
	{
		//Scene::SetColor(1,1,1,0.8f);
		//Scene::PutRegularFilledRect();

		//Scene::SetColor(0,0,0,0.5f);
		//Scene::PutRegularOutlineRect();
		const auto*gen = approx.FindGeneration(generation);
		if (!gen)
			return;
		auto out = gen->GetOutput();
		if (!out)
			return;
		foreach (out->entities,e)
		{
			RenderEntityDelta(*e,correct,generation);
		}
	}

	void RenderFaultEdge(const int2&delta)
	{
		constexpr float ext = 0.05f;
		float y = 0.5f + (0.5f - ext)*delta.y;
		float x = 0.5f + (0.5f - ext)*delta.x;
		if (delta.y == 0)
		{
			Scene::PutFilledRect(M::Rect<>(x-ext,0,x+ext,1));
			return;
		}
		if (delta.x == 0)
		{
			Scene::PutFilledRect(M::Rect<>(0,y-ext,1,y+ext));
			return;
		}
		Scene::PutFilledRect(M::Rect<>(x-ext,y-ext,x+ext,y+ext));

	}



	void BuildGeneration(Grid::Layer&l, index_t g, index_t recoveryIteration, bool isHistory)
	{
		index_t offsetX = l.shardGrid.GetWidth() /2;
		index_t offsetY = l.shardGrid.GetHeight() /2;

		index_t max = 0;
		foreach (l.shardGrid,s)
			max = std::max(max,s->sds.Count());

		for (index_t x = 0; x < l.shardGrid.GetWidth(); x++)
			for (index_t y = 0; y < l.shardGrid.GetHeight(); y++)
			{
				Shard&s = l.shardGrid.Get(x,y);
				auto*sds = s.FindGeneration(g);
				//if (!sds)
				//	continue;


				M::TVec2<> offset;
				offset.x = 1.1f * (int)(x - offsetX);
				offset.y = 1.1f * (int)(y - offsetY);
				Scene::PushOffset();
				Scene::AlterOffset(offset);

				//if (s.mergeCounter > 0)
				//{
				//	if (Scene::GetLayer()==0)
				//	{
				//		Scene::PushOffset();
				//		Scene::AlterOffset(float2(0.5f));
				//			Scene::SetColor(0,0,1,0.25f);
				//			Scene::PutInterlayerBeam(1,0.0625f,1.f);
				//		Scene::PopOffset();
				//	}
				//	s.mergeCounter = 0;
				//}
				//if (s.copyInCounter > 0)
				//{
				//	if (Scene::GetLayer()==0)
				//	{
				//		Scene::PushOffset();
				//		Scene::AlterOffset(float2(0.5f));
				//			Scene::SetColor(0,1,1,1);
				//			Scene::PutInterlayerBeam(1,0.125f,0.5f);
				//		Scene::PopOffset();
				//	}
				//	else
				//	{
				//		Scene::PushOffset();
				//		Scene::AlterOffset(float2(0.5f));
				//			Scene::SetColor(1,1,0,1);
				//			Scene::PutInterlayerBeam(1,0.125f,-0.5f);
				//		Scene::PopOffset();
				//	}
				//	s.copyInCounter = 0;
				//}




				{
					//Scene::SetColor(1,1,1,0.8f);
					switch (s.liveliness.state)
					{
						case Shard::TLiveliness::Alive:
								Scene::SetColor(1,1,1,1);
							//if (s.sds.Last().GetOutput()->IsFullyConsistent())
							//else
								//Scene::SetColor(1,0.75,0.75,1);
						break;
						case Shard::TLiveliness::Dead:
							Scene::SetColor(0,0,0,1);
						break;
						case Shard::TLiveliness::Recovering:
							Scene::SetColor(0.5,1,0.5,1);
						break;
					}
					Scene::PutFilledRect(M::Rect<>(-0.05f,-0.05f,1.05f,1.05f));


					if (!sds)
					{
						Scene::PopOffset();
						continue;
					}


					if (s.liveliness.state == Shard::TLiveliness::Alive && !sds->GetOutput()->IsFullyConsistent())
					{
						const auto&ic = sds->GetOutput()->ic;
						static const float Size = 1.f / float(InconsistencyCoverage::Resolution);
						for (UINT32 x = 0; x < ic.Resolution; x++)
							for (UINT32 y = 0; y < ic.Resolution; y++)
							{
								const auto&sample = ic.GetSample(TGridCoords(x,y));
								if (!sample.IsConsistent())
								{
									const float fx = float(x) * Size;
									const float fy = float(y) * Size;
									float fc = 1.f / (1.f + 0.025f*sample.depth);
									Scene::SetColor(1*fc,0.75*fc,0.75*fc,1);
									Scene::PutFilledRect(M::Rect<>(fx,fy,fx+Size,fy+Size));
								}
							}
					}


					float a = 0.5f + 0.5f / (1.f + (recoveryIteration - sds->lastTouched));



					Scene::SetColor(0,0,0,1);
					//if (sds->outputConsistent)
					//	Scene::SetColor(0,0,0,a);
					//else
					//	if (sds->inputConsistent)
					//		Scene::SetColor(0.5,0.5,0,a);
					//	else
					//		Scene::SetColor(0.5,0,0,a);
					Scene::PutRegularOutlineRect();
					Scene::SetColor(float4(0.5,0,0,1.f));

					const float coreExtent = 0.125f;
					if (s.sds.Count() > 1)
					{
						float relative = float(s.sds.Count()) / float(IC::MaxDepth) * (1.f - coreExtent) * 0.5f;
						Scene::PutFilledRect(M::Rect<>(0.5f-coreExtent-relative,0.5f-coreExtent-relative,0.5f+coreExtent+relative,0.5f+coreExtent+relative));
					}

					//Scene::PutFilledRect(M::Rect<>(0,0,relative,1));
					//Scene::PutFilledRect(M::Rect<>(1.f - relative,0,1,1));
					//Scene::PutFilledRect(M::Rect<>(relative,0,1.f-relative,relative));
					//Scene::PutFilledRect(M::Rect<>(relative,1.f-relative,1.f-relative,1));

					count_t delta = max - s.sds.Count();
					if (delta <= 10)
					{
						float relative = std::min(1.f, float(s.sds.Count()) / 10);
						Scene::SetColor(float4(0.5f+relative,0.5f-relative,0.5f-relative,1.f));
						Scene::PushOffset();
						Scene::AlterOffset(float2(0.5f));
						float indent = (1.f - (1.f / (0.9f + 0.1f * s.sds.Count()))) *0.25f;
						float m = std::min<float>(max,10);
						float h = 0.125f * std::max(0.f,float(s.sds.Count())+m-max-1);
						if (h > 0)
							Scene::PutRotatedFilledCube(0.f,float3(coreExtent,coreExtent,h));
						Scene::PopOffset();
					}
					//Scene::PutFilledQuad(float3(p0,0),float3(p1,0),float3(p1,h),float3(p0,h));
					//Scene::PutFilledQuad(float3(p1,0),float3(p2,0),float3(p2,h),float3(p1,h));
					//Scene::PutFilledQuad(float3(p2,0),float3(p3,0),float3(p3,h),float3(p2,h));
					//Scene::PutFilledQuad(float3(p3,0),float3(p0,0),float3(p0,h),float3(p3,h));

					//for (index_t i = 1; i < s.sds.Count(); i++)
					//	Scene::PutOutlinedRect(M::Rect<>(0,0,1,1).Expand(-0.03f * i));
					Scene::SetColor(0,0,0,1);

					//if (sds->textureOutdated)
					//	sds->LoadCoverageTexture();
					//if (sds->inconsistencyTexture.IsNotEmpty())
					//{
					//	Scene::SetColor(1,1,1,0.5f);
					//	Scene::PutRegularTexturedRect(sds->inconsistencyTexture.Refer(),Image()/* sds->GetOutput()->ic.ic*/);
					//}

					Scene::SetColor(0.5,0,0,0.5);
					foreach (s.neighbors,n)
					{
						if (!sds->outboundRCS[n-s.neighbors.begin()].confirmed)
							RenderFaultEdge(n->delta);
					}


					if (!isHistory && sds->GetOutput() && sds->GetOutput()->entities.Count() < 256)
						foreach (sds->GetOutput()->entities,e)
						{
							RenderEntity(*e,1.5f);
						}
				}
				Scene::PopOffset();
			}
	}

	void	BuildGrid(Grid&grid, float yOffset=0)
	{
		if (grid.layers.IsEmpty())
			return;
		Scene::PushOffset();
		Scene::AlterOffset(float2(0,yOffset));
		foreach (grid.layers,l)
		{
			float c = 0;///*0.5f **/ (grid.topGeneration - grid.oldestLivingGeneration);

			Scene::SetLayer(l - grid.layers.pointer());
	
//			for (index_t g = grid.oldestLivingGeneration; g < grid.topGeneration; g++)
			{
				//BuildGeneration(l,g,grid.recoveryIteration,true);
			}
			//Scene::SetLayer(grid.topGeneration-grid.oldestLivingGeneration);
			BuildGeneration(*l,grid.topGeneration,grid.recoveryIteration,false);
		}
		Scene::PopOffset();
	}

	#endif


	void VerifyEntityDelta(const Entity&e, const Grid::Layer&approx, const Grid::Layer&control, index_t generation)
	{
		const Entity*e2 = nullptr;
		const Shard*s = control.GetShard(e.GetShardCoords());

		//foreach (control.shardGrid,s)
		if (s)
		{
			const auto*gen = s->FindGeneration(generation);
			//if (!gen)
				//continue;
			if (gen)
			{
				e2 =gen->GetOutput()->entities.FindEntity(e.guid);
			}
		}

		//if (e2 && Vec::equal(e2->coordinates,e.coordinates))
		if (e2 && e == *e2)
		{
			ASSERT__(e.omega == 0);
			return;
		}
		InconsistencyCoverage::TVerificationContext ctx;
		ctx.approximate = true;
		ctx.coordinates = e.coordinates;
		ctx.generation = generation;
		ctx.id = e.guid;
		ctx.layer = approx.index;
		approx.VerifyIsInconsistent(ctx);

		if (e2)
		{
			//const Shard*s1 = approx.GetShard(s->gridCoords);
			//const auto*gen = s1->FindGeneration(generation);
			//const Entity*e3 = gen ? gen->GetOutput()->FindEntity(e.guid) : nullptr;
			//if (e3 && Vec::equal(e3->coordinates,e2->coordinates))
			//	return;	//has consistent duplicate

			ctx.approximate = false;
			ctx.coordinates = e2->coordinates;
			ctx.id = e2->guid;

			approx.VerifyIsInconsistent(ctx);
		}
	}


	void VerifyGenerationDelta(const Shard&approx, const Grid::Layer&approxLayer, const Grid::Layer&control, index_t generation)
	{
		const auto*gen = approx.FindGeneration(generation);

		const Shard&correct = control.shardGrid[approx.linearCoords];
		const auto*gen1 = correct.FindGeneration(generation);
		if (!gen)
			return;
		auto out = gen->GetOutput();
		if (!out)
			return;
		if (gen1)
			gen->AssertSelectiveEquality(*gen1,approx.gridCoords);

		//foreach (out->entities,e)
		//{
		//	VerifyEntityDelta(*e,approxLayer,control,generation);
		//}
	}

}


#ifdef INT_MESSAGES
	void	Simulation::DispatchUserMessage(const GUID&target, LogicProcess targetProcess, UINT64 message)
	{
		if (useControl)
			control.DispatchUserMessage(target, targetProcess, message);
		simulated.DispatchUserMessage(target, targetProcess, message);

	}
#else
	void		Simulation::DispatchUserMessage(const GUID&target, LogicProcess targetProcess, const void*payload, size_t payloadSize)
	{
		if (useControl)
			control.DispatchUserMessage(target, targetProcess, payload, payloadSize);
		simulated.DispatchUserMessage(target, targetProcess, payload, payloadSize);
	}
#endif


void Simulation::RebuildIfOudatedFor(float seconds)
{
	if (!sceneOutdated)
		return;
	if (timer.GetSecondsSince(lastSceneChange) > seconds)
		Rebuild();
}

void Simulation::Rebuild()
{
	sceneOutdated = false;

	Scene3D::Clear();
	#ifndef D3
		Scene::Clear();
	#endif
	if (simulated.layers.IsEmpty())
		return;
	Build::BuildGrid(simulated);

	#ifndef D3
	if (!useControl)
		return;


	int h = (int)control.layers.First().shardGrid.GetHeight();
	M::TVec2<int> offset;
	Vec::def(offset,
		control.layers.First().shardGrid.GetWidth() /2,
		h /2
	);
	foreach (simulated.layers,l)
	{
		Scene::SetLayer(l - simulated.layers.pointer());
		foreach (l->shardGrid,s)
		{
			if (s->sds.IsEmpty() || s->sds.First().processed.Count() > 256)
				continue;
			Scene::PushOffset();
				Scene::AlterOffset(float2(1.1f * (s->gridCoords.x - offset.x) ,1.1f * (s->gridCoords.y - offset.y) /*+ 1.1f*h*/));
				//for (index_t g = s->sds.First().GetGeneration(); g <= control.topGeneration; g++)
				{
					Build::RenderGenerationDelta(*s,control.layers.First(),simulated.topGeneration);
				}
			Scene::PopOffset();
		}
	}

	#endif
}

void Simulation::Verify()
{
	if (simulated.layers.IsEmpty())
		return;
	if (!useControl)
		return;
	control.TrimTo(simulated);

	simulated.ShardParallel([this](Grid::Layer&l, Shard&s)
		{
			if (s.sds.IsEmpty())
			{
				ASSERT__(!s.IsFullyAvailable());
				return;
			}
			for (index_t g = s.sds.First().GetGeneration(); g <= control.topGeneration; g++)
			{
				Build::VerifyGenerationDelta(s, l,control.layers.First(),   g);
			}
		}
	);
	
}

void Accumulate(Image16&outImage, BYTE channel, const TEntityCoords&coords)
{
	UINT32 x = M::Min((UINT32)floor(M::Frac(coords.x) * outImage.GetWidth()),outImage.GetWidth()-1);
	UINT32 y = M::Min((UINT32)floor(M::Frac(coords.y) * outImage.GetHeight()),outImage.GetHeight()-1);
	outImage.Get(x,y)[channel]++;
}

void Simulation::AccumulateInconsistentEntities(const TGridCoords&shardCoords, index_t generation, index_t layer, Image16&outImage) const
{
	ASSERT__(useControl);
	const Shard*shard0 = simulated.layers[layer].GetShard(shardCoords);
	const Shard*shard1 = control.layers.First().GetShard(shardCoords);
	ASSERT_NOT_NULL__(shard0);
	ASSERT_NOT_NULL__(shard1);
	const SDS*gen0 = shard0->FindGeneration(generation);
	const SDS*gen1 = shard1->FindGeneration(generation);
	ASSERT_NOT_NULL__(gen0);
	ASSERT_NOT_NULL__(gen1);

	foreach (gen0->GetOutput()->entities,e0)
	{
		::Accumulate(outImage,1,e0->coordinates);
		const Entity*e1 = gen1->GetOutput()->entities.FindEntity(e0->guid);
		if (!e1)
			::Accumulate(outImage,0,e0->coordinates);
		elif (!Vec::equal(e0->coordinates,e1->coordinates))
		{
			::Accumulate(outImage,0,e0->coordinates);
			::Accumulate(outImage,0,e1->coordinates);
		}
	}
	foreach (gen1->GetOutput()->entities,e1)
	{
		::Accumulate(outImage,1,e1->coordinates);
		if (!gen0->GetOutput()->entities.FindEntity(e1->guid))
		{
			::Accumulate(outImage,0,e1->coordinates);
		}
	}
}

void Simulation::AccumulateTraffic(const TGridCoords&shardCoords, index_t layer, index_t neighbor, DataSize&sent, DataSize&confirmed) const
{
	const Shard*shard0 = simulated.layers[layer].GetShard(shardCoords);
	sent += shard0->neighbors[neighbor].dataSent;
	confirmed += shard0->neighbors[neighbor].dataConfirmed;
}


bool		Simulation::GetSDSCoverage(const TGridCoords&shardCoords,index_t generation, index_t layer, InconsistencyCoverage&outResult) const
{
	const Shard*shard = simulated.layers[layer].GetShard(shardCoords);
	if (!shard)
		return false;
	const SDS*gen = shard->FindGeneration(generation);
	if (!gen)
		return false;
	outResult = gen->GetOutput()->ic;
	return true;
}


void		Simulation::VerifyLocationConsistency(const Entity*e, index_t generation, const InconsistencyCoverage&ic) const
{
	if (doNotVerify || !useControl)
		return;


	{
		const Shard*shard = control.layers.First().GetShard(e->GetShardCoords());
		if (shard)
		{
			const SDS*gen = shard->FindGeneration(generation);
			if (gen)
			{
				const Entity*e2 = gen->GetOutput()->entities.FindEntity(e->guid);
				if (e2)
				{
					if (e2->coordinates == e->coordinates)
						return;
					ASSERT__(ic.IsInconsistent(::Frac(e2->coordinates)));
				}
			}
		}
	}


	InconsistencyCoverage::TVerificationContext ctx;
	ctx.coordinates = e->coordinates;
	ctx.shardCoordinates = e->GetShardCoords();
	ctx.generation = generation;
	ctx.id = e->guid;
	ctx.layer = 0;

	ic.VerifyIsInconsistent(::Frac(e->coordinates),ctx);

}


void		Simulation::Accumulate(Shard::TMergeStatistics&stat) const
{
	foreach (simulated.layers.First().shardGrid,s)	//only first layer. second one is mirrored
	{
		stat += s->mergeStats;
	}
}

