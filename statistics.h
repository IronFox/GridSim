#pragma once

#include <global_string.h>
#include "InconsistencyCoverage.h"
#include "simulation.h"
#include <io/xml.h>
	


namespace Statistics
{
	void	Include(const IC::TSample& textureInconsistency, count_t inconsistentEntities, count_t totalEntities);

	float	GetMean(const IC::TSample& textureInconsistency);
	
	void	ExportToFile(const PathString&);
	void	ImportMean(const PathString&);

	bool		DoCompareEntityConsistency();

	TExperiment	SignalRecoveryFailed(const Simulation&sim);
	TExperiment	SignalSimulationEnd(const Simulation&sim);

	TExperiment	Begin();




	template <typename T>
	struct TSDSample
	{
		T		sum = 0,
				squareSum = 0;
		count_t numSamples = 0;


		/**/	TSDSample()	{}
		/**/	TSDSample(T sum, count_t numSamples =1):sum(sum),squareSum(sum*sum), numSamples(numSamples)
		{}
		/**/	TSDSample(T sum, T squareSum, count_t numSamples):sum(sum),squareSum(squareSum), numSamples(numSamples)
		{}

		void	operator+=(const TSDSample<T>&other)
		{
			sum += other.sum;
			squareSum += other.squareSum;
			numSamples += other.numSamples;
		}
		void	operator+=(T v)
		{
			sum += v;
			squareSum += v*v;
			numSamples ++;
		}

		TSDSample<T> operator+(const TSDSample<T>&other) const
		{
			return TSDSample<T>(sum + other.sum,squareSum + other.squareSum, numSamples + other.numSamples);
		}

		void	ToXML(XML::Node&outNode) const
		{
			double mean = (double)sum/double(numSamples);
			double sqrMean = (double)squareSum/double(numSamples);
			double deviation = sqrt(sqrMean - sqr(mean));

			outNode.Set("mean",mean);
			outNode.Set("deviation",deviation);
			outNode.Set("sum",sum);
			outNode.Set("squareSum",squareSum);
			outNode.Set("numSamples",numSamples);
		}

		void	ToXML(XML::Node&outNode, const TSDSample<T>&min, const TSDSample<T>&max,const TSDSample<T>&avg) const
		{
			double mean = (double)sum/double(numSamples);
			double sqrMean = (double)squareSum/double(numSamples);
			double deviation = sqrt(sqrMean - sqr(mean));

			const float av = avg.Get();
			const float mn = min.Get();
			const float mx = max.Get();
			ASSERT_LESS_OR_EQUAL__(mn,av);
			ASSERT_LESS_OR_EQUAL__(av,mx);
			const float rel = mean - av;
			const float rng = rel < 0 ? (av - mn) : (mx - av);
			outNode.Set("relative",rel/rng);
			outNode.Set("mean",mean);
			outNode.Set("deviation",deviation);
			outNode.Set("sum",sum);
			outNode.Set("squareSum",squareSum);
			outNode.Set("numSamples",numSamples);
		}



		void	Import(const XML::Node*node)
		{
			if (!node)
				return;
			Import(*node,"sum",sum);
			Import(*node,"squareSum",squareSum);
			Import(*node,"numSamples",numSamples);
		}

		template <typename T1>
		static void	Import(const XML::Node&node, const char*key,T1&value)
		{
			StringRef sval;
			ASSERT1__(node.Query(key,sval),key);
			ASSERT1__(convert(sval.pointer(),sval.GetLength(),value),sval);
		}


		float	Get() const
		{
			return numSamples ? (float)sum / numSamples : 0;
		}
	};


	struct TStateDifference
	{
		TSDSample<double>	inconsistency,
							omega,
							icSize,	//!< Number of inconsistent IC cells
							entitiesInInconsistentArea,
							missingEntities,	//!< Entities that should be there according to consistent state, but were not found in local state
							overAccountedEntities;	//!< Entities in the local state, that are not supposed to be here according to the consistent state

		/**/				TStateDifference()	{};
		static TStateDifference Minimum(const TStateDifference&a, const TStateDifference&b)
		{
			TStateDifference rs;
			rs.omega = std::min(a.omega.Get(),b.omega.Get());
			rs.inconsistency = std::min(a.inconsistency.Get(),b.inconsistency.Get());
			rs.icSize = std::min(a.icSize.Get(),b.icSize.Get());	//might make more sense to compare to minimum here
			rs.entitiesInInconsistentArea = std::min(a.entitiesInInconsistentArea.Get(),b.entitiesInInconsistentArea.Get());
			rs.missingEntities = std::min(a.missingEntities.Get(),b.missingEntities.Get());
			rs.overAccountedEntities = std::min(a.overAccountedEntities.Get(),b.overAccountedEntities.Get());
			return rs;
		}
		static TStateDifference Maximum(const TStateDifference&a, const TStateDifference&b)
		{
			TStateDifference rs;
			rs.omega = std::max(a.omega.Get(),b.omega.Get());
			rs.inconsistency = std::max(a.inconsistency.Get(),b.inconsistency.Get());
			rs.icSize = std::max(a.icSize.Get(),b.icSize.Get());	//might make more sense to compare to minimum here
			rs.entitiesInInconsistentArea = std::max(a.entitiesInInconsistentArea.Get(),b.entitiesInInconsistentArea.Get());
			rs.missingEntities = std::max(a.missingEntities.Get(),b.missingEntities.Get());
			rs.overAccountedEntities = std::max(a.overAccountedEntities.Get(),b.overAccountedEntities.Get());
			return rs;
		}
		static TStateDifference Average(const TStateDifference&a, const TStateDifference&b)
		{
			TStateDifference rs;
			rs.omega = Avg(a.omega.Get(),b.omega.Get());
			rs.inconsistency = Avg(a.inconsistency.Get(),b.inconsistency.Get());
			rs.icSize = Avg(a.icSize.Get(),b.icSize.Get());	//might make more sense to compare to minimum here
			rs.entitiesInInconsistentArea = Avg(a.entitiesInInconsistentArea.Get(),b.entitiesInInconsistentArea.Get());
			rs.missingEntities = Avg(a.missingEntities.Get(),b.missingEntities.Get());
			rs.overAccountedEntities = Avg(a.overAccountedEntities.Get(),b.overAccountedEntities.Get());
			return rs;
		}
		static double Avg(double a, double b)
		{
			return (a+b)*0.5;
		}

		void				operator+=(const TStateDifference&other)
		{
			omega += other.omega;
			inconsistency += other.inconsistency;
			icSize += other.icSize;
			entitiesInInconsistentArea += other.entitiesInInconsistentArea;
			missingEntities += other.missingEntities;
			overAccountedEntities += other.overAccountedEntities;
		}

		void				AddMean(const TStateDifference&other)
		{
			omega += other.omega.Get();
			inconsistency += other.inconsistency.Get();
			icSize += other.icSize.Get();
			entitiesInInconsistentArea += other.entitiesInInconsistentArea.Get();
			missingEntities += other.missingEntities.Get();
			overAccountedEntities += other.overAccountedEntities.Get();
		}

		TStateDifference	operator+(const TStateDifference&other) const
		{
			TStateDifference rs;
			rs.omega = omega + other.omega;
			rs.inconsistency = inconsistency + other.inconsistency;
			rs.icSize = icSize + other.icSize;
			rs.entitiesInInconsistentArea = entitiesInInconsistentArea + other.entitiesInInconsistentArea;
			rs.missingEntities = missingEntities + other.missingEntities;
			rs.overAccountedEntities = overAccountedEntities + other.overAccountedEntities;
			return rs;
		}

		void	ToXML(XML::Node&outNode) const
		{
			#undef EXPORT
			#define EXPORT(X)	X.ToXML(outNode.Create(#X));
			EXPORT(omega);
			EXPORT(inconsistency);
			EXPORT(icSize);
			EXPORT(entitiesInInconsistentArea);
			EXPORT(missingEntities);
			EXPORT(overAccountedEntities);
		}

		void	ToXML(XML::Node&outNode, const TStateDifference&min, const TStateDifference&max, const TStateDifference&avg) const
		{
			#undef EXPORT
			#define EXPORT(X)	X.ToXML(outNode.Create(#X),min.X,max.X,avg.X);
			EXPORT(omega);
			EXPORT(inconsistency);
			EXPORT(icSize);
			EXPORT(entitiesInInconsistentArea);
			EXPORT(missingEntities);
			EXPORT(overAccountedEntities);
		}

		void	Import(const XML::Node*node)
		{
			if (!node)
				return;
			#undef IMPORT
			#define IMPORT(X)	X.Import(node->Find(#X));
			IMPORT(omega);
			IMPORT(inconsistency);
			IMPORT(icSize);
			IMPORT(entitiesInInconsistentArea);
			IMPORT(missingEntities);
			IMPORT(overAccountedEntities);
		}

	};



	void	CapturePreMerge(const TStateDifference&preMergeA, const TStateDifference&preMergeB);
	void	CaptureMergeResult(const IC::Comparator&comp, bool exclusive,const TStateDifference&postMerge);
	void	ExportMergeResults();
	void	ImportMergeResults();
}