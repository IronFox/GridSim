#include "hashable.h"
#include "../global_string.h"	//for Template:: namespace

namespace GlobalHashFunctions
{

	hash_t Hash(const IHashable&hashable)
	{
		return hashable.ToHash();
	}


	hash_t MemHash(const void*memory, size_t size)
	{
		return StdMemHash(memory,size);
	}

	hash_t Hash(const char*ident)
	{
		return StdCharHash(ident,strlen(ident));
	}
	hash_t Hash(const wchar_t*ident)
	{
		return StdCharHash(ident,Template::strlen(ident));
	}


	//hash_t Hash(const String&ident)
	//{
	//	return StdCharHash(ident.c_str(),ident.length());
	//}
	//hash_t Hash(const StringW&ident)
	//{
	//	return StdCharHash(ident.c_str(),ident.length());
	//}
	hash_t 			Hash(const std::string&ident)
	{
		return StdCharHash(ident.c_str(),ident.length());
	}
	hash_t 			Hash(const std::wstring&ident)
	{
		return StdCharHash(ident.c_str(),ident.length());
	}
	//hash_t 			Hash(const StringRef&ident)
	//{
	//	return StdCharHash(ident.pointer(),ident.length());
	//}
	//hash_t 			Hash(const StringRefW&ident)
	//{
	//	return StdCharHash(ident.pointer(),ident.length());
	//}

	hash_t 			Hash(float value)
	{
		return *(const UINT32*)&value;
	}

	hash_t 			Hash(double value)
	{
		return *(const UINT64*)&value;
	}

	hash_t 			Hash(long double value)
	{
		return Hash((double)value);
	}



	hash_t Hash(const void*ident)
	{
		return reinterpret_cast<hash_t>(ident);
	}
	

	hash_t Hash(int ident)	//identity
	{
		return (hash_t)ident;
	}

	hash_t Hash(char ident)	//identity
	{
		return (hash_t)(BYTE)ident;
	}

	hash_t Hash(short ident)	//identity
	{
		return (hash_t)(USHORT)ident;
	}

	hash_t Hash(long ident)	//identity
	{
		return (hash_t)ident;
	}

	hash_t Hash(long long ident)	//identity
	{
		return (hash_t)ident;	//need to fix this
	}




	hash_t Hash(unsigned int ident)	//identity
	{
		return (hash_t)ident;
	}

	hash_t Hash(unsigned char ident)	//identity
	{
		return (hash_t)(BYTE)ident;
	}

	hash_t Hash(unsigned short ident)	//identity
	{
		return (hash_t)(USHORT)ident;
	}

	hash_t Hash(unsigned long ident)	//identity
	{
		return (hash_t)ident;
	}

	hash_t Hash(unsigned long long ident)	//identity
	{
		return (hash_t)ident;
	}

}
