#ifndef ito_stringH
#define ito_stringH

#include "common.h"

template <typename T>
	class StringTemplate;

/**
	@brief String convertible object
	
	IToString provides an abstract super type for all objects that can implicitly be converted into a string.
*/
interface IToString
{
protected:
virtual								~IToString()	{};
public:
virtual	StringTemplate<char>		toString()	const=0;	//!< Converts the local object into a string to display to the user
};

#include "../string/str_class.h"


#endif
