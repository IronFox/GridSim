#include "../global_root.h"
#include "string_converter.h"

/******************************************************************

Collection of string-to-type parsers.

******************************************************************/

#ifdef _MSC_VER

	#ifndef strcmpi
		#define strcmpi _strcmpi
	#endif
	#ifndef strncmpi
		#define strncmpi _strnicmp
	#endif
#elif __GNUC__
	#ifndef strncmpi
		#define strncmpi strnicmp
	#endif


#endif

#include <limits>


// #undef INFINITY
// #ifndef _MSC_VER
	// #define INFINITY (1.0e1000)
// #else
	// #define INFINITY	pow(10.0,1000)
// #endif



#define BYTE_LOW_ID     '0'
#define BYTE_HIGH_ID    '9'


#ifndef __BORLANDC__
    char     DecimalSeparator='.';
#endif

#ifndef ARRAYSIZE
    #define ARRAYSIZE(array)    sizeof(array)/sizeof(array[0])
#endif


struct TKeyName
{
	Key::Name	index;
	const char*	name;
};



static  const char  *Yes[] = {"YES","TRUE","ON","ENABLED","1"},
                    *No[] = {"NO","FALSE","OFF","DISABLED","0"};

#undef KN
#define KN(key)	{Key::key,#key}

#undef JK
#define JK(INDEX)	\
	KN(Joystick##INDEX##Button0),\
	KN(Joystick##INDEX##Button1),\
	KN(Joystick##INDEX##Button2),\
	KN(Joystick##INDEX##Button3),\
	KN(Joystick##INDEX##Button4),\
	KN(Joystick##INDEX##Button5),\
	KN(Joystick##INDEX##Button6),\
	KN(Joystick##INDEX##Button7),\
	KN(Joystick##INDEX##Button8),\
	KN(Joystick##INDEX##Button9),\
	KN(Joystick##INDEX##Button10),\
	KN(Joystick##INDEX##Button11),\
	KN(Joystick##INDEX##Button12),\
	KN(Joystick##INDEX##Button13),\
	KN(Joystick##INDEX##Button14),\
	KN(Joystick##INDEX##Button15),\
	KN(Joystick##INDEX##Button16),\
	KN(Joystick##INDEX##Button17),\
	KN(Joystick##INDEX##Button18),\
	KN(Joystick##INDEX##Button19),\
	KN(Joystick##INDEX##Button20),\
	KN(Joystick##INDEX##Button21),\
	KN(Joystick##INDEX##Button22),\
	KN(Joystick##INDEX##Button23),\
	KN(Joystick##INDEX##Button24),\
	KN(Joystick##INDEX##Button25),\
	KN(Joystick##INDEX##Button26),\
	KN(Joystick##INDEX##Button27),\
	KN(Joystick##INDEX##Button28),\
	KN(Joystick##INDEX##Button29),\
	KN(Joystick##INDEX##Button30),\
	KN(Joystick##INDEX##Button31),


static  TKeyName    K_ID[]=
					{
						KN(Undefined),
						KN(A),KN(B),KN(C),KN(D),KN(E),KN(F),KN(G),KN(H),KN(I),KN(J),KN(K),KN(L),KN(M),KN(N),KN(O),KN(P),KN(Q),KN(R),KN(S),KN(T),KN(U),KN(V),KN(W),KN(X),KN(Y),KN(Z),
						{Key::OE,"Ö"},{Key::AE,"Ä"},{Key::UE,"Ü"},{Key::SZ,"ß"},
						KN(Tab),KN(CapsLock),KN(Shift),KN(Alt),KN(Ctrl),KN(Del),
						KN(PadPlus),KN(PadMinus),KN(Plus),KN(Minus),
						KN(Space),KN(BackSpace),KN(Return),
						KN(Esc), KN(Escape),
						KN(F1),KN(F2),KN(F3),KN(F4),KN(F5),KN(F6),KN(F7),KN(F8),KN(F9),KN(F10),KN(F11),KN(F12),
						KN(LeftMouseButton), KN(MiddleMouseButton), KN(RightMouseButton),
						KN(MouseButton0),KN(MouseButton1),KN(MouseButton2),KN(MouseButton3),KN(MouseButton4),
						{Key::Caret,"^"},
						{Key::Mesh,"#"},
						{Key::Comma,","},
						{Key::Period,"."},
						{Key::ColonSemicolon,"Semicolon"},
						{Key::ColonSemicolon,"Colon"},
						{Key::ColonSemicolon,"ColonSemicolon"},
						{Key::ColonSemicolon,";"},
						{Key::ColonSemicolon,":"},
						{Key::BracketOpen,"BracketOpen"},
						{Key::BracketOpen,"LeftBracket"},
						{Key::BracketOpen,"["},
						{Key::BracketOpen,"{"},
						{Key::BracketClose,"BracketClose"},
						{Key::BracketClose,"RightBracket"},
						{Key::BracketClose,"]"},
						{Key::BracketClose,"}"},
						{Key::N0,"0"},{Key::N1,"1"},{Key::N2,"2"},{Key::N3,"3"},{Key::N4,"4"},{Key::N5,"5"},{Key::N6,"6"},{Key::N7,"7"},{Key::N8,"8"},{Key::N9,"9"},
						KN(Up),KN(Down),KN(Left),KN(Right),KN(PageUp),KN(PageDown),KN(Home),KN(End),KN(Insert),



						JK(0)
						JK(1)
						JK(2)
						JK(3)
						JK(4)
						JK(5)
						JK(6)
						JK(7)
						JK(8)
						JK(9)
						JK(10)
						JK(11)
						JK(12)
						JK(13)
						JK(14)
						JK(15)
					};
#undef KN
					
//#include <iostream>

const char* resolveKeyName(Key::Name key)
{
	static const char*name_field[Key::Max+1];
	static bool name_field_set = false;
	if (!name_field_set)
	{
		for (unsigned i = 0; i < ARRAYSIZE(name_field); i++)
			name_field[i] = "Unnamed";
		for (unsigned i = 0; i < ARRAYSIZE(K_ID); i++)
			if (K_ID[i].index >= 0 && K_ID[i].index <= Key::Max)
				name_field[K_ID[i].index] = K_ID[i].name;
		name_field_set = true;
	}
	if (key >= 0 && key <= Key::Max)
		return name_field[key];
    return "Unknown";
}




void    to8ByteName(const char*origin, char target[8])
{
    BYTE pos = 0;
	while (target[pos] && pos < 8)
	{
        target[pos] = origin[pos];
		pos++;
	}
    while (pos < 8)
        target[pos++] = ' ';
}

void    to8ByteName(const char*origin, __int64&target)
{
    to8ByteName(origin,(char*)&target);
}

void    to8ByteName(const char*origin, size_t length, char target[8])
{
    BYTE pos = 0;
	while (size_t(pos)<length && pos < 8)
	{
        target[pos] = origin[pos];
		pos++;
	}

    while (pos < 8)
        target[pos++] = ' ';
}

void    to8ByteName(const char*origin, size_t length, __int64&target)
{
    to8ByteName(origin,length,(char*)&target);
}


bool isDec(const char*string, bool allow_negative)
{
	if (*string == '-')
	{
		if (!allow_negative)
			return false;
		string++;
	}
	elif (*string == '+')
		string++;
    unsigned index = 0;
    while (string[index])
    {
        if (string[index]<'0' || string[index] > '9')
            return false;
        index++;
    }
    return index!=0;
}


bool segIsDec(const char*string, size_t length, bool allow_negative)
{
	if (!length)
		return false;
	if (*string == '-')
	{
		if (!allow_negative)
			return false;
		string++;
		length--;
	}
	elif (*string == '+')
	{
		string++;
		length--;
	}
    size_t index = 0;
    while (index < length)
    {
        if (string[index]<'0' || string[index] > '9')
            return false;
        index++;
    }
    return index!=0;
}

bool isHex(const char*string, bool allow_negative)
{
	if (*string == '-')
	{
		if (!allow_negative)
			return false;
		string++;
	}
	elif (*string == '+')
		string++;
		
	if (*string++ != '0')
		return false;
	if (*string++ != 'x')
		return false;

    unsigned index = 0;
    while (string[index])
    {
        if ((string[index]<'0' || string[index] > '9') && (string[index]<'a' && string[index]>'f') && (string[index]<'A' && string[index]>'F'))
            return false;
        index++;
    }
    return index!=0;
}


bool segIsHex(const char*string, size_t length, bool allow_negative)
{
	if (!length)
		return false;
	if (*string == '-')
	{
		if (!allow_negative)
			return false;
		string++;
		length--;
	}
	elif (*string == '+')
	{
		string++;
		length--;
	}
	if (length <= 2)
		return false;
	if (*string++ != '0')
		return false;
	if (*string++ != 'x')
		return false;
	length -= 2;

    size_t index = 0;
    while (index < length)
    {
        if ((string[index]<'0' || string[index] > '9') && (string[index]<'a' && string[index]>'f') && (string[index]<'A' && string[index]>'F'))
            return false;
        index++;
    }
    return index!=0;
}

bool isInt(const char*string, bool allow_negative)
{
	if (*string == '-')
	{
		if (!allow_negative)
			return false;
		string++;
	}
	elif (*string == '+')
		string++;

	if (string[0] == '0' && string[1] == 'x')
		return isHex(string,false);
	return isDec(string,false);
}

bool segIsInt(const char*string, size_t length, bool allow_negative)
{
	if (!length)
		return false;
	if (*string == '-')
	{
		if (!allow_negative)
			return false;
		string++;
		length--;
	}
	elif (*string == '+')
	{
		string++;
		length--;
	}

	if (length > 2 && string[0] == '0' && string[1] == 'x')
		return segIsHex(string,length,false);
	return segIsDec(string,length,false);
}



bool isFloat(const char*string)
{
	if (*string == '-' || *string=='+')
		string++;
    bool dot(false), exp(false);
	const char*c = string;
    unsigned index(0);
    while (*c)
    {
		if (*c == 'e' || *c == 'E')
		{
			if (c==string)
				return false;
			c++;
			if (*c == '-')
				c++;
			return isDec(c);
		}
        elif (*c == DecimalSeparator)
		{
            if (dot||c==string)
			{
                return false;
			}
            else
                dot = true;
		}
        else
            if ((*c) < '0' || (*c) > '9')
			{
                return false;
			}
        c++;
    }
    return c!=string && *(c-1) != DecimalSeparator;
}


bool segIsFloat(const char*string, size_t length)
{
	if (!length)
		return false;
	if (*string == '-' || *string=='+')
	{
		string++;
		length--;
		if (!length)
			return false;
	}
    bool dot(false), exp(false);
	const char	*c = string,
				*end = string+length;
    while (c < end)
    {
		if (*c == 'e' || *c == 'E')
		{
			if (c==string)
				return false;
			c++;
			if (c < end && *c == '-')
				c++;
			return segIsDec(c,end-c);
		}
        elif (*c == DecimalSeparator)
		{
            if (dot||c==string)
			{
                return false;
			}
            else
                dot = true;
		}
        else
            if ((*c) < '0' || (*c) > '9')
			{
                return false;
			}
        c++;
    }
    return c!=string && *(c-1) != DecimalSeparator;
}






template <typename T>
	static inline T extractUnsignedHexType(const char*string)
	{
		if (*string == '0' && string[1] == 'x')
			string += 2;
		T value=0;
		while (*string)
		{
			value*=0x10;
			char c = *string;
			if (c>='0' && c <= '9')
				value+=(BYTE)c-'0';
			elif (c>='a' && c <= 'f')
				value+=0xA+(BYTE)c-'a';
			elif (c>='A' && c <= 'F')
				value+=0xA+(BYTE)c-'A';
			string++;
		}
		return value;
	}

template <typename T>
	static inline T extractUnsignedHexType(const char*string, size_t length)
	{
		if (!length)
			return 0;
		if (length >= 2 && *string == '0' && string[1] == 'x')
		{
			string += 2;
			length -= 2;
		}
		T value=0;
		const char*end = string+length;
		while (string<end)
		{
			value*=0x10;
			char c = *string;
			if (c>='0' && c <= '9')
				value+=(BYTE)c-'0';
			elif (c>='a' && c <= 'f')
				value+=0xA+(BYTE)c-'a';
			elif (c>='A' && c <= 'F')
				value+=0xA+(BYTE)c-'A';
			string++;
		}
		return value;
	}

template <typename T>
	static inline T extractUnsignedDecType(const char*string)
	{
		T value=0;
		while (*string)
		{
			value*=10;
			if ((*string>='0') && (*string <= '9'))
				value+=(unsigned char)*string-'0';
			string++;
		}
		return value;
	}

template <typename T>
	static inline T extractUnsignedDecType(const char*string, size_t length)
	{
		if (!length)
			return 0;
		T value=0;
		const char*end = string+length;
		while (string<end)
		{
			value*=10;
			if ((*string>='0') && (*string <= '9'))
				value+=(unsigned char)*string-'0';
			string++;
		}
		return value;
	}

template <typename T>
	static inline T extractSignedHexType(const char*string)
	{
		bool negative = *string=='-';
		if (negative || *string == '+')
			string++;
		if (*string == '0' && string[1] == 'x')
			string += 2;
		T value=0;
		while (*string)
		{
			value*=0x10;
			char c = *string;
			if (c>='0' && c <= '9')
				value+=(BYTE)c-'0';
			elif (c>='a' && c <= 'f')
				value+=0xA+(BYTE)c-'a';
			elif (c>='A' && c <= 'F')
				value+=0xA+(BYTE)c-'A';
			string++;
		}
		if (negative)
			value*=-1;
		return value;
	}

template <typename T>
	static inline T extractSignedHexType(const char*string, size_t length)
	{
		if (!length)
			return 0;
		bool negative = *string=='-';
		if (negative || *string == '+')
		{
			string++;
			length--;
			if (!length)
				return 0;
		}
		if (length >= 2 && *string == '0' && string[1] == 'x')
		{
			string += 2;
			length -= 2;
		}
		T value=0;
		const char*end = string+length;
		while (string<end)
		{
			value*=0x10;
			char c = *string;
			if (c>='0' && c <= '9')
				value+=(BYTE)c-'0';
			elif (c>='a' && c <= 'f')
				value+=0xA+(BYTE)c-'a';
			elif (c>='A' && c <= 'F')
				value+=0xA+(BYTE)c-'A';
			string++;
		}
		if (negative)
			value*=-1;
		return value;
	}

template <typename T>
	static inline T extractSignedDecType(const char*string)
	{
		bool negative = *string=='-';
		if (negative || string[0] == '+')
			string++;
		T value=0;
		while (*string)
		{
			value*=10;
			if ((*string>='0') && (*string <= '9'))
				value+=(unsigned char)*string-'0';
			string++;
		}
		if (negative)
			value*=-1;
		return value;
	}

template <typename T>
	static inline T extractSignedDecType(const char*string, size_t length)
	{
		if (!length)
			return 0;
		bool negative = *string=='-';
		if (negative || string[0] == '+')
		{
			string++;
			length--;
		}
		T value=0;
		const char*end = string+length;
		while (string<end)
		{
			value*=10;
			if ((*string>='0') && (*string <= '9'))
				value+=(unsigned char)*string-'0';
			string++;
		}
		if (negative)
			value*=-1;
		return value;
	}

template <typename T>
	static inline T extractSignedIntType(const char*string)
	{
		bool negative = *string=='-';
		if (negative || string[0] == '+')
			string++;
		T result;
		if (string[0] == '0' && string[1] == 'x')
			result = extractSignedHexType<T>(string+2);
		else
			result = extractSignedDecType<T>(string);
		if (negative)
			result *= -1;
		return result;
	}

template <typename T>
	static inline T extractSignedIntType(const char*string, size_t length)
	{
		if (!length)
			return 0;
		bool negative = *string=='-';
		if (negative || string[0] == '+')
		{
			string++;
			length--;
		}
		T result;
		if (length > 2 && string[0] == '0' && string[1] == 'x')
			result = extractSignedHexType<T>(string+2,length-2);
		else
			result = extractSignedDecType<T>(string,length);
		if (negative)
			result *= -1;
		return result;
	}

template <typename T>
	static inline T extractUnsignedIntType(const char*string)
	{
		T result;
		if (string[0] == '0' && string[1] == 'x')
			result = extractUnsignedHexType<T>(string+2);
		else
			result = extractUnsignedDecType<T>(string);
		return result;
	}

template <typename T>
	static inline T extractUnsignedIntType(const char*string, size_t length)
	{
		if (!length)
			return 0;
		T result;
		if (length > 2 && string[0] == '0' && string[1] == 'x')
			result = extractUnsignedHexType<T>(string+2,length-2);
		else
			result = extractUnsignedDecType<T>(string,length);
		return result;
	}


int		extractInt(const char*string)
{
	return extractSignedIntType<int>(string);
}

int		extractInt(const char*string, size_t length)
{
	return extractSignedIntType<int>(string,length);
}


unsigned	extractUnsigned(const char*string)
{
	return extractUnsignedIntType<unsigned>(string);
}

unsigned	extractUnsigned(const char*string, size_t length)
{
	return extractUnsignedIntType<unsigned>(string,length);
}

int      extractDec(const char*string)
{
	return extractSignedDecType<int>(string);
}

int      extractDec(const char*string, size_t length)
{
	return extractSignedDecType<int>(string,length);
}


int      extractHex(const char*string)
{
	return extractSignedHexType<int>(string);
}

int      extractHex(const char*string, size_t length)
{
	return extractSignedHexType<int>(string,length);
}


unsigned      extractUnsignedDec(const char*string)
{
	return extractUnsignedDecType<unsigned>(string);
}

unsigned      extractUnsignedDec(const char*string, size_t length)
{
	return extractUnsignedDecType<unsigned>(string,length);
}


unsigned      extractUnsignedHex(const char*string)
{
	return extractUnsignedHexType<unsigned>(string);
}

unsigned      extractUnsignedHex(const char*string, size_t length)
{
	return extractUnsignedHexType<unsigned>(string,length);
}









long long		extractInt64(const char*string)
{
	return extractSignedIntType<long long>(string);
}


long long		extractInt64(const char*string, size_t length)
{
	return extractSignedIntType<long long>(string,length);
}


long long      extractDec64(const char*string)
{
	return extractSignedDecType<long long>(string);
}


long long      extractDec64(const char*string, size_t length)
{
	return extractSignedDecType<long long>(string,length);
}


long long      extractHex64(const char*string)
{
	return extractSignedHexType<long long>(string);
}

long long      extractHex64(const char*string, size_t length)
{
	return extractSignedHexType<long long>(string,length);
}



unsigned long long		extractUnsignedInt64(const char*string)
{
	return extractUnsignedIntType<unsigned long long>(string);
}


unsigned long long		extractUnsignedInt64(const char*string, size_t length)
{
	return extractUnsignedIntType<unsigned long long>(string,length);
}


unsigned long long      extractUnsignedDec64(const char*string)
{
	return extractUnsignedDecType<unsigned long long>(string);
}


unsigned long long      extractUnsignedDec64(const char*string, size_t length)
{
	return extractUnsignedDecType<unsigned long long>(string,length);
}


unsigned long long      extractUnsignedHex64(const char*string)
{
	return extractUnsignedHexType<unsigned long long>(string);
}

unsigned long long      extractUnsignedHex64(const char*string, size_t length)
{
	return extractUnsignedHexType<unsigned long long>(string,length);
}



template <typename T>
	static T extractFloatType(const char*string)
	{
		bool negative = string[0]=='-';
		bool offset = negative || string[0] == '+';
		const char*c = string;
		if (offset)
			c++;
		T value=0;
		while (*c >= '0' && *c <= '9')
		{
			value*=10;
			value+=(unsigned char)*c-'0';
			c++;
		}
		if (*c==DecimalSeparator)
		{
			int sub_level=1;
			c++;
			while (*c >= '0' && *c <= '9')
			{
				value+=((T)(unsigned char)*c-'0')/pow(T(10),sub_level);
				c++;
				sub_level++;
			}
		}
		if (*c=='e' || *c=='E')
		{
			c++;
			int exponent = extractDec(c);
			value *= pow(T(10),exponent);
		}
		if (negative)
			value*=-1;
		return value;
	}

template <typename T>
	static T extractFloatType(const char*string, size_t length)
	{
		if (!length)
			return 0;
		bool negative = string[0]=='-';
		bool offset = negative || string[0] == '+';
		const char*c = string;
		if (offset)
		{
			c++;
			length--;
			if (!length)
				return 0;
		}
		T value=0;
		const char*end = c+length;
		while (c < end && *c >= '0' && *c <= '9')
		{
			value*=10;
			value+=(unsigned char)*c-'0';
			c++;
		}
		if (c < end && *c==DecimalSeparator)
		{
			int sub_level=1;
			c++;
			while (c < end && *c >= '0' && *c <= '9')
			{
				value+=((T)(unsigned char)*c-'0')/pow(T(10),sub_level);
				c++;
				sub_level++;
			}
		}
		if (c < end && (*c=='e' || *c=='E'))
		{
			c++;
			int exponent = extractDec(c,end-c);
			value *= pow(T(10),exponent);
		}
		if (negative)
			value*=-1;
		return value;
	}


float    extractFloat(const char*string)
{
	return extractFloatType<float>(string);
}


float    extractFloat(const char*string, size_t length)
{
	return extractFloatType<float>(string,length);
}


double    extractDouble(const char*string)
{
	return extractFloatType<double>(string);
}

double    extractDouble(const char*string, size_t length)
{
	return extractFloatType<double>(string,length);
}


long double    extractLongDouble(const char*string)
{
    bool negative = string[0]=='-';
	bool offset = negative || string[0] == '+';
	const char*c = string;
	if (offset)
		c++;
    long double value=0;
    while (*c >= '0' && *c <= '9')
    {
        value*=10;
        value+=(unsigned char)*c-'0';
        c++;
    }
    if (*c==DecimalSeparator)
    {
        int sub_level=1;
        c++;
		while (*c >= '0' && *c <= '9')
        {
            value+=((long double)(unsigned char)*c-'0')/powl(10.0,sub_level);
            c++;
            sub_level++;
        }
    }
	if (*c=='e' || *c=='E')
	{
		c++;
		int exponent = extractDec(c);
		value *= powl(10.0,exponent);
	}
    if (negative)
		value*=-1;
    return value;
}


long double    extractLongDouble(const char*string, size_t length)
{
	if (!length)
		return 0;
    bool negative = string[0]=='-';
	bool offset = negative || string[0] == '+';
	const char*c = string;
	if (offset)
	{
		c++;
		length--;
		if (!length)
			return 0;
	}
    long double value=0;
	const char*end = string+length;
    while (c < end && *c >= '0' && *c <= '9')
    {
        value*=10;
        value+=(unsigned char)*c-'0';
        c++;
    }
    if (c < end && *c==DecimalSeparator)
    {
        int sub_level=1;
        c++;
		while (c < end && *c >= '0' && *c <= '9')
        {
            value+=((long double)(unsigned char)*c-'0')/powl(10.0,sub_level);
            c++;
            sub_level++;
        }
    }
	if (c < end && (*c=='e' || *c=='E'))
	{
		c++;
		int exponent = extractDec(c,end-c);
		value *= powl(10.0,exponent);
	}
    if (negative)
		value*=-1;
    return value;
}



bool convertToKey(const char*string, Key::Name*Out)
{
    for (unsigned i = 0; i < ARRAYSIZE(K_ID); i++)
        if (!strcmpi(K_ID[i].name,string))
        {
            (*Out) = K_ID[i].index;
            return true;
        }
    return false;
}

bool convertToKey(const char*string, size_t length, Key::Name*Out)
{
    for (unsigned i = 0; i < ARRAYSIZE(K_ID); i++)
	{
        if (!strncmpi(K_ID[i].name,string,length) && K_ID[i].name[length] == 0)
        {
            (*Out) = K_ID[i].index;
            return true;
        }
	}
    return false;
}

bool convertToLong(const char*string, long*OutInt)
{
    if (!isInt(string))
        return false;
	if (sizeof(long)==4)
		(*OutInt) = (long)extractInt(string);
	else
		(*OutInt) = (long)extractInt64(string);
    return true;
}

bool convertToLong(const char*string, size_t length, long*OutInt)
{
    if (!segIsInt(string,length))
        return false;
	if (sizeof(long)==4)
		(*OutInt) = (long)extractInt(string,length);
	else
		(*OutInt) = (long)extractInt64(string,length);
    return true;
}


bool convertToInt64(const char*string, long long*OutInt)
{
    if (!isInt(string))
        return false;
    (*OutInt) = extractInt64(string);
    return true;
}

bool convertToInt64(const char*string, size_t length, long long*OutInt)
{
    if (!segIsInt(string,length))
        return false;
    (*OutInt) = extractInt64(string,length);
    return true;
}

bool convertToUnsignedInt64(const char*string, unsigned long long*OutInt)
{
    if (!isInt(string,false))
        return false;
    (*OutInt) = extractUnsignedInt64(string);
    return true;
}

bool convertToUnsignedInt64(const char*string, size_t length, unsigned long long*OutInt)
{
    if (!segIsInt(string,length,false))
        return false;
    (*OutInt) = extractUnsignedInt64(string,length);
    return true;
}



bool convertToInt(const char*string, int*OutInt)
{
    if (!isInt(string))
        return false;
    (*OutInt) = extractInt(string);
    return true;
}

bool convertToInt(const char*string, size_t length, int*OutInt)
{
    if (!segIsInt(string,length))
        return false;
    (*OutInt) = extractInt(string,length);
    return true;
}

bool convertToShort(const char*string, short*OutShort)
{
    if (!isInt(string))
        return false;
	int rs = extractInt(string);
	if (rs > 32767 || rs < -32768)
		return false;
	(*OutShort) = (short)rs;
    return true;
}

bool convertToShort(const char*string, size_t length, short*OutShort)
{
    if (!segIsInt(string,length))
        return false;
	int rs = extractInt(string,length);
	if (rs > 32767 || rs < -32768)
		return false;
    (*OutShort) = (short)rs;
    return true;
}


bool convertToChar(const char*string, char*OutChar)
{
    if (!isInt(string))
        return false;
	int rs = extractInt(string);
	if (rs > 127 || rs < -128)
		return false;
    (*OutChar) = (char)rs;
    return true;
}

bool convertToChar(const char*string, size_t length, char*OutChar)
{
    if (!segIsInt(string,length))
        return false;
	int rs = extractInt(string,length);
	if (rs > 127 || rs < -128)
		return false;
    (*OutChar) = (char)rs;
    return true;
}


bool convertToUnsigned(const char*string, unsigned*Out)
{
    if (!isInt(string,false))
        return false;
    (*Out) = extractUnsigned(string);
    return true;
}

bool convertToUnsigned(const char*string, size_t length, unsigned*Out)
{
    if (!segIsInt(string,length,false))
        return false;
    (*Out) = extractUnsigned(string,length);
    return true;
}


bool convertToUnsignedShort(const char*string, unsigned short*OutShort)
{
    if (!isInt(string,false))
        return false;
	unsigned rs = extractUnsigned(string);
	if (rs > 65535)
		return false;
    (*OutShort) = (unsigned short)rs;
    return true;
}

bool convertToUnsignedShort(const char*string, size_t length, unsigned short*OutShort)
{
    if (!segIsInt(string,length,false))
        return false;
	unsigned rs = extractUnsigned(string,length);
	if (rs > 65535)
		return false;
    (*OutShort) = (unsigned short)rs;
    return true;
}

bool convertToByte(const char*string, unsigned char*Out)
{
    if (!isInt(string,false))
        return false;
	unsigned rs = extractUnsigned(string);
	if (rs > 255)
		return false;
    (*Out) = (BYTE)rs;
    return true;
}

bool convertToByte(const char*string, size_t length, unsigned char*Out)
{
    if (!segIsInt(string,length,false))
        return false;
	unsigned rs = extractUnsigned(string,length);
	if (rs > 255)
		return false;
    (*Out) = (BYTE)rs;
    return true;
}

bool convertToFloat(const char*string, float*OutFloat)
{
	bool offset = string[0] == '-' || string[0] == '+';
    if (!isFloat(string+offset))
	{
		if (!strcmpi(string+offset,"inf"))
		{
			*OutFloat = offset && string[0] == '-'?-std::numeric_limits<float>::infinity():std::numeric_limits<float>::infinity();
			return true;
		}
        return false;
	}
    (*OutFloat) = (float)extractFloat(string);
    return true;
}

bool convertToFloat(const char*string, size_t length, float*OutFloat)
{
	if (!length)
		return false;
	bool offset = string[0] == '-' || string[0] == '+';
    if (!segIsFloat(string+offset,length-offset))
	{
		if (length-offset==3 && !strncmpi(string+offset,"inf",3))
		{
			*OutFloat = offset && string[0] == '-'?-std::numeric_limits<float>::infinity():std::numeric_limits<float>::infinity();
			return true;
		}
        return false;
	}
    (*OutFloat) = extractFloat(string,length);
    return true;
}


bool convertToDouble(const char*string, double*OutDouble)
{
    bool offset = string[0] == '-' || string[0] == '+';
    if (!isFloat(&string[offset]))
	{
		if (!strcmpi(string+offset,"inf"))
		{
			*OutDouble = offset && string[0] == '-'?-std::numeric_limits<double>::infinity():std::numeric_limits<double>::infinity();
			return true;
		}	
        return false;
	}
    (*OutDouble) = extractDouble(string);
    return true;
}


bool convertToDouble(const char*string, size_t length, double*OutDouble)
{
	if (!length)
		return false;
    bool offset = string[0] == '-' || string[0] == '+';
    if (!segIsFloat(string+offset,length-offset))
	{
		if (length-offset == 3 && !strncmpi(string+offset,"inf",3))
		{
			*OutDouble = offset && string[0] == '-'?-std::numeric_limits<double>::infinity():std::numeric_limits<double>::infinity();
			return true;
		}	
        return false;
	}
    (*OutDouble) = extractDouble(string,length);
    return true;
}



bool convertToLongDouble(const char*string, long double*OutDouble)
{
    bool offset = string[0] == '-' || string[0] == '+';
    if (!isFloat(&string[offset]))
	{
		if (!strcmpi(string+offset,"inf"))
		{
			*OutDouble = offset && string[0] == '-'?-std::numeric_limits<double>::infinity():std::numeric_limits<double>::infinity();
			return true;
		}	
        return false;
	}
    (*OutDouble) = extractLongDouble(string);
    return true;
}

bool convertToLongDouble(const char*string, size_t length, long double*OutDouble)
{
    bool offset = string[0] == '-' || string[0] == '+';
    if (!segIsFloat(string + offset,length-offset))
	{
		if (length-offset==3 && !strncmpi(string+offset,"inf",3))
		{
			*OutDouble = offset && string[0] == '-'?-std::numeric_limits<long double>::infinity():std::numeric_limits<long double>::infinity();
			return true;
		}	
        return false;
	}
    (*OutDouble) = extractLongDouble(string,length);
    return true;
}



bool	isBool(const char*string)
{
    for (unsigned i = 0; i < ARRAYSIZE(Yes); i++)
        if (!strcmpi(Yes[i],string))
            return true;
        else
            if (!strcmpi(No[i],string))
                return true;
    if (!strcmpi(string,"TOGGLE"))
        return true;
    return false;
}

bool	isBool(const char*string, size_t length)
{
    for (unsigned i = 0; i < ARRAYSIZE(Yes); i++)
        if (!strncmpi(Yes[i],string,length))
            return true;
        else
            if (!strncmpi(No[i],string,length))
                return true;
    if (!strncmpi(string,"TOGGLE",length))
        return true;
    return false;
}



bool convertToBool(const char*string, bool*OutBool)
{
    for (unsigned i = 0; i < ARRAYSIZE(Yes); i++)
        if (!strcmpi(Yes[i],string))
        {
            (*OutBool) = true;
            return true;
        }
        else
            if (!strcmpi(No[i],string))
            {
                (*OutBool) = false;
                return true;
            }
    if (!strcmpi(string,"TOGGLE"))
    {
        (*OutBool) = !(*OutBool);
        return true;
    }
    return false;
}

bool convertToBool(const char*string, size_t length, bool*OutBool)
{
    for (unsigned i = 0; i < ARRAYSIZE(Yes); i++)
        if (!strncmpi(Yes[i],string,length))
        {
            (*OutBool) = true;
            return true;
        }
        else
            if (!strncmpi(No[i],string,length))
            {
                (*OutBool) = false;
                return true;
            }
    if (!strncmpi(string,"TOGGLE",length))
    {
        (*OutBool) = !(*OutBool);
        return true;
    }
    return false;
}














bool convertToKey(const char*string, Key::Name&out)
{
	return convertToKey(string,&out);
}

bool convertToKey(const char*string, size_t length, Key::Name&out)
{
	return convertToKey(string,length,&out);
}

bool convertToLong(const char*string, long&OutInt)
{
    if (!isInt(string))
        return false;
	if (sizeof(long)==4)
		OutInt = (long)extractInt(string);
	else
		OutInt = (long)extractInt64(string);
    return true;
}

bool convertToLong(const char*string, size_t length, long&OutInt)
{
    if (!segIsInt(string,length))
        return false;
	if (sizeof(long)==4)
		OutInt = (long)extractInt(string,length);
	else
		OutInt = (long)extractInt64(string,length);
    return true;
}


bool convertToInt64(const char*string, long long&OutInt)
{
    if (!isInt(string))
        return false;
    OutInt = extractInt64(string);
    return true;
}

bool convertToInt64(const char*string, size_t length, long long&OutInt)
{
    if (!segIsInt(string,length))
        return false;
    OutInt = extractInt64(string,length);
    return true;
}


bool convertToUnsignedInt64(const char*string, unsigned long long&OutInt)
{
    if (!isInt(string,false))
        return false;
    OutInt = extractUnsignedInt64(string);
    return true;
}

bool convertToUnsignedInt64(const char*string, size_t length, unsigned long long&OutInt)
{
    if (!segIsInt(string,length,false))
        return false;
    OutInt = extractUnsignedInt64(string,length);
    return true;
}


bool convertToInt(const char*string, int&OutInt)
{
    if (!isInt(string))
        return false;
    OutInt = extractInt(string);
    return true;
}

bool convertToInt(const char*string, size_t length, int&OutInt)
{
    if (!segIsInt(string,length))
        return false;
    OutInt = extractInt(string,length);
    return true;
}

bool convertToShort(const char*string, short&OutShort)
{
    if (!isInt(string))
        return false;
	int rs = extractInt(string);
	if (rs > 32767 || rs < -32768)
		return false;
	OutShort = (short)rs;
    return true;
}

bool convertToShort(const char*string, size_t length, short&OutShort)
{
    if (!segIsInt(string,length))
        return false;
	int rs = extractInt(string,length);
	if (rs > 32767 || rs < -32768)
		return false;
    OutShort = (short)rs;
    return true;
}


bool convertToChar(const char*string, char&OutChar)
{
    if (!isInt(string))
        return false;
	int rs = extractInt(string);
	if (rs > 127 || rs < -128)
		return false;
    OutChar = (char)rs;
    return true;
}

bool convertToChar(const char*string, size_t length, char&OutChar)
{
    if (!segIsInt(string,length))
        return false;
	int rs = extractInt(string,length);
	if (rs > 127 || rs < -128)
		return false;
    OutChar = (char)rs;
    return true;
}


bool convertToUnsigned(const char*string, unsigned&Out)
{
    if (!isInt(string,false))
        return false;
    Out = extractUnsigned(string);
    return true;
}

bool convertToUnsigned(const char*string, size_t length, unsigned&Out)
{
    if (!segIsInt(string,length,false))
        return false;
    Out = extractUnsigned(string,length);
    return true;
}


bool convertToUnsignedShort(const char*string, unsigned short&OutShort)
{
    if (!isInt(string,false))
        return false;
	unsigned rs = extractUnsigned(string);
	if (rs > 65535)
		return false;
    OutShort = (unsigned short)rs;
    return true;
}

bool convertToUnsignedShort(const char*string, size_t length, unsigned short&OutShort)
{
    if (!segIsInt(string,length,false))
        return false;
	unsigned rs = extractUnsigned(string,length);
	if (rs > 65535)
		return false;
    OutShort = (unsigned short)rs;
    return true;
}

bool convertToByte(const char*string, unsigned char&Out)
{
    if (!isInt(string,false))
        return false;
	unsigned rs = extractUnsigned(string);
	if (rs > 255)
		return false;
    Out = (BYTE)rs;
    return true;
}

bool convertToByte(const char*string, size_t length, unsigned char&Out)
{
    if (!segIsInt(string,length,false))
        return false;
	unsigned rs = extractUnsigned(string,length);
	if (rs > 255)
		return false;
    Out = (BYTE)rs;
    return true;
}

bool convertToFloat(const char*string, float&OutFloat)
{
	bool offset = string[0] == '-' || string[0] == '+';
    if (!isFloat(string+offset))
	{
		if (!strcmpi(string+offset,"inf"))
		{
			OutFloat = offset && string[0] == '-'?-std::numeric_limits<float>::infinity():std::numeric_limits<float>::infinity();
			return true;
		}
        return false;
	}
    OutFloat = (float)extractFloat(string);
    return true;
}

bool convertToFloat(const char*string, size_t length, float&OutFloat)
{
	if (!length)
		return false;
	bool offset = string[0] == '-' || string[0] == '+';
    if (!segIsFloat(string+offset,length-offset))
	{
		if (length-offset==3 && !strncmpi(string+offset,"inf",3))
		{
			OutFloat = offset && string[0] == '-'?-std::numeric_limits<float>::infinity():std::numeric_limits<float>::infinity();
			return true;
		}
        return false;
	}
    OutFloat = extractFloat(string,length);
    return true;
}


bool convertToDouble(const char*string, double&OutDouble)
{
    bool offset = string[0] == '-' || string[0] == '+';
    if (!isFloat(&string[offset]))
	{
		if (!strcmpi(string+offset,"inf"))
		{
			OutDouble = offset && string[0] == '-'?-std::numeric_limits<double>::infinity():std::numeric_limits<double>::infinity();
			return true;
		}	
        return false;
	}
    OutDouble = extractDouble(string);
    return true;
}


bool convertToDouble(const char*string, size_t length, double&OutDouble)
{
	if (!length)
		return false;
    bool offset = string[0] == '-' || string[0] == '+';
    if (!segIsFloat(string+offset,length-offset))
	{
		if (length-offset == 3 && !strncmpi(string+offset,"inf",3))
		{
			OutDouble = offset && string[0] == '-'?-std::numeric_limits<double>::infinity():std::numeric_limits<double>::infinity();
			return true;
		}	
        return false;
	}
    OutDouble = extractDouble(string,length);
    return true;
}



bool convertToLongDouble(const char*string, long double&OutDouble)
{
    bool offset = string[0] == '-' || string[0] == '+';
    if (!isFloat(&string[offset]))
	{
		if (!strcmpi(string+offset,"inf"))
		{
			OutDouble = offset && string[0] == '-'?-std::numeric_limits<long double>::infinity():std::numeric_limits<long double>::infinity();
			return true;
		}	
        return false;
	}
    OutDouble = extractLongDouble(string);
    return true;
}

bool convertToLongDouble(const char*string, size_t length, long double&OutDouble)
{
    bool offset = string[0] == '-' || string[0] == '+';
    if (!segIsFloat(string + offset,length-offset))
	{
		if (length-offset==3 && !strncmpi(string+offset,"inf",3))
		{
			OutDouble = offset && string[0] == '-'?-std::numeric_limits<long double>::infinity():std::numeric_limits<long double>::infinity();
			return true;
		}	
        return false;
	}
    OutDouble = extractLongDouble(string,length);
    return true;
}





bool convertToBool(const char*string, bool&OutBool)
{
    for (unsigned i = 0; i < ARRAYSIZE(Yes); i++)
        if (!strcmpi(Yes[i],string))
        {
            OutBool = true;
            return true;
        }
        else
            if (!strcmpi(No[i],string))
            {
                OutBool = false;
                return true;
            }
    if (!strcmpi(string,"TOGGLE"))
    {
        OutBool = !OutBool;
        return true;
    }
    return false;
}

bool convertToBool(const char*string, size_t length, bool&OutBool)
{
    for (unsigned i = 0; i < ARRAYSIZE(Yes); i++)
        if (!strncmpi(Yes[i],string,length))
        {
            OutBool = true;
            return true;
        }
        else
            if (!strncmpi(No[i],string,length))
            {
                OutBool = false;
                return true;
            }
    if (!strncmpi(string,"TOGGLE",length))
    {
        OutBool = !OutBool;
        return true;
    }
    return false;
}

