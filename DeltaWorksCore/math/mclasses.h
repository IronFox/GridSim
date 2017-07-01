#ifndef mclassesH
#define mclassesH

/******************************************************************

Collection of matrix-classes.

******************************************************************/


#include "matrix.h"
#include "vclasses.h"

namespace Math
{
	/** matrix-classes **
	
		VariableMatrix represents a MxN-matrix compatible to the VecN-class.
		when multiplying a VariableMatrix with a VecN make sure, the in- and out-vectors match the corresponding
		matrix-width /-height.
		
		SystemMatrix is an optimized 4x4-matrix in the following form:

		v0x v1x v2x bx
		v0y v1y v2y by
		v0z v1z v2z bz
		0	0	0	1
		
		this form must be maintained at all times.
		SystemMatrix is compatible to Vec3, Vec4 and Line
		
		
		
	
	
	*/



	template <class, unsigned, unsigned> class VariableMatrix;
	template <class> class SystemMatrix;




	template <class C, unsigned Rows, unsigned Cols>class	VariableMatrix
	{
	public:
		union
		{
						C								vector[Cols][Rows];
						C								field[Cols*Rows];
		};

			MF_CONSTRUCTOR								VariableMatrix();
			MF_CONSTRUCTOR								VariableMatrix(const VariableMatrix<C,Rows,Cols>& m);
			MF_DECLARE(void)							loadIdentity();
			MFUNC1 (void)								loadInvert(const VariableMatrix<C0,Rows,Cols>&m);
			MFUNC1 (void)								loadTranspose(const VariableMatrix<C0,Cols,Rows>&m);
			MF_INIT VariableMatrix<C,Rows,Cols> MF_CC			invert();
			MF_INIT VariableMatrix<C,Cols,Rows> MF_CC			transpose();
			MF_DECLARE(C)								determinant();
			MF_DECLARE(String)							ToString();
			MFUNC1 (void)								operator=(const VariableMatrix<C0,Rows,Cols>&m);
		template <class C0>
			MF_INIT VariableMatrix<C,Rows,Cols> MF_CC			operator+(const VariableMatrix<C0,Rows,Cols>&m)							const;
		template <class C0>
			MF_INIT VariableMatrix<C,Rows,Cols> MF_CC			operator-(const VariableMatrix<C0,Rows,Cols>&m)							const;
		template <class C0, unsigned Cols0>
			MF_INIT VariableMatrix<C,Rows,Cols0> MF_CC			operator*(const VariableMatrix<C0,Cols,Cols0>&m)							const;

		template <class C0>
			MF_INIT VecN<C,Rows> MF_CC					operator*(const VecN<C0,Cols>&v)									const;
			MFUNC1 (bool)								operator==(const VariableMatrix<C0,Rows,Cols>&m)							const;
			MFUNC1 (void)								operator+=(const VariableMatrix<C0,Rows,Cols>&m);
			MFUNC1 (void)								operator-=(const VariableMatrix<C0,Rows,Cols>&m);
			MFUNC1 (void)								operator*=(const VariableMatrix<C0,Cols,Cols>&m);
	};




	template <class C>
		class SystemMatrix : public TMatrix4<C>
		{
		public:

		MF_DECLARE(void)						loadIdentity();							//loads the identity-matrix to the local data
		MFUNC1 (void)							loadInvert(const SystemMatrix<C0>&);			//creates the system-invert of the given matrix in the local data
		MFUNC4 (void)							buildRotation(const C0&angle, const C1&ax, const C2&ay, const C3&az);
		MFUNC2 (void)							buildRotation(const C0&angle, const TVec3<C1>& axis);
		MFUNC2 (void)							buildSystem(const TVec3<C0>&position, const TVec3<C1>& axis, BYTE direction);
		MFUNC4 (void)							rotate(const C0&angle, const C1&axis_x, const C2&axis_y, const C3&axis_z);
		MFUNC2 (void)							rotate(const C0&angle, const TVec3<C1>&axis);
		MF_DECLARE(void)						normalize();							//normalizes v0,v1 und v2
		MF_DECLARE(SystemMatrix<C>&)			invertThis();
		MF_DECLARE(SystemMatrix<C>)				invert()								const;
		MF_DECLARE(String)						ToString()								const;
		//faster operators:
		MFUNC1 (void)							operator+=(const SystemMatrix<C0>&);			//adds all coords from the given system to the local data
		MFUNC1 (void)							operator+=(const TVec3<C0>&);			//translates the system
		MFUNC1 (void)							operator-=(const SystemMatrix<C0>&);
		MFUNC1 (void)							operator-=(const TVec3<C0>&);
		MFUNC1 (void)							operator*=(const SystemMatrix<C0>&);
		//slower operators:
		MFUNC1 (SystemMatrix<C>)				operator+(const TVec3<C0>&)				const;
		MFUNC1 (SystemMatrix<C>)				operator+(const SystemMatrix<C0>&)		const;
		MFUNC1 (SystemMatrix<C>)				operator-(const TVec3<C0>&)				const;
		MFUNC1 (SystemMatrix<C>)				operator-(const SystemMatrix<C0>&)		const;
		MFUNC1 (SystemMatrix<C>)				operator*(const SystemMatrix<C0>&)		const;
		MFUNC1 (TVec3<C>)						operator*(const TVec3<C0>&)				const;
		MFUNC1 (TVec4<C>)						operator*(const TVec4<C0>&)				const;
		MFUNC1 (Line<C>)						operator*(const Line<C0>&)				const;
	
		};


}

using namespace Math;


#include "mclasses.tpl.h"


#endif
