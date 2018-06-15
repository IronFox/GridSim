#include <global_root.h>
#include "SDS.h"
#include "entity.h"
#include "shard.h"
#include <math.h>
#include <container/sorter.h>
#include <io/delta_crc32.h>
#include "shard.h"
#include "Database.h"
#include "statistics.h"

///*static*/ float		FullShardDomainState::minEntityDistance = 0.05f;

const PRCS edgeInboundRCS(new RCS(TCodeLocation()));


void	CoreShardDomainState::Hash(Hasher&inputHash)	const
{
	entities.Hash(inputHash);
	ic.Hash(inputHash);
	inputHash.AppendPOD(generation);
}


void					RemoteChangeSet::Verify(const PRCS&consistentMatch)
{
	cs.AssertEqual(consistentMatch->cs);
	ASSERT_EQUAL__(ic.GetHighestInconsistency(),consistentMatch->ic.GetHighestInconsistency());
	ASSERT__(ic == consistentMatch->ic);
	verifiedWith = consistentMatch;
}


DB::Result			FullShardDomainState::DownloadOutput(Shard&s)
{
	DB::Result rs = s.client.Download(generation, output);	//copy from db
	if (rs == DB::Result::Downloaded)
	{
		ASSERT_EQUAL__(output->generation,generation);
	}
	return rs;
}



void					RemoteChangeSet::VerifyIntegrity(const TCodeLocation&loc) const
{
	ic.VerifyIntegrity(loc);
	//ASSERT_EQUAL__(permanence.isStoredInDatabase, ic.IsFullyConsistent());
}


void FullShardDomainState::PrecomputeSuccessor(Shard&shard, SDS & rs, const TBoundaries&motionSpace, index_t currentTimestep, const TCodeLocation&caller) const
{
	#ifdef DBG_SHARD_HISTORY
		shard.LogEvent("Precompute SDS g"+String(generation+1)); 
	#endif

	const auto input = this->GetOutput();
	ASSERT__(input);
	ASSERT__(input->hGrid.core.grid.IsNotEmpty());

	ASSERT_EQUAL__(input->generation,generation);

	const SDS*const consistentMatch = shard.consistentMatch ? shard.consistentMatch->FindGeneration(generation) : nullptr;
	const SDS*const consistentSuccessorMatch = shard.consistentMatch ? shard.consistentMatch->FindGeneration(generation+1) : nullptr;


	if (consistentMatch )
	{
		if (input->IsFullyConsistent())
			AssertTotalEquality(*consistentMatch,shard.gridCoords);
		else
			AssertSelectiveEquality(*consistentMatch,shard.gridCoords);
	}

	rs.pGrid.reset(new HashProcessGrid(input->hGrid.core,  generation+1,shard.gridCoords,false));

	input->ic.VerifyIntegrity(CLOCATION);
	Hasher	inputHash;
	inputHash.AppendPOD(input->IsFullyConsistent());
	input->Hash(inputHash);
	//foreach (userMessages,msg)
	//{
	//	msg->target.Hash(inputHash);
	//	inputHash.AppendPOD(msg->targetProcess);
	//	inputHash.Append(msg->message);
	//}

	Hasher::HashContainer	c;
	inputHash.Finish(c);
	ASSERT__(!rs.GetOutput() || !rs.GetOutput()->IsFullyConsistent());

	rs.InitGeneration(generation + 1, currentTimestep,true,CLOCATION,caller);
	ASSERT__(!rs.GetOutput()->ic.IsSealed());
	if (c == rs.inputHash)
	{
		rs.GetOutput()->entities = rs.processed;
	//	if (!rs.outputConsistent)
		{
			InconsistencyCoverage ic;
			input->ic.Grow(ic);
			rs.GetOutput()->ic.CopyCoreArea(TGridCoords(),ic);
		}
		return;
	}
	if (consistentSuccessorMatch && input->IsFullyConsistent())
		ASSERT__(c == consistentSuccessorMatch->inputHash);
	rs.inputHash = c;
	rs.processed = input->entities;


	foreach (userMessages,msg)
	{
		Entity*e = rs.processed.FindEntity(msg->target.guid);
		if (!e)
			LogUnexpected("User Message: Unable to find target entity",msg->target);
		else
		{
			auto ws = e->FindLogic(msg->targetProcess);
			if (!ws)
				LogUnexpected("User Message: Unable to find target logic process",msg->target);
			else
				ws->receiver.Append().data=msg->message;
		}
	}

	InconsistencyCoverage ic;
	input->ic.Grow(ic);
	rs.GetOutput()->ic.CopyCoreArea(TGridCoords(),ic);

	rs.GetOutput()->ic.VerifyIntegrity(CLOCATION);

	rs.localCS.Clear();
	rs.ExecuteLogic(rs.localCS,rs.GetGeneration(),shard.gridCoords, motionSpace);
	if (consistentSuccessorMatch && input->IsFullyConsistent())
	{
		rs.localCS.AssertEqual(consistentSuccessorMatch->localCS);
	}

	for (int i = 0; i < NumNeighbors; i++)
	{
		const TGridCoords&delta = shard.neighbors[i].delta;
		if (!motionSpace.Contains(shard.gridCoords + delta))
		{
			ASSERT__(shard.neighbors[i].shard == nullptr);
			const index_t inbound = shard.neighbors[i].inboundIndex;
			auto&in = rs.inboundRCS[inbound];
			in = edgeInboundRCS;
			in->VerifyIntegrity(CLOCATION);
		}
		else
			ASSERT__(shard.neighbors[i].shard != nullptr);
		OutRemoteChangeSet&rcs = rs.outboundRCS[i];
		if (rcs.ref && rcs.ref->ic.IsFullyConsistent())
		{
			//no changes since already consistent


			rcs.ref->VerifyIntegrity(CLOCATION);

			if (consistentSuccessorMatch)
				rcs.ref->Verify(consistentSuccessorMatch->outboundRCS[i].ref);
			continue;
		}
		ASSERT__(rcs.ref != edgeInboundRCS);
		rcs.ref.reset(new RCS(caller));
		rcs.ref->ic.CopyCoreArea(-delta,ic);
		rs.localCS.ExportEdge(delta,shard.gridCoords, rcs.ref->cs);
		input->hGrid.core.ExportEdge(rcs.ref->hGrid,delta);
		rcs.confirmed = shard.neighbors[i].shard == nullptr;
		ASSERT_EQUAL__(rcs.confirmed,!motionSpace.Contains(shard.gridCoords + delta));


		if (consistentSuccessorMatch && rcs.ref->ic.IsFullyConsistent())
			rcs.ref->Verify(consistentSuccessorMatch->outboundRCS[i].ref);


		if (rcs.ref->ic.IsFullyConsistent() && shard.neighbors[i].shard)
		{
			const auto id = DB::ID(&shard,shard.neighbors[i].shard,rs.GetGeneration());
			shard.client.Upload(id,rcs.ref);
		}

		rcs.ref->VerifyIntegrity(CLOCATION);


		if (input->IsFullyConsistent())
			ASSERT__(rcs.ref->ic.IsFullyConsistent());

	}
}

TSDSCheckResult	FullShardDomainState::CheckMissingRCS(Shard&s)
{
	ASSERT__(!GetOutput() || !GetOutput()->IsFullyConsistent());
	TSDSCheckResult rs;
	rs.predecessorIsConsistent = inputConsistent;
	rs.thisIsConsistent = GetOutput() && GetOutput()->ic.IsFullyConsistent();

	for (index_t i = 0; i < s.neighbors.Count(); i++)
	{
		Shard*other = s.neighbors[i].shard;
		if (inputConsistent && other && (!outboundRCS[i].ref || !outboundRCS[i].ref->ic.IsFullyConsistent()))
			rs.outRCSUpdatable++;
		index_t inbound = s.neighbors[i].inboundIndex;
		auto&rcs = inboundRCS[inbound];
		if (rcs && rcs->ic.IsFullyConsistent())
			continue;
		if (!other)
			continue;


		ASSERT__(rcs != edgeInboundRCS);


		//try get from database:
		if (s.client.IsCached(Database::ID(other,&s,generation)))
		{
			auto previous = rcs;
			auto rc = s.client.DownloadInbound(other,generation,rcs);
			ASSERT__(rc == DB::Result::Downloaded);
			ASSERT__(rcs);
			ASSERT__(rcs != previous);
			ASSERT__(rcs->ic.IsFullyConsistent());

			rs.rcsRestoredFromCache++;
			continue;
		}
		rs.missingRCS ++;

		bool check = false;
		
		//try to get from neighbor:
		if (other->IsFullyAvailable())
		{
			rs.rcsAvailableFromNeighbor++;	//optimisitic guess


			//SDS*remoteSDS = other->FindGeneration(this->GetOutput()->generation);
			//if (remoteSDS)
			//{
			//	const auto&nRCS = remoteSDS->outboundRCS[inbound].ref;
			//	if ((nRCS && nRCS->ic.IsFullyConsistent()) || )
			//	{
			//		rs.rcsAvailableFromNeighbor++;
			//		continue;
			//	}
			//}
			//else
			//	check = true;
		}
		if (s.client.ExistsAnywhere(Database::ID(other,&s,GetOutput()->generation)))
		{
			this->inRCSPermanence[inbound].Request();
			rs.rcsAvailableFromDatabase++;
		}
#if defined _DEBUG || 1
		elif (check)
		{
			if (!other->client.IsUploading(DB::ID(other,&s,generation)))
			{
				bool found = false;
				for (index_t g = GetOutput()->generation; g < GetOutput()->generation+1000; g++)
				{
					if (s.client.ExistsAnywhere(DB::ID(s,g)))
					{
						found = true;
						break;
					}
				}
				ASSERT__(found);
			}
		}
#endif
	}
	return *(const_cast<TSDSCheckResult*>(&rs));
}


bool				FullShardDomainState::AllRCSAreLocalAndConsistent() const
{
	foreach (inboundRCS,rcs)
		if (!*rcs || !(*rcs)->ic.IsFullyConsistent())
			return false;
	return true;
}


bool FullShardDomainState::IsConsistentAndConfirmed() const
{
	if (!GetOutput()->IsFullyConsistent())
		return false;
	return AllConfirmed();
}

bool FullShardDomainState::AllConfirmed() const
{
	foreach (outboundRCS,rcs)
		if (!rcs->confirmed)
			return false;
	return true;
}

bool FullShardDomainState::AllReceived() const
{
	for (index_t i = 0; i < NumNeighbors; i++)
		if (!inboundRCS[i])
			return false;
	return true;
}


void				FullShardDomainState::SetOpenEdgeRCS(const Shard&owner)
{
	for (index_t i = 0; i < NumNeighbors; i++)
	{
		const index_t inbound = owner.neighbors[i].inboundIndex;
		if (!owner.neighbors[i].shard)
			inboundRCS[inbound] = edgeInboundRCS;
	}
}

void	FullShardDomainState::InitGeneration(index_t gen, index_t currentTimestep, bool reallocateOutput, const TCodeLocation&caller, const TCodeLocation&precomputeCaller)
{
	generation = gen;
	if (!reallocateOutput)
		output.reset();
	else
	{
		output.reset(new CoreShardDomainState(gen,currentTimestep,caller,precomputeCaller));
		
	}

}

void FullShardDomainState::OverrideSetOutput(Shard & parent, const PCoreShardDomainState & out, const TCodeLocation&caller)
{
	if (output == out)
		return;
	if (out)
		ASSERT_EQUAL__(out->generation,this->generation);
	output = out;
	#ifdef DBG_SHARD_HISTORY
		parent.LogEvent("Updated output at generation g"+String(generation)+". New output consistency is: "+String(out->IsFullyConsistent())+", called by "+String(caller));
	#endif
}

void FullShardDomainState::FinalizeComputation(Shard&shard, const TCodeLocation&caller)
{
	auto out = GetOutput();

	const SDS*const consistentMatch = shard.consistentMatch ? shard.consistentMatch->FindGeneration(generation) : nullptr;

	ASSERT_EQUAL__(out->generation,generation);

	#ifdef DBG_SHARD_HISTORY
		shard.LogEvent("Finalize SDS g"+String(generation)); 
	#endif

	out->finalizeCaller = caller;
	out->ic.VerifyIntegrity(CLOCATION);
	//static InconsistencyCoverage bad;
	//static Sys::SpinLock badLock;
	//if (bad.GetGrid().IsEmpty())
	//{
	//	badLock.lock();
	//	if (bad.GetGrid().IsEmpty())
	//	{
	//		InconsistencyCoverage core;
	//		core.AllocateDefaultSize();
	//		core.FillMinInconsistent();
	//		core.Grow(bad);
	//	}
	//	badLock.unlock();
	//}

	CS cs = localCS;

	for (int i = 0; i < NumNeighbors; i++)
	{
		const index_t inbound = shard.neighbors[i].inboundIndex;
		auto&rcs = inboundRCS[inbound];
		if (rcs)
		{
			out->hGrid.edge[i] = rcs->hGrid;
			if (rcs == edgeInboundRCS)
			{
				ASSERT__(rcs->ic.IsFullyConsistent());
				ASSERT__(rcs->cs.CountInstructions() == 0);
			}
			else
			{
				rcs->VerifyIntegrity(CLOCATION);
				cs.Merge(rcs->cs);
			
				if (!this->GetOutput()->ic.Include(TGridCoords(),rcs->ic) && consistentMatch &&  rcs->ic.IsFullyConsistent())
				{
					rcs->Verify(consistentMatch->inboundRCS[inbound]);
				}
			}
		}
		else
		{
			ASSERT_NOT_NULL__(shard.neighbors[i].shard);
			//if (shard.client.Exists(DB::ID(shard.neighbors[i].shard,&shard,GetOutput()->generation)))
				inRCSPermanence[inbound].Request();
			//this->GetOutput()->ic.Include(shard.neighbors[i].delta,bad);
			ASSERT_LESS__(generation,std::numeric_limits<IC::generation_t>::max());
			this->GetOutput()->ic.IncludeMissing(shard.neighbors[i].delta,VectorToIndex( shard.gridCoords),shard.parentGrid->currentSize,IC::generation_t(generation));
		}
	}

	significantInboundChange = false;

	cs.Apply(shard.gridCoords,*out);
	out->ic.VerifyIntegrity(CLOCATION);

	pGrid->Include(out->entities);
	pGrid->Finish(out->hGrid.core);
	pGrid.reset();


	if (out->ic.IsFullyConsistent())
	{
		#ifdef DBG_SHARD_HISTORY
			shard.LogEvent("Finalized SDS g"+String(generation)+" is consistent"); 
		#endif

		shard.client.Upload(out);

		if (consistentMatch)
			AssertTotalEquality(*consistentMatch,shard.gridCoords);
	}
	else
	{
	
		if (shard.consistentMatch)
		{
			const auto&layer = shard.consistentMatch->parentGrid->layers.First();
			foreach (out->entities,e)
			{
				if (out->ic.IsInconsistent(e->coordinates-shard.gridCoords))
				{
					auto found = layer.FindFirstEntity(*e,out->generation);
					if (found)
						e->ProgressOmega(Vec::distance(e->coordinates,found->coordinates));
					//else
						//FATAL__("Entity should not exist");	// for now this should never happen. don't know. happens anyways. very very rarely
				}
			}
		}

		if (consistentMatch)
		{
			AssertSelectiveEquality(*consistentMatch,shard.gridCoords);

			Statistics::CaptureInconsistency(out->ic,out->entities,consistentMatch->GetOutput()->entities,shard.gridCoords);
		}
	}

}

/*
void FullShardDomainState::AssertEquality(const SDS & other) const
{
}
*/


#pragma optimize( "", off )

/*static*/ void			FullShardDomainState::AssertSelectiveEquality(const PCoreShardDomainState&local, const PCoreShardDomainState&remote, const TGridCoords&shardOffset)
{
	ASSERT__(local);
	ASSERT__(remote);

	if (local && remote)
	{

		for (index_t i = 0; i < local->ic.GetGrid().Count(); i++)
		{
			const auto&ls = local->ic.GetGrid()[i];
			const auto&rs = remote->ic.GetGrid()[i];
			if (!ls.IsConsistent() || !rs.IsConsistent())
				continue;
			ASSERT__(ls.unavailableShards.AllZero());
			ASSERT__(rs.unavailableShards.AllZero());

			const auto&lh = local->hGrid.core.grid[i];
			const auto&rh = remote->hGrid.core.grid[i];
			ASSERT_EQUAL__(lh.prev, rh.prev);
			ASSERT_EQUAL__(lh.next, rh.next);
		}


		foreach(local->entities, p)
		{
			const TEntityCoords c=  p->coordinates-shardOffset;
			if (local->ic.IsInconsistent(c) || remote->ic.IsInconsistent(c))
				continue;


			const Entity*p2 = remote->entities.FindEntity(p->guid);
			ASSERT2__(p2 && (*p2) == *p, local->birthPlace,remote->birthPlace);
		}

		foreach(remote->entities, p)
		{
			const TEntityCoords c= ( p->coordinates - shardOffset);
			if (local->ic.IsInconsistent(c) || remote->ic.IsInconsistent(c))
				continue;
			const Entity*p2 = local->entities.FindEntity(p->guid);
			ASSERT_NOT_NULL2__(p2, local->birthPlace,remote->birthPlace);	//equality already checked if existent
		}

	}

}
#pragma optimize( "", on )


void				FullShardDomainState::AssertSelectiveEquality(const SDS&other, const TGridCoords&shardOffset)const
{
	AssertSelectiveEquality(GetOutput(),other.GetOutput(),shardOffset);
}


void FullShardDomainState::AssertTotalEquality(const SDS & other, const TGridCoords&shardOffset) const
{
	ASSERT_EQUAL__(generation,other.generation);
	auto local = GetOutput();
	auto remote = other.GetOutput();
	ASSERT__(local);
	ASSERT__(remote);

	if (local && remote)
	{
		foreach(local->entities, p)
		{
			const Entity*p2 = remote->entities.FindEntity(p->guid);
			ASSERT2__(p2 && (*p2) == *p, local->birthPlace,remote->birthPlace);
		}

		foreach(remote->entities, p)
		{
			const Entity*p2 = local->entities.FindEntity(p->guid);
			ASSERT_NOT_NULL2__(p2, local->birthPlace,remote->birthPlace);	//equality already checked if existent
		}

		for (index_t i = 0; i < local->ic.GetGrid().Count(); i++)
		{
			const auto&ls = local->ic.GetGrid()[i];
			const auto&rs = remote->ic.GetGrid()[i];
			ASSERT__(ls.unavailableShards.AllZero());
			ASSERT__(rs.unavailableShards.AllZero());

			const auto&lh = local->hGrid.core.grid[i].next;
			const auto&rh = remote->hGrid.core.grid[i].next;
			ASSERT_EQUAL__(lh, rh);
		}


	}
	//ASSERT__(s->inputHash==o->inputHash);

	/*
	ASSERT_EQUAL__(GetOutput()->entities.Count(),other.GetOutput()->entities.Count());
	foreach (GetOutput()->entities,e)
	{
		const Entity*oe = other.GetOutput()->entities.FindEntity(e->guid);
		ASSERT_NOT_NULL__(oe);
		if ((*e) != (*oe))
		{
			if (e->coordinates != oe->coordinates)
			{
				const Op::Motion*m0=nullptr, *m1=nullptr;
				foreach (localCS.motionOps,op)
					if (op->target.guid == e->guid)
					{
						m0 = op;
						break;
					}
				foreach (other.localCS.motionOps,op)
					if (op->target.guid == e->guid)
					{
						m1 = op;
						break;
					}
				FATAL__("Coordinate mismatch");
			}

			Buffer0<index_t> a,b,a1,b1;
			for (index_t i = 0; i < NumNeighbors; i++)
			{
				{
					const auto&in = inboundRCS[i];
					foreach (in->cs.stateAdvertisementOps,op)
						if ( Metric::Distance(op->origin.coordinates,e->coordinates) <= Entity::MaxAdvertisementRadius)
							a << i;
				}
				{
					const auto&in = other.inboundRCS[i];
					foreach (in->cs.stateAdvertisementOps,op)
						if ( Metric::Distance(op->origin.coordinates,e->coordinates) <= Entity::MaxAdvertisementRadius)
							b << i;
				}
			}
			FATAL__("break");

		}
	}
	*/
}


void		FullShardDomainState::ClearDemand()
{
	permanence.FlushRequests();
	foreach (inRCSPermanence,rcs)
		rcs->FlushRequests();
}

void		FullShardDomainState::RegisterDemand(DownloadDemandRegistry&reg,count_t generationsToTop, count_t generationsToBottom)
{
	if (GetOutput() && GetOutput()->IsFullyConsistent())
		return;

	Shard&s = reg.GetShard();

	{
		const auto id = Database::ID(reg.GetShard(),generation);
		float d = permanence.GetSDSDemand(generationsToBottom,s.client.ExistsAnywhere(id));
		reg.Register(d,id,output);
	}
	for (index_t i = 0; i < NumNeighbors; i++)
	{
		if (!s.neighbors[i].shard)
			continue;
		const index_t inbound = s.neighbors[i].inboundIndex;
		const auto*const ptr = inboundRCS[inbound].get();
		if (ptr && ptr->ic.IsFullyConsistent())
			continue;
		ASSERT__(ptr != edgeInboundRCS.get());

		const auto id = DB::ID(s.neighbors[i].shard,&s,generation);
		float d= inRCSPermanence[inbound].GetDemand(generationsToTop,s.client.ExistsAnywhere(id));
		reg.Register(d,id,inboundRCS[inbound]);
	}
}


//
//
//void		FullShardDomainState::PullDemanded(Shard&s,count_t&limit)
//{
//	if (limit == 0)
//		return;
//	count_t total = 0;
//	if (permanence.HasDemand())
//	{
//		total++;
//		permanence.FlushRequests();
//		if (s.client.Download(generation, output) == DB::Result::Downloaded)
//		{
//			ASSERT_EQUAL__(generation,output->generation);
//			//significantInboundChange = true;	//unnecessary, is not fully consistent
//		}
//	}
//	if (GetOutput() && GetOutput()->IsFullyConsistent())
//		return;
//	for (index_t i = 0; i < NumNeighbors; i++)
//	{
//		if (!s.neighbors[i].shard)
//			continue;
//
//		const index_t inbound = s.neighbors[i].inboundIndex;
//		if (inboundRCS[inbound] && inboundRCS[inbound]->ic.IsFullyConsistent())
//			continue;
//		if (inRCSPermanence[inbound].HasDemand())
//		{
//			total++;
//			if (s.client.DownloadInbound(s.neighbors[i].shard,generation,inboundRCS[inbound]) == DB::Result::Downloaded)
//				significantInboundChange = true;
//			inRCSPermanence[inbound].FlushRequests();
//		}
//	}
//	if (total < limit)
//		limit -= total;
//	else
//		limit = 0;
//}


void		FullShardDomainState::LosePermanence(bool isFirst)
{
	output.reset();
	this->inputHash = inputHash.Empty;
	inputConsistent = false;
	this->processed.Clear();
	foreach (inboundRCS,rcs)
	{
		if (*rcs != edgeInboundRCS)
			rcs->reset();
	}
}

void FullShardDomainState::ExecuteLogic(CS & outLocal, index_t generation, const TGridCoords&gridCoords, const TBoundaries&motionSpace)
{
	CRC32::Sequence seq;
	seq.AppendPOD(gridCoords);
	seq.AppendPOD(generation);


	foreach (processed,e)
	{

		CRC32::Sequence seq2 = seq;
		seq2.AppendPOD(e->guid);
		//float2	motion;
		MessageDispatcher dispatcher;
		EntityShape shape = *e;
		Vec::clear(shape.velocity);

		if (e->guid.Data1 == 0x15)
		{
			bool brk = true;
		}
		foreach (e->logic,l)
		//e->logic.VisitAllEntries([&motion,e,&dispatcher,gridCoords,this,seq2](const void*p, LogicState&state)
		{
			//LogicProcess f = (LogicProcess)p;
			CRC32::Sequence seq3 = seq2;
			seq3.AppendPOD(l->process);
			
			Random rnd(seq3.Finish());
			dispatcher.fromProcess = l->process;
			l->process(l->state,generation,*e,shape,rnd,l->receiver,dispatcher);
		}
		((EntityShape&)*e) = shape;
		//float2 motion = e->coordinates - originPosition;
		//e->coordinates = originPosition;
		outLocal.Add(*e,dispatcher);

		#ifndef NO_SENSORY
			Op::StateAdvertisement&ad = outLocal.stateAdvertisementOps.Append();
			ad.origin = *e;
			ad.shape = *e;
		#endif

		if (!Vec::zero(shape.velocity))
		{
			TEntityCoords coords2 = e->coordinates + shape.velocity;
			motionSpace.Clamp(coords2);
			shape.velocity = coords2 - e->coordinates;
	
			Metric::Distance len(shape.velocity);
			//ASSERT__(!isnan(len));
			if (len > Entity::MaxMotionDistance*0.99f)
				shape.velocity *= Entity::MaxMotionDistance / *len*0.99f;

			outLocal.AddSelfMotion(*e,e->coordinates + shape.velocity);
			#ifndef NO_SENSORY
				ad.shape.velocity = shape.velocity;
				ad.origin.coordinates += shape.velocity;
			#endif
			//Add(*e,Op::StateAdvertisement(motion));
		}
		else
		{
			//outLocal.Add(*e,Op::StateAdvertisement());
		}
	}
	GetOutput()->entities = processed;
}


void CheckConsistent(const PCoreShardDomainState&consistentMatch, const Entity&e)
{
	if (!consistentMatch)
		return;
	Entity*e2 = consistentMatch->entities.FindEntity(e.guid);
	ASSERT__(e2 && *e2 == e);

}


void	InsertEntityUpdateOmega(EntityStorage&outEntities,Entity input,const Grid::Layer*consistentComparison, index_t generation)
{
	if (consistentComparison)
	{
		const Entity*consistent = consistentComparison->FindFirstEntity(input,generation);
		ASSERT_NOT_NULL__(consistent);	//temporary. once entities can be spawned, this changes
		EntityError last = input;
		input.ReplaceLastProgression(Vec::distance(input.coordinates,consistent->coordinates));
	}
	outEntities.InsertEntity(input);
}



const int SelectExclusiveSource(const CoreShardDomainState&a, const CoreShardDomainState&b, 
				const IC::Comparator&comp)
{
	int balance = 0;
	for (index_t i = 0; i < a.ic.GetGrid().Count(); i++)
	{
		const auto&as = a.ic.GetGrid()[i];
		const auto&bs = b.ic.GetGrid()[i];
		balance += comp(as,bs);
	}
	return balance < 0 ? -1 : 1;
}


static void MergeInconsistentEntitiesEx(CoreShardDomainState&merged, const CoreShardDomainState&source, 
				const TGridCoords&shardOffset, 
				bool correctLocations, const Grid::Layer*consistentComparison)
{
	static const float searchScope = 0.5f;

	foreach (source.entities,e0)
	{
		if (merged.entities.FindEntity(e0->guid))
			continue;	//already good

		const TEntityCoords c0 = (e0->coordinates - shardOffset);
		if (merged.ic.IsInconsistent(c0))
		{
			merged.entities.InsertEntity(*e0);
		}
		else
		{
			if (correctLocations)
			{
				TEntityCoords c = c0;
				if (merged.ic.FindInconsistentPlacementCandidate(c,searchScope))
				{
					Entity me = *e0;
					me.coordinates = c + shardOffset;
					InsertEntityUpdateOmega(merged.entities,me,consistentComparison, source.generation);
				}

			}
			//only increases overaccounted entities:
			//if (merged.ic.FindInconsistentPlacementCandidate(c0,searchScope))
			//{
			//	Entity me = *e0;
			//	me.coordinates = c0 + shardOffset;
			//	merged.entities.InsertEntity(me);
			//}
		}
	}


	{
		HashProcessGrid grid(merged.hGrid.core,source.generation, shardOffset,true);
		grid.Include(merged.entities);
		grid.Finish(merged.hGrid.core);
	}

}


void MergeInconsistentEntitiesComp(CoreShardDomainState&merged, const CoreShardDomainState&a, const CoreShardDomainState&b, 
				const IC::Comparator&comp, const TGridCoords&shardOffset, 
				const Grid::Layer*consistentComparison
)
{
	static const float searchScope = 0.5f;

	foreach (a.entities,e0)
	{
		if (merged.entities.FindEntity(e0->guid))
			continue;	//already good
		//entity is inconsistent and not in merged state yet
		const auto c0 = e0->coordinates - shardOffset;
		auto e1 = b.entities.FindEntity(e0->guid);
		const auto c1 = e1 ? (e1->coordinates - shardOffset) : TEntityCoords();
		if (!merged.ic.IsInconsistent(c0))
		{
			//this is tricky. entity not merged, but would reside in consistent space (bad).
			if (e1)
			{
		//		ASSERT__(b.ic.IsInconsistent(c1));	//otherwise it would have been added prior, and we would not be here
				//so this entity exists in both SDS'
				{
					//we now have the same entity twice, both inconsistent, residing each in the consistent space of the other SDS'
					//let's assume the entity isn't supposed to exist here anyways

					const Entity*candidate = nullptr;
					int sc = comp(a.ic.GetSample(c0), b.ic.GetSample(c1));
					if (sc < 0)
						candidate = e0;
					elif (sc > 0)
						candidate = e1;
					elif (*e0  < *e1)
						candidate = e0;
					else
						candidate = e1;


					TEntityCoords c = (candidate->coordinates - shardOffset);
					if (merged.ic.FindInconsistentPlacementCandidate(c,searchScope))
					{
						Entity me = *candidate;
						me.coordinates = c + shardOffset;
						InsertEntityUpdateOmega(merged.entities,me,consistentComparison, a.generation);
					}

				}
			}
			else
			{
				//entity exists only in local SDS.
				//let's assume the entity isn't supposed to exist here anyways

				//TEntityCoords c = Frac(e0->coordinates);
				//if (merged.ic.FindInconsistentPlacementCandidate(c,searchScope))
				//{
				//	Entity copy = *e0;
				//	copy.coordinates = c + shardOffset;
				//	//ASSERT__(merged.ic.IsInconsistent(Frac(copy.coordinates)));
				//	merged.entities.InsertEntity(copy);
				//}
				//else
				//	FATAL__("bad");
			}
		}
		else
		{
			//entity location is inconsistent in both SDS'. This is expected to be the most common case
			if (e1)
			{
				if (*e1 == *e0)
				{
					//probably actually consistent
//						ASSERT__(merged.ic.IsInconsistent(Frac(e0->coordinates)));
					merged.entities.InsertEntity(*e0);
				}
				else
				{

					if (!merged.ic.IsInconsistent(c1))
					{
						ASSERT__(merged.ic.IsInconsistent(c0));
						merged.entities.InsertEntity(*e0);
					}
					else
					{
						const Entity*candidate = nullptr;
						int sc = comp(a.ic.GetSample(c0), b.ic.GetSample(c1));
						if (sc < 0)
							candidate = e0;
						elif (sc > 0)
							candidate = e1;
						elif (*e0  < *e1)
							candidate = e0;
						else
							candidate = e1;
					
						//common case. Choose one
						ASSERT__(merged.ic.IsInconsistent(candidate->coordinates-shardOffset));
						merged.entities.InsertEntity(*candidate);
					}
				}
			}
			else
			{
				//only e0 exists
				int sc = comp(a.ic.GetSample(c0),b.ic.GetSample(c0));
				//ASSERT__(merged.ic.IsInconsistent(Frac(e0->coordinates)));
				if (sc <= 0)
					merged.entities.InsertEntity(*e0);
			}
		}
	}
	foreach (b.entities,e0)
	{
		if (merged.entities.FindEntity(e0->guid))
			continue;	//already good
		//entity is inconsistent and not in merged state yet
		const auto c0 = e0->coordinates - shardOffset;
		//const auto c1 = e1 ? (e1->coordinates - shardOffset) : TEntityCoords();
		if (!merged.ic.IsInconsistent(c0))
		{
			#if 0
			//this is tricky. entity not merged, but would reside in consistent space (bad).
			if (e1)
			{
				//case handled
				//FATAL__("bad");
			}
			else
			{
				//entity exists only in local SDS.
				//let's assume the entity isn't supposed to exist here anyways

				//TEntityCoords c = Frac(e0->coordinates);
				//if (merged.ic.FindInconsistentPlacementCandidate(c,searchScope))
				//{
				//	Entity copy = *e0;
				//	copy.coordinates = c + shardOffset;
				//	//ASSERT__(merged.ic.IsInconsistent(Frac(copy.coordinates)));
				//	merged.entities.InsertEntity(copy);
				//}
			/*	else
					FATAL__("bad");*/
			}
			#endif /*0*/
		}
		else
		{
			auto e1 = a.entities.FindEntity(e0->guid);
			//entity location is inconsistent in both SDS'. This is expected to be the most common case
			if (e1)
			{
				//case handled
				//FATAL__("bad");
			}
			else
			{
				//only e0 exists
				int sc = comp(a.ic.GetSample(c0),b.ic.GetSample(c0));
				//ASSERT__(merged.ic.IsInconsistent(Frac(e0->coordinates)));
				if (sc >= 0)
					merged.entities.InsertEntity(*e0);
			}
		}
	}

}

static PCoreShardDomainState		Merge(
	const CoreShardDomainState&a, const CoreShardDomainState&b, const IC::Comparator&comp, index_t currentGen
	, const Shard&myShard
	, Statistics::MergeStrategy strategy
	, const Grid::Layer*consistentComparison
	, Statistics::TSDSample<double>*outInconsistentEntitiesOutsideIC
//,const PCoreShardDomainState&consistentMatch
)
{
	PCoreShardDomainState rs(new CoreShardDomainState(a.generation,currentGen,CLOCATION,CLOCATION));
	ASSERT_EQUAL__(a.generation,b.generation);

	auto&merged = *rs;


	const CoreShardDomainState*exclusiveSource = nullptr;
	int exclusiveChoice = 0;

	if (strategy == Statistics::MergeStrategy::Exclusive || strategy == Statistics::MergeStrategy::ExclusiveWithPositionCorrection)
	{
		exclusiveChoice = SelectExclusiveSource(a,b,comp);;
		exclusiveSource = &(exclusiveChoice == -1 ? a : b);
	}

	{
		const GridSize size(IC::Resolution);

		const auto&g0 = a.ic.GetGrid();
		const auto&g1 = b.ic.GetGrid();
		GridArray<IC::TExtSample> gm;
		ASSERT_EQUAL__(g0.GetSize(),size);
		ASSERT_EQUAL__(g1.GetSize(),size);
		gm.SetSize(size);

		const auto&h0 = a.hGrid.core.grid;
		const auto&h1 = b.hGrid.core.grid;
		auto&hm = merged.hGrid.core.grid;
		ASSERT_EQUAL__(h0.GetSize(),size);
		ASSERT_EQUAL__(h1.GetSize(),size);
		hm.SetSize(size);

		for (index_t i = 0; i < g0.Count(); i++)
		{
			const auto	&s0 = g0[i],
						&s1 = g1[i];
			auto		&sm = gm[i];
			const auto	&hs0 = h0[i],
						&hs1 = h1[i];
			auto		&hsm = hm[i];


			if (s0.IsConsistent())
			{
				sm = s0;
				hsm = hs0;
			}
			elif (s1.IsConsistent())
			{
				sm = s1;
				hsm = hs1;
			}
			else
			{
				int choice;
				if (exclusiveChoice)
					choice = exclusiveChoice;
				else
					choice = comp(s0,s1);

				sm = choice <= 0 ? s0 : s1;
//				sm.SetWorst(s0,s1);
				hsm = choice <= 0 ? hs0 : hs1;
			}
		}
		merged.ic.SetGrid(std::move(gm));
		merged.ic.Seal();


		//now for the 'fun' part
		for (index_t n = 0; n < NumNeighbors; n++)
		{
			const auto	&e0 = a.hGrid.edge[n].grid,
						&e1 = b.hGrid.edge[n].grid;
			auto		&em = merged.hGrid.edge[n].grid;
			const GridSize size = max(e0.GetSize(),e1.GetSize());
			em.SetSize(size);
			const TGridCoords delta = Shard::LinearToNeighbor(n);

			for (index_t i = 0; i < em.Count(); i++)
			{
				auto idx = em.ToVectorIndexNoCheck(i);
				if (delta.x == 1)
					idx.x = IC::Resolution-1;
				if (delta.y == 1)
					idx.y = IC::Resolution-1;
				#ifdef D3
					if (delta.z == 1)
						idx.z = IC::Resolution-1;
				#endif
				const auto&s0 = g0.Get(idx);
				const auto&s1 = g1.Get(idx);
				int choice;
				if (exclusiveChoice)
					choice = s0.IsConsistent() ? -1 : (s1.IsConsistent()  ? 1 : exclusiveChoice);
				else
					choice = comp(s0,s1);
				bool set = false;
				if (choice < 0 || (choice == 0 && e1.IsEmpty()))
				{
					if (e0.IsNotEmpty())
					{
						em[i] = e0[i];
						set = true;
					}
				}
				else
				{
					if (e1.IsNotEmpty())
					{
						em[i] = e1[i];
						set = true;
					}
				}

				if (!set)
				{
					auto idSet = em.ToVectorIndexNoCheck(i);
					if (delta.x == -1)
						idSet.x = IC::Resolution-1;
					if (delta.y == -1)
						idSet.y = IC::Resolution-1;
					#ifdef D3
						if (delta.z == -1)
							idSet.z = IC::Resolution-1;
					#endif
					em[i] = HGrid::EmptyCell(myShard.neighbors[n].shard->gridCoords,idSet);
				}
			}
		}
	}

	foreach (a.entities,e)
	{
		if (a.ic.IsInconsistent(e->coordinates-myShard.gridCoords))
			continue;	//for now

	//	CheckConsistent(consistentMatch,*e);

		merged.entities.InsertEntity(*e);
	}
	foreach (b.entities,e)
	{
		if (b.ic.IsInconsistent(e->coordinates-myShard.gridCoords))
			continue;	//for now
		if (merged.entities.FindEntity(e->guid))
			continue;

		//CheckConsistent(consistentMatch,*e);

		merged.entities.InsertEntity(*e);
	}
	//at this point we merged all fully consistent entities from either. If the inconsistent areas did not overlap then the result should contain all entities in their consistent state
	
	
	if (!merged.ic.IsFullyConsistent())
	{
		if (outInconsistentEntitiesOutsideIC)
		{
			foreach (a.entities,e)
				if (!merged.entities.FindEntity(e->guid))
				{
					const auto c0 = e->coordinates-myShard.gridCoords;
					if (merged.ic.IsInconsistent(c0))
						(*outInconsistentEntitiesOutsideIC)+=0;
					else
						(*outInconsistentEntitiesOutsideIC)+=1;
				}
			foreach (b.entities,e)
				if (!merged.entities.FindEntity(e->guid))
				{
					const auto c0 = e->coordinates-myShard.gridCoords;
					if (merged.ic.IsInconsistent(c0))
						(*outInconsistentEntitiesOutsideIC)+=0;
					else
						(*outInconsistentEntitiesOutsideIC)+=1;
				}
		}
		if (strategy == Statistics::MergeStrategy::EntitySelective)
			MergeInconsistentEntitiesComp(merged,a,b,comp,myShard.gridCoords, consistentComparison);
		else
			MergeInconsistentEntitiesEx(merged,*exclusiveSource,myShard.gridCoords,strategy == Statistics::MergeStrategy::ExclusiveWithPositionCorrection,consistentComparison);

		merged.mergeCounter = std::max(a.mergeCounter,b.mergeCounter)+1;

	}
	
	{
		HashProcessGrid grid(merged.hGrid.core,a.generation, myShard.gridCoords,true);
		grid.Include(merged.entities);
		grid.Finish(merged.hGrid.core);
	}

	return rs;
}




Statistics::TStateDifference	CompareStates(const CoreShardDomainState&approx, const CoreShardDomainState&consistent, const IC&mask, const TGridCoords&shardOffset)
{
	Statistics::TStateDifference rs;
	rs.icSize = approx.ic.CountInconsistentSamples();

	if (rs.icSize.sum == 0)
		return rs;	//nothing to measure

	count_t entitiesInInconsistentArea = 0;
	count_t missingEntities = 0;
	count_t overAccountedEntities=0,
			totalInconsistentEntites = 0,
			inconsistentEntitiesOutsideMask = 0;

	foreach (consistent.entities,e)
	{
		const auto inner = e->coordinates-shardOffset;

		auto e2 = approx.entities.FindEntity(e->guid);
		bool consistent = true;
		bool missing = false;
		float error = 0;
		if (e2)
		{
			if (*e2 != *e)
			{
				error = Vec::distance(e->coordinates,e2->coordinates);
				consistent = false;
			}
		}
		else
		{
			consistent = false;
			missing = true;
		}

		if (!consistent)
		{
			totalInconsistentEntites++;
		}


		if (mask.IsInconsistent(inner))
		{
			entitiesInInconsistentArea++;
			if (missing)
				missingEntities++;
			else
				rs.inconsistency += error;
			
			rs.inconsistencyProbability += consistent?0:1;
		}
		else if (!consistent)
		{
			inconsistentEntitiesOutsideMask++;
		}
	}

	foreach (approx.entities,e)
	{
		const auto inner = e->coordinates-shardOffset;
		if (mask.IsInconsistent(inner))
			rs.omega += e->omega;
		auto e2 = consistent.entities.FindEntity(e->guid);
		if (!e2)
			overAccountedEntities ++;
	}

	rs.entitiesInInconsistentArea = entitiesInInconsistentArea;
	rs.missingEntities = missingEntities;
	rs.overAccountedEntities = overAccountedEntities;
	rs.entitiesInSDS = consistent.entities.Count();

	return rs;

}


static void		CompareMerge(const CoreShardDomainState&a, const CoreShardDomainState&b, 
							const IC::Comparator&comp, Statistics::MergeStrategy strategy, index_t currentGen, const Grid::Layer&consistentLayer, const PCoreShardDomainState&consistent, const IC&mask, const Shard&shard)
{
	Statistics::TSDSample<double> incSample;
	auto merged = Merge(a,b,comp,currentGen,shard,strategy,&consistentLayer,&incSample);


	for (index_t i = 0; i < merged->ic.GetGrid().Count(); i++)
	{
		ASSERT_EQUAL__(merged->ic.GetGrid()[i].IsConsistent(),mask.GetGrid()[i].IsConsistent());
	}

	Statistics::TStateDifference diffM = CompareStates(*merged,*consistent,merged->ic,shard.gridCoords);
	diffM.inconsistentEntitiesOutsideIC = incSample;

	Statistics::CaptureMergeResult(comp, strategy,diffM);



}


struct TTest
{
	bool	isSet = false;
	TEntityCoords	any;
	TGridCoords		grid;

	void	Set(const TEntityCoords&any)
	{
		isSet = true;
		this->any = any;
		grid = any * IC::Resolution;
	}

};


#pragma optimize( "", off )
void				TestProbabilisticICReduction(const CoreShardDomainState&a, const ExtHGrid&ag, const CoreShardDomainState&b,const ExtHGrid&bg,
	const GridArray<bool>&actuallyConsistent,const GridArray<TTest>&shouldNotBeConsidered,const IC&mergedIC, const Statistics::TICReductionConfig&strat)
{
	const count_t fullSamples = mergedIC.GetGrid().Count();
	count_t totalGuesses = 0,
			consideredConsistent = 0,
			correctGuesses = 0,
			actuallyConsistentCount = 0,
			shouldHaveConsideredConsistent = 0,
			shouldNotHaveConsideredConsistent = 0;


	for (index_t i = 0; i < fullSamples; i++)
	{
		if (mergedIC.GetGrid()[i].IsConsistent())
		{
			continue;
		}
		const auto&as = a.ic.GetGrid()[i];
		const auto&bs = b.ic.GetGrid()[i];
		ASSERT__(!as.IsConsistent());
		ASSERT__(!bs.IsConsistent());
		totalGuesses++;
		bool match = true;
		bool icViolation = false;
		if (strat.maxDepth < as.depth && strat.maxDepth < bs.depth)
		{
			match = false;
			icViolation = true;
		}
		if (strat.minSpatialDistance > as.spatialDistance && strat.minSpatialDistance > bs.spatialDistance)
		{
			match = false;
			icViolation = true;
		}


		if (match && strat.flags & Statistics::ICReductionFlags::RegardEntityState)
		{
			if (strat.flags & Statistics::ICReductionFlags::RegardEntityEnvironment)
			{
				if (!ExtHGrid::ExtMatch(ag,bg,ToVector( actuallyConsistent.ToVectorIndex(i) ),ExtHGrid::MissingEdgeTreatment::Fail,strat.minEntityPresence))
					match = false;
			}
			else
				if (!ExtHGrid::CoreMatch(ag,bg,ToVector( actuallyConsistent.ToVectorIndex(i) ),strat.minEntityPresence))
					match = false;
		}
		if (match && (strat.flags & Statistics::ICReductionFlags::RegardOriginBitField))
		{
			const BitArray&ba = a.ic.GetGrid()[i].unavailableShards;
			const BitArray&bb = b.ic.GetGrid()[i].unavailableShards;
			ASSERT__(!ba.AllZero());
			ASSERT__(!bb.AllZero());
			if (ba.OverlapsWith(bb))
				match = false;
		}
		if (match && (strat.flags & Statistics::ICReductionFlags::RegardOriginRange))
		{
			const auto&p0 = a.ic.GetGrid()[i].precise;
			const auto&p1 = b.ic.GetGrid()[i].precise;
			ASSERT__(!p0.AllUndefined());
			ASSERT__(!p1.AllUndefined());
			if (p0.OverlapsWith(p1,strat.overlapTolerance))
				match = false;
		}
		if (match && (strat.flags & Statistics::ICReductionFlags::RegardFuzzyOriginRange))
		{
			const auto&p0 = a.ic.GetGrid()[i].fuzzy;
			const auto&p1 = b.ic.GetGrid()[i].fuzzy;
			ASSERT__(!p0.AllUndefined());
			ASSERT__(!p1.AllUndefined());
			if (p0.OverlapsWith(p1,strat.overlapTolerance))
				match = false;
		}
		
		const bool consistent = actuallyConsistent[i];
		const auto&test = shouldNotBeConsidered[i];

		if (
			(match && test.isSet && (strat.flags & Statistics::ICReductionFlags::RegardEntityState))
			||
			(!match && consistent
				&& strat.minEntityPresence == 0 && !icViolation
				&& (strat.flags == Statistics::ICReductionFlags::RegardEntityState) 
			)

		)
		{
			if (match)
			{
				if (strat.flags & Statistics::ICReductionFlags::RegardEntityEnvironment)
				{
					ExtHGrid::RequireExtMismatch(ag,bg,ToVector( actuallyConsistent.ToVectorIndex(i) ),ExtHGrid::MissingEdgeTreatment::Fail,i);
				}
				else
					ExtHGrid::RequireCoreMismatch(ag,bg,ToVector( actuallyConsistent.ToVectorIndex(i) ),i);
			}
			else
				ExtHGrid::RequireCoreMatch(ag,bg,ToVector( actuallyConsistent.ToVectorIndex(i) ),i);
			FATAL__("Consistency violation");
		}

		if (consistent)
			actuallyConsistentCount++;
		if (match)
		{
			consideredConsistent++;
			if (consistent)
				correctGuesses ++;
			else
				shouldNotHaveConsideredConsistent ++;
		}
		else
		{
			if (consistent)
				shouldHaveConsideredConsistent ++;
			else
				correctGuesses ++;
		}

	}

	Statistics::TProbabilisticICReduction rs;
	rs.config = strat;

	rs.actuallyConsistent = actuallyConsistentCount;
	rs.correctGuesses = correctGuesses;
	rs.consideredConsistent = consideredConsistent;
	rs.shouldHaveConsideredConsistent = shouldHaveConsideredConsistent;
	rs.shouldNotHaveConsideredConsistent = shouldNotHaveConsideredConsistent;
	rs.totalGuesses = totalGuesses;

	Statistics::CaptureICTest(rs);
}
#pragma optimize( "", on )

void				TestProbabilisticICReduction(const CoreShardDomainState&a,const CoreShardDomainState&b,const GridArray<bool>&actuallyConsistent,const GridArray<TTest>&shouldNotBeConsidered,const IC&mergedIC, const Statistics::TICReductionConfig&strat, const TGridCoords&shardOffset)
{
	if (!strat.IsPossible())
		return;

	//if (!(strat & Statistics::ICReductionFlags::RegardHistory))
	//{
	//	ExtHGrid ag,bg;
	//	HashProcessGrid processorA(0,shardOffset),processorB(0,shardOffset);
	//	processorA.Include(a.entities);
	//	processorB.Include(b.entities);
	//	processorA.Finish(ag.core);
	//	processorB.Finish(bg.core);
	//	TestProbabilisticICReduction(a,ag,b,bg,actuallyConsistent,shouldNotBeConsidered,mergedIC,strat);
	//}
	//else
	TestProbabilisticICReduction(a,a.hGrid,b,b.hGrid,actuallyConsistent,shouldNotBeConsidered,mergedIC,strat);

}


void				FullShardDomainState::SynchronizeWithSibling(Shard&myShard,  Shard&siblingShard, SDS&sibling, const IC::Comparator&comp, index_t currentTimestep)
{
	if (GetOutput() == sibling.GetOutput())
		return;
	ASSERT_EQUAL__(generation,sibling.GetGeneration());
	if (!GetOutput() || !sibling.GetOutput())
		return;	//nothing there to work with
	if (GetOutput()->IsFullyConsistent() && sibling.GetOutput()->IsFullyConsistent())
	{
		if (GetOutput() != sibling.GetOutput())
		{
			OverrideSetOutput(myShard, sibling.GetOutput(),CLOCATION);
			GetOutput()->migrationCounter++;
		}
		myShard.VerifyIntegrity();
		//ASSERT__(GetOutput() == sibling.GetOutput());
		return;	//nothing to do
	}

	if (GetOutput()->IsFullyConsistent())
	{
		myShard.mergeStats.copyOutCounter++;
		siblingShard.mergeStats.copyInCounter++;
		sibling.OverrideSetOutput(siblingShard,GetOutput(),CLOCATION);
		GetOutput()->migrationCounter++;
		return;
	}

	if (sibling.GetOutput()->IsFullyConsistent())
	{
		siblingShard.mergeStats.copyOutCounter++;
		myShard.mergeStats.copyInCounter++;
		OverrideSetOutput(myShard, sibling.GetOutput(),CLOCATION );
		GetOutput()->migrationCounter++;
		myShard.VerifyIntegrity();
		return;
	}

	const SDS*const consistentMatch = myShard.consistentMatch ? myShard.consistentMatch->FindGeneration(generation) : nullptr;
	const auto consistentOutput = consistentMatch ? consistentMatch->GetOutput() : nullptr;
	const auto*layer = myShard.consistentMatch ? myShard.consistentMatch->parentGrid->layers.begin() : nullptr;

	const auto&a = *GetOutput();
	const auto&b = *sibling.GetOutput();
	PCoreShardDomainState merged  = Merge(a,b,comp,currentTimestep,myShard,Statistics::MergeStrategy::Exclusive, layer,nullptr);

	AssertSelectiveEquality(merged,consistentOutput,myShard.gridCoords);


	if (consistentOutput)
	{
		const Statistics::TStateDifference diffA = CompareStates(a,*consistentOutput,merged->ic,myShard.gridCoords);
		const Statistics::TStateDifference diffB = CompareStates(b,*consistentOutput,merged->ic,myShard.gridCoords);

		Statistics::CapturePreMerge(diffA,diffB);

		{
			GridArray<bool>	actuallyConsistent = GridArray<bool>(GridSize(IC::Resolution));
			GridArray<TTest>	shouldNotBeConsidered = GridArray<TTest>(GridSize(IC::Resolution));
			actuallyConsistent.Fill(true);
			foreach (consistentOutput->entities,e)
			{
				const auto*e0 = a.entities.FindEntity(e->guid);
				const auto*e1 = b.entities.FindEntity(e->guid);
				const bool mismatch0 = !e0 || (!e->operator==(*e0));
				const bool mismatch1 = !e1 || (!e->operator==(*e1));

				if (mismatch0 != mismatch1)
				{
					if (mismatch0)
						shouldNotBeConsidered.Get(VectorToIndex( IC::ToPixels(e1->coordinates - myShard.gridCoords) )).Set(e1->coordinates);
					else
						shouldNotBeConsidered.Get(VectorToIndex( IC::ToPixels(e0->coordinates - myShard.gridCoords) )).Set(e0->coordinates);
				}

				if (mismatch0 || mismatch1)
				{
					actuallyConsistent.Get(VectorToIndex( IC::ToPixels(e->coordinates - myShard.gridCoords) )) = false;
					if (e0)
						actuallyConsistent.Get(VectorToIndex( IC::ToPixels(e0->coordinates - myShard.gridCoords) )) = false;
					if (e1)
						actuallyConsistent.Get(VectorToIndex( IC::ToPixels(e1->coordinates - myShard.gridCoords) )) = false;
				}
				if (mismatch0 && mismatch1)
				{
					ASSERT__(!merged->ic.GetGrid().Get(VectorToIndex( IC::ToPixels(e->coordinates - myShard.gridCoords) )).IsConsistent());
				}
			}
			foreach (a.entities,e)
			{
				if (!consistentOutput->entities.FindEntity(e->guid))
					actuallyConsistent.Get(VectorToIndex( IC::ToPixels(e->coordinates - myShard.gridCoords) )) = false;
			}
			foreach (b.entities,e)
			{
				if (!consistentOutput->entities.FindEntity(e->guid))
					actuallyConsistent.Get(VectorToIndex( IC::ToPixels(e->coordinates - myShard.gridCoords) )) = false;
			}


			const count_t presenceStep = (count_t)std::max(1.0,Statistics::GetEntityDensityPerRCube()*0.25);	//keep in sync with statistics.cpp

			for (IC::content_t maxD = 0; maxD <= 3; maxD++)
				for (IC::content_t minS = 0; minS <= 16; minS++)
					for (index_t overlap = 0; overlap <= 0; overlap++)
						for (index_t minPresence = 0; minPresence <= 5; minPresence++)	//keep in sync with statistics.cpp
							for (index_t i = 0; i < (count_t)Statistics::ICReductionFlags::NumCombinations; i++)
							{
								Statistics::TICReductionConfig cfg;
								cfg.flags = Statistics::ICReductionFlags(i);
								cfg.maxDepth = maxD != 0 ? maxD : IC::MaxDepth;
								cfg.minSpatialDistance = minS;
								cfg.minEntityPresence = minPresence * presenceStep;	//keep in sync with statistics.cpp
								cfg.overlapTolerance = overlap;
								TestProbabilisticICReduction(a,b,actuallyConsistent,shouldNotBeConsidered,merged->ic, cfg,myShard.gridCoords);
							}
		}


		for (int i = 0; i < (int)Statistics::MergeStrategy::Count; i++)
		{
			const Statistics::MergeStrategy s = (Statistics::MergeStrategy)i;
			CompareMerge(a,b,IC::BinaryComparator(),s,currentTimestep,*layer,consistentOutput,merged->ic,myShard);
			CompareMerge(a,b,IC::OrthographicComparator(),s,currentTimestep,*layer,consistentOutput,merged->ic,myShard);
			CompareMerge(a,b,IC::ReverseOrthographicComparator(),s,currentTimestep,*layer,consistentOutput,merged->ic,myShard);
			CompareMerge(a,b,IC::DepthComparator(),s,currentTimestep,*layer,consistentOutput,merged->ic,myShard);
			CompareMerge(a,b,IC::ExtentComparator(),s,currentTimestep,*layer,consistentOutput,merged->ic,myShard);
			CompareMerge(a,b,IC::PlaneComparator(float3(-0.00575048150,0.00748061994,0.0350000001)),s,currentTimestep,*layer,consistentOutput,merged->ic,myShard);
			#ifdef MERGE_PROFILE_IS_COMPARATOR_SOURCE
				CompareMerge(a,b,Statistics::ProfileComparator(),s,currentTimestep,*layer,consistentOutput,merged->ic,myShard);
			#endif
		}
	}



	myShard.mergeStats.totalMerges++;
	siblingShard.mergeStats.totalMerges++;

	auto ic = merged->ic.GetTotalBadness();
	if (ic.total == 0)
	{
		myShard.mergeStats.perfectMerges ++;
		siblingShard.mergeStats.perfectMerges ++;
	}
	else
	{
		auto ic0 = GetOutput()->ic.GetTotalBadness();
		auto ic1 = sibling.GetOutput()->ic.GetTotalBadness();
		if (ic.inconsistentSamples < std::min(ic0.inconsistentSamples,ic1.inconsistentSamples))
		{
			myShard.mergeStats.icSizeReducingMerges++;
			siblingShard.mergeStats.icSizeReducingMerges++;
		}
		elif (ic.total < std::min(ic0.total,ic1.total))
		{
			myShard.mergeStats.improvingMerges++;
			siblingShard.mergeStats.improvingMerges++;
		}
		else
			myShard.mergeStats.failedMerges++;
			siblingShard.mergeStats.failedMerges++;
	}
	

	OverrideSetOutput(myShard,merged,CLOCATION);
	sibling.OverrideSetOutput(siblingShard,merged,CLOCATION);
	myShard.VerifyIntegrity();

	if (GetOutput()->IsFullyConsistent())
	{
		myShard.client.Upload(GetOutput());
		//siblingShard.client.Upload(GetOutput());	//upload both to maintain some checks?
	}
}

