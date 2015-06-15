#ifndef typedefsH
#define typedefsH

/******************************************************************

Global typedefs of types that should be available anywhere
in the foundation.

******************************************************************/

#if SYSTEM==UNIX
	#include "../gl/glx.h"
#endif




#if !defined(_MSC_VER) || 1
	#include <stdint.h>
#else




	#if _MSC_VER > 1000
		#pragma once
	#endif


	// 7.18.1 Integer types

	// 7.18.1.1 Exact-width integer types
	typedef __int8            int8_t;
	typedef __int16           int16_t;
	typedef __int32           int32_t;
	typedef __int64           int64_t;
	typedef unsigned __int8   uint8_t;
	typedef unsigned __int16  uint16_t;
	typedef unsigned __int32  uint32_t;
	typedef unsigned __int64  uint64_t;

	// 7.18.1.2 Minimum-width integer types
	typedef int8_t    int_least8_t;
	typedef int16_t   int_least16_t;
	typedef int32_t   int_least32_t;
	typedef int64_t   int_least64_t;
	typedef uint8_t   uint_least8_t;
	typedef uint16_t  uint_least16_t;
	typedef uint32_t  uint_least32_t;
	typedef uint64_t  uint_least64_t;

	// 7.18.1.3 Fastest minimum-width integer types
	typedef int8_t    int_fast8_t;
	typedef int16_t   int_fast16_t;
	typedef int32_t   int_fast32_t;
	typedef int64_t   int_fast64_t;
	typedef uint8_t   uint_fast8_t;
	typedef uint16_t  uint_fast16_t;
	typedef uint32_t  uint_fast32_t;
	typedef uint64_t  uint_fast64_t;

	// 7.18.1.4 Integer types capable of holding object pointers
	#if defined(_WIN64) || defined(_M_X64) // [
	   typedef __int64           intptr_t;
	   typedef unsigned __int64  uintptr_t;
	#else // _WIN64 ][
	   typedef int               intptr_t;
	   typedef unsigned int      uintptr_t;
	#endif // _WIN64 ]

	// 7.18.1.5 Greatest-width integer types
	typedef int64_t   intmax_t;
	typedef uint64_t  uintmax_t;


	// 7.18.2 Limits of specified-width integer types

	#if !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS) // [   See footnote 220 at page 257 and footnote 221 at page 259

	// 7.18.2.1 Limits of exact-width integer types
	#define INT8_MIN     ((int8_t)_I8_MIN)
	#define INT8_MAX     _I8_MAX
	#define INT16_MIN    ((int16_t)_I16_MIN)
	#define INT16_MAX    _I16_MAX
	#define INT32_MIN    ((int32_t)_I32_MIN)
	#define INT32_MAX    _I32_MAX
	#define INT64_MIN    ((int64_t)_I64_MIN)
	#define INT64_MAX    _I64_MAX
	#define UINT8_MAX    _UI8_MAX
	#define UINT16_MAX   _UI16_MAX
	#define UINT32_MAX   _UI32_MAX
	#define UINT64_MAX   _UI64_MAX

	// 7.18.2.2 Limits of minimum-width integer types
	#define INT_LEAST8_MIN    INT8_MIN
	#define INT_LEAST8_MAX    INT8_MAX
	#define INT_LEAST16_MIN   INT16_MIN
	#define INT_LEAST16_MAX   INT16_MAX
	#define INT_LEAST32_MIN   INT32_MIN
	#define INT_LEAST32_MAX   INT32_MAX
	#define INT_LEAST64_MIN   INT64_MIN
	#define INT_LEAST64_MAX   INT64_MAX
	#define UINT_LEAST8_MAX   UINT8_MAX
	#define UINT_LEAST16_MAX  UINT16_MAX
	#define UINT_LEAST32_MAX  UINT32_MAX
	#define UINT_LEAST64_MAX  UINT64_MAX

	// 7.18.2.3 Limits of fastest minimum-width integer types
	#define INT_FAST8_MIN    INT8_MIN
	#define INT_FAST8_MAX    INT8_MAX
	#define INT_FAST16_MIN   INT16_MIN
	#define INT_FAST16_MAX   INT16_MAX
	#define INT_FAST32_MIN   INT32_MIN
	#define INT_FAST32_MAX   INT32_MAX
	#define INT_FAST64_MIN   INT64_MIN
	#define INT_FAST64_MAX   INT64_MAX
	#define UINT_FAST8_MAX   UINT8_MAX
	#define UINT_FAST16_MAX  UINT16_MAX
	#define UINT_FAST32_MAX  UINT32_MAX
	#define UINT_FAST64_MAX  UINT64_MAX

	// 7.18.2.4 Limits of integer types capable of holding object pointers
	#ifdef _WIN64 // [
	#  define INTPTR_MIN   INT64_MIN
	#  define INTPTR_MAX   INT64_MAX
	#  define UINTPTR_MAX  UINT64_MAX
	#else // _WIN64 ][
	#  define INTPTR_MIN   INT32_MIN
	#  define INTPTR_MAX   INT32_MAX
	#  define UINTPTR_MAX  UINT32_MAX
	#endif // _WIN64 ]

	// 7.18.2.5 Limits of greatest-width integer types
	#define INTMAX_MIN   INT64_MIN
	#define INTMAX_MAX   INT64_MAX
	#define UINTMAX_MAX  UINT64_MAX

	// 7.18.3 Limits of other integer types

	#ifdef _WIN64 // [
	#  define PTRDIFF_MIN  _I64_MIN
	#  define PTRDIFF_MAX  _I64_MAX
	#else  // _WIN64 ][
	#  define PTRDIFF_MIN  _I32_MIN
	#  define PTRDIFF_MAX  _I32_MAX
	#endif  // _WIN64 ]

	#define SIG_ATOMIC_MIN  INT_MIN
	#define SIG_ATOMIC_MAX  INT_MAX

	#ifndef SIZE_MAX // [
	#  ifdef _WIN64 // [
	#     define SIZE_MAX  _UI64_MAX
	#  else // _WIN64 ][
	#     define SIZE_MAX  _UI32_MAX
	#  endif // _WIN64 ]
	#endif // SIZE_MAX ]

	// WCHAR_MIN and WCHAR_MAX are also defined in <wchar.h>
	#ifndef WCHAR_MIN // [
	#  define WCHAR_MIN  0
	#endif  // WCHAR_MIN ]
	#ifndef WCHAR_MAX // [
	#  define WCHAR_MAX  _UI16_MAX
	#endif  // WCHAR_MAX ]

	#define WINT_MIN  0
	#define WINT_MAX  _UI16_MAX

	#endif // __STDC_LIMIT_MACROS ]


	// 7.18.4 Limits of other integer types

	#if !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS) // [   See footnote 224 at page 260
	
		// 7.18.4.1 Macros for minimum-width integer constants
	
		#define INT8_C(val)  val##i8
		#define INT16_C(val) val##i16
		#define INT32_C(val) val##i32
		#define INT64_C(val) val##i64
	
		#define UINT8_C(val)  val##ui8
		#define UINT16_C(val) val##ui16
		#define UINT32_C(val) val##ui32
		#define UINT64_C(val) val##ui64
	
		// 7.18.4.2 Macros for greatest-width integer constants
		#define INTMAX_C   INT64_C
		#define UINTMAX_C  UINT64_C
	
	#endif // __STDC_CONSTANT_MACROS ]
#endif

#ifdef __GNUC__

	#if SYSTEM==WINDOWS
		typedef signed	__int64 INT64;			//!< Compatiblity typedef
		typedef unsigned	__int64 UINT64;		//!< Compatiblity typedef
	#else

		typedef int64_t		__int64;		//!< Standard signed 64 bit integer
		typedef unsigned int	UINT;			//!< Compatiblity typedef
		typedef long		LONG;			//!< Compatiblity typedef
		typedef unsigned long	ULONG;			//!< Compatiblity typedef
		typedef uint32_t	UINT32;			//!< Compatiblity typedef
//		typedef int32_t		INT32;			//!< Compatiblity typedef
		typedef int			DWORD;			//!< Compatiblity typedef
		typedef short		WORD;			//!< Compatiblity typedef
		typedef int			INT;			//!< Compatiblity typedef

		typedef int32_t		__int32;		//!< Standard signed 32 bit integer
		typedef int64_t		INT64;			//!< Compatiblity typedef
		typedef uint64_t	UINT64;			//!< Compatiblity typedef
		typedef int8_t		__int8;			//!< Standard signed 8 bit integer (char)
		typedef int16_t		__int16;		//!< Standard signed 16 bit integer (short)
		typedef void*		HINSTANCE;
		typedef int			HDC;
//		typedef Window		HWND;			//long unsigned int
		typedef long unsigned int	HWND;

		#undef Bool
		typedef bool		Bool;

		typedef long long		LONGLONG;
		typedef unsigned long long DWORDLONG;
		typedef void*			PVOID;
		typedef DWORD 			HANDLE;
		typedef HANDLE*			PHANDLE;
		typedef unsigned long*	ULONG_PTR;
		typedef long*			LONG_PTR;
		typedef wchar_t			WCHAR;
		typedef	float			FLOAT;
		typedef unsigned char	UCHAR;
		typedef size_t			SIZE;
		typedef char			TCHAR;
		typedef char			CHAR;
		typedef unsigned long	ACCESS_MASK;
		
		#define MAX_PATH		255
		
		
		#define DECLSPEC_IMPORT
		#define WAVE_FORMAT_PCM	1
		#define SUCCEEDED(result) (result)
		#define WINAPI
		#define NTAPI
		#define IN
		#define OUT
		#define __cdecl
		
		#define _strnicmp	strncasecmp
		
		typedef struct { 
		    WORD  wFormatTag; 
		    WORD  nChannels; 
		    DWORD nSamplesPerSec; 
		    DWORD nAvgBytesPerSec; 
		    WORD  nBlockAlign; 
		} WAVEFORMAT; 
		
		typedef struct { 
		    WAVEFORMAT wf; 
		    WORD       wBitsPerSample; 
		} PCMWAVEFORMAT; 

	#endif


	typedef int8_t			INT8;			//!< Compatiblity typedef
	typedef uint8_t			UINT8;			//!< Compatiblity typedef
	typedef int16_t			INT16;			//!< Compatiblity typedef
	typedef uint16_t		UINT16;			//!< Compatiblity typedef (it appears gcc likes interpreting __int16 as __int32...)
	typedef signed		short   SHORT;			//!< Compatiblity typedef
	typedef unsigned	short   USHORT;			//!< Compatiblity typedef
	typedef unsigned char		BYTE;			//!< Compatiblity typedef

#endif


typedef size_t	index_t,count_t;

static const index_t		InvalidIndex = static_cast<index_t>(-1);


#if defined(_WIN64) || defined(_M_X64)
	typedef __int64	signed_index_t;
#else
	typedef int signed_index_t;
#endif



typedef uint32_t	serial_size_t;	//!< Size type for serial IO operations
static const serial_size_t MaxSerialSize = (1U << 29) - 1;	//first three bits can't be used



#ifdef __GNUC__
	#define ALIGN1  __attribute__ ((aligned(1)))
	#define ALIGN2  __attribute__ ((aligned(2)))
	#define ALIGN4  __attribute__ ((aligned(4)))
	#define ALIGN8  __attribute__ ((aligned(8)))
	#define ALIGN16 __attribute__ ((aligned(16)))
	
	#define DEF_ALIGN(TYPE,NEW_TYPE,ALIGNMENT)	typedef TYPE NEW_TYPE ALIGNMENT;
#else
	#ifdef _MSC_VER

		#define ALIGN1  __declspec(align(1))
		#define ALIGN2  __declspec(align(2))
		#define ALIGN4  __declspec(align(4))
		#define ALIGN8  __declspec(align(8))
		#define ALIGN16 __declspec(align(16))
	
		#define DEF_ALIGN(TYPE,NEW_TYPE,ALIGNMENT)	typedef ALIGNMENT TYPE NEW_TYPE;
	#else
		#define ALIGN1
		#define ALIGN2
		#define ALIGN4
		#define ALIGN8
		#define ALIGN16

		#define DEF_ALIGN(TYPE,NEW_TYPE,ALIGNMENT)	typedef TYPE NEW_TYPE;
	#endif
#endif

/*
*
when using member-variables of greater alignment than the alignment of its struct/class then the alignment of
that struct/class will be altered to the largest alignment of its members.
thus the alignment of for example __int64 or double has to be altered if the entire structure is of lesser
alignment even if their location would be aligned to 8 anyway.
*


DEF_ALIGN(UINT32,	FUINT32,	ALIGN1);
DEF_ALIGN(INT32,	FINT32, 	ALIGN1);	 //!< 1 byte align file typedef
DEF_ALIGN(INT64,	FINT64, 	ALIGN1);	 //!< 1 byte align file typedef
DEF_ALIGN(UINT64,	FUINT64,	ALIGN1);
DEF_ALIGN(INT8,		FINT8,		ALIGN1);	 //!< 1 byte align file typedef
DEF_ALIGN(UINT8,	FUINT8,		ALIGN1);	 //!< 1 byte align file typedef
DEF_ALIGN(INT16,	FINT16,		ALIGN1);	 //!< 1 byte align file typedef
DEF_ALIGN(UINT16,	FUINT16,	ALIGN1);	 //!< 1 byte align file typedef
DEF_ALIGN(BYTE,		FBYTE,		ALIGN1);	 //!< 1 byte align file typedef

*/

#endif
