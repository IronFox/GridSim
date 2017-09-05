#pragma once

#include <io/file_system.h>
#include <image/image.h>
#include <geometry/cgs.h>
#include <math/vclasses.h>
#include "sample.h"
#include <array>
#include <math/polynomial.h>

using namespace DeltaWorks;

struct TLinearFunction
{
	float	base = 0,
			inclination = 0.1f;

	float	operator()(float x) const
	{
		return base + inclination * x;
	}

	/**/	TLinearFunction(float base=0, float inclination=0.1f):base(base),inclination(inclination)	{}

	friend String	ToString(const TLinearFunction&f)
	{
		return String(f.base)+"+X*"+String(f.inclination);
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

	M::Polynomial<double,TNCoef>		temporalProfile;
	M::Polynomial<double,SNCoef>		spatialProfile0,spatialProfile1;

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
	M::NegativePolynomial2D<PFloat,4,7>	pBase;
	M::NegativePolynomial2D<PFloat,4,7>	pX;
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

	static void				TrainRow(const Table&table, index_t rowIndex, M::Polynomial<double,TAgeHypothesis::SNCoef>&poly, bool smooth);
};

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


class Table
{
public:
	typedef std::function<TSurfaceSample(float,float)> FSurface;

	class Mesh
	{
	public:
		struct TVertex
		{
			float2		st;
			float4		color;
			TSample			sample;
		};

		struct TTriangle
		{
			index_t	v[3];
		
		};

		struct TEdge
		{
			index_t	v[2];

		};


		Buffer0<TVertex>	vertices;
		Buffer0<TTriangle>	triangles;
		Buffer0<TEdge>		edges;
		index_t				vOffset = 0;


		void				SetIndexOffsetToVertexCount()
		{
			vOffset = vertices.Count();
		}

		void				AddTriangle(index_t v0, index_t v1, index_t v2)
		{
			TTriangle&t = triangles.Append();
			t.v[0] = v0 + vOffset;
			t.v[1] = v1 + vOffset;
			t.v[2] = v2 + vOffset;
		}

		void				MakeFlatQuad(const float2&center, const TSample&sample, const float3&color, float sExt, float tExt)
		{
			const float2 ext{sExt,tExt};
			SetIndexOffsetToVertexCount();
			vertices.Append().sample = sample;
			vertices.Last().st = center - ext;
			vertices.Last().color = float4(color,1.f);

			vertices.Append().sample = sample;
			vertices.Last().st = center + float2(sExt,-tExt);
			vertices.Last().color = float4(color,1.f);

			vertices.Append().sample = sample;
			vertices.Last().st = center + float2(sExt,tExt);
			vertices.Last().color = float4(color,1.f);

			vertices.Append().sample = sample;
			vertices.Last().st = center + float2(-sExt,tExt);
			vertices.Last().color = float4(color,1.f);

			AddTriangle(0,1,2);
			AddTriangle(0,2,3);
		}

		bool				IsNotEmpty() const {return triangles.IsNotEmpty();}
		void				swap(Mesh&other)
		{
			triangles.swap(other.triangles);
			vertices.swap(other.vertices);
			edges.swap(other.edges);
		}
	};


private:
	void					UpdateImage(SampleType t);
	TSurfacePoint			GetGeometryPointF(SampleType t, float spatialDistance, float temporalDistance) const;
	void					TraceLineF(const M::TFloatRange<>&range, const std::function<float3(float)>&, count_t stResolution);

	static void				BuildRegularSurface(CGS::Constructor<>::Object&target, const FSurface&surfaceFunction, count_t stResolution);
	static void				BuildMeshSurface(CGS::Constructor<>::Object&target, const Mesh&mesh, SampleType);
	void					FinishConstruction(const CGS::Constructor<>::Object&constructor, const CGS::Constructor<>::Object&transparentConstructor, const CGS::Constructor<>::Object&holeConstructor);

public:
	struct TLineSegment
	{
		float4		color;
		float				width=2;
		Buffer0<M::TVec3<> >points;

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
		M::TFloatRange<>	range;
		float				average;
	};


	Buffer0<TLineSegment,Swap>	lineSegments;
	typedef Buffer0<TSample>	SampleRow;

	String				sourceFile;
	TValueRange				currentRange;
	TMetrics				metrics;

	//THypothesisData			hypothesis;
	Buffer0<SampleRow>	samples;
	std::function<float(float spatialDistance, float temporalDistance, SampleType)>		heightFunction;

	std::function<M::TVec4<>(float spatialDistance, float temporalDistance, SampleType t)> colorFunction;
	//FloatImage				image;

//	static const constexpr unsigned Resolution = 400;
//	static const constexpr float step = 1.0f/Resolution;
	
	index_t					solidPlotGeometryHandle = InvalidIndex,
							plotHoleGeometryHandle = InvalidIndex,
							transparentGeometryHandle = InvalidIndex;
	float4				color = float4(1);
	Mesh					mesh;

	const bool				IsRangeTable;
	
	/**/					Table(bool isRangeTable=false):IsRangeTable(isRangeTable)	{}

	void					swap(Table&other)
	{
		swp(metrics,other.metrics);
		//swp(hypothesis,other.hypothesis);
		heightFunction.swap(other.heightFunction);
		colorFunction.swap(other.colorFunction);
		swp(currentRange,other.currentRange);
		lineSegments.swap(other.lineSegments);
		samples.swap(other.samples); 
		sourceFile.swap(other.sourceFile); 
		mesh.swap(other.mesh);
	}
	friend void				swap(Table&a, Table&b)	{a.swap(b);}

	const TSample&			GetSample(index_t spatialDistance, index_t temporalDistance)	const;
	//float					Get(index_t spatialDistance, index_t temporalDistance, SampleType t)	const;
	float					GetSmoothed(float spatialDistance, float temporalDistance,SampleType t) const;


	/**
	Accumulates all samples found at a spatial distance of @a r around (iRow,iCol).
	Distance is measured in chebyshev
	@return Accumulated samples
	*/
	TSample					AccumulateAtR(index_t iRow, index_t iCol, index_t r) const;



	/**
	Constructs local samples from blurred data found in the given table.
	The local data set is filled with one sample per source sample, and blurred as found necessary
	*/
	void					Blur(const Table&source);

	/**
	Loads samples from the specified file.
	@param f File to load from
	@param mFilter Optional function to check the loaded metrics before loading data from the file.
		If set, metrics are found, and the function returns false, then loading will abort and LoadSamples() returns false
	@return true if the file has successfully been loaded. False if the file could not be opened, or any set filter function returned false on loaded metrics
	*/
	bool					LoadSamples(const FileSystem::File&f, const std::function<bool(const TMetrics&)> mFilter=std::function<bool(const TMetrics&)>());
	void					UpdateCurrentRange(SampleType type);
	void					UpdatePlotGeometry(SampleType type, bool window);

	void					TrainHypothesis(index_t sourceTableID, SampleType t, bool isStatic);
	float3			TrainFlatOverestimation(index_t sourceTableID, SampleType t,float threshold);

	void					RemovePlotGeometry();

	void					RenderLines()	const;

	/**
	Flips S and T axes on local samples.
	The first row is ignored as some methods expect it to be empty.
	*/
	void					TransposeSamples();

	/**
	Stretches samples along the S axis, such that between each two raw samples, a new interpolated sample is placed.
	Use for data that is not 0.5-step precise
	*/
	void					StretchSamplesS();

	void					SetTransformed(const Table&source, const std::function<TSample(TSample)>&transformFunction, const float4&color);

	float					SmoothGeometryHeightFunctionF(SampleType t, float spatialDistance, float temporalDistance) const;
	float					FindLevelSpatialDistance(SampleType t, index_t temporalDistance, float level) const;
	float					FindNormalDeviation(SampleType t, index_t temporalDistance, float mean) const;
	float					GetSpatialNormalError(SampleType t, index_t temporalDistance, float mean, float deviation) const;
};



void RemoveAllPlotGeometries();
extern Buffer0<Table,Swap>	tables;

extern Table				rangeTable;


float Relative2TextureHeight(float h);

void LoadTables(const std::initializer_list<const FileSystem::File>&files, bool addBlurredClone);
void LoadTables(const Buffer0<FileSystem::File>&files, bool addBlurredClone);
void UpdateColors();
/**
Updates all table currentRange contents by calling t->UpdateCurrentRange(type)
*/
void UpdateTableRanges(SampleType type);


float			GetVerticalRange(SampleType t);
