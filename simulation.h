#pragma once

#include "types.h"
#include "grid.h"
#include <general/timer.h>
#include <math/SimplexNoise.h>
#include <future>



//#define ERRORS_IN_CENTRAL_NODE_ONLY


class Simulation
{
	Grid			simulated,
					control;
	Timer::Time		timeSpentSimulating=0,lastSceneChange;
	count_t			numIterations = 0, nodeDeathGeneration=0;
	GridSize		gridSize;
	bool			useControl=false,doNotVerify=false,forceRecovery=false,sceneOutdated=false;
	SimplexNoise	simplexNoise = SimplexNoise(0);
	RNG<std::mt19937_64>	random;
	std::atomic<ptrdiff_t>	extraNodes;
	std::future<bool>	future;
	count_t			deadNodes = 0, recoveringNodes = 0;
	std::atomic<count_t>	revivalDepth=0,revivalCount=0;

	count_t			Recover(Shard&s);

	bool			RunIteration(const TExperiment&exp, bool correctionIterationsAffectFaultProbability);

	void			ProcessNodeDeaths(float reliability);

	Buffer0<Shard*>	recovering;


	Grid::TCStat	lastRecoveryStats, totalRecoveryStats;
	count_t			totalRecoveryChances=0;
public:
	enum class Result
	{
		Okay,
		Failed,
		Delayed
	};

	/**
	Resets the local simulation to the given configuration.
	Any previous simulation data is discarded.
	@param width Number of horizontal shards
	@param height Number of vertical shards
	@param useControl Implements an additional independent simulation, that shares the local configuration, but suffers no faults (for comparison)
	@param simplexSeed Random seed to use for simplex random source
	@param numLayers Number of layers to use for the regular (faulty) simulation
	@param forceRecovery Disallows faults around a specific shard, once any samples in its IC reach an age of 255. Automatically re-enables faults once it recovered to 0.
	*/
	void		Reset(const GridSize&size, bool useControl, int64_t simplexSeed, count_t numLayers, const ArrayRef<Entity>&initialState);

	/**
	Runs a single simulation iteration
	@param reliability Percentage (in [0,1]) that a node will not die
	@param correctionIterations Number ot times correction should be computed before returning
	*/
	Result		RunIterationAsync(const TExperiment&exp, bool correctionIterationsAffectFaultProbability);

	void		RecoverOne(const TExperiment&exp, bool correctionIterationsAffectFaultProbability);
	bool		IsAsync() const;


	#ifdef INT_MESSAGES
		void	DispatchUserMessage(const GUID&target, LogicProcess targetProcess, UINT64 message);
	#else
		void	DispatchUserMessage(const GUID&target, LogicProcess targetProcess, const void*payload, size_t payloadSize);
	#endif

	count_t		CountSingleLayerShards() const {return simulated.layers.IsNotEmpty() ? simulated.layers.First().shardGrid.Count() : 1;}
	count_t		CountIterations() const {return numIterations;}
	index_t		GetTopGeneration() const;
	index_t		GetOldestGeneration() const;
	count_t		CountCurrentEntities() const;
	count_t		CountInconsistentSDS() const;
	count_t		CountConsistentTopGenerationSDS() const;
	count_t		CountUnconfirmedTopLevelUpdates() const;
	double		GetAverageRecoveryOps() const {return double(lastRecoveryStats.recoveryOps) / totalRecoveryChances;}
	double		GetAverageRecoverCompletedSDS() const {return double(lastRecoveryStats.recoverCompletedSDS) / totalRecoveryChances;}

	count_t		LastRecoveryOps() const {return lastRecoveryStats.recoveryOps;}
	count_t		LastRecoverCompletedSDS() const {return lastRecoveryStats.recoverCompletedSDS;}
	count_t		CountNonFullyAvailableNodes();

	count_t		GetTotalDeadNodes() const {return deadNodes;}
	double		GetRelativeNodeMortality() const {return double(deadNodes) / simulated.layers.Count() / simulated.layers.First().shardGrid.Count() / nodeDeathGeneration;}
	double		GetRelativeNodeRecovery() const {return double(recoveringNodes) / simulated.layers.Count() / simulated.layers.First().shardGrid.Count() / nodeDeathGeneration;}
	double		GetAverageRevivalDepth() const {return double(revivalDepth) / double(revivalCount);}


	void		Accumulate(Shard::TMergeStatistics&stat) const;


	GridSize	GetGridSize() const {return gridSize;}

	void		Rebuild();
	void		RebuildIfOudatedFor(float seconds);
	void		Verify();
	
	void		AssertIsRecovering();

	void		VerifyLocationConsistency(const Entity*e, index_t generation, const InconsistencyCoverage&ic) const;
	void		AccumulateInconsistentEntities(const TGridCoords&shardCoords, index_t generation, index_t layer, Image16&) const;
	void		AccumulateTraffic(const TGridCoords&shardCoords, index_t layer, index_t neighbor, DataSize&sent, DataSize&confirmed) const;
	bool		GetSDSCoverage(const TGridCoords&shardCoords,index_t generation,index_t layer, InconsistencyCoverage&outResult) const;
	//struct TMetrics
	//{
	//	count_t	controlEntities=0,
	//			simulatedEntites=0,
	//			inconsistentEntites=0,
	//			inconsistentlyMovedEntities=0,
	//			inconsistentlyLostEntites=0,
	//			inconsistentlyDuplicatedEntites=0;
	//	float	aggregatedInconsistency=0,
	//			highestEntityInconsistency=0;
	//};
	//
	//
	//void		AnalyzeGeneration(index_t generation, TMetrics&, float deathInconsistencyWeight=1.f) const;
};
