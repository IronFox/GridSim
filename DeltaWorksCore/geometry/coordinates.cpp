#include "../global_root.h"
#include "coordinates.h"


namespace DeltaWorks
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
			TVec3<sector_t> sdelta;
			Vec::div(remainder,sector_size,sdelta);
			Vec::add(sector,sdelta);
			Vec::mad(remainder,sdelta,-sector_size);
		}



	}
}

