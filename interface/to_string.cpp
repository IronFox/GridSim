#include "to_string.h"


StringType::Template<char>	StringConversion::ToString(const IToString&str)
{
	index_t index = 4;
	DBG_ASSERT_LESS__(index,4);


	return str.ConvertToString();
}


