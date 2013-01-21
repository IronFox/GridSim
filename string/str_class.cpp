#include "../global_root.h"
#include "str_class.h"

/******************************************************************

String-class providing mostly identical functionality
as the Borland AnsiString-class.

******************************************************************/


template StringTemplate<char>;
template StringTemplate<wchar_t>;
template ReferenceExpression<char>;
template ReferenceExpression<wchar_t>;

#define ErrBox(msg) ErrMessage(msg)


static  char		zc(0);

		unsigned	GLOBAL_STRING_ID_COUNTER(0);
		void*	   GLOBAL_TRACED_ORIGINAL_ADDR;


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
		COORD position = { x, y } ; 
     
		SetConsoleCursorPosition( hStdout, position ) ;
	#endif
 
}

static inline bool isWhitespace(char c)
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}


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
	void strlwr(char*target)
	{
		if (!target) return;
		while (*target)
		{
			if (*target >= 'A' && *target <= 'Z')
				(*target) += 'a' - 'A';
			target++;
		}
	}
	void strupr(char*target)
	{
		if (!target) return;
		while (*target)
		{
			if (*target >= 'a' && *target <= 'z')
				(*target) -= 'a' - 'A';
			target++;
		}
	}
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
	displayMessage(NULL,line);
}

void			displayMessage(const char*head, const char*line)
{
	#if SYSTEM==WINDOWS
		MessageBoxA (NULL,line,head?head:"message",MB_OK);
	#elif SYSTEM==UNIX
		if (head)
			cout << head << ":"<<endl<<"  ";
		cout << line<<endl;
	#endif
}

void			displayMessage(const char*head, const String&line)
{
	#if SYSTEM==WINDOWS
		MessageBoxA (NULL,line.c_str(),head?head:"message",MB_OK);
	#elif SYSTEM==UNIX
		if (head)
			cout << head << ":"<<endl<<"  ";
		cout << line<<endl;
	#endif

}

void			displayMessage(const String&head, const String&line)
{
	displayMessage(head.c_str(),line);
}

void ShowMessage(const String&line)
{
	displayMessage(NULL,line);
}


void ErrMessage(const char*line)
{
	displayMessage("error",line);
}

void ErrMessage(const String&line)
{
	displayMessage("error",line);
}

void			displayMessageW(const wchar_t*head, const wchar_t*line)
{
	#if SYSTEM==WINDOWS
		MessageBoxW (NULL,line,head?head:(L"message"),MB_OK);
	#elif SYSTEM==UNIX
		if (head)
			wcout << head << ":"<<endl<<"  ";
		wcout << line<<endl;
	#endif
}

void			displayMessageW(const wchar_t*head, const StringW&line)
{
	#if SYSTEM==WINDOWS
		MessageBoxW (NULL,line.c_str(),head?head:L"message",MB_OK);
	#elif SYSTEM==UNIX
		if (head)
			cout << head << ":"<<endl<<"  ";
		cout << line<<endl;
	#endif

}

void			displayMessageW(const StringW&head, const StringW&line)
{
	displayMessageW(head.c_str(),line);
}

void ShowMessageW(const StringW&line)
{
	displayMessageW(NULL,line);
}


void ErrMessageW(const wchar_t*line)
{
	displayMessageW(L"error",line);
}

void ErrMessageW(const StringW&line)
{
	displayMessageW(L"error",line);
}





static char* pointerToHex(const void*pointer, int min_len, char*end, char*first)
{
	unsigned long value = (unsigned long)pointer;
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


template <class Type>
	static char* floatToStr(Type value, unsigned char exactness, char*end, char*first)
	{
		/*if (value > INT_MAX||value < INT_MIN)
		{
			end-=8;
			strcpy(end,"exceeded");
			return end;
		}*/

		if (isinf(value))
		{
			end -= 4;
			if (value > 0)
				strcpy(end,"+INF");
			else
				strcpy(end,"-INF");
			return end;
		}
		
		if (isnan(value))
		{
			end-=3;
			strcpy(end,"NAN");
			return end;
		}
		bool negative = value < 0;
		if (negative)
			value*=-1;
		
		char*c = end;
		(*c) = 0;
		if (value > INT_MAX)
		{
			int exponent = (int)log10(value);
			//cout << "exponent of "<<value<<" is "<<exponent << endl;
			value /= pow(10.0,exponent);
			c = signedToStr<int,unsigned>(exponent,c,first);
			if (c != first)
				(*(--c)) = 'e';
		}
			
			
		if (pow(10.0f,exactness)*value > INT_MAX)
			exactness = (BYTE)log10(INT_MAX/value);
		UINT64 v = (UINT64)(value*pow(10.0f,exactness));
		unsigned at(0);
		bool write = !exactness;
		while (v && c != first)
		{
			char ch = '0'+(v%10);
			write = write || ch != '0' || at==exactness;
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
				(*--c) = '0';
				at++;
			}
			if (at == exactness && c != first)
			{
				if ((*c) != DecimalSeparator)
					(*--c) = DecimalSeparator;
				if (c != first)
					(*--c) = '0';
			}
			if (negative && c != first)
				(*--c) = '-';
		}
		else
			(*--c) = '0';
	//	len = end-c;
		return c;
	}

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

String floatToStr(float value)
{
	return value;
}


#endif




//#if 0







