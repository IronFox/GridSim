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

#define SIMULATION_FAILED_AT_DD	20


#define NO_SENSORY	//completely disables all entity sensory functionality
#define DISPLACED_MESSAGES //uses sensory logic on messages (radiated from future location, reduced range of motion and communication)
#define INT_MESSAGES // causes messages to be simple uint64 values as opposed to arbitrary byte sequences. massively reduces memory consumption

/*
CLAMP_ENTITIES and CLAMP_MESSAGES are now defined in the respective project settings. DO NOT uncomment them here.
*/
//#define CLAMP_ENTITIES	//disallow entity losses. disallows motion. DO NOT UNCOMMENT
//#define CLAMP_MESSAGES	//if entities are clamped (CLAMP_ENTITIES), their messages are relocated alongside. this can have negative impact on IC expansion. DO NOT UNCOMMENT



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
	typedef Index3D<index_t> Index;
	typedef Size3D<index_t> Size;


#else
	static const constexpr count_t Dimensions=2;
	static const constexpr count_t NumNeighbors = 8;
	template <typename T=float>
		using Volume = M::Rect<T>;

	#include <container/array2d.h>

	template <typename T,typename Strategy = typename Strategy::StrategySelector<T>::Default>
		using GridArray = Array2D<T,Strategy>;
	typedef Index2D<index_t> Index;
	typedef Size2D<index_t> Size;

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
inline M::Vec2<index_t>	ToVector(const Index2D<index_t>&s)
{
	return M::Vec2<index_t>(s.x,s.y);
}
inline M::Vec3<index_t>	ToVector(const Index3D<index_t>&s)
{
	return M::Vec3<index_t>(s.x,s.y,s.z);
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
//extern Sys::SpinLock	messageLogLock;
//extern Buffer0<String>	messageLog;
void LogMessage(const String&msg);

struct TExperiment;

class DownloadDemandRegistry;


inline hash_t Hash(const GUID&g)
{
	return HashValue() << g.Data1 << g.Data2 << g.Data3 << g.Data4;
}

struct TSDSCheckResult;

struct TMergeConfig;
