#pragma once

#include <image/image.h>
#include "types.h"
#include <math/vclasses.h>

class InconsistencyCoverage
{
public:
	typedef BYTE	content_t;
	static const constexpr content_t MaxInconsistency = 0xff;

	class Comparator;

	struct TSample
	{
		/**
		Captures the step-distance from the closest actual inconsistency source. Initialized to the maximum possible value.
		Set to 0 when depth is set from some external source.
		Must be 0xFE if depth is 0
		*/
		content_t	blurExtent=0xFE;
		/**
		Divergence depth of this sample. Incremented when not zero during each growth phase.
		Zero indicates that this sample is fully consistent
		*/
		content_t	depth = 0;

		constexpr	TSample(){}
		constexpr	TSample(content_t blur, content_t depth):blurExtent(blur),depth(depth)	{}
		constexpr bool		operator==(const TSample&other)	const {return blurExtent == other.blurExtent && depth == other.depth;}
		constexpr bool		operator!=(const TSample&other)	const {return blurExtent != other.blurExtent || depth != other.depth;}

		bool		IsConsistent() const {return depth == 0;}
		bool		IsInvalid() const { return blurExtent == 0xFF; }

		void		IncreaseDepth();
		void		SetWorst(const TSample&a, const TSample&b);
		TSample&	IntegrateGrowingNeighbor(const TSample&n, UINT32 distance);
		void		SetBest(const TSample&a, const TSample&b, const Comparator&comp);
	};

private:
	//bool		isFullyConsistent=true;
	content_t	highest=0;
	bool		sealed = false;

	typedef GridArray<TSample,POD>	TGrid;

	TGrid	grid;

public:
	static const UINT32		Resolution = 8;


	TGridCoords		offset;	//!< Offset to the bottom left corner of the local shard space in pixels (can be negative)


	class Comparator
	{
	public:
		/**
		Compares s0 and s1.
		@return -1 if s0 is less bad than s1, +1 if s0 is worse than s1, and 0 if both are equal
		*/
		virtual int	operator()(const TSample&s0, const TSample&s1) const = 0;
		virtual const char* GetName() const = 0;

		static int		Compare(int b0, int b1)
		{
			if (b0 < b1)
				return -1;
			if (b0 > b1)
				return 1;
			return 0;
		}
	};

	class OrthographicComparator : public Comparator
	{
	public:
		virtual int	operator()(const TSample&s0, const TSample&s1) const override;
		virtual const char* GetName() const override {return "Orthographic (depth>extent)";}
	};

	class ReverseOrthographicComparator : public Comparator
	{
	public:
		virtual int	operator()(const TSample&s0, const TSample&s1) const override;
		virtual const char* GetName() const override {return "Orthographic (extent>depth)";}
	};

	class DepthComparator : public Comparator
	{
	public:
		virtual int	operator()(const TSample&s0, const TSample&s1) const override
		{
			return Compare(s0.depth,s1.depth);
		}
		virtual const char* GetName() const override {return "Depth";}
	};

	class ExtentComparator : public Comparator
	{
	public:
		virtual int	operator()(const TSample&s0, const TSample&s1) const override
		{
			return Compare(s1.blurExtent, s0.blurExtent);	//must be flipped
		}
		virtual const char* GetName() const override {return "Extent";}
	};


	class BinaryComparator : public Comparator
	{
	public:
		virtual int	operator()(const TSample&s0, const TSample&s1) const override
		{
			const int b0 = s0.IsConsistent() ? 0 : 1;
			const int b1 = s1.IsConsistent() ? 0 : 1;
			return Compare(b0,b1);
		}
		virtual const char* GetName() const override {return "Binary";}
	};


	

	/**/		InconsistencyCoverage()	{}
	/**/		InconsistencyCoverage(const InconsistencyCoverage&) = delete;
	void		operator=(const InconsistencyCoverage&other)
	{
		ASSERT__(!sealed);
		highest = other.highest;
		offset = other.offset;
		grid = other.grid;
	}

	/**
	Allocates the full shard default size
	*/
	void		AllocateDefaultSize()
	{
		ASSERT__(!sealed);
		offset = TGridCoords();
		#ifdef D3
			grid.SetSize(Resolution,Resolution,Resolution);
		#else
			grid.SetSize(Resolution,Resolution);
		#endif
		FlushInconsistency();
	}

	void		swap(InconsistencyCoverage&other)
	{
		ASSERT__(!sealed);
		ASSERT__(!other.sealed);
		swp(offset,other.offset);
		swp(highest,other.highest);
		grid.swap(other.grid);
		//swp(sealed,other.sealed);
	}



	const TGrid&	GetGrid() const {return grid;}

	/**
	Expands inconsistency by Entity::MaxOperationalRadius, and puts the result in @a rs.
	The resulting ic image may be larger with a diverging offset.
	Any existing ic in @a rs is wiped
	*/
	void		Grow(InconsistencyCoverage&rs)	const;
	/**
	Sets the local ic to the union of the local and @a remote ic.
	Areas outside the local ic image are ignored
	@return true, if any inconsistency was imported
	*/
	bool		Include(const TGridCoords&sectorDelta, const InconsistencyCoverage&remote);

	/**
	Resizes the local area to at most Resolution*Resolution and copies all values from the specified remote ic map
	*/
	void		CopyCoreArea(const TGridCoords&sectorDelta, const InconsistencyCoverage&remote);

	/**
	Fills all allocated pixels with the minimum inconsistent value
	*/
	void		FillMinInconsistent();

	void		VerifyIntegrity(const TCodeLocation&) const;

	/**
	Sets all covered elements to fully consistent
	*/
	void		FlushInconsistency()
	{
		ASSERT__(!sealed);
		grid.Fill(TSample());
		highest = 0;
	}

	bool		IsSealed() const {return sealed;}
	void		Seal() {sealed = true;}
	bool		IsSealedAndConsistent() const {return sealed && IsFullyConsistent();}


	struct TVerificationContext
	{
		GUID			id;
		bool			approximate=true;
		index_t			layer = InvalidIndex,
						generation = InvalidIndex,
						sdsIndex = InvalidIndex,
						sdsCount = InvalidIndex;
		TEntityCoords	coordinates;
		TGridCoords		shardCoordinates=TGridCoords(-1);



		String			ToString()	const;
	};


	/**
	Flags the ic cell corresponding to the given coordinates as inconsistent.
	@param coords Entity coordinates to flag inconsistent. Must be in the range [0,0]-(1,1)
	*/
	void		FlagInconsistent(const TEntityCoords&coords);

	bool		IsInconsistent(const TEntityCoords&coords) const;
	content_t	GetInconsistency(const TEntityCoords&coords) const;
	content_t	GetPixelInconsistency(const TGridCoords&) const;
	const TSample& GetSample(TGridCoords) const;
	void		VerifyIsInconsistent(const TEntityCoords&coords, const TVerificationContext&context) const;
	static TGridCoords	ToPixels(const TEntityCoords&coords);

	bool		IsFullyConsistent() const {return highest == 0;}
	content_t	GetHighestInconsistency() const {return highest;}

	count_t		CountInconsistentSamples() const;

	/**
	Attempts to find a suitable inconsistent location as close to the specified coords as possible
	@param[inout] inOutCoords Relative entity coordinates in [0,1]
	@param maxDist Maximum distance to look, using eucledian norm
	@return true if an inconsistent location was found, and @a inOutCoords have been updated, false otherwise
	*/
	bool		FindInconsistentPlacementCandidate(TEntityCoords&inOutCoords, float maxDist=2.f) const;
	

	struct TBadness
	{
		count_t	inconsistentSamples=0,
				total=0;	//Sum of (MaxInconsistency+1)+depth of each inconsistent sample
	};
	TBadness	GetTotalBadness() const;

	void		LoadMinimum(const InconsistencyCoverage&a, const InconsistencyCoverage&b, const Comparator&);

	bool		operator==(const InconsistencyCoverage&other) const;
	bool		operator!=(const InconsistencyCoverage&other) const	{return !operator==(other);}
};

