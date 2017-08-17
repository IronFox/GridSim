#pragma once
#include <container/buffer.h>
#include <container/array2d.h>

using namespace DeltaWorks;

class Hypothesis
{
	bool		active = false;

	Array2D<float>	spatialInconsistency;
public:
	Array<float>sigma,
				mu,
				height;

	float		operator()(index_t spatialDistance, index_t temporalDistance) const;
	float		GetDerivative(index_t spatialDistance, index_t temporalDistance) const;
	float		GetAccumulatedError(index_t spatialDistance, index_t temporalDistance) const;

	void		Deactivate() {active = false;}
	void		Activate();

	float		GetHeightScale(float spatialDistance) const;
	float		GetAlpha(float spatialDistance) const;
	float		GetBeta(float spatialDistance) const;


	float		Sample(const Array<float>&ref, float spatialDistance) const;
};

double Erf3(double x);
double LogNormalCDF(double x, double sigma, double mu = 0.0);
