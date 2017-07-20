#pragma once

#include "types.h"
#include "DataSize.h"
#include <math/vclasses.h>
#include <functional>
#include "entity.h"
#include "SDS.h"
#include <engine/renderer/opengl.h>
#include "Database.h"


class DownloadDemandRegistry
{
private:
	struct TEntry
	{
		union
		{
			DB::SDS_ID	sdsID;
			DB::RCS_ID	rcsID;
		};
		union
		{
			DB::SDS	*sdsTarget;
			DB::RCS	*rcsTarget;
		};

		bool	isRCS=false;
		float	demand=0;

		/**/	TEntry(){}
	};
	Buffer0<TEntry>	entries;
	Shard			*shard;
public:
	/**/	DownloadDemandRegistry(Shard*shard=nullptr):shard(shard)	{}
	void	Reset(Shard*shard)	{entries.Clear(); this->shard = shard;}

	void	Register(float demand, const DB::SDS_ID&id, DB::SDS&target);
	void	Register(float demand, const DB::RCS_ID&id, DB::RCS&target);

	count_t	Pull(count_t maxRequests);

	Shard&	GetShard() {return *shard;}
};


/**
Maps a coordinate space in [0,1)
*/
class Shard
{
public:

	struct TNeighbor
	{
		Shard				*shard = nullptr;
		TGridCoords			delta;
		DataSize			dataSent,dataConfirmed;
		index_t				inboundIndex=0;
	};

	struct TLiveliness
	{
		enum state_t
		{
			Dead,
			Recovering,
			Alive,
		};

		state_t				state = Alive;
		index_t				diedInTimestep=InvalidIndex;
		index_t				userValue = InvalidIndex;
	};

	bool					faultFree = false;	//!< Indicates that connections from/to this nose never fail. May be used as a balancing mechanism in case a node has degenerated too much

	Shard					*consistentMatch=nullptr;
	Grid					*parentGrid=nullptr;

	RNG<std::mt19937>		syncRandomSource;

	#ifdef DBG_SHARD_HISTORY
		struct HistoryElement
		{
			index_t			localIndex;
			TSDSCheckResult	checkResult;
			bool			significantInboundChange;

		};
		Buffer0<String>		newHistory,recordedHistory;
		SpinLock			eventLock;
	#endif

	TLiveliness				liveliness;
	Buffer0<SDS>			sds;
	TGridCoords				gridCoords;
	index_t					linearCoords=InvalidIndex;
	index_t					layer=0;;
	index_t					recoveryAt=InvalidIndex,
							rrAt=0,user=InvalidIndex,user2=InvalidIndex;
	Buffer0<Op::Message>	userMessages;
	count_t					recoveryOperations=0;
	index_t					newestConsistentGeneration=0;	//!< Tracks the newest consistent generation. It is MANDATORY, that this value is updated only when Trim() is invoked on the grid. In essence this value may be OUTDATED
	/**
	The oldest inconsistent SDS generation that requires remote RCS to reach consistency
	*/
	index_t					oldestNonIsolatedInconsistentGeneration=0;
	index_t					oldestRecoverableGeneration=InvalidIndex;	//synchronized among neighbors. 
	Database::Client		client;

	FixedArray<TNeighbor,NumNeighbors>	neighbors;


	struct TMergeStatistics
	{
		count_t				copyOutCounter=0,
							copyInCounter=0,
							totalMerges=0,
							perfectMerges=0,
							improvingMerges=0,
							icSizeReducingMerges=0,
							failedMerges=0;

		void operator+=(const TMergeStatistics&other)
		{
			copyOutCounter += other.copyOutCounter;
			copyInCounter += other.copyInCounter;
			totalMerges += other.totalMerges;
			perfectMerges += other.perfectMerges;
			improvingMerges += other.improvingMerges;
			icSizeReducingMerges += other.icSizeReducingMerges;
			failedMerges += other.failedMerges;
		}
	};

	TMergeStatistics		mergeStats;

	/**/					Shard();

	index_t					NewestConsistentSDSIndex() const;
	index_t					NewestDefinedSDSIndex() const;

	void					VerifyIntegrity() const;

	const SDS&				NewestConsistentSDS() const
	{
		return sds[NewestConsistentSDSIndex()];
	}
	SDS&				NewestConsistentSDS()
	{
		return sds[NewestConsistentSDSIndex()];
	}

	#ifdef DBG_SHARD_HISTORY
		void				LogEvent(const String&str);
	#endif

	void					RequestHighestDemand(count_t limit, DownloadDemandRegistry&reg);
	bool					IsDead() const {return liveliness.state == TLiveliness::Dead;}
	bool					IsFullyAvailable() const {return liveliness.state == TLiveliness::Alive;}
	void					Die(index_t topGeneration);
	void					Setup(Grid&, const TGridCoords&myCoords, index_t myLinearCoords, index_t layer);
	static index_t			NeighborToLinear(const TGridCoords&delta);
	void					UploadInitialStates();

	/**
	Removes all consistent and confirmed SDS up to the first non-confirmed or inconsistent SDS.
	Does not remove the last SDS
	*/
	void					Trim();
	void					UpdateOldestRecoverableGeneration();


	void					TrimTo(index_t gen);

	SDS*					FindGeneration(index_t);
	const SDS*				FindGeneration(index_t)	const;
	index_t					FindGenerationIndex(index_t)	const;

	bool					CorrectIsolatedSDS(const TBoundaries&boundaries, index_t opIndex, index_t currentTimestep);

	void					CorrectSome(count_t extra, const TBoundaries&boundaries, index_t opIndex, index_t currentTimestep);
	bool					RecomputeSDS(index_t index, const TBoundaries&boundaries, index_t opIndex, index_t currentTimestep);

	void					CompareConsistent(const Shard&)	const;
	void					AssertIsEqual(const Shard&other) const;



};
