#include "../global_root.h"
#include "bvh.h"

namespace DeltaWorks
{
	template BVH<void,1>;
	template BVH<void, 2>;
	template SphereBVH <void, 1>;
	template SphereBVH <void, 2>;
}