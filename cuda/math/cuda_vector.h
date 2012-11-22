#ifndef cuda_math_vectorH
#define cuda_math_vectorH

#include "cuda_basic.h"




__device__ float dvLength(const float v[3])
{
	return sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

__device__ float dvLength(const int v[3])
{
	return sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}



__device__	void	dvCenter(const float v0[3], const float v1[3], const float v2[3], float out[3])
{
	out[0] = __fdividef(v0[0]+v1[0]+v2[0],3.0f);
	out[1] = __fdividef(v0[1]+v1[1]+v2[1],3.0f);
	out[2] = __fdividef(v0[2]+v1[2]+v2[2],3.0f);
}

__device__	void	dvCenter(const float v0[3], const float v1[3], float out[3])
{
	out[0] = __fdividef(v0[0]+v1[0],2.0f);
	out[1] = __fdividef(v0[1]+v1[1],2.0f);
	out[2] = __fdividef(v0[2]+v1[2],2.0f);
}

__device__	void	dvCenter(const int v0[3], const int v1[3], int out[3])
{
	out[0] = (v0[0]+v1[0])>>1;
	out[1] = (v0[1]+v1[1])>>1;
	out[2] = (v0[2]+v1[2])>>1;
}


__device__	void	dvAdd(const float v0[3], const float v1[3], float out[3])
{
	out[0] = v0[0]+v1[0];
	out[1] = v0[1]+v1[1];
	out[2] = v0[2]+v1[2];
}

__device__	void	dvAdd(const int v0[3], const int v1[3], int out[3])
{
	out[0] = v0[0]+v1[0];
	out[1] = v0[1]+v1[1];
	out[2] = v0[2]+v1[2];
}

__device__	void	dvAdd(float v0[3], const float v1[3])
{
	v0[0]+=v1[0];
	v0[1]+=v1[1];
	v0[2]+=v1[2];
}

__device__	void	dvAdd(int v0[3], const int v1[3])
{
	v0[0]+=v1[0];
	v0[1]+=v1[1];
	v0[2]+=v1[2];
}



/**
	@brief Multiply and add operation
	
	Calculates @a b += @a d * @a scalar
*/
__device__ void	dvMad(float b[3], const float d[3], float scalar)
{
	b[0] += d[0] * scalar;
	b[1] += d[1] * scalar;
	b[2] += d[2] * scalar;
}

/**
	@brief Multiply and add operation
	
	Calculates @a out = @a b + @a d * @a scalar
*/
__device__ void	dvMad(const float b[3], const float d[3], float scalar, float out[3])
{
	out[0] = b[0] + d[0] * scalar;
	out[1] = b[1] + d[1] * scalar;
	out[2] = b[2] + d[2] * scalar;
}

/**
	@brief Multiply and add operation
	
	Calculates @a out = @a b + @a d * @a scalar
*/
__device__ void	dvMad(const float b[3], const int d[3], float scalar, float out[3])
{
	out[0] = b[0] + (float)d[0] * scalar;
	out[1] = b[1] + (float)d[1] * scalar;
	out[2] = b[2] + (float)d[2] * scalar;
}

/**
	@brief Multiply and add operation
	
	Calculates @a out = @a b + @a d * @a scalar
*/
__device__ void	dvMad(const int b[3], const int d[3], int scalar, float out[3])
{
	out[0] = b[0] + (float)d[0] * scalar;
	out[1] = b[1] + (float)d[1] * scalar;
	out[2] = b[2] + (float)d[2] * scalar;
}

/**
	@brief Divides a vector by a value
	
	Calculates @a out = @a v / @a value
*/
__device__ void dvDiv(const float v[3], float value, float out[3])
{
	out[0] = __fdividef(v[0],value);
	out[1] = __fdividef(v[1],value);
	out[2] = __fdividef(v[2],value);
}

__device__ void dvDiv(float v[3], float value)
{
	v[0] = __fdividef(v[0],value);
	v[1] = __fdividef(v[1],value);
	v[2] = __fdividef(v[2],value);
}

__device__ void dvC3(const unsigned char v[3], unsigned char w[3])
{
	w[0] = v[0];
	w[1] = v[1];
	w[2] = v[2];
}


__device__ void dvC3(const float v[3], int w[3])
{
	w[0] = (int)v[0];
	w[1] = (int)v[1];
	w[2] = (int)v[2];
}

__device__ void dvC3(const float v[3], float w[3])
{
	w[0] = v[0];
	w[1] = v[1];
	w[2] = v[2];
}

__device__ void dvC3(const int v[3], float w[3])
{
	w[0] = v[0];
	w[1] = v[1];
	w[2] = v[2];
}

__device__ void dvC3(const float v[3], float3&w)
{
	w.x = v[0];
	w.y = v[1];
	w.z = v[2];
}

__device__ void dvC3(const float3& v, float w[3])
{
	w[0] = v.x;
	w[1] = v.y;
	w[2] = v.z;
}

__device__ void dvC4(const unsigned char v[4], unsigned char w[4])
{
	w[0] = v[0];
	w[1] = v[1];
	w[2] = v[2];
	w[3] = v[3];
}


__device__ void dvC4(const float v[4], int w[4])
{
	w[0] = (int)v[0];
	w[1] = (int)v[1];
	w[2] = (int)v[2];
	w[3] = (int)v[3];
}

__device__ void dvC4(const float v[4], float w[4])
{
	w[0] = v[0];
	w[1] = v[1];
	w[2] = v[2];
	w[3] = v[3];
}

__device__ void dvC4(const int v[4], float w[4])
{
	w[0] = v[0];
	w[1] = v[1];
	w[2] = v[2];
	w[3] = v[3];
}

__device__ void dvC4(const float v[4], float4&w)
{
	w.x = v[0];
	w.y = v[1];
	w.z = v[2];
	w.w = v[3];
}

__device__ void dvC4(const float4& v, float w[4])
{
	w[0] = v.x;
	w[1] = v.y;
	w[2] = v.z;
	w[3] = v.w;
}


__device__ void dvNormalize(float v[3])
{
		float len = sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
		v[0]=__fdividef(v[0],len);
		v[1]=__fdividef(v[1],len);
		v[2]=__fdividef(v[2],len);
}

__device__ void dvNormalize(float3& v)
{
		float len = sqrtf(v.x*v.x+v.y*v.y+v.z*v.z);
		v.x=__fdividef(v.x,len);
		v.y=__fdividef(v.y,len);
		v.z=__fdividef(v.z,len);
}



__device__ void dvSub(const float v[3], const float w[3], float out[3])
{
		out[0] = v[0] - w[0];
		out[1] = v[1] - w[1];
		out[2] = v[2] - w[2];
}

__device__ void dvSub(const int v[3], const int w[3], float out[3])
{
		out[0] = v[0] - w[0];
		out[1] = v[1] - w[1];
		out[2] = v[2] - w[2];
}

__device__ void dvSub(const int v[3], const int w[3], int out[3])
{
		out[0] = v[0] - w[0];
		out[1] = v[1] - w[1];
		out[2] = v[2] - w[2];
}


__device__ void dvSub(float v[3], const float w[3])
{
		v[0] -= w[0];
		v[1] -= w[1];
		v[2] -= w[2];
}

__device__ void dvSub(float v[3], const int w[3])
{
		v[0] -= w[0];
		v[1] -= w[1];
		v[2] -= w[2];
}

__device__ void dvSub(int v[3], const int w[3])
{
		v[0] -= w[0];
		v[1] -= w[1];
		v[2] -= w[2];
}

__device__ void	dvMult(const int v[3], float factor, float out[3])
{
	out[0] = factor*v[0];
	out[1] = factor*v[1];
	out[2] = factor*v[2];
}


__device__ void	dvMult(int v[3], int factor)
{
	v[0] *= factor;
	v[1] *= factor;
	v[2] *= factor;
}

__device__ void	dvMult(float v[3], float factor)
{
	v[0] *= factor;
	v[1] *= factor;
	v[2] *= factor;
}

__device__ void dvMult(const float v[3],float factor,float out[3])
{
	out[0] = factor*v[0];
	out[1] = factor*v[1];
	out[2] = factor*v[2];
}


__device__	UINT64	dvDot(const UINT64 v[3])
{
	return (UINT64)v[0]*v[0] + (UINT64)v[1]*v[1] + (UINT64)v[2]*v[2];
}

__device__	INT64	dvDot(const INT64 v[3])
{
	return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

__device__	UINT64	dvDot(const UINT32 v[3])
{
	return (UINT64)v[0]*v[0] + (UINT64)v[1]*v[1] + (UINT64)v[2]*v[2];
}


__device__	float	dvDot(const int v[3])
{
	return (float)v[0]*v[0] + (float)v[1]*v[1] + (float)v[2]*v[2];
}

__device__	float	dvDot(const float v[3])
{
	return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

__device__ float	dvDot(const float v[3], const float w[3])
{
	return v[0]*w[0]+v[1]*w[1]+v[2]*w[2];
}


__device__ float	dvDistance(const float v[3],const float w[3])
{
	return sqrtf(dbSqr(v[0]-w[0])+dbSqr(v[1]-w[1])+dbSqr(v[2]-w[2]));
}

__device__ float	dvDistance(const int v[3],const int w[3])
{
	return sqrtf(dbSqr((float)(v[0]-w[0]))+dbSqr((float)(v[1]-w[1]))+dbSqr((float)(v[2]-w[2])));
}

__device__ float	dvDistanceSquare(const float v[3],const float w[3])
{
	return (dbSqr(v[0]-w[0])+dbSqr(v[1]-w[1])+dbSqr(v[2]-w[2]));
}

__device__ void		dvClear(float v[3])
{
	v[0] = 0;
	v[1] = 0;
	v[2] = 0;
}

__device__ float	dvDistance(const float v0[3], const float v1[3], const float p[3])
	{
		float	d[3],r[3];
		dvSub(v1,v0,d);
		float alpha = __fdividef(dvDot(p,d)-dvDot(v0,d),dvDot(d));
		dvMad(v0,d,alpha,r);
		return dvDistance(r,p);
	}

__device__ void		dvReflectN(const float base[3], const float normal[3], const float p[3], float out[3])
{
	dvMad(p,normal,2*(dvDot(base,normal)-dvDot(p,normal)),out);
}

__device__ void		dvRefractN(float normal[3], const float vec[3], float eta, float out[3])
{
	float ni = dvDot(normal,vec);
	if (ni > 0)
	{
		dvC3(vec,out);
		return;
	}


	float k = fabsf(1.0 - eta * eta * (1.0 - dbSqr(ni)));
	{
		float sk = sqrtf(k);
		out[0] = eta * vec[0] - (eta*ni + sk)*normal[0];
		out[1] = eta * vec[1] - (eta*ni + sk)*normal[1];
		out[2] = eta * vec[2] - (eta*ni + sk)*normal[2];
	}
}



__device__ void	dvCross(const float v[3], const float w[3], float out[3])
{
	out[0] = v[1]*w[2] - v[2]*w[1];
	out[1] = v[2]*w[0] - v[0]*w[2];
	out[2] = v[0]*w[1] - v[1]*w[0];
}


__device__ float		dvDistance(const float v0[3], const float d0[3], const float w0[3], const float d1[3])
{
	float	n[3],dif[3];
	dvSub(w0,v0,dif);
	dvCross(d0,d1,n);
	dvNormalize(n);
	return fabsf(dvDot(n,dif));
}

__device__ float		dvDistance(const float v0[3], const float d0[3], const float w0[3], const float d1[3], float n[3])
{
	float	dif[3];
	dvSub(w0,v0,dif);
	dvCross(d0,d1,n);
	dvNormalize(n);
	return fabsf(dvDot(n,dif));
}


#endif
