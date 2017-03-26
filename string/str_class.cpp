#include "../global_root.h"
#include "str_class.h"
#include "../io/file_system.h"
#include <algorithm>

/******************************************************************

String-class providing mostly identical functionality
as the Borland AnsiString-class.

******************************************************************/


template class StringTemplate<char>;
template class StringTemplate<wchar_t>;
template class ReferenceExpression<char>;
template class ReferenceExpression<wchar_t>;

#define ErrBox(msg) ErrMessage(msg)


static  char		zc(0);

		unsigned	GLOBAL_STRING_ID_COUNTER(0);
		void*	   GLOBAL_TRACED_ORIGINAL_ADDR;

#if SYSTEM!=WINDOWS
	static const int MB_OK = 0;
#endif
		

void gotoxy(int x, int y)
{
	#if SYSTEM==UNIX
		char essq[100];		// String variable to hold the escape sequence
		char xstr[100];		// Strings to hold the x and y coordinates
		char ystr[100];		// Escape sequences must be built with characters
	 
		/*
		** Convert the screen coordinates to strings
		*/
		sprintf(xstr, "%d", x);
		sprintf(ystr, "%d", y);
	 
		/*
		** Build the escape sequence (vertical move)
		*/
		essq[0] = '\0';
		strcat(essq, "\033[");
		strcat(essq, ystr);
	 
		/*
		** Described in man terminfo as vpa=\E[%p1%dd
		** Vertical position absolute
		*/
		strcat(essq, "d");
	 
		/*
		** Horizontal move
		** Horizontal position absolute
		*/
		strcat(essq, "\033[");
		strcat(essq, xstr);
		// Described in man terminfo as hpa=\E[%p1%dG
		strcat(essq, "G");
	 
		/*
		** Execute the escape sequence
		** This will move the cursor to x, y
		*/
		printf("%s", essq);
	#elif SYSTEM==WINDOWS
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE) ; 
		COORD position = { (SHORT)x, (SHORT)y } ; 
     
		SetConsoleCursorPosition( hStdout, position ) ;
	#endif
 
}

static inline bool isWhitespace(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}


#if SYSTEM==UNIX
	#include <iconv.h>

	String	ToUTF8(const StringW&str)
	{
		iconv_t	iv = iconv_open("WCHAR_T", "UTF-8");
		size_t maxLen = str.length() * 4;
		Array<char>	field(maxLen);
		char*source = const_cast<char*>((const char*)str.c_str());
		char*dest = field.pointer();
		size_t sourceBytes = (str.length()) * sizeof(wchar_t);
		size_t destBytes = maxLen;
		size_t resultLength = iconv(iv, &source, &sourceBytes, &dest, &destBytes);
		if (resultLength == InvalidIndex)
			return "";
		return String(field.pointer(),maxLen - destBytes);
	}


#endif




char*	strnchr(char*string,size_t cnt, char c)
{
	char*end = string+cnt;
	while (string != end)
	{
		if (*string == c)
			return string;
		string++;
	}
	return NULL;
}

char*	strnstr(char *big, const char *little, size_t len)
{
	size_t len2 = strlen(little);
	if (len2 > len)
		return NULL;
	char*end = big+len-len2;
	while (big != end)
		if (!strncmp(big,little,len2))
			return big;
		else
			big++;
	return NULL;
}



static unsigned	genericLengthFunction(const char*begin, const char*end)
{
	return end-begin;
}






#if SYSTEM==WINDOWS


#else

	void strlwr(char*str)
	{
		while (*str)
		{
			*str = tolower(*str);
			str++;
		}
	}
	void strupr(char*str)
	{
		while (*str)
		{
			*str = toupper(*str);
			str++;
		}
	}

	void wcslwr(wchar_t*str)
	{
		while (*str)
		{
			*str = towlower(*str);
			str++;
		}
	}
	void wcsupr(wchar_t*str)
	{
		while (*str)
		{
			*str = towupper(*str);
			str++;
		}
	}


	//void strlwr(char*target)
	//{
	//	if (!target) return;
	//	while (*target)
	//	{
	//		if (*target >= 'A' && *target <= 'Z')
	//			(*target) += 'a' - 'A';
	//		target++;
	//	}
	//}
	//void strupr(char*target)
	//{
	//	if (!target) return;
	//	while (*target)
	//	{
	//		if (*target >= 'a' && *target <= 'z')
	//			(*target) -= 'a' - 'A';
	//		target++;
	//	}
	//}
#endif





char*	strAllocate(unsigned size)
{
	if (size <= 1)
		return &zc;
	#if (STRING_ALLOCATION_MODE == 0)
		return (char*)malloc(size);
	#elif (STRING_ALLOCATION_MODE == 1)
		return SHIELDED_ARRAY(new char[size],size);
	#else
		return sys::allocate(size);
	#endif
}

char*	strRelocate(char*current, unsigned current_size, unsigned new_size)
{
	
	char*next = strAllocate(new_size);
	if (new_size>current_size)
		memcpy(next,current,current_size);
	else
		memcpy(next,current,new_size);
	strFree(current);
	return next;
}


void	 strFree(char*current)
{
	if (current == &zc || !current)
		return;
	#if (STRING_ALLOCATION_MODE == 0)
		free(current);
	#elif (STRING_ALLOCATION_MODE == 1)
		DISCARD_ARRAY(current);
	#else
		sys::delocate(current);
	#endif
}

void ShowMessage(const char*line)
{
	DisplayMessage("message",line);
}


static void _msgBox(const char*body, const char*head, UINT type)
{
	#if SYSTEM == WINDOWS
		MessageBoxA (NULL,body,head,type);
	#else
		std::cout << "["<<head<<"]:"<<body<<std::endl;
	#endif
}

static void _msgBox(const wchar_t*body, const wchar_t*head, UINT type)
{
	#if SYSTEM == WINDOWS
		MessageBoxW (NULL,body,head,type);
	#else
		std::cout << "["<<head<<"]:"<<body<<std::endl;
	#endif
}

template <typename T>
	const T*	_empty()
	{
		return NULL;
	}
template <>
	const char*	_empty<char>()
	{
		return "<empty string>";
	}
template <>
	const wchar_t*	_empty<wchar_t>()
	{
		return L"<empty string>";
	}

	
#if SYSTEM==WINDOWS
	void	GetProgramFileName(wchar_t*out)
	{
		GetModuleFileNameW( NULL, out, MAX_PATH );
	}
#elif SYSTEM_VARIANCE==LINUX
	extern char * program_invocation_name;

	void	GetProgramFileName(char*out)
	{
//		char real[PATH_MAX] = {0};
		realpath(program_invocation_name, out);
	}
#elif SYSTEM_VARIANCE==FREEBSD
	#include <stdlib.h>
	void	GetProgramFileName(char*out)
	{
//		char real[PATH_MAX] = {0};
		realpath(getprogname(), out);
	}
#else
	#error find some solution
#endif
	

template <typename T>
	StringTemplate<T>	_getApplicationName()
	{
		PathString::char_t szFileName[MAX_PATH];

		GetProgramFileName(  szFileName);

		return StringTemplate<T>(FileSystem::ExtractFileName(PathString(szFileName))+": ");
	}


template <typename T0, typename T1>
	static void _displayMessage(const T0*head_, const T1&line, bool prefixProgramName)
	{
		Array<StringTemplate<T0> >	lines;
		explode((T0)'\n',line,lines);
		StringTemplate<T0> head = prefixProgramName ? _getApplicationName<T0>() + head_ : head_;
		if (!lines.count())
			_msgBox(_empty<T0>(),head.c_str(),MB_OK);
		StringTemplate<T0> sum;
		head += (T0)' ';
		count_t rounds = lines.count() / 40;
		if (lines.count() % 40)
			rounds ++;
		for (index_t i = 0; i < lines.count(); i+= 40)
		{
			sum = implode((T0)'\n',lines + i,std::min<count_t>(40,lines.count() - i));
			if (lines.count() > 40)
			{
				_msgBox(sum.c_str(),(head+ StringTemplate<T0>(i/40 +1)+(T0)'/'+ StringTemplate<T0>(rounds)).c_str(),MB_OK);
			}
			else
				_msgBox(sum.c_str(),head.c_str(),MB_OK);
		}
	}

void			DisplayMessage(const char*head, const char*line, bool prefixProgramName/*=true*/)
{
	#if SYSTEM==WINDOWS
		_displayMessage(head,line,prefixProgramName);
		//MessageBoxA (NULL,line,head?head:"message",MB_OK);
	#elif SYSTEM==UNIX
		if (head)
			std::cout << head << ":"<<std::endl<<"  ";
		std::cout << line<<std::endl;
	#endif
}

void			DisplayMessage(const char*head, const String&line, bool prefixProgramName/*=true*/)
{
	#if SYSTEM==WINDOWS
		_displayMessage(head,line,prefixProgramName);
		//MessageBoxA (NULL,line.c_str(),head?head:"message",MB_OK);
	#elif SYSTEM==UNIX
		if (head)
			std::cout << head << ":"<<std::endl<<"  ";
		std::cout << line<<std::endl;
	#endif

}

void			DisplayMessage(const String&head, const String&line, bool prefixProgramName/*=true*/)
{
	DisplayMessage(head.c_str(),line,prefixProgramName);
}

void ShowMessage(const String&line)
{
	DisplayMessage("message",line);
}


void ErrMessage(const char*line)
{
	DisplayMessage("error",line);
}

void ErrMessage(const String&line)
{
	DisplayMessage("error",line);
}

void			DisplayMessageW(const wchar_t*head, const wchar_t*line, bool prefixProgramName/*=true*/)
{
	#if SYSTEM==WINDOWS
		_displayMessage(head,line,prefixProgramName);
	#elif SYSTEM==UNIX
		if (head)
			std::wcout << head << ":"<<std::endl<<"  ";
		std::wcout << line<<std::endl;
	#endif
}

void			DisplayMessageW(const wchar_t*head, const StringW&line, bool prefixProgramName/*=true*/)
{
	#if SYSTEM==WINDOWS
		_displayMessage(head,line,prefixProgramName);
	#elif SYSTEM==UNIX
		if (head)
			std::wcout << head << ":"<<std::endl<<"  ";
		std::wcout << line<<std::endl;
	#endif

}

void			DisplayMessageW(const StringW&head, const StringW&line, bool prefixProgramName/*=true*/)
{
	DisplayMessageW(head.c_str(),line,prefixProgramName);
}

void ShowMessageW(const StringW&line)
{
	DisplayMessageW(L"message",line);
}

void ShowMessageW(const wchar_t*line)
{
	DisplayMessageW(L"message",line);
}


void ErrMessageW(const wchar_t*line)
{
	DisplayMessageW(L"error",line);
}

void ErrMessageW(const StringW&line)
{
	DisplayMessageW(L"error",line);
}





static char* pointerToHex(const void*pointer, int min_len, char*end, char*first)
{
	size_t value = (size_t)pointer;
	while (value && end != first)
	{
		BYTE c = value & (0xF);
		if (c < 10)
			(*--end) = '0'+c;
		else
			(*--end) = 'A'+c-10;
		value>>=4;
		min_len--;
	}
	while (min_len-->0 && end!=first)
		(*--end) = '0';
	return end;
}

static char* intToHex(unsigned value, int min_len, char*end, char*first)
{
	while (value && end != first)
	{
		BYTE c = value & (0xF);
		if (c < 10)
			(*--end) = '0'+c;
		else
			(*--end) = 'A'+c-10;
		value>>=4;
		min_len--;
	}
	while (min_len-->0 && end!=first)
		(*--end) = '0';
	return end;
}

static char* intToHex(UINT64 value, int min_len, char*end, char*first)
{
	while (value && end != first)
	{
		BYTE c = value & (0xF);
		if (c < 10)
			(*--end) = '0'+c;
		else
			(*--end) = 'A'+c-10;
		value>>=4;
		min_len--;
	}
	while (min_len-->0 && end!=first)
		(*--end) = '0';
	return end;
}


template <class Type, class UType> 
	static char* signedToStr(Type value, char*end, char*first)
	{
		bool negative = false;
		if (value < 0)
		{
			value*=-1;
			negative = true;
		}

		char *c = end;
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

//
//template <class Type>
//	static char* floatToStr(Type value, unsigned char exactness, char*end, char*first)
//	{
//		/*if (value > INT_MAX||value < INT_MIN)
//		{
//			end-=8;
//			strcpy(end,"exceeded");
//			return end;
//		}*/
//
//		if (isinf(value))
//		{
//			end -= 4;
//			if (value > 0)
//				strcpy(end,"+INF");
//			else
//				strcpy(end,"-INF");
//			return end;
//		}
//		
//		if (isnan(value))
//		{
//			end-=3;
//			strcpy(end,"NAN");
//			return end;
//		}
//		bool negative = value < 0;
//		if (negative)
//			value*=-1;
//		
//		char*c = end;
//		(*c) = 0;
//		if (value > INT_MAX)
//		{
//			int exponent = (int)log10(value);
//			//cout << "exponent of "<<value<<" is "<<exponent << endl;
//			value /= pow(10.0,exponent);
//			c = signedToStr<int,unsigned>(exponent,c,first);
//			if (c != first)
//				(*(--c)) = 'e';
//		}
//			
//			
//		if (pow(10.0f,exactness)*value > INT_MAX)
//			exactness = (BYTE)log10(INT_MAX/value);
//		UINT64 v = (UINT64)(value*pow(10.0f,exactness));
//		unsigned at(0);
//		bool Write = !exactness;
//		while (v && c != first)
//		{
//			char ch = '0'+(v%10);
//			Write = Write || ch != '0' || at==exactness;
//			if (Write)
//				(*--c) = ch;
//			v/=10;
//			at++;
//			if (at == exactness && c != first && Write)
//				(*--c) = DecimalSeparator;
//		}
//		if (c != end)
//		{
//			while (at < exactness && c != first)
//			{
//				(*--c) = '0';
//				at++;
//			}
//			if (at == exactness && c != first)
//			{
//				if ((*c) != DecimalSeparator)
//					(*--c) = DecimalSeparator;
//				if (c != first)
//					(*--c) = '0';
//			}
//			if (negative && c != first)
//				(*--c) = '-';
//		}
//		else
//			(*--c) = '0';
//	//	len = end-c;
//		return c;
//	}

char*			writeToCharField(unsigned value, char*offset, char*end)
{
	char	*c = offset;
	while (value && c != end)
	{
		(*(c++)) = '0'+(value%10);
		value/=10;
	}
	if (c==offset)
		(*(c++)) = '0';
	
	for (char*first = offset, *last=c; first < last; first++, last--)
		swp(*first,*last);
	return c;
}

char*			writeFloatToCharField(float value, char*offset, char*end)
{
	char	*c = offset;


	if (value > INT_MAX||value < INT_MIN)
	{
		if (end-c>=8)
		{
			strncpy(c,"exceeded",8);
			return c+8;
		}
		return c;
	}
	if (isnan(value))
	{
		if (end-c>=3)
		{
			strncpy(c,"NAN",3);
			return c+3;
		}
		return c;
	}
	bool negative = value < 0;
	if (negative)
		value*=-1;
	float exactness = 5;
	if (pow(10.0f,exactness)*value > INT_MAX)
		exactness = (BYTE)log10(INT_MAX/value);
	UINT64 v = (UINT64)(value*pow(10.0f,exactness));
	unsigned at(0);
	bool write = !exactness;
	while (v && c != end)
	{
		char ch = '0'+(v%10);
		write = write || ch != '0' || at==exactness;
		if (write)
			(*c++) = ch;
		v/=10;
		at++;
		if (at == exactness && c != end && write)
			(*c++) = DecimalSeparator;
	}
	if (c != offset)
	{
		while (at < exactness && c != end)
		{
			(*c++) = '0';
			at++;
		}
		if (at == exactness && c != end)
		{
			if ((*c) != DecimalSeparator)
				(*c++) = DecimalSeparator;
			if (c != end)
				(*c++) = '0';
		}
		if (negative && c != end)
			(*c++) = '-';
	}
	
	for (char*first = offset, *last=c; first < last; first++, last--)
		swp(*first,*last);
	return c;
}

String PointerToHex(const void*pointer)
{
	char buffer[257];
	buffer[sizeof(buffer)-1] = 0;
	return pointerToHex(pointer,sizeof(pointer)*2,buffer+sizeof(buffer)-1,buffer);
}


static char		hexChar(BYTE value)
{
	if (value < 10)
		return '0'+value;
	return 'A'+(value-10);
}

void		binaryToHex(const void*data, size_t data_size, String&target)
{
	target.resize((unsigned)data_size<<1);
	const BYTE*pntr = (const BYTE*)data;
	for (unsigned i = 0; i < data_size; i++)
	{
		target.set((i<<1),hexChar(((pntr[i]>>4)&0xF)));
		target.set((i<<1)+1, hexChar((pntr[i])&0xF));
	}
}

String		binaryToHex(const void*data, size_t data_size)
{
	String rs;
	binaryToHex(data,data_size,rs);
	return rs;
}



#ifndef __BORLANDC__
String IntToHex(int value, BYTE max_len)
{
	char buffer[257];
	buffer[sizeof(buffer)-1] = 0;
	return intToHex((unsigned)value,max_len,buffer+sizeof(buffer)-1,buffer);
}

String IntToHex(__int64 value, BYTE max_len)
{
	char buffer[257];
	buffer[sizeof(buffer)-1] = 0;
	return intToHex((UINT64)value,max_len,buffer+sizeof(buffer)-1,buffer);
}

String IntToStr(int value)
{
	return String(value);
}

String FloatToStr(float value)
{
	return String(value);
}
#else
String intToHex(int value, BYTE max_len)
{
	char buffer[257];
	buffer[sizeof(buffer)-1] = 0;
	return intToHex((unsigned)value,max_len,buffer+sizeof(buffer)-1,buffer);
}

String intToHex(__int64 value, BYTE max_len)
{
	char buffer[257];
	buffer[sizeof(buffer)-1] = 0;
	return intToHex((UINT64)value,max_len,buffer+sizeof(buffer)-1,buffer);
}

String intToStr(int value)
{
	return String(value);
}

String FloatToStr(float value)
{
	return value;
}


#endif


namespace StringConversion
{
	bool	IsAnsiMultiByte(char csource)
	{
		return ((signed char)csource) < 0;
	}

	void AnsiCharToUtf8(char csource, char*&dest, const char*end)
	{
		unsigned char usource = (unsigned char)csource;
		ASSERT_LESS__(dest,end);
		if (usource < 128)
		{
			*dest = csource;
			dest++;
			return;
		}
		*dest = (char)(0xC0 | (usource >> 6));
		dest++;
		ASSERT_LESS__(dest,end);
		*dest = (char)(0x80 | (usource & 0x3f));
		dest++;
	}

	void	AnsiToUtf8(const char ansiSource, UTF8Char&utf8Dest)
	{
		char*at = utf8Dest.encoded;
		AnsiCharToUtf8(ansiSource,at,at+6);
		utf8Dest.numCharsUsed = (BYTE)(at - utf8Dest.encoded);
	}



	bool	Utf8ToAnsi(const String&utf8Source, String&ansiDest)
	{
		return Utf8ToAnsi(utf8Source.ref(),ansiDest);
	}

	void	AnsiToUtf8(const String&ansiSource, String&utf8Dest)
	{
		AnsiToUtf8(ansiSource.ref(),utf8Dest);
	}

	void	AnsiToUtf8(const StringRef&ansiSource, String&utf8Dest)
	{
		count_t len = 0;
		for (index_t i = 0; i < ansiSource.length(); i++)
			if (IsAnsiMultiByte(ansiSource[i]))
				len += 2;
			else
				len ++;
		utf8Dest.setLength(len);
		index_t at = 0;
		char*out = utf8Dest.mutablePointer();
		char*end = out + len;
		for (index_t i = 0; i < ansiSource.length(); i++)
			AnsiCharToUtf8(ansiSource[i],out,end);
	}


	bool Utf8CharToAnsi(const char*&ch, const char*const inEnd, char&out)
	{
		if (ch >= inEnd)
			return false;

		unsigned char usource = (unsigned char)*ch;
		ch++;
		if (!(usource & 0x80))
			out = (char)usource;
		else
		{
			if (ch >= inEnd)
				return false;
			unsigned char usource1 = (unsigned char)*ch;	//should be caught. if not, and out of range, must be terminating 0.
			ch++;
				
			unsigned char payload0 = (unsigned char)(usource & ~0xC0);
			if (payload0 & 0x3C)
			{
				//not ansi
				return false;
			}
			out = (char)((payload0 << 6) | ((usource1 & ~0x80)));
		}
		return true;
	}


	//http://stackoverflow.com/questions/7153935/how-to-convert-utf-8-stdstring-to-utf-16-stdwstring/7154226#7154226
	bool	Utf8ToUnicode(const StringRef&utf8Source, StringTemplate<char32_t>&unicodeDest)
	{
		const char*utf8 = utf8Source.pointer();
		char const*const end = utf8 + utf8Source.length();

		count_t len = 0;

		while (utf8 < end)
		{
			count_t localLen = 0;
			unsigned char usource = (unsigned char)*utf8;
			while (usource & 0x80)
			{
				usource <<= 1;
				localLen ++;
			}
			localLen = std::max<count_t>(localLen,1U);
			utf8 += localLen;
			len++;
		}
		utf8 = utf8Source.pointer();

		unicodeDest.setLength(len);
		index_t at= 0;

		for (;utf8 != end;  utf8++)
		{
			char32_t uni;
			size_t extra;
			bool error = false;
			unsigned char ch = *utf8;
			if (ch <= 0x7F)
			{
				uni = ch;
				extra = 0;
			}
			else if (ch <= 0xBF)
			{
				return false;
			}
			else if (ch <= 0xDF)
			{
				uni = ch&0x1F;
				extra = 1;
			}
			else if (ch <= 0xEF)
			{
				uni = ch&0x0F;
				extra = 2;
			}
			else if (ch <= 0xF7)
			{
				uni = ch&0x07;
				extra = 3;
			}
			else
			{
				return false;
			}
			for (size_t j = 0; j < extra; ++j)
			{
				if (utf8+1 == end)
					return false;
				unsigned char ch = *(++utf8);
				if (ch < 0x80 || ch > 0xBF)
					return false;
				uni <<= 6;
				uni |= ch & 0x3F;
			}
			if (uni >= 0xD800 && uni <= 0xDFFF)
				return false;;
			if (uni > 0x10FFFF)
				return false;

			ASSERT_LESS__(at,unicodeDest.length());
			unicodeDest.set(at++,uni);
		}
		ASSERT_EQUAL__(at,unicodeDest.length());
		return true;
	}


	
	template <typename T>
	void	UnicodeToUtf16T(const ReferenceExpression<char32_t>&unicodeSource, StringTemplate<T>&utf16Dest)
	{
		count_t len = 0;

		auto*s = unicodeSource.pointer(),
			*const end = unicodeSource.end();
		for (; s != end; ++s)
		{
			const auto uni = *s;
			len++;
			if (uni > 0xFFFF)
				len++;
		}

		utf16Dest.setLength(len);
		s = unicodeSource.pointer();
		index_t at = 0;
		for (; s != end; ++s)
		{
			auto uni = *s;
			if (uni <= 0xFFFF)
			{
				utf16Dest.set(at++,(T)uni);
			}
			else
			{
				uni -= 0x10000;
				utf16Dest.set(at++,(T)((uni >> 10) + 0xD800));
				utf16Dest.set(at++,(T)((uni & 0x3FF) + 0xDC00));
			}
		}
		ASSERT_EQUAL__(at,utf16Dest.length());
	}

	template <typename T>
	bool	Utf8ToUtf16T(const StringRef&utf8Source, StringTemplate<T>&utf16Dest)
	{
		count_t len = 0;
		const char*utf8 = utf8Source.pointer();
		char const*const end = utf8 + utf8Source.length();

		StringTemplate<char32_t> unicode;
		if (!Utf8ToUnicode(utf8Source,unicode))
			return false;

		UnicodeToUtf16T(unicode.ref(),utf16Dest);
		return true;
	}


	#ifdef WIN32
		static_assert(sizeof(wchar_t)==sizeof(char16_t),"Expected wchar_t to be 16 bit on windows");
		void	UnicodeToUtf16(const ReferenceExpression<char32_t>&unicodeSource, StringTemplate<wchar_t>&utf16Dest)
		{
			UnicodeToUtf16T(unicodeSource,utf16Dest);
		}
		bool	Utf8ToUtf16(const StringRef&utf8Source, StringTemplate<wchar_t>&utf16Dest)
		{
			return Utf8ToUtf16T(utf8Source,utf16Dest);
		}
	#endif

	void	UnicodeToUtf16(const ReferenceExpression<char32_t>&unicodeSource, StringTemplate<char16_t>&utf16Dest)
	{
		UnicodeToUtf16T(unicodeSource,utf16Dest);
	}

	bool	Utf8ToUtf16(const StringRef&utf8Source, StringTemplate<char16_t>&utf16Dest)
	{
		return Utf8ToUtf16T(utf8Source,utf16Dest);
	}



	bool	Utf8ToAnsi(const StringRef&utf8Source, String&ansiDest)
	{
		count_t len = 0;
		const char*utf8 = utf8Source.pointer();
		char const*const end = utf8 + utf8Source.length();
		while (utf8 < end)
		{
			count_t localLen = 0;
			unsigned char usource = (unsigned char)*utf8;
			while (usource & 0x80)
			{
				usource <<= 1;
				localLen ++;
			}
			localLen = std::max<count_t>(localLen,1U);
			if (localLen > 2)
				return false;
			utf8 += localLen;
			len++;
		}

		ansiDest.setLength(len);
		index_t at = 0;
		char*out = ansiDest.mutablePointer();
		const char*in = utf8Source.pointer();
		const char*const inEnd = in + utf8Source.length();
		while (in < inEnd)
		{
			Utf8CharToAnsi(in,inEnd, *out);
			out++;
		}
		ASSERT__(out == ansiDest.c_str() + ansiDest.length());
		return true;

	}
}




//#if 0







