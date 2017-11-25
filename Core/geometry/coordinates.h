#ifndef coordinatesH
#define coordinatesH


#include "../math/vclasses.h"
#include "../math/matrix.h"

namespace DeltaWorks
{
	namespace Math
	{
		/**
		@brief Integer offset coordinate structures
		*/
		namespace Composite
		{

			extern const float		default_sector_size;	//!< Global default sector size variable.Set to 50000.0f
	
			typedef	int		sector_t;	//!< Sector type. For now common integers. May enhance this to 64bit integers in the future

			/**
				@brief Divides a non-structure composite by a scalar value
				The result is written back to the composite itself.
		
				@param sector Sector offset of the composite to divide
				@param remainder Remainder of the composite to divide
				@param scalar Value to divide by
				@param sector_size Applied sector size for this operation
			*/
			inline	void			divComposite(sector_t&sector, float&remainder, float scalar, float sector_size)
			{
				remainder /= scalar;
				sector_t new_sector = (sector_t)(sector/scalar);
				if ((sector_t)(new_sector*scalar) != sector)
					remainder += (sector-(sector_t)(new_sector*scalar))*sector_size/scalar;
				sector = new_sector;							
			}
	
			/**
				@brief Divides a non-structure composite by an integer value
				The result is written back to the composite itself.
		
				@param sector Sector offset of the composite to divide
				@param remainder Remainder of the composite to divide
				@param scalar Value to divide by
				@param sector_size Applied sector size for this operation
			*/
			inline	void			divIntComposite(sector_t&sector, float&remainder, int scalar, float sector_size)
			{
				remainder /= scalar;
				sector_t	rest = sector%scalar;
				sector /= scalar;
				if (rest!=0)
					remainder += (float)rest/(float)scalar*sector_size;
			}
	
			/**
				@brief Compares two non-structure composites
				The result is:
				- #-1 if the first composite @a a is less than the second composite @a b
				- #0 if the two composites are closer than the floating point error constant
				- #+1 if the first composite @a a is greater than the second composite @a b
		
				@param sector_a Sector offset of the first composite
				@param remainder_a Remainder of the first composite 
				@param sector_b Sector offset of the second composite
				@param remainder_b Remainder of the second composite 
				@param sector_size Applied sector size for this operation
			*/
			inline	char			compareComposites(sector_t sector_a, float remainder_a, sector_t sector_b, float remainder_b, float sector_size)
			{
				float delta = (sector_a-sector_b)*sector_size+(remainder_a-remainder_b);
				if (delta > TypeInfo<float>::error)
					return 1;
				if (delta < TypeInfo<float>::error)
					return -1;
				return 0;
			}
	
			template <typename T0, typename T1>
				inline	void		addToComposite(sector_t&sector,float&remainder,T0 value,T1 sector_size)
				{
					sector_t	sdelta = (sector_t)(value/sector_size);
					value -= sdelta * sector_size;
					sector += sdelta;
					remainder += value;
				}

	
	
			/**
				@brief Linear integer offset floating point value
	
			*/
			class Scalar
			{
			public:
					sector_t		sector;
					float			remainder;
			
					void			wrap(float sector_size);			//!< Modulates remainder value if it exceeds the specified sector size and increments/decrements the sector offset as appropriate. In general assures that the local composite stays within its optimal boundaries. Should be called whenever the remainder was manually altered
			
									Scalar()
									{}
									Scalar(sector_t sector_,float remainder_):sector(sector_),remainder(remainder_)
									{}
			
					float			toFloat(float sector_size)	const
									{
										return (float)(sector*sector_size)+remainder;
									}
							
					double			toDouble(double sector_size)	const
									{
										return (double)(sector*sector_size)+(double)remainder;
									}
							
					void			div(float scalar, float sector_size)
									{
										divComposite(sector,remainder,scalar,sector_size);
									}
					void			divInt(int scalar, float sector_size)
									{
										divIntComposite(sector,remainder,scalar,sector_size);
									}
					void			mult(float scalar, float sector_size)	//must test
									{
										remainder *= scalar;
										sector_t new_sector = (sector_t)(sector*scalar);
										if ((sector_t)(new_sector/scalar) != sector)
											remainder -= (sector-(sector_t)(new_sector/scalar))*sector_size*scalar;
										sector = new_sector;
									}
					char			CompareTo(const Scalar&other, float sector_size)	const	//! Compares the local composite to the specified one and returns the result. @param other Composite to compare to @param sector_size Sector size used for comparison @return -1 if the local object is less than the specified other composite, 0 if they are similar, 1 if the local composite is greater
									{
										return compareComposites(this->sector,this->remainder,other.sector,other.remainder,sector_size);
									}
							
					String			ConvertToString(float sector_size)	const
									{
										return "["+String(sector)+"] "+String(remainder)+" (="+String(sector*sector_size+remainder)+")";
									}
							
					friend String	ToString(const Scalar&s)
									{
										using StringConversion::ToString;
										return "["+ToString(s.sector)+"] "+ToString(s.remainder);
									}
					friend StringW	ToStringW(const Scalar&s)
									{
										using StringConversion::ToStringW;
										return L"["+ToStringW(s.sector)+L"] "+ToStringW(s.remainder);
									}

					operator bool() const	{return sector != 0 || remainder != 0;}

			};
	
	
			inline void		sub(const Scalar&a, const Scalar&b, Scalar&result)
							{
								result.sector = a.sector-b.sector;
								result.remainder = a.remainder-b.remainder;
							}
			inline Scalar	sub(const Scalar&a, const Scalar&b)
							{
								return Scalar(a.sector-b.sector,a.remainder-b.remainder);
							}
			inline void		add(const Scalar&a, const Scalar&b, Scalar&result)
							{
								result.sector = a.sector+b.sector;
								result.remainder = a.remainder+b.remainder;
							}

			inline void		center(const Scalar&a, const Scalar&b, Scalar&result, float sector_size)	//! Determines the center composite exactly between @b a and @b b and writes the result to @b result
							{
								add(a,b,result);
								result.divInt(2,sector_size);
								result.wrap(sector_size);
					
							}
	
	
	
			/**
				@brief Integer offset coordinates
		
				Enhances common single precision floating point coordinates by integer offset coordinates of variable sector size.
			*/
			class Coordinates
			{
			public:
					typedef Composite::sector_t	sector_t;
	
					TVec3<sector_t>	sector;				//!< Coordinate offset
					TVec3<>			remainder;			//!< Floating point remainder
			static	const float		default_sector_size;	//!< Global default sector size variable.Set to 50000.0f
			
					void			wrap(float sector_size);			//!< Modulates remainder values that exceed the specified sector size and increments/decrements the respective sector offset as appropriate. In general assures that the local coordinates stay within their optimal boundaries. Should be called whenever the remainder vector was manually altered
			
									Coordinates()
									{
										M::Vec::clear(sector);
										M::Vec::clear(remainder);
									}
			
				template <typename T>
					void			add(const TVec3<T>&delta, float sector_size)
									{
										M::Vec::add(remainder,delta);
										wrap(sector_size);
									}
				template <typename T>
					void			mad(const TVec3<T>&delta, float factor, float sector_size)
									{
										M::Vec::mad(remainder,delta,factor);
										wrap(sector_size);
									}
			
					double			length(float sector_size)	const
									{
										TVec3<double> v;
										convertToAbsolute(v,sector_size);
										return M::Vec::length(v);
							
									}
			
					/**
						@brief Determines the translation vector from the local space coordinates to the specified sector's center
				
						Adding the resulting vector to the local vector will transport it to the center of the specified sector.
				
						@param other_sector [in] Sector coordinates of the sector to translate into
						@param out [out] Out vector. The precision of the used type will be used to calculate the translation vector
					*/
				template <typename T0, typename T1>
					void			getTranslationTo(const TVec3<sector_t>& other_sector, TVec3<T0>&out, T1 sector_size)	const
									{
										out.x = (T0)(other_sector.x-sector.x)*(T0)sector_size  - (T0)remainder.x;
										out.y = (T0)(other_sector.y-sector.y)*(T0)sector_size  - (T0)remainder.y;
										out.z = (T0)(other_sector.z-sector.z)*(T0)sector_size  - (T0)remainder.z;
									}
		
				template <typename T0, typename T1>
					void			getTranslationFrom(const TVec3<sector_t>&other_sector, TVec3<T0>&out, T1 sector_size)	const
									{
										out.x = -((T0)(other_sector.x-sector.x)*(T0)sector_size  - (T0)remainder.x);
										out.y = -((T0)(other_sector.y-sector.y)*(T0)sector_size  - (T0)remainder.y);
										out.z = -((T0)(other_sector.z-sector.z)*(T0)sector_size  - (T0)remainder.z);
									}
					/**
						@brief Determines the translation vector from the local space coordinates to the specified coordinates
				
						Adding the resulting vector to the local vector will transport it to the center of the specified location.
				
						@param other_sector [in] Sector coordinates of the coordinates to translate to
						@param vec [in] Remainder vector of the coordinates to translate to
						@param out [out] Out vector. The precision of the used type will be used to calculate the translation vector
					*/
				template <typename T0, typename T1, typename T2>
					void			getTranslationTo(const TVec3<sector_t>&other_sector, const TVec3<T0>&vec,TVec3<T1>&out, T2 sector_size)	const
									{
										out.x = (T1)(other_sector.x-sector.x)*(T1)sector_size + vec.x - remainder.x;
										out.y = (T1)(other_sector.y-sector.y)*(T1)sector_size + vec.y - remainder.y;
										out.z = (T1)(other_sector.z-sector.z)*(T1)sector_size + vec.z - remainder.z;
									}
					/**
						@brief Determines the translation vector from the local space coordinates to the specified space coordinates
				
						Adding the resulting vector to the local vector will transport it to the specified space coordinates.
				
						@param other [in] Space coordinates to translate to
						@param out [out] Out vector. The precision of the used type will be used to calculate the translation vector
					*/
				template <typename T0, typename T1>
					void			getTranslationTo(const Coordinates&other, TVec3<T0>&out, T1 sector_size)	const
									{
										out.x = (T0)(other.sector.x-sector.x)*(T0)sector_size + (T0)other.remainder.x - (T0)remainder.x;
										out.y = (T0)(other.sector.y-sector.y)*(T0)sector_size + (T0)other.remainder.y - (T0)remainder.y;
										out.z = (T0)(other.sector.z-sector.z)*(T0)sector_size + (T0)other.remainder.z - (T0)remainder.z;
									}

					/**
						@brief Retrieves the distance between the local space coordinates and the specified other space coordinates in double precision
					*/
					double			getDistanceTo(const Coordinates&other, double sector_size)	const
									{
										TVec3<double> delta;
										getTranslationTo(other,delta,sector_size);
										return M::Vec::length(delta);
									}
					/**
						@brief Retrieves the squared distance between the local space coordinates and the specified other space coordinates in double precision
					*/
					double			getDistanceSquareTo(const Coordinates&other, double sector_size)	const
									{
										TVec3<double> delta;
										getTranslationTo(other,delta,sector_size);
										return M::Vec::dot(delta);
									}
			
					/**
						@brief Converts a coordinate vector from local sector scope to global scope
				
						Only the local sector coordinates are used for this operation. @a remainder is ignored.
				
						@param relative [in] Sector relative coordinates
						@param absolute [out] Resulting absolute vector. The precision of this type is used for calculation
					*/
				template <typename T0, typename T1, typename T2>
					void			convertToAbsolute(const TVec3<T0>& relative, TVec3<T1>& absolute, T2 sector_size)	const
									{
										absolute.x = (T1)sector_size * (T1)sector.x + (T1)relative.x;
										absolute.y = (T1)sector_size * (T1)sector.y + (T1)relative.y;
										absolute.z = (T1)sector_size * (T1)sector.z + (T1)relative.z;
									}			
					/**
						@brief Converts the local space coordinates to an absolute vector
				
						@param absolute [out] Resulting absolute vector. The precision of the used type is also used for calculation
					*/
				template <typename T0, typename T1>
					void			convertToAbsolute(TVec3<T0>&absolute, T1 sector_size)	const
									{
										absolute.x = (T0)sector_size * (T0)sector.x + (T0)remainder.x;
										absolute.y = (T0)sector_size * (T0)sector.y + (T0)remainder.y;
										absolute.z = (T0)sector_size * (T0)sector.z + (T0)remainder.z;
									}
					/**
						@brief Converts a coordinate vector from global to local sector scope
				
						Only the local sector coordinates are used for this operation. @a remainder is ignored.
				
						@param relative [in] Absolute coordinates. The precision of this type is used for most of the calculation
						@param absolute [out] Resulting sector relative vector
					*/
				template <typename T0, typename T1, typename T2>
					void			convertToRelative(const TVec3<T0>&absolute, TVec3<T1>&relative, T2 sector_size)	const
									{
										relative.x = (T1)(absolute.x-(T0)sector_size * (T0)sector.x);
										relative.y = (T1)(absolute.y-(T0)sector_size * (T0)sector.y);
										relative.z = (T1)(absolute.z-(T0)sector_size * (T0)sector.z);
									}
				template <typename T>
					void			relativateToSector(const TVec3<sector_t>&other_sector, T sector_size)	//! Translates the local coordinates so that they lay in the specified sector space
									{
										TVec3<T> delta;
										M::Vec::sub(other_sector,sector,delta);
										M::Vec::mad(remainder,delta,sector_size);
										M::Vec::copy(other_sector,sector);
									}
			
					friend String	ToString(const Coordinates&c)
									{
										using StringConversion::ToString;
										return "["+ToString(c.sector.x)+", "+ToString(c.sector.y)+", "+ToString(c.sector.z)+"] "+ToString(c.remainder.x)+", "+ToString(c.remainder.y)+", "+ToString(c.remainder.z);
									}
			
					String			ConvertToString(float sector_size)	const	//! Converts the local space coordinates to a string representation
									{
										using StringConversion::ToString;
										return "["+ToString(sector.x)+", "+ToString(sector.y)+", "+ToString(sector.z)+"] "+ToString(remainder.x)+", "+ToString(remainder.y)+", "+ToString(remainder.z)+" (="+ToString(sector.x*sector_size+remainder.x)+", "+ToString(sector.y*sector_size+remainder.y)+", "+ToString(sector.z*sector_size+remainder.z)+")";
									}
				template <class Stream>
					Stream&			stream(Stream&target) const
									{
										return target<< '['<<sector.x<<", "<<sector.y<<", "<<sector.z<<"] "<<remainder.x<<", "<<remainder.y<<", "<<remainder.z;
									}
				template <class Stream>
					Stream&			stream(Stream&target, float sector_size) const
									{
										return stream(target)<<" (="<<(sector.x*sector_size+remainder.x)<<", "<<(sector.y*sector_size+remainder.y)<<", "<<(sector.z*sector_size+remainder.z)<<')';
									}
							
					void			Set(float x, float y, float z)	//! Redefines the remainder. The local sector offset is left unchanged
									{
										M::Vec::def(remainder,x,y,z);
									}
					void			setAbsolute(double x, double y, double z, double sector_size)	//! Sets both sector and remainder to ideally match the specified absolute vector defined by x, y, and z
									{
										sector.x = (sector_t)(x/sector_size);
										sector.y = (sector_t)(y/sector_size);
										sector.z = (sector_t)(z/sector_size);
										remainder.x = (float)(x-sector.x*sector_size);
										remainder.y = (float)(y-sector.x*sector_size);
										remainder.z = (float)(z-sector.x*sector_size);
									}
							
					void			reset()		//!< Resets both sector and remainder to 0
									{
										M::Vec::clear(sector);
										M::Vec::clear(remainder);
									}
			
					double			axisToDouble(BYTE axis, double sector_size)	const	//! Converts a single axis into an absolute double value
									{
										return static_cast<double>(sector.v[axis])*sector_size + remainder.v[axis];
									}
					Scalar			axis(BYTE axis)	const	//! Retrieves a single axis as composite structure
									{
										return Scalar(sector.v[axis],remainder.v[axis]);
									}			
					void			getAxis(BYTE axis, Scalar&target)	const	//! Retrieves a single axis as composite structure
									{
										target.sector = sector.v[axis];
										target.remainder = remainder.v[axis];
									}
							
					void			setAxis(BYTE axis, const Scalar&target)	//! Sets a single axis via composite structure. The result is @b not automatically wrapped
									{
										sector.v[axis] = target.sector;
										remainder.v[axis] = target.remainder;
									}
			
					double			separation(BYTE axis, Coordinates&other, double sector_size)	const	//! Determines the absolute axis separation between the local and specified other coordinate vector. Only the specified axis is used for the calculation
									{
										using std::fabs;
										return fabs((double)((sector.v[axis]-other.sector.v[axis])*sector_size) + (double)remainder.v[axis]-(double)other.remainder.v[axis]);
									}
			
					void			div(float scalar, float sector_size)
									{
										divComposite(sector.x,remainder.x,scalar,sector_size);
										divComposite(sector.y,remainder.y,scalar,sector_size);
										divComposite(sector.z,remainder.z,scalar,sector_size);
									}
					void			divInt(int scalar, float sector_size)
									{
										divIntComposite(sector.x,remainder.x,scalar,sector_size);
										divIntComposite(sector.y,remainder.y,scalar,sector_size);
										divIntComposite(sector.z,remainder.z,scalar,sector_size);
									}
				template <typename T0, typename T1>
					void			addVal(T0 value, T1 sector_size)
									{
										addToComposite(sector.x,remainder.x,value,sector_size);
										addToComposite(sector.y,remainder.y,value,sector_size);
										addToComposite(sector.z,remainder.z,value,sector_size);
									}
			
				template <typename T0, typename T1>
					void			subVal(T0 value, T1 sector_size)
									{
										addToComposite(sector.x,remainder.x,-value,sector_size);
										addToComposite(sector.y,remainder.y,-value,sector_size);
										addToComposite(sector.z,remainder.z,-value,sector_size);
									}
		
					/**
						@brief Rotates the local coordinates
				
						rotate3() calculates <i>matrix*<b>this</b></i> and writes the result back to the local object.
						Precision errors occur to the same degree as if the local coordinates vector was a simple double vector.
						If the vector was wrapped before then it will be wrapped again once the operation finished
				
						@param matrix Matrix to multiply the local coordinates vector with. Elements of this matrix are cast to double for each operation
						@param sector_size Sector size to apply to the result
					*/
				template <typename T>
					void			rotate3(const TMatrix3<T>&matrix, double sector_size)
									{
										//the following method produces the same precision fault as a simple absolute double vector would:
										//... at least it works to that degree :P
										TVec3<double>	new_sector,
														new_remainder;
										TVec3<sector_t>	sector_add;
								
										new_sector.x = (double)matrix.x.x*(double)sector.x + (double)matrix.y.x*(double)sector.y + (double)matrix.z.x*(double)sector.z;
										new_sector.y = (double)matrix.x.y*(double)sector.x + (double)matrix.y.y*(double)sector.y + (double)matrix.z.y*(double)sector.z;
										new_sector.z = (double)matrix.x.z*(double)sector.x + (double)matrix.y.z*(double)sector.y + (double)matrix.z.z*(double)sector.z;
								
							
										new_remainder.x = (double)matrix.x.x*(double)remainder.x + (double)matrix.y.x*(double)remainder.y + (double)matrix.z.x*(double)remainder.z;
										new_remainder.y = (double)matrix.x.y*(double)remainder.x + (double)matrix.y.y*(double)remainder.y + (double)matrix.z.y*(double)remainder.z;
										new_remainder.z = (double)matrix.x.z*(double)remainder.x + (double)matrix.y.z*(double)remainder.y + (double)matrix.z.z*(double)remainder.z;
							
										sector.x = (sector_t)((double)new_sector.x);
										sector.y = (sector_t)((double)new_sector.y);
										sector.z = (sector_t)((double)new_sector.z);
								
										new_remainder.x -= ((double)sector.x-new_sector.x)*sector_size;
										new_remainder.y -= ((double)sector.y-new_sector.y)*sector_size;
										new_remainder.z -= ((double)sector.z-new_sector.z)*sector_size;
								
										//cout << _toString(sector)<<endl;
										//cout << _toString(new_remainder)<<endl;
								
										sector_add.x = (sector_t)(new_remainder.x/sector_size);
										sector_add.y = (sector_t)(new_remainder.y/sector_size);
										sector_add.z = (sector_t)(new_remainder.z/sector_size);
								
								
										new_remainder.x -= sector_size*sector_add.x;
										new_remainder.y -= sector_size*sector_add.y;
										new_remainder.z -= sector_size*sector_add.z;
								
										M::Vec::add(sector,sector_add);
										M::Vec::copy(new_remainder,remainder);
							
									}
			
			};
	
	
			/**
				\brief Calculates the absolute distance between two space locations
			*/
			inline	double		distance(const Coordinates&a, const Coordinates&b, double sector_size)
			{
				return a.getDistanceTo(b,sector_size);
			}
	
			/**
				\brief Calculates the square absolute distance between two space locations
			*/
			inline	double		distanceSquare(const Coordinates&a, const Coordinates&b, double sector_size)
			{
				return a.getDistanceSquareTo(b,sector_size);
			}
	
			/**
				\brief Calculates the square absolute distance between two space locations
			*/
			inline	double		quadraticDistance(const Coordinates&a, const Coordinates&b, double sector_size)
			{
				return a.getDistanceSquareTo(b,sector_size);
			}
	
			/**
				\brief Calculates the absolute difference between two space locations. Both coordinates are expected to use the same sector size
			*/
			template <typename T0, typename T1>
				inline	void		sub(const Coordinates&a, const Coordinates&b, TVec3<T0>&out, T1 sector_size)
				{
					b.getTranslationTo(a,out,(T0)sector_size);
				}

			/**
				\brief Determine whether or not v0 is larger than v1 in at least one dimension
			*/
			template <typename T>
				bool oneGreater(const Composite::Coordinates&v0, const Composite::Coordinates&v1, T sector_size)
				{
					TVec3<T> dif;
					Composite::sub(v0,v1,dif,sector_size);
					return M::Vec::oneGreater(dif,Vector3<T>::zero);
				}

			/**
				\brief Determine whether or not v0 is less than v1 in at least one dimension
			*/
			template <typename T>
				bool oneLess(const Composite::Coordinates&v0, const Composite::Coordinates&v1, T sector_size)
				{
					TVec3<T> dif;
					Composite::sub(v0,v1,dif,sector_size);
					return M::Vec::oneLess(dif,Vector3<T>::zero);
				}
	
			/**
				\brief Calculates the absolute difference between a sector space and a full space location.
			*/
			template <typename T0, typename T1>
				inline	void		sub(const Coordinates&a, const TVec3<Coordinates::sector_t>& sector, TVec3<T0>& out, T1 sector_size)
				{
					a.getTranslationFrom(sector,out,(T0)sector_size);
				}
	
			/**
				\brief Calculates the absolute difference between a sector space and a full space location.
			*/
			template <typename T0, typename T1>
				inline	void		sub(const TVec3<Coordinates::sector_t>& sector, const Coordinates&b, TVec3<T0>& out, T1 sector_size)
				{
					b.getTranslationTo(sector,out,(T0)sector_size);
				}
	
			/**
				\brief Calculates the absolute difference between two sectors
			*/
			template <typename T0, typename T1>
				inline	void		sub(const TVec3<Coordinates::sector_t>&sector0, const TVec3<Coordinates::sector_t>&sector1, TVec3<T0>& out, T1 sector_size)
				{
					out.x = (T0)(sector0.x-sector1.x)*(T0)sector_size;
					out.y = (T0)(sector0.y-sector1.y)*(T0)sector_size;
					out.z = (T0)(sector0.z-sector1.z)*(T0)sector_size;
				}


			template <typename T0, typename T1>
				void getTranslation(const Coordinates&observer,const Coordinates&shape_offset,const TVec3<sector_t>&face_sector,TVec3<T0>&out,T1 sector_size)
				{
					out.x = (T0)(shape_offset.sector.x+face_sector.x-observer.sector.x)*(T0)sector_size + (T0)shape_offset.remainder.x - (T0)observer.remainder.x;
					out.y = (T0)(shape_offset.sector.y+face_sector.y-observer.sector.y)*(T0)sector_size + (T0)shape_offset.remainder.y - (T0)observer.remainder.y;
					out.z = (T0)(shape_offset.sector.z+face_sector.z-observer.sector.z)*(T0)sector_size + (T0)shape_offset.remainder.z - (T0)observer.remainder.z;
				}
	
	
			inline void		add(const Coordinates&a, const Coordinates&b, Coordinates&result)
			{
				M::Vec::add(a.sector,b.sector,result.sector);
				M::Vec::add(a.remainder,b.remainder,result.remainder);
			}	
			inline void		sub(const Coordinates&a, const Coordinates&b, Coordinates&result)
			{
				M::Vec::sub(a.sector,b.sector,result.sector);
				M::Vec::sub(a.remainder,b.remainder,result.remainder);
			}
	
	
			inline void		add(const Coordinates&a, const Coordinates&b, Coordinates&result, float sector_size)
			{
				M::Vec::add(a.sector,b.sector,result.sector);
				M::Vec::add(a.remainder,b.remainder,result.remainder);
				result.wrap(sector_size);
			}	
			inline void		sub(const Coordinates&a, const Coordinates&b, Coordinates&result, float sector_size)
			{
				M::Vec::sub(a.sector,b.sector,result.sector);
				M::Vec::sub(a.remainder,b.remainder,result.remainder);
				result.wrap(sector_size);
			}
	

	
			inline void		center(const Coordinates&a, const Coordinates&b, Coordinates&result, float sector_size)	//! Determines the center coordinates exactly between @b a and @b b and writes the result to @b result
			{
				add(a,b,result);
				result.divInt(2,sector_size);
				result.wrap(sector_size);
			}		
			template <typename T0, typename T1>
			inline void		center(const Coordinates&a, const Coordinates&b, TVec3<T0>&result, T1 sector_size)	//! Determines the center coordinates exactly between @b a and @b b and writes the result to @b result
			{
				Coordinates c;
				add(a,b,c);
				c.divInt(2,sector_size);
				c.convertToAbsolute(result,sector_size);
			}		
	
			inline void		min(const Coordinates&a, const Coordinates&b, Coordinates&result, float sector_size)		//!< Writes the component-wise minimum coordinate vector of @b a and @b b to @b result
			{
				if (compareComposites(a.sector.x,a.remainder.x,b.sector.x,b.remainder.x,sector_size) > 0)
				{
					result.sector.x = b.sector.x;
					result.remainder.x = b.remainder.x;
				}
				else
				{
					result.sector.x = a.sector.x;
					result.remainder.x = a.remainder.x;
				}
	
				if (compareComposites(a.sector.y,a.remainder.y,b.sector.y,b.remainder.y,sector_size) > 0)
				{
					result.sector.y = b.sector.y;
					result.remainder.y = b.remainder.y;
				}
				else
				{
					result.sector.y = a.sector.y;
					result.remainder.y = a.remainder.y;
				}
		
				if (compareComposites(a.sector.z,a.remainder.z,b.sector.z,b.remainder.z,sector_size) > 0)
				{
					result.sector.z = b.sector.z;
					result.remainder.z = b.remainder.z;
				}
				else
				{
					result.sector.z = a.sector.z;
					result.remainder.z = a.remainder.z;
				}
			}
	
			inline void		max(const Coordinates&a, const Coordinates&b, Coordinates&result, float sector_size)		//!< Writes the component-wise maximum coordinate vector of @b a and @b b to @b result
			{
				if (compareComposites(a.sector.x,a.remainder.x,b.sector.x,b.remainder.x,sector_size) < 0)
				{
					result.sector.x = b.sector.x;
					result.remainder.x = b.remainder.x;
				}
				else
				{
					result.sector.x = a.sector.x;
					result.remainder.x = a.remainder.x;
				}
	
				if (compareComposites(a.sector.y,a.remainder.y,b.sector.y,b.remainder.y,sector_size) < 0)
				{
					result.sector.y = b.sector.y;
					result.remainder.y = b.remainder.y;
				}
				else
				{
					result.sector.y = a.sector.y;
					result.remainder.y = a.remainder.y;
				}
		
				if (compareComposites(a.sector.z,a.remainder.z,b.sector.z,b.remainder.z,sector_size) < 0)
				{
					result.sector.z = b.sector.z;
					result.remainder.z = b.remainder.z;
				}
				else
				{
					result.sector.z = a.sector.z;
					result.remainder.z = a.remainder.z;
				}
			}



		}



	}


}

#endif
