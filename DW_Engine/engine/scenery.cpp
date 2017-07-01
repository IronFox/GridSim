#include "scenery.h"

#include "renderer/opengl.h"

namespace Engine
{
	template class Scenery<OpenGL>;
	template class MappedScenery<OpenGL>;
	template class SceneryTree<CGS::StdDef>;
	//float global_shortest_edge_length=0.1f;
}
