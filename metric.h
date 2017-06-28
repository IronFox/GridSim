#pragma once

#include "types.h"
#include <math/vector.h>
#include <general/random.h>

namespace Metric
{
	struct Distance
	{
	private:
		float value;
	public:
		/**/	Distance():value(0)	{}
		/**/	Distance(const TVec2<>&a, const TVec2<>&b);
		/**/	Distance(const TVec3<>&a, const TVec3<>&b);
		/**/	Distance(const TVec2<>&v);
		/**/	Distance(const TVec3<>&v);

		constexpr	Distance(float constant):
			#if defined USE_CHEBYSHEV_DISTANCE || defined USE_RECTILINEAR_DISTANCE
				value (constant)
			#else
				value (constant*constant)
			#endif
		{}

		bool	operator<(float threshold) const
		{
			#if defined USE_CHEBYSHEV_DISTANCE || defined USE_RECTILINEAR_DISTANCE
				return value < threshold;
			#else
				return value < threshold*threshold;
			#endif
		}

		bool	operator<=(float threshold) const
		{
			#if defined USE_CHEBYSHEV_DISTANCE || defined USE_RECTILINEAR_DISTANCE
				return value <= threshold;
			#else
				return value <= threshold*threshold;
			#endif
		}
		bool	operator<(const Distance&threshold) const
		{
			return value < threshold.value;
		}

		bool	operator<=(const Distance&threshold) const
		{
			return value <= threshold.value;
		}

		bool	operator>(float threshold) const
		{
			#if defined USE_CHEBYSHEV_DISTANCE || defined USE_RECTILINEAR_DISTANCE
				return value > threshold;
			#else
				return value > threshold*threshold;
			#endif
		}

		bool	operator>=(float threshold) const
		{
			#if defined USE_CHEBYSHEV_DISTANCE || defined USE_RECTILINEAR_DISTANCE
				return value >= threshold;
			#else
				return value >= threshold*threshold;
			#endif
		}
		bool	operator>(const Distance&threshold) const
		{
			return value > threshold.value;
		}

		bool	operator>=(const Distance&threshold) const
		{
			return value >= threshold.value;
		}

		float	Squared() const
		{
			#if defined USE_CHEBYSHEV_DISTANCE || defined USE_RECTILINEAR_DISTANCE
				return value*value;
			#else
				return value;
			#endif
		}

		float	operator*() const
		{
			#if defined USE_CHEBYSHEV_DISTANCE || defined USE_RECTILINEAR_DISTANCE
				return value;
			#else
				return sqrt(value);
			#endif
		}

	};


	struct Direction2D
	{
		typedef float	Angle;
		float		value;

		/**/		Direction2D():value(0)	{}
		explicit	Direction2D(Random&random):value(Mod(random.NextFloat(MaxValue))){};
		explicit	Direction2D(const TVec2<>&);
		explicit	Direction2D(float linear01):value(Mod(linear01 * MaxValue))	{}

		static const constexpr float MaxValue = 
			#if defined USE_CHEBYSHEV_DISTANCE || defined USE_RECTILINEAR_DISTANCE
				4.f;
			#else
				2.f * M_PIF;
			#endif
/*
		float	GetChebyshevAngle() const
		{
			#ifdef USE_CHEBYSHEV_DISTANCE
				return value;
			#else
				return value / M_PIF / 2.f * MaxValue;
			#endif
		}*/

		float	GetAngle() const
		{
			#ifdef USE_CHEBYSHEV_DISTANCE
				return value / MaxValue * M_PIF * 2.f;
			#elif defined USE_RECTILINEAR_DISTANCE
			return value / MaxValue * M_PIF * 2.f;
			#else
				return value;
			#endif
		}

		bool	operator==(const Direction2D&d)	 const {return value == d.value;}
		bool	operator!=(const Direction2D&d)	 const {return value != d.value;}

		void	operator+=(float delta) {value = Mod(value+delta);}

		float	MinimalDifferenceTo(const Direction2D&other) const
		{
			float rs = fabs(value - other.value);
			if (rs > MaxValue/2)
				rs = MaxValue - rs;
			return rs;
		}

		static float Mod(float x)
		{
		    return x - MaxValue * floor( x / MaxValue );
		}

		void	ToVector(TVec2<>&rs)	const;
		TVec2<>	ToVector() const	{TVec2<> rs; ToVector(rs); return rs;}
	};

	struct Direction3D
	{
		typedef float2 Angle;
		Angle		value;

		/**/		Direction3D():value(0)	{}
		explicit	Direction3D(Random&random):value(random.NextFloat(MaxValueX),random.NextFloat(MaxValueY)){};
		explicit	Direction3D(const TVec3<>&);
		explicit	Direction3D(float linear01X, float linear01Y):value(linear01X * MaxValueX,linear01Y*MaxValueY)	{}

		static const constexpr float MaxValueX = 
			#if defined USE_CHEBYSHEV_DISTANCE || defined USE_RECTILINEAR_DISTANCE
				6.f;	//side index (0..5) + side X axis [0,1]
			#else
				2.f * M_PIF;
			#endif

		static const constexpr float MaxValueY = 
			#if defined USE_CHEBYSHEV_DISTANCE || defined USE_RECTILINEAR_DISTANCE
				1.f;	//side Y axis  [0,1]
			#else
				2.f * M_PIF;
			#endif

		static const constexpr TVec2<> MaxValue = {MaxValueX, MaxValueY};


		Angle	GetAngle() const
		{
			return value;
		}

		bool	operator==(const Direction3D&d)	 const {return value == d.value;}
		bool	operator!=(const Direction3D&d)	 const {return value != d.value;}

		void	operator+=(const Angle&delta) {value = Wrap(value+delta);}

		static Angle Wrap(Angle x)
		{
			x.x -=  MaxValueX * floor( x.x / MaxValueX );
			x.y -=  MaxValueY * floor( x.y / MaxValueY );
			return x;
		}

		void	ToVector(TVec3<>&rs)	const;
		TVec3<>	ToVector() const	{TVec3<> rs; ToVector(rs); return rs;}
	};


	#ifdef D3
		typedef Direction3D	Direction;
	#else
		typedef Direction2D	Direction;
	#endif

	bool	CheckDistance(const char*operation, const EntityID&a, const EntityID&b, float maxDistance);

}
