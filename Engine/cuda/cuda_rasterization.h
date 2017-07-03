#ifndef cuda_rasterizationH
#define cuda_rasterizationH

#include "../math/vector.h"



struct TCudaVertex
{
	union
	{
		struct
		{
			float			x,
							y,
							depth;
		};
		float				v[2];
	};
};


struct TCudaTriangle
{
	union
	{
		struct
		{
			TCudaVertex		v0,
							v1,
							v2;
		};
		TCudaVertex			v[3];
	};
	
	float					barycentric_divisor[3];
};

struct TCudaQuadPreparation
{
	float					n0[2],
							nd[2],
							ca,
							cb,
							cc;


};

struct TCudaQuad
{
	union
	{
		struct
		{
			TCudaVertex		v0,
							v1,
							v2,
							v3;
		};
		TCudaVertex			v[4];
	};
	
	TCudaQuadPreparation	x,
							y;
};



struct TCudaFaceSector
{
	unsigned				min_x,
							max_x,
							min_y,
							max_y,
							face_index;	//item to rasterize
};

namespace Rasterization
{

	template <unsigned v0, unsigned v1>
		inline	float	barycentricFactor(const TCudaVertex*v, /*BYTE v0, BYTE v1, */float x, float y)
		{
			return (v[v0].y - v[v1].y) * x + (v[v1].x - v[v0].x) * y + v[v0].x * v[v1].y - v[v1].x * v[v0].y;
		}

	inline	float	barycentricFactor01(const TCudaTriangle&t, float x, float y)
	{
		return (t.v0.y - t.v1.y) * x + (t.v1.x - t.v0.x) * y + t.v0.x * t.v1.y - t.v1.x * t.v0.y;
	}

	inline	float	barycentricFactor12(const TCudaTriangle&t, float x, float y)
	{
		return (t.v1.y - t.v2.y) * x + (t.v2.x - t.v1.x) * y + t.v1.x * t.v2.y - t.v2.x * t.v1.y;
	}

	inline	float	barycentricFactor20(const TCudaTriangle&t, float x, float y)
	{
		return (t.v2.y - t.v0.y) * x + (t.v0.x - t.v2.x) * y + t.v2.x * t.v0.y - t.v0.x * t.v2.y;
	}

}

#ifdef __CUDACC__

	__device__	float	drBarycentricFactor(const TCudaTriangle&t, BYTE v0, BYTE v1, float x, float y)
	{
		return (t.v[v0].y - t.v[v1].y) * x + (t.v[v1].x - t.v[v0].x) * y + t.v[v0].x * t.v[v1].y - t.v[v1].x * t.v[v0].y;
	}

	
	__device__	float	drBarycentricFactor01(const TCudaTriangle&t, float x, float y)
	{
		return (t.v0.y - t.v1.y) * x + (t.v1.x - t.v0.x) * y + t.v0.x * t.v1.y - t.v1.x * t.v0.y;
	}

	__device__	float	drBarycentricFactor12(const TCudaTriangle&t, float x, float y)
	{
		return (t.v1.y - t.v2.y) * x + (t.v2.x - t.v1.x) * y + t.v1.x * t.v2.y - t.v2.x * t.v1.y;
	}

	__device__	float	drBarycentricFactor20(const TCudaTriangle&t, float x, float y)
	{
		return (t.v2.y - t.v0.y) * x + (t.v0.x - t.v2.x) * y + t.v2.x * t.v0.y - t.v0.x * t.v2.y;
	}

#endif

namespace Rasterization
{

	inline void		prepare(TCudaTriangle&t)
	{
		t.barycentric_divisor[0] = barycentricFactor12(t,t.v0.x,t.v0.y);
		t.barycentric_divisor[1] = barycentricFactor20(t,t.v1.x,t.v1.y);
		t.barycentric_divisor[2] = barycentricFactor01(t,t.v2.x,t.v2.y);
	}

	inline float	d(const TCudaVertex&v0, const TCudaVertex&v1)
	{
		return v0.x*v1.y - v0.y*v1.x;
	}

	inline void		prepare(TCudaQuad&t)
	{
		{
			float	n1[2] = {-(t.v2.y-t.v1.y),(t.v2.x-t.v1.x)},
					bd[2] = {t.v1.x - t.v0.x,t.v1.y-t.v0.y};
			t.x.n0[0] = -(t.v3.y-t.v0.y);
			t.x.n0[1] = (t.v3.x-t.v0.x);
			t.x.nd[0] = n1[0]-t.x.n0[0];
			t.x.nd[1] = n1[1]-t.x.n0[1];
			t.x.ca = _dot2(bd,t.x.nd);
			t.x.cb = - _dot2(t.x.n0,bd) - _dot2(t.x.nd,t.v0.v);
		}
		
		{
			float	n1[2] = {-(t.v2.y-t.v3.y),(t.v2.x-t.v3.x)},
					bd[2] = {t.v3.x - t.v0.x,t.v3.y-t.v0.y};
			t.y.n0[0] = -(t.v1.y-t.v0.y);
			t.y.n0[1] = (t.v1.x-t.v0.x);
			t.y.nd[0] = n1[0]-t.y.n0[0];
			t.y.nd[1] = n1[1]-t.y.n0[1];
			t.y.ca = _dot2(bd,t.y.nd);
			t.y.cb = - _dot2(t.y.n0,bd) - _dot2(t.y.nd,t.v0.v);
		}
	}

	inline float	barycentricAlpha(const TCudaTriangle&t, float x, float y)
	{
		return barycentricFactor12(t,x,y)/t.barycentric_divisor[0];
	}

	inline float	barycentricBeta(const TCudaTriangle&t, float x, float y)
	{
		return barycentricFactor20(t,x,y)/t.barycentric_divisor[1];
	}

	inline float	barycentricGamma(const TCudaTriangle&t, float x, float y)
	{
		return barycentricFactor01(t,x,y)/t.barycentric_divisor[2];
	}
	
	
}

#ifdef __CUDACC__


	__device__ float drBarycentricAlpha(const TCudaTriangle&t, float x, float y)
	{
		return drBarycentricFactor12(t,x,y)/t.barycentric_divisor[0];
	}

	__device__ float drBarycentricBeta(const TCudaTriangle&t, float x, float y)
	{
		return drBarycentricFactor20(t,x,y)/t.barycentric_divisor[1];
	}

	__device__ float drBarycentricGamma(const TCudaTriangle&t, float x, float y)
	{
		return drBarycentricFactor01(t,x,y)/t.barycentric_divisor[2];
	}

#endif

namespace Rasterization
{
	inline bool inTriangle(float alpha, float beta, float gamma)
	{
		return alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1;
	}
	
	inline bool inQuad(float alpha, float beta, float gamma, float delta)
	{
		return alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1 && delta >= 0 && delta <= 1;
	}
}

#ifdef __CUDACC__

	__device__ bool drInTriangle(float alpha, float beta, float gamma)
	{
		return alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1;
	}
#endif


namespace Rasterization
{
	inline bool	interpolate(const TCudaTriangle&t, float x, float y, float&result)
	{
		float 	alpha = barycentricAlpha(t,x,y),
				beta = barycentricBeta(t,x,y),
				gamma = barycentricGamma(t,x,y);
		if (!inTriangle(alpha,beta,gamma))
			return false;
		/*result.x = alpha*t.v0.x + beta*t.v1.x + gamma*t.v2.x;
		result.y = alpha*t.v0.y + beta*t.v1.y + gamma*t.v2.y;*/
		result = alpha*t.v0.depth + beta*t.v1.depth + gamma*t.v2.depth;
		return true;
	}
	
	inline bool	interpolate(const TCudaQuad&t, float x, float y, TCudaVertex&result)
	{
		float	
				p[2] = {x,y},
				rs[2];
		BYTE num_rs = solveSqrEquation(t.x.ca,t.x.cb + _dot2(t.x.nd,p),_dot2(t.x.n0,p)-t.x.cc,rs);
		if (!num_rs)
			return false;
		float fx = smallestPositiveResult(rs,num_rs);
		if (fx < 0 || fx > 1)
			return false;
		
		num_rs = solveSqrEquation(t.y.ca,t.y.cb + _dot2(t.y.nd,p),_dot2(t.y.n0,p)-t.y.cc,rs);
		if (!num_rs)
			return false;
		float fy = smallestPositiveResult(rs,num_rs);
		if (fy < 0 || fy > 1)
			return false;
	
		float 	alpha = (1-fx)*(1-fy),
				beta = fx*(1-fy),
				gamma = fx*fy,
				delta = (1-fx)*fy;
				
		result.x = alpha*t.v0.x + beta*t.v1.x + gamma*t.v2.x + delta*t.v3.x;
		result.y = alpha*t.v0.y + beta*t.v1.y + gamma*t.v2.y + delta*t.v3.y;
		result.depth = alpha*t.v0.depth + beta*t.v1.depth + gamma*t.v2.depth + delta*t.v3.depth;
		return true;
	}
}

#ifdef __CUDACC__

	__device__ bool	drInterpolate(const TCudaTriangle&t, float x, float y, float&result)
	{
		float 	alpha = drBarycentricAlpha(t,x,y),
				beta = drBarycentricBeta(t,x,y),
				gamma = drBarycentricGamma(t,x,y);
		if (!drInTriangle(alpha,beta,gamma))
			return false;
		/*result.x = alpha*t.v0.x + beta*t.v1.x + gamma*t.v2.x;
		result.y = alpha*t.v0.y + beta*t.v1.y + gamma*t.v2.y;*/
		result = alpha*t.v0.depth + beta*t.v1.depth + gamma*t.v2.depth;
		return true;
	}

#endif

#endif
