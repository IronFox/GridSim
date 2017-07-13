#include "geometryPool.h"


index_t				Geometry::Embed(const D::CGS::Constructor<>::Object&cgs)
{
	const auto&lod = cgs.GetLOD(0);

	if (lod.triangleIndices.IsEmpty() && lod.quadIndices.IsEmpty())
		return InvalidIndex;


	if (indexData.IsEmpty())
	{
		const count_t vertexFloats = cgs.GetVertexSize();
		const UINT flags = cgs.GetVertexFlags();
		const count_t numLayers = cgs.CountTextureLayers();

		vertexBinding.color.unset();
		vertexBinding.floats_per_vertex = (UINT16)vertexFloats;
		vertexBinding.vertex.set(0,3);
		UINT16 offset = 3;
		if (flags & D::CGS::HasNormalFlag)
		{
			vertexBinding.normal.set(offset,3);
			offset += 3;
		}
		if (flags & D::CGS::HasTangentFlag)
		{
			vertexBinding.tangent.set(offset,3);
			offset += 3;
		}
		if (flags & D::CGS::HasColorFlag)
		{
			vertexBinding.color.set(offset,4);
			offset += 4;
		}
		vertexBinding.texcoords.SetSize(numLayers);
		for (index_t i = 0; i < numLayers; i++)
		{
			vertexBinding.texcoords[i].set(offset,2);
			offset+=2;
		}
		ASSERT_EQUAL__(vertexBinding.floats_per_vertex,offset);

		//#define VSIZE(vlyr, flags) (3+2*vlyr+(flags&CGS::HasTangentFlag?3:0)+(flags&CGS::HasNormalFlag?3:0)+(flags&CGS::HasColorFlag?4:0))
	}
	else
		ASSERT_EQUAL__(vertexBinding.floats_per_vertex,cgs.GetVertexSize());

	index_t idx = idxCounter++;

	TSection&section = sectionMap.Set(idx);
	section.iRange.start = indexData.Count();
	section.vRange.start = vertexData.Count() / this->vertexBinding.floats_per_vertex;
	vertexData.Append(lod.vertexData);
	indexData.Append(lod.triangleIndices);
	count_t numQuads = lod.quadIndices.Count()/4;
	for (index_t i = 0; i < numQuads; i++)
	{
		indexData << lod.quadIndices[i*4] << lod.quadIndices[i*4+1] << lod.quadIndices[i*4+2];
		indexData << lod.quadIndices[i*4] << lod.quadIndices[i*4+2] << lod.quadIndices[i*4+3];
	}
	section.vRange.end = vertexData.Count() / this->vertexBinding.floats_per_vertex;
	section.iRange.end = indexData.Count();

	for (index_t i = section.iRange.start; i < section.iRange.end; i++)
		indexData[i] += section.vRange.start;

	isDirty = true;

	return idx;
}

void Geometry::Render()
{
	RebuildIfNeeded();
	display.bindIndices(ibo);
	display.bindVertices(vbo,vertexBinding);

	display.render(0,indexData.Count());

	display.unbindIndices();
	//display.unbindVertices();	undefined
}

void				Geometry::Remove(index_t&idx)
{
	if (idx == InvalidIndex)
		return;
	TSection section;
	if (!sectionMap.QueryAndUnset(idx,section))
	{
		idx = InvalidIndex;
		return;
	}
	idx = InvalidIndex;

	const count_t vCount = section.vRange.GetExtent();
	const count_t iCount = section.iRange.GetExtent();
	vertexData.Erase(section.vRange.start * vertexBinding.floats_per_vertex, vCount * vertexBinding.floats_per_vertex);
	indexData.Erase(section.iRange.start, section.iRange.GetExtent());
	for (index_t i = section.iRange.start; i < indexData.Count(); i++)
		indexData[i] -= vCount;

	sectionMap.VisitAllEntries([section,idx](index_t key, TSection&sec)
	{
		if (sec.iRange.start > section.iRange.start)
			sec.iRange -= section.iRange.GetExtent();
		if (sec.vRange.start > section.vRange.start)
			sec.vRange -= section.vRange.GetExtent();
	});

	isDirty = true;
}

void				Geometry::RebuildIfNeeded()
{
	if (!isDirty)
		return;
	isDirty = false;

	vbo.load(vertexData);
	ibo.load(indexData);
}



