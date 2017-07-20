#include <global_root.h>
#include "shard.h"
#include "grid.h"
#include "metric.h"


const PRCS emptyRCS(new RCS(TCodeLocation()));


index_t					Shard::NewestConsistentSDSIndex() const
{
//	VerifyIntegrity();
	index_t i = sds.Count()-1;
	for (; i < sds.Count() && !sds[i].GetOutput()->IsFullyConsistent(); i--)
	{}
	return i;
}
index_t					Shard::NewestDefinedSDSIndex() const
{
//	VerifyIntegrity();
	index_t i = 0;
	for (; i < sds.Count() && sds[i].GetOutput(); i++)
	{}
	i--;
	return i;
}

	

/*static*/ index_t			Shard::NeighborToLinear(const TGridCoords&delta)
{
	#ifdef D3
	index_t linear = (delta.x+1)*9
					+(delta.y+1)*3
					+(delta.z+1);
	#else
	index_t linear = (delta.x+1)*3
					+(delta.y+1);
	#endif

	if (delta.x > 0 || 
		(delta.x == 0 && 
			(
				delta.y > 0
				#ifdef D3
				||
				(delta.y == 0 && delta.z > 0)
				#endif
			)
		)
	   )
		linear--;
	return linear;
}



void Shard::Setup(Grid&grid, const TGridCoords&myCoords, index_t myLinearCoords, index_t layer)
{
	this->layer = layer;
	this->parentGrid = &grid;
	gridCoords =myCoords;
	linearCoords = myLinearCoords;
	Grid::Layer&layerRef = grid.layers[layer];
	index_t at = 0;
	for (int x = -1; x <= 1; x++)
		for (int y = -1; y <= 1; y++)
			#ifdef D3
				for (int z = -1; z <= 1; z++)
					if (x != 0 || y != 0 || z != 0)
			#else
				if (x != 0 || y != 0)
			#endif
			{
				#ifdef D3
					ASSERT_EQUAL__(NeighborToLinear(TGridCoords(x,y,z)),at);
				#else
					ASSERT_EQUAL__(NeighborToLinear(TGridCoords(x,y)),at);
				#endif
				TNeighbor&n = neighbors[at++];
				#ifdef D3
					n.delta = TGridCoords(x,y,z);
				#else
					n.delta = int2(x,y);
				#endif
				n.shard = layerRef.GetShard(myCoords + n.delta);
			}
	ASSERT__(at == neighbors.size());
	foreach (neighbors,n)
	{
		TGridCoords dir = -n->delta;
		foreach (neighbors,n2)
			if (n2->delta == dir)
				n2->inboundIndex = n-neighbors.begin();
	}

	auto&s = sds.First();
	s.SetOpenEdgeRCS(*this);

}

void	Shard::UploadInitialStates()
{
	ASSERT_EQUAL__(sds.Count(),1);
	auto&s = sds.First();
	s.pGrid.reset(new HashProcessGrid(s.GetGeneration(),gridCoords));
	s.pGrid->Include(s.GetOutput()->entities);
	s.pGrid->Finish(s.GetOutput()->hGrid.core);
	s.pGrid.reset();

	client.Upload(sds.First().GetOutput(),true);
	VerifyIntegrity();
}


void	Shard::RequestHighestDemand(count_t limit, DownloadDemandRegistry&reg)
{
	reg.Reset(this);
	foreach (sds,s)
		s->RegisterDemand(reg,sds.Last().GetGeneration() - s->GetGeneration(), s->GetGeneration() - sds.First().GetGeneration());

	volatile count_t pulled = reg.Pull(limit);

}

void Shard::VerifyIntegrity() const
{
	foreach (sds,s)
	{
		const auto &out = s->GetOutput();
		if (out)
		{
			ASSERT__(out->hGrid.core.grid.IsNotEmpty());
			ASSERT_EQUAL__(out->generation,s->GetGeneration());
			ASSERT_GREATER_OR_EQUAL__(out->createdInTimestep,out->generation);
		}
	}

	//for (index_t i = 0; i < NumNeighbors; i++)
	//{
	//	const index_t inbound =neighbors[i].inboundIndex;
	//	if (!neighbors[i].shard)
	//	{
	//		foreach (sds,s)
	//			ASSERT2__(s->inboundRCS[inbound] == edgeInboundRCS,inbound,s->inboundRCS[inbound]==nullptr);
	//	}
	//	else
	//		foreach (sds,s)
	//			ASSERT1__(s->inboundRCS[inbound] != edgeInboundRCS,inbound);
	//}
	ASSERT__(edgeInboundRCS->ic.IsFullyConsistent());
	ASSERT_EQUAL__(edgeInboundRCS->cs.CountInstructions(),0);


	if (IsDead())
		ASSERT__(sds.IsEmpty())
	else
	{
		//for (index_t i = 1; i < sds.Count(); i++)
		//{
		//	if (sds[i].GetOutput()->IsFullyConsistent())
		//		ASSERT__(sds[i-1].GetOutput()->IsFullyConsistent());
		//}


		if (IsFullyAvailable())
		{
			ASSERT__(sds.IsNotEmpty());
		}
		if (sds.IsNotEmpty() && sds.First().GetOutput())
			ASSERT__(sds.First().GetOutput()->IsFullyConsistent());
	}
}

void Shard::Trim()
{
	ASSERT__(recoveryAt == InvalidIndex);
	index_t trimTo = 0;
	const index_t consistent = NewestConsistentSDSIndex();
	if (consistent != InvalidIndex)
	{
//#ifdef _DEBUG
//		for (index_t i = 0; i < consistent; i++)
//		{
//			const auto&sds = this->sds[i];
//			if (sds.GetOutput() && sds.GetOutput()->IsFullyConsistent())
//			{
//				auto id = DB::ID(*this,sds.GetGeneration());
//				const index_t topGen = DB::NewestKnownSDSGeneration(id.destination);
//				ASSERT__(client.ExistsAnywhere(id) || client.IsUploading(id) || topGen > id.generation);
//
//				for (index_t n = 0; n < NumNeighbors; n++)
//				{
//					Shard*s = this->neighbors[n].shard;
//					if (s)
//					{
//						auto rcs = sds.outboundRCS[n].ref;
//						if (rcs && rcs->ic.IsFullyConsistent())
//						{
//							auto id = DB::ID(this,s,sds.GetGeneration());
//							const index_t gen = DB::NewestKnownSDSGeneration(id.destination);
//							if (!client.ExistsAnywhere(id) && !client.IsUploading(id) && gen < id.generation)
//							{
//								ASSERT__(client.Upload(id,rcs));
//								bool brk = true;
//								FATAL__("Missing upload");
//							}
//							
//						}
//					}
//				}
//			}
//		}
//#endif
		trimTo = consistent;
		if (trimTo > 0)
		{
			#ifdef DBG_SHARD_HISTORY
				LogEvent("Trimming away "+String(trimTo)+" SDS(s). New first (consistent) generation is g"+String(sds[trimTo].GetGeneration()));
			#endif

			sds.Erase(0,trimTo);
			ASSERT__(sds.First().GetOutput()->IsFullyConsistent());

			client.UpdateMinimumGeneration(sds.First().GetGeneration()+1,CLOCATION);
		}

	}

	//index_t i = NewestConsistentSDSIndex();
	//if (i != InvalidIndex)
	//{
	//	sds.Erase(0,i);
	//}
	index_t newestConsistentGenerationIndex = this->NewestConsistentSDSIndex();
	if (newestConsistentGenerationIndex != InvalidIndex)
	{
		ASSERT_EQUAL__(newestConsistentGenerationIndex,0);	//kind of implied now
		newestConsistentGeneration = this->sds[newestConsistentGenerationIndex].GetGeneration();
		oldestNonIsolatedInconsistentGeneration = newestConsistentGeneration+1;
		SDS*s;
		while ((s = FindGeneration(oldestNonIsolatedInconsistentGeneration)) && s->AllRCSAreLocalAndConsistent())
			oldestNonIsolatedInconsistentGeneration++;
	}
	this->VerifyIntegrity();
}

void Shard::UpdateOldestRecoverableGeneration()
{
	if (IsDead())
		return;

	oldestRecoverableGeneration = sds.last().GetGeneration();
	foreach (neighbors,n)
		if (n->shard && !n->shard->IsDead())
			oldestRecoverableGeneration = vmin(oldestRecoverableGeneration,n->shard->oldestNonIsolatedInconsistentGeneration);
	oldestRecoverableGeneration = vmax(oldestRecoverableGeneration,newestConsistentGeneration+1);
}

void Shard::TrimTo(index_t gen)
{
	ASSERT__(recoveryAt == InvalidIndex);
	index_t at = FindGenerationIndex(gen);
	if (at != InvalidIndex)
		sds.Erase(0,at);
}

SDS * Shard::FindGeneration(index_t gen)
{
	if (sds.IsEmpty())
		return nullptr;
	if (gen < sds.first().GetGeneration())
	{
		//FATAL__("System fault");
		return nullptr;
	}
	if (gen > sds.last().GetGeneration())
		return nullptr;
	return sds + (gen - sds.first().GetGeneration());
}

const SDS * Shard::FindGeneration(index_t gen) const
{
	if (sds.IsEmpty())
		return nullptr;
	if (gen < sds.first().GetGeneration())
	{
		//FATAL__("System fault");
		return nullptr;
	}
	if (gen > sds.last().GetGeneration())
		return nullptr;
	return sds + (gen - sds.first().GetGeneration());
}

index_t Shard::FindGenerationIndex(index_t gen) const
{
	if (sds.IsEmpty())
		return InvalidIndex;
	if (gen < sds.first().GetGeneration())
	{
		//FATAL__("System fault");
		return InvalidIndex;
	}
	if (gen > sds.last().GetGeneration())
		return InvalidIndex;
	return (gen - sds.first().GetGeneration());
}

bool	Shard::CorrectIsolatedSDS(const TBoundaries&boundaries, index_t opIndex, index_t currentTimestep)
{
	index_t at = NewestConsistentSDSIndex();
	at++;
	if (at >= sds.Count() || sds[at].GetOutput()->IsFullyConsistent())
		return false;
	if (!sds[at].AllRCSAreLocalAndConsistent())
		return false;

	RecomputeSDS(at, boundaries,opIndex,currentTimestep);
	recoveryOperations ++;
	return true;
}

void Shard::CorrectSome(count_t extra, const TBoundaries&boundaries, index_t opIndex, index_t currentTimestep)
{
	//if (extra > recoveryOperations)
	//	extra -= recoveryOperations;
	//else
	//	return;
	recoveryOperations += extra;
	if (sds.Count() > 1)
	{
		//RecomputeSDS(1,boundaries,opIndex);
		rrAt = std::max(sds.first().GetGeneration()+1,rrAt);
		index_t at = FindGenerationIndex(rrAt);

		for (index_t i = 0; i < extra; i++)
			RecomputeSDS(at + i, boundaries,opIndex,currentTimestep);

		rrAt += extra;
		if (rrAt > sds.last().GetGeneration())
			rrAt = sds.first().GetGeneration()+1;
	}

}

bool Shard::RecomputeSDS(index_t index, const TBoundaries&boundaries, index_t opIndex, index_t currentTimestep)
{
	if (!index || index >= sds.Count())
		return false;
	SDS&newState = sds[index];
	ASSERT__(newState.lastTouched != opIndex);
	newState.lastTouched = opIndex;
	if (newState.GetOutput()->IsFullyConsistent())
		return true;

	const index_t gen = sds[index-1].GetGeneration()+1;
	sds[index-1].PrecomputeSuccessor(*this,newState,boundaries,currentTimestep,CLOCATION);
	newState.FinalizeComputation(*this, CLOCATION);
	//newState.localCS.Clear();
	//newState.inconsistencyTexture.Load(newState.GetOutput()->ic.ic);
	return newState.GetOutput()->IsFullyConsistent();
}

void Shard::AssertIsEqual(const Shard&other) const
{
	foreach (sds,s)
	{
		const SDS*o = other.FindGeneration(s->GetOutput()->generation);
		ASSERT_NOT_NULL__(o);
		s->AssertTotalEquality(*o);
	}
}


void					Shard::Die(index_t topGeneration)
{
	if (liveliness.state == TLiveliness::Dead)
		return;
	ASSERT__(recoveryAt == InvalidIndex);

	client.SignalShutdown();
	liveliness.state = TLiveliness::Dead;
	liveliness.diedInTimestep = topGeneration;

	sds.Clear();
}



Shard::Shard():client(this)
{
	{
		auto&initial = sds.Append();
		initial.AllocateOutput(initial.GetGeneration(),CLOCATION);
	}
	ASSERT__(sds.Count()==1);
	ASSERT__(sds.First().GetOutput());
	for (index_t i = 0; i < NumNeighbors; i++)
	{
		{
			auto ref =&sds.First().outboundRCS[i];
			ref->ref = emptyRCS;
			ref->confirmed = true;
		}
		{
			auto ref =&sds.First().inboundRCS[i];
			(*ref) = emptyRCS;
		}
	}
	auto&s = sds.First();
	s.inputHash = Hasher::HashContainer::Empty;
	s.GetOutput()->ic.FlushInconsistency();

	ASSERT__(emptyRCS);
}


#ifdef DBG_SHARD_HISTORY
	void Shard::LogEvent(const String&str)
	{
		eventLock.lock();
		index_t at = newHistory.Count();
		newHistory << str;
		if (recordedHistory.Count() > at)
		{
			volatile const char* shouldBe = recordedHistory[at].c_str();
			ASSERT_EQUAL__(str,recordedHistory[at]);
		}
		eventLock.unlock();
	}
#endif

void Shard::CompareConsistent(const Shard & other) const
{
	if (!IsFullyAvailable())
		return;	//there's some weird intermediate states otherwise
	foreach (sds,s)
		if (s->GetOutput()->IsFullyConsistent())
		{
			const SDS*o = other.FindGeneration(s->GetOutput()->generation);
			if (!o || !o->GetOutput()->IsFullyConsistent())
				continue;
			o->AssertTotalEquality(*s);

		}
}

void DownloadDemandRegistry::Register(float demand, const DB::SDS_ID & id, DB::SDS&target)
{
	#ifdef DBG_SHARD_HISTORY
		shard->LogEvent("Register SDS demand="+String(demand)+", id="+id.ToString());
	#endif

	ASSERT__(id.destination == shard->gridCoords);

	auto&e = entries.Append();
	e.demand = demand;
	e.isRCS = false;
	e.sdsID = id;
	e.sdsTarget = &target; 
}

void DownloadDemandRegistry::Register(float demand, const DB::RCS_ID & id, DB::RCS&target)
{
	#ifdef DBG_SHARD_HISTORY
		shard->LogEvent("Register RCS demand="+String(demand)+", id="+id.ToString());
	#endif
	
	ASSERT__(id.destination == shard->gridCoords);

	auto&e = entries.Append();
	e.demand = demand;
	e.isRCS = true;
	e.rcsID = id;
	e.rcsTarget = &target; 
}


count_t	DownloadDemandRegistry::Pull(count_t maxRequests)
{
	if (!maxRequests)
		return 0;
	std::stable_sort(entries.begin(),entries.end(),[](const TEntry&a, const TEntry&b){return a.demand > b.demand;});

	count_t cnt = 0;
	foreach (entries,e)
	{
		maxRequests--;
		cnt++;
		if (e->isRCS)
			shard->client.Download(e->rcsID,*e->rcsTarget);
		else
			shard->client.Download(e->sdsID.generation,*e->sdsTarget);

		if (!maxRequests)
			break;
	}
	entries.Clear();
	return cnt;
}
