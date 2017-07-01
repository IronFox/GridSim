#ifndef ito_stringH
#define ito_stringH

#include "common.h"

namespace DeltaWorks
{

	namespace StringType
	{
		template <typename T>
			class Template;
	}

	/**
	@brief String convertible object
	
	IToString provides an abstract super type for all objects that can implicitly be converted into a string.
	*/
	interface IToString
	{
	protected:
		virtual								~IToString()	{};
	public:
		virtual	StringType::Template<char>	ConvertToString()	const=0;	//!< Converts the local object into a string to display to the user
	};

}
#include "../string/str_class.h"

namespace DeltaWorks
{

	namespace StringConversion
	{
		StringType::Template<char>	ToString(const IToString&str);
	}
}

#endif
