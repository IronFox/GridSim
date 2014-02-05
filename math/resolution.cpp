#include "../global_root.h"
#include "resolution.h"


Resolution::Resolution(const Resolution3D&res):width(res.width),height(res.height)
{
//	DBG_ASSERT__(res.depth == 1);	going to tolerate this for now
}


hash_t Resolution::Hash::hash(const Resolution&res)
{
	return stdMemHashT<sizeof(Resolution)>(&res);
}

hash_t Resolution3D::Hash::hash(const Resolution3D&res)
{
	return stdMemHashT<sizeof(Resolution3D)>(&res);
}
