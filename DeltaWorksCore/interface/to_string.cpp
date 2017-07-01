#include "to_string.h"

namespace DeltaWorks
{
	StringType::Template<char>	StringConversion::ToString(const IToString&str)
	{
		index_t index = 4;
		DBG_ASSERT_LESS__(index,4);


		return str.ConvertToString();
	}
}

