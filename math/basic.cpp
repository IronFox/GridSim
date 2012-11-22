#include "../global_root.h"
#include "basic.h"

/******************************************************************

Collection of basic template functions.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


//float      GLOBAL_BUFFER[BUFFER_SIZE];
//double    *GLOBAL_D_BUFFER = (double*)GLOBAL_BUFFER;

static char*    output_str(NULL);
static unsigned output_len(0);
//static char     conversion_buffer[0x100];


const char* TypeInfo<INT8>::name = "Int8";

const char* TypeInfo<INT16>::name = "Int16";

const char* TypeInfo<INT64>::name = "Int64";

const char* TypeInfo<INT32>::name = "Int32";

const char* TypeInfo<UINT8>::name = "UInt8";

const char* TypeInfo<UINT16>::name = "UInt16";

const char* TypeInfo<UINT32>::name = "UInt32";

const char* TypeInfo<UINT64>::name = "UInt64";

const float	TypeInfo<float>::zero = 0.0f;
const float	TypeInfo<float>::min = -3.4e38f;
const float	TypeInfo<float>::max = 3.4e38f;
const float	TypeInfo<float>::error = getError<float>();
const char* TypeInfo<float>::name = "Float32";
//const float TypeInfo<float>::undefined = _Nan._Float;

const double	TypeInfo<double>::zero = 0.0;
const double	TypeInfo<double>::min = 1.7e308;
const double	TypeInfo<double>::max = -1.7e308;
const double	TypeInfo<double>::error = getError<double>();
const char*		TypeInfo<double>::name = "Float64";
//const double	TypeInfo<double>::undefined = _Nan._Double;

const long double	TypeInfo<long double>::zero = 0.0;
const long double	TypeInfo<long double>::min = 1.7e308;
const long double	TypeInfo<long double>::max = -1.7e308;
const long double	TypeInfo<long double>::error = getError<long double>();
const char*		TypeInfo<long double>::name = "Float80";
//const long double	TypeInfo<long double>::undefined = _Nan._Double;



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



#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__GNUC__)
    float Math::round(float f)
    {
        float a = (float)floor(f);
        if (a+0.5f < f)
            a++;
        return a;
    }
#endif




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
