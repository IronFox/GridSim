#pragma once

#include <memory>
#include <geometry/cgs.h>
#include <math/matrix.h>
#include <engine/renderer/opengl.h>
#include <engine/display.h>
#include <container/buffer.h>


namespace D = DeltaWorks;
namespace M = DeltaWorks::Math;

typedef Engine::OpenGL			Renderer;

extern Engine::Display<Renderer>	display;

class Geometry
{
public:


	void				Render();
	index_t				Embed(const D::CGS::Constructor<>::Object&);
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

	D::IndexTable<TSection>sectionMap;


	bool				isDirty = true;
	index_t				idxCounter = 0;

	size_t				vertexFloats=1;
	D::Buffer0<float>	vertexData;
	D::Buffer0<UINT32>	indexData;
};

