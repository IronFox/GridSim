#include "../global_root.h"
#include "basic.h"

/******************************************************************

Collection of basic template functions.


******************************************************************/


//float      GLOBAL_BUFFER[BUFFER_SIZE];
//double    *GLOBAL_D_BUFFER = (double*)GLOBAL_BUFFER;

static char*    output_str(NULL);
static unsigned output_len(0);
//static char     conversion_buffer[0x100];

const char* TypeInfo<bool>::name = "Bool";

const char* TypeInfo<INT8>::name = "Int8";

const char* TypeInfo<INT16>::name = "Int16";

const char* TypeInfo<INT64>::name = "Int64";

const char* TypeInfo<INT32>::name = "Int32";

const char* TypeInfo<UINT8>::name = "UInt8";

const char* TypeInfo<UINT16>::name = "UInt16";

const char* TypeInfo<UINT32>::name = "UInt32";

const char* TypeInfo<UINT64>::name = "UInt64";

const char* TypeInfo<LONG>::name = "Long";

const char* TypeInfo<ULONG>::name = "ULong";


const float	TypeInfo<float>::zero = 0.0f;
const float	TypeInfo<float>::min = -3.4e38f;
const float	TypeInfo<float>::max = 3.4e38f;
const float	TypeInfo<float>::error = FLT_EPSILON;
const char* TypeInfo<float>::name = "Float32";
//const float TypeInfo<float>::undefined = _Nan._Float;

const double	TypeInfo<double>::zero = 0.0;
const double	TypeInfo<double>::min = 1.7e308;
const double	TypeInfo<double>::max = -1.7e308;
const double	TypeInfo<double>::error = DBL_EPSILON;
const char*		TypeInfo<double>::name = "Float64";
//const double	TypeInfo<double>::undefined = _Nan._Double;

const long double	TypeInfo<long double>::zero = 0.0;
const long double	TypeInfo<long double>::min = 1.7e308;
const long double	TypeInfo<long double>::max = -1.7e308;
const long double	TypeInfo<long double>::error = LDBL_EPSILON;
const char*		TypeInfo<long double>::name = "Float80";
//const long double	TypeInfo<long double>::undefined = _Nan._Double;




namespace Math
{
	// Transforms the subnormal representation to a normalized one. 
	UINT THalf::Helper::ConvertMantissa(UINT i)
	{
		UINT m = (UINT)(i << 13); // Zero pad mantissa bits
		UINT e = 0; // Zero exponent

		// While not normalized
		while ((m & 0x00800000) == 0)
		{
			e -= 0x00800000; // Decrement exponent (1<<23)
			m <<= 1; // Shift mantissa                
		}
		m &= ((UINT)~0x00800000); // Clear leading 1 bit
		e += 0x38800000; // Adjust bias ((127-14)<<23)
		return m | e; // Return combined number
	}

	void THalf::Helper::FillMantissaTable()
	{
		//uint[] mantissaTable = new uint[2048];
		mantissaTable[0] = 0;
		for (UINT i = 1; i < 1024; i++)
		{
			mantissaTable[i] = ConvertMantissa(i);
		}
		for (UINT i = 1024; i < 2048; i++)
		{
			mantissaTable[i] = (UINT)(0x38000000 + ((i - 1024) << 13));
		}

		//return mantissaTable;
	}
	void THalf::Helper::FillExponentTable()
	{
		//uint[] exponentTable = new uint[64];
		exponentTable[0] = 0;
		for (UINT i = 1; i < 31; i++)
		{
			exponentTable[i] = (i << 23);
		}
		exponentTable[31] = 0x47800000;
		exponentTable[32] = 0x80000000;
		for (UINT i = 33; i < 63; i++)
		{
			exponentTable[i] = (UINT)(0x80000000 + ((i - 32) << 23));
		}
		exponentTable[63] = 0xc7800000;

		//return exponentTable;
	}
	void THalf::Helper::FillOffsetTable()
	{
		//ushort[] offsetTable = new ushort[64];
		offsetTable[0] = 0;
		for (UINT i = 1; i < 32; i++)
		{
			offsetTable[i] = 1024;
		}
		offsetTable[32] = 0;
		for (UINT i = 33; i < 64; i++)
		{
			offsetTable[i] = 1024;
		}

		//return offsetTable;
	}
	void THalf::Helper::FillBaseTable()
	{
		//ushort[] baseTable = new ushort[512];
		for (int i = 0; i < 256; ++i)
		{
			char e = (char)(127 - i);
			if (e > 24)
			{ // Very small numbers map to zero
				baseTable[i | 0x000] = 0x0000;
				baseTable[i | 0x100] = 0x8000;
			}
			else if (e > 14)
			{ // Small numbers map to denorms
				baseTable[i | 0x000] = (USHORT)(0x0400 >> (18 + e));
				baseTable[i | 0x100] = (USHORT)((0x0400 >> (18 + e)) | 0x8000);
			}
			else if (e >= -15)
			{ // Normal numbers just lose precision
				baseTable[i | 0x000] = (USHORT)((15 - e) << 10);
				baseTable[i | 0x100] = (USHORT)(((15 - e) << 10) | 0x8000);
			}
			else if (e > -128)
			{ // Large numbers map to Infinity
				baseTable[i | 0x000] = 0x7c00;
				baseTable[i | 0x100] = 0xfc00;
			}
			else
			{ // Infinity and NaN's stay Infinity and NaN's
				baseTable[i | 0x000] = 0x7c00;
				baseTable[i | 0x100] = 0xfc00;
			}
		}

		//return baseTable;
	}
	void THalf::Helper::FillShiftTable()
	{
		//sbyte[] shiftTable = new sbyte[512];
		for (int i = 0; i < 256; ++i)
		{
			char e = (char)(127 - i);
			if (e > 24)
			{ // Very small numbers map to zero
				shiftTable[i | 0x000] = 24;
				shiftTable[i | 0x100] = 24;
			}
			else if (e > 14)
			{ // Small numbers map to denorms
				shiftTable[i | 0x000] = (char)(e - 1);
				shiftTable[i | 0x100] = (char)(e - 1);
			}
			else if (e >= -15)
			{ // Normal numbers just lose precision
				shiftTable[i | 0x000] = 13;
				shiftTable[i | 0x100] = 13;
			}
			else if (e > -128)
			{ // Large numbers map to Infinity
				shiftTable[i | 0x000] = 24;
				shiftTable[i | 0x100] = 24;
			}
			else
			{ // Infinity and NaN's stay Infinity and NaN's
				shiftTable[i | 0x000] = 13;
				shiftTable[i | 0x100] = 13;
			}
		}

		//return shiftTable;
	}


	THalf::Helper THalf::helper;

	THalf	THalf::min = THalf::Interpret(0x0001),
			THalf::max = THalf::Interpret(0x7BFF),
			THalf::epsilon = THalf::Interpret(0x1400),	//should be 2^-10
			THalf::infinity = THalf::Interpret(0x7c00),
			THalf::negativeInfinity = THalf::Interpret(0xfc00),
			THalf::quietNaN = THalf::Interpret(0x7E00),
			THalf::signalingNaN = THalf::Interpret(0x7DFF);


}

const THalf	TypeInfo<THalf>::zero = THalf::Make(0.f);
const THalf	TypeInfo<THalf>::min = -THalf::max;
const THalf	TypeInfo<THalf>::max = THalf::max;
const THalf	TypeInfo<THalf>::error = THalf::epsilon;
const char* TypeInfo<THalf>::name = "Float16";
//const float TypeInfo<float>::undefined = _Nan._Float;


char*Math::makeStr(unsigned len)
{
    unsigned mod = (len/0x100);
    if (len%0x100)
        mod++;
    if (mod == output_len)
        return output_str;
    dealloc(output_str);
    output_str = alloc<char>(mod*0x100);
    output_len = mod;
    return output_str;
}

void Math::dropStr()
{
    dealloc(output_str);
}




float Math::belowOne(float f)
{
    return f<=1?f:1;
}

//
//
//#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__GNUC__)
//    float Math::round(float f)
//    {
//        float a = (float)floor(f);
//        if (a+0.5f < f)
//            a++;
//        return a;
//    }
//#endif
//
//
//

double Math::dRound(double f)
{
    double a = (float)floor(f);
    if (a+0.49999 <= f)
        a++;
    return a;
}

#if 0

String Math::float2str(double value, BYTE exactness)
{
	return value;
    if (value > INT_MAX||value < INT_MIN)
        return "exceeded";
    if (isnan(value))
        return "NAN";
    char first[256],
        *end = first+sizeof(first)-1;
    bool negative = value < 0;
    if (negative)
        value*=-1;
    if (pow(10.0f,exactness)*value > INT_MAX)
        exactness = (BYTE)log10(INT_MAX/value);
    char*c = end;
    (*c) = 0;
    UINT64 v = (UINT64)(value*pow(10.0f,exactness));
    unsigned at(0);
    bool write = !exactness;
    while (v && c != first)
    {
        char ch = '0'+(v%10);
        write = write || ch != '0' || at==(unsigned)exactness;
        if (write)
            (*--c) = ch;
        v/=10;
        at++;
        if (at == (unsigned)exactness && c != first && write)
            (*--c) = DecimalSeparator;
    }
    while (at < (unsigned)exactness && c != first)
    {
        (*--c) = '0';
        at++;
    }
    if (at == (unsigned)exactness && c != first)
    {
        if ((*c) != DecimalSeparator)
            (*--c) = DecimalSeparator;
        if (c != first)
            (*--c) = '0';
    }
    if (negative && c != first)
        (*--c) = '-';
    return c;
}




String Math::floatStr(float value, bool force)
{
    if (value > INT_MAX||value < INT_MIN)
        return "exceeded";
    if (isnan(value))
        return "NAN";
    char first[256],
        *end = first+sizeof(first)-1;
    bool negative = value < 0;
    if (negative)
        value*=-1;
    unsigned exactness = 3;
    if (pow(10.0f,(int)exactness)*value > INT_MAX)
        exactness = (BYTE)log10(INT_MAX/value);
    char*c = end;
    (*c) = 0;
    UINT64 v = (UINT64)(value*pow(10.0f,(int)exactness));
    unsigned at(0);
    bool write = force;
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
    return c;
}

String Math::doubleStr(double value, bool force)
{
    if (value > INT_MAX||value < INT_MIN)
        return "exceeded";
    if (isnan(value))
        return "NAN";
    char first[256],
        *end = first+sizeof(first)-1;
    bool negative = value < 0;
    if (negative)
        value*=-1;
    unsigned exactness = 3;
    if (pow(10.0f,(int)exactness)*value > INT_MAX)
        exactness = (BYTE)log10(INT_MAX/value);
    char*c = end;
    (*c) = 0;
    UINT64  v = (UINT64)(value*pow(10.0f,(int)exactness));
    unsigned at(0);
    bool write = force;
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
    return c;
}
#endif
