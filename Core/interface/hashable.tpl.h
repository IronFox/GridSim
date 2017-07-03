#pragma once



namespace GlobalHashFunctions
{

	template <typename T0, typename T1>
		inline hash_t 			Hash(const std::pair<T0,T1>&ident)
		{
			hash_t	result = Hash(ident.first);
			result *= 17;
			result += Hash(ident.second);
			return result;
		}


}



template <typename T>
	inline hash_t			HashField(const T*field, count_t length)
	{
		using GlobalHashFunctions::Hash;
		hash_t	result = 0;
		for (size_t i = 0; i < length; i++)
		{
			result *= 17;
			result += Hash(field[i]);
		}
		return result;
	}

template <>
	inline hash_t 			HashField(const void*field, count_t length)
	{
		return StdMemHash(field,length*sizeof(void*));
	}

template <>
	inline hash_t 			HashField(const char*field, count_t length)
	{
		return StdCharHash(field,length);
	}
template <>
	inline hash_t 			HashField(const wchar_t*field, count_t length)
	{
		return StdCharHash(field,length);
	}
template <>
	inline hash_t 			HashField(const short*field, count_t length)
	{
		return StdMemHash(field,length*sizeof(*field));
	}
template <>
	inline hash_t 			HashField(const int*field, count_t length)
	{
		return StdMemHash(field,length*sizeof(*field));
	}
template <>
	inline hash_t 			HashField(const long*field, count_t length)
	{
		return StdMemHash(field, length*sizeof(*field));
	}
template <>
	inline hash_t 			HashField(const long long*field, count_t length)
	{
		return StdMemHash(field,length*sizeof(*field));
	}

template <>
	inline hash_t 			HashField(const unsigned char*field, count_t length)
	{
		return StdMemHash(field,length*sizeof(*field));
	}

template <>
	inline hash_t 			HashField(const unsigned short*field, count_t length)
	{
		return StdMemHash(field,length*sizeof(*field));
	}

template <>
	inline hash_t 			HashField(const unsigned int*field, count_t length)
	{
		return StdMemHash(field,length*sizeof(*field));
	}

template <>
	inline hash_t 			HashField(const unsigned long*field, count_t length)
	{
		return StdMemHash(field,length*sizeof(*field));
	}

template <>
	inline hash_t 			HashField(const unsigned long long*field, count_t length)
	{
		return StdMemHash(field,length*sizeof(*field));
	}

