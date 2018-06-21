#include <global_root.h>
#include "grid.h"
#include "experiment.h"

void Grid::Layer::Create(const GridSize&size, Grid&g, index_t layer)
{
	index = layer;
	shardGrid.SetSize(size);
	#ifdef D3
		for (index_t x = 0; x < size.width; x++)
			for (index_t y = 0; y < size.height; y++)
				for (index_t z = 0; z < size.depth; z++)
				{
					shardGrid.Get(x,y,z).Setup(g,int3(x,y,z),shardGrid.ToLinearIndexNoCheck(x,y,z),layer);
				}
	#else
		for (index_t x = 0; x < size.width; x++)
			for (index_t y = 0; y < size.height; y++)
			{
				shardGrid.Get(x,y).Setup(g,int2(x,y),shardGrid.ToLinearIndexNoCheck(x,y),  layer);
			}
	#endif
}

const Entity*	Grid::Layer::FindFirstEntity(const GUID&guid, index_t generation) const
{
	foreach(shardGrid, s)
	{
		const SDS*gen = s->FindGeneration(generation);
		if (!gen)
			continue;
		auto out = gen->GetOutput();
		if (!out)
			continue;
		const Entity*e = out->entities.FindEntity(guid);
		if (e)
			return e;
	}
	return nullptr;
}

const Entity*	Grid::Layer::FindFirstEntity(const EntityID&id, index_t generation) const
{
	auto lookFrom = id.GetShardCoords();

	auto s = GetShard(lookFrom);
	if (s)
	{
		const SDS*gen = s->FindGeneration(generation);
		if (gen)
		{
			auto out = gen->GetOutput();
			if (out)
			{
				const Entity*e = out->entities.FindEntity(id.guid);
				if (e)
					return e;
			}
		}
	}
	
	return FindFirstEntity(id.guid,generation);
}

bool	Grid::Layer::IsInconsistent(const TEntityCoords&coordinates, index_t generation) const
{
	TGridCoords at = Entity::GetShardCoords(coordinates);
	
	
	TEntityCoords fr = ::Frac(coordinates);
	const Shard*sh = GetShard(at);
	if (!sh)
		return false;
	const SDS*sds = sh->FindGeneration(generation);
	if (!sds)
		return false;
	return sds->GetOutput()->ic.IsInconsistent(fr);
}

void	Grid::Layer::VerifyIsInconsistent(InconsistencyCoverage::TVerificationContext&ctx) const
{
	ctx.shardCoordinates = Entity::GetShardCoords(ctx.coordinates);
	TEntityCoords fr = ::Frac(ctx.coordinates);
	const Shard*sh = GetShard(ctx.shardCoordinates);
	if (!sh)
		FATAL__("No shard at requested grid coords ("+ctx.ToString()+")");
	const SDS*sds = sh->FindGeneration(ctx.generation);
	if (!sds)
		FATAL__("No SDS at given generation on shard ("+ctx.ToString()+")");

	ctx.sdsIndex = sds - sh->sds.pointer();
	ctx.sdsCount = sh->sds.Count();
	sds->GetOutput()->ic.VerifyIsInconsistent(fr, ctx);
}


Shard * Grid::Layer::GetShard(const TGridCoords&c)
{
	if (c.x >= 0 && (index_t)c.x < shardGrid.GetWidth()
		&& c.y >= 0 && (index_t)c.y < shardGrid.GetHeight()
		#ifdef D3
			&& c.z >= 0 && (index_t)c.z < shardGrid.GetHeight()
		#endif
		)
		#ifdef D3
			return &shardGrid.Get(c.x,c.y,c.z);
		#else
			return &shardGrid.Get(c.x,c.y);
		#endif
	return nullptr;
}

const Shard * Grid::Layer::GetShard(const TGridCoords&c) const
{
	if (c.x >= 0 && (index_t)c.x < shardGrid.GetWidth()
		&& c.y >= 0 && (index_t)c.y < shardGrid.GetHeight()
		#ifdef D3
			&& c.z >= 0 && (index_t)c.z < shardGrid.GetHeight()
		#endif
		)
		#ifdef D3
			return &shardGrid.Get(c.x,c.y,c.z);
		#else
			return &shardGrid.Get(c.x,c.y);
		#endif
	return nullptr;
}


static bool AllConnectedTo(const Shard&s)
{
	if (s.IsDead())
		return false;
	foreach (s.outboundNeighbors,n)
		if (n->shard && n->shard->IsDead())
			return false;
	return true;
}


static bool CopyConsistentGenerationFromSibling(Shard&source, Shard&dest, index_t generation, const TExperiment&exp, index_t currentTimestep)
{
	source.VerifyIntegrity();
	const auto* copy = source.FindGeneration(generation);
	auto*d = dest.FindGeneration(generation);
	ASSERT_NOT_NULL__(d);
	if (copy)
	{
		ASSERT__(copy->GetOutput()->IsFullyConsistent());

		if (currentTimestep - copy->GetOutput()->createdInTimestep >= (count_t)exp.siblingSyncDelay)
		{
			d->OverrideSetOutput(dest,copy->GetOutput(),CLOCATION );	//copy from neighbor
			source.mergeStats.copyOutCounter++;
			dest.mergeStats.copyInCounter++;
			return true;
//			d->significantInboundChange = true;
		}
		else
			return false;
	}
	else
	{
		FATAL__("Sibling does not have requested SDS");
		DB::Result rs = d->DownloadOutput(dest);
	}
	return false;
}

volatile index_t oldest;

volatile bool brk = false;


Grid::TCStat Grid::LockstepCorrectSome(const TExperiment&exp, const std::function<void()>&postIterationCallback, count_t maxDemandRequestsPerShardToDatabase)
{
	VerifyIntegrity();
	recoveryIteration++;
	TCStat rs;

	#if 1
	if (layers.Count() == 2 && exp.maxSiblingSyncOperations > 0)
	{
		auto&grid0 = layers.First().shardGrid;
		auto&grid1 = layers.Last().shardGrid;
		ASSERT_EQUAL__(grid0.Count() , grid1.Count());
		ASSERT_EQUAL__(grid0.GetWidth() , grid1.GetWidth());


		IC::OrthographicComparator comp;




		Concurrency::parallel_for(index_t(0),grid0.Count(), [this,&grid0,&grid1,comp,exp](index_t i)
		{
			auto&s0 = grid0[i];
			auto&s1 = grid1[i];
			ASSERT2__(s0.gridCoords==s1.gridCoords,s0.gridCoords.ToString(),s1.gridCoords.ToString());

			/**
			Protocol:
				Siblings communicate last consistent and newest available generation indexes, producing four regions:

				1: mutually consistent. nothing to do
				2: consistent on one, inconsistent on other: copy entire consistent SDS, if different, flag consistent otherwise.
					alternatively: retrieve from persistent distributed DB
				3: mutually inconsistent: cross-send date, merge on both
					alternatively: send A to B, B merges, returns result
				4: [theoretical]: one SDS inconsistent, one SDS non-existent
			*/



			if (!s0.IsFullyAvailable() || !s1.IsFullyAvailable())
				return;	//at least one node is dead or in recovery mode. No synchronization attempted

			//state: first SDS will be consistent either way. If only one SDS each, nothing to sync
			if (s0.sds.Count() == 1 && s1.sds.Count() == 1)
				return;	//all consistent
			
					
			/*
			Arguably the best approach is to detect the first non-merged mutually inconsistent generation
			*/

			auto&sds0 = s0.NewestConsistentSDS();
			auto&sds1 = s1.NewestConsistentSDS();

			count_t remaining = exp.maxSiblingSyncOperations;
			ASSERT__(remaining > 0);

			if (remaining)
				if (sds0.GetGeneration() < sds1.GetGeneration())
				{
					if (CopyConsistentGenerationFromSibling(s1,s0,sds1.GetGeneration(),exp,topGeneration))
						remaining--;
				}
				else
					if (sds0.GetGeneration() > sds1.GetGeneration())
					{
						if (CopyConsistentGenerationFromSibling(s0,s1,sds0.GetGeneration(),exp,topGeneration))
							remaining--;
					}

			if (remaining)
			{
				auto&sds0 = s0.NewestConsistentSDS();
				auto&sds1 = s1.NewestConsistentSDS();
				const index_t base = std::max(sds0.GetGeneration(),sds1.GetGeneration())+1;
				//same base
				count_t synced = 0;
				for (index_t gen = base; gen <= topGeneration; gen++)
				{
					auto*sds0 = s0.FindGeneration(gen);
					auto*sds1 = s1.FindGeneration(gen);
					if (!sds0 || !sds1)
						break;
					if (sds0->GetOutput() != sds1->GetOutput())
					{
						if (topGeneration - sds0->GetOutput()->createdInTimestep >= (count_t)exp.siblingSyncDelay
							&&
							topGeneration - sds1->GetOutput()->createdInTimestep >= (count_t)exp.siblingSyncDelay)
						{
							sds0->SynchronizeWithSibling(s0,s1, *sds1,comp,topGeneration);

							remaining--;
							if (!remaining)
								break;
						}
					}
				}
			}


		});

		Trim();
	}
	#endif







	UpdateOldestRecoverableGeneration();

	oldest = InvalidIndex;

	foreach (this->layers,l)
	{
		foreach (l->shardGrid,s)
		{
			if (s->IsFullyAvailable())
			{
				oldest = M::Min(oldest,s->sds.First().GetGeneration());
				s->recoveryOperations=0;
			}
			//while (s->recoveryOperations < extra && s->CorrectIsolatedSDS(boundaries,recoveryIteration))
			//{};
		}
	}


	//LockstepCorrectOldest(c);
	for (index_t i = 0; i < (count_t)exp.recoveryIterations; i++)
	{
		rs += LockstepCorrectSelective([i](Shard&s)->index_t
		{
			if (s.sds.Count() <= 1)
				return InvalidIndex;
					
			index_t at = s.NewestConsistentSDSIndex()+1;
			if (at >= s.sds.Count())
				return InvalidIndex;	//happens
			index_t currentGen = s.sds[at].GetGeneration();
			for (;at < s.sds.Count(); at++)
			{
				TSDSCheckResult de = s.sds[at].CheckMissingRCS(s);


				#ifdef DBG_SHARD_HISTORY
				{
					s.LogEvent("Correct.localIndex="+String(at));
					s.LogEvent("Correct.significantInboundChange="+String(s.sds[at].significantInboundChange));
					s.LogEvent("Correct.missingRCS="+String(de.missingRCS));
					s.LogEvent("Correct.outRCSUpdatable="+String(de.outRCSUpdatable));
					s.LogEvent("Correct.predecessorIsConsistent="+String(de.predecessorIsConsistent));
					s.LogEvent("Correct.rcsAvailableFromDatabase="+String(de.rcsAvailableFromDatabase));
					s.LogEvent("Correct.rcsAvailableFromNeighbor="+String(de.rcsAvailableFromNeighbor));
					s.LogEvent("Correct.rcsRestoredFromCache="+String(de.rcsRestoredFromCache));
					s.LogEvent("Correct.thisIsConsistent="+String(de.thisIsConsistent));
					s.LogEvent("Correct.thisGen="+String(s.sds[at].GetGeneration()));
				}
				#endif



				if (s.sds[at].significantInboundChange || de.ShouldRecoverThis())
					break;
			}


			if (at < s.sds.Count())
			{
				#ifdef DBG_SHARD_HISTORY
					s.LogEvent("Suggesting generation g"+String(s.sds[at].GetGeneration())+" @"+String(at)+" for correction");
				#endif
				return s.sds[at].GetGeneration();
			}
			return InvalidIndex;
		} );
		if (postIterationCallback)
			postIterationCallback();
	}
	Trim();




	#if 0
	if (extra)
	{
		foreach (layers,l)
		{
			foreach (l->shardGrid,s)
			{
				s->CorrectSome(extra,boundaries,recoveryIteration);
			}
		}

		Trim();
	}
	#endif /*0*/


	
	if (maxDemandRequestsPerShardToDatabase > 0)
	{
		demandRegistries.SetSize(Sys::ParallelLoop::GetGlobalWorkerCount());
		ShardParallelW([this,maxDemandRequestsPerShardToDatabase](Layer&l, Shard&s, index_t i)	//parallel
		{
			if (s.IsFullyAvailable())
			{
				DownloadDemandRegistry&reg = demandRegistries[i];
				s.RequestHighestDemand(maxDemandRequestsPerShardToDatabase,reg);
			}
		});
	}

	Trim();

	VerifyIntegrity();
	return rs;
}

bool Grid::DeliverGeneration(index_t gen)
{
	bool errors = false;
	ShardParallel([&errors,gen](Layer&layer,Shard&s)	//parallel
		{
			if (!s.IsFullyAvailable())
				return;
			index_t at = s.FindGenerationIndex(gen);
			if (at > 0 && at != InvalidIndex)
			{
				SDS&sds = s.sds[at];
				if (!Deliver(s,sds,layer))
					errors = true;
			}
		}
	);

//	VerifyIntegrity();
	return !errors;
}


bool Grid::DeliverAll(bool requireAll/*=false*/)
{
	bool errors = false;
	ShardParallel([&errors,requireAll](Layer&l, Shard&s)
		{
			if (s.IsFullyAvailable())
				foreach (s.sds,sds)
				{
					if (!Deliver(s,*sds,l,requireAll))
						errors = true;
				}
		}
	);
	Trim();
	return !errors;
}

void Grid::CompareConsistent(const Grid & other) const
{
	for (index_t i = 0; i < layers.Count() && i < other.layers.Count(); i++)
	{
		const Layer	&a = layers[i],
					&b = other.layers.First();
		for (auto it0 = a.shardGrid.begin(), it1 = b.shardGrid.begin(); it0 != a.shardGrid.end() && it1 != b.shardGrid.end(); ++it0, ++it1)
			it0->CompareConsistent(*it1);
	}
}

void Grid::TrimTo(index_t generation)
{
	VerifyIntegrity();
	oldestLivingGeneration = std::min(std::max(oldestLivingGeneration,generation), topGeneration);
	ShardIterative([generation](Layer&l, Shard&s)
	{
		s.TrimTo(generation);
	});
	VerifyIntegrity();

}

void Grid::TrimTo(const Grid&reference)
{
	VerifyIntegrity();
	oldestLivingGeneration = InvalidIndex;
	ASSERT_EQUAL__(1,layers.Count());

	auto*l = layers.pointer();
	index_t lowest = InvalidIndex;
	foreach (l->shardGrid,s)
	{
		foreach (reference.layers,l2)
		{
			ASSERT_EQUAL__(l->shardGrid.GetWidth(),l2->shardGrid.GetWidth());
			ASSERT_EQUAL__(l->shardGrid.GetHeight(),l2->shardGrid.GetHeight());

			const auto*s2 = l2->shardGrid + (s - l->shardGrid.pointer());
			if (s2->sds.IsEmpty())
				continue;	//recoverying
			index_t to = s2->sds.First().GetGeneration();
			lowest = M::Min(lowest,to);
		}
	}
	if (lowest != InvalidIndex)
	{
		TrimTo(lowest);
		oldestLivingGeneration = M::Min(oldestLivingGeneration,lowest);
	}


	//ASSERT_EQUAL__(reference.layers.Count(),layers.Count());
	/*foreach (layers,l)
	{
		const auto*l2 = reference.layers + (l - layers.pointer());
		ASSERT_EQUAL__(l->shardGrid.GetWidth(),l2->shardGrid.GetWidth());
		ASSERT_EQUAL__(l->shardGrid.GetHeight(),l2->shardGrid.GetHeight());
		foreach (l->shardGrid,s)
		{
			const auto*s2 = l2->shardGrid + (s - l->shardGrid.pointer());
			index_t to = s2->sds.First().GetGeneration();
			s->TrimTo(to);
			oldestLivingGeneration = vmin(oldestLivingGeneration,to);
		}
	}*/
	VerifyIntegrity();

}


String ToString(const GUID&guid, bool full/*=false*/)
{
	int v = *(const USHORT*)&guid;
	return full ?  IntToHex(*(const INT64*)&guid,16) : IntToHex(v,4);	//not quite full, but better
	//Vec::toStringD((const BYTE*)&guid,4);
}




namespace DeltaWorks
{
	String ToString(const Hasher::HashContainer&guid, bool full/*=false*/)
	{
		int v = *(const USHORT*)&guid;
		return full ?  IntToHex(*(const INT64*)&guid,16) : IntToHex(v,4);	//not quite full, but better

	}

}


void Grid::VerifyIntegrity() const
{
	ShardParallel([](const Layer&layer, const Shard&s)
	{
		s.VerifyIntegrity();
	});

	foreach (layers,l)
	{
		HashTable<GUID,TGridCoords>	map;

		foreach (l->shardGrid,s)
		{
			if (s->IsFullyAvailable() && s->sds.Last().GetOutput()->IsFullyConsistent())
				foreach (s->sds.Last().GetOutput()->entities,e)
					if (!map.SetNew(e->guid,s->gridCoords))
					{
						FATAL__("Trying to double-register entity "+ToString(e->guid)+" in "+map.Require(e->guid).ToString()+" and "+s->gridCoords.ToString());

					}
			//foreach (s->sds,sds)
			//{
			//	for (int i = 0; i < NumNeighbors; i++)
			//		if (!sds->outboundRCS[i].confirmed)
			//		{
			//			//int2 coords = s->gridCoords + s->->delta;
			//			const Shard*s2 = s->outboundNeighbors[i].shard;
			//			if (!s2)
			//				continue;
			//			//ASSERT_NOT_NULL__(s2);
			//			if (s2->IsDead())
			//				continue;
			//			if (sds->GetOutput()->generation > s2->sds.Last().GetGeneration())
			//				continue;	//can happen if remote node was dead
			//			if (sds->GetOutput()->generation < s2->sds.First().GetGeneration())
			//				continue;	//can happen if local node was dead
			//			const auto*gen = s2->FindGeneration(sds->GetOutput()->generation);
			//			ASSERT_NOT_NULL__(gen);
			//			ASSERT__(!gen->GetOutput()->IsFullyConsistent() || !gen->permanence.isLocallyAvailable);
			//		}
			//}
		}

		//foreach (l->shardGrid,s)
		//{
		//	foreach (s->sds.Last().entities,e)
		//		lostEntities.Unset(e->guid);
		//}
		//Array<GUID>	ar;
		//lostEntities.exportKeys(ar);
		//if (ar.IsNotEmpty())
		//{
		//	GUID track = ar.First();

		//	ChangeSet set;
		//	foreach (l->shardGrid,s)
		//	{
		//		s->sds.Last().cs.Filter(track,set);
		//	}
		//	bool b = true;
		//}			

	}

}

void	Grid::FlagInconsistent(const TEntityCoords&coords)
{
	TGridCoords c = Entity::GetShardCoords(coords);
	TEntityCoords fc = coords - c;
	foreach (layers,l)
	{
		Shard*s = l->GetShard(c);
		if (s)
			s->sds.Last().GetOutput()->ic.FlagInconsistent(fc);
	}

}


//void Grid::CorrectSome(count_t extra)
//{
//	VerifyIntegrity();
//	recoveryIteration++;
//	foreach (layers,l)
//	{
//		Rect<> boundaries (0,0,l->shardGrid.GetWidth()-0.001f,l->shardGrid.GetHeight()-0.001f);
//		foreach (l->shardGrid,s)
//		{
//			s->CorrectSome(extra,boundaries,recoveryIteration);
//		}
//	}
//	Trim();
//}

void Grid::PrecomputeAll(index_t gen)
{
	FATAL__("deprecated");
}

void Grid::FinalizeAll(index_t gen)
{
	FATAL__("deprecated");
	foreach (layers,l)
	{
		foreach (l->shardGrid,s)
		{
			if (s->recoveryAt == InvalidIndex)
				continue;
			s->recoveryAt = InvalidIndex;
			index_t at = s->FindGenerationIndex(gen);
			if (at > 0 && at != InvalidIndex)
			{
				SDS&newState = s->sds[at];

				newState.FinalizeComputation(*s,CLOCATION);
			}
		}
	}
}

/*static*/ bool Grid::Deliver(Shard &shard, SDS &sds, Layer&l, bool require/*=false*/)
{
	bool errors = false;
	for (index_t i = 0; i < sds.outboundRCS.Count(); i++)
	{
		auto&rcs = sds.outboundRCS[i];
		//rcs.VerifyIntegrity();
		/*problem:
		rcs.confirmed: true
		rcs.isSet: true
		rcs.ic: consistent
		rcs.isStoredInDatabase: false
		rcs.isLocallyAvailable: true

		inRCS.confirmed: false;
		inRCS.isSet: true
		inRCS.ic: inconsistent
		inRCS.isStoredInDatabase: false
		inRCS.isLocallyAvailable: true
		*/
		if (rcs.confirmed || !rcs.ref)
			continue;
		Shard*neighbor = shard.outboundNeighbors[i].shard;
		if (!neighbor)
		{
			rcs.confirmed = true;
			continue;
		}
		if (shard.IsDead())
		{
			errors = true;
			ASSERT__(!require);
			break;
		}

		const auto&size = rcs.ref->GetDataSize();
		shard.outboundNeighbors[i].dataSent += size;
		if (neighbor->IsDead())
		{
			errors = true;
			ASSERT__(!require);
			continue;
		}
		shard.outboundNeighbors[i].dataConfirmed += size;
		SDS*other = neighbor->FindGeneration(sds.GetGeneration());
		if (!other)
		{
			if (neighbor->sds.IsEmpty() ||  sds.GetGeneration() < neighbor->sds.First().GetGeneration())
			{
				/**
				This happens if a neighbor has trimmed away fully consistent SDS.
				Even anpartially inconsistent SDS such as the local one can generate neighbor updates
				that are fully consistent, thus not affecting neighbor evolution (yet)
				*/
				//LogUnexpected("Expected remote SDS, but neighbor has advanced",EntityID());
				rcs.confirmed = true;	//no point delivering to neighbor sds that no longer exists
			}
			continue;
		}
		//if (s->gridCoords == int2(1))
			//if (neighbor->gridCoords == int2(2,1))
			//	continue;	//connection error test
		auto&inRCS = other->inboundRCS[i];
		if (inRCS)
			inRCS->VerifyIntegrity(CLOCATION);
		bool significantChange = !inRCS || (!inRCS->ic.IsFullyConsistent() && rcs.ref->ic.IsFullyConsistent());
		ASSERT__(inRCS != edgeInboundRCS);
		inRCS = rcs.ref;
		ASSERT__(inRCS);
		rcs.confirmed = true;
		if (significantChange)
			other->significantInboundChange = true;

		inRCS->VerifyIntegrity(CLOCATION);
	}
	return !errors;
}



void	Grid::SynchronizeLayers()
{
	IC::OrthographicComparator comp;
	for (index_t i = 0; i+1 < layers.Count(); i++)
	{
		Layer	&layer0 = layers[i],
				&layer1 = layers[i+1];
		Concurrency::parallel_for(index_t(0),M::Min(layer0.shardGrid.Count(),layer1.shardGrid.Count()),[this,&layer0,&layer1,comp](index_t j)
		{
			Shard	&s0 = layer0.shardGrid[j],
					&s1 = layer1.shardGrid[j];
			if (!s0.IsFullyAvailable() || !s1.IsFullyAvailable())
				return;

			const index_t	g0 = s0.NewestConsistentSDS().GetGeneration()+1,
							g1 = s1.NewestConsistentSDS().GetGeneration()+1,
							gMax0 = s0.sds.Last().GetGeneration(),
							gMax1 = s1.sds.Last().GetGeneration();
			ASSERT_EQUAL__(gMax0,topGeneration);
			ASSERT_EQUAL__(gMax1,topGeneration);
			//if (gMax0 != gMax1)
			//{
			//	//help recover. I wonder if this even happens
			//	if (gMax0 < gMax1)
			//		s0.sds << *s1.FindGeneration(gMax0+1);
			//	else
			//		s1.sds << *s0.FindGeneration(gMax1+1);
			//	return;
			//}


			const index_t	genFrom = M::Min(g0,g1),
							genMax = gMax0;
			if (genFrom > genMax)
				return;	//nothing to sync
			const index_t syncGen = s0.syncRandomSource.NextIndex(genMax + 1 - genFrom)+genFrom;
			SDS	*sds0 = s0.FindGeneration(syncGen),
				*sds1 = s1.FindGeneration(syncGen);
			ASSERT_NOT_NULL__(sds0);
			ASSERT_NOT_NULL__(sds1);
			sds0->SynchronizeWithSibling(s0,s1,*sds1,comp,topGeneration);
		});

	}

}


static Array<Grid::TCorrectionStatistics>	statistics;

Grid::TCorrectionStatistics	Grid::LockstepCorrectSelective(const std::function<index_t(Shard&)>&baseSelector)
{
	VerifyIntegrity();
	ASSERT__(layers.IsNotEmpty());

	if (statistics.IsEmpty())
		statistics.SetSize(Sys::ParallelLoop::GetGlobalWorkerCount());
	else
		statistics.Fill(TCStat());

	ShardParallel([baseSelector,this](Layer&l, Shard&s)	//parallel
	{
		ASSERT__(s.recoveryAt == InvalidIndex);
		if (!s.IsFullyAvailable())
			return;
		index_t gen = baseSelector(s);
		if (gen == InvalidIndex)
			return;
		s.recoveryAt = s.FindGenerationIndex(gen);
		if (s.recoveryAt == 0)
			s.recoveryAt = InvalidIndex;
	});

	ShardParallel([baseSelector,this](Layer&l, Shard&s)	//parallel
	{
		if (s.recoveryAt != InvalidIndex)
		{
			if (!s.sds[s.recoveryAt-1].GetOutput())
			{
				s.sds[s.recoveryAt-1].permanence.Request();
				return;
			}
			SDS&newState = s.sds[s.recoveryAt];
			if (newState.GetOutput()->IsFullyConsistent())
			{
				return;
			}
			s.recoveryOperations++;

			s.sds[s.recoveryAt-1].PrecomputeSuccessor(s,newState,boundaries,topGeneration,CLOCATION);

			Deliver(s,newState,l);
		}
	});

	

	ShardParallelW([baseSelector,this](Layer&l, Shard&s, index_t workerIndex)
	{
		//index_t at = s->FindGenerationIndex(baseSelector(*s));
		if (s.recoveryAt != InvalidIndex)
		{
			SDS&newState = s.sds[s.recoveryAt];

			const index_t gen = s.sds[s.recoveryAt-1].GetGeneration()+1;

			auto&stat = statistics[workerIndex];
			stat.recoveryOps++;
			newState.FinalizeComputation(s,CLOCATION);
			if (newState.GetOutput()->IsFullyConsistent())
				stat.recoverCompletedSDS++;
			s.user = newState.GetGeneration();
			s.recoveryAt = InvalidIndex;
		}
	});
	TCStat rs;
	foreach (statistics,s)
		rs += *s;
	return rs;

}

void		AssertRecoveringInput(const PRCS&in)
{
	ASSERT__(in);
	in->VerifyIntegrity(CLOCATION);
	ASSERT__(in->ic.IsFullyConsistent());
}

void		AssertRecovering(const SDS::OutRemoteChangeSet&out, const PRCS&in)
{
	ASSERT__(out.ref);
	ASSERT__(out.confirmed);
	ASSERT__(out.ref->ic.IsFullyConsistent());
	AssertRecoveringInput(in);
}

void		AssertRecovering(const SDS*mySDS, const SDS*nSDS, index_t i)
{
	if (mySDS && nSDS)
	{
		if (nSDS->GetOutput()->IsFullyConsistent())
			return;	//don't care. all there
		const auto&out = mySDS->outboundRCS[i];
		const auto&in = nSDS->inboundRCS[i];
		AssertRecovering(out,in);
	}
	elif (mySDS)
	{
		const auto&out = mySDS->outboundRCS[i];
		ASSERT__(out.ref->ic.IsFullyConsistent());
	}
	elif (nSDS)
	{
		if (nSDS->GetOutput()->IsFullyConsistent())
			return;//don't care. all there
		const auto&in = nSDS->inboundRCS[i];
		AssertRecoveringInput(in);
	}
}

void		Grid::AssertIsRecovering()
{
	foreach (layers,l)
	{
		foreach (l->shardGrid,s)
		{
			if (!s->IsFullyAvailable())
				return;	//nope, one is currently dead
		}
	}


	DeliverAll(true);

	foreach (layers,l)
	{
		index_t oldest = std::numeric_limits<index_t>::max();

		foreach (l->shardGrid,s)
		{
			oldest = std::min(oldest,s->sds.First().GetGeneration());
			ASSERT__(s->sds.First().GetOutput()->IsFullyConsistent());
		}

		foreach (l->shardGrid,s)
		{
			for (index_t i =0; i < s->outboundNeighbors.Count(); i++)
			{
				const auto*n = s->outboundNeighbors[i].shard;
				if (n)
				{
					for (index_t j = 0; j < 2 && j < s->sds.Count(); j++)
					{
						const auto*mySDS = s->FindGeneration(oldest+j);
						const auto*nSDS = n->FindGeneration(oldest+j);
						AssertRecovering(mySDS,nSDS,i);
					}
				}
				else
				{
					for (index_t j = 0; j < 2 && j < s->sds.Count(); j++)
					{
						const auto*mySDS = s->FindGeneration(oldest+j);
						const index_t reverse = s->outboundNeighbors[i].inboundIndex;
						AssertRecovering(nullptr,mySDS,reverse);
					}
				}
			}
		}

	}

}


#ifdef INT_MESSAGES
	void		Grid::DispatchUserMessage(const GUID&target, LogicProcess targetProcess, UINT64 payload)
	{
		MessageDataContainer msg = payload;
		foreach (layers,l)
		{
			foreach (l->shardGrid,s)
			{
				s->userMessages.Append().message = msg;
				s->userMessages.Last().target.guid = target;
				#ifndef ONE_LOGIC_PER_ENTITY
					s->userMessages.Last().targetProcess = targetProcess;
				#endif
			}
		}
	}
#else
	void		Grid::DispatchUserMessage(const GUID&target, LogicProcess targetProcess, const void*payload, size_t payloadSize)
	{
		MessageDataContainer msg {new MessageData((const BYTE*)payload,payloadSize)};
		foreach (layers,l)
		{
			foreach (l->shardGrid,s)
			{
				s->userMessages.Append().message = msg;
				s->userMessages.Last().target.guid = target;
				#ifndef ONE_LOGIC_PER_ENTITY
					s->userMessages.Last().targetProcess = targetProcess;
				#endif
			}
		}
	}
#endif

void Check(const Shard&s, const FullShardDomainState&sds)
{
	if (sds.GetOutput()->IsFullyConsistent())
	{
		auto id = DB::ID(s,sds.GetGeneration());
		ASSERT__(DB::IsLocked() || DB::NewestKnownSDSGeneration(id.destination)> id.generation || s.client.ExistsAnywhere(id) || s.client.IsUploading(id));
	}

	for (index_t n = 0; n < NumNeighbors; n++)
		if (s.outboundNeighbors[n].shard)
		{
			const auto&rcs = sds.outboundRCS[n].ref;
			if (rcs && rcs->ic.IsFullyConsistent())
			{
				auto id = DB::ID(&s,s.outboundNeighbors[n].shard,  sds.GetGeneration());
				ASSERT__(DB::IsLocked() || s.client.ExistsAnywhere(id) || s.client.IsUploading(id));
			}
		}
}

void Grid::EvolveTop()
{
	VerifyIntegrity();

	//foreach (layers,l)
	//{
	//	//Rect<> boundaries (0,0,l->shardGrid.GetWidth()-0.001f,l->shardGrid.GetHeight()-0.001f);
	//	foreach (l->shardGrid,s)
	//	{
	//		if (s->sds.Last().GetGeneration() == topGeneration)
	//		{
	//			SDS&newState = s->sds.Append();
	//			newState.userMessages.adoptData(s->userMessages);
	//			s->sds.fromEnd(1).PrecomputeSuccessor(*s,newState,boundaries);
	//		}
	//	}
	//}

	ShardParallel([this](Layer&l, Shard&s)	//parallel
		{
			if (!s.IsFullyAvailable())
				return;
			s.VerifyIntegrity();
			if (s.sds.Last().GetGeneration() == topGeneration)
			{
				SDS&newState = s.sds.Append();
				newState.userMessages.adoptData(s.userMessages);

				const index_t gen = topGeneration+1;

				s.sds.GetFromEnd(1).PrecomputeSuccessor(s,newState,boundaries,gen,CLOCATION);
			}
		}
	);



	bool allOkay = DeliverGeneration(topGeneration+1);
	ShardParallel([this](Layer&l, Shard&s)	//parallel
		{
			if (!s.IsFullyAvailable())
				return;
			if (s.sds.Last().GetGeneration() == topGeneration+1)
			{
				SDS&newState = s.sds.Last();

				const index_t gen = topGeneration+1;
				newState.FinalizeComputation(s,CLOCATION);

				Check(s,newState);
				//numCurrentEntities += newState.GetOutput()->Count();
			}
			s.VerifyIntegrity();
		}

	);

	Trim();

	topGeneration++;
}


void		Grid::Trim()
{
	VerifyIntegrity();
	if (lockAutoTrim)
		return;
	oldestLivingGeneration = topGeneration;
	ShardParallel([this](Layer&l, Shard&s)	//parallel
	{
		{
			s.Trim();
		}
	});

	ShardIterative([this](Layer&l, Shard&s)
	{
		if (s.IsFullyAvailable())
			oldestLivingGeneration = M::Min(oldestLivingGeneration,s.sds.First().GetGeneration());
	});

	VerifyIntegrity();
}


void		Grid::UpdateOldestRecoverableGeneration()
{
	foreach (layers,l)
	{
		foreach (l->shardGrid,s)
		{
			if (s->IsFullyAvailable())
				s->UpdateOldestRecoverableGeneration();
		}
	}
}


#define ALWAYS_USE_DW_PARALLEL_FOR


void	Grid::ShardParallel(const std::function<void(Layer&, Shard&)>&f)
{
	#ifdef _DEBUG
		ShardIterative(f);
		return;
	#endif
	
	#ifdef ALWAYS_USE_DW_PARALLEL_FOR
		ShardParallelW([f](Layer&l, Shard&s,index_t){f(l,s);});
	#else

		if (layers.IsEmpty())
			return;
		const count_t shardsPerLayer = layers.First().shardGrid.Count();
		const count_t totalShards = shardsPerLayer * layers.Count();
		Concurrency::parallel_for(index_t(0),totalShards,[shardsPerLayer,this,f](index_t at)
		//for (index_t at = 0; at < totalShards; at++)
		{
			const index_t layer = at / shardsPerLayer;
			const index_t inner = at % shardsPerLayer;
			Layer&l = layers[layer];
			f(l,l.shardGrid[inner]);
		}
		);
	#endif
}


void	Grid::ShardParallel(const std::function<void(const Layer&, const Shard&)>&f)	const
{
	#ifdef _DEBUG
		ShardIterative(f);
		return;
	#endif

	#ifdef ALWAYS_USE_DW_PARALLEL_FOR
		ShardParallelW([f](const Layer&l, const Shard&s,index_t){f(l,s);});
	#else

		if (layers.IsEmpty())
			return;
		const count_t shardsPerLayer = layers.First().shardGrid.Count();
		const count_t totalShards = shardsPerLayer * layers.Count();
		Concurrency::parallel_for(index_t(0),totalShards,[shardsPerLayer,this,f](index_t at)
		//for (index_t at = 0; at < totalShards; at++)
		{
			const index_t layer = at / shardsPerLayer;
			const index_t inner = at % shardsPerLayer;
			const Layer&l = layers[layer];
			f(l,l.shardGrid[inner]);
		}
		);
	#endif
}

static const constexpr count_t JobsPerExec = 8;

void	Grid::ShardParallelW(const std::function<void(Layer&, Shard&, index_t)>&f)
{
	if (layers.IsEmpty())
		return;
	const count_t shardsPerLayer = layers.First().shardGrid.Count();
	const count_t totalShards = shardsPerLayer * layers.Count();
	Sys::ParallelFor(totalShards,JobsPerExec,[shardsPerLayer,this,f](index_t at, index_t worker)
	//for (index_t at = 0; at < totalShards; at++)
	{
		index_t layer = at / shardsPerLayer;
		index_t inner = at % shardsPerLayer;
		Layer&l = layers[layer];
		f(l,l.shardGrid[inner],worker);
	}
	);
}

void	Grid::ShardParallelW(const std::function<void(const Layer&, const Shard&, index_t)>&f) const
{
	if (layers.IsEmpty())
		return;
	const count_t shardsPerLayer = layers.First().shardGrid.Count();
	const count_t totalShards = shardsPerLayer * layers.Count();
	Sys::ParallelFor(totalShards,JobsPerExec,[shardsPerLayer,this,f](index_t at, index_t worker)
	//for (index_t at = 0; at < totalShards; at++)
	{
		const index_t layer = at / shardsPerLayer;
		const index_t inner = at % shardsPerLayer;
		const Layer&l = layers[layer];
		f(l,l.shardGrid[inner],worker);
	}
	);
}

void	Grid::ShardIterative(const std::function<void(Layer&, Shard&)>&f)
{
	if (layers.IsEmpty())
		return;
	const count_t shardsPerLayer = layers.First().shardGrid.Count();
	const count_t totalShards = shardsPerLayer * layers.Count();
	for (index_t at = 0; at < totalShards; at++)
	{
		const index_t layer = at / shardsPerLayer;
		const index_t inner = at % shardsPerLayer;
		Layer&l = layers[layer];
		f(l,l.shardGrid[inner]);
	}
}

void	Grid::ShardIterative(const std::function<void(const Layer&, const Shard&)>&f)	const
{
	if (layers.IsEmpty())
		return;
	const count_t shardsPerLayer = layers.First().shardGrid.Count();
	const count_t totalShards = shardsPerLayer * layers.Count();
	for (index_t at = 0; at < totalShards; at++)
	{
		const index_t layer = at / shardsPerLayer;
		const index_t inner = at % shardsPerLayer;
		const Layer&l = layers[layer];
		f(l,l.shardGrid[inner]);
	}
}
