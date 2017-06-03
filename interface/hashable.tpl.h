#pragma once


namespace GlobalHashFunctions
{
	template <typename T>
		inline hash_t 			Hash(const ArrayData<T>&field)
		{
			hash_t	result = 0;
			for (size_t i = 0; i < field.length(); i++)
			{
				result *= 17;
				result += Hash(field[i]);
			}
			return result;
		}

	template <typename T0, typename T1>
		inline hash_t 			Hash(const std::pair<T0,T1>&ident)
		{
			hash_t	result = Hash(ident.first);
			result *= 17;
			result += Hash(ident.second);
			return result;
		}


	template <>
		inline hash_t 			Hash(const ArrayData<const void*>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const ArrayData<void*>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const ArrayData<char>&field)
		{
			return StdCharHash(field.pointer(),field.GetContentSize());
		}
	template <>
		inline hash_t 			Hash(const ArrayData<wchar_t>&field)
		{
			return StdCharHash(field.pointer(),field.GetContentSize());
		}
	template <>
		inline hash_t 			Hash(const ArrayData<short>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}
	template <>
		inline hash_t 			Hash(const ArrayData<int>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}
	template <>
		inline hash_t 			Hash(const ArrayData<long>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}
	template <>
		inline hash_t 			Hash(const ArrayData<long long>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const ArrayData<unsigned char>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const ArrayData<unsigned short>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const ArrayData<unsigned int>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const ArrayData<unsigned long>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const ArrayData<unsigned long long>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}


}
