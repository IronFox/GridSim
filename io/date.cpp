#include "../global_root.h"
#include "date.h"

Date	date;

const char*		Date::ToString(const char*format)
{
    time_t tt = time(NULL);
    const tm*t = localtime(&tt);
    strftime(buffer,(unsigned)sizeof(buffer),format,t);
	return buffer;
}

const char*		Date::ToString()
{
	return ToString("%Y %B %d. %H:%M:%S");
}

