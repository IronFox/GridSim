#include "../global_root.h"
#include "resolution.h"


Resolution::Resolution(const Resolution3D&res):width(res.width),height(res.height)
{
//	DBG_ASSERT__(res.depth == 1);	going to tolerate this for now
}


