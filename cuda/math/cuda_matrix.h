#ifndef cuda_math_matrixH
#define cuda_math_matrixH


#include "cuda_vector.h"

__device__ void dmMult441(const float m[4*4], const float n[4], float out[4])
{
        out[0] = m[0] * n[0] + m[4] * n[1] + m[8]  * n[2] + m[12] * n[3];
        out[1] = m[1] * n[0] + m[5] * n[1] + m[9]  * n[2] + m[13] * n[3];
        out[2] = m[2] * n[0] + m[6] * n[1] + m[10] * n[2] + m[14] * n[3];
        out[3] = m[3] * n[0] + m[7] * n[1] + m[11] * n[2] + m[15] * n[3];
}


__device__ void    dmRotate(const float system[4*4], const float vec[3], float out[3])
{
	out[0] = system[0]*vec[0] + system[4]*vec[1] + system[8] *vec[2];
	out[1] = system[1]*vec[0] + system[5]*vec[1] + system[9] *vec[2];
	out[2] = system[2]*vec[0] + system[6]*vec[1] + system[10]*vec[2];
}

__device__ void    dmTransform(const float system[4*4], const float point[3], float out[3])
{
	out[0] = system[0]*point[0] + system[4]*point[1] + system[8] *point[2] + system[12];
	out[1] = system[1]*point[0] + system[5]*point[1] + system[9] *point[2] + system[13];
	out[2] = system[2]*point[0] + system[6]*point[1] + system[10]*point[2] + system[14];
}


#endif
