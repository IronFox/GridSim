#ifndef cuda_math_float_compositeH
#define cuda_math_float_compositeH


/**
	@brief Sector based composite floating point value
	
	The composite structure keeps track of the number of integral sector sizes and the floating point remainder.
	As such the absolute value is offset*sector_size+remainder but such calculation is highly discouraged.
*/
struct TComposite
{
	int		offset;
	float	remainder;
};


namespace Composite
{
	static const	TComposite	zero={0,0},
								null={0,0},
								one={0,1};

	inline double		toDouble(const TComposite&c, float sector_size)
	{
		double rs = c.offset;
		rs *= sector_size;
		rs += c.remainder;
		return rs;
	}
	
	inline TComposite	fromDouble(double value, float sector_size)
	{
		TComposite result;
		result.offset = (int)(value/sector_size);
		result.remainder = value-sector_size*result.offset;
		return result;
	}
								
	inline	TComposite	build(int offset, float remainder)
	{
		TComposite result = {offset,remainder};
		return result;
	}

	inline	void	add(TComposite&c0, const TComposite&c1, float sector_size)
	{
		c0.offset += c1.offset;
		c0.remainder += c1.remainder;
		
		int offset_change = (int)(c0.remainder/sector_size);
		c0.offset += offset_change;
		c0.remainder -= (float)offset_change*sector_size;
	}
	
	inline	void	add(TComposite&c0, float remainder1, float sector_size)
	{
		c0.remainder += remainder1;
		
		int offset_change = (int)(c0.remainder/sector_size);
		c0.offset += offset_change;
		c0.remainder -= (float)offset_change*sector_size;
	}
	
	
	inline	void	add(const TComposite&c0, const TComposite&c1, TComposite&result, float sector_size)
	{
		result.offset = c0.offset + c1.offset;
		result.remainder = c0.remainder + c1.remainder;
		
		int offset_change = (int)(result.remainder/sector_size);
		result.offset += offset_change;
		result.remainder -= (float)offset_change*sector_size;
	}


	inline	void	sub(const TComposite&c0, const TComposite&c1, TComposite&result, float sector_size)
	{
		result.offset = c0.offset - c1.offset;
		result.remainder = c0.remainder - c1.remainder;
		
		int offset_change = (int)(result.remainder/sector_size);
		result.offset += offset_change;
		result.remainder -= (float)offset_change*sector_size;
	}

	inline	void	sub(TComposite&c0, float remainder1, float sector_size)
	{
		c0.remainder -= remainder1;
		
		int offset_change = (int)(c0.remainder/sector_size);
		c0.offset += offset_change;
		c0.remainder -= (float)offset_change*sector_size;
	}
	
	inline	float	sub(const TComposite&c0, const TComposite&c1, float sector_size)
	{
		return sector_size*(c0.offset - c1.offset) + c0.remainder-c1.remainder;
	}

	inline	void	sub(const int sector0[3], const int sector1[3], float vector_out[3], float sector_size)
	{
		vector_out[0] = (sector0[0]-sector1[0]);
		vector_out[1] = (sector0[1]-sector1[1]);
		vector_out[2] = (sector0[2]-sector1[2]);
		vector_out[0] *= sector_size;
		vector_out[1] *= sector_size;
		vector_out[2] *= sector_size;
	}


	inline	void	add(const TComposite&c0, float remainder1, TComposite&result, float sector_size)
	{
		result.offset = c0.offset;
		result.remainder = c0.remainder + remainder1;
		
		int offset_change = (int)(result.remainder/sector_size);
		result.offset += offset_change;
		result.remainder -= (float)offset_change*sector_size;
	}


	inline	void	sub(const TComposite&c0, float remainder1, TComposite&result, float sector_size)
	{
		result.offset = c0.offset;
		result.remainder = c0.remainder - remainder1;
		
		int offset_change = (int)(result.remainder/sector_size);
		result.offset += offset_change;
		result.remainder -= (float)offset_change*sector_size;
	}
}






#endif
