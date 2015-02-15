#ifndef complex_mathTplH
#define complex_mathTplH

/******************************************************************

Collection of template functions regarding complex math.

******************************************************************/


namespace ComplexMath
{

	MFUNC5 (void) _q(TVec4<C0>&out,const C1&a,const C2&x,const C3&y,const C4&z)
	{
		C0 s = vsin((C0)a*M_PI/360)/vsqrt((C0)x*x+(C0)y*y+(C0)z*z);
		out.x = vcos((C0)a*M_PI/360);
		out.y = s*x;
		out.z = s*y;
		out.w = s*z;
	}

	MFUNC3 (void)	_qRotateSystemCW(const TVec4<C0>&q, const TMatrix4<C1>&matrix, TMatrix4<C2>&m_out)
	{
		TMatrix3<C1> m;
		_q2RotMatrixCW(q,m);
		Mat::mult(m,matrix.x.xyz,m_out.x.xyz);
		Mat::mult(m,matrix.y.xyz,m_out.y.xyz);
		Mat::mult(m,matrix.z.xyz,m_out.z.xyz);
		Mat::mult(m,matrix.w.xyz,m_out.w.xyz);
		Mat::resetBottomRow(m_out);
	}

	MFUNC2 (void)	_qRotateSystemCW(const TVec4<C0>&q, TMatrix4<C1>&matrix)
	{
		TMatrix3<C1> m;
		_q2RotMatrixCW(q,m);
		Mat::mult(m,matrix.x.xyz);
		Mat::mult(m,matrix.y.xyz);
		Mat::mult(m,matrix.z.xyz);
		Mat::mult(m,matrix.w.xyz);
	}

	MFUNC3 (void)	_qRotateSystemCCW(const TVec4<C0>&q, const TMatrix4<C1>&matrix, TMatrix4<C2>&m_out)
	{
		TMatrix3<C1> m;
		_q2RotMatrixCCW(q,m);
		Mat::mult(m,matrix.x.xyz,m_out.x.xyz);
		Mat::mult(m,matrix.y.xyz,m_out.y.xyz);
		Mat::mult(m,matrix.z.xyz,m_out.z.xyz);
		Mat::mult(m,matrix.w.xyz,m_out.w.xyz);
		Mat::resetBottomRow(m_out);
	}

	MFUNC2 (void)	_qRotateSystemCCW(const TVec4<C0>&q, TMatrix4<C1>&matrix)
	{
		TMatrix3<C1> m;
		_q2RotMatrixCCW(q,m);
		Mat::mult(m,matrix.x.xyz);
		Mat::mult(m,matrix.y.xyz);
		Mat::mult(m,matrix.z.xyz);
		Mat::mult(m,matrix.w.xyz);
	}


	MFUNC3 (void)	_qRotateMatrix3x3CW(const TVec4<C0>&q, const TMatrix3<C1>&matrix, TMatrix3<C2>&m_out)
	{
		TMatrix3<C1> m;
		_q2RotMatrixCW(q,m);
		Mat::mult(m,matrix,m_out);
	}

	MFUNC2 (void)	_qRotateMatrix3x3CW(const TVec4<C0>&q, TMatrix3<C1>&m_inout)
	{
		TMatrix3<C1> m;
		_q2RotMatrixCW(q,m);
		Mat::mult(m,m_inout);
	}

	MFUNC3 (void)	_qRotateMatrix3x3CCW(const TVec4<C0>&q, const TMatrix3<C1>&matrix, TMatrix3<C2>&m_out)
	{
		TMatrix3<C1> m;
		_q2RotMatrixCCW(q,m);
		Mat::mult(m,matrix,m_out);
	}

	MFUNC2 (void)	_qRotateMatrix3x3CCW(const TVec4<C0>&q, TMatrix3<C1>&m_inout)
	{
		TMatrix3<C1> m;
		_q2RotMatrixCCW(q,m);
		Mat::mult(m,m_inout);
	}

	MFUNC4 (void)	_qMultiply(const TVec4<C0>&q0, const TVec4<C1>&q1, const TVec4<C2>&q2, TVec4<C3>&out)
	{
		TVec4<C0> buffer;
		_qMultiply(q0,q1,buffer);
		_qMultiply(buffer,q2,out);
	}

	MFUNC3 (void) _qRotateCW(const TVec4<C0>&q, const TVec3<C1>&p, TVec3<C2>&out)
	{
		C0	t0 =	-q.y*p.x - q.z*p.y - q.w*p.z,
			t1 =	 q.x*p.x + q.z*p.z - q.w*p.y,
			t2 =	 q.x*p.y - q.y*p.z + q.w*p.x,
			t3 =	 q.x*p.z + q.y*p.y - q.z*p.x;

		out.x = -t0*q.y + t1*q.x - t2*q.w + t3*q.z;
		out.y = -t0*q.z + t1*q.w + t2*q.x - t3*q.y;
		out.z = -t0*q.w - t1*q.z + t2*q.y + t3*q.x;
	}

	MFUNC2 (void) _qRotateCW(const TVec4<C0>&q, TVec3<C1>&p)
	{
		C0	t0 =	-q.y*p.x - q.z*p.y - q.w*p.z,
			t1 =	 q.x*p.x + q.z*p.z - q.w*p.y,
			t2 =	 q.x*p.y - q.y*p.z + q.w*p.x,
			t3 =	 q.x*p.z + q.y*p.y - q.z*p.x;

		p.x = -t0*q.y + t1*q.x - t2*q.w + t3*q.z;
		p.y = -t0*q.z + t1*q.w + t2*q.x - t3*q.y;
		p.z = -t0*q.w - t1*q.z + t2*q.y + t3*q.x;
	}
	
	MFUNC3 (void) _qRotateCCW(const TVec4<C0>&q, const TVec3<C1>&p, TVec3<C2>&out)
	{
		C0	t0 =	 q.y*p.x + q.z*p.y + q.w*p.z,
			t1 =	 q.x*p.x - q.z*p.z + q.w*p.y,
			t2 =	 q.x*p.y + q.y*p.z - q.w*p.x,
			t3 =	 q.x*p.z - q.y*p.y + q.z*p.x;

		out.x = t0*q.y + t1*q.x + t2*q.w - t3*q.z;
		out.y = t0*q.z - t1*q.w + t2*q.x + t3*q.y;
		out.z = t0*q.w + t1*q.z - t2*q.y + t3*q.x;
	}

	MFUNC2 (void) _qRotateCCW(const TVec4<C0>&q, TVec3<C1>&p)
	{
		C0	t0 =	 q.y*p.x + q.z*p.y + q.w*p.z,
			t1 =	 q.x*p.x - q.z*p.z + q.w*p.y,
			t2 =	 q.x*p.y + q.y*p.z - q.w*p.x,
			t3 =	 q.x*p.z - q.y*p.y + q.z*p.x;

		p.x = t0*q.y + t1*q.x + t2*q.w - t3*q.z;
		p.y = t0*q.z - t1*q.w + t2*q.x + t3*q.y;
		p.z = t0*q.w + t1*q.z - t2*q.y + t3*q.x;
	}



	MFUNC3 (void)	_qMultiply(const TVec4<C0>&q0, const TVec4<C1>&q1, TVec4<C2>&out)
	{
		out.x = q0.x*q1.x - q0.y*q1.y - q0.z*q1.z - q0.w*q1.w;
		out.y = q0.x*q1.y + q0.y*q1.x + q0.z*q1.w - q0.w*q1.z;
		out.z = q0.x*q1.z - q0.y*q1.w + q0.z*q1.x + q0.w*q1.y;
		out.w = q0.x*q1.w + q0.y*q1.z - q0.z*q1.y + q0.w*q1.x;
	}

	MFUNC3 (void)	_qDivide(const TVec4<C0>&q0, const TVec4<C1>&q1, TVec4<C2>&out)
	{
		C0 div = q1.x*q1.x+q1.y*q1.y+q1.z*q1.z+q1.w*q1.w;

		out.x = ( q0.x*q1.x + q0.y*q1.y + q0.z*q1.z + q0.w*q1.w) / div;
		out.y = (-q0.x*q1.y + q0.y*q1.x - q0.z*q1.w + q0.w*q1.z) / div;
		out.z = (-q0.x*q1.z + q0.y*q1.w + q0.z*q1.x - q0.w*q1.y) / div;
		out.w = (-q0.x*q1.w - q0.y*q1.z + q0.z*q1.y + q0.w*q1.x) / div;
	}

	MFUNC3 (void)	_qAdd(const TVec4<C0>&q0, const TVec4<C1>&q1, TVec4<C2>&out)
	{
		out.x = q0.x+q1.x;
		out.y = q0.y+q1.y;
		out.z = q0.z+q1.z;
		out.w = q0.w+q1.w;
	}

	MFUNC2 (void)	_qAdd(TVec4<C0>&inout, const TVec4<C1>&q1)
	{
		inout.x+=q1.x;
		inout.y+=q1.y;
		inout.z+=q1.z;
		inout.w+=q1.w;
	}

	MFUNC3 (void)	_qSubtract(const TVec4<C0>&q0, const TVec4<C1>&q1, TVec4<C2>&out)
	{
		out.x = q0.x-q1.x;
		out.y = q0.y-q1.y;
		out.z = q0.z-q1.z;
		out.w = q0.w-q1.w;
	}

	MFUNC2 (void)	_qSubtract(TVec4<C0>&inout, const TVec4<C1>&q1)
	{
		inout.x-=q1.x;
		inout.y-=q1.y;
		inout.z-=q1.z;
		inout.w-=q1.w;
	}

	MFUNC2 (void)	_qConjugate(const TVec4<C0>&q, TVec4<C1>&out)
	{
		out.x = q.x;
		out.y = -q.y;
		out.z = -q.z;
		out.w = -q.w;
	}

	MFUNC (void)	_qConjugate(TVec4<C>&inout)
	{
		inout.y = -inout.y;
		inout.z = -inout.z;
		inout.w = -inout.w;
	}

	MFUNC2 (void)	_qInvert(const TVec4<C0>&q, TVec4<C1>&out)
	{
		C0 div = q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;
		out.x =	q.x/div;
		out.y = -q.y/div;
		out.z = -q.z/div;
		out.w = -q.w/div;
	}

	MFUNC2 (void)	_q2AxisRotation(const TVec4<C0>&q, TVec4<C1>&out)
	{
		out.w = acos(q.x)*360 /M_PI;
		out.xyz = q.yzw;
		//_c3(&q.y,out);
		Vec::normalize(out.xyz);
	}

	MFUNC2 (void) _q2Matrix(const TVec4<C0>&q, TMatrix4<C1>&out) //untested
	{
		Vec::def(out.x,	q.x,	q.y,	q.z,	q.w);
		Vec::def(out.y,	-q.y,	q.x,	-q.w,	q.z);
		Vec::def(out.z,	-q.z,	q.w,	q.x,	-q.y);
		Vec::def(out.w,	-q.w,	-q.z,	q.y,	q.x);
	}
	
	MFUNC2 (void) _q2RotMatrixCCW(const TVec4<C0>&q, TMatrix3<C1>&out)
	{
			//ccw-version:
		out.x.x = q.x*q.x + q.y*q.y - q.z*q.z - q.w*q.w;
		out.x.y = 2*q.y*q.z - 2*q.x*q.w;
		out.x.z = 2*q.y*q.w + 2*q.x*q.z;

		out.y.x = 2*q.y*q.z + 2*q.x*q.w;
		out.y.y = q.x*q.x - q.y*q.y + q.z*q.z - q.w*q.w;
		out.y.z = 2*q.z*q.w - 2*q.x*q.y;

		out.z.x = 2*q.y*q.w - 2*q.x*q.z;
		out.z.y = 2*q.z*q.w + 2*q.x*q.y;
		out.z.z = q.x*q.x - q.y*q.y - q.z*q.z + q.w*q.w;
	}

	
	MFUNC2 (void) _q2RotMatrixCW(const TVec4<C0>&q, TMatrix3<C1>&out)
	{
			//cw-version:
			out.x.x = q.x*q.x + q.y*q.y - q.z*q.z - q.w*q.w;
			out.x.y = 2*q.y*q.z + 2*q.x*q.w;
			out.x.z = 2*q.y*q.w - 2*q.x*q.z;

			out.y.x = 2*q.y*q.z - 2*q.x*q.w;
			out.y.y = q.x*q.x - q.y*q.y + q.z*q.z - q.w*q.w;
			out.y.z = 2*q.z*q.w + 2*q.x*q.y;

			out.z.x = 2*q.y*q.w + 2*q.x*q.z;
			out.z.y = 2*q.z*q.w - 2*q.x*q.y;
			out.z.z = q.x*q.x - q.y*q.y - q.z*q.z + q.w*q.w;
	}
	
	MFUNC3 (void) _q2RotMatrixCCW(const C0& angle, const TVec3<C1>&axis, TMatrix3<C2>&out)
	{
		TVec4<C2>	q;
		_q2Quaternion(angle,axis,q);
		_q2RotMatrixCCW(q,out);
	}

	MFUNC5 (void) _q2RotMatrixCCW(const C0& angle, const C1&x, const C2&y, const C3&z, TMatrix3<C4>&out)
	{
		TVec3<C4>	axis = {x,y,z};
		Vec::normalize(axis);
		_q2RotMatrixCCW(angle,axis,out);
	}

	MFUNC3 (void) _q2RotMatrixCW(const C0& angle, const TVec3<C1>&axis, TMatrix3<C2>&out)
	{
		TVec4<C2>	q;
		_q2Quaternion(angle,axis,q);
		_q2RotMatrixCW(q,out);
	}

	MFUNC5 (void) _q2RotMatrixCW(const C0& angle, const C1&x, const C2&y, const C3&z, TMatrix3<C4>&out)
	{
		TVec3<C4>	axis = {x,y,z};
		_normalize(axis);
		_q2RotMatrixCW(angle,axis,out);
	}


	

	MFUNC2 (void)	_q2Quaternion(const TVec4<C0>&axis_rotation, TVec4<C1>&out)
	{
		out.x = cos((C1)axis_rotation.w*M_PI/360);
		C1 s = sin((C1)axis_rotation.w*M_PI/360);
		Vec::multiply(axis_rotation.xyz,s,out.yzw);
	}

	MFUNC3 (void)	_q2Quaternion(const C0&angle, const TVec3<C1>&axis, TVec4<C2>&out)
	{
		out.x = cos((C2)angle*M_PI/360);
		C2 s = sin((C2)angle*M_PI/360);
		Vec::multiply(axis,s,out.yzw);
	}

	MFUNC5 (void)	_q2Quaternion(const C0&angle, const C1&x, const C2&y, const C3&z, TVec4<C4>&out)
	{
		out.x = cos((C4)angle*M_PI/360);
		C4 s = sin((C4)angle*M_PI/360) / vsqrt((C4)x*x + (C4)y*y + (C4)z*z);
		out.y = s * x;
		out.z = s * y;
		out.w = s * z;
	}

	MFUNC (C)	 _qLength(const TVec4<C>&q)
	{
		return vsqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);
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
