#include "BitArray.h"
#include <math/basic.h>


void			BitArray::Hash(Hasher&hasher) const
{
	hasher.AppendPOD(storage.Count());
	hasher.Append(storage.pointer(),storage.GetContentSize());
}


bool			BitArray::AllZero() const
{
	foreach (storage,s)
		if (*s)
			return false;
	return true;
}


	
void			BitArray::operator&=(const BitArray&other)
{
	//if empty: stay empty
	auto it0 = storage.begin();
	auto it1 = other.storage.begin();
	const auto end0 = storage.end();
	const auto end1 = other.storage.end();

	for (; it0 != end0 && it1 != end1; ++it0, ++it1)
	{
		*it0 &= *it1;
	}
}

void			BitArray::operator|=(const BitArray&other)
{
	if (storage.IsEmpty())
	{
		//copy other:
		storage = other.storage;
		return;
	}
	if (other.storage.Count() > storage.Count())
		storage.ResizePreserveContent(other.storage.Count(),0);
	auto it0 = storage.begin();
	auto it1 = other.storage.begin();
	const auto end1 = other.storage.end();

	for (; it1 != end1; ++it0, ++it1)
	{
		*it0 |= *it1;
	}
}

void			BitArray::operator^=(const BitArray&other)
{
	if (storage.IsEmpty())
	{
		storage = other.storage;
		return;
	}
	if (other.storage.Count() > storage.Count())
		storage.ResizePreserveContent(other.storage.Count(),0);
	auto it0 = storage.begin();
	auto it1 = other.storage.begin();
	const auto end1 = other.storage.end();

	for (; it1 != end1; ++it0, ++it1)
	{
		*it0 ^= *it1;
	}
}
	
bool			BitArray::operator[](index_t at) const
{
	const index_t element = at / (sizeof(size_t)*8);
	const index_t bit = at % (sizeof(size_t)*8);

	const size_t value = storage[element];
	return ((value >> bit) & 1) != 0;
}

void			BitArray::SetBit(index_t at, bool newBit)
{
	const index_t element = at / (sizeof(size_t)*8);
	const index_t bit = at % (sizeof(size_t)*8);

	size_t&value = storage[element];
	const size_t bitMask = (size_t(1) << bit);
	if (newBit)
	{
		value |= bitMask;
	}
	else
	{
		value &= ~bitMask;
	}
}


/*static*/ count_t	BitArray::TranslateBitSize(count_t numBits)
{
	return DeltaWorks::Math::CeilDiv(numBits,sizeof(size_t)*8);
}
