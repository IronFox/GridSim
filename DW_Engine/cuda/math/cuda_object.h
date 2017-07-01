#ifndef cuda_math_objectH
#define cuda_math_objectH

#include "cuda_vector.h"

__device__ void			doCalculateTriangleNormal(const float p0[3], const float p1[3], const float p2[3], float out[3])
{
	float	v[3]={p1[0]-p0[0],p1[1]-p0[1],p1[2]-p0[2]},
			w[3]={p2[0]-p0[0],p2[1]-p0[1],p2[2]-p0[2]};

	out[0] = v[1]*w[2] - v[2]*w[1];
	out[1] = v[2]*w[0] - v[0]*w[2];
	out[2] = v[0]*w[1] - v[1]*w[0];

}

__device__ void			doAddTriangleNormal(const float p0[3], const float p1[3], const float p2[3], float inout[3])
{
	//dvAdd(inout,p0);
	//return;
	float	v[3]={p1[0]-p0[0],p1[1]-p0[1],p1[2]-p0[2]},
			w[3]={p2[0]-p0[0],p2[1]-p0[1],p2[2]-p0[2]};

	inout[0] += v[1]*w[2] - v[2]*w[1];
	inout[1] += v[2]*w[0] - v[0]*w[2];
	inout[2] += v[0]*w[1] - v[1]*w[0];
}



__device__ void			doAddTriangleNormal(const int p0[3], const int p1[3], const int p2[3], float inout[3])
{
	//dvAdd(inout,p0);
	//return;
	float	v[3]={p1[0]-p0[0],p1[1]-p0[1],p1[2]-p0[2]},
			w[3]={p2[0]-p0[0],p2[1]-p0[1],p2[2]-p0[2]};

	inout[0] += v[1]*w[2] - v[2]*w[1];
	inout[1] += v[2]*w[0] - v[0]*w[2];
	inout[2] += v[0]*w[1] - v[1]*w[0];
}




__device__ bool		doDetectOpticalSphereIntersection(const float center[3], float radius, bool positive, const float b[3], const float d[3], float point[3], float&distance)
{
	float	delta[3];
	dvSub(b,center,delta);

	float	pa = 1,
			pb = 2*dvDot(d,delta),
			pc = dvDot(delta)-radius*radius,
			rs[2];

	unsigned char num_rs = dbSolveSqrEquation(pa,pb,pc,rs);
	if (!num_rs)
		return false;


	float dist = rs[0];
	if (!positive && num_rs == 2)
		dist = rs[1];
	//if (num_rs > 1 && dist > rs[1])
		//dist = rs[1];

	if (dist < 0 || dist > distance)
		return false;
	distance = dist;
	dvMad(b,d,distance,point);

	return true;
}

__device__ bool		doDetectOpticalSphereIntersection2(const float center[3], float radius, const float b[3], const float d[3])
{
	float	delta[3];
	dvSub(b,center,delta);

	float	pa = 1,
			pb = 2*dvDot(d,delta),
			pc = dvDot(delta)-radius*radius,
			rs[2];

	unsigned char num_rs = dbSolveSqrEquation(pa,pb,pc,rs);
	if (!num_rs)
		return false;


	float dist = rs[0];
	//if (num_rs > 1 && dist > rs[1])
		//dist = rs[1];

	if (dist < 0)
		return false;
	return true;
}




#endif
