#ifndef mclassesTplH
#define mclassesTplH

/******************************************************************

Collection of matrix-classes.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/



namespace Math
{

    template <class C, unsigned Rows, unsigned Cols>    MF_CONSTRUCTOR          VariableMatrix<C,Rows,Cols>::VariableMatrix()
    {}

    template <class C, unsigned Rows, unsigned Cols>    MF_CONSTRUCTOR          VariableMatrix<C,Rows,Cols>::VariableMatrix(const VariableMatrix<C,Rows,Cols>& m)
    {
        VecUnroll<Rows*Cols>::copy(m.field,field);
    }

    template <class C, unsigned Rows, unsigned Cols>    MF_DECLARE  (void)    VariableMatrix<C,Rows,Cols>::loadIdentity()
    {
        if (Cols == Rows)
            __eye<C,Rows>(field);
        else
            MATRIX_LOOP(Cols,Rows)
                vector[x][y] = x==y;
    }



    template <class C, unsigned Rows, unsigned Cols>    MF_INIT VariableMatrix<C,Rows,Cols> MF_CC VariableMatrix<C,Rows,Cols>::invert()
    {
        VariableMatrix<C,Rows,Cols>   result;
        if (Rows != Cols)
            return result;
        __invertMatrix<C,C,Rows>(field,result.field);
        return result;
    }
    
    template <class C, unsigned Rows, unsigned Cols>
    template <class C0> MF_DECLARE(void)                      VariableMatrix<C,Rows,Cols>::loadInvert(const VariableMatrix<C0,Rows,Cols>&m)
    {
        if (Rows != Cols)
            return;
        __invertMatrix<C0,C,Rows>(m.field,field);
    }


    template <class C, unsigned Rows, unsigned Cols>    MF_INIT VariableMatrix<C,Cols,Rows> MF_CC VariableMatrix<C,Rows,Cols>::transpose()
    {
        VariableMatrix<C,Cols,Rows>   result;
        MATRIX_LOOP(Cols,Rows)
            result.vector[y][x] = vector[x][y];
        return result;
    }
    
    template <class C, unsigned Rows, unsigned Cols>
    template <class C0> MF_DECLARE(void) VariableMatrix<C,Rows,Cols>::loadTranspose(const VariableMatrix<C0,Cols,Rows>&m)
    {
//        __transpose<C0,C,Cols,Rows>(m.system,system);
        MATRIX_LOOP(Cols,Rows)
            vector[y][x] = m.vector[x][y];
    }

    template <class C, unsigned Rows, unsigned Cols> MF_DECLARE(C)    VariableMatrix<C,Rows,Cols>::determinant()
    {
        return __determinant<C,Rows,Cols>(field);
    }

    template <class C, unsigned Rows, unsigned Cols>
    template <class C0>                                 MF_DECLARE(void)                    VariableMatrix<C,Rows,Cols>::operator=(const VariableMatrix<C0,Rows,Cols>&m)
    {
        VecUnroll<Rows*Cols>::copy(m.field,field);
    }

    template <class C, unsigned Rows, unsigned Cols>
    template <class C0>                                 MF_INIT  VariableMatrix<C,Rows,Cols> MF_CC   VariableMatrix<C,Rows,Cols>::operator+(const VariableMatrix<C0,Rows,Cols>&m)                           const
    {
        VariableMatrix<C,Rows,Cols>   result;
		VecUnroll<Rows*Cols>::add(field,m.field,result.field);
        return result;
    }

    template <class C, unsigned Rows, unsigned Cols>
    template <class C0>                                 MF_INIT  VariableMatrix<C,Rows,Cols> MF_CC  VariableMatrix<C,Rows,Cols>::operator-(const VariableMatrix<C0,Rows,Cols>&m)                           const
    {
        VariableMatrix<C,Rows,Cols>   result;
		VecUnroll<Rows*Cols>::sub(field,m.field,result.field);
        return result;
    }

    template <class C, unsigned Rows, unsigned Cols>
    template <class C0, unsigned Cols0>                 MF_INIT  VariableMatrix<C,Rows,Cols0> MF_CC VariableMatrix<C,Rows,Cols>::operator*(const VariableMatrix<C0,Cols,Cols0>&m)                          const
    {
        VariableMatrix<C,Rows,Cols0>  result;
        __multiply<C,C0,C,Rows,Cols,Cols0>(field,m.field,result.field);
        return result;
    }

    template <class C, unsigned Rows, unsigned Cols>
    template <class C0>                                 MF_INIT  VecN<C,Rows>   MF_CC       VariableMatrix<C,Rows,Cols>::operator*(const VecN<C0,Cols>&v)                                const
    {
        TVecN<C,Rows>  result;
        __multiply<C,C0,C,Rows,Cols,1>(field,v.v,result.v);
        return result;
    }

    template <class C, unsigned Rows, unsigned Cols>
    MFUNC1  (bool)                    VariableMatrix<C,Rows,Cols>::operator==(const VariableMatrix<C0,Rows,Cols>&m)                                const
    {
        return VecUnroll<Rows*Cols>::similar(field,m.field,TypeInfo<C>::error);
    }

    template <class C, unsigned Rows, unsigned Cols>
    MFUNC1  (void)                    VariableMatrix<C,Rows,Cols>::operator+=(const VariableMatrix<C0,Rows,Cols>&m)
    {
		VecUnroll<Rows*Cols>::add(field,m.field);
    }

    template <class C, unsigned Rows, unsigned Cols>
    MFUNC1  (void)                    VariableMatrix<C,Rows,Cols>::operator-=(const VariableMatrix<C0,Rows,Cols>&m)
    {
		VecUnroll<Rows*Cols>::sub(field,m.field);
    }

    template <class C, unsigned Rows, unsigned Cols>
    MFUNC1  (void)                    VariableMatrix<C,Rows,Cols>::operator*=(const VariableMatrix<C0,Cols,Cols>&m)
    {
        C   temp[Rows*Cols];
        __multiply<C,C0,C,Rows,Cols,Cols>(field,m.field,temp);
		VecUnroll<Rows*Cols>::copy(temp,field);
    }

    template <class C, unsigned Rows, unsigned Cols>    MF_DECLARE  (String)                VariableMatrix<C,Rows,Cols>::toString()
    {
        return __toString(field,Rows,Cols);
    }






    template <class C> MF_DECLARE  (void) SystemMatrix<C>::loadIdentity()
    {
		Mat::eye(*this);
    }

    template <class C>
    MFUNC1  (void) SystemMatrix<C>::loadInvert(const SystemMatrix<C0>&sys)
    {
        Mat::invertSystem(sys,*this);
    }
    
    template <class C>
    MFUNC4 (void) SystemMatrix<C>::buildRotation(const C0&angle, const C1&x, const C2&y, const C3&z)
    {
		TMatrix3<C>	temp;
		Mat::rotationMatrix(angle,x,y,z,temp);
		x.xyz = temp.x;
		y.xyz = temp.y;
		z.xyz = temp.z;
		Vec::clear(w.xyz);
		Mat::resetBottomRow(*this);
    }

    template <class C>
    MFUNC2 (void) SystemMatrix<C>::buildRotation(const C0&angle, const TVec3<C1>& axis)
    {
		TMatrix3<C>	temp;
		Mat::rotationMatrix(angle,axis,temp);
		x.xyz = temp.x;
		y.xyz = temp.y;
		z.xyz = temp.z;
		Vec::clear(w.xyz);
		Mat::resetBottomRow(*this);
    }
	
	template <class C> MFUNC4 (void)	SystemMatrix<C>::rotate(const C0&angle, const C1&axis_x, const C2&axis_y, const C3&axis_z)
		{
			TMatrix3<C>	rsystem;
			Mat::rotationMatrix(angle,axis_x,axis_y,axis_z,rsystem);

			Mat::mult(rsystem,x.xyz);
			Mat::mult(rsystem,y.xyz);
			Mat::mult(rsystem,z.xyz);
			Mat::mult(rsystem,w.xyz);
		}
	
	template <class C> MFUNC2 (void)	SystemMatrix<C>::rotate(const C0&angle, const TVec3<C1>& axis)
		{
			TMatrix3<C>	rsystem;
			Mat::rotationMatrix(angle,axis,rsystem);

			Mat::mult(rsystem,x.xyz);
			Mat::mult(rsystem,y.xyz);
			Mat::mult(rsystem,z.xyz);
			Mat::mult(rsystem,w.xyz);
		}
    
    template <class C>
    MFUNC2 (void) SystemMatrix<C>::buildSystem(const TVec3<C0>&position, const TVec3<C1>& axis, BYTE direction)
    {
		Mat::makeAxisSystem(position,axis,direction,*this);
    }

    template <class C> MF_DECLARE  (void) SystemMatrix<C>::normalize()
    {
		Vec::normalize(x.xyz);
		Vec::normalize(y.xyz);
		Vec::normalize(z.xyz);
    }
    
    template <class C>
    MFUNC1  (void) SystemMatrix<C>::operator+=(const TVec3<C0>&vec)
    {
        Vec::add(w.xyz,vec.v);
    }

    template <class C>
    MFUNC1  (void) SystemMatrix<C>::operator-=(const SystemMatrix<C0>&sys)
    {
        Vec::sub(x.xyz,sys.x.xyz);
        Vec::sub(y.xyz,sys.y.xyz);
        Vec::sub(z.xyz,sys.z.xyz);
        Vec::sub(w.xyz,sys.w.xyz);
    }
    
    template <class C>
    MFUNC1  (void) SystemMatrix<C>::operator-=(const TVec3<C0>&vec)
    {
        Vec::sub(w.xyz,vec);
    }


    template <class C>
    MFUNC1  (void) SystemMatrix<C>::operator+=(const SystemMatrix<C0>&other)
    {
        Vec::add(x.xyz,sys.x.xyz);
        Vec::add(y.xyz,sys.y.xyz);
        Vec::add(z.xyz,sys.z.xyz);
        Vec::add(w.xyz,sys.w.xyz);
    }

    template <class C>
    MFUNC1  (void) SystemMatrix<C>::operator*=(const SystemMatrix<C0>&other)
    {
        TVec3<C>   _x,_y,_z,_b;
		Mat::rotate(*this,other.x.xyz,_x);
		Mat::rotate(*this,other.y.xyz,_y);
		Mat::rotate(*this,other.z.xyz,_z);
		Mat::transform(*this,other.w.xyz,_b);
		x.xyz = _x;
		y.xyz = _y;
		z.xyz = _z;
		w.xyz = _b;
    }

    template <class C>
    MFUNC1  (SystemMatrix<C>) SystemMatrix<C>::operator+(const SystemMatrix<C0>&other)    const
    {
        SystemMatrix<C>    result;
        Vec::add(x.xyz,other.x.xyz,result.x.xyz);
        Vec::add(y.xyz,other.y.xyz,result.y.xyz);
        Vec::add(z.xyz,other.z.xyz,result.z.xyz);
        Vec::add(w.xyz,other.w.xyz,result.w.xyz);
		Mat::resetBottomRow(result);
        return result;
    }

    template <class C>
    MFUNC1  (SystemMatrix<C>) SystemMatrix<C>::operator+(const TVec3<C0>&translation)    const
    {
        SystemMatrix<C>    result = *this;
		Vec::add(result.w.xyz,translation);
        return result;
    }

    template <class C>
    MFUNC1  (SystemMatrix<C>) SystemMatrix<C>::operator-(const SystemMatrix<C0>&other)    const
    {
        SystemMatrix<C>    result;
        Vec::sub(x.xyz,other.x.xyz,result.x.xyz);
        Vec::sub(y.xyz,other.y.xyz,result.y.xyz);
        Vec::sub(z.xyz,other.z.xyz,result.z.xyz);
        Vec::sub(w.xyz,other.w.xyz,result.w.xyz);
		Mat::resetBottomRow(result);
        return result;
    }

    template <class C>
    MFUNC1  (SystemMatrix<C>) SystemMatrix<C>::operator-(const TVec3<C0>&translation)    const
    {
        SystemMatrix<C>    result = *this;
		Vec::sub(result.w.xyz,translation);
        return result;
    }

    template <class C>
    MFUNC1  (SystemMatrix<C>)   SystemMatrix<C>::operator*(const SystemMatrix<C0>&other)   const
    {
        SystemMatrix<C>       result;
		Mat::rotate(*this,other.x.xyz,result.x.xyz);
		Mat::rotate(*this,other.y.xyz,result.y.xyz);
		Mat::rotate(*this,other.z.xyz,result.z.xyz);
		Mat::transform(*this,other.w.xyz,result.w.xyz);
		Mat::resetBottomRow(result);
        return result;
    }

    template <class C>
    MFUNC1  (TVec3<C>)   SystemMatrix<C>::operator*(const TVec3<C0>&other) const
    {
		TVec3<C> result;
		Mat::transform(*this,other,result);
		return result;
    }

    template <class C>
    MFUNC1  (TVec4<C>)   SystemMatrix<C>::operator*(const TVec4<C0>&other) const
    {
		TVec4<C> result;
		Mat::mult(*this,other,result);
		return result;
    }


    template <class C>
    MFUNC1 (Line<C>)  SystemMatrix<C>::operator*(const Line<C0>&other) const
    {
        Line<C>  result;
        Mat::transform(*this,other.position,result.position);
        Mat::rotate(*this,other.direction,result.direction);
        return result;
    }


    template <class C>  MF_DECLARE(SystemMatrix<C>)    SystemMatrix<C>::invert()                         const
    {
        SystemMatrix<C>    result;
		Mat::invertSystem(*this,result.system);
        return result;
    }

    template <class C>  MF_DECLARE(SystemMatrix<C>&)    SystemMatrix<C>::invertThis()
    {
        TMatrix4<C>    temp=*this;
        Mat::invertSystem(temp,*this);
        return *this;
    }

    template <class C>  MF_DECLARE(String)    SystemMatrix<C>::toString()                        const
    {
        return __toString(v,4,4);
    }
    
    
}

#endif

