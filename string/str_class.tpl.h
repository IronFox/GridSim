#ifndef str_classTplH
#define str_classTplH

#include <wchar.h>


namespace Template
{
	template <>
		inline size_t	__fastcall strlen<char>(const char*string) throw()
		{
			return ::strlen(string);
		}

	template <>
		inline size_t	__fastcall strlen<wchar_t>(const wchar_t*string) throw()
		{
			return ::wcslen(string);
		}

	template <typename T>
		inline size_t	__fastcall strlen(const T*string) throw()
		{
			size_t rs = 0;
			while (*string++)
				rs++;
			return rs;
		}
		
	template <>
		inline const char*	__fastcall strstr<char,char>(const char*haystack, const char*needle) throw()
		{
			return ::strstr(haystack,needle);
		}

	template <>
		inline const wchar_t*	__fastcall strstr<wchar_t,wchar_t>(const wchar_t*haystack, const wchar_t*needle) throw()
		{
			return ::wcsstr(haystack,needle);
		}
		
	template <typename T0, typename T1>
		inline const T0*	__fastcall strstr(const T0*haystack, const T1*needle) throw()
		{
			while (*haystack)
			{
				const T0	*p0 = haystack;
				const T1	*p1 = needle;
				while (*p0 && *p0 == (T0)*p1)
				{
					p0++;
					p1++;
				}
				if (!*p1)
					return haystack;
				haystack++;
			}
			return NULL;
		}

	template <typename T0, typename T1>
		inline const T0*	__fastcall strstr(const T0*haystack, const T0*const haystackEnd, const T1*needle, const T1*const needleEnd) throw()
		{
			while (haystack != haystackEnd)
			{
				const T0	*p0 = haystack;
				const T1	*p1 = needle;
				while (p0 != haystackEnd && p1 != needleEnd && *p0 == (T0)*p1)
				{
					p0++;
					p1++;
				}
				if (p1 == needleEnd)
					return haystack;
				haystack++;
			}
			return NULL;
		}
		
	template <typename T0, typename T1>
		inline const T0*	__fastcall stristr(const T0*haystack, const T1*needle) throw()
		{
			while (*haystack)
			{
				const T0	*p0 = haystack;
				const T1	*p1 = needle;
				while (*p0 && tolower(*p0) == (T0)tolower(*p1))
				{
					p0++;
					p1++;
				}
				if (!*p1)
					return haystack;
				haystack++;
			}
			return NULL;
		}

	template <>
		inline const char*	__fastcall strchr(const char*haystack, char needle) throw()
		{
			return ::strchr(haystack,needle);
		}
	
	template <>
		inline const wchar_t*	__fastcall strchr(const wchar_t*haystack, wchar_t needle) throw()
		{
			return ::wcschr(haystack,needle);
		}

	template <typename T>
		inline const T*	__fastcall strichr(const T*haystack, T needle) throw()
		{
			needle = tolower(needle);
			while (*haystack)
			{
				if (tolower(*haystack) == needle)
					return haystack;
				haystack++;
			}
			return NULL;
		}
		
	template <typename T>
		inline const T*	__fastcall strchr(const T*haystack, T needle) throw()
		{
			while (*haystack)
			{
				if (*haystack == needle)
					return haystack;
				haystack++;
			}
			return NULL;
		}

	template <typename T>
		inline const T*	__fastcall strchr(const T*haystack, const T*const haystackEnd, T needle) throw()
		{
			while (haystack != haystackEnd)
			{
				if (*haystack == needle)
					return haystack;
				haystack++;
			}
			return NULL;
		}
	
	template <typename T>
		inline const T*	__fastcall strchr(const T*haystack, bool isNeedle(T)) throw()
		{
			while (*haystack)
			{
				if (isNeedle(*haystack))
					return haystack;
				haystack++;
			}
			return NULL;
		}
	
	template <typename T>
		inline const T*	__fastcall strchr(const T*haystack, const T*const haystackEnd, bool isNeedle(T)) throw()
		{
			while (haystack != haystackEnd)
			{
				if (isNeedle(*haystack))
					return haystack;
				haystack++;
			}
			return NULL;
		}
	
	template <>
		inline void __fastcall strncpy<char,char>(char*target,const char*source,size_t length) throw()
		{
			::strncpy(target,source,length);
		}

	template <>
		inline void __fastcall strncpy<wchar_t,wchar_t>(wchar_t*target,const wchar_t*source,size_t length) throw()
		{
			::wcsncpy(target,source,length);
		}

	template <typename T0, typename T1>
		inline void __fastcall strncpy(T0*target,const T1*source,size_t length) throw()
		{
			for (size_t i = 0; i < length; i++)
				(*target++) = (T0)(*source++);
		}
		
	template <>
		inline int __fastcall strcmp<char,char>(const char*string0,const char*string1) throw()
		{
			return ::strcmp(string0,string1);
		}
		
	template <>
		inline int __fastcall strcmp<wchar_t,wchar_t>(const wchar_t*string0,const wchar_t*string1) throw()
		{
			return ::wcscmp(string0,string1);
		}
		
	template <typename T0, typename T1>
		inline int __fastcall strcmp(const T0*string0,const T1*string1) throw()
		{
			while (*string0 && *string1)
			{
				if (*string0 < *string1)
					return -1;
				if (*string0 > *string1)
					return 1;
				string0++;
				string1++;
			}
			if (*string0 < *string1)
				return -1;
			if (*string0 > *string1)
				return 1;
			return 0;
		}
		
	template <>
		inline int __fastcall strncmp<char,char>(const char*string0,const char*string1, size_t length) throw()
		{
			return ::strncmp(string0,string1,length);
		}
		
	template <>
		inline int __fastcall strncmp<wchar_t,wchar_t>(const wchar_t*string0,const wchar_t*string1, size_t length) throw()
		{
			return ::wcsncmp(string0,string1,length);
		}
		
	template <typename T0, typename T1>
		inline int __fastcall strncmp(const T0*string0,const T1*string1, size_t length) throw()
		{
			for (size_t i = 0; i < length; i++)
			{
				if (*string0 < *string1)
					return -1;
				if (*string0 > *string1)
					return 1;
				string0++;
				string1++;
			}
			return 0;
		}
	
	template <typename T>
		inline T	__fastcall chrlwr(T c)
		{
			if (c >= (T)'A' && c <= (T)'Z')
				return c + (T)'a' - (T)'A';
			return c;
		}
	
	template <>
		inline int __fastcall strncmpi<char,char>(const char*string0,const char*string1, size_t length) throw()
		{
			return ::strnicmp(string0,string1,length);
		}
		
	template <>
		inline int __fastcall strncmpi<wchar_t,wchar_t>(const wchar_t*string0,const wchar_t*string1, size_t length) throw()
		{
			return ::wcsnicmp(string0,string1,length);
		}


	template <typename T0, typename T1>
		inline int __fastcall strncmpi(const T0*string0,const T1*string1, size_t length) throw()
		{
			for (size_t i = 0; i < length; i++)
			{
				T0	c0 = chrlwr(*string0);
				T1	c1 = chrlwr(*string1);
				if (c0 < c1)
					return -1;
				if (c0 > c1)
					return 1;
				string0++;
				string1++;
			}
			return 0;
		}
				
		
	template <>
		inline int __fastcall strcmpi<char,char>(const char*string0,const char*string1) throw()
		{
			return ::strcmpi(string0,string1);
		}
		
	template <>
		inline int __fastcall strcmpi<wchar_t,wchar_t>(const wchar_t*string0,const wchar_t*string1) throw()
		{
			return ::wcscmpi(string0,string1);
		}


	
	template <typename T0, typename T1>
		inline int __fastcall strcmpi(const T0*string0,const T1*string1) throw()
		{
			while (*string0 && *string1)
			{
				T0	c0 = chrlwr(*string0);
				T1	c1 = chrlwr(*string1);
				if (c0 < c1)
					return -1;
				if (c0 > c1)
					return 1;
				string0++;
				string1++;
			}
			T0	c0 = chrlwr(*string0);
			T1	c1 = chrlwr(*string1);
			if (c0 < c1)
				return -1;
			if (c0 > c1)
				return 1;
			return 0;			
		}	

	template <typename T>
		inline void __fastcall strupr(T*string) throw()
		{
			while (*string != 0)
			{
				*string = toupper(*string);
				string++;
			}
		}

	template <>
		inline void __fastcall strupr<char>(char*string) throw()
		{
			::strupr(string);
		}
	template <>
		inline void __fastcall strupr<wchar_t>(wchar_t*string) throw()
		{
			::wcsupr(string);
		}

	template <typename T>
		inline void __fastcall strlwr(T*string) throw()
		{
			while (*string != 0)
			{
				*string = tolower(*string);
				string++;
			}
		}

	template <>
		inline void __fastcall strlwr<char>(char*string) throw()
		{
			::strlwr(string);
		}

	template <>
		inline void __fastcall strlwr<wchar_t>(wchar_t*string) throw()
		{
			::wcslwr(string);
		}

	template <typename T>
		inline T	__fastcall toupper(T chr) throw()
		{
			if (chr >= T('a') && chr <= T('z'))
				return chr + (T('A') - T('a'));
			//if (chr >= T('�') && chr <= T('�'))
			//	return chr + (T('�') - T('�'));
			return chr;
		}

	template <>
		inline char	__fastcall toupper<char>(char chr) throw()
		{
			return ::toupper(chr);
		}

	template <>
		inline wchar_t	__fastcall toupper<wchar_t>(wchar_t chr) throw()
		{
			return ::towupper(chr);
		}


	template <typename T>
		inline T	__fastcall tolower(T chr) throw()
		{
			if (chr >= T('A') && chr <= T('Z'))
				return chr + (T('a') - T('A'));
			//if (chr >= T('�') && chr <= T('�'))
			//	return chr + (T('�') - T('�'));
			return chr;
		}

	template <>
		inline char	__fastcall tolower<char>(char chr) throw()
		{
			return ::tolower(chr);
		}

	template <>
		inline wchar_t	__fastcall tolower<wchar_t>(wchar_t chr) throw()
		{
			return ::towlower(chr);
		}	

	template <typename T>
		inline bool __fastcall isalpha(T chr) throw()
		{
			return ::iswalpha((wchar_t)chr) != 0;
			//return (chr >= T('a') && chr <= T('z'))
			//	||
			//	(chr >= T('A') && chr <= T('Z'))
			//	||
			//	(chr >= T('�') && chr <= T('�') && chr != T('�') && chr != T('�'));
		}

	template <>
		inline bool __fastcall isalpha(char chr) throw()
		{
			return ::isalpha((BYTE)chr)!=0;
		}

	template <>
		inline bool __fastcall isalpha(wchar_t chr) throw()
		{
			return ::iswalpha(chr)!=0;
		}


	template <typename T>
		inline bool __fastcall isdigit(T chr) throw()
		{
			return (chr >= T('0') && chr <= T('9'));
		}

	template <>
		inline bool __fastcall isdigit(char chr) throw()
		{
			return ::isdigit((BYTE)chr)!=0;
		}

	template <>
		inline bool __fastcall isdigit(wchar_t chr) throw()
		{
			return ::iswdigit(chr)!=0;
		}
		
	template <typename T>
		inline bool	__fastcall isalnum(T chr) throw()
		{
			return isalpha(chr) || isdigit(chr);
		}

	template <>
		inline bool	__fastcall isalnum<char>(char chr) throw()
		{
			return ::isalnum(chr)!=0;
		}

	template <>
		inline bool	__fastcall isalnum<wchar_t>(wchar_t chr) throw()
		{
			return ::iswalnum(chr)!=0;
		}

	template <typename T>
		inline bool __fastcall isEscapable(T chr) throw()
		{
			return chr == T('\\') || chr == T('\'') || chr == T('\"');
		}


	template <typename T0, typename T1>
		inline void	__fastcall Cast(const T0*from, T1*to, count_t numChars)
		{
			for (index_t i = 0; i < numChars; i++)
				to[i] =  (T1)(index_t)from[i];
		}

	template <>
		inline void	__fastcall Cast<char,wchar_t>(const char*from, wchar_t*to, count_t numChars)
		{
			for (index_t i = 0; i < numChars; i++)
				to[i] =  (unsigned char)from[i];
		}

}


template <typename T>
	inline std::ostream& operator<<(std::ostream&stream, const StringTemplate<T>&string)
	{
		const T*c = string.c_str();
		while (*c)
			stream << (char)*c++;
		return stream;
	}

template <>
	inline std::ostream& operator<<(std::ostream&stream, const StringTemplate<char>&string)
	{
		return stream << string.c_str();
	}


template <typename T>
	inline std::ostream& operator<<(std::ostream&stream, const ReferenceExpression<T>&expression)
	{
		expression.print(stream);
		return stream;
	}


template <typename E0, typename E1>
	inline std::ostream& operator<<(std::ostream&stream, const ConcatExpression<E0, E1>&expression)
	{
		expression.print(stream);
		return stream;
	}



template <typename T>
	inline std::ostream& operator<<(std::ostream&stream, const CharacterExpression<T>&expression)
	{
		expression.print(stream);
		return stream;
	}
	


#ifdef WCOUT

	template <typename T>
		inline std::wostream& operator<<(std::wostream&stream, const StringTemplate<T>&string)
		{
			const T*c = string.c_str();
			while (*c)
				stream << (wchar_t)*c++;
			return stream;
		}

	template <>
		inline std::wostream& operator<<(std::wostream&stream, const StringTemplate<char>&string)
		{
			return stream << string.c_str();
		}
	
	template <>
		inline std::wostream& operator<<(std::wostream&stream, const StringTemplate<wchar_t>&string)
		{
			return stream << string.c_str();
		}
		


	template <typename T>
		inline std::wostream& operator<<(std::wostream&stream, const ReferenceExpression<T>&expression)
		{
			expression.print(stream);
			return stream;
		}


	template <typename E0, typename E1>
		inline std::wostream& operator<<(std::wostream&stream, const ConcatExpression<E0, E1>&expression)
		{
			expression.print(stream);
			return stream;
		}



	template <typename T>
		inline std::wostream& operator<<(std::wostream&stream, const CharacterExpression<T>&expression)
		{
			expression.print(stream);
			return stream;
		}
		


#endif


template <typename T>
	T	StringTemplate<T>::zero	= 0;
template <typename T>
	T	*StringTemplate<T>::sz		= &StringTemplate<T>::zero;

template <typename T>
	template <typename Expression>
		inline void StringTemplate<T>::makeFromExpression(const Expression&e)
		{
			string_length = e.length();
			field = allocate(string_length);
			e.writeTo(field);
			//ASSERT_NOT_NULL__(field);
		}

template <typename T>
	template <typename T0, typename T1>
		StringTemplate<T>::StringTemplate(const ConcatExpression<T0,T1>&expression)
		{
			makeFromExpression(expression);
			//ASSERT_NOT_NULL__(field);
		}
		
template <typename T>
	template <class T0>
		StringTemplate<T>::StringTemplate(const ReferenceExpression<T0>&expression)
		{
			makeFromExpression(expression);
			//ASSERT_NOT_NULL__(field);
		}

template <typename T>
	template <class T0>
		StringTemplate<T>::StringTemplate(const StringExpression<T0>&expression)
		{
			makeFromExpression(expression);
			//ASSERT_NOT_NULL__(field);
		}

	
template <typename T>
	template <typename T2>
		StringTemplate<T>::StringTemplate(const T2*string, size_t length)
		{
			field = allocate(length);
			string_length = length;
			Template::Cast(string,field,length);
			//ASSERT_NOT_NULL__(field);
		}	
		
template <typename T>
		StringTemplate<T>::StringTemplate(const ArrayData<T>&array)
		{
			size_t len = array.length();
			if (len > 0 && array.last() == (T)0)
				len --;
			field = allocate(len);
			string_length = len;
			for (size_t i = 0; i < len; i++)
				field[i] = array[i];
			//ASSERT_NOT_NULL__(field);
		}
	

template <>
	inline	StringTemplate<char>::StringTemplate(const char*string)
	{
		size_t len = Template::strlen(string);
		field = allocate(len);
		string_length = len;
		memcpy(field,string,len);
		//ASSERT_NOT_NULL__(field);
	}
	
template <typename T>
	inline	StringTemplate<T>::StringTemplate(const char*string)
	{
		size_t len = Template::strlen(string);
		field = allocate(len);
		string_length = len;
		Template::Cast(string,field,len);
		//ASSERT_NOT_NULL__(field);
	}



template <typename T>
	inline	StringTemplate<T>::StringTemplate(const wchar_t*string)
	{
		size_t len = Template::strlen(string);
		field = allocate(len);
		string_length = len;
		Template::Cast(string,field,len);
		//ASSERT_NOT_NULL__(field);
	}

template <>
	inline	StringTemplate<wchar_t>::StringTemplate(const wchar_t*string)
	{
		size_t len = Template::strlen(string);
		field = allocate(len);
		string_length = len;
		memcpy(field,string,len*sizeof(wchar_t));
		//ASSERT_NOT_NULL__(field);
	}

template <typename T>
	static inline T* pointerToHexT(const void*pointer, int min_len, T*end, T*first)
	{
		size_t value = (size_t)pointer;
		while (value && end != first)
		{
			BYTE c = value & (0xF);
			if (c < 10)
				(*--end) = (T)('0'+c);
			else
				(*--end) = (T)('A'+c-10);
			value>>=4;
			min_len--;
		}
		while (min_len-->0 && end!=first)
			(*--end) = (T)'0';
		return end;
	}

template <typename T>
	StringTemplate<T>::StringTemplate(const void*pointer)
	{
		T buffer[257];
		buffer[256] = 0;
		const T*first = pointerToHexT<T>(pointer,sizeof(pointer)*2,buffer+256,buffer);
		size_t len = buffer+256-first;
		
		field = allocate(len);
		memcpy(field,first,len);
		string_length = len;
		//ASSERT_NOT_NULL__(field);
	}


template <typename T>
	StringTemplate<T>::StringTemplate(bool b)
	{
		if (b)
		{
			string_length = 4;
			field = allocate(4);
			field[0] = (T)'t';
			field[1] = (T)'r';
			field[2] = (T)'u';
			field[3] = (T)'e';
		}
		else
		{
			string_length = 5;
			field = allocate(5);
			field[0] = (T)'f';
			field[1] = (T)'a';
			field[2] = (T)'l';
			field[3] = (T)'s';
			field[4] = (T)'e';
		}
	}

template <typename T>
	StringTemplate<T>::StringTemplate(char c)
	{
		string_length = 1;
		field = allocate(1);
		field[0] = (T)c;
		//ASSERT_NOT_NULL__(field);
	}

template <typename T>
	StringTemplate<T>::StringTemplate(wchar_t c)
	{
		string_length = 1;
		field = allocate(1);
		field[0] = (T)c;
		//ASSERT_NOT_NULL__(field);
	}
	
	
template <typename T>
	StringTemplate<T>::StringTemplate(short value)
	{
		makeSigned<short,unsigned short>(value);
	}
	
template <typename T>
	StringTemplate<T>::StringTemplate(unsigned short value)
	{
		makeUnsigned<unsigned short>(value);
	}
	
template <typename T>
	StringTemplate<T>::StringTemplate(long value)
	{
		makeSigned<long,unsigned long>(value);
	}
	
template <typename T>
	StringTemplate<T>::StringTemplate(int value)
	{
		makeSigned<int,unsigned int>(value);
	}
	
template <typename T>
	StringTemplate<T>::StringTemplate(unsigned value)
	{
		makeUnsigned(value);
	}
	
template <typename T>
	StringTemplate<T>::StringTemplate(unsigned long value)
	{
		makeUnsigned(value);
	}
	
template <typename T>
	StringTemplate<T>::StringTemplate(float value, unsigned char precision, bool force_trailing_zeros)
	{
		makeFloat(value,precision,force_trailing_zeros);
	}

template <typename T>
	StringTemplate<T>::StringTemplate(double value, unsigned char precision, bool force_trailing_zeros)
	{
		makeFloat(value,precision,force_trailing_zeros);
	}
	
template <typename T>
	StringTemplate<T>::StringTemplate(long double value, unsigned char precision, bool force_trailing_zeros)
	{
		makeFloat(value,precision,force_trailing_zeros);
	}
	
template <typename T>
	StringTemplate<T>::StringTemplate(long long value)
	{
		makeSigned<long long, unsigned long long>(value);
	}
	
template <typename T>
	StringTemplate<T>::StringTemplate(unsigned long long value)
	{
		makeUnsigned(value);
	}

#ifdef DSTRING_H
	template <typename T>
		StringTemplate<T>::StringTemplate(const AnsiString&string)
		{
			string_length = string.Length();
			field = allocate(string_length);
			Template::strncpy(field,string.c_str(),string_length);
		}
#endif

	
template <typename T>
	StringTemplate<T>::~StringTemplate()
	{
		delocate(field);
	}

template <typename T>
	void			StringTemplate<T>::swap(StringTemplate<T>&other)
	{
		swp(field,other.field);
		swp(string_length,other.string_length);
	}

template <typename T>
	void			StringTemplate<T>::adoptData(StringTemplate<T>&other)
	{
		if (&other == this)
			return;
		/*
			case 1: fields are identical:
				situation: count(field) = count(other.field) >= 2
				delocate(field) same as delocate(other.field) decreases counter but does not reach 0 (actual memory freeing)
				field copies other.field (which generates an unnecessary copy, though harmless)
				same for string_length
				other.field and other.string_length are set to empty
				valid situation assuming it was valid before
			case 2: fields are not identical
				trivial
		*/
		delocate(field);
		field = other.field;
		string_length = other.string_length;
		
		other.field = sz;
		other.string_length = 0;
	}
	
	
template <typename T>
	inline void StringTemplate<T>::resize(size_t new_length)
	{
		//ASSERT_NOT_NULL__(field);
		STRING_DEBUG("invoking resize. current length is "<<(string_length+1)<<", new length is "<<(new_length+1));
		
		#if __STR_BLOCK_ALLOC__
			size_t		current_alloc = field!=sz?capacity(field):1,
						new_alloc = pad(new_length+1);
		#else
			size_t		current_alloc = string_length+1,
						new_alloc = new_length+1;
		#endif

		STRING_DEBUG(" current allocation is "<<current_alloc<<" at "<<(void*)field);
		STRING_DEBUG(" new allocation is "<<new_alloc);
		
		if (current_alloc == new_alloc
			#if __STR_REFERENCE_COUNT__
				&& (new_alloc==1 || count(field) == 1)
			#endif
			)
		{
			STRING_DEBUG(" nothing new. aborting");
		
			#if __STR_BLOCK_ALLOC__
				if (new_length != string_length)
				{
					STRING_DEBUG(" replacing trailing zero and updating string_length");

					field[new_length] = 0;
					string_length = new_length;
				}
			#endif
			return;
		}
		STRING_DEBUG(" reallocating and copying");
		

		
		delocate(field);
		
		if (!new_length)
		{
			STRING_DEBUG("clearing");
			field = sz;
			string_length = 0;
			return;
		}
		field = allocateField(new_alloc);
		field[new_length] = 0;	
		string_length = new_length;
		
		STRING_DEBUG("resize done");
	}
	
template <typename T>
	inline void StringTemplate<T>::resizeCopy(size_t new_length)
	{
		STRING_DEBUG("invoking resize copy. current length is "<<(string_length+1)<<", new length is "<<(new_length+1));
		#if __STR_BLOCK_ALLOC__
			size_t		current_alloc = field!=sz?capacity(field):1,
						new_alloc = pad(new_length+1);
		#else
			size_t		current_alloc = (string_length+1),
						new_alloc = (new_length+1);
		#endif
		STRING_DEBUG(" current allocation is "<<current_alloc<<" at "<<(void*)field);
		STRING_DEBUG(" new allocation is "<<new_alloc);
		if (current_alloc == new_alloc
			#if __STR_REFERENCE_COUNT__
				&& (new_alloc==1 || count(field) == 1)
			#endif
			)
		{
			STRING_DEBUG(" nothing new. aborting");
			#if __STR_BLOCK_ALLOC__
				if (new_length != string_length)
				{
					STRING_DEBUG(" replacing trailing zero and updating string_length");
					field[new_length] = 0;
					string_length = new_length;
				}
			#endif
			return;
		}
		
		if (!new_length)
		{
			STRING_DEBUG(" reducing to empty string");
			delocate(field);
			field = sz;
			string_length = 0;
			STRING_DEBUG(" resize done");
			return;
		}
		STRING_DEBUG(" reallocating and copying");
		T*new_field = allocateField(new_alloc);
		
		STRING_DEBUG("copying");
		
		if (new_length > string_length)
			memcpy(new_field,field,string_length*sizeof(T));
		else
			memcpy(new_field,field,new_length*sizeof(T));
		new_field[new_length] = 0;	
		
		delocate(field);
		field = new_field;
		string_length = new_length;
		STRING_DEBUG("resize done");
	}
	
	
template <typename T>
	inline	size_t	StringTemplate<T>::length()	const
	{
		return string_length;
	}
	
template <typename T>
	const T*			StringTemplate<T>::c_str()		const
	{
		return field;
	}
	
template <typename T>
	T*					StringTemplate<T>::mutablePointer()
	{
		#if __STR_REFERENCE_COUNT__
			duplicate();
		#endif
		return field;
	}

template <typename T>
	void				StringTemplate<T>::free()
	{
		resize(0);
	}
	/*
template <typename T>
	inline				StringTemplate<T>::operator const T*()					const
	{
		return field;
	}*/


template <typename T>
	template <class Marker>
		void					StringTemplate<T>::genericEraseCharacters(const Marker &marked, bool erase_matches)
		{
			index_t erase_count = 0;
			const T*read = field;
			while (*read)
				erase_count += (marked(*read++) == erase_matches);
			if (!erase_count)
				return;
			if (erase_count == string_length)
			{
				free();
				return;
			}

			T*n = allocate(string_length-erase_count);
			T*write = n;
			read = field;
			while (*read)
			{
				if (marked(*read) != erase_matches)
					(*write++) = *read;
				read++;
			}
			ASSERT_EQUAL__(read,field+string_length);
			ASSERT_EQUAL__(write,n+string_length-erase_count);

			string_length -= erase_count;
			delocate(field);
			field = n;
		}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::eraseCharacter(T chr)
	{
		genericEraseCharacters(CharacterMarker(chr),true);
		return *this;
	}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::eraseCharacters(const StringTemplate<T>& characters, bool erase_matches)
	{
		if (!characters.string_length)
			return *this;
		genericEraseCharacters(FieldMarker(characters.field,characters.string_length),erase_matches);
		return *this;
	}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::eraseCharacters(const T* characters, bool erase_matches)
	{
		if (!characters)
			return *this;
		size_t len = Template::strlen(characters);
		if (!len)
			return *this;
		genericEraseCharacters(FieldMarker(characters,len),erase_matches);
		return *this;
	}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::eraseCharacters(const T* characters, count_t character_count, bool erase_matches)
	{
		if (!characters || !character_count)
			return *this;
		genericEraseCharacters(FieldMarker(characters,character_count),erase_matches);
		return *this;
	}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::eraseCharacters(bool doReplace(T character), bool erase_matches)
	{
		genericEraseCharacters(doReplace,erase_matches);
		return *this;
	}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::eraseCharactersIgnoreCase(const StringTemplate<T>& characters, bool erase_matches)
	{
		if (!characters.string_length)
			return *this;
		genericEraseCharacters(CaseInsensitiveFieldMarker(characters.field,characters.string_length),erase_matches);
		return *this;
	}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::eraseCharactersIgnoreCase(const T* characters, bool erase_matches)										//!< Erases any character that is contained in the zero-terminated string specified by @a characters . Case insensitive
	{
		if (!characters)
			return *this;
		size_t len = Template::strlen(characters);
		if (!len)
			return *this;
		
		genericEraseCharacters(CaseInsensitiveFieldMarker(characters,len),erase_matches);
		return *this;
	}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::eraseCharactersIgnoreCase(const T* characters, count_t character_count, bool erase_matches)
	{
		if (!characters || !character_count)
			return *this;
		genericEraseCharacters(CaseInsensitiveFieldMarker(characters,character_count),erase_matches);
		return *this;
	}


template <typename T>
	template <class Marker>
		count_t					StringTemplate<T>::genericCountCharacters(const Marker&marked, bool count_matches)const
		{
			index_t count = 0;
			const T*read = field;
			while (*read)
				count += (marked(*read++) == count_matches);
			return count;
		}

template <typename T>
	count_t					StringTemplate<T>::countCharacters(const StringTemplate<T>& characters, bool count_matches)	const
	{
		if (!characters.string_length)
			return 0;

		return genericCountCharacters(FieldMarker(characters.field,characters.string_length),count_matches);
	}
template <typename T>
	count_t					StringTemplate<T>::countCharacters(const T* characters, bool count_matches)	const
	{
		if (!characters)
			return 0;
		size_t len = Template::strlen(characters);
		if (!len)
			return 0;
		return genericCountCharacters(FieldMarker(characters,len),count_matches);
	}

template <typename T>
	count_t					StringTemplate<T>::countCharacters(const T* characters, count_t character_count, bool count_matches)	const
	{
		if (!characters || !character_count)
			return 0;
		return genericCountCharacters(FieldMarker(characters,character_count),count_matches);
	}
template <typename T>
	count_t					StringTemplate<T>::countCharacters(bool isMatch(T character), bool count_matches)	const
	{
		return genericCountCharacters(isMatch,count_matches);
	}
template <typename T>
	count_t					StringTemplate<T>::countCharactersIgnoreCase(const StringTemplate<T>& characters, bool count_matches)	const
	{
		if (!characters.string_length)
			return 0;
		return genericCountCharacters(CaseInsensitiveFieldMarker(characters.field,characters.string_length),count_matches);
	}
template <typename T>
	count_t					StringTemplate<T>::countCharactersIgnoreCase(const T* characters, bool count_matches)	const
	{
		if (!characters)
			return 0;
		size_t len = Template::strlen(characters);
		if (!len)
			return 0;

		return genericCountCharacters(CaseInsensitiveFieldMarker(characters,len),count_matches);
	}
template <typename T>
	count_t					StringTemplate<T>::countCharactersIgnoreCase(const T* characters, count_t character_count, bool count_matches)	const
	{
		if (!characters || !character_count)
			return 0;
		return genericCountCharacters(CaseInsensitiveFieldMarker(characters,character_count),count_matches);
	}

template <typename T>
	template <class Marker>
		void					StringTemplate<T>::genericAddSlashes(const Marker&marked)
		{
			count_t matches = genericCountCharacters(marked,true);
			if (!matches)
				return;

			T*n = allocate(string_length+matches);
			T*write = n;
			const T*read = field;
			while (*read)
			{
				if (marked(*read))
					(*write++) = T('\\');
				(*write++) = *read;
				read++;
			}
			ASSERT_EQUAL__(read,field+string_length);
			ASSERT_EQUAL__(write,n+string_length+matches);

			string_length += matches;
			delocate(field);
			field = n;
		}


template <typename T>
	StringTemplate<T>		StringTemplate<T>::addSlashes()	const
	{
		return StringTemplate<T>(*this).addSlashesToThis();
	}
template <typename T>
	StringTemplate<T>		StringTemplate<T>::addSlashes(const T*	before_characters)	const
	{
		return StringTemplate<T>(*this).addSlashesToThis(before_characters);
	}

template <typename T>
	StringTemplate<T>		StringTemplate<T>::addSlashes(const T*	before_characters, count_t before_character_count)	const
	{
		return StringTemplate<T>(*this).addSlashesToThis(before_characters,before_character_count);
	}
template <typename T>
	StringTemplate<T>		StringTemplate<T>::addSlashes(bool isMatch(T character))	const
	{
		return StringTemplate<T>(*this).addSlashesToThis(isMatch);
	}


template <typename T>
	StringTemplate<T>&		StringTemplate<T>::addSlashesToThis()
	{
		genericAddSlashes(Template::isEscapable<T>);
		return *this;
	}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::addSlashesToThis(const T*	before_characters)
	{
		if (!before_characters)
			return *this;
		size_t len = Template::strlen(before_characters);
		if (!len)
			return *this;		
		genericAddSlashes(FieldMarker(before_characters,len));
		return *this;
	}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::addSlashesToThis(const T*	before_characters, count_t before_character_count)
	{
		if (!before_characters || !before_character_count)
			return *this;
		genericAddSlashes(FieldMarker(before_characters,before_character_count));
		return *this;
	}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::addSlashesToThis(bool isMatch(T character))
	{
		genericAddSlashes(isMatch);
		return *this;
	}

template <typename T>
	StringTemplate<T>		StringTemplate<T>::stripSlashes()	const
	{
		return StringTemplate<T>(*this).stripSlashesInThis();
	}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::stripSlashesInThis()
	{
		return eraseCharacter('\\');
	}




template <typename T>
	StringTemplate<T>&		StringTemplate<T>::erase(size_t index,size_t count)
	{
		size_t old_len = string_length;
		if (index >= string_length)
			return *this;
		if (count > string_length-index)
			count = string_length-index;
		if (!count)
			return *this;

		T*n = allocate(string_length-count);
		memcpy(n,field,index*sizeof(T));
		memcpy(n+index,field+(index+count),(string_length-index-count)*sizeof(T));
		string_length -= count;
		delocate(field);
		field = n;
		return *this;
	}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::eraseLeft(size_t count_)
	{
		if (!count_)
			return *this;
		if (count_ >= string_length)
		{
			delocate(field);
			field = sz;
			string_length = 0;
			return *this;
		}
		size_t	new_length = string_length-count_;

		
		STRING_DEBUG("invoking eraseLeft. current length is "<<(string_length+1)<<", new length is "<<(new_length+1));
		#if __STR_BLOCK_ALLOC__
			size_t	current_alloc = field!=sz?capacity(field):1,
					new_alloc = pad(new_length+1);
			STRING_DEBUG(" current allocation is "<<current_alloc<<" at "<<(void*)field);
		#else
			size_t	new_alloc = (new_length+1);
		#endif
		
		STRING_DEBUG(" new allocation is "<<new_alloc);
		#if __STR_BLOCK_ALLOC__
			if (current_alloc == new_alloc
				#if __STR_REFERENCE_COUNT__
					&& count(field) == 1
				#endif
				)
			{
				STRING_DEBUG(" no re-allocation necessary. simply moving to front");
				memcpy(field, field+count_,new_length);
				field[new_length] = 0;
				string_length = new_length;
				
				return *this;
			}
		#endif
		
		T*new_field = allocateField(new_alloc);
		
		STRING_DEBUG("copying");
		
		memcpy(new_field,field+count_,new_length*sizeof(T));
		new_field[new_length] = 0;	
		
		delocate(field);
		field = new_field;
		string_length = new_length;
		STRING_DEBUG("operation done");
		
		
		return *this;
	}

template <typename T>
	StringTemplate<T>&		StringTemplate<T>::Truncate(size_t maxLength)
	{
		if (string_length <= maxLength)
			return *this;
		return eraseRight(string_length - maxLength);
	}


template <typename T>
	StringTemplate<T>&		StringTemplate<T>::eraseRight(size_t count)
	{
		if (!count)
			return *this;
		if (count >= string_length)
		{
			delocate(field);
			field = sz;
			string_length = 0;
			return *this;
		}
		
		resizeCopy(string_length-count);

		
		return *this;
	}


template <typename T>
	bool			StringTemplate<T>::contains(const StringTemplate<T>&sub_str)	const
	{
		return find(sub_str) != 0;
	}
template <typename T>
	bool			StringTemplate<T>::contains(const T*sub_str, size_t length)	const
	{
		return find(sub_str,length) != 0;
	}
template <typename T>
	bool			StringTemplate<T>::contains(const T*sub_str)		const
	{
		return find(sub_str) != 0;
	}
template <typename T>
	bool			StringTemplate<T>::contains(T c)					const
	{
		return find(c) != 0;
	}
template <typename T>
	bool			StringTemplate<T>::containsWord(const T*sub_str) const
	{
		return findWord(sub_str) != 0;
	}
template <typename T>
	bool			StringTemplate<T>::containsWord(const StringTemplate<T>&sub_str) const
	{
		return findWord(sub_str) != 0;
	}
template <typename T>
	bool			StringTemplate<T>::containsIgnoreCase(const StringTemplate<T>&sub_str)	const
	{
		return findIgnoreCase(sub_str) != 0;
	}
template <typename T>
	bool			StringTemplate<T>::containsIgnoreCase(const T*sub_str, size_t length)	const
	{
		return findIgnoreCase(sub_str,length) != 0;
	}
template <typename T>
	bool			StringTemplate<T>::containsIgnoreCase(const T*sub_str)		const
	{
		return findIgnoreCase(sub_str) != 0;
	}
template <typename T>
	bool			StringTemplate<T>::containsIgnoreCase(T c)					const
	{
		return findIgnoreCase(c) != 0;
	}

template <typename T>
	bool			StringTemplate<T>::containsWordIgnoreCase(const T*sub_str) const
	{
		return findWordIgnoreCase(sub_str) != 0;
	}
template <typename T>
	bool			StringTemplate<T>::containsWordIgnoreCase(const StringTemplate<T>&sub_str) const
	{
		return findWordIgnoreCase(sub_str) != 0;
	}


template <typename T>
	index_t			StringTemplate<T>::findWord(const T*sub_str) const
	{
		if (!*sub_str)
			return 0;
		const T*offset = field;
		size_t len = Template::strlen(sub_str);
		while (true)
		{
			const T*ptr = Template::strstr(offset,sub_str);
			if (!ptr)
				return 0;
			if (!Template::isalnum(ptr[len]))
				return ptr-field+1;
			offset = ptr+len;
			while (*offset && Template::isalnum(*offset))
				offset++;
			if (!*offset)
				return 0;
		}
	}
	
template <typename T>
	index_t			StringTemplate<T>::findWord(const StringTemplate<T>&sub_str) const
	{
		if (!sub_str.length())
			return 0;
		const T*offset = field;
		size_t len = sub_str.length();
		while (true)
		{
			const T*ptr = Template::strstr(offset,sub_str.field);
			if (!ptr)
				return 0;
			if (!Template::isalnum(ptr[len]))
				return ptr-field+1;
			offset = ptr+len;
			while (*offset && Template::isalnum(*offset))
				offset++;
			if (!*offset)
				return 0;
		}
	}
	

template <typename T>
	index_t			StringTemplate<T>::findWordIgnoreCase(const T*sub_str) const
	{
		if (!*sub_str)
			return 0;
		const T*offset = field;
		size_t len = Template::strlen(sub_str);
		while (true)
		{
			const T*ptr = Template::stristr(offset,sub_str);
			if (!ptr)
				return 0;
			if (!Template::isalnum(ptr[len]))
				return ptr-field+1;
			offset = ptr+len;
			while (*offset && Template::isalnum(*offset))
				offset++;
			if (!*offset)
				return 0;
		}
	}
	
template <typename T>
	index_t			StringTemplate<T>::findWordIgnoreCase(const StringTemplate<T>&sub_str) const
	{
		if (!sub_str.length())
			return 0;
		const T*offset = field;
		size_t len = sub_str.length();
		while (true)
		{
			const T*ptr = Template::stristr(offset,sub_str.field);
			if (!ptr)
				return 0;
			if (!Template::isalnum(ptr[len]))
				return ptr-field+1;
			offset = ptr+len;
			while (*offset && Template::isalnum(*offset))
				offset++;
			if (!*offset)
				return 0;
		}
	}

	
template <typename T>
	index_t			StringTemplate<T>::find(const StringTemplate<T>&sub_str)	const
	{
		if (!sub_str.length())
			return 0;
		const T*ptr = Template::strstr(field,sub_str.field);
		return ptr?(ptr-field)+1:0;
	}

template <typename T>
	index_t			StringTemplate<T>::find(const T*needle, size_t length)	const
	{
		if (!length)
			return 0;
		const T	*haystack = field,
				*end = field+string_length,
				*needle_end = needle+length;
		while (haystack+length <= end)
		{
			const T	*p0 = haystack;
			const T	*p1 = needle;
			while (*p0 && p1 < needle_end && *p0 == *p1)
			{
				p0++;
				p1++;
			}
			if (p1==needle_end)
				return haystack-field+1;
			haystack++;
		}
		return 0;
	}
	
template <typename T>
	index_t			StringTemplate<T>::find(const T*sub_str)	const
	{
		if (!*sub_str)
			return 0;
		const T*ptr = Template::strstr(field,sub_str);
		return ptr?(ptr-field)+1:0;
	}

template <typename T>
	index_t			StringTemplate<T>::find(T c)					const
	{
		const T*pntr = Template::strchr(field,c);
		return pntr?(pntr-field)+1:0;
	}
	
template <typename T>
	index_t			StringTemplate<T>::GetIndexOf(const StringTemplate<T>&sub_str)	const
	{
		if (!sub_str.length())
			return 0;
		const T*ptr = Template::strstr(field,sub_str.field);
		return ptr?(ptr-field)+1:0;
	}

template <typename T>
	index_t			StringTemplate<T>::GetIndexOf(const T*needle, size_t length)	const
	{
		if (!length)
			return 0;
		const T	*haystack = field,
				*end = field+string_length,
				*needle_end = needle+length;
		while (haystack+length <= end)
		{
			const T	*p0 = haystack;
			const T	*p1 = needle;
			while (*p0 && p1 < needle_end && *p0 == *p1)
			{
				p0++;
				p1++;
			}
			if (p1==needle_end)
				return haystack-field+1;
			haystack++;
		}
		return 0;
	}
	
template <typename T>
	index_t			StringTemplate<T>::GetIndexOf(const T*sub_str)	const
	{
		if (!*sub_str)
			return 0;
		const T*ptr = Template::strstr(field,sub_str);
		return ptr?(ptr-field)+1:0;
	}

template <typename T>
	index_t			StringTemplate<T>::GetIndexOf(T c)					const
	{
		const T*pntr = Template::strchr(field,c);
		return pntr?(pntr-field)+1:0;
	}


	
template <typename T>
	index_t			StringTemplate<T>::find(bool callback(T))					const
	{
		const T *end = field+string_length,
				*haystack = field;
		while (haystack < end && !callback(*haystack))
			haystack++;
		return haystack < end?haystack-field+1:0;
	}






template <typename T>
	index_t			StringTemplate<T>::findLastWord(const T*sub_str) const
	{
		if (!*sub_str)
			return 0;
		size_t len = Template::strlen(sub_str);
		const T*offset = field+string_length-len;
		while (offset >= field)
		{
			while (offset > field && (Template::isalnum(offset[-1]) || Template::isalnum(offset[len])))
				offset--;
			if (Template::isalnum(offset[len]))
				return 0;
			if (!Template::strncmp(offset,sub_str, len))
				return offset-field+1;
			offset--;
		}
		return 0;
	}
	
template <typename T>
	index_t			StringTemplate<T>::findLastWord(const StringTemplate<T>&sub_str) const
	{
		if (!sub_str.length())
			return 0;
		size_t len = sub_str.length();
		const T*offset = field+string_length-len;
		while (offset >= field)
		{
			while (offset > field && (Template::isalnum(offset[-1]) || Template::isalnum(offset[len])))
				offset--;
			if (Template::isalnum(offset[len]))
				return 0;
			if (!Template::strncmp(offset,sub_str.field, len))
				return offset-field+1;
			offset--;
		}
		return 0;
	}
	
	
template <typename T>
	index_t			StringTemplate<T>::findLast(const StringTemplate<T>&sub_str)	const
	{
		return findLast(sub_str.c_str(),sub_str.length());
	}

template <typename T>
	index_t			StringTemplate<T>::findLast(const T*needle, size_t length)	const
	{
		if (!length)
			return 0;
		const T	
				*end = field+string_length,
				*haystack = end-length,
				*needle_end = needle+length;
		while (haystack >= field)
		{
			const T	*p0 = haystack;
			const T	*p1 = needle;
			while (*p0 && p1 < needle_end && *p0 == *p1)
			{
				p0++;
				p1++;
			}
			if (p1==needle_end)
				return haystack-field+1;
			haystack--;
		}
		return 0;
	}
	
template <typename T>
	index_t			StringTemplate<T>::findLast(const T*sub_str)	const
	{
		return findLast(sub_str,Template::strlen(sub_str));
	}

template <typename T>
	index_t			StringTemplate<T>::findLast(T c)					const
	{
		const T *end = field+string_length,
				*haystack = end-1;
		while (haystack >= field && *haystack != c)
			haystack--;
		return haystack >= field?haystack-field+1:0;
	}


template <typename T>
	index_t			StringTemplate<T>::findLast(bool callback(T))					const
	{
		const T *end = field+string_length,
				*haystack = end-1;
		while (haystack >= field && !callback(*haystack))
			haystack--;
		return haystack >= field?haystack-field+1:0;
	}
	
template <typename T>
	index_t			StringTemplate<T>::lastIndexOf(const StringTemplate<T>&sub_str)	const
	{
		return findLast(sub_str);
	}

template <typename T>
	index_t			StringTemplate<T>::lastIndexOf(const T*needle, size_t length)	const
	{
		return findLast(needle,length);
	}
	
template <typename T>
	index_t			StringTemplate<T>::lastIndexOf(const T*sub_str)	const
	{
		return findLast(sub_str);
	}

template <typename T>
	index_t			StringTemplate<T>::lastIndexOf(T c)					const
	{
		return findLast(c);
	}













	
template <typename T>
	index_t			StringTemplate<T>::findIgnoreCase(const StringTemplate<T>&sub_str)	const
	{
		if (!sub_str.length())
			return 0;
		const T*ptr = Template::stristr(field,sub_str.field);
		return ptr?(ptr-field)+1:0;
	}

template <typename T>
	index_t			StringTemplate<T>::findIgnoreCase(const T*needle, size_t length)	const
	{
		if (!length)
			return 0;
		const T	*haystack = field,
				*end = field+string_length,
				*needle_end = needle+length;
		while (haystack+length <= end)
		{
			const T	*p0 = haystack;
			const T	*p1 = needle;
			while (*p0 && p1 < needle_end && Template::tolower(*p0) == Template::tolower(*p1))
			{
				p0++;
				p1++;
			}
			if (p1==needle_end)
				return haystack-field+1;
			haystack++;
		}
		return 0;
	}
	
template <typename T>
	index_t			StringTemplate<T>::findIgnoreCase(const T*sub_str)	const
	{
		if (!*sub_str)
			return 0;
		const T*ptr = Template::stristr(field,sub_str);
		return ptr?(ptr-field)+1:0;
	}

template <typename T>
	index_t			StringTemplate<T>::findIgnoreCase(T c)					const
	{
		const T*pntr = Template::strichr(field,c);
		return pntr?(pntr-field)+1:0;
	}
	
template <typename T>
	index_t			StringTemplate<T>::indexOfIgnoreCase(const StringTemplate<T>&sub_str)	const
	{
		if (!sub_str.length())
			return 0;
		const T*ptr = Template::stristr(field,sub_str.field);
		return ptr?(ptr-field)+1:0;
	}

template <typename T>
	index_t			StringTemplate<T>::indexOfIgnoreCase(const T*needle, size_t length)	const
	{
		if (!length)
			return 0;
		const T	*haystack = field,
				*end = field+string_length,
				*needle_end = needle+length;
		while (haystack+length <= end)
		{
			const T	*p0 = haystack;
			const T	*p1 = needle;
			while (*p0 && p1 < needle_end && Template::tolower(*p0) == Template::tolower(*p1))
			{
				p0++;
				p1++;
			}
			if (p1==needle_end)
				return haystack-field+1;
			haystack++;
		}
		return 0;
	}
	
template <typename T>
	index_t			StringTemplate<T>::indexOfIgnoreCase(const T*sub_str)	const
	{
		if (!*sub_str)
			return 0;
		const T*ptr = Template::stristr(field,sub_str);
		return ptr?(ptr-field)+1:0;
	}

template <typename T>
	index_t			StringTemplate<T>::indexOfIgnoreCase(T c)					const
	{
		const T*pntr = Template::strichr(field,c);
		return pntr?(pntr-field)+1:0;
	}







template <typename T>
	index_t			StringTemplate<T>::findLastWordIgnoreCase(const T*sub_str) const
	{
		if (!*sub_str)
			return 0;
		size_t len = Template::strlen(sub_str);
		const T*offset = field+string_length-len;
		while (offset >= field)
		{
			while (offset > field && (Template::isalnum(offset[-1]) || Template::isalnum(offset[len])))
				offset--;
			if (Template::isalnum(offset[len]))
				return 0;
			if (!Template::strncmpi(offset,sub_str, len))
				return offset-field+1;
			offset--;
		}
		return 0;
	}
	
template <typename T>
	index_t			StringTemplate<T>::findLastWordIgnoreCase(const StringTemplate<T>&sub_str) const
	{
		if (!sub_str.length())
			return 0;
		size_t len = sub_str.length();
		const T*offset = field+string_length-len;
		while (offset >= field)
		{
			while (offset > field && (Template::isalnum(offset[-1]) || Template::isalnum(offset[len])))
				offset--;
			if (Template::isalnum(offset[len]))
				return 0;
			if (!Template::strncmpi(offset,sub_str.field, len))
				return offset-field+1;
			offset--;
		}
		return 0;
	}
	
	
template <typename T>
	index_t			StringTemplate<T>::findLastIgnoreCase(const StringTemplate<T>&sub_str)	const
	{
		return findLastIgnoreCase(sub_str.c_str(),sub_str.length());
	}

template <typename T>
	index_t			StringTemplate<T>::findLastIgnoreCase(const T*needle, size_t length)	const
	{
		if (!length)
			return 0;
		const T	
				*end = field+string_length,
				*haystack = end-length,
				*needle_end = needle+length;
		while (haystack >= field)
		{
			const T	*p0 = haystack;
			const T	*p1 = needle;
			while (*p0 && p1 < needle_end && Template::tolower(*p0) == Template::tolower(*p1))
			{
				p0++;
				p1++;
			}
			if (p1==needle_end)
				return haystack-field+1;
			haystack--;
		}
		return 0;
	}
	
template <typename T>
	index_t			StringTemplate<T>::findLastIgnoreCase(const T*sub_str)	const
	{
		return findLastIgnoreCase(sub_str,Template::strlen(sub_str));
	}

template <typename T>
	index_t			StringTemplate<T>::findLastIgnoreCase(T c)					const
	{
		c = Template::tolower(c);
		const T *end = field+string_length,
				*haystack = end-1;
		while (haystack >= field && Template::tolower(*haystack) != c)
			haystack--;
		return haystack >= field?haystack-field+1:0;
	}



template <typename T>
	index_t			StringTemplate<T>::lastIndexOfIgnoreCase(const StringTemplate<T>&sub_str)	const
	{
		return findLastIgnoreCase(sub_str);
	}

template <typename T>
	index_t			StringTemplate<T>::lastIndexOfIgnoreCase(const T*needle, size_t length)	const
	{
		return findLastIgnoreCase(needle,length);
	}
	
template <typename T>
	index_t			StringTemplate<T>::lastIndexOfIgnoreCase(const T*sub_str)	const
	{
		return findLastIgnoreCase(sub_str);
	}

template <typename T>
	index_t			StringTemplate<T>::lastIndexOfIgnoreCase(T c)					const
	{
		return findLastIgnoreCase(c);
	}













template <typename T>
	hash_t	StringTemplate<T>::hashCode() const
	{
		return stdCharHash(field,field+string_length);
	}





template <typename T>
	StringTemplate<T>&	StringTemplate<T>::setLength(size_t newLength)
	{
		resizeCopy(newLength);
		return *this;
	}


template <typename T>
	template <typename IndexType>
	StringTemplate<T>				StringTemplate<T>::subString(IndexType index, size_t count) const
	{
		if (index<0)
		{
			count += index;
			index=0;
		}
		if ((size_t)index >= string_length)
			return StringTemplate<T>();
		if (count > string_length)
			count = string_length;
		if (index+count>string_length)
			count = string_length-index;
		return StringTemplate<T>(field+index,count);
	}


	
template <typename T>
	ReferenceExpression<T>				StringTemplate<T>::subStringRef(int index, size_t count) const
	{
		if (index<0)
		{
			count += index;
			index=0;
		}
		if ((size_t)index >= string_length)
			return ReferenceExpression<T>(field,0);
		if (count > string_length)
			count = string_length;
		if (index+count>string_length)
			count = string_length-index;
		return ReferenceExpression<T>(field+index,count);
	}

template <typename T>
	ReferenceExpression<T>				StringTemplate<T>::ref()	const
	{
		return ReferenceExpression<T>(field,string_length);
	}




template <typename T>
	StringTemplate<T>&				StringTemplate<T>::trimThis()
	{
		if (!string_length || (!isWhitespace(field[0]) && !isWhitespace(field[string_length-1])))
			return *this;

		const T*left=field;
		while ((*left) && isWhitespace(*left))
			left++;
		if (!*left)
		{
			delocate(field);
			field = sz;
			string_length = 0;
			return *this;
		}
		const T*right = field+string_length-1;
		while (isWhitespace(*right))
			right--;
		size_t len = right-left+1;

		T*n = allocate(len);
		memcpy(n,left,len*sizeof(T));
		delocate(field);
		field = n;
		string_length = len;
		return *this;
	}


template <typename T>
	ReferenceExpression<T>				ReferenceExpression<T>::Trim()			const
	{
		if (!len)
			return ReferenceExpression<T>();
		const T*left=reference;
		const T*const end = reference + len;
		while (left < end && isWhitespace(*left))
			left++;
		if (left == end)
			return ReferenceExpression<T>();
		const T*right = end-1;
		while (isWhitespace(*right))
			right--;
		size_t len2 = right-left+1;
		if (!left && len2 == len)
			return *this;
		return ReferenceExpression<T>(left,len2);
	}

template <typename T>
	ReferenceExpression<T>				ReferenceExpression<T>::SubStringRef(int index, count_t count /*= (count_t)-1*/)	 const
	{
		if (index<0)
		{
			count += index;
			index=0;
		}
		if ((size_t)index >= len)
			return ReferenceExpression<T>(reference,0);
		if (count > len)
			count = len;
		if (index+count>len)
			count = len-index;
		return ReferenceExpression<T>(reference+index,count);
	}


template <typename T>
	ReferenceExpression<T>				ReferenceExpression<T>::TrimLeft()		const
	{
		size_t left = 0;
//		const T*const end = pointer + len;
		while (left < len && isWhitespace(reference[left]))
			left++;
		if (!left)
			return *this;
		return ReferenceExpression<T>(reference+left,len-left);
	}


template <typename T>
	ReferenceExpression<T>				ReferenceExpression<T>::TrimRight()		const
	{
		if (!len)
			return ReferenceExpression<T>();
		size_t right = len-1;
		while (right && isWhitespace(reference[right]))
			right--;
		right+=!isWhitespace(reference[right]);  //should increase cnt if necessary
		
		if (right == len)
			return *this;
		return ReferenceExpression<T>(reference,right);
	}

	

template <typename T>
	StringTemplate<T>				StringTemplate<T>::trim()			const
	{
		return ref().Trim();
	}

template <typename T>
	ReferenceExpression<T>				StringTemplate<T>::trimRef()			const
	{
		return ref().Trim();
	}

template <typename T>
	StringTemplate<T>				StringTemplate<T>::trimLeft()		const
	{
		return ref().TrimLeft();
	}

template <typename T>
	ReferenceExpression<T>				StringTemplate<T>::trimLeftRef()		const
	{
		return ref().TrimLeft();
	}

template <typename T>
	StringTemplate<T>				StringTemplate<T>::trimRight()		const
	{
		return ref().TrimRight();
	}

template <typename T>
	ReferenceExpression<T>				StringTemplate<T>::trimRightRef()		const
	{
		return ref().TrimRight();
	}


template <typename T>
	StringTemplate<T>&				StringTemplate<T>::insert(size_t index, const StringTemplate<T>&str)
	{
		if (!str.string_length)
			return *this;
		if (!string_length)
			return this->operator=(str);

		if (index >= string_length)
			index = string_length-1;
		T*n = allocate(string_length+str.string_length);
		
		memcpy(n,field,index*sizeof(T));
		memcpy(n+index,str.field,str.string_length*sizeof(T));
		memcpy(n+index+str.string_length,field+index,(string_length-index)*sizeof(T));
		string_length += str.string_length;
		delocate(field);
		field = n;
		return *this;
	}

template <typename T>
	StringTemplate<T>&				StringTemplate<T>::replaceSubString(size_t index, size_t count, const StringTemplate<T>&str)
	{
		if (!str.string_length)
			return erase(index,count);
		
		if (!string_length)
			return this->operator=(str);
			
		if (index >= string_length)
			index = string_length-1;
		if (count > string_length-index)
			count = string_length-index;

		size_t new_len = string_length-count+str.string_length;
		T*n = allocate(new_len);
		memcpy(n,field,index*sizeof(T));
		memcpy(n+index,str.field,str.string_length*sizeof(T));
		memcpy(n+index+str.string_length,field+(index+count),(string_length-index-count)*sizeof(T));
		string_length = new_len;
		delocate(field);
		field = n;
		return *this;
	}

template <typename T>
	StringTemplate<T>&				StringTemplate<T>::insert(size_t index, T c)
	{
		if (!string_length)
		{
			this->operator=(c);
			return *this;
		}
		if (index >= string_length)
			index = string_length-1;
		T*n = allocate(string_length+1);
		
		memcpy(n,field,index*sizeof(T));
		n[index] = c;
		memcpy(n+index+1,field+index,(string_length-index)*sizeof(T));
		string_length ++;
		delocate(field);
		field = n;
		return *this;

	}


template <typename T>
	StringTemplate<T>&				StringTemplate<T>::convertToLowerCase()
	{
		#if __STR_REFERENCE_COUNT__
			duplicate();
		#endif
		Template::strlwr(field);
		return *this;
	}

template <typename T>
	StringTemplate<T>&				StringTemplate<T>::convertToUpperCase()
	{
		#if __STR_REFERENCE_COUNT__
			duplicate();
		#endif
		Template::strupr(field);
		return *this;
	}

template <typename T>
	StringTemplate<T>				StringTemplate<T>::copyToLowerCase()	const
	{
		return StringTemplate<T>(*this).convertToLowerCase();
	}

template <typename T>
	StringTemplate<T>				StringTemplate<T>::copyToUpperCase()	const
	{
		return StringTemplate<T>(*this).convertToUpperCase();
	}

template <typename T>
	StringTemplate<T>				StringTemplate<T>::getBetween(const StringTemplate<T>&left_delimiter, const StringTemplate<T>&right_delimiter)	const
	{
		const T*begin = Template::strstr(field,left_delimiter.field);
		if (!begin)
			return StringTemplate<T>();
		begin += left_delimiter.string_length;
		const T*end = Template::strstr(begin,right_delimiter.field);
		if (!end)
			return StringTemplate<T>();
		return StringTemplate<T>(begin,end-begin);
	}

template <typename T>
	ReferenceExpression<T>				StringTemplate<T>::getBetweenRef(const StringTemplate<T>&left_delimiter, const StringTemplate<T>&right_delimiter)	const
	{
		const T*begin = Template::strstr(field,left_delimiter.field);
		if (!begin)
			return ReferenceExpression<T>(field,0);
		begin += left_delimiter.string_length;
		const T*end = Template::strstr(begin,right_delimiter.field);
		if (!end)
			return ReferenceExpression<T>(field,0);
		return ReferenceExpression<T>(begin,end-begin);
	}


template <typename T>
	bool				StringTemplate<T>::beginsWith(const T*string)		const
	{
		size_t len = Template::strlen(string);
		if (len > string_length)
			return false;
		return !Template::strncmp(field,string,len);
	}

template <typename T>
	bool				StringTemplate<T>::beginsWith(const StringTemplate<T>&string)		const
	{
		if (string.string_length > string_length)
			return false;
		return !Template::strncmp(field,string.field,string.string_length);

	}
template <typename T>
	bool				StringTemplate<T>::beginsWith(const ReferenceExpression<T>&string)		const
	{
		if (string.length() > string_length)
			return false;
		return !Template::strncmp(field,string.pointer(),string.length());

	}

template <typename T>
	bool				StringTemplate<T>::endsWith(const T*string)			const
	{
		size_t len = Template::strlen(string);
		if (len > string_length)
			return false;
		return !Template::strncmp(field+string_length-len,string,len);
	}

template <typename T>
	bool				StringTemplate<T>::endsWith(const StringTemplate<T>&string)		const
	{
		if (string.string_length > string_length)
			return false;
		return !Template::strncmp(field+string_length-string.string_length,string.field,string.string_length);
	}

template <typename T>
	bool				StringTemplate<T>::endsWith(const ReferenceExpression<T>&string)		const
	{
		if (string.length() > string_length)
			return false;
		return !Template::strncmp(field+string_length-string.length(),string.pointer(),string.length());
	}

template <typename T>
	bool				StringTemplate<T>::beginsWithCaseIgnore(const T*string)		const
	{
		size_t len = Template::strlen(string);
		if (len > string_length)
			return false;
		return !Template::strncmpi(field,string,len);
	}

template <typename T>
	bool				StringTemplate<T>::beginsWithCaseIgnore(const StringTemplate<T>&string)		const
	{
		if (string.string_length > string_length)
			return false;
		return !Template::strncmpi(field,string.field,string.string_length);

	}
template <typename T>
	bool				StringTemplate<T>::beginsWithCaseIgnore(const ReferenceExpression<T>&string)		const
	{
		if (string.length() > string_length)
			return false;
		return !Template::strncmpi(field,string.pointer(),string.length());

	}

template <typename T>
	bool				StringTemplate<T>::endsWithCaseIgnore(const T*string)			const
	{
		size_t len = Template::strlen(string);
		if (len > string_length)
			return false;
		return !Template::strncmpi(field+string_length-len,string,len);
	}

template <typename T>
	bool				StringTemplate<T>::endsWithCaseIgnore(const StringTemplate<T>&string)		const
	{
		if (string.string_length > string_length)
			return false;
		return !Template::strncmpi(field+string_length-string.string_length,string.field,string.string_length);
	}

template <typename T>
	bool				StringTemplate<T>::endsWithCaseIgnore(const ReferenceExpression<T>&string)		const
	{
		if (string.length() > string_length)
			return false;
		return !Template::strncmpi(field+string_length-string.length(),string.pointer(),string.length());
	}

	
template <typename T>
	T				StringTemplate<T>::firstChar()							const
	{
		return field[0];
	}

template <typename T>
	T				StringTemplate<T>::lastChar()							const
	{
		return field[string_length-1];
	}

template <typename T>
	const T*		StringTemplate<T>::lastCharPointer()					const
	{
		return field+string_length-1;
	}



template <typename T>
	StringTemplate<T>				StringTemplate<T>::firstWord()							const
	{
		const T*begin = field;
		while (isWhitespace(*begin))
			begin++;
		const T*end = begin;
		while (*end && !isWhitespace(*end))
			end++;
		return StringTemplate<T>(begin,end-begin);
	}

template <typename T>
	ReferenceExpression<T>				StringTemplate<T>::firstWordRef()							const
	{
		const T*begin = field;
		while (isWhitespace(*begin))
			begin++;
		const T*end = begin;
		while (*end && !isWhitespace(*end))
			end++;
		return ReferenceExpression<T>(begin,end-begin);
	}

template <typename T>
	StringTemplate<T>				StringTemplate<T>::lastWord()							const
	{
		const T	*end = field+string_length-1,
				*begin = field;
		while (end >= begin && isWhitespace(*end))
			end--;
		begin = end;
		while (begin >= field && !isWhitespace(*begin))
			begin--;
		begin++;
		end++;
		
		return StringTemplate<T>(begin,begin-end);
	}
	
template <typename T>
	ReferenceExpression<T>				StringTemplate<T>::lastWordRef()							const
	{
		const T	*end = field+string_length-1,
				*begin = field;
		while (end >= begin && isWhitespace(*end))
			end--;
		begin = end;
		while (begin >= field && !isWhitespace(*begin))
			begin--;
		begin++;
		end++;
		
		return ReferenceExpression<T>(begin,begin-end);
	}


template <typename T>
	StringTemplate<T>&				StringTemplate<T>::replace(const StringTemplate<T>&needle, const StringTemplate<T>&replacement)
	{
		while (size_t at = find(needle))
			replaceSubString(at-1,needle.string_length,replacement);
		return *this;
	}

template <typename T>
	StringTemplate<T>&				StringTemplate<T>::replace(T needle_char, const StringTemplate<T>&replacement)
	{
		while (size_t at = find(needle_char))
			replaceSubString(at-1,1,replacement);
		return *this;
	}

template <typename T>
	StringTemplate<T>&	StringTemplate<T>::replaceCharacters(bool doReplace(T character), T replacement)
	{
		#if __STR_REFERENCE_COUNT__
			duplicate();
		#endif
		for (size_t i = 0; i < string_length; i++)
			if (doReplace(field[i]))
				field[i] = replacement;
		return *this;
	}


template <typename T>
	bool	StringTemplate<T>::isValid(bool validCharacter(T character))	const
	{
		for (size_t i = 0; i < string_length; i++)
			if (!validCharacter(field[i]))
				return false;
		return true;
	}

template <typename T>
	StringTemplate<T>&	StringTemplate<T>::replace(T replace_what, T replace_with)
	{
		#if __STR_REFERENCE_COUNT__
			duplicate();
		#endif
		for (size_t i = 0; i < string_length; i++)
			if (field[i] == replace_what)
				field[i] = replace_with;
		return *this;
	}

template <typename T>
	void				StringTemplate<T>::set(size_t index, T c)
	{
		if (index >= string_length)
			return;
		#if __STR_REFERENCE_COUNT__
			duplicate();
		#endif
		field[index] = c;
	}

template <typename T>
	T				StringTemplate<T>::get(size_t index) const
	{
		if (index >= string_length)
			return 0;
		return field[index];
	}


#if 0
template <typename T>
	T				StringTemplate<T>::operator[](size_t index) const
	{
		if (index >= string_length)
			return 0;
		return field[index];
	}

template <typename T>
	T				StringTemplate<T>::operator[](int index)			const
	{
		if (/*index < 0 ||*/ (size_t)index >= string_length)
			return 0;
		return field[index];
	}

template <typename T>
	T				StringTemplate<T>::operator[](BYTE index)			const
	{
		if ((size_t)index >= string_length)
			return 0;
		return field[index];
	}
#endif

template <typename T>
	void	StringTemplate<T>::operator+=(const StringTemplate<T>&other)
	{
		if (!other.string_length)
			return;
		STRING_METHOD_BEGIN("(const StringTemplate<T>&other)",other);
		size_t old_length = string_length;
		size_t new_length = string_length + other.string_length;
		resizeCopy(new_length);
		if (&other != this)
			memcpy(field+old_length,other.field,other.string_length*sizeof(T));
		else
			memcpy(field+old_length,field,old_length*sizeof(T));
		STRING_METHOD_END
	}

template <typename T>	
	void	StringTemplate<T>::operator+=(const T*string)
	{
		if (!string)
			return;
		STRING_METHOD_BEGIN("(const T*string)",string);
		size_t 	len = Template::strlen(string);
		
		if (!len)
		{
			STRING_METHOD_END
			return;
		}
		size_t	old_length = string_length,
				new_length = string_length+len;
		bool self = field == string;
		resizeCopy(new_length);
		if (!self)
			memcpy(field+old_length,string,len*sizeof(T));
		else
			memcpy(field+old_length,field,old_length*sizeof(T));
		STRING_METHOD_END
	}

template <typename T>	
	void	StringTemplate<T>::operator+=(T c)
	{
		STRING_METHOD_BEGIN("(T c)",c);
		size_t new_length = string_length + 1;
		resizeCopy(new_length);
		field[string_length-1] = c;	//string_length increased during resizeCopy(). Trailing zero should be set
		STRING_METHOD_END
	}

	
template <typename T>
	template <typename Expression>
		inline void	StringTemplate<T>::appendExpression(const Expression&expression)
		{
			size_t	old_length = string_length,
					new_length = string_length + expression.length();
			if (new_length == string_length)
				return;
			if (string_length && expression.references(field))
			{
				T*new_field = allocate(new_length);
				memcpy(new_field,field,old_length*sizeof(T));
				expression.writeTo(new_field+old_length);
				delocate(field);
				field = new_field;
				string_length = new_length;
			}
			else
			{
				resizeCopy(new_length);
				expression.writeTo(field+old_length);
			}
		}

template <typename T>	
	template <typename T0, typename T1>
		void	StringTemplate<T>::operator+=(const ConcatExpression<T0,T1>&expression)
		{
			appendExpression(expression);
		}
		
template <typename T>	
	template <class T0>
		void				StringTemplate<T>::operator+=(const CharacterExpression<T0>&expression)
		{
			appendExpression(expression);
		}
				
template <typename T>	
	template <class T0>
		void				StringTemplate<T>::operator+=(const ReferenceExpression<T0>&expression)
		{
			appendExpression(expression);
		}
		
template <typename T>
	template <class T0>
		void				StringTemplate<T>::operator+=(const StringExpression<T0>&expression)
		{
			appendExpression(expression);
		}



template <typename T>	
	template <typename T0, typename T1>
		bool	StringTemplate<T>::operator<(const ConcatExpression<T0,T1>&expression)	const
		{
			return expression.compareTo(field,string_length)>0;
		}
		
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::operator<(const ReferenceExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)>0;
		}
				
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::operator<(const CharacterExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)>0;
		}
		
template <typename T>
	template <class T0>
		bool				StringTemplate<T>::operator<(const StringExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)>0;
		}

template <typename T>	
	template <typename T0, typename T1>
		bool	StringTemplate<T>::operator<=(const ConcatExpression<T0,T1>&expression)	const
		{
			return expression.compareTo(field,string_length)>=0;
		}
		
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::operator<=(const CharacterExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)>=0;
		}
				
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::operator<=(const ReferenceExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)>=0;
		}
		
template <typename T>
	template <class T0>
		bool				StringTemplate<T>::operator<=(const StringExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)>=0;
		}

template <typename T>	
	template <typename T0, typename T1>
		bool	StringTemplate<T>::operator>=(const ConcatExpression<T0,T1>&expression)	const
		{
			return expression.compareTo(field,string_length)<=0;
		}
		
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::operator>=(const CharacterExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)<=0;
		}
		
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::operator>=(const ReferenceExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)<=0;
		}
		
template <typename T>
	template <class T0>
		bool				StringTemplate<T>::operator>=(const StringExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)<=0;
		}

template <typename T>	
	template <typename T0, typename T1>
		bool	StringTemplate<T>::operator>(const ConcatExpression<T0,T1>&expression)	const
		{
			return expression.compareTo(field,string_length)<0;
		}
		
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::operator>(const ReferenceExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)<0;
		}
		
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::operator>(const CharacterExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)<0;
		}
		
template <typename T>
	template <class T0>
		bool				StringTemplate<T>::operator>(const StringExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)<0;
		}

template <typename T>	
	template <typename T0, typename T1>
		bool	StringTemplate<T>::operator!=(const ConcatExpression<T0,T1>&expression)	const
		{
			return expression.compareTo(field,string_length)!=0;
		}
		
template <typename T>	
	template <typename T0>
		bool	StringTemplate<T>::operator!=(const CharacterExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)!=0;
		}
		
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::operator!=(const ReferenceExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)!=0;
		}
		
template <typename T>
	template <class T0>
		bool				StringTemplate<T>::operator!=(const StringExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)!=0;
		}

template <typename T>	
	template <typename T0, typename T1>
		bool	StringTemplate<T>::operator==(const ConcatExpression<T0,T1>&expression)	const
		{
			return expression.compareTo(field,string_length)==0;
		}
		
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::operator==(const ReferenceExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)==0;
		}
				
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::operator==(const CharacterExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)==0;
		}
		
template <typename T>
	template <class T0>
		bool				StringTemplate<T>::operator==(const StringExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)==0;
		}

template <typename T>	
	template <typename T0, typename T1>
		bool	StringTemplate<T>::equals(const ConcatExpression<T0,T1>&expression)	const
		{
			return expression.compareTo(field,string_length)==0;
		}
		
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::equals(const ReferenceExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)==0;
		}
				
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::equals(const CharacterExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)==0;
		}
		
template <typename T>
	template <class T0>
		bool				StringTemplate<T>::equals(const StringExpression<T0>&expression)	const
		{
			return expression.compareTo(field,string_length)==0;
		}

template <typename T>	
	template <typename T0, typename T1>
		bool	StringTemplate<T>::equalsIgnoreCase(const ConcatExpression<T0,T1>&expression)	const
		{
			return expression.compareToIgnoreCase(field,string_length)==0;
		}
		
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::equalsIgnoreCase(const ReferenceExpression<T0>&expression)	const
		{
			return expression.compareToIgnoreCase(field,string_length)==0;
		}
				
template <typename T>	
	template <class T0>
		bool				StringTemplate<T>::equalsIgnoreCase(const CharacterExpression<T0>&expression)	const
		{
			return expression.compareToIgnoreCase(field,string_length)==0;
		}
		
template <typename T>
	template <class T0>
		bool				StringTemplate<T>::equalsIgnoreCase(const StringExpression<T0>&expression)	const
		{
			return expression.compareToIgnoreCase(field,string_length)==0;
		}

		
#ifdef DSTRING_H
	template <typename T>
		bool				StringTemplate<T>::operator<(const AnsiString&string)	const
		{
			return Template::strcmp(field,string.c_str())<0;
		}

	template <typename T>
		bool				StringTemplate<T>::operator<=(const AnsiString&string)	const
		{
			return Template::strcmp(field,string.c_str())<=0;
		}

	template <typename T>
		bool				StringTemplate<T>::operator>(const AnsiString&string)	const
		{
			return Template::strcmp(field,string.c_str())>0;
		}

	template <typename T>
		bool				StringTemplate<T>::operator>=(const AnsiString&string)	const
		{
			return Template::strcmp(field,string.c_str())>=0;
		}

	template <typename T>
		bool				StringTemplate<T>::operator==(const AnsiString&string)	const
		{
			return Template::strcmp(field,string.c_str())==0;
		}

	template <typename T>
		bool				StringTemplate<T>::equals(const AnsiString&string)	const
		{
			return Template::strcmp(field,string.c_str())==0;
		}

	template <typename T>
		bool				StringTemplate<T>::equalsCaseIgnore(const AnsiString&string)	const
		{
			return Template::strcmpi(field,string.c_str())==0;
		}

	template <typename T>
		bool				StringTemplate<T>::operator!=(const AnsiString&string)	const
		{
			return Template::strcmp(field,string.c_str())!=0;
		}

#endif
	

template <typename T>
	bool				StringTemplate<T>::operator<(const StringTemplate<T>&string)		const
	{
		return Template::strcmp(field,string.field)<0;
	}
	
template <typename T>
	bool				StringTemplate<T>::operator<=(const StringTemplate<T>&string)		const
	{
		return Template::strcmp(field,string.field)<=0;
	}
	
template <typename T>
	bool				StringTemplate<T>::operator>(const StringTemplate<T>&string)		const
	{
		return Template::strcmp(field,string.field)>0;
	}
	
template <typename T>
	bool				StringTemplate<T>::operator>=(const StringTemplate<T>&string)		const
	{
		return Template::strcmp(field,string.field)>=0;
	}
	
template <typename T>
	bool				StringTemplate<T>::operator==(const StringTemplate<T>&string)		const
	{
		return Template::strcmp(field,string.field)==0;
	}
	
template <typename T>
	bool				StringTemplate<T>::operator!=(const StringTemplate<T>&string)		const
	{
		return Template::strcmp(field,string.field)!=0;
	}

template <typename T>
	bool				StringTemplate<T>::operator<(const T*string)		const
	{
		return Template::strcmp(field,string)<0;
	}

template <typename T>
	bool				StringTemplate<T>::operator<=(const T*string)		const
	{
		return Template::strcmp(field,string)<=0;
	}

template <typename T>
	bool				StringTemplate<T>::operator>(const T*string)		const
	{
		return Template::strcmp(field,string)>0;
	}

template <typename T>
	bool				StringTemplate<T>::operator>=(const T*string)		const
	{
		return Template::strcmp(field,string)>=0;
	}

template <typename T>
	bool				StringTemplate<T>::operator==(const T*string)		const
	{
		return Template::strcmp(field,string)==0;
	}

template <typename T>
	bool				StringTemplate<T>::equals(const T*string)		const
	{
		return Template::strcmp(field,string)==0;
	}
template <typename T>
	bool				StringTemplate<T>::equals(T chr)		const
	{
		return field[0] == chr && !field[1];;
	}
template <typename T>
	bool				StringTemplate<T>::equals(const StringTemplate<T>&string)		const
	{
		return Template::strcmp(field,string.field)==0;
	}

template <typename T>
	bool				StringTemplate<T>::equalsIgnoreCase(const T*string)		const
	{
		return Template::strcmpi(field,string)==0;
	}

template <typename T>
	bool				StringTemplate<T>::equalsIgnoreCase(const StringTemplate<T>&string)		const
	{
		return Template::strcmpi(field,string.field)==0;
	}
template <typename T>
	bool				StringTemplate<T>::equalsIgnoreCase(T chr)		const
	{
		return Template::toupper(field[0]) == Template::toupper(chr) && !field[1];
	}



template <typename T>
	bool				StringTemplate<T>::operator!=(const T*string)		const
	{
		return Template::strcmp(field,string)!=0;
	}


template <typename T>
	bool				StringTemplate<T>::operator<(T c)		const
	{
		return field[0] < c;
	}
	
template <typename T>
	bool				StringTemplate<T>::operator<=(T c)		const
	{
		return field[0] <= c;
	}
	
template <typename T>
	bool				StringTemplate<T>::operator>(T c)		const
	{
		return field[0] > c;
	}
	
template <typename T>
	bool				StringTemplate<T>::operator>=(T c)		const
	{
		return field[0] >= c;
	}
	
template <typename T>
	bool				StringTemplate<T>::operator==(T c)		const
	{
		return field[0] == c && string_length==1;
	}
	
template <typename T>
	bool				StringTemplate<T>::operator!=(T c)		const
	{
		return field[0] != c || string_length!=1;
	}
	

	
#ifdef DSTRING_H
	template <typename T>
		StringTemplate<T>&	StringTemplate<T>::operator=(const AnsiString&string)
		{
			resize(string.Length());
			Template::strncpy(field,string.c_str(),string_length);
			return *this;
		}
#endif
	

template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(const ArrayData<T>&string)
	{
		//ASSERT_NOT_NULL__(field);
		STRING_METHOD_BEGIN("(const ArrayData<T>&string)",string.pointer());
		count_t length = string.length();
		if (length > 0 && string.last() == (T)0)
			length--;
		if (!length)
			resize(0);
		else
		{
			const T	*first = string.pointer(),
					*last = first+length-1;
			while (last >= first && !*last)
				last--;
			resize(last-first+1);
			Template::strncpy(field,string.pointer(),string_length);
		}
		STRING_METHOD_END
		return *this;
	}

#if __STR_RVALUE_REFERENCES__			
template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(StringTemplate<T>&&string)
	{
		adoptData(string);
		return *this;
	}
#endif


template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(const StringTemplate<T>&string)
	{
		//ASSERT_NOT_NULL__(field);
		STRING_METHOD_BEGIN("(const StringTemplate<T>&string)",string.c_str());
		if (&string == this)
		{
			STRING_METHOD_END
			return *this;
		}
		#if __STR_REFERENCE_COUNT__
			delocate(field);
			string_length = string.string_length;
			field = string.field;
			if (field != sz)
				count(field)++;
		#else
			resize(string.string_length);
			memcpy(field,string.field,string_length*sizeof(T));
		#endif
		STRING_METHOD_END
		return *this;
	}
	
template <typename T>
	template <typename T2>
		StringTemplate<T>&	StringTemplate<T>::operator=(const T2*string)
		{
			//ASSERT_NOT_NULL__(field);
			STRING_METHOD_BEGIN("(const T2*string)",string);
			if (string == (const T2*)field)
			{
				STRING_METHOD_END
				return *this;
			}
			resize(Template::strlen(string));
			for (size_t i = 0; i < string_length; i++)
				field[i] = (T)string[i];
			STRING_METHOD_END
			return *this;
		}	
template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(const T*string)
	{
		//ASSERT_NOT_NULL__(field);
		STRING_METHOD_BEGIN("(const T*string)",string);
		if (string == field)
		{
			STRING_METHOD_END
			return *this;
		}
		resize(Template::strlen(string));
		memcpy(field,string,string_length*sizeof(T));
		STRING_METHOD_END
		return *this;
	}

template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(char c)
	{
		//ASSERT_NOT_NULL__(field);
		STRING_METHOD_BEGIN("(char c)",c);
		STRING_DEBUG("resize to 1");
		
		resize(1);
		
		STRING_DEBUG("write char");
		
		field[0] = (T)c;
		
		STRING_DEBUG("done");
		STRING_METHOD_END
		return *this;
	}

template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(wchar_t c)
	{
		//ASSERT_NOT_NULL__(field);
		STRING_METHOD_BEGIN("(wchar_t c)",(char)c);		
		resize(1);
		field[0] = (T)c;
		STRING_METHOD_END
		return *this;
	}



template <typename T>
	template <class Type, class UType>
		void StringTemplate<T>::copySigned(Type value)
		{
			bool negative = false;
			if (value < 0)
			{
				value*=-1;
				negative = true;
			}

			T	char_buffer[256],
				*end = char_buffer+ARRAYSIZE(char_buffer),
				*c = end;
			UType uval = value;
			while (uval && c != char_buffer)
			{
				(*(--c)) = (T)('0'+(uval%10));
				uval/=10;
			}
			if (c==end)
				(*(--c)) = (T)'0';
			resize(end-c+negative);
			if (negative)
				field[0] = '-';
			memcpy(field+negative,c,(end-c)*sizeof(T));
		}

template <typename T>
	template <class Type>
		void StringTemplate<T>::copyUnsigned(Type value)
		{
			T	char_buffer[256],
				*end = char_buffer+ARRAYSIZE(char_buffer),
				*c = end;
			while (value && c != char_buffer)
			{
				(*(--c)) = (T)('0'+(value%10));
				value/=10;
			}
			if (c==end)
				(*(--c)) = (T)'0';
			resize(end-c);
			memcpy(field,c,(end-c)*sizeof(T));
		}
		

template <typename T>
	template <class Type, class UType>
		void StringTemplate<T>::makeSigned(Type value)
		{
			bool negative = false;
			if (value < 0)
			{
				value*=-1;
				negative = true;
			}

			T	char_buffer[256],
				*end = char_buffer+ARRAYSIZE(char_buffer),
				*c = end;
			UType uval = value;
			while (uval && c != char_buffer)
			{
				(*(--c)) = (T)('0'+(uval%10));
				uval/=10;
			}
			if (c==end)
				(*(--c)) = (T)'0';
			string_length = end-c+negative;
			field = allocate(string_length);
			if (negative)
				field[0] = '-';
			memcpy(field+negative,c,(end-c)*sizeof(T));
		}


template <typename T>
	template <class Type>
		void StringTemplate<T>::makeUnsigned(Type value)
		{
			T	char_buffer[256],
				*end = char_buffer+ARRAYSIZE(char_buffer),
				*c = end;
			while (value && c != char_buffer)
			{
				(*(--c)) = (T)('0'+(value%10));
				value/=10;
			}
			if (c==end)
				(*(--c)) = (T)'0';
			string_length = end-c;
			field = allocate(string_length);
			memcpy(field,c,(end-c)*sizeof(T));
		}


template <typename T>
	/*virtual override*/ serial_size_t			StringTemplate<T>::GetSerialSize(bool export_size) const
	{
		return (serial_size_t)(string_length*sizeof(T))+(export_size?GetSerialSizeOfSize((serial_size_t)string_length):0);
	}

template <typename T>
	/*virtual override*/ bool			StringTemplate<T>::Serialize(IWriteStream&out_stream, bool export_size) const
	{
		if (export_size)
		{
			//cout << "encoding string length "<<string_length<<endl;
			if (!out_stream.WriteSize(string_length))
				return false;
		}
		return out_stream.Write(field,(serial_size_t)(string_length*sizeof(T)));
	}

template <typename T>
	/*virtual override*/ bool			StringTemplate<T>::Deserialize(IReadStream&in_stream, serial_size_t fixed_size)
	{
		if (fixed_size != EmbeddedSize)
			setLength((fixed_size/sizeof(T)));
		else
		{
			serial_size_t len;
			if (!in_stream.ReadSize(len))
				return false;
			//cout << "decoded string length "<<len<<endl;
			setLength(len);
		}
		return in_stream.Read(field,(serial_size_t)(string_length*sizeof(T)));
	}



	

template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(short value)
	{
		copySigned<short,unsigned short>(value);
		return *this;
	}

template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(unsigned short value)
	{
		copyUnsigned<unsigned short>(value);
		return *this;
	}



template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(int value)
	{
		copySigned<int,unsigned>(value);
		return *this;
	}

template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(long value)
	{
		copySigned<long,unsigned long>(value);
		return *this;
	}


template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(unsigned value)
	{
		copyUnsigned<unsigned>(value);
		return *this;
	}

template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(unsigned long value)
	{
		copyUnsigned<unsigned long>(value);
		return *this;
	}


template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(long long value)
	{
		copySigned<long long, unsigned long long>(value);
		return *this;
	}

template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(unsigned long long value)
	{
		copyUnsigned<unsigned long long>(value);
		return *this;
	}

template <typename T>
	template <class Type, class UType> 
		T* StringTemplate<T>::signedToStr(Type value, T*end, T*first)
		{
			bool negative = false;
			if (value < 0)
			{
				value*=-1;
				negative = true;
			}

			T *c = end;
			UType uval = value;
			while (uval && c != first)
			{
				(*(--c)) = '0'+(uval%10);
				uval/=10;
			}
			if (c==end)
				(*(--c)) = '0';
			if (negative && c != first)
				(*(--c)) = '-';
			return c;
		}
	
template <typename T>
	template <typename Type>
		T* StringTemplate<T>::floatToStr(Type value, unsigned char exactness, bool force_trailing_zeros, T*end, T*first)
		{
			ASSERT__(end > first);
			if (isinf(value))
			{
				end -= 4;
				if (value > 0)
					(*end) = L'+';
				else
					(*end) = L'-';
				end[1] = L'I';
				end[2] = L'N';
				end[3] = L'F';
				return end;
			}
			
			if (isnan(value))
			{
				end-=3;
				end[0] = L'N';
				end[1] = L'A';
				end[2] = L'N';
				return end;
			}
			bool negative = value < 0;
			if (negative)
				value*=-1;
			
			T*c = end;
			//(*c) = 0; no other possibility writes terminal zeros. so this one does neither
			if (value > INT_MAX)
			{
				int exponent = (int)log10(value);
				value /= pow(10.0,exponent);
				c = signedToStr<int,unsigned>(exponent,c,first);
				if (c != first)
					(*(--c)) = L'e';
			}
				
			double sub_decimal = pow(10.0f,exactness)*value;
			if (sub_decimal > INT_MAX)
			{
				exactness = (BYTE)log10(INT_MAX/value);
				sub_decimal = pow(10.0f,exactness)*value;
			}
			UINT64 v = (UINT64)round(sub_decimal);

			size_t at(0);
			bool write = !exactness || force_trailing_zeros;	//start write trailing zeros immediately
			while (v && c != first)
			{
				T ch = '0'+(v%10);
				write = write || ch != L'0' || at==exactness;
				if (write)
					(*--c) = ch;
				v/=10;
				at++;
				if (at == exactness && c != first && write)
					(*--c) = DecimalSeparator;
			}
			if (c != end)
			{
				while (at < exactness && c != first)
				{
					(*--c) = L'0';
					at++;
				}
				if (at == exactness && c != first)
				{
					if ((*c) != DecimalSeparator)
						(*--c) = DecimalSeparator;
					if (c != first)
						(*--c) = L'0';
				}
				if (negative && c != first)
					(*--c) = L'-';
			}
			else
			{
				if (force_trailing_zeros)
				{
					while (exactness && first != c)
					{
						(*--c) = L'0';
						exactness--;
					}
					if (c != first)
						(*--c) = DecimalSeparator;
					if (c != first)
						(*--c) = L'0';
				}
				else
					(*--c) = L'0';
			}
		//	len = end-c;
			return c;
		}


template <typename T>
	template <typename Float>
		inline void StringTemplate<T>::copyFloat(Float value)
		{
			T	buffer[257],
				*str = floatToStr(value, 5, false,buffer+ARRAYSIZE(buffer)-1, buffer);

			resize(buffer+ARRAYSIZE(buffer)-str-1);
			memcpy(field,str,string_length*sizeof(T));
		}

template <typename T>
	template <typename Float>
		inline void StringTemplate<T>::makeFloat(Float value,unsigned char precision, bool force_trailing_zeros)
		{
			T	buffer[257],
				*str = floatToStr(value, precision, force_trailing_zeros, buffer+ARRAYSIZE(buffer)-1, buffer);

			string_length = buffer+ARRAYSIZE(buffer)-str-1;
			field = allocate(string_length);
			memcpy(field,str,string_length*sizeof(T));
		}
		
template <typename T>
	inline	T*			StringTemplate<T>::writeTo(T*target)	const
						{
							memcpy(target,field,string_length*sizeof(T));
							return target+string_length;
						}
template <typename T>
	template <typename T2>
		inline	T2*		StringTemplate<T>::writeTo(T2*target)	const
						{
							for (size_t i = 0; i < string_length; i++)
								(*target++) = field[i];
							return target;
						}
						
template <typename T>
	inline	T*			StringTemplate<T>::writeTo(T*target, T*end)	const
						{
							size_t cpy = end-target;
							if (string_length < cpy)
								cpy = string_length;
							memcpy(target,field,cpy*sizeof(T));
							return target+cpy;
						}
						
template <typename T>
	template <typename T2>
		inline	T2*		StringTemplate<T>::writeTo(T2*target, T2*end)	const
						{
							for (size_t i = 0; i < string_length && target < end; i++)
								(*target++) = field[i];
							return target;
						}		


template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(double value)
	{
		copyFloat(value);
		return *this;
	}

template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(long double value)
	{
		copyFloat(value);
		return *this;
	}


template <typename T>
	StringTemplate<T>&	StringTemplate<T>::operator=(float value)
	{
		copyFloat(value);
		return *this;
	}

template <typename T>
	template <typename Expression>
		inline void StringTemplate<T>::copyExpression(const Expression&expression)
		{
			if (string_length && expression.references(field))
			{
				size_t len = expression.length();
				T*new_field = allocate(len);
				STRING_DEBUG("writing expression to new field...");
				expression.writeTo(new_field);
				STRING_DEBUG("new field is filled with '"<<new_field<<"' now");
				STRING_DEBUG("discarding old field");
				delocate(field);
				field = new_field;
				string_length = len;
			}
			else
			{
				resize(expression.length());
				expression.writeTo(field);
			}
			//CHECK_STRING(*this);
		}
	
template <typename T>
	template <typename T0, typename T1>
		StringTemplate<T>&	StringTemplate<T>::operator=(const ConcatExpression<T0,T1>&expression)
		{
			copyExpression(expression);
			return *this;
		}
		
template <typename T>
	template <class T0>
		StringTemplate<T>&	StringTemplate<T>::operator=(const ReferenceExpression<T0>&expression)
		{
			copyExpression(expression);
			return *this;
		}
		
template <typename T>
	template <class T0>
		StringTemplate<T>&	StringTemplate<T>::operator=(const StringExpression<T0>&expression)
		{
			copyExpression(expression);
			return *this;
		}

#if __STR_RVALUE_REFERENCES__		
	template <typename T>
		StringTemplate<T>&	StringTemplate<T>::operator=(StringExpression<T>&&expression)
		{
			adoptData(expression);
			return *this;
		}
#endif

#if __STR_EXPRESSION_TMPLATES__
	template <typename T>
		template <typename T0>
			ConcatExpression<StringExpression<T>,ReferenceExpression<T0> >
				StringTemplate<T>::operator+(const T0*string)	const
				{
					return ConcatExpression<StringExpression<T>,ReferenceExpression<T0> >(StringExpression<T>(*this),ReferenceExpression<T0>(string));
				}
			
	template <typename T>
		ConcatExpression<StringExpression<T>,CharacterExpression<T> >
			StringTemplate<T>::operator+(T c)	const
			{
				return ConcatExpression<StringExpression<T>,CharacterExpression<T> >(StringExpression<T>(*this),CharacterExpression<T>(c));
			}
			

	template <typename T>
		template <typename T0, typename T1>
			ConcatExpression<StringExpression<T>,ConcatExpression<T0,T1> >
				StringTemplate<T>::operator+(const ConcatExpression<T0,T1>&expression)	const
				{
					return ConcatExpression<StringExpression<T>,ConcatExpression<T0,T1> >(StringExpression<T>(*this),expression);
				}
					
	template <typename T>
		template <class T0>
			ConcatExpression<StringExpression<T>,ReferenceExpression<T0> >
				StringTemplate<T>::operator+(const ReferenceExpression<T0>&expression)	const
				{
					return ConcatExpression<StringExpression<T>,ReferenceExpression<T0> >(StringExpression<T>(*this),expression);
				}
				
	template <typename T>
		template <class T0>
			ConcatExpression<StringExpression<T>,StringExpression<T0> >
				StringTemplate<T>::operator+(const StringExpression<T0>&expression)	const
				{
					return ConcatExpression<StringExpression<T>,StringExpression<T0> >(StringExpression<T>(*this),expression);
				}


	template <typename T0, typename T1>
		ConcatExpression<ReferenceExpression<T0>,StringExpression<T1> > operator+(const T0*c, const StringTemplate<T1>& string)
		{
			return ConcatExpression<ReferenceExpression<T0>,StringExpression<T1> >(ReferenceExpression<T0>(c),StringExpression<T1>(string));
		}

	template <typename T>
		ConcatExpression<CharacterExpression<char>,StringExpression<T> > operator+(char c, const StringTemplate<T>& string)
		{
			return ConcatExpression<CharacterExpression<char>,StringExpression<T> >(CharacterExpression<char>(c),StringExpression<T>(string));
		}


	template <typename T>
		ConcatExpression<CharacterExpression<wchar_t>,StringExpression<T> > operator+(wchar_t c, const StringTemplate<T>& string)
		{
			return ConcatExpression<CharacterExpression<wchar_t>,StringExpression<T> >(CharacterExpression<wchar_t>(c),StringExpression<T>(string));
		}
#else
	template <typename T>
		StringTemplate<T>
			StringTemplate<T>::operator+(const T*string)	const
			{
				size_t len = Template::strlen(string);
				if (!len)
					return *this;
				StringTemplate<T> result = StringTemplate<T>(TStringLength(string_length+(size_t)len));
				memcpy(result.field,field,string_length*sizeof(T));
				memcpy(result.field+string_length,string,len*sizeof(T));
				return result;
			}
	
	template <typename T>
		template <typename T1>
			StringTemplate<T>
				StringTemplate<T>::operator+(const T1*string)	const
				{
					size_t len = Template::strlen(string);
					if (!len)
						return *this;
					StringTemplate<T> result = StringTemplate<T>(TStringLength(string_length+len));
					T*out = result.field;
					for (size_t i = 0; i < string_length; i++)
						(*out++) = (T)field[i];
					for (size_t i = 0; i < len; i++)
						(*out++) = (T)string[i];
					return result;
				}
			
	template <typename T>
		StringTemplate<T>
			StringTemplate<T>::operator+(T c)	const
			{
				StringTemplate<T> result = StringTemplate<T>(TStringLength(string_length+1));
				memcpy(result.field,field,string_length*sizeof(T));
				result.field[string_length] = c;
				return result;
			}
			

	template <typename T>
		template <typename T0, typename T1>
			StringTemplate<T>
				StringTemplate<T>::operator+(const ConcatExpression<T0,T1>&expression)	const
				{
					size_t len = expression.length();
					if (!len)
						return *this;
					StringTemplate<T> result = StringTemplate<T>(TStringLength(string_length+len));
					memcpy(result.field,field,string_length*sizeof(T));
					expression.writeTo(result.field+string_length);
					return result;
				}
					
	template <typename T>
		template <class T0>
			StringTemplate<T>
				StringTemplate<T>::operator+(const ReferenceExpression<T0>&expression)	const
				{
					size_t len = expression.length();
					if (!len)
						return *this;
					StringTemplate<T> result = StringTemplate<T>(TStringLength(string_length+len));
					memcpy(result.field,field,string_length*sizeof(T));
					expression.writeTo(result.field+string_length);
					return result;
				}
				
	template <typename T>
		template <class T0>
			StringTemplate<T>
				StringTemplate<T>::operator+(const StringExpression<T0>&expression)	const
				{
					size_t len = expression.length();
					if (!len)
						return *this;
					StringTemplate<T> result = StringTemplate<T>(TStringLength(string_length+len));
					memcpy(result.field,field,string_length*sizeof(T));
					expression.writeTo(result.field+string_length);
					return result;
				}
				
	template <typename T>
		template <class T0>
			StringTemplate<T>
				StringTemplate<T>::operator+(const StringTemplate<T0>&expression)	const
				{
					size_t len = expression.length();
					if (!len)
						return *this;
					StringTemplate<T> result = StringTemplate<T>(TStringLength(string_length+len));
					memcpy(result.field,field,string_length*sizeof(T));
					expression.writeTo(result.field+string_length);
					return result;
				}


	template <typename T0, typename T1>
		StringTemplate<T1> inline operator+(const T0*c, const StringTemplate<T1>& string)
		{
			size_t len = Template::strlen(c);
			if (!len)
				return string;
			StringTemplate<T1> result = StringTemplate<T1>(TStringLength(string.string_length+len));
			T1*out = result.field;
			for (size_t i = 0; i < len; i++)
				(*out++) = c[i];
			for (size_t i = 0; i < string.string_length; i++)
				(*out++) = string.field[i];
			return result;
		}
/*		
	template <>
		StringTemplate<char> inline operator+(const char*c, const StringTemplate<char>& string)
		{
			size_t len = Template::strlen(c);
			if (!len)
				return string;
			StringTemplate<char> result = StringTemplate<char>(TStringLength(string.string_length+len));
			memcpy(result.field,c,len);
			memcpy(result.field+len,string.field,string.string_length);
			return result;
		}
		
	template <>
		StringTemplate<wchar_t> inline operator+(const wchar_t*c, const StringTemplate<wchar_t>& string)
		{
			size_t len = Template::strlen(c);
			if (!len)
				return string;
			StringTemplate<wchar_t> result = StringTemplate<wchar_t>(TStringLength(string.string_length+len));
			memcpy(result.field,c,len*sizeof(wchar_t));
			memcpy(result.field+len,string.field,string.string_length*sizeof(wchar_t));
			return result;
		}
*/
	template <typename T>
		StringTemplate<T> inline operator+(char c, const StringTemplate<T>& string)
		{
			StringTemplate<T> result = StringTemplate<T>(TStringLength(string.string_length+1));
			memcpy(result.field+1,string.field,string.string_length*sizeof(T));
			result.field[0] = (T)c;
			return result;
		}


	template <typename T>
		StringTemplate<T> inline operator+(wchar_t c, const StringTemplate<T>& string)
		{
			StringTemplate<T> result = StringTemplate<T>(TStringLength(string.string_length+1));
			memcpy(result.field+1,string.field,string.string_length*sizeof(T));
			result.field[0] = (T)c;
			return result;
		}





#endif

template <typename T0, typename T1, typename T2>
	void	explode(const T0*delimiter, size_t delimiter_length, const T1*string, ArrayData<T2>&result)
	{
		if (!string)
			return;
		explode(delimiter, delimiter_length,ReferenceExpression<T1>(string,Template::strlen(string)),result);
	}

	
template <typename T0, typename T1, typename T2>
	void	explode(const T0*delimiter, size_t delimiter_length, const ReferenceExpression<T1>&string, ArrayData<T2>&result)
	{
		if (!delimiter)
			return;
		if (!string.length())
		{
			result.SetSize(1);
			result[0] = string;
			return;
		}
		const T0*const delimiterEnd = delimiter+delimiter_length;
		
		//ArrayData<T1>	buffer(string);
		const T1*at = string.pointer();
		const T1*const end = at+string.length();
		size_t count = 0;
		while (at != end)
		{
			count ++;
			if (const T1*found = Template::strstr<T1,T0>(at,end,delimiter,delimiterEnd))
			{
				at = found + delimiter_length;
			}
			else
				break;
		}
		result.SetSize(count);
		at = string.pointer();
		count = 0;
		while (at != end)
		{
			if (const T1*found = Template::strstr<T1,T0>(at,end,delimiter,delimiterEnd))
			{
				result[count++] = T2(at,found-at);
				/*(*found) = 0;
				result[count++] = at;*/
				at = found + delimiter_length;
			}
			else
			{
				result[count++] = T2(at,end-at);
				return;
			}
		}
	}

template <typename T0, typename T1, typename T2>
	void	explode(const T0*delimiter, size_t delimiter_length, const StringTemplate<T1>&string, ArrayData<T2>&result)
	{
		explode(delimiter,delimiter_length,string.ref(),result);
	}





template <typename T0, typename T1, typename T2>
	void	explode(const StringTemplate<T0>&delimiter, const StringTemplate<T1>&string, ArrayData<T2>&result)
	{
		explode(delimiter.c_str(),delimiter.length(),string,result);
	}


template <typename T0, typename T1, typename T2>
	void	explode(const T0*delimiter, const StringTemplate<T1>&string, ArrayData<T2>&result)
	{
		explode(delimiter,Template::strlen(delimiter),string,result);
	}


template <typename T0, typename T1, typename T2>
	void	explode(const T0*delimiter, const T1*string, ArrayData<T2>&result)
	{
		explode(delimiter,Template::strlen(delimiter),string,result);
	}

template <typename T0, typename T1>
	void	explode(T0 delimiter, const T0*string, ArrayData<T1>&result)
	{
		if (!string)
			return;
		explode(delimiter, ReferenceExpression<T0>(string,Template::strlen(string)),result);
	}

template <typename T0, typename T1>
	void	explode(T0 delimiter, const ReferenceExpression<T0>&string, ArrayData<T1>&result)
	{
		//ArrayData<T0>	buffer(string);
		const T0*at = string.pointer();
		const T0*const end = at + string.length();
		size_t count = 0;
		while (at != end)
		{
			count ++;
			if (const T0*found = Template::strchr(at,end,delimiter))
			{
				at = found +1;
			}
			else
				break;
		}
		result.SetSize(count);
		at = string.pointer();
		count = 0;
		while (at != end)
		{
			if (const T0*found = Template::strchr(at,end,delimiter))
			{
				/*(*found) = 0;
				result[count++] = at;*/
				result[count++] = T1(at,found-at);
				at = found + 1;
			}
			else
			{
				result[count++] = T1(at,end-at);
				return;
			}
		}
	}

template <typename T0, typename T1>
	void	explode(T0 delimiter, const StringTemplate<T0>&string, ArrayData<T1>&result)
	{
		explode(delimiter,string.ref(),result);
	}

template <typename T0, typename T1>
	void	explodeCallback(bool isDelimiter(T0), const T0*string, ArrayData<T1>&result)
	{
		if (!string)
			return;
		explodeCallback(isDelimiter, ReferenceExpression<T0>(string,Template::strlen(string)),result);
	}

template <typename T0, typename T1>
	void	explodeCallback(bool isDelimiter(T0), const ReferenceExpression<T0>&string, ArrayData<T1>&result)
	{
		//ArrayData<T0>	buffer(string);
		const T0*at = string.pointer();
		const T0*const end = at + string.length();
		size_t count = 0;
		while (at!=end)
		{
			count ++;
			if (const T0*found = Template::strchr(at,end,isDelimiter))
			{
				at = found +1;
			}
			else
				break;
		}
		result.SetSize(count);
		at = string.pointer();
		count = 0;
		while (*at)
		{
			if (const T0*found = Template::strchr(at,end,isDelimiter))
			{
				/*(*found) = 0;
				result[count++] = at;*/
				result[count++] = T1(at,found-at);
				at = found + 1;
			}
			else
			{
				result[count++] = T1(at,end-at);
				return;
			}
		}
	}

template <typename T0, typename T1>
	void	explodeCallback(bool isDelimiter(T0), const StringTemplate<T0>&string, ArrayData<T1>&result)
	{
		explodeCallback(isDelimiter,string.ref(),result);
	}




	



template <typename T0, typename T1>
	StringTemplate<T1>		implode(const StringTemplate<T0>&glue, const ReferenceExpression<T1>*pieces, count_t numPieces)
	{
		if (!numPieces)
			return "";
		size_t len = 0;
		for (size_t i = 0; i < numPieces; i++)
			len += pieces[i].length();
		const T0*glue_str = glue.c_str();
		size_t glue_len = glue.length();
		len += (numPieces-1)*glue_len;
		StringTemplate<T1> result = TStringLength(len);
		T1*out = result.mutablePointer();
		for (size_t i = 0; i < numPieces-1;i++)
		{
			Template::strncpy(out,pieces[i].pointer(),pieces[i].length());
			out+=pieces[i].length();
			Template::strncpy(out,glue_str,glue_len);
			out+=glue_len;
		}
		Template::strncpy(out,pieces[numPieces-1].pointer(),pieces[numPieces-1].length());
		//out+=pieces[numPieces-1].length();
		//(*out) = 0;
		//ASSERT_EQUAL__(out, (result.c_str()+result.length()));
		return result;
	}

template <typename T>
	StringTemplate<T>		implode(T glue, const ReferenceExpression<T>*pieces, count_t numPieces)
	{
		if (!numPieces)
			return "";
		size_t len = 0;
		for (size_t i = 0; i < numPieces; i++)
			len += pieces[i].length();
		len += (numPieces-1);
		StringTemplate<T> result = TStringLength(len);
		T*out = result.mutablePointer();
		for (size_t i = 0; i < numPieces-1;i++)
		{
			Template::strncpy(out,pieces[i].pointer(),pieces[i].length());
			out+=pieces[i].length();
			(*out++) = glue;
		}
		Template::strncpy(out,pieces[numPieces-1].pointer(),pieces[numPieces-1].length());
		//out+=pieces.last().length();
		//(*out) = 0;
		//ASSERT_EQUAL__(out, (result.c_str()+result.length()));
		return result;
	}
	
template <typename T0, typename T1>
	StringTemplate<T1>		implode(const T0*glue_str, const ReferenceExpression<T1>*pieces, count_t numPieces)
	{
		if (!numPieces)
			return "";
		size_t len = 0;
		for (size_t i = 0; i < numPieces; i++)
			len += pieces[i].length();
		size_t glue_len = Template::strlen(glue_str);
		len += (numPieces-1)*glue_len;
		StringTemplate<T1> result = TStringLength(len);
		T1*out = result.mutablePointer();
		for (size_t i = 0; i < numPieces-1;i++)
		{
			Template::strncpy(out,pieces[i].pointer(),pieces[i].length());
			out+=pieces[i].length();
			Template::strncpy(out,glue_str,glue_len);
			out+=glue_len;
		}
		Template::strncpy(out,pieces[numPieces-1].pointer(),pieces[numPieces-1].length());
		//out+=pieces.last().length();
		//(*out) = 0;
		//ASSERT_EQUAL__(out, (result.c_str()+result.length()));
		return result;
	}




template <typename T0, typename T1>
	StringTemplate<T1>		implode(const StringTemplate<T0>&glue, const StringTemplate<T1>*pieces, count_t numPieces)
	{
		if (!numPieces)
			return "";
		size_t len = 0;
		for (size_t i = 0; i < numPieces; i++)
			len += pieces[i].length();
		const T0*glue_str = glue.c_str();
		size_t glue_len = glue.length();
		len += (numPieces-1)*glue_len;
		StringTemplate<T1> result = TStringLength(len);
		T1*out = result.mutablePointer();
		for (size_t i = 0; i < numPieces-1;i++)
		{
			Template::strncpy(out,pieces[i].c_str(),pieces[i].length());
			out+=pieces[i].length();
			Template::strncpy(out,glue_str,glue_len);
			out+=glue_len;
		}
		Template::strncpy(out,pieces[numPieces-1].c_str(),pieces[numPieces-1].length());
		//out+=pieces[numPieces-1].length();
		//(*out) = 0;
		//ASSERT_EQUAL__(out, (result.c_str()+result.length()));
		return result;
	}

template <typename T>
	StringTemplate<T>		implode(T glue, const StringTemplate<T>*pieces, count_t numPieces)
	{
		if (!numPieces)
			return "";
		size_t len = 0;
		for (size_t i = 0; i < numPieces; i++)
			len += pieces[i].length();
		len += (numPieces-1);
		StringTemplate<T> result = TStringLength(len);
		T*out = result.mutablePointer();
		for (size_t i = 0; i < numPieces-1;i++)
		{
			Template::strncpy(out,pieces[i].c_str(),pieces[i].length());
			out+=pieces[i].length();
			(*out++) = glue;
		}
		Template::strncpy(out,pieces[numPieces-1].c_str(),pieces[numPieces-1].length());
		//out+=pieces.last().length();
		//(*out) = 0;
		//ASSERT_EQUAL__(out, (result.c_str()+result.length()));
		return result;
	}
	
template <typename T0, typename T1>
	StringTemplate<T1>		implode(const T0*glue_str, const StringTemplate<T1>*pieces, count_t numPieces)
	{
		if (!numPieces)
			return "";
		size_t len = 0;
		for (size_t i = 0; i < numPieces; i++)
			len += pieces[i].length();
		size_t glue_len = Template::strlen(glue_str);
		len += (numPieces-1)*glue_len;
		StringTemplate<T1> result = TStringLength(len);
		T1*out = result.mutablePointer();
		for (size_t i = 0; i < numPieces-1;i++)
		{
			Template::strncpy(out,pieces[i].c_str(),pieces[i].length());
			out+=pieces[i].length();
			Template::strncpy(out,glue_str,glue_len);
			out+=glue_len;
		}
		Template::strncpy(out,pieces[numPieces-1].c_str(),pieces[numPieces-1].length());
		//out+=pieces.last().length();
		//(*out) = 0;
		//ASSERT_EQUAL__(out, (result.c_str()+result.length()));
		return result;
	}

template <typename T0, typename T1>
	StringTemplate<T1>		implode(const StringTemplate<T0>&glue, const ArrayRef<StringTemplate<T1> >&pieces)
	{
		if (!pieces.count())
			return "";
		size_t len = 0;
		for (size_t i = 0; i < pieces.count(); i++)
			len += pieces[i].length();
		const T0*glue_str = glue.c_str();
		size_t glue_len = glue.length();
		len += (pieces.count()-1)*glue_len;
		StringTemplate<T1> result = TStringLength(len);
		T1*out = result.mutablePointer();
		for (size_t i = 0; i < pieces.count()-1;i++)
		{
			Template::strncpy(out,pieces[i].c_str(),pieces[i].length());
			out+=pieces[i].length();
			Template::strncpy(out,glue_str,glue_len);
			out+=glue_len;
		}
		Template::strncpy(out,pieces.last().c_str(),pieces.last().length());
		//out+=pieces.last().length();
		//(*out) = 0;
		//ASSERT_EQUAL__(out, (result.c_str()+result.length()));
		return result;
	}

template <typename T>
	StringTemplate<T>		implode(T glue, const ArrayRef<StringTemplate<T> >&pieces)
	{
		if (!pieces.count())
			return "";
		size_t len = 0;
		for (size_t i = 0; i < pieces.count(); i++)
			len += pieces[i].length();
		len += (pieces.count()-1);
		StringTemplate<T> result = TStringLength(len);
		T*out = result.mutablePointer();
		for (size_t i = 0; i < pieces.count()-1;i++)
		{
			Template::strncpy(out,pieces[i].c_str(),pieces[i].length());
			out+=pieces[i].length();
			(*out++) = glue;
		}
		Template::strncpy(out,pieces.last().c_str(),pieces.last().length());
		//out+=pieces.last().length();
		//(*out) = 0;
		//ASSERT_EQUAL__(out, (result.c_str()+result.length()));
		return result;
	}
	
template <typename T0, typename T1>
	StringTemplate<T1>		implode(const T0*glue_str, const ArrayRef<StringTemplate<T1> >&pieces)
	{
		if (!pieces.count())
			return "";
		size_t len = 0;
		for (size_t i = 0; i < pieces.count(); i++)
			len += pieces[i].length();
		size_t glue_len = Template::strlen(glue_str);
		len += (pieces.count()-1)*glue_len;
		StringTemplate<T1> result = TStringLength(len);
		T1*out = result.mutablePointer();
		for (size_t i = 0; i < pieces.count()-1;i++)
		{
			Template::strncpy(out,pieces[i].c_str(),pieces[i].length());
			out+=pieces[i].length();
			Template::strncpy(out,glue_str,glue_len);
			out+=glue_len;
		}
		Template::strncpy(out,pieces.last().c_str(),pieces.last().length());
		//out+=pieces.last().length();
		//(*out) = 0;
		//ASSERT_EQUAL__(out, (result.c_str()+result.length()));
		return result;
	}
	
	

template <typename T0, typename T1, typename T2>
	static void RunTWrap(const T0*string, T1 max_line_length, T1(*lengthFunction)(T0 character),const T2&callback)
	{
		T1			line_width = 0,
					word_width = 0;
		const T0	*at = string,
					*word_begin=string,
					*line_begin=string;
		bool word_ended = false;
		bool forceNewline = false;
		//cout << "wrapping '"<<string<<"'"<<endl;
		while (*at)
		{
			T1 len = lengthFunction(*at);
			line_width += len;
			word_width += len;
			if ((forceNewline || line_width >= max_line_length) && word_begin < at)
			{
				if (line_width < max_line_length)
				{
					word_begin = at-1;
					word_width = len;
				}
				//cout << "passed length barrier of "<<max_line_length<<" at "<<(at-string)<<endl;
				if (word_begin == line_begin)
				{
					//ASSERT_EQUAL__(word_width,line_width);
					T1 current = 0;
					for(;;)
					{
						ASSERT_LESS_OR_EQUAL__(word_begin,at);
						T1 chrWidth = lengthFunction(*word_begin);
						if (word_begin < at && (current == 0 || current + chrWidth <= max_line_length))
						{
							word_begin++;
							current += chrWidth;
							word_width -= chrWidth;
						}
						else
							break;
					}
				}
				callback(ReferenceExpression<T0>(line_begin,word_begin-line_begin));
				if (line_width < max_line_length)
				{
					word_begin = at;
					word_width = 0;
				}
				line_begin = word_begin;
				line_width = word_width;
				//length = len;
				forceNewline = false;
			}
		
		
			if (isWhitespace(*at))
			{
				if (isNewline(*at))
					forceNewline = true;
				//*at = ' ';
				if (line_begin == at && !forceNewline)
				{
					line_begin++;
					word_begin++;
				}
				word_ended = true;
			}
			else
				if (word_ended)
				{
					word_ended = false;
					word_begin = at;
					word_width = 0;
				}
			at++;
		}
		if (at > word_begin)
		{
			callback(ReferenceExpression<T0>(line_begin, at - line_begin));
		}
	}
	
template <typename T0, typename T1, typename T2>
	static void twrap(const T0*string, T1 max_line_length, T1 (*lengthFunction)(T0 character),ArrayData<StringTemplate<T2> >&result)
	{
		size_t lines=0;
		RunTWrap(string,max_line_length,lengthFunction,[&lines](const ReferenceExpression<T0>&)	{lines++;});

		
		result.SetSize(lines);
		index_t at = 0;
		RunTWrap(string,max_line_length,lengthFunction,[&result,&at](const ReferenceExpression<T0>&exp)
		{
			result[at] = StringTemplate<T2>(exp);
			result[at].replaceCharacters(isWhitespace<T2>,(T2)' ');
			at++;
		});
		ASSERT_EQUAL__(at,lines);
			
	}	
	
	

template <typename T>
	static size_t	constantLengthFunction(T)
	{
		return 1;
	}
	
template <typename T0, typename T1>
	void 	wrap(const T0*string, size_t line_length, ArrayData<StringTemplate<T1> >&result)
	{
		twrap<T0,size_t,T1>(string,line_length,constantLengthFunction<T0>,result);
	}

template <typename T0, typename T1>
	void 	wrap(const T0*string, size_t line_length, size_t (*lengthFunction)(T0),ArrayData<StringTemplate<T1> >&result)
	{
		twrap<T0, size_t, T1>(string,line_length,lengthFunction,result);
	}

template <typename T0, typename T1>
	void 	wrapf(const T0*string, float line_length, float (*lengthFunction)(T0),ArrayData<StringTemplate<T1> >&result)
	{
		twrap<T0, float, T1>(string,line_length,lengthFunction,result);
	}
	
	
template <typename T0, typename T1>
	void 	wrap(const StringTemplate<T0>&string, size_t line_length, ArrayData<StringTemplate<T1> >&result)
	{
		twrap<T0,size_t,T1>(string.c_str(),line_length,constantLengthFunction<T0>,result);
	}

template <typename T0, typename T1>
	void 	wrap(const StringTemplate<T0>&string, size_t line_length, size_t (*lengthFunction)(T0),ArrayData<StringTemplate<T1> >&result)
	{
		twrap<T0, size_t, T1>(string.c_str(),line_length,lengthFunction,result);
	}

template <typename T0, typename T1>
	void 	wrapf(const StringTemplate<T0>&string, float line_length, float (*lengthFunction)(T0),ArrayData<StringTemplate<T1> >&result)
	{
		twrap<T0, float, T1>(string.c_str(),line_length,lengthFunction,result);
	}
	
	

	
template <typename T>
	inline	StringTemplate<char>		ReferenceExpression<T>::ToString()			const
						{
							return StringTemplate<char>("Reference<")+StringTemplate<char>(len)+">";
						}
						
template <>
	inline	void		ReferenceExpression<char>::print(std::ostream&stream)	const
						{
							stream.write(reference,len);
						}
						
template <typename T>
	inline	void		ReferenceExpression<T>::print(std::ostream&stream)	const
						{
							for (size_t i = 0; i < len; i++)
								stream << (char)reference[i];
						}

#ifdef WCOUT

	template <>
		inline	void		ReferenceExpression<wchar_t>::print(std::wostream&stream)	const
							{
								stream.write(reference,len);
							}

							
	template <typename T>
		inline	void		ReferenceExpression<T>::print(std::wostream&stream)	const
							{
								for (size_t i = 0; i < len; i++)
									stream << (wchar_t)reference[i];
							}

#endif						

template <typename E0, typename E1>
	inline	StringTemplate<char>		ConcatExpression<E0,E1>::ToString()			const
						{
							return StringTemplate<char>("Concat<")+exp0.ToString()+", "+exp1.ToString()+">";
						}
						
						
	



template <typename T>
	inline	StringTemplate<char>		StringExpression<T>::ToString()			const
						{
							return StringTemplate<char>("Temporary<")+StringTemplate<char>(StringTemplate<T>::string_length)+">";
						}
						
template <typename T>
	template <class Stream>
		inline	void		StringExpression<T>::print(Stream&stream)			const
							{
								stream << StringTemplate<T>::c_str();
							}						

template <typename T>
	template <typename T2>
		inline	int		StringExpression<T>::compareSegment(const T2*string, size_t string_length_)	const
		{
			int result = Template::strncmp(StringTemplate<T>::field,string,string_length_<StringTemplate<T>::string_length?string_length_:StringTemplate<T>::string_length);
			if (result)
				return result;
			if (string_length_ < StringTemplate<T>::string_length)
				return 1;
			return 0;
		}
		
template <typename T>
	template <typename T2>
		inline	int		StringExpression<T>::compareTo(const T2*string, size_t string_length_)	const
		{
			int result = compareSegment(string,string_length_);
			if (!result && string_length_ > StringTemplate<T>::string_length)
				result = -1;
			return result;
		}

template <typename T>
	template <typename T2>
		inline	int		StringExpression<T>::compareSegmentIgnoreCase(const T2*string, size_t string_length_)	const
		{
			int result = Template::strncmpi(StringTemplate<T>::field,string,string_length_<StringTemplate<T>::string_length?string_length_:StringTemplate<T>::string_length);
			if (result)
				return result;
			if (string_length_ < StringTemplate<T>::string_length)
				return 1;
			return 0;
		}
		
template <typename T>
	template <typename T2>
		inline	int		StringExpression<T>::compareToIgnoreCase(const T2*string, size_t string_length_)	const
		{
			int result = compareSegmentIgnoreCase(string,string_length_);
			if (!result && string_length_ > StringTemplate<T>::string_length)
				result = -1;
			return result;
		}
						

template <typename T0, typename T1>
	template <typename T2>
		inline ConcatExpression<ConcatExpression<T0,T1>,StringExpression<T2> >
						ConcatExpression<T0,T1>::operator+(const StringExpression<T2>&expression)	const
						{
							return ConcatExpression<ConcatExpression<T0,T1>,StringExpression<T2> >(*this,expression);
						}



template <typename T>
	template <typename T2>
		inline ConcatExpression<ReferenceExpression<T>,ReferenceExpression<T2> >
						ReferenceExpression<T>::operator+(const ReferenceExpression<T2>&expression)	const
						{
							return ConcatExpression<ReferenceExpression<T>,ReferenceExpression<T2> >(*this,expression);
						}
template <typename T>
	template <typename T2>
		inline ConcatExpression<ReferenceExpression<T>,StringExpression<T2> >
						ReferenceExpression<T>::operator+(const StringExpression<T2>&expression)	const
						{
							return ConcatExpression<ReferenceExpression<T>,StringExpression<T2> >(*this,expression);
						}
template <typename T>
	template <typename T2, typename T3>
		inline ConcatExpression<ReferenceExpression<T>,ConcatExpression<T2,T3> >
						ReferenceExpression<T>::operator+(const ConcatExpression<T2,T3>&expression)	const
						{
							return ConcatExpression<ReferenceExpression<T>,ConcatExpression<T2,T3> >(*this,expression);
						}
template <typename T>
	template <typename T2>
		inline ConcatExpression<ReferenceExpression<T>,CharacterExpression<T2> >
						ReferenceExpression<T>::operator+(const CharacterExpression<T2>&expression)	const
						{
							return ConcatExpression<ReferenceExpression<T>,CharacterExpression<T2> >(*this,expression);
						}		


template <typename T>
	template <typename T2>
		inline ConcatExpression<StringExpression<T>,ReferenceExpression<T2> >
						StringExpression<T>::operator+(const ReferenceExpression<T2>&expression)	const
						{
							return ConcatExpression<StringExpression<T>,ReferenceExpression<T2> >(*this,expression);
						}
template <typename T>
	template <typename T2>
		inline ConcatExpression<StringExpression<T>,CharacterExpression<T2> >
						StringExpression<T>::operator+(const CharacterExpression<T2>&expression)	const
						{
							return ConcatExpression<StringExpression<T>,CharacterExpression<T2> >(*this,expression);
						}
template <typename T>
	template <typename T2>
		inline ConcatExpression<StringExpression<T>,StringExpression<T2> >
						StringExpression<T>::operator+(const StringExpression<T2>&expression)	const
						{
							return ConcatExpression<StringExpression<T>,StringExpression<T2> >(*this,expression);
						}
template <typename T>
	template <typename T2, typename T3>
		inline ConcatExpression<StringExpression<T>,ConcatExpression<T2,T3> >
						StringExpression<T>::operator+(const ConcatExpression<T2,T3>&expression)	const
						{
							return ConcatExpression<StringExpression<T>,ConcatExpression<T2,T3> >(*this,expression);
						}		


	

template <typename T0, typename T1>
	inline ConcatExpression<ReferenceExpression<T0>,ReferenceExpression<T1> > operator+(const T0*a, const ReferenceExpression<T1>&b)
	{
		return ConcatExpression<ReferenceExpression<T0>,ReferenceExpression<T1> >(a,b);
	}
	
template <typename T>
	inline ConcatExpression<CharacterExpression<char>,ReferenceExpression<T> > operator+(char a, const ReferenceExpression<T>&b)
	{
		return ConcatExpression<CharacterExpression<char>,ReferenceExpression<T> >(a,b);
	}
	
template <typename T>
	inline ConcatExpression<CharacterExpression<wchar_t>,ReferenceExpression<T> > operator+(wchar_t a, const ReferenceExpression<T>&b)
	{
		return ConcatExpression<CharacterExpression<wchar_t>,ReferenceExpression<T> >(a,b);
	}
	
template <typename T0, typename T1>
	inline ConcatExpression<ReferenceExpression<T0>,CharacterExpression<T1> > operator+(const T0*a, const CharacterExpression<T1>&b)
	{
		return ConcatExpression<ReferenceExpression<T0>,CharacterExpression<T1> >(a,b);
	}
	
template <typename T>
	inline ConcatExpression<CharacterExpression<char>,CharacterExpression<T> > operator+(char a, const CharacterExpression<T>&b)
	{
		return ConcatExpression<CharacterExpression<char>,CharacterExpression<T> >(a,b);
	}
	
template <typename T>
	inline ConcatExpression<CharacterExpression<wchar_t>,CharacterExpression<T> > operator+(wchar_t a, const CharacterExpression<T>&b)
	{
		return ConcatExpression<CharacterExpression<wchar_t>,CharacterExpression<T> >(a,b);
	}
	

template <typename T0, typename T1, typename T2>
	inline ConcatExpression<ReferenceExpression<T0>,ConcatExpression<T1, T2> > operator+(const T0*a, const ConcatExpression<T1, T2>&b)
	{
		return ConcatExpression<ReferenceExpression<T0>,ConcatExpression<T1, T2> >(a,b);
	}
	
template <typename T0, typename T1>
	inline ConcatExpression<CharacterExpression<char>,ConcatExpression<T0, T1> > operator+(char a, const ConcatExpression<T0, T1>&b)
	{
		return ConcatExpression<CharacterExpression<char>,ConcatExpression<T0, T1> >(a,b);
	}
	
template <typename T0, typename T1>
	inline ConcatExpression<CharacterExpression<wchar_t>,ConcatExpression<T0, T1> > operator+(wchar_t a, const ConcatExpression<T0, T1>&b)
	{
		return ConcatExpression<CharacterExpression<wchar_t>,ConcatExpression<T0, T1> >(a,b);
	}
	
template <typename T0, typename T1>
	inline ConcatExpression<ReferenceExpression<T0>,StringExpression<T1> > operator+(const T0*a, const StringExpression<T1>&b)
	{
		return ConcatExpression<ReferenceExpression<T0>,StringExpression<T1> >(a,b);
	}
	
template <typename T>
	inline ConcatExpression<CharacterExpression<char>,StringExpression<T> > operator+(char a, const StringExpression<T>&b)
	{
		return ConcatExpression<CharacterExpression<char>,StringExpression<T> >(a,b);
	}
	
template <typename T>
	inline ConcatExpression<CharacterExpression<wchar_t>,StringExpression<T> > operator+(wchar_t a, const StringExpression<T>&b)
	{
		return ConcatExpression<CharacterExpression<wchar_t>,StringExpression<T> >(a,b);
	}


template <typename E0, typename E1>
	template <typename T2>
		bool			ConcatExpression<E0, E1>::operator<(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())<0;
		}
		
template <typename E0, typename E1>
	template <typename T2>
		bool			ConcatExpression<E0, E1>::operator<=(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())<=0;
		}
		
template <typename E0, typename E1>
	template <typename T2>
		bool			ConcatExpression<E0, E1>::operator>(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())>0;
		}
		
template <typename E0, typename E1>
	template <typename T2>
		bool			ConcatExpression<E0, E1>::operator>=(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())>=0;
		}
		
template <typename E0, typename E1>
	template <typename T2>
		bool			ConcatExpression<E0, E1>::operator!=(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())!=0;
		}
	
		
template <typename E0, typename E1>
	template <typename T2>
		bool			ConcatExpression<E0, E1>::operator==(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())==0;
		}

template <typename T>
	template <typename T2>
		bool			ReferenceExpression<T>::operator<(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())<0;
		}
		
template <typename T>
	template <typename T2>
		bool			ReferenceExpression<T>::operator<=(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())<=0;
		}
		
template <typename T>
	template <typename T2>
		bool			ReferenceExpression<T>::operator>(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())>0;
		}
		
template <typename T>
	template <typename T2>
		bool			ReferenceExpression<T>::operator>=(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())>=0;
		}
		
template <typename T>
	template <typename T2>
		bool			ReferenceExpression<T>::operator!=(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())!=0;
		}
	
		
template <typename T>
	template <typename T2>
		bool			ReferenceExpression<T>::operator==(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())==0;
		}

	
template <typename T>
	template <typename T2>
		bool			CharacterExpression<T>::operator<(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())<0;
		}
		
template <typename T>
	template <typename T2>
		bool			CharacterExpression<T>::operator<=(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())<=0;
		}
		
template <typename T>
	template <typename T2>
		bool			CharacterExpression<T>::operator>(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())>0;
		}
		
template <typename T>
	template <typename T2>
		bool			CharacterExpression<T>::operator>=(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())>=0;
		}
		
template <typename T>
	template <typename T2>
		bool			CharacterExpression<T>::operator!=(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())!=0;
		}
	
		
template <typename T>
	template <typename T2>
		bool			CharacterExpression<T>::operator==(const StringTemplate<T2>&other)	const
		{
			return compareTo(other.c_str(),other.length())==0;
		}

	

#define DEFINE_COMPARE_OPERATORS(_OPERATOR_,_REVERSE_OPERATOR_)\
	template <typename T0, typename T1>\
		inline bool operator _OPERATOR_(const T0*a, const StringTemplate<T1>&b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T>\
		inline bool operator _OPERATOR_(char a, const StringTemplate<T>& b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T>\
		inline bool operator _OPERATOR_(wchar_t a, const StringTemplate<T>& b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T0, typename T1>\
		inline bool operator _OPERATOR_(const T0*a, const ReferenceExpression<T1>&b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T>\
		inline bool operator _OPERATOR_(char a, const ReferenceExpression<T>& b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T>\
		inline bool operator _OPERATOR_(wchar_t a, const ReferenceExpression<T>& b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T0, typename T1>\
		inline bool operator _OPERATOR_(const T0*a, const CharacterExpression<T1>&b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T>\
		inline bool operator _OPERATOR_(char a, const CharacterExpression<T>& b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T>\
		inline bool operator _OPERATOR_(wchar_t a, const CharacterExpression<T>& b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T0, typename T1>\
		inline bool operator _OPERATOR_(const T0*a, const StringExpression<T1>&b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T>\
		inline bool operator _OPERATOR_(char a, const StringExpression<T>& b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T>\
		inline bool operator _OPERATOR_(wchar_t a, const StringExpression<T>& b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T0, typename T1, typename T2>\
		inline bool operator _OPERATOR_(const T0*a, const ConcatExpression<T1, T2>&b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T0, typename T1>\
		inline bool operator _OPERATOR_(char a, const ConcatExpression<T0, T1>&b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}\
	template <typename T0, typename T1>\
		inline bool operator _OPERATOR_(wchar_t a, const ConcatExpression<T0, T1>&b)\
		{\
			return !(b _REVERSE_OPERATOR_ a);\
		}
	


DEFINE_COMPARE_OPERATORS(<,>=)
DEFINE_COMPARE_OPERATORS(>,<=)
DEFINE_COMPARE_OPERATORS(<=,>)
DEFINE_COMPARE_OPERATORS(>=,<)
DEFINE_COMPARE_OPERATORS(==,!=)
DEFINE_COMPARE_OPERATORS(!=,==)

	

#endif
