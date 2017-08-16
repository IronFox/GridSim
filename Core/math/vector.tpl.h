#ifndef vector_mathTplH
#define vector_mathTplH

/******************************************************************

Collection of template vector-math-functions.

******************************************************************/


namespace Math
{
	#ifndef __BORLANDC__

	//helper-classes for operations on vectors of static sizes - sadly borland doesn't like efficient stuff
	PERFORMER_CLASS2(QCopy,const,,(*b)=(*a))
	PERFORMER_CLASS4(QAdd3,const,const,const,,(*d) = (*a)+(*b)+(*c))
	PERFORMER_CLASS(QRotateLeft,(*a) = (*(a+1)))
	PERFORMER_CLASS(QMakeAbs,(*a)=vabs(*a))
	PERFORMER_CLASS3(QSub,const, const,,(*c)=(*a)-(*b))
	PERFORMER_CLASS2(QSubDirect,,const,(*a)-=(*b))
	PERFORMER_CLASS3(QAdd,const,const,,(*c)=(*a)+(*b))
	PERFORMER_CLASS2(QAddDirect,,const,(*a)+=(*b))
	PERFORMER_CLASS3(QStretch,const,const,,(*c)=(*a)*(*b))
	PERFORMER_CLASS3(QResolve,const,const,,(*c)=(*a)/(*b))
	PERFORMER_CLASS2(QVectorSort,,,if ((*a)>(*b)) {C0 save = (*a); (*a) = (*b); (*b) = save;})

	QUERY_CLASS2(QAllGreater,bool,const,const,return (*a) >= (*b),return previous && (*a) >= (*b))
	QUERY_CLASS2(QAllLess,bool,const,const,return (*a)<=(*b),return previous && (*a) <= (*b))
	QUERY_CLASS2(QOneGreater,bool,const,const,return (*a)>=(*b),return previous || (*a)>=(*b))
	QUERY_CLASS2(QOneLess,bool,const,const,return (*a)<=(*b),return previous || (*a)<=(*b))
	QUERY_CLASS(QGreatest,C,const,return (*a),return (*a)>previous?(*a):previous)
	QUERY_CLASS(QLeast,C,const,return (*a),return (*a)<previous?(*a):previous)
	QUERY_CLASS(QZero,bool,const,return (*a)==0,return previous && (*a)==0)
	QUERY_CLASS2(QDot,C0,const,const,return (*a)*(*b),return previous+(*a)*(*b))
	QUERY_CLASS(QIsNan,bool,const,return (*a),return previous||isnan(*a))
	QUERY_CLASS(QSum,C,const,return (*a),return previous+(*a))
	QUERY_CLASS2(QEqual,bool,const,const,return (*a)==(*b),return previous&&(*a)==(*b))
	QUERY_CLASS2(QMaxAxisDistance,C0,const,const,return vabs((*a)-(*b)),C0 val = vabs((*a)-(*b)); return val>previous?val:previous)
	QUERY_CLASS2(QDistance,C0,const,const,return ((*a)-(*b))*((*a)-(*b)), return previous+((*a)-(*b))*((*a)-(*b)))

	OPERATION_CLASS4(QMad,const C0*b,const C1*d, const C2&scalar, C3*out,b+1,d+1,scalar,out+1,(*out) = (*b) + (*d)*scalar)
	OPERATION_CLASS3(QMadDirect,C0*b,const C1*d, const C2&scalar, b+1,d+1,scalar,(*b)+=(*d)*scalar)
	OPERATION_CLASS2(QScale,C0*v, const C1&factor,v+1,factor,(*v)*=factor)
	OPERATION_CLASS(QRotateRight,C*v,v-1,(*v) = (*(v-1)))
	OPERATION_CLASS4(QInterpolate,const C0*c,const C1*v, const C2&f, C3*out,c+1,v+1,f,out+1,(*out) = (*c)+((*v)-(*c))*f)
	OPERATION_CLASS3(QSubVal,const C0*v, const C1&value, C2*out, v+1,value,out+1, (*out) = (*v)-value)
	OPERATION_CLASS2(QSubValDirect,C0*v,const C1&value,v+1,value,(*v)-=value)
	OPERATION_CLASS2(QSet,C0*v, const C1&val,v+1,val,(*v)=val)
	OPERATION_CLASS3(QClamp,C0*v, const C1&min, const C2&max, v+1, min, max, (*v) = clamped((*v),min,max))
	OPERATION_CLASS3(QAddVal,const C0*v, const C1&val, C2*out,v+1,val,out+1,(*out)=(*v)+val)
	OPERATION_CLASS2(QAddValDirect,C0*v,const C1&value,v+1,value,(*v)+=value)
	OPERATION_CLASS3(QMult,const C0*v, const C1&value, C2*out,v+1,value,out+1,(*out)=(*v)*value)
	OPERATION_CLASS3(QDiv,const C0*v,const C1&value, C2*out,v+1,value,out+1,(*out)=(*v)/value)
	OPERATION_CLASS2(QDivDirect,C0*v,const C1&value,v+1,value,(*v)/=value)

	#endif

	MFUNC3 (char)		_compare(const C0*v0, const C1*v1, const C2&tolerance, count_t el_count)
	{
		VECTOR_LOOP(el_count)
		{
			C0 delta = v0[el]-v1[el];
			if (delta < -tolerance)
				return -1;
			if (delta > tolerance)
				return 1;
		}
		return 0;
	}

	#undef COMPARE
	#define COMPARE(a,b)	delta = (C0)(a-b); if (delta < -tolerance) return -1; if (delta > tolerance) return 1;

	
	namespace ByReference
	{
		MFUNC3
			(char)		_compare(const C0 v0[3], const C1 v1[3], const C2&tolerance)
			{

				C0 delta;

				COMPARE(v0[0],v1[0])
				COMPARE(v0[1],v1[1])
				COMPARE(v0[2],v1[2])

				return 0;
			}
	
	
		MFUNC3
			(char)		_compare2(const C0 v0[2], const C1 v1[2], const C2&tolerance)
			{
				C0	delta;

				COMPARE(v0[0],v1[0])
				COMPARE(v0[1],v1[1])

				return 0;
			}

		MFUNC3
			(char)		_compare4(const C0 v0[4], const C1 v1[4], const C2&tolerance)
			{
				C0	delta;

				COMPARE(v0[0],v1[0])
				COMPARE(v0[1],v1[1])
				COMPARE(v0[2],v1[2])
				COMPARE(v0[3],v1[3])

				return 0;
			}
		
		MFUNC3
			(char)		_compare6(const C0 v0[6], const C1 v1[6], const C2&tolerance)
			{
				C0	delta;

				COMPARE(v0[0],v1[0])
				COMPARE(v0[1],v1[1])
				COMPARE(v0[2],v1[2])
				COMPARE(v0[3],v1[3])
				COMPARE(v0[4],v1[4])
				COMPARE(v0[5],v1[5])

				return 0;
			}


		MFUNC3
			(char)		_compare8(const C0 v0[8], const C1 v1[8], const C2&tolerance)
			{
				C0	delta;

				COMPARE(v0[0],v1[0])
				COMPARE(v0[1],v1[1])
				COMPARE(v0[2],v1[2])
				COMPARE(v0[3],v1[3])
				COMPARE(v0[4],v1[4])
				COMPARE(v0[5],v1[5])
				COMPARE(v0[6],v1[6])
				COMPARE(v0[7],v1[7])

				return 0;
			}

		
		MFUNC3V
			(char)		_compareV(const C0*v0, const C1*v1, const C2&tolerance)
			{
				VECTOR_LOOP(Elements)
				{
					C0 delta = v0[el]-v1[el];
					if (delta < -tolerance)
						return -1;
					if (delta > tolerance)
						return 1;
				}
				return 0;
			}

		MFUNC3
			(void)		_multAdd(C0*b, const C1*d, const C2&scalar, count_t el_count)
			{
				VECTOR_LOOP(el_count)
					b[el]+=d[el]*scalar;
			}

		MFUNC3
			(void)		_multAdd(C0 b[3], const C1 d[3], const C2&scalar)
			{
				b[0]+=d[0]*scalar;
				b[1]+=d[1]*scalar;
				b[2]+=d[2]*scalar;
			}


		MFUNC3
			(void)		_multAdd2(C0 b[2], const C1 d[2], const C2&scalar)
			{
				b[0]+=d[0]*scalar;
				b[1]+=d[1]*scalar;
			}
			
		MFUNC3
			(void)		_multAdd4(C0 b[4], const C1 d[4], const C2&scalar)
			{
				b[0]+=d[0]*scalar;
				b[1]+=d[1]*scalar;
				b[2]+=d[2]*scalar;
				b[3]+=d[3]*scalar;
			}

		MFUNC3V
			(void)		_multAddV(C0*b, const C1*d,const C2&scalar)
			{
				QMadDirect<C0,C1,C2,Dimensions>::perform(b,d,scalar);
			}

		MFUNC4V
			(void)	_multAddV(const C0*b, const C1*d,const C2&scalar, C3*out)
			{
				QMad<C0,C1,C2,C3,Dimensions>::perform(b,d,scalar,out);
			}

		
		MFUNC4
			(void)	_multAdd(const C0*b, const C1*d, const C2&scalar, C3*out,count_t el_count)
			{
				VECTOR_LOOP(el_count)
					out[el] = b[el] + d[el] * scalar;
			}


		MFUNC4
			(void)	_multAdd(const C0 b[3], const C1 d[3], const C2&scalar, C3 out[3])
			{
				out[0] = b[0] + d[0] * scalar;
				out[1] = b[1] + d[1] * scalar;
				out[2] = b[2] + d[2] * scalar;
			}


		MFUNC4
			(void)	_multAdd2(const C0 b[2], const C1 d[2], const C2&scalar, C3 out[2])
			{
				out[0] = b[0] + d[0] * scalar;
				out[1] = b[1] + d[1] * scalar;
			}


		MFUNC4
			(void)	_multAdd4(const C0 b[4], const C1 d[4], const C2&scalar, C3 out[4])
			{
				out[0] = b[0] + d[0] * scalar;
				out[1] = b[1] + d[1] * scalar;
				out[2] = b[2] + d[2] * scalar;
				out[3] = b[3] + d[3] * scalar;
			}

		MFUNC2V
			(void)	_setLenV(C0*v, const C1&alength)
			{
				C0 len = QDot<C0,C0,Dimensions>::perform(v,v);
				len = vsqrt(len);
				QScale<C0,C1,Dimensions>::perform(v,(C0)alength/len);
			}


		MFUNC2
			(void) _setLen(C0*v, const C1&alength, count_t el_count)
			{
				C0 len = 0;
				VECTOR_LOOP(el_count)
					len+=v[el]*v[el];
				len = (C0)alength/vsqrt(len);
				VECTOR_LOOP(el_count)
					v[el]*=len;
			}


		MFUNC2
			(void) _setLen(C0 v[3], const C1&alength)
			{
				C0 factor = (C0)alength/vsqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
				v[0]*=factor;
				v[1]*=factor;
				v[2]*=factor;
			}

		MFUNC2
			(void) _setLen2(C0 v[2], const C1&alength)
			{
				C0 factor = (C0)alength/vsqrt(v[0]*v[0]+v[1]*v[1]);
				v[0]*=factor;
				v[1]*=factor;
			}

		MFUNC2
			(void) _setLen4(C0 v[4], const C1&alength)
			{
				C0 factor = (C0)alength/vsqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]+v[3]*v[3]);
				v[0]*=factor;
				v[1]*=factor;
				v[2]*=factor;
				v[3]*=factor;
			}


		MFUNC2
			(void)		_setLen0(C0*v, const C1&alength, count_t el_count)
			{
				C0 len = 0;
				VECTOR_LOOP(el_count)
					len+=v[el]*v[el];
				if (len <= GetError<C0>()*GetError<C0>())
					v[0] = alength;
				else
				{
					len = (C0)alength/vsqrt(len);
					VECTOR_LOOP(el_count)
						v[el]*=len;
				}
			}


		MFUNC2
			(void)		_setLen0(C0 v[3], const C1&alength)
			{
				C0 val = v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
				if (val <= GetError<C0>()*GetError<C0>())
					v[0] = alength;
				else
				{
					val = (C0)alength/vsqrt(val);
					v[0] *= val;
					v[1] *= val;
					v[2] *= val;
				}
			}


		MFUNC2
			(void)		_setLen02(C0 v[2], const C1&alength)
			{
				C0 val = v[0]*v[0]+v[1]*v[1];
				if (val <= GetError<C0>()*GetError<C0>())
					v[0] = alength;
				else
				{
					val = (C0)alength/vsqrt(val);
					v[0] *= val;
					v[1] *= val;
				}
			}
		MFUNC2
			(void)		_setLen04(C0 v[4], const C1&alength)
			{
				C0 val = v[0]*v[0]+v[1]*v[1]+v[2]*v[2]+v[3]*v[3];
				if (val <= GetError<C0>()*GetError<C0>())
					v[0] = alength;
				else
				{
					val = (C0)alength/vsqrt(val);
					v[0] *= val;
					v[1] *= val;
					v[2] *= val;
					v[3] *= val;
				}
			}

		MFUNC2V
			(void)		_setLen0V(C0*v, const C1&alength)
			{
				C0 len = QDot<C0,C0,Dimensions>::perform(v,v);
				if (len <= GetError<C0>()*GetError<C0>())
					v[0] = alength;
				else
					QScale<C0,C1,Dimensions>::perform(v,(C0)alength/vsqrt(len));
			}


		MFUNC3V
			(void) _scaleV(const C0*center, const C1&factor, C2*v)
			{
				QInterpolate<C0,C2,C1,C2,Dimensions>::perform(center,v,factor,v);
			}


		MFUNC3
			(void) _scale(const C0*center,const C1&factor, C2*v, count_t el_count)
			{
				VECTOR_LOOP(el_count)
					v[el] = center[el]+(v[el]-center[el])*factor;
			}


		MFUNC3
			(void) _scale(const C0 center[3],const C1& factor, C2 v[3])
			{
				v[0] = center[0]+(v[0]-center[0])*factor;
				v[1] = center[1]+(v[1]-center[1])*factor;
				v[2] = center[2]+(v[2]-center[2])*factor;
			}

		MFUNC3V
			(void)	_scaleAbsoluteV(const C0*center, C1*v, const C2&distance)
			{
				C2	temp[Dimensions];
				_subtractV<C0,C1,C2,Dimensions>(v,center,temp);
				C2 len=QDot<C2,C2,Dimensions>::perform(temp);
				len = vsqrt(len);
				QMad<C0,C2,C2,C1,Dimensions>::perform(center,temp,distance/len,v);
			}


		MFUNC3
			(void) _scaleAbsolute(const C0*center,C1*v,const C2&factor,count_t el_count)
			{
				C2 len=0;
				VECTOR_LOOP(el_count)
					len+=sqr(v[el]-center[el]);
				len = vsqrt(len);
				VECTOR_LOOP(el_count)
					v[el] = center[el]+(v[el]-center[el])*factor/len;
			}


		MFUNC3
			(void) _scaleAbsolute(const C0 center[3],C1 v[3],const C2&factor)
			{
				C1 len = vsqrt(sqr(v[0]-center[0])+sqr(v[1]-center[1])+sqr(v[2]-center[2]));
				v[0] = center[0]+(v[0]-center[0])*factor/len;
				v[1] = center[1]+(v[1]-center[1])*factor/len;
				v[2] = center[2]+(v[2]-center[2])*factor/len;
			}
			
		MFUNC3V
			(void)	_scaleAbsolute0V(const C0*center, C1*v, const C2&distance)
			{
				C2	temp[Dimensions];
				_subtractV<C0,C1,C2,Dimensions>(v,center,temp);
				C2 len=QDot<C2,C2,Dimensions>::perform(temp);
				len = vsqrt(len);
				if (len > GetError<C2>())
					QMad<C0,C2,C2,C1,Dimensions>::perform(center,temp,distance/len,v);
				else
					v[0] += distance;
			}


		MFUNC3
			(void) _scaleAbsolute0(const C0*center,C1*v,const C2&factor,count_t el_count)
			{
				C2 len=0;
				VECTOR_LOOP(el_count)
					len+=sqr(v[el]-center[el]);
					
				len = vsqrt(len);
				if (len > GetError<C2>())
				{
					VECTOR_LOOP(el_count)
						v[el] = center[el]+(v[el]-center[el])*factor/len;
				}
				else
					v[0] += factor;
			}


		MFUNC3
			(void) _scaleAbsolute0(const C0 center[3],C1 v[3],const C2&factor)
			{
				C1 len = vsqrt(sqr(v[0]-center[0])+sqr(v[1]-center[1])+sqr(v[2]-center[2]));
				
				if (len > GetError<C1>())
				{
					v[0] = center[0]+(v[0]-center[0])*factor/len;
					v[1] = center[1]+(v[1]-center[1])*factor/len;
					v[2] = center[2]+(v[2]-center[2])*factor/len;
				}
				else
					v[0] += factor;
			}


		MFUNC4V
			(void)	_interpolateV(const C0*v, const C1*w, const C2&factor, C3*out)
			{
				QInterpolate<C0,C1,C2,C3,Dimensions>::perform(v,w,factor,out);
			}


		MFUNC4
			(void) _interpolate(const C0*v,const C1*w, const C2&factor, C3*out,count_t el_count)
			{
				VECTOR_LOOP(el_count)
					out[el] = v[el]+(w[el]-v[el])*factor;
			}


		MFUNC4
			(void) _interpolate2(const C0 v[2],const C1 w[2], const C2&factor, C3 out[2])
			{
				out[0] = v[0]+(w[0]-v[0])*factor;
				out[1] = v[1]+(w[1]-v[1])*factor;
			}


		MFUNC4
			(void) _interpolate4(const C0 v[4],const C1 w[4], const C2&factor, C3 out[4])
			{
				out[0] = v[0]+(w[0]-v[0])*factor;
				out[1] = v[1]+(w[1]-v[1])*factor;
				out[2] = v[2]+(w[2]-v[2])*factor;
				out[3] = v[3]+(w[3]-v[3])*factor;
			}

		MFUNC4
			(void) _interpolate(const C0 v[3],const C1 w[3], const C2&factor, C3 out[3])
			{
				out[0] = v[0]+(w[0]-v[0])*factor;
				out[1] = v[1]+(w[1]-v[1])*factor;
				out[2] = v[2]+(w[2]-v[2])*factor;
			}


		MFUNC3
			(void)	_subtractValue2(const C0 v[2], const C1&value, C2 out[2])
			{
				out[0] = v[0]-value;
				out[1] = v[1]-value;
			}


		MFUNC3
			(void)	_subtractValue4(const C0 v[4], const C1&value, C2 out[4])
			{
				out[0] = v[0]-value;
				out[1] = v[1]-value;
				out[2] = v[2]-value;
				out[3] = v[3]-value;
			}


		MFUNC3V
			(void)	_subtractValueV(const C0*v, const C1&value, C2*out)
			{
				QSubVal<C0,C1,C2,Dimensions>::perform(v,value,out);
			}

			
		MFUNC3
			(void)	_subtractValue(const C0*v, const C1&value, C2*out,count_t el_count)
			{
				VECTOR_LOOP(el_count)
					out[el] = v[el]-value;
			}

		MFUNC3
			(void)	_subtractValue(const C0 v[3], const C1&value, C2 out[3])
			{
				out[0] = v[0]-value;
				out[1] = v[1]-value;
				out[2] = v[2]-value;
			}


		MFUNC2
			(void)	_subtractValue2(C0 v[2], const C1&value)
			{
				v[0] -= value;
				v[1] -= value;
			}


		MFUNC2
			(void)	_subtractValue4(C0 v[4], const C1&value)
			{
				v[0] -= value;
				v[1] -= value;
				v[2] -= value;
				v[3] -= value;
			}

		MFUNC2V
			(void)	_subtractValueV(C0*v, const C1&value)
			{
				QSubValDirect<C0,C1,Dimensions>::perform(v,value);
			}

		MFUNC2
			(void)	_subtractValue(C0*v, const C1&value,count_t el_count)
			{
				VECTOR_LOOP(el_count)
					v[el] -= value;
			}

		MFUNC2
			(void)	_subtractValue(C0 v[3], const C1&value)
			{
				v[0] -= value;
				v[1] -= value;
				v[2] -= value;
			}


		MFUNC3
			(void)	_addValue2(const C0 v[2], const C1&value, C2 out[2])
			{
				out[0] = (C1)(v[0] + value);
				out[1] = (C1)(v[1] + value);
			}


		MFUNC3
			(void)	_addValue4(const C0 v[4], const C1&value, C2 out[4])
			{
				out[0] = (C1)(v[0] + value);
				out[1] = (C1)(v[1] + value);
				out[2] = (C1)(v[2] + value);
				out[3] = (C1)(v[3] + value);
			}

		MFUNC3V
			(void)	_addValueV(const C0*v, const C1&value, C2*out)
			{
				QAddVal<C0,C1,C2,Dimensions>::perform(v,value,out);
			}


		MFUNC3
			(void) _addValue(const C0*v, const C1&value, C2*out, count_t el_count)
			{
				VECTOR_LOOP(el_count)
					out[el] = v[el]+value;
			}

		MFUNC3
			(void) _addValue(const C0 v[3], const C1&value, C2 out[3])
			{
				out[0] = (C1)(v[0] + value);
				out[1] = (C1)(v[1] + value);
				out[2] = (C1)(v[2] + value);
			}

		MFUNC2
			(void)	_addValue2(C0 v[2], const C1&value)
			{
				v[0] += value;
				v[1] += value;
			}

		MFUNC2
			(void)	_addValue4(C0 v[4], const C1&value)
			{
				v[0] += value;
				v[1] += value;
				v[2] += value;
				v[3] += value;
			}

		MFUNC2V
			(void)	_addValueV(C0*v, const C1&value)
			{
				QAddValDirect<C0,C1,Dimensions>::perform(v,value);
			}


		MFUNC2
			(void) _addValue(C0*v, const C1&value, count_t el_count)
			{
				VECTOR_LOOP(el_count)
					v[el]+=value;
			}


		MFUNC2
			(void) _addValue(C0 v[3], const C1&value)
			{
				v[0]+=value;
				v[1]+=value;
				v[2]+=value;
			}

			

		MFUNC3V
			(void)	_multiplyV(const C0*v, const C1&value, C2*out)
			{
				#ifndef __BORLANDC__
					QMult<C0,C1,C2,Dimensions>::perform(v,value,out);
				#else
					for (count_t i = 0; i < Dimensions; i++)
						out[i] = v[i] * value;
				#endif
			}


		MFUNC3
			(void)	_multiply(const C0*v, const C1&value, C2*out, count_t el_count)
			{
				VECTOR_LOOP(el_count)
					out[el] = v[el] * value;
			}


		MFUNC3
			(void)	_multiply(const C0 v[3], const C1&value, C2 out[3])
			{
				out[0] = (C2)(v[0]*value);
				out[1] = (C2)(v[1]*value);
				out[2] = (C2)(v[2]*value);
			}

		MFUNC3
			(void)		_multiply2(const C0 v[2], const C1&value, C2 out[2])
			{
				out[0] = (C2)(v[0]*value);
				out[1] = (C2)(v[1]*value);
			}


		MFUNC3
			(void)		_multiply4(const C0 v[4], const C1&value, C2 out[4])
			{
				out[0] = (C2)(v[0]*value);
				out[1] = (C2)(v[1]*value);
				out[2] = (C2)(v[2]*value);
				out[3] = (C2)(v[3]*value);
			}

		MFUNC2V
			(void)	_multiplyV(C0*v, const C1&value)
			{
				#ifndef __BORLANDC__
					QScale<C0,C1,Dimensions>::perform(v,value);
				#else
					for (count_t i = 0; i < Dimensions; i++)
						v[i]*=value;
				#endif
			}

			
		MFUNC2
			(void)	_multiply(C0*v, const C1&value, count_t el_count)
			{
				VECTOR_LOOP(el_count)
					v[el]*=value;
			}


		MFUNC2
			(void)		_multiply2(C0 v[2], const C1&value)
			{
				v[0]*=value;
				v[1]*=value;
			}


		MFUNC2
			(void)		_multiply4(C0 v[4], const C1&value)
			{
				v[0]*=value;
				v[1]*=value;
				v[2]*=value;
				v[3]*=value;
			}

		MFUNC2
			(void)	_multiply(C0 v[3], const C1&value)
			{
				v[0]*=value;
				v[1]*=value;
				v[2]*=value;
			}
			
			

		MFUNC6
			(void)	_resolveUCBS(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], const C4&t, C5 out[3])
			{
				C4	//i = 1-t,
					f0 = (-t*t*t+3*t*t-3*t+1)/6,
					f1 = (3*t*t*t-6*t*t+4)/6,
					f2 = (-3*t*t*t+3*t*t+3*t+1)/6,
					f3 = (t*t*t)/6;
					
				out[0] = p0[0]*f0 + p1[0]*f1 + p2[0]*f2 + p3[0]*f3;
				out[1] = p0[1]*f0 + p1[1]*f1 + p2[1]*f2 + p3[1]*f3;
				out[2] = p0[2]*f0 + p1[2]*f1 + p2[2]*f2 + p3[2]*f3;
			}
			

		MFUNC6
			(void)	_resolveUCBSaxis(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], const C4&t, C5 out[3])
			{
				C5	//i = 1-t,
					f0 = (t*t-2*t+1)/2,
					f1 = (-2*t*t+2*t+1)/2,
					f2 = (t*t)/2;
				
					
				out[0] = (p1[0]-p0[0])*f0 + (p2[0]-p1[0])*f1 + (p3[0]-p2[0])*f2;
				out[1] = (p1[1]-p0[1])*f0 + (p2[1]-p1[1])*f1 + (p3[1]-p2[1])*f2;
				out[2] = (p1[2]-p0[2])*f0 + (p2[2]-p1[2])*f1 + (p3[2]-p2[2])*f2;
			}
			


		MFUNC6
			(void)	_resolveSpline(const C0*p0, const C1*p1, const C2*p2, const C3*p3, const C4&t, C5*out, count_t el_count)
			{
				C4	i = 1-t;
				VECTOR_LOOP(el_count)
					out[el] = i*i*i*p0[el] + t*i*i*3*p1[el] + t*t*i*3*p2[el] + t*t*t*p3[el];
			}

		MFUNC6
			(void)	_resolveSpline(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], const C4&t, C5 out[3])
			{
				C4	i = 1-t,
					f3 = t*t*t,
					f2 = t*t*i*3,
					f1 = t*i*i*3,
					f0 = i*i*i;
				out[0] = p0[0]*f0 + p1[0]*f1 + p2[0]*f2 + p3[0]*f3;
				out[1] = p0[1]*f0 + p1[1]*f1 + p2[1]*f2 + p3[1]*f3;
				out[2] = p0[2]*f0 + p1[2]*f1 + p2[2]*f2 + p3[2]*f3;
			}

		MFUNC6
			(void)	_resolveSpline2(const C0 p0[2], const C1 p1[2], const C2 p2[2], const C3 p3[2], const C4&t, C5 out[2])
			{
				C4	i = 1-t,
					f3 = t*t*t,
					f2 = t*t*i*3,
					f1 = t*i*i*3,
					f0 = i*i*i;
				out[0] = p0[0]*f0 + p1[0]*f1 + p2[0]*f2 + p3[0]*f3;
				out[1] = p0[1]*f0 + p1[1]*f1 + p2[1]*f2 + p3[1]*f3;
			}

		MFUNC6
			(void)	_resolveSpline4(const C0 p0[4], const C1 p1[4], const C2 p2[4], const C3 p3[4], const C4&t, C5 out[4])
			{
				C4	i = 1-t,
					f3 = t*t*t,
					f2 = t*t*i*3,
					f1 = t*i*i*3,
					f0 = i*i*i;
				out[0] = p0[0]*f0 + p1[0]*f1 + p2[0]*f2 + p3[0]*f3;
				out[1] = p0[1]*f0 + p1[1]*f1 + p2[1]*f2 + p3[1]*f3;
				out[2] = p0[2]*f0 + p1[2]*f1 + p2[2]*f2 + p3[2]*f3;
				out[3] = p0[3]*f0 + p1[3]*f1 + p2[3]*f2 + p3[3]*f3;
			}

		MFUNC6V
			(void)	_resolveSplineV(const C0*p0, const C1*p1, const C2*p2, const C3*p3, const C4&t, C5*out)
			{
				C4	i = 1-t;
				_multiplyV<C0,C4,C5,Dimensions>(p3,t*t*t,out);
				_multAddV<C5,C1,C4,Dimensions>(out,p2,t*t*i*3);
				_multAddV<C5,C2,C4,Dimensions>(out,p1,t*i*i*3);
				_multAddV<C5,C3,C4,Dimensions>(out,p0,i*i*i);
			}

		MFUNC6
			(void)	_resolveSplineAxis(const C0*p0, const C1*p1, const C2*p2, const C3*p3, const C4&t, C5*out, count_t el_count)
			{
				C4 i = 1-t;
				C5 *temp = alloc<C5>(el_count);
				_subtract(p1,p0,temp,el_count);
				_multiply(temp,i*i,out,el_count);
				_subtract(p2,p1,temp,el_count);
				_multAdd(out,temp,t*i*2,el_count);
				_subtract(p3,p2,temp,el_count);
				_multAdd(out,temp,t*t,el_count);
				dealloc(temp);
				_normalize0(out);
			}

		MFUNC6
			(void)	_resolveSplineAxis(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], const C4&t, C5 out[3])
			{
				C4 i = 1-t;
				C5	temp[3];
				_subtract(p1,p0,temp);
				_multiply(temp,i*i,out);
				_subtract(p2,p1,temp);
				_multAdd(out,temp,t*i*2);
				_subtract(p3,p2,temp);
				_multAdd(out,temp,t*t);
			}

		MFUNC6
			(void)	_resolveSplineAxis2(const C0 p0[2], const C1 p1[2], const C2 p2[2], const C3 p3[2], const C4&t, C5 out[2])
			{
				C4 i = 1-t;
				C5	temp[2];
				_subtract2(p1,p0,temp);
				_multiply2(temp,i*i,out);
				_subtract2(p2,p1,temp);
				_multAdd2(out,temp,t*i*2);
				_subtract2(p3,p2,temp);
				_multAdd2(out,temp,t*t);
			}

		MFUNC6
			(void)	_resolveSplineAxis4(const C0 p0[4], const C1 p1[4], const C2 p2[4], const C3 p3[4], const C4&t, C5 out[4])
			{
				C4 i = 1-t;
				C5	temp[4];
				_subtract4(p1,p0,temp);
				_multiply4(temp,i*i,out);
				_subtract4(p2,p1,temp);
				_multAdd4(out,temp,t*i*2);
				_subtract4(p3,p2,temp);
				_multAdd4(out,temp,t*t);
			}

		MFUNC6V
			(void)	_resolveSplineAxisV(const C0*p0, const C1*p1, const C2*p2, const C3*p3, const C4&t, C5*out)
			{
				C4 i = 1-t;
				C5	temp[Dimensions];
				_subtractV<C0,C1,C5,Dimensions>(p1,p0,temp);
				_multiplyV<C5,C4,C5,Dimensions>(temp,i*i,out);
				_subtractV<C2,C1,C5,Dimensions>(p2,p1,temp);
				_multAddV<C5,C5,C4,Dimensions>(out,temp,t*i*2);
				_subtractV<C3,C2,C5,Dimensions>(p3,p2,temp);
				_multAddV<C5,C5,C4,Dimensions>(out,temp,t*t);
			}

		MFUNC3V
			(void)	_divideV(const C0*v, const C1&value, C2*out)
			{
				QDiv<C0,C1,C2,Dimensions>::perform(v,value,out);
			}

		MFUNC3
			(void) _divide(const C0*v, const C1&value, C2*out, count_t el_count)
			{
				VECTOR_LOOP(el_count)
					out[el] = (C2)(v[el]/value);
			}

		MFUNC3
			(void) _divide(const C0 v[3], const C1&value, C2 out[3])
			{
				out[0] = (C2)(v[0]/value);
				out[1] = (C2)(v[1]/value);
				out[2] = (C2)(v[2]/value);
			}

		MFUNC3
			(void) _divide2(const C0 v[2], const C1&value, C2 out[2])
			{
				out[0] = (C2)(v[0]/value);
				out[1] = (C2)(v[1]/value);
			}

		MFUNC3
			(void) _divide4(const C0 v[4], const C1&value, C2 out[4])
			{
				out[0] = (C2)(v[0]/value);
				out[1] = (C2)(v[1]/value);
				out[2] = (C2)(v[2]/value);
				out[3] = (C2)(v[3]/value);
			}

		MFUNC2V
			(void)	_divideV(C0*v, const C1&value)
			{
				QDivDirect<C0,C1,Dimensions>::perform(v,value);
			}

		MFUNC2
			(void) _divide(C0*v, const C1&value, count_t el_count)
			{
				VECTOR_LOOP(el_count)
					v[el]/=value;
			}

		MFUNC2
			(void) _divide(C0 v[3], const C1&value)
			{
				v[0]/=value;
				v[1]/=value;
				v[2]/=value;
			}

		MFUNC2
			(void) _divide2(C0 v[2], const C1&value)
			{
				v[0]/=value;
				v[1]/=value;
			}

		MFUNC2
			(void) _divide4(C0 v[4], const C1&value)
			{
				v[0]/=value;
				v[1]/=value;
				v[2]/=value;
				v[3]/=value;
			}

		MFUNC3V
			(bool)	_similarV(const C0*v, const C1*w, const C2&distance)
			{
				C0 temp[Dimensions];
				_subtractV<C0,C1,C0,Dimensions>(v,w,temp);
				return QDot<C0,C1,Dimensions>::perform(temp,temp)<distance*distance;
			}

		MFUNC3
			(bool) _similar(const C0*v, const C1*w, const C2&distance, count_t el_count)
			{
				C2 d = sqr(v[0]-w[0]);
				for (count_t el = 1; el < el_count; el++)
					d+=sqr(v[el]-w[el]);
				return d < distance*distance;
			}

		MFUNC3
			(bool) _similar(const C0 v[3], const C1 w[3], const C2&distance)
			{
				return sqr(v[0]-w[0])+sqr(v[1]-w[1])+sqr(v[2]-w[2]) < distance*distance;
			}

		MFUNC3
			(bool) _similar2(const C0 v[2], const C1 w[2], const C2&distance)
			{
				return sqr(v[0]-w[0])+sqr(v[1]-w[1]) < distance*distance;
			}

		MFUNC3
			(bool) _similar4(const C0 v[4], const C1 w[4], const C2&distance)
			{
				return sqr(v[0]-w[0])+sqr(v[1]-w[1])+sqr(v[2]-w[2])+sqr(v[3]-w[3]) < distance*distance;
			}

		MFUNC2
			(C0) _angle360(const C0&x, const C1&y)
			{
				return _angleOne(x,y)*360;
			}

		MFUNC2
			(C0)	_angle2PI(const C0&x, const C1&y)
			{
				return _angleOne(x,y)*2*M_PI;
			}


		MFUNC
			(C)	_angleOne(const C v[2])
			{
				return _angleOne(v[0],v[1]);
			}

		MFUNC
			(C)		_angle2PI(const C v[2])
			{
				return _angleOne(v)*2*M_PI;
			}

		MFUNC2
			(C0)		_angleOne(const C0&x_, const C1&y_)
			{
				if (!x_ && !y_)
					return 0;
				C0 x,y;
				BYTE sector;
				if (x_>=0 && y_>=0)
				{
					x = (C0)x_;
					y = (C0)y_;
					sector = 0;
				}
				else
					if (x_ > 0)
					{
						x = (C0)-y_;
						y = (C0)x_;
						sector = 1;
					}
					else
						if (y_ >= 0)
						{
							x = (C0)y_;
							y = (C0)-x_;
							sector = 3;
						}
						else
						{
							x = (C0)-x_;
							y = (C0)-y_;
							sector = 2;
						}
				C0 d = (C0)(vatan2(x,y)/M_PI_2);
				return (d+sector)/4;
			}

		MFUNC2
			(void) _set(C0*v, const C1&value, count_t el_count)
			{
				VECTOR_LOOP(el_count)
					v[el] = value;
			}

		MFUNC2V
			(void) _setV(C0*v, const C1&value)
			{
				QSet<C0,C1,Dimensions>::perform(v,value);
			}

		MFUNC2
			(void) _set(C0 v[3], const C1&value)
			{
				v[0] = (C0)value;
				v[1] = (C0)value;
				v[2] = (C0)value;
			}

		MFUNC2
			(void) _set4(C0 v[4], const C1&value)
			{
				v[0] = (C0)value;
				v[1] = (C0)value;
				v[2] = (C0)value;
				v[3] = (C0)value;
			}

		MFUNC2
			(void) _set6(C0 v[6], const C1&value)
			{
				v[0] = (C0)value;
				v[1] = (C0)value;
				v[2] = (C0)value;
				v[3] = (C0)value;
				v[4] = (C0)value;
				v[5] = (C0)value;
			}

		MFUNC2
			(void) _set9(C0 v[9], const C1&value)
			{
				v[0] = (C0)value;
				v[1] = (C0)value;
				v[2] = (C0)value;
				v[3] = (C0)value;
				v[4] = (C0)value;
				v[5] = (C0)value;
				v[6] = (C0)value;
				v[7] = (C0)value;
				v[8] = (C0)value;
			}

		MFUNC2
			(void) _set16(C0 v[16], const C1&value)
			{
				v[0] = (C0)value;
				v[1] = (C0)value;
				v[2] = (C0)value;
				v[3] = (C0)value;
				v[4] = (C0)value;
				v[5] = (C0)value;
				v[6] = (C0)value;
				v[7] = (C0)value;
				v[8] = (C0)value;
				v[9] = (C0)value;
				v[10] = (C0)value;
				v[11] = (C0)value;
				v[12] = (C0)value;
				v[13] = (C0)value;
				v[14] = (C0)value;
				v[15] = (C0)value;
			}

		MFUNC2
			(void) _set2(C0 v[2], const C1&value)
			{
				v[0] = (C0)value;
				v[1] = (C0)value;
			}

		MFUNC3
			(void)		_clamp(C0*v, const C1&min, const C2&max, count_t el_count)
			{
				VECTOR_LOOP(el_count)
					v[el] = clamped(v[el],min,max);
			}

		MFUNC3
			(void)		_clamp(C0 v[3], const C1&min, const C2&max)
			{
				v[0] = clamped(v[0],min,max);
				v[1] = clamped(v[1],min,max);
				v[2] = clamped(v[2],min,max);
			}

		MFUNC3
			(void)		_clamp2(C0 v[2], const C1&min, const C2&max)
			{
				v[0] = clamped(v[0],min,max);
				v[1] = clamped(v[1],min,max);
			}

		MFUNC3
			(void)		_clamp4(C0 v[4], const C1&min, const C2&max)
			{
				v[0] = clamped(v[0],min,max);
				v[1] = clamped(v[1],min,max);
				v[2] = clamped(v[2],min,max);
				v[3] = clamped(v[3],min,max);
			}

		MFUNC3V
			(void)		_clampV(C0*v, const C1&min, const C2&max)
			{
				#ifndef __BORLANDC__
					QClamp<C0,C1,C2,Dimensions>::perform(v,min,max);
				#else
					for (count_t i = 0; i < Dimensions; i++)
						v[i] = clamped(v[i],min,max);
				#endif
			}

		MFUNC4
			(void)		_sphereCoords(const C0&pangle, const C1&hangle, const C2&radius, C3 out[3])
			{
				out[1] = vsin(hangle*M_PI/180)*radius;
				C3	r2 = vcos(hangle*M_PI/180)*radius;
				out[0] = vcos(pangle*M_PI/180)*r2;
				out[2] = vsin(pangle*M_PI/180)*r2;
			}

		MFUNC3
			(void)		_sphereCoordsRad(const C0&pangle, const C1&hangle, C2 out[3])
			{
				out[1] = vsin(hangle);
				C2	r2 = vcos(hangle);
				out[0] = vcos(pangle)*r2;
				out[2] = vsin(pangle)*r2;
			}

		MFUNC3
			(void)	_v2(C0 v[2], const C1&x, const C2&y)
			{
				v[0] = (C0)x;
				v[1] = (C0)y;
			}

		MFUNC4
			(void)	_v3(C0 v[3], const C1&x, const C2&y, const C3&z)
			{
				v[0] = (C0)x;
				v[1] = (C0)y;
				v[2] = (C0)z;
			}

		MFUNC5
			(void)	_v4(C0 v[4], const C1&x, const C2&y, const C3&z, const C4&a)
			{
				v[0] = (C0)x;
				v[1] = (C0)y;
				v[2] = (C0)z;
				v[3] = (C0)a;
			}
	}
	

	MFUNC3
		(char)		_compare(const C0 v0[3], const C1 v1[3], C2 tolerance)
		{

			C0 delta;

			COMPARE(v0[0],v1[0])
			COMPARE(v0[1],v1[1])
			COMPARE(v0[2],v1[2])

			return 0;
		}

	MFUNC3
		(char)		_compare2(const C0 v0[2], const C1 v1[2], C2 tolerance)
		{
			C0	delta;

			COMPARE(v0[0],v1[0])
			COMPARE(v0[1],v1[1])

			return 0;
		}

	MFUNC3
		(char)		_compare4(const C0 v0[4], const C1 v1[4], C2 tolerance)
		{
			C0	delta;

			COMPARE(v0[0],v1[0])
			COMPARE(v0[1],v1[1])
			COMPARE(v0[2],v1[2])
			COMPARE(v0[3],v1[3])

			return 0;
		}

	MFUNC3
		(char)		_compare6(const C0 v0[6], const C1 v1[6], C2 tolerance)
		{
			C0	delta;

			COMPARE(v0[0],v1[0])
			COMPARE(v0[1],v1[1])
			COMPARE(v0[2],v1[2])
			COMPARE(v0[3],v1[3])
			COMPARE(v0[4],v1[4])
			COMPARE(v0[5],v1[5])

			return 0;
		}

	MFUNC3
		(char)		_compare8(const C0 v0[8], const C1 v1[8], C2 tolerance)
		{
			C0	delta;

			COMPARE(v0[0],v1[0])
			COMPARE(v0[1],v1[1])
			COMPARE(v0[2],v1[2])
			COMPARE(v0[3],v1[3])
			COMPARE(v0[4],v1[4])
			COMPARE(v0[5],v1[5])
			COMPARE(v0[6],v1[6])
			COMPARE(v0[7],v1[7])

			return 0;
		}

	MFUNC3V
		(char)		_compareV(const C0*v0, const C1*v1, C2 tolerance)
		{
			VECTOR_LOOP(Elements)
			{
				C0 delta = v0[el]-v1[el];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	MFUNC2 (char)		_compare(const C0 v0[3], const C1 v1[3])
	{
		C0	tolerance = GetError<C0>(),
			delta;

		COMPARE(v0[0],v1[0])
		COMPARE(v0[1],v1[1])
		COMPARE(v0[2],v1[2])

		return 0;
	}

	MFUNC2 (char)		_compare2(const C0 v0[2], const C1 v1[2])
	{
		C0	tolerance = GetError<C0>(),
			delta;

		COMPARE(v0[0],v1[0])
		COMPARE(v0[1],v1[1])

		return 0;
	}


	MFUNC2 (char)		_compare4(const C0 v0[4], const C1 v1[4])
	{
		C0	tolerance = GetError<C0>(),
			delta;

		COMPARE(v0[0],v1[0])
		COMPARE(v0[1],v1[1])
		COMPARE(v0[2],v1[2])
		COMPARE(v0[3],v1[3])

		return 0;

	}

	MFUNC2 (char)		_compare6(const C0 v0[6], const C1 v1[6])
	{
		C0	tolerance = GetError<C0>(),
			delta;

		COMPARE(v0[0],v1[0])
		COMPARE(v0[1],v1[1])
		COMPARE(v0[2],v1[2])
		COMPARE(v0[3],v1[3])
		COMPARE(v0[4],v1[4])
		COMPARE(v0[5],v1[5])

		return 0;

	}

	MFUNC2 (char)		_compare8(const C0 v0[8], const C1 v1[8])
	{
		C0	tolerance = GetError<C0>(),
			delta;

		COMPARE(v0[0],v1[0])
		COMPARE(v0[1],v1[1])
		COMPARE(v0[2],v1[2])
		COMPARE(v0[3],v1[3])
		COMPARE(v0[4],v1[4])
		COMPARE(v0[5],v1[5])
		COMPARE(v0[6],v1[6])
		COMPARE(v0[7],v1[7])

		return 0;

	}

	MFUNC2V (char)		_compareV(const C0*v0, const C1*v1)
	{
		return _compareV<C0,C1,Elements>(v0,v1,GetError<C0>());
	}

	#undef COMPARE



	MFUNC (String) _toString(const C*v, count_t el_count)
	{
		String rs = " (";
		rs.Set(0,vChar<C>());

		if (el_count)
		{
			if (!v)//Interpret first 128 elements following NULL as NULL, too (for nested vectors of NULL structures). seems unnecessary: the compiler changes those pointers to NULL automatically. interesting.
				rs += "NULL";
			else
			{
				rs+=v2str(v[0]);
				for (index_t i = 1; i < el_count; i++)
				{
					rs+=", "+v2str(v[i]);
				}
			}
		}
		rs+=")";
		return rs;
	}



	MFUNC2 (void)	_copy3(const C0 origin[3],C1 destination[3])
	{
		destination[0] = (C1)origin[0];
		destination[1] = (C1)origin[1];
		destination[2] = (C1)origin[2];
	}

	MFUNC2 (void)	_copy2(const C0 origin[2],C1 destination[2])
	{
		destination[0] = (C1)origin[0];
		destination[1] = (C1)origin[1];
	}

	MFUNC2 (void)	_copy4(const C0 origin[4],C1 destination[4])
	{
		destination[0] = (C1)origin[0];
		destination[1] = (C1)origin[1];
		destination[2] = (C1)origin[2];
		destination[3] = (C1)origin[3];
	}

	MFUNC2 (void)	_copy6(const C0 origin[6],C1 destination[6])
	{
		destination[0] = (C1)origin[0];
		destination[1] = (C1)origin[1];
		destination[2] = (C1)origin[2];
		destination[3] = (C1)origin[3];
		destination[4] = (C1)origin[4];
		destination[5] = (C1)origin[5];
	}

	MFUNC2 (void)	_copy8(const C0 origin[8],C1 destination[8])
	{
		destination[0] = (C1)origin[0];
		destination[1] = (C1)origin[1];
		destination[2] = (C1)origin[2];
		destination[3] = (C1)origin[3];
		destination[4] = (C1)origin[4];
		destination[5] = (C1)origin[5];
		destination[6] = (C1)origin[6];
		destination[7] = (C1)origin[7];
	}




	MFUNC2V (void)	_copyV(const C0*origin, C1*destination)
	{
		#ifndef __BORLANDC__
			QCopy<C0,C1,Dimensions>::perform(origin,destination);
		#else
			_copy(origin,destination,Dimensions);
		#endif
	}

	MFUNC2 (void)	_copy(const C0*origin,C1*destination,count_t el_count)
	{
		VECTOR_LOOP(el_count)
			destination[el] = (C1)origin[el];
	}



	MFUNC2V (bool)	_allGreaterV(const C0*v, const C1*w)
	{
		#ifndef __BORLANDC__
			return QAllGreater<C0,C1,Dimensions>::perform(v,w);
		#else
			return _allGreater(v,w,Dimensions);
		#endif
	}

	MFUNC2 (bool) _allGreater(const C0*v,const C1*w, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			if (v[el] < w[el])
				return false;
		return true;
	}

	MFUNC2 (bool) _allGreater(const C0*v,const C1*w)
	{
		return v[0] >= w[0] && v[1] >= w[1] && v[2] >= w[2];
	}


	MFUNC2V (bool)	_allLessV(const C0*v, const C1*w)
	{
		#ifndef __BORLANDC__
			return QAllLess<C0,C1,Dimensions>::perform(v,w);
		#else
			return _allLess(v,w,Dimensions);
		#endif
	}

	MFUNC2 (bool) _allLess(const C0*v,const C1*w, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			if (v[el] > w[el])
				return false;
		return true;
	}

	MFUNC2 (bool) _allLess(const C0*v,const C1*w)
	{
		return v[0] <= w[0] && v[1] <= w[1] && v[2] <= w[2];
	}


	MFUNC2V (bool)	_oneGreaterV(const C0*v, const C1*w)
	{
		#ifndef __BORLANDC__
			return QOneGreater<C0,C1,Dimensions>::perform(v,w);
		#else
			return _oneGreater(v,w,Dimensions);
		#endif
	}

	MFUNC2 (bool) _oneGreater(const C0*v, const C1*w, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			if (v[el] >= w[el])
				return true;
		return false;
	}

	MFUNC2 (bool) _oneGreater(const C0 v[3], const C1 w[3])
	{
		return v[0] >= w[0] || v[1] >= w[1] || v[2] >= w[2];
	}


	MFUNC2V (bool)	_oneLessV(const C0*v, const C1*w)
	{
		#ifndef __BORLANDC__
			return QOneLess<C0,C1,Dimensions>::perform(v,w);
		#else
			return _oneLess(v,w,Dimensions);
		#endif
	}

	MFUNC2 (bool) _oneLess(const C0*v, const C1*w, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			if (v[el] <= w[el])
				return true;
		return false;
	}

	MFUNC2 (bool) _oneLess(const C0 v[3], const C1 w[3])
	{
		return v[0] <= w[0] || v[1] <= w[1] || v[2] <= w[2];
	}


	MFUNC3
		(void)		_max(const C0 v[3], const C1 w[3], C2 out[3])
		{
			out[0] = vmax(v[0],w[0]);
			out[1] = vmax(v[1],w[1]);
			out[2] = vmax(v[2],w[2]);
		}
	
	MFUNC3
		(void)		_min(const C0 v[3], const C1 w[3], C2 out[3])
		{
			out[0] = vmin(v[0],w[0]);
			out[1] = vmin(v[1],w[1]);
			out[2] = vmin(v[2],w[2]);
		}
	
	MFUNC
		(const C&)	_min(const C v[3])
		{
			if (v[0] < v[1])
			{
				if (v[0] < v[2])
					return v[0];
				else
					return v[2];
			}
			else
				if (v[1] < v[2])
					return v[1];
				else
					return v[2];
		}
	
	MFUNC
		(const C&)	_max(const C v[3])
		{
			if (v[0] > v[1])
			{
				if (v[0] > v[2])
					return v[0];
				else
					return v[2];
			}
			else
				if (v[1] > v[2])
					return v[1];
				else
					return v[2];
		}
	

	
	MFUNCV (C)		_greatestV(const C*v)
	{
		#ifndef __BORLANDC__
			return QGreatest<C,Dimensions>::perform(v);
		#else
			return _greatest(v,Dimensions);
		#endif
	}


	MFUNC (C)	_greatest(const C*v, count_t el_count)
	{
		if (!el_count)
			return (C)0;
		C rs = v[0];
		for (count_t el = 1; el < el_count; el++)
			if (v[el] > rs)
				rs = v[el];
		return rs;
	}

	MFUNC (C)	_greatest(const C v[3])
	{
		C rs = v[0];
		if (v[1] > rs)
			rs = v[1];
		if (v[2] > rs)
			rs = v[2];
		return rs;
	}

	MFUNC (C)	_greatest2(const C v[2])
	{
		return v[0] < v[1] ? v[1] : v[0];
	}

	MFUNC (C)	_greatest4(const C v[4])
	{
		C rs = v[0];
		if (v[1] > rs)
			rs = v[1];
		if (v[2] > rs)
			rs = v[2];
		if (v[3] > rs)
			rs = v[3];
		return rs;
	}


	MFUNCV (C)		_leastV(const C*v)
	{
		return QLeast<C,Dimensions>::perform(v);
	}

	MFUNC (C)	_least(const C*v, count_t el_count)
	{
		C rs = v[0];
		for (count_t el = 1; el < el_count; el++)
			if (v[el] < rs)
				rs = v[el];
		return rs;
	}

	MFUNC (C)	_least(const C v[3])
	{
		C rs = v[0];
		if (v[1] < rs)
			rs = v[1];
		if (v[2] < rs)
			rs = v[2];
		return rs;
	}

	MFUNC (C)	_least2(const C v[2])
	{
		return v[0] < v[1] ? v[0] : v[1];
	}

	MFUNC (C)	_least4(const C v[4])
	{
		C rs = v[0];
		if (v[1] < rs)
			rs = v[1];
		if (v[2] < rs)
			rs = v[2];
		if (v[3] < rs)
			rs = v[3];
		return rs;
	}




	MFUNC3 (void)	_cross(const C0 v[3], const C1 w[3], C2 out[3])
	{
		out[0] = v[1]*w[2] - v[2]*w[1];
		out[1] = v[2]*w[0] - v[0]*w[2];
		out[2] = v[0]*w[1] - v[1]*w[0];
	}

	MFUNC3 (C0)		_crossDot(const C0 u[3], const C1 v[3], const C2 w[3])
	{
		return	(u[1]*v[2] - u[2]*v[1]) * w[0]
				+(u[2]*v[0] - u[0]*v[2]) * w[1]
				+(u[0]*v[1] - u[1]*v[0]) * w[2];
	}

	MFUNC2 (void)		_crossVertical(const C0 v[3], C1 out[3])
	{
		out[0] = -v[2];
		out[1] = 0;
		out[2] = v[0];
	}

	MFUNC2 (void)		_crossZ(const C0 v[3], C1 out[3])
	{
		out[0] = v[1];
		out[1] = -v[0];
		out[2] = 0;
	}





	MFUNC3
		(void)		_multAdd(C0*b, const C1*d, C2 scalar, count_t el_count)
		{
			VECTOR_LOOP(el_count)
				b[el]+=d[el]*scalar;
		}

	MFUNC3
		(void)		_multAdd(C0 b[3], const C1 d[3], C2 scalar)
		{
			b[0]+=d[0]*scalar;
			b[1]+=d[1]*scalar;
			b[2]+=d[2]*scalar;
		}

	MFUNC3
		(void)		_multAdd2(C0 b[2], const C1 d[2], C2 scalar)
		{
			b[0]+=d[0]*scalar;
			b[1]+=d[1]*scalar;
		}

	MFUNC3
		(void)		_multAdd4(C0 b[4], const C1 d[4], C2 scalar)
		{
			b[0]+=d[0]*scalar;
			b[1]+=d[1]*scalar;
			b[2]+=d[2]*scalar;
			b[3]+=d[3]*scalar;
		}

	MFUNC3V
		(void)		_multAddV(C0*b, const C1*d,C2 scalar)
		{
			QMadDirect<C0,C1,C2,Dimensions>::perform(b,d,scalar);
		}


	MFUNC4V
		(void)	_multAddV(const C0*b, const C1*d,C2 scalar, C3*out)
		{
			QMad<C0,C1,C2,C3,Dimensions>::perform(b,d,scalar,out);
		}

	MFUNC4
		(void)	_multAdd(const C0*b, const C1*d, C2 scalar, C3*out,count_t el_count)
		{
			VECTOR_LOOP(el_count)
				out[el] = b[el] + d[el] * scalar;
		}

	MFUNC4
		(void)	_multAdd(const C0 b[3], const C1 d[3], C2 scalar, C3 out[3])
		{
			out[0] = b[0] + d[0] * scalar;
			out[1] = b[1] + d[1] * scalar;
			out[2] = b[2] + d[2] * scalar;
		}

	MFUNC4
		(void)	_multAdd2(const C0 b[2], const C1 d[2], C2 scalar, C3 out[2])
		{
			out[0] = b[0] + d[0] * scalar;
			out[1] = b[1] + d[1] * scalar;
		}

	MFUNC4
		(void)	_multAdd4(const C0 b[4], const C1 d[4], C2 scalar, C3 out[4])
		{
			out[0] = b[0] + d[0] * scalar;
			out[1] = b[1] + d[1] * scalar;
			out[2] = b[2] + d[2] * scalar;
			out[3] = b[3] + d[3] * scalar;
		}

	MFUNC4
		(void)	_add2(const C0 u[2], const C1 v[2], const C2 w[2], C3 out[2])
		{
			out[0] = u[0]+v[0]+w[0];
			out[1] = u[1]+v[1]+w[1];
		}

	MFUNC4 (void)	_add4(const C0 u[4], const C1 v[4], const C2 w[4], C3 out[4])
	{
		out[0] = u[0]+v[0]+w[0];
		out[1] = u[1]+v[1]+w[1];
		out[2] = u[2]+v[2]+w[2];
		out[3] = u[3]+v[3]+w[3];
	}


	MFUNC4V (void)	_addV(const C0*u, const C1*v, const C2*w, C3*out)
	{
		QAdd3<C0,C1,C2,C3,Dimensions>::perform(u,v,w,out);
	}

	MFUNC4 (void) _add(const C0*u, const C1*v, const C2*w, C3*out, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			out[el] = u[el]+v[el]+w[el];
	}

	MFUNC4 (void) _add(const C0 u[3], const C1 v[3], const C2 w[3], C3 out[3])
	{
		out[0] = u[0]+v[0]+w[0];
		out[1] = u[1]+v[1]+w[1];
		out[2] = u[2]+v[2]+w[2];
	}

	MFUNCV (bool)	_zeroV(const C*v)
	{
		return QZero<C,Dimensions>::perform(v);
	}

	MFUNC (bool) _zero(const C*v, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			if (v[el])
				return false;
		return true;
	}

	MFUNC (bool) _zero(const C v[3])
	{
		return !v[0] && !v[1] && !v[2];
	}

	MFUNC (bool) _zero4(const C v[4])
	{
		return !v[0] && !v[1] && !v[2] && !v[3];
	}

	MFUNC (bool) _zero2(const C v[2])
	{
		return !v[0] && !v[1];
	}




	MFUNCV (C)		_lengthV(const C*v)
	{
		C	len = QDot<C,C,Dimensions>::perform(v,v);
		return vsqrt(len);
	}

	MFUNC (C)	_length(const C*v, count_t el_count)
	{
		C len = 0;
		VECTOR_LOOP(el_count)
			len+=v[el]*v[el];
		return vsqrt(len);
	}

	MFUNC (C)	_length(const C v[3])
	{
		return vsqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	}

	MFUNC (C)		_length2(const C v[2])
	{
		return vsqrt(v[0]*v[0]+v[1]*v[1]);
	}

	MFUNC (C)		_length4(const C v[4])
	{
		return vsqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]+v[3]*v[3]);
	}


	MFUNC2V
		(void)	_setLenV(C0*v, C1 alength)
		{
			C0 len = QDot<C0,C0,Dimensions>::perform(v,v);
			len = vsqrt(len);
			QScale<C0,C1,Dimensions>::perform(v,(C0)alength/len);
		}

	MFUNC2
		(void) _setLen(C0*v, C1 alength, count_t el_count)
		{
			C0 len = 0;
			VECTOR_LOOP(el_count)
				len+=v[el]*v[el];
			len = (C0)alength/vsqrt(len);
			VECTOR_LOOP(el_count)
				v[el]*=len;
		}

	MFUNC2
		(void) _setLen(C0 v[3], C1 alength)
		{
			C0 factor = (C0)alength/vsqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
			v[0]*=factor;
			v[1]*=factor;
			v[2]*=factor;
		}

	MFUNC2
		(void) _setLen2(C0 v[2], C1 alength)
		{
			C0 factor = (C0)alength/vsqrt(v[0]*v[0]+v[1]*v[1]);
			v[0]*=factor;
			v[1]*=factor;
		}

	MFUNC2
		(void) _setLen4(C0 v[4], C1 alength)
		{
			C0 factor = (C0)alength/vsqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]+v[3]*v[3]);
			v[0]*=factor;
			v[1]*=factor;
			v[2]*=factor;
			v[3]*=factor;
		}

	MFUNC2
		(void)		_setLen0(C0*v, C1 alength, count_t el_count)
		{
			C0 len = 0;
			VECTOR_LOOP(el_count)
				len+=v[el]*v[el];
			if (len <= GetError<C0>()*GetError<C0>())
				v[0] = alength;
			else
			{
				len = (C0)alength/vsqrt(len);
				VECTOR_LOOP(el_count)
					v[el]*=len;
			}
		}

	MFUNC2
		(void)		_setLen0(C0 v[3], C1 alength)
		{
			C0 val = v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
			if (val <= GetError<C0>()*GetError<C0>())
				v[0] = alength;
			else
			{
				val = (C0)alength/vsqrt(val);
				v[0] *= val;
				v[1] *= val;
				v[2] *= val;
			}
		}

	MFUNC2
		(void)		_setLen02(C0 v[2], C1 alength)
		{
			C0 val = v[0]*v[0]+v[1]*v[1];
			if (val <= GetError<C0>()*GetError<C0>())
				v[0] = alength;
			else
			{
				val = (C0)alength/vsqrt(val);
				v[0] *= val;
				v[1] *= val;
			}
		}
	MFUNC2
		(void)		_setLen04(C0 v[4], C1 alength)
		{
			C0 val = v[0]*v[0]+v[1]*v[1]+v[2]*v[2]+v[3]*v[3];
			if (val <= GetError<C0>()*GetError<C0>())
				v[0] = alength;
			else
			{
				val = (C0)alength/vsqrt(val);
				v[0] *= val;
				v[1] *= val;
				v[2] *= val;
				v[3] *= val;
			}
		}

	MFUNC2V
		(void)		_setLen0V(C0*v, C1 alength)
		{
			C0 len = QDot<C0,C0,Dimensions>::perform(v,v);
			if (len <= GetError<C0>()*GetError<C0>())
				v[0] = alength;
			else
				QScale<C0,C1,Dimensions>::perform(v,(C0)alength/vsqrt(len));
		}


	MFUNC4 (void)		_reflectN(const C0*base, const C1*normal, const C2*p, C3*out, count_t el_count)
	{
		_mad(p,normal,2*(_dot(base,normal,el_count)-_dot(p,normal,el_count)),out,el_count);
	}

	MFUNC4 (void)		_reflectN(const C0 base[3], const C1 normal[3], const C2 p[3], C3 out[3])
	{
		_mad(p,normal,2*(_dot(base,normal)-_dot(p,normal)),out);
	}

	MFUNC4 (void)		_reflectN2(const C0 base[2], const C1 normal[2], const C2 p[2], C3*out[2])
	{
		_mad2(p,normal,2*(_dot2(base,normal)-_dot2(p,normal)),out);
	}

	MFUNC4 (void)		_reflectN4(const C0 base[4], const C1 normal[4], const C2 p[4], C3 out[4])
	{
		_mad4(p,normal,2*(_dot4(base,normal)-_dot4(p,normal)),out);
	}

	MFUNC4V (void)		_reflectNV(const C0*base, const C1*normal, const C2*p, C3*out)
	{
		_madV<C2,C1,C0,Dimensions>(p,normal,2*(_dotV<C0,C1,Dimensions>(base,normal)-_dotV<C2,C1,Dimensions>(p,normal)),out);
	}


	MFUNC4 (void)		_reflect(const C0*base, const C1*normal, const C2*p, C3*out, count_t el_count)
	{
		_mad(p,normal,2*(_dot(base,normal,el_count)-_dot(p,normal,el_count))/_dot(normal,el_count),out,el_count);
	}

	MFUNC4 (void)		_reflect(const C0 base[3], const C1 normal[3], const C2 p[3], C3 out[3])
	{
		_mad(p,normal,2*(_dot(base,normal)-_dot(p,normal))/_dot(normal),out);
	}

	MFUNC4 (void)		_reflect2(const C0 base[2], const C1 normal[2], const C2 p[2], C3*out[2])
	{
		_mad2(p,normal,2*(_dot2(base,normal)-_dot2(p,normal))/_dot2(normal),out);
	}

	MFUNC4 (void)		_reflect4(const C0 base[4], const C1 normal[4], const C2 p[4], C3 out[4])
	{
		_mad4(p,normal,2*(_dot4(base,normal)-_dot4(p,normal))/_dot4(normal),out);
	}

	MFUNC4V (void)		_reflectV(const C0*base, const C1*normal, const C2*p, C3*out)
	{
		_madV<C2,C1,C0,Dimensions>(p,normal,2*(_dotV<C0,C1,Dimensions>(base,normal)-_dotV<C2,C1,Dimensions>(p,normal))/_dotV<C1,Dimensions>(normal),out);
	}

	MFUNC3 (void)		_reflect(const C0*base, const C1*normal, C2*p, count_t el_count)
	{
		_mad(p,normal,2*(_dot(base,normal,el_count)-_dot(p,normal,el_count))/_dot(normal,el_count));
	}

	MFUNC3 (void)		_reflect(const C0 base[3], const C1 normal[3], C2 p[3])
	{
		_mad(p,normal,2*(_dot(base,normal)-_dot(p,normal))/_dot(normal));
	}

	MFUNC3 (void)		_reflect2(const C0 base[2], const C1 normal[2], C2 p[2])
	{
		_mad2(p,normal,2*(_dot2(base,normal)-_dot2(p,normal))/_dot2(normal));
	}

	MFUNC3 (void)		_reflect4(const C0 base[4], const C1 normal[4], C2 p[4])
	{
		_mad4(p,normal,2*(_dot4(base,normal)-_dot4(p,normal))/_dot4(normal));
	}

	MFUNC3V (void)		_reflectV(const C0*base, const C1*normal, C2*p)
	{
		_madV<C2,C1,Dimensions>(p,normal,2*(_dotV<C0,C1,Dimensions>(base,normal)-_dotV<C2,C1,Dimensions>(p,normal))/_dotV<C1,Dimensions>(normal));
	}


	MFUNC3 (void)		_reflectN(const C0*base, const C1*normal, C2*p, count_t el_count)
	{
		_mad(p,normal,2*(_dot(base,normal,el_count)-_dot(p,normal,el_count)));
	}

	MFUNC3 (void)		_reflectN(const C0 base[3], const C1 normal[3], C2 p[3])
	{
		_mad(p,normal,2*(_dot(base,normal)-_dot(p,normal)));
	}

	MFUNC3 (void)		_reflectN2(const C0 base[2], const C1 normal[2], C2 p[2])
	{
		_mad2(p,normal,2*(_dot2(base,normal)-_dot2(p,normal)));
	}

	MFUNC3 (void)		_reflectN4(const C0 base[4], const C1 normal[4], C2 p[4])
	{
		_mad4(p,normal,2*(_dot4(base,normal)-_dot4(p,normal)));
	}

	MFUNC3V (void)		_reflectNV(const C0*base, const C1*normal, C2*p)
	{
		_madV<C2,C1,Dimensions>(p,normal,2*(_dotV<C0,C1,Dimensions>(base,normal)-_dotV<C2,C1,Dimensions>(p,normal)));
	}







	MFUNC3 (void)		_reflectVectorN(const C0*normal, const C1*v, C2*out, count_t el_count)
	{
		_mad(v,normal,2*(-_dot(v,normal,el_count)),out,el_count);
	}

	MFUNC3 (void)		_reflectVectorN(const C0 normal[3], const C1 v[3], C2 out[3])
	{
		_mad(v,normal,2*(-_dot(v,normal)),out);
	}

	MFUNC3 (void)		_reflectVectorN2(const C0 normal[2], const C1 v[2], C2*out[2])
	{
		_mad2(v,normal,2*(-_dot2(v,normal)),out);
	}

	MFUNC3 (void)		_reflectVectorN4(const C0 normal[4], const C1 v[4], C2 out[4])
	{
		_mad4(v,normal,2*(-_dot4(v,normal)),out);
	}

	MFUNC3V (void)		_reflectVectorNV(const C0*normal, const C1*v, C2*out)
	{
		_madV<C1,C0,C0,Dimensions>(v,normal,2*(-_dotV<C1,C0,Dimensions>(v,normal)),out);
	}


	MFUNC3 (void)		_reflectVector( const C0*normal, const C1*v, C2*out, count_t el_count)
	{
		_mad(v,normal,2*(-_dot(v,normal,el_count))/_dot(normal,el_count),out,el_count);
	}

	MFUNC3 (void)		_reflectVector(const C0 base[3], const C0 normal[3], const C1 v[3], C2 out[3])
	{
		_mad(v,normal,2*(-_dot(v,normal))/_dot(normal),out);
	}

	MFUNC3 (void)		_reflectVector2(const C0 normal[2], const C1 v[2], C2*out[2])
	{
		_mad2(v,normal,2*(-_dot2(v,normal))/_dot2(normal),out);
	}

	MFUNC3 (void)		_reflectVector4(const C0 normal[4], const C1 v[4], C2 out[4])
	{
		_mad4(v,normal,2*(-_dot4(v,normal))/_dot4(normal),out);
	}

	MFUNC3V (void)		_reflectVectorV(const C0*normal, const C1*v, C2*out)
	{
		_madV<C1,C0,C0,Dimensions>(v,normal,2*(-_dotV<C1,C0,Dimensions>(v,normal))/_dotV<C0,Dimensions>(normal),out);
	}

	MFUNC2 (void)		_reflectVector(const C0*normal, C1*v, count_t el_count)
	{
		_mad(v,normal,2*(-_dot(v,normal,el_count))/_dot(normal,el_count));
	}

	MFUNC2 (void)		_reflectVector(const C0 normal[3], C1 v[3])
	{
		_mad(v,normal,2*(-_dot(v,normal))/_dot(normal));
	}

	MFUNC2 (void)		_reflectVector2(const C0 normal[2], C1 v[2])
	{
		_mad2(v,normal,2*(-_dot2(v,normal))/_dot2(normal));
	}

	MFUNC2 (void)		_reflectVector4(const C0 normal[4], C1 v[4])
	{
		_mad4(v,normal,2*(-_dot4(v,normal))/_dot4(normal));
	}

	MFUNC2V (void)		_reflectVectorV(const C0*normal, C1*v)
	{
		_madV<C1,C0,Dimensions>(v,normal,2*(-_dotV<C1,C0,Dimensions>(v,normal))/_dotV<C0,Dimensions>(normal));
	}


	MFUNC3 (void)		_reflectVector(const C0 normal[3], const C1 v[3], C2 out[3])
	{
		_mad(v,normal,2*(-_dot(v,normal))/_dot(normal),out);
	}

	MFUNC3 (void)		_reflectVector2(const C0 normal[2], const C1 v[2], C2 out[2])
	{
		_mad2(v,normal,2*(-_dot2(v,normal))/_dot2(normal),out);
	}



	MFUNC2 (void)		_reflectVectorN(const C0*normal, C1*v, count_t el_count)
	{
		_mad(v,normal,2*(-_dot(v,normal,el_count)));
	}

	MFUNC2 (void)		_reflectVectorN(const C0 normal[3], C1 v[3])
	{
		_mad(v,normal,2*(-_dot(v,normal)));
	}

	MFUNC2 (void)		_reflectVectorN2(const C0 normal[2], C1 v[2])
	{
		_mad2(v,normal,2*(-_dot2(v,normal)));
	}

	MFUNC2 (void)		_reflectVectorN4(const C0 normal[4], C1 v[4])
	{
		_mad4(v,normal,2*(-_dot4(v,normal)));
	}

	MFUNC2V (void)		_reflectVectorNV(const C0*normal, C1*v)
	{
		_madV<C1,C0,Dimensions>(v,normal,2*(-_dotV<C1,C0,Dimensions>(v,normal)));
	}




	MFUNC (void) _rotateLeft(C*v, count_t el_count)
	{
		C backup = v[0];
		VECTOR_LOOP(el_count-1)
			v[el] = v[el+1];
		v[el_count-1] = backup;
	}

	MFUNC (void) _rotateLeft(C*v)
	{
		C backup = v[0];
		v[0] = v[1];
		v[1] = v[2];
		v[2] = backup;


	}

	MFUNC (void) _rotateLeft4(C*v)
	{
		C backup = v[0];
		v[0] = v[1];
		v[1] = v[2];
		v[2] = v[3];
		v[3] = backup;
	}



	MFUNCV (void) _rotateLeftV(C*v)
	{
		C backup = v[0];
		QRotateLeft<C,Elements-1>::perform(v);
		v[Elements-1] = backup;
	}

	MFUNCV (void) _rotateRight(C*v, count_t el_count)
	{
		C backup = v[el_count-1];
		for (count_t i = el_count-1; i; i--)
			v[i] = v[i-1];
		v[0] = backup;
	}

	MFUNC (void) _rotateRight(C*v)
	{
		C backup = v[2];
		v[2] = v[1];
		v[1] = v[0];
		v[0] = backup;
	}

	MFUNC (void) _rotateRight4(C*v)
	{
		C backup = v[3];
		v[3] = v[2];
		v[2] = v[1];
		v[1] = v[0];
		v[0] = backup;
	}


	MFUNCV (void) _rotateRightV(C*v)
	{
		C backup = v[Elements-1];
		QRotateRight<C,Elements-1>::perform(v+Elements-1);
		v[0] = backup;
	}



	MFUNCV (void)	_normalizeV(C*v)
	{
		C len = QDot<C,C,Dimensions>::perform(v,v);
		len = vsqrt(len);
		QScale<C,C,Dimensions>::perform(v,(C)1/len);
	}

	MFUNC (void) _normalize(C*v, count_t el_count)
	{
		C len = 0;
		VECTOR_LOOP(el_count)
			len+=v[el]*v[el];
		len = vsqrt(len);
		VECTOR_LOOP(el_count)
			v[el]/=len;
	}

	MFUNC (void) _normalize(C v[3])
	{
		C len = vsqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
		v[0]/=len;
		v[1]/=len;
		v[2]/=len;
	}

	MFUNC (void) _normalize4(C v[4])
	{
		C len = vsqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]+v[3]*v[3]);
		v[0]/=len;
		v[1]/=len;
		v[2]/=len;
		v[3]/=len;
	}

	MFUNC (void) _normalize2(C v[2])
	{
		C len = vsqrt(v[0]*v[0]+v[1]*v[1]);
		v[0]/=len;
		v[1]/=len;
	}

	MFUNC (void)		_normalize02(C v[2])
	{
		C dlen = v[0]*v[0]+v[1]*v[1];
		if (dlen<=GetError<C>())
		{
			v[0] = 1;
			return;
		}
		dlen = vsqrt(dlen);
		v[0] /= dlen;
		v[1] /= dlen;
	}

	MFUNC (void)		_normalize04(C v[4])
	{
		C dlen = v[0]*v[0]+v[1]*v[1]+v[2]*v[2]+v[3]*v[3];
		if (dlen<=GetError<C>())
		{
			v[0] = 1;
			return;
		}
		dlen = vsqrt(dlen);
		v[0] /= dlen;
		v[1] /= dlen;
		v[2] /= dlen;
		v[3] /= dlen;
	}


	MFUNCV (void)	_normalize0V(C*v)
	{
		C len = QDot<C,C,Dimensions>::perform(v,v);
		if (len<=GetError<C>())
		{
			v[0] = 1;
			return;
		}
		len = vsqrt(len);
		QScale<C,C,Dimensions>::perform(v,(C)1/len);
	}


	MFUNC (void) _normalize0(C*v, count_t el_count)
	{
		C len = 0;
		VECTOR_LOOP(el_count)
			len+=v[el]*v[el];
		if (len<=GetError<C>())
		{
			v[0] = 1;
			return;
		}
		len = vsqrt(len);
		VECTOR_LOOP(el_count)
			v[el]/=len;
	}

	MFUNC (void) _normalize0(C v[3])
	{
		C len = v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
		if (isnan(len) || len<=GetError<C>())
		{
			v[0] = 1;
			v[1] = 0;
			v[2] = 0;
			return;
		}
		len = vsqrt(len);
		v[0]/=len;
		v[1]/=len;
		v[2]/=len;
	}


	MFUNC3V
		(void) _scaleV(const C0*center, C1 factor, C2*v)
		{
			QInterpolate<C0,C2,C1,C2,Dimensions>::perform(center,v,factor,v);
		}

	MFUNC3
		(void) _scale(const C0*center,C1 factor, C2*v, count_t el_count)
		{
			VECTOR_LOOP(el_count)
				v[el] = center[el]+(v[el]-center[el])*factor;
		}

	MFUNC3
		(void) _scale(const C0 center[3],C1 factor, C2 v[3])
		{
			v[0] = center[0]+(v[0]-center[0])*factor;
			v[1] = center[1]+(v[1]-center[1])*factor;
			v[2] = center[2]+(v[2]-center[2])*factor;
		}

	MFUNC3V
		(void)	_scaleAbsoluteV(const C0*center, C1*v, C2 distance)
		{
			C2	temp[Dimensions];
			_subtractV<C0,C1,C2,Dimensions>(v,center,temp);
			C2 len=QDot<C2,C2,Dimensions>::perform(temp,temp);
			len = vsqrt(len);
			QMad<C0,C2,C2,C1,Dimensions>::perform(center,temp,distance/len,v);
		}

	MFUNC3
		(void) _scaleAbsolute(const C0*center,C1*v,C2 factor,count_t el_count)
		{
			C2 len=0;
			VECTOR_LOOP(el_count)
				len+=sqr(v[el]-center[el]);
			len = vsqrt(len);
			VECTOR_LOOP(el_count)
				v[el] = center[el]+(v[el]-center[el])*factor/len;
		}

	MFUNC3
		(void) _scaleAbsolute(const C0 center[3],C1 v[3],C2 factor)
		{
			C1 len = vsqrt(sqr(v[0]-center[0])+sqr(v[1]-center[1])+sqr(v[2]-center[2]));
			v[0] = center[0]+(v[0]-center[0])*factor/len;
			v[1] = center[1]+(v[1]-center[1])*factor/len;
			v[2] = center[2]+(v[2]-center[2])*factor/len;
		}
		
	MFUNC3V
		(void)	_scaleAbsolute0V(const C0*center, C1*v, C2 distance)
		{
			C2	temp[Dimensions];
			_subtractV<C0,C1,C2,Dimensions>(v,center,temp);
			C2 len=QDot<C2,C2,Dimensions>::perform(temp);
			len = vsqrt(len);
			if (len > GetError<C2>())
				QMad<C0,C2,C2,C1,Dimensions>::perform(center,temp,distance/len,v);
			else
				v[0] += distance;
		}

	MFUNC3
		(void) _scaleAbsolute0(const C0*center,C1*v,C2 factor,count_t el_count)
		{
			C2 len=0;
			VECTOR_LOOP(el_count)
				len+=sqr(v[el]-center[el]);
				
			len = vsqrt(len);
			if (len > GetError<C2>())
			{
				VECTOR_LOOP(el_count)
					v[el] = center[el]+(v[el]-center[el])*factor/len;
			}
			else
				v[0] += factor;
		}

	MFUNC3
		(void) _scaleAbsolute0(const C0 center[3],C1 v[3],C2 factor)
		{
			C1 len = vsqrt(sqr(v[0]-center[0])+sqr(v[1]-center[1])+sqr(v[2]-center[2]));
			
			if (len > GetError<C1>())
			{
				v[0] = center[0]+(v[0]-center[0])*factor/len;
				v[1] = center[1]+(v[1]-center[1])*factor/len;
				v[2] = center[2]+(v[2]-center[2])*factor/len;
			}
			else
				v[0] += factor;
		}

	MFUNC3V (void)	_centerV(const C0*v, const C1*w,C2*out)
	{
		VECTOR_LOOP(Dimensions)
			out[el] = (v[el]+w[el])/2;
	}

	MFUNC3 (void) _center(const C0*v,const C1*w,C2*out,count_t el_count)
	{
		VECTOR_LOOP(el_count)
			out[el] = (v[el]+w[el])/2;
	}

	MFUNC3 (void) _center(const C0 v[3],const C1 w[3],C2 out[3])
	{
		out[0] = (v[0]+w[0])/2;
		out[1] = (v[1]+w[1])/2;
		out[2] = (v[2]+w[2])/2;
	}

	MFUNC3 (void)		_center2(const C0 v[2],const C1 w[2],C2 out[2])
	{
		out[0] = (v[0]+w[0])/2;
		out[1] = (v[1]+w[1])/2;
	}

	MFUNC3 (void)		_center4(const C0 v[4],const C1 w[4],C2 out[4])
	{
		out[0] = (v[0]+w[0])/2;
		out[1] = (v[1]+w[1])/2;
		out[2] = (v[2]+w[2])/2;
		out[3] = (v[3]+w[3])/2;
	}


	MFUNC4V (void)	_centerV(const C0*u, const C1*v, const C2*w,C3*out)
	{
		QAdd3<C0,C1,C2,C3,Dimensions>::perform(u,v,w,out);
		QScale<C3,C3,Dimensions>::perform(out,(C3)1/3);
	}

	MFUNC4 (void) _center(const C0*u,const C1*v,const C2*w,C3*out,count_t el_count)
	{
		VECTOR_LOOP(el_count)
			out[el] = (u[el]+v[el]+w[el])/3;
	}

	MFUNC4 (void) _center(const C0*u,const C1*v,const C2*w,C3*out)
	{
		out[0] = (u[0]+v[0]+w[0])/3;
		out[1] = (u[1]+v[1]+w[1])/3;
		out[2] = (u[2]+v[2]+w[2])/3;
	}


	MFUNC4 (void) _center2(const C0*u,const C1*v,const C2*w,C3*out)
	{
		out[0] = (u[0]+v[0]+w[0])/3;
		out[1] = (u[1]+v[1]+w[1])/3;
	}


	MFUNC4 (void) _center4(const C0*u,const C1*v,const C2*w,C3*out)
	{
		out[0] = (u[0]+v[0]+w[0])/3;
		out[1] = (u[1]+v[1]+w[1])/3;
		out[2] = (u[2]+v[2]+w[2])/3;
		out[3] = (u[3]+v[3]+w[3])/3;
	}



	MFUNC4V
		(void)	_interpolateV(const C0*v, const C1*w, C2 factor, C3*out)
		{
			QInterpolate<C0,C1,C2,C3,Dimensions>::perform(v,w,factor,out);
		}

	MFUNC4
		(void) _interpolate(const C0*v,const C1*w, C2 factor, C3*out,count_t el_count)
		{
			VECTOR_LOOP(el_count)
				out[el] = v[el]+(w[el]-v[el])*factor;
		}

	MFUNC4
		(void) _interpolate2(const C0 v[2],const C1 w[2], C2 factor, C3 out[2])
		{
			out[0] = v[0]+(w[0]-v[0])*factor;
			out[1] = v[1]+(w[1]-v[1])*factor;
		}

	MFUNC4
		(void) _interpolate4(const C0 v[4],const C1 w[4], C2 factor, C3 out[4])
		{
			out[0] = v[0]+(w[0]-v[0])*factor;
			out[1] = v[1]+(w[1]-v[1])*factor;
			out[2] = v[2]+(w[2]-v[2])*factor;
			out[3] = v[3]+(w[3]-v[3])*factor;
		}

	MFUNC4
		(void) _interpolate(const C0 v[3],const C1 w[3], C2 factor, C3 out[3])
		{
			out[0] = v[0]+(w[0]-v[0])*factor;
			out[1] = v[1]+(w[1]-v[1])*factor;
			out[2] = v[2]+(w[2]-v[2])*factor;
		}



	MFUNCV
		(void)	_absV(C*v)
		{
			QMakeAbs<C,Dimensions>::perform(v);
		}

	MFUNC (void)	_abs(C*v, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			v[el] = vabs(v[el]);
	}

	MFUNC (void)	_abs(C v[3])
	{
		v[0] = vabs(v[0]);
		v[1] = vabs(v[1]);
		v[2] = vabs(v[2]);
	}


	MFUNCV (C)		_sumV(const C*v)
	{
		return QSum<C,Dimensions>::perform(v);
	}

	MFUNC (C)	_sum(const C*v, count_t el_count)
	{
		C value = v[0];
		for (count_t el = 1; el < el_count; el++)
			value+=v[el];
		return value;
	}

	MFUNC (C)	_sum(const C v[3])
	{
		return v[0]+v[1]+v[2];
	}


	MFUNC3 (void)	_subtract2(const C0 v[2], const C1 w[2], C2 out[2])
	{
		out[0] = v[0] - w[0];
		out[1] = v[1] - w[1];
	}

	MFUNC3 (void)	_subtract4(const C0 v[4], const C1 w[4], C2 out[4])
	{
		out[0] = v[0] - w[0];
		out[1] = v[1] - w[1];
		out[2] = v[2] - w[2];
		out[3] = v[3] - w[3];
	}


	MFUNC3V (void)	_subtractV(const C0*v, const C1*w, C2*out)
	{
		QSub<C0,C1,C2,Dimensions>::perform(v,w,out);
	}

	MFUNC3 (void)	_subtract(const C0*v, const C1*w, C2*out,count_t el_count)
	{
		VECTOR_LOOP(el_count)
			out[el] = v[el] - w[el];
	}

	MFUNC3 (void) _subtract(const C0 v[3], const C1 w[3], C2 out[3])
	{
		out[0] = v[0] - w[0];
		out[1] = v[1] - w[1];
		out[2] = v[2] - w[2];
	}

	MFUNC2 (void)	_subtract2(C0 v[2], const C1 w[2])
	{
		v[0] -= w[0];
		v[1] -= w[1];
	}

	MFUNC2 (void)	_subtract4(C0 v[4], const C1 w[4])
	{
		v[0] -= w[0];
		v[1] -= w[1];
		v[2] -= w[2];
		v[3] -= w[3];
	}


	MFUNC2V (void)	_subtractV(C0*v, const C1*w)
	{
		#ifndef __BORLANDC__
			QSubDirect<C0,C1,Dimensions>::perform(v,w);
		#else
			VECTOR_LOOP(Dimensions)
				v[el] -= w[el];
		#endif
	}

	MFUNC2 (void) _subtract(C0*v, const C1*w,count_t el_count)
	{
		VECTOR_LOOP(el_count)
			v[el]-=w[el];
	}

	MFUNC2 (void)	_subtract(C0 v[3], const C1 w[3])
	{
		v[0] -= w[0];
		v[1] -= w[1];
		v[2] -= w[2];
	}


	






	MFUNC3
		(void)	_subtractValue2(const C0 v[2], C1 value, C2 out[2])
		{
			out[0] = v[0]-value;
			out[1] = v[1]-value;
		}

	MFUNC3
		(void)	_subtractValue4(const C0 v[4], C1 value, C2 out[4])
		{
			out[0] = v[0]-value;
			out[1] = v[1]-value;
			out[2] = v[2]-value;
			out[3] = v[3]-value;
		}


	MFUNC3V
		(void)	_subtractValueV(const C0*v, C1 value, C2*out)
		{
			QSubVal<C0,C1,C2,Dimensions>::perform(v,value,out);
		}

	MFUNC3
		(void)	_subtractValue(const C0*v, C1 value, C2*out,count_t el_count)
		{
			VECTOR_LOOP(el_count)
				out[el] = v[el]-value;
		}

	MFUNC3
		(void)	_subtractValue(const C0 v[3], C1 value, C2 out[3])
		{
			out[0] = v[0]-value;
			out[1] = v[1]-value;
			out[2] = v[2]-value;
		}

	MFUNC2
		(void)	_subtractValue2(C0 v[2], C1 value)
		{
			v[0] -= value;
			v[1] -= value;
		}

	MFUNC2
		(void)	_subtractValue4(C0 v[4], C1 value)
		{
			v[0] -= value;
			v[1] -= value;
			v[2] -= value;
			v[3] -= value;
		}


	MFUNC2V
		(void)	_subtractValueV(C0*v, C1 value)
		{
			QSubValDirect<C0,C1,Dimensions>::perform(v,value);
		}

	MFUNC2
		(void)	_subtractValue(C0*v, C1 value,count_t el_count)
		{
			VECTOR_LOOP(el_count)
				v[el] -= value;
		}

	MFUNC2
		(void)	_subtractValue(C0 v[3], C1 value)
		{
			v[0] -= value;
			v[1] -= value;
			v[2] -= value;
		}


	MFUNC3 (void)	_add2(const C0 v[2], const C1 w[2], C2 out[2])
	{
		out[0] = v[0] + w[0];
		out[1] = v[1] + w[1];
	}

	MFUNC3 (void)	_add4(const C0 v[4], const C1 w[4], C2 out[4])
	{
		out[0] = v[0] + w[0];
		out[1] = v[1] + w[1];
		out[2] = v[2] + w[2];
		out[3] = v[3] + w[3];
	}



	MFUNC3V (void)	_addV(const C0*v, const C1*w, C2*out)
	{
		QAdd<C0,C1,C2,Dimensions>::perform(v,w,out);
	}

	MFUNC3 (void)	_add(const C0*v, const C1*w, C2*out, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			out[el] = v[el]+w[el];
	}

	MFUNC3 (void)	_add(const C0 v[3], const C1 w[3], C2 out[3])
	{
		out[0] = v[0] + w[0];
		out[1] = v[1] + w[1];
		out[2] = v[2] + w[2];
	}

	MFUNC2 (void)	_add2(C0 v[2], const C1 w[2])
	{
		v[0] += w[0];
		v[1] += w[1];
	}

	MFUNC2 (void)	_add4(C0 v[4], const C1 w[4])
	{
		v[0] += w[0];
		v[1] += w[1];
		v[2] += w[2];
		v[3] += w[3];
	}


	MFUNC2V (void)	_addV(C0*v, const C1*w)
	{
		#ifndef __BORLANDC__
			QAddDirect<C0,C1,Dimensions>::perform(v,w);
		#else
			for (count_t i = 0; i < Dimensions; i++)
				v[i] += w[i];
		#endif
	}

	MFUNC2 (void)	_add(C0*v, const C1*w, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			v[el]+=w[el];
	}

	MFUNC2 (void)	_add(C0 v[3], const C1 w[3])
	{
		v[0] += w[0];
		v[1] += w[1];
		v[2] += w[2];
	}


	MFUNC3
		(void)	_addValue2(const C0 v[2], C1 value, C2 out[2])
		{
			out[0] = (C2)(v[0] + value);
			out[1] = (C2)(v[1] + value);
		}

	MFUNC3
		(void)	_addValue4(const C0 v[4], C1 value, C2 out[4])
		{
			out[0] = (C2)(v[0] + value);
			out[1] = (C2)(v[1] + value);
			out[2] = (C2)(v[2] + value);
			out[3] = (C2)(v[3] + value);
		}


	MFUNC3V
		(void)	_addValueV(const C0*v, C1 value, C2*out)
		{
			QAddVal<C0,C1,C2,Dimensions>::perform(v,value,out);
		}

	MFUNC3
		(void) _addValue(const C0*v, C1 value, C2*out, count_t el_count)
		{
			VECTOR_LOOP(el_count)
				out[el] = v[el]+value;
		}

	MFUNC3
		(void) _addValue(const C0 v[3], C1 value, C2 out[3])
		{
			out[0] = (C2)(v[0] + value);
			out[1] = (C2)(v[1] + value);
			out[2] = (C2)(v[2] + value);
		}

	MFUNC2
		(void)	_addValue2(C0 v[2], C1 value)
		{
			v[0] += value;
			v[1] += value;
		}

	MFUNC2
		(void)	_addValue4(C0 v[4], C1 value)
		{
			v[0] += value;
			v[1] += value;
			v[2] += value;
			v[3] += value;
		}


	MFUNC2V
		(void)	_addValueV(C0*v, C1 value)
		{
			QAddValDirect<C0,C1,Dimensions>::perform(v,value);
		}

	MFUNC2
		(void) _addValue(C0*v, C1 value, count_t el_count)
		{
			VECTOR_LOOP(el_count)
				v[el]+=value;
		}

	MFUNC2
		(void) _addValue(C0 v[3], C1 value)
		{
			v[0]+=value;
			v[1]+=value;
			v[2]+=value;
		}


	MFUNC3V (void)	_stretchV(const C0*v, const C1*w, C2*out)
	{
		QStretch<C0,C1,C2,Dimensions>::perform(v,w,out);
	}

	MFUNC3 (void)	_stretch(const C0*v, const C1*w, C2*out, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			out[el] = v[el]*w[el];
	}

	MFUNC3 (void) _stretch2(const C0 v[2], const C1 w[2], C2 out[2])
	{
		out[0] = v[0]*w[0];
		out[1] = v[1]*w[1];
	}

	MFUNC3 (void) _stretch4(const C0 v[4], const C1 w[4], C2 out[4])
	{
		out[0] = v[0]*w[0];
		out[1] = v[1]*w[1];
		out[2] = v[2]*w[2];
		out[3] = v[3]*w[3];
	}


	MFUNC3 (void) _stretch(const C0 v[3], const C1 w[3], C2 out[3])
	{
		out[0] = v[0]*w[0];
		out[1] = v[1]*w[1];
		out[2] = v[2]*w[2];
	}

	MFUNC2 (void)		_stretch(C0*in_out, const C1*w, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			in_out[el] *= w[el];
	}

	MFUNC2 (void)		_stretch(C0 in_out[3], const C1 w[3])
	{
		in_out[0] *= w[0];
		in_out[1] *= w[1];
		in_out[2] *= w[2];
	}

	MFUNC2V (void)		_stretchV(C0*in_out, const C1*w)
	{
		QStretch<C0,C1,C0,Dimensions>::perform(in_out,w,in_out);
	}



	MFUNC3V
		(void)	_multiplyV(const C0*v, C1 value, C2*out)
		{
			#ifndef __BORLANDC__
				QMult<C0,C1,C2,Dimensions>::perform(v,value,out);
			#else
				for (count_t i = 0; i < Dimensions; i++)
					out[i] = v[i] * value;
			#endif
		}

	MFUNC3
		(void)	_multiply(const C0*v, C1 value, C2*out, count_t el_count)
		{
			VECTOR_LOOP(el_count)
				out[el] = v[el] * value;
		}


	MFUNC3
		(void)	_multiply(const C0 v[3], C1 value, C2 out[3])
		{
			out[0] = (C2)(v[0]*value);
			out[1] = (C2)(v[1]*value);
			out[2] = (C2)(v[2]*value);
		}

	MFUNC3
		(void)		_multiply2(const C0 v[2], C1 value, C2 out[2])
		{
			out[0] = (C2)(v[0]*value);
			out[1] = (C2)(v[1]*value);
		}

	MFUNC3
		(void)		_multiply4(const C0 v[4], C1 value, C2 out[4])
		{
			out[0] = (C2)(v[0]*value);
			out[1] = (C2)(v[1]*value);
			out[2] = (C2)(v[2]*value);
			out[3] = (C2)(v[3]*value);
		}

	MFUNC2V
		(void)	_multiplyV(C0*v, C1 value)
		{
			#ifndef __BORLANDC__
				QScale<C0,C1,Dimensions>::perform(v,value);
			#else
				for (count_t i = 0; i < Dimensions; i++)
					v[i]*=value;
			#endif
		}

	MFUNC2
		(void)	_multiply(C0*v, C1 value, count_t el_count)
		{
			VECTOR_LOOP(el_count)
				v[el]*=value;
		}

	MFUNC2
		(void)		_multiply2(C0 v[2], C1 value)
		{
			v[0]*=value;
			v[1]*=value;
		}

	MFUNC2
		(void)		_multiply4(C0 v[4], C1 value)
		{
			v[0]*=value;
			v[1]*=value;
			v[2]*=value;
			v[3]*=value;
		}

	MFUNC2
		(void)	_multiply(C0 v[3], C1 value)
		{
			v[0]*=value;
			v[1]*=value;
			v[2]*=value;
		}


	MFUNC3V (void)	_resolveV(const C0*v, const C1*w, C2*out)
	{
		QResolve<C0,C1,C2,Dimensions>::perform(v,w,out);
	}

	MFUNC3 (void)	_resolve(const C0*v, const C1*w, C2*out, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			out[el] = v[el]/w[el];
	}

	MFUNC3 (void) _resolve(const C0 v[3], const C1 w[3], C2 out[3])
	{
		out[0] = v[0]/w[0];
		out[1] = v[1]/w[1];
		out[2] = v[2]/w[2];
	}
	
			

	MFUNC6
		(void)	_resolveUCBS(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], C4 t, C5 out[3])
		{
			C4	//i = 1-t,
				f0 = (-t*t*t+3*t*t-3*t+1)/6,
				f1 = (3*t*t*t-6*t*t+4)/6,
				f2 = (-3*t*t*t+3*t*t+3*t+1)/6,
				f3 = (t*t*t)/6;
				
			out[0] = p0[0]*f0 + p1[0]*f1 + p2[0]*f2 + p3[0]*f3;
			out[1] = p0[1]*f0 + p1[1]*f1 + p2[1]*f2 + p3[1]*f3;
			out[2] = p0[2]*f0 + p1[2]*f1 + p2[2]*f2 + p3[2]*f3;
		}
		

	MFUNC6
		(void)	_resolveUCBSaxis(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], C4 t, C5 out[3])
		{
			C5	//i = 1-t,
				f0 = (t*t-2*t+1)/2,
				f1 = (-2*t*t+2*t+1)/2,
				f2 = (t*t)/2;
			
				
			out[0] = (p1[0]-p0[0])*f0 + (p2[0]-p1[0])*f1 + (p3[0]-p2[0])*f2;
			out[1] = (p1[1]-p0[1])*f0 + (p2[1]-p1[1])*f1 + (p3[1]-p2[1])*f2;
			out[2] = (p1[2]-p0[2])*f0 + (p2[2]-p1[2])*f1 + (p3[2]-p2[2])*f2;
		}	

	MFUNC6
		(void)	_resolveSpline(const C0*p0, const C1*p1, const C2*p2, const C3*p3, C4 t, C5*out, count_t el_count)
		{
			C4	i = 1-t;
			VECTOR_LOOP(el_count)
				out[el] = i*i*i*p0[el] + t*i*i*3*p1[el] + t*t*i*3*p2[el] + t*t*t*p3[el];
		}


	MFUNC6
		(void)	_resolveSpline(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], C4 t, C5 out[3])
		{
			C4	i = 1-t,
				f3 = t*t*t,
				f2 = t*t*i*3,
				f1 = t*i*i*3,
				f0 = i*i*i;
			out[0] = p0[0]*f0 + p1[0]*f1 + p2[0]*f2 + p3[0]*f3;
			out[1] = p0[1]*f0 + p1[1]*f1 + p2[1]*f2 + p3[1]*f3;
			out[2] = p0[2]*f0 + p1[2]*f1 + p2[2]*f2 + p3[2]*f3;
		}

	MFUNC6
		(void)	_resolveSpline2(const C0 p0[2], const C1 p1[2], const C2 p2[2], const C3 p3[2], C4 t, C5 out[2])
		{
			C4	i = 1-t,
				f3 = t*t*t,
				f2 = t*t*i*3,
				f1 = t*i*i*3,
				f0 = i*i*i;
			out[0] = p0[0]*f0 + p1[0]*f1 + p2[0]*f2 + p3[0]*f3;
			out[1] = p0[1]*f0 + p1[1]*f1 + p2[1]*f2 + p3[1]*f3;
		}

	MFUNC6
		(void)	_resolveSpline4(const C0 p0[4], const C1 p1[4], const C2 p2[4], const C3 p3[4], C4 t, C5 out[4])
		{
		
			C4	i = 1-t,
				f3 = t*t*t,
				f2 = t*t*i*3,
				f1 = t*i*i*3,
				f0 = i*i*i;
			out[0] = p0[0]*f0 + p1[0]*f1 + p2[0]*f2 + p3[0]*f3;
			out[1] = p0[1]*f0 + p1[1]*f1 + p2[1]*f2 + p3[1]*f3;
			out[2] = p0[2]*f0 + p1[2]*f1 + p2[2]*f2 + p3[2]*f3;
			out[3] = p0[3]*f0 + p1[3]*f1 + p2[3]*f2 + p3[3]*f3;
		}



	MFUNC6V
		(void)	_resolveSplineV(const C0*p0, const C1*p1, const C2*p2, const C3*p3, C4 t, C5*out)
		{
			C4	i = 1-t;
			_multiplyV<C0,C4,C5,Dimensions>(p3,t*t*t,out);
			_multAddV<C5,C1,C4,Dimensions>(out,p2,t*t*i*3);
			_multAddV<C5,C2,C4,Dimensions>(out,p1,t*i*i*3);
			_multAddV<C5,C3,C4,Dimensions>(out,p0,i*i*i);
		}

	MFUNC6
		(void)	_resolveSplineAxis(const C0*p0, const C1*p1, const C2*p2, const C3*p3, C4 t, C5*out, count_t el_count)
		{
			C4 i = 1-t;
			C5 *temp = alloc<C5>(el_count);
			_subtract(p1,p0,temp,el_count);
			_multiply(temp,i*i,out,el_count);
			_subtract(p2,p1,temp,el_count);
			_multAdd(out,temp,t*i*2,el_count);
			_subtract(p3,p2,temp,el_count);
			_multAdd(out,temp,t*t,el_count);
			dealloc(temp);
			_normalize0(out);
		}

	MFUNC6
		(void)	_resolveSplineAxis(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], C4 t, C5 out[3])
		{
			C4 i = 1-t;
			C5	temp[3];
			_subtract(p1,p0,temp);
			_multiply(temp,i*i,out);
			_subtract(p2,p1,temp);
			_multAdd(out,temp,t*i*2);
			_subtract(p3,p2,temp);
			_multAdd(out,temp,t*t);
		}

	MFUNC6
		(void)	_resolveSplineAxis2(const C0 p0[2], const C1 p1[2], const C2 p2[2], const C3 p3[2], C4 t, C5 out[2])
		{
			C4 i = 1-t;
			C5	temp[2];
			_subtract2(p1,p0,temp);
			_multiply2(temp,i*i,out);
			_subtract2(p2,p1,temp);
			_multAdd2(out,temp,t*i*2);
			_subtract2(p3,p2,temp);
			_multAdd2(out,temp,t*t);
		}

	MFUNC6
		(void)	_resolveSplineAxis4(const C0 p0[4], const C1 p1[4], const C2 p2[4], const C3 p3[4], C4 t, C5 out[4])
		{
			C4 i = 1-t;
			C5	temp[4];
			_subtract4(p1,p0,temp);
			_multiply4(temp,i*i,out);
			_subtract4(p2,p1,temp);
			_multAdd4(out,temp,t*i*2);
			_subtract4(p3,p2,temp);
			_multAdd4(out,temp,t*t);
		}


	MFUNC6V
		(void)	_resolveSplineAxisV(const C0*p0, const C1*p1, const C2*p2, const C3*p3, C4 t, C5*out)
		{
			C4 i = 1-t;
			C5	temp[Dimensions];
			_subtractV<C0,C1,C5,Dimensions>(p1,p0,temp);
			_multiplyV<C5,C4,C5,Dimensions>(temp,i*i,out);
			_subtractV<C2,C1,C5,Dimensions>(p2,p1,temp);
			_multAddV<C5,C5,C4,Dimensions>(out,temp,t*i*2);
			_subtractV<C3,C2,C5,Dimensions>(p3,p2,temp);
			_multAddV<C5,C5,C4,Dimensions>(out,temp,t*t);
		}


	MFUNC3V
		(void)	_divideV(const C0*v, C1 value, C2*out)
		{
			QDiv<C0,C1,C2,Dimensions>::perform(v,value,out);
		}

	MFUNC3
		(void) _divide(const C0*v, C1 value, C2*out, count_t el_count)
		{
			VECTOR_LOOP(el_count)
				out[el] = (C2)(v[el]/value);
		}

	MFUNC3
		(void) _divide(const C0 v[3], C1 value, C2 out[3])
		{
			out[0] = (C2)(v[0]/value);
			out[1] = (C2)(v[1]/value);
			out[2] = (C2)(v[2]/value);
		}

	MFUNC3
		(void) _divide2(const C0 v[2], C1 value, C2 out[2])
		{
			out[0] = (C2)(v[0]/value);
			out[1] = (C2)(v[1]/value);
		}

	MFUNC3
		(void) _divide4(const C0 v[4], C1 value, C2 out[4])
		{
			out[0] = (C2)(v[0]/value);
			out[1] = (C2)(v[1]/value);
			out[2] = (C2)(v[2]/value);
			out[3] = (C2)(v[3]/value);
		}


	MFUNC2V
		(void)	_divideV(C0*v, C1 value)
		{
			QDivDirect<C0,C1,Dimensions>::perform(v,value);
		}

	MFUNC2
		(void) _divide(C0*v, C1 value, count_t el_count)
		{
			VECTOR_LOOP(el_count)
				v[el]/=value;
		}

	MFUNC2
		(void) _divide(C0 v[3], C1 value)
		{
			v[0]/=value;
			v[1]/=value;
			v[2]/=value;
		}

	MFUNC2
		(void) _divide2(C0 v[2], C1 value)
		{
			v[0]/=value;
			v[1]/=value;
		}

	MFUNC2
		(void) _divide4(C0 v[4], C1 value)
		{
			v[0]/=value;
			v[1]/=value;
			v[2]/=value;
			v[3]/=value;
		}


	MFUNC (C)		_dot(const C*v, count_t el_count)
	{
		C	sum = v[0] * v[0];
		for (count_t el = 1; el < el_count; el++)
			sum += v[el] * v[el];
		return sum;
	}

	MFUNC (C)		_dot(const C v[3])
	{
		return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	}


	MFUNC (C)		_dot2(const C v[2])
	{
		return v[0]*v[0] + v[1]*v[1];
	}

	MFUNC (C)		_dot4(const C v[4])
	{
		return v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3];
	}

	MFUNCV (C)		_dotV(const C*v)
	{
		return QDot<C,C,Dimensions>::perform(v,v);
	}

	MFUNC2V (C0)		_dotV(const C0*v, const C1*w)
	{
		return QDot<C0,C1,Dimensions>::perform(v,w);
	}

	MFUNC2 (C0)	_dot(const C0*v, const C1*w, count_t el_count)
	{
		C0 sum = 0;
		VECTOR_LOOP(el_count)
			sum+=v[el]*w[el];
		return sum;
	}

	MFUNC2 (C0)	_dot(const C0 v[3], const C1 w[3])
	{
		return v[0]*w[0]+v[1]*w[1]+v[2]*w[2];
	}

	MFUNC2 (C0)	_dot2(const C0 v[2], const C1 w[2])
	{
		return v[0]*w[0]+v[1]*w[1];
	}

	MFUNC2 (C0)	_dot4(const C0 v[4], const C1 w[4])
	{
		return v[0]*w[0]+v[1]*w[1]+v[2]*w[2]+v[3]*w[3];
	}


	MFUNC2V (bool)	_equalV(const C0*v, const C1*w)
	{
		#ifndef __BORLANDC__
			return QEqual<C0,C1,Dimensions>::perform(v,w);
		#else
			for (count_t i = 0; i < Dimensions; i++)
				if (v[i] != w[i])
					return false;
			return true;
		#endif
	}

	MFUNC2 (bool) _equal(const C0*v, const C1*w, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			if (v[el] != w[el]) return false;
		return true;
	}

	MFUNC2 (bool) _equal(const C0 v[3], const C1 w[3])
	{
		return (v[0] == w[0] && v[1] == w[1] && v[2] == w[2]);
	}

	MFUNC3V
		(bool)	_similarV(const C0*v, const C1*w, C2 distance)
		{
			C0 temp[Dimensions];
			_subtractV<C0,C1,C0,Dimensions>(v,w,temp);
			return QDot<C0,C1,Dimensions>::perform(temp,temp)<distance*distance;
		}

	MFUNC3
		(bool) _similar(const C0*v, const C1*w, C2 distance, count_t el_count)
		{
			C2 d = sqr(v[0]-w[0]);
			for (count_t el = 1; el < el_count; el++)
				d+=sqr(v[el]-w[el]);
			return d < distance*distance;
		}

	MFUNC3
		(bool) _similar(const C0 v[3], const C1 w[3], C2 distance)
		{
			return sqr(v[0]-w[0])+sqr(v[1]-w[1])+sqr(v[2]-w[2]) < distance*distance;
		}

	MFUNC3
		(bool) _similar2(const C0 v[2], const C1 w[2], C2 distance)
		{
			return sqr(v[0]-w[0])+sqr(v[1]-w[1]) < distance*distance;
		}

	MFUNC3
		(bool) _similar4(const C0 v[4], const C1 w[4], C2 distance)
		{
			return sqr(v[0]-w[0])+sqr(v[1]-w[1])+sqr(v[2]-w[2])+sqr(v[3]-w[3]) < distance*distance;
		}

	MFUNC2
		(bool)		_similar(const C0*v, const C1*w)
		{
			return sqr(v[0]-w[0])+sqr(v[1]-w[1])+sqr(v[2]-w[2]) < GetError<C0>()*GetError<C0>();
		}

	MFUNC2 (bool)		_similar2(const C0*v, const C1*w)
	{
		return sqr(v[0]-w[0])+sqr(v[1]-w[1]) < GetError<C0>()*GetError<C0>();
	}

	MFUNC2 (bool)		_similar4(const C0*v, const C1*w)
	{
		return sqr(v[0]-w[0])+sqr(v[1]-w[1])+sqr(v[2]-w[2])+sqr(v[3]-w[3]) < GetError<C0>()*GetError<C0>();
	}

	MFUNC2V (bool) _similarV(const C0*v, const C1*w)
	{
		C0 temp[Dimensions];
		_subtractV<C0,C1,C0,Dimensions>(v,w,temp);
		return QDot<C0,C1,Dimensions>::perform(temp,temp)<GetError<C0>()*GetError<C0>();
	}



	MFUNC2V (C0)		_maxAxisDistanceV(const C0*v, const C1*w)
	{
		return QMaxAxisDistance<C0,C1,Dimensions>::perform(v,w);
	}


	MFUNC2 (C0)	_maxAxisDistance(const C0*v, const C1*w, count_t el_count)
	{
		C0	dist = vabs(w[0]-v[0]);
		for (count_t el = 1; el < el_count; el++)
		{
			C0	d = vabs(w[el]-v[el]);
			if (d > dist)
				dist = d;
		}
		return dist;
	}

	MFUNC2 (C0)	_maxAxisDistance(const C0 v[3], const C1 w[3])
	{
		C0 d;
		C0 dist = vabs(w[0]-v[0]);
		d = vabs(w[1]-v[1]);
		if (d > dist)
			dist = d;
		d = vabs(w[2]-v[2]);
		if (d > dist)
			dist = d;
		return dist;
	}

	MFUNC3 (C0)		_planePointDistance(const C0*base, const C1*normal, const C2*p, count_t el_count)
	{
		return (_dot(p,normal,el_count)-_dot(base,normal,el_count))/_length(normal,el_count);
	}

	MFUNC3 (C0)		_planePointDistance(const C0 base[3], const C1 normal[3], const C2 p[3])
	{
		return ((p[0]*normal[0] + p[1]*normal[1] + p[2]*normal[2])-(base[0]*normal[0] + base[1]*normal[1] + base[2]*normal[2]))/vsqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
		//return (_dot(p,normal)-_dot(base,normal))/_length(normal);
	}

	MFUNC3 (C0)		_planePointDistance2(const C0 base[2], const C1 normal[2], const C2 p[2])
	{
		return (_dot2(p,normal)-_dot2(base,normal))/_length2(normal);
	}

	MFUNC3 (C0)		_planePointDistance4(const C0 base[4], const C1 normal[4], const C2 p[4])
	{
		return (_dot4(p,normal)-_dot4(base,normal))/_length4(normal);
	}

	MFUNC3V (C0)		_planePointDistanceV(const C0*base, const C1*normal, const C2*p)
	{
		return (_dotV<C2,C1,Dimensions>(p,normal)-_dotV<C0,C1,Dimensions>(base,normal))/_lengthV<C1,Dimensions>(normal);
	}



	MFUNC3 (C0)		_planePointDistanceN(const C0*base, const C1*normal, const C2*p, count_t el_count)
	{
		return (_dot(p,normal,el_count)-_dot(base,normal,el_count));
	}

	MFUNC3 (C0)		_planePointDistanceN(const C0 base[3], const C1 normal[3], const C2 p[3])
	{
		return ((p[0]*normal[0] + p[1]*normal[1] + p[2]*normal[2])-(base[0]*normal[0] + base[1]*normal[1] + base[2]*normal[2]));
//		return (_dot(p,normal)-_dot(base,normal));
	}

	MFUNC3 (C0)		_planePointDistanceN2(const C0 base[2], const C1 normal[2], const C2 p[2])
	{
		return (_dot2(p,normal)-_dot2(base,normal));
	}

	MFUNC3 (C0)		_planePointDistanceN4(const C0 base[4], const C1 normal[4], const C2 p[4])
	{
		return (_dot4(p,normal)-_dot4(base,normal));
	}

	MFUNC3V (C0)		_planePointDistanceNV(const C0*base, const C1*normal, const C2*p)
	{
		return (_dotV<C2,C1,Dimensions>(p,normal)-_dotV<C0,C1,Dimensions>(base,normal));
	}





	MFUNC4 (C0)		_planePointDistance(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p[3])
	{
		C0 d0[3],d1[3],n[3];
		_subtract(p1,p0,d0);
		_subtract(p2,p0,d1);
		_cross(d0,d1,n);
		if (_zero(n))
			return 0;
		_normalize(n);
		return _dot(p,n)-_dot(p0,n);
	}

	MFUNC3 (C0)	_distance(const C0 v0[3], const C1 v1[3], const C2 p[3])
	{
		C0	d0[3],d1[3],r[3];
		_subtract(v1,v0,d0);
		_subtract(v0,p,d1);
		_cross(d0,d1,r);
		return _length(r)/_length(d0);
	}

	MFUNC3 (C0)	_distanceSquare(const C0 v0[3], const C1 v1[3], const C2 p[3])
	{
		C0	d0[3],d1[3],r[3];
		_subtract(v1,v0,d0);
		_subtract(v0,p,d1);
		_cross(d0,d1,r);
		return _dot(r)/_dot(d0);
	}


	MFUNC4 (C0)		_distance(const C0 v0[3], const C1 v1[3], const C2 w0[3], const C3 w1[3])
	{
		C0	d0[3],d1[3],n[3],dif[3];
		_subtract(v1,v0,d0);
		_subtract(w1,w0,d1);
		_subtract(w0,v0,dif);
		_cross(d0,d1,n);
		_normalize(n);
		return _dot(n,dif) / vabs(_dot(d0,d1));
	}


	MFUNC2V (C0)		_distanceV(const C0*v, const C1*w)
	{
		return vsqrt(QDistance<C0,C1,Dimensions>::perform(v,w));
	}

	MFUNC2 (C0)	_distance(const C0*v,const C1*w,count_t el_count)
	{
		C0 distance = 0;
		VECTOR_LOOP(el_count)
			distance+=sqr(v[el]-w[el]);
		return vsqrt(distance);
	}

	MFUNC2 (C0)	_distance(const C0 v[3],const C1 w[3])
	{
		return vsqrt(sqr(v[0]-w[0])+sqr(v[1]-w[1])+sqr(v[2]-w[2]));
	}

	MFUNC2 (C0)	_distance2(const C0 v[2],const C1 w[2])
	{
		return vsqrt(sqr(v[0]-w[0])+sqr(v[1]-w[1]));
	}

	MFUNC2 (C0)	_distance4(const C0 v[4],const C1 w[4])
	{
		return vsqrt(sqr(v[0]-w[0])+sqr(v[1]-w[1])+sqr(v[2]-w[2])+sqr(v[3]-w[3]));
	}

	MFUNC2V (C0)		_distanceSquareV(const C0*v, const C1*w)
	{
		return QDistance<C0,C1,Dimensions>::perform(v,w);
	}

	MFUNC2 (C0)	_distanceSquare(const C0*v,const C1*w,count_t el_count)
	{
		C0 distance = 0;
		VECTOR_LOOP(el_count)
			distance+=sqr(v[el]-w[el]);
		return distance;
	}

	MFUNC2 (C0)	_distanceSquare(const C0 v[3],const C1 w[3])
	{
		return sqr(v[0]-w[0])+sqr(v[1]-w[1])+sqr(v[2]-w[2]);
	}

	MFUNC2 (C0)	_distanceSquare2(const C0 v[2],const C1 w[2])
	{
		return sqr(v[0]-w[0])+sqr(v[1]-w[1]);
	}

	MFUNC2 (C0)	_distanceSquare4(const C0 v[4],const C1 w[4])
	{
		return sqr(v[0]-w[0])+sqr(v[1]-w[1])+sqr(v[2]-w[2])+sqr(v[3]-w[3]);
	}



	MFUNC (C)		_angle360(const C v[2])
	{
		return _angleOne(v)*360;
	}

	MFUNC2
		(C0) _angle360(C0 x, C1 y)
		{
			return _angleOne(x,y)*360;
		}

	MFUNC
		(C)		_angle2PI(const C v[2])
		{
			return _angleOne(v)*2*M_PI;
		}

	MFUNC2
		(C0)	_angle2PI(C0 x, C1 y)
		{
			return _angleOne(x,y)*2*M_PI;
		}

	MFUNC
		(C)	_angleOne(const C v[2])
		{
			return _angleOne(v[0],v[1]);
		}

	MFUNC2
		(C0)		_angleOne(C0 x_, C1 y_)
		{
			if (!x_ && !y_)
				return 0;
			C0 x,y;
			BYTE sector;
			if (x_>=0 && y_>=0)
			{
				x = (C0)x_;
				y = (C0)y_;
				sector = 0;
			}
			else
				if (x_ > 0)
				{
					x = (C0)-y_;
					y = (C0)x_;
					sector = 1;
				}
				else
					if (y_ >= 0)
					{
						x = (C0)y_;
						y = (C0)-x_;
						sector = 3;
					}
					else
					{
						x = (C0)-x_;
						y = (C0)-y_;
						sector = 2;
					}
			C0 d = (C0)(vatan2(x,y)/M_PI_2);
			return (d+sector)/4;
		}

	MFUNC2V (C0)		_angleV(const C0*v, const C1*w)
	{
		return acos((_dotV<C0,C1,Dimensions>(v,w)/(_lengthV<C0,Dimensions>(v)*_lengthV<C1,Dimensions>(w))))*180/M_PI;
	}

	MFUNC2 (C0)	_angle(const C0*v, const C1*w, count_t el_count)
	{
		return acos((_dot(v,w,el_count)/(_length(v,el_count)*_length(w,el_count))))*180/M_PI;
	}

	MFUNC2 (C0)	_angle(const C0 v[3], const C1 w[3])
	{
		return acos((_dot(v,w)/(_length(v)*_length(w))))*180/M_PI;
	}

	MFUNC2 (C0)	_angle2(const C0 v[2], const C1 w[2])
	{
		return acos((_dot2(v,w)/(_length2(v)*_length2(w))))*180/M_PI;
	}

	MFUNC (bool)		_isNAN(const C*v, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			if (isnan(v[el]))
				return true;
		return false;
	}

	MFUNC (bool)		_isNAN(const C v[3])
	{
		return isnan(v[0]) || isnan(v[1]) || isnan(v[2]);
	}

	MFUNC (bool)		_isNAN2(const C v[2])
	{
		return isnan(v[0]) || isnan(v[1]);
	}

	MFUNC (bool)		_isNAN4(const C v[4])
	{
		return isnan(v[0]) || isnan(v[1]) || isnan(v[2]) || isnan(v[3]);
	}

	MFUNCV (bool)		_isNANV(const C*v)
	{
		return QIsNan<C,Dimensions>::perform(v);
	}


	MFUNC2V (C0)	_intensityV(const C0*v, const C1*w)
	{
		return _dotV<C0,C1,Dimensions>(v,w)/(_lengthV<C0,Dimensions>(v)*_length<C1,Dimensions>(w));
	}

	MFUNC2 (C0)	_intensity(const C0*v, const C1*w, count_t el_count)
	{
		return _dot(v,w,el_count)/(_length(v,el_count)*_length(w,el_count));
	}

	MFUNC2 (C0)	_intensity(const C0*v, const C1*w)
	{
		return (v[0]*w[0]+v[1]*w[1]+v[2]*w[2])/(vsqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2])*vsqrt(w[0]*w[0]+w[1]*w[1]+w[2]*w[2]));
	}

	MFUNC2 (C0)	_intensity2(const C0*v, const C1*w)
	{
		return (v[0]*w[0]+v[1]*w[1])/(vsqrt(v[0]*v[0]+v[1]*v[1])*vsqrt(w[0]*w[0]+w[1]*w[1]));
	}

	MFUNC2 (C0)	_intensity4(const C0*v, const C1*w)
	{
		return (v[0]*w[0]+v[1]*w[1]+v[2]*w[2]+v[3]*w[3])/(vsqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]+v[3]*v[3])*vsqrt(w[0]*w[0]+w[1]*w[1]+w[2]*w[2]+w[3]*w[3]));
	}


	MFUNC2
		(void) _set(C0*v, C1 value, count_t el_count)
		{
			VECTOR_LOOP(el_count)
				v[el] = value;
		}

	MFUNC2V
		(void) _setV(C0*v, C1 value)
		{
			QSet<C0,C1,Dimensions>::perform(v,value);
		}

	MFUNC2
		(void) _set(C0 v[3], C1 value)
		{
			v[0] = (C0)value;
			v[1] = (C0)value;
			v[2] = (C0)value;
		}

	MFUNC2
		(void) _set4(C0 v[4], C1 value)
		{
			v[0] = (C0)value;
			v[1] = (C0)value;
			v[2] = (C0)value;
			v[3] = (C0)value;
		}
		
		
	MFUNC2
		(void) _set6(C0 v[6], C1 value)
		{
			v[0] = (C0)value;
			v[1] = (C0)value;
			v[2] = (C0)value;
			v[3] = (C0)value;
			v[4] = (C0)value;
			v[5] = (C0)value;
		}

	MFUNC2
		(void) _set9(C0 v[9], C1 value)
		{
			v[0] = (C0)value;
			v[1] = (C0)value;
			v[2] = (C0)value;
			v[3] = (C0)value;
			v[4] = (C0)value;
			v[5] = (C0)value;
			v[6] = (C0)value;
			v[7] = (C0)value;
			v[8] = (C0)value;
		}

	MFUNC2
		(void) _set16(C0 v[16], C1 value)
		{
			v[0] = (C0)value;
			v[1] = (C0)value;
			v[2] = (C0)value;
			v[3] = (C0)value;
			v[4] = (C0)value;
			v[5] = (C0)value;
			v[6] = (C0)value;
			v[7] = (C0)value;
			v[8] = (C0)value;
			v[9] = (C0)value;
			v[10] = (C0)value;
			v[11] = (C0)value;
			v[12] = (C0)value;
			v[13] = (C0)value;
			v[14] = (C0)value;
			v[15] = (C0)value;
		}		

	MFUNC2
		(void) _set2(C0 v[2], C1 value)
		{
			v[0] = (C0)value;
			v[1] = (C0)value;
		}



	MFUNC3
		(void)		_clamp(C0*v, C1 min, C2 max, count_t el_count)
		{
			VECTOR_LOOP(el_count)
				v[el] = clamped(v[el],min,max);
		}

	MFUNC3
		(void)		_clamp(C0 v[3], C1 min, C2 max)
		{
			v[0] = clamped(v[0],min,max);
			v[1] = clamped(v[1],min,max);
			v[2] = clamped(v[2],min,max);
		}

	MFUNC3
		(void)		_clamp2(C0 v[2], C1 min, C2 max)
		{
			v[0] = clamped(v[0],min,max);
			v[1] = clamped(v[1],min,max);
		}

	MFUNC3
		(void)		_clamp4(C0 v[4], C1 min, C2 max)
		{
			v[0] = clamped(v[0],min,max);
			v[1] = clamped(v[1],min,max);
			v[2] = clamped(v[2],min,max);
			v[3] = clamped(v[3],min,max);
		}

	MFUNC3V
		(void)		_clampV(C0*v, C1 min, C2 max)
		{
			#ifndef __BORLANDC__
				QClamp<C0,C1,C2,Dimensions>::perform(v,min,max);
			#else
				for (count_t i = 0; i < Dimensions; i++)
					v[i] = clamped(v[i],min,max);
			#endif
		}


	MFUNCV
		(void)	_clearV(C*v)
		{
			#ifndef __BORLANDC__
				QSet<C,C,Dimensions>::perform(v,0);
			#else
				for (count_t i = 0; i < Dimensions; i++)
					v[i] = 0;
			#endif
	}

	MFUNC (void) _clear(C*v, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			v[el] = 0;
	}

	MFUNC (void) _clear(C*v)
	{
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
	}

	MFUNC (void) _clear2(C*v)
	{
		v[0] = 0;
		v[1] = 0;
	}

	MFUNC (void) _clear4(C*v)
	{
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		v[3] = 0;
	}

	MFUNC (void) _clear6(C*v)
	{
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
		v[3] = 0;
		v[4] = 0;
		v[5] = 0;
	}


	MFUNC2V (void)	_vectorSortV(C0 v[3], C1 w[3])
	{
		#ifndef __BORLANDC__
			QVectorSort<C0,C1,Dimensions>::perform(v,w);
		#else
		VECTOR_LOOP(Dimensions)
			if (w[el]<v[el])
			{
				C0 save = w[el];
				w[el]=v[el];
				v[el]=save;
			}
		#endif
	}

	MFUNC2 (void) _vectorSort(C0*v, C1*w, count_t el_count)
	{
		VECTOR_LOOP(el_count)
			if (w[el]<v[el])
			{
				C0 save = w[el];
				w[el]=v[el];
				v[el]=save;
			}
	}

	MFUNC2 (void) _vectorSort(C0*v, C1*w)
	{
		C0 save;
		if (w[0]<v[0])
		{
			C0 save = w[0];
			w[0]=v[0];
			v[0]=save;
		}
		if (w[1]<v[1])
		{
			C0 save = w[1];
			w[1]=v[1];
			v[1]=save;
		}
		if (w[2]<v[2])
		{
			C0 save = w[2];
			w[2]=v[2];
			v[2]=save;
		}
	}

	MFUNC4
		(void)		_sphereCoords(C0 pangle, C1 hangle, C2 radius, C3 out[3])
		{
			out[1] = vsin(hangle*M_PI/180)*radius;
			C3	r2 = vcos(hangle*M_PI/180)*radius;
			out[0] = vcos(pangle*M_PI/180)*r2;
			out[2] = vsin(pangle*M_PI/180)*r2;
		}

	MFUNC3
		(void)		_sphereCoordsRad(C0 pangle, C1 hangle, C2 out[3])
		{
			out[1] = vsin(hangle);
			C2	r2 = vcos(hangle);
			out[0] = vcos(pangle)*r2;
			out[2] = vsin(pangle)*r2;
		}

	MFUNC3
		(void)	_v2(C0 v[2], C1 x, C2 y)
		{
			v[0] = (C0)x;
			v[1] = (C0)y;
		}

	MFUNC4
		(void)	_v3(C0 v[3], C1 x, C2 y, C3 z)
		{
			v[0] = (C0)x;
			v[1] = (C0)y;
			v[2] = (C0)z;
		}

	MFUNC5
		(void)	_v4(C0 v[4], C1 x, C2 y, C3 z, C4 a)
		{
			v[0] = (C0)x;
			v[1] = (C0)y;
			v[2] = (C0)z;
			v[3] = (C0)a;
		}
	
	MFUNC2
		(void)	_normal(const C0 v[3], C1 out[3])
		{
			C1	help[3] = {vabs((C1)v[1])+1,(C1)v[2],(C1)v[0]};
			_cross(v,help,out);
		}

	MFUNC2
		(void)		_makeNormalTo(C0 v[3], const C1 reference[3])
		{
			C0 temp[3];
			_cross(v,reference,temp);
			_cross(reference,temp,v);
		}

	MFUNC4
		(C3)		_intercept(const C0 position[3], const C1 velocity[3], C2 interception_velocity, C3 out[3])
		{
			C3	rs[2],
				a = _dot(velocity) - interception_velocity*interception_velocity,
				b = 2*_dot(position,velocity),
				c = _dot(position);
			BYTE num_rs = solveSqrEquation(a, b, c, rs);
			if (!num_rs)
				return 0;
			C3	t = smallestPositiveResult(rs,num_rs);
			if (t<=0)
				return 0;
			_mad(velocity,position,(C3)1/t,out);
			_normalize(out);
			//z_axis = d/t + va;
			return t;
		}
}



#endif
