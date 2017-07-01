#pragma once

#include "string_converter.h"
#include "../global_string.h"

namespace DeltaWorks
{
	template <typename T>
		inline bool	Convert(const String&source, T&numericTarget)
		{
			return convert(source.c_str(),source.GetLength(),numericTarget);
		}

	template <>
		inline bool	Convert<String>(const String&source, String&stringTarget)
		{
			stringTarget = source;
		}

	template <typename T>
		inline bool	Convert(const StringRef&source, T&numericTarget)
		{
			return convert(source.pointer(),source.GetLength(),numericTarget);
		}

	template <>
		inline bool	Convert<String>(const StringRef&source, String&stringTarget)
		{
			stringTarget = source;
		}
}


