#pragma once
#define DECL_TYPE(X)	class X; typedef std::shared_ptr<X> P##X;


//DECL_TYPE(Shard);
#include <global_string.h>
#include <container/buffer.h>
#include <io/cryptographic_hash.h>
#include <functional>
#include <general/thread_system.h>
#include <math/vclasses.h>
#include <Rpc.h>

using namespace DeltaWorks;

//#define DBG_SHARD_HISTORY
//#define RECOVERY_TEST
//#define PERFORMANCE_TEST_TIMESTEPS	200

/**
Extends information stored in each IC cell that can be used during merged.
Must be defined when testing merge strategies involving these flags:
	RegardOriginBitField
	RegardOriginRange
	RegardFuzzyOriginRange
*/
//#define EXTENDED_IC_GRID

/**
	Thoroughly checks equality with consistent match and that inconsistency is where you expect it to be.
	These checks are mostly redundant and should not trigger
*/
//#define THOROUGH_SDS_EVOLUTION_CHECK

/**
Replaces MessageData with a custom class to accumulate contained data
*/
//#define CAPTURE_MESSAGE_VOLUME

#define ONE_LOGIC_PER_ENTITY


#if defined PERFORMANCE_TEST_TIMESTEPS && !defined RECOVERY_TEST
	#define RECOVERY_TEST
#endif


#define NO_SENSORY	//completely disables all entity sensory functionality
#define DISPLACED_MESSAGES //uses sensory logic on messages (radiated from future location, reduced range of motion and communication)
#define INT_MESSAGES // causes messages to be simple uint64 values as opposed to arbitrary byte sequences. massively reduces memory consumption

#define CLAMP_ENTITIES	//disallow entity losses



#ifdef INT_MESSAGES
	typedef UINT64 MessageDataContainer;
	typedef Strategy::Copy MessageDataStrategy;

	inline int CompareMessages(MessageDataContainer a, MessageDataContainer b)
	{
		return a < b ? -1 : (a > b ? 1 : 0);
	}

	inline ArrayRef<const BYTE> ToArrayRef(const MessageDataContainer&a)
	{
		return ArrayRef<const BYTE>((const BYTE*)&a,sizeof(a));
	}

#else

	#ifdef CAPTURE_MESSAGE_VOLUME
		class CustomMessageData;
		typedef CustomMessageData MessageData;
	#else
		typedef Array<BYTE>	MessageData;
	#endif
	typedef std::shared_ptr<MessageData>	MessageDataContainer;
	DECLARE_DEFAULT_STRATEGY(MessageDataContainer,Swap);
	typedef Strategy::Swap MessageDataStrategy;

	inline int CompareMessage(const MessageDataContainer&a, const MessageDataContainer&b)
	{
		if (a == b)
			return 0;
		return a->CompareTo(*b,Compare::Primitives<BYTE>);
	}
	inline ArrayRef<const BYTE> ToArrayRef(const MessageDataContainer&a)
	{
		return *a;
	}
#endif



//#define D3	//3d-grid

#ifdef D3
	static const constexpr count_t Dimensions=3;
	static const constexpr count_t NumNeighbors = 26;
	template <typename T=float>
		using Volume = M::Box<T>;

	#include <container/array3d.h>

	template <typename T,typename Strategy = typename Strategy::StrategySelector<T>::Default>
		using GridArray = Array3D<T,Strategy>;


#else
	static const constexpr count_t Dimensions=2;
	static const constexpr count_t NumNeighbors = 8;
	template <typename T=float>
		using Volume = M::Rect<T>;

	#include <container/array2d.h>

	template <typename T,typename Strategy = typename Strategy::StrategySelector<T>::Default>
		using GridArray = Array2D<T,Strategy>;


#endif

typedef GridArray<int>::Size	GridSize;
typedef GridArray<int>::Index	GridIndex;

typedef M::VecN<int,Dimensions>	TGridCoords;
typedef M::VecN<float,Dimensions>	TEntityCoords;
typedef Volume<>				TBoundaries;

inline M::TVec2<count_t>	ToVector(const Size2D<count_t>&s)
{
	M::TVec2<count_t> rs;
	Vec::def(rs,s.width,s.height);
	return rs;
}
inline M::TVec3<count_t>	ToVector(const Size3D<count_t>&s)
{
	M::TVec3<count_t> rs;
	Vec::def(rs,s.width,s.height,s.depth);
	return rs;

}
inline M::TVec2<index_t>	ToVector(const Index2D<index_t>&s)
{
	M::TVec2<index_t> rs;
	Vec::def(rs,s.x,s.y);
	return rs;
}
inline M::TVec3<index_t>	ToVector(const Index3D<index_t>&s)
{
	M::TVec3<index_t> rs;
	Vec::def(rs,s.x,s.y,s.z);
	return rs;
}

template <typename T>
inline Size2D<index_t>	VectorToSize(const M::TVec2<T>&v)
{
	return Size2D<index_t> (v.x,v.y);
}
template <typename T>
inline Index2D<index_t>	VectorToIndex(const M::TVec2<T>&v)
{
	return Index2D<index_t> (v.x,v.y);
}
template <typename T>
inline Size3D<index_t>	VectorToSize(const M::TVec3<T>&v)
{
	return Size3D<index_t> (v.x,v.y,v.z);
}
template <typename T>
inline Index3D<index_t>	VectorToIndex(const M::TVec3<T>&v)
{
	return Index3D<index_t> (v.x,v.y,v.z);
}


class DataSize;
class Grid;
class Shard;
class FullShardDomainState;
class CoreShardDomainState;	//subset of FullShardDomainState which only contains entities and ic
typedef std::shared_ptr<CoreShardDomainState> PCoreShardDomainState;
//class RemoteChangeSet;
class ChangeSet;
class InconsistencyCoverage;
class RemoteChangeSet;
class HGrid;
class ExtHGrid;
struct Entity;
struct EntityID;
class EntityStorage;

namespace Database
{
	class Client;
	struct Result;
};

namespace DB=Database;


typedef FullShardDomainState SDS;
typedef ChangeSet CS;
typedef InconsistencyCoverage IC;
typedef RemoteChangeSet RCS;
typedef std::shared_ptr<RCS> PRCS;
//typedef RemoteChangeSet RCS;

typedef SHA1	Hasher;

typedef CoreShardDomainState	CSDS;

String ToString(const GUID&guid, bool full=false);
namespace DeltaWorks
{
	String ToString(const Hasher::HashContainer&guid, bool full=false);
	template <typename T>
		inline String		ToString(const Size2D<T>&s)	{using StringConversion::ToString; return ToString(s.width)+"*"+ToString(s.height);}
	template <typename T>
		inline String		ToString(const Size3D<T>&s)	{using StringConversion::ToString; return ToString(s.width)+"*"+ToString(s.height)+"*"+ToString(s.depth);}
	template <typename T>
		inline String		ToString(const Index2D<T>&s)	{using StringConversion::ToString; return "["+ToString(s.x)+","+ToString(s.y)+"]";}
	template <typename T>
		inline String		ToString(const Index3D<T>&s)	{using StringConversion::ToString; return "["+ToString(s.x)+","+ToString(s.y)+","+ToString(s.z)+"]";}

}
extern Sys::SpinLock	messageLogLock;
extern Buffer0<String>	messageLog;

struct TExperiment;

class DownloadDemandRegistry;


inline hash_t Hash(const GUID&g)
{
	return HashValue() << g.Data1 << g.Data2 << g.Data3 << g.Data4;
}


struct TSDSCheckResult
{
	count_t			missingRCS=0;
	count_t			rcsAvailableFromNeighbor=0,
					outRCSUpdatable=0,
					rcsAvailableFromDatabase=0,
					rcsRestoredFromCache = 0;
	bool			predecessorIsConsistent = false,
					thisIsConsistent = false;

	bool			AllThere() const {return missingRCS == 0;}
	bool			MissingAvailableFromNeighbors() const {return missingRCS == rcsAvailableFromNeighbor;}
	bool			MissingAvailableFromAnywhere() const {return missingRCS == rcsAvailableFromNeighbor + rcsAvailableFromDatabase;}
	bool			AnyAvailableFromNeighbors() const volatile {return rcsAvailableFromNeighbor > 0;}
	bool			AnyAvailableFromAnywhere() const {return rcsAvailableFromNeighbor > 0 || rcsAvailableFromDatabase > 0;}
	//bool			ShouldRecoverThis() const {return AnyAvailableFromNeighbors() || rcsRestoredFromCache > 0 || outRCSUpdatable > 0 || (missingRCS == 0 && predecessorIsConsistent);}
	bool			ShouldRecoverThis() const volatile {return !thisIsConsistent && (AnyAvailableFromNeighbors() || rcsRestoredFromCache > 0 || outRCSUpdatable > 0 || (missingRCS == 0 && predecessorIsConsistent) || rcsRestoredFromCache > 0);}

	void			AssertEqualTo(const TSDSCheckResult&other) const
	{
		ASSERT_EQUAL__(missingRCS,other.missingRCS);
		ASSERT_EQUAL__(rcsAvailableFromNeighbor,other.rcsAvailableFromNeighbor);
		ASSERT_EQUAL__(outRCSUpdatable,other.outRCSUpdatable);
		ASSERT_EQUAL__(rcsAvailableFromDatabase,other.rcsAvailableFromDatabase);
		ASSERT_EQUAL__(rcsRestoredFromCache,other.rcsRestoredFromCache);
		ASSERT_EQUAL__(predecessorIsConsistent,other.predecessorIsConsistent);
		ASSERT_EQUAL__(thisIsConsistent,other.thisIsConsistent);
	}
};
