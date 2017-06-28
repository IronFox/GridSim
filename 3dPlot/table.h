#pragma once

#include <io/file_system.h>
#include <image/image.h>
#include <geometry/cgs.h>
#include <math/vclasses.h>
#include "sample.h"
#include <array>
#include <math/polynomial.h>

struct TLinearFunction
{
	float	base = 0,
			inclination = 0.1f;

	float	operator()(float x) const
	{
		return base + inclination * x;
	}

	/**/	TLinearFunction(float base=0, float inclination=0.1f):base(base),inclination(inclination)	{}

	String	ToString() const {return String(base)+"+X*"+String(inclination);}

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

	Math::Polynomial<double,TNCoef>		temporalProfile;
	Math::Polynomial<double,SNCoef>		spatialProfile0,spatialProfile1;

	float					max = 1;
	count_t					maxSpatialDistance=0,
							temporalLinearBreak=0,	//t >= temporalLinearBreak : temporalLinearPart takes over
							spatialProfile0TAt=0,
							maxTemporalDistance=0;
	TLinearFunction			temporalLinearPart;



	bool					useLinearApproximation = true;

	float3					linearPlaneFunc;	//x,y: axial inclination, z: offset



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
	NegativePolynomial2D<PFloat,4,7>	pBase;
	NegativePolynomial2D<PFloat,4,7>	pX;
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

	static void				TrainRow(const Table&table, index_t rowIndex, Math::Polynomial<double,TAgeHypothesis::SNCoef>&poly, bool smooth);
};

class Table
{
private:

	struct TSurfacePoint
	{
		float				height;
		float4				color;


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
	void					TraceLine(const TIntRange<UINT>&range, const std::function<float3(UINT)>&);

	static void				BuildPlotSurface(CGS::Constructor<>::Object&target, const std::function<TSurfaceSample(index_t,index_t)>&surfaceFunction);
	static void				BuildPlotHoleWalls(CGS::Constructor<>::Object&obj,const std::function<TInclinedSurfacePoint(index_t,index_t)>&surfaceFunction, UINT xExtent, UINT yExtent, float thickness = 0.03f);
	void					FinishConstruction(const CGS::Constructor<>&constructor, const CGS::Constructor<>&transparentConstructor, const CGS::Constructor<>&holeConstructor);

public:
	struct TLineSegment
	{
		float4				color;
		float				width=2;
		Buffer0<TVec3<> >	points;

		void				swap(TLineSegment&other)
		{
			swp(color,other.color);
			swp(width,other.width);
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
		TFloatRange<>		range;
		float				average;
	};


	Buffer0<TLineSegment,Swap>	lineSegments;
	Buffer0<Buffer0<TSample> >	samples;
	String					sourceFile;
	TValueRange				currentRange;
	TMetrics				metrics;

	THypothesisData			hypothesis;
	//FloatImage				image;

	static const constexpr unsigned Resolution = 400;
	static const constexpr float step = 1.0f/Resolution;
	
	CGS::Geometry<>			solidPlotGeometry, plotHoleGeometry,transparentGeometry;
	bool					isEmbedded=false;
	float4					color = float4(1);

	const bool				IsRangeTable;
	
	/**/					Table(bool isRangeTable=false, bool isHypothesis=false, bool isStaticHypothesis=false):IsRangeTable(isRangeTable),hypothesis(isHypothesis,isStaticHypothesis)	{}

	void					swap(Table&other)
	{
		swp(metrics,other.metrics);
		swp(hypothesis,other.hypothesis);
		swp(currentRange,other.currentRange);
		lineSegments.swap(other.lineSegments);
		samples.swap(other.samples); 
		sourceFile.swap(other.sourceFile); 
	}
	friend void				swap(Table&a, Table&b)	{a.swap(b);}

	const TSample&			GetSample(index_t iX, index_t iY)	const;
	float					Get(index_t spatialDistance, index_t temporalDistance, SampleType t)	const;
	float					GetSmoothed(float spatialDistance, float temporalDistance,SampleType t) const;

	bool					LoadSamples(const FileSystem::File&f, const std::function<bool(const TMetrics&)> mFilter=std::function<bool(const TMetrics&)>());
	void					UpdateCurrentRange(SampleType type);
	void					UpdatePlotGeometry(SampleType type, bool window);

	void					TrainHypothesis(index_t sourceTableID, SampleType t);

	void					EmbedPlotGeometry();
	void					RemovePlotGeometry();

	void					RenderLines()	const;

	float					SmoothGeometryHeightFunction(SampleType t, float x, float y) const;
	float					FindLevelSpatialDistance(SampleType t, index_t temporalDistance, float level) const;
	float					FindNormalDeviation(SampleType t, index_t temporalDistance, float mean) const;
	float					GetSpatialNormalError(SampleType t, index_t temporalDistance, float mean, float deviation) const;
};



void RemoveAllPlotGeometries();
extern Buffer0<Table,Swap>	tables;

extern Table				rangeTable;


float Relative2TextureHeight(float h);

void LoadTables(const std::initializer_list<const FileSystem::File>&files);
void LoadTables(const Buffer0<FileSystem::File>&files);
void UpdateColors();
/**
Updates all table currentRange contents by calling t->UpdateCurrentRange(type)
*/
void UpdateTableRanges(SampleType type);


float			GetVerticalRange(SampleType t);
