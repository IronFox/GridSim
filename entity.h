#pragma once

#include "types.h"
#include <math/vclasses.h>
#include <functional>
#include <container/buffer.h>
#include <container/hashtable.h>
#include <general/random.h>
#include <io/cryptographic_hash.h>
#include <math/object.h>
#include "metric.h"


inline float2 Frac(const M::TVec2<>&v)	{return float2(M::Frac(v.x),M::Frac(v.y));}
inline float3 Frac(const M::TVec3<>&v)	{return float3(M::Frac(v.x),M::Frac(v.y),M::Frac(v.z));}


/**
Full identifier of an entity.
The definition also includes coordinates to ease update dispatching
*/
struct EntityID
{
	TEntityCoords		coordinates;	//absolute coordinates
	GUID				guid;


	void				swap(EntityID&other)
	{
		swp(coordinates,other.coordinates);
		swp(guid,other.guid);
	}

	bool				operator==(const EntityID&other) const
	{
		return coordinates == other.coordinates && guid == other.guid;
	}

	void				GenerateID()
	{
		HRESULT hr = CoCreateGuid(&guid);
		ASSERT__(hr == S_OK);
	}

	TGridCoords		GetShardCoords() const
	{
		return GetShardCoords(coordinates);
	}
	static TGridCoords			GetShardCoords(const TEntityCoords&coordinates)
	{
		TGridCoords rs;
		Vec::copy(coordinates,rs);	//floor for positive values
		return rs;
	}

	void			Hash(Hasher&hash)	const
	{
		hash.AppendPOD(coordinates);
		hash.AppendPOD(guid);
	}

	int				compareTo(const EntityID&other)	const
	{
		return memcmp(&guid,&other.guid,sizeof(guid));
	}

	bool			operator<(const EntityID&other) const
	{
		return compareTo(other) < 0;
	}
};

struct EntityError
{
	float		lastOmega = 0,	
				omega = 0;

	void		ProgressOmega(float error)
	{
		lastOmega = omega;
		omega += error;
	}

	void		ReplaceLastProgression(float error)
	{
		omega = lastOmega + error;
	}
};


struct EntityShape
{
	enum class Shape
	{
		None,
		LineCube,
		SolidCube,
		Sphere
	};

	Shape				shape = Shape::None;
	Metric::Direction	orientation;
	TEntityCoords		scale = TEntityCoords(0.01f);
	float3				color;
	float				zOffset=0;
	TEntityCoords		velocity;	//distance that this entity has moved during the last generation
	UINT32				userFlags = 0;

	bool	operator==(const EntityShape&other) const
	{
		return shape == other.shape && orientation == other.orientation && scale == other.scale && color == other.color && zOffset == other.zOffset && velocity == other.velocity && userFlags == other.userFlags;
	}
	bool	operator!=(const EntityShape&other) const {return !operator==(other);}

	/**
	Fetches a unit vector of the current entity direction. The resulting vector will be of unit length in accordance with the currently applied metric
	*/
	void	GetDirection(TEntityCoords& rs) const	{orientation.ToVector(rs);}
};


/*abstract*/ class Collider
{
public:
	virtual bool	TestEdge(const TEntityCoords&e0, const TEntityCoords&e1, bool prognosedPosition, float*optionalOutDistance=nullptr) const =0;
};

struct EntityAppearance : public EntityID, public EntityShape, public Collider, public EntityError
{
	typedef EntityID Super;

	virtual bool		Collider::TestEdge(const TEntityCoords&e0, const TEntityCoords&e1,bool prognosedPosition, float*optionalOutDistance=nullptr) const override
	{
		float r = scale.GetLength();
		M::TVec2<> d;
		bool rs = M::Obj::DetectSphereEdgeIntersection(prognosedPosition ? coordinates+velocity : coordinates,r,e0,e1,d);
		if (optionalOutDistance)
		{
			if (rs)
			{
				(*optionalOutDistance) = Math::smallestPositiveResult(d.v,2);
			}
			else
				(*optionalOutDistance) = std::numeric_limits<float>::max();
		}
		return rs;
	}

	bool	operator==(const EntityAppearance&other) const
	{
		return EntityID::operator==(other) && EntityShape::operator==(other);
	}
	bool	operator!=(const EntityAppearance&other) const
	{
		return !operator==(other);
	}

};
typedef Buffer0<EntityAppearance>	EntityNeighborhood;

void	LogUnexpected(const String&message,const EntityID&p0, const EntityID*p1=nullptr);


typedef Array<BYTE>	MessageData;




class NeighborhoodCollider : public Collider
{
public:
	typedef std::function<bool(const EntityAppearance&)>	FFilter;
	const EntityNeighborhood&	hood;
	const FFilter				filter;
	/**/		NeighborhoodCollider(const EntityNeighborhood&n, const FFilter&filter = FFilter()):hood(n),filter(filter)
	{}
	
	virtual bool		Collider::TestEdge(const TEntityCoords&e0, const TEntityCoords&e1,bool prognosedPosition, float*optionalOutDistance=nullptr) const override
	{
		bool rs = false;
		float d = std::numeric_limits<float>::max();
		foreach (hood,n)
		{
			if (filter && !filter(*n))
				continue;
			float d2;
			if (n->TestEdge(e0,e1,prognosedPosition,&d2))
			{
				if (!optionalOutDistance)
					return true;
				rs = true;
				d = M::Min(d,d2);
			}
		}
		if (optionalOutDistance)
			(*optionalOutDistance) = d;
		return rs;
	}
};


class MessageReceiver;
class MessageDispatcher;

typedef void (*LogicProcess)(Array<BYTE>&serialState, count_t generation, const Entity&, EntityShape&inOutShape,Random&, const MessageReceiver&, MessageDispatcher&);


struct Message
{
	EntityID		sender;
	LogicProcess	senderProcess=nullptr;
	MessageData		data;

	void			adoptData(Message&msg)
	{
		sender = msg.sender;
		senderProcess = msg.senderProcess;
		data.adoptData(msg.data);
	}


	int			compareTo(const Message&other) const
	{
		int cmp = memcmp(&sender,&other.sender,sizeof(sender));
		if (cmp != 0)
			return cmp;
		if (senderProcess < other.senderProcess)
			return -1;
		if (senderProcess > other.senderProcess)
			return 1;
		return data.CompareTo(other.data,Compare::Primitives<BYTE>);
	}

	bool operator==(const Message&other) const
	{
		return compareTo(other) == 0;
	}
	bool operator!=(const Message&other) const
	{
		return compareTo(other) != 0;
	}

	void			Hash(Hasher&hash)	const
	{
		hash.AppendPOD(sender);
		hash.AppendPOD(senderProcess);
		hash.Append(data);
	}
};

DECLARE_DEFAULT_STRATEGY(Message,Adopt)

class MessageReceiver : public Buffer0<Message>	
{
public:
	
};

class LogicState
{
public:
	LogicProcess	process;
	Array<BYTE>		state;
	
	bool			operator==(const LogicState&other)	const
	{
		return process == other.process && state == other.state;
	}
	bool			operator!=(const LogicState&other)	const
	{
		return !operator==(other);
	}

	void			swap(LogicState&other)
	{
		swp(process,other.process);
		state.swap(other.state);
	}

	template <typename H>
	void			Hash(H&hash)	const
	{
		hash.AppendPOD(process);
		hash.Append(state);
	}
};

class LogicWorkspace : public LogicState
{
	typedef LogicState	Super;
public:
	MessageReceiver	receiver;
	
	void			swap(LogicWorkspace&other)
	{
		Super::swap(other);
		receiver.swap(other.receiver);
	}

	template <typename H>
	void			Hash(H&hash)	const
	{
		Super::Hash(hash);
		hash.AppendPOD(receiver.GetLength());
		foreach (receiver,m)
			m->Hash(hash);
	}


	bool	operator==(const LogicWorkspace&other) const
	{
		return LogicState::operator==(other) && receiver == other.receiver;

	}
};

typedef Buffer0<LogicWorkspace,Swap>	LogicCluster;


/**
Single, complete entity in the simulated space.
Must be copyable
*/
struct Entity : public EntityAppearance
{
	typedef EntityAppearance	Super;
	static const float	MaxInfluenceRadius,	//!< Maximum possible distance at which one entity can affect another in any way from one generation to the next
						MaxAdvertisementRadius,	//!< Maximum visibility distance between two entities
						MaxMotionDistance;		//!< Maximum motion of an entity per generation
	EntityNeighborhood	neighborhood;	//accumulated during CS/RCS evaluation
	LogicCluster				logic;

	void			swap(Entity&other)
	{
		swp((Super&)*this, (Super&)other);
		neighborhood.swap(other.neighborhood);
		logic.swap(other.logic);
	}


	LogicWorkspace*				FindLogic(LogicProcess p)
	{
		foreach (logic,l)
			if (l->process == p)
				return l;
		return nullptr;
	}

	const LogicWorkspace*		FindLogic(LogicProcess p) const
	{
		foreach (logic,l)
			if (l->process == p)
				return l;
		return nullptr;
	}

	void						AddLogic(LogicProcess p)
	{
		if (FindLogic(p))
			return;
		logic.Append().process = p;
	}

	void			Hash(Hasher&hash)	const
	{
		Super::Hash(hash);
		hash.AppendPOD(neighborhood.Count());
		foreach (neighborhood,n)
			n->Hash(hash);
		foreach (logic,l)
			l->Hash(hash);
	}



	bool			operator==(const Entity&other) const
	{
		return Vec::equal(this->coordinates,other.coordinates) && neighborhood == other.neighborhood && logic == other.logic && EntityShape::operator==(other);
	}

	bool			operator!=(const Entity&other) const
	{
		return !operator==(other);
	}

};
DECLARE_DEFAULT_STRATEGY(Entity, Swap)

namespace Op
{

	class Targeted
	{
	public:
		EntityID			target;

		void				swap(Targeted&other)	{target.swap(other.target);}
		bool				operator==(const Targeted&other) const {return target == other.target;}
		bool				operator!=(const Targeted&other) const {return !operator==(other);}
	};

	class Base
	{
	public:
		EntityID			origin;
		TGridCoords			originSector;
		typedef TFalse		DispatchToOrigin;
		typedef TFalse		DispatchRadially;

		void				swap(Base&other)
		{
			origin.swap(other.origin);
		}

		void				SetOrigin(const EntityID&es)
		{
			origin = es;
			originSector = es.GetShardCoords();
		}
		bool				operator==(const Base&other) const {return origin == other.origin && originSector == other.originSector;}
		bool				operator!=(const Base&other) const {return !operator==(other);}
	};

	class Removal : public Base, public Targeted
	{
	public:
		static const float	MaxRange;
		void				swap(Removal&other)	{Base::swap(other); Targeted::swap(other);}
		bool				operator==(const Removal&other) const {return Base::operator==(other) && Targeted::operator==(other);}
		bool				operator!=(const Removal&other) const {return !operator==(other);}
	};

	class Instantiation : public Base, public Targeted
	{
	public:
		static const float	MaxRange;
		Array<LogicState>	logic;
		EntityShape			shape;

		void				swap(Instantiation&other)	{Base::swap(other); Targeted::swap(other); logic.swap(other.logic); swp(shape,other.shape);}
		bool				operator==(const Instantiation&other) const {return Base::operator==(other) && Targeted::operator==(other) && logic == other.logic && logic == other.logic;}
		bool				operator!=(const Instantiation&other) const {return !operator==(other);}
	};

	class Motion : public Instantiation
	{
	public:
		static const float	MaxRange;
		typedef TTrue		DispatchToOrigin;
	};

	class Message : public Base, public Targeted
	{
	public:
		static const float	MaxRange;
		LogicProcess		targetProcess,sourceProcess;
		MessageData			message;

		void				swap(Message&other)	{Base::swap(other); 
									Targeted::swap(other);
									swp(targetProcess,other.targetProcess);
									swp(sourceProcess,other.sourceProcess);
									message.swap(other.message);}
		bool				operator==(const Message&other) const {return Base::operator==(other) && Targeted::operator==(other) && targetProcess == other.targetProcess && sourceProcess == other.sourceProcess && message == other.message;}
		bool				operator!=(const Message&other) const {return !operator==(other);}
	};

	class Broadcast : public Base
	{
	public:
		static const float	MaxRange;
		typedef TTrue		DispatchRadially;
		LogicProcess		targetProcess,sourceProcess;
		MessageData			message;
		
		void				swap(Broadcast&other)
							{
								Base::swap(other); 
								swp(targetProcess,other.targetProcess);
								swp(sourceProcess,other.sourceProcess);
								message.swap(other.message);
							}
		bool				operator==(const Broadcast&other) const {return Base::operator==(other) && targetProcess == other.targetProcess && sourceProcess == other.sourceProcess && message == other.message;}
		bool				operator!=(const Broadcast&other) const {return !operator==(other);}
	};

	class StateAdvertisement : public Base
	{
	public:
		static const float	MaxRange;
		typedef TTrue		DispatchRadially;
		EntityShape			shape;

		/**/				StateAdvertisement(){}
		/**/				StateAdvertisement(const EntityShape&shape):shape(shape){}
		void				swap(StateAdvertisement&other)	{Base::swap(other);swp(shape,other.shape);}
		bool				operator==(const StateAdvertisement&other) const {return Base::operator==(other) && shape == other.shape;}
		bool				operator!=(const StateAdvertisement&other) const {return !operator==(other);}
		void				SetOrigin(const EntityID&es)
		{
			Base::SetOrigin(es);
			origin.coordinates += shape.velocity;
		}
	};
}

DECLARE_DEFAULT_STRATEGY(Op::Base,Swap)
DECLARE_DEFAULT_STRATEGY(Op::Removal,Swap)
DECLARE_DEFAULT_STRATEGY(Op::Instantiation,Swap)
DECLARE_DEFAULT_STRATEGY(Op::Message,Swap)
DECLARE_DEFAULT_STRATEGY(Op::Broadcast,Swap)
DECLARE_DEFAULT_STRATEGY(Op::StateAdvertisement,Swap)

class MessageDispatcher
{
public:
	Buffer0<Op::Message>	messages;

	LogicProcess			fromProcess;

	void	Init(LogicProcess p)
	{
		fromProcess = p;
		messages.Clear();
	}

	template <typename T>
	void	DispatchPOD(const EntityID&targetEntity, LogicProcess targetProcess, const T&msg)
	{
		Op::Message&m = messages.Append();
		
		m.sourceProcess = fromProcess;
		m.target = targetEntity;
		m.targetProcess = targetProcess;
		m.message.SetSize(sizeof(T));
		memcpy(m.message.pointer(),&msg,sizeof(T));
	}

	void	DispatchFleeting(const EntityID&targetEntity, LogicProcess targetProcess, MessageData&&msg)
	{
		Op::Message&m = messages.Append();
		m.message.adoptData(msg);
		m.sourceProcess = fromProcess;
		m.target = targetEntity;
		m.targetProcess = targetProcess;
	};

	void	DispatchCopy(const EntityID&targetEntity, LogicProcess targetProcess, const MessageData&msg)
	{
		Op::Message&m = messages.Append();
		m.message = msg;
		m.sourceProcess = fromProcess;
		m.target = targetEntity;
		m.targetProcess = targetProcess;
	};
};


class ChangeSet
{
public:
	Buffer0<Op::Removal>			removalOps;
	Buffer0<Op::Instantiation>		instantiationOps;
	Buffer0<Op::Motion>				motionOps;
	Buffer0<Op::Message>			messageOps;
	Buffer0<Op::Broadcast>			broadcastOps;
	Buffer0<Op::StateAdvertisement>	stateAdvertisementOps;
	

	void	swap(ChangeSet&other)
	{
		removalOps.swap(other.removalOps);
		instantiationOps.swap(other.instantiationOps);
		motionOps.swap(other.motionOps);
		messageOps.swap(other.messageOps);
		broadcastOps.swap(other.broadcastOps);
		stateAdvertisementOps.swap(other.stateAdvertisementOps);
	}
	void	Clear()
	{
		removalOps.Clear();
		instantiationOps.Clear();
		motionOps.Clear();
		messageOps.Clear();
		broadcastOps.Clear();
		stateAdvertisementOps.Clear();
	}
	count_t	CountInstructions()	const
	{
		return removalOps.Count()
				+instantiationOps.Count()
				+motionOps.Count()
				+messageOps.Count()
				+broadcastOps.Count()
				+stateAdvertisementOps.Count();
	}
	void	Add(const Entity&e, MessageDispatcher&dispatcher)	{foreach (dispatcher.messages,msg) messageOps.MoveAppend(*msg).SetOrigin(e); }
	void	Add(const Entity&e, const Op::Instantiation&op)	{instantiationOps.Append(op).SetOrigin(e);}
	void	Add(const Entity&e, const Op::Removal&op)		{removalOps.Append(op).SetOrigin(e);}
	void	Add(const Entity&e, const Op::Motion&op)		{motionOps.Append(op).SetOrigin(e);}
	void	Add(const Entity&e, const Op::Message&op)	{messageOps.Append(op).SetOrigin(e);}
	void	Add(const Entity&e, const Op::StateAdvertisement&op)	{stateAdvertisementOps.Append(op).SetOrigin(e);}
	void	AddSelfMotion(const Entity&e, const TEntityCoords&newCoordinates);

	void	AssertEqual(const ChangeSet&other)	const;
	bool	operator==(const ChangeSet&other)	const;

	void	ExportEdge(const TGridCoords&relative, const TGridCoords&originSectorCoordinates,ChangeSet&target) const;
	void	Merge(const ChangeSet&other);
	void	Apply(const TGridCoords&shardCoords,CoreShardDomainState&) const;

	void	Filter(const GUID&entityID, ChangeSet&addTo)	const;

	template <typename Op>
		static count_t	Find(const EntityID&entityID, const BasicBuffer<Op>&list)
		{
			count_t rs = 0;
			foreach (list,op)
				if (op->origin.guid == entityID.guid)
					rs++;
			return rs;
		}

};

DECLARE_DEFAULT_STRATEGY(ChangeSet,Swap);


