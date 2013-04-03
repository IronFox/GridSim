#ifndef complex_mathTplH
#define complex_mathTplH

/******************************************************************

Collection of template functions regarding complex math.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


namespace ComplexMath
{

	MFUNC5 (void) _q(C0 out[4],const C1&a,const C2&x,const C3&y,const C4&z)
	{
		C0 s = vsin((C0)a*M_PI/360)/vsqrt((C0)x*x+(C0)y*y+(C0)z*z);
		out[0] = vcos((C0)a*M_PI/360);
		out[1] = s*x;
		out[2] = s*y;
		out[3] = s*z;
	}

	MFUNC3 (void)	_qRotateSystemCW(const C0 q[4], const C1 matrix[16], C2 m_out[16])
	{
		C1 m[9];
		_q2RotMatrixCW(q,m);
		__multiply331(m,matrix,m_out);
		__multiply331(m,&matrix[4],&m_out[4]);
		__multiply331(m,&matrix[8],&m_out[8]);
		__multiply331(m,&matrix[12],&m_out[12]);
		m_out[3] = m_out[7] = m_out[11] = 0;
		m_out[15] = 1;
	}

	MFUNC2 (void)	_qRotateSystemCW(const C0 q[4], C1 matrix[16])
	{
		C1 m[9],t[12];
		_q2RotMatrixCW(q,m);
		__multiply331(m,matrix,t);
		__multiply331(m,&matrix[4],&t[3]);
		__multiply331(m,&matrix[8],&t[6]);
		__multiply331(m,&matrix[12],&t[9]);
		_c3(t,matrix);
		_c3(&t[3],&matrix[4]);
		_c3(&t[6],&matrix[8]);
		_c3(&t[9],&matrix[12]);
	}

	MFUNC3 (void)	_qRotateSystemCCW(const C0 q[4], const C1 matrix[16], C2 m_out[16])
	{
		C1 m[9];
		_q2RotMatrixCCW(q,m);
		__multiply331(m,matrix,m_out);
		__multiply331(m,&matrix[4],&m_out[4]);
		__multiply331(m,&matrix[8],&m_out[8]);
		__multiply331(m,&matrix[12],&m_out[12]);
		m_out[3] = m_out[7] = m_out[11] = 0;
		m_out[15] = 1;
	}

	MFUNC2 (void)	_qRotateSystemCCW(const C0 q[4], C1 matrix[16])
	{
		C1 m[9],t[12];
		_q2RotMatrixCCW(q,m);
		__multiply331(m,matrix,t);
		__multiply331(m,&matrix[4],&t[3]);
		__multiply331(m,&matrix[8],&t[6]);
		__multiply331(m,&matrix[12],&t[9]);
		_c3(t,matrix);
		_c3(&t[3],&matrix[4]);
		_c3(&t[6],&matrix[8]);
		_c3(&t[9],&matrix[12]);
	}


	MFUNC3 (void)	_qRotateMatrix3x3CW(const C0 q[4], const C1 matrix[9], C2 m_out[9])
	{
		C1 m[9];
		_q2RotMatrixCW(q,m);
		__multiply3(m,matrix,m_out);
	}

	MFUNC2 (void)	_qRotateMatrix3x3CW(const C0 q[4], C1 m_inout[9])
	{
		C1 m[9],t[9];
		_q2RotMatrixCW(q,m);
		__multiply3(m,m_inout,t);
		_c9(t,m_inout);
	}

	MFUNC3 (void)	_qRotateMatrix3x3CCW(const C0 q[4], const C1 matrix[9], C2 m_out[9])
	{
		C1 m[9];
		_q2RotMatrixCCW(q,m);
		__multiply3(m,matrix,m_out);
	}

	MFUNC2 (void)	_qRotateMatrix3x3CCW(const C0 q[4], C1 m_inout[9])
	{
		C1 m[9],t[9];
		_q2RotMatrixCCW(q,m);
		__multiply3(m,m_inout,t);
		_c9(t,m_inout);
	}

	MFUNC4 (void)	_qMultiply(const C0 q0[4], const C1 q1[4], const C2 q2[4], C3 out[4])
	{
		C0 buffer[4];
		_qMultiply(q0,q1,buffer);
		_qMultiply(buffer,q2,out);
	}

	MFUNC3 (void) _qRotateCW(const C0 q[4], const C1 p[3], C2 out[3])
	{
		C0	t0 =	-q[1]*p[0] - q[2]*p[1] - q[3]*p[2],
			t1 =	 q[0]*p[0] + q[2]*p[2] - q[3]*p[1],
			t2 =	 q[0]*p[1] - q[1]*p[2] + q[3]*p[0],
			t3 =	 q[0]*p[2] + q[1]*p[1] - q[2]*p[0];

		out[0] = -t0*q[1] + t1*q[0] - t2*q[3] + t3*q[2];
		out[1] = -t0*q[2] + t1*q[3] + t2*q[0] - t3*q[1];
		out[2] = -t0*q[3] - t1*q[2] + t2*q[1] + t3*q[0];
	}

	MFUNC2 (void) _qRotateCW(const C0 q[4], C1 p[3])
	{
		C0	t0 =	-q[1]*p[0] - q[2]*p[1] - q[3]*p[2],
			t1 =	 q[0]*p[0] + q[2]*p[2] - q[3]*p[1],
			t2 =	 q[0]*p[1] - q[1]*p[2] + q[3]*p[0],
			t3 =	 q[0]*p[2] + q[1]*p[1] - q[2]*p[0];

		p[0] = -t0*q[1] + t1*q[0] - t2*q[3] + t3*q[2];
		p[1] = -t0*q[2] + t1*q[3] + t2*q[0] - t3*q[1];
		p[2] = -t0*q[3] - t1*q[2] + t2*q[1] + t3*q[0];
	}
	
	MFUNC3 (void) _qRotateCCW(const C0 q[4], const C1 p[3], C2 out[3])
	{
		C0	t0 =	 q[1]*p[0] + q[2]*p[1] + q[3]*p[2],
			t1 =	 q[0]*p[0] - q[2]*p[2] + q[3]*p[1],
			t2 =	 q[0]*p[1] + q[1]*p[2] - q[3]*p[0],
			t3 =	 q[0]*p[2] - q[1]*p[1] + q[2]*p[0];

		out[0] = t0*q[1] + t1*q[0] + t2*q[3] - t3*q[2];
		out[1] = t0*q[2] - t1*q[3] + t2*q[0] + t3*q[1];
		out[2] = t0*q[3] + t1*q[2] - t2*q[1] + t3*q[0];
	}

	MFUNC2 (void) _qRotateCCW(const C0 q[4], C1 p[3])
	{
		C0	t0 =	 q[1]*p[0] + q[2]*p[1] + q[3]*p[2],
			t1 =	 q[0]*p[0] - q[2]*p[2] + q[3]*p[1],
			t2 =	 q[0]*p[1] + q[1]*p[2] - q[3]*p[0],
			t3 =	 q[0]*p[2] - q[1]*p[1] + q[2]*p[0];

		p[0] = t0*q[1] + t1*q[0] + t2*q[3] - t3*q[2];
		p[1] = t0*q[2] - t1*q[3] + t2*q[0] + t3*q[1];
		p[2] = t0*q[3] + t1*q[2] - t2*q[1] + t3*q[0];
	}



	MFUNC3 (void)	_qMultiply(const C0 q0[4], const C1 q1[4], C2 out[4])
	{
		out[0] = q0[0]*q1[0] - q0[1]*q1[1] - q0[2]*q1[2] - q0[3]*q1[3];
		out[1] = q0[0]*q1[1] + q0[1]*q1[0] + q0[2]*q1[3] - q0[3]*q1[2];
		out[2] = q0[0]*q1[2] - q0[1]*q1[3] + q0[2]*q1[0] + q0[3]*q1[1];
		out[3] = q0[0]*q1[3] + q0[1]*q1[2] - q0[2]*q1[1] + q0[3]*q1[0];
	}

	MFUNC3 (void)	_qDivide(const C0 q0[4], const C1 q1[4], C2 out[4])
	{
		C0 div = q1[0]*q1[0]+q1[1]*q1[1]+q1[2]*q1[2]+q1[3]*q1[3];

		out[0] = ( q0[0]*q1[0] + q0[1]*q1[1] + q0[2]*q1[2] + q0[3]*q1[3]) / div;
		out[1] = (-q0[0]*q1[1] + q0[1]*q1[0] - q0[2]*q1[3] + q0[3]*q1[2]) / div;
		out[2] = (-q0[0]*q1[2] + q0[1]*q1[3] + q0[2]*q1[0] - q0[3]*q1[1]) / div;
		out[3] = (-q0[0]*q1[3] - q0[1]*q1[2] + q0[2]*q1[1] + q0[3]*q1[0]) / div;
	}

	MFUNC3 (void)	_qAdd(const C0 q0[4], const C1 q1[4], C2 out[4])
	{
		out[0] = q0[0]+q1[0];
		out[1] = q0[1]+q1[1];
		out[2] = q0[2]+q1[2];
		out[3] = q0[3]+q1[3];
	}

	MFUNC2 (void)	_qAdd(C0 inout[4], const C1 q1[4])
	{
		inout[0]+=q1[0];
		inout[1]+=q1[1];
		inout[2]+=q1[2];
		inout[3]+=q1[3];
	}

	MFUNC3 (void)	_qSubtract(const C0 q0[4], const C1 q1[4], C2 out[4])
	{
		out[0] = q0[0]-q1[0];
		out[1] = q0[1]-q1[1];
		out[2] = q0[2]-q1[2];
		out[3] = q0[3]-q1[3];
	}

	MFUNC2 (void)	_qSubtract(C0 inout[4], const C1 q1[4])
	{
		inout[0]-=q1[0];
		inout[1]-=q1[1];
		inout[2]-=q1[2];
		inout[3]-=q1[3];
	}

	MFUNC2 (void)	_qConjugate(const C0 q[4], C1 out[4])
	{
		out[0] = q[0];
		out[1] = -q[1];
		out[2] = -q[2];
		out[3] = -q[3];
	}

	MFUNC (void)	_qConjugate(C inout[4])
	{
		inout[1] = -inout[1];
		inout[2] = -inout[2];
		inout[3] = -inout[3];
	}

	MFUNC2 (void)	_qInvert(const C0 q[4], C1 out[4])
	{
		C0 div = q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3];
		out[0] =	q[0]/div;
		out[1] = -q[1]/div;
		out[2] = -q[2]/div;
		out[3] = -q[3]/div;
	}

	MFUNC2 (void)	_q2AxisRotation(const C0 q[4], C1 out[4])
	{
		out[3] = acos(q[0])*360 /M_PI;
		_c3(&q[1],out);
		_normalize0(out);
	}

	MFUNC2 (void) _q2Matrix(const C0 q[4], C1 out[16]) //untested
	{
		_v4(out,		q[0],	q[1],	q[2],	q[3]);
		_v4(&out[4],	-q[1],	q[0],	-q[3],	q[2]);
		_v4(&out[8],	-q[2],	q[3],	q[0],	-q[1]);
		_v4(&out[12],	-q[3],	-q[2],	q[1],	q[0]);
	}
	
	MFUNC2 (void) _q2RotMatrixCCW(const C0 q[4], C1 out[9])
	{
			//ccw-version:
			out[0] = q[0]*q[0] + q[1]*q[1] - q[2]*q[2] - q[3]*q[3];
			out[1] = 2*q[1]*q[2] - 2*q[0]*q[3];
			out[2] = 2*q[1]*q[3] + 2*q[0]*q[2];

			out[3] = 2*q[1]*q[2] + 2*q[0]*q[3];
			out[4] = q[0]*q[0] - q[1]*q[1] + q[2]*q[2] - q[3]*q[3];
			out[5] = 2*q[2]*q[3] - 2*q[0]*q[1];

			out[6] = 2*q[1]*q[3] - 2*q[0]*q[2];
			out[7] = 2*q[2]*q[3] + 2*q[0]*q[1];
			out[8] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];
	}

	
	MFUNC2 (void) _q2RotMatrixCW(const C0 q[4], C1 out[9])
	{
			//cw-version:
			out[0] = q[0]*q[0] + q[1]*q[1] - q[2]*q[2] - q[3]*q[3];
			out[1] = 2*q[1]*q[2] + 2*q[0]*q[3];
			out[2] = 2*q[1]*q[3] - 2*q[0]*q[2];

			out[3] = 2*q[1]*q[2] - 2*q[0]*q[3];
			out[4] = q[0]*q[0] - q[1]*q[1] + q[2]*q[2] - q[3]*q[3];
			out[5] = 2*q[2]*q[3] + 2*q[0]*q[1];

			out[6] = 2*q[1]*q[3] + 2*q[0]*q[2];
			out[7] = 2*q[2]*q[3] - 2*q[0]*q[1];
			out[8] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];
	}
	
	MFUNC3 (void) _q2RotMatrixCCW(const C0& angle, const C1 axis[3], C2 out[9])
	{
		C2	q[4];
		_q2Quaternion(angle,axis,q);
		_q2RotMatrixCCW(q,out);
	}

	MFUNC5 (void) _q2RotMatrixCCW(const C0& angle, const C1&x, const C2&y, const C3&z, C4 out[9])
	{
		C4	axis[] = {x,y,z};
		_normalize(axis);
		_q2RotMatrixCCW(angle,axis,out);
	}

	MFUNC3 (void) _q2RotMatrixCW(const C0& angle, const C1 axis[3], C2 out[9])
	{
		C2	q[4];
		_q2Quaternion(angle,axis,q);
		_q2RotMatrixCW(q,out);
	}

	MFUNC5 (void) _q2RotMatrixCW(const C0& angle, const C1&x, const C2&y, const C3&z, C4 out[9])
	{
		C4	axis[] = {x,y,z};
		_normalize(axis);
		_q2RotMatrixCW(angle,axis,out);
	}


	

	MFUNC2 (void)	_q2Quaternion(const C0 axis_rotation[4], C1 out[4])
	{
		out[0] = cos((C1)axis_rotation[3]*M_PI/360);
		C1 s = sin((C1)axis_rotation[3]*M_PI/360);
		_multiply(axis_rotation,s,&out[1]);
	}

	MFUNC3 (void)	_q2Quaternion(const C0&angle, const C1 axis[3], C2 out[4])
	{
		out[0] = cos((C2)angle*M_PI/360);
		C2 s = sin((C2)angle*M_PI/360);
		_multiply(axis,s,&out[1]);
	}

	MFUNC5 (void)	_q2Quaternion(const C0&angle, const C1&x, const C2&y, const C3&z, C4 out[4])
	{
		out[0] = cos((C4)angle*M_PI/360);
		C4 s = sin((C4)angle*M_PI/360) / vsqrt((C4)x*x + (C4)y*y + (C4)z*z);
		out[1] = s * x;
		out[2] = s * y;
		out[3] = s * z;
	}

	MFUNC (C)	 _qLength(const C q[4])
	{
		return vsqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
	}







	template <class C>
	MF_CONSTRUCTOR1 Complex<C>::Complex(const C0&r):re(r),im(0)
	{}


	template <class C>
	MF_CONSTRUCTOR2 Complex<C>::Complex(const C0&r, const C1&i):re(r),im(i)
	{}

	template <class C> MF_CONSTRUCTOR Complex<C>::Complex()
	{}

	template <class C>
	MFUNC1 (void) Complex<C>::operator+=(const Complex<C0>&other)
	{
		re+= other.re;
		im+= other.im;
	}

	template <class C>
	MFUNC1 (void) Complex<C>::operator-=(const Complex<C0>&other)
	{
		re-= other.re;
		im-= other.im;
	}

	template <class C>
	MFUNC1 (void) Complex<C>::operator*=(const Complex<C0>&other)
	{
		C	re_ = re*other.re - im*other.im,
			im_ = re*other.im + im*other.re;
		re = re_;
		im = im_;
	}

	template <class C>
	MFUNC1 (void) Complex<C>::operator/=(const Complex<C0>&other)
	{
		C	re_ = (re*other.re + im*other.im)/(other.re*other.re + other.im*other.im),
			im_ = (im*other.re + re*other.im)/(other.re*other.re + other.im*other.im);
		re = re_;
		im = im_;
	}

	template <class C>
	MFUNC1 (Complex<C>) Complex<C>::operator+(const Complex<C0>&other) const
	{
		return Complex<C>(re+other.re,im+other.im);
	}

	template <class C>
	MFUNC1(Complex<C>) Complex<C>::operator-(const Complex<C0>&other) const
	{
		return Complex<C>(re-other.re,im-other.im);
	}

	template <class C>
	MFUNC1(Complex<C>) Complex<C>::operator*(const Complex<C0>&other) const
	{
		return Complex<C>(re*other.re-im*other.im,re*other.im+im*other.re);
	}

	template <class C>
	MFUNC1(Complex<C>) Complex<C>::operator/(const Complex<C0>&other) const
	{
		return Complex<C>(
							(re*other.re+im*other.im)/(other.re*other.re+other.im*other.im),
							(im*other.re+re*other.im)/(other.re*other.re+other.im*other.im)
							);
	}

	template <class C>
	MFUNC1(bool) Complex<C>::operator==(const Complex<C0>&other)
	{
		return sqr(re - other.re) + sqr(im - other.im) < getError<C>() * getError<C0>();
	}

	template <class C>
	MFUNC1(bool) Complex<C>::operator!=(const Complex<C0>&other)
	{
		return sqr(re - other.re) + sqr(im - other.im) >= getError<C>() * getError<C0>();
	}

	MFUNC (C) Complex<C>::length()
	{
		return sqrt(re * re + im * im);
	}

	MFUNC (Complex<C>) Complex<C>::conjugate()
	{
		return Complex<C>(re,-im);
	}
	
	MFUNC (String)		 Complex<C>::ToString()
	{
		unsigned len = v2len(re)+1,l;
		if (im)
			len += v2len(im) + 1;
		if (im > 0)
			len++;
		char*buffer = makeStr(len),
			*rs = buffer;
		strcpy(rs,v2str(re,l));
		rs+=l;
		if (im)
		{
			if (im > 0)
				(*rs++) = '+';
			strcpy(rs,v2str(im,l));
			rs+=l;
			(*rs++) = 'i';
		}
		(*rs++) = 0;
		if (rs != buffer+len)
			FATAL__("transformation-error");
		return buffer;
	}







	template <class C> MF_CONSTRUCTOR Quaternion<C>::Quaternion()//:re(0),i(0),j(0),k(0)
	{}

	template <class C> MF_CONSTRUCTOR1 Quaternion<C>::Quaternion(const C0&r_):re(r_),i(0),j(0),k(0)
	{}

	template <class C>
	MF_CONSTRUCTOR4 Quaternion<C>::Quaternion(const C0&r_, const C1&i_, const C2&j_, const C3&k_):re(r_),i(i_),j(j_),k(k_)
	{}


	template <class C>
	MFUNC1 (void) Quaternion<C>::operator+=(const Quaternion<C0>&other)
	{
		re += other.re;
		i += other.i;
		j += other.j;
		k += other.k;
	}

	template <class C>
	MFUNC1 (void) Quaternion<C>::operator-=(const Quaternion<C0>&other)
	{
		re -= other.re;
		i -= other.i;
		j -= other.j;
		k -= other.k;
	}

	template <class C>
	MFUNC1 (void) Quaternion<C>::operator*=(const Quaternion<C0>&other)
	{
		C buffer[4];
		_qMultiply(q,other.q,buffer);
		_c4(buffer,q);
	}

	template <class C>
	MFUNC1 (void) Quaternion<C>::operator/=(const Quaternion<C0>&other)
	{
		C buffer[4];
		_qDivide(q,other.q,buffer);
		_c4(buffer,q);
	}

	template <class C>
	MFUNC1 (Quaternion<C>) Quaternion<C>::operator+(const Quaternion<C0>&other)		const
	{
		return Quaternion<C>(	re + other.re,
								i + other.i,
								j + other.j,
								k + other.k);
	}

	template <class C>
	MFUNC1 (Quaternion<C>) Quaternion<C>::operator-(const Quaternion<C0>&other)		const
	{
		return Quaternion<C>(	re - other.re,
								i - other.i,
								j - other.j,
								k - other.k);
	}

	template <class C>
	MFUNC1 (Quaternion<C>)	 Quaternion<C>::operator*(const Quaternion<C0>&other)		const
	{
		Quaternion<C> result;
		_qMultiply(q,other.q,result.q);
		return result;
	}

	template <class C>
	MFUNC1 (Quaternion<C>)	 Quaternion<C>::operator/(const Quaternion<C0>&other)		const
	{
		Quaternion<C> result;
		_qDivide(q,other.w,result.q);
		return result;
	}

	template <class C>
	MFUNC1 (bool) Quaternion<C>::operator==(const Quaternion<C0>&other)
	{
		return _similar(q,other.q,getError<C>());
	}

	template <class C>
	MFUNC1 (bool) Quaternion<C>::operator!=(const Quaternion<C0>&other)
	{
		return !_similar(q,other.q,getError<C>());
	}

	MFUNC (C) Quaternion<C>::length()
	{
		return _length4(q);
	}

	MFUNC (Quaternion<C>) Quaternion<C>::invert()
	{
		C div = q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3];

		return Quaternion<C>(re/div, -i/div, -j/div, -k/div);
	}

	MFUNC (Quaternion<C>) Quaternion<C>::conjugate()
	{
		return Quaternion<C>(re, -i, -j, -k);
	}
	
	template <class C>
	MFUNC2 (void) Quaternion<C>::buildFrom(const C0&angle, const C1 axis[3])
	{
		_q2Quaternion(angle,axis,q);
	}
	
	template <class C>
	MFUNC4 (void) Quaternion<C>::buildFrom(const C0&angle, const C1&ax, const C2&ay, const C3&az)
	{
		_q2Quaternion(angle,ax,ay,az,q);
	}
	
	template <class C>
	MFUNC1 (void) Quaternion<C>::buildRotMatrixCW(C0 matrix[3*3])
	{
		_q2RotMatrixCW(q,matrix);
	}

	template <class C>
	MFUNC1 (void) Quaternion<C>::buildRotMatrixCCW(C0 matrix[3*3])
	{
		_q2RotMatrixCCW(q,matrix);
	}
	
	template <class C>
	MFUNC2 (void) Quaternion<C>::rotateCW(const C0 vec[3], C1 out[3])
	{
		_qRotateCW(q,vec,out);
	}

	template <class C>
	MFUNC2 (void) Quaternion<C>::rotateCCW(const C0 vec[3], C1 out[3])
	{
		_qRotateCCW(q,vec,out);
	}
	
	template <class C>
	MFUNC1 (void) Quaternion<C>::rotateCW(C0 vec[3])
	{
		_qRotateCW(q,vec);
	}
	
	template <class C>
	MFUNC1 (void) Quaternion<C>::rotateCCW(C0 vec[3])
	{
		_qRotateCCW(q,vec);
	}

}

#endif
