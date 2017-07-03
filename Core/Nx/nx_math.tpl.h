#ifndef nx_mathTplH
#define nx_mathTplH

namespace Math
{
	namespace NxVec
	{

		template <typename T>
			inline void		convert(const TVec3<T>&vec,NxVec3&out)
			{
				out.x = (NxReal)vec.x;
				out.y = (NxReal)vec.y;
				out.z = (NxReal)vec.z;
			}

		template <typename T>
			inline void		convert(const NxVec3&vec,TVec3<T>&out)
			{
				out.x = (T)vec.x;
				out.y = (T)vec.y;
				out.z = (T)vec.z;
			}

		template <typename T>
			inline	NxVec3			convert(const TVec3<T>&vec)
			{
				return NxVec3(vec.x,vec.y,vec.z);
			}
		inline		Vec3<NxReal>	convert(const NxVec3&vec)
		{
			return Vec3<NxReal>(vec.x,vec.y,vec.z);
		}


		inline	bool _similar(const NxVec3&v,const NxVec3&w)
		{
			return sqr(v.x-w.x)+sqr(v.y-w.y)+sqr(v.z-w.z) < TypeInfo<NxReal>::error * TypeInfo<NxReal>::error;
		}

		template <typename T>
			inline bool similar(const NxVec3&v,const TVec3<T>&w)
			{
				return sqr(v.x-w.x)+sqr(v.y-w.y)+sqr(v.z-w.z) < TypeInfo<NxReal>::error*TypeInfo<T>::error;
			}
	
		template <typename T>
			inline bool similar(const TVec3<T>&w,const NxVec3&v)
			{
				return sqr(v.x-w.x)+sqr(v.y-w.y)+sqr(v.z-w.z) < TypeInfo<NxReal>::error * TypeInfo<T>::error;
			}



		inline	bool similar(const NxVec3&v,const NxVec3&w, NxReal delta)
		{
			return sqr(v.x-w.x)+sqr(v.y-w.y)+sqr(v.z-w.z) < delta*delta;
		}

		template <typename T>
			inline bool similar(const NxVec3&v,const TVec3<T>&w, T delta)
			{
				return sqr(v.x-w.x)+sqr(v.y-w.y)+sqr(v.z-w.z) < delta*delta;
			}
	
		template <typename T>
			inline bool similar(const TVec3<T>&w,const NxVec3&v, T delta)
			{
				return sqr(v.x-w.x)+sqr(v.y-w.y)+sqr(v.z-w.z) < delta*delta;
			}

		template <typename T>
			inline	void	div(const TVec3<T>&vec, T scalar, NxVec3&out)
			{
				out.x = vec.x/scalar;
				out.y = vec.y/scalar;
				out.z = vec.z/scalar;
			}
	
		template <typename T>
			inline	void	mult(const NxVec3&vec, T scalar, TVec3<T>& out)
			{
				out.x = vec.x*scalar;
				out.y = vec.y*scalar;
				out.z = vec.z*scalar;
			}

	}

	namespace NxMat
	{
		template <typename T>
			void convert(const TMatrix4<T>& system,NxMat34&out)
			{
				out.t.x = (NxReal)system.w.x;
				out.t.y = (NxReal)system.w.y;
				out.t.z = (NxReal)system.w.z;
	
				NxReal sys[9] =
							{
								system.x.x,
								system.x.y,
								system.x.z,

								system.y.x,
								system.y.y,
								system.y.z,

								system.z.x,
								system.z.y,
								system.z.z
							};
				out.M.setColumnMajor(sys);	//may have to replace with setRowMajorStride4 depending on what 'major' means...

			}

		template <typename T>
			inline void convert(const NxMat34&m,TMatrix4<T>& out)
			{
				Vec::convert(m.t,out.w.xyz);
	
				NxReal sys[9];
	
				m.M.getColumnMajor(sys);
				out.x.x = sys[0];
				out.x.y = sys[1];
				out.x.z = sys[2];
				out.y.x = sys[3];
				out.y.y = sys[4];
				out.y.z = sys[5];
				out.z.x = sys[6];
				out.z.y = sys[7];
				out.z.z = sys[8];

				Mat::resetBottomRow(out);
			}

		template <>
			inline void convert(const TMatrix4<NxReal>&system,NxMat34&out)
			{
				out.t.x = system.w.x;
				out.t.y = system.w.y;
				out.t.z = system.w.z;
	
				out.M.setColumnMajorStride4(system.v);	//may have to replace with setRowMajorStride4 depending on what 'major' means...
			}

		template <>
			inline void convert(const TMatrix3<NxReal>&system,NxMat33&out)
			{
				out.setColumnMajor(system.v);	//may have to replace with getRowMajor depending on what 'major' means...
			}

		template<>
			inline void  convert(const NxMat34&m,TMatrix4<NxReal>&out)
			{
				out.w.x = m.t.x;
				out.w.y = m.t.y;
				out.w.z = m.t.z;
	
				m.M.getColumnMajorStride4(out.v);
				Mat::resetBottomRow(out);
			}

		template <typename T>
			inline void		convert(const NxMat33&m,TMatrix3<T>&out_matrix)
			{
				m.getColumnMajor(out_matrix.v);
			}

	}
}

#endif

