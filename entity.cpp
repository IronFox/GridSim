#include <global_root.h>
#include "entity.h"
#include "SDS.h"
#include "metric.h"

/*static*/ const float Entity::MaxInfluenceRadius = 0.125f,
			#ifndef NO_SENSORY
				Entity::MaxAdvertisementRadius = Entity::MaxInfluenceRadius/2,
				Entity::MaxMotionDistance = Entity::MaxInfluenceRadius/2,
			#else
				Entity::MaxMotionDistance = Entity::MaxInfluenceRadius,
			#endif
				Op::Removal::MaxRange = Entity::MaxInfluenceRadius,
				Op::Instantiation::MaxRange = Entity::MaxMotionDistance,
				Op::Message::MaxRange = Entity::MaxInfluenceRadius,
				Op::Broadcast::MaxRange = Entity::MaxInfluenceRadius,
				Op::Motion::MaxRange = Entity::MaxMotionDistance
		#ifndef NO_SENSORY
				,Op::StateAdvertisement::MaxRange = Entity::MaxAdvertisementRadius
		#endif
				
				
				;


namespace Detail
{
	template <typename Op, typename IsTargeted, typename Radial>
		struct CopyEdge
		{
			
		};


	template <typename Op, typename IsTargeted>
		struct CopyEdge<Op,IsTargeted,TTrue>
		{
			static void Execute(const Buffer0<Op>&from, Buffer0<Op>&to, const TGridCoords&delta, const TGridCoords&originSectorCoordinates)
			{
				foreach (from, op)
				{
					TEntityCoords f;
					Vec::sub(op->origin.coordinates,originSectorCoordinates,f);

					bool include = true;

					ASSERT__(!Vec::zero(delta));

					for (index_t d = 0; d < Dimensions; d++)
					{
						switch (delta.v[d])
						{
							case -1:
								include = f.v[d] <= Op::MaxRange;
							break;
							case 1:
								include = f.v[d] >= 1.f - Op::MaxRange;
							break;
						}
						if (!include)
							break;
					}
					if (include)
						to << *op;
				}
			}
		};
	template <typename Op>
		struct CopyEdge<Op,TTrue,TFalse>
		{
			static void Execute(const Buffer0<Op>&from, Buffer0<Op>&to, const TGridCoords&delta, const TGridCoords&originSectorCoordinates)
			{
				foreach (from, op)
				{
					TGridCoords rel = Entity::GetShardCoords(op->target.coordinates) - Entity::GetShardCoords(op->origin.coordinates);
					if (rel == delta)
						to << *op;
				}
			}
		};


}


template <typename Op>
	struct IsTargeted
	{
		typedef TFalse	T;
	};

template <>
	struct IsTargeted<Op::Targeted>
	{
		typedef TTrue	T;
	};

TTrue	IsT(const Op::Targeted*)	{return TTrue();}
TFalse	IsT(const void*)	{return TFalse();}


template <typename Op>
	static void CopyEdge(const Buffer0<Op>&from, Buffer0<Op>&to, const TGridCoords&delta, const TGridCoords&originSectorCoordinates)
	{
		to.Clear();
		Detail::CopyEdge<Op, decltype(IsT(from.pointer())), typename Op::DispatchRadially>::Execute(from,to,delta,originSectorCoordinates);
	}


template <typename Op>
	static void Merge(Buffer0<Op>&local, const Buffer0<Op>&remote)
	{
		local.AppendAll(remote);
	}

template <typename Op>
	static void AssertEqual(const Buffer0<Op>&a, const Buffer0<Op>&b)
	{
		for (index_t i = 0; i < a.Count() && i < b.Count(); i++)
		{
			const Op&op0 = a[i];
			const Op&op1 = b[i];
			ASSERT2__(op0 == op1,ToString(op0.origin.guid),ToString(op1.origin.guid));
		}
		ASSERT_EQUAL__(a.Count(),b.Count());
	}

template <typename Op>
	static bool IsEqual(const Buffer0<Op>&a, const Buffer0<Op>&b)
	{
		for (index_t i = 0; i < a.Count() && i < b.Count(); i++)
		{
			const Op&op0 = a[i];
			const Op&op1 = b[i];
			if (op0 != op1)
				return false;
		}
		return (a.Count() == b.Count());
	}


void ChangeSet::ExportEdge(const TGridCoords&relative, const TGridCoords&originSectorCoordinates, ChangeSet & target) const
{
	::CopyEdge(messageOps,target.messageOps,relative,originSectorCoordinates);
	::CopyEdge(broadcastOps,target.broadcastOps,relative,originSectorCoordinates);
	::CopyEdge(removalOps,target.removalOps,relative,originSectorCoordinates);
	::CopyEdge(instantiationOps,target.instantiationOps,relative,originSectorCoordinates);
	::CopyEdge(motionOps,target.motionOps,relative,originSectorCoordinates);
	#ifndef NO_SENSORY
		::CopyEdge(stateAdvertisementOps,target.stateAdvertisementOps,relative,originSectorCoordinates);
	#endif
}



void ChangeSet::Merge(const ChangeSet & other)
{
	::Merge(messageOps,other.messageOps);
	::Merge(broadcastOps,other.broadcastOps);
	::Merge(removalOps,other.removalOps);
	::Merge(instantiationOps,other.instantiationOps);
	::Merge(motionOps,other.motionOps);
	#ifndef NO_SENSORY
		::Merge(stateAdvertisementOps,other.stateAdvertisementOps);
	#endif
}

void	ChangeSet::AssertEqual(const ChangeSet&other)	const
{
	::AssertEqual(messageOps,other.messageOps);
	::AssertEqual(broadcastOps,other.broadcastOps);
	::AssertEqual(removalOps,other.removalOps);
	::AssertEqual(instantiationOps,other.instantiationOps);
	::AssertEqual(motionOps,other.motionOps);
	#ifndef NO_SENSORY
		::AssertEqual(stateAdvertisementOps,other.stateAdvertisementOps);
	#endif
}

bool	ChangeSet::operator==(const ChangeSet&other)	const
{
	return	::IsEqual(messageOps,other.messageOps)
		&&	::IsEqual(broadcastOps,other.broadcastOps)
		&&	::IsEqual(removalOps,other.removalOps)
		&&	::IsEqual(instantiationOps,other.instantiationOps)
		&&	::IsEqual(motionOps,other.motionOps)
		#ifndef NO_SENSORY
			&&	::IsEqual(stateAdvertisementOps,other.stateAdvertisementOps)
		#endif
		;

}






void ChangeSet::Apply(const TGridCoords&shardCoords,CoreShardDomainState &target) const
{
	const auto i2 = M::Sqr(Entity::MaxInfluenceRadius);
	const auto m2 = M::Sqr(Entity::MaxMotionDistance);
	#ifndef NO_SENSORY
		const auto a2 = M::Sqr(Entity::MaxAdvertisementRadius);
	#endif

	foreach (target.entities,e)
		foreach (e->logic,l)
			l->receiver.Clear();
	foreach (messageOps,op)
	{
		Entity*e = target.entities.FindEntity(op->target.guid);
		if (!e)
		{
			LogUnexpected("Message: Target entity not found",op->target);
			continue;
		}
		if (!Vec::equal(op->target.coordinates,e->coordinates))
		{
			LogUnexpected("Message: OP Target Mismatch",op->target,e);
			continue;
		}
		if (!Metric::CheckDistance("Message",*e,op->origin,Op::Message::MaxRange))
			continue;
		auto*p = e->FindLogic(op->targetProcess);
		if (p)
		{
			auto&m = p->receiver.Append();
			m.data = op->message;
			m.sender = op->origin;
			m.senderProcess = op->sourceProcess;
			m.isBroadcast = false;
		}
		else
		{
			LogUnexpected("Message: Target LP not found: "+String(op->targetProcess),op->target);
			continue;
		}
	}
	foreach (broadcastOps,op)
	{
		foreach (target.entities,e)
		{
			if (Metric::Distance(e->coordinates,op->origin.coordinates) > Op::Broadcast::MaxRange)
				continue;
			auto*p = e->FindLogic(op->targetProcess);
			if (p)
			{
				auto&m = p->receiver.Append();
				m.data = op->message;
				m.sender = op->origin;
				m.senderProcess = op->sourceProcess;
				m.isBroadcast = true;
			}
			else
			{
				//LogUnexpected("Message: Target LP not found: "+String(op->targetProcess),op->target);
				continue;
			}
		}

	}

	foreach (motionOps,op)
	{
		const auto opCoords = op->target.GetShardCoords();

		if (Entity*e = target.entities.FindEntity(op->target.guid))
		{
			if (!Vec::equal(op->origin.coordinates,e->coordinates))
			{
				if (Vec::equal(op->target.coordinates,e->coordinates))
					continue;	//some weird case
				/* this may imply:
					* an entity returned to an SD but has never actually left (due to inconsistency duplication)
					* an entity has entered the same SD from different origins (again, inconsistency duplication)

					In any event this is a conflict between the entity's current location and the one intended by this operation.
					Chosing either is correct as long as both target locations are of some inconsistency.
					If one is consistent then that must be chosen
				*/

				BYTE currentInconsistency = target.ic.GetInconsistency(::Frac(e->coordinates));
				BYTE targetInconsistency = target.ic.GetInconsistency(::Frac(op->target.coordinates));
				if (currentInconsistency < targetInconsistency)
				{
					count_t overload = Find(op->target,motionOps);
					LogUnexpected("Motion: OP origin mismatch (found "+String(overload)+" motion ops)",op->origin,e);
					continue;
				}

				LogUnexpected("Rectified: OP origin mismatch",op->origin,e);

				((EntityID&)*e) = op->target;
				((EntityShape&)*e) = op->shape;
				e->velocity = op->target.coordinates - op->origin.coordinates;
				e->logic.Clear();
				foreach (op->logic,l)
					((LogicState&)e->logic.Append()) = *l;

				//e->coordinates = op->target.coordinates;
				//e->velocity = op->target.coordinates - op->origin.coordinates;
				continue;

				//ChangeSet ops;
				//Filter(op->target.guid,ops);
			}
			if (!Metric::CheckDistance("Motion",op->target,*e,Entity::MaxMotionDistance))
				continue;
			
			if (opCoords == shardCoords)
			{
				e->coordinates = op->target.coordinates;
				e->velocity = op->target.coordinates - op->origin.coordinates;
			}
			else
				target.entities.RemoveEntity(e);
			continue;
		}
		else
		{
			if (opCoords != shardCoords)
			{
				LogUnexpected("Motion: Shard coordinate mismatch. Local="+shardCoords.ToString(),op->target);
				continue;
			}
			if (!Metric::CheckDistance("Motion",op->origin,op->target,Entity::MaxMotionDistance))
				continue;
			e = target.entities.CreateEntity(op->target.guid);
			if (!e)
			{
				LogUnexpected("Motion: Entity allocation failed",op->target);
				continue;
			}
			((EntityID&)*e) = op->target;
			((EntityShape&)*e) = op->shape;
			e->velocity = op->target.coordinates - op->origin.coordinates;
			foreach (op->logic,l)
				((LogicState&)e->logic.Append()) = *l;
		}

	}



	foreach (removalOps,op)
	{
		if (Entity*e = target.entities.FindEntity(op->target.guid))
		{
			if (!Vec::equal(op->target.coordinates,e->coordinates))
			{
				LogUnexpected("Removal: OP Target Mismatch",op->target,e);
				continue;
			}
			if (!Metric::CheckDistance("Removal",op->origin,*e,Entity::MaxInfluenceRadius))	//state advertisement is late one generation
				continue;

			target.entities.RemoveEntity(e);
		}
		else
		{
			LogUnexpected("Removal: Target entity not found",op->target);
			continue;
		}
	}
	foreach (instantiationOps,op)
	{
		const auto opCoords = op->target.GetShardCoords();
		if (opCoords != shardCoords)
		{
			LogUnexpected("Instantiation: Shard coordinate mismatch. Local="+shardCoords.ToString(),op->target);
			continue;
		}
		if (!Metric::CheckDistance("Instantiation",op->origin,op->target,Entity::MaxMotionDistance))	//state advertisement is late one generation
			continue;
		Entity*e = target.entities.CreateEntity(op->target.guid);
		if (!e)
		{
			LogUnexpected("Instantiation: Entity allocation failed",op->target);
			continue;
		}
		((EntityID&)*e) = op->target;
		((EntityShape&)*e) = op->shape;

		foreach (op->logic,l)
			((LogicState&)e->logic.Append()) = *l;
	}

	#ifndef NO_SENSORY
		foreach (target.entities,e)
		{
			e->neighborhood.Clear();
			foreach (stateAdvertisementOps,op)
			{
				if (e->guid != op->origin.guid && Metric::Distance(e->coordinates,op->origin.coordinates) <= Entity::MaxAdvertisementRadius)
				{
					EntityAppearance&app = e->neighborhood.Append();
					((EntityID&)app) = op->origin;
					((EntityShape&)app) = op->shape;
				}
			}
		}

		foreach (target.entities,e)
		{
			std::sort(e->neighborhood.begin(),e->neighborhood.end());
			//ByMethod::QuickSort(e->neighborhood);
		}
	#endif
}

void	ChangeSet::Add(const Entity&e, MessageDispatcher&dispatcher)
{
	foreach (dispatcher.messages,msg)
		messageOps.MoveAppend(*msg).SetOrigin(e); 
	foreach (dispatcher.broadcasts,msg)
		broadcastOps.MoveAppend(*msg).SetOrigin(e); 
}


void	ChangeSet::AddSelfMotion(const Entity&e, const TEntityCoords&newCoordinates)
{
	Op::Motion move;
	move.target = e;
	move.origin = e;
	move.shape = e;
	move.target.coordinates = newCoordinates;
	if (move.origin.GetShardCoords() != move.target.GetShardCoords())
	{
		move.logic.SetSize(e.logic.Count());
		auto*at = move.logic.pointer();
		foreach (e.logic,l)
		{
			*at = *l;
			at++;
		};
		ASSERT_CONCLUSION(move.logic,at);
	}
	Add(e,move);
}

template <typename Op>
	static void Filter(const GUID&eID, const Buffer0<Op>&source, Buffer0<Op>&dest)
	{
		foreach (source,op)
			if (op->origin.guid == eID)
				dest << *op;
	}

void	ChangeSet::Filter(const GUID&entityID, ChangeSet&addTo)	const
{
	::Filter(entityID,removalOps,addTo.removalOps);
	::Filter(entityID,instantiationOps,addTo.instantiationOps);
	::Filter(entityID,motionOps,addTo.motionOps);
	::Filter(entityID,messageOps,addTo.messageOps);
	::Filter(entityID,broadcastOps,addTo.broadcastOps);
	#ifndef NO_SENSORY
		::Filter(entityID,stateAdvertisementOps,addTo.stateAdvertisementOps);
	#endif
}

void LogUnexpected(const String & message, const EntityID & p0, const EntityID * p1)
{
	String msg = message + " p0="+ToString(p0.guid)+" (@"+p0.coordinates.ToString()+")";
	if (p1)
		msg += " p1="+ToString(p1->guid)+" (@"+p1->coordinates.ToString()+")";
	messageLogLock.lock();
	messageLog.MoveAppend(msg);
	messageLog.Truncate(5);
	messageLogLock.unlock();
}

