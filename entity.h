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
#include <container/sorter.h>


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

	int				CompareTo(const EntityID&other)	const
	{
		return memcmp(&guid,&other.guid,sizeof(guid));
	}

	bool			operator<(const EntityID&other) const
	{
		return CompareTo(other) < 0;
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

#ifndef NO_SENSORY
	typedef Buffer0<EntityAppearance>	EntityNeighborhood;
#endif

void	LogUnexpected(const String&message,const EntityID&p0, const EntityID*p1=nullptr);

#ifdef CAPTURE_MESSAGE_VOLUME
	class MessageData
	{
		Array<BYTE> payload;

	public:
		static std::atomic<size_t>	totalMessageVolume;

		/**/		MessageData()
		{}

		/**/		MessageData(const void*data, size_t dataSize):payload((const BYTE*)data,dataSize)
		{
			totalMessageVolume += dataSize;
		}

		/**/		MessageData(const MessageData&other):payload(other.payload)
		{
			totalMessageVolume += payload.Count();
		}

		virtual		~MessageData()
		{
			totalMessageVolume -= payload.Count();
		}

		void	ResizeAndCopy(const BYTE*data, size_t numBytes)
		{
			totalMessageVolume -= payload.Count();
			payload.ResizeAndCopy(data,numBytes);
			totalMessageVolume += payload.Count();
		}

		template <typename T>
		void	SetPOD(const T&item)
		{
			ResizeAndCopy((const BYTE*)&item,sizeof(T));
		}

		const BYTE* GetPointer() const
		{
			return payload.GetPointer();
		}

		size_t	Length() const {return payload.Length();}

		void	SetSize(size_t newSize)
		{
			if (newSize == payload.Length())
				return;

			totalMessageVolume -= payload.Count();
			payload.SetSize(newSize);
			totalMessageVolume += payload.Count();
		}

		void	operator=(const MessageData&data)
		{
			totalMessageVolume -= payload.Count();
			payload = data.payload;
			totalMessageVolume += payload.Count();
		}

		void	adoptData(MessageData&other)
		{
			totalMessageVolume -= payload.Count();
			payload.adoptData(other.payload);
			//no other change
		}

		int		CompareTo(const MessageData&other, int comparator(const BYTE&,const BYTE&)) const
		{
			return payload.CompareTo(other.payload,comparator);
		}

		operator const ArrayRef<BYTE>&() const
		{
			return payload;
		}

		void	swap(MessageData&other)
		{
			payload.swap(other.payload);
		}

		bool	operator==(const MessageData&other) const
		{
			return payload == other.payload;
		}

		bool	operator!=(const MessageData&other) const
		{
			return payload != other.payload;
		}

	};
#else
	typedef Array<BYTE>	MessageData;
#endif

typedef std::shared_ptr<MessageData>	PMessageData;

//typedef Array<BYTE>	MessageData;




#ifndef NO_SENSORY
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
#endif


class MessageReceiver;
class MessageDispatcher;

typedef void (*LogicProcess)(Array<BYTE>&serialState, count_t generation, const Entity&, EntityShape&inOutShape,Random&, const MessageReceiver&, MessageDispatcher&);


struct Message
{
	EntityID		sender;
	#ifndef ONE_LOGIC_PER_ENTITY
	LogicProcess	senderProcess=nullptr;
	#endif
	PMessageData	data;
	bool			isBroadcast = false;

	void			adoptData(Message&msg)
	{
		sender = msg.sender;
		#ifndef ONE_LOGIC_PER_ENTITY
			senderProcess = msg.senderProcess;
		#endif
		data.swap(msg.data);
		isBroadcast = msg.isBroadcast;
	}


	int			CompareTo(const Message&other) const
	{
		int cmp = memcmp(&sender,&other.sender,sizeof(sender));
		if (cmp != 0)
			return cmp;
		#ifndef ONE_LOGIC_PER_ENTITY
			if (senderProcess < other.senderProcess)
				return -1;
			if (senderProcess > other.senderProcess)
				return 1;
		#endif
		if (isBroadcast != other.isBroadcast)
			return isBroadcast ? 1 : -1;
		if (data == other.data)
			return 0;
		return data->CompareTo(*other.data,Compare::Primitives<BYTE>);
	}

	bool operator==(const Message&other) const
	{
		return CompareTo(other) == 0;
	}
	bool operator!=(const Message&other) const
	{
		return CompareTo(other) != 0;
	}

	void			Hash(Hasher&hash)	const
	{
		hash.AppendPOD(sender);
		#ifndef ONE_LOGIC_PER_ENTITY
			hash.AppendPOD(senderProcess);
		#endif
		hash.AppendPOD(isBroadcast);
		hash.Append(*data);
	}
};

DECLARE_DEFAULT_STRATEGY(Message,Adopt)


/**
Per-logic receiver container
*/
class MessageReceiver
{
	typedef Buffer0<Message> List;
	List			messages;
	bool			isSorted = true;	//empty is sorted

public:
	void			swap(MessageReceiver&other)
	{
		messages.swap(other.messages);
		swp(isSorted,other.isSorted);
	}

	void			Sort();
	bool			IsSorted() const {return isSorted;}
	void			Clear() {messages.Clear(); isSorted = true;}
	Message&		Append() {isSorted = false; return messages.Append();}
	void			Append(const Message&m) {Append() = m;}
	void			AppendMove(Message&m) {Append().adoptData(m);}
	void			Append(Message&&m) {AppendMove(m);}
	count_t			GetLength() const {return messages.GetLength();}

	bool			operator==(const MessageReceiver&other) const;

	typedef List::const_iterator const_iterator;
	const_iterator	begin() const {DBG_ASSERT__(isSorted); return messages.begin();}
	const_iterator	end() const {return messages.end();}

};

/**
LP and function associated with one entity.
Each entity can have multiple ones of these
*/
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

/**
The logic function, state, and incoming messages associated with one logic of one entity
*/
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

#ifndef ONE_LOGIC_PER_ENTITY
	/**
	Stack of logics assignable to one entity
	*/
	typedef Buffer0<LogicWorkspace,Swap>	LogicCluster;
#endif


/**
Single, complete entity in the simulated space.
Must be copyable
*/
struct Entity : public EntityAppearance
{
	typedef EntityAppearance	Super;
	static const float	MaxInfluenceRadius,	//!< Maximum possible distance at which one entity can affect another in any way from one generation to the next
					#ifndef NO_SENSORY
						MaxAdvertisementRadius,	//!< Maximum visibility distance between two entities
					#endif
						MaxMotionDistance;		//!< Maximum motion of an entity per generation
	#ifndef NO_SENSORY
		EntityNeighborhood	neighborhood;	//accumulated during CS/RCS evaluation
	#endif
	#ifndef ONE_LOGIC_PER_ENTITY
		LogicCluster				logic;
	#else
		LogicWorkspace				logic;
	#endif

	void			swap(Entity&other)
	{
		swp((Super&)*this, (Super&)other);
		#ifndef NO_SENSORY
			neighborhood.swap(other.neighborhood);
		#endif
		logic.swap(other.logic);
	}


	LogicWorkspace*				FindLogic(LogicProcess p)
	{
		#ifndef ONE_LOGIC_PER_ENTITY
			foreach (logic,l)
				if (l->process == p)
					return l;
		#else
			if (logic.process == p)
				return &logic;
		#endif
		return nullptr;
	}

	const LogicWorkspace*		FindLogic(LogicProcess p) const
	{
		#ifndef ONE_LOGIC_PER_ENTITY
			foreach (logic,l)
				if (l->process == p)
					return l;
		#else
			if (logic.process == p)
				return &logic;
		#endif
		return nullptr;
	}

	void						AddLogic(LogicProcess p)
	{
		#ifndef ONE_LOGIC_PER_ENTITY
			if (FindLogic(p))
				return;
			logic.Append().process = p;
		#else
			logic.process = p;
		#endif
	}

	void			Hash(Hasher&hash)	const
	{
		Super::Hash(hash);
		#ifndef NO_SENSORY
			hash.AppendPOD(neighborhood.Count());
			foreach (neighborhood,n)
				n->Hash(hash);
		#endif
		#ifndef ONE_LOGIC_PER_ENTITY
			foreach (logic,l)
				l->Hash(hash);
		#else
			logic.Hash(hash);
		#endif
	}



	bool			operator==(const Entity&other) const
	{
		return Vec::equal(this->coordinates,other.coordinates) 
		#ifndef NO_SENSORY
			&& neighborhood == other.neighborhood 
		#endif
			&& logic == other.logic 
			&& EntityShape::operator==(other);
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
			swp(originSector,other.originSector);
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
		#ifndef ONE_LOGIC_PER_ENTITY
			Array<LogicState>	logic;
		#else
			LogicState		logic;
		#endif
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

	inline bool MessagesEqual(const PMessageData&a, const PMessageData&b) {return a == b || (*a) == (*b);}


	class BaseMessage : public Base
	{
	public:
		#ifndef ONE_LOGIC_PER_ENTITY
			LogicProcess	targetProcess,sourceProcess;
		#endif
		PMessageData		message;

		void				swap(BaseMessage&other)
							{
									Base::swap(other); 
									#ifndef ONE_LOGIC_PER_ENTITY
										swp(targetProcess,other.targetProcess);
										swp(sourceProcess,other.sourceProcess);
									#endif
									message.swap(other.message);
							}
		bool				operator==(const BaseMessage&other) const
		{
			return Base::operator==(other) 
				#ifndef ONE_LOGIC_PER_ENTITY
					&& targetProcess == other.targetProcess 
					&& sourceProcess == other.sourceProcess 
				#endif
				&& MessagesEqual(message, other.message);
		}
		bool				operator!=(const BaseMessage&other) const {return !operator==(other);}

	};

	class Message : public BaseMessage, public Targeted
	{
	public:
		static const float	MaxRange;

		void				swap(Message&other)
							{
									BaseMessage::swap(other); 
									Targeted::swap(other);
							}

		bool				operator==(const Message&other) const
		{
			return BaseMessage::operator==(other) 
				&& Targeted::operator==(other);
		}
		bool				operator!=(const Message&other) const {return !operator==(other);}
	};

	class Broadcast : public BaseMessage
	{
	public:
		static const float	MaxRange;
		typedef TTrue		DispatchRadially;
	
	};

	#ifndef NO_SENSORY
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
	#endif
}

DECLARE_DEFAULT_STRATEGY(Op::Base,Swap)
DECLARE_DEFAULT_STRATEGY(Op::Removal,Swap)
DECLARE_DEFAULT_STRATEGY(Op::Instantiation,Swap)
DECLARE_DEFAULT_STRATEGY(Op::Message,Swap)
DECLARE_DEFAULT_STRATEGY(Op::Broadcast,Swap)
#ifndef NO_SENSORY
	DECLARE_DEFAULT_STRATEGY(Op::StateAdvertisement,Swap)
#endif

class MessageDispatcher
{
public:
	Buffer0<Op::Message>	messages;
	Buffer0<Op::Broadcast>	broadcasts;

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

	void	DispatchFleeting(const EntityID&targetEntity, LogicProcess targetProcess, PMessageData&&msg)
	{
		Op::Message&m = messages.Append();
		m.message = std::move(msg);
		#ifndef ONE_LOGIC_PER_ENTITY
			m.sourceProcess = fromProcess;
			m.targetProcess = targetProcess;
		#endif
		m.target = targetEntity;
	};

	void	DispatchCopy(const EntityID&targetEntity, LogicProcess targetProcess, const PMessageData&msg)
	{
		Op::Message&m = messages.Append();
		m.message = msg;
		#ifndef ONE_LOGIC_PER_ENTITY
			m.sourceProcess = fromProcess;
			m.targetProcess = targetProcess;
		#endif
		m.target = targetEntity;
	};

	template <typename T>
	void	BroadcastPOD(LogicProcess targetProcess, const T&msg)
	{
		auto&m = broadcasts.Append();
		
		#ifndef ONE_LOGIC_PER_ENTITY
			m.sourceProcess = fromProcess;
			m.targetProcess = targetProcess;
		#endif
		m.message.reset(new MessageData((const BYTE*)&msg,sizeof(msg)));
	}

	void	BroadcastFleeting(LogicProcess targetProcess, PMessageData&&msg)
	{
		auto&m = broadcasts.Append();
		m.message = std::move(msg);
		#ifndef ONE_LOGIC_PER_ENTITY
			m.sourceProcess = fromProcess;
			m.targetProcess = targetProcess;
		#endif
	};

	void	BroadcastCopy(LogicProcess targetProcess, const PMessageData&msg)
	{
		auto&m = broadcasts.Append();
		m.message = msg;
		#ifndef ONE_LOGIC_PER_ENTITY
			m.sourceProcess = fromProcess;
			m.targetProcess = targetProcess;
		#endif
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
	#ifndef NO_SENSORY
		Buffer0<Op::StateAdvertisement>	stateAdvertisementOps;
	#endif
	

	void	swap(ChangeSet&other)
	{
		removalOps.swap(other.removalOps);
		instantiationOps.swap(other.instantiationOps);
		motionOps.swap(other.motionOps);
		messageOps.swap(other.messageOps);
		broadcastOps.swap(other.broadcastOps);
		#ifndef NO_SENSORY
			stateAdvertisementOps.swap(other.stateAdvertisementOps);
		#endif
	}
	void	Clear()
	{
		removalOps.Clear();
		instantiationOps.Clear();
		motionOps.Clear();
		messageOps.Clear();
		broadcastOps.Clear();
		#ifndef NO_SENSORY
			stateAdvertisementOps.Clear();
		#endif
	}
	count_t	CountInstructions()	const
	{
		return removalOps.Count()
				+instantiationOps.Count()
				+motionOps.Count()
				+messageOps.Count()
				+broadcastOps.Count()
			#ifndef NO_SENSORY
				+stateAdvertisementOps.Count()
			#endif
			;
	}
	void	Add(const Entity&e, MessageDispatcher&dispatcher);
	void	Add(const Entity&e, const Op::Instantiation&op)	{instantiationOps.Append(op).SetOrigin(e);}
	void	Add(const Entity&e, const Op::Removal&op)		{removalOps.Append(op).SetOrigin(e);}
	void	Add(const Entity&e, const Op::Motion&op)		{motionOps.Append(op).SetOrigin(e);}
	void	Add(const Entity&e, const Op::Message&op)	{messageOps.Append(op).SetOrigin(e);}
	#ifndef NO_SENSORY
		void	Add(const Entity&e, const Op::StateAdvertisement&op)	{stateAdvertisementOps.Append(op).SetOrigin(e);}
	#endif
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


