#ifndef cuda_math_compositeH
#define cuda_math_compositeH

#include "float_composite.h"


__device__	void	cAdd(const TComposite&c0, const TComposite&c1, TComposite&result, float sector_size)
{
	result.offset = c0.offset + c1.offset;
	result.remainder = c0.remainder + c1.remainder;
	
	int offset_change = (int)(result.remainder/sector_size);
	result.offset += offset_change;
	result.remainder -= (float)offset_change*sector_size;
}


__device__	void	cSub(const TComposite&c0, const TComposite&c1, TComposite&result, float sector_size)
{
	result.offset = c0.offset - c1.offset;
	result.remainder = c0.remainder - c1.remainder;
	
	int offset_change = (int)(result.remainder/sector_size);
	result.offset += offset_change;
	result.remainder -= (float)offset_change*sector_size;
}


__device__	float	cSub(const TComposite&c0, const TComposite&c1, float sector_size)
{
	return sector_size*(c0.offset - c1.offset) + c0.remainder-c1.remainder;
}

__device__	void	cSub(const int sector0[3], const int sector1[3], float vector_out[3], float sector_size)
{
	vector_out[0] = (sector0[0]-sector1[0]);
	vector_out[1] = (sector0[1]-sector1[1]);
	vector_out[2] = (sector0[2]-sector1[2]);
	vector_out[0] *= sector_size;
	vector_out[1] *= sector_size;
	vector_out[2] *= sector_size;
}

__device__	void	cSub(const int sector0[3], const int sector1[3], int vector_out[3], int sector_size)
{
	vector_out[0] = (sector0[0]-sector1[0]);
	vector_out[1] = (sector0[1]-sector1[1]);
	vector_out[2] = (sector0[2]-sector1[2]);
	vector_out[0] *= sector_size;
	vector_out[1] *= sector_size;
	vector_out[2] *= sector_size;
}


__device__	void	cAdd(const TComposite&c0, float remainder1, TComposite&result, float sector_size)
{
	result.offset = c0.offset;
	result.remainder = c0.remainder + remainder1;
	
	int offset_change = (int)(result.remainder/sector_size);
	result.offset += offset_change;
	result.remainder -= (float)offset_change*sector_size;
}


__device__	void	cSub(const TComposite&c0, float remainder1, TComposite&result, float sector_size)
{
	result.offset = c0.offset;
	result.remainder = c0.remainder - remainder1;
	
	int offset_change = (int)(result.remainder/sector_size);
	result.offset += offset_change;
	result.remainder -= (float)offset_change*sector_size;
}






#endif
