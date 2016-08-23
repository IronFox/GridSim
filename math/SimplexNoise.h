#pragma once

#undef INLINE
#include "../third_party/open-simplex-noise/open-simplex-noise.h"

#include "basic.h"

/**
Wrapper for Open Simplex Noise.
https://github.com/smcameron/open-simplex-noise-in-c
*/
class SimplexNoise
{
	osn_context*	ctx = nullptr;
	
public:
	/**/		SimplexNoise(int64_t seed)
	{
		open_simplex_noise(seed, &ctx);
	}
	
	/**/		~SimplexNoise()
	{
		open_simplex_noise_free(ctx);
	}
	
	/**
	Samples semi-gaussian volumetric noise in the range [-1,+1]
	*/
	double		Get(double x, double y, double z)	const
	{
		return open_simplex_noise3(ctx, x, y, z);
	}

	/**
	Samples (close to) uniformly distributed volumetric noise in the range [0,1]
	*/
	double		GetContinuous(double x, double y, double z) const
	{
		return ToContinuousDistribution(Get(x,y,z));
	}

	static double	ToContinuousDistribution(double x)
	{
		double x2 = x*x;
		double x3 = x2*x;
		double x4 = x3*x;
		double x5 = x4*x;
		return vmax(0.0,0.499877 + 1.12521 * x + 0.00298947 * x2 - 1.0534 *x3 - 0.00365494 *x4 + 0.431418 *x5);
	}
};
