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
using namespace DeltaWorks::Math;

//#define DBG_SHARD_HISTORY
//#define RECOVERY_TEST
//#define PERFORMANCE_TEST_TIMESTEPS	200


#if defined PERFORMANCE_TEST_TIMESTEPS && !defined RECOVERY_TEST
	#define RECOVERY_TEST
#endif


//#define D3	//3d-grid

#ifdef D3
	static const constexpr count_t Dimensions=3;
	static const constexpr count_t NumNeighbors = 26;
	template <typename T=float>
		using Volume = Box<T>;

	#include <container/array3d.h>

	template <typename T,typename Strategy = typename Strategy::StrategySelector<T>::Default>
		using GridArray = Array3D<T,Strategy>;

	typedef Size3D<index_t> GridSize;
	inline TVec3<index_t>	ToVector(const GridSize&s)
	{
		TVec3<index_t> rs;
		Vec::def(rs,s.width,s.height,s.depth);
		return rs;
	}
	template <typename T>
	inline Size3D<index_t>	VectorToSize(const TVec3<T>&v)
	{
		return Size3D<index_t> (v.x,v.y,v.z);
	}
	template <typename T>
	inline Index3D<index_t>	VectorToIndex(const TVec3<T>&v)
	{
		return Index3D<index_t> (v.x,v.y,v.z);
	}


#else
	static const constexpr count_t Dimensions=2;
	static const constexpr count_t NumNeighbors = 8;
	template <typename T=float>
		using Volume = Rect<T>;

	#include <container/array2d.h>

	template <typename T,typename Strategy = typename Strategy::StrategySelector<T>::Default>
		using GridArray = Array2D<T,Strategy>;


	typedef Size2D<index_t> GridSize;
	inline TVec2<index_t>	ToVector(const GridSize&s)
	{
		TVec2<index_t> rs;
		Vec::def(rs,s.width,s.height);
		return rs;
	}
	template <typename T>
	inline Size2D<index_t>	VectorToSize(const TVec2<T>&v)
	{
		return Size2D<index_t> (v.x,v.y);
	}
	template <typename T>
	inline Index2D<index_t>	VectorToIndex(const TVec2<T>&v)
	{
		return Index2D<index_t> (v.x,v.y);
	}
#endif

typedef VecN<int,Dimensions>	TGridCoords;
typedef VecN<float,Dimensions>	TEntityCoords;
typedef Volume<>				TBoundaries;



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
