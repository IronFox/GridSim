#include <global_root.h>
#include "InconsistencyCoverage.h"
#include "entity.h"

#include <container/array2d.h>
#include <container/array3d.h>
#include "EntityStorage.h"
#include "shard.h"


HashProcessGrid::HashProcessGrid(const HGrid&h,index_t timestep,const TGridCoords&localOffset):localOffset(localOffset)
{
	ASSERT__(h.grid.IsNotEmpty());
	grid.SetSize(h.grid.GetSize());

	Concurrency::parallel_for(index_t(0),grid.Count(),[this,&h,timestep](index_t i)
	{
		grid[i].hasher.AppendPOD(h.grid[i]);
		grid[i].hasher.AppendPOD(timestep);
	});
}

HashProcessGrid::HashProcessGrid(index_t timestep,const TGridCoords&localOffset):localOffset(localOffset)
{
	static const auto Resolution = HGrid::Resolution;
	grid.SetSize(TGrid::Size(Resolution));

	Concurrency::parallel_for(index_t(0),grid.Count(),[this,timestep](index_t i)
	{
		grid[i].hasher.AppendPOD(timestep);
	});
}

void		HashProcessGrid::Finish(HGrid&h)
{
	ASSERT__(grid.IsNotEmpty());
	h.grid.SetSize(grid.GetSize());
	Concurrency::parallel_for(index_t(0),grid.Count(),[this,&h](index_t i)
	{
		grid[i].hasher.Finish(h.grid[i].history);
	});

}

void		HashProcessGrid::Include(const EntityStorage&store)
{
	Concurrency::parallel_for(index_t(0),store.Count(),[this,&store](index_t i)
	{
		const auto&e = store[i];
		auto&cell = GetCellOfW(e.coordinates);
		cell.lock.lock();
		e.Hash(cell.hasher);
		cell.lock.unlock();
	});
}

HashProcessGrid::Cell&		HashProcessGrid::GetCellOfW(const TEntityCoords&worldCoords)
{
	return GetCellOfL(TEntityCoords(worldCoords - localOffset));
}

HashProcessGrid::Cell&		HashProcessGrid::GetCellOfL(const TEntityCoords&localCoords)
{
	TGridCoords c = localCoords / HGrid::Resolution;
	Vec::clamp(c,0,HGrid::Resolution-1);
	return GetCellOfL(c);
}

HashProcessGrid::Cell&		HashProcessGrid::GetCellOfL(const TGridCoords&localCoords)
{
	#ifdef D3
		return grid.Get(localCoords.x,localCoords.y,localCoords.z);
	#else
		return grid.Get(localCoords.x,localCoords.y);
	#endif
}


void		HGrid::ExportEdge(HGrid&target, const TGridCoords&edgeDelta) const
{
	const Volume<int> local = Volume<int>(TGridCoords(0),TGridCoords(Resolution));
	Volume<int> vol = local;
	vol.Translate(edgeDelta * (Resolution -1));
	vol.ConstrainBy(local);


	target.grid.ResizeToAndCopyBlock(grid,VectorToIndex(vol.min()),VectorToSize(vol.GetExtent()));

}


const HGrid::TCell&		HGrid::GetCellOfW(const TEntityCoords&worldCoords, const TGridCoords&localOffset) const
{
	return GetCellOfL(TEntityCoords(worldCoords - localOffset));
}

const HGrid::TCell&		HGrid::GetCellOfL(const TEntityCoords&localCoords) const
{
	TGridCoords c = localCoords / Resolution;
	Vec::clamp(c,0,Resolution-1);
	return GetCellOfL(c);
}

const HGrid::TCell&		HGrid::GetCellOfL(const TGridCoords&localCoords) const
{
	#ifdef D3
		return grid.Get(localCoords.x,localCoords.y,localCoords.z);
	#else
		return grid.Get(localCoords.x,localCoords.y);
	#endif
}






/*virtual override*/ int	InconsistencyCoverage::OrthographicComparator::operator()(const TSample&s0, const TSample&s1) const
{
	if (s0.depth < s1.depth)
		return -1;
	if (s0.depth > s1.depth)
		return 1;
	if (s0.depth == 0)
		return 0;
	if (s0.blurExtent > s1.blurExtent)
		return -1;	//larger extent means probably not as bad
	if (s0.blurExtent < s1.blurExtent)
		return 1;
	return 0;
}

/*virtual override*/ int	InconsistencyCoverage::ReverseOrthographicComparator::operator()(const TSample&s0, const TSample&s1) const
{
	int rs = Compare(s0.depth != 0, s1.depth != 0);
	if (rs || s0.depth == 0)
		return rs;
	if (s0.blurExtent > s1.blurExtent)
		return -1;	//larger extent means probably not as bad
	if (s0.blurExtent < s1.blurExtent)
		return 1;
	if (s0.depth < s1.depth)
		return -1;
	if (s0.depth > s1.depth)
		return 1;
	return 0;
}


void		InconsistencyCoverage::TSample::IncreaseDepth()
{
	if (depth < 0xFF)
		depth ++;
}


void	InconsistencyCoverage::TSample::SetWorst(const TSample&a, const TSample&b)
{
	if (a.blurExtent == 0xFF || b.blurExtent == 0xFF)
		blurExtent = 0xFF;
	else
		blurExtent = vmin(a.blurExtent,b.blurExtent);
	depth = vmax(a.depth,b.depth);
}

void	InconsistencyCoverage::TSample::Include(const TSample&other)
{
	if (blurExtent == 0xFF || other.blurExtent == 0xFF)
		blurExtent = 0xFF;
	else
		blurExtent = vmin(blurExtent,other.blurExtent);
	depth = vmax(depth,other.depth);
}


InconsistencyCoverage::TSample&	InconsistencyCoverage::TSample::IntegrateGrowingNeighbor(const TSample&n, UINT32 distance)
{
	if (n.depth <= distance)
		return *this;
	//if (blurExtent != 0xFF && distance != 0)	//else: copy from self
	//{
	//	DBG_ASSERT_LESS_OR_EQUAL__(n.depth-distance, depth);
	//	DBG_ASSERT_GREATER__(n.depth, distance);
	//	//return *this;
	//}
	//content_t oldBlur = blurExtent;
	content_t nDepth = n.depth - distance;
	if (blurExtent != 0xFF)
	{
		if (n.blurExtent == 0xFF)
			blurExtent = 0xFF;
		else
		{
			const content_t old = blurExtent;
			const UINT32 nExt = (UINT32)n.blurExtent + distance;
			blurExtent = (content_t)vmin(0xFE, vmin<UINT32>(blurExtent, nExt));
			//if (blurExtent < old && old != 0xFE && nDepth <= depth)
			//	blurExtent = 0xFF;
		}
	}

	if (nDepth > depth)
	{
		depth = nDepth;
	}
	//ASSERT__(!IsInvalid());
	return *this;
}

void		InconsistencyCoverage::TSample::SetBest(const TSample&a, const TSample&b, const Comparator&comp)
{
	int c = comp(a,b);
	if (c <= 0)
		*this = a;
	else
		*this = b;
	//blurExtent = vmax(a.blurExtent,b.blurExtent);
	//depth = vmin(a.depth,b.depth);
}


void		InconsistencyCoverage::LoadMinimum(const InconsistencyCoverage&a, const InconsistencyCoverage&b, const Comparator&comp)
{
	ASSERT__(!sealed);
	
	offset.x = vmax(offset.x,a.offset.x);
	offset.y = vmax(offset.y,a.offset.y);
	#ifdef D3
		offset.z = vmax(offset.z,a.offset.z);
	#endif

	grid.SetSize(
						vmin(a.grid.GetWidth(),b.grid.GetWidth()),
						vmin(a.grid.GetHeight(),b.grid.GetHeight())
						#ifdef D3
							,vmin(a.grid.GetDepth(),b.grid.GetDepth())
						#endif
					);
	highest = 0;

	#ifdef D3
		foreach (grid.Horizontal(),x)
			foreach (grid.Vertical(),y)
				foreach (grid.Deep(),z)
				{
					TGridCoords c;
					Vec::def(c,*x + offset.x,*y + offset.y,*z + offset.z);


					const TSample&va = a.GetSample(c);
					const TSample&vb = b.GetSample(c);
					TSample&merged = grid.Get(x,y,z);
					merged.SetBest(va,vb,comp);
					highest = vmax(highest,merged.depth);
				}
	#else

		foreach (grid.Horizontal(),x)
			foreach (grid.Vertical(),y)
			{
				TGridCoords c((int)*x + offset.x,(int)*y + offset.y);

				const TSample&va = a.GetSample(c);
				const TSample&vb = b.GetSample(c);
				TSample&merged = grid.Get(x,y);
				merged.SetBest(va,vb,comp);
				highest = vmax(highest,merged.depth);
			}
	#endif

	this->VerifyIntegrity(CLOCATION);
}


bool		InconsistencyCoverage::operator==(const InconsistencyCoverage&other) const
{
	return highest == other.highest && offset == other.offset && 
		grid == other.grid;
}


void InconsistencyCoverage::FillMinInconsistent()
{
	ASSERT__(!sealed);
	TExtSample sample;
	sample.depth = 1;
	sample.blurExtent = 0;
	grid.Fill(sample);
	highest = sample.depth;
}


void		InconsistencyCoverage::Hash(Hasher&inputHash)	const
{
	const auto size = grid.GetSize();
	static_assert(sizeof(size) == sizeof(index_t)*Dimensions,"POD constraints violated");
	inputHash.AppendPOD(size);

	foreach (grid,c)
	{
		c->Hash(inputHash);
	}

}


void InconsistencyCoverage::Grow(InconsistencyCoverage & rs) const
{
	ASSERT__(!rs.sealed);
	VerifyIntegrity(CLOCATION);
	const UINT32 expandBy = (UINT32)ceil(Entity::MaxInfluenceRadius * Resolution);
	#ifdef D3
		rs.grid.SetSize(grid.GetWidth() + 2 *expandBy,grid.GetHeight() + 2*expandBy,grid.GetDepth() + 2*expandBy);
	#else
		rs.grid.SetSize(grid.GetWidth() + 2 *expandBy,grid.GetHeight() + 2*expandBy);
	#endif
	rs.offset = offset - expandBy;


	#ifdef D3
		//core
		for (UINT32 z = 0; z < this->grid.GetDepth(); z++)
			for (UINT32 y = 0; y < this->grid.GetHeight(); y++)
				for (UINT32 x = 0; x < this->grid.GetWidth(); x++)
				{
					const TSample&v = grid.Get(x, y, z);
					auto&out = rs.grid.Get(x + expandBy, y + expandBy, z + expandBy);
					out = v;
					if (!out.IsConsistent())
						out.IncreaseDepth();
				}
		//blurred
		for (UINT32 z = 0; z < this->grid.GetDepth(); z++)
			for (UINT32 y = 0; y < grid.GetHeight(); y++)
				for (UINT32 x = 0; x < grid.GetWidth(); x++)
				{
					TSample v = grid.Get(x,y,z);
					if (!v.IsConsistent())
						v.IncreaseDepth();
					if (v.depth < 2)
						continue;
					for (UINT32 i = 0; i <= expandBy; i++)
						for (UINT32 j = 0; j <= expandBy; j++)
							for (UINT32 k = 0; k <= expandBy; k++)
							if (i || j || k)
							{
								UINT32 dist = vmax(k,vmax(i, j));
								rs.grid.Get(x + expandBy - i, y + expandBy - j, z + expandBy - k).IntegrateGrowingNeighbor(v, dist);
								if (j != 0)
									rs.grid.Get(x + expandBy - i, y + expandBy + j, z + expandBy - k).IntegrateGrowingNeighbor(v,dist);
								if (i != 0)
								{
									rs.grid.Get(x + expandBy + i, y + expandBy - j, z+expandBy-k).IntegrateGrowingNeighbor(v, dist);
									if (j != 0)
										rs.grid.Get(x + expandBy + i, y + expandBy + j, z+expandBy-k).IntegrateGrowingNeighbor(v, dist);
								}

								if (k != 0)
								{
									rs.grid.Get(x + expandBy - i, y + expandBy - j, z + expandBy + k).IntegrateGrowingNeighbor(v, dist);
									if (j != 0)
										rs.grid.Get(x + expandBy - i, y + expandBy + j, z + expandBy + k).IntegrateGrowingNeighbor(v,dist);
									if (i != 0)
									{
										rs.grid.Get(x + expandBy + i, y + expandBy - j, z+expandBy+k).IntegrateGrowingNeighbor(v, dist);
										if (j != 0)
											rs.grid.Get(x + expandBy + i, y + expandBy + j, z+expandBy+k).IntegrateGrowingNeighbor(v, dist);
									}
								}
							}
				}

	#else

		//core
		for (UINT32 y = 0; y < this->grid.GetHeight(); y++)
			for (UINT32 x = 0; x < this->grid.GetWidth(); x++)
			{
				const TExtSample&v = grid.Get(x, y);
				auto&out = rs.grid.Get(x + expandBy, y + expandBy);
				out = v;
				if (!out.IsConsistent())
					out.IncreaseDepth();
			}
		//blurred
		for (UINT32 y = 0; y < grid.GetHeight(); y++)
			for (UINT32 x = 0; x < grid.GetWidth(); x++)
			{
				TSample v = grid.Get(x,y);
				if (!v.IsConsistent())
					v.IncreaseDepth();
				if (v.depth < 2)
					continue;
				for (UINT32 i = 0; i <= expandBy; i++)
					for (UINT32 j = 0; j <= expandBy; j++)
						if (i || j)
						{
							UINT32 dist = vmax(i, j);
							rs.grid.Get(x + expandBy - i, y + expandBy - j).IntegrateGrowingNeighbor(v, dist);
							if (j != 0)
								rs.grid.Get(x + expandBy - i, y + expandBy + j).IntegrateGrowingNeighbor(v,dist);
							if (i != 0)
							{
								rs.grid.Get(x + expandBy + i, y + expandBy - j).IntegrateGrowingNeighbor(v, dist);
								if (j != 0)
									rs.grid.Get(x + expandBy + i, y + expandBy + j).IntegrateGrowingNeighbor(v, dist);
							}
						}
			}
	#endif
	rs.highest = highest;
	if (highest && highest < 0xFF)
		rs.highest++;
	rs.VerifyIntegrity(CLOCATION);
}

template <class S>
static const InconsistencyCoverage::TExtSample*	GetVerified(const Array2D<InconsistencyCoverage::TExtSample,S>&array, const VecN<int,2>&c)
{
	if (c.x < 0 || c.y < 0)
		return nullptr;
	if ((index_t)c.x >= array.GetWidth() || (index_t)c.y >= array.GetHeight())
		return nullptr;
	return &array.Get(c.x,c.y);
}

template <class S>
static InconsistencyCoverage::TExtSample*	GetVerified(Array2D<InconsistencyCoverage::TExtSample,S>&array, const VecN<int,2>&c)
{
	if (c.x < 0 || c.y < 0)
		return nullptr;
	if ((index_t)c.x >= array.GetWidth() || (index_t)c.y >= array.GetHeight())
		return nullptr;
	return &array.Get(c.x,c.y);
}


template <class S>
static const InconsistencyCoverage::TSample*	GetVerified(const Array3D<InconsistencyCoverage::TSample,S>&array, const VecN<int,3>&c)
{
	if (c.x < 0 || c.y < 0 || c.z < 0)
		return nullptr;
	if ((index_t)c.x >= array.GetWidth() || (index_t)c.y >= array.GetHeight() || (index_t)c.z >= array.GetDepth())
		return nullptr;
	return &array.Get(c.x,c.y,c.z);
}

template <class S>
static InconsistencyCoverage::TSample*	GetVerified(Array3D<InconsistencyCoverage::TSample,S>&array, const VecN<int,3>&c)
{
	if (c.x < 0 || c.y < 0 || c.z < 0)
		return nullptr;
	if ((index_t)c.x >= array.GetWidth() || (index_t)c.y >= array.GetHeight() || (index_t)c.z >= array.GetDepth())
		return nullptr;
	return &array.Get(c.x,c.y,c.z);
}

void InconsistencyCoverage::IncludeMissing(const TGridCoords&sectorDelta, index_t linearShardIndex, count_t numShards)
{
	ASSERT__(!sealed);
	VerifyIntegrity(CLOCATION);
	TGridCoords pixelDelta = offset - sectorDelta * Resolution - (-1);

	TExtSample v2;
	v2.depth = 1;
	v2.blurExtent = 0;
	v2.unavailableShards.SetSize(numShards);
	v2.unavailableShards.SetBit(linearShardIndex,true);

	#ifdef D3
		for (UINT32 z = 0; z < grid.GetDepth(); z++)
	#endif
	for (UINT32 y = 0; y < grid.GetHeight(); y++)
		for (UINT32 x = 0; x < grid.GetWidth(); x++)
		{
			#ifdef D3
				TGridCoords at2 = TGridCoords(x,y,z) + pixelDelta;
			#else
				TGridCoords at2 = TGridCoords(x,y) + pixelDelta;
			#endif
			if (at2.x < 0 || at2.x >= Resolution+2
				|| at2.y < 0 || at2.y >= Resolution+2
				#ifdef D3
					|| at2.z < 0 || at2.z >= Resolution+2
				#endif
				)
				continue;
			#ifdef D3
				TSample&v = grid.Get(x,y,z);
			#else
				TSample&v = grid.Get(x,y);
			#endif
			v.Include(v2);
			highest = vmax(highest,v.depth);
		}
	VerifyIntegrity(CLOCATION);
}



bool InconsistencyCoverage::Include(const TGridCoords & sectorDelta, const InconsistencyCoverage & remote)
{
	ASSERT__(!sealed);
	VerifyIntegrity(CLOCATION);
	remote.VerifyIntegrity(CLOCATION);
	bool rs = false;
	TGridCoords pixelDelta = offset - sectorDelta * Resolution - remote.offset;
	#ifdef D3
		for (UINT32 z = 0; z < grid.GetDepth(); z++)
	#endif
	for (UINT32 y = 0; y < grid.GetHeight(); y++)
		for (UINT32 x = 0; x < grid.GetWidth(); x++)
		{
			#ifdef D3
				TGridCoords at2 = TGridCoords(x,y,z) + pixelDelta;
			#else
				TGridCoords at2 = TGridCoords(x,y) + pixelDelta;
			#endif
			const TExtSample*v2 = GetVerified(remote.grid,at2);
			if (!v2 || v2->IsConsistent())
				continue;
			#ifdef D3
				TSample&v = grid.Get(x,y,z);
			#else
				TSample&v = grid.Get(x,y);
			#endif
			v.Include(*v2);
			//v.SetWorst(v,*v2);
			highest = vmax(highest,v.depth);
			rs = true;
		}
	VerifyIntegrity(CLOCATION);
	return rs;
}

void InconsistencyCoverage::CopyCoreArea(const TGridCoords & sectorDelta, const InconsistencyCoverage & remote)
{
	ASSERT__(!sealed);
	VerifyIntegrity(CLOCATION);
	offset = TGridCoords();

	Volume<int>	finalVolume = Volume<int>(TGridCoords(0),TGridCoords(Resolution));
	for (index_t d = 0; d < Dimensions; d++)
		if (sectorDelta.v[d] < 0)
			finalVolume.axis[d].max = 1;
		elif (sectorDelta.v[d] > 0)
			finalVolume.axis[d].min = Resolution-1;

	offset = finalVolume.min();
	const auto ext = finalVolume.GetExtent();

	#ifdef D3
		grid.SetSize(ext.x,ext.y,ext.z);
	#else
		grid.SetSize(ext.x,ext.y);
	#endif
	highest = 0;
	TGridCoords pixelDelta = 					//(0,0)
					- sectorDelta * Resolution	//(1,0)*Resolution
					- remote.offset + offset;			//(-1,-1)
	//=>pixelDelta = (-Resolution,0)+(1,1) = (-Resolution+1,1)
	
	#ifdef D3
		for (UINT32 z = 0; z < grid.GetDepth(); z++)
	#endif
	for (UINT32 y = 0; y < grid.GetHeight(); y++)
		for (UINT32 x = 0; x < grid.GetWidth(); x++)
		{
			#ifdef D3
				TGridCoords at2 = TGridCoords(x,y,z) + pixelDelta;	//assume (ic.width()-1,0)=(Resolution-1,0)
			#else
				TGridCoords at2 = TGridCoords(x,y) + pixelDelta;	//assume (ic.width()-1,0)=(Resolution-1,0)
			#endif
			//=> at2 = (-Resolution+1+Resolution-1, 1+0)=(0,1)
			const TExtSample*v2 = GetVerified(remote.grid,at2);
			#ifdef D3
				TExtSample&v = grid.Get(x,y,z);
			#else
				TExtSample&v = grid.Get(x,y);
			#endif
			if (!v2)
				v = TExtSample();
			else
			{
				highest = vmax(highest,v2->depth);
				v = *v2;
			}
		}
	VerifyIntegrity(CLOCATION);
}


void		InconsistencyCoverage::FlagInconsistent(const TEntityCoords&coords)
{
	ASSERT__(!sealed);
	static const TEntityCoords zero,one(1);
	if (Vec::oneLess(coords,zero) || Vec::oneGreater(coords,one))
		return;
	TGridCoords c;
	Vec::mult(coords,Resolution,c);
	Vec::clamp(c,0,Resolution-1);
	static const constexpr TSample Max = {0,0xFE};
	TSample&s = (*GetVerified(grid,c));
	s = Max;	//force nullpointer exception if out of range
	highest = Max.depth;
}

bool		InconsistencyCoverage::IsInconsistent(const TEntityCoords&coords) const
{
	static const TEntityCoords zero,one(1);
	if (Vec::oneLess(coords,zero) || Vec::oneGreater(coords,one))
		return false;
	TGridCoords c;
	Vec::mult(coords,Resolution,c);
	Vec::clamp(c,0,Resolution-1);
	return (*GetVerified(grid,c)).depth != 0;	//force nullpointer exception if out of range
}

InconsistencyCoverage::content_t		InconsistencyCoverage::GetInconsistency(const TEntityCoords&coords) const
{
	static const TEntityCoords zero,one(1);
	if (Vec::oneLess(coords,zero) || Vec::oneGreater(coords,one))
		return 0;
	TGridCoords c;
	Vec::mult(coords,Resolution,c);
	Vec::clamp(c,0,Resolution-1);
	return (*GetVerified(grid,c)).depth;	//force nullpointer exception if out of range
}

/*static*/ TGridCoords	InconsistencyCoverage::ToPixels(const TEntityCoords&coords)
{
	TGridCoords c;
	Vec::mult(coords,Resolution,c);
	Vec::clamp(c,0,Resolution-1);
	return c;
}

bool		InconsistencyCoverage::FindInconsistentPlacementCandidate(TEntityCoords&inOutCoords, float maxDist/*=1.f*/) const
{
	TGridCoords c= ToPixels(inOutCoords);

	const int maxDist2 = sqr(maxDist * Resolution);
	int d2 = maxDist2;
	TGridCoords closest;
	bool any = false;
	#ifdef D3
		for (UINT32 z = 0; z < this->grid.GetDepth(); z++)
		for (UINT32 y = 0; y < this->grid.GetHeight(); y++)
		for (UINT32 x = 0; x < this->grid.GetWidth(); x++)
		{
			if (grid.Get(x,y,z).IsConsistent())
				continue;
			int dist = sqr(x-c.x)+sqr(y-c.y)+sqr(z-c.z);
			if (dist >= d2)
				continue;
			d2 =dist;
			any = true;
			closest = TGridCoords(x,y,z);
		}

	#else

	for (UINT32 y = 0; y < this->grid.GetHeight(); y++)
		for (UINT32 x = 0; x < this->grid.GetWidth(); x++)
		{
			if (grid.Get(x,y).IsConsistent())
				continue;
			int dist = sqr(x-c.x)+sqr(y-c.y);
			if (dist >= d2)
				continue;
			d2 =dist;
			any = true;
			closest = TGridCoords(x,y);
		}
	#endif
	if (!any)
		return false;
	inOutCoords = (TEntityCoords(closest)+0.5f)/Resolution;
	return true;

}


InconsistencyCoverage::content_t		InconsistencyCoverage::GetPixelInconsistency(const TGridCoords&coords) const
{
	return GetSample(coords).depth;
}

const InconsistencyCoverage::TExtSample&		InconsistencyCoverage::GetSample(TGridCoords coords) const
{
	static const TExtSample empty;
	//ASSERT__(empty.blurExtent == TExtSample().blurExtent);
	//ASSERT__(empty.depth == TExtSample().depth);
	static const TGridCoords zero,one(1);
	if (Vec::oneLess(coords,zero))
		return empty;
	coords -= offset;
	if ((index_t)coords.x >= grid.GetWidth() || (index_t)coords.y >= grid.GetHeight())
		return empty;
	#ifdef D3
		if ((index_t)coords.z >= grid.GetDepth())
			return empty;
		return grid.Get(coords.x,coords.y,coords.z);
	#else
		return grid.Get(coords.x,coords.y);
	#endif
}


void		InconsistencyCoverage::VerifyIsInconsistent(const TEntityCoords&coords, const TVerificationContext&context) const
{
	static const TEntityCoords zero,one(1);
	if (Vec::oneLess(coords,zero) || Vec::oneGreater(coords,one))
		FATAL__("coordinate remainder is out of bounds "+coords.ToString()+" ("+context.ToString()+")");
	TGridCoords c;
	Vec::mult(coords,Resolution,c);
	Vec::clamp(c,0,Resolution-1);
	if (GetVerified(grid,c)->depth == 0)
		FATAL__("No inconsistency at pixel coordinates "+c.ToString()+" "+coords.ToString()+" ("+context.ToString()+")");
}

void		InconsistencyCoverage::VerifyIntegrity(const TCodeLocation&loc) const
{
	content_t highest = 0;
	foreach (grid,c)
	{
		const TSample&px = *c;

		if (px.IsInvalid())
			Except::fatal(loc, String(__func__)+": Invalid sample: "+String(px.depth)+"|"+String(px.blurExtent));

		highest = vmax(highest,px.depth);
		if (px.depth == 0 && px.blurExtent != 0xFE)
			Except::fatal(loc, String(__func__)+": Unexpected px.depth/px.blurExtent: "+String(px.depth)+"|"+String(px.blurExtent));
	}	
	if (highest != this->highest)
		Except::fatal(loc, String(__func__) + ": Expected highest (" + String(highest)+") == this->highest ("+String(this->highest)+")");
}


String	InconsistencyCoverage::TVerificationContext::ToString()	const
{
	String rs = ::ToString(id) + " @"+coordinates.ToString();
	if (shardCoordinates != TGridCoords(-1))
	{
		rs += " on shard "+shardCoordinates.ToString();
		if (sdsIndex != InvalidIndex)
		{
			rs += " "+String(sdsIndex)+"/("+String(sdsCount)+"-1)";
		}
	}
	if (layer != InvalidIndex)
		rs +=", layer "+String(layer);
	rs +=", gen "+String(generation);
	rs += approximate ? " (approx)": " (correct)";

	return rs;
}

count_t		InconsistencyCoverage::CountInconsistentSamples() const
{
	if (IsFullyConsistent())
		return 0;
	count_t cnt = 0;
	foreach (this->grid,s)
		if (!s->IsConsistent())
			cnt++;
	return cnt;
}


InconsistencyCoverage::TBadness	InconsistencyCoverage::GetTotalBadness() const
{
	TBadness rs;
	foreach (this->grid,s)
		if (!s->IsConsistent())
		{
			rs.inconsistentSamples++;
			rs.total += (MaxInconsistency+1) + s->depth;
		}
	return rs;
}

/*static*/ bool	ExtHGrid::ExtMatch(const ExtHGrid&a, const ExtHGrid&b,  const TGridCoords&cellCoords)
{
	for (int x = cellCoords.x -1; x <= cellCoords.x+1; x++)
		for (int y = cellCoords.y -1; y <= cellCoords.y+1; y++)
			#ifdef D3
				for (int z = cellCoords.z -1; z <= cellCoords.z+1; z++)
			#endif
			{
				const TGridCoords coords = 
					#ifdef D3
						TGridCoords(x,y,z);
					#else
						TGridCoords(x,y);
					#endif
				const auto&as = a.GetSample(coords).history;
				const auto&bs = b.GetSample(coords).history;
				if (as != bs)
					return false;
			}
	return true;
}

/*static*/ bool	ExtHGrid::CoreMatch(const ExtHGrid&a, const ExtHGrid&b,  const TGridCoords&cellCoords)
{
	const auto&as = a.core.GetCellOfL(cellCoords).history;
	const auto&bs = b.core.GetCellOfL(cellCoords).history;
	return as == bs;
}

const HGrid::TCell&		ExtHGrid::GetSample(const TGridCoords&coords) const
{
	TGridCoords nVector;
	if (coords.x < 0)
		nVector.x = -1;
	elif (coords.x >= IC::Resolution)
		nVector.x = 1;

	if (coords.y < 0)
		nVector.y = -1;
	elif (coords.y >= IC::Resolution)
		nVector.y = 1;

	#ifdef D3
		if (coords.z < 0)
			nVector.z = -1;
		elif (coords.z >= IC::Resolution)
			nVector.z = 1;
	#endif


	if (Vec::zero(nVector))
	{
		return core.GetCellOfL(coords);
	}


	TGridCoords c2 = coords;	//factors along each axis
	if (nVector.x != 0)
		c2.x = 0;
	if (nVector.y != 0)
		c2.y = 0;
	#ifdef D3
		if (nVector.z != 0)
			c2.z = 0;
	#endif

	const index_t linear = Shard::NeighborToLinear(nVector);
	return edge[linear].GetCellOfL(c2);
}
