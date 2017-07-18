#include <global_root.h>
#include "table.h"
#include "rendering.h"

using namespace DeltaWorks;
using namespace Math;

Buffer0<Table,Swap>	tables;
Table rangeTable(true);

static float Lerp(float v0, float v1, float x)
{
	return v0 * (1.f - x) + v1 * x;
}


const TSample& Table::GetSample(index_t iX, index_t iY) const
{
	static const TSample empty;


	if (iY >= samples.Count())
		return empty;
	if (iX >= samples[iY].Count())
		return empty;
	return samples[iY][iX];
}

float	Table::FindLevelSpatialDistance(SampleType t, index_t temporalDistance, float level) const
{
	index_t left = 0;
	index_t right = samples[1].Count();
	if (!right)
		return -1;
	right--;
	float leftH = Get(left,temporalDistance,t);
	float rightH = Get(right,temporalDistance,t);
	if (leftH < level == rightH < level)
		return -1;


	for (int i = 0; i < 10; i++)
	{
		index_t middle = (left+right)/2;
		float middleH = Get(middle,temporalDistance,t);
		if (middleH < level == leftH < level)
		{
			left = middle;
			leftH = middleH;
		}
		elif (middleH < level == rightH < level)
		{
			right = middle;
			rightH = middleH;
		}
		else
		{
			ASSERT_EQUAL__(leftH < level,rightH < level);
			return -1;
		}
	}
	float x = (level - leftH)/(rightH - leftH);
	return left + (right-left)*x;
}


float	Table::GetSpatialNormalError(SampleType t, index_t temporalDistance, float mean, float deviation) const
{
	const auto&row = samples[temporalDistance];
	float error = 0;
	for (index_t x = 0; x < row.Count(); x++)
	{
		if (x > mean)
			continue;
		float expect = 0.5f * (1.f + Erf3(float(mean-x)/(deviation * M_SQRT2)));
		float have = row[x].Get(t);
		error += (have-expect) * row[x].numEntities;
	}
	return error;
}

float	Table::FindNormalDeviation(SampleType t, index_t temporalDistance, float mean) const
{
	float left = 0.001f;
	float right = 100.f;
	float leftH = GetSpatialNormalError(t,temporalDistance,mean,left);
	float rightH = GetSpatialNormalError(t,temporalDistance,mean,right);
	if (leftH < 0 == rightH < 0)
		return -1;


	for (int i = 0; i < 10; i++)
	{
		float middle = (left+right)/2;
		float middleH = GetSpatialNormalError(t,temporalDistance,mean,middle);
		if (middleH < 0 == leftH < 0)
		{
			left = middle;
			leftH = middleH;
		}
		elif (middleH < 0 == rightH < 0)
		{
			right = middle;
			rightH = middleH;
		}
		else
		{
			ASSERT_EQUAL__(leftH < 0,rightH < 0);
			return -1;
		}
	}
	float x = (0 - leftH)/(rightH - leftH);
	return left + (right-left)*x;
}


void	THypothesisData::TrainRow(const Table&raw, index_t rowIndex, Math::Polynomial<double,TAgeHypothesis::SNCoef>&poly, bool smooth)
{
	const auto&row = raw.samples[rowIndex];

	count_t effectiveWidth = 0;
	for (; effectiveWidth < row.GetLength(); effectiveWidth++)
		if (row[effectiveWidth].Get(SampleType::DivergenceDepth) /*< age.max*0.00001f*/ == 0)
			break;
	if (effectiveWidth+1 < row.GetLength())
		effectiveWidth++;

	float xScale = float(raw.samples.Count()-1) / float(rowIndex);

	float max = row.First().Get(SampleType::DivergenceDepth);

	std::array<TVec2<double>,TAgeHypothesis::SNCoef>	samplePoints;
	for (index_t at = 0; at < TAgeHypothesis::SNCoef; at++)
	{
		const float x = smooth ?
									CubicFactor(float(at)/(TAgeHypothesis::SNCoef-1))*effectiveWidth:
									float(at)*effectiveWidth/(TAgeHypothesis::SNCoef-1);
		const float y0 = row[floor(x)].Get(SampleType::DivergenceDepth)/max;
		const float y1 = row[ceil(x)].Get(SampleType::DivergenceDepth)/max;
		const float y = Lerp(y0,y1,frac(x));

		float fx = float(x)/raw.metrics.spatialSupersampling*xScale;
		samplePoints[at] = TVec2<double>{fx,y};
	}
	ASSERT__(poly.MatchPoints(samplePoints));
}

float3 AverageSpatialPoint(const Table&raw,SampleType t,index_t row, float z, float zRadius)
{
	float3 rs;
	count_t numSamples = 0;
	for (int x = -5; x <= 5; x++)
		for (int y = -4; y <= 4; y++)
		{
			float3 p(0,row+x,z+zRadius*y/4);
			p.x = raw.FindLevelSpatialDistance(SampleType::DivergenceDepth,row+x,p.z)/raw.metrics.spatialSupersampling;
			if (p.x >= 0)
			{
				rs += p;
				numSamples++;
			}
		}
	return rs / numSamples;
}


void	THypothesisData::Train(index_t sourceTableID, SampleType t)
{
	this->sourceTableID= sourceTableID;
	const auto&raw = tables[sourceTableID];

	this->sourceDensity = raw.metrics.entityDensity;
	this->sourceRelativeSensorRange = 1.f - raw.metrics.relativeMovementRange;
		


	const float deviationLevel = 0.5f * (1.f + Erf3(1.0/M_SQRT2));

	float max = raw.samples.Last().First().Get(SampleType::DivergenceDepth);
	age.max = max;
	age.maxTemporalDistance = raw.samples.Count()-1;


#if 0

	{

		float error = std::numeric_limits<float>::max();
		TAgeHypothesis best = age;
		for (int i = 1; i < raw.samples.Count(); i++)
		{
			std::array<TVec2<double>,TAgeHypothesis::TNCoef>	samplePoints;

			age.temporalLinearBreak = i;
			float last = 0;
			index_t lastX = 0;
			for (index_t at = 0; at < TAgeHypothesis::TNCoef; at++)
			{
				const index_t x = //::vmin(age.maxTemporalDistance,pow(float(at)/(TAgeHypothesis::TNCoef-1),1.2f)*age.maxTemporalDistance);
								at*age.temporalLinearBreak/(TAgeHypothesis::TNCoef-1);
				const auto&row = raw.samples[x];
				const float y = row.IsNotEmpty()? row.First().Get(SampleType::DivergenceDepth) : 0;

				samplePoints[at] = TVec2<double>{double(x),y};
				last = y;
				lastX = x;
			}
			if (!age.temporalProfile.Approximate(samplePoints))
				continue;

	//		float yLast = raw.samples.Last().First().Get(SampleType::DivergenceDepth);
			if (age.temporalLinearBreak+1 < raw.samples.Count())
			{
				age.temporalLinearPart.inclination = (max - last) / float((raw.samples.Count()-1 - age.temporalLinearBreak));
				age.temporalLinearPart.base = last - lastX * age.temporalLinearPart.inclination;
			}
			else
			{
				age.temporalLinearPart.inclination = 0;
				age.temporalLinearPart.base = last;
			}

			float err = 0;
			for (index_t j = 1; j < raw.samples.Count(); j++)
			{
				const float measured = raw.samples[j].First().Get(SampleType::DivergenceDepth);
				const float synthetic = age.TemporalSample(j);
				ASSERT1__(!isnan(measured),j);
				ASSERT1__(!isnan(synthetic),j);
				err += sqr(measured - synthetic) * 1.f/float(pow(j,1.4f));
			}
			if (err < error)
			{
				error = err;
				best = age;
			}
		}
		age = best;
		//D S7: 0.0504900739
		//D S6: 0.186867312
		//D S5: 0.29
		//D S4: 1.39251947

		//Deg S7 T7: 0.0435041673
		//Deg S7 T6: 0.0426488556
		//Deg S7 T5: 0.0403990969
		//Deg S7 T4: 0.0406806208
		//Deg S7 T3: 0.0406661704
		//float testError = (age.GetAverageSquaredError(raw));
		//__debugbreak();
	}




	{
		const auto&row = raw.samples.Last();	//largest
		count_t effectiveWidth = 0;
		for (; effectiveWidth < row.GetLength(); effectiveWidth++)
			if (row[effectiveWidth].Get(SampleType::DivergenceDepth) /*< age.max*0.00001f*/ == 0)
				break;
		age.maxSpatialDistance = effectiveWidth/raw.spatialOversampling;
	}
	age.spatialProfile0TAt = raw.samples.Count()/4;
	TrainRow(raw, age.spatialProfile0TAt,age.spatialProfile0,true);
	TrainRow(raw, raw.samples.Count()-1,age.spatialProfile1,true);

#endif
	if (!isStatic)
	{
#if 0
		Polynomial2D<double,2,2>	plane;
		Buffer0<TVec3<double> > points;
		const float top = raw.samples.Last().First().Get(t);
		foreach (raw.samples,row)
			foreach (*row,s)
			{
				float data = s->Get(t);//inclination 1
				if (data > 0.01f * top)
				{
					auto&p = points.Append();
					p.x = double(s - row->pointer()) / raw.metrics.spatialSupersampling;
					p.y = row - raw.samples.pointer();
					p.z = data;
				}
			}

		ASSERT__(plane.Approximate(points.ToRef()));


		age.linearPlaneFunc.x = plane.coefficients.y.x;
		age.linearPlaneFunc.y = plane.coefficients.x.y;
		age.linearPlaneFunc.z = plane.coefficients.x.x;



#else

		const float lowZ = max*0.35f,
					highZ = max*0.65f;
		const index_t	middleY = age.maxTemporalDistance/2;
		float3 p1 = AverageSpatialPoint(raw,SampleType::DivergenceDepth,0.9f * age.maxTemporalDistance,lowZ,max*0.15f);
		float3 p0 = AverageSpatialPoint(raw,SampleType::DivergenceDepth,0.9f * age.maxTemporalDistance,highZ,max*0.15f);
		float3 p2 = AverageSpatialPoint(raw,SampleType::DivergenceDepth,middleY,lowZ,max*0.1f);

		//const float lowX = raw.FindLevelSpatialDistance(SampleType::DivergenceDepth,age.maxTemporalDistance,lowZ)/raw.metrics.spatialSupersampling;
		//const float highX = raw.FindLevelSpatialDistance(SampleType::DivergenceDepth,age.maxTemporalDistance,highZ)/raw.metrics.spatialSupersampling;
		//const float middleX = raw.FindLevelSpatialDistance(SampleType::DivergenceDepth,middleY,lowZ)/raw.metrics.spatialSupersampling;

		//float3 p[3],n;
		//p[1] = float3(lowX,age.maxTemporalDistance,lowZ);
		//p[0] = float3(highX,age.maxTemporalDistance,highZ);
		//p[2] = float3(middleX,middleY,lowZ);
		float3 normal;
		Obj::triangleNormal(p0,p1,p2,normal);
		Vec::normalize(normal);
		float w = Vec::dot(normal,(p0+p1+p2)/3);
		age.linearPlaneFunc.x = -normal.x/normal.z;
		age.linearPlaneFunc.y = -normal.y/normal.z;

		age.linearPlaneFunc.z = (w) / normal.z;
#endif /*0*/
	}


	return;
	{
		


		//Array<double> X(effectiveWidth * TAgeHypothesis::NCoef),	//looks okay
		//			X_T(TAgeHypothesis::NCoef*effectiveWidth),
		//			X_T_X(TAgeHypothesis::NCoef*TAgeHypothesis::NCoef),
		//			test(TAgeHypothesis::NCoef*TAgeHypothesis::NCoef),
		//			X_T_X_I(TAgeHypothesis::NCoef*TAgeHypothesis::NCoef),
		//			X_T_X_I_X_T(TAgeHypothesis::NCoef*effectiveWidth),
		//			Y(effectiveWidth);
		////column major:
		//for (index_t x = 0; x < effectiveWidth; x++)
		//{
		//	const float y = row[x].Get(SampleType::DivergenceDepth)/age.max;

		//	for (index_t e = 0; e < TAgeHypothesis::NCoef; e++)
		//	{
		//		float xN = pow(x,e);
		//		X[effectiveWidth*e + x] = xN;
		//		X_T[TAgeHypothesis::NCoef*x + e] = xN;
		//	}
		//	Y[x] = y;
		//}
		//
		//Mat::Raw::MultD(X_T.pointer(), X.pointer(), X_T_X.pointer(), TAgeHypothesis::NCoef, effectiveWidth, TAgeHypothesis::NCoef);
	 //   bool rs = Mat::Raw::Invert<double,double,TAgeHypothesis::NCoef>(X_T_X.pointer(),X_T_X_I.pointer());
		//Mat::Raw::MultD(X_T_X.pointer(),X_T_X_I.pointer(),test.pointer(),TAgeHypothesis::NCoef,TAgeHypothesis::NCoef,TAgeHypothesis::NCoef);
		//Mat::Raw::MultD(X_T_X_I.pointer(),X_T.pointer(),X_T_X_I_X_T.pointer(),TAgeHypothesis::NCoef,TAgeHypothesis::NCoef,effectiveWidth);
		//Mat::Raw::MultD(X_T_X_I_X_T.pointer(),Y.pointer(),age.coefficients.v,TAgeHypothesis::NCoef,TAgeHypothesis::NCoef,1);
		//ASSERT__(rs);
	}


#if 0
	age.mean = raw.FindLevelSpatialDistance(SampleType::DivergenceDepth,raw.samples.Count()-1,age.max*0.5f);
	age.nonZeroExtent = raw.FindLevelSpatialDistance(SampleType::DivergenceDepth,raw.samples.Count()-1,age.max*0.05f);
	TAgeHypothesis amin,amax;
	amin.curveExtent = 1.f;
	amax.curveExtent = raw.samples[2].Count();
	amin.max = age.max*0.9f;
	amax.max = age.max*1.2f;
	amin.mean = 0;
	amax.mean = raw.samples[2].Count();
	amin.xScale = 0.001f;
	amax.xScale = 10.f;

	const auto vmin = amin.ToVector();
	const auto vmax = amax.ToVector();

	for (int i = 0; i < 100; i++)
	{
		const auto vector = age.ToVector();
		auto best=vector;
		const float baseError = age.GetAverageSquaredError(raw);
		
		for (int axis = 0; axis < 4; axis++)
		{
			//if (axis == 1)
			//	continue;
			TAgeHypothesis test = age;
			auto vec = test.ToVector();
			float error = baseError;
			for (int r = 1; r < 4; r++)
			{
				float val = vec[axis];
				const float base = vec[axis];
				float step = 1.f / pow(8,r);
				for (int k = -10; k <= 10; k++)
				//	if (k != 0)
				{
					float kf = float(k)/10.f;
					float at = Math::clamp( base +kf*step,vmin[axis],vmax[axis]);

					vec[axis] = at;
					test.LoadVector(vec);
					float err = test.GetAverageSquaredError(raw);
					if (err < error)
					{
						error = err;
						val = vec[axis];
				//		ShowMessage("improved");
					}
				}
				vec[axis] = val;
				test.LoadVector(vec);
				float err = test.GetAverageSquaredError(raw);
				ASSERT_EQUAL__(err,error);
			}
			best[axis] = vec[axis];
			//age.LoadVector(vec);
		}

		auto delta = best - vector;
		age.LoadVector(vector + delta*0.1f);
		std::cout << age.ToVector().ToString()<<std::endl;
	}
#endif /*0*/

	Array<float> mean(raw.samples.Count()), deviation(raw.samples.Count());
	for (index_t i = 0; i < mean.GetLength(); i++)
	{
		mean[i] = raw.FindLevelSpatialDistance(t,i,0.5f);
		//if (mean[i] >= 0)
		//	deviation[i] = raw.FindNormalDeviation(t,i,mean[i]);
		//else
		//	deviation[i] = -1;
		deviation[i] = raw.FindLevelSpatialDistance(t,i,deviationLevel);
		if (mean[i] < 0)
			deviation[i] = -1;
		elif (deviation[i] >= 0)
			deviation[i] = fabs(deviation[i] - mean[i]);
		ASSERT__(!isnan(mean[i]));
	}
	index_t step = Math::vmax(1,mean.GetLength()-50);
	float baseSum = 0, incSum = 0,
			devBaseSum = 0, devIncSum = 0;
	count_t sum = 0,devSum = 0;
	for (index_t i = 0; i+step < mean.GetLength(); i++)
	{
		{
			const float s0 = mean[i];
			const float s1 = mean[i+step];
			if (s0 < 0 || s1 < 0)
				continue;
			float inclination = (s1 - s0) / step;
			float base = s0 - inclination*i;
			baseSum += base;
			incSum += inclination;
			sum++;
		}
		{
			const float d0 = deviation[i];
			const float d1 = deviation[i+step];
			if (d0 < 0 || d1 < 0)
				continue;
			float inclination = (d1 - d0) / step;
			float base = d0 - inclination*i;
			devBaseSum += base;
			devIncSum += inclination;
			devSum++;
		}
	}
	if (sum)
	{
		meanFunction.base = baseSum / sum;
		meanFunction.inclination = incSum / sum;
	}
	if (devSum)
	{
		deviationFunction.base = devBaseSum / devSum;
		deviationFunction.inclination = devIncSum / devSum;
	}
}


float3	Table::TrainFlatOverestimation(index_t sourceTableID, SampleType t,float threshold)
{
	const auto& source = tables[sourceTableID];


	const float farSpatialBegin = source.FindLevelSpatialDistance(t,source.samples.Count()-1,threshold);
	const float middleSpatialBegin = source.FindLevelSpatialDistance(t,source.samples.Count()/2,threshold);
	const float closeSpatialBegin = source.FindLevelSpatialDistance(t,source.samples.Count()/4,threshold);

	D::M::float3			linearPlaneFunc;	//x: spatial,y: temporal inclination, z: offset

	if (farSpatialBegin < 0)
		return linearPlaneFunc;

	const float relative = closeSpatialBegin / farSpatialBegin;
	const float D = 1.f - relative;
	//linearPlaneFunc.z = 1.f - D*4/3;
	linearPlaneFunc.z =  0.035;
	linearPlaneFunc.y = (1.f - linearPlaneFunc.z) / source.samples.Count();
	linearPlaneFunc.x = -1.f / farSpatialBegin;

	//linearPlaneFunc.z = 0;
	//linearPlaneFunc.y = 1.f / source.samples.Count();
	//linearPlaneFunc.x = -1.f / farSpatialBegin;

	this->heightFunction = [linearPlaneFunc,sourceTableID](float spatialDistance, float temporalDistance, SampleType t) -> float
	{
		//float h = D::M::Max(0.f, (linearPlaneFunc.z + linearPlaneFunc.x * spatialDistance + linearPlaneFunc.y * temporalDistance));
		//return h;
		//if (h == 0)
		//	return 0;
		const auto& source = tables[sourceTableID];
		return source.GetSmoothed(spatialDistance,temporalDistance,t) * 1.1f;
	};

	this->colorFunction = [linearPlaneFunc,sourceTableID](float spatialDistance, float temporalDistance, SampleType t) -> float4
	{
		const float h = D::M::Max(0.f, (linearPlaneFunc.z + linearPlaneFunc.x * spatialDistance + linearPlaneFunc.y * temporalDistance));
		const auto& source = tables[sourceTableID];
		const float s = source.GetSmoothed(spatialDistance,temporalDistance,t);

		//float level = s/h;
		float4 rs;
		rs.a = 1.f;
		Vec::HSL2RGB(float3(fmod(h*20.f,1.f),1.f,0.5f),rs.rgb);
		return rs;

		//if (h == 0)
		//	return 0;
		//const auto& source = tables[sourceTableID];
		//return source.GetSmoothed(spatialDistance,temporalDistance,t) / h;
	};


	this->sourceFile = source.sourceFile+" (H) T "+linearPlaneFunc.ToString();
	return linearPlaneFunc;
}


void Table::TrainHypothesis(index_t sourceTableID, SampleType t, bool isStatic)
{
	THypothesisData hypothesis(true,isStatic);
	hypothesis.Train(sourceTableID,t);
	this->heightFunction = [hypothesis](float spatialDistance, float temporalDistance, SampleType t) -> float
	{
		return hypothesis.Sample(spatialDistance,temporalDistance,t);
	};

	this->colorFunction = [hypothesis](float spatialDistance, float temporalDistance, SampleType t) -> TVec4<>
	{
		const float shouldBe = tables[hypothesis.sourceTableID].SmoothGeometryHeightFunction(t,temporalDistance,spatialDistance);
		const float error = (shouldBe-hypothesis.Sample(spatialDistance,temporalDistance,t));
		//rs.height = fabs(shouldBe-rs.height);
		TVec4<> rs;
		rs.a = 1;
		rs.r = 1;
		rs.gb = 1.f - float2(fabs(error)*50);

		return rs;
	};



	this->sourceFile = tables[sourceTableID].sourceFile+" (H) T "+hypothesis.age.linearPlaneFunc.ToString();
}



float TAgeHypothesis::TemporalSample(float temporalDistance) const
{
	if (temporalDistance < 0)
		return 0;
	if (temporalDistance < temporalLinearBreak)
		return temporalProfile(temporalDistance);
	return temporalLinearPart(temporalDistance);

}

float TAgeHypothesis::SpatialSample(float spatialDistance, float temporalDistance) const
{
	if (spatialDistance > maxSpatialDistance)
		return 0;

	const float s0 = spatialProfile0(spatialDistance);
	if (temporalDistance < spatialProfile0TAt)
		return clamp01(s0);
	const float s1 = spatialProfile1(spatialDistance);
	return clamp01(Lerp(s0,s1,(temporalDistance-spatialProfile0TAt)/(maxTemporalDistance - spatialProfile0TAt)));

	//return clamp01(spatialProfile(spatialDistance));
}



float TStaticAgeHypothesis::Sample(float spatialDistance, float temporalDistance, float density, float relativeSensorRange) const
{
	float3 linearPlaneFunc(pX(density,relativeSensorRange),1,pBase(density,relativeSensorRange));  
	float h = linearPlaneFunc.x * spatialDistance + linearPlaneFunc.y * temporalDistance + linearPlaneFunc.z;
	return vmax(h,0.f);
}

float TAgeHypothesis::Sample(float spatialDistance, float temporalDistance) const
{
	const float ext = (temporalDistance / maxTemporalDistance);

	if (!ext)
		return 0;

	if (useLinearApproximation)
	{
		float h = linearPlaneFunc.x * spatialDistance + linearPlaneFunc.y * temporalDistance + linearPlaneFunc.z;
			//(linearPlane.w - linearPlane.x * spatialDistance - linearPlane.y*temporalDistance) / linearPlane.z;
		return vmax(h,0.f);
	}
	
	float t= TemporalSample(temporalDistance);
	//float ext = t / max;

	//float x = (mean*ext - spatialDistance);
	//four unknown variables: mean, xScale, ageDeviation, maxAge



	return SpatialSample(spatialDistance/ext,temporalDistance)*t;

	//const float cdf = 1.f - 1.f / (1.f + exp(-x));

	////const float cdf = 1.f - LogNormalCDF(x, deviation);
	////const float cdf = 0.5f * (1.f + Erf3(x/(deviation*ext * M_SQRT2)));

	//return cdf * ext*max;
}

float	TAgeHypothesis::GetAverageSquaredError(const Table&rawData) const
{
	float error = 0;
	count_t numSamples=0;
	for (index_t age = rawData.samples.Count()-1; age < rawData.samples.Count(); age--)
	{
		const auto&row = rawData.samples[age];
		numSamples += row.Count();
		for (index_t spatial = 0; spatial < row.Count(); spatial++)
			error += sqr(Sample(spatial,age));
		//if (row.IsNotEmpty())
			//break;

	}
	return error / numSamples;
}


TStaticAgeHypothesis THypothesisData::staticAgeHypothesis;


float THypothesisData::Sample(float spatialDistance, float temporalDistance, SampleType t) const
{
	if (t == SampleType::DivergenceDepth)
	{
		if (isStatic)
			return staticAgeHypothesis.Sample(spatialDistance /tables[sourceTableID].metrics.spatialSupersampling,temporalDistance,sourceDensity,sourceRelativeSensorRange);
		return age.Sample(spatialDistance /tables[sourceTableID].metrics.spatialSupersampling,temporalDistance);
	}

	const float mean = meanFunction(temporalDistance);
	const float deviation = deviationFunction(temporalDistance);
	float x = mean - spatialDistance;	//reverse
	return 0.5f * (1.f + Erf3(x/(deviation * M_SQRT2)));
}

float Table::Get(index_t spatialDistance, index_t temporalDistance, SampleType t) const
{
	if (heightFunction)
		return heightFunction(spatialDistance,temporalDistance,t);
	return GetSample(spatialDistance,temporalDistance).Get(t);
}

float				Table::TInclinedSurfacePoint::ThicknessToOffsetX(float t) const
{
	float maxInclination = fabs(xInclination);
	float rs = t / sqrt(sqr(maxInclination)+1) 
			+ maxInclination * (t * sqrt(1.f - 1.f / (sqr(maxInclination)+1)));
	DBG_ASSERT_GREATER_OR_EQUAL__(rs,t);
	return rs;
}

float				Table::TInclinedSurfacePoint::ThicknessToOffsetY(float t) const
{
	float maxInclination = fabs(yInclination);
	float rs = t / sqrt(sqr(maxInclination)+1) 
			+ maxInclination * (t * sqrt(1.f - 1.f / (sqr(maxInclination)+1)));
	DBG_ASSERT_GREATER_OR_EQUAL__(rs,t);
	return rs;
}


bool Table::LoadSamples(const FileSystem::File&f, const std::function<bool(const TMetrics&)> mFilter/*=std::function<bool(const TMetrics&)>()*/)
{
	StringFile file;
	count_t cnt = 0;
	for(;;)
	{
		try
		{
			file.Open(f.GetLocation());
			break;
		}
		catch (...)
		{}
		Sleep(200);
		if (++cnt > 5)
			return false;
	}

	metrics = TMetrics();
	samples.Clear();
	String line;
	Array<StringRef>	parts;
	samples.Append();
	bool hasLocationErrorSum = false;
	bool hasMissingEntites = false;
	bool hasInconsistencyAge = false;
	while (file >> line)
	{
		if (CharFunctions::isalpha(line.firstChar()))
		{
			//attribute or metric
			index_t eqAt = line.find('=');
			if (eqAt == 0)
			{
				std::cout << "File contains malformatted metric '"<<line<<"'. Ignoring."<<std::endl;
				continue;
			}
			const StringRef svalue = line.subStringRef((int)eqAt);

			count_t value;
			float fvalue;
			bool haveInt = convert(svalue.pointer(),svalue.GetLength(),value);
			bool haveFloat = convert(svalue.pointer(),svalue.GetLength(),fvalue);
			StringRef metric = line.subStringRef(0,(int)eqAt-1);
			if (metric == "TotalSimulations" && haveInt)
			{
				metrics.totalSimulationRuns = value;
				continue;
			}

			if (haveInt && (metric == "SpatialOversampling" || metric == "SpatialSupersampling"))
			{
				metrics.spatialSupersampling = value;
				continue;
			}
			if (haveInt && (metric == "Dimensions"))
			{
				metrics.dimensions = int(value);
				continue;
			}
			if (metric == "EntityDeath")
			{
				metrics.entityDeathAllowed = svalue == "Allowed";
				continue;
			}
			if (metric == "EntityMotionProfile")
			{
				if (svalue == "linear")
					metrics.motionProfile = TMetrics::Linear;
				elif (svalue == "")
					metrics.motionProfile = TMetrics::Binary;
				else
					metrics.motionProfile = TMetrics::Exotic;
				continue;
			}
			if (metric == "EnvironmentImpact")
			{
				metrics.unaffectedRandomMotion = svalue != "AppearanceAndMotion";
				continue;
			}

			if (metric == "RelativeMovementRange" && haveFloat)
			{
				metrics.relativeMovementRange = fvalue;
				continue;
			}

			if (metric == "EntityDensity" && haveFloat)
			{
				metrics.entityDensity = fvalue;
				continue;
			}

			if (metric == "EntitiesPerSimulation" && haveInt)
			{
				metrics.entitiesPerSimulation = value;
				continue;
			}
			std::cout << "File contains unknown metric '"<<String(metric)<<"' in '"<<line<<"'. Ignoring."<<std::endl;
			continue;
		}


		if (line.beginsWith('[') && line.endsWith(']'))	//flags
		{
			if (mFilter && !mFilter(metrics))
				return false;
			explode(' ',line.getBetweenRef('[',']'),parts);
			foreach(parts, part)
			{
				if (part->CompareToIgnoreCase("PositionErrorSum")==0 || part->CompareToIgnoreCase("LocationError")==0 || part->CompareToIgnoreCase("LocationError1")==0)
					hasLocationErrorSum = true;
				elif (part->CompareToIgnoreCase("MissingEntities")==0)
					hasMissingEntites = true;
				elif (part->CompareToIgnoreCase("DivergenceDepth")==0 || part->CompareToIgnoreCase("InconsistencyAgeSum")==0)
					hasInconsistencyAge = true;
				elif (part->CompareToIgnoreCase("NumEntities")==0 || part->CompareToIgnoreCase("InconsistentEntities")==0 || part->CompareToIgnoreCase("NewlyInconsistentEntities")==0)
				{}
				else
					FATAL__("Unsupported flag in file: '"+String(*part)+"'");
			}
			continue;
		}
		explode(tab,line,parts);
		Buffer0<TSample>&line = samples.Append();

		count_t numParts = 3 + hasLocationErrorSum + hasMissingEntites + hasInconsistencyAge;

		for (index_t i = 0; i*numParts+numParts < parts.Count(); i++)
		{
			const auto&total = parts[i*numParts+1];
			const auto&inc = parts[i * numParts + 2];
			const auto&newlyInc = parts[i*numParts+3];
			line.Append().Decode(total,inc,newlyInc
				,hasLocationErrorSum ? parts[i*numParts+4] : StringRef()
				,hasMissingEntites ? parts[i*numParts+5] : StringRef()
				,hasInconsistencyAge ? parts[i*numParts+6] : StringRef()
				);
		}
		bool brk = true;
	}

	//reduce to half (actual) resolution:
	//for (index_t i = 1; i < samples.Count(); i++)
	//{
	//	for (index_t j = 0; j < samples[i].Count(); j++)
	//	{
	//		samples[i][j].Include(samples[i][j+1]);
	//		samples[i].Erase(j+1);
	//	}
	//}


	file.Close();
	sourceFile = String(f.GetInnerName());
	return true;
}






inline float Interpolate(float v0, float v1, float x)
{
	//return (x < 0.5f) ? v0 : v1;

	//return vmin(v0, v1);
	return v0 * (1.f - x) + v1 * x;
}

float	Table::GetSmoothed(float spatialDistance, float temporalDistance,SampleType t) const
{
	if (heightFunction)
		return heightFunction(spatialDistance,temporalDistance,t);

	float atX = spatialDistance;
	float atY = temporalDistance;


	float s00,s01,s11,s10;

	index_t iX = (index_t)floor(atX);
	index_t iY = (index_t)floor(atY);

	s00 = Get(iX,iY, t);
	s01 = Get(iX+1,iY, t);
	s10 = Get(iX, iY+1, t);
	s11 = Get(iX + 1, iY+1, t);
	float s0 = Interpolate(s00,s01,frac(atX));
	float s1 = Interpolate(s10,s11,frac(atX));
	return Interpolate(s0,s1,frac(atY));
}


float Table::SmoothGeometryHeightFunction(SampleType t, float x, float y) const
{
	return range.z.Relativate(GetSmoothed(y,x,t));
}


float Relative2TextureHeight(float h)
{
	return h * 1.1f;
}




Table::TSurfacePoint	Table::GetGeometryPoint(SampleType t, index_t ix, index_t iy) const
{
	float fx = range.x.Derelativate( (float)ix/(Resolution-1) );
	float fy = range.y.Derelativate( (float)iy/(Resolution-1) );
	TSurfacePoint rs;
	rs.height =  SmoothGeometryHeightFunction(t,fx,fy);

	if (colorFunction)
		rs.color = colorFunction(fy,fx,t);
	else
		rs.color = color;

	return rs;
}

Table::TInclinedSurfacePoint	Table::GetInclinedGeometryPoint(SampleType t, index_t ix, index_t iy) const
{
	float fx = range.x.Derelativate( (float)ix/(Resolution-1) );
	float fy = range.y.Derelativate( (float)iy/(Resolution-1) );
	TInclinedSurfacePoint rs;
	((TSurfacePoint&)rs) = GetGeometryPoint(t,ix,iy);
	rs.xInclination = 0;
	rs.yInclination = 0;
	//for (int step = 1; step <= 10;step++)
	{
		float step = 5.f;
		rs.xInclination += (SmoothGeometryHeightFunction(t,fx+step,fy)-SmoothGeometryHeightFunction(t,fx-step,fy))/step/2;
		rs.yInclination += (SmoothGeometryHeightFunction(t,fx,fy+step)-SmoothGeometryHeightFunction(t,fx,fy-step))/step/2;
	}
	//rs.xInclination/=10;
	//rs.yInclination/=10;

	return rs;
}

/*static*/ void	Table::BuildPlotSurface(CGS::Constructor<>::Object&obj,const std::function<TSurfaceSample(index_t,index_t)>&surfaceFunction)
{
	//for (unsigned iy = 0; iy < Resolution; iy++)
	//{
	//	float fy = (float)iy/(Resolution-1) * 2.f - 1.f;

	//	const auto s = surfaceFunction(0,iy);
	//	{
	//		float h = s.height;
	//			//image.get(0,iy)[0];
	//		obj.MakeVertex(float3(-1,fy,h),s.color,float2(Relative2TextureHeight(h)));
	//	}
	//	{
	//		obj.MakeVertex(float3(-1,fy,0),s.color,float2());
	//	}
	//	if (iy > 0 && s.cover)
	//		obj.MakeQuadInv((iy-1)*2,(iy-1)*2+1,(iy)*2+1,(iy)*2);
	//}

	//obj.SetVertexOffsetToCurrent();

	//for (unsigned ix = 0; ix < Resolution; ix++)
	//{
	//	float fx0 = (float)ix/(Resolution-1) * 2.f - 1.f;
	//	//float fx1 = (float)(ix+1)/(res);


	//	const auto s = surfaceFunction(ix,0);
	//	TVec2<> coords={0,0};
	//	obj.MakeVertex(float3(fx0,-1,0) ,s.color,coords);
	//	{
	//		float h = s.height;
	//			//image.get(ix,0)[0];
	//		obj.MakeVertex(float3(fx0,-1,h),s.color,float2(Relative2TextureHeight(h)));
	//	}
	//	if (ix > 0 && s.cover)
	//		obj.MakeQuadInv((ix-1)*2,(ix-1)*2+1,(ix)*2+1,(ix)*2);
	//}

	obj.SetVertexOffsetToCurrent();

	for (unsigned ix = 0; ix < Resolution; ix++)
	{
		float fx0 = (float)ix/(Resolution-1) * 2.f - 1.f;
		for (unsigned iy = 0; iy < Resolution; iy++)
		{
			float fy = (float)iy/(Resolution-1) * 2.f - 1.f;
			const auto s = surfaceFunction(ix,iy);

			{
				float h = s.height;
					//image.get(ix,iy)[0];
				obj.MakeVertex(float3(fx0,fy,h),s.color,float2(Relative2TextureHeight(h)));
			}
			if (ix && iy && s.cover)
				obj.MakeQuadInv( (ix-1)*Resolution + (iy-1), (ix-1)*Resolution + (iy),  (ix)*Resolution + (iy),  (ix)*Resolution + (iy-1));
		}
	}

//	obj.SetVertexOffsetToCurrent();
//
//	for (unsigned ix = 0; ix < Resolution; ix++)
//	{
//		float fx0 = (float)ix/(Resolution-1) * 2.f - 1.f;
//		//float fx1 = (float)(ix+1)/(res);
//		const auto s = surfaceFunction(ix,Resolution-1);
//
//
//		TVec2<> coords={0,0};
//		obj.MakeVertex(float3(fx0,1,0) ,s.color,coords);
//		{
////			float h = image.get(ix,res-1)[0];
//			float h = s.height;
//			obj.MakeVertex(float3(fx0,1,h),s.color,float2(Relative2TextureHeight(h)));
//		}
//		if (ix > 0 && s.cover)
//			obj.MakeQuad((ix-1)*2,(ix-1)*2+1,(ix)*2+1,(ix)*2);
//	}


//	obj.SetVertexOffsetToCurrent();
//	for (unsigned iy = 0; iy < Resolution; iy++)
//	{
//		float fy = (float)iy/(Resolution-1) * 2.f - 1.f;
//
//		const auto s = surfaceFunction(Resolution-1,iy);
//		{
//			float h = s.height;
//			obj.MakeVertex(float3(1,fy,h),s.color,float2(Relative2TextureHeight(h)));
//		}
//		{
//			obj.MakeVertex(float3(1,fy,0),s.color,float2());
//		}
//		if (iy > 0 && s.cover)
//			obj.MakeQuad((iy-1)*2,(iy-1)*2+1,(iy)*2+1,(iy)*2);
//	}

}

/*static*/	void				Table::BuildPlotHoleWalls(CGS::Constructor<>::Object&obj,const std::function<TInclinedSurfacePoint(index_t,index_t)>&surfaceFunction, UINT xExtent, UINT yExtent, float thickness /*= 0.03f*/)
{
	obj.SetVertexOffsetToCurrent();
	for (unsigned iy = yExtent-1; iy < Resolution; iy++)
	{
		float fy = (float)iy/(Resolution-1) * 2.f - 1.f;
		unsigned ix = Resolution-xExtent;
		float fx = (float)ix/(Resolution-1) * 2.f - 1.f;
		auto s = surfaceFunction(ix,iy);
		s.xInclination *= range.x.GetExtent();
		s.yInclination *= range.y.GetExtent();

		{
//			float h = image.get(res-1,iy)[0];
			float h = s.height;
			obj.MakeVertex(float3(fx,fy,h),s.color, float2(Relative2TextureHeight(h)));
		}
		{
			float t = s.ThicknessToOffsetY(thickness);

			obj.MakeVertex(float3(fx,fy,vmax(s.height-t,0)),s.color,float2());
		}
		if (iy >= yExtent)
		{
			unsigned at = iy - yExtent+1;
			obj.MakeQuadInv((at-1)*2,(at-1)*2+1,(at)*2+1,(at)*2);
		}
	}

	obj.SetVertexOffsetToCurrent();
	for (unsigned ix = 0; ix <= Resolution-xExtent; ix++)
	{
		float fx0 = (float)ix/(Resolution-1) * 2.f - 1.f;
		unsigned iy = yExtent-1;
		float fy = (float)iy/(Resolution-1) * 2.f - 1.f;
		//float fx1 = (float)(ix+1)/(res);


		TVec2<> coords={0,0};
		auto s = surfaceFunction(ix,iy);
		s.xInclination *= range.x.GetExtent();
		s.yInclination *= range.y.GetExtent();
		float t = s.ThicknessToOffsetX(thickness);
		obj.MakeVertex(float3(fx0,fy,vmax(s.height-t,0)) ,s.color, coords);
		{
			float h = s.height;
				//image.get(ix,0)[0];
			obj.MakeVertex(float3(fx0,fy,h),s.color, float2(Relative2TextureHeight(h)));
		}
		if (ix > 0)
			obj.MakeQuad((ix-1)*2,(ix-1)*2+1,(ix)*2+1,(ix)*2);
	}



}



/*static*/ Table::TSurfacePoint			Table::SampleRangePixelAt(SampleType t, index_t ix, index_t iy, index_t at)
{
	TSurfacePoint values[0x100];
	for (index_t i = 0; i < tables.Count(); i++)
	{
		((TSurfacePoint&)values[i]) = tables[i].GetGeometryPoint(t,ix,iy);
		values[i].color = tables[i].color;
	}
	std::sort(values,values+tables.Count());
	return values[at];
}

void					Table::TraceLine(const TIntRange<UINT>&range, const std::function<float3(UINT)>&f)
{
	TLineSegment&seg = this->lineSegments.Append();
	seg.color = color;
	seg.color.rgb /= 2.f;
	seg.width = 4;
	for (UINT x = range.start; x < range.end; x++)
	{
		float3 raw = f(x);
		TVec3<>&rel = seg.points.Append();
		//::range.Relativate(raw,rel);
		//::range.z.Relativate(raw.z,rel.z);
		//rel.z *= 1.1f;
		rel.z = raw.z;
		rel.xy = raw.xy;
		rel.xy /= float(Resolution-1);
		rel.xy *= 2.f;
		rel.xy -= 1.f;
	}

	//lineSegments.Append();
	//lineSegments.Last() = lineSegments.fromEnd(1);
	//lineSegments.Last().width = 2;
	//lineSegments.Last().color.rgb = color.rgb;

}

void					Table::RenderLines() const
{
	foreach (lineSegments,seg)
	{
		glColor4fv(seg->color.v);
		glLineWidth(seg->width);
		glBegin(GL_LINE_STRIP);
			foreach (seg->points,p)
				glVertex3fv(p->v);
		glEnd();
	}
}

void	Table::UpdateCurrentRange(SampleType t)
{
	currentRange.average = 0;
	currentRange.range = MaxInvalidRange<float>();
	count_t numSamples = 0;
	foreach (samples,row)
	{
		numSamples += row->Count();
		foreach (*row,col)
		{
			float h = col->Get(t);
			currentRange.range.Include(h);
			currentRange.average += h;
		}
	}
	currentRange.average /= numSamples;
}


void	Table::SetTransformed(const Table&source, const std::function<TSample(TSample)>&transformFunction, const Math::float4&color)
{
	this->color = color;
	this->samples = source.samples;
	foreach (samples,row)
	{
		foreach (*row,s)
		{
			*s = transformFunction(*s);
		}
	}
	
}


void	Table::UpdatePlotGeometry(SampleType t, bool window)
{
	ASSERT__(tables.Count() < 0x100);
	CGS::Constructor<>	constructor(1,CGS::HasNormalFlag|CGS::HasColorFlag),
						transparentConstructor(1,CGS::HasNormalFlag|CGS::HasColorFlag),
						holeConstructor(1,CGS::HasNormalFlag|CGS::HasColorFlag);

	auto&obj = constructor.AppendObject();
	auto&tobj = transparentConstructor.AppendObject();
	auto&holeShape = holeConstructor.AppendObject();
	lineSegments.Clear();
	if (IsRangeTable)
	{
		#ifdef LIVE_SORT
			foreach (tables,table)
			{
				volatile UINT at = UINT(table - tables.pointer());
				const UINT width = Resolution/2-at*vmax(1,Resolution/32),height = width/8;

				BuildPlotSurface(obj,	[t,this,at,width,height](index_t x, index_t y)
										{
											TSurfaceSample s;
											(TSurfacePoint&)s = SampleRangePixelAt(t,x,y,at);
											s.cover = at == 0 ? true : x > Resolution - width || y < height;
											return s;
										}
										);
				//if (at > 0)
				//	BuildPlotHoleWalls(holeShape,	[t,this,at](index_t x, index_t y){return SampleRangePixelAt(t,x,y,at);},
				//						width,height
				//						);
			}
		#else
			Sorting::ByComparator::QuickSort(tables,[](const Table&a, const Table&b)
			{
				if (a.currentRange.average < b.currentRange.average)
					return -1;
				if (a.currentRange.average > b.currentRange.average)
					return 1;
				return 0;
			});

			foreach (tables,table)
			{
				volatile UINT at = UINT(table - tables.pointer());
				const UINT width = Resolution/2-at*vmax(1,Resolution/32),height = width/8;

				BuildPlotSurface(obj,	[t,this,at,width,height,&table](index_t x, index_t y)
										{
											TSurfaceSample s;
											(TSurfacePoint&)s = table->GetGeometryPoint(t,x,y);
											s.cover = at == 0 ? true : x > Resolution - width || y < height;
											return s;
										}
										);
				//if (at > 0)
				//	BuildPlotHoleWalls(holeShape,	[t,this,at,&table](index_t x, index_t y){return table->GetGeometryPoint(t,x,y);},
				//						width,height
				//						);
			}

		#endif

	}
	else
	{
		if (window)
		{
			const UINT width = Resolution/4,height = width/4;
			BuildPlotSurface(obj,	[t,this,width,height](index_t x, index_t y)
									{
										TSurfaceSample rs; 
										((TSurfacePoint&)rs) = GetGeometryPoint(t,x,y);
										rs.cover = x > Resolution - width || y < height;
										return rs;
									});
			BuildPlotSurface(tobj,	[t,this,width,height](index_t x, index_t y)
									{
										TSurfaceSample rs; 
										((TSurfacePoint&)rs) = GetGeometryPoint(t,x,y);
										rs.color.a = 0.25f;
										rs.cover = !(x > Resolution - width || y < height);
										return rs;
									});

			//BuildPlotHoleWalls(holeShape,	[t,this](index_t x, index_t y)
			//					{
			//						return GetGeometryPoint(t,x,y);
			//					},
			//					width,height
			//					);


			TraceLine(IntRange<UINT>(0,Resolution-width+1),[this,t,height](UINT x)
			{
				return float3(x,height-1,GetGeometryPoint(t,x,height-1).height);
			});
			TraceLine(IntRange<UINT>(height-1,Resolution),[this,t,width](UINT y)
			{
				return float3(Resolution-width+1,y, GetGeometryPoint(t,Resolution-width+1,y).height);
			});
		}
		else
		{
			BuildPlotSurface(obj,	[t,this](index_t x, index_t y)
									{
										TSurfaceSample rs; 
										((TSurfacePoint&)rs) = GetGeometryPoint(t,x,y);
										rs.cover = true;
										return rs;
									});
		}

		TraceLine(IntRange<UINT>(0,Resolution),[this,t](UINT x)
		{
			return float3(x,0,GetGeometryPoint(t,x,0).height);
		});
		//TraceLine(IntRange<UINT>(0,Resolution),[this,t](UINT x)
		//{
		//	return float3(x,Resolution,GetGeometryPoint(t,x,Resolution-1).height);
		//});
		TraceLine(IntRange<UINT>(0,Resolution),[this,t](UINT y)
		{
			return float3(Resolution-1,y, GetGeometryPoint(t,Resolution-1,y).height);
		});
		TraceLine(IntRange<UINT>(0,Resolution),[this,t](UINT y)
		{
			return float3(0,y, GetGeometryPoint(t,0,y).height);
		});

	}
	tobj.ComputeNormals();
	obj.ComputeNormals();
	holeShape.ComputeNormals();

	FinishConstruction(obj,tobj, holeShape);
}

void	Table::FinishConstruction(const CGS::Constructor<>::Object&constructor, const CGS::Constructor<>::Object&transparentConstructor,const CGS::Constructor<>::Object&holeConstructor)
{
	RemovePlotGeometry();

	solidPlotGeometryHandle = scenery.Embed(constructor);
	plotHoleGeometryHandle = holeScenery.Embed(holeConstructor);
	transparentGeometryHandle = transparentScenery.Embed(transparentConstructor);

}

void Table::RemovePlotGeometry()
{
	scenery.Remove(solidPlotGeometryHandle);
	holeScenery.Remove(plotHoleGeometryHandle);
	transparentScenery.Remove(transparentGeometryHandle);
}



void RemoveAllPlotGeometries()
{
	foreach (tables,t)
		t->RemovePlotGeometry();
}

void UpdateColors()
{
	for (index_t i = 0; i < tables.Count(); i++)
		tables[i].color = float4( 1.f - float( i%2 ),1.f - float((i/2)%2),1.f - float((i/4)%2),1.f) * 0.5f + 0.5f;
}

void LoadTables(const Buffer0<FileSystem::File>&files)
{
	RemoveAllPlotGeometries();
	tables.Clear();
	foreach (files,file)
		tables.Append().LoadSamples(*file);
	UpdateColors();
}


void LoadTables(const std::initializer_list<const FileSystem::File>&files)
{
	RemoveAllPlotGeometries();
	tables.Clear();
	foreach (files,file)
		tables.Append().LoadSamples(*file);
	UpdateColors();
}

void UpdateTableRanges(SampleType t)
{
	foreach (tables,table)
	{
		table->UpdateCurrentRange(t);
	}
}


float			GetVerticalRange(SampleType t)
{

	float max = 0;
	foreach (tables,t)
	{
		max = std::max(max,t->currentRange.range.max);
	}
//	if (t == SampleType::DivergenceDepth)
	//	return std::max<float>(max,130);
	return std::max(1.f,max);

	//switch (t)
	//{
	//	case SampleType::DivergenceDepth:
	//		return temporalRange;
	//	case SampleType::Inconsistency:
	//	case SampleType::InconsistencyDelta:
	//	case SampleType::InconsistencyHypothesis:
	//	case SampleType::ComputedConsistencyDelta:
	//	case SampleType::ConsistencyDeltaHypothesis:
	//		return 1.f;
	//	case SampleType::EntityCount:
	//		return 1000000;
	//	case SampleType::LocationError:
	//	case SampleType::LocationErrorHypothesis:
	//		return 20.f;
	//	case SampleType::MissingEntities:
	//		return 1.f;
	//}
	//return 1.f;
}

