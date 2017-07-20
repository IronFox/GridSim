#include "../global_root.h"
#include "resolution.h"

namespace DeltaWorks
{

	Resolution::Resolution(const Resolution3D&res):Super(res.width,res.height)
	{
	//	DBG_ASSERT__(res.depth == 1);	going to tolerate this for now
	}
}

