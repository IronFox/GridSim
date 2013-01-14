#ifndef vector_mathH
#define vector_mathH

/******************************************************************

Collection of template vector-math-functions.

******************************************************************/

#include "../global_string.h"
#include "basic.h"


namespace Math
{
	template <count_t Value0, count_t Value1>
		struct GreaterOrEqual
		{
			static const bool eval=Value0>=Value1;
		};

	template <typename T=float>
		struct TVec0
		{
			typedef T				Type;
			static	const count_t	Dimensions=0;

			T						v[0];	//!< Standard accessor
		};

	template <typename T=float>
		struct TVec1
		{
			typedef T				Type;
			static	const count_t	Dimensions=1;

			union
			{
				T				v[1];	//!< Standard accessor
				struct
				{
					T			x;	//!< 1D coordinate accessor
				};
				struct
				{
					T			a;//!< Polynomial accessor
				};
				struct
				{
					T			alpha;	//!< Color accessor
				};
			};
		};

	template <typename T=float>
		struct TVec2
		{
			typedef T				Type;
			static	const count_t	Dimensions=2;

			union
			{
				T				v[2];	//!< Standard accessor
				struct
				{
					T			x,y;	//!< 2D coordinate accessors
				};
				struct
				{
					T			lower,upper;	//!< TRange accessors
				};
				struct
				{
					T			a,b;	//!< Polynomial accessors
				};
				struct
				{
					T			luminance, alpha;	//!< Color accessors
				};
				//T				xy[2];	//!< Named accessor
				T				la[2];	//!< Color accessor
			};
		};

	template <typename T=float>
		struct TVec3
		{
			typedef T				Type;
			static	const count_t	Dimensions=3;

			union
			{
				T				v[3];	//!< Standard accessor
				struct
				{
					T			x,y,z;	//!< 3D coordinate accessors
				};
				struct
				{
					TVec2<T>	xy;
				};
				struct
				{
					T			x_;
					TVec2<T>	yz;
				};
				struct
				{
					T			red, green, blue;	//!< Color accessors
				};
				struct
				{
					T			r, g, b;	//!< Color accessors
				};
			};
		};
	template <typename T=float>
		struct TVec4
		{
			typedef T				Type;
			static	const count_t	Dimensions=4;

			union
			{
				T				v[4];	//!< Standard accessor
				struct
				{
					T			x,y,z,w;	//!< 4D coordinate accessors
				};
				struct
				{
					TVec2<T>	xy,zw;
				};
				struct
				{
					T			x_;
					TVec3<T>	yzw; //!< yzw accessor
				};
				struct
				{
					T			a,b,c,d;//!< Possibly incomplete polynomial accessors
				};
				struct
				{
					T			red, green, blue, alpha;	//!< Color accessors
				};
				TVec3<T>		rgb;		//!< Color accessor
				TVec3<T>		xyz;		//!< Color accessor
				T				rgba[4];	//!< Color accessor
			};
		};


	template <typename T, count_t D>	//due to subsequent specializations for D=0, D=1, D=2, D=3, and D=4, D must be > 4 here
		struct TVec
		{
			typedef T				Type;
			static	const count_t	Dimensions=D;

			union
			{
				T				v[D];	//!< Standard accessor
				struct
				{
					T			x,y,z,w;	//!< 4D coordinate accessors
				};
				struct
				{
					TVec2<T>	xy,zw; //!< xy and zw accessors
				};
				struct
				{
					TVec3<T>	xyz;	//!< xyz accessor
				};
				struct
				{
					T			x_;
					TVec3<T>	yzw; //!< yzw accessor
				};
				struct
				{
					T			a,b,c,d;//!< Possibly incomplete polynomial accessors
				};
				struct
				{
					T			red, green, blue, alpha;	//!< Color accessors
				};
				TVec3<T>		rgb;		//!< Color accessor
				TVec4<T>		rgba;	//!< Color accessor
				TVec4<T>		xyzw;	//!< Color accessor
			};
		};




	template <typename T>
		struct TVec<T,0>:public TVec0<T>
		{};

	template <typename T>
		struct TVec<T,1>:public TVec1<T>
		{};
			

	template <typename T>
		struct TVec<T,2>:public TVec2<T>
		{};

	template <typename T>
		struct TVec<T,3>:public TVec3<T>
		{};
			

	template <typename T>
		struct TVec<T,4>:public TVec4<T>
		{};


	template <typename T=float>
		class TRange
		{
		public:
			union
			{
				struct
				{
					T				min,max;
				};
				T					extreme[2];
			};



			template <typename T1>
				TRange<T>&			operator=(const TRange<T1>&other)
									{
										min = (T)other.min;
										max = (T)other.max;
										return *this;
									}
									
			MF_DECLARE(void)		set(const T&min, const T& max)
									{
										this->min = min;
										this->max = max;
									}
			template<typename T0>
				MF_DECLARE(void)	set(const TRange<T0>&other)
									{
										min = (T)other.min;
										max = (T)other.max;
									}
			MF_DECLARE(void)		setCenter(const T&center)
									{
										T	ext = (max - min)/(T)2;
										setCenter(center,ext);
									}
			MF_DECLARE(void)		setCenter(const T&center, const T&ext)
									{
										min = center - ext;
										max = center + ext;
									}
			template <typename T0>
				MF_DECLARE(void)	setAll(const T0&value)
									{
										min = max = (T)value;
									}
			MF_DECLARE(void)		scale(const T&by)	//! Scales the range from its central extend
									{
										T	c = center();
										T	ext  = (max - c);
										setCenter(c,ext*by);
									}
			MF_DECLARE(void)		expand(const T& by)	//! Expands the range. @b min is expected to be less than @b max
									{
										min -= by;
										max += by;
									}
			MF_DECLARE(void)		constrainBy(const TRange<T>&constraint)	//! Modifies the local range so that it lies within the specified constraint range
									{
										min = Math::clamp(min,constraint.min,constraint.max);
										max = Math::clamp(max,constraint.min,constraint.max);
									}
			MF_DECLARE(T)			center()	const	//! Retrieves the center of the local range
									{
										return (min+max)/(T)2;
									}
			MF_DECLARE(T)			extend()	const	//! Retrieves the difference between max and min
									{
										return max - min;
									}
			template<typename T0>
				MF_DECLARE(bool)	contains(const T0&value)	const //! Checks if the specified value lies within the local range
									{
										return (T)value >= min && (T)value <= max;
									}
			template<typename T0>
				MF_DECLARE(void)	include(const T0&value)	//!< Expands the local range so that it includes the specified value
									{
										min = std::min<T>(min,value);
										max = std::max<T>(max,value);
									}
			template<typename T0>
				MF_DECLARE(void)	include(const TRange<T0>&other)	//!< Expands the local range so that it includes the specified other range
									{
										min = std::min<T>(min,other.min);
										max = std::max<T>(max,other.max);
									}
			template <typename T0>
				MF_DECLARE(void)	translate(const T0&delta)
									{
										min = (T)(min + delta);
										max = (T)(max + delta);
									}
			template <typename T0,typename T1>
				MF_DECLARE(void)	relativate(const T0& absolute, T1&relative_out)	const
									{
										relative_out = (T1)((absolute-min)/extend());
									}
			template <typename T0,typename T1>
				MF_DECLARE(void)	relativateRange(const TRange<T0>& absolute, TRange<T1>&relative_out)	const
									{
										relativate(absolute.min,relative_out.min);
										relativate(absolute.max,relative_out.max);
									}
			template <typename T0,typename T1>
				MF_DECLARE(void)	derelativate(const T0& relative, T1&absolute_out)	const
									{
										absolute_out = (T1)(min + relative*extend());
									}
			template <typename T0,typename T1>
				MF_DECLARE(void)	derelativateRange(const TRange<T0>& relative, TRange<T1>&absolute_out)	const
									{
										derelativate(relative.min,absolute_out.min);
										derelativate(relative.max,absolute_out.max);
									}

				
			template <typename T0,typename T1>
				MF_DECLARE(void)	makeRelative(const T0& absolute, T1&relative_out)	const
									{
										relative_out = (T1)((absolute-min)/extend());
									}
			template <typename T0,typename T1>
				MF_DECLARE(void)	makeRangeRelative(const TRange<T0>& absolute, TRange<T1>&relative_out)	const
									{
										relativate(absolute.min,relative_out.min);
										relativate(absolute.max,relative_out.max);
									}
			template <typename T0,typename T1>
				MF_DECLARE(void)	makeAbsolute(const T0& relative, T1&absolute_out)	const
									{
										absolute_out = (T1)(min + relative*extend());
									}
			template <typename T0,typename T1>
				MF_DECLARE(void)	makeRangeAbsolute(const TRange<T0>& relative, TRange<T1>&absolute_out)	const
									{
										derelativate(relative.min,absolute_out.min);
										derelativate(relative.max,absolute_out.max);
									}
			T						clamp(const T&value)	const
									{
										return Math::clamp(value,min,max);
									}
				
			template <typename T1>
				MF_DECLARE(bool)	intersects(const TRange<T1>&other)	const
									{
										return min <= other.max && max >= other.min;
									}
			template <typename T1>
				MF_DECLARE(void)	operator*=(T1 factor)
									{
										min *= factor;
										max *= factor;
									}
			template <typename T1>
				MF_DECLARE(void)	operator/=(T1 factor)
									{
										min /= factor;
										max /= factor;
									}

		};

	template <typename T>
		inline TRange<T>	range(const T&min, const T&max)
		{
			TRange<T> result = {min,max};
			return result;
		}

	template <typename T>
		class Quad
		{
		public:
			union
			{
				struct
				{
					T				left,
									bottom,
									right,
									top;
				};
				T					value[4];
			};

			Quad()					{}
			Quad(const T&v)			:left(v),bottom(v),right(v),top(v)	{}
			/**/					Quad(const T&left_, const T&bottom_, const T&right_, const T&top_)
										:left(left_),bottom(bottom_),right(right_),top(top_)	{}

			/**
			@brief Updates all values of the local quad
			*/
			MF_DECLARE(void)		set(const T&left, const T& bottom, const T& right, const T& top)
									{
										this->left = left;
										this->right = right;
										this->bottom = bottom;
										this->top = top;
									}

			template <typename T0>
				MF_DECLARE(void)	setAll(const T0&value)
									{
										left = (T)value;
										right = (T)value;
										bottom = (T)value;
										top = (T)value;
									}
		};


	
	template <typename T=float>
		class Rect: public IToString	//! General purpose rectangle
		{
		public:
			union
			{
				struct
				{
					TRange<T>		x,y;
				};
				TRange<T>			axis[2];
			};

			/**/					Rect(){}
			/**/					Rect(const T&left, const T& bottom, const T& right, const T& top)
									{
										set(left,bottom,right,top);
									}
			/**/					Rect(const TVec2<T>&min, const TVec2<T>&max)
									{
										set(min,max);
									}
			/**/					Rect(const TVec2<T>&min_and_max)
									{
										set(min_and_max);
									}
			template<typename T1>
				/**/				Rect(const Rect<T1>&other)
										:x(other.x),y(other.y)	{}

			template <typename T1>
				Rect<T>&			operator=(const Rect<T1>&other)
									{
										x = other.x;
										y = other.y;
										return *this;
									}
									
			/**
			@brief Updates all values of the local rectangle
			*/
			MF_DECLARE(void)		set(const T&left, const T& bottom, const T& right, const T& top)
									{
										x.set(left,right);
										y.set(bottom,top);
									}
			/**
			@brief Updates all values of the local rectangle
			*/
			MF_DECLARE(void)		set(const TVec2<T>&min, const TVec2<T>&max)
									{
										x.set(min.x,max.x);
										y.set(min.y,max.y);
									}
			/**
			@brief Updates all values of the local rectangle
			*/
			MF_DECLARE(void)		set(const TVec2<T>&min_and_max)
									{
										x.setAll(min_and_max.x);
										y.setAll(min_and_max.y);
									}
			/**
			@brief Relocates the center of the local rectangle.
			Width and height remain unchanged.
			*/
			MF_DECLARE(void)		setCenter(const T&x, const T& y)
									{
										this->x.setCenter(x);
										this->y.setCenter(y);
									}
			/**
			@brief Updates all values of the local rectangle so that the center points to the specified x and y coordinates, and width and height match twice the specified extend
			@param x X coordinate of the new center
			@param y Y coordinate of the new center
			@param ext_x Extend in both directions along the x axis. Equals half the resulting width
			@param ext_y Extend in both directions along the y axis. Equals half the resulting height
			*/
			MF_DECLARE(void)		setCenter(const T&x, const T& y, const T&ext_x, const T&ext_y)
									{
										this->x.setCenter(x,ext_x);
										this->y.setCenter(y,ext_y);
									}
			/**
			@brief Updates all values of the local rectangle so that the center points to the specified x and y coordinates, and width and height match twice the specified extend
			@param x X coordinate of the new center
			@param y Y coordinate of the new center
			@param ext Extend in both directions along the x and y axes. Equals half the resulting width/height
			*/
			MF_DECLARE(void)		setCenter(const T&x, const T& y, const T&ext)
									{
										this->x.setCenter(x,ext);
										this->y.setCenter(y,ext);
									}
			/**
			@brief Updates all values of the local rectangle to the specified value
			*/
			template <typename T0>
				MF_DECLARE(void)	setAll(const T0&value)
									{
										x.setAll(value);
										y.setAll(value);
									}
			/**
			@brief Updates all min values of the local rectangle to the specified value
			*/
			template <typename T0>
				MF_DECLARE(void)	setAllMin(const T0&value)
									{
										x.min = (T)value;
										y.min = (T)value;
									}
			/**
			@brief Updates all max values of the local rectangle to the specified value
			*/
			template <typename T0>
				MF_DECLARE(void)	setAllMax(const T0&value)
									{
										x.max = (T)value;
										y.max = (T)value;
									}
			/**
			@brief Updates the minimum values of the local rectangle
			*/
			MF_DECLARE(void)		setMin(const TVec2<T>&min)
									{
										x.min = min.x;
										y.min = min.y;
									}
			/**
			@brief Updates the maximum values of the local rectangle
			*/
			MF_DECLARE(void)		setMax(const TVec2<T>&max)
									{
										x.max = max.x;
										y.max = max.y;
									}
			/**
			@brief Updates the minimum values of the local rectangle
			*/
			template<typename T0, typename T1>
				MF_DECLARE(void)	setMin(const T0&min_x, const T1&min_y)
									{
										x.min = (T)min_x;
										y.min = (T)min_y;
									}
			/**
			@brief Updates the maximum values of the local rectangle
			*/
			template<typename T0, typename T1>
				MF_DECLARE(void)	setMax(const T0&min_x, const T1&min_y)
									{
										x.max = (T)max_x;
										y.max = (T)max_y;
									}

			MF_DECLARE(void)		scale(const T&by)	//! Scales the rectangle from its central location
									{
										x.scale(by);
										y.scale(by);
									}
			MF_DECLARE(void)		expand(const T& by)	//! Expands the rectangle. @b bottom is expected to be less than @b top
									{
										x.expand(by);
										y.expand(by);
									}
									
			MF_DECLARE(void)		constrainBy(const Rect<T>&constraint)	//! Modifies the local rectangle so that it lies within the specified constraint rectangle. @b bottom is expected to be less than @b top
									{
										x.constrainBy(constraint.x);
										y.constrainBy(constraint.y);
									}
			/*!
			@brief	Calculate aspect of the rectangle
			@return Aspect
			*/
			MF_DECLARE(T)			aspect()	const
									{
										return x.extend()/y.extend();
									}
			/*!
			@brief	Determines whether or not the specified point lies within the local rectangle. @b top is expected to be greater or equal @b bottom
			@param p Pointer to a 2d vector
			@return true if the point lies in the rectangle
			*/
			template <typename T0>
				MF_DECLARE(bool)	contains(const TVec2<T0>&p)	const
									{
										return x.contains(p.x) && y.contains(p.y);
									}
		
			/*!
			@brief Modifies the local rectangle so that it contains the specified point. @b top is expected to be greater or equal @b bottom
			*/
			template <typename T0, typename T1>
				MF_DECLARE(void)	include(const T0&x, const T1&y)
									{
										this->x.include(x);
										this->y.include(y);
									}
			/*!
			@brief Modifies the local rectangle so that it contains the specified point. @b top is expected to be greater or equal @b bottom
			*/
			template <typename T0>
				MF_DECLARE(void)	include(const TVec2<T0>&p)
									{
										x.include(p.x);
										y.include(p.y);
									}
			/*!
			@brief Modifies the local rectangle so that it contains the specified rectangle. @b top is expected to be greater or equal @b bottom
			*/
			template <typename T0>
				MF_DECLARE(void)	include(const Rect<T0>&other)
									{
										x.include(other.x);
										y.include(other.y);
									}
		
				/*!
				\brief	Determines whether or not the specified point lies within the local rectangle
				\param x X coordinate of the point
				\param y Y coordinate of the point
				\return true if the point lies in the rectangle
				*/
			template <typename T0, typename T1>
				MF_DECLARE(bool)	contains(const T0& x, const T1& y)	const
									{
										return this->x.contains(x) && this->y.contains(y);
									}
			/*!
				\brief Calculates the width of the rectangle
				\return Rectangle width
			*/
			MF_DECLARE(T)			width()		const
									{
										return x.extend();
									}
			/*!
				\brief Calculates the height of the rectangle
				\return Rectangle height
			*/
			MF_DECLARE(T)			height()	const
									{
										return y.extend();
									}
									
			template <typename T0, typename T1>
				MF_DECLARE(void)	translate(const T0&delta_x, const T1&delta_y)
									{
										x.translate(delta_x);
										y.translate(delta_y);
									}
			
			template <typename T0,typename T1>
				MF_DECLARE(void)	makeRelative(const T& x, const T& y, T0&x_out, T1&y_out)	const
									{
										this->x.makeRelative(x,x_out);
										this->y.makeRelative(y,y_out);
									}
			template <typename T0>
				MF_DECLARE(void)	makeRelative(const TVec2<T>&p, TVec2<T0>&out)	const
									{
										makeRelative(p.x,p.y,out.x,out.y);
									}			
			
			template <typename T0,typename T1>
				MF_DECLARE(void)	relativate(const T& x, const T& y, T0&x_out, T1&y_out)	const
									{
										makeRelative(x,y,x_out,y_out);
									}
			template <typename T0>
				MF_DECLARE(void)	relativate(const TVec2<T>&absolute, TVec2<T0>&relative_out)	const
									{
										makeRelative(absolute.x,absolute.y,relative_out.x,relative_out.y);
									}			
			

			template <typename T0,typename T1>
				MF_DECLARE(void)	derelativate(const T& x, const T& y, T0&x_out, T1&y_out)	const
									{
										makeAbsolute(x,y,x_out,y_out);
									}
			
			template <typename T0,typename T1>
				MF_DECLARE(void)	makeAbsolute(const T& x, const T& y, T0&x_out, T1&y_out)	const
									{
										this->x.makeAbsolute(x,x_out);
										this->y.makeAbsolute(y,y_out);
									}
			template <typename T0,typename T1>
				MF_DECLARE(void)	derelativate(const TVec2<T0>& relative, TVec2<T0>&absolute_out)	const
									{
										x.makeAbsolute(relative.x,absolute_out.x);
										y.makeAbsolute(relative.y,absolute_out.y);
									}
			template <typename T0,typename T1>
				MF_DECLARE(void)	makeAbsolute(const TVec2<T0>& relative, TVec2<T0>&absolute_out)	const
									{
										x.makeAbsolute(relative.x,absolute_out.x);
										y.makeAbsolute(relative.y,absolute_out.y);
									}
			
	
			template <typename T0, typename T1>
				MF_DECLARE(void)	makeRelative(const Rect<T0>&absolute, Rect<T1>&relative_out)	const
									{
										x.relativateRange(absolute.x,relative_out.x);
										y.relativateRange(absolute.y,relative_out.y);
									}			
			
			template <typename T0, typename T1>
				MF_DECLARE(void)	relativate(const Rect<T0>&absolute, Rect<T1>&relative_out)	const
									{
										x.relativateRange(absolute.x,relative_out.x);
										y.relativateRange(absolute.y,relative_out.y);
									}			

			template <typename T0, typename T1>
				MF_DECLARE(void)	makeAbsolute(const Rect<T0>&relative, Rect<T1>&absolute_out)	const
									{
										x.derelativateRange(relative.x,absolute_out.x);
										y.derelativateRange(relative.y,absolute_out.y);
									}			
			
			template <typename T0, typename T1>
				MF_DECLARE(void)	derelativate(const Rect<T0>&relative, Rect<T1>&absolute_out)	const
									{
										x.derelativateRange(relative.x,absolute_out.x);
										y.derelativateRange(relative.y,absolute_out.y);
									}			



			MF_DECLARE(T&)			left()
									{
										return x.min;
									}
			MF_DECLARE(T&)			right()
									{
										return x.max;
									}
			MF_DECLARE(T&)			bottom()
									{
										return y.min;
									}
			MF_DECLARE(T&)			top()
									{
										return y.max;
									}

			MF_DECLARE(const T&)	left()	const
									{
										return x.min;
									}
			MF_DECLARE(const T&)	right()	const
									{
										return x.max;
									}
			MF_DECLARE(const T&)	bottom()	const
									{
										return y.min;
									}
			MF_DECLARE(const T&)	top()	const
									{
										return y.max;
									}

			MF_DECLARE(const T&)	operator[](index_t index)	const
									{
										DBG_ASSERT_LESS__(index,4);
										return axis[index%2].extreme[index/2];
									}
			MF_DECLARE(T&)			operator[](index_t index)
									{
										DBG_ASSERT_LESS__(index,4);
										return axis[index%2].extreme[index/2];
									}
			
			template <typename T0>
				MF_DECLARE(void)	getMin(TVec2<T0>&out)	const
									{
										out.x = (T0)x.min;
										out.y = (T0)y.min;
									}
			template <typename T0>
				MF_DECLARE(void)	getMax(TVec2<T0>&out)	const
									{
										out.x = (T0)x.max;
										out.y = (T0)y.max;
									}
			template <typename T0>
				MF_DECLARE(void)	getCenter(TVec2<T0>&out)	const
									{
										out.x = (T0)x.center();
										out.y = (T0)y.center();
									}
			template <typename T0>
				MF_DECLARE(void)	getExtend(TVec2<T0>&out)	const
									{
										out.x = (T0)x.extend();
										out.y = (T0)y.extend();
									}
			MF_DECLARE(TVec2<T>)	min()	const
									{
										TVec2<T> rs ={x.min,y.min};
										return rs;
									}
			MF_DECLARE(TVec2<T>)	max()	const
									{
										TVec2<T> rs ={x.max,y.max};
										return rs;
									}
			MF_DECLARE(TVec2<T>)	center()	const
									{
										TVec2<T> rs = {x.center(),y.center()};
										return rs;
									}
			MF_DECLARE(TVec2<T>)	extend()	const
									{
										TVec2<T> rs = {x.extend(),y.extend()};
										return rs;
									}

			template <typename T1>
				MF_DECLARE(bool)	intersects(const Rect<T1>&other)	const
									{
										return x.intersects(other.x) && y.intersects(other.y);
									}
			
			virtual String			toString()	const	override
									{
										return	"("+String(x.min)+", "+String(y.min)+") - ("+String(x.max)+", "+String(y.max)+")";
									}
			template <typename T1>
				MF_DECLARE(void)	operator*=(T1 factor)
									{
										x *= factor;
										y *= factor;
									}
			template <typename T1>
				MF_DECLARE(void)	operator/=(T1 factor)
									{
										x /= factor;
										y /= factor;
									}
		};
	
	typedef Rect<float>	TFloatRect;



	template <typename T=float>
		class Box: public IToString	//! General purpose box
		{
		public:
			union
			{
				struct
				{
					TRange<T>		x,y,z;
				};
				TRange<T>			axis[3];
			};

			/**/					Box(){}
			/**/					Box(const T&min_x, const T& min_y, const T& min_z, const T&max_x, const T& max_y, const T& max_z)
									{
										set(min_x, min_y, min_z, max_x, max_y, max_z);
									}
			/**/					Box(const TVec3<T>&min, const TVec3<T>&max)
									{
										set(min,max);
									}

			/**/					Box(const TVec3<T>&min_and_max)
									{
										set(min_and_max);
									}
			template<typename T1>
				/**/				Box(const Box<T1>&other)
									{
										x.set(other.x);
										y.set(other.y);
										z.set(other.z);
									}

			template <typename T1>
				Box<T>&				operator=(const Box<T1>&other)
									{
										x.set(other.x);
										y.set(other.y);
										z.set(other.z);
										return *this;
									}
									
			/**
			@brief Updates all values of the local box
			*/
			MF_DECLARE(void)		set(const T&min_x, const T& min_y, const T& min_z, const T&max_x, const T& max_y, const T& max_z)
									{
										x.set(min_x,max_x);
										y.set(min_y,max_y);
										z.set(min_z,max_z);
									}
			/**
			@brief Updates all values of the local rectangle
			*/
			MF_DECLARE(void)		set(const TVec3<T>&min, const TVec3<T>&max)
									{
										x.set(min.x,max.x);
										y.set(min.y,max.y);
										z.set(min.z,max.z);
									}
			/**
			@brief Updates all values of the local rectangle
			*/
			MF_DECLARE(void)		set(const TVec3<T>&min_and_max)
									{
										x.setAll(min_and_max.x);
										y.setAll(min_and_max.y);
										z.setAll(min_and_max.z);
									}
			/**
			@brief Relocates the center of the local box.
			Width, height, and depth remain unchanged.
			*/
			MF_DECLARE(void)		setCenter(const T&x, const T& y, const T&z)
									{
										this->x.setCenter(x);
										this->y.setCenter(y);
										this->z.setCenter(z);
									}
			/**
			@brief Updates all values of the local box so that the center points to the specified x, y, z coordinates, and width, height, depth match twice the specified extend
			@param center New center
			@param ext Extend in both directions along the respective axis. Equals half the resulting width/height/depth
			*/
			template<typename T0, typename T1>
				MF_DECLARE(void)	setCenter(const TVec3<T0>&center, const TVec3<T1>&ext)
									{
										x.setCenter(center.x,ext.x);
										y.setCenter(center.y,ext.y);
										z.setCenter(center.z,ext.z);
									}
			/**
			@brief Updates all values of the local box so that the center points to the specified x, y, z coordinates, and width, height, depth match twice the specified extend
			@param center New center
			@param ext Extend in both directions along the respective axis. Equals half the resulting width/height/depth
			*/
			template<typename T0, typename T1>
				MF_DECLARE(void)	setCenter(const TVec3<T0>&center, const T1&ext)
									{
										x.setCenter(center.x,ext);
										y.setCenter(center.y,ext);
										z.setCenter(center.z,ext);
									}
			/**
			@brief Updates all values of the local box to the specified value
			*/
			template <typename T0>
				MF_DECLARE(void)	setAll(const T0&value)
									{
										x.setAll(value);
										y.setAll(value);
										z.setAll(value);
									}
			/**
			@brief Updates all min values of the local box to the specified value
			*/
			template <typename T0>
				MF_DECLARE(void)	setAllMin(const T0&value)
									{
										x.min = (T)value;
										y.min = (T)value;
										z.min = (T)value;
									}
			/**
			@brief Updates all max values of the local box to the specified value
			*/
			template <typename T0>
				MF_DECLARE(void)	setAllMax(const T0&value)
									{
										x.max = (T)value;
										y.max = (T)value;
										z.max = (T)value;
									}
			/**
			@brief Updates the minimum values of the local box
			*/
			MF_DECLARE(void)		setMin(const TVec3<T>&min)
									{
										x.min = min.x;
										y.min = min.y;
										z.min = min.z;
									}
			/**
			@brief Updates the maximum values of the local box
			*/
			MF_DECLARE(void)		setMax(const TVec3<T>&max)
									{
										x.max = max.x;
										y.max = max.y;
										z.max = max.z;
									}
			/**
			@brief Updates the minimum values of the local box
			*/
			template <typename T0, typename T1, typename T2>
				MF_DECLARE(void)	setMin(const T0&min_x,const T1&min_y,const T2&min_z)
									{
										x.min = (T)min_x;
										y.min = (T)min_y;
										z.min = (T)min_z;
									}
			/**
			@brief Updates the maximum values of the local box
			*/
			template <typename T0, typename T1, typename T2>
				MF_DECLARE(void)	setMax(const T0&max_x,const T1&max_y,const T2&max_z)
									{
										x.max = (T)max_x;
										y.max = (T)max_y;
										z.max = (T)max_z;
									}

			MF_DECLARE(void)		scale(const T&by)	//! Scales the box from its central location
									{
										x.scale(by);
										y.scale(by);
										z.scale(by);
									}
			MF_DECLARE(void)		expand(const T& by)	//! Expands the box
									{
										x.expand(by);
										y.expand(by);
										z.expand(by);
									}
									
			MF_DECLARE(void)		constrainBy(const Box<T>&constraint)	//! Modifies the local box so that it lies within the specified constraint box
									{
										x.constrainBy(constraint.x);
										y.constrainBy(constraint.y);
										z.constrainBy(constraint.z);
									}
			/*!
			@brief	Determines whether or not the specified point lies within the local box
			@param p 3d point
			@return true if the point lies in the box
			*/
			template <typename T0>
				MF_DECLARE(bool)	contains(const TVec3<T0>&p)	const
									{
										return x.contains(p.x) && y.contains(p.y) && z.contains(p.z);
									}
		
			/*!
			@brief Modifies the local box so that it contains the specified point
			*/
			template <typename T0, typename T1, typename T2>
				MF_DECLARE(void)	include(const T0&x, const T1&y, const T2&z)
									{
										this->x.include(x);
										this->y.include(y);
										this->z.include(z);
									}
			/*!
			@brief Modifies the local box so that it contains the specified point
			*/
			template <typename T0>
				MF_DECLARE(void)	include(const TVec3<T0>&p)
									{
										x.include(p.x);
										y.include(p.y);
										z.include(p.z);
									}
			/*!
			@brief Modifies the local box so that it contains the specified other box
			*/
			template <typename T0>
				MF_DECLARE(void)	include(const Box<T0>&other)
									{
										x.include(other.x);
										y.include(other.y);
										z.include(other.z);
									}
		
			/*!
			\brief	Determines whether or not the specified point lies within the local rectangle
			\param x X coordinate of the point
			\param y Y coordinate of the point
			\return true if the point lies in the rectangle
			*/
			template <typename T0, typename T1, typename T2>
				MF_DECLARE(bool)	contains(const T0& x, const T1& y, const T2& z)	const
									{
										return this->x.contains(x) && this->y.contains(y) && this->z.contains(z);
									}
			/*!
			\brief Calculates the width of the box
			\return Rectangle width
			*/
			MF_DECLARE(T)			width()		const
									{
										return x.extend();
									}
			/*!
			\brief Calculates the height of the box
			\return Rectangle height
			*/
			MF_DECLARE(T)			height()	const
									{
										return y.extend();
									}
			/*!
			\brief Calculates the depth of the box
			\return Rectangle depth
			*/
			MF_DECLARE(T)			depth()	const
									{
										return z.extend();
									}
									
			template <typename T0, typename T1, typename T2>
				MF_DECLARE(void)	translate(const T0&delta_x, const T1&delta_y, const T2&delta_z)
									{
										x.translate(delta_x);
										y.translate(delta_y);
										z.translate(delta_z);
									}
			template <typename T0>
				MF_DECLARE(void)	translate(const TVec3<T0>&delta)
									{
										x.translate(delta.x);
										y.translate(delta.y);
										z.translate(delta.z);
									}
			


			template <typename T0, typename T1>
				MF_DECLARE(void)	makeRelative(const TVec3<T0>&absolute, TVec3<T1>&relative_out)	const
									{
										x.relativate(absolute.x,relative_out.x);
										y.relativate(absolute.y,relative_out.y);
										z.relativate(absolute.y,relative_out.z);
									}			
			
			template <typename T0>
				MF_DECLARE(void)	relativate(const TVec2<T>&absolute, TVec2<T0>&relative_out)	const
									{
										x.relativate(absolute.x,relative_out.x);
										y.relativate(absolute.y,relative_out.y);
										z.relativate(absolute.y,relative_out.z);
									}			
			

			template <typename T0,typename T1>
				MF_DECLARE(void)	derelativate(const TVec3<T0>& relative, TVec3<T0>&absolute_out)	const
									{
										x.makeAbsolute(relative.x,absolute_out.x);
										y.makeAbsolute(relative.y,absolute_out.y);
										z.makeAbsolute(relative.z,absolute_out.z);
									}
			template <typename T0,typename T1>
				MF_DECLARE(void)	makeAbsolute(const TVec3<T0>& relative, TVec3<T0>&absolute_out)	const
									{
										x.makeAbsolute(relative.x,absolute_out.x);
										y.makeAbsolute(relative.y,absolute_out.y);
										z.makeAbsolute(relative.z,absolute_out.z);
									}
			
	
			template <typename T0, typename T1>
				MF_DECLARE(void)	makeRelative(const Box<T0>&absolute, Box<T1>&relative_out)	const
									{
										x.relativateRange(absolute.x,relative_out.x);
										y.relativateRange(absolute.y,relative_out.y);
										z.relativateRange(absolute.z,relative_out.z);
									}			
			
			template <typename T0, typename T1>
				MF_DECLARE(void)	relativate(const Box<T0>&absolute, Box<T1>&relative_out)	const
									{
										x.relativateRange(absolute.x,relative_out.x);
										y.relativateRange(absolute.y,relative_out.y);
										z.relativateRange(absolute.z,relative_out.z);
									}			

			template <typename T0, typename T1>
				MF_DECLARE(void)	makeAbsolute(const Box<T0>&relative, Box<T1>&absolute_out)	const
									{
										x.derelativateRange(relative.x,absolute_out.x);
										y.derelativateRange(relative.y,absolute_out.y);
										z.derelativateRange(relative.z,absolute_out.z);
									}			
			
			template <typename T0, typename T1>
				MF_DECLARE(void)	derelativate(const Box<T0>&relative, Box<T1>&absolute_out)	const
									{
										x.derelativateRange(relative.x,absolute_out.x);
										y.derelativateRange(relative.y,absolute_out.y);
										z.derelativateRange(relative.z,absolute_out.z);
									}			
			template <typename T0>
				void				getMin(TVec3<T0>&out)	const
									{
										out.x = (T0)x.min;
										out.y = (T0)y.min;
										out.z = (T0)z.min;
									}
			template <typename T0>
				void				getMax(TVec3<T0>&out)	const
									{
										out.x = (T0)x.max;
										out.y = (T0)y.max;
										out.z = (T0)z.max;
									}
			template <typename T0>
				void				getCenter(TVec3<T0>&out)	const
									{
										out.x = (T0)x.center();
										out.y = (T0)y.center();
										out.z = (T0)z.center();
									}
			template <typename T0>
				void				getExtend(TVec3<T0>&out)	const
									{
										out.x = (T0)x.extend();
										out.y = (T0)y.extend();
										out.z = (T0)z.extend();
									}
			MF_DECLARE(TVec3<T>)	center()	const
									{
										TVec3<T> rs = {x.center(),y.center(),z.center()};
										return rs;
									}
			MF_DECLARE(TVec3<T>)	min()	const
									{
										TVec3<T> rs = {x.min,y.min,z.min};
										return rs;
									}
			MF_DECLARE(TVec3<T>)	max()	const
									{
										TVec3<T> rs = {x.max,y.max,z.max};
										return rs;
									}
			MF_DECLARE(TVec3<T>)	extend()	const
									{
										TVec3<T> rs = {x.extend(),y.extend(),z.extend()};
										return rs;
									}

			template <typename T1>
				MF_DECLARE(bool)	intersects(const Box<T1>&other)	const
									{
										return x.intersects(other.x) && y.intersects(other.y) && z.intersects(other.z);
									}
			
			virtual String			toString()	const	override
									{
										return	"("+String(x.min)+", "+String(y.min)+", "+String(z.min)+") - ("+String(x.max)+", "+String(y.max)+", "+String(z.max)+")";
									}
			template <typename T1>
				MF_DECLARE(void)	operator*=(T1 factor)
									{
										x *= factor;
										y *= factor;
										z *= factor;
									}
		};


	



	/**
		@brief Predefined global 2d vectors
	*/
	template <class C=float>
		struct Vector2
		{
			static const TVec2<C>	x_axis,
									y_axis,
									negative_x_axis,
									negative_y_axis,
									zero,
									null,
									one;
			static TVec2<C>			dummy;


		private:
									Vector2()	{};	//cannot be instantiated
		};

	/**
		@brief Predefined global 3d vectors
	*/
	template <class C=float>
		struct Vector
		{
			static const TVec3<C>	x_axis,
									y_axis,
									z_axis,
									negative_x_axis,
									negative_y_axis,
									negative_z_axis,
									zero,
									null,
									one;
			static TVec3<C>			dummy;


		private:
									Vector()	{};	//cannot be instantiated
		};

	/**
		@brief Predefined global 4d vectors
	*/
	template <class C=float>
		struct Vector4
		{
			static const TVec4<C>	x_axis,
									y_axis,
									z_axis,
									w_axis,
									negative_x_axis,
									negative_y_axis,
									negative_z_axis,
									negative_w_axis,
									zero,
									null,
									one;
			static TVec4<C>			dummy;


		private:
									Vector4()	{};	//cannot be instantiated
		};

	template <class C>
		const TVec2<C>		Vector2<C>::x_axis = {1,0};
	template <class C>
		const TVec2<C>		Vector2<C>::y_axis = {0,1};
	template <class C>
		const TVec2<C>		Vector2<C>::negative_x_axis = {-1,0};
	template <class C>
		const TVec2<C>		Vector2<C>::negative_y_axis = {0,-1};
	template <class C>
		const TVec2<C>		Vector2<C>::zero = {0,0};
	template <class C>
		const TVec2<C>		Vector2<C>::null = {0,0};
	template <class C>
		const TVec2<C>		Vector2<C>::one = {1,1};
	template <class C>
		TVec2<C>				Vector2<C>::dummy = {0,0};


	template <class C>
		const TVec3<C>		Vector<C>::x_axis = {1,0,0};
	template <class C>
		const TVec3<C>		Vector<C>::y_axis = {0,1,0};
	template <class C>
		const TVec3<C>		Vector<C>::z_axis = {0,0,1};
	template <class C>
		const TVec3<C>		Vector<C>::negative_x_axis = {-1,0,0};
	template <class C>
		const TVec3<C>		Vector<C>::negative_y_axis = {0,-1,0};
	template <class C>
		const TVec3<C>		Vector<C>::negative_z_axis = {0,0,-1};
	template <class C>
		const TVec3<C>		Vector<C>::zero = {0,0,0};
	template <class C>
		const TVec3<C>		Vector<C>::null = {0,0,0};
	template <class C>
		const TVec3<C>		Vector<C>::one = {1,1,1};
	template <class C>
		TVec3<C>				Vector<C>::dummy = {0,0,0};



	template <class C>
		const TVec4<C>		Vector4<C>::x_axis = {1,0,0,0};
	template <class C>
		const TVec4<C>		Vector4<C>::y_axis = {0,1,0,0};
	template <class C>
		const TVec4<C>		Vector4<C>::z_axis = {0,0,1,0};
	template <class C>
		const TVec4<C>		Vector4<C>::w_axis = {0,0,0,1};
	template <class C>
		const TVec4<C>		Vector4<C>::negative_x_axis = {-1,0,0,0};
	template <class C>
		const TVec4<C>		Vector4<C>::negative_y_axis = {0,-1,0,0};
	template <class C>
		const TVec4<C>		Vector4<C>::negative_z_axis = {0,0,-1,0};
	template <class C>
		const TVec4<C>		Vector4<C>::negative_w_axis = {0,0,0,-1};
	template <class C>
		const TVec4<C>		Vector4<C>::zero = {0,0,0,0};
	template <class C>
		const TVec4<C>		Vector4<C>::null = {0,0,0,0};
	template <class C>
		const TVec4<C>		Vector4<C>::one = {1,1,1,1};
	template <class C>
		TVec4<C>				Vector4<C>::dummy = {0,0,0,0};


/**
	\brief Assembler style vector operators

	Though these functions may look primitive, they are highly optimized.
	<br />Function variances:<br /><ul>
	<li>_someFunc(parameter(...),result[3]) - 3 dimensional variance</li>
	<li>_someFunc(inout_parameter[3],parameter(...)) - in-place 3 dimensional variance</li>
	<li>_someFunc2(parameter(...),result[2]) - 2 dimensional variance</li>
	<li>_someFuncV(parameter(...),*result) - N-dimensional compile time variance (Number of components are specified in the last template parameter)</li>
	<li>_someFunc(parameter(...),*result,dimensions) - N-dimensional run-time variance</li></ul>
*/
#if 0
namespace Math
{
	namespace ByReference
	{
		MFUNC3	(void)		_addValue(const C0*v, const C1&value, C2*out, count_t el_count);	//!< Calculates \b out = \b v +\b value for n-dimensional vectors and a given value \param value Scalar value to add \param el_count Number of elements in all vectors
		MFUNC3	(void)		_addValue(const C0 v[3], const C1&value, C2 out[3]);					//!< Calculates \b out = \b v +\b value for 3-dimensional vectors and a given value \param value Scalar value to add
		MFUNC3	(void)		_addValue2(const C0 v[2], const C1&value, C2 out[2]);				//!< Calculates \b out = \b v +\b value for 2-dimensional vectors and a given value \param value Scalar value to add
		MFUNC3	(void)		_addValue4(const C0 v[4], const C1&value, C2 out[4]);				//!< Calculates \b out = \b v +\b value for 4-dimensional vectors and a given value \param value Scalar value to add
		MFUNC3V (void)		_addValueV(const C0*v, const C1&value, C2*out);						//!< Calculates \b out = \b v +\b value for n-dimensional vectors and a given value \param value Scalar value to add
		MFUNC2	(void)		_addValue(C0*v, const C1&value, count_t el_count);					//!< Calculates \b v += \b value for each component of \b v .\param el_count Number of element in v
		MFUNC2	(void)		_addValue(C0 v[3], const C1&value);									//!< Calculates \b v += \b value for echo component of \b v .\param v 3d Vector \param value Scalar value to add
		MFUNC2	(void)		_addValue2(C0 v[2], const C1&value);									//!< Calculates \b v += \b value for echo component of \b v .\param v 2d Vector \param value Scalar value to add
		MFUNC2	(void)		_addValue4(C0 v[4], const C1&value);									//!< Calculates \b v += \b value for echo component of \b v .\param v 4d Vector \param value Scalar value to add
		MFUNC2V	(void)		_addValueV(C0*v, const C1&value);										//!< Calculates \b v += \b value for echo component of \b v .\param v n dimensional Vector \param value Scalar value to add
		MFUNC2	(C0)		_angleOne(const C0&x, const C1&y);							//!< Calculates the absolute angle of (x,y) \param x X-coordinate of the vector \param y Y-coordinate of the vector \return Absolute angle of (x,y) ranging in the range of [0,1]
		MFUNC2	(C0)		_angle360(const C0&x, const C1&y);							//!< Calculates the absolute angle of (x,y) \param x X-coordinate of the vector \param y Y-coordinate of the vector \return Absolute angle of (x,y) ranging in the range of [0,360]
		MFUNC2	(C0)		_angle2PI(const C0&x, const C1&y);							//!< Calculates the absolute angle of (x,y) \param x X-coordinate of the vector \param y Y-coordinate of the vector \return Absolute angle of (x,y) ranging in the range of [0,2PI]
		MFUNC3	(void)		_clamp(C0*v, const C1&min, const C2&max, count_t el_count);		//!< Clamps all members of \b v to the range [min,max] \param v Vector to clamp \param min Minimum constraint \param max Maximum constraint \param el_count Number of elements in \b v
		MFUNC3	(void)		_clamp(C0 v[3], const C1&min, const C2&max);						//!< Clamps all members of \b v to the range [min,max] \param v 3d vector to clamp \param min Minimum constraint \param max Maximum constraint
		MFUNC3	(void)		_clamp2(C0 v[2], const C1&min, const C2&max);						//!< Clamps all members of \b v to the range [min,max] \param v 2d vector to clamp \param min Minimum constraint \param max Maximum constraint
		MFUNC3	(void)		_clamp4(C0 v[4], const C1&min, const C2&max);						//!< Clamps all members of \b v to the range [min,max] \param v 4d vector to clamp \param min Minimum constraint \param max Maximum constraint
		MFUNC3V (void)		_clampV(C0*v, const C1&min, const C2&max);							//!< Clamps all members of \b v to the range [min,max] \param v Vector to clamp \param min Minimum constraint \param max Maximum constraint
		MFUNC3	(char)		_compare(const C0*v0, const C1*v1, const C2&tolerance, count_t el_count);	//!< Compares two number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \param el_count Number of elements to compare ot \a v0 and \a v1 \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
		MFUNC3	(char)		_compare(const C0 v0[3], const C1 v1[3], const C2&tolerance);				//!< Compares the first 3 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
		MFUNC3	(char)		_compare2(const C0 v0[2], const C1 v1[2], const C2&tolerance);		//!< Compares the first 2 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
		MFUNC3	(char)		_compare4(const C0 v0[4], const C1 v1[4], const C2&tolerance);		//!< Compares the first 4 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
		MFUNC3	(char)		_compare6(const C0 v0[6], const C1 v1[6], const C2&tolerance);		//!< Compares the first 6 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
		MFUNC3	(char)		_compare8(const C0 v0[8], const C1 v1[8], const C2&tolerance);		//!< Compares the first 8 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
		MFUNC3V	(char)		_compareV(const C0*v0, const C1*v1, const C2&tolerance);			//!< Compares two number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
		MFUNC3	(void)		_divide(const C0*v, const C1&value, C2*out, count_t el_count);		//!< Divides \b out by \b value and stores the resulting vector in \b out \param el_count Number of elements in \b v and \b out
		MFUNC3	(void)		_divide(const C0 v[3], const C1&value, C2 out[3]);					//!< Divides \b out by \b value and stores the resulting vector in \b out
		MFUNC3	(void)		_divide2(const C0 v[2], const C1&value, C2 out[2]);					//!< Divides \b out by \b value and stores the resulting vector in \b out
		MFUNC3	(void)		_divide4(const C0 v[4], const C1&value, C2 out[4]);					//!< Divides \b out by \b value and stores the resulting vector in \b out
		MFUNC3V (void)		_divideV(const C0*v, const C1&value, C2*out);						//!< Divides \b out by \b value and stores the resulting vector in \b out. The number of elements in \b v and \b out is specified by the last template parameter
		MFUNC2	(void)		_divide(C0*v, const C1&value, count_t el_count);					//!< Divides \b out by \b value. The result is directly applied to out (using /=) \param el_count Number of elements in \b v
		MFUNC2	(void)		_divide(C0 v[3], const C1&value);									//!< Divides \b out by \b value. The result is directly applied to out (using /=)
		MFUNC2	(void)		_divide2(C0 v[2], const C1&value);									//!< Divides \b out by \b value. The result is directly applied to out (using /=)
		MFUNC2	(void)		_divide4(C0 v[4], const C1&value);									//!< Divides \b out by \b value. The result is directly applied to out (using /=)
		MFUNC2V (void)		_divideV(C0*v, const C1&value);										//!< Divides \b out by \b value. The result is directly applied to out (using /=). The number of elements in \b v is specified by the last template parameter
		MFUNC4	(C3)		_intercept(const C0 position[3], const C1 velocity[3], C2 interception_velocity, C3 out[3]);	//!< Calculates the interception direction given the position and velocity of a remote point and velocity as well as the interception velocity. @param position Relative position of the remote point @param velocity Relative velocity of the remote point @param interception_velocity Velocity of the intercepting entity @param out [out] Normalized out direction. Valid only if the result is >0 @return linear time scalar at which point the point and intercepting entity collide. >0 if a valid interception was detected, 0 otherwise
		MFUNC4	(void)		_interpolate(const C0*v, const C1*w, const C2&factor, C3*out,count_t el_count);		//!< Interpolates \b v and \b w. The resulting vector is calculated via \b v + (\b w - \b v ) * \b factor \param el_count Number of elements in \b v, \b w and \b out
		MFUNC4	(void)		_interpolate(const C0 v[3], const C1 w[3], const C2&factor, C3 out[3]);					//!< Interpolates \b v and \b w. The resulting vector is calculated via \b v + (\b w - \b v ) * \b factor
		MFUNC4	(void)		_interpolate2(const C0 v[2], const C1 w[2], const C2&factor, C3 out[2]);					//!< Interpolates \b v and \b w. The resulting vector is calculated via \b v + (\b w - \b v ) * \b factor
		MFUNC4	(void)		_interpolate4(const C0 v[4], const C1 w[4], const C2&factor, C3 out[4]);					//!< Interpolates \b v and \b w. The resulting vector is calculated via \b v + (\b w - \b v ) * \b factor
		MFUNC4V (void)		_interpolateV(const C0*v, const C1*w, const C2&factor, C3*out);			//!< Interpolates \b v and \b w. The resulting vector is calculated via \b v + (\b w - \b v ) * \b factor. The number of elements in \b v, \b w and \b out is specified by the last template parameter
		MFUNC3	(void)		_multiply(const C0*v, const C1&value, C2*out, count_t el_count);	//!< Multiplies \b v with \b value and stores the resulting vector in \b out \param el_count Number of elements in \b v and \b out
		MFUNC3	(void)		_multiply(const C0 v[3], const C1&value, C2 out[3]);				//!< Multiplies \b v with \b value and stores the resulting vector in \b out 
		MFUNC3	(void)		_multiply2(const C0 v[2], const C1&value, C2 out[2]);				//!< Multiplies \b v with \b value and stores the resulting vector in \b out 
		MFUNC3	(void)		_multiply4(const C0 v[4], const C1&value, C2 out[4]);				//!< Multiplies \b v with \b value and stores the resulting vector in \b out 
		MFUNC3V (void)		_multiplyV(const C0*v, const C1&value, C2*out);						//!< Multiplies \b v with \b value and stores the resulting vector in \b out. The number of elements in \b v and \b out is specified by the last template parameter
		MFUNC2	(void)		_multiply(C0*v, const C1&value, count_t el_count);					//!< Multiplies \b v with \b value and stores the result in \b v (using /=) \param el_count Number of elements in \b v
		MFUNC2	(void)		_multiply(C0 v[3], const C1&value);									//!< Multiplies \b v with \b value and stores the result in \b v (using /=)
		MFUNC2	(void)		_multiply2(C0 v[2], const C1&value);								//!< Multiplies \b v with \b value and stores the result in \b v (using /=)
		MFUNC2	(void)		_multiply4(C0 v[4], const C1&value);								//!< Multiplies \b v with \b value and stores the result in \b v (using /=)
		MFUNC2V (void)		_multiplyV(C0*v, const C1&value);									//!< Multiplies \b v with \b value and stores the result in \b v (using /=). The number of elements in \b v is specified by the last template parameter
		MFUNC3	(void)		_multAdd(C0*b, const C1*d, const C2&scalar, count_t el_count);		//!< Calculates \b b += \b d * \b scalar \param el_count Number of elements in \b b and \b d
		MFUNC3	(void)		_multAdd(C0 b[3], const C1 d[3], const C2&scalar);					//!< Calculates \b b += \b d * \b scalar
		MFUNC3	(void)		_multAdd2(C0 b[2], const C1 d[2], const C2&scalar);					//!< Calculates \b b += \b d * \b scalar
		MFUNC3	(void)		_multAdd4(C0 b[4], const C1 d[4], const C2&scalar);					//!< Calculates \b b += \b d * \b scalar
		MFUNC3V (void)		_multAddV(C0*b, const C1*d,const C2&scalar);						//!< Calculates \b b += \b d * \b scalar. The number of elements in \b b and \b d is specified by the last template parameter
		MFUNC4	(void)		_multAdd(const C0*b, const C1*d, const C2&scalar, C3*out,count_t el_count);	//!< Calculates \b out = \b b + \b d * \b scalar \param el_count Number of elements in \b b, \b d and \b out
		MFUNC4	(void)		_multAdd(const C0 b[3], const C1 d[3], const C2&scalar, C3 out[3]);				//!< Calculates \b out = \b b + \b d * \b scalar
		MFUNC4	(void)		_multAdd2(const C0 b[2], const C1 d[2], const C2&scalar, C3 out[2]);			//!< Calculates \b out = \b b + \b d * \b scalar
		MFUNC4	(void)		_multAdd4(const C0 b[4], const C1 d[4], const C2&scalar, C3 out[4]);			//!< Calculates \b out = \b b + \b d * \b scalar
		MFUNC4V (void)		_multAddV(const C0*b, const C1*d,const C2&scalar, C3*out);						//!< Calculates \b out = \b b + \b d * \b scalar. The number of elements in \b b, \b d and \b out is specified by the last template parameter
		
		
		MFUNC6	(void)		_resolveSpline(const C0*p0, const C1*p1, const C2*p2, const C3*p3, const C4&t, C5*out, count_t el_count);		//!< Resolves a Spline point. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS position \param el_count Number of elements in each vector
		MFUNC6	(void)		_resolveSpline(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], const C4&t, C5 out[3]);			//!< Resolves a Spline point. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS position
		MFUNC6	(void)		_resolveSpline2(const C0 p0[2], const C1 p1[2], const C2 p2[2], const C3 p3[2], const C4&t, C5 out[2]);			//!< Resolves a Spline point. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS position
		MFUNC6	(void)		_resolveSpline4(const C0 p0[4], const C1 p1[4], const C2 p2[4], const C3 p3[4], const C4&t, C5 out[4]);			//!< Resolves a Spline point. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS position
		MFUNC6V (void)		_resolveSplineV(const C0*p0, const C1*p1, const C2*p2, const C3*p3, const C4&t, C5*out);							//!< Resolves a Spline point. The number of elements in each vector is specified by the last template parameter  \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS position
		MFUNC6	(void)		_resolveSplineAxis(const C0*p0, const C1*p1, const C2*p2, const C3*p3, const C4&t, C5*out, count_t el_count);		//!< Resolves the Spline direction at a specific position. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS direction \param el_count Number of elements in each vector
		MFUNC6	(void)		_resolveSplineAxis(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], const C4&t, C5 out[3]);		//!< Resolves the Spline direction at a specific position. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS direction
		MFUNC6	(void)		_resolveSplineAxis2(const C0 p0[2], const C1 p1[2], const C2 p2[2], const C3 p3[2], const C4&t, C5 out[2]);		//!< Resolves the Spline direction at a specific position. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS direction
		MFUNC6	(void)		_resolveSplineAxis4(const C0 p0[4], const C1 p1[4], const C2 p2[4], const C3 p3[4], const C4&t, C5 out[4]);		//!< Resolves the Spline direction at a specific position. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS direction
		MFUNC6V (void)		_resolveSplineAxisV(const C0*p0, const C1*p1, const C2*p2, const C3*p3, const C4&t, C5*out);						//!< Resolves the Spline direction at a specific position. The number of elements in each vector is specified by the last template parameter   \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS direction
		
		MFUNC6	(void)		_resolveUCBS(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], const C4&t, C5 out[3]);			//!< Resolves a Uniform cubic B-spline point. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved spline position
		MFUNC6	(void)		_resolveUCBSaxis(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], const C4&t, C5 out[3]);		//!< Resolves a Uniform cubic B-spline axis. The result is not normalized. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved spline direction
		
		MFUNC3	(void)		_scale(const C0*center, const C1&factor, C2*v, count_t el_count);												//!< Scales \b v relative to \b center by \b factor
		MFUNC3	(void)		_scale(const C0 center[3], const C1&factor, C2 v[3]);															//!< Scales \b v relative to \b center by \b factor
		MFUNC3V (void)		_scaleV(const C0*center, const C1*factor, C2*v);																//!< Scales \b v relative to \b center by \b factor
		MFUNC3	(void)		_scaleAbsolute(const C0*center, C1*v, const C2&distance, count_t el_count);				//!< Scales \b v relative to \b center so that its distance from \b center matches \b distance
		MFUNC3	(void)		_scaleAbsolute(const C0 center[3], C1 v[3], const C2&distance);						//!< Scales \b v relative to \b center so that its distance from \b center matches \b distance
		MFUNC3V (void)		_scaleAbsoluteV(const C0*center, C1*v, const C2&distance);							//!< Scales \b v relative to \b center so that its distance from \b center matches \b distance
		MFUNC3	(void)		_scaleAbsolute0(const C0*center, C1*v, const C2&distance, count_t el_count);		//!< Scales \b v relative to \b center so that its distance from \b center matches \b distance
		MFUNC3	(void)		_scaleAbsolute0(const C0 center[3], C1 v[3], const C2&distance);					//!< Scales \b v relative to \b center so that its distance from \b center matches \b distance
		MFUNC3V (void)		_scaleAbsolute0V(const C0*center, C1*v, const C2&distance);							//!< Scales \b v relative to \b center so that its distance from \b center matches \b distance
		MFUNC2	(void)		_set(C0*v, const C1&value, count_t el_count);										//!< Sets all components of \b v to \b value
		MFUNC2	(void)		_set(C0 v[3], const C1&value);														//!< Sets all components of \b v to \b value
		MFUNC2	(void)		_set2(C0 v[2], const C1&value);									//!< Sets all components of \b v to \b value
		MFUNC2	(void)		_set4(C0 v[4], const C1&value);									//!< Sets all components of \b v to \b value
		MFUNC2	(void)		_set6(C0 v[6], const C1&value);									//!< Sets all components of \b v to \b value
		MFUNC2	(void)		_set9(C0 v[9], const C1&value);									//!< Sets all components of \b v to \b value
		MFUNC2	(void)		_set16(C0 v[16], const C1&value);								//!< Sets all components of \b v to \b value
		MFUNC2V (void)		_setV(C0*v, const C1&value);									//!< Sets all components of \b v to \b value
		MFUNC2	(void)		_setLen(C0*v, const C1&length, count_t el_count);				//!< Scales \b v to match the specified length. \b v must be of length greater 0
		MFUNC2	(void)		_setLen(C0 v[3], const C1&length);								//!< Scales \b v to match the specified length. \b v must be of length greater 0
		MFUNC2	(void)		_setLen2(C0 v[2], const C1&length);								//!< Scales \b v to match the specified length. \b v must be of length greater 0
		MFUNC2	(void)		_setLen4(C0 v[4], const C1&length);								//!< Scales \b v to match the specified length. \b v must be of length greater 0
		MFUNC2V (void)		_setLenV(C0*v, const C1&length);								//!< Scales \b v to match the specified length. \b v must be of length greater 0
		MFUNC2	(void)		_setLen0(C0*v, const C1&length, count_t el_count);				//!< Scales/sets \b v to match the specified length. \b v may be of any length
		MFUNC2	(void)		_setLen0(C0 v[3], const C1&length);								//!< Scales/sets \b v to match the specified length. \b v may be of any length
		MFUNC2	(void)		_setLen02(C0 v[2], const C1&length);							//!< Scales/sets \b v to match the specified length. \b v may be of any length
		MFUNC2	(void)		_setLen04(C0 v[4], const C1&length);							//!< Scales/sets \b v to match the specified length. \b v may be of any length
		MFUNC2V (void)		_setLen0V(C0*v, const C1&length);								//!< Scales/sets \b v to match the specified length. \b v may be of any length
		MFUNC3	(bool)		_similar(const C0*v, const C1*w, const C2&distance, count_t el_count); //!< Returns true if the distance between \b v and \b w is less than \b distance
		MFUNC3	(bool)		_similar(const C0*v, const C1*w, const C2&distance);					//!< Returns true if the distance between \b v and \b w is less than \b distance
		MFUNC3	(bool)		_similar2(const C0*v, const C1*w, const C2&distance);					//!< Returns true if the distance between \b v and \b w is less than \b distance
		MFUNC3	(bool)		_similar4(const C0*v, const C1*w, const C2&distance);					//!< Returns true if the distance between \b v and \b w is less than \b distance
		MFUNC3V (bool)		_similarV(const C0*v, const C1*w, const C2&distance);			//!< Returns true if the distance between \b v and \b w is less than \b distance
		MFUNC4	(void)		_sphereCoords(const C0&p_angle, const C1&h_angle, const C2&radius, C3 out[3]);//!< Generates 3-dimensional sphere-coordinates \param p_angle Planar angle. Angle around the polar axis (-360 - +360) \param h_angle Height angle (-90 - +90) \param radius Sphere radius \param out Out vector
		MFUNC4	(void)		_sphereCoordsRad(const C0&p_angle, const C1&h_angle, C3 out[3]);			//!< Generates 3-dimensional sphere-coordinates \param p_angle Planar angle. Angle around the polar axis (-2PI - +2PI) \param h_angle Height angle (-PI/2 - +PI/2) \param out Out vector
		MFUNC3	(void)		_subtractValue(const C0*v, const C1&value, C2*out,count_t el_count);	//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b out ( \b out = \b v - \b value)
		MFUNC3	(void)		_subtractValue(const C0 v[3], const C1&value, C2 out[3]);				//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b out ( \b out = \b v - \b value)
		MFUNC3	(void)		_subtractValue2(const C0 v[2], const C1&value, C2 out[2]);					//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b out ( \b out = \b v - \b value)
		MFUNC3	(void)		_subtractValue4(const C0 v[4], const C1&value, C2 out[4]);		//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b out ( \b out = \b v - \b value)
		MFUNC3V (void)		_subtractValueV(const C0*v, const C1&value, C2*out);			//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b out ( \b out = \b v - \b value)
		MFUNC2	(void)		_subtractValue(C0*v, const C1&value,count_t el_count);		//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b v ( \b v -= \b value)
		MFUNC2	(void)		_subtractValue(C0 v[3], const C1&value);						//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b v ( \b v -= \b value)
		MFUNC2	(void)		_subtractValue2(C0 v[2], const C1&value);						//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b v ( \b v -= \b value)
		MFUNC2	(void)		_subtractValue4(C0 v[4], const C1&value);						//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b v ( \b v -= \b value)
		MFUNC2V	(void)		_subtractValueV(C0*v, const C1&value);						//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b v ( \b v -= \b value)
		MFUNC3	(void)		_v2(C0 v[2], const C1&x, const C2&y);					//!< Sets two components of a vector
		MFUNC4	(void)		_v3(C0 v[3], const C1&x, const C2&y, const C3&z);				//!< Sets three components of a vector
		MFUNC5	(void)		_v4(C0 v[4], const C1&x, const C2&y, const C3&z, const C4&a);	//!< Sets four components of a vector
	}


	MFUNC	(String)		_toString(const C*v, count_t el_count=3);					//!< Converts the specified vector into a string \param v Pointer to the first element of the vector \param el_count Number of elements in \b v .\return String representation of the vector
	MFUNC	(void)		_abs(C*v, count_t el_count);								//!< Converts vector elements into their absolute value \param v Pointer to the first element of the vector \param el_count Number of elements in \b v
	MFUNC	(void)		_abs(C v[3]);															//!< Converts the three first vector elements into their absolute value \param v Pointer to the first element of the 3d vector \b v
	MFUNCV	(void)		_absV(C*v);																//!< Converts vector elements into their resprective absolute value \param v Pointer to the first element of the vector \b v
	MFUNC4	(void)		_add(const C0*u, const C1*v, const C2*w, C3*out, count_t el_count);	//!< Calculates \b out = \b u +\b v +\b w for n-dimensional vectors \param out Pointer to the first element of the output vector \param el_count Number of elements in \b u \b v and \b w
	MFUNC4	(void)		_add(const C0 u[3], const C1 v[3], const C2 w[3], C3 out[3]);			//!< Calculates \b out = \b u +\b v +\b w for 3-dimensional vectors \param out Pointer to the first element of the 3 dimensional output vector
	MFUNC4	(void)		_add2(const C0 u[2], const C1 v[2], const C2 w[2], C3 out[2]);			//!< Calculates \b out = \b u +\b v +\b w for 2-dimensional vectors \param out Pointer to the first element of the 2 dimensional output vector
	MFUNC4	(void)		_add4(const C0 u[4], const C1 v[4], const C2 w[4], C3 out[4]);			//!< Calculates \b out = \b u +\b v +\b w for 4-dimensional vectors \param out Pointer to the first element of the 4 dimensional output vector
	MFUNC4V (void)		_addV(const C0*u, const C1*v, const C2*w, C3*out);						//!< Calculates \b out = \b u +\b v +\b w for n-dimensional vectors \param out Pointer to the first element of the n dimensional output vector
	MFUNC3	(void)		_add(const C0*v, const C1*w, C2*out, count_t el_count);	//!< Calculates \b out = \b u +\b v for n-dimensional vectors \param out Pointer to the first element of the output vector \param el_count Number of elements in all vectors
	MFUNC3	(void)		_add(const C0 v[3], const C1 w[3], C2 out[3]);				//!< Calculates \b out = \b u +\b v for 3-dimensional vectors \param out Pointer to the first element of the 3 dimensional output vector
	MFUNC3	(void)		_add2(const C0 v[2], const C1 w[2], C2 out[2]);				//!< Calculates \b out = \b u +\b v for 2-dimensional vectors \param out Pointer to the first element of the 2 dimensional output vector
	MFUNC3	(void)		_add4(const C0 v[4], const C1 w[4], C2 out[4]);				//!< Calculates \b out = \b u +\b v for 4-dimensional vectors \param out Pointer to the first element of the 4 dimensional output vector
	MFUNC3V (void)		_addV(const C0*v, const C1*w, C2*out);						//!< Calculates \b out = \b u +\b v for n-dimensional vectors \param out Pointer to the first element of the n dimensional output vector
	MFUNC2	(void)		_add(C0*v, const C1*w, count_t el_count);					//!< Calculates \b v += \b w for n-dimensional vectors \param v Vector to add to, \param w Vector to add \param el_count Number of elements in all vectors
	MFUNC2	(void)		_add(C0 v[3], const C1 w[3]);								//!< Calculates \b v += \b w for 3-dimensional vectors \param v Vector to add to, \param w Vector to add
	MFUNC2	(void)		_add2(C0 v[2], const C1 w[2]);								//!< Calculates \b v += \b w for 2-dimensional vectors \param v Vector to add to, \param w Vector to add
	MFUNC2	(void)		_add4(C0 v[4], const C1 w[4]);								//!< Calculates \b v += \b w for 4-dimensional vectors \param v Vector to add to, \param w Vector to add
	MFUNC2V (void)		_addV(C0*v, const C1*w);										//!< Calculates \b v += \b w for n-dimensional vectors \param v Vector to add to, \param w Vector to add
	#define				_addVal		_addValue
	#define				_addVal2	_addValue2
	#define				_addVal4	_addValue4
	#define				_addValV	_addValueV
	MFUNC3	(void)		_addValue(const C0*v, C1 value, C2*out, count_t el_count);	//!< Calculates \b out = \b v +\b value for n-dimensional vectors and a given value \param value Scalar value to add \param el_count Number of elements in all vectors
	MFUNC3	(void)		_addValue(const C0 v[3], C1 value, C2 out[3]);					//!< Calculates \b out = \b v +\b value for 3-dimensional vectors and a given value \param value Scalar value to add
	MFUNC3	(void)		_addValue2(const C0 v[2], C1 value, C2 out[2]);				//!< Calculates \b out = \b v +\b value for 2-dimensional vectors and a given value \param value Scalar value to add
	MFUNC3	(void)		_addValue4(const C0 v[4], C1 value, C2 out[4]);				//!< Calculates \b out = \b v +\b value for 4-dimensional vectors and a given value \param value Scalar value to add
	MFUNC3V (void)		_addValueV(const C0*v, C1 value, C2*out);						//!< Calculates \b out = \b v +\b value for n-dimensional vectors and a given value \param value Scalar value to add
	MFUNC2	(void)		_addValue(C0*v, C1 value, count_t el_count);					//!< Calculates \b v += \b value for each component of \b v .\param el_count Number of element in v
	MFUNC2	(void)		_addValue(C0 v[3], C1 value);									//!< Calculates \b v += \b value for echo component of \b v .\param v 3d Vector \param value Scalar value to add
	MFUNC2	(void)		_addValue2(C0 v[2], C1 value);									//!< Calculates \b v += \b value for echo component of \b v .\param v 2d Vector \param value Scalar value to add
	MFUNC2	(void)		_addValue4(C0 v[4], C1 value);									//!< Calculates \b v += \b value for echo component of \b v .\param v 4d Vector \param value Scalar value to add
	MFUNC2V	(void)		_addValueV(C0*v, C1 value);										//!< Calculates \b v += \b value for echo component of \b v .\param v n dimensional Vector \param value Scalar value to add
	MFUNC2	(bool)		_allGreater(const C0*v,const C1*w, count_t el_count);			//!< Checks wether or not all elements of \b v are greater or equal \b w .\param el_count Number of elements in \b v and \b w .\return true if all elements in \b v are greater or equal the respective elements in \b w
	MFUNC2	(bool)		_allGreater(const C0*v,const C1*w);							//!< Checks wether or not all 3 elements of \b v are greater or equal \b w .\return true if all elements in \b v are greater or equal the respective elements in \b w
	MFUNC2V (bool)		_allGreaterV(const C0*v, const C1*w);						//!< Checks wether or not all elements of \b v are greater or equal \b w .\return true if all elements in \b v are greater or equal the respective elements in \b w
	MFUNC2	(bool)		_allLess(const C0*v,const C1*w, count_t el_count);			//!< Checks wether or not all elements of \b v are less or equal \b w .\param el_count Number of elements in \b v and \b w .\return true if all elements in \b v are less or equal the respective elements in \b w
	MFUNC2	(bool)		_allLess(const C0*v,const C1*w);							//!< Checks wether or not all 3 elements of \b v are less or equal \b w .\return true if all elements in \b v are less or equal the respective elements in \b w
	MFUNC2V (bool)		_allLessV(const C0*v, const C1*w);							//!< Checks wether or not all elements of \b v are less or equal \b w .\return true if all elements in \b v are less or equal the respective elements in \b w
	MFUNC2	(C0)			_angle(const C0*v, const C1*w, count_t el_count);			//!< Calculates the angle between \b v and \b w . \param el_count Number of elements in \b v and \b w .\return Angle between \b v and \b w in the range [0, 180]
	MFUNC2	(C0)			_angle(const C0*v, const C1*w);								//!< Calculates the angle between \b v and \b w in 3d \return Angle between \b v and \b w in the range [0, 180]
	MFUNC2	(C0)			_angle2(const C0*v, const C1*w);							//!< Calculates the angle between \b v and \b w in 3d \return Angle between \b v and \b w in the range [0, 180]
	MFUNC2V (C0)			_angleV(const C0*v, const C1*w);							//!< Calculates the angle between \b v and \b w .\return Angle between \b v and \b w in the range [0, 180]
	MFUNC2	(C0)			_angleOne(C0 x, C1 y);										//!< Calculates the absolute angle of (x,y) \param x X-coordinate of the vector \param y Y-coordinate of the vector \return Absolute angle of (x,y) ranging in the range of [0,1]
	MFUNC	(C)			_angleOne(const C v[2]);									//!< Calculates the absolute angle of \b v .\return Absolute angle of v ranging in the range of [0,1]
	MFUNC2	(C0)			_angle360(C0 x, C1 y);										//!< Calculates the absolute angle of (x,y) \param x X-coordinate of the vector \param y Y-coordinate of the vector \return Absolute angle of (x,y) ranging in the range of [0,360]
	MFUNC	(C)			_angle360(const C v[2]);									//!< Calculates the absolute angle of \b v .\return Absolute angle of v ranging in the range of [0,360]
	MFUNC2	(C0)			_angle2PI(C0 x, C1 y);										//!< Calculates the absolute angle of (x,y) \param x X-coordinate of the vector \param y Y-coordinate of the vector \return Absolute angle of (x,y) ranging in the range of [0,2PI]
	MFUNC	(C)			_angle2PI(const C v[2]);									//!< Calculates the absolute angle of \b v .\return Absolute angle of v ranging in the range of [0,2PI]
	#define				_c2						_copy2								//!< Shortcut to _copy2()
	#define				_c3						_copy3								//!< Shortcut to _copy3()
	#define				_c4						_copy4								//!< Shortcut to _copy4()
	#define				_c6						_copy6								//!< Shortcut to _copy6()
	#define				_c8						_copy8									//!< Shortcut to _copy8()
	MFUNC3	(void)		_center(const C0*v,const C1*w,C2*out,count_t el_count);			//!< Calculates the arithmetic mean of \b v and \b w . The result is written to \b out . \param out Output vector \param el_count Number of elements in \b v \b w and \b out
	MFUNC3	(void)		_center(const C0 v[3],const C1 w[3],C2 out[3]);						//!< Calculates the 3d arithmetic mean of \b v and \b w . The result is written to \b out . \param out 3d output vector
	MFUNC3	(void)		_center2(const C0 v[2],const C1 w[2],C2 out[2]);					//!< Calculates the 2d arithmetic mean of \b v and \b w . The result is written to \b out . \param out 2d output vector
	MFUNC3	(void)		_center4(const C0 v[4],const C1 w[4],C2 out[4]);					//!< Calculates the 4d arithmetic mean of \b v and \b w . The result is written to \b out . \param out 4d output vector
	MFUNC3V (void)		_centerV(const C0*v, const C1*w,C2*out);							//!< Calculates the arithmetic mean of \b v and \b w . The result is written to \b out . \param out Output vector
	MFUNC4	(void)		_center(const C0*u,const C1*v,const C2*w,C3*out,count_t el_count); //!< Calculates the arithmetic mean of \b u, \b v and \b w . The result is written to \b out . \param out Output vector \param el_count Number of elements in \b u \b v \b w and \b out
	MFUNC4	(void)		_center(const C0 u[3],const C1 v[3],const C2 w[3],C3 out[3]);		//!< Calculates the 3d arithmetic mean of \b u, \b v and \b w . The result is written to \b out . \param out 3d output vector
	MFUNC4	(void)		_center2(const C0 u[3],const C1 v[3],const C2 w[3],C3 out[3]);		//!< Calculates the 2d arithmetic mean of \b u, \b v and \b w . The result is written to \b out . \param out 3d output vector
	MFUNC4	(void)		_center4(const C0 u[3],const C1 v[3],const C2 w[3],C3 out[3]);		//!< Calculates the 4d arithmetic mean of \b u, \b v and \b w . The result is written to \b out . \param out 3d output vector
	MFUNC4V (void)		_centerV(const C0*u, const C1*v, const C2*w,C3*out);				//!< Calculates the arithmetic mean of \b u, \b v and \b w . The result is written to \b out . \param out Output vector
	MFUNC3	(void)		_clamp(C0*v, C1 min, C2 max, count_t el_count);		//!< Clamps all members of \b v to the range [min,max] \param v Vector to clamp \param min Minimum constraint \param max Maximum constraint \param el_count Number of elements in \b v
	MFUNC3	(void)		_clamp(C0 v[3], C1 min, C2 max);						//!< Clamps all members of \b v to the range [min,max] \param v 3d vector to clamp \param min Minimum constraint \param max Maximum constraint
	MFUNC3	(void)		_clamp2(C0 v[2], C1 min, C2 max);						//!< Clamps all members of \b v to the range [min,max] \param v 2d vector to clamp \param min Minimum constraint \param max Maximum constraint
	MFUNC3	(void)		_clamp4(C0 v[4], C1 min, C2 max);						//!< Clamps all members of \b v to the range [min,max] \param v 4d vector to clamp \param min Minimum constraint \param max Maximum constraint
	MFUNC3V (void)		_clampV(C0*v, C1 min, C2 max);							//!< Clamps all members of \b v to the range [min,max] \param v Vector to clamp \param min Minimum constraint \param max Maximum constraint
	MFUNC	(void)		_clear(C*v, count_t el_count);										//!< Assigns 0 to all members of \b v \param v Vector to clear \param el_count Number of elements in \b v
	MFUNC	(void)		_clear(C*v);														//!< Assigns 0 to all members of \b v \param v 3d vector to clear
	MFUNC	(void)		_clear2(C*v);														//!< Assigns 0 to all members of \b v \param v 2d vector to clear
	MFUNC	(void)		_clear4(C*v);														//!< Assigns 0 to all members of \b v \param v 4d vector to clear
	MFUNC	(void)		_clear6(C*v);														//!< Assigns 0 to all members of \b v \param v 6d vector to clear
	MFUNCV	(void)		_clearV(C*v);																//!< Assigns 0 to all members of \b v \param v Vector to clear
	MFUNC3	(char)		_compare(const C0*v0, const C1*v1, C2 tolerance, count_t el_count);	//!< Compares two number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \param el_count Number of elements to compare ot \a v0 and \a v1 \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
	MFUNC3	(char)		_compare(const C0 v0[3], const C1 v1[3], C2 tolerance);				//!< Compares the first 3 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
	MFUNC3	(char)		_compare2(const C0 v0[2], const C1 v1[2], C2 tolerance);			//!< Compares the first 2 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
	MFUNC3	(char)		_compare4(const C0 v0[4], const C1 v1[4], C2 tolerance);			//!< Compares the first 4 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
	MFUNC3	(char)		_compare6(const C0 v0[6], const C1 v1[6], C2 tolerance);			//!< Compares the first 6 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
	MFUNC3	(char)		_compare8(const C0 v0[8], const C1 v1[8], C2 tolerance);			//!< Compares the first 8 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
	MFUNC3V	(char)		_compareV(const C0*v0, const C1*v1, C2 tolerance);					//!< Compares two number sequences. \param v0 First sequence, \param v1 Second sequence. \param tolerance Equality tolerance \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
	MFUNC2	(char)		_compare(const C0 v0[3], const C1 v1[3]);							//!< Compares the first 3 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
	MFUNC2	(char)		_compare2(const C0 v0[2], const C1 v1[2]);							//!< Compares the first 2 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
	MFUNC2	(char)		_compare4(const C0 v0[4], const C1 v1[4]);							//!< Compares the first 4 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
	MFUNC2	(char)		_compare6(const C0 v0[6], const C1 v1[6]);							//!< Compares the first 6 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
	MFUNC2	(char)		_compare8(const C0 v0[8], const C1 v1[8]);							//!< Compares the first 8 elements of both number sequences. \param v0 First sequence, \param v1 Second sequence. \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
	MFUNC2V	(char)		_compareV(const C0*v0, const C1*v1);								//!< Compares two number sequences. \param v0 First sequence, \param v1 Second sequence. \return 0 if both sequences are equal/similar, -1 if \a v0 is less than \a v1, +1 if \a v0 is greater than \a v1
	MFUNC2	(void)		_copy2(const C0 origin[2],C1 destination[2]);						//!< Copies 2 elements from \b origin to \b destination . \param origin Source to copy from \param destination Target to copy to
	MFUNC2	(void)		_copy3(const C0 origin[3],C1 destination[3]);						//!< Copies 3 elements from \b origin to \b destination . \param origin Source to copy from \param destination Target to copy to
	MFUNC2	(void)		_copy4(const C0 origin[4],C1 destination[4]);						//!< Copies 4 elements from \b origin to \b destination . \param origin Source to copy from \param destination Target to copy to
	MFUNC2	(void)		_copy6(const C0 origin[6],C1 destination[6]);						//!< Copies 6 elements from \b origin to \b destination . \param origin Source to copy from \param destination Target to copy to
	MFUNC2	(void)		_copy8(const C0 origin[8],C1 destination[8]);						//!< Copies 8 elements from \b origin to \b destination . \param origin Source to copy from \param destination Target to copy to
	MFUNC2V (void)		_copyV(const C0*origin, C1*target);									//!< Copies n elements from \b origin to \b destination . \param origin Source to copy from \param destination Target to copy to
	MFUNC2	(void)		_copy(const C0*origin,C1*destination,count_t el_count);			//!< Copies \b el_count elements from \b origin to \b destination . \param origin Source to copy from \param destination Target to copy to \param el_count Number of elements to copy
	MFUNC3	(void)		_cross(const C0 v[3], const C1 w[3], C2 out[3]);					//!< Writes the cross product of \b v and \b w to \b out
	MFUNC3	(C0)		_crossDot(const C0 u[3], const C1 v[3], const C2 w[3]);				//!< Calculates (\b u x \b v ) * \b w . \return (\b u x \b v ) * \b w
	MFUNC2	(void)		_crossVertical(const C0 v[3], C1 out[3]);							//!< Writes \b v x (0,1,0) to \b out
	MFUNC2	(void)		_crossZ(const C0 v[3], C1 out[3]);									//!< Writes \b v x (0,0,1) to \b out
	#define				_dist					_distance									//!< Shortcut to _distance()
	#define				_dist2					_distance2									//!< Shortcut to _distance2()
	#define				_dist4					_distance4									//!< Shortcut to _distance4()
	#define				_distV					_distanceV									//!< Shortcut to _distanceV()
	#define				_distSQR				_distanceSquare								//!< Shortcut to _distanceSquare()
	#define				_distSQR2				_distanceSquare2							//!< Shortcut to _distanceSquare2()
	#define				_distSQR4				_distanceSquare4							//!< Shortcut to _distanceSquare4()
	#define				_distSQRV				_distanceSquareV							//!< Shortcut to _distanceSquareV()
	MFUNC2	(C0)			_distance(const C0*v, const C1*w,count_t el_count);				//!< Calculates the distance between \b v and \b w . \param el_count Number of elements in \b v and \b w . \return Distance between \b v and \b w
	MFUNC2	(C0)			_distance(const C0 v[3], const C1 w[3]);							//!< Calculates the 3d distance between \b v and \b w . \return Distance between \b v and \b w
	MFUNC2	(C0)			_distance2(const C0 v[2], const C1 w[2]);							//!< Calculates the 2d distance between \b v and \b w . \return Distance between \b v and \b w
	MFUNC2	(C0)			_distance4(const C0 v[4], const C1 w[4]);							//!< Calculates the 4d distance between \b v and \b w . \return Distance between \b v and \b w
	MFUNC2V (C0)			_distanceV(const C0*v, const C1*w);									//!< Calculates the distance between \b v and \b w . \return Distance between \b v and \b w. The number of elements in \b v and \b w is specified by the last template parameter
	MFUNC3	(C0)			_distance(const C0 v0[3], const C1 v1[3], const C2 p[3]);			//!< Calculates the distance between a straight line and a point. \return Distance between the straight line going through \b v0 and \b v1 and the point \b p
	MFUNC3	(C0)			_distanceSquare(const C0 v0[3], const C1 v1[3], const C2 p[3]);		//!< Calculates the square of the distance between a straight line and a point. \return Distance between the straight line going through \b v0 and \b v1 and the point \b p
	MFUNC4	(C0)			_distance(const C0 v0[3], const C1 v1[3], const C2 w0[3], const C3 w1[3]);		//!< Calculates the distance between two straight lines in R3. \return Minimum distance between the straight line going through \b v0 and \b v1 and the straight line going through \b w0 and \b w1
	MFUNC2	(C0)			_distanceSquare(const C0*v, const C1*w,count_t el_count);			//!< Calculates the square of the distance between \b v and \b w (avoiding sqrt) \param el_count Number of elements in \b v and \b w \return Square of the distance between \b v and \b w
	MFUNC2	(C0)			_distanceSquare(const C0 v[3], const C1 w[3]);						//!< Calculates the square of the 3d distance between \b v and \b w (avoiding sqrt) \return Square of the distance between \b v and \b w
	MFUNC2	(C0)			_distanceSquare2(const C0 v[2], const C1 w[2]);						//!< Calculates the square of the 2d distance between \b v and \b w (avoiding sqrt) \return Square of the distance between \b v and \b w
	MFUNC2	(C0)			_distanceSquare4(const C0 v[4], const C1 w[4]);						//!< Calculates the square of the 4d distance between \b v and \b w (avoiding sqrt) \return Square of the distance between \b v and \b w
	MFUNC2V (C0)			_distanceSquareV(const C0*v, const C1*w);							//!< Calculates the square of the distance between \b v and \b w (avoiding sqrt) \return Square of the distance between \b v and \b w. The number of elements in \b v and \b w is specified by the last template parameter
	#define				_div					_divide										//!< Shortcut to _divide()
	#define				_div2					_divide2									//!< Shortcut to _divide2()
	#define				_div4					_divide4									//!< Shortcut to _divide4()
	#define				_divV					_divideV									//!< Shortcut to _divideV()
	MFUNC3	(void)		_divide(const C0*v, C1 value, C2*out, count_t el_count);		//!< Divides \b out by \b value and stores the resulting vector in \b out \param el_count Number of elements in \b v and \b out
	MFUNC3	(void)		_divide(const C0 v[3], C1 value, C2 out[3]);					//!< Divides \b out by \b value and stores the resulting vector in \b out
	MFUNC3	(void)		_divide2(const C0 v[2], C1 value, C2 out[2]);					//!< Divides \b out by \b value and stores the resulting vector in \b out
	MFUNC3	(void)		_divide4(const C0 v[4], C1 value, C2 out[4]);					//!< Divides \b out by \b value and stores the resulting vector in \b out
	MFUNC3V (void)		_divideV(const C0*v, C1 value, C2*out);						//!< Divides \b out by \b value and stores the resulting vector in \b out. The number of elements in \b v and \b out is specified by the last template parameter
	MFUNC2	(void)		_divide(C0*v, C1 value, count_t el_count);					//!< Divides \b out by \b value. The result is directly applied to out (using /=) \param el_count Number of elements in \b v
	MFUNC2	(void)		_divide(C0 v[3], C1 value);									//!< Divides \b out by \b value. The result is directly applied to out (using /=)
	MFUNC2	(void)		_divide2(C0 v[2], C1 value);									//!< Divides \b out by \b value. The result is directly applied to out (using /=)
	MFUNC2	(void)		_divide4(C0 v[4], C1 value);									//!< Divides \b out by \b value. The result is directly applied to out (using /=)
	MFUNC2V (void)		_divideV(C0*v, C1 value);										//!< Divides \b out by \b value. The result is directly applied to out (using /=). The number of elements in \b v is specified by the last template parameter
	MFUNC2	(C0)			_dot(const C0*v, const C1*w, count_t el_count);					//!< Calculates the dot-product of \b v and \b w \param el_count Number of elements in \b v and \b w
	MFUNC2	(C0)			_dot(const C0 v[3], const C1 w[3]);							//!< Calculates the dot-product of \b v and \b w in R3
	MFUNC2	(C0)			_dot2(const C0 v[2], const C1 w[2]);						//!< Calculates the dot-product of \b v and \b w in R2
	MFUNC2	(C0)			_dot4(const C0 v[4], const C1 w[4]);						//!< Calculates the dot-product of \b v and \b w in R4
	MFUNC2V (C0)			_dotV(const C0*v, const C1*w);								//!< Calculates the dot-product of \b v and \b w. The number of elements in \b v and \b w is specified by the last template parameter
	MFUNC	(C)			_dot(const C*v, count_t el_count);							//!< Calculates the dot square (\b v * \b v ) \param el_count Number of elements in \b v
	MFUNC	(C)			_dot(const C v[3]);											//!< Calculates the dot square (\b v * \b v ) in R3
	MFUNC	(C)			_dot2(const C v[2]);										//!< Calculates the dot square (\b v * \b v ) in R2
	MFUNC	(C)			_dot4(const C v[4]);										//!< Calculates the dot square (\b v * \b v ) in R4
	MFUNCV	(C)			_dotV(const C*v);											//!< Calculates the dot square (\b v * \b v ). The number of elements in \b v is specified by the last template parameter
	MFUNC2	(bool)		_equal(const C0*v, const C1*w, count_t el_count);			//!< Checks for equality of \b v and \b w \param el_count Number of elements in \b v and \b w \return true if all elements in \b v are equal to their respective counter parts in \b w
	MFUNC2	(bool)		_equal(const C0 v[3], const C1 w[3]);						//!< Checks for equality of \b v and \b w \return true if all elements in \b v are equal to their respective counter parts in \b w
	MFUNC2V (bool)		_equalV(const C0*v, const C1*w);							//!< Checks for equality of \b v and \b w. The number of elements in \b v and \b w is specified by the last template parameter \return true if all elements in \b v are equal to their respective counter parts in \b w
	MFUNC	(C)			_greatest(const C*v, count_t el_count);					//!< Determines the greatest component of \b v \param el_count Number of element in \b v
	MFUNC	(C)			_greatest(const C v[3]);									//!< Determines the greatest component of \b v
	MFUNC	(C)			_greatest2(const C v[2]);									//!< Determines the greatest component of \b v
	MFUNC	(C)			_greatest4(const C v[4]);									//!< Determines the greatest component of \b v
	MFUNCV	(C)			_greatestV(const C*v);										//!< Determines the greatest component of \b v. The number of elements in \b v is specified by the last template parameter
	MFUNC2	(C0)			_intensity(const C0*v, const C1*w, count_t el_count);		//!< Calculates the cosinus of the angle between \b v and \b w \param el_count Number of elements in \b v and \b w
	MFUNC2	(C0)			_intensity(const C0 v[3], const C1 w[3]);					//!< Calculates the cosinus of the angle between \b v and \b w in R3
	MFUNC2	(C0)			_intensity2(const C0 v[2], const C1 w[2]);					//!< Calculates the cosinus of the angle between \b v and \b w in R2
	MFUNC2	(C0)			_intensity4(const C0 v[4], const C1 w[4]);					//!< Calculates the cosinus of the angle between \b v and \b w in R4
	MFUNC2V (C0)			_intensityV(const C0*v, const C1*w);						//!< Calculates the cosinus of the angle between \b v and \b w. The number of elements in \b v and \b w is specified by the last template parameter
	MFUNC4	(void)		_interpolate(const C0*v, const C1*w, C2 factor, C3*out,count_t el_count);	//!< Interpolates \b v and \b w. The resulting vector is calculated via \b v + (\b w - \b v ) * \b factor \param el_count Number of elements in \b v, \b w and \b out
	MFUNC4	(void)		_interpolate(const C0 v[3], const C1 w[3], C2 factor, C3 out[3]);				//!< Interpolates \b v and \b w. The resulting vector is calculated via \b v + (\b w - \b v ) * \b factor
	MFUNC4	(void)		_interpolate2(const C0 v[2], const C1 w[2], C2 factor, C3 out[2]);					//!< Interpolates \b v and \b w. The resulting vector is calculated via \b v + (\b w - \b v ) * \b factor
	MFUNC4	(void)		_interpolate4(const C0 v[4], const C1 w[4], C2 factor, C3 out[4]);					//!< Interpolates \b v and \b w. The resulting vector is calculated via \b v + (\b w - \b v ) * \b factor
	MFUNC4V (void)		_interpolateV(const C0*v, const C1*w, C2 factor, C3*out);			//!< Interpolates \b v and \b w. The resulting vector is calculated via \b v + (\b w - \b v ) * \b factor. The number of elements in \b v, \b w and \b out is specified by the last template parameter
	MFUNC	(bool)		_isNAN(const C*v, count_t el_count);									//!< Checks if at least one component of \b v is NAN (Not A Number) \param el_count number of elements in \b v
	MFUNC	(bool)		_isNAN(const C v[3]);										//!< Checks if at least one component of \b v is NAN (Not A Number)
	MFUNC	(bool)		_isNAN2(const C v[2]);										//!< Checks if at least one component of \b v is NAN (Not A Number)
	MFUNC	(bool)		_isNAN4(const C v[4]);										//!< Checks if at least one component of \b v is NAN (Not A Number)
	MFUNCV	(bool)		_isNANV(const C*v);											//!< Checks if at least one component of \b v is NAN (Not A Number). The number of elements in \b v is specified by the last template parameter
	MFUNC	(C)			_least(const C*v, count_t el_count);						//!< Determines the least component of \b v \param el_count Number of element in \b v
	MFUNC	(C)			_least(const C v[3]);										//!< Determines the least component of \b v
	MFUNC	(C)			_least2(const C v[2]);										//!< Determines the least component of \b v
	MFUNC	(C)			_least4(const C v[4]);										//!< Determines the least component of \b v
	MFUNCV	(C)			_leastV(const C*v);											//!< Determines the least component of \b v. The number of elements in \b v is specified by the last template parameter
	MFUNC	(C)			_length(const C*v, count_t el_count);						//!< Calculates the length of \b v \param el_count Number of elements in \b v
	MFUNC	(C)			_length(const C v[3]);										//!< Calculates the length of \b v
	MFUNC	(C)			_length2(const C v[2]);										//!< Calculates the length of \b v
	MFUNC	(C)			_length4(const C v[4]);										//!< Calculates the length of \b v
	MFUNCV	(C)			_lengthV(const C*v);										//!< Calculates the length of \b v. The number of elements in \b v is specified by the last template parameter
	#define				_mad					_multAdd							//!< Shortcut to _multAdd()
	#define				_mad2					_multAdd2							//!< Shortcut to _multAdd(2)
	#define				_mad4					_multAdd4								//!< Shortcut to _multAdd4()
	#define				_madV					_multAddV								//!< Shortcut to _multAddV()
	MFUNC2	(void)		_makeNormalTo(C0 v[3], const C1 reference[3]);					//!< Alters @b v so that it is orthogonal to @b reference @param v [in/out] Vector to alter @param reference Vector to use as reference that @a v should be orthogonal to
	MFUNC	(const C&)	_max(const C v[3]);												//!< Finds the maximum value of the specified 3d vector @param v [in] vector to find the maximum of @return maximum
	MFUNC3	(void)		_max(const C0 v[3], const C1 w[3], C2 out[3]);					//!< Writes the component wise maximum of the two vectors to the resulting vector
	MFUNC2	(C0)			_maxAxisDistance(const C0*v, const C1*w, count_t el_count);	//!< Returns the greates component-difference of \b v and \b w \param el_count Number of elements in \b v and \b w
	MFUNC2	(C0)			_maxAxisDistance(const C0 v[3], const C1 w[3]);					//!< Returns the greates component-difference of \b v and \b w
	MFUNC2V (C0)			_maxAxisDistanceV(const C0*v, const C1*w);						//!< Returns the greates component-difference of \b v and \b w. The number of elements in \b v and \b w is specified by the last template parameter
	MFUNC	(const C&)	_min(const C v[3]);												//!< Finds the minimum value of the specified 3d vector @param v [in] vector to find the minimum of @return minimum
	MFUNC3	(void)		_min(const C0 v[3], const C1 w[3], C2 out[3]);					//!< Writes the component wise minimum of the two vectors to the resulting vector
	MFUNC3	(void)		_multiply(const C0*v, C1 value, C2*out, count_t el_count);	//!< Multiplies \b v with \b value and stores the resulting vector in \b out \param el_count Number of elements in \b v and \b out
	MFUNC3	(void)		_multiply(const C0 v[3], C1 value, C2 out[3]);				//!< Multiplies \b v with \b value and stores the resulting vector in \b out 
	MFUNC3	(void)		_multiply2(const C0 v[2], C1 value, C2 out[2]);				//!< Multiplies \b v with \b value and stores the resulting vector in \b out 
	MFUNC3	(void)		_multiply4(const C0 v[4], C1 value, C2 out[4]);				//!< Multiplies \b v with \b value and stores the resulting vector in \b out 
	MFUNC3V (void)		_multiplyV(const C0*v, C1 value, C2*out);						//!< Multiplies \b v with \b value and stores the resulting vector in \b out. The number of elements in \b v and \b out is specified by the last template parameter
	MFUNC2	(void)		_multiply(C0*v, C1 value, count_t el_count);					//!< Multiplies \b v with \b value and stores the result in \b v (using /=) \param el_count Number of elements in \b v
	MFUNC2	(void)		_multiply(C0 v[3], C1 value);									//!< Multiplies \b v with \b value and stores the result in \b v (using /=)
	MFUNC2	(void)		_multiply2(C0 v[2], C1 value);								//!< Multiplies \b v with \b value and stores the result in \b v (using /=)
	MFUNC2	(void)		_multiply4(C0 v[4], C1 value);								//!< Multiplies \b v with \b value and stores the result in \b v (using /=)
	MFUNC2V (void)		_multiplyV(C0*v, C1 value);									//!< Multiplies \b v with \b value and stores the result in \b v (using /=). The number of elements in \b v is specified by the last template parameter
	MFUNC2	(void)		_normal(const C0 v[3], C1 out[3]);									//!< Determines an arbitrary vector that is normal to the specified one. The resulting vector is defined as long as |v| is greater 0
	MFUNC	(void)		_normalize(C*v, count_t el_count);									//!< Normalizes \b v (scales \b v so that |\b v | equals 1). The length of \b v before normalization must be greater 0 \param el_count Number of elements in \b v
	MFUNC	(void)		_normalize(C v[3]);													//!< Normalizes \b v (scales \b v so that |\b v | equals 1). The length of \b v before normalization must be greater 0
	MFUNC	(void)		_normalize2(C v[2]);												//!< Normalizes \b v (scales \b v so that |\b v | equals 1). The length of \b v before normalization must be greater 0
	MFUNC	(void)		_normalize4(C v[4]);												//!< Normalizes \b v (scales \b v so that |\b v | equals 1). The length of \b v before normalization must be greater 0
	MFUNCV	(void)		_normalizeV(C*v);													//!< Normalizes \b v (scales \b v so that |\b v | equals 1). The length of \b v before normalization must be greater 0. The number of elements in \b v is specified by the last template parameter
	MFUNC	(void)		_normalize0(C*v, count_t el_count);								//!< Normalizes \b v (scales/sets \b v so that |\b v | equals 1). \b v may be of any length before normalization. \param el_count Number of elements in \b v
	MFUNC	(void)		_normalize0(C v[3]);												//!< Normalizes \b v (scales/sets \b v so that |\b v | equals 1). \b v may be of any length before normalization
	MFUNC	(void)		_normalize02(C v[2]);												//!< Normalizes \b v (scales/sets \b v so that |\b v | equals 1). \b v may be of any length before normalization
	MFUNC	(void)		_normalize04(C v[4]);												//!< Normalizes \b v (scales/sets \b v so that |\b v | equals 1). \b v may be of any length before normalization
	MFUNCV	(void)		_normalize0V(C*v);													//!< Normalizes \b v (scales/sets \b v so that |\b v | equals 1). \b v may be of any length before normalization. The number of elements in \b v is specified by the last template parameter
	#define				_mul					_multiply
	#define				_mul2					_multiply2
	#define				_mul4					_multiply4
	#define				_mulV					_multiplyV
	#define				_mult					_multiply
	#define				_mult2					_multiply2
	#define				_mult4					_multiply4
	#define				_multV					_multiplyV
	MFUNC3	(void)		_multAdd(C0*b, const C1*d, C2 scalar, count_t el_count);		//!< Calculates \b b += \b d * \b scalar \param el_count Number of elements in \b b and \b d
	MFUNC3	(void)		_multAdd(C0 b[3], const C1 d[3], C2 scalar);					//!< Calculates \b b += \b d * \b scalar
	MFUNC3	(void)		_multAdd2(C0 b[2], const C1 d[2], C2 scalar);					//!< Calculates \b b += \b d * \b scalar
	MFUNC3	(void)		_multAdd4(C0 b[4], const C1 d[4], C2 scalar);					//!< Calculates \b b += \b d * \b scalar
	MFUNC3V (void)		_multAddV(C0*b, const C1*d,C2 scalar);						//!< Calculates \b b += \b d * \b scalar. The number of elements in \b b and \b d is specified by the last template parameter
	MFUNC4	(void)		_multAdd(const C0*b, const C1*d, C2 scalar, C3*out,count_t el_count);	//!< Calculates \b out = \b b + \b d * \b scalar \param el_count Number of elements in \b b, \b d and \b out
	MFUNC4	(void)		_multAdd(const C0 b[3], const C1 d[3], C2 scalar, C3 out[3]);				//!< Calculates \b out = \b b + \b d * \b scalar
	MFUNC4	(void)		_multAdd2(const C0 b[2], const C1 d[2], C2 scalar, C3 out[2]);			//!< Calculates \b out = \b b + \b d * \b scalar
	MFUNC4	(void)		_multAdd4(const C0 b[4], const C1 d[4], C2 scalar, C3 out[4]);			//!< Calculates \b out = \b b + \b d * \b scalar
	MFUNC4V (void)		_multAddV(const C0*b, const C1*d,C2 scalar, C3*out);						//!< Calculates \b out = \b b + \b d * \b scalar. The number of elements in \b b, \b d and \b out is specified by the last template parameter
	MFUNC2	(bool)		_oneGreater(const C0*v, const C1*w, count_t el_count); 			//!< Checks if there is at least one element in \b v >= than the respective element in \b w \param el_count Number of elements in \b v and \b w
	MFUNC2	(bool)		_oneGreater(const C0 v[3], const C1 w[3]);							//!< Checks if there is at least one element in \b v >= than the respective element in \b w
	MFUNC2V (bool)		_oneGreaterV(const C0*v, const C1*w);								//!< Checks if there is at least one element in \b v >= than the respective element in \b w. The number of elements in \b v and \b w is specified by the last template parameter
	MFUNC2	(bool)		_oneLess(const C0*v, const C1*w, count_t el_count);				//!< Checks if there is at least one element in \b v <= than the respective element in \b w \param el_count Number of elements in \b v and \b w
	MFUNC2	(bool)		_oneLess(const C0 v[3], const C1 w[3]);								//!< Checks if there is at least one element in \b v <= than the respective element in \b w
	MFUNC2V (bool)		_oneLessV(const C0*v, const C1*w);									//!< Checks if there is at least one element in \b v <= than the respective element in \b w. The number of elements in \b v and \b w is specified by the last template parameter
	MFUNC3	(C0)			_planePointDistance(const C0*base, const C1*normal, const C2*p, count_t el_count); //!< Calculates the distance between a point and a plane \param base Base vector of the plane \param normal Plane normal (any length greater 0) \param p Point \param el_count Number of elements in \b base, \b normal and \b p \return distance
	MFUNC3	(C0)			_planePointDistance(const C0 base[3], const C1 normal[3], const C2 p[3]);			//!< Calculates the distance between a point and a plane in R3 \param base Base vector of the plane \param normal Plane normal (any length greater 0) \param p Point \return distance
	MFUNC3	(C0)			_planePointDistance2(const C0 base[2], const C1 normal[2], const C2 p[2]);			//!< Calculates the distance between a point and a plane in R2 \param base Base vector of the plane \param normal Plane normal (any length greater 0) \param p Point \return distance
	MFUNC3	(C0)			_planePointDistance4(const C0 base[4], const C1 normal[4], const C2 p[4]);			//!< Calculates the distance between a point and a plane in R4 \param base Base vector of the plane \param normal Plane normal (any length greater 0) \param p Point \return distance
	MFUNC3	(C0)			_planePointDistanceV(const C0*base, const C1*normal, const C2*p);					//!< Calculates the distance between a point and a plane. The number of elements in \b base, \b normal and \b p is specified by the last template parameter \param base Base vector of the plane \param normal Plane normal (any length greater 0) \param p Point \return distance
	MFUNC3	(C0)			_planePointDistanceN(const C0*base, const C1*normal, const C2*p, count_t el_count);//!< Calculates the distance between a point and a plane with \b normal pre-normalized \param base Base vector of the plane \param normal Plane normal (required normalized) \param p Point \param el_count Number of elements in \b base, \b normal and \b p \return distance
	MFUNC3	(C0)			_planePointDistanceN(const C0 base[3], const C1 normal[3], const C2 p[3]);			//!< Calculates the distance between a point and a plane in R3  with \b normal pre-normalized \param base Base vector of the plane \param normal Plane normal (required normalized) \param p Point \return distance
	MFUNC3	(C0)			_planePointDistanceN2(const C0 base[2], const C1 normal[2], const C2 p[2]);			//!< Calculates the distance between a point and a plane in R2  with \b normal pre-normalized \param base Base vector of the plane \param normal Plane normal (required normalized) \param p Point \return distance
	MFUNC3	(C0)			_planePointDistanceN4(const C0 base[4], const C1 normal[4], const C2 p[4]);			//!< Calculates the distance between a point and a plane in R4  with \b normal pre-normalized \param base Base vector of the plane \param normal Plane normal (required normalized) \param p Point \return distance
	MFUNC3	(C0)			_planePointDistanceNV(const C0*base, const C1*normal, const C2*p);					//!< Calculates the distance between a point and a plane with \b normal pre-normalized. The number of elements in \b base, \b normal and \b p is specified by the last template parameter \param base Base vector of the plane \param normal Plane normal (required normalized) \param p Point \return distance
	MFUNC4	(C0)			_planePointDistance(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p[3]);	//!< Calculates the distance between a point and a plane formed by a triangle \param p0 First vertex of the plane triangle \param p1 Second vertex of the plane triangle \param p2 Third vertex of the plane triangle \param p Point \return distance
	MFUNC4	(void)		_reflect(const C0*base, const C1*normal, const C2*p, C3*out, count_t el_count);	//!< Reflects \b p on the given plane and stores the resulting vector in \b out \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (may be of any length) \param p Point to reflect \param out Out vector for the reflection result \param el_count Number of elements in each vector
	MFUNC4	(void)		_reflect(const C0 base[3], const C1 normal[3], const C2 p[3], C3 out[3]);			//!< Reflects \b p on the given plane and stores the resulting vector in \b out \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (may be of any length) \param p Point to reflect \param out Out vector for the reflection result
	MFUNC4	(void)		_reflect2(const C0 base[2], const C1 normal[2], const C2 p[2], C3 out[2]);			//!< Reflects \b p on the given plane and stores the resulting vector in \b out \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (may be of any length) \param p Point to reflect \param out Out vector for the reflection result
	MFUNC4	(void)		_reflect4(const C0 base[4], const C1 normal[4], const C2 p[4], C3 out[4]);			//!< Reflects \b p on the given plane and stores the resulting vector in \b out \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (may be of any length) \param p Point to reflect \param out Out vector for the reflection result
	MFUNC4	(void)		_reflectV(const C0*base, const C1*normal, const C2*p, C3*out);						//!< Reflects \b p on the given plane and stores the resulting vector in \b out. The number of elements in each vector is specified by the last template parameter \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (may be of any length) \param p Point to reflect \param out Out vector for the reflection result
	MFUNC3	(void)		_reflect(const C0*base, const C1*normal, C2*p, count_t el_count);					//!< Reflects \b p on the given plane \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (may be of any length) \param p (in/out) Point to reflect \param el_count Number of elements in each vector
	MFUNC3	(void)		_reflect(const C0 base[3], const C1 normal[3], C2 p[3]);							//!< Reflects \b p on the given plane \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (may be of any length) \param p (in/out) Point to reflect
	MFUNC3	(void)		_reflect2(const C0 base[2], const C1 normal[2], C2 p[2]);						//!< Reflects \b p on the given plane \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (may be of any length) \param (in/out) p Point to reflect
	MFUNC3	(void)		_reflect4(const C0 base[4], const C1 normal[4], C2 p[4]);						//!< Reflects \b p on the given plane \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (may be of any length) \param (in/out) p Point to reflect
	MFUNC3	(void)		_reflectV(const C0*base, const C1*normal, C2*p);						//!< Reflects \b p on the given plane. The number of elements in each vector is specified by the last template parameter \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (may be of any length) \param p (in/out) Point to reflect
	MFUNC4	(void)		_reflectN(const C0*base, const C1*normal, const C2*p, C3*out, count_t el_count);	//!< Reflects \b p on the given plane and stores the resulting vector in \b out \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (required normalized) \param p Point to reflect \param out Out vector for the reflection result \param el_count Number of elements in each vector
	MFUNC4	(void)		_reflectN(const C0 base[3], const C1 normal[3], const C2 p[3], C3 out[3]);			//!< Reflects \b p on the given plane and stores the resulting vector in \b out \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (required normalized) \param p Point to reflect \param out Out vector for the reflection result
	MFUNC4	(void)		_reflectN2(const C0 base[2], const C1 normal[2], const C2 p[2], C3 out[2]);			//!< Reflects \b p on the given plane and stores the resulting vector in \b out \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (required normalized) \param p Point to reflect \param out Out vector for the reflection result
	MFUNC4	(void)		_reflectN4(const C0 base[4], const C1 normal[4], const C2 p[4], C3 out[4]);			//!< Reflects \b p on the given plane and stores the resulting vector in \b out \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (required normalized) \param p Point to reflect \param out Out vector for the reflection result
	MFUNC4	(void)		_reflectNV(const C0*base, const C1*normal, const C2*p, C3*out);						//!< Reflects \b p on the given plane and stores the resulting vector in \b out. The number of elements in each vector is specified by the last template parameter \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (required normalized) \param p Point to reflect \param out Out vector for the reflection result
	MFUNC3	(void)		_reflectN(const C0*base, const C1*normal, C2*p, count_t el_count);					//!< Reflects \b p on the given plane \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (required normalized) \param p (in/out) Point to reflect \param el_count Number of elements in each vector
	MFUNC3	(void)		_reflectN(const C0 base[3], const C1 normal[3], C2 p[3]);							//!< Reflects \b p on the given plane \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (required normalized) \param p (in/out) Point to reflect
	MFUNC3	(void)		_reflectN2(const C0 base[2], const C1 normal[2], C2 p[2]);						//!< Reflects \b p on the given plane \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (required normalized) \param p (in/out) Point to reflect
	MFUNC3	(void)		_reflectN4(const C0 base[4], const C1 normal[4], C2 p[4]);						//!< Reflects \b p on the given plane \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (required normalized) \param p (in/out) Point to reflect
	MFUNC3	(void)		_reflectNV(const C0*base, const C1*normal, C2*p);								//!< Reflects \b p on the given plane. The number of elements in each vector is specified by the last template parameter \param base Base point of the reflection plane \param normal Normal vector of the reflecting plane (required normalized) \param p (in/out) Point to reflect
	MFUNC3	(void)		_reflectVector(const C0*normal, const C1*v, C2*out, count_t el_count);	//!< Reflects \b v about the given normal and stores the resulting vector in \b out \param normal Normal vector of the reflecting plane (may be of any length) \param v Vector to reflect \param out Out vector for the reflection result \param el_count Number of elements in each vector
	MFUNC3	(void)		_reflectVector(const C0 normal[3], const C1 v[3], C2 out[3]);			//!< Reflects \b v about the given normal and stores the resulting vector in \b out \param normal Normal vector of the reflecting plane (may be of any length) \param v Vector to reflect \param out Out vector for the reflection result
	MFUNC3	(void)		_reflectVector2(const C0 normal[2], const C1 v[2], C2 out[2]);			//!< Reflects \b v about the given normal and stores the resulting vector in \b out \param normal Normal vector of the reflecting plane (may be of any length) \param v Vector to reflect \param out Out vector for the reflection result
	MFUNC3	(void)		_reflectVector4(const C0 normal[4], const C1 v[4], C2 out[4]);			//!< Reflects \b v about the given normal and stores the resulting vector in \b out \param normal Normal vector of the reflecting plane (may be of any length) \param v Vector  to reflect \param out Out vector for the reflection result
	MFUNC3	(void)		_reflectVectorV(const C0*normal, const C1*v, C2*out);						//!< Reflects \b v about the given normal and stores the resulting vector in \b out. The number of elements in each vector is specified by the last template parameter \param normal Normal vector of the reflecting plane (may be of any length) \param v Vector to reflect \param out Out vector for the reflection result
	MFUNC2	(void)		_reflectVector(const C0*normal, C1*v, count_t el_count);					//!< Reflects \b v about the given normal \param normal Normal vector of the reflecting plane (may be of any length) \param v (in/out) Vector to reflect \param el_count Number of elements in each vector
	MFUNC2	(void)		_reflectVector(const C0 normal[3], C1 v[3]);							//!< Reflects \b v about the given normal \param normal Normal vector of the reflecting plane (may be of any length) \param v (in/out) Vector to reflect
	MFUNC2	(void)		_reflectVector2(const C0 normal[2], C1 v[2]);						//!< Reflects \b v about the given normal \param normal Normal vector of the reflecting plane (may be of any length) \param v (in/out) Vector to reflect
	MFUNC2	(void)		_reflectVector4(const C0 normal[4], C1 v[4]);						//!< Reflects \b v about the given normal \param normal Normal vector of the reflecting plane (may be of any length) \param v (in/out) Vector to reflect
	MFUNC2	(void)		_reflectVectorV(const C0*normal, C1*v);										//!< Reflects \b v about the given normal. The number of elements in each vector is specified by the last template parameter \param normal Normal vector of the reflecting plane (may be of any length) \param v (in/out) Vector to reflect
	MFUNC3	(void)		_reflectVectorN(const C0*normal, const C1*v, C2*out, count_t el_count);	//!< Reflects \b v about the given normal and stores the resulting vector in \b out \param normal Normal vector of the reflecting plane (required normalized) \param v Vector to reflect \param out Out vector for the reflection result \param el_count Number of elements in each vector
	MFUNC3	(void)		_reflectVectorN(const C0 normal[3], const C1 v[3], C2 out[3]);			//!< Reflects \b v about the given normal and stores the resulting vector in \b out \param normal Normal vector of the reflecting plane (required normalized) \param v Vector to reflect \param out Out vector for the reflection result
	MFUNC3	(void)		_reflectVectorN2(const C0 normal[2], const C1 v[2], C2 out[2]);			//!< Reflects \b v about the given normal and stores the resulting vector in \b out \param normal Normal vector of the reflecting plane (required normalized) \param v Vector to reflect \param out Out vector for the reflection result
	MFUNC3	(void)		_reflectVectorN4(const C0 normal[4], const C1 v[4], C2 out[4]);			//!< Reflects \b v about the given normal and stores the resulting vector in \b out \param normal Normal vector of the reflecting plane (required normalized) \param v Vector to reflect \param out Out vector for the reflection result
	MFUNC3	(void)		_reflectVectorNV(const C0*normal, const C1*v, C2*out);						//!< Reflects \b v about the given normal and stores the resulting vector in \b out. The number of elements in each vector is specified by the last template parameter \param normal Normal vector of the reflecting plane (required normalized) \param v Vector to reflect \param out Out vector for the reflection result
	MFUNC2	(void)		_reflectVectorN(const C0*normal, C1*v, count_t el_count);					//!< Reflects \b v about the given normal \param normal Normal vector of the reflecting plane (required normalized) \param v (in/out) Vector to reflect \param el_count Number of elements in each vector
	MFUNC2	(void)		_reflectVectorN(const C0 normal[3], C1 v[3]);							//!< Reflects \b v about the given normal \param normal Normal vector of the reflecting plane (required normalized) \param v (in/out) Vector to reflect
	MFUNC2	(void)		_reflectVectorN2(const C0 normal[2], C1 v[2]);						//!< Reflects \b v about the given normal \param normal Normal vector of the reflecting plane (required normalized) \param v (in/out) Vector to reflect
	MFUNC2	(void)		_reflectVectorN4(const C0 normal[4], C1 v[4]);						//!< Reflects \b v about the given normal \param normal Normal vector of the reflecting plane (required normalized) \param v (in/out) Vector to reflect
	MFUNC2	(void)		_reflectVectorNV(const C0*normal, C1*v);								//!< Reflects \b v about the given normal. The number of elements in each vector is specified by the last template parameter \param normal Normal vector of the reflecting plane (required normalized) \param v (in/out) Vector to reflect
	MFUNC3	(void)		_resolve(const C0*v, const C1*w, C2*out, count_t el_count);					//!< Calculates \b out = \b v / \b w for all components 
	MFUNC3	(void)		_resolve(const C0 v[3], const C1 w[3], C2 out[3]);								//!< Calculates \b out = \b v / \b w for all components 
	MFUNC3V (void)		_resolveV(const C0*v, const C1*w, C2*out);																		//!< Calculates \b out = \b v / \b w for all components 
	MFUNC6	(void)		_resolveSpline(const C0*p0, const C1*p1, const C2*p2, const C3*p3, C4 t, C5*out, count_t el_count);		//!< Resolves a Spline point. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS position \param el_count Number of elements in each vector
	MFUNC6	(void)		_resolveSpline(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], C4 t, C5 out[3]);			//!< Resolves a Spline point. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS position
	MFUNC6	(void)		_resolveSpline2(const C0 p0[2], const C1 p1[2], const C2 p2[2], const C3 p3[2], C4 t, C5 out[2]);			//!< Resolves a Spline point. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS position
	MFUNC6	(void)		_resolveSpline4(const C0 p0[4], const C1 p1[4], const C2 p2[4], const C3 p3[4], C4 t, C5 out[4]);			//!< Resolves a Spline point. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS position
	MFUNC6V (void)		_resolveSplineV(const C0*p0, const C1*p1, const C2*p2, const C3*p3, C4 t, C5*out);							//!< Resolves a Spline point. The number of elements in each vector is specified by the last template parameter  \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS position
	MFUNC6	(void)		_resolveSplineAxis(const C0*p0, const C1*p1, const C2*p2, const C3*p3, C4 t, C5*out, count_t el_count);		//!< Resolves the Spline direction at a specific position. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS direction \param el_count Number of elements in each vector
	MFUNC6	(void)		_resolveSplineAxis(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], C4 t, C5 out[3]);		//!< Resolves the Spline direction at a specific position. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS direction
	MFUNC6	(void)		_resolveSplineAxis2(const C0 p0[2], const C1 p1[2], const C2 p2[2], const C3 p3[2], C4 t, C5 out[2]);		//!< Resolves the Spline direction at a specific position. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS direction
	MFUNC6	(void)		_resolveSplineAxis4(const C0 p0[4], const C1 p1[4], const C2 p2[4], const C3 p3[4], C4 t, C5 out[4]);		//!< Resolves the Spline direction at a specific position. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS direction
	MFUNC6V (void)		_resolveSplineAxisV(const C0*p0, const C1*p1, const C2*p2, const C3*p3, C4 t, C5*out);						//!< Resolves the Spline direction at a specific position. The number of elements in each vector is specified by the last template parameter   \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved NURBS direction

	MFUNC6	(void)		_resolveUCBS(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], C4 t, C5 out[3]);			//!< Resolves a Uniform cubic B-spline point. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved spline position
	MFUNC6	(void)		_resolveUCBSaxis(const C0 p0[3], const C1 p1[3], const C2 p2[3], const C3 p3[3], C4 t, C5 out[3]);		//!< Resolves a Uniform cubic B-spline axis. The result is not normalized. \param p0 Curve start point \param p1 First curve control point \param p2 Second curve control point \param p3 Curve end point \param t Position scalar along the curve (0-1) \param out Out vector for the resolved spline direction
	
	MFUNC3	(void)		_scale(const C0*center, C1 factor, C2*v, count_t el_count);												//!< Scales \b v relative to \b center by \b factor
	MFUNC3	(void)		_scale(const C0 center[3], C1 factor, C2 v[3]);															//!< Scales \b v relative to \b center by \b factor
	MFUNC3V (void)		_scaleV(const C0*center, C1 factor, C2*v);																//!< Scales \b v relative to \b center by \b factor
	MFUNC3	(void)		_scaleAbsolute(const C0*center, C1*v, C2 distance, count_t el_count);				//!< Scales \b v relative to \b center so that its distance from \b center matches \b distance
	MFUNC3	(void)		_scaleAbsolute(const C0 center[3], C1 v[3], C2 distance);						//!< Scales \b v relative to \b center so that its distance from \b center matches \b distance
	MFUNC3V (void)		_scaleAbsoluteV(const C0*center, C1*v, C2 distance);							//!< Scales \b v relative to \b center so that its distance from \b center matches \b distance
	MFUNC3	(void)		_scaleAbsolute0(const C0*center, C1*v, C2 distance, count_t el_count);		//!< Scales \b v relative to \b center so that its distance from \b center matches \b distance
	MFUNC3	(void)		_scaleAbsolute0(const C0 center[3], C1 v[3], C2 distance);					//!< Scales \b v relative to \b center so that its distance from \b center matches \b distance
	MFUNC3V (void)		_scaleAbsolute0V(const C0*center, C1*v, C2 distance);							//!< Scales \b v relative to \b center so that its distance from \b center matches \b distance
	MFUNC2	(void)		_set(C0*v, C1 value, count_t el_count);										//!< Sets all components of \b v to \b value
	MFUNC2	(void)		_set(C0 v[3], C1 value);														//!< Sets all components of \b v to \b value
	MFUNC2	(void)		_set2(C0 v[2], C1 value);									//!< Sets all components of \b v to \b value
	MFUNC2	(void)		_set4(C0 v[4], C1 value);									//!< Sets all components of \b v to \b value
	MFUNC2	(void)		_set6(C0 v[6], C1 value);									//!< Sets all components of \b v to \b value
	MFUNC2	(void)		_set9(C0 v[9], C1 value);									//!< Sets all components of \b v to \b value
	MFUNC2	(void)		_set16(C0 v[16], C1 value);								//!< Sets all components of \b v to \b value
	MFUNC2V (void)		_setV(C0*v, C1 value);									//!< Sets all components of \b v to \b value
	MFUNC2	(void)		_setLen(C0*v, C1 length, count_t el_count);				//!< Scales \b v to match the specified length. \b v must be of length greater 0
	MFUNC2	(void)		_setLen(C0 v[3], C1 length);								//!< Scales \b v to match the specified length. \b v must be of length greater 0
	MFUNC2	(void)		_setLen2(C0 v[2], C1 length);								//!< Scales \b v to match the specified length. \b v must be of length greater 0
	MFUNC2	(void)		_setLen4(C0 v[4], C1 length);								//!< Scales \b v to match the specified length. \b v must be of length greater 0
	MFUNC2V (void)		_setLenV(C0*v, C1 length);								//!< Scales \b v to match the specified length. \b v must be of length greater 0
	MFUNC2	(void)		_setLen0(C0*v, C1 length, count_t el_count);				//!< Scales/sets \b v to match the specified length. \b v may be of any length
	MFUNC2	(void)		_setLen0(C0 v[3], C1 length);								//!< Scales/sets \b v to match the specified length. \b v may be of any length
	MFUNC2	(void)		_setLen02(C0 v[2], C1 length);							//!< Scales/sets \b v to match the specified length. \b v may be of any length
	MFUNC2	(void)		_setLen04(C0 v[4], C1 length);							//!< Scales/sets \b v to match the specified length. \b v may be of any length
	MFUNC2V (void)		_setLen0V(C0*v, C1 length);								//!< Scales/sets \b v to match the specified length. \b v may be of any length
	MFUNC3	(bool)		_similar(const C0*v, const C1*w, C2 distance, count_t el_count); //!< Returns true if the distance between \b v and \b w is less than \b distance
	MFUNC3	(bool)		_similar(const C0*v, const C1*w, C2 distance);					//!< Returns true if the distance between \b v and \b w is less than \b distance
	MFUNC3	(bool)		_similar2(const C0*v, const C1*w, C2 distance);					//!< Returns true if the distance between \b v and \b w is less than \b distance
	MFUNC3	(bool)		_similar4(const C0*v, const C1*w, C2 distance);					//!< Returns true if the distance between \b v and \b w is less than \b distance
	MFUNC3V (bool)		_similarV(const C0*v, const C1*w, C2 distance);			//!< Returns true if the distance between \b v and \b w is less than \b distance
	MFUNC2	(bool)		_similar(const C0*v, const C1*w);								//!< Returns true if the distance between \b v and \b w is less or equal the respective type error constant
	MFUNC2	(bool)		_similar2(const C0*v, const C1*w);								//!< Returns true if the distance between \b v and \b w is less or equal the respective type error constant
	MFUNC2	(bool)		_similar4(const C0*v, const C1*w);								//!< Returns true if the distance between \b v and \b w is less or equal the respective type error constant
	MFUNC2V (bool)		_similarV(const C0*v, const C1*w);								//!< Returns true if the distance between \b v and \b w is less or equal the respective type error constant
	MFUNC	(void)		_rotateLeft(C*v, count_t el_count);							//!< Shifts values to the left (element 3 to element2, element 2 to element 1, etc). The left-most element will be moved to the right-most element
	MFUNC	(void)		_rotateLeft(C*v);												//!< Shifts values to the left (element 3 to element2, element 2 to element 1, etc). The left-most element will be moved to the right-most element
	MFUNC	(void)		_rotateLeft4(C*v);												//!< Shifts values to the left (element 3 to element2, element 2 to element 1, etc). The left-most element will be moved to the right-most element
	MFUNCV	(void)		_rotateLeftV(C*v);												//!< Shifts values to the left (element 3 to element2, element 2 to element 1, etc). The left-most element will be moved to the right-most element
	MFUNC	(void)		_rotateRight(C*v, count_t el_count);							//!< Shifts values to the right (element 0 to element1, element 1 to element 2, etc). The right-most element will be moved to the left-most element
	MFUNC	(void)		_rotateRight(C*v);												//!< Shifts values to the right (element 0 to element1, element 1 to element 2, etc). The right-most element will be moved to the left-most element
	MFUNC	(void)		_rotateRight4(C*v);												//!< Shifts values to the right (element 0 to element1, element 1 to element 2, etc). The right-most element will be moved to the left-most element
	MFUNCV	(void)		_rotateRightV(C*v);															//!< Shifts values to the right (element 0 to element1, element 1 to element 2, etc). The right-most element will be moved to the left-most element
	MFUNC4	(void)		_sphereCoords(C0 p_angle, C1 h_angle, C2 radius, C3 out[3]);//!< Generates 3-dimensional sphere-coordinates \param p_angle Planar angle. Angle around the polar axis (-360 - +360) \param h_angle Height angle (-90 - +90) \param radius Sphere radius \param out Out vector
	MFUNC3	(void)		_sphereCoordsRad(C0 p_angle, C1 h_angle, C2 out[3]);			//!< Generates 3-dimensional sphere-coordinates \param p_angle Planar angle. Angle around the polar axis (-2PI - +2PI) \param h_angle Height angle (-PI/2 - +PI/2) \param out Out vector
	MFUNC3	(void)		_stretch(const C0*v, const C1*w, C2*out, count_t el_count);				//!< Calculates the componentwise product of \b v and \b w and stores it in \b out
	MFUNC3	(void)		_stretch(const C0 v[3], const C1 w[3], C2 out[3]);						//!< Calculates the componentwise product of \b v and \b w and stores it in \b out
	MFUNC3	(void)		_stretch2(const C0 v[2], const C1 w[2], C2 out[2]);						//!< Calculates the componentwise product of \b v and \b w and stores it in \b out
	MFUNC3	(void)		_stretch4(const C0 v[4], const C1 w[4], C2 out[4]);						//!< Calculates the componentwise product of \b v and \b w and stores it in \b out
	MFUNC3V (void)		_stretchV(const C0*v, const C1*w, C2*out);						//!< Calculates the componentwise product of \b v and \b w and stores it in \b out
	MFUNC2	(void)		_stretch(C0*in_out, const C1*w, count_t el_count);				//!< Calculates the componentwise product of \b in_out and \b w and directly applies it to \b in_out (using *=)
	MFUNC2	(void)		_stretch(C0*in_out, const C1*w);								//!< Calculates the componentwise product of \b in_out and \b w and directly applies it to \b in_out (using *=)
	MFUNC2V (void)		_stretchV(C0*in_out, const C1*w);								//!< Calculates the componentwise product of \b in_out and \b w and directly applies it to \b in_out (using *=)
	#define				_sub					_subtract
	#define				_sub2					_subtract2
	#define				_sub4					_subtract4
	#define				_subV					_subtractV
	#define				_subVal					_subtractValue
	#define				_subVal2				_subtractValue2
	#define				_subVal4				_subtractValue4
	#define				_subValV				_subtractValueV
	MFUNC3	(void)		_subtract(const C0*v, const C1*w, C2*out,count_t el_count);	//!< Calculates the difference between \b v and \b w and stores the resulting vector in \b out ( \b out = \b v - \b w)
	MFUNC3	(void)		_subtract(const C0 v[3], const C1 w[3], C2 out[3]);				//!< Calculates the difference between \b v and \b w and stores the resulting vector in \b out ( \b out = \b v - \b w)
	MFUNC3	(void)		_subtract2(const C0 v[2], const C1 w[2], C2 out[2]);			//!< Calculates the difference between \b v and \b w and stores the resulting vector in \b out ( \b out = \b v - \b w)
	MFUNC3	(void)		_subtract4(const C0 v[4], const C1 w[4], C2 out[4]);			//!< Calculates the difference between \b v and \b w and stores the resulting vector in \b out ( \b out = \b v - \b w)
	MFUNC3V (void)		_subtractV(const C0*v, const C1*w, C2*out);						//!< Calculates the difference between \b v and \b w and stores the resulting vector in \b out ( \b out = \b v - \b w)
	MFUNC2	(void)		_subtract(C0*v, const C1*w,count_t el_count);				//!< Calculates the difference between \b v and \b w and stores the result in \b v( \b v -= \b w)
	MFUNC2	(void)		_subtract(C0 v[3], const C1 w[3]);							//!< Calculates the difference between \b v and \b w and stores the result in \b v( \b v -= \b w)
	MFUNC2	(void)		_subtract2(C0 v[2], const C1 w[2]);							//!< Calculates the difference between \b v and \b w and stores the result in \b v( \b v -= \b w)
	MFUNC2	(void)		_subtract4(C0 v[4], const C1 w[4]);							//!< Calculates the difference between \b v and \b w and stores the result in \b v( \b v -= \b w)
	MFUNC2V (void)		_subtractV(C0*v, const C1*w);									//!< Calculates the difference between \b v and \b w and stores the result in \b v( \b v -= \b w)
	MFUNC3	(void)		_subtractValue(const C0*v, C1 value, C2*out,count_t el_count);	//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b out ( \b out = \b v - \b value)
	MFUNC3	(void)		_subtractValue(const C0 v[3], C1 value, C2 out[3]);				//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b out ( \b out = \b v - \b value)
	MFUNC3	(void)		_subtractValue2(const C0 v[2], C1 value, C2 out[2]);					//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b out ( \b out = \b v - \b value)
	MFUNC3	(void)		_subtractValue4(const C0 v[4], C1 value, C2 out[4]);		//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b out ( \b out = \b v - \b value)
	MFUNC3V (void)		_subtractValueV(const C0*v, C1 value, C2*out);			//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b out ( \b out = \b v - \b value)
	MFUNC2	(void)		_subtractValue(C0*v, C1 value,count_t el_count);		//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b v ( \b v -= \b value)
	MFUNC2	(void)		_subtractValue(C0 v[3], C1 value);						//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b v ( \b v -= \b value)
	MFUNC2	(void)		_subtractValue2(C0 v[2], C1 value);						//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b v ( \b v -= \b value)
	MFUNC2	(void)		_subtractValue4(C0 v[4], C1 value);						//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b v ( \b v -= \b value)
	MFUNC2V	(void)		_subtractValueV(C0*v, C1 value);						//!< Calculates the component-wise difference between \b v and the fixed scalar \b value and stores the result in \b v ( \b v -= \b value)
	MFUNC	(C)			_sum(const C*v, count_t el_count);					//!< Calculates the sum of all components of \b v
	MFUNC	(C)			_sum(const C v[3]);									//!< Calculates the sum of all components of \b v
	MFUNCV	(C)			_sumV(const C*v);									//!< Calculates the sum of all components of \b v
	MFUNC3	(void)		_v2(C0 v[2], C1 x, C2 y);						//!< Sets two components of a vector
	MFUNC4	(void)		_v3(C0 v[3], C1 x, C2 y, C3 z);					//!< Sets three components of a vector
	MFUNC5	(void)		_v4(C0 v[4], C1 x, C2 y, C3 z, C4 a);			//!< Sets four components of a vector
	MFUNC2	(void)		_vectorSort(C0*v, C1*w, count_t el_count);					//!< Swaps components of \b v and \b w so that each component of \b v is less than (or equal) the respective component of \b w
	MFUNC2	(void)		_vectorSort(C0 v[3], C1 w[3]);							//!< Swaps components of \b v and \b w so that each component of \b v is less than (or equal) the respective component of \b w
	MFUNC2V (void)		_vectorSortV(C0*v, C1*w);							//!< Swaps components of \b v and \b w so that each component of \b v is less than (or equal) the respective component of \b w
	MFUNC	(bool)		_zero(const C*v, count_t el_count);				//!< Returns true if |\b v | equals 0 (exactly)
	MFUNC	(bool)		_zero(const C v[3]);								//!< Returns true if |\b v | equals 0 (exactly)
	MFUNC	(bool)		_zero2(const C v[2]);								//!< Returns true if |\b v | equals 0 (exactly)
	MFUNC	(bool)		_zero4(const C v[4]);								//!< Returns true if |\b v | equals 0 (exactly)
	MFUNCV	(bool)		_zeroV(const C*v);								//!< Returns true if |\b v | equals 0 (exactly)

	deprecated. very much so
}
#endif

	namespace Vec
	{
	
		template <typename T>
			MF_DECLARE (TVec4<T>&) 	ref4(T*vec)
			{
				return *reinterpret_cast<TVec4<T>*>(vec);
			}


		template <typename T>
			MF_DECLARE (const TVec4<T>&) 	ref4(const T*vec)
			{
				return *reinterpret_cast<const TVec4<T>*>(vec);
			}

		template <typename T>
			MF_DECLARE (TVec3<T>&) 	ref3(T*vec)
			{
				return *reinterpret_cast<TVec3<T>*>(vec);
			}


		template <typename T>
			MF_DECLARE (const TVec3<T>&) 	ref3(const T*vec)
			{
				return *reinterpret_cast<const TVec3<T>*>(vec);
			}

		template <typename T>
			MF_DECLARE (TVec2<T>&) 	ref2(T*vec)
			{
				return *reinterpret_cast<TVec2<T>*>(vec);
			}


		template <typename T>
			MF_DECLARE (const TVec2<T>&) 	ref2(const T*vec)
			{
				return *reinterpret_cast<const TVec2<T>*>(vec);
			}

		template <typename T>
			MF_DECLARE (TVec1<T>&) 	ref1(T*vec)
			{
				return *reinterpret_cast<TVec1<T>*>(vec);
			}


		template <typename T>
			MF_DECLARE (const TVec1<T>&) 	ref1(const T*vec)
			{
				return *reinterpret_cast<const TVec1<T>*>(vec);
			}



		template <typename T0, typename T1, typename T2>
			MF_DECLARE(void)	def(TVec2<T0>&v, T1 x, T2 y)
			{
				v.x = (T0)x;
				v.y = (T0)y;
			}
		template <typename T0, typename T1, typename T2, typename T3>
			MF_DECLARE(void)	def(TVec3<T0>&v, T1 x, T2 y, T3 z)
			{
				v.x = (T0)x;
				v.y = (T0)y;
				v.z = (T0)z;
			}
		template <typename T0, typename T1, typename T2, typename T3, typename T4>
			MF_DECLARE(void)	def(TVec4<T0>&v, T1 x, T2 y, T3 z, T4 w)
			{
				v.x = (T0)x;
				v.y = (T0)y;
				v.z = (T0)z;
				v.w = (T0)w;
			}

		template <typename T0, typename T1, typename T2, typename T3>
			MF_DECLARE(void)	def3(T0*v, T1 x, T2 y, T3 z)
			{
				v[0] = (T0)x;
				v[1] = (T0)y;
				v[2] = (T0)z;
			}
			
		template <typename T>
			inline void	assertConsistency()
			{
				TVec1<T>	t1;
				TVec2<T>	t2;
				TVec3<T>	t3;
				TVec4<T>	t4;

				//ASSERT_EQUAL__((const void*)t0.v,(const void*)&t0);
				ASSERT_EQUAL__((const void*)t1.v,(const void*)&t1);
				ASSERT_EQUAL__((const void*)t2.v,(const void*)&t2);
				ASSERT_EQUAL__((const void*)t3.v,(const void*)&t3);
				ASSERT_EQUAL__((const void*)t4.v,(const void*)&t4);


				ASSERT_EQUAL__((const void*)(((const T*)&t2.y)-1),(const void*)&t2);
				ASSERT_EQUAL__((const void*)(((const T*)&t3.y)-1),(const void*)&t3);
				ASSERT_EQUAL__((const void*)(((const T*)&t4.y)-1),(const void*)&t4);

				ASSERT_EQUAL__((const void*)(((const T*)&t3.z)-2),(const void*)&t3);
				ASSERT_EQUAL__((const void*)(((const T*)&t4.z)-2),(const void*)&t4);

				ASSERT_EQUAL__((const void*)(((const T*)&t4.w)-3),(const void*)&t4);

				ASSERT_EQUAL__(&t1.x,&t1.alpha);

				ASSERT_EQUAL__(&t2.y,&t2.alpha);
				ASSERT_EQUAL__(&t2.x,&t2.luminance);

				ASSERT_EQUAL__(t2.v,t2.la);
				//ASSERT_EQUAL__(t3.v,t3.rgb);
				ASSERT_EQUAL__((const void*)t4.v,(const void*)&t4.rgb);
				ASSERT_EQUAL__((const void*)(t4.v+2),(const void*)&t4.zw);
				ASSERT_EQUAL__(t4.v,t4.rgba);

				ASSERT_EQUAL__(&t3.z,&t3.blue);
				ASSERT_EQUAL__(&t3.y,&t3.green);
				ASSERT_EQUAL__(&t3.x,&t3.red);

				ASSERT_EQUAL__(&t4.w,&t4.alpha);
				ASSERT_EQUAL__(&t4.z,&t4.blue);
				ASSERT_EQUAL__(&t4.y,&t4.green);
				ASSERT_EQUAL__(&t4.x,&t4.red);

				ASSERT_EQUAL__((const T*)&ref4(t4.v),t4.v);
				ASSERT_EQUAL__((const T*)&ref3(t3.v),t3.v);
				ASSERT_EQUAL__((const T*)&ref2(t2.v),t2.v);
				ASSERT_EQUAL__((const T*)&ref1(t1.v),t1.v);
			};

	}



	#include "vector_operations.h"

}



//#include "vector.tpl.h"
#endif
