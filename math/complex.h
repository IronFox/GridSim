#ifndef complex_mathH
#define complex_mathH

/******************************************************************

Collection of template functions regarding complex math.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

#include "matrix.h"

namespace ComplexMath
{

	template <class> class Complex;
	template <class> class Quaternion;

	//quaternion-functions:


	MFUNC5	(void)	_q(C0 out[4],const C1&a,const C2&x,const C3&y,const C4&z);				//creates a quaternion using a as angle (-360..360), (x, y, z) as axis (normalizes)
	MFUNC3	(void)	_qRotateSystemCW(const C0 q[4], const C1 matrix[4*4], C2 m_out[4*4]);	//rotates a given 4x4 sys-matrix in clockwise direction using a quaternion (may not be normalized)
	MFUNC2	(void)	_qRotateSystemCW(const C0 q[4], C1 m_inout[4*4]);					 //same for inout matrix
	MFUNC3	(void)	_qRotateSystemCCW(const C0 q[4], const C1 matrix[4*4], C2 m_out[4*4]); //same for counter-clockwise direction
	MFUNC2	(void)	_qRotateSystemCCW(const C0 q[4], C1 m_inout[4*4]);					//same for io-matrix
	MFUNC3	(void)	_qRotateMatrix3x3CW(const C0 q[4], const C1 matrix[3*3], C2 m_out[3*3]);	//same for 3x3 matrices
	MFUNC2	(void)	_qRotateMatrix3x3CW(const C0 q[4], C1 m_inout[3*3]);
	MFUNC3	(void)	_qRotateMatrix3x3CCW(const C0 q[4], const C1 matrix[3*3], C2 m_out[3*3]);
	MFUNC2	(void)	_qRotateMatrix3x3CCW(const C0 q[4], C1 m_inout[3*3]);
	MFUNC3	(void)	_qRotateCW(const C0 q[4], const C1 p[3], C2 out[3]);					//rotates p using q clockwise (|q| must be 1)
	MFUNC2	(void)	_qRotateCW(const C0 q[4], C1 p[3]);									 //same for io-p
	MFUNC3	(void)	_qRotateCCW(const C0 q[4], const C1 p[3], C2 out[3]);					//same for counter-clockwise
	MFUNC2	(void)	_qRotateCCW(const C0 q[4], C1 p[3]);									//same for io-p
	MFUNC3	(void)	_qMultiply(const C0 q0[4], const C1 q1[4], C2 out[4]);					//q-product of q0 and q1: out = q0 * q1
	MFUNC4	(void)	_qMultiply(const C0 q0[4], const C1 q1[4], const C2 q2[4], C3 out[4]);	//q-product of q0, q1 and q2: out = q0 * q1 * q2
	MFUNC3	(void)	_qDivide(const C0 q0[4], const C1 q1[4], C2 out[4]);					//q-quotient of p0 and p1: out = q0 / q1
	MFUNC3	(void)	_qAdd(const C0 q0[4], const C1 q1[4], C2 out[4]);						//out = q0 + q1
	MFUNC2	(void)	_qAdd(C0 inout[4], const C1 q1[4]);									 //inout += q1
	MFUNC3	(void)	_qSubtract(const C0 q0[4], const C1 q1[4], C2 out[4]);					//out = q0 - q1
	MFUNC2	(void)	_qSubtract(C0 inout[4], const C1 q1[4]);								//inout -= q1
	MFUNC2	(void)	_qConjugate(const C0 q[4], C1 out[4]);									//conjugates q
	MFUNC	(void)	_qConjugate(C inout[4]);												//conjugates inout
	MFUNC2	(void)	_qInvert(const C0 q[4], C1 out[4]);									 //inverts q
	MFUNC2	(void)	_q2AxisRotation(const C0 q[4], C1 out[4]);								//extrapolates axis-rotation from quaternion
	MFUNC2	(void)	_q2Matrix(const C0 q[4], C1 out[16]);									//creates matrix equivalent to the given quaternion
	MFUNC2	(void)	_q2RotMatrixCCW(const C0 q[4], C1 out[9]);								//creates counter-clockwise rotation-matrix from quaternion
	MFUNC2	(void)	_q2RotMatrixCW(const C0 q[4], C1 out[9]);								//creates clockwise rotation-matrix form quaternion
	MFUNC3	(void)	_q2RotMatrixCCW(const C0& angle, const C1 axis[3], C2 out[3*3]);						//creates matrices from axes and angles rather than qs:
	MFUNC5	(void)	_q2RotMatrixCCW(const C0& angle, const C1&x, const C2&y, const C3&z, C4 out[3*3]);
	MFUNC3	(void)	_q2RotMatrixCW(const C0& angle, const C1 axis[3], C2 out[3*3]);
	MFUNC5	(void)	_q2RotMatrixCW(const C0& angle, const C1&x, const C2&y, const C3&z, C4 out[3*3]);
	MFUNC2	(void)	_q2Quaternion(const C0 axis_rotation[4], C1 out[4]);								//creates quaternion from axis. axis is expected: (x,y,z,a)
	MFUNC3	(void)	_q2Quaternion(const C0&angle, const C1 axis[3], C2 out[4]);						 //creates quaternion from angle and axis
	MFUNC5	(void)	_q2Quaternion(const C0&angle, const C1&x, const C2&y, const C3&z, C4 out[4]);		//creates quaternion from angle and axis-coords (axis is normalized)
	MFUNC	(C)		_qLength(const C q[4]);															 //returns quaternion-norm



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
