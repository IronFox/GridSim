#ifndef string_converterH
#define string_converterH

/******************************************************************

Collection of string-to-type parsers.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include "keys.h"
#include <math.h>

#if SYSTEM==UNIX && !defined(strncmpi)
    #define strncmpi    strncasecmp
    #define strcmpi     strcasecmp
#endif


/*

Use this unit to perform standard string-to-value parsing.
Default pattern:
    bool parseValue(const char*string, *Value or &Value);

Make sure the input-string is zero-terminated. The function returns true
if the string was successfully converted and false if errors occured.
Any of the below listet functions should handle any kind of error (except
non-zero-terminated strings).
*/

#ifndef __BORLANDC__
    extern char     DecimalSeparator;
#endif


bool        isInt(const char*string, bool allow_negative=true);						//!< Checks if the specified string is a correctly formated decimal or hexadecimal integer. Hexadecimal integers are expected to begin with '0x' (following the negative sign if any) @param string String to check for compliance @param allow_negative Set true to allow a negative sign out front @return true if the specified string complies
bool		segIsInt(const char*string, size_t length, bool allow_negative=true);	//!< Identical to isInt() but only tests the specified number of characters
bool        isDec(const char*string, bool allow_negative=true);						//!< Checks if the specified string is a correctly formated decimal integer. @param string String to check for compliance @param allow_negative Set true to allow a negative sign out front @return true if the specified string complies
bool		segIsDec(const char*string, size_t length, bool allow_negative=true);	//!< Identical to isDec() but only tests the specified number of characters
bool        isHex(const char*string, bool allow_negative=true);						//!< Checks if the specified string is a correctly formated hexadecimal integer. Hexadecimal integers are expected to begin with '0x' (following the negative sign if any) @param string String to check for compliance @param allow_negative Set true to allow a negative sign out front @return true if the specified string complies
bool		segIsHex(const char*string, size_t length, bool allow_negative=true);	//!< Identical to isHex() but only tests the specified number of characters
bool        isFloat(const char*string);												//!< Checks if the specified string is a correctly formated floating point value.  @param string String to check for compliance @return true if the specified string complies
bool		segIsFloat(const char*string, size_t length);		//!< Identical to isFloat() but only tests the specified number of characters
bool	    isBool(const char*string);							//!< Checks if the specified string is an interpretable boolean value such as 'on', 'off', 'true', 'false', etc.  @param string String to check for compliance @return true if the specified string complies
bool		segIsBool(const char*string, size_t length);		//!< Identical to isBool() but only tests the specified number of characters

int         extractHex(const char*string);						//!< Extracts a common integer from a string. The function may return undesired results if the specified string is no valid hexadecimal string @param string String to extract an integer from @return Extracted integer
int         extractHex(const char*string, size_t length);		//!< @overload @param length Number of characters to parse
int         extractDec(const char*string);						//!< Extracts a common integer from a string. The function may return undesired results if the specified string is no valid decimal string @param string String to extract an integer from @return Extracted integer
int         extractDec(const char*string, size_t length);		//!< @overload @param length Number of characters to parse
int         extractInt(const char*string);						//!< Extracts a common integer from a string. The function may return undesired results if the specified string is neither a valid hexadecimal nor decimal string @param string String to extract an integer from @return Extracted integer
int         extractInt(const char*string, size_t length);		//!< @overload @param length Number of characters to parse
unsigned    extractUnsignedHex(const char*string);						//!< Extracts a common unsigned integer from a string. The function may return undesired results if the specified string is no valid hexadecimal string @param string String to extract an integer from @return Extracted integer
unsigned    extractUnsignedHex(const char*string, size_t length);		//!< @overload @param length Number of characters to parse
unsigned    extractUnsignedDec(const char*string);						//!< Extracts a common unsigned integer from a string. The function may return undesired results if the specified string is no valid decimal string @param string String to extract an integer from @return Extracted integer
unsigned    extractUnsignedDec(const char*string, size_t length);		//!< @overload @param length Number of characters to parse
unsigned    extractUnsignedInt(const char*string);						//!< Extracts a common unsigned integer from a string. The function may return undesired results if the specified string is neither a valid hexadecimal nor decimal string @param string String to extract an integer from @return Extracted integer
unsigned    extractUnsignedInt(const char*string, size_t length);		//!< @overload @param length Number of characters to parse
long long   extractHex64(const char*string);					//!< 64 bit version of extractHex()
long long   extractHex64(const char*string, size_t length);	//!< @overload @param length Number of characters to parse
long long   extractDec64(const char*string);					//!< 64 bit version of extractDec()
long long   extractDec64(const char*string, size_t length);	//!< @overload @param length Number of characters to parse
long long   extractInt64(const char*string);					//!< 64 bit version of extractInt()
long long   extractInt64(const char*string, size_t length);	//!< @overload @param length Number of characters to parse
unsigned long long   extractUnsignedHex64(const char*string);					//!< 64 bit version of extractUnsignedHex()
unsigned long long   extractUnsignedHex64(const char*string, size_t length);	//!< @overload @param length Number of characters to parse
unsigned long long   extractUnsignedDec64(const char*string);					//!< 64 bit version of extractUnsignedDec()
unsigned long long   extractUnsignedDec64(const char*string, size_t length);	//!< @overload @param length Number of characters to parse
unsigned long long   extractUnsignedInt64(const char*string);					//!< 64 bit version of extractUnsignedInt()
unsigned long long   extractUnsignedInt64(const char*string, size_t length);	//!< @overload @param length Number of characters to parse
float       extractFloat(const char*string);					//!< Extracts a 32 bit float from the specified string. The function may return undesired results if the specified string is not a correctly formated floating point value
float       extractFloat(const char*string, size_t length);	//!< @overload @param length Number of characters to parse
double      extractDouble(const char*string);					//!< Extracts a 64 bit float from the specified string. The function may return undesired results if the specified string is not a correctly formated floating point value
double      extractDouble(const char*string, size_t length);	//!< @overload @param length Number of characters to parse
long double extractLongDouble(const char*string);					//!< Extracts an 80 bit float from the specified string. The function may return undesired results if the specified string is not a correctly formated floating point value
long double extractLongDouble(const char*string, size_t length);	//!< @overload @param length Number of characters to parse

void        to8ByteName(const char*origin, char target[8]);
void        to8ByteName(const char*origin, __int64&target);
void        to8ByteName(const char*origin, size_t length, char target[8]);
void        to8ByteName(const char*origin, size_t length, __int64&target);

bool        convertToByte(const char*string, unsigned char*OutChar);
bool        convertToByte(const char*string, size_t length, unsigned char*OutChar);
bool        convertToChar(const char*string, char*OutChar);
bool        convertToChar(const char*string, size_t length, char*OutChar);
bool        convertToShort(const char*string, short*OutShort);
bool        convertToShort(const char*string, size_t length, short*OutShort);
bool        convertToLong(const char*string, long*OutShort);
bool        convertToLong(const char*string, size_t length, long*OutShort);
bool        convertToInt(const char*string, int*OutInt);
bool        convertToInt(const char*string, size_t length, int*OutInt);
bool        convertToInt64(const char*string, long long*OutInt);
bool        convertToInt64(const char*string, size_t length, long long*OutInt);
bool        convertToUnsignedInt64(const char*string, unsigned long long*OutInt);
bool        convertToUnsignedInt64(const char*string, size_t length, unsigned long long*OutInt);
bool        convertToLongDouble(const char*string, long double*OutDouble);
bool        convertToLongDouble(const char*string, size_t length, long double*OutDouble);
bool        convertToDouble(const char*string, double*OutDouble);
bool        convertToDouble(const char*string, size_t length, double*OutDouble);
bool        convertToFloat(const char*string, float*OutFloat);
bool        convertToFloat(const char*string, size_t length, float*OutFloat);
bool        convertToBool(const char*string, bool*OutBool);
bool        convertToBool(const char*string, size_t length, bool*OutBool);
bool        convertToUnsigned(const char*string, unsigned*Out);
bool        convertToUnsigned(const char*string, size_t length, unsigned*Out);
bool        convertToUnsignedShort(const char*string, unsigned short*OutShort);
bool        convertToUnsignedShort(const char*string, size_t length, unsigned short*OutShort);
bool        convertToKey(const char*string, Key::Name*Out);
bool        convertToKey(const char*string, size_t length, Key::Name*Out);

bool        convertToByte(const char*string, unsigned char&OutChar);
bool        convertToByte(const char*string, size_t length, unsigned char&OutChar);
bool        convertToChar(const char*string, char&OutChar);
bool        convertToChar(const char*string, size_t length, char&OutChar);
bool        convertToShort(const char*string, short&OutShort);
bool        convertToShort(const char*string, size_t length, short&OutShort);
bool        convertToLong(const char*string, long&OutShort);
bool        convertToLong(const char*string, size_t length, long&OutShort);
bool        convertToInt(const char*string, int&OutInt);
bool        convertToInt(const char*string, size_t length, int&OutInt);
bool        convertToInt64(const char*string, long long&OutInt);
bool        convertToInt64(const char*string, size_t length, long long&OutInt);
bool        convertToUnsignedInt64(const char*string, unsigned long long&OutInt);
bool        convertToUnsignedInt64(const char*string, size_t length, unsigned long long&OutInt);
bool        convertToDouble(const char*string, double&OutDouble);
bool        convertToDouble(const char*string, size_t length, double&OutDouble);
bool        convertToLongDouble(const char*string, long double&OutDouble);
bool        convertToLongDouble(const char*string, size_t length, long double&OutDouble);
bool        convertToFloat(const char*string, float&OutFloat);
bool        convertToFloat(const char*string, size_t length, float&OutFloat);
bool        convertToBool(const char*string, bool&OutBool);
bool        convertToBool(const char*string, size_t length, bool&OutBool);
bool        convertToUnsigned(const char*string, unsigned&Out);
bool        convertToUnsigned(const char*string, size_t length, unsigned&Out);
bool        convertToUnsignedShort(const char*string, unsigned short&OutShort);
bool        convertToUnsignedShort(const char*string, size_t length, unsigned short&OutShort);
bool        convertToKey(const char*string, Key::Name&Out);
bool        convertToKey(const char*string, size_t length, Key::Name&Out);

const char* resolveKeyName(Key::Name key);


template <typename T>
	inline bool	convert(const char*string, T&out);
template <typename T>
	inline bool	convert(const char*string, size_t length, T&out);

#include "string_converter.tpl.h"

#endif

