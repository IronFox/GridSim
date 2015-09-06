#include "../global_root.h"
#include "aspect.h"

namespace Engine
{
	template class AngularCamera<float>;
	template class AngularCamera<double>;
	template class VectorCamera<float>;
	template class VectorCamera<double>;
	template class OrthographicAspect<float>;
	template class OrthographicAspect<double>;
	template class Frustum<float>;
	template class Frustum<double>;


	namespace GlobalAspectConfiguration
	{
	bool					worldZIsUp = false,
							loadAsProjection = false;
	};

}
