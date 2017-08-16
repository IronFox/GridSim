#ifndef mclassesTplH
#define mclassesTplH

/******************************************************************

Collection of matrix-classes.

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
        return VecUnroll<Rows*Cols>::Similar(field,m.field,TypeInfo<C>::error);
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

    template <class C, unsigned Rows, unsigned Cols>    MF_DECLARE  (String)                VariableMatrix<C,Rows,Cols>::ToString()
    {
        return M::Mat::Raw::ToString4x4(field,Rows,Cols);
    }






    template <class C> MF_DECLARE  (void) SystemMatrix<C>::loadIdentity()
    {
		M::Mat::Eye(*this);
    }

    template <class C>
    MFUNC1  (void) SystemMatrix<C>::loadInvert(const SystemMatrix<C0>&sys)
    {
        M::Mat::invertSystem(sys,*this);
    }
    
    template <class C>
    MFUNC4 (void) SystemMatrix<C>::buildRotation(const C0&angle, const C1&x, const C2&y, const C3&z)
    {
		M::TMatrix3<C>	temp;
		M::Mat::BuildRotationMatrix(angle,x,y,z,temp);
		x.xyz = temp.x;
		y.xyz = temp.y;
		z.xyz = temp.z;
		M::Vec::clear(w.xyz);
		M::Mat::resetBottomRow(*this);
    }

    template <class C>
    MFUNC2 (void) SystemMatrix<C>::buildRotation(const C0&angle, const M::TVec3<C1>& axis)
    {
		M::TMatrix3<C>	temp;
		M::Mat::BuildRotationMatrix(angle,axis,temp);
		x.xyz = temp.x;
		y.xyz = temp.y;
		z.xyz = temp.z;
		M::Vec::clear(w.xyz);
		M::Mat::resetBottomRow(*this);
    }
	
	template <class C> MFUNC4 (void)	SystemMatrix<C>::rotate(const C0&angle, const C1&axis_x, const C2&axis_y, const C3&axis_z)
		{
			M::TMatrix3<C>	rsystem;
			M::Mat::BuildRotationMatrix(angle,axis_x,axis_y,axis_z,rsystem);

			M::Mat::Mult(rsystem,x.xyz);
			M::Mat::Mult(rsystem,y.xyz);
			M::Mat::Mult(rsystem,z.xyz);
			M::Mat::Mult(rsystem,w.xyz);
		}
	
	template <class C> MFUNC2 (void)	SystemMatrix<C>::rotate(const C0&angle, const M::TVec3<C1>& axis)
		{
			M::TMatrix3<C>	rsystem;
			M::Mat::BuildRotationMatrix(angle,axis,rsystem);

			M::Mat::Mult(rsystem,x.xyz);
			M::Mat::Mult(rsystem,y.xyz);
			M::Mat::Mult(rsystem,z.xyz);
			M::Mat::Mult(rsystem,w.xyz);
		}
    
    template <class C>
    MFUNC2 (void) SystemMatrix<C>::buildSystem(const M::TVec3<C0>&position, const M::TVec3<C1>& axis, BYTE direction)
    {
		M::Mat::makeAxisSystem(position,axis,direction,*this);
    }

    template <class C> MF_DECLARE  (void) SystemMatrix<C>::normalize()
    {
		M::Vec::normalize(x.xyz);
		M::Vec::normalize(y.xyz);
		M::Vec::normalize(z.xyz);
    }
    
    template <class C>
    MFUNC1  (void) SystemMatrix<C>::operator+=(const M::TVec3<C0>&vec)
    {
        M::Vec::add(w.xyz,vec.v);
    }

    template <class C>
    MFUNC1  (void) SystemMatrix<C>::operator-=(const SystemMatrix<C0>&sys)
    {
        M::Vec::sub(x.xyz,sys.x.xyz);
        M::Vec::sub(y.xyz,sys.y.xyz);
        M::Vec::sub(z.xyz,sys.z.xyz);
        M::Vec::sub(w.xyz,sys.w.xyz);
    }
    
    template <class C>
    MFUNC1  (void) SystemMatrix<C>::operator-=(const M::TVec3<C0>&vec)
    {
        M::Vec::sub(w.xyz,vec);
    }


    template <class C>
    MFUNC1  (void) SystemMatrix<C>::operator+=(const SystemMatrix<C0>&other)
    {
        M::Vec::add(x.xyz,sys.x.xyz);
        M::Vec::add(y.xyz,sys.y.xyz);
        M::Vec::add(z.xyz,sys.z.xyz);
        M::Vec::add(w.xyz,sys.w.xyz);
    }

    template <class C>
    MFUNC1  (void) SystemMatrix<C>::operator*=(const SystemMatrix<C0>&other)
    {
        M::TVec3<C>   _x,_y,_z,_b;
		M::Mat::rotate(*this,other.x.xyz,_x);
		M::Mat::rotate(*this,other.y.xyz,_y);
		M::Mat::rotate(*this,other.z.xyz,_z);
		M::Mat::Transform(*this,other.w.xyz,_b);
		x.xyz = _x;
		y.xyz = _y;
		z.xyz = _z;
		w.xyz = _b;
    }

    template <class C>
    MFUNC1  (SystemMatrix<C>) SystemMatrix<C>::operator+(const SystemMatrix<C0>&other)    const
    {
        SystemMatrix<C>    result;
        M::Vec::add(x.xyz,other.x.xyz,result.x.xyz);
        M::Vec::add(y.xyz,other.y.xyz,result.y.xyz);
        M::Vec::add(z.xyz,other.z.xyz,result.z.xyz);
        M::Vec::add(w.xyz,other.w.xyz,result.w.xyz);
		M::Mat::resetBottomRow(result);
        return result;
    }

    template <class C>
    MFUNC1  (SystemMatrix<C>) SystemMatrix<C>::operator+(const M::TVec3<C0>&translation)    const
    {
        SystemMatrix<C>    result = *this;
		M::Vec::add(result.w.xyz,translation);
        return result;
    }

    template <class C>
    MFUNC1  (SystemMatrix<C>) SystemMatrix<C>::operator-(const SystemMatrix<C0>&other)    const
    {
        SystemMatrix<C>    result;
        M::Vec::sub(x.xyz,other.x.xyz,result.x.xyz);
        M::Vec::sub(y.xyz,other.y.xyz,result.y.xyz);
        M::Vec::sub(z.xyz,other.z.xyz,result.z.xyz);
        M::Vec::sub(w.xyz,other.w.xyz,result.w.xyz);
		M::Mat::resetBottomRow(result);
        return result;
    }

    template <class C>
    MFUNC1  (SystemMatrix<C>) SystemMatrix<C>::operator-(const M::TVec3<C0>&translation)    const
    {
        SystemMatrix<C>    result = *this;
		M::Vec::sub(result.w.xyz,translation);
        return result;
    }

    template <class C>
    MFUNC1  (SystemMatrix<C>)   SystemMatrix<C>::operator*(const SystemMatrix<C0>&other)   const
    {
        SystemMatrix<C>       result;
		M::Mat::rotate(*this,other.x.xyz,result.x.xyz);
		M::Mat::rotate(*this,other.y.xyz,result.y.xyz);
		M::Mat::rotate(*this,other.z.xyz,result.z.xyz);
		M::Mat::Transform(*this,other.w.xyz,result.w.xyz);
		M::Mat::resetBottomRow(result);
        return result;
    }

    template <class C>
    MFUNC1  (M::TVec3<C>)   SystemMatrix<C>::operator*(const M::TVec3<C0>&other) const
    {
		M::TVec3<C> result;
		M::Mat::Transform(*this,other,result);
		return result;
    }

    template <class C>
    MFUNC1  (M::TVec4<C>)   SystemMatrix<C>::operator*(const M::TVec4<C0>&other) const
    {
		M::TVec4<C> result;
		M::Mat::Mult(*this,other,result);
		return result;
    }


    template <class C>
    MFUNC1 (Line<C>)  SystemMatrix<C>::operator*(const Line<C0>&other) const
    {
        Line<C>  result;
        M::Mat::Transform(*this,other.position,result.position);
        M::Mat::rotate(*this,other.direction,result.direction);
        return result;
    }


    template <class C>  MF_DECLARE(SystemMatrix<C>)    SystemMatrix<C>::invert()                         const
    {
        SystemMatrix<C>    result;
		M::Mat::invertSystem(*this,result.system);
        return result;
    }

    template <class C>  MF_DECLARE(SystemMatrix<C>&)    SystemMatrix<C>::invertThis()
    {
        M::TMatrix4<C>    temp=*this;
        M::Mat::invertSystem(temp,*this);
        return *this;
    }

    template <class C>  MF_DECLARE(String)    SystemMatrix<C>::ToString()                        const
    {
        return M::Mat::Raw::ToString4x4(v,4,4);
    }
    
    
}

#endif

