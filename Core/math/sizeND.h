#pragma once

namespace DeltaWorks
{
	template <typename T>
		class Size2D;
	template <typename T>
		class Size3D;

	template <typename T>
		class Index2D
		{
		public:
			typedef Index2D<T>	Self;
			
			T			x,
						y;

			explicit	Index2D(T diagonal=0):x(diagonal),y(diagonal)	{}
			/**/		Index2D(T x, T y):x(x),y(y)	{}
			bool		operator==(const Self&other) const {return x == other.x && y == other.y;}
			bool		operator!=(const Self&other) const {return !operator==(other);}
			Self		operator+(const Size2D<T>&) const;
			Size2D<T>	operator-(const Self&) const;
			void		Set(T x, T y)
			{
				this->x = x;
				this->y = y;
			}
		};

	template <typename T>
		class Size2D
		{
		public:
			typedef Size2D<T>	Self;
			T			width,
						height;

			explicit	Size2D(T size=0):width(size),height(size)	{}
			/**/		Size2D(T w, T h):width(w),height(h)	{}
			bool		operator==(const Self&other) const {return width == other.width && height == other.height;}
			bool		operator!=(const Self&other) const {return !operator==(other);}
			count_t		CountElements() const {return width*height;}
			Self		operator-(const Index2D<T>&idx) const
			{
				return Self(width - idx.x,height - idx.y);
			}
			void		Limit(const Self&other)
			{
				using std::min;
				width = min(width,other.width);
				height = min(height,other.height);
			}
			void		Set(T width, T height)
			{
				this->width = width;
				this->height = height;
			}
			void		Set(T diagonalSize)
			{
				width = diagonalSize;
				height = diagonalSize;
			}

			friend Self max(const Self&a, const Self&b)
			{
				using std::max;
				return Self(max(a.width,b.width),max(a.height,b.height));
			}
			friend Self min(const Self&a, const Self&b)
			{
				using std::min;
				return Self(min(a.width,b.width),min(a.height,b.height));
			}
		};

	template <typename T>
		class Index3D
		{
		public:
			typedef Index3D<T>	Self;
			
			T			x,
						y,
						z;

			explicit	Index3D(T diagonal=0):x(diagonal),y(diagonal),z(diagonal)	{}
			/**/		Index3D(T x, T y, T z):x(x),y(y),z(z)	{}
			bool		operator==(const Self&other) const {return x == other.x && y == other.y && z == other.z;}
			bool		operator!=(const Self&other) const {return !operator==(other);}
			Self		operator+(const Size3D<T>&) const;
			Size3D<T>	operator-(const Self&) const;
			void		Set(T x, T y, T z)
			{
				this->x = x;
				this->y = y;
				this->z = z;
			}
		};

	template <typename T>
		class Size3D
		{
		public:
			typedef Size3D<T>	Self;
			T			width,
						height,
						depth;

			explicit	Size3D(T size=0):width(size),height(size),depth(size)	{}
			/**/		Size3D(T w, T h, T d):width(w),height(h),depth(d)	{}
			bool		operator==(const Self&other) const {return width == other.width && height == other.height && depth == other.depth;}
			bool		operator!=(const Self&other) const {return !operator==(other);}
			count_t		CountElements() const {return width*height*depth;}
			Self		operator-(const Index3D<T>&idx) const
			{
				return Self(width - idx.x,height - idx.y, depth - idx.z);
			}
			void		Limit(const Self&other)
			{
				using std::min;
				width = min(width,other.width);
				height = min(height,other.height);
				depth = min(depth,other.depth);
			}
			void		Set(T width, T height, T depth)
			{
				this->width = width;
				this->height = height;
				this->depth = depth;
			}
			void		Set(T diagonalSize)
			{
				width = diagonalSize;
				height = diagonalSize;
				depth = diagonalSize;
			}

			friend Self max(const Self&a, const Self&b)
			{
				using std::max;
				return Self(max(a.width,b.width),max(a.height,b.height),max(a.depth,b.depth));
			}
			friend Self min(const Self&a, const Self&b)
			{
				using std::min;
				return Self(min(a.width,b.width),min(a.height,b.height),min(a.depth,b.depth));
			}
		};

	template <typename T>
		Index2D<T>	Index2D<T>::operator+(const Size2D<T>&s) const
		{
			return Self(x + s.width, y + s.height);
		}
	template <typename T>
		Size2D<T>	Index2D<T>::operator-(const Index2D<T>&other) const
		{
			return Size2D<T>(
				x > other.x ? x - other.x : 0,
				y > other.y ? y - other.y : 0
				);
		}

	template <typename T>
		Index3D<T>	Index3D<T>::operator+(const Size3D<T>&s) const
		{
			return Self(x + s.width, y + s.height, z + s.depth);
		}
	template <typename T>
		Size3D<T>	Index3D<T>::operator-(const Index3D<T>&other) const
		{
			return Size3D<T>(
				x > other.x ? x - other.x : 0,
				y > other.y ? y - other.y : 0,
				z > other.z ? z - other.z : 0
				);
		}
	
}
