#ifndef basic_mathH
#define basic_mathH

/******************************************************************

Collection of basic template functions.

******************************************************************/

#include <stdlib.h>
#include <math.h>
#ifdef __BORLANDC__
	#include <math.hpp>
#endif
#include <limits.h>

#include <cmath>

#if defined(__GNUC__) || defined(__STDC__)
	#define __STDC_LIMIT_MACROS
	#include <stdint.h>
#endif




#include "../general/undef.h"

#include "../global_string.h"
#include "../global_root.h"






#ifndef INT_MAX
	#define INT_MAX INT32_MAX
#endif



#include "definitions.h"


/*!
	\brief General math namespace

*/
namespace Math
{
		template <count_t Val0, count_t Val1>
			struct Eval
			{
				static const count_t	Min = Val0 < Val1?Val0:Val1;
				static const count_t	Max = Val0 > Val1?Val0:Val1;
				static const bool		GreaterOrEqual=Val0>=Val1;
				static const bool		Greater=Val0>Val1;
				static const bool		Equal = Val0==Val1;
				static const bool		NotEqual = Val0!=Val1;
				static const bool		LessOrEqual=Val0<=Val1;
				static const bool		Less=Val0<Val1;
			};



			template <typename T>
				T		rnd(const T&val);
			//#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__GNUC__)
			//	float	round(float f);
			//#else
			//	#define round(x)	std::round(x)
			//#endif
			
			#ifdef _MSC_VER
				MF_DECLARE(float)	fmax(float a, float b);
			#endif

			float		belowOne(float f);

			double		dRound(double f);
			double		dSqr(double f);

			char*		makeStr(unsigned len);
			void		dropStr();
			
		#if 0
			String	 float2str(double,BYTE precission);
			String	 doubleStr(double f, bool force);
			String	 floatStr(float f, bool force=false);
		#endif


	inline   __declspec(nothrow)	void	__fastcall test() ;


	MFUNC3	(C0)		linearStep(C0 v, C1 lower, C2 upper);		 					//!< Returns linear step \param v Value to examine \param lower Lower boundary \param upper Upper boundary \return 0 for v=lower, 1 for v=upper, and (v-lower)/(upper-lower) for lower<v<upper
	MFUNC3	(C0)		cosStep(C0 v, C1 lower, C2 upper);								//!< Returns cos smoothed step \param v Value to examin \param lower Lower boundary \param upper Upper boundary \return cosFactor(linearStep(...))
	MFUNC3	(C0)		cubicStep(C0 v, C1 lower, C2 upper);							//!< Returns cubic (x^3)  smoothed step \param v Value to examin \param lower Lower boundary \param upper Upper boundary \return cubicFactor(linearStep(...))
	MFUNC3	(C0)		smoothstep(C1 lower, C2 upper,C0 v)	{return cubicStep(v,lower,upper);}
	MFUNC3	(C0)		clamp(C0 v, C1 min, C2 max);									//!< Returns clamped value \param v Value to examine \param min Lower boundary \param max Upper boundary \return Value clamped to [min,max]
	MFUNC3	(C0)		clamped(C0 v, C1 min, C2 max);									//!< Returns clamped value \param v Value to examine \param min Lower boundary \param max Upper boundary \return Value clamped to [min,max]
	MFUNC2	(C0)		aligned(C0 i, C1 at);											//!< Aligns i to the upper step ceiling \param i Value to examine \param at Step \return ceil(i/at)*at
	MFUNC	(C)			cosFactor(C f);													//!< Smooths f using cos \param f Value to examine in the boundary [0,1] \return smoothed factor [0,1]
	MFUNC	(C)			cubicFactor(C f);												//!< Smooths f using a cubic function \param f Value to examine in the boundary [0,1] \return smoothed factor [0,1]
	MFUNC2	(C0)		loop_exp(C0 b, C1 e);											//!< Returns b^e where C1 should be an integer type \param b Basis \param e Exponent \return b^e
	MFUNC	(C)			notZero(C f);											 		//!< Insures that f is not zero. \param f Value to examine \return f if f is non-zero. Otherwise a small non-zero value
	MFUNC3	(bool)		sqrEquationHasResults(C0 a, C1 b, C2 c); 						//!< Determines whether or not ax^2 + bx + c = 0 has at least one result. @return True if the equation has at least one result, false otherwise
	MFUNC4	(BYTE)		solveSqrEquation(C0 a, C1 b, C2 c, C3 rs[2]);					//!< Solves ax^2 + bx + c = 0. \param rs Result field - must be at least 2 elements in size \return Number of solutions (0, 1 or 2)
	MFUNC3	(BYTE)		solveSqrEquation(C0 p, C1 q, C2 rs[2]);							//!< Solves x^2 + px + q = 0. \param rs Result field - must be at least 2 elements in size \return Number of solutions (0, 1 or 2)
	
	MFUNC9	(void)		determineCubicParameters(C0 x1, C1 y1, C2 x2, C3 y2, C4 x3, C5 y3, C6&a, C7&b, C8&c);
	MFUNC	(C)			smallestPositiveResult(const C rs[2],BYTE num_res);			 	//!< Extracts the smallest positive result from a solveSqrEquation call \param rs Result field used in a solveSqrEquation call \param num_res Number of results. Must be 0 or 1 \return Smallest positive result
	MFUNC	(C)			sqr(C f);												 		//!< Calculates the square of f \param f Value to calculate square of \return f*f
	MFUNC	(C)			cubic(C f);														//!< Calculates the cube of f \param f Value to calculate square of \return f*f*f
	MFUNCV	(C)			resolveNURBSweight(unsigned element, const C&fc);				//!< Probably broken
	MFUNC	(char)		sign(C f);														//!< Extracts the sign of f \param f Value to examine \return -1 for f < -error, 0 for	error >= f >= -error, 1 otherwise
	MFUNC3	(C0)		normalDistribution(C0 x, C1 mean, C2 deviation);				//!< Estimates the normal distributed probability
	
	
	MFUNC	(bool)		nearingZero(C value);											//!< Checks if the specified value is approximately 0 (using getError<C>())
	MFUNC	(bool)		nearingOne(C value);											//!< Checks if the specified value is approximately 1 (using getError<C>())
	MFUNC2	(bool)		similar(C0 v0, C1 v1);											//!< Checks if the two specified values are similar (using getError<C>())
	MFUNC3	(bool)		similar(C0 v0, C1 v1, C2 tolerance);							//!< Checks if the two specified values are similar (using tolerance)

	template <unsigned Value>
	MF_DECLARE	(unsigned)	factorial();													//!< Calculates factorial during compile-time \return Value!
	MF_DECLARE	(unsigned)	factorial(unsigned value);										//!< Calculates factorial during run-time \param value Value to calculate factorial from \return value!
	MF_DECLARE	(void)		toggleBool(bool&b);											 	//!< Toggle boolean value \param b Boolean to toggle (true => false / false => true)

	MFUNC2	(C0)		vmin(C0 value0, C1 value1);						 				//!< Calculate min \return Lesser of both values (parsed to C0)
	MFUNC2	(C0)		vmax(C0 value0, C1 value1);						 				//!< Calculate max \return Greater of both values (parsed to C0)
	MFUNC3	(C0)		vmin(C0 value0, C1 value1, C2 value2);							//!< Calculates the minimum of three values \return Least of all three values (parsed to C0)
	MFUNC3	(C0)		vmax(C0 value0, C1 value1, C2 value2);							//!< Calculates the maximum of three values \return Greatest of all three values (parsed to C0)
	MFUNC	(C)			vabs(C v);														//!< Calculate abs
	MFUNC	(C)			vsqrt(C v);														//!< Calculate square root
	MFUNC	(C)			vlog(C v);														//!< Calculates log(v)
	MFUNC2	(C0)		vpow(C0 b,C1 e);												//!< Calculates b^e
	MFUNC	(C)			vexp(C e);														//!< Calculates (e)^e
	MFUNC	(C)			vpow10(C e);													//!< Calculates 10^e
	MFUNC	(C)			vlog10(C v);													//!< Calculates log(10)e
	MFUNC	(C)			vsin(C v);
	MFUNC	(C)			vcos(C v);
	MFUNC	(C)			vasin(C v);
	MFUNC	(C)			vacos(C v);
	MFUNC	(C)			vtan(C v);
	MFUNC	(C)			vatan(C v);
	MFUNC2	(C0)		vatan2(C0 n, C1 d);

	MFUNC	(char)		vChar();														//!< Determines type-specific character \return character abbreviation of the specified type
	MFUNC	(const char*)	vName();
	MFUNC	(String)	v2str(const C&value);											//!< Converts value to string. \param value Value to convert \return String containing converted value
	MFUNC	(C)			getError();													 //!< Query error constant \return A type-specific error-constant

	MFUNC	(C)			gaussian(C value);											//!< Retrieves the normal distributed probability for the specified value

	
	/**
		@brief Variants of standard math functions for by-reference parameters
		
		Use these variants if your used classes are considerably larger than standard primitives and/or expensive to copy
	*/
	namespace ByReference
	{
	
		MFUNC3	(C0)		linearStep(const C0&v, const C1&lower, const C2&upper);		//!< By reference variant of Math::linearStep()
		MFUNC3	(C0)		cosStep(const C0&v, const C1&lower, const C2&upper);		//!< By reference variant of Math::cosStep()
		MFUNC3	(C0)		cubicStep(const C0&v, const C1&lower, const C2&upper);		//!< By reference variant of Math::cubicStep()
		MFUNC3	(C0)		clamp(const C0&v, const C1&min, const C2&max);				//!< By reference variant of Math::clamp()
		MFUNC3	(C0)		clamped(const C0&v, const C1&min, const C2&max);			//!< By reference variant of Math::clamped()
		MFUNC2	(C0)		aligned(const C0&i, const C1&at);							//!< By reference variant of Math::aligned()
		MFUNC	(C)			cosFactor(const C&f);										//!< By reference variant of Math::cosFactor()
		MFUNC	(C)			cubicFactor(const C&f);										//!< By reference variant of Math::cubicFactor()
		MFUNC2	(C0)		loop_exp(const C0&b, const C1&e);							//!< By reference variant of Math::loop_ext()
		MFUNC	(C)			notZero(const C&f);											//!< By reference variant of Math::notZero()
		MFUNC3	(bool)		sqrEquationHasResults(const C0&a, const C1&b, const C2&c);		//!< By reference variant of Math::sqrEquationHasResults()
		MFUNC4	(BYTE)		solveSqrEquation(const C0&a, const C1&b, const C2&c, C3 rs[2]); //!< By reference variant of Math::solveSqrEquation()
		MFUNC3	(BYTE)		solveSqrEquation(const C0&p, const C1&q, C2 rs[2]);				//!< By reference variant of Math::solveSqrEquation()
		MFUNC	(C)			sqr(const C&f);												//!< By reference variant of Math::sqr()
		MFUNC	(C)			cubic(const C&f);											//!< By reference variant of Math::cubic()
		MFUNC	(char)		sign(const C&f);											//!< By reference variant of Math::sign()
		MFUNC3	(C0)		normalDistribution(const C0&x, const C1&mean, const C2&deviation);	//!< By reference variant of Math::normalDistribution()
		
		
		MFUNC	(bool)		nearingZero(const C&value);									//!< By reference variant of Math::nearingZero()
		MFUNC	(bool)		nearingOne(const C&value);									//!< By reference variant of Math::nearingOne()
		MFUNC2	(bool)		similar(const C0&v0, const C1&v1);							//!< By reference variant of Math::similar()
		MFUNC3	(bool)		similar(const C0&v0, const C1&v1, const C2&tolerance);		//!< By reference variant of Math::similar()

		MFUNC2	(C0)		vmin(const C0&value0, const C1&value1);						//!< By reference variant of Math::vmin()
		MFUNC2	(C0)		vmax(const C0&value0, const C1&value1);						//!< By reference variant of Math::vmax()
		MFUNC3	(C0)		vmin(const C0&value0, const C1&value1, const C2&value2);	//!< By reference variant of Math::vmin()
		MFUNC3	(C0)		vmax(const C0&value0, const C1&value1, const C2&value2);	//!< By reference variant of Math::vmax()
		MFUNC	(C)			vabs(const C&v);											//!< By reference variant of Math::vabs()
		MFUNC	(C)			vsqrt(const C&v);											//!< By reference variant of Math::vsqrt()
		MFUNC	(C)			vlog(const C&v);											//!< By reference variant of Math::vlog()
		MFUNC2	(C0)		vpow(const C0&b,const C1&e);								//!< By reference variant of Math::vpow()
		MFUNC	(C)			vexp(const C&e);											//!< By reference variant of Math::vexp()
		MFUNC	(C)			vpow10(const C&e);											//!< By reference variant of Math::vpow10()
		MFUNC	(C)			vlog10(const C&v);											//!< By reference variant of Math::vlog10()
		MFUNC	(C)			vsin(const C&v);
		MFUNC	(C)			vcos(const C&v);
		MFUNC	(C)			vasin(const C&v);
		MFUNC	(C)			vacos(const C&v);
		MFUNC	(C)			vtan(const C&v);
		MFUNC	(C)			vatan(const C&v);
		MFUNC2	(C0)		vatan2(const C0&n, const C1&d);

		MFUNC	(C)			gaussian(const C&value);									//!< By reference version of Math::gaussian()
	
	
	
	
	}



	/**
	Logical storage class that distinguishes only two values: 0 and 1
	*/
	class Bit
	{
	private:
		BYTE	value;
	public:
		/**/	Bit(int val = 0) : value(val ? 1 : 0)	{DBG_ASSERT__(val == 0 || val == 1);}
		Bit&	operator=(int val) {DBG_ASSERT__(val == 0 || val == 1); value = val ? 1 : 0;return *this;}
		operator int() const {return (int)value;}
	};


	template <typename T>
		class TypeInfo
		{
			/*
				A TypeInfo definition must provide the following definitions and static const variables:
				typedef Type: Reflection to type T
				typedef SignedType: Typedef to a TypeInfo<*> that provides the signed variant of the local type. If no such exists or the local type already matches the specification then the typedef should link to this type
				typedef UnsignedType: Typedef to a TypeInfo<*> that provides the unsigned variant of the local type. If no such exists or the local type already matches the specification then the typedef should link to this type
				typedef GreaterType: Typedef to a TypeInfo<*> that provides a greater integer range. Floating point type infos should link to themselves.
				typedef LesserType: Typedef to a TypeInfo<*> that provides a lesser integer range. Floating point type infos should link to themselves.
				bool is_signed: true if this data type is signed
				bool is_float: true if this data type is floating point
				T zero: 0 constant
				T max: Maximum (finite and valid for floats) possible value
				T min: Minimum (finite and valid for floats) possible value. This value is negative for signed types and 0 for unsigned ones
				const char*name: Name of this type
			*/

			typedef TFalse		IsPrimitive;
		};

	template <>
		class TypeInfo<INT8>
		{
		public:
		typedef INT8		Type;
		typedef TypeInfo<UINT8>	UnsignedType;
		typedef TypeInfo<INT8>	SignedType;
		typedef TypeInfo<INT16>	GreaterType;
		typedef TypeInfo<INT8>	LesserType;
		typedef TTrue		IsPrimitive;
		static const bool	is_signed=true,
							is_float=false;
		static const Type	zero = 0;
		static const Type	max = 0x7F;
		static const Type	min = -max-1;
		static const Type	error = 0;
		static const char*	name;
		};

	template <>
		class TypeInfo<INT16>
		{
		public:
		typedef INT16		Type;
		typedef TypeInfo<UINT16>UnsignedType;
		typedef TypeInfo<INT16>	SignedType;
		typedef TypeInfo<INT32>	GreaterType;
		typedef TypeInfo<INT8>	LesserType;
		typedef TTrue		IsPrimitive;
		static const bool	is_signed=true,
							is_float=false;
		static const Type	zero = 0;
		static const Type	max = 0x7FFF;
		static const Type	min = -max-1;
		static const Type	error = 0;
		static const char*	name;
		};

	template <>
		class TypeInfo<INT32>
		{
		public:
		typedef INT32		Type;
		typedef TypeInfo<UINT32>UnsignedType;
		typedef TypeInfo<INT32>	SignedType;
		typedef TypeInfo<INT64>	GreaterType;
		typedef TypeInfo<INT16>	LesserType;
		typedef TTrue		IsPrimitive;
		static const bool	is_signed=true,
							is_float=false;
		static const Type	zero = 0;
		static const Type	max = 0x7FFFFFFF;
		static const Type	min = -max-1;
		static const Type	error = 0;
		static const char*	name;
		};

	template <>
		class TypeInfo<INT64>
		{
		public:
		typedef INT64		Type;
		typedef TypeInfo<UINT64>UnsignedType;
		typedef TypeInfo<INT64>	SignedType;
		typedef TypeInfo<INT64>	GreaterType;
		typedef TypeInfo<INT32>	LesserType;
		typedef TTrue		IsPrimitive;
		static const bool	is_signed=true,
							is_float=false;
		static const Type	zero = 0;
		static const Type	max = 0x7FFFFFFFFFFFFFFFLL;
		static const Type	min = -max-1;
		static const Type	error = 0;
		static const char*	name;
		};

	template <>
		class TypeInfo<UINT8>
		{
		public:
		typedef UINT8		Type;
		typedef TypeInfo<UINT8>	UnsignedType;
		typedef TypeInfo<INT8>	SignedType;
		typedef TypeInfo<UINT16>GreaterType;
		typedef TypeInfo<UINT8>	LesserType;
		typedef TTrue		IsPrimitive;
		static const bool	is_signed=false,
							is_float=false;
		static const Type	zero = 0;
		static const Type	max = 0xFF;
		static const Type	min = 0x00;
		static const Type	error = 0;
		static const char*	name;
		};

	template <>
		class TypeInfo<UINT16>
		{
		public:
		typedef UINT16		Type;
		typedef TypeInfo<UINT16>UnsignedType;
		typedef TypeInfo<INT16>	SignedType;
		typedef TypeInfo<UINT32>GreaterType;
		typedef TypeInfo<UINT8>	LesserType;
		typedef TTrue		IsPrimitive;
		static const bool	is_signed=false,
							is_float=false;
		static const Type	zero = 0;
		static const Type	max = 0xFFFF;
		static const Type	min = 0x0000;
		static const Type	error = 0;
		static const char*	name;
		};

	template <>
		class TypeInfo<UINT32>
		{
		public:
		typedef UINT32		Type;
		typedef TypeInfo<UINT32>UnsignedType;
		typedef TypeInfo<INT32>	SignedType;
		typedef TypeInfo<UINT64>GreaterType;
		typedef TypeInfo<UINT16>LesserType;
		typedef TTrue		IsPrimitive;
		static const bool	is_signed=false,
							is_float=false;
		static const Type	zero = 0;
		static const Type	max = 0xFFFFFFFF;
		static const Type	min = 0x00000000;
		static const Type	error = 0;
		static const char*	name;
		};

	template <>
		class TypeInfo<UINT64>
		{
		public:
		typedef UINT64		Type;
		typedef TypeInfo<UINT64>UnsignedType;
		typedef TypeInfo<INT64>	SignedType;
		typedef TypeInfo<UINT64>GreaterType;
		typedef TypeInfo<UINT32>LesserType;
		typedef TTrue		IsPrimitive;
		static const bool	is_signed=false,
							is_float=false;
		static const Type	zero = 0;
		static const Type	max = 0xFFFFFFFFFFFFFFFFULL;
		static const Type	min = 0x0000000000000000ULL;
		static const Type	error = 0;
		static const char*	name;
		};

	template <>
		class TypeInfo<float>
		{
		public:
		typedef float		Type;
		typedef TypeInfo<Type>	UnsignedType;
		typedef TypeInfo<Type>	SignedType;
		typedef TypeInfo<Type>	GreaterType;
		typedef TypeInfo<Type>	LesserType;
		typedef TTrue		IsPrimitive;
		static const bool	is_signed=true,
							is_float=true;
		static const Type	zero;
		static const Type	max;
		static const Type	min;
		static const Type	error;
		static const char*	name;
		};

	template <>
		class TypeInfo<double>
		{
		public:
		typedef double		Type;
		typedef TypeInfo<Type>	UnsignedType;
		typedef TypeInfo<Type>	SignedType;
		typedef TypeInfo<Type>	GreaterType;
		typedef TypeInfo<Type>	LesserType;
		typedef TTrue		IsPrimitive;
		static const bool	is_signed=true,
							is_float=true;
		static const Type	zero;
		static const Type	max;
		static const Type	min;
		static const Type	error;
		static const char*	name;
		};

	template <>
		class TypeInfo<long double>
		{
		public:
		typedef long double	Type;
		typedef TypeInfo<Type>	UnsignedType;
		typedef TypeInfo<Type>	SignedType;
		typedef TypeInfo<Type>	GreaterType;
		typedef TypeInfo<Type>	LesserType;
		typedef TTrue		IsPrimitive;
		static const bool	is_signed=true,
							is_float=true;
		static const Type	zero;
		static const Type	max;
		static const Type	min;
		static const Type	undefined;
		static const Type	error;
		static const char*	name;
		};





}

using namespace Math;
namespace M = Math;

#include "basic.tpl.h"
#endif
