#pragma once
#include <container/array.h>

using namespace DeltaWorks;

struct TExperiment
{
	int		reliabilityLevel=4,
			recoveryIterations=2,
			numLayers=1,
			maxSiblingSyncOperations=1,
			siblingSyncDelay=0,
			numEntities = 0;

	float	GetReliability() const;
	bool	operator==(const TExperiment&other) const
	{
		return	reliabilityLevel == other.reliabilityLevel
			&&	recoveryIterations == other.recoveryIterations 
			&&	numLayers == other.numLayers
			&&	maxSiblingSyncOperations == other.maxSiblingSyncOperations
			&&	siblingSyncDelay == other.siblingSyncDelay
			&&	numEntities == other.numEntities
			;
	}

	bool	operator!=(const TExperiment&other) const {return !operator==(other);}


	void	Describe(Array<std::pair<const char*,int*> >&);
	void	Describe(Array<std::pair<const char*,int> >&) const;

};
