#ifndef math_sseH
#define math_sseH

#include "vector.h"
#include "object.h"
#include "../global_string.h"
	


#undef SSE__
#if (!defined(__GNUC__) || defined(__SSE__))
	#define _interlockedbittestandset yadda
	#define _interlockedbittestandreset yadda2
	#include <xmmintrin.h>
	#include <pmmintrin.h>
	#ifndef __GNUC__
		#include <intrin.h>
	#endif
	#undef _interlockedbittestandset
	#undef _interlockedbittestandreset
	/*#include <xmmintrin.h>
	#include <emmintrin.h>*/
	#define SSE__
#else
	#include "object.h"
#endif
#ifdef SSE__





	
	#ifndef __GNUC__
		#define ALIGNED16	__declspec(align(16))
	#else
		#define ALIGNED16
	#endif
	
namespace DeltaWorks
{
	namespace SSE
	{
		#ifndef __GNUC__
			#define SSE_VECTOR	__m128
			#define SSE_DEFINE4	_mm_setr_ps
			#define SSE_DEFINE1	_mm_set1_ps
		#else
			class Vector
			{
			public:
					__m128		value;
			
			inline				Vector()
								{}
			inline				Vector(const __m128&v):value(v)
								{}
			inline				Vector(float w, float x, float y, float z):value(_mm_setr_ps(w,x,y,z))
								{}
			inline				Vector(float v):value(_mm_set1_ps(v))
								{}
					
			inline operator __m128() const
								{
									return value;
								}
			};
			#define SSE_VECTOR	::SSE::Vector
			#define SSE_DEFINE1	::SSE::Vector
			#define SSE_DEFINE4	::SSE::Vector
		
		#endif	
	
		static	const	SSE_VECTOR	one = _mm_set1_ps(1.0f),
									two = _mm_set1_ps(2.0f),
									three = _mm_set1_ps(3.0f),
									four = _mm_set1_ps(4.0f),
									five = _mm_set1_ps(5.0f),
									six = _mm_set1_ps(6.0f),
									seven = _mm_set1_ps(7.0f),
									eight = _mm_set1_ps(8.0f),
									nine = _mm_set1_ps(9.0f),
									ten = _mm_set1_ps(10.0f),
									twenty = _mm_set1_ps(20.0f),
									thirty = _mm_set1_ps(30.0f),
									hundred = _mm_set1_ps(100.0f),
									c255 = _mm_set1_ps(255.0f),
									c127 = _mm_set1_ps(127.0f),
									c128 = _mm_set1_ps(128.0f),
									zero = _mm_setzero_ps(),
									minus_one = _mm_set1_ps(-1.0f),
									minus_two = _mm_set1_ps(-2.0f),
									minus_three = _mm_set1_ps(-3.0f),
									minus_four = _mm_set1_ps(-4.0f),
									half = _mm_set1_ps(0.5f),
									onethird = _mm_set1_ps(1.0f/3.0f),
									twothirds = _mm_set1_ps(2.0f/3.0f),
									point125 = _mm_set1_ps(0.125f),
									point25 = _mm_set1_ps(0.25f),
									point375 = _mm_set1_ps(0.375f),
									point75 = _mm_set1_ps(0.75f),
									point01 = _mm_set1_ps(0.01f),
									point02 = _mm_set1_ps(0.02f),
									point03 = _mm_set1_ps(0.03f),
									point04 = _mm_set1_ps(0.04f),
									point05 = _mm_set1_ps(0.05f),
									point1 = _mm_set1_ps(0.1f),
									point2 = _mm_set1_ps(0.2f),
									point3 = _mm_set1_ps(0.3f),
									point4 = _mm_set1_ps(0.4f),
									point5 = _mm_set1_ps(0.5f),
									point6 = _mm_set1_ps(0.6f),
									point7 = _mm_set1_ps(0.7f),
									point8 = _mm_set1_ps(0.8f),
									point9 = _mm_set1_ps(0.9f),
									onepoint1 = _mm_set1_ps(1.1f),
									onepoint2 = _mm_set1_ps(1.2f),
									onepoint3 = _mm_set1_ps(1.3f),
									onepoint4 = _mm_set1_ps(1.4f),
									onepoint5 = _mm_set1_ps(1.5f);
	}
	

	namespace SSE
	{
		inline String ToString(const __m128&val);
		
	
		inline SSE_VECTOR	load(volatile float v0, volatile float v1, volatile float v2, volatile float v3)
		{
			volatile ALIGNED16 float interchange[4] = {v0,v1,v2,v3};
			return _mm_load_ps((float*)interchange);
		}
		
		inline SSE_VECTOR	load(volatile float val)
		{
			volatile ALIGNED16 float interchange[4] = {val,val,val,val};
			return _mm_load_ps((float*)interchange);
		}
		
		inline __m128 clamp(const __m128&val, float lower_boundary, float upper_boundary)
		{
			__m128	lower = _mm_set1_ps(lower_boundary),
					upper = _mm_set1_ps(upper_boundary);
			return _mm_min_ps(_mm_max_ps(val,lower),upper);
		}
		
		inline __m128 clamp(const __m128&val, const __m128& lower, const __m128& upper)
		{
			return _mm_min_ps(_mm_max_ps(val,lower),upper);
		}
		
		
		/**
			@brief Calculates the linear step of @b val in the boundaries of @b lower_boundary and @b upper_boundary
			
			Component wise returns:<ul>
			<li>0 if val[i] &lt; lower_boundary</li>
			<li>1 if val[i] &gt; upper_boundary</li>
			<li>(val[i]-lower_boundary)/(upper_boundary/lower_boundary) otherwise</li>
			</ul>
		*/
		inline __m128 linearStep(const __m128&val, float lower_boundary, float upper_boundary)
		{
			__m128	lower = _mm_set1_ps(lower_boundary),
					upper = _mm_set1_ps(upper_boundary),
					factor = _mm_set1_ps(1.0f/(upper_boundary-lower_boundary));
			return	_mm_mul_ps(
						_mm_sub_ps(
							_mm_min_ps(
								_mm_max_ps(val,lower),
								upper
							),
							lower
						),
						factor
					);
		}
		
		inline __m128 linearStep(const __m128&val, const __m128&lower, const __m128&upper)
		{
			__m128	factor = _mm_rcp_ps(_mm_sub_ps(upper,lower));
			return	_mm_mul_ps(
						_mm_sub_ps(
							_mm_min_ps(
								_mm_max_ps(val,lower),
								upper
							),
							lower
						),
						factor
					);
		}
		
		inline void linearStep(const __m128&val, float lower_boundary, float upper_boundary, __m128&result)	//! @overload
		{
			__m128	lower = _mm_set1_ps(lower_boundary),
					upper = _mm_set1_ps(upper_boundary),
					factor = _mm_set1_ps(1.0f/(upper_boundary-lower_boundary));
			result=	_mm_mul_ps(
						_mm_sub_ps(
							_mm_min_ps(
								_mm_max_ps(val,lower),
								upper
							),
							lower
						),
						factor
					);
		}
		
		/**
			@brief Calculates cubic step for the specified value within the specified range
			
			The result effectively equals cubicFactor(linearStep(val,lower_boundary,upper_boundary))
			
			@param val Value to linearize and apply cubic factor to
			@param lower_boundary Lower applied range boundary. Must be less than upper_boundary
			@param upper_boundary Upper applied range boundary. Must be greater than lower_boundary
			
			@return Component wise cubicFactor(linearStep(val,lower_boundary,upper_boundary))
		*/
		inline __m128 cubicStep(const __m128&val, float lower_boundary, float upper_boundary)
		{
			__m128	lower = _mm_set1_ps(lower_boundary),
					upper = _mm_set1_ps(upper_boundary),
					factor = _mm_set1_ps(1.0f/(upper_boundary-lower_boundary)),
					f = _mm_mul_ps(_mm_sub_ps(_mm_min_ps(_mm_max_ps(val,lower),upper),lower),factor),
					sqr = _mm_mul_ps(f,f);
			
			return	_mm_add_ps(
						_mm_mul_ps(
							sqr,
							_mm_mul_ps(f,minus_two)
						),
						_mm_mul_ps(sqr,three)
					);
		}		
		
		inline __m128 cubicStep(const __m128&val, const __m128&lower, const __m128&upper)
		{
			__m128	factor = _mm_rcp_ps(_mm_sub_ps(upper,lower)),	
					f = _mm_mul_ps(_mm_sub_ps(_mm_min_ps(_mm_max_ps(val,lower),upper),lower),factor),
					sqr = _mm_mul_ps(f,f);
			
			return	_mm_add_ps(
						_mm_mul_ps(
							sqr,
							_mm_mul_ps(f,minus_two)
						),
						_mm_mul_ps(sqr,three)
					);
		}
		
		inline void cubicStep(const __m128&val, float lower_boundary, float upper_boundary, __m128&result)	//! @overload
		{
			__m128	lower = _mm_set1_ps(lower_boundary),
					upper = _mm_set1_ps(upper_boundary),
					factor = _mm_set1_ps(1.0f/(upper_boundary-lower_boundary)),
					f = _mm_mul_ps(_mm_sub_ps(_mm_min_ps(_mm_max_ps(val,lower),upper),lower),factor),
					sqr = _mm_mul_ps(f,f);
			
			result=	_mm_add_ps(
						_mm_mul_ps(
							sqr,
							_mm_mul_ps(f,minus_two)
						),
						_mm_mul_ps(sqr,three)
					);
		}
		
		/**
			@brief Converts a linear value in the range [0,1] to a smoothed cubic function value
			
			The curve described by this function smoothes the linear interval of [0,1]. The applied function f(x) is cubic with f(0)=0, f(1) = 1, f(0.5) = 0.5, f'(0) = f'(1) = 0.
			
			@param f Linear value to apply
			@return Cubic value
		*/
		inline __m128 cubicFactor(const __m128&f)
		{
			__m128 sqr = _mm_mul_ps(f,f);
			
			return	_mm_add_ps(
						_mm_mul_ps(
							sqr,
							_mm_mul_ps(f,minus_two)
						),
						_mm_mul_ps(sqr,three)
					);
		}
		
		inline __m128 abs(const __m128&x)
		{
			ALIGNED16	const UINT32 mask = 0x7FFFFFFF;
			return _mm_and_ps(_mm_load1_ps((const float*)&mask),x);
		}
		
		/**
			@brief Performs a comparison and converts the result to factor 0.0 or 1.0
		*/
		inline __m128 factorCompareGreater(const __m128&a, const __m128&b)
		{
			return _mm_and_ps(_mm_cmpgt_ps(a,b),_mm_set1_ps(1.0f));
		}
		
		
		inline __m128	cross(const __m128&v, const __m128&w)
		{
			__m128	fc0 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 3, 2, 0)),
					fc1 = _mm_shuffle_ps(w, w, _MM_SHUFFLE(2, 1, 3, 0));
			/*cout << "fc0="<<ToString(fc0)<<endl;
			cout << "fc1="<<ToString(fc1)<<endl;*/

			__m128 product0 = _mm_mul_ps(fc0, fc1);

			fc0 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 1, 3, 0));
			fc1 = _mm_shuffle_ps(w, w, _MM_SHUFFLE(1, 3, 2, 0));
			/*cout << "fc0="<<ToString(fc0)<<endl;
			cout << "fc1="<<ToString(fc1)<<endl;*/

			__m128 product1 = _mm_mul_ps(fc0, fc1);
			
			return _mm_sub_ps(product0, product1);
		}
		
		#ifndef __GNUC__
		
			inline float	floatDot(const __m128&v, const __m128&w)
			{
				__m128	dot = _mm_mul_ps(v,w);
				dot = _mm_hadd_ps(dot,dot);
				dot = _mm_hadd_ps(dot,dot);
				float rs;
				_mm_store_ss(&rs,dot);
				return rs;
			}
		#endif

		
		inline void addTriangleNormal(const __m128&p0, const __m128&p1, const __m128&p2, float normal[3])
		{
			ALIGNED16 float out[4];// = (float*) _aligned_malloc(4 * sizeof(float), 16);

			__m128	v = _mm_sub_ps(p1,p0),
					w = _mm_sub_ps(p2,p0);
			/*cout << "v="<<ToString(v)<<endl;
			cout << "w="<<ToString(w)<<endl;*/

			/*
				out[0] = v[1]*w[2] - v[2]*w[1];
				out[1] = v[2]*w[0] - v[0]*w[2];
				out[2] = v[0]*w[1] - v[1]*w[0];
			*/
			__m128	fc0 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 3, 2, 0)),
					fc1 = _mm_shuffle_ps(w, w, _MM_SHUFFLE(2, 1, 3, 0));
			/*cout << "fc0="<<ToString(fc0)<<endl;
			cout << "fc1="<<ToString(fc1)<<endl;*/

			__m128 product0 = _mm_mul_ps(fc0, fc1);

			fc0 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 1, 3, 0));
			fc1 = _mm_shuffle_ps(w, w, _MM_SHUFFLE(1, 3, 2, 0));
			/*cout << "fc0="<<ToString(fc0)<<endl;
			cout << "fc1="<<ToString(fc1)<<endl;*/

			__m128 product1 = _mm_mul_ps(fc0, fc1);
			
			v = _mm_sub_ps(product0, product1);
			

			_mm_store_ps(out, v);

			normal[0] += out[1];
			normal[1] += out[2];
			normal[2] += out[3];
		}
		
		/**
			@brief Calculates the triangle normal specified by the three points
			
			@param p0 Coordinate of corner vertex 0. xyz are located in 1-3. Component 0 (w) is ignored
			@param p1 Coordinate of corner vertex 1. xyz are located in 1-3. Component 0 (w) is ignored
			@param p2 Coordinate of corner vertex 2. xyz are located in 1-3. Component 0 (w) is ignored
			@param result [out] Normal out
		*/
		inline void triangleNormal(const __m128&p0, const __m128&p1, const __m128&p2, float result[3])
		{
			ALIGNED16 float out[4];// = (float*) _aligned_malloc(4 * sizeof(float), 16);

			__m128	v = _mm_sub_ps(p1,p0),
					w = _mm_sub_ps(p2,p0);
			/*cout << "v="<<ToString(v)<<endl;
			cout << "w="<<ToString(w)<<endl;*/

			/*
				out[0] = v[1]*w[2] - v[2]*w[1];
				out[1] = v[2]*w[0] - v[0]*w[2];
				out[2] = v[0]*w[1] - v[1]*w[0];
			*/
			__m128	fc0 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 3, 2, 0)),
					fc1 = _mm_shuffle_ps(w, w, _MM_SHUFFLE(2, 1, 3, 0));
			/*cout << "fc0="<<ToString(fc0)<<endl;
			cout << "fc1="<<ToString(fc1)<<endl;*/

			__m128 product0 = _mm_mul_ps(fc0, fc1);

			fc0 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 1, 3, 0));
			fc1 = _mm_shuffle_ps(w, w, _MM_SHUFFLE(1, 3, 2, 0));
			/*cout << "fc0="<<ToString(fc0)<<endl;
			cout << "fc1="<<ToString(fc1)<<endl;*/

			__m128 product1 = _mm_mul_ps(fc0, fc1);
			
			v = _mm_sub_ps(product0, product1);
			

			_mm_store_ps(out, v);

			result[0] = out[1];
			result[1] = out[2];
			result[2] = out[3];
		}
		
		/**
			@brief Calculates the triangle normal specified by the three points
			
			@param p0 Coordinate of corner vertex 0. xyz are located in 1-3. Component 0 (w) is ignored
			@param p1 Coordinate of corner vertex 1. xyz are located in 1-3. Component 0 (w) is ignored
			@param p2 Coordinate of corner vertex 2. xyz are located in 1-3. Component 0 (w) is ignored
			@param result [out] Normal out. The first component (w) is undefined
		*/
		inline void triangleNormal(const __m128&p0, const __m128&p1, const __m128&p2, __m128&normal)
		{
			ALIGNED16 __m128	v = _mm_sub_ps(p1,p0),
								w = _mm_sub_ps(p2,p0);
			
			normal = _mm_sub_ps(
						_mm_mul_ps(
							_mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 3, 2, 0)),
							_mm_shuffle_ps(w, w, _MM_SHUFFLE(2, 1, 3, 0))
						),
						_mm_mul_ps(
							_mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 1, 3, 0)),
							_mm_shuffle_ps(w, w, _MM_SHUFFLE(1, 3, 2, 0))
						)
					);
		}
		
		inline void center(const __m128&a, const __m128&b, __m128&result)
		{
			result = _mm_mul_ps(_mm_add_ps(a,b),half);
		}
		
		inline __m128 rawCenter(volatile const __m128&a, volatile const __m128&b)
		{
			volatile ALIGNED16 float va[4], vb[4];
			_mm_store_ps((float*)va,(const __m128&)a);
			_mm_store_ps((float*)vb,(const __m128&)b);
			return load((va[0]+vb[0])/2.0f,
						(va[1]+vb[1])/2.0f,
						(va[2]+vb[2])/2.0f,
						(va[3]+vb[3])/2.0f);
		}
		
		inline void center3(const __m128&a, const __m128&b, const __m128&c, __m128&result)
		{
			result = _mm_mul_ps(_mm_add_ps(_mm_add_ps(a,b),c),onethird);
		}
		
		inline __m128 center3(const __m128&a, const __m128&b, const __m128&c)
		{
			return _mm_mul_ps(_mm_add_ps(_mm_add_ps(a,b),c),onethird);
		}
		
		inline void mad(const __m128&b, const __m128&v, float factor_, __m128&result)
		{
			result = _mm_add_ps(b,_mm_mul_ps(v,_mm_load1_ps(&factor_)));	//should work because factor _should_ be aligned (16)
		}
		
		inline void mad(const __m128&b, const __m128&v, const __m128&factor, __m128&result)
		{
			result = _mm_add_ps(b,_mm_mul_ps(v,factor));
		}
		
		inline void normalizeBlock(	const float in0[3], const float in1[3], const float in2[3], const float in3[3],
									float out0[3], float out1[3], float out2[3], float out3[3])
		{
			__m128	x = _mm_setr_ps(in0[0],in1[0],in2[0],in3[0]),
					y = _mm_setr_ps(in0[1],in1[1],in2[1],in3[1]),
					z = _mm_setr_ps(in0[2],in1[2],in2[2],in3[2]),
					factor = _mm_rsqrt_ps(
								_mm_add_ps(
									_mm_add_ps(
										_mm_mul_ps(x,x),
										_mm_mul_ps(y,y)
									),
									_mm_mul_ps(z,z)
								)
							);
			
			ALIGNED16	float	exported[4];
			
			_mm_store_ps(exported,_mm_mul_ps(x,factor));
			out0[0] = exported[0];
			out1[0] = exported[1];
			out2[0] = exported[2];
			out3[0] = exported[3];
			
			_mm_store_ps(exported,_mm_mul_ps(y,factor));
			out0[1] = exported[0];
			out1[1] = exported[1];
			out2[1] = exported[2];
			out3[1] = exported[3];
			
			_mm_store_ps(exported,_mm_mul_ps(z,factor));
			out0[2] = exported[0];
			out1[2] = exported[1];
			out2[2] = exported[2];
			out3[2] = exported[3];
		}

		inline void normalizeBlock(	const float in0[3], const float in1[3], const float in2[3], const float in3[3],
									TVec3<>& out0, TVec3<>& out1, TVec3<>& out2, TVec3<>& out3)
		{
			__m128	x = _mm_setr_ps(in0[0],in1[0],in2[0],in3[0]),
					y = _mm_setr_ps(in0[1],in1[1],in2[1],in3[1]),
					z = _mm_setr_ps(in0[2],in1[2],in2[2],in3[2]),
					factor = _mm_rsqrt_ps(
								_mm_add_ps(
									_mm_add_ps(
										_mm_mul_ps(x,x),
										_mm_mul_ps(y,y)
									),
									_mm_mul_ps(z,z)
								)
							);
			
			ALIGNED16	float	exported[4];
			
			_mm_store_ps(exported,_mm_mul_ps(x,factor));
			out0.x = exported[0];
			out1.x = exported[1];
			out2.x = exported[2];
			out3.x = exported[3];
			
			_mm_store_ps(exported,_mm_mul_ps(y,factor));
			out0.y = exported[0];
			out1.y = exported[1];
			out2.y = exported[2];
			out3.y = exported[3];
			
			_mm_store_ps(exported,_mm_mul_ps(z,factor));
			out0.z = exported[0];
			out1.z = exported[1];
			out2.z = exported[2];
			out3.z = exported[3];
		}


		inline void normalizeBlock(	const TVec3<>&in0, const TVec3<>&in1, const TVec3<>&in2, const TVec3<>&in3,
									TVec3<>& out0, TVec3<>& out1, TVec3<>& out2, TVec3<>& out3)
		{
			__m128	x = _mm_setr_ps(in0.x,in1.x,in2.x,in3.x),
					y = _mm_setr_ps(in0.y,in1.y,in2.y,in3.y),
					z = _mm_setr_ps(in0.z,in1.z,in2.z,in3.z),
					factor = _mm_rsqrt_ps(
								_mm_add_ps(
									_mm_add_ps(
										_mm_mul_ps(x,x),
										_mm_mul_ps(y,y)
									),
									_mm_mul_ps(z,z)
								)
							);
			
			ALIGNED16	float	exported[4];
			
			_mm_store_ps(exported,_mm_mul_ps(x,factor));
			out0.x = exported[0];
			out1.x = exported[1];
			out2.x = exported[2];
			out3.x = exported[3];
			
			_mm_store_ps(exported,_mm_mul_ps(y,factor));
			out0.y = exported[0];
			out1.y = exported[1];
			out2.y = exported[2];
			out3.y = exported[3];
			
			_mm_store_ps(exported,_mm_mul_ps(z,factor));
			out0.z = exported[0];
			out1.z = exported[1];
			out2.z = exported[2];
			out3.z = exported[3];
		}
		
		
		/**
			@brief Normalizes a vector of sse vectors.
			
			The input is interpreted as a matrix of 4 3d vectors with their x coordinates stored in v[0], y in v[1], and z in v[2]
		*/
		inline void normalize(SSE_VECTOR v[3])
		{
			SSE_VECTOR factor = 
				_mm_rsqrt_ps( 
					_mm_add_ps(
						_mm_add_ps(
							_mm_mul_ps(v[0],v[0]),
							_mm_mul_ps(v[1],v[1])
						),
						_mm_mul_ps(v[2],v[2])
					)
				);
			v[0] = _mm_mul_ps(v[0],factor);
			v[1] = _mm_mul_ps(v[1],factor);
			v[2] = _mm_mul_ps(v[2],factor);
		}
		
		/**
			@brief Calculates the four center points of the specified four NURBS curves
		*/
		inline void nurbsCenter(const SSE_VECTOR c0[3], const SSE_VECTOR c1[3], const SSE_VECTOR c2[3], const SSE_VECTOR c3[3], SSE_VECTOR out[3])
		{
			/*out[0] = c0[0]*SSE::point125 + c1[0]*SSE::point375 + c2[0]*SSE::point375 + c3[0]*SSE::point125;
			out[1] = c0[1]*SSE::point125 + c1[1]*SSE::point375 + c2[1]*SSE::point375 + c3[1]*SSE::point125;
			out[2] = c0[2]*SSE::point125 + c1[2]*SSE::point375 + c2[2]*SSE::point375 + c3[2]*SSE::point125;*/
			
			out[0] = _mm_add_ps(
						_mm_add_ps(
							_mm_mul_ps(c0[0],point125),
							_mm_mul_ps(c1[0],point375)
						),
						_mm_add_ps(
							_mm_mul_ps(c2[0],point375),
							_mm_mul_ps(c3[0],point125)
						)
					);
			out[1] = _mm_add_ps(
						_mm_add_ps(
							_mm_mul_ps(c0[1],point125),
							_mm_mul_ps(c1[1],point375)
						),
						_mm_add_ps(
							_mm_mul_ps(c2[1],point375),
							_mm_mul_ps(c3[1],point125)
						)
					);
			out[2] = _mm_add_ps(
						_mm_add_ps(
							_mm_mul_ps(c0[2],point125),
							_mm_mul_ps(c1[2],point375)
						),
						_mm_add_ps(
							_mm_mul_ps(c2[2],point375),
							_mm_mul_ps(c3[2],point125)
						)
					);
		}
		
		#ifndef __GNUC__
			inline void normalize(const __m128&v, __m128&result)
			{
				ALIGNED16 __m128	intermediate = _mm_mul_ps(v,v);
				intermediate = _mm_hadd_ps(intermediate,intermediate),
				intermediate = _mm_rsqrt_ps(_mm_hadd_ps(intermediate,intermediate));	//The result of the operation on operand a (A3, A2, A1, A0) and operand b (B3, B2, B1, B0) is (B3 + B2, B1 + B0, A3 + A2, A1 + A0). => therefore twice
					//normalize
				result = _mm_mul_ps(v,intermediate);
			}

			inline void normalize(__m128 & v)
			{
				ALIGNED16 __m128	intermediate = _mm_mul_ps(v,v);
				intermediate = _mm_hadd_ps(intermediate,intermediate),
				intermediate = _mm_rsqrt_ps(_mm_hadd_ps(intermediate,intermediate));	//The result of the operation on operand a (A3, A2, A1, A0) and operand b (B3, B2, B1, B0) is (B3 + B2, B1 + B0, A3 + A2, A1 + A0). => therefore twice
					//normalize
				v = _mm_mul_ps(v,intermediate);
			}

			inline __m128 normalized(const __m128 & v)
			{
				ALIGNED16 __m128	intermediate = _mm_mul_ps(v,v);
				intermediate = _mm_hadd_ps(intermediate,intermediate),
				intermediate = _mm_rsqrt_ps(_mm_hadd_ps(intermediate,intermediate));	//The result of the operation on operand a (A3, A2, A1, A0) and operand b (B3, B2, B1, B0) is (B3 + B2, B1 + B0, A3 + A2, A1 + A0). => therefore twice
					//normalize
				return _mm_mul_ps(v,intermediate);
			}
			
			/**
				@brief Returns the absolute distance between the two specified SSE 4xFloat vectors
			*/
			inline float	floatDistance(const __m128&a, const __m128&b)
			{
				__m128 distance = _mm_sub_ps(b,a);
				distance = _mm_mul_ps(distance,distance);
				distance = _mm_hadd_ps(distance,distance);
				distance = _mm_hadd_ps(distance,distance);
				distance = _mm_sqrt_ps(distance);
				float rs;
				_mm_store_ss(&rs,distance);
				return rs;
			}
		#endif
		
		inline __m128	vector3(const float v[3])
		{
			return _mm_setr_ps(0,v[0],v[1],v[2]);
		}
		
		inline void		exportVector3(const __m128&sse_vector, TVec3<>&out)
		{
			ALIGNED16	float exported[4];
			_mm_store_ps(exported,sse_vector);
			out.x = exported[1];
			out.y = exported[2];
			out.z = exported[3];
		}

		
		/**
			@brief Converts the current value of the specified SSE float vector to string
			
			The resulting string will be of the form f(w, x, y, z)
			
			@return Converted strin
		*/
		inline String ToString(const __m128&val)
		{
			ALIGNED16 TVec4<> out;// = (float*) _aligned_malloc(4 * sizeof(float), 16);
			_mm_store_ps(out.v, val);
			return Vec::toString(out);
		}
		
		inline String toString8(const __m128i&val)
		{
			ALIGNED16 BYTE out[16];
			
			_mm_store_si128((__m128i*)out,val);
			return Vec::toStringD(out,16);
		}
		
		inline String toString32(const __m128i&val)
		{
			ALIGNED16 INT32 out[4];
			
			_mm_store_si128((__m128i*)out,val);
			return Vec::toStringD(out,4);
		}		
		
		inline String toString32u(const __m128i&val)
		{
			ALIGNED16 UINT32 out[4];
			
			_mm_store_si128((__m128i*)out,val);
			return Vec::toStringD(out,4);
		}
		
		inline String toString32hex(const __m128i&val)
		{
			ALIGNED16 UINT32 out[4];
			
			_mm_store_si128((__m128i*)out,val);
			return IntToHex((int)out[0],8)+", "+IntToHex((int)out[1],8)+", "+IntToHex((int)out[2],8)+", "+IntToHex((int)out[3],8);
		}
		
		
		enum version_t
		{
			Unsupported = 0,
			V1 = 10,
			V2 = 20,
			V3 = 30,
			V3Supplemental = 35,
			V4 = 40
		};
		
		inline const char*		versionToString(version_t version)
		{
			#undef CASE
			#define CASE(_CASE_)	case _CASE_: return #_CASE_;
			switch (version)
			{
				CASE(Unsupported)
				case V1:
					return "SSE1";
				case V2:
					return "SSE2";
				case V3:
					return "SSE3";
				case V3Supplemental:
					return "Supplemental SSE3";
				case V4:
					return "SSE4";
			}
			return "Unknown or unsupported SSE version";
		}







	}
	
	
	inline SSE_VECTOR operator+(float value, const SSE_VECTOR&v)
	{
		return _mm_add_ps(_mm_set1_ps(value),v);
	}
	inline SSE_VECTOR operator*(float value, const SSE_VECTOR&v)
	{
		return _mm_mul_ps(_mm_set1_ps(value),v);
	}
	inline SSE_VECTOR operator-(float value, const SSE_VECTOR&v)
	{
		return _mm_sub_ps(_mm_set1_ps(value),v);
	}
	inline SSE_VECTOR operator/(float value, const SSE_VECTOR&v)
	{
		return _mm_div_ps(_mm_set1_ps(value),v);
	}

	inline SSE_VECTOR operator+(const SSE_VECTOR&v, float value)
	{
		return _mm_add_ps(v,_mm_set1_ps(value));
	}
	inline SSE_VECTOR operator*(const SSE_VECTOR&v, float value)
	{
		return _mm_mul_ps(v,_mm_set1_ps(value));
	}
	inline SSE_VECTOR operator-(const SSE_VECTOR&v, float value)
	{
		return _mm_sub_ps(v,_mm_set1_ps(value));
	}
	inline SSE_VECTOR operator/(const SSE_VECTOR&v, float value)
	{
		return _mm_div_ps(v,_mm_set1_ps(value));
	}
	
	inline void operator+=(SSE_VECTOR&v0, const SSE_VECTOR&v1)
	{
		v0 = _mm_add_ps(v0,v1);
	}
	inline void operator*=(SSE_VECTOR&v0, const SSE_VECTOR&v1)
	{
		v0 = _mm_mul_ps(v0,v1);
	}
	inline void operator-=(SSE_VECTOR&v0, const SSE_VECTOR&v1)
	{
		v0 = _mm_sub_ps(v0,v1);
	}
	inline void operator/=(SSE_VECTOR&v0, const SSE_VECTOR&v1)
	{
		v0 = _mm_div_ps(v0,v1);
	}
	
	inline SSE_VECTOR operator+(const SSE_VECTOR&v0, const SSE_VECTOR&v1)
	{
		return _mm_add_ps(v0,v1);
	}
	inline SSE_VECTOR operator*(const SSE_VECTOR&v0, const SSE_VECTOR&v1)
	{
		return _mm_mul_ps(v0,v1);
	}
	inline SSE_VECTOR operator-(const SSE_VECTOR&v0, const SSE_VECTOR&v1)
	{
		return _mm_sub_ps(v0,v1);
	}
	inline SSE_VECTOR operator-(const SSE_VECTOR&v0)
	{
		return _mm_mul_ps(v0,SSE::minus_one);
	}
	inline SSE_VECTOR operator/(const SSE_VECTOR&v0, const SSE_VECTOR&v1)
	{
		return _mm_div_ps(v0,v1);
	}
	
	namespace SSE
	{
		inline void resolveSplineQuad(const SplineQuad<SSE_VECTOR>&quad, const SSE_VECTOR&x, const SSE_VECTOR&y, SSE_VECTOR out[3])
		{
			SSE_VECTOR	ix = SSE::one-x,
						x3 = x*x*x,
						x2 = x*x*ix*SSE::three,
						x1 = x*ix*ix*SSE::three,
						x0 = ix*ix*ix,
						iy = SSE::one-y,
						y3 = y*y*y,
						y2 = y*y*iy*SSE::three,
						y1 = y*iy*iy*SSE::three,
						y0 = iy*iy*iy;
			
			out[0] =	quad.control[0][0][0]*x0*y0 + quad.control[1][0][0]*x1*y0 + quad.control[2][0][0]*x2*y0 + quad.control[3][0][0]*x3*y0
						+
						quad.control[0][1][0]*x0*y1 + quad.control[1][1][0]*x1*y1 + quad.control[2][1][0]*x2*y1 + quad.control[3][1][0]*x3*y1
						+
						quad.control[0][2][0]*x0*y2 + quad.control[1][2][0]*x1*y2 + quad.control[2][2][0]*x2*y2 + quad.control[3][2][0]*x3*y2
						+
						quad.control[0][3][0]*x0*y3 + quad.control[1][3][0]*x1*y3 + quad.control[2][3][0]*x2*y3 + quad.control[3][3][0]*x3*y3;
			
			out[1] =	quad.control[0][0][1]*x0*y0 + quad.control[1][0][1]*x1*y0 + quad.control[2][0][1]*x2*y0 + quad.control[3][0][1]*x3*y0
						+
						quad.control[0][1][1]*x0*y1 + quad.control[1][1][1]*x1*y1 + quad.control[2][1][1]*x2*y1 + quad.control[3][1][1]*x3*y1
						+
						quad.control[0][2][1]*x0*y2 + quad.control[1][2][1]*x1*y2 + quad.control[2][2][1]*x2*y2 + quad.control[3][2][1]*x3*y2
						+
						quad.control[0][3][1]*x0*y3 + quad.control[1][3][1]*x1*y3 + quad.control[2][3][1]*x2*y3 + quad.control[3][3][1]*x3*y3;
			
			out[2] =	quad.control[0][0][2]*x0*y0 + quad.control[1][0][2]*x1*y0 + quad.control[2][0][2]*x2*y0 + quad.control[3][0][2]*x3*y0
						+
						quad.control[0][1][2]*x0*y1 + quad.control[1][1][2]*x1*y1 + quad.control[2][1][2]*x2*y1 + quad.control[3][1][2]*x3*y1
						+
						quad.control[0][2][2]*x0*y2 + quad.control[1][2][2]*x1*y2 + quad.control[2][2][2]*x2*y2 + quad.control[3][2][2]*x3*y2
						+
						quad.control[0][3][2]*x0*y3 + quad.control[1][3][2]*x1*y3 + quad.control[2][3][2]*x2*y3 + quad.control[3][3][2]*x3*y3;
		}

		inline void resolveSplineQuadCenter(const SplineQuad<SSE_VECTOR>&quad, SSE_VECTOR out[3])
		{
			SSE_VECTOR	x3 = SSE::point125,
						x2 = SSE::point375,
						x1 = SSE::point375,
						x0 = SSE::point125,
						y3 = SSE::point125,
						y2 = SSE::point375,
						y1 = SSE::point375,
						y0 = SSE::point125;
			
			out[0] =	quad.control[0][0][0]*x0*y0 + quad.control[1][0][0]*x1*y0 + quad.control[2][0][0]*x2*y0 + quad.control[3][0][0]*x3*y0
						+
						quad.control[0][1][0]*x0*y1 + quad.control[1][1][0]*x1*y1 + quad.control[2][1][0]*x2*y1 + quad.control[3][1][0]*x3*y1
						+
						quad.control[0][2][0]*x0*y2 + quad.control[1][2][0]*x1*y2 + quad.control[2][2][0]*x2*y2 + quad.control[3][2][0]*x3*y2
						+
						quad.control[0][3][0]*x0*y3 + quad.control[1][3][0]*x1*y3 + quad.control[2][3][0]*x2*y3 + quad.control[3][3][0]*x3*y3;
			
			out[1] =	quad.control[0][0][1]*x0*y0 + quad.control[1][0][1]*x1*y0 + quad.control[2][0][1]*x2*y0 + quad.control[3][0][1]*x3*y0
						+
						quad.control[0][1][1]*x0*y1 + quad.control[1][1][1]*x1*y1 + quad.control[2][1][1]*x2*y1 + quad.control[3][1][1]*x3*y1
						+
						quad.control[0][2][1]*x0*y2 + quad.control[1][2][1]*x1*y2 + quad.control[2][2][1]*x2*y2 + quad.control[3][2][1]*x3*y2
						+
						quad.control[0][3][1]*x0*y3 + quad.control[1][3][1]*x1*y3 + quad.control[2][3][1]*x2*y3 + quad.control[3][3][1]*x3*y3;
			
			out[2] =	quad.control[0][0][2]*x0*y0 + quad.control[1][0][2]*x1*y0 + quad.control[2][0][2]*x2*y0 + quad.control[3][0][2]*x3*y0
						+
						quad.control[0][1][2]*x0*y1 + quad.control[1][1][2]*x1*y1 + quad.control[2][1][2]*x2*y1 + quad.control[3][1][2]*x3*y1
						+
						quad.control[0][2][2]*x0*y2 + quad.control[1][2][2]*x1*y2 + quad.control[2][2][2]*x2*y2 + quad.control[3][2][2]*x3*y2
						+
						quad.control[0][3][2]*x0*y3 + quad.control[1][3][2]*x1*y3 + quad.control[2][3][2]*x2*y3 + quad.control[3][3][2]*x3*y3;
		}


	}


	template <>
	class TypeInfo < SSE_VECTOR >
	{
	public:
		typedef SSE_VECTOR	Type;
		typedef Type		UnionCompatibleBase;
		typedef TypeInfo<Type>	UnsignedType;
		typedef TypeInfo<Type>	SignedType;
		typedef TypeInfo<Type>	GreaterType;
		typedef TypeInfo<Type>	LesserType;
		typedef TTrue		IsPrimitive;
		static const bool	is_signed = true,
			is_float = true;
		//static const Type	zero;
		//static const Type	max;
		//static const Type	min;
		//static const Type	undefined;
		//static const Type	error;
		//static const char*	name;
	};

	namespace Vec
	{
		template <>
			inline	void	__fastcall	normalize(TVec3<SSE_VECTOR>& vector)throw()
			{
				SSE::normalize(vector.v);
			}

		template <>
			inline SSE_VECTOR __fastcall length(const TVec3<SSE_VECTOR>& v)throw()
			{
				return
					_mm_sqrt_ps( 
						_mm_add_ps(
							_mm_add_ps(
								_mm_mul_ps(v.x,v.x),
								_mm_mul_ps(v.y,v.y)
							),
							_mm_mul_ps(v.z,v.z)
						)
					);


			}
	}




	namespace Math
	{

	}
}

#endif






#endif
