#include <global_root.h>
#include "metric.h"
#include "entity.h"

namespace Metric
{

	static float RectilinearLength(const M::TVec2<>&v)
	{
		return fabs(v.x) + fabs(v.y);
	}
//		#if defined USE_CHEBYSHEV_DISTANCE || defined USE_RECTILINEAR_DISTANCE
	static float	RectilinearDistance(const M::TVec2<>&a, const M::TVec2<>&b)
	{
		M::TVec2<> v;
		Vec::sub(a, b, v);
		return RectilinearLength(v);
	}


	static float	ChebyshevLength(const M::TVec2<>&v)
	{
		return M::Max( fabs(v.x),fabs(v.y));
	}

	static float	ChebyshevDistance(const M::TVec2<>&a, const M::TVec2<>&b)
	{
		M::TVec2<> v;
		Vec::sub(a,b,v);
		return ChebyshevLength(v);
	}

	static float	ChebyshevLength(const M::TVec3<>&v)
	{
		return M::Max(fabs(v.z), M::Max( fabs(v.x),fabs(v.y)));
	}

	static float	ChebyshevDistance(const M::TVec3<>&a, const M::TVec3<>&b)
	{
		M::TVec3<> v;
		Vec::sub(a,b,v);
		return ChebyshevLength(v);
	}

	Distance::Distance(const M::TVec3<>&v)
	{
		#ifdef USE_CHEBYSHEV_DISTANCE
			value = ChebyshevLength(v);
		#elif defined USE_RECTILINEAR_DISTANCE
			value = RectilinearLength(v);
		#else
			value = Vec::dot(v);
		#endif
	}

	Distance::Distance(const M::TVec2<>&v)
	{
		#ifdef USE_CHEBYSHEV_DISTANCE
			value = ChebyshevLength(v);
		#elif defined USE_RECTILINEAR_DISTANCE
			value = RectilinearLength(v);
		#else
			value = Vec::dot(v);
		#endif
	}

	Distance::Distance(const M::TVec2<>&a, const M::TVec2<>&b)
	{
		#ifdef USE_CHEBYSHEV_DISTANCE
			value = ChebyshevDistance(a,b);
		#elif defined USE_RECTILINEAR_DISTANCE
			value = RectilinearDistance(a,b);
		#else
			value = Vec::quadraticDistance(a,b);
		#endif
	}

	Distance::Distance(const M::TVec3<>&a, const M::TVec3<>&b)
	{
		#ifdef USE_CHEBYSHEV_DISTANCE
			value = ChebyshevDistance(a,b);
		#elif defined USE_RECTILINEAR_DISTANCE
			value = RectilinearDistance(a,b);
		#else
			value = Vec::quadraticDistance(a,b);
		#endif
	}

	bool	CheckDistance(const char*operation, const EntityID&a, const EntityID&b, float maxDistance)
	{
		Distance m(a.coordinates,b.coordinates);
		if (m <= maxDistance)
			return true;
		LogUnexpected(String(operation)+": OP range exceeded ("+String(*m)+">"+String(maxDistance)+")",a,&b);
		return false;
	}


	float DecodeChebyshevDirection(const M::TVec2<>&dir)
	{
		if (!dir.x && !dir.y)
			return 0;
		float len = ChebyshevLength(dir);
		float x = dir.x / len;
		float y = dir.y / len;
		if (fabs(y) >= fabs(x))
		{
			if (y > 0)
				return (1.f - x)*0.5f;
			return (1.f + x) * 0.5f + 2.f;
		}
		if (x > 0)
			return (1.f + y) * 0.5f + 3.f;
		return (1.f - y) * 0.5f + 1.f;
	}

	float DecodeRectilinearDirection(const M::TVec2<>&dir)
	{
		M::TVec2<> dir2;
		Vec::div(dir, RectilinearLength(dir),dir2);
		if (dir2.y >= 0)
			return 1.f - dir2.x;
		return 3.f + dir2.x;
	}

	float DecodeDirection(const M::TVec2<>&dir)
	{
		#ifdef USE_CHEBYSHEV_DISTANCE
			return DecodeChebyshevDirection(dir);
		#elif defined USE_RECTILINEAR_DISTANCE
			return DecodeRectilinearDirection(dir);
		#else
			return Vec::angle2PI(dir);
		#endif
	}

	float2 DecodeChebyshevDirection(M::TVec3<> dir);

	float2 DecodeDirection(const M::TVec3<>&dir)
	{
		#ifdef USE_CHEBYSHEV_DISTANCE
			return DecodeChebyshevDirection(dir);
		#elif defined USE_RECTILINEAR_DISTANCE
			return DecodeRectilinearDirection(dir);
		#else
			return Vec::angle2PI(dir);
		#endif
	}


	static void ChebychevToVector(float value, M::TVec2<>&rs)
	{
		if (value < 1)
		{
			rs.x = 1.f - value * 2.f;
			rs.y = 1.f;
		}
		elif(value < 2)
		{
			rs.y = 1.f - (value - 1.f) * 2.f;
			rs.x = -1.f;
		}
		elif(value < 3)
		{
			rs.x = -1.f + (value - 2.f) * 2.f;
			rs.y = -1.f;
		}
		else
		{
			rs.y = -1.f + (value - 3.f) * 2.f;
			rs.x = 1.f;
		}
		DBG_ASSERT__(M::Similar(1.f, *Distance(rs)));
	}

	float2 DecodeChebyshevDirection(M::TVec3<> dir)
	{
		M::TVec3<> abs=dir;
		Vec::abs(abs);

		float max = Vec::max(abs);
		if (max == 0)
			return float2();
		//abs /= max;
		dir /= max;

		float2 rs;

		if (abs.z >= abs.x && abs.z >= abs.y)
		{
			if (dir.z > 0)
			{
				//first side
				rs.x = dir.x * 0.5f + 0.5f;
				rs.y = dir.y * 0.5f + 0.5f;
			}
			else
			{
				//second side
				rs.x = 1.f + (dir.x * 0.5f + 0.5f);
				rs.y = dir.y * 0.5f + 0.5f;
			}
		}
		elif (abs.y >= abs.x && abs.y >= abs.z)
		{
			if (dir.y > 0)
			{
				//third side
				rs.x = 2.f + dir.x * 0.5f + 0.5f;
				rs.y = dir.z * 0.5f + 0.5f;
			}
			else
			{
				//fourth side
				rs.x = 3.f + dir.x * 0.5f + 0.5f;
				rs.y = dir.z * 0.5f + 0.5f;
			}
		}
		else
		{
			if (dir.x > 0)
			{
				//fifth side
				rs.x = 4.f + dir.z * 0.5f + 0.5f;
				rs.y = dir.y * 0.5f + 0.5f;
			}
			else
			{
				//sixth side
				rs.x = 5.f + dir.z * 0.5f + 0.5f;
				rs.y = dir.y * 0.5f + 0.5f;
			}
		}

		return rs;
	}

	static void ChebychevToVector(const M::TVec2<>&value, M::TVec3<>&rs)
	{
		if (value.x < 1)
		{
			//front side
			rs.z = 1;
			rs.x = value.x*2.f - 1.f;
			rs.y = value.y*2.f - 1.f;
		}
		elif(value.x < 2)
		{
			//back side
			rs.z = -1;
			rs.x = fmod(value.x,1)*2.f - 1.f;
			rs.y = value.y*2.f - 1.f;
		}
		elif(value.x < 3)
		{
			rs.y = 1;
			rs.x = fmod(value.x,1)*2.f - 1.f;
			rs.z = value.y*2.f - 1.f;
		}
		elif(value.x < 4)
		{
			rs.y = -1;
			rs.x = fmod(value.x,1)*2.f - 1.f;
			rs.z = value.y*2.f - 1.f;
		}
		elif(value.x < 5)
		{
			rs.x = 1;
			rs.z = fmod(value.x,1)*2.f - 1.f;
			rs.y = value.y*2.f - 1.f;
		}
		else
		{
			rs.x = -1;
			rs.z = fmod(value.x,1)*2.f - 1.f;
			rs.y = value.y*2.f - 1.f;
		}
		DBG_ASSERT__(M::Similar(1.f, *Distance(rs)));
	}
	static void RectilinearToVector(float value, M::TVec2<>&rs)
	{
		if (value <= 2)
		{
			rs.x = 1.f - value;
			rs.y = 1.f - fabs(1.f - value);
		}
		else
		{
			rs.x = value - 3.f;
			rs.y = -(1.f - fabs(3.f - value));
		}
	}



	Direction2D::Direction2D(const M::TVec2<>&dir):value(DecodeDirection(dir))
	{}


	void	Direction2D::ToVector(M::TVec2<>&rs)	const
	{
		#ifdef USE_CHEBYSHEV_DISTANCE
			ChebychevToVector(Mod(value), rs);
		#elif defined USE_RECTILINEAR_DISTANCE
			RectilinearToVector(Mod(value), rs);
		#else
			rs.x = cos(value);
			rs.y = sin(value);
		#endif

	}




	Direction3D::Direction3D(const M::TVec3<>&dir):value(DecodeDirection(dir))
	{}


	void	Direction3D::ToVector(M::TVec3<>&rs)	const
	{
		#ifdef USE_CHEBYSHEV_DISTANCE
			ChebychevToVector(value, rs);
		#elif defined USE_RECTILINEAR_DISTANCE
			RectilinearToVector(value, rs);
		#else
			rs.x = cos(value);
			rs.y = sin(value);
		#endif

	}



}
