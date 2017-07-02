#pragma once


namespace GlobalHashFunctions
{
	template <typename T>
		inline hash_t 			Hash(const Ctr::ArrayData<T>&field)
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
		inline hash_t 			Hash(const Ctr::ArrayData<const void*>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const Ctr::ArrayData<void*>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const Ctr::ArrayData<char>&field)
		{
			return StdCharHash(field.pointer(),field.GetContentSize());
		}
	template <>
		inline hash_t 			Hash(const Ctr::ArrayData<wchar_t>&field)
		{
			return StdCharHash(field.pointer(),field.GetContentSize());
		}
	template <>
		inline hash_t 			Hash(const Ctr::ArrayData<short>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}
	template <>
		inline hash_t 			Hash(const Ctr::ArrayData<int>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}
	template <>
		inline hash_t 			Hash(const Ctr::ArrayData<long>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}
	template <>
		inline hash_t 			Hash(const Ctr::ArrayData<long long>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const Ctr::ArrayData<unsigned char>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const Ctr::ArrayData<unsigned short>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const Ctr::ArrayData<unsigned int>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const Ctr::ArrayData<unsigned long>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}

	template <>
		inline hash_t 			Hash(const Ctr::ArrayData<unsigned long long>&field)
		{
			return StdMemHash(field.pointer(),field.GetContentSize());
		}


}
