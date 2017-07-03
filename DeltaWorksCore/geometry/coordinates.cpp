#include "../global_root.h"
#include "coordinates.h"


namespace DeltaWorks
{
	namespace Math
	{
		namespace Composite
		{
			const float		Coordinates::default_sector_size=50000.0f;
			const float		default_sector_size=Coordinates::default_sector_size;
	

			void	Scalar::wrap(float sector_size)
			{
				sector_t sdelta = (sector_t)(remainder/sector_size);
				sector += sdelta;
				remainder -= sdelta*sector_size;
			}

			void	Coordinates::wrap(float sector_size)
			{
				M::TVec3<sector_t> sdelta;
				M::Vec::div(remainder,sector_size,sdelta);
				M::Vec::add(sector,sdelta);
				M::Vec::mad(remainder,sdelta,-sector_size);
			}



		}
	}
}

