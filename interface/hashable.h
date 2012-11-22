#ifndef hashableH
#define hashableH

#include "common.h"


typedef size_t		hash_t;


/**
	@brief Computes a binary hash of the specified memory section

	@param memory Memory to calculate the hash value of. May be NULL if size is 0
	@param size Memory size in bytes

	@return Hash value
*/
inline hash_t stdMemHash(const void*memory, size_t size)
{
	const char*ident = (const char*)memory;
	hash_t	hash = 0;
	for (size_t i = 0; i < size; i++)
	{
		hash += ident[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;	
}

/**
	@copydoc stdMemHash()

	Template parameter version for size component, if you're hoping for the compiler to unroll this
*/
template <size_t size>
	inline hash_t stdMemHashT(const void*memory)
	{
		const char*ident = (const char*)memory;
		hash_t	hash = 0;
		for (size_t i = 0; i < size; i++)
		{
			hash += ident[i];
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);
		return hash;	
	}

/**
	@brief Calculates a character based hash value

	@param begin Pointer to the first character to calculate the hash value of
	@param end Pointer one past the last character to calculate the hash value of
	@return Hash value
*/

template <typename CharType>
	inline hash_t stdCharHash(const CharType*begin, const CharType*end)
	{
		hash_t	result = 0;
		while (begin != end)
		{
			result += *begin++;
			result += (result << 10);
			result ^= (result >> 6);
		}
		result += (result << 3);
		result ^= (result >> 11);
		result += (result << 15);
		return result;	
	}

template <typename CharType>
	inline hash_t stdCharHash(const CharType*begin, size_t len)	//! @overload
	{
		return stdCharHash(begin,begin+len);
	}
		

class HashValue
{
protected:
		hash_t					current_value;
public:

								HashValue():current_value(0) {}
								HashValue(hash_t initial_value):current_value(initial_value) {}

		HashValue&				add(hash_t hash_value)
								{
									current_value *= 17;
									current_value += hash_value;
									return *this;
								};

		HashValue&				addFloat(float value)
								{
									return add(*(const UINT32*)&value);
								};

		HashValue&				addFloat(double value)
								{
									return add(*(const UINT64*)&value);
								};

		operator hash_t() const
		{
			return current_value;
		}
								
};



/**
	@brief Interface used to classify objects that can generate a hash value of their current data

	Ideally hashable objects should also implement == and != operators

*/
interface IHashable
{
protected:
virtual							~IHashable()	{};
public:
virtual	hash_t					hashCode()	const=0;
};


#endif
