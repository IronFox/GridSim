#include "../global_root.h"
#include "int_name.h"

/******************************************************************

64-bit integer-name handler.

******************************************************************/


String    name2str(const int64_t&name)
{
    char    temp[9];
    temp[8] = 0;
    (*(int64_t*)temp) = name;
	char*end = temp+7;
	while (end != temp && *end == ' ')
		(*end--) = 0;
    return String(temp);
}

int64_t     str2name(const String&str)
{
    char    result[8];
    for (BYTE k = 0; k < 8; k++)
        if (k < (BYTE)str.length())
            result[k] = str.get(k);
        else
            result[k] = ' ';
    return *(int64_t*)result;
}

int64_t     str2name(const char*str)
{
    char    result[8],*c(result),*end(result+8);
    while (*str && c != end)
        (*c++) = (*str++);
    while (c != end)
        (*c++) = ' ';
    return *(int64_t*)result;
}


void        incrementName(int64_t&name)
{
    char	*c = (char*)&name,
			*end = c+7;
    while (end != c && *end==' ')
        end--;
    if (!isdigit(*end))
    {
		if (end-c < 7)
			(*++end) = '0';
		else
			*end = '0';
		return;
    }
	char	*digit_begin = end;
	
	while (digit_begin != c && isdigit(*digit_begin))
		digit_begin--;
	if (!isdigit(*digit_begin))
		digit_begin++;
	
	char*inc = end;
	while (inc >= digit_begin)
	{
		if (*inc != '9')
		{
			(*inc)++;
			return;
		}
		*inc-- = '0';
	}
	if (end-c < 7)
	{
		*digit_begin = '1';
		end++;
		inc = digit_begin+1;
		while (inc <= end)
			(*inc++) = '0';
		return;
	}
	if (digit_begin!=c)
		(*--digit_begin) = '1';
	return;
}
