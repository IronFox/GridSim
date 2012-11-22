#include "../global_root.h"
#include "resolution.h"

hash_t Resolution::Hash::hash(const Resolution&res)
{
	return stdMemHashT<sizeof(Resolution)>(&res);
}

