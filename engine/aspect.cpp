#include "../global_root.h"
#include "aspect.h"

namespace Engine
{
	template AngularCamera<float>;
	template AngularCamera<double>;
	template VectorCamera<float>;
	template VectorCamera<double>;
	template OrthographicAspect<float>;
	template OrthographicAspect<double>;


	namespace GlobalAspectConfiguration
	{
	bool					worldZIsUp = false,
							loadAsProjection = false;
	};

}
