#ifndef vclassesH
#define vclassesH





#include "vector.h"

namespace Math
{

	/** vector-classes **
	
	provided 2,3,4 and N-dimensional vectors work a very simple level providing the most common operators.
	if efficiency is of essence it's recommend to use the more simplistic base vector-functions described
	in "basic.h"
	
	example on vectors:

		Vec3<float>	u,v(2,3.7,-4),w(-1,0,0);
		u = v+w;
		u = v-w;
		v+=w;
		float dot_product = (v-w)*u,
				component = u[1], // equal to u.y or u.v[1]
				length = u.length();

	Line/Line is an experimental combination of a point and a vector forming a straight line in 3-dimensional space

	*/
	
	

	template <class, count_t> class VecN;	//static n-dimensional vector
	template <class> class Vec2;				//2-dimensional vector
	template <class> class Vec3;				//3-dimensional vector
	template <class> class Vec4;				//4-dimensional vector
	template <class> class Line;				//3-dimensional straight line
	
	

	#define	MF_VNT					template <class C0, count_t Len0>		//!< MF VecN Template definition
	#define MFUNCV1_(_return_type_) MF_VNT MF_DECLARE(_return_type_)




	template <class C=float> class Vec3:public TVec<C,3>
	{
	public:
		typedef Vec3<C>					Self;

		MF_CONSTRUCTOR					Vec3();
		explicit MF_CONSTRUCTOR			Vec3(const C&value);
		MF_CONSTRUCTOR2					Vec3(const TVec2<C0>&other,const C1&z);
		MF_CONSTRUCTOR1					Vec3(const TVec3<C0>&other);
		MF_CONSTRUCTOR3					Vec3(const C0&x, const C1&y, const C2&z);
		MF_CONSTRUCTOR1					Vec3(const C0 field[3]);

		static MF_DECLARE(Self&)		reinterpret(TVec3<C>&vec)	{return reinterpret_cast<Self&>(vec);}
		static MF_DECLARE(Self&)		Reinterpret(TVec3<C>&vec)	{return reinterpret_cast<Self&>(vec);}
		static MF_DECLARE(const Self&)	reinterpret(const TVec3<C>&vec)	{return reinterpret_cast<const Self&>(vec);}
		static MF_DECLARE(const Self&)	Reinterpret(const TVec3<C>&vec)	{return reinterpret_cast<const Self&>(vec);}

		MFUNC3 (void)					set(const C0&x, const C1&y, const C2&z)	{Vec::def(*this,x,y,z);}
		MFUNC3 (void)					Set(const C0&x, const C1&y, const C2&z)	{Vec::def(*this,x,y,z);}
		MFUNC1 (void)					set(const C0&v)							{Vec::set(*this,v);}
		MFUNC1 (void)					Set(const C0&v)							{Vec::set(*this,v);}
		MF_DECLARE(void)				clear()									{Vec::clear(*this);}
		MF_DECLARE(void)				Clear()									{Vec::clear(*this);}
		MF_DECLARE(String)				ToString()								const	{return Vec::toString(*this);}
		MF_DECLARE(C)					length()								const	{return Vec::length(*this);}
		MF_DECLARE(C)					Length()								const	{return Vec::length(*this);}
		MF_DECLARE(C)					quadraticLength()						const	{return Vec::dot(*this);}
		MF_DECLARE(C)					QuadraticLength()						const	{return Vec::dot(*this);}
		MF_DECLARE(C)					sqr()							 		const	{return Vec::dot(*this);}
		MF_DECLARE(C)					Sqr()							 		const	{return Vec::dot(*this);}
		MF_DECLARE(C)					summary()								const	{return x+y+z;}
		MF_DECLARE(C)					Summary()								const	{return x+y+z;}
		MF_DECLARE(Vec3<C>)				normalized()							const;		//returns normalized vector
		MF_DECLARE(Vec3<C>)				normalized0()							const;
		MF_DECLARE(Self&)				normalize()								{Vec::normalize(*this);return *this;}
		MF_DECLARE(Self&)				Normalize()								{Vec::normalize(*this);return *this;}
		MF_DECLARE(Self&)				normalize0()							{Vec::normalize0(*this);return *this;}
		MF_DECLARE(Self&)				Normalize0()							{Vec::normalize0(*this);return *this;}
		MFUNC1 (Vec3<C>)				operator+(const TVec3<C0>&other)		const;
		MF_DECLARE(Vec3<C>)				operator+(const C&value)				const;
		MFUNC1 (void)					operator+=(const TVec3<C0>&other);
		MF_DECLARE(void)				operator+=(const C&value);
		MFUNC1 (C)						operator*(const TVec3<C0>&other)		const;
		MF_DECLARE(Vec3<C>)				operator*(const C&factor)				const;
		MF_DECLARE(void)				operator*=(const C&factor);
		MFUNC1 (Vec3<C>)				operator/(const C0&factor)				const;
		MFUNC1 (void)					operator/=(const C0&factor);
		MFUNC1 (Vec3<C>)				operator-(const TVec3<C0>&other)		const;
		MF_DECLARE(Vec3<C>)				operator-(const C&value)				const;
		MF_DECLARE(Vec3<C>)				operator-()							 const;
		MFUNC1 (void)					operator-=(const TVec3<C0>&other);
		MF_DECLARE(void)				operator-=(const C&value);
		MFUNC1 (Vec3<C>)				operator|(const TVec3<C0>&other)		const;
		MFUNC1 (Vec3<C>)				operator&(const TVec3<C0>&other)		const;
		MFUNC1 (void)					operator=(const TVec3<C0>&other);
		MFUNC1 (bool)					operator==(const TVec3<C0>&other)		const;
		MFUNC1 (bool)					operator!=(const TVec3<C0>&other)		const;
		MFUNC1 (bool)					operator>(const TVec3<C0>&other)		const;		//!< Lexicographic order using _compare
		MFUNC1 (bool)					operator<(const TVec3<C0>&other)		const;		//!< Lexicographic order using _compare
		MFUNC1 (int)					compareTo(const TVec3<C0>&other)		const	{return Vec::compare(*this,other);}
		MFUNC1 (int)					CompareTo(const TVec3<C0>&other)		const	{return Vec::compare(*this,other);}
		MF_DECLARE(C&)					operator[](index_t component);						//return component of the vector
		MF_DECLARE(const C&)			operator[](index_t component)			const;

		MFUNC1 (C)						operator()(const C0&x)					const;		//!< Evaluates the local vector as a polynomial ax² + bx + c

	};


	template <class C=float> class Vec2:public TVec<C,2>
	{
	public:
		typedef Vec2<C>					Self;

		MF_CONSTRUCTOR					Vec2();
		explicit MF_CONSTRUCTOR			Vec2(const C&value);
		MF_CONSTRUCTOR1					Vec2(const TVec2<C0>&other);
		MF_CONSTRUCTOR2					Vec2(const C0&x, const C1&y);
		MF_CONSTRUCTOR1					Vec2(const C0 field[2]);

		static MF_DECLARE(Self&)		reinterpret(TVec2<C>&vec)	{return reinterpret_cast<Self&>(vec);}
		static MF_DECLARE(Self&)		Reinterpret(TVec2<C>&vec)	{return reinterpret_cast<Self&>(vec);}
		static MF_DECLARE(const Self&)	reinterpret(const TVec2<C>&vec)	{return reinterpret_cast<const Self&>(vec);}
		static MF_DECLARE(const Self&)	Reinterpret(const TVec2<C>&vec)	{return reinterpret_cast<const Self&>(vec);}

		MFUNC2 (void)					set(const C0&x, const C1&y)				{Vec::def(*this,x,y);}
		MFUNC2 (void)					Set(const C0&x, const C1&y)				{Vec::def(*this,x,y);}
		MF_DECLARE(void)				clear()									{Vec::clear(*this);}
		MF_DECLARE(void)				Clear()									{Vec::clear(*this);}
		MF_DECLARE(String)				ToString()								const	{return Vec::toString(*this);}
		MF_DECLARE(C)					length()								const	{return Vec::length(*this);}
		MF_DECLARE(C)					Length()								const	{return Vec::length(*this);}
		MF_DECLARE(C)					sqr()							 		const	{return Vec::dot(*this);}
		MF_DECLARE(C)					Sqr()							 		const	{return Vec::dot(*this);}
		MF_DECLARE(C)					summary()								const	{return x+y;}
		MF_DECLARE(C)					Summary()								const	{return x+y;}
		MF_DECLARE(Vec2<C>)				normalized()							const;		//returns normalized vector
		MF_DECLARE(Vec2<C>)				normalized0()							const;
		MF_DECLARE(Self&)				normalize()								{Vec::normalize(*this);return *this;}
		MF_DECLARE(Self&)				Normalize()								{Vec::normalize(*this);return *this;}
		MF_DECLARE(Self&)				normalize0()							{Vec::normalize0(*this);return *this;}
		MF_DECLARE(Self&)				Normalize0()							{Vec::normalize0(*this);return *this;}
		MF_DECLARE(void)				flip()									{swp(x,y);}
		MF_DECLARE(void)				Flip()									{swp(x,y);}
		MF_DECLARE(Vec2<C>)				flipped()								const	{return Vec2<C>(y,x);}
		MF_DECLARE(Vec2<C>)				Flipped()								const	{return Vec2<C>(y,x);}
		MF_DECLARE(Vec2<C>)				normal()								const	{return Vec2<C>(-y,x);}	//!< Returns a vector normal to this one
		MF_DECLARE(Vec2<C>)				Normal()								const	{return Vec2<C>(-y,x);}	//!< Returns a vector normal to this one
		MFUNC1 (Vec2<C>)				operator+(const TVec2<C0>&other)		const;
		MF_DECLARE(Vec2<C>)				operator+(const C&value)				const;
		MFUNC1 (void)					operator+=(const TVec2<C0>&other);
		MF_DECLARE(void)				operator+=(const C&value);
		MFUNC1 (C)						operator*(const TVec2<C0>&other)		const;
		MF_DECLARE(Vec2<C>)				operator*(const C&factor)				const;
		MF_DECLARE(void)				operator*=(const C&factor);
		MFUNC1 (Vec2<C>)				operator/(const C0&factor)				const;
		MFUNC1 (void)					operator/=(const C0&factor);
		MFUNC1 (Vec2<C>)				operator-(const TVec2<C0>&other)		const;
		MF_DECLARE(Vec2<C>)				operator-(const C&value)				const;
		MF_DECLARE(Vec2<C>)				operator-()							 const;
		MFUNC1 (void)					operator-=(const TVec2<C0>&other);
		MF_DECLARE(void)				operator-=(const C&value);
		MFUNC1 (Vec2<C>)				operator&(const TVec2<C0>&other)		const;
		MFUNC1 (void)					operator=(const TVec2<C0>&other);
		MFUNC1 (bool)					operator==(const TVec2<C0>&other)		const;
		MFUNC1 (bool)					operator!=(const TVec2<C0>&other)		const;
		MFUNC1 (bool)					operator>(const TVec2<C0>&other)		const;		//!< Lexicographic order using _compare
		MFUNC1 (bool)					operator<(const TVec2<C0>&other)		const;		//!< Lexicographic order using _compare
		MFUNC1 (int)					compareTo(const TVec2<C0>&other)		const	{return Vec::compare(*this,other);}
		MFUNC1 (int)					CompareTo(const TVec2<C0>&other)		const	{return Vec::compare(*this,other);}
		MF_DECLARE(C&)					operator[](index_t component);						//return component of the vector
		MF_DECLARE(const C&)			operator[](index_t component)			const;
		MFUNC1 (C)						operator()(const C0&x)					const;		//!< Evaluates the local vector as a polynomial ax + b

	};


	template <class C=float> class Vec4:public TVec<C,4>
	{
	public:
		typedef Vec4<C>					Self;

		MF_CONSTRUCTOR					Vec4();
		explicit MF_CONSTRUCTOR			Vec4(const C&value);
		MF_CONSTRUCTOR3					Vec4(const TVec2<C0>&xy,const C1&z, const C2&w);
		MF_CONSTRUCTOR2					Vec4(const TVec2<C0>&xy,const TVec2<C1>&zw);
		MF_CONSTRUCTOR2					Vec4(const TVec3<C0>&other, const C1&w);
		MF_CONSTRUCTOR1					Vec4(const TVec4<C0>&other);
		MF_CONSTRUCTOR4					Vec4(const C0&x, const C1&y, const C2&z, const C3&a);
		MF_CONSTRUCTOR1					Vec4(const C0 field[4]);

		MFUNC4 (void)					set(const C0&x, const C1&y, const C2&z, const C3&w)	{Vec::def(*this,x,y,z,w);}
		MFUNC4 (void)					Set(const C0&x, const C1&y, const C2&z, const C3&w)	{Vec::def(*this,x,y,z,w);}
		MF_DECLARE(void)				clear()									{Vec::clear(*this);}
		MF_DECLARE(void)				Clear()									{Vec::clear(*this);}
		MF_DECLARE(String)				ToString()								const	{return Vec::toString(*this);}
		MF_DECLARE(C)					length()								const	{return Vec::length(*this);}
		MF_DECLARE(C)					Length()								const	{return Vec::length(*this);}
		MF_DECLARE(C)					sqr()							 		const	{return Vec::dot(*this);}
		MF_DECLARE(C)					Sqr()							 		const	{return Vec::dot(*this);}
		MF_DECLARE(C)					summary()								const	{return x+y+z+w;}
		MF_DECLARE(C)					Summary()								const	{return x+y+z+w;}
		MF_DECLARE(Vec4<C>)				normalized()							const;		//returns normalized vector
		MF_DECLARE(Self&)				normalize()								{Vec::normalize(*this);return *this;}
		MF_DECLARE(Self&)				Normalize()								{Vec::normalize(*this);return *this;}
		MF_DECLARE(Self&)				normalize0()							{Vec::normalize0(*this);return *this;}
		MF_DECLARE(Self&)				Normalize0()							{Vec::normalize0(*this);return *this;}

		MF_DECLARE(Vec3<C>)				vector(BYTE c0, BYTE c1, BYTE c2)		const;
		MF_DECLARE(Vec2<C>)				vector(BYTE c0, BYTE c1)				const;
		MFUNC1 (Vec4<C>)				operator+(const TVec4<C0>&other)		const;
		MF_DECLARE(Vec4<C>)				operator+(const C&value)				const;
		MFUNC1 (void)					operator+=(const TVec4<C0>&other);
		MF_DECLARE(void)				operator+=(const C&value);
		MFUNC1 (C)						operator*(const TVec4<C0>&other)		const;
		MF_DECLARE(Vec4<C>)				operator*(const C&factor)				const;
		MF_DECLARE(void)				operator*=(const C&factor);
		MFUNC1 (Vec4<C>)				operator/(const C0&factor)				const;
		MFUNC1 (void)					operator/=(const C0&factor);
		MFUNC1 (Vec4<C>)				operator-(const TVec4<C0>&other)		const;
		MF_DECLARE(Vec4<C>)				operator-()							 const;
		MF_DECLARE(Vec4<C>)				operator-(const C&value)				const;
		MFUNC1 (void)					operator-=(const TVec4<C0>&other);
		MF_DECLARE(void)				operator-=(const C&value);
		MFUNC1 (Vec3<C>)				operator|(const TVec4<C0>&other)		const;
		MFUNC1 (void)					operator=(const TVec2<C0>&other);
		MFUNC1 (void)					operator=(const TVec3<C0>&other);
		MFUNC1 (void)					operator=(const TVec4<C0>&other);
		MFUNC1 (bool)					operator==(const TVec4<C0>&other)		const;
		MFUNC1 (bool)					operator!=(const TVec4<C0>&other)		const;
		MFUNC1 (bool)					operator>(const TVec4<C0>&other)		const;		//!< Lexicographic order using _compare
		MFUNC1 (bool)					operator<(const TVec4<C0>&other)		const;		//!< Lexicographic order using _compare
		MFUNC1 (int)					compareTo(const TVec4<C0>&other)		const	{return Vec::compare(*this,other);}
		MFUNC1 (int)					CompareTo(const TVec4<C0>&other)		const	{return Vec::compare(*this,other);}
		MF_DECLARE(C&)					operator[](index_t component);						//return component of the vector
		MF_DECLARE(const C&)			operator[](index_t component)			const;
	};




	template <class C=float> class Line
	{
	public:
		Vec3<C>			position,direction;

		MF_CONSTRUCTOR				Line();
		MF_CONSTRUCTOR				Line(const Line<C>&);
		MF_CONSTRUCTOR1				Line(const Line<C0>&);
		MF_CONSTRUCTOR2				Line(const TVec3<C0>&, const TVec3<C1>&);
		MF_DECLARE(String)			ToString();
		MFUNC1 (Vec3<C>)			resolvePoint(const C0&);
		MFUNC1 (void)				operator+=(const Line<C0>&);
		MFUNC1 (void)				operator-=(const Line<C0>&);
		MFUNC1 (void)				operator*=(const C0&);
		MFUNC1 (void)				operator/=(const C0&);
		MFUNC1 (void)				operator=(const Line<C0>&);
		MFUNC1 (Line<C>)			operator+(const Line<C0>&)			 const;
		MFUNC1 (Line<C>)			operator-(const Line<C0>&)			 const;
		MFUNC1 (Line<C>)			operator*(const C0&)					const;
		MFUNC1 (Line<C>)			operator/(const C0&)					const;
		MF_DECLARE(Line<C>)		operator-()							 const;
	};





	template <class C, count_t Len> class VecN:public TVec<C,Len>
	{
	public:

			MF_CONSTRUCTOR1					VecN(const TVec2<C> &v)
											{
												_copyV<C,C,Len<2?Len:2>(other.v,v);
											}
			MF_CONSTRUCTOR1					VecN(const TVec3<C> &v)
											{
												_copyV<C,C,Len<3?Len:3>(other.v,v);
											}

			MF_CONSTRUCTOR1					VecN(const TVec4<C> &v)
											{
												_copyV<C,C,Len<4?Len:4>(other.v,v);
											}
			MF_CONSTRUCTOR1					VecN(const TVec<C,Len> &v)
											{
												_copyV<C,C,Len>(other.v,v);
											}

			MF_CONSTRUCTOR1					VecN()									{}
			MF_CONSTRUCTOR1					VecN(C x, C y)							{v[0] = x; v[1] = y;};
			MF_CONSTRUCTOR1					VecN(C x, C y, C z)					{v[0] = x; v[1] = y; v[2] = z;};
			MF_CONSTRUCTOR1					VecN(C x, C y, C z, C a)				{v[0] = x; v[1] = y; v[2] = z; v[3] = a;};
			MF_CONSTRUCTOR1					VecN(const C0 field[Len])				{_copyV<C0,C,Len>(field,v);};
			MF_DECLARE(void)				clear();											//sets all values to 0
			MF_DECLARE(String)				ToString()								const;		//returns string of vector-content
			MF_DECLARE(C)					length()								const;		//calculates norm of the vector
			MF_DECLARE(C)					sqr()							 		const;		//calculates square of the vector
			MF_DECLARE(C)					summary()								const;		//calculates sum of all vector-components
			MF_INIT VecN<C,Len> MF_CC		normalized()							const;		//returns normalized vector
	MF_VNT	MF_INIT VecN<C, Eval<Len,Len0>::Min > MF_CC
											operator+(const TVec<C0,Len0>&v)		const;
			MF_INIT VecN<C,Len> MF_CC		operator+(const C&scalar)				const;
			MFUNCV1_(void)					operator+=(const TVec<C0,Len0>&v);
			MF_DECLARE(void)				operator+=(const C&scalar);
			MFUNCV1_(C)						operator*(const TVec<C0,Len0>&v)		const;
			MF_INIT VecN<C,Len> MF_CC		operator*(const C&factor)				const;
			MFUNC1 (void)					operator*=(const C0&factor);
			template <class C0>
			MF_INIT VecN<C,Len> MF_CC		operator/(const C0&factor)				const;
			MFUNC1 (void)					operator/=(const C0&factor);
	MF_VNT	MF_INIT VecN<C,Eval<Len,Len0>::Min > MF_CC
											operator-(const TVec<C0,Len0>&v)		const;
			MF_INIT VecN<C,Len> MF_CC		operator-(const C&scalar)				const;
			MF_INIT VecN<C,Len> MF_CC		operator-()							 const;
			MFUNCV1_(void)					operator-=(const TVec<C0,Len0>&v);
			MF_DECLARE(void)				operator-=(const C&scalar);
	MF_VNT	MF_INIT Vec3<C> MF_CC			operator|(const TVec<C0,Len0>&v)		const;
	MF_VNT	MF_INIT Vec3<C> MF_CC			operator&(const TVec<C0,Len0>&v)		const;
			MFUNCV1_(void)					operator=(const TVec<C0,Len0>&v);
			MFUNC1 (void)					operator=(const TVec2<C0>&v);
			MFUNC1 (void)					operator=(const TVec3<C0>&v);
			MFUNC1 (void)					operator=(const TVec4<C0>&v);
			MFUNCV1_(bool)					operator==(const TVec<C0,Len0>&v)		const;
			MFUNCV1_(bool)					operator!=(const TVec<C0,Len0>&v)		const;
			MFUNC1 (bool)					operator>(const TVec<C0,Len>&other)		const;		//!< Lexicographic order using _compare
			MFUNC1 (bool)					operator<(const TVec<C0,Len>&other)		const;		//!< Lexicographic order using _compare
			MFUNC1 (char)					compareTo(const TVec<C0,Len>&other)		const;		//!< Lexicographic order using _compare
			MF_DECLARE(C&)					operator[](index_t component);						//return component of the vector
			MF_DECLARE(const C&)			operator[](index_t component)			const;
	};





	typedef Vec2<float>	float2;
	typedef Vec3<float>	float3;
	typedef Vec4<float>	float4;
	typedef Vec2<double>	double2;
	typedef Vec3<double>	double3;
	typedef Vec4<double>	double4;
	typedef Vec2<int>		int2;
	typedef Vec3<int>		int3;
	typedef Vec4<int>		int4;
	typedef Vec2<short>		short2;
	typedef Vec3<short>		short3;
	typedef Vec4<short>		short4;
	typedef Vec2<bool>		bool2;
	typedef Vec3<bool>		bool3;
	typedef Vec4<bool>		bool4;




	template <typename T>
		inline Vec2<T>	operator-(const TVec2<T>&a, const TVec2<T>&b)
		{
			return Vec2<T>(a.x - b.x, a.y - b.y);
		}
	template <typename T>
		inline Vec3<T>	operator-(const TVec3<T>&a, const TVec3<T>&b)
		{
			return Vec3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
		}
	template <typename T>
		inline Vec4<T>	operator-(const TVec4<T>&a, const TVec4<T>&b)
		{
			return Vec4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}
	template <typename T>
		inline Vec2<T>	operator+(const TVec2<T>&a, const TVec2<T>&b)
		{
			return Vec2<T>(a.x + b.x, a.y + b.y);
		}
	template <typename T>
		inline Vec3<T>	operator+(const TVec3<T>&a, const TVec3<T>&b)
		{
			return Vec3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
		}
	template <typename T>
		inline Vec4<T>	operator+(const TVec4<T>&a, const TVec4<T>&b)
		{
			return Vec4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}


	template <typename T>
		inline Vec2<T>	operator*(const TVec2<T>&a, const T&f)
		{
			return Vec2<T>(a.x *f, a.y *f);
		}
	template <typename T>
		inline Vec3<T>	operator*(const TVec3<T>&a, const T&f)
		{
			return Vec3<T>(a.x * f, a.y * f, a.z * f);
		}
	template <typename T>
		inline Vec4<T>	operator*(const TVec4<T>&a, const T&f)
		{
			return Vec4<T>(a.x * f, a.y * f, a.z * f, a.w * f);
		}

	template <typename T>
		inline Vec2<T>	operator*(const T&f, const TVec2<T>&a)
		{
			return Vec2<T>(a.x *f, a.y *f);
		}
	template <typename T>
		inline Vec3<T>	operator*(const T&f, const TVec3<T>&a)
		{
			return Vec3<T>(a.x * f, a.y * f, a.z * f);
		}
	template <typename T>
		inline Vec4<T>	operator*(const T&f, const TVec4<T>&a)
		{
			return Vec4<T>(a.x * f, a.y * f, a.z * f, a.w * f);
		}


	template <typename T>
		inline Vec2<T>	operator/(const TVec2<T>&a, const T&f)
		{
			return Vec2<T>(a.x /f, a.y /f);
		}
	template <typename T>
		inline Vec3<T>	operator/(const TVec3<T>&a, const T&f)
		{
			return Vec3<T>(a.x / f, a.y / f, a.z / f);
		}
	template <typename T>
		inline Vec4<T>	operator/(const TVec4<T>&a, const T&f)
		{
			return Vec4<T>(a.x / f, a.y / f, a.z / f, a.w / f);
		}

	template <typename T>
		inline Vec2<T> operator-(const TVec2<T>&v)
		{
			return Vec2<T>(-v.x,-v.y);
		}
	template <typename T>
		inline Vec3<T> operator-(const TVec3<T>&v)
		{
			return Vec3<T>(-v.x,-v.y,-v.z);
		}
	template <typename T>
		inline Vec4<T> operator-(const TVec4<T>&v)
		{
			return Vec4<T>(-v.x,-v.y,-v.z,-v.w);
		}


	template <typename T>
		inline TVec2<T>&	operator-=(TVec2<T>&a, const TVec2<T>&b)
		{
			a.x -= b.x;
			a.y -= b.y;
			return a;
		}

	template <typename T>
		inline TVec3<T>&	operator-=(TVec3<T>&a, const TVec3<T>&b)
		{
			a.x -= b.x;
			a.y -= b.y;
			a.z -= b.z;
			return a;
		}

	template <typename T>
		inline TVec4<T>&	operator-=(TVec4<T>&a, const TVec4<T>&b)
		{
			a.x -= b.x;
			a.y -= b.y;
			a.z -= b.z;
			a.w -= b.w;
			return a;
		}

	template <typename T>
		inline TVec2<T>&	operator+=(TVec2<T>&a, const TVec2<T>&b)
		{
			a.x += b.x;
			a.y += b.y;
			return a;
		}

	template <typename T>
		inline TVec3<T>&	operator+=(TVec3<T>&a, const TVec3<T>&b)
		{
			a.x += b.x;
			a.y += b.y;
			a.z += b.z;
			return a;
		}

	template <typename T>
		inline TVec4<T>&	operator+=(TVec4<T>&a, const TVec4<T>&b)
		{
			a.x += b.x;
			a.y += b.y;
			a.z += b.z;
			a.w += b.w;
			return a;
		}






	template <typename T>
		inline TVec2<T>&	operator-=(TVec2<T>&a, const T&b)
		{
			a.x -= b;
			a.y -= b;
			return a;
		}

	template <typename T>
		inline TVec3<T>&	operator-=(TVec3<T>&a, const T&b)
		{
			a.x -= b;
			a.y -= b;
			a.z -= b;
			return a;
		}

	template <typename T>
		inline TVec4<T>&	operator-=(TVec4<T>&a, const T&b)
		{
			a.x -= b;
			a.y -= b;
			a.z -= b;
			a.w -= b;
			return a;
		}

	template <typename T>
		inline TVec2<T>&	operator+=(TVec2<T>&a, const T&b)
		{
			a.x += b;
			a.y += b;
			return a;
		}

	template <typename T>
		inline TVec3<T>&	operator+=(TVec3<T>&a, const T&b)
		{
			a.x += b;
			a.y += b;
			a.z += b;
			return a;
		}

	template <typename T>
		inline TVec4<T>&	operator+=(TVec4<T>&a, const T&b)
		{
			a.x += b;
			a.y += b;
			a.z += b;
			a.w += b;
			return a;
		}




	template <typename T>
		inline TVec2<T>&	operator*=(TVec2<T>&a, const T&b)
		{
			a.x *= b;
			a.y *= b;
			return a;
		}

	template <typename T>
		inline TVec3<T>&	operator*=(TVec3<T>&a, const T&b)
		{
			a.x *= b;
			a.y *= b;
			a.z *= b;
			return a;
		}

	template <typename T>
		inline TVec4<T>&	operator*=(TVec4<T>&a, const T&b)
		{
			a.x *= b;
			a.y *= b;
			a.z *= b;
			a.w *= b;
			return a;
		}
	template <typename T>
		inline TVec2<T>&	operator/=(TVec2<T>&a, const T&b)
		{
			a.x /= b;
			a.y /= b;
			return a;
		}

	template <typename T>
		inline TVec3<T>&	operator/=(TVec3<T>&a, const T&b)
		{
			a.x /= b;
			a.y /= b;
			a.z /= b;
			return a;
		}

	template <typename T>
		inline TVec4<T>&	operator/=(TVec4<T>&a, const T&b)
		{
			a.x /= b;
			a.y /= b;
			a.z /= b;
			a.w /= b;
			return a;
		}
}




#include "vclasses.tpl.h"


#endif
