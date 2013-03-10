#ifndef global_rootH
#define global_rootH

/******************************************************************

Global super-include. This file is included by all .cpp-files.

******************************************************************/

#include "general/undef.h"
#include <stdlib.h>

#undef DEBUG_LEVEL

#define DEBUG_LEVEL 0

/** debug-level **

0: no checking
1: new/delete-checking
2: list-checking

*/


/**
	@brief Type that indicates, well, nothing. Constructors that accept this type should behave just like parameter-less constructors
*/
struct Nothing
{
	inline bool operator==(const Nothing&)	const	{return true;}
	inline bool operator!=(const Nothing&)	const	{return false;}
};


#include "general/enumeration.h"





/**
	@brief True type to use with template evaluations
 */
struct TTrue
{
	static const bool is_true=true;
};

/**
	@brief False type to use with template evaluations
 */
struct TFalse
{
	static const bool is_false=true;
};


namespace DW
{

	/**
		@brief Base object that only provides a destructor and the character of an object, rather than a void pointer
	*/
	class Object
	{
	public:
	virtual			~Object()	{};
	};

	inline bool isLittleEndian()
	{
		static const int	one = 1;
		return (((unsigned char*)(&one))[0]==1);
	}

}
using namespace DW;








#ifdef _MSC_VER
	#pragma warning(disable:4355 4305 4244 4101 4996)

	#include <stdio.h>
	static inline FILE*	_fopen(const char*filename, const char*open_mode)
	{
		FILE* result;
		if (fopen_s(&result,filename,open_mode) != 0)
			return NULL;
		return result;
	}

	#define fopen(FILENAME,OPENMODE)	_fopen(FILENAME,OPENMODE)

#endif


/* SYSTEMs and SYSTEM_TYPEs  */

#undef LINUX

#define WINDOWS 0
#define UNIX    1
#define LINUX   2
#define FREEBSD	3
#define MACOS   4
#define MACOSX  5
#define SOLARIS 6
#define OTHER   7
#define UNKNOWN 8


/*depending on the type of system being compiled under the macros SYSTEM and SYSTEM_TYPE are specified.
SYSTEM specifies the general system (eg. UNIX) while SYSTEM_VARIANCE characterizes the sub-system (eg. LINUX)*/

#if defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(_Windows)
    #define SYSTEM              WINDOWS
    #define SYSTEM_VARIANCE     WINDOWS
    #define SYSTEM_NAME         "Windows"
#elif defined(__linux) || defined(linux) || defined(unix) || defined(__unix__) || defined(__unix) || defined(__FreeBSD__)
    #define SYSTEM              UNIX
    #if defined(__linux) || defined(linux)
        #define SYSTEM_VARIANCE LINUX
        #define SYSTEM_NAME     "Linux"
    #elif defined(__FreeBSD__)
		#define SYSTEM_VARIANCE	FREEBSD
		#define SYSTEM_NAME		"FreeBSD"
	#elif defined(__SUNPRO_C) || defined(__sun) || defined(sun)
		#define SYSTEM_VARIANCE     SOLARIS
		#define SYSTEM_NAME         "Solaris"
	#else
		#define SYSTEM_VARIANCE UNIX
		#define SYSTEM_NAME     "Unix"
	#endif
#elif defined(__MACOS__) || defined (__MacOS__)
	#define SYSTEM				MACOS
	#define SYSTEM_NAME			"MacOS"
    #define SYSTEM_VARIANCE     MACOS
#elif defined(__MACOSX__) || defined(__MacOSX__)
    #define SYSTEM              MACOSX
	#define SYSTEM_NAME			"MacOSX"
    #define SYSTEM_VARIANCE     MACOSX
#elif defined(__SYMBIAN32__)
    #define SYSTEM              SYMBIAN
    #define SYSTEM_VARIANCE     SYMBIAN
    #define SYSTEM_NAME         "Symbian"
#else
    #define SYSTEM              OTHER
    #define SYSTEM_VARIANCE     UNKNOWN
    #define SYSTEM_NAME         "Unspecified System"
#endif






#ifdef __BORLANDC__
    #include <vcl.h>
    #pragma hdrstop
    #include <windef.h>
    #include <algorith.h>
#else
    #ifdef _WIN32
        #undef _WIN32_WINNT //override
		#define _WIN32_WINNT 0x1000
		#include <WinSock2.h>
        #include <windows.h>
		
    #else
        #include <string.h>
    #endif
    #include <stdio.h>
	#undef ARRAYSIZE
    #define ARRAYSIZE(array) (sizeof(array)/sizeof(array[0]))


#endif

    template <class C> inline void  swp(C&e0, C&e1)
                                    {
                                        C temp = e0;
                                        e0 = e1;
                                        e1 = temp;
                                    }


#define UNSIGNED_UNDEF      ((unsigned)-1)
#define UINT32_UNDEF      	(0xFFFFFFFF)
#define MAX_TEMPLATE_DEPTH  300


#include "general/typedefs.h"

//#define INLINE inline __attribute__((always_inline))
#define INLINE  inline




#include "general/exception.h"

#if DEBUG_LEVEL > 0
    #include "general/memory_shield.h"


    #define SHIELD                      mem_shield.locate(__FILE__,__func__,__LINE__)

    #define DISCARD(item)               {SHIELD.release(item); delete item;}
    #define DISCARD_ARRAY(item)         {SHIELD.release(item); delete[] item;}
    #define SHIELDED                    SHIELD.shield
    #define SHIELDED_ARRAY              SHIELD.shieldArray
#else

    #define DISCARD(item)               {delete (item); }
    #define DISCARD_ARRAY(item)         {delete[] (item); }
    #define SHIELDED
    #define SHIELDED_ARRAY(item, count) (item)

#endif

#include "general/alloc_templates.h"
//#include <general/managed.h>


#if SYSTEM!=WINDOWS

    struct RECT
    {
        short   left,
                top,
                right,
                bottom;
    };
    
    struct POINT
    {
        int             x,y;
    };

    #define FOLDER_SLASH        '/'
    #define FOLDER_SLASH_STR    "/"
    #define ENV_PATH_SEPARATOR  ':'
    #define FS_CASE_IGNORE      false
    #define FS_SHARED_LIB_EXT   "so"

//    #include <general/xportal.h>

#else
    #ifndef STILL_RUNNING
        #define STILL_RUNNING   0x0103
    #endif
    #define FOLDER_SLASH        '\\'
    #define FOLDER_SLASH_STR    "\\"
    #define ENV_PATH_SEPARATOR  ';'
    #define FS_CASE_IGNORE      true
    #define FS_SHARED_LIB_EXT   "dll"
	
	#define popen				_popen
	#define pclose				_pclose

	
	
	
#endif

#define elif    else if
    
	
#include "general/root.h"

/*#if !defined(main) && !defined(_DLL)
	#error main not defined
#endif*/


/* naming-convention *

class   ObjectName
struct  SFragmentName	(outdated)
struct  TFragmentName		<-- use this
struct  tFragmentName	(outdated)

template <class ObjectName, int IntParameter> ...

ObjectName::functionName(...)

namespace NameSpace {...}            <-- use this
namespace name_space {...}

ObjectName variable_name;

typedef (...)   TypeName		<-- applicable if nested (outdated)
typedef (basic)	tTypeName
typedef (class)	TypeName
typedef (struct)	TTypeName
typedef (func)	pFuncTypeName

enum    en_EnumerationName{ST_OPT0, ST_OPT1, ...}	(outdated)
enum    eEnumerationName{...}                       <-- use this

struct/class/union/namespace
{
	enum EnumeratorName {Option0, Option1, ...}    setting;
	struct SubStruct	{};	<-- only in structs/classes
	struct TSubStruct	{};	<-- use this
	class SubClass	{};	<-- only in structs/classes
	class SubClass	{};	<-- use this
};



Obviously the originally very strict rules are slowly softened. More and more objects can be written ObjectName though
classes should be written ObjectName and structs TStructName for all times (for clearer distinction).
Enumerations can practically have any naming-style as long as they remain inside a class. Otherwise use the standard
#define-like style preceded by a short 1 to 3 character long enum-class (i.e. 'EO_ENUM_OPTION') to avoid global ambiguity.

Functions should remain functionName for the time being though FunctionName might be considerable.
Functionpointer-types are usually written _pointerType or pPointerType but this is no fixed rule.


*/



/* addresses:

bad:
0x00000000 (:P)
0xabababab
0xbaadf00d


unknown:
0x2710a7f


gcc-error-messages and what they mean:

* request for member 'x' in 'y' which is of non-class type 'z'
    :exchange '.' with '->' or the other way around.

* passing `const x' as `this' argument of `y' discards qualifiers
    :make function y non const


*/

#define foreach(LIST,ITERATOR)	for (auto ITERATOR = (LIST).begin(); ITERATOR != (LIST).end(); ++ITERATOR)



#endif
