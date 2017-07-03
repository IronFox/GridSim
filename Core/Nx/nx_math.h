#ifndef nx_mathH
#define nx_mathH

#include "../math/matrix.h"

namespace Math
{
	namespace NxVector
	{
		static const NxVec3	x_axis = NxVec3(1,0,0),
							y_axis = NxVec3(0,1,0),
							z_axis = NxVec3(0,0,1),
							vertical = NxVec3(0,1,0),
							zero = NxVec3(0,0,0),
							null = NxVec3(0,0,0),
							one = NxVec3(1,0,0);
		static NxVec3		dummy = NxVec3(0,0,0);
	};

	namespace NxVec
	{
		template <typename T>
			inline	void	convert(const TVec3<T>&vec,NxVec3&out);
		template <typename T>
			inline	void	convert(const NxVec3&vec,TVec3<T>&out);
		template <typename T>
			inline	NxVec3			convert(const TVec3<T>&vec);
		inline		Vec3<NxReal>	convert(const NxVec3&vec);

		inline		bool	similar(const NxVec3&vec0,const NxVec3&vec1);

		template <typename T>
			inline	bool	similar(const NxVec3&vec0,const TVec3<T>&vec1);
		template <typename T>
			inline	bool	similar(const TVec3<T>&vec0,const NxVec3&vec1);

		inline		bool	similar(const NxVec3&vec0,const NxVec3&vec1,NxReal delta);
		template <typename T>
			inline	bool	similar(const NxVec3&vec0,const TVec3<T>&vec1,T delta);
		template <typename T>
			inline	bool	similar(const TVec3<T>&vec0,const NxVec3&vec1,T delta);
		template <typename T>
			inline	void	div(const TVec3<T>&vec, T scalar, NxVec3&out);
		template <typename T>
			inline	void	mult(const NxVec3&vec, T scalar, TVec3<T>&out);

	}

	namespace NxMat
	{
		template <typename T>
			inline void		convert(const TMatrix4<T>&system,NxMat34&out_matrix);
		template <typename T>
			inline void		convert(const TMatrix3<T>&orientation,NxMat33&out_matrix);
		template <typename T>
			inline void		convert(const NxMat33&m,TMatrix3<T>&out_matrix);
		template <typename T>
			inline void		convert(const NxMat34&matrix,TMatrix4<T>&out_system);
	}
}

#include "nx_math.tpl.h"

#endif

