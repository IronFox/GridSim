#pragma once
#include <container/array.h>
#include "types.h"

class BitArray
{
public:
	/**/			BitArray(count_t minBits=0)
	{
		storage.SetSize(TranslateBitSize(minBits));
		storage.Fill(0);
	}
	void			SetSize(count_t minBits=0)
	{
		storage.SetSize(TranslateBitSize(minBits));
		storage.Fill(0);
	}
	
	void			SetAllZero()
	{
		storage.Fill(0);
	}
	void			SetAllOne()
	{
		storage.Fill(std::numeric_limits<size_t>::max());
	}
	bool			AllZero() const;
	
	void			operator&=(const BitArray&other);
	void			operator|=(const BitArray&other);
	void			operator^=(const BitArray&other);
	
	bool			operator[](index_t) const;
	void			SetBit(index_t, bool);
	void			Hash(Hasher&hasher) const;
	
private:
	DeltaWorks::Array<size_t>	storage;
	
	static count_t	TranslateBitSize(count_t numBits);
};
