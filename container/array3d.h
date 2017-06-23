#pragma once

#include "array.h"

namespace Container
{
	/**
	@brief Three dimensional, row-major array
		
	The Array3D class maps 3d access to a single-dimensional field. Internally it behaves like a normal Array instance, but it provides some 3d access helper method.
	*/
	template <typename T, class Strategy=typename Strategy::StrategySelector<T>::Default>
		class Array3D:public Array<T,Strategy>
		{
		public:
			typedef Array3D<T,Strategy>	Self;
			typedef Array<T,Strategy>	Super;
		protected:
				
			Arrays::count_t	w,h;
				
			using Super::SetSize;
			using Super::resizePreserveContent;
			using Super::erase;
			using Super::append;
		public:

			template <int AxisIndex>
				class Axis;

			template <int AxisIndex>
				class Iterator
				{
					index_t	at;
					friend class Axis<AxisIndex>;

					/**/			Iterator(index_t at):at(at)	{}
				public:
					typedef Iterator<AxisIndex>	Self;
					Iterator		operator++(int) {Iterator rs(*this); at++; return rs;}
					Iterator&		operator++() {at++; return *this;}
					Iterator		operator--(int) {Iterator rs(*this); at--; return rs;}
					Iterator&		operator--() {at--; return *this;}
					index_t			operator*() const {return at;}
					bool			operator==(index_t other) const {return at == other;}
					bool			operator!=(index_t other) const {return at != other;}
					bool			operator<=(index_t other) const {return at <= other;}
					bool			operator>=(index_t other) const {return at >= other;}
					bool			operator<(index_t other) const {return at < other;}
					bool			operator>(index_t other) const {return at > other;}
					bool			operator==(const Self&other) const {return at == other.at;}
					bool			operator!=(const Self&other) const {return at != other.at;}
					bool			operator<=(const Self&other) const {return at <= other.at;}
					bool			operator>=(const Self&other) const {return at >= other.at;}
					bool			operator<(const Self&other) const {return at < other.at;}
					bool			operator>(const Self&other) const {return at > other.at;}
				};
			
			template <int AxisIndex>
				class Axis
				{
					const index_t		extent;

					friend class Self;

					/**/			Axis(index_t extent):extent(extent)	{}
				public:
					typedef Iterator<AxisIndex>	iterator,const_iterator;

					constexpr iterator	begin() const {return 0;}
					iterator		end() const {return extent;}
					count_t			size() const {return extent;}
				};	


			Array3D():w(0),h(0)
			{}
			Array3D(Arrays::count_t width, Arrays::count_t height, Arrays::count_t depth):Super(width*height*depth),w(width),h(height)
			{}
			Array3D(Arrays::count_t width, Arrays::count_t height, Arrays::count_t depth, const T&initial):Super(width*height*depth),w(width),h(height)
			{
				Fill(initial);
			}

			template <class T, class OtherStrategy>
				Array3D(const Array3D<T,OtherStrategy>&other):Super(other),w(other.w),h(other.h)
				{}
					
			#if __ARRAY_RVALUE_REFERENCES__
				Array3D(const Self&other):Super(other),w(other.w),h(other.h)	{}
				Array3D(Self&&other):Super(std::move( other )),w(other.w),h(other.h)
				{
					other.w = 0;
					other.h = 0;
				}
				void operator=(Self&&other)
				{
					adoptData(other);
				}
			#endif

			void operator=(const Self&other)
			{
				Super::operator=(other);
				w = other.w;
				h = other.h;
			}
				
			inline	Arrays::count_t	GetWidth()	const	//! Retrieves this array's width \return width
			{
				return w;
			}
			inline	Arrays::count_t	GetDepth()	const	//! Retrieves this array's depth \return depth
			{
				return w&&h?Super::elements/w/h:0;
			}
			inline	Arrays::count_t	GetHeight() const	//! Retrieves this array's height \return height
			{
				return h;
			}

			Axis<0>			Horizontal() const {return Axis<0>(GetWidth());}
			Axis<1>			Vertical() const {return Axis<1>(GetHeight());}
			Axis<2>			Deep() const {return Axis<2>(GetDepth());}



			index_t		ToIndex(index_t x, index_t y, index_t z) const
			{
				#ifdef __ARRAY_DBG_RANGE_CHECK__
					if (x >= w || y >= h || z >= GetDepth())
						FATAL__("Index out of bounds");
				#endif
				return ToIndexNoCheck(x,y,z);
			}

			index_t		ToIndexNoCheck(index_t x, index_t y, index_t z) const
			{
				return z*w*h + y*w + x;
			}


			void		SetSize(Arrays::count_t width, Arrays::count_t height, Arrays::count_t depth)	//! Resizes the local 2d array to match the specified dimensions. The local array content is lost if the array's total size is changed
			{
				Super::SetSize(width*height*depth);
				w = width;
				h = height;
			}
			void		Set(index_t x, index_t y, index_t z, const T&value)
			{
				Super::data[ToIndex(x,y,z)] = value;
			}
			
			T&			Get(index_t x, index_t y, index_t z)
			{
				return Super::data[ToIndex(x,y,z)];
			}
			const T&			Get(index_t x, index_t y, index_t z) const
			{
				return Super::data[ToIndex(x,y,z)];
			}
			T&			Get(const Iterator<0>&x, const Iterator<1>&y, const Iterator<2>&z)
			{
				return Get(*x,*y,*z);
			}
			
			void	adoptData(Self&other)	//! Adopts pointer and size and sets both NULL of the specified origin array.
			{
				w = other.w;
				h = other.h;
				Super::adoptData(other);
				other.w = 0;
				other.h = 0;
			}
			void	swap(Self&other)
			{
				Super::swap(other);
				swp(w,other.w);
				swp(h,other.h);
			}
			friend void swap(Self&a, Self&b)
			{
				a.swap(b);
			}

			bool	operator==(const Self&other) const
			{
				return w == other.w && h == other.h && Super::operator==(other);
			}

			bool	operator!=(const Self&other) const
			{
				return !operator==(other);
			}
		};
}

DECLARE_T2__(Container::Array3D,typename,class,SwapStrategy);

