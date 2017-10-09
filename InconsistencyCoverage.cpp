#include <global_root.h>
#include "InconsistencyCoverage.h"
#include "entity.h"

#include <container/array2d.h>
#include <container/array3d.h>
#include "EntityStorage.h"
#include "shard.h"


bool	IC::RangeArray::OverlapsWith(const RangeArray&other, count_t tolerance) const
{
	const count_t cnt = M::Min(container.Count(),other.container.Count());
	for (index_t i = 0; i < cnt; i++)
	{
		const auto&a = container[i];
		const auto&b = other.container[i];
		if (a.Intersects(b))
		{
			const generation_t	min = M::Max(a.min,b.min),
								max = M::Min(a.max,b.max);
			
			const generation_t delta = max-min+1;
			if (max >= min && (delta > tolerance))
				return true;
		}
	}
	return false;
}

void	IC::RangeArray::Include(index_t shardIndex, generation_t generation)
{
	if (shardIndex >= container.Count())
		container.ResizePreserveContent(shardIndex+1,Math::MaxInvalidRange<generation_t>());

	container[shardIndex].Include(generation);

	allUndefined = false;
}

void	IC::RangeArray::Merge(const RangeArray&other)
{
	if (other.container.Count() > container.Count())
		container.ResizePreserveContent(other.container.Count(),Math::MaxInvalidRange<generation_t>());
	for (index_t i = 0; i < container.Count() && i < other.container.Count(); i++)
		container[i].Include(other.container[i]);
	if (!other.allUndefined)
		allUndefined = false;
}

void	IC::RangeArray::ClearAndMerge(const RangeArray&a, const RangeArray&b)
{
	container = a.container;
	allUndefined = a.allUndefined;
	Merge(b);
}

void HashProcessGrid::SetupEmpty(index_t timestep)
{
	static const auto Resolution = HGrid::Resolution;
	grid.SetSize(TGrid::Size(Resolution));

	Concurrency::parallel_for(index_t(0),grid.Count(),[this,timestep](index_t i)
	{
		grid[i].prev = Hasher::HashContainer::Empty;
		grid[i].hasher.AppendPOD(timestep);
	});
}

HashProcessGrid::HashProcessGrid(const HGrid&h,index_t timestep,const TGridCoords&localOffset, bool usePrev):localOffset(localOffset)
{
	if (!RegardHistory)
	{
		SetupEmpty(timestep);
		return;
	}

	ASSERT__(h.grid.IsNotEmpty());
	grid.SetSize(h.grid.GetSize());

	Concurrency::parallel_for(index_t(0),grid.Count(),[this,&h,timestep,usePrev](index_t i)
	{
		auto&out = grid[i];
		const auto&in = h.grid[i];
		out.prev = usePrev ? in.prev: in.next;
		out.hasher.AppendPOD(out.prev);
		out.hasher.AppendPOD(timestep);
	});
}

HashProcessGrid::HashProcessGrid(index_t timestep,const TGridCoords&localOffset):localOffset(localOffset)
{
	SetupEmpty(timestep);
}

void		HashProcessGrid::Finish(HGrid&h)
{
	ASSERT__(grid.IsNotEmpty());
	h.grid.SetSize(grid.GetSize());
	Concurrency::parallel_for(index_t(0),grid.Count(),[this,&h](index_t i)
	{
		auto&in = grid[i];
		auto&out = h.grid[i];
		out.prev = in.prev;
		in.hasher.Finish(out.next);
		out.originShard = this->localOffset;
		out.originCell = grid.ToVectorIndex(i);
		out.numEntities = in.numEntities;
	});

}

void		HashProcessGrid::Include(const EntityStorage&store)
{
	foreach (store, e)
	{
		auto&cell = GetCellOfW(e->coordinates);
		e->Hash(cell.hasher);
		cell.numEntities++;
	}
}

HashProcessGrid::Cell&		HashProcessGrid::GetCellOfW(const TEntityCoords&worldCoords)
{
	return GetCellOfL(TEntityCoords(worldCoords - localOffset));
}

HashProcessGrid::Cell&		HashProcessGrid::GetCellOfL(const TEntityCoords&localCoords)
{
	TGridCoords c = localCoords * HGrid::Resolution;
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


/*static*/ HGrid::TCell	HGrid::EmptyCell(const TGridCoords&shardCoords,const GridIndex&cellIndex)
{
	HGrid::TCell rs;
	rs.next = Hasher::HashContainer::Empty;
	rs.prev = Hasher::HashContainer::Empty;
	rs.originCell = cellIndex;
	rs.originShard = shardCoords;
	return rs;
}


void		HGrid::ExportEdge(HGrid&target, const TGridCoords&edgeDelta) const
{
	const Volume<int> local = Volume<int>(TGridCoords(0),TGridCoords(Resolution));
	Volume<int> vol = local;
	vol.Translate(edgeDelta * (Resolution -1));
	vol.ConstrainBy(local);


	target.grid.ResizeToAndCopyBlock(grid,VectorToIndex(vol.min()),VectorToSize(vol.GetExtent()));

}

GridIndex			HGrid::GetIndexOfW(const TEntityCoords&worldCoords, const TGridCoords&localOffset) const
{
	return GetIndexOfL(TEntityCoords(worldCoords - localOffset));
}

GridIndex			HGrid::GetIndexOfL(const TEntityCoords&localCoords) const
{
	TGridCoords c = localCoords * Resolution;
	Vec::clamp(c,0,Resolution-1);
	return VectorToIndex(c);
}


const HGrid::TCell&		HGrid::GetCellOfW(const TEntityCoords&worldCoords, const TGridCoords&localOffset) const
{
	return GetCellOfL(TEntityCoords(worldCoords - localOffset));
}

const HGrid::TCell&		HGrid::GetCellOfL(const TEntityCoords&localCoords) const
{
	TGridCoords c = localCoords * Resolution;
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
	if (s0.spatialDistance > s1.spatialDistance)
		return -1;	//larger extent means probably not as bad
	if (s0.spatialDistance < s1.spatialDistance)
		return 1;
	return 0;
}

/*virtual override*/ int	InconsistencyCoverage::ReverseOrthographicComparator::operator()(const TSample&s0, const TSample&s1) const
{
	int rs = Compare(s0.depth != 0, s1.depth != 0);
	if (rs || s0.depth == 0)
		return rs;
	if (s0.spatialDistance > s1.spatialDistance)
		return -1;	//larger extent means probably not as bad
	if (s0.spatialDistance < s1.spatialDistance)
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
	if (a.spatialDistance == 0xFF || b.spatialDistance == 0xFF)
		spatialDistance = 0xFF;
	else
		spatialDistance = M::Min(a.spatialDistance,b.spatialDistance);
	depth = M::Max(a.depth,b.depth);
}

void	InconsistencyCoverage::TSample::Include(const TSample&other)
{
	if (spatialDistance == 0xFF || other.spatialDistance == 0xFF)
		spatialDistance = 0xFF;
	else
		spatialDistance = M::Min(spatialDistance,other.spatialDistance);
	depth = M::Max(depth,other.depth);
}


bool	InconsistencyCoverage::TSample::IntegrateGrowingNeighbor(const TSample&n, UINT32 distance)
{
	if (n.depth <= distance)
		return false;
	//if (spatialDistance != 0xFF && distance != 0)	//else: copy from self
	//{
	//	DBG_ASSERT_LESS_OR_EQUAL__(n.depth-distance, depth);
	//	DBG_ASSERT_GREATER__(n.depth, distance);
	//	//return *this;
	//}
	//content_t oldBlur = spatialDistance;
	content_t nDepth = n.depth - distance;
	if (spatialDistance != 0xFF)
	{
		if (n.spatialDistance == 0xFF)
			spatialDistance = 0xFF;
		else
		{
			const content_t old = spatialDistance;
			const UINT32 nExt = (UINT32)n.spatialDistance + distance;
			spatialDistance = (content_t)M::Min(MaxDistance, M::Min<UINT32>(spatialDistance, nExt));
			//if (spatialDistance < old && old != 0xFE && nDepth <= depth)
			//	spatialDistance = 0xFF;
		}
	}

	if (nDepth > depth)
	{
		depth = nDepth;
	}
	//ASSERT__(!IsInvalid());
	return true;
}

InconsistencyCoverage::TExtSample&	InconsistencyCoverage::TExtSample::IntegrateGrowingNeighbor(const TExtSample&n, UINT32 distance)
{
	if (Super::IntegrateGrowingNeighbor(n,distance))
	{
		ASSERT__(distance > 0);
		ASSERT__(n.spatialDistance + distance >= spatialDistance);
		unavailableShards |= n.unavailableShards;

		precise.Merge(n.precise);
		fuzzy.Merge(n.fuzzy);

		ASSERT_EQUAL__(unavailableShards.AllZero(),IsConsistent());
		ASSERT_EQUAL__(precise.AllUndefined(),IsConsistent());
		ASSERT_EQUAL__(fuzzy.AllUndefined(),IsConsistent());
	}
	return *this;
}


void		InconsistencyCoverage::TSample::SetBest(const TSample&a, const TSample&b, const Comparator&comp)
{
	int c = comp(a,b);
	if (c <= 0)
		*this = a;
	else
		*this = b;
}

void		InconsistencyCoverage::TExtSample::SetBest(const TExtSample&a, const TExtSample&b, const Comparator&comp)
{
	int c = comp(a,b);
	if (c <= 0)
		*this = a;
	else
		*this = b;
}


void		InconsistencyCoverage::LoadMinimum(const InconsistencyCoverage&a, const InconsistencyCoverage&b, const Comparator&comp)
{
	ASSERT__(!sealed);
	
	offset.x = M::Max(offset.x,a.offset.x);
	offset.y = M::Max(offset.y,a.offset.y);
	#ifdef D3
		offset.z = M::Max(offset.z,a.offset.z);
	#endif

	grid.SetSize(
						M::Min(a.grid.GetWidth(),b.grid.GetWidth()),
						M::Min(a.grid.GetHeight(),b.grid.GetHeight())
						#ifdef D3
							,M::Min(a.grid.GetDepth(),b.grid.GetDepth())
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


					const TExtSample&va = a.GetSample(c);
					const TExtSample&vb = b.GetSample(c);
					TExtSample&merged = grid.Get(x,y,z);
					merged.SetBest(va,vb,comp);
					highest = M::Max(highest,merged.depth);
				}
	#else

		foreach (grid.Horizontal(),x)
			foreach (grid.Vertical(),y)
			{
				TGridCoords c((int)*x + offset.x,(int)*y + offset.y);

				const TExtSample&va = a.GetSample(c);
				const TExtSample&vb = b.GetSample(c);
				TExtSample&merged = grid.Get(x,y);
				merged.SetBest(va,vb,comp);
				highest = M::Max(highest,merged.depth);
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
	sample.spatialDistance = 0;
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
					const TExtSample&v = grid.Get(x, y, z);
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
					TExtSample v = grid.Get(x,y,z);
					if (!v.IsConsistent())
						v.IncreaseDepth();
					if (v.depth < 2)
						continue;
					for (UINT32 i = 0; i <= expandBy; i++)
						for (UINT32 j = 0; j <= expandBy; j++)
							for (UINT32 k = 0; k <= expandBy; k++)
							if (i || j || k)
							{
								UINT32 dist = M::Max(k,M::Max(i, j));
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
				TExtSample v = grid.Get(x,y);
				if (!v.IsConsistent())
					v.IncreaseDepth();
				if (v.depth < 2)
					continue;
				for (UINT32 i = 0; i <= expandBy; i++)
					for (UINT32 j = 0; j <= expandBy; j++)
						if (i || j)
						{
							UINT32 dist = M::Max(i, j);
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
static const InconsistencyCoverage::TExtSample*	GetVerified(const Array2D<InconsistencyCoverage::TExtSample,S>&array, const M::VecN<int,2>&c)
{
	if (c.x < 0 || c.y < 0)
		return nullptr;
	if ((index_t)c.x >= array.GetWidth() || (index_t)c.y >= array.GetHeight())
		return nullptr;
	return &array.Get(c.x,c.y);
}

template <class S>
static InconsistencyCoverage::TExtSample*	GetVerified(Array2D<InconsistencyCoverage::TExtSample,S>&array, const M::VecN<int,2>&c)
{
	if (c.x < 0 || c.y < 0)
		return nullptr;
	if ((index_t)c.x >= array.GetWidth() || (index_t)c.y >= array.GetHeight())
		return nullptr;
	return &array.Get(c.x,c.y);
}


template <class S>
static const InconsistencyCoverage::TExtSample*	GetVerified(const Array3D<InconsistencyCoverage::TExtSample,S>&array, const M::VecN<int,3>&c)
{
	if (c.x < 0 || c.y < 0 || c.z < 0)
		return nullptr;
	if ((index_t)c.x >= array.GetWidth() || (index_t)c.y >= array.GetHeight() || (index_t)c.z >= array.GetDepth())
		return nullptr;
	return &array.Get(c.x,c.y,c.z);
}

template <class S>
static InconsistencyCoverage::TExtSample*	GetVerified(Array3D<InconsistencyCoverage::TExtSample,S>&array, const M::VecN<int,3>&c)
{
	if (c.x < 0 || c.y < 0 || c.z < 0)
		return nullptr;
	if ((index_t)c.x >= array.GetWidth() || (index_t)c.y >= array.GetHeight() || (index_t)c.z >= array.GetDepth())
		return nullptr;
	return &array.Get(c.x,c.y,c.z);
}

void InconsistencyCoverage::IncludeMissing(const TGridCoords&sectorDelta, const GridIndex&shardIndex, const GridSize&shardGridSize, generation_t generation)
{
	ASSERT__(!sealed);
	VerifyIntegrity(CLOCATION);
	TGridCoords pixelDelta = offset - sectorDelta * Resolution - (-1);

	TExtSample v2;
	v2.depth = 1;
	v2.spatialDistance = 0;
	//v2.unavailableShards.SetSize(numShards);
	{
		const index_t linear = shardGridSize.ToLinearIndex(shardIndex);
		v2.unavailableShards.SetBit(linear,true);
		v2.precise.Include(linear,generation);

		shardIndex.IterateNeighborhood([&v2,shardGridSize,generation](const GridIndex&idx)
		{
			v2.fuzzy.Include(shardGridSize.ToLinearIndex(idx),generation);
		},shardGridSize);
	}


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
				TExtSample&v = grid.Get(x,y,z);
			#else
				TExtSample&v = grid.Get(x,y);
			#endif
			v.Include(v2);
			highest = M::Max(highest,v.depth);
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
				TExtSample&v = grid.Get(x,y,z);
			#else
				TExtSample&v = grid.Get(x,y);
			#endif
			v.Include(*v2);
			//v.SetWorst(v,*v2);
			highest = M::Max(highest,v.depth);
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
				highest = M::Max(highest,v2->depth);
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
	//static const constexpr TSample Max = {0,0xFE};
	TExtSample&s = (*GetVerified(grid,c));
	s.depth = MaxDepth;
	s.spatialDistance = 0;
	highest = M::Max(highest,s.depth);
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
	ASSERT__(!Vec::oneLess(c,TGridCoords(0)));
	ASSERT__(!Vec::oneGreater(c,TGridCoords(Resolution)));
	Vec::clamp(c,0,Resolution-1);
	return c;
}

bool		InconsistencyCoverage::FindInconsistentPlacementCandidate(TEntityCoords&inOutCoords, float maxDist/*=1.f*/) const
{
	TGridCoords c= ToPixels(inOutCoords);

	const int maxDist2 = M::Sqr(maxDist * Resolution);
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
			int dist = M::Sqr(x-c.x)+M::Sqr(y-c.y)+M::Sqr(z-c.z);
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
			int dist = M::Sqr(x-c.x)+M::Sqr(y-c.y);
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
	//ASSERT__(empty.spatialDistance == TExtSample().spatialDistance);
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
		const TExtSample&px = *c;

		const bool zero = px.unavailableShards.AllZero();
		ASSERT_EQUAL__(zero,px.IsConsistent());


		if (px.IsInvalid())
			Except::TriggerFatal(loc, String(__func__)+": Invalid sample: "+String(px.depth)+"|"+String(px.spatialDistance));

		highest = M::Max(highest,px.depth);
		if (px.depth == 0 && px.spatialDistance != MaxDistance)
			Except::TriggerFatal(loc, String(__func__)+": Unexpected px.depth/px.spatialDistance: "+String(px.depth)+"|"+String(px.spatialDistance));
	}	
	if (highest != this->highest)
		Except::TriggerFatal(loc, String(__func__) + ": Expected highest (" + String(highest)+") == this->highest ("+String(this->highest)+")");
}

void		InconsistencyCoverage::SetGrid(TGrid&&grid)
{
	ASSERT__(!sealed);
	this->grid = std::move(grid);
	highest = 0;
	foreach (this->grid,c)
		highest = Math::Max(highest,c->depth);
	VerifyIntegrity(CLOCATION);
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
			rs.total += (MaxDepth+1) + s->depth;
		}
	return rs;
}

/*static*/ bool	ExtHGrid::ExtMatch(const ExtHGrid&a, const ExtHGrid&b,  const TGridCoords&cellCoords,MissingEdgeTreatment t, count_t minEntityCount)
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
				const auto*as = a.GetSample(coords);
				const auto*bs = b.GetSample(coords);
				if (as == nullptr || bs == nullptr || as->next == Hasher::HashContainer::Empty || bs->next == Hasher::HashContainer::Empty)
				{
					if (t == MissingEdgeTreatment::Fail)
						return false;
					continue;
				}

				if (as->numEntities != bs->numEntities)
					return false;
				if (as->numEntities < minEntityCount)
					return false;
				if (as->next != bs->next)
					return false;
			}
	return true;
}


/*static*/ void	ExtHGrid::RequireExtMismatch(const ExtHGrid&a, const ExtHGrid&b,  const TGridCoords&cellCoords,MissingEdgeTreatment t, index_t expectedLinear)
{
	auto at = VectorToIndex(cellCoords);
	ASSERT_EQUAL1__(expectedLinear,a.core.grid.ToLinearIndex(at),at);
	ASSERT_EQUAL__(a.core.grid.GetSize(),GridSize(IC::Resolution));
	ASSERT_EQUAL__(b.core.grid.GetSize(),GridSize(IC::Resolution));
	count_t sampleMatches = 0;
	count_t ignored = 0;
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
				const auto*as = a.GetSample(coords);
				const auto*bs = b.GetSample(coords);
				if (as == nullptr || bs == nullptr || as->next == Hasher::HashContainer::Empty || bs->next == Hasher::HashContainer::Empty)
				{
					if (t == MissingEdgeTreatment::Fail)
						return;
					ignored++;
					continue;
				}

				if (as->next != bs->next)
					return;
				sampleMatches++;
			}
	FATAL__("Match with "+String(sampleMatches)+" matches, ignored="+String(ignored)+", at="+ToString(at)+" ("+String(expectedLinear)+")");
}

/*static*/ bool	ExtHGrid::CoreMatch(const ExtHGrid&a, const ExtHGrid&b,  const TGridCoords&cellCoords, count_t minEntityCount)
{
	const auto&as = a.core.GetCellOfL(cellCoords);
	const auto&bs = b.core.GetCellOfL(cellCoords);

	if (as.numEntities != bs.numEntities)
		return false;
	if (as.numEntities < minEntityCount)
		return false;

	return as.next == bs.next;
}

/*static*/ void	ExtHGrid::RequireCoreMismatch(const ExtHGrid&a, const ExtHGrid&b,  const TGridCoords&cellCoords, index_t expectedLinear)
{
	auto at = VectorToIndex(cellCoords);
	ASSERT_EQUAL1__(expectedLinear,a.core.grid.ToLinearIndex(at),at);
	const auto&as = a.core.GetCellOfL(cellCoords).next;
	const auto&bs = b.core.GetCellOfL(cellCoords).next;
	ASSERT_NOT_EQUAL1__( as , bs , at);
}

/*static*/ void	ExtHGrid::RequireCoreMatch(const ExtHGrid&a, const ExtHGrid&b,  const TGridCoords&cellCoords, index_t expectedLinear)
{
	auto at = VectorToIndex(cellCoords);
	ASSERT_EQUAL1__(expectedLinear,a.core.grid.ToLinearIndex(at),at);
	const auto&as = a.core.GetCellOfL(cellCoords).next;
	const auto&bs = b.core.GetCellOfL(cellCoords).next;
	ASSERT_EQUAL1__( as , bs , at);
}

const HGrid::TCell*		ExtHGrid::GetSample(const TGridCoords&coords) const
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
		return &core.GetCellOfL(coords);
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
	if (edge[linear].grid.IsEmpty())
		return nullptr;
	return &edge[linear].GetCellOfL(c2);
}
