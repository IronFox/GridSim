#ifndef string_interfaceH
#define string_interfaceH

#include "common.h"


namespace DeltaWorks
{

	/**
	@brief Generic IString interface

	The used type 'T' must comply with the following constraints:<br>
	<ul>
	<li>T must provide implicit conversation to bool, such that 'if(var)' and 'if(!var)' are possible where var is of type T </li>
	<li>'if (var)' must trigger for all values that are not the terminal character. Conversely 'if (!var)' must not trigger for any value except the terminal character </li>
	<li>T must provide a type conversion to char and wchar_t, such that 'char(var)' and 'wchar_t(var)' are possible, where var is of type T </li>
	<li>T must provide a type constructor from char and wchar_t, such that 'T(var)' is possible, where var is of either type char or type wchar_t </li>
	<li>T must provide comparison with its own type, such that 'var0 == var1' is possible where var0 and var1 are of type T </li>
	</ul><br>
	These conditions are automatically met by any native numerical data type, including but not limited to 'char' and 'wchar_t' <br>
	Given these constraints methods such as 'T(char(var)) == var' can be used to determine whether a variable 'var' of type T is within valid range of type char
	*/
	template <typename T>
		interface IString
		{
		protected:
			virtual				~IString()	{};
		public:
			/**
			@brief Retrieves the data content of the string interface compatible object. This must be a not-NULL zero-terminated string of the implemented type

			The return string must be stored elsewhere, either globally or within the IString-Object (as mutable). It must not be deleted by any client function invoking this method.
			*/
			virtual	const T*	ToCString()	const=0;
		};





	namespace StringConverterDetail
	{
		template <typename ToType, typename FromType, size_t Size=0x200>
			struct TConvert
			{
				static inline const ToType*	convert(const FromType*pntr, ToType*field)	throw()
				{
					if (!pntr)
					{
						field[0] = 0;
						return field;
					}
					const FromType	*end = pntr;
					while (*end)
						end++;
					size_t len = end-pntr;
					if (len > Size-1)
						len = Size-1;
					for (index_t i = 0; i < len; i++)
						if (FromType(ToType(pntr[i])) == pntr[i])
							field[i] = ToType(pntr[i]);
						else
							field[i] = ToType('?');
					field[len] = 0;
					return field;
				}
			};
		template <typename SameType, size_t Size>
			struct TConvert<SameType,SameType,Size>
			{
				static inline const SameType*	convert(const SameType*pntr, SameType*field)	throw()
				{
					if (pntr)
						return pntr;
					field[0] = 0;
					return field;
				}
			};


	}


	/**
	@brief Simply string conversion method

	The converter object may be used to convert one string type into another. String data is stored in a constant size character field of the specified OutType.
	Following the constraint definition of IString, StringConverter will always work for ToType = wchar_t or ToType = char, but may work for other types too
	*/
	template <typename ToType, size_t Size=0x200>
		class StringConverter
		{
		protected:
				ToType					field[Size];

		public:
			

			/**
				@brief Converts the given zero-terminated string of type FromType into a zero-terminated string of type ToType

				Any data beyond the limitations of the internal storage field are truncated and properly terminated.

				@param pntr Zero terminated string to convert or NULL, resulting in an empty (not-NULL) string
				@return Converted string pointing to the local character storage or directly to the passed string if conversion is not required. The returned string is never NULL and always zero-terminated. Deletion of the result is not allowed
			*/
			template <typename FromType>
				inline const ToType*	convert(const FromType*pntr)	throw()
				{
					return StringConverterDetail::TConvert<ToType,FromType,Size>::convert(pntr,field);
				}

			/**
				@brief Conversion method for objects implementing the IString interface

				@param string String to convert the characters of
				@return Converted string pointing to the local character storage or directly to the result of string.ToCString() if conversion is not required. The returned string is never NULL and always zero-terminated. Deletion of the result is not allowed
			*/
			template <typename FromType>
				inline const ToType*	convert(const IString<FromType>&string)
				{
					return StringConverterDetail::TConvert<ToType,FromType,Size>::convert(string.ToCString(),field);
				}

			template <typename FromType>
				inline const ToType*	operator()(const FromType*pntr) throw()	//! Identical to resolve(pntr)
				{
					return StringConverterDetail::TConvert<ToType,FromType,Size>::convert(pntr,field);
				}

			template <typename FromType>
				inline const ToType*	operator()(const IString<FromType>&string)	//!< Identical to resolve(string)
				{
					return StringConverterDetail::TConvert<ToType,FromType,Size>::convert(string.ToCString(),field);
				}


		};


}
#endif
