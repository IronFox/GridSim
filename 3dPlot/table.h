#pragma once

#include <io/file_system.h>
#include <image/image.h>
#include <geometry/cgs.h>
#include <math/vclasses.h>
#include "sample.h"
#include <array>
#include <math/polynomial.h>

namespace D = DeltaWorks;
namespace M = DeltaWorks::Math;

struct TLinearFunction
{
	float	base = 0,
			inclination = 0.1f;

	float	operator()(float x) const
	{
		return base + inclination * x;
	}

	/**/	TLinearFunction(float base=0, float inclination=0.1f):base(base),inclination(inclination)	{}

	friend D::String	ToString(const TLinearFunction&f)
	{
		return D::String(f.base)+"+X*"+D::String(f.inclination);
	}

	void	Set(float x0, float y0, float x1, float y1)
	{
		if (x0 != x1)
		{
			inclination = (y1-y0)/(x1-x0);
			base = y0 - inclination*x0;
		}
		else
		{
			base = y0;
			inclination = 0;
		}

	}
};

class Table;

struct TAgeHypothesis
{
	static const count_t	TDegree = 5,TNCoef = TDegree+1;
	static const count_t	SDegree = 8,SNCoef = SDegree+1;

	D::M::Polynomial<double,TNCoef>		temporalProfile;
	D::M::Polynomial<double,SNCoef>		spatialProfile0,spatialProfile1;

	float					max = 1;
	count_t					maxSpatialDistance=0,
							temporalLinearBreak=0,	//t >= temporalLinearBreak : temporalLinearPart takes over
							spatialProfile0TAt=0,
							maxTemporalDistance=0;
	TLinearFunction			temporalLinearPart;



	bool					useLinearApproximation = true;

	D::M::float3			linearPlaneFunc;	//x,y: axial inclination, z: offset



	float					SpatialSample(float spatialDistance, float temporalDistance) const;
	float					TemporalSample(float temporalDistance) const;
	float					Sample(float spatialDistance, float temporalDistance) const;
	float					GetAverageSquaredError(const Table&rawData) const;

	//float4					ToVector() const {return float4(mean,max,xScale,curveExtent);}
	//void					LoadVector(const float4&v) {mean = v.x; max = v.y; xScale = v.z; curveExtent = v.w;}

};

struct TStaticAgeHypothesis
{
	typedef double PFloat;
	D::M::NegativePolynomial2D<PFloat,4,7>	pBase;
	D::M::NegativePolynomial2D<PFloat,4,7>	pX;
	float					Sample(float spatialDistance, float temporalDistance, float density, float relativeSensorRange) const;
};


struct THypothesisData
{
	static TStaticAgeHypothesis	staticAgeHypothesis;

	TLinearFunction			meanFunction = TLinearFunction(-1.5f,0.7f),
							deviationFunction = TLinearFunction(1.8f,0.03f);
	index_t					sourceTableID=InvalidIndex;
	TAgeHypothesis			age;
	bool					isEnabled=false, isStatic=false;
	float					sourceDensity,sourceRelativeSensorRange;

	/**/					THypothesisData(bool enable, bool isStatic):isEnabled(enable),isStatic(isStatic)	{}
	float					Sample(float spatialDistance, float temporalDistance, SampleType t) const;
	void					Train(index_t sourceTableID, SampleType t);

	static void				TrainRow(const Table&table, index_t rowIndex, D::M::Polynomial<double,TAgeHypothesis::SNCoef>&poly, bool smooth);
};

class Table
{
private:

	struct TSurfacePoint
	{
		float				height;
		D::M::float4		color;


		bool				operator>(const TSurfacePoint&other) const
		{
			return height > other.height;
		}
		bool				operator<(const TSurfacePoint&other) const
		{
			return height < other.height;
		}
	};

	struct TInclinedSurfacePoint : public TSurfacePoint
	{
		float				xInclination,yInclination;
		
		float				ThicknessToOffsetX(float t) const;
		float				ThicknessToOffsetY(float t) const;
	};

	struct TSurfaceSample : public TSurfacePoint
	{
		bool				cover;
	};



	void					UpdateImage(SampleType t);
	TSurfacePoint			GetGeometryPoint(SampleType t, index_t ix, index_t iy) const;
	TInclinedSurfacePoint	GetInclinedGeometryPoint(SampleType t, index_t ix, index_t iy) const;
	static TSurfacePoint	SampleRangePixelAt(SampleType t, index_t ix, index_t iy, index_t at);
	void					TraceLine(const D::M::TIntRange<UINT>&range, const std::function<D::M::float3(UINT)>&);

	static void				BuildPlotSurface(D::CGS::Constructor<>::Object&target, const std::function<TSurfaceSample(index_t,index_t)>&surfaceFunction);
	static void				BuildPlotHoleWalls(D::CGS::Constructor<>::Object&obj,const std::function<TInclinedSurfacePoint(index_t,index_t)>&surfaceFunction, UINT xExtent, UINT yExtent, float thickness = 0.03f);
	void					FinishConstruction(const D::CGS::Constructor<>::Object&constructor, const D::CGS::Constructor<>::Object&transparentConstructor, const D::CGS::Constructor<>::Object&holeConstructor);

public:
	struct TLineSegment
	{
		D::M::float4		color;
		float				width=2;
		D::Buffer0<D::M::TVec3<> >points;

		void				swap(TLineSegment&other)
		{
			D::swp(color,other.color);
			D::swp(width,other.width);
			points.swap(other.points);
		}
	};

	struct TMetrics
	{
		enum MotionProfile_t
		{
			Binary,
			Linear,
			Exotic
		};

		MotionProfile_t		motionProfile;
		bool				entityDeathAllowed = false,unaffectedRandomMotion=false;
		float				entityDensity=0,relativeMovementRange=0.5f;
		int					dimensions=1;
		count_t				spatialSupersampling=1,entitiesPerSimulation=0;
		UINT64				totalSimulationRuns=0;
	};

	struct TValueRange
	{
		D::M::TFloatRange<>	range;
		float				average;
	};


	D::Buffer0<TLineSegment,D::Swap>	lineSegments;
	D::Buffer0<D::Buffer0<TSample> >samples;
	D::String					sourceFile;
	TValueRange				currentRange;
	TMetrics				metrics;

	//THypothesisData			hypothesis;
	std::function<float(float spatialDistance, float temporalDistance, SampleType)>		heightFunction;
	std::function<D::M::TVec4<>(float spatialDistance, float temporalDistance, SampleType t)> colorFunction;
	//FloatImage				image;

	static const constexpr unsigned Resolution = 400;
	static const constexpr float step = 1.0f/Resolution;
	
	index_t					solidPlotGeometryHandle = InvalidIndex,
							plotHoleGeometryHandle = InvalidIndex,
							transparentGeometryHandle = InvalidIndex;
	D::M::float4			color = D::M::float4(1);

	const bool				IsRangeTable;
	
	/**/					Table(bool isRangeTable=false):IsRangeTable(isRangeTable)	{}

	void					swap(Table&other)
	{
		D::swp(metrics,other.metrics);
		//D::swp(hypothesis,other.hypothesis);
		heightFunction.swap(other.heightFunction);
		colorFunction.swap(other.colorFunction);
		D::swp(currentRange,other.currentRange);
		lineSegments.swap(other.lineSegments);
		samples.swap(other.samples); 
		sourceFile.swap(other.sourceFile); 
	}
	friend void				swap(Table&a, Table&b)	{a.swap(b);}

	const TSample&			GetSample(index_t iX, index_t iY)	const;
	float					Get(index_t spatialDistance, index_t temporalDistance, SampleType t)	const;
	float					GetSmoothed(float spatialDistance, float temporalDistance,SampleType t) const;

	bool					LoadSamples(const D::FileSystem::File&f, const std::function<bool(const TMetrics&)> mFilter=std::function<bool(const TMetrics&)>());
	void					UpdateCurrentRange(SampleType type);
	void					UpdatePlotGeometry(SampleType type, bool window);

	void					TrainHypothesis(index_t sourceTableID, SampleType t, bool isStatic);
	D::M::float3			TrainFlatOverestimation(index_t sourceTableID, SampleType t,float threshold);

	void					RemovePlotGeometry();

	void					RenderLines()	const;

	void					SetTransformed(const Table&source, const std::function<TSample(TSample)>&transformFunction, const D::M::float4&color);

	float					SmoothGeometryHeightFunction(SampleType t, float x, float y) const;
	float					FindLevelSpatialDistance(SampleType t, index_t temporalDistance, float level) const;
	float					FindNormalDeviation(SampleType t, index_t temporalDistance, float mean) const;
	float					GetSpatialNormalError(SampleType t, index_t temporalDistance, float mean, float deviation) const;
};



void RemoveAllPlotGeometries();
extern D::Buffer0<Table,D::Swap>	tables;

extern Table				rangeTable;


float Relative2TextureHeight(float h);

void LoadTables(const std::initializer_list<const D::FileSystem::File>&files);
void LoadTables(const D::Buffer0<D::FileSystem::File>&files);
void UpdateColors();
/**
Updates all table currentRange contents by calling t->UpdateCurrentRange(type)
*/
void UpdateTableRanges(SampleType type);


float			GetVerticalRange(SampleType t);
