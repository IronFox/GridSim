#include <global_root.h>
#include "hypothesis.h"
#include "rendering.h"



/*
https://social.msdn.microsoft.com/Forums/vstudio/en-US/784bc42b-3aa9-4754-8774-403f23d789fb/statistical-function-upper-incomplete-gamma-function?forum=vcgeneral
this works
*/
template <typename T> static T upper_incomplete_gamma(const T& s, const T& x) {
	static const T T_0 = T(0), T_1 = T(1), T_2 = T(2), T_3 = T(3);

	T A_prev = T_0;
	T B_prev = T_1;
	T A_cur  = pow(x, s) / exp(x);
	T B_cur  = x - s + T_1;

	T a = s - T_1;
	T b = B_cur + T_2;
	T n = s - T_3;

	for(index_t numIterations = 0; numIterations < 10000; numIterations ++) {
		const T A_next = b * A_cur + a * A_prev;
		const T B_next = b * B_cur + a * B_prev;

		if(A_next * B_cur == A_cur * B_next) {
			return A_cur / B_cur;
		}

		A_prev = A_cur;
		A_cur = A_next;

		B_prev = B_cur;
		B_cur = B_next;

		a = a + n;
		b = b + T_2;
		n = n - T_2;
	}

	return std::numeric_limits<T>::max();
}

#define A 12
double sp_gamma(double z)
{
  const int a = A;
  static double c_space[A];
  static double *c = NULL;
  int k;
  double accm;
 
  if ( c == NULL ) {
    double k1_factrl = 1.0; /* (k - 1)!*(-1)^k with 0!==1*/
    c = c_space;
    c[0] = sqrt(2.0*M_PI);
    for(k=1; k < a; k++) {
      c[k] = exp(a-k) * pow(a-k, k-0.5) / k1_factrl;
	  k1_factrl *= -k;
    }
  }
  accm = c[0];
  for(k=1; k < a; k++) {
    accm += c[k] / ( z + k );
  }
  accm *= exp(-(z+a)) * pow(z+a, z+0.5); /* Gamma(z+1) */
  return accm/z;
}
 


double GammaCDF(double sigma, double mu, float scale, double x)
{
	if (x <= 0)
		return 0;
	
	return (1.0 - upper_incomplete_gamma(sigma,mu*x) / sp_gamma(sigma)) * scale;
}


float		Hypothesis::GetHeightScale(float spatialDistance) const
{
	return Sample(height,spatialDistance);

	//float yOffset = -1.05 + entityDensity * 8.68666 + sqr(entityDensity) * -12.32 + sqr(entityDensity) * entityDensity * 5.65333;
	////55.09999	-200.79996	300.79999	-140.8
	//float xOffset = 55.1 + entityDensity * -200.8 + sqr(entityDensity) * 300.8 + sqr(entityDensity) * entityDensity * -140.8;
	////0.033	0.032	-0.072	0.032
	//float xScale = 0.033 + entityDensity * 0.032 + sqr(entityDensity) * -0.072 + sqr(entityDensity) * entityDensity * 0.032;

	//float heightScale = vmin(1.0, yOffset + sqr((spatialDistance+1 - xOffset) * xScale));
	return 1;
}


float		Hypothesis::Sample(const Array<float>&ref, float spatialDistance) const
{
	if (ref.IsEmpty())
		return 0;
	if (spatialDistance < 0)
		return ref.First();
	index_t lower = (index_t)floor(spatialDistance);
	index_t upper = (index_t)ceil(spatialDistance);
	if (lower >= ref.Count())
		return ref.Last();

	float v0 = ref[lower];
	if (lower == upper || upper >= ref.Count())
	{
		return v0;
	}
	float v1 = ref[upper];
	float a = M::frac(spatialDistance);
	return v0 * (1.f - a) + v1 * a;
}


float		Hypothesis::GetAlpha(float spatialDistance) const
{
	return Sample(sigma,spatialDistance);
}

float		Hypothesis::GetBeta(float spatialDistance) const
{
	return Sample(mu,spatialDistance);
}

double Erf3(double x)
{
	if (x < 0.0)
		return -Erf3(-x);
	double a1 = 0.0705230784, a2 = 0.0422820123, a3 = 0.0092705272, a4 = 0.0001520143, a5 = 0.0002765672, a6 = 0.0000430638;
	double sub = (((((a6 * x + a5) * x + a4) * x + a3) * x + a2) * x + a1) * x + 1.0;
	sub *= sub; //sub2
	sub *= sub; //sub4
	sub *= sub; //sub8
	sub *= sub; //sub16
	return 1.0 - 1.0 / sub;
}


double LogNormalCDF(double x, double sigma, double mu /*= 0.0*/)
{
	if (x < 0.0)
		x = 0.0;
	//x *= 2.0;
	return 0.5 + 0.5 * Erf3((log(x)-mu) / (sqrt(2) * sigma));
}


static double LogNormalDerivative(double x, double sigma, double mu = 0.0)
{
	if (x <= 0.0)
		x = 0.0000000001;

	return	1.0 / (x * sigma * sqrt(2.0 * M_PI))
			* exp(M::sqr(log(x)-mu)/(-2.0 * M::sqr(sigma)) );
}

float		Hypothesis::operator()(index_t iSpatialDistance, index_t iTemporalDistance) const
{
	if (!active)
		return 0.f;
	float x = float(iSpatialDistance);///2;
	//x -= 0.25f;
	float temporalDistance = float(iTemporalDistance)-0.5f;
	float offset = (x-1) + 0.5;
	float heightScale = GetHeightScale(x);
	float sigma = GetAlpha(x);
	float mu = GetBeta(x);
	float scale = 0.2f;

	return LogNormalCDF((double)(temporalDistance - offset) * scale, sigma, mu) * heightScale;
}

float		Hypothesis::GetDerivative(index_t iSpatialDistance, index_t iTemporalDistance) const
{
	if (!active)
		return 0.f;
	float x = float(iSpatialDistance);///2;
	//x -= 0.25f;
	float temporalDistance = float(iTemporalDistance)-0.5f;
	float offset = (x-1) + 0.5;
	float heightScale = GetHeightScale(x);
	float sigma = GetAlpha(x);
	float mu = GetBeta(x);
	float scale = 0.2f;

	return LogNormalDerivative((double)(temporalDistance - offset) * scale, sigma, mu) * heightScale * (scale);
}

float		Hypothesis::GetAccumulatedError(index_t iSpatialDistance, index_t iTemporalDistance) const
{
	if (iSpatialDistance >= spatialInconsistency.GetWidth() || iTemporalDistance >= spatialInconsistency.GetHeight())
		return 0;
	return spatialInconsistency.Get(iSpatialDistance,iTemporalDistance);


	//if (!active)
	//	return 0.f;
	//float s = float(iSpatialDistance);///2;
	////x -= 0.25f;
	//float temporalDistance = float(iTemporalDistance)-0.5f;
	//float offset = (s-1) + 0.5;
	//float heightScale = GetHeightScale(s);
	//float sigma = GetAlpha(s);
	//float mu = GetBeta(s);
	//float scale = 0.2f;



	//float x = (double)(temporalDistance - offset) * scale;
	//x = vmax(x,0.000001);
	//return (sqrt(2.0 / M_PI) * (pow(x,3.0/2.0)-1.0) * exp(-sqr(mu-log(x))/(2.0 * sqr(sigma)))) / (3.0 * sigma * x);
}


void		Hypothesis::Activate()
{
	active = true;


	spatialInconsistency.SetSize(sigma.Count(),128);
	spatialInconsistency.Fill(0);

	foreach (spatialInconsistency.Horizontal(),x)
		foreach (spatialInconsistency.Vertical(),y)
	{
		float d = GetDerivative(*x,*y);
		if (d == 0)
			continue;
		for (index_t k = *y; k < spatialInconsistency.GetHeight(); k++)
			spatialInconsistency.Get(x,k) += d * sqrt(k-*y+1);
	}

}
