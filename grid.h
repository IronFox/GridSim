#pragma once

#include "types.h"
#include <math/vclasses.h>
#include "shard.h"



CONSTRUCT_ENUMERATION5(CorrectionStrategy,
						InterleavedParallel,
						LockstepCorrectOldest,
						BaseParallel,
						LockstepCorrectBase,
						ConservativeLockstepCorrectBase
						);

class Grid
{
public:
	class Layer;

	typedef GridArray<Shard>	ShardGrid;
	typedef ShardGrid::Size		GridSize;
private:
	void				PrecomputeAll(index_t gen);
	void				FinalizeAll(index_t gen);
	static bool			Deliver(Shard&,SDS&, Layer&l, bool require=false);
	TBoundaries			boundaries;
	M::TVec<count_t,Dimensions>		extent;
	Array<DownloadDemandRegistry>	demandRegistries;
public:

	struct TCorrectionStatistics
	{
		count_t			recoveryOps = 0,
						recoverCompletedSDS = 0;


		void			operator+=(const TCorrectionStatistics&other)
		{
			recoverCompletedSDS += other.recoverCompletedSDS;
			recoveryOps += other.recoveryOps;
		}
	};

	typedef TCorrectionStatistics	TCStat;


	class Layer
	{
	public:
		count_t			numCurrentEntities=0;
		ShardGrid		shardGrid;
		index_t			index = InvalidIndex;

		void			Create(const GridSize&size, Grid&, index_t layer);
		void			swap(Layer&other)	{shardGrid.swap(other.shardGrid);}
		void			adoptData(Layer&other)	{shardGrid.adoptData(other.shardGrid);}

		Shard*			GetShard(const TGridCoords&);
		const Shard*	GetShard(const TGridCoords&) const;
		bool			IsInconsistent(const TEntityCoords&coordinates, index_t generation) const;
		void			VerifyIsInconsistent(InconsistencyCoverage::TVerificationContext&context) const;
		const Entity*	FindFirstEntity(const GUID&, index_t generation) const;
		const Entity*	FindFirstEntity(const EntityID&, index_t generation) const;
	};
	
	Array<Layer,Swap>	layers;
	index_t				topGeneration=0,
						oldestLivingGeneration=0,
						recoveryIteration=0,
						recoveryAt=0;
	bool				lockAutoTrim=false;
	GridSize			currentSize;

	/**/				Grid()	{}

	void				Reset(const GridSize&size, count_t numLayers, const ArrayRef<Entity>&initialState)
	{
		currentSize = size;
		topGeneration = oldestLivingGeneration=recoveryIteration=recoveryAt=0;
		lockAutoTrim = false;
		layers.Clear();
		layers.SetSize(numLayers);
		foreach (layers,l)
		{
			l->Create(size,*this,l-layers.pointer());

			foreach (initialState,e)
			{
				auto shardCoords = e->GetShardCoords();
				auto*shard = l->GetShard(shardCoords);
				if (shard)
					shard->sds.Last().GetOutput()->entities.CreateEntity(*e);
			}
		}

		#ifdef D3
			boundaries  = M::Box<>(0,0,0,(float)size.width-0.001f,(float)size.height-0.001f,(float)size.depth-0.001f);
		#else
			boundaries  = M::Rect<>(0,0,(float)size.width-0.001f,(float)size.height-0.001f);
		#endif
		Vec::copy(ToVector(size),extent);
		ShardParallel([](Layer&l, Shard&s)
		{
			s.UploadInitialStates();
		});
	}

	/**
	Completely evolves the most recent SDS of the entire grid
	*/
	void				EvolveTop();

	/**
	Broadcasts the specified message to all sectors
	*/
	void				DispatchUserMessage(const GUID&target, LogicProcess targetProcess, const void*payload, size_t payloadSize);


	const TBoundaries&	GetBoundaries() const {return boundaries;}

	//void				LockstepCorrectOldest();
	TCStat				LockstepCorrectSelective(const std::function<index_t(Shard&)>&baseSelector);

	/**
	Corrects the oldest generation, and a definable number of extra generations of each shard.
	No communication is evaluated, but the result is trimmed.
	*/
	//void				CorrectSome(count_t extra);


	/**
	Utilizes smart generation selection to attempt correction of (partially) inconsistent SDS'.
	If the simulation provides two layers, the SDS' are merged where sibling shards are alive. Currently no constellation beyond two layers is supported for this.
	Trims away SDS that are no longer necessary as a result.
	@param exp Experiment configuration
	@param maxDemandRequestsPerShardToDatabase The maximum number of backuped SDS and RCS that may be retrieved from the persistent DB during the entire operation (not per iteration). May be 0
	@param postIterationCallback Callback function that is executed after each executed recovery iteration. May be empty
	*/
	TCStat				LockstepCorrectSome(const TExperiment&exp, const std::function<void()>&postIterationCallback, count_t maxDemandRequestsPerShardToDatabase);

	void				SynchronizeLayers();

	/**
	Reattempts delivery of all RCS of one generation to neighbors of all shards on all layers
	@param gen Generation to re-deliver
	@param connected Function to determine connectivity between two shards. If the function is defined,
	and returns false for two shards, then connection/delivery will fail in that direction.
	@return true if no errors occurred (as a result of calls to @a connected), false if any delivery failed
	*/
	bool				DeliverGeneration(index_t gen);
	bool				DeliverAll(bool requireAll=false);

	/**
	Flags a single point as inconsistent across all layers
	*/
	void				FlagInconsistent(const TEntityCoords&coords);

	/**
	Removes SDS that are no longer relevant for recovery and simulation
	*/
	void				Trim();
	void				UpdateOldestRecoverableGeneration();

	/**
	Removes all SDS older than the specified generation.
	The specified generation itself is preserved if existent
	*/
	void				TrimTo(index_t generation);
	/**
	Removes all SDS that are not contained in @a reference
	*/
	void				TrimTo(const Grid&reference);

	void				CompareConsistent(const Grid&)	const;

	void				VerifyIntegrity()	const;

	void				AssertIsRecovering();


	void				ShardParallel(const std::function<void(Layer&, Shard&)>&);
	void				ShardParallel(const std::function<void(const Layer&, const Shard&)>&)	const;
	void				ShardParallelW(const std::function<void(Layer&, Shard&, index_t workerIndex)>&);
	void				ShardParallelW(const std::function<void(const Layer&, const Shard&, index_t workerIndex)>&) const;
	void				ShardIterative(const std::function<void(Layer&, Shard&)>&);
	void				ShardIterative(const std::function<void(const Layer&, const Shard&)>&) const;
};


