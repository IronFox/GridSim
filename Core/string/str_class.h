#ifndef str_classH
#define str_classH

/******************************************************************

String-class providing mostly identical functionality
as the Borland AnsiString-class.


******************************************************************/

//string configuration:


#define __STR_BLOCK_ALLOC__			0		//!< Allocates multiples of 8 characters in advance rather than the exact required section. May increase += operator efficiency
#define __STR_REFERENCE_COUNT__		0		//!< Keeps an internal reference counter to speed up copy instructions. Slows down everything else
#define __STR_EXPRESSION_TMPLATES__	0		//!< Uses expression templates to evaluate complex string operator expressions. Slow
#define __STR_RVALUE_REFERENCES__	1		//!< Allows && references in constructors and assignment operators







#include <string.h>






#ifdef _MSC_VER
	#include <float.h>

	//#define strlwr	_strlwr
	//#define strupr	_strupr

#endif


#include <math.h>
#ifndef isinf
	#if defined(_MSC_VER)
		#define isinf(x) (!_finite(x))
	#endif
#endif

#ifndef isnan
    #ifdef __BORLANDC__
        #define isnan IsNan
    #else
        #ifdef __GNUC__
            #define isnan(x) (sizeof (x) == sizeof (float) ? __isnanf (x)	\
        		  : sizeof (x) == sizeof (double) ? __isnan (x)	\
        		  : __isnanl (x))
        
		#elif defined(_MSC_VER)
			#define isnan _isnan
        #else
            #error isnan not defined!
        #endif
    #endif
#endif


//#ifdef _MSC_VER
	#define WCOUT
//#endif

//#include "../general/comparable.h"


char*	strnchr(char*offset,size_t cnt, char c);
char*	strnstr(char *big, const char *little, size_t len);

void	gotoxy(int x, int y);	//!< Locates the console out cursor at the specified position

#include "../interface/to_string.h"
#include "../interface/serializable.h"
#include "../interface/hashable.h"

	
	
#if SYSTEM==WINDOWS
	#include <winbase.h>
	//#include <iostream>
	//#include <cstdlib>



	//using namespace std;



#elif SYSTEM==UNIX
	#include <stdlib.h>
	#include <wctype.h>
	#include <ctype.h>

	
	void strlwr(char*target);
	void strupr(char*target);
	void wcslwr(wchar_t*target);
	void wcsupr(wchar_t*target);
#endif

#define STRING_DEBUG(_MSG_)	//cout << _MSG_<<endl; ::flush(cout);
#define STRING_METHOD_BEGIN(_PARAMETER_, _OUTPUT_)	//cout << "function enter ("<<__LINE__<<"): "<<__func__<<_PARAMETER_<<" "<<#_OUTPUT_<<"='"<<_OUTPUT_<<"'"<<endl; ::flush(cout);
#define STRING_METHOD_END	//cout << "function leave ("<<__LINE__<<"): "<<__func__<<endl; ::flush(cout);


#define STRING_CHECK(_STRING_)	ASSERT_EQUAL__((_STRING_).length(),strlen((_STRING_).c_str()))

#define CHECK_STRING(_STRING_)	STRING_CHECK(_STRING_)

#ifdef _MSC_VER
	#define strcmpi _strcmpi
	#define strnicmp _strnicmp
	#define wcscmpi _wcsicmp

#endif

#ifdef __llvm__
	#define strnicmp	strncasecmp
	#define wcscmpi		wcscasecmp
	#define wcsnicmp	wcsncasecmp


#endif


#include <iostream>
//using namespace std;



#include "string_converter.h"

#include <math.h>
#include <limits.h>

#include "../container/array.h"



namespace DeltaWorks
{

	template <typename T>
		inline bool IsWhitespace(T c)
		{
			return c == (T)' ' || c == (T)'\t' || c == (T)'\r' || c == (T)'\n';
		}

	template <typename T>
		inline bool IsNewline(T c)
		{
			return c == (T)'\r' || c == (T)'\n';
		}



	namespace CharFunctions
	{

		template <typename T>
			inline size_t	__fastcall strlen(const T*string) throw();
		template <typename T0, typename T1>
			inline const T0*	__fastcall strstr(const T0*haystack, const T1*needle) throw();
		template <typename T0, typename T1>
			inline const T0*	__fastcall strstr(const T0*haystack, const T0*const haystackEnd, const T1*needle, const T1*const needleEnd) throw();
		template <typename T0, typename T1>
			inline const T0*	__fastcall stristr(const T0*haystack, const T0*const haystackEnd, const T1*needle, const T1*const needleEnd) throw();
		template <typename T0, typename T1>
			inline const T0*	__fastcall stristr(const T0*haystack, const T1*needle) throw();
		template <typename T>
			inline const T*	__fastcall strchr(const T*haystack, T needle) throw();
		template <typename T>
			inline const T*	__fastcall strchr(const T*haystack, const T*const haystackEnd, T needle) throw();
		template <typename T>
			inline const T*	__fastcall strchr(const T*haystack, bool isNeedle(T)) throw();
		template <typename T>
			inline const T*	__fastcall strchr(const T*haystack, const T*const haystackEnd, bool isNeedle(T)) throw();
		template <typename T>
			inline const T*	__fastcall strichr(const T*haystack, T needle) throw();
		template <typename T0, typename T1>
			inline void __fastcall strncpy(T0*target,const T1*source,size_t length) throw();
		template <typename T0, typename T1>
			inline int __fastcall strncmp(const T0*string0,const T1*string1, size_t length) throw();
		template <typename T0, typename T1>
			inline int __fastcall strncmpi(const T0*string0,const T1*string1, size_t length) throw();
		template <typename T0, typename T1>
			inline int __fastcall strcmp(const T0*string0,const T1*string1) throw();
		template <typename T0, typename T1>
			inline int __fastcall strcmpi(const T0*string0,const T1*string1) throw();
		template <typename T>
			inline void __fastcall strupr(T*string) throw();
		template <typename T>
			inline void __fastcall strlwr(T*string) throw();
		template <typename T>
			inline T	__fastcall toupper(T chr) throw();
		template <typename T>
			inline T	__fastcall tolower(T chr) throw();
		template <typename T>
			inline bool	__fastcall isalnum(T chr) throw();
		template <typename T>
			inline bool	__fastcall isdigit(T chr) throw();
		template <typename T>
			inline bool	__fastcall isalpha(T chr) throw();
		template <typename T>
			inline bool __fastcall isEscapable(T chr) throw();	//!< Returns true for characters that can be escaped
		template <typename T0, typename T1>
			inline void	__fastcall Cast(const T0*from, T1*to, count_t numChars);
	}



	namespace StringType
	{

		extern unsigned GLOBAL_STRING_ID_COUNTER;
		extern void*	GLOBAL_TRACED_ORIGINAL_ADDR;

		/*
			#define COUNT_STRING_ID	 //<- use this line to enable string-id-counting
			#define TRACE_STRING_ID 8541	//<- and this to trace a strings activities once you got its id
		*/



		void			ErrMessage(const char*line);
		void			ShowMessage(const char*line);
		void			displayMessage(const char*head, const char*line);




		char*	strAllocate(size_t size);
		char*	strRelocate(char*current, size_t current_size, size_t new_size);
		void	 strFree(char*current);
		void	 checkTracedString(const char*from);	//<- use this function at any point in your program to check wether the traced string has been modified or not. Only works if both COUNT_STRING_ID and TRACE_STRING_ID are defined.

		/*
		void	explode(const String&delimiter, const String&string, Ctr::ArrayData<String>&result);
		void	explode(const char*delimiter, const String&string, Ctr::ArrayData<String>&result);
		void	explode(const char*delimiter, const char*string, Ctr::ArrayData<String>&result);
		void	explode(char delimiter, const char*string, Ctr::ArrayData<String>&result);
		void	explode(char delimiter, const String&string, Ctr::ArrayData<String>&result);
		String	implode(const String&glue, const Ctr::ArrayData<String>&pieces);
		String	implode(char glue, const Ctr::ArrayData<String>&pieces);
		void 	wrap(const char*string, size_t line_length, Ctr::ArrayData<String>&result);
		void 	wrap(const char*string, size_t line_length, size_t (*lengthFunction)(const char*begin, const char*end),Ctr::ArrayData<String>&result);
		void 	wrapf(const char*string, float line_length, float (*lengthFunction)(const char*begin, const char*end),Ctr::ArrayData<String>&result);
		*/


		template <typename T>
			class Template;





		template <typename T0, typename T1>
			class ConcatExpression;
		template <typename T>
			class StringExpression;
		template <typename T>
			class CharacterExpression;

		/**
		@brief String reference expression template type
	
		Used to reference a character array segment without copying
		*/
		template <typename T>
			class ReferenceExpression
			{
			private:
				const T			*reference;
				size_t			len;
			public:
				typedef const T*iterator,const_iterator;

				/**/			ReferenceExpression():reference(NULL),len(0) {}
				/**/			ReferenceExpression(const T*str):reference(str),len(CharFunctions::strlen(str)) {}
				/**/			ReferenceExpression(const T*str, size_t length):reference(str),len(length) {}


				friend hash_t	Hash(const ReferenceExpression<T>&ref)
				{
					return StdCharHash(ref.pointer(),ref.size());
				}

				iterator		begin() const {return reference;}
				iterator		end() const {return reference + len;}
				bool			IsEmpty() const {return len == 0;}
				bool			IsNotEmpty() const {return len != 0;}
				inline bool		DropLastChar()
								{
									if (!len)
										return false;
									len--;
									return true;
								}
				inline bool		DropFirstChar()
								{
									if (!len)
										return false;
									len--;
									reference++;
									return true;
								}
				inline bool		EndsWith(const T*str) const
								{
									size_t slen = CharFunctions::strlen(str);
									if (slen > len)
										return false;
									return !CharFunctions::strncmp(reference+len-slen,str, slen);
								}
				inline bool		EndsWith(T c) const
								{
									if (!len)
										return false;
									return reference[len-1] == c;
								}
				inline bool		BeginsWith(const T*str) const
								{
									size_t slen = CharFunctions::strlen(str);
									if (slen > len)
										return false;
									return !CharFunctions::strncmp(reference,str, slen);
								}
				inline bool		BeginsWith(T c) const
								{
									if (!len)
										return false;
									return reference[0] == c;
								}
				inline T		GetFirstChar() const
								{
									if (!len)
										FATAL__("Cannot access first char. Expression is empty");
									return *reference;
								}
				inline T		GetLastChar() const
								{
									if (!len)
										FATAL__("Cannot access first char. Expression is empty");
									return reference[len-1];
								}
				inline T		operator[](index_t at) const
								{
									return reference[at];
								}
				inline size_t	length()	const	//! Retrieves the length of the reference character segment (excluding any trailing zero) @return Length in characters
								{
									return len;
								}
				inline size_t	size() const {return len;}
				inline size_t	GetLength() const {return len;}
				inline const T*	pointer()	const
								{
									return reference;
								}
				inline ReferenceExpression<T>	SubStringRef(int index, count_t count = (count_t)-1)	 const;
				inline ReferenceExpression<T>	Trim() const;
				inline ReferenceExpression<T>	TrimLeft() const;
				inline ReferenceExpression<T>	TrimRight() const;
				inline Template<char>		ToString()			const;	//!< Converts the expression architecture including data to a string for debug output
				inline void		print(std::ostream&stream)			const;	//!< Prints the local expression content to the specified stream @param stream Stream to print to
				#ifdef WCOUT
					inline void	print(std::wostream&stream)			const;	//!< Prints the local expression content to the specified wide character stream @param stream Stream to print to
				#endif
				template <class Stream>
					inline void	printArchitecture(Stream&stream)			const
								{
									stream << "reference<\"";
									print(stream);
									stream << "\", "<<length()<<">";
								}
	
				inline T*		WriteTo(T*target)	const				//! Writes the local expression content to a character array. The array must be sufficiently large since the operation does not perform any length checks. @param target Character array to write to @return Ctr::Array position once the operation is completed. The returned pointer points one character past the last written character
								{
									memcpy(target,reference,len*sizeof(T));
									return target+len;
								}
				template <typename T2>
					inline	T2*	WriteTo(T2*target)	const				//! Type variant version of the above. @overload
								{
									for (size_t i = 0; i < len; i++)
										(*target++) = reference[i];
									return target;
								}
						
				inline T*		WriteTo(T*target, T*end)	const		//! Writes the local expression content to a character array of constrained size. @param target Character array to write to @param end Pointer to one past the last writable character. @return Ctr::Array position once the operation is completed. The returned pointer points one character past the last written character. The returned pointer will not point further than @a end
								{
									size_t cpy = end-target;
									if (len < cpy)
										cpy = len;
									memcpy(target,reference,cpy*sizeof(T));
									return target+cpy;
								}
						
				template <typename T2>
					inline T2*	WriteTo(T2*target, T2*end)	const				//! Type variant version of the above. @overload
								{
									for (size_t i = 0; i < len && target < end; i++)
										(*target++) = reference[i];
									return target;
								}

				template <typename T2>
					inline int	compareSegment(const T2*string, size_t string_length)	const	//! Nested compare method
								{
									int result = CharFunctions::strncmp(reference,string,string_length<len?string_length:len);
									if (result)
										return result;
									if (string_length < len)
										return 1;
									return 0;
								}
				template <typename T2>
					inline int	compareTo(const T2*string, size_t string_length)	const	//! Expression comparison. Allows to orthographically compare the local string to the specified character segment @param string String to compare to @param string_length Number of characters that the specified string is in length @return -1 if the local expression is orthographically smaller than the specified string, 0 if it is equal, +1 if it is orthographically larger
								{
									int result = compareSegment(string,string_length);
									if (!result && string_length > len)
										result = -1;
									return result;
								}
				template <typename T2>
					inline int	compareSegmentIgnoreCase(const T2*string, size_t string_length)	const	//! Nested compare method (case ignore)
								{
									int result = CharFunctions::strncmpi(reference,string,string_length<len?string_length:len);
									if (result)
										return result;
									if (string_length < len)
										return 1;
									return 0;
								}
				template <typename T2>
					inline int	compareToIgnoreCase(const T2*string, size_t string_length)	const	//! Expression comparison. Allows to orthographically compare the local string to the specified character segment @param string String to compare to @param string_length Number of characters that the specified string is in length @return -1 if the local expression is orthographically smaller than the specified string, 0 if it is equal, +1 if it is orthographically larger
								{
									int result = compareSegmentIgnoreCase(string,string_length);
									if (!result && string_length > len)
										result = -1;
									return result;
								}
				template <typename T2>
					inline int	CompareToIgnoreCase(const T2*string)	const	//! Expression comparison. Allows to orthographically compare the local string to the specified character segment @param string String to compare to @param string_length Number of characters that the specified string is in length @return -1 if the local expression is orthographically smaller than the specified string, 0 if it is equal, +1 if it is orthographically larger
								{
									size_t slen = CharFunctions::strlen(string);
									int result = compareSegmentIgnoreCase(string,slen);
									if (!result && slen > len)
										result = -1;
									return result;
								}
				template <typename T2>
					inline bool	references(const T2*field)	const	//! Checks whether this expression references the specified field @param field Pointer to the field to look for @return true if the pointer was found, false otherwise
								{
									return (const T*)field == reference;
								}
				template <typename T2>
					inline ConcatExpression<ReferenceExpression<T>,ReferenceExpression<T2> >
								operator+(const ReferenceExpression<T2>&expression)	const;
				template <typename T2>
					inline ConcatExpression<ReferenceExpression<T>,StringExpression<T2> >
								operator+(const StringExpression<T2>&expression)	const;
				template <typename T2, typename T3>
					inline ConcatExpression<ReferenceExpression<T>,ConcatExpression<T2,T3> >
								operator+(const ConcatExpression<T2,T3>&expression)	const;
				template <typename T2>
					inline ConcatExpression<ReferenceExpression<T>,CharacterExpression<T2> >
								operator+(const CharacterExpression<T2>&expression)	const;
	
				template <typename T2>
					bool		operator<(const Template<T2>&other)	const;
				template <typename T2>
					bool		operator<=(const Template<T2>&other)	const;
				template <typename T2>
					bool		operator>(const Template<T2>&other)	const;
				template <typename T2>
					bool		operator>=(const Template<T2>&other)	const;
				template <typename T2>
					bool		operator!=(const Template<T2>&other)	const;
				template <typename T2>
					bool		operator==(const Template<T2>&other)	const;
				bool			operator==(const T*str)	const	{size_t l = CharFunctions::strlen(str); return l == len && CharFunctions::strncmp(str,reference,len) == 0;}
				bool			operator!=(const T*str)	const	{return !operator==(str);}
			};

		/**
			@brief Template expression template type
	
			Used to reference a single character
		*/
		template <typename T>
			class CharacterExpression
			{
			private:
					T			ch;
			public:
								CharacterExpression(T c):ch(c)
								{}
			inline	size_t		length()	const	//! Retrieves the length of the local expression @return Length in characters
								{
									return 1;
								}
			inline	Template<char>		ToString()			const;
			inline	void		print(std::ostream&stream)			const	//! Prints the local expression content to the specified stream @param stream Stream to print to
								{
									stream << (char)ch;
								}
			#ifdef WCOUT
				inline	void	print(std::wostream&stream)			const
								{
									stream << (wchar_t)ch;
								}
			#endif
			template <class Stream>
				inline	void	printArchitecture(Stream&stream)			const
								{
									stream << "character<";
									print(stream);
									stream << ", "<<length()<<">";
								}
	
			inline	T*			WriteTo(T*target)	const				//! Writes the local expression content to a character array. The array must be sufficiently large since the operation does not perform any length checks. @param target Character array to write to @return Ctr::Array position once the operation is completed. The returned pointer points one character past the last written character
								{
									*target++ = ch;
									return target;
								}
			template <typename T2>
				inline	T2*		WriteTo(T2*target)	const				//! Type variant version of the above. @overload
								{
									*target++ = (T2)ch;
									return target;
								}
						
			inline	T*			WriteTo(T*target, T*end)	const		//! Writes the local expression content to a character array of constrained size. @param target Character array to write to @param end Pointer to one past the last writable character. @return Ctr::Array position once the operation is completed. The returned pointer points one character past the last written character. The returned pointer will not point further than @a end
								{
									if (target < end)
										*target++ = ch;
									return target;
								}
						
			template <typename T2>
				inline	T2*		WriteTo(T2*target, T2*end)	const				//! Type variant version of the above. @overload
								{
									if (target < end)
										*target++ = (T2)ch;
									return target;
								}

			template <typename T2>
				inline	int		compareSegment(const T2*string, size_t string_length)	const	//! Nested compare method
								{
									T	compare_to[2] = {ch,0};
									int result = CharFunctions::strncmp(compare_to,string,string_length?1:0);
									if (result)
										return result;
									if (string_length < 1)
										return 1;
									return 0;
								}
			template <typename T2>
				inline	int		compareTo(const T2*string, size_t string_length)	const	//! Expression comparison. Allows to orthographically compare the local string to the specified character segment @param string String to compare to @param string_length Number of characters that the specified string is in length @return -1 if the local expression is orthographically smaller than the specified string, 0 if it is equal, +1 if it is orthographically larger
								{
									int result = compareSegment(string,string_length);
									if (!result && string_length > 1)
										result = -1;
									return result;
								}
						
			template <typename T2>
				inline	int		compareSegmentIgnoreCase(const T2*string, size_t string_length)	const	//! Nested compare method
								{
									T	compare_to[2] = {ch,0};
									int result = CharFunctions::strncmpi(compare_to,string,string_length?1:0);
									if (result)
										return result;
									if (string_length < 1)
										return 1;
									return 0;
								}
			template <typename T2>
				inline	int		compareToIgnoreCase(const T2*string, size_t string_length)	const	//! Expression comparison. Allows to orthographically compare the local string to the specified character segment @param string String to compare to @param string_length Number of characters that the specified string is in length @return -1 if the local expression is orthographically smaller than the specified string, 0 if it is equal, +1 if it is orthographically larger
								{
									int result = compareSegmentIgnoreCase(string,string_length);
									if (!result && string_length > 1)
										result = -1;
									return result;
								}
			template <typename T2>
				inline	bool	references(const T2*field)	const	//! Checks whether this expression references the specified field @param field Pointer to the field to look for @return true if the pointer was found, false otherwise
								{
									return false;
								}
			template <typename T2>
				inline ConcatExpression<CharacterExpression<T>,CharacterExpression<T2> >
								operator+(const CharacterExpression<T2>&expression)	const;
			template <typename T2>
				inline ConcatExpression<CharacterExpression<T>,ReferenceExpression<T2> >
								operator+(const ReferenceExpression<T2>&expression)	const;
			template <typename T2>
				inline ConcatExpression<CharacterExpression<T>,StringExpression<T2> >
								operator+(const StringExpression<T2>&expression)	const;
			template <typename T2, typename T3>
				inline ConcatExpression<CharacterExpression<T>,ConcatExpression<T2,T3> >
								operator+(const ConcatExpression<T2,T3>&expression)	const;
	
			template <typename T2>
				bool			operator<(const Template<T2>&other)	const;
			template <typename T2>
				bool			operator<=(const Template<T2>&other)	const;
			template <typename T2>
				bool			operator>(const Template<T2>&other)	const;
			template <typename T2>
				bool			operator>=(const Template<T2>&other)	const;
			template <typename T2>
				bool			operator!=(const Template<T2>&other)	const;
			template <typename T2>
				bool			operator==(const Template<T2>&other)	const;
						
			};


		/**
			@brief Template expression template type
	
			Used to reference a temporary string generated from non-string data (e.g. an int)
		*/
		template <typename T>
			class StringExpression:public Template<T>
			{
			public:
					#ifdef __GNUC__
						using Template<T>::length;
						using Template<T>::writeTo;
			
					#endif
	
								StringExpression(const Template<T>&other):Template<T>(other)
								{}
							#if __STR_RVALUE_REFERENCES__
								StringExpression(Template<T>&&other):Template<T>(std::move(other))
								{}
							#endif
			template <typename T2>
				inline	int		compareSegment(const T2*string, size_t string_length)	const;	//! Nested compare method
			template <typename T2>
				inline	int		compareTo(const T2*string, size_t string_length)	const;	//! Expression comparison. Allows to orthographically compare the local string to the specified character segment @param string String to compare to @param string_length Number of characters that the specified string is in length @return -1 if the local expression is orthographically smaller than the specified string, 0 if it is equal, +1 if it is orthographically larger
			template <typename T2>
				inline	int		compareSegmentIgnoreCase(const T2*string, size_t string_length)	const;	//! Nested compare method
			template <typename T2>
				inline	int		compareToIgnoreCase(const T2*string, size_t string_length)	const;	//! Expression comparison. Allows to orthographically compare the local string to the specified character segment @param string String to compare to @param string_length Number of characters that the specified string is in length @return -1 if the local expression is orthographically smaller than the specified string, 0 if it is equal, +1 if it is orthographically larger
			inline	Template<char>		ToString()			const;
		

			template <typename T2>
				inline ConcatExpression<StringExpression<T>,ReferenceExpression<T2> >
								operator+(const ReferenceExpression<T2>&expression)	const;
			template <typename T2>
				inline ConcatExpression<StringExpression<T>,CharacterExpression<T2> >
								operator+(const CharacterExpression<T2>&expression)	const;
			template <typename T2>
				inline ConcatExpression<StringExpression<T>,StringExpression<T2> >
								operator+(const StringExpression<T2>&expression)	const;
			template <typename T2, typename T3>
				inline ConcatExpression<StringExpression<T>,ConcatExpression<T2,T3> >
								operator+(const ConcatExpression<T2,T3>&expression)	const;
						
			template <typename T2>
				inline	bool	references(const T2*pointer)	const	//! Checks whether this expression references the specified field @param field Pointer to the field to look for @return true if the pointer was found, false otherwise
								{
									return false; //pointer == Template<T>::field;	//don't need to actually compare because being a string object the pointer would be managed even if matching, preventing its deletion
								}
			template <class Stream>
				inline	void	print(Stream&stream)			const;	//!< Prints the local expression content to the specified stream @param stream Stream to print to
			template <class Stream>
				inline	void	printArchitecture(Stream&stream)			const	//! Prints the local expression content and architecture to the specified stream @param stream Stream to print to
								{
									stream << "string<\"";
									print(stream);
									stream << "\", "<<Template<T>::length()<<">";
								}
						
			};

		/**
			@brief Template expression template type
	
			Used to reference a concatenation expression of two sub expressions
		*/
		template <typename T0, typename T1>
			class ConcatExpression
			{
			private:
			const	T0			exp0;
			const	T1			exp1;
						
			public:
								ConcatExpression(const T0&expression0, const T1&expression1):exp0(expression0),exp1(expression1)
								{}
							#if __STR_RVALUE_REFERENCES__
								ConcatExpression(T0&&expression0, T1&&expression1):exp0(expression0),exp1(expression1)
								{}
							#endif
			inline	size_t		length()	const	//! Retrieves the length of the local expression @return Length in characters
								{
									return exp0.length()+exp1.length();
								}
			template <typename T2>
				inline	T2*		WriteTo(T2*target)	const				//! Writes the local expression content to a character array. The array must be sufficiently large since the operation does not perform any length checks. @param target Character array to write to @return Ctr::Array position once the operation is completed. The returned pointer points one character past the last written character
								{
									return exp1.WriteTo(exp0.WriteTo(target));
								}
			template <typename T2>
				inline	T2*		WriteTo(T2*target, T2*end)	const		//! Writes the local expression content to a character array of constrained size. @param target Character array to write to @param end Pointer to one past the last writable character. @return Ctr::Array position once the operation is completed. The returned pointer points one character past the last written character. The returned pointer will not point further than @a end
								{
									return exp1.WriteTo(exp0.WriteTo(target,end),end);
								}
						
						
			inline	Template<char>		ToString()			const;

			template <class Stream>
				inline	void	print(Stream&stream)			const	//! Prints the local expression content to the specified stream @param stream Stream to print to
								{
									exp0.print(stream);
									exp1.print(stream);
								}			
			template <class Stream>
				inline	void	printArchitecture(Stream&stream)			const
								{
									stream << "concat<";
									exp0.printArchitecture(stream);
									stream << ", ";
									exp1.printArchitecture(stream);
									stream << ", "<<length()<<">";
								}
						
			template <typename T2>
				inline	int		compareSegment(const T2*string, size_t string_length)	const	//! Nested compare method
								{
									int rs = exp0.compareSegment(string,string_length);
									if (rs)
										return rs;
									size_t	len0 = exp0.length(),
											len1 = exp1.length();
									if (string_length == len0)
										return !!len1;			//return isgreater if the second expression is longer 0, isequal otherwise
									return exp1.compareSegment(string+len0,string_length-len0);
							
								}
			template <typename T2>
				inline	int		compareTo(const T2*string, size_t string_length)	const	//! Expression comparison. Allows to orthographically compare the local string to the specified character segment @param string String to compare to @param string_length Number of characters that the specified string is in length @return -1 if the local expression is orthographically smaller than the specified string, 0 if it is equal, +1 if it is orthographically larger
								{
									int result = compareSegment(string,string_length);
									if (!result && string_length > length())
										result = -1;
									return result;
								}						
			template <typename T2>
				inline	int		compareSegmentIgnoreCase(const T2*string, size_t string_length)	const	//! Nested compare method
								{
									int rs = exp0.compareSegmentIgnoreCase(string,string_length);
									if (rs)
										return rs;
									size_t	len0 = exp0.length(),
											len1 = exp1.length();
									if (string_length == len0)
										return !!len1;			//return isgreater if the second expression is longer 0, isequal otherwise
									return exp1.compareSegmentIgnoreCase(string+len0,string_length-len0);
							
								}
			template <typename T2>
				inline	int		compareToIgnoreCase(const T2*string, size_t string_length)	const	//! Expression comparison. Allows to orthographically compare the local string to the specified character segment @param string String to compare to @param string_length Number of characters that the specified string is in length @return -1 if the local expression is orthographically smaller than the specified string, 0 if it is equal, +1 if it is orthographically larger
								{
									int result = compareSegmentIgnoreCase(string,string_length);
									if (!result && string_length > length())
										result = -1;
									return result;
								}						
						
			template <typename T2>
				inline ConcatExpression<ConcatExpression<T0,T1>,StringExpression<T2> >
								operator+(const Template<T2>&string)	const
								{
									return ConcatExpression<ConcatExpression<T0,T1>,StringExpression<T2> >(*this,StringExpression<T2>(string));
								}

			template <typename T2>
				inline ConcatExpression<ConcatExpression<T0,T1>,ReferenceExpression<T2> >
								operator+(const T2*string)	const
								{
									return ConcatExpression<ConcatExpression<T0,T1>,ReferenceExpression<T2> >(*this,ReferenceExpression<T2>(string));
								}
				inline ConcatExpression<ConcatExpression<T0,T1>,CharacterExpression<char> >
								operator+(char c)	const
								{
									return ConcatExpression<ConcatExpression<T0,T1>,CharacterExpression<char> >(*this,CharacterExpression<char>(c));
								}
				inline ConcatExpression<ConcatExpression<T0,T1>,CharacterExpression<wchar_t> >
								operator+(wchar_t c)	const
								{
									return ConcatExpression<ConcatExpression<T0,T1>,CharacterExpression<wchar_t> >(*this,CharacterExpression<wchar_t>(c));
								}
			template <typename T2>
				inline ConcatExpression<ConcatExpression<T0,T1>,CharacterExpression<T2> >
								operator+(const CharacterExpression<T2>&expression)	const
								{
									return ConcatExpression<ConcatExpression<T0,T1>,CharacterExpression<T2> >(*this,expression);
								}
			template <typename T2>
				inline ConcatExpression<ConcatExpression<T0,T1>,ReferenceExpression<T2> >
								operator+(const ReferenceExpression<T2>&expression)	const
								{
									return ConcatExpression<ConcatExpression<T0,T1>,ReferenceExpression<T2> >(*this,expression);
								}
			template <typename T2>
				inline ConcatExpression<ConcatExpression<T0,T1>,StringExpression<T2> >
								operator+(const StringExpression<T2>&expression)	const;

			template <typename T2, typename T3>
				inline ConcatExpression<ConcatExpression<T0,T1>,ConcatExpression<T2,T3> >
								operator+(const ConcatExpression<T2,T3>&expression)	const
								{
									return ConcatExpression<ConcatExpression<T0,T1>,ConcatExpression<T2,T3> >(*this,expression);
								}
			template <typename T2>
				inline	bool	references(const T2*field)	const	//! Checks whether this expression references the specified field @param field Pointer to the field to look for @return true if the pointer was found, false otherwise
								{
									return exp0.references(field) || exp1.references(field);
								}
		
		
			template <typename T2>
				bool			operator<(const Template<T2>&other)	const;
			template <typename T2>
				bool			operator<=(const Template<T2>&other)	const;
			template <typename T2>
				bool			operator>(const Template<T2>&other)	const;
			template <typename T2>
				bool			operator>=(const Template<T2>&other)	const;
			template <typename T2>
				bool			operator!=(const Template<T2>&other)	const;
			template <typename T2>
				bool			operator==(const Template<T2>&other)	const;
						
			};


		struct TStringLength
		{
			size_t	characters;
	
					TStringLength(size_t num_characters):characters(num_characters)
					{}
		};

		#if !__STR_EXPRESSION_TMPLATES__
			template <typename T0, typename T1>
				inline Template<T1> operator+(const T0*c, const Template<T1>&);
			template <typename T>
				inline Template<T> operator+(char c, const Template<T>&);
			template <typename T>
				inline Template<T> operator+(wchar_t c, const Template<T>&);
		#endif

		/**
			@brief Base template string object
	
			The class allows to generate strings from any data type, usually however char or wchar_t.
			Other types may result in compilation errors or unexpected behavior.<br /><br />
	
			The string object uses copy-on-write and block-wise allocations to keep memory allocations and freeing minimal.
		*/
		template <typename T>
			class Template:public IHashable, public IString<T>
			{
				typedef Template<T>	Self;
			public:
				typedef T		char_t;
				static const count_t MaxLengthConst = std::numeric_limits<count_t>::max();
			protected:
	
	
	
			#if __STR_BLOCK_ALLOC__
				static const size_t		block_size=8;	//!< Number of characters that any allocated array is padded to.
			#endif
				static const int 		anchor_entries = (int)__STR_REFERENCE_COUNT__ + (int)__STR_BLOCK_ALLOC__;
				static const size_t 	anchor_size = sizeof(UINT32)*anchor_entries;
	
					/**
						@brief Zero terminated string field. Never NULL
			
						@a field points to the first entry of the allocated character array. The size of this contained character array is aligned to the constant value stored in @a block_size<br />
						Unless @a field points to @a sz, the four bytes preceding this pointer are used to store the reference counter.
						If the reference counter (exists and) is 1 then the entire array is proprietary to the local string object and deleted together with it. The same character array may be shared between different string objects.
						Shared character arrays are copied when modified.
					*/
					T				*field;
					size_t			string_length;	//!< Number of characters of @a field that are currently used. Does neither include preceeding reference counter nor trailing zero. Trailing padding characters are also not counted.
			
				
			static	T				zero;	//!< Terminal zero character (optional)
			static	T				*sz;	//!< Points to a string containing just one character: the trailing zero. The allocation call allocating an empty string returns this pointer while delocating it results in no action taken.

			#if __STR_REFERENCE_COUNT__
				static inline UINT32&	count(T*field)	//! Retrieves a reference to the 32bit reference-counter preceding the specified character array. The specified array must be valid and not NULL
				{
					return *(((UINT32*)field)-1);
				}
			#endif
	
			#if !__STR_EXPRESSION_TMPLATES__
				template <typename T0, typename T1>
					friend inline Template<T1> operator+(const T0*c, const Template<T1>&);
				template <typename T0>
					friend inline Template<T0> operator+(char c, const Template<T0>&);
				template <typename T0>
					friend inline Template<T0> operator+(wchar_t c, const Template<T0>&);
			#endif
	
			static inline char*	root(T*field)	//!< Retrieves a pointer to the actually allocated memory section. Retrieving this pointer is required only if it is about to be deleted
			{
				return (char*)(((UINT32*)field)
					-(int)__STR_BLOCK_ALLOC__
					-(int)__STR_REFERENCE_COUNT__
				);
			}
	
			#if __STR_BLOCK_ALLOC__
				static inline UINT32&	capacity(T*field)
				{
					return *(((UINT32*)field)-1-(int)__STR_REFERENCE_COUNT__);
				}
				/**
					@brief Computes the recommended allocation size for a field that should contain at least @a field_length characters (including trailing zero).

					pad() determines the field length (in characters) that should actually be allocated given the specified number of entries. Given that the trailing zero should be included in the calculation, a valid parameter would be at least 1.
					Since 1 indicates an actually empty string, however, containing only the trailing zero, the result in this case would be 1 as well. All other values are padded to the smallest greater multiple of @a block_size
			
					@param field_length Number of required characters (including trailing zero)
					@return Number of characters that should be allocated based on @a field_length . The result is 1 if @a field_length is 1
				*/
				static inline size_t pad(size_t field_length)
				{
					if (field_length <= 1)
						return field_length;
					size_t remainder;
					if (remainder = field_length%block_size)
						return field_length+block_size-remainder;
					return field_length;
				}
			#endif

			public:
				typedef const T*	iterator, const_iterator;

				inline iterator	begin() const {return field;}
				inline iterator	end() const {return field + string_length;}

				/**
				@brief Writes a floating point value to the specified char field
		
				This function is rather lowlevel. Use with caution.
				The function does @a NOT write a terminating zero.
		
				@param value Value to convert to string
				@param exactness Number of digits following the decimal point
				@param force_trailing_zeros Set true to always write the specified number of trailing digits (zeros in this case). Trailing zeros are otherwise trimmed
				@param end Pointer to the character one past the last available character slot. An actual digit will be written to @a *(end-1). It is up to the calling function to write a terminating zero to @a (*end) if needed
				@param first Character to the first available character slot
				@return Pointer to the first actually written character. This is at least @a first but may be anywhere between @a first and @a end
				*/
				template <typename Type>
					static T* FloatToStr(Type value, unsigned char exactness, bool force_trailing_zeros, T*end, T*first);
		
				/**
				@brief Writes a signed integer value to the specified char field
		
				This function is rather lowlevel. Use with caution.
				The function does @a NOT Write a terminating zero.
		
				@param Type Type of @a value
				@param UType Unsigned equivalent of @a Type
				@param value Value to convert to string
				@param end Pointer to the character one past the last available character slot. An actual digit will be written to @a *(end-1). It is up to the calling function to Write a terminating zero to @a (*end) if needed
				@param first Character to the first available character slot
				@return Pointer to the first actually written character. This is at least @a first but may be anywhere between @a first and @a end
				*/
				template <class Type, class UType> 
					static T* SignedToStr(Type value, T*end, T*first);
			protected:
	
	
			template <typename Expression>
				inline char		compareExpression(const Expression&expression)	const;
			template <typename Expression>
				inline void		appendExpression(const Expression&expression);
			template <typename Expression>
				inline void 	makeFromExpression(const Expression&expression);
			template <typename Expression>
				inline void 	copyExpression(const Expression&expression);
			template <typename Float>
				inline void 	copyFloat(Float value);
			template <typename Float>
				inline void 	makeFloat(Float value, unsigned char precision, bool force_trailing_zeros);
			template <class Type, class UType>
				void 			makeSigned(Type value);
			template <class Type>
				void 			makeUnsigned(Type value);
			template <class Type, class UType>
				void 			copySigned(Type value);
			template <class Type>
				void 			copyUnsigned(Type value);
	
			/**
				@brief Allocates a new character array including preceding reference counter. 
		
				If the passed value is 0 then @a sz is returned instead. @a sz does not feature a preceding reference counter and should not be modified.
		
				@param length Number of characters (not including trailing zero) that the new field should hold (at least).
			*/
			static inline T*		allocate(size_t length)
					{
						STRING_DEBUG("allocating new field of length "<<length);
						if (!length)
						{
							STRING_DEBUG("return sz");
							//ASSERT_NOT_NULL__(&zero);
							return &zero;
						}
						#if __STR_REFERENCE_COUNT__ || __STR_BLOCK_ALLOC__
							#if __STR_BLOCK_ALLOC__
								UINT32 cap = pad(length+1);
								size_t len = cap*sizeof(T)+anchor_size;
							#else
								size_t len = (length+1)*sizeof(T)+anchor_size;//reference count
							#endif
							STRING_DEBUG(" allocating "<<len<<" bytes");
							char*cresult = SHIELDED_ARRAY(new char[len],len);
							T*result = (T*)(cresult+anchor_size);
							#if __STR_BLOCK_ALLOC__
								capacity(result) = cap;
							#endif
							#if __STR_REFERENCE_COUNT__
								count(result) = 1;
							#endif
							STRING_DEBUG("raw result is "<<(void*)cresult<<". characters starting at "<<(void*)result<<". setting values...");
						#else
							T*result = alloc<T>(length+1);
						#endif
						//*((UINT32*)cresult) = 1;
						result[length] = 0;
						STRING_DEBUG("done");
						//cout << "allocated via new"<<endl;
						//ASSERT_NOT_NULL__(result);
						return result;
					}
	
			/**
				@brief Allocates a new field including anchor (if any)
		
				allocateField() allocates the specified number of characters (non-padded) plus any number of required anchor bytes (reference counter, capacity, etc).
				The method does not write any data to the newly allocated field, not even a trailing zero.
				Should reference counting and/or character block allocation be enabled then their respective anchor values will be properly initialized (ref_count=1, capacity=new_alloc) after allocation
		
				@param new_alloc Number of characters to allocate. @b Important: Must not be 0 and already include space for any trailing zero
	
			*/
			static inline T*		allocateField(size_t character_field_size)
					{
						#if __STR_REFERENCE_COUNT__ || __STR_BLOCK_ALLOC__
							size_t alloc = sizeof(T)*character_field_size+anchor_size;
					
							STRING_DEBUG("allocating "<<alloc<<" bytes");
							char*cresult = SHIELDED_ARRAY(new char[alloc],alloc);
				
							T*rs = (T*)(cresult+anchor_size);
					
							STRING_DEBUG("raw result is "<<(void*)cresult<<". characters starting at "<<(void*)rs<<". setting values...");
							#if __STR_REFERENCE_COUNT__
								count(rs) = 1;
							#endif
							#if __STR_BLOCK_ALLOC__
								capacity(rs) = character_field_size;
							#endif
							STRING_DEBUG("done");
							return rs;
						#else
							return alloc<T>(character_field_size);
						#endif			
					}
			/**
				@brief Signals that the specified pointer is no longer required and should be deleted
		
				The pointer is deleted only if its reference counter hits zero after decrementation. Otherwise it is assumed it's still referenced some place else.
			*/
			static inline void			delocate(T*c)
					{
						//ASSERT_NOT_NULL__(c);
						STRING_DEBUG("delocating field "<<(void*)c);
						if (c == &zero)
						{
							STRING_DEBUG("is sz. ignoring request");
							return;
						}

						//cout << "decrementing counter (currently "<<count(c)<<")"<<endl;
						STRING_DEBUG("decrementing counter");
						#if __STR_REFERENCE_COUNT__
							STRING_DEBUG("counter is currently "<<count(c));
							if (!--count(c))
							{
								STRING_DEBUG("discarding array");
								//cout << "counter hit zero. erasing "<<c<<endl;
								//cout << "erasing array "<<endl;
								DISCARD_ARRAY(root(c));
							}
						#else
							dealloc(c);
						#endif
						STRING_DEBUG("done");
					}
			#if __STR_REFERENCE_COUNT__
				inline	void				duplicate()	//! Signals that the array will be modified in succeeding operations and should be made exclusive property of the local string object. The method has no effect if the local string is empty or its reference counter 1
						{
							//ASSERT_NOT_NULL__(field);
							if (field == &zero)
								return;
							UINT32&cnt = count(field);
							if (cnt == 1)
								return;
							T*n = allocate(string_length);
							memcpy(n,field,string_length*sizeof(T));
							cnt--;
							field = n;
						}
			#endif

			protected:

					struct FieldMarker
					{
						const T				*begin,
											*end;

											FieldMarker(const T*begin_, const T*end_):begin(begin_),end(end_)
											{}
											FieldMarker(const T*begin_, size_t length):begin(begin_),end(begin_+length)
											{}

						bool				operator()(T c) const throw()
											{
												for (const T*it = begin; it < end; it++)
													if ((*it) == c)
														return true;
												return false;
											}
					};

					struct CaseInsensitiveFieldMarker
					{
						const T				*begin,
											*end;

											CaseInsensitiveFieldMarker(const T*begin_, const T*end_):begin(begin_),end(end_)
											{}
											CaseInsensitiveFieldMarker(const T*begin_, size_t length):begin(begin_),end(begin_+length)
											{}

						bool				operator()(T c) const throw()
											{
												c = CharFunctions::tolower(c);
												for (const T*it = begin; it < end; it++)
													if (CharFunctions::tolower(*it) == c)
														return true;
												return false;
											}
					};

					struct CharacterMarker
					{
						T					chr;
											CharacterMarker(T chr_):chr(chr_)
											{}

						bool				operator()(T c) const throw()
											{
												return c == chr;
											}
					};
				template <class Marker>
					void					genericEraseCharacters(const Marker &marked, bool erase_matches);
				template <class Marker>
					count_t					genericCountCharacters(const Marker&marked, bool count_matches)const;
				template <class Marker>
					void					genericAddSlashes(const Marker&marked);

				void						SetupFromCharArray(const T*);
				template <typename T2>
					void					SetupFromCastCharArray(const T2*);

			public:

				template <typename T1>
				explicit					Template(const Template<T1>&other)
											{
												string_length = other.length();
												field = allocate(string_length);
												const T1*otherField = other.c_str();
												DBG_ASSERT_NOT_NULL__(field);	//gonna crash with an access violation otherwise anyway
												CharFunctions::Cast(otherField,field,string_length);
											}

											Template():field(sz),string_length(0)
											{
												ASSERT_NOT_NULL__(field);
											}
											Template(const TStringLength&len):field(allocate(len.characters)),string_length(len.characters)
											{
												ASSERT_NOT_NULL__(field);
											}
										#if __STR_RVALUE_REFERENCES__
											Template(Template<T>&&other):field(other.field),string_length(other.string_length)
											{
												other.field = sz;
												other.string_length = 0;
											}
										#endif

											Template(const Template<T>&other)
											{
												string_length = other.string_length;
												#if __STR_REFERENCE_COUNT__
													field = other.field;
													if (field != &zero)
														count(field)++;
												#else
													field = allocate(string_length);
													memcpy(field, other.field,string_length*sizeof(T));
												#endif
												DBG_ASSERT_NOT_NULL__(field);
											}
			template <typename T2>
				/**/						Template(const T2*string, size_t length);
			template <size_t MaxLength>
				/**/						Template(const TFixedString<MaxLength>&string);
			/**/							Template(const TCodeLocation&loc);
			inline							Template(const char*string);
			inline							Template(const wchar_t*string);
											Template(bool);
											Template(char);
											Template(wchar_t);
											Template(short);
											Template(unsigned short);
											Template(long);
											Template(int);
											Template(unsigned);
											Template(unsigned long);
											Template(float, unsigned char precision=5, bool force_trailing_zeros=false);
											Template(double, unsigned char precision=8, bool force_trailing_zeros=false);
											Template(long double, unsigned char precision=10, bool force_trailing_zeros=false);
											Template(long long);
											Template(unsigned long long);
											Template(const Ctr::ArrayData<T>&array);
											Template(const void*);
									
										#ifdef DSTRING_H
											Template(const AnsiString&);
										#endif
										template <typename T0, typename T1>
											Template(const ConcatExpression<T0,T1>&expression);
										template <class T0>
											Template(const ReferenceExpression<T0>&expression);
										template <class T0>
											Template(const CharacterExpression<T0>&expression);
										template <class T0>
											Template(const StringExpression<T0>&expression);
								
								
				virtual					~Template();
				void					adoptData(Template<T>&other);	//!< Adopts all local variables from the specified string without actually copying any strings. Any local data is erased, the specified other string left empty
				void					swap(Template<T>&other);	//!< Swaps all data with other string
				friend void				swap(Self&a, Self&b)		{a.swap(b);}
	
				void					free();					//!< Replaces local string content with an empty string
				void					Clear()	{free();}
				/**
				Retrieves the current string length in characters (not including trailing zero)
				@return String length in characters
				*/
				inline count_t			length()	const {return string_length;}
				/**
				Retrieves the current string length in characters (not including trailing zero)
				@return String length in characters
				*/
				inline count_t			size() const {return string_length;}
				/**
				Retrieves the current string length in characters (not including trailing zero)
				@return String length in characters
				*/
				inline count_t			GetLength() const {return string_length;}
				const T*				c_str()		const;		//!< Retrieves a constant zero terminated character array containing the local string
				T*						mutablePointer();				//!< Retrieves a writable zero terminated character array containing the local string. Characters may be modified but the trailing zero and thus the length of the string should not be modified.

				Template<T>&			Erase(size_t index,size_t count=size_t(-1));	//!< Erases a sub string segment of the local string @param index Index of the first character to erase (0=first character). Invalid range values are clamped automatically. @param count Number of characters to erase starting at @a index @return Reference to the local string once the operation is completed
				Template<T>&			EraseLeft(size_t count);								//!< Erases the specified number of characters from the beginning of the string @param count Number of characters to erase from the beginning @return Reference to the local string once the operation is completed
				Template<T>&			EraseRight(size_t count);								//!< Erases the specified number of characters from the end of the string @param count Number of characters to erase from the end @return Reference to the local string once the operation is completed

				Template<T>&			Truncate(size_t maxLength);	//!< Removes characters from the end such that the total length of the local string is less or equal to the specified length. If the local string already contains at most that many characters, then nothing changes

				Template<T>&			EraseCharacter(T chr);																				//!< Erases all occurrences of @a chr in the local string
				Template<T>&			EraseCharacters(const Template<T>& characters, bool erase_matches=true);										//!< Erases any character that is contained in the zero-terminated string specified by @a characters
				Template<T>&			EraseCharacters(const T* characters, bool erase_matches=true);										//!< Erases any character that is contained in the zero-terminated string specified by @a characters
				Template<T>&			EraseCharacters(const T* characters, count_t character_count, bool erase_matches=true);				//!< Erases any character that is contained in the range starting at @a characters . Checks @a character_count characters 
				Template<T>&			EraseCharacters(bool doReplace(T character), bool erase_matches=true);	//!< Erases any character for which the specified function returns true, if @a erase_matches is true / false, if @a erase_matches is false
				Template<T>&			EraseCharactersIgnoreCase(const Template<T>& characters, bool erase_matches=true);										//!< Erases any character that is contained in the zero-terminated string specified by @a characters . Case insensitive
				Template<T>&			EraseCharactersIgnoreCase(const T* characters, bool erase_matches=true);										//!< Erases any character that is contained in the zero-terminated string specified by @a characters . Case insensitive
				Template<T>&			EraseCharactersIgnoreCase(const T* characters, count_t character_count, bool erase_matches=true);				//!< Erases any character that is contained in the range starting at @a characters . Checks @a character_count characters  . Case insensitive

				count_t					CountCharacters(const Template<T>& characters, bool count_matches=true)	const;							//!< Counts how often each of the characters in the specified string is contained in the local string;	@param count_matches Set true to count matching characters, false to count non-matching characters
				count_t					CountCharacters(const T* characters, bool count_matches=true)	const;											//!< Counts how often each of the characters in the specified string is contained in the local string;	@param count_matches Set true to count matching characters, false to count non-matching characters
				count_t					CountCharacters(const T* characters, count_t character_count, bool count_matches=true)	const;							//!< Counts how often each of the characters in the specified string is contained in the local string;	@param count_matches Set true to count matching characters, false to count non-matching characters
				count_t					CountCharacters(bool isMatch(T character), bool count_matches=true)	const;							//!< Counts how often characters are contained by the local string for which isMatch(c) returns true (or false if @a count_matches is false).	@param count_matches Set true to count matching characters, false to count non-matching characters
				count_t					CountCharactersIgnoreCase(const Template<T>& characters, bool count_matches=true)	const;							//!< Counts how often each of the characters in the specified string is contained in the local string; Case insensitive version	@param count_matches Set true to count matching characters, false to count non-matching characters
				count_t					CountCharactersIgnoreCase(const T* characters, bool count_matches=true)	const;											//!< Counts how often each of the characters in the specified string is contained in the local string; Case insensitive version	@param count_matches Set true to count matching characters, false to count non-matching characters
				count_t					CountCharactersIgnoreCase(const T* characters, count_t character_count, bool count_matches=true)	const;							//!< Counts how often each of the characters in the specified string is contained in the local string; Case insensitive version	@param count_matches Set true to count matching characters, false to count non-matching characters


				Template<T>				Escape()	const;		//!< Returns a copy with \ added before \, ', and " characters
				Template<T>				Escape(const T*	before_characters) const;
				Template<T>				Escape(const T*	before_characters, count_t before_character_count) const;
				Template<T>				Escape(bool isMatch(T character)) const;
			
				Template<T>&			EscapeThis();		//!< Adds \ added before \, ', and " characters
				Template<T>&			EscapeThis(const T*	before_characters);
				Template<T>&			EscapeThis(const T*	before_characters, count_t before_character_count);
				Template<T>&			EscapeThis(bool isMatch(T character));

				Template<T>				StripSlashes()	const;		//!< Returns a copy with stripped \ characters
				Template<T>&			StripSlashesFromThis();		//!< Strips \ characters

				/**
				Checks if the specified token is part of the local string
				*/
				bool					Contains(const Template<T>&needle)	const	{return Find(needle) != InvalidIndex;}
				bool					Contains(const ReferenceExpression<T>&needle)	const	{return Find(needle) != InvalidIndex;}
				bool					Contains(const T*needle)		const	{return Find(needle) != InvalidIndex;}
				bool					Contains(T c)					const	{return Find(c) != InvalidIndex;}
				bool					ContainsWord(const T*word) const		{return FindWord(word) != InvalidIndex;}
				bool					ContainsWord(const Template<T>&word) const	{return FindWord(word) != InvalidIndex;}
				bool					ContainsWord(const ReferenceExpression<T>&word) const	{return FindWord(word) != InvalidIndex;}
				bool					ContainsIgnoreCase(const Template<T>&needle)	const	{return FindIgnoreCase(needle) != InvalidIndex;}
				bool					ContainsIgnoreCase(const ReferenceExpression<T>&needle)	const	{return FindIgnoreCase(needle) != InvalidIndex;}
				bool					ContainsIgnoreCase(const T*needle)		const	{return FindIgnoreCase(needle) != InvalidIndex;}
				bool					ContainsIgnoreCase(T needle)			const	{return FindIgnoreCase(needle) != InvalidIndex;}
				bool					ContainsWordIgnoreCase(const T*word)	const	{return FindWordIgnoreCase(word) != InvalidIndex;}
				bool					ContainsWordIgnoreCase(const Template<T>&word) const{return FindWordIgnoreCase(word) != InvalidIndex;}
				bool					ContainsWordIgnoreCase(const ReferenceExpression<T>&word) const{return FindWordIgnoreCase(word) != InvalidIndex;}


				/**
				Attempts to locate the first occurance of the specified string.
				Search is case sensitive.
				@param needle String segment to look for
				@param offset Index to start from 
				@return Start index of the found word (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					Find(const Template<T>&needle, index_t offset = 0)	const	{return Find(needle.ToRef(),offset);}
				/**
				@copydoc Find();
				*/
				index_t					Find(const ReferenceExpression<T>&needle, index_t offset = 0)	const;
				/**
				Attempts to locate the first occurance of the specified (zero-terminated) string.
				Search is case sensitive.
				@param needle Pointer to the first character of the string segment to look for
				@param offset Index to start from 
				@return Start index of the found word (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					Find(const T*needle, index_t offset = 0)		const	{return Find(ReferenceExpression<T>(needle),offset);}
				/**
				Attempts to locate the first occurance of the specified character.
				Search is case sensitive.
				@param needle Character to look for
				@param offset Index to start from 
				@return Index of the found character (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					Find(T needle, index_t offset = 0)					const;
				/**
				Attempts to locate the first occurance of a character for which the specified callback function returns true.
				Search is case sensitive.
				@param callback Pointer to a function to determine whether a character is a valid match. The function should return true if the character is a valid match, false otherwise
				@param offset Index to start from 
				@return Index of the found character (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					Find(bool callback(T), index_t offset = 0) const;
				/**
				Searches the local string for the first occurance of the specified sub string with neither the succeeding or preceeding characters being alpha-numeric.
				Search is case sensitive.
				@param needle String to search for
				@param offset Index to start from 
				@return Starting index of the found word (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindWord(const T*needle, index_t offset = 0) const	{return FindWord(ReferenceExpression<T>(needle),offset);}
				/**
				@copydoc FindWord()
				*/
				index_t					FindWord(const Template<T>&needle, index_t offset = 0) const	{return FindWord(needle.ToRef(),offset);}
				/**
				@copydoc FindWord()
				*/
				index_t					FindWord(const ReferenceExpression<T>&needle, index_t offset = 0) const;



				
				/**
				Attempts to locate the first occurance of the specified string.
				Search ignores case.
				@param needle String segment to look for
				@param offset Index to start from 
				@return Start index of the found word (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindIgnoreCase(const Template<T>&needle, index_t offset = 0)	const	{return FindIgnoreCase(needle.ToRef(),offset);}
				/**
				@copydoc FindIgnoreCase()
				*/
				index_t					FindIgnoreCase(const ReferenceExpression<T>&needle, index_t offset = 0)	const;
				/**
				Attempts to locate the first occurance of the specified (zero-terminated) string.
				Search ignores case.
				@param needle Pointer to the first character of the string segment to look for
				@param offset Index to start from 
				@return Start index of the found word (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindIgnoreCase(const T*needle, index_t offset = 0)		const	{return FindIgnoreCase(ReferenceExpression<T>(needle),offset);}
				/**
				Attempts to locate the first occurance of the specified character.
				Search ignores case.
				@param needle Character to look for
				@param offset Index to start from 
				@return Index of the found character (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindIgnoreCase(T needle, index_t offset = 0)					const;
				/**
				Searches the local string for the first occurance of the specified sub string with neither the succeeding or preceeding characters being alpha-numeric.
				Search ignores case.
				@param needle String to search for
				@param offset Index to start from 
				@return Starting index of the found word (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindWordIgnoreCase(const T*needle, index_t offset = 0) const	{return FindWordIgnoreCase(ReferenceExpression<T>(needle),offset);}
				/**
				@copydoc FindWordIgnoreCase()
				*/
				index_t					FindWordIgnoreCase(const Template<T>&needle, index_t offset = 0) const	{return FindWordIgnoreCase(needle.ToRef(),offset);}
				/**
				@copydoc FindWordIgnoreCase()
				*/
				index_t					FindWordIgnoreCase(const ReferenceExpression<T>&needle, index_t offset = 0) const;




				/**
				Attempts to locate the last occurance of the specified string.
				Search is case sensitive.
				@param needle String segment to look for
				@param offset Index to start from (first character). Reduced if necessary
				@return Start index of the found word (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindLast(const Template<T>&needle, index_t offset = InvalidIndex)	const	{return FindLast(needle.ToRef(),offset);}
				/**
				@copydoc FindLast()
				*/
				index_t					FindLast(const ReferenceExpression<T>&needle, index_t offset = InvalidIndex)	const;
				/**
				Attempts to locate the last occurance of the specified (zero-terminated) string.
				Search is case sensitive.
				@param needle Pointer to the first character of the string segment to look for
				@param offset Index to start from (first character). Reduced if necessary
				@return Start index of the found word (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindLast(const T*needle, index_t offset = InvalidIndex)		const	{return FindLast(ReferenceExpression<T>(needle),offset);}
				/**
				Attempts to locate the last occurance of the specified character.
				Search is case sensitive.
				@param needle Character to look for
				@param offset Index to start from. Reduced if necessary
				@return Index of the found character (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindLast(T needle, index_t offset = InvalidIndex)					const;
				/**
				Attempts to locate the last occurance of a character for which the specified callback function returns true.
				Search is case sensitive.
				@param callback Pointer to a function to determine whether a character is a valid match. The function should return true if the character is a valid match, false otherwise
				@param offset Index to start from. Reduced if necessary
				@return Index of the found character (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindLast(bool callback(T), index_t offset = InvalidIndex) const;
				/**
				Searches the local string for the last occurance of the specified sub string with neither the succeeding or preceeding characters being alpha-numeric.
				Search is case sensitive.
				@param needle String to search for
				@param offset Index to start from (first character). Reduced if necessary
				@return Starting index of the found word (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindLastWord(const T*needle, index_t offset = InvalidIndex) const	{return FindLastWord(ReferenceExpression<T>(needle),offset);}
				/**
				@copydoc FindLastWord()
				*/
				index_t					FindLastWord(const Template<T>&needle, index_t offset = InvalidIndex) const	{return FindLastWord(needle.ToRef(),offset);}
				/**
				@copydoc FindLastWord()
				*/
				index_t					FindLastWord(const ReferenceExpression<T>&needle, index_t offset = InvalidIndex) const;


				

				/**
				Attempts to locate the last occurance of the specified string.
				Search is case sensitive.
				@param needle String segment to look for
				@param offset Index to start from (first character). Reduced if necessary
				@return Start index of the found word (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindLastIgnoreCase(const Template<T>&needle, index_t offset = InvalidIndex)	const	{return FindLastIgnoreCase(needle.ToRef(),offset);}
				/**
				@copydoc FindLastIgnoreCase()
				*/
				index_t					FindLastIgnoreCase(const ReferenceExpression<T>&needle, index_t offset = InvalidIndex)	const;
				/**
				Attempts to locate the last occurance of the specified (zero-terminated) string.
				Search is case sensitive.
				@param needle Pointer to the first character of the string segment to look for
				@param offset Index to start from (first character). Reduced if necessary
				@return Start index of the found word (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindLastIgnoreCase(const T*needle, index_t offset = InvalidIndex)		const	{return FindLastIgnoreCase(ReferenceExpression<T>(needle),offset);}
				/**
				Attempts to locate the last occurance of the specified character.
				Search is case sensitive.
				@param needle Character to look for
				@param offset Index to start from. Reduced if necessary
				@return Index of the found character (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindLastIgnoreCase(T needle, index_t offset = InvalidIndex)					const;
				/**
				Searches the local string for the last occurance of the specified sub string with neither the succeeding or preceeding characters being alpha-numeric.
				Search is case sensitive.
				@param needle String to search for
				@param offset Index to start from (first character). Reduced if necessary
				@return Starting index of the found word (0=first char) or InvalidIndex if no occurance was found
				*/
				index_t					FindLastWordIgnoreCase(const T*needle, index_t offset = InvalidIndex) const	{return FindLastWordIgnoreCase(ReferenceExpression<T>(needle),offset);}
				/**
				@copydoc FindLastWordIgnoreCase()
				*/
				index_t					FindLastWordIgnoreCase(const Template<T>&needle, index_t offset = InvalidIndex) const	{return FindLastWordIgnoreCase(needle.ToRef(),offset);}
				/**
				@copydoc FindLastWordIgnoreCase()
				*/
				index_t					FindLastWordIgnoreCase(const ReferenceExpression<T>&needle, index_t offset = InvalidIndex) const;








				virtual hash_t			ToHash()	const override;

				Template<T>&			setLength(size_t newLength);								//!< Updates the local string to match the given number of characters. The new array will be uninitialized, except for the trailing zero @param newLength New string length in characters (not including trailing zero) @return Reference to the local string object once the modification is done
				Template<T>&			SetLength(size_t newLength)	{return setLength(newLength);}
				template <typename IndexType>
					Template<T>			subString(IndexType index, count_t count=MaxLengthConst) const;	//!< Creates a string copy containing the specified sub string of the local string @param index Index of the sub string to extract with 0 being the first character  Invalid values are clamped to the valid range. @param count Number of characters to extract starting at @a index @return Extracted string
				ReferenceExpression<T>	subStringRef(sindex_t index, count_t count=MaxLengthConst) const;	//!< Creates a string reference expression pointing to the specified sub string of the local string. The returned object remains valid as long as the local string object is not deleted or modified @param index Index of the sub string to extract with 0 being the first character  Invalid values are clamped to the valid range. @param count Number of characters to extract starting at @a index @return String segment
				ReferenceExpression<T>	SubStringRef(sindex_t index, count_t count=MaxLengthConst) const	/**@copydoc subStringRef()*/ {return subStringRef(index,count);}
				ReferenceExpression<T>	SubStringRef(index_t index, count_t count=MaxLengthConst) const	/**@copydoc subStringRef()*/ {return subStringRef(sindex_t(index),count);}

	
				/*!	\brief Removes whitespace characters from the beginning and the end of the local string
				\return Reference to the local string
									
				The following characters are considered whitespace: space (' '), tab ('\\t'), carriage return ('\\r') and newline ('\\n').*/
				Template<T>&			TrimThis();
				ReferenceExpression<T>	ref()			const;	//!< Creates a reference expression of the local string
				ReferenceExpression<T>	ToRef()			const	{return ref();}
		
				/*!	\brief Creates a copy of the local string with whitespace characters removed from the beginning and the end of the local string
				\return Trimmed copy of the local string.
									
				The resulting string has all preceeding and trailing whitespace characters removed. The following characters are considered whitespace: space (' '), tab ('\\t'), carriage return ('\\r') and newline ('\\n').*/
				Template<T>				Trim()			const;
				ReferenceExpression<T>	TrimRef()		const;	//!< Identical to the above but returns a reference instead
		
				/*!	\brief Creates a copy of the local string with whitespace characters removed from the beginning of the local string
				\return Trimmed copy of the local string.
									
				The resulting string has all preceeding whitespace characters removed. The following characters are considered whitespace: space (' '), tab ('\\t'), carriage return ('\\r') and newline ('\\n').*/
				Template<T>				TrimLeft()		const;
				ReferenceExpression<T>	TrimLeftRef()	const;	//!< Identical to the above but returns a reference instead
		
				/*!	\brief Creates a copy of the local string with whitespace characters removed from the end of the local string
				\return Trimmed copy of the local string.
									
				The resulting string has all trailing whitespace characters removed. The following characters are considered whitespace: space (' '), tab ('\\t'), carriage return ('\\r') and newline ('\\n').*/
				Template<T>				TrimRight()		const;
				ReferenceExpression<T>	TrimRightRef()	const;	//!< Identical to the above but returns a reference instead
		
				/*!	\brief Inserts a string into the local string
				\param index Character offset to insert before (in the range [0,length()-1])
				\param str String to insert
				\return Reference to this string
									
				This method inserts the string \b before the specified character offset. Passing 0 as \b index would insert before the first character.*/
				Template<T>&			Insert(size_t index, const Template<T>&str)	{return Insert(index,str.ToRef());}
				Template<T>&			Insert(size_t index, const ReferenceExpression<T>&str);
		
				/*!	\brief Inserts a character into the local string
				\param index Character offset to insert before (in the range [0,length()-1])
				\param c Character to insert
				\return Reference to this string
									
				This method inserts the character \b before the specified character offset. Passing 0 as \b index would insert before the first character.*/
				Template<T>&			Insert(size_t index, T c);

				Template<T>&			ConvertToLowerCase();						//!< Transforms all characters of the local string to lower case \return Local string object after case change.
				Template<T>&			ConvertToUpperCase();						//!< Transforms all characters of the local string to upper case \return Local string object after case change.
				Template<T>				CopyToLowerCase() const;					//!< Transforms all characters of a copy of the local string to lower case \return Copy of the local string with changed case
				Template<T>				CopyToUpperCase() const;					//!< Transforms all characters of a copy of the local string to upper case \return Copy of the local string with changed case
			

				/*!	\brief Extracts a string section between two given string segments
				\param left_delimiter Left delimiter string
				\param right_delimiter Right delimiter string
				\return First found string between the two given delimiters or an empty string if no match was found
								
				GetBetween() attempts to extract a string section following the last character of the first found match of \a left_delimiter to the last character before the first following match
				of \a right_delimiter.<br />
				Example:<br />
				String a = "abcdefghijklghi";<br />
				a.GetBetween("cd","ghi") will return "ef"
				*/
				Template<T>				GetBetween(const Template<T>&left_delimiter, const Template<T>&right_delimiter)	const;
				ReferenceExpression<T>	GetBetweenRef(const Template<T>&left_delimiter, const Template<T>&right_delimiter)	const;		//!< Identical to the above but returns a reference instead
				bool					BeginsWith(const T*string)		const;		//!< Returns true if the local string begins with the specified string, false otherwise
				bool					BeginsWith(const Template<T>&string)	const;		//!< Returns true if the local string begins with the specified string, false otherwise
				bool					BeginsWith(const ReferenceExpression<T>&string)	const;		//!< Returns true if the local string begins with the specified string, false otherwise
				bool					EndsWith(const T*string)			const;		//!< Returns true if the local string ends with the specified string, false otherwise
				bool					EndsWith(const Template<T>&string)		const;		//!< Returns true if the local string ends with the specified string, false otherwise
				bool					EndsWith(const ReferenceExpression<T>&string)		const;		//!< Returns true if the local string ends with the specified string, false otherwise
				bool					BeginsWithIgnoreCase(const T*string)		const;		//!< Returns true if the local string begins with the specified string, false otherwise
				bool					BeginsWithIgnoreCase(const Template<T>&string)	const;		//!< Returns true if the local string begins with the specified string, false otherwise
				bool					BeginsWithIgnoreCase(const ReferenceExpression<T>&string)	const;		//!< Returns true if the local string begins with the specified string, false otherwise
				bool					EndsWithIgnoreCase(const T*string)			const;		//!< Returns true if the local string ends with the specified string, false otherwise
				bool					EndsWithIgnoreCase(const Template<T>&string)		const;		//!< Returns true if the local string ends with the specified string, false otherwise
				bool					EndsWithIgnoreCase(const ReferenceExpression<T>&string)		const;		//!< Returns true if the local string ends with the specified string, false otherwise
				T						FirstChar()							const;		//!< Returns the first character
				T						LastChar()							const;		//!< Returns the last character. Access is valid only if the local string is not empty
				const T*				lastCharPointer()					const;		//!< Returns a pointer to the last character. The returned pointer points one before the trailing zero and is valid only if the local string is not empty.
				Template<T>				GetFirstWord()							const;		//!< Retrieves the first word, either the full string or until the first whitespace character is encountered
				ReferenceExpression<T>	GetFirstWordRef()						const;		//!< Identical to the above but returns a reference instead
				Template<T>				GetLastWord()							const;		//!< Retrieves the last word, either the full string or from where the last whitespace character is encountered. May require testing!
				ReferenceExpression<T>	GetLastWordRef()						const;		//!< Identical to the above but returns a reference instead
				Template<T>&			ReplaceSubString(size_t offset, size_t count, const ReferenceExpression<T>&replacement);	//!< Replaces a sub string section of the local string with the specified replacement @param offset Index of the first character to replace @param count Number of characters to replace. If this value is zero then the method effectively behaves like Insert() @param replacement String to replace the specified input section with @return Reference to @a this
				Template<T>&			ReplaceSubString(size_t offset, size_t count, const Template<T>&replacement)	/**@copydoc ReplaceSubString() */ {return ReplaceSubString(offset,count,replacement.ToRef());}
				/**
				Replaces all occurances of \b needle in the local string with \b replacement
				@return Number of replacements
				*/
				count_t					FindAndReplace(const ReferenceExpression<T>&needle, const ReferenceExpression<T>&replacement);
				count_t					FindAndReplace(const T*needle, const T*replacement)	/** @copydoc FindAndReplace(); */ {return FindAndReplace(ReferenceExpression<T>(needle),ReferenceExpression<T>(replacement));}
				count_t					FindAndReplace(const Template<T>&needle, const Template<T>&replacement)	/** @copydoc FindAndReplace(); */ {return FindAndReplace(needle.ToRef(),replacement.ToRef());}
				/**
				Replaces all occurances of \b needle_char in the local string with \b replacement
				@return Number of replacements
				*/
				count_t					FindAndReplace(T needle, const ReferenceExpression<T>&replacement);
				count_t					FindAndReplace(T needle, const T*replacement)	/** @copydoc FindAndReplace(); */ {return FindAndReplace(needle,ReferenceExpression<T>(replacement));}
				count_t					FindAndReplace(T needle, const Template<T>&replacement)	/** @copydoc FindAndReplace(); */ {return FindAndReplace(needle,replacement.ToRef());}
				count_t					FindAndReplace(T needle, T replacement);						//!< @copydoc FindAndReplace()

				/**
				Replaces all characters who are identified by the specified filter function with the specified replacement
				@param doReplace Function pointer to identify characters that should be replaced.
				@param replacement Character to replace with
				@return Number of replacements
				*/
				count_t					FindAndReplace(bool doReplace(T character), T replacement);

				bool					IsValid(bool validCharacter(T character))	const;					//!< Runs each character of the local string by the specified validation function. Returns true if all characters passed the validation, false otherwise.
				void					set(size_t index, T c);
				void					Set(size_t index, T c)	{set(index,c);}
			
									/*!	\brief Const character access method
										\param Index of the character to retrieve in the range [0,length()-1]
									
										If \b index is not in the range [0,length()-1] (0=first character) then the result will be 0.*/
				T						GetChar(index_t index) const;
				T						operator[](index_t index) const {return GetChar(index);}
				#if 0
									/*!	\brief Const character access operator
										\param Index of the character to retrieve in the range [0,length()-1]
									
										If \b index is not in the range [0,length()-1] (0=first character) then the result will be 0.*/
				T						operator[](size_t index) const;
									/*!	\brief Const character access operator
										\param Index of the character to retrieve in the range [0,length()-1]
									
										If \b index is not in the range [0,length()-1] (0=first character) then the result will be 0.*/
				T						operator[](int index)			const;
									/*!	\brief Const character access operator
										\param Index of the character to retrieve in the range [0,length()-1]
									
										If \b index is not in the range [0,length()-1] (0=first character) then the result will be 0.*/
				T						operator[](BYTE index)			const;
				#endif
				void					operator+=(const Template<T>&other);			//!< Appends the specified string to the end of the local string. \param other String to append to the local string.
				void					operator+=(const T*string);				//!< Appends the specified string to the end of the local string. \param string String to append to the local string.
				void					operator+=(T c);							//!< Appends the specified character to the end of the local string. \param c Character to append to the local string.
				template <typename T0, typename T1>
					void				operator+=(const ConcatExpression<T0,T1>&expression);
				template <class T0>
					void				operator+=(const ReferenceExpression<T0>&expression);
				template <class T0>
					void				operator+=(const CharacterExpression<T0>&expression);
				template <class T0>
					void				operator+=(const StringExpression<T0>&expression);			
				#ifdef DSTRING_H
					bool				operator<(const AnsiString&)	const;		//!< Tests if the specifed string object is alphabetically greater than the local string content. Comparison is case sensitive.
				#endif
				bool					operator<(const Template<T>&)		const;		//!< Tests if the local string content is alphabetically less compared to the content of the specified String object. Comparison is case sensitive.
				bool					operator<(const T*)			const;		//!< Tests if the local string content is alphabetically less compared to the content of the specified const char array. Comparison is case sensitive.
				bool					operator<(T)					const;		//!< Tests if the local string content is alphabetically less compared to the specified character. Comparison is case sensitive.
				template <typename T0, typename T1>
					bool				operator<(const ConcatExpression<T0,T1>&expression)	const;
				template <class T0>
					bool				operator<(const ReferenceExpression<T0>&expression)	const;
				template <class T0>
					bool				operator<(const CharacterExpression<T0>&expression)	const;
				template <class T0>
					bool				operator<(const StringExpression<T0>&expression)	const;

				#ifdef DSTRING_H
					bool				operator<=(const AnsiString&)	const;		//!< Tests if the specifed string object is alphabetically greater than or identical to the local string content. Comparison is case sensitive.
				#endif
				bool					operator<=(const Template<T>&)		const;		//!< Tests if the local string content is alphabetically less or equal compared to the content of the specified String object. Comparison is case sensitive.
				bool					operator<=(const T*) 		const;		//!< Tests if the local string content is alphabetically less or equal compared to the content of the specified const char array. Comparison is case sensitive.
				bool					operator<=(T)				const;		//!< Tests if the local string content is alphabetically less or equal compared to the specified character. Comparison is case sensitive.
				template <typename T0, typename T1>
					bool				operator<=(const ConcatExpression<T0,T1>&expression)	const;
				template <class T0>
					bool				operator<=(const ReferenceExpression<T0>&expression)	const;
				template <class T0>
					bool				operator<=(const CharacterExpression<T0>&expression)	const;
				template <class T0>
					bool				operator<=(const StringExpression<T0>&expression)	const;
			
				#ifdef DSTRING_H
					Template<T>&		operator=(const AnsiString&);
				#endif
				Template<T>&			operator=(const Template<T>&);					//!< Overwrites the local string content with the content of the specified Template object. \return Reference to the local string object.
				#if __STR_RVALUE_REFERENCES__
					Template<T>&		operator=(Template<T>&&);					//!< Overwrites the local string content with the content of the specified Template object. \return Reference to the local string object.
				#endif
				Template<T>&			operator=(const Ctr::ArrayData<T>&array);
			
				template <typename T2>
					Template<T>&		operator=(const T2*);						//!< Overwrites the local string content with the content of the specified const char array. \return Reference to the local string object.
				Template<T>&			operator=(const T*);						//!< Overwrites the local string content with the content of the specified const char array. \return Reference to the local string object.
				Template<T>&			operator=(char);								//!< Overwrites the local string content with the specified character. \return Reference to the local string object.
				Template<T>&			operator=(wchar_t);								//!< Overwrites the local string content with the specified character. \return Reference to the local string object.
				Template<T>&			operator=(short);							//!< Overwrites the local string content with the converted (decimal) number. \return Reference to the local string object.
				Template<T>&			operator=(unsigned short);					//!< Overwrites the local string content with the converted (decimal) number. \return Reference to the local string object.
				Template<T>&			operator=(long);							//!< Overwrites the local string content with the converted (decimal) number. \return Reference to the local string object.
				Template<T>&			operator=(int);								//!< Overwrites the local string content with the converted (decimal) number. \return Reference to the local string object.
				Template<T>&			operator=(unsigned);						//!< Overwrites the local string content with the converted (decimal) number. \return Reference to the local string object.
				Template<T>&			operator=(unsigned long);					//!< Overwrites the local string content with the converted (decimal) number. \return Reference to the local string object.
				Template<T>&			operator=(float);							//!< Overwrites the local string content with the converted (decimal) floating point value. \return Reference to the local string object.
				Template<T>&			operator=(double);							//!< Overwrites the local string content with the converted (decimal) floating point value. \return Reference to the local string object.
				Template<T>&			operator=(long double);						//!< Overwrites the local string content with the converted (decimal) floating point value. \return Reference to the local string object.
				Template<T>&			operator=(long long);						//!< Overwrites the local string content with the converted (decimal) number. \return Reference to the local string object.
				Template<T>&			operator=(unsigned long long);				//!< Overwrites the local string content with the converted (decimal) number. \return Reference to the local string object.
				template <typename T0, typename T1>
					Template<T>&		operator=(const ConcatExpression<T0,T1>&expression);
				template <class T0>
					Template<T>&		operator=(const ReferenceExpression<T0>&expression);
				template <class T0>
					Template<T>&		operator=(const CharacterExpression<T0>&expression);
				template <class T0>
					Template<T>&		operator=(const StringExpression<T0>&expression);

				#if __STR_RVALUE_REFERENCES__
					Template<T>&		operator=(StringExpression<T>&&expression);
				#endif
			
				#if __STR_EXPRESSION_TMPLATES__
					template <typename T0>
						ConcatExpression<StringExpression<T>,ReferenceExpression<T0> >
											operator+(const T0*string)								const;
						ConcatExpression<StringExpression<T>,CharacterExpression<T> >
											operator+(T)											const;
					template <typename T0, typename T1>
						ConcatExpression<StringExpression<T>,ConcatExpression<T0,T1> >
											operator+(const ConcatExpression<T0,T1>&expression)	const;
					template <class T0>
						ConcatExpression<StringExpression<T>,ReferenceExpression<T0> >
											operator+(const ReferenceExpression<T0>&expression)	const;
					template <class T0>
						ConcatExpression<StringExpression<T>,StringExpression<T0> >
											operator+(const StringExpression<T0>&expression)		const;
					template <class T0>
						ConcatExpression<StringExpression<T>,StringExpression<T0> >
											operator+(const Template<T0>&expression)		const;
					template <class T0>
						ConcatExpression<StringExpression<T>,CharacterExpression<T0> >
											operator+(const CharacterExpression<T0>&expression)	const;
				#else
					template <typename T0>
						Template<T>	operator+(const T0*string)								const;
						Template<T>	operator+(const T*string)								const;
						Template<T>	operator+(T)											const;
					template <typename T0, typename T1>
						Template<T>	operator+(const ConcatExpression<T0,T1>&expression)	const;
					template <typename T0>
						Template<T>	operator+(const ReferenceExpression<T0>&expression)	const;
					template <typename T0>
						Template<T>	operator+(const Template<T0>&expression)		const;
					template <typename T0>
						Template<T>	operator+(const StringExpression<T0>&expression)		const;
					template <typename T0>
						Template<T>	operator+(const CharacterExpression<T0>&expression)	const;
				#endif
			
				#ifdef DSTRING_H
					bool					Equals(const AnsiString&)	const;
				#endif
					bool					Equals(const Template<T>&)		const;		//!< Tests for case sensitive equality to another String object. \return true if the local and the remote string content are identical, false otherwise.
					bool					Equals(const T*)			const;		//!< Tests for case sensitive equality to a const char array. \return true if the local string content and the remote const char*array content are identical, false otherwise.
					bool					Equals(T)					const;		//!< Tests for case sensitive equality to a character. \return true if the local string content matches the specified character, false otherwise.
				template <typename T0, typename T1>
					bool					Equals(const ConcatExpression<T0,T1>&expression)	const;
				template <class T0>
					bool					Equals(const ReferenceExpression<T0>&expression)	const;
				template <class T0>
					bool					Equals(const CharacterExpression<T0>&expression)	const;
				template <class T0>
					bool					Equals(const StringExpression<T0>&expression)	const;
			
				#ifdef DSTRING_H
					bool					EqualsIgnoreCase(const AnsiString&)	const;
				#endif
					bool					EqualsIgnoreCase(const Template<T>&)		const;		//!< Tests for case insensitive equality to another String object. \return true if the local and the remote string content are identical, false otherwise.
					bool					EqualsIgnoreCase(const T*)			const;		//!< Tests for case insensitive equality to a const char array. \return true if the local string content and the remote const char*array content are identical, false otherwise.
					bool					EqualsIgnoreCase(T)					const;		//!< Tests for case insensitive equality to a character. \return true if the local string content matches the specified character, false otherwise.
				template <typename T0, typename T1>
					bool					EqualsIgnoreCase(const ConcatExpression<T0,T1>&expression)	const;
				template <class T0>
					bool					EqualsIgnoreCase(const ReferenceExpression<T0>&expression)	const;
				template <class T0>
					bool					EqualsIgnoreCase(const CharacterExpression<T0>&expression)	const;
				template <class T0>
					bool					EqualsIgnoreCase(const StringExpression<T0>&expression)	const;
						
				#ifdef DSTRING_H
					bool					operator==(const AnsiString&)	const;
				#endif
					bool					operator==(const Template<T>&)		const;		//!< Tests for case sensitive equality to another String object. \return true if the local and the remote string content are identical, false otherwise.
					bool					operator==(const T*)			const;		//!< Tests for case sensitive equality to a const char array. \return true if the local string content and the remote const char*array content are identical, false otherwise.
					bool					operator==(T)					const;		//!< Tests for case sensitive equality to a character. \return true if the local string content matches the specified character, false otherwise.
				template <typename T0, typename T1>
					bool					operator==(const ConcatExpression<T0,T1>&expression)	const;
				template <class T0>
					bool					operator==(const ReferenceExpression<T0>&expression)	const;
				template <class T0>
					bool					operator==(const CharacterExpression<T0>&expression)	const;
				template <class T0>
					bool					operator==(const StringExpression<T0>&expression)	const;
			
				#ifdef DSTRING_H
					bool					operator!=(const AnsiString&)	const;
				#endif
					bool					operator!=(const Template<T>&)		const;		//!< Tests for case sensitive non-equality to another String object. \return true if the local and the remote string content are not identical, false otherwise.
					bool					operator!=(const T*)			const;		//!< Tests for case sensitive non-equality to a const char array. \return true if the local string content and the remote const char*array content are not identical, false otherwise.
					bool					operator!=(T)					const;		//!< Tests for case sensitive non-equality to a character. \return true if the local string content does not match the specified character, false otherwise.
				template <typename T0, typename T1>
					bool					operator!=(const ConcatExpression<T0,T1>&expression)	const;
				template <class T0>
					bool					operator!=(const ReferenceExpression<T0>&expression)	const;
				template <class T0>
					bool					operator!=(const CharacterExpression<T0>&expression)	const;
				template <class T0>
					bool					operator!=(const StringExpression<T0>&expression)	const;
			
				#ifdef DSTRING_H
					bool					operator>(const AnsiString&)	const;		//!< Tests if the specifed string object is alphabetically less than the local string content. Comparison is case sensitive.
				#endif
					bool					operator>(const Template<T>&)		const;		//!< Tests if the local string content is alphabetically greater compared to the content of the specified String object. Comparison is case sensitive.
					bool					operator>(const T*)				const;		//!< Tests if the local string content is alphabetically greater compared to the content of the specified const char array. Comparison is case sensitive.
					bool					operator>(T)					const;		//!< Tests if the local string content is alphabetically greater compared to the specified character. Comparison is case sensitive.
				template <typename T0, typename T1>
					bool					operator>(const ConcatExpression<T0,T1>&expression)	const;
				template <class T0>
					bool					operator>(const ReferenceExpression<T0>&expression)	const;
				template <class T0>
					bool					operator>(const CharacterExpression<T0>&expression)	const;
				template <class T0>
					bool					operator>(const StringExpression<T0>&expression)	const;
				#ifdef DSTRING_H
					bool					operator>=(const AnsiString&)	const;		//!< Tests if the specifed string object is alphabetically less than or equal to the local string content. Comparison is case sensitive.
				#endif
					bool					operator>=(const Template<T>&)		const;		//!< Tests if the local string content is alphabetically greater or equal compared to the content of the specified String object. Comparison is case sensitive.
					bool					operator>=(const T*)			const;		//!< Tests if the local string content is alphabetically greater or equal compared to the content of the specified const char array. Comparison is case sensitive.
					bool					operator>=(T)					const;		//!< Tests if the local string content is alphabetically greater or equal compared to the specified character. Comparison is case sensitive.
				template <typename T0, typename T1>
					bool					operator>=(const ConcatExpression<T0,T1>&expression)	const;
				template <class T0>
					bool					operator>=(const ReferenceExpression<T0>&expression)	const;
				template <class T0>
					bool					operator>=(const CharacterExpression<T0>&expression)	const;
				template <class T0>
					bool					operator>=(const StringExpression<T0>&expression)	const;
				inline	void				Resize(size_t new_length);		//!< Resizes the local string length to match the specified number of characters (not including trailing zero). The resize operation leaves the local string content uninitialized save for an automatically set trailing zero. The string is duplicated even if the new size matches the existing one if it's not exclusively owned
				inline	void				resizeCopy(size_t new_length);	//!< Resizes the local string length to match the specified number of characters (not including trailing zero). Any available string content is copied and the trailing zero set. If the local string length is increased then the new characters following the existing ones are left undefined.
				inline	bool				IsEmpty()	const	{return string_length==0;}
				inline	bool				IsNotEmpty()const	{return string_length!=0;}
				inline	T*					WriteTo(T*target)	const;
				template <typename T2>
					inline	T2*				WriteTo(T2*target)	const;
				inline	T*					WriteTo(T*target, T*end)	const;
				template <typename T2>
					inline	T2*				WriteTo(T2*target, T2*end)	const;


				friend hash_t				Hash(const Template<T>&ref)
				{
					return StdCharHash(ref.c_str(),ref.size());
				}	

				virtual const T*			ToCString() const override {return field;}	// IString::ToCString()


				friend void					SerialSync(IWriteStream&s, const Self&v)
				{
					s.WriteSize(v.string_length);
					s.Write(v.field,(serial_size_t)(v.string_length*sizeof(T)));
				}
				friend void					SerialSync(IReadStream&s, Self&v)
				{
					v.SetLength(s.ReadSize());
					s.Read(v.field,(serial_size_t)(v.string_length*sizeof(T)));
				}
			};
		/*
		template <typename T> inline
			ConcatExpression<StringExpression<T>,StringExpression<T> > operator+(const Template<T>&str0, const Template<T>&str1)
			{	
				return ConcatExpression<StringExpression<T>,StringExpression<T> >(StringExpression<T>(str0),StringExpression<T>(str1));
			}*/




		typedef Template<char>		String;
		typedef Template<wchar_t>		WString;
		typedef Template<wchar_t>		StringW;
		typedef ReferenceExpression<char>	StringRef;
		typedef ReferenceExpression<wchar_t>WStringRef;
		typedef ReferenceExpression<wchar_t>StringRefW;




		inline const String&	ToString(const String&s)	{return s;}
		inline StringW			ToStringW(const String&s)	{return StringW(s);}
		inline String			ToString(const StringW&s)	{return String(s);}
		inline const StringW&	ToStringW(const StringW&s)	{return s;}

		template <typename T>
			String	ToString(const ReferenceExpression<T>&ref) {return String(ref);}
		template <typename T>
			StringW	ToStringW(const ReferenceExpression<T>&ref){return StringW(ref);}



	}

	typedef StringType::String	String;
	typedef StringType::StringW	StringW;
	typedef StringType::StringRef	StringRef;
	typedef StringType::StringRefW	StringRefW;
	typedef StringType::TStringLength	TStringLength;

	typedef StringType::Template<char32_t>	UTF32String;
	typedef StringType::Template<char16_t>	UTF16String;	//only one effective use for this kind of character

	namespace StringConversion
	{

		inline String			ToString(long long i)	{return String(i);}
		inline StringW			ToStringW(long long i)	{return StringW(i);}
		inline String			ToString(long i)	{return String(i);}
		inline StringW			ToStringW(long i)	{return StringW(i);}
		inline String			ToString(int i)		{return String(i);}
		inline StringW			ToStringW(int i)	{return StringW(i);}
		inline String			ToString(short i)	{return String(i);}
		inline StringW			ToStringW(short i)	{return StringW(i);}
		inline String			ToString(bool i)	{return String(i);}
		inline StringW			ToStringW(bool i)	{return StringW(i);}

		inline String			ToString(const char*ch)		{return String(ch);}
		inline StringW			ToStringW(const char*ch)	{return StringW(ch);}
		inline String			ToString(const wchar_t*ch)	{return String(ch);}
		inline StringW			ToStringW(const wchar_t*ch)	{return StringW(ch);}


		inline String			ToString(unsigned long long i)	{return String(i);}
		inline StringW			ToStringW(unsigned long long i)	{return StringW(i);}
		inline String			ToString(unsigned long i)	{return String(i);}
		inline StringW			ToStringW(unsigned long i)	{return StringW(i);}
		inline String			ToString(unsigned int i)		{return String(i);}
		inline StringW			ToStringW(unsigned int i)	{return StringW(i);}
		inline String			ToString(unsigned short i)	{return String(i);}
		inline StringW			ToStringW(unsigned short i)	{return StringW(i);}
		inline String			ToString(unsigned char i)	{return String(i);}
		inline StringW			ToStringW(unsigned char i)	{return StringW(i);}

		inline String			ToString(float i)	{return String(i);}
		inline StringW			ToStringW(float i)	{return StringW(i);}
		inline String			ToString(double i)	{return String(i);}
		inline StringW			ToStringW(double i)	{return StringW(i);}
		inline String			ToString(long double i)	{return String(i);}
		inline StringW			ToStringW(long double i)	{return StringW(i);}

		inline String			ToString(const TCodeLocation& i)	{return String(i);}
		inline StringW			ToStringW(const TCodeLocation& i)	{return StringW(i);}

		template <size_t MaxLength>
			inline String		ToString(const TFixedString<MaxLength>& i)	{return String(i);}
		template <size_t MaxLength>
			inline StringW		ToStringW(const TFixedString<MaxLength>& i){return StringW(i);}
	}

	#ifndef __BORLANDC__
		String		IntToHex(int value, BYTE min_len);
		String		IntToHex(__int64 value, BYTE min_len);
		String		IntToStr(int value);
		String		FloatToStr(float value);
	#else
		#include <math.hpp>
		#define		isnan	IsNan
		String		intToHex(int value, BYTE min_len);
		String		intToHex(__int64 value, BYTE min_len);
		String		intToStr(int value);
		String		FloatToStr(float value);
		#define		IntToStr	intToStr
		#define		IntToHex	intToHex
	#endif
					#define PointerToString	PointerToHex
					#define PointerToStr	PointerToHex
	String			PointerToHex(const void*pointer);	//!< Converts the specified pointer to a hex string without leading 0x
	String			DataToBinary(const void*data, size_t byteSize);
	//int				extractInt(const char*string);
	//float			extractFloat(const char*string);

	char*			writeToCharField(unsigned value, char*offset, char*end);	//!< Writes the specified unsigned value to the specified char field. Does \b not add a trailing 0. \param value Unsigned value to transform \param offset Character out offset \param end Pointer to one character past the end of the field. The function will stop when it hits this pointer.
	char*			writeFloatToCharField(float value, char*offset, char*end);	//!< Writes the specified float value to the specified char field. Does \b not add a trailing 0. \param value Unsigned value to transform \param offset Character out offset \param end Pointer to one character past the end of the field. The function will stop when it hits this pointer.


	void			ErrMessage(const String&line);
	void			ShowMessage(const String&line);
	void			DisplayMessage(const char*head, const String&line, bool prefixProgramName=true);
	void			DisplayMessage(const String&head, const String&line, bool prefixProgramName=true);


	void			ErrMessageW(const wchar_t*line);
	void			ErrMessageW(const StringW&line);
	void			ShowMessageW(const wchar_t*line);
	void			ShowMessageW(const StringW&line);
	void			DisplayMessageW(const wchar_t*head, const wchar_t*line, bool prefixProgramName=true);
	void			DisplayMessageW(const wchar_t*head, const StringW&line, bool prefixProgramName=true);
	void			DisplayMessageW(const StringW&head, const StringW&line, bool prefixProgramName=true);

					#define BinaryToHex		binaryToHex
					#define BinaryToStr		binaryToHex
					#define BinaryToString	binaryToHex

	void			binaryToHex(const void*data, size_t data_size, String&target);	//!< Converts a binary data section to a hex number string without leading '0x'
	String			binaryToHex(const void*data, size_t data_size);					//!< @overload


	template <typename T0, typename T1, typename T2>
		void	explode(const StringType::Template<T0>&delimiter, const StringType::Template<T1>&string, Ctr::ArrayData<T2>&result);
	template <typename T0, typename T1, typename T2>
		void	explode(const StringType::Template<T0>&delimiter, const StringType::ReferenceExpression<T1>&string, Ctr::ArrayData<T2>&result);
	template <typename T0, typename T1, typename T2>
		void	explode(const T0*delimiter, const StringType::Template<T1>&string, Ctr::ArrayData<T2>&result);
	template <typename T0, typename T1, typename T2>
		void	explode(const T0*delimiter, const StringType::ReferenceExpression<T1>&string, Ctr::ArrayData<T2>&result);
	template <typename T0, typename T1, typename T2>
		void	explode(const T0*delimiter, const T1*string, Ctr::ArrayData<T2>&result);
	template <typename T0, typename T1>
		void	explode(T0 delimiter, const T0*string, Ctr::ArrayData<T1>&result);
	template <typename T0, typename T1>
		void	explode(T0 delimiter, const StringType::Template<T0>&string, Ctr::ArrayData<T1>&result);
	template <typename T0, typename T1>
		void	explode(T0 delimiter, const StringType::ReferenceExpression<T0>&string, Ctr::ArrayData<T1>&result);
	template <typename T0, typename T1>
		void	explodeCallback(bool isDelimiter(T0), const T0*string, Ctr::ArrayData<T1>&result);
	template <typename T0, typename T1>
		void	explodeCallback(bool isDelimiter(T0), const StringType::Template<T0>&string, Ctr::ArrayData<T1>&result);
	template <typename T0, typename T1>
		void	explodeCallback(bool isDelimiter(T0), const StringType::ReferenceExpression<T0>&string, Ctr::ArrayData<T1>&result);




	template <typename T0, typename T1>
		StringType::Template<T1>		implode(const StringType::Template<T0>&glue, const Ctr::ArrayRef<StringType::ReferenceExpression<T1> >&pieces);
	template <typename T>
		StringType::Template<T>		implode(T glue, const Ctr::ArrayRef<StringType::ReferenceExpression<T> >&pieces);
	template <typename T0, typename T1>
		StringType::Template<T1>		implode(const T0*glue, const Ctr::ArrayRef<StringType::ReferenceExpression<T1> >&pieces);

	template <typename T0, typename T1>
		StringType::Template<T1>		implode(const StringType::Template<T0>&glue, const Ctr::ArrayRef<StringType::Template<T1> >&pieces);
	template <typename T>
		StringType::Template<T>		implode(T glue, const Ctr::ArrayRef<StringType::Template<T> >&pieces);
	template <typename T0, typename T1>
		StringType::Template<T1>		implode(const T0*glue, const Ctr::ArrayRef<StringType::Template<T1> >&pieces);
	template <typename T0, typename T1>
		StringType::Template<T1>		implode(const StringType::Template<T0>&glue, const StringType::Template<T1>*pieces, count_t numPieces);
	template <typename T>
		StringType::Template<T>		implode(T glue, const StringType::Template<T>*pieces, count_t numPieces);
	template <typename T0, typename T1>
		StringType::Template<T1>		implode(const T0*glue, const StringType::Template<T1>*pieces, count_t numPieces);


	template <typename T0, typename T1>
		void 	wrap(const T0*string, size_t line_length, Ctr::ArrayData<StringType::Template<T1> >&result);
	template <typename T0, typename T1>
		void 	wrap(const T0*string, size_t line_length, size_t (*lengthFunction)(T0 character),Ctr::ArrayData<StringType::Template<T1> >&result);
	template <typename T0, typename T1>
		void 	wrapf(const T0*string, float line_length, float (*lengthFunction)(T0 character),Ctr::ArrayData<StringType::Template<T1> >&result);


	template <typename T0, typename T1>
		void 	wrap(const StringType::Template<T0>&string, size_t line_length, Ctr::ArrayData<StringType::Template<T1> >&result);
	template <typename T0, typename T1>
		void 	wrap(const StringType::Template<T0>&string, size_t line_length, size_t (*lengthFunction)(T0 character),Ctr::ArrayData<StringType::Template<T1> >&result);
	template <typename T0, typename T1>
		void 	wrapf(const StringType::Template<T0>&string, float line_length, float (*lengthFunction)(T0 character),Ctr::ArrayData<StringType::Template<T1> >&result);




	void			ShowString(const String&line);

	template <typename T>
		inline std::ostream& operator<<(std::ostream&stream, const StringType::Template<T>&string);

	template <typename E0, typename E1>
		inline std::ostream& operator<<(std::ostream&stream, const StringType::ConcatExpression<E0,E1>&expression);
	template <typename T>
		inline std::ostream& operator<<(std::ostream&stream, const StringType::ReferenceExpression<T>&expression);
	template <typename T>
		inline std::ostream& operator<<(std::ostream&stream, const StringType::CharacterExpression<T>&expression);

	#ifdef WCOUT
		template <typename T>
			inline std::wostream& operator<<(std::wostream&stream, const StringType::Template<T>&string);
		template <typename E0, typename E1>
			inline std::wostream& operator<<(std::wostream&stream, const StringType::ConcatExpression<E0,E1>&expression);
		template <typename T>
			inline std::wostream& operator<<(std::wostream&stream, const StringType::ReferenceExpression<T>&expression);
		template <typename T>
			inline std::wostream& operator<<(std::wostream&stream, const StringType::CharacterExpression<T>&expression);
	#endif




	template <typename T>
		class StrategySelector<StringType::Template<T> >
		{
		public:
				typedef	AdoptStrategy		Default;
		};

	
	#if SYSTEM==UNIX

		String	ToUTF8(const StringW&str);
	#endif


	


	#undef FOPEN
	#if SYSTEM==WINDOWS
		typedef StringW	PathString;
		#define FOPEN(_FILE_,_MODE_)	_wfopen((_FILE_),L##_MODE_)
	#else
		typedef String	PathString;	//uses UTF8
		#define FOPEN(_FILE_,_MODE_)	fopen((_FILE_),_MODE_)
	#endif




	#include "str_class.tpl.h"
}

#endif
