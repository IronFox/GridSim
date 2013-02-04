#include "scenery.h"

#include "renderer/opengl.h"

namespace Engine
{
	template Scenery<OpenGL>;
	template MappedScenery<OpenGL>;
	template SceneryTree<CGS::StdDef>;
	//float global_shortest_edge_length=0.1f;
}
