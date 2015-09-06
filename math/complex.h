#ifndef complex_mathH
#define complex_mathH

/******************************************************************

Collection of template functions regarding complex math.

******************************************************************/

#include "matrix.h"
#include "../general/undef.h"

namespace ComplexMath
{

	template <class> class Complex;
	template <class> class Quaternion;

	//quaternion-functions:


	MFUNC5	(void)	_q(TVec4<C0>&out,const C1&a,const C2&x,const C3&y,const C4&z);				//creates a quaternion using a as angle (-360..360), (x, y, z) as axis (normalizes)
	MFUNC3	(void)	_qRotateSystemCW(const TVec4<C0>&q, const TMatrix4<C1>&matrix, TMatrix4<C2>& m_out);	//rotates a given 4x4 sys-matrix in clockwise direction using a quaternion (may not be normalized)
	MFUNC2	(void)	_qRotateSystemCW(const TVec4<C0>&q, TMatrix4<C1>&m_inout);					 //same for inout matrix
	MFUNC3	(void)	_qRotateSystemCCW(const TVec4<C0>&q, const TMatrix4<C1>&matrix, TMatrix4<C2>& m_out); //same for counter-clockwise direction
	MFUNC2	(void)	_qRotateSystemCCW(const TVec4<C0>&q, TMatrix4<C1>& m_inout);					//same for io-matrix
	MFUNC3	(void)	_qRotateMatrix3x3CW(const TVec4<C0>&q, const TMatrix3<C1>&matrix, TMatrix3<C2>&m_out);	//same for 3x3 matrices
	MFUNC2	(void)	_qRotateMatrix3x3CW(const TVec4<C0>&q, TMatrix3<C1>&m_inout);
	MFUNC3	(void)	_qRotateMatrix3x3CCW(const TVec4<C0>&q, const TMatrix3<C1>&matrix, TMatrix3<C2>&m_out);
	MFUNC2	(void)	_qRotateMatrix3x3CCW(const TVec4<C0>&q, TMatrix3<C1>& m_inout);
	MFUNC3	(void)	_qRotateCW(const TVec4<C0>&q, const TVec3<C1>&p, TVec3<C2>&out);					//rotates p using q clockwise (|q| must be 1)
	MFUNC2	(void)	_qRotateCW(const TVec4<C0>&q, TVec3<C1>&p);									 //same for io-p
	MFUNC3	(void)	_qRotateCCW(const TVec4<C0>&q, const TVec3<C1>&p, TVec3<C2>&out);					//same for counter-clockwise
	MFUNC2	(void)	_qRotateCCW(const TVec4<C0>&q, TVec3<C1>&p);									//same for io-p
	MFUNC3	(void)	_qMultiply(const TVec4<C0>&q0, const TVec4<C1>&q1, TVec4<C2>&out);					//q-product of q0 and q1: out = q0 * q1
	MFUNC4	(void)	_qMultiply(const TVec4<C0>&q0, const TVec4<C1>&q1, const TVec4<C2>&q2, TVec4<C3>&out);	//q-product of q0, q1 and q2: out = q0 * q1 * q2
	MFUNC3	(void)	_qDivide(const TVec4<C0>&q0, const TVec4<C1>&q1, TVec4<C2>&out);					//q-quotient of p0 and p1: out = q0 / q1
	MFUNC3	(void)	_qAdd(const TVec4<C0>&q0, const TVec4<C1>&q1, TVec4<C2>&out);						//out = q0 + q1
	MFUNC2	(void)	_qAdd(TVec4<C0>&inout, const TVec4<C1>&q1);									 //inout += q1
	MFUNC3	(void)	_qSubtract(const TVec4<C0>&q0, const TVec4<C1>&q1, TVec4<C2>&out);					//out = q0 - q1
	MFUNC2	(void)	_qSubtract(TVec4<C0>&inout, const TVec4<C1>&q1);								//inout -= q1
	MFUNC2	(void)	_qConjugate(const TVec4<C0>&q, TVec4<C1>&out);									//conjugates q
	MFUNC	(void)	_qConjugate(TVec4<C>&inout);												//conjugates inout
	MFUNC2	(void)	_qInvert(const TVec4<C0>&q, TVec4<C1>&out);									 //inverts q
	MFUNC2	(void)	_q2AxisRotation(const TVec4<C0>&q, TVec4<C1>&out);								//extrapolates axis-rotation from quaternion
	MFUNC2	(void)	_q2Matrix(const TVec4<C0>&q, TMatrix4<C1>&out);									//creates matrix equivalent to the given quaternion
	MFUNC2	(void)	_q2RotMatrixCCW(const TVec4<C0>&q, TMatrix3<C1>&out);								//creates counter-clockwise rotation-matrix from quaternion
	MFUNC2	(void)	_q2RotMatrixCW(const TVec4<C0>&q, TMatrix3<C1>&out);								//creates clockwise rotation-matrix form quaternion
	MFUNC3	(void)	_q2RotMatrixCCW(const C0& angle, const TVec3<C1>&axis, TMatrix3<C2>&out);						//creates matrices from axes and angles rather than qs:
	MFUNC5	(void)	_q2RotMatrixCCW(const C0& angle, const C1&x, const C2&y, const C3&z, TMatrix3<C4>&out);
	MFUNC3	(void)	_q2RotMatrixCW(const C0& angle, const TVec3<C1>&axis, TMatrix3<C2>&out);
	MFUNC5	(void)	_q2RotMatrixCW(const C0& angle, const C1&x, const C2&y, const C3&z, TMatrix3<C4>&out);
	MFUNC2	(void)	_q2Quaternion(const TVec4<C0>&axis_rotation, TVec4<C1>&out);								//creates quaternion from axis. axis is expected: (x,y,z,a)
	MFUNC3	(void)	_q2Quaternion(const C0&angle, const TVec3<C1>&axis, TVec4<C2>&out);						 //creates quaternion from angle and axis
	MFUNC5	(void)	_q2Quaternion(const C0&angle, const C1&x, const C2&y, const C3&z, TVec4<C4>&out);		//creates quaternion from angle and axis-coords (axis is normalized)
	MFUNC	(C)		_qLength(const TVec4<C>&q);															 //returns quaternion-norm



	template <class C> class Complex
	{
	public:
			C					re,im;

	MF_CONSTRUCTOR				Complex();
	MF_CONSTRUCTOR1				Complex(const C0&r);
	MF_CONSTRUCTOR2				Complex(const C0&r, const C1&i);

	MFUNC1	(void)				operator+=(const Complex<C0>&other);
	MFUNC1	(void)				operator-=(const Complex<C0>&other);
	MFUNC1	(void)				operator*=(const Complex<C0>&other);
	MFUNC1	(void)				operator/=(const Complex<C0>&other);
	MFUNC1	(Complex<C>)		operator+(const Complex<C0>&other)	const;
	MFUNC1	(Complex<C>)		operator-(const Complex<C0>&other)	const;
	MFUNC1	(Complex<C>)		operator*(const Complex<C0>&other)	const;
	MFUNC1	(Complex<C>)		operator/(const Complex<C0>&other)	const;
	MFUNC1	(bool)				operator==(const Complex<C0>&other);
	MFUNC1	(bool)				operator!=(const Complex<C0>&other);

	MF_DECLARE(C)				length();
	MF_DECLARE(Complex<C>)		conjugate();
	MF_DECLARE(String)			ToString();
	};

	template <class C> class Quaternion
	{
	public:

	union
	{
		struct
		{
			C					re,i,j,k;
		};
		C						q[4];
	};

	MF_CONSTRUCTOR				Quaternion();
	MF_CONSTRUCTOR1				Quaternion(const C0&r_);
	MF_CONSTRUCTOR4				Quaternion(const C0&r_, const C1&i_, const C2&j_, const C3&k_);

	MFUNC1	(void)				operator+=(const Quaternion<C0>&other);
	MFUNC1	(void)				operator-=(const Quaternion<C0>&other);
	MFUNC1	(void)				operator*=(const Quaternion<C0>&other);
	MFUNC1	(void)				operator/=(const Quaternion<C0>&other);
	MFUNC1	(Quaternion<C>)	operator+(const Quaternion<C0>&other)		const;
	MFUNC1	(Quaternion<C>)	operator-(const Quaternion<C0>&other)		const;
	MFUNC1	(Quaternion<C>)	operator*(const Quaternion<C0>&other)		const;
	MFUNC1	(Quaternion<C>)	operator/(const Quaternion<C0>&other)		const;
	MFUNC1	(bool)				operator==(const Quaternion<C0>&other);
	MFUNC1	(bool)				operator!=(const Quaternion<C0>&other);

	MF_DECLARE(C)				length();
	MF_DECLARE(Quaternion<C>)	invert();
	MF_DECLARE(Quaternion<C>)	conjugate();
	MFUNC2	(void)				buildFrom(const C0&angle, const C1 axis[3]);
	MFUNC4	(void)				buildFrom(const C0&angle, const C1&ax, const C2&ay, const C3&az);
	MFUNC1	(void)				buildRotMatrixCW(C0 matrix[3*3]);
	MFUNC1	(void)				buildRotMatrixCCW(C0 matrix[3*3]);
	MFUNC2	(void)				rotateCW(const C0 vec[3], C1 out[3]);
	MFUNC2	(void)				rotateCCW(const C0 vec[3], C1 out[3]);
	MFUNC1	(void)				rotateCW(C0 vec[3]);
	MFUNC1	(void)				rotateCCW(C0 vec[3]);
	};

}

using namespace ComplexMath;

#include "complex.tpl.h"

#endif
