#include "experiment.h"

float	TExperiment::GetReliability() const
{
	return 1.f - pow(10.f, -float(reliabilityLevel) / 8);
}


void	TExperiment::Describe(Array<std::pair<const char*,int*> >&rs)
{
	rs.SetSize(6);
	index_t at=0;
	rs[at++] = std::make_pair("reliabilityLevel",&reliabilityLevel);
	rs[at++] = std::make_pair("recoveryIterations",&recoveryIterations);
	rs[at++] = std::make_pair("numLayers",&numLayers);
	rs[at++] = std::make_pair("maxSiblingSyncOperations",&maxSiblingSyncOperations);
	rs[at++] = std::make_pair("siblingSyncDelay",&siblingSyncDelay);
	rs[at++] = std::make_pair("numEntities",&numEntities);
}

void	TExperiment::Describe(Array<std::pair<const char*,int> >&rs) const
{
	rs.SetSize(6);
	index_t at=0;
	rs[at++] = std::make_pair("reliabilityLevel",reliabilityLevel);
	rs[at++] = std::make_pair("recoveryIterations",recoveryIterations);
	rs[at++] = std::make_pair("numLayers",numLayers);
	rs[at++] = std::make_pair("maxSiblingSyncOperations",maxSiblingSyncOperations);
	rs[at++] = std::make_pair("siblingSyncDelay",siblingSyncDelay);
	rs[at++] = std::make_pair("numEntities",numEntities);
}
