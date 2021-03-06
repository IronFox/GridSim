#pragma once

#include <memory>
#include <geometry/cgs.h>
#include <math/matrix.h>
#include <engine/renderer/opengl.h>
#include <engine/display.h>
#include <container/buffer.h>


using namespace DeltaWorks;

typedef Engine::OpenGL			Renderer;

extern Engine::Display<Renderer>	display;

class Geometry
{
public:


	void				Render();
	index_t				Embed(const CGS::Constructor<>::Object&);
	void				Remove(index_t&);
	void				RebuildIfNeeded();

private:
	Renderer::IBO		ibo;
	Renderer::VBO		vbo;
	Engine::VertexBinding	vertexBinding;

	struct TSection
	{
		M::TIntRange<index_t>	vRange,
								iRange;
	};

	IndexTable<TSection>sectionMap;


	bool				isDirty = true;
	index_t				idxCounter = 0;

	Buffer0<float>	vertexData;
	Buffer0<UINT32>	indexData;
};

