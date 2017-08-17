#include <global_root.h>
#include "table.h"
#include "rendering.h"
#include <math/graph.h>
#include <container/queue.h>


Buffer0<Table,Swap>	tables;
Table rangeTable(true);

static float Lerp(float v0, float v1, float x)
{
	return v0 * (1.f - x) + v1 * x;
}


const TSample& Table::GetSample(index_t spatialDistance, index_t temporalDistance) const
{
	static const TSample empty;


	if (temporalDistance >= samples.Count())
		return empty;
	const auto&row = samples[temporalDistance];
	if (spatialDistance >= row.Count())
		return empty;
	return row[spatialDistance];
}

float	Table::FindLevelSpatialDistance(SampleType t, index_t temporalDistance, float level) const
{
	index_t left = 0;
	index_t right = samples[1].Count();
	if (!right)
		return -1;
	right--;
	float leftH = this->GetSample(left,temporalDistance).Get(t);
	float rightH = GetSample(right,temporalDistance).Get(t);
	if (leftH < level == rightH < level)
		return -1;


	for (int i = 0; i < 10; i++)
	{
		index_t middle = (left+right)/2;
		float middleH = GetSample(middle,temporalDistance).Get(t);
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

	std::array<M::TVec2<double>,TAgeHypothesis::SNCoef>	samplePoints;
	for (index_t at = 0; at < TAgeHypothesis::SNCoef; at++)
	{
		const float x = smooth ?
									M::CubicFactor(float(at)/(TAgeHypothesis::SNCoef-1))*effectiveWidth:
									float(at)*effectiveWidth/(TAgeHypothesis::SNCoef-1);
		const float y0 = row[floor(x)].Get(SampleType::DivergenceDepth)/max;
		const float y1 = row[ceil(x)].Get(SampleType::DivergenceDepth)/max;
		const float y = Lerp(y0,y1,M::frac(x));

		float fx = float(x)/raw.metrics.spatialSupersampling*xScale;
		samplePoints[at] = M::TVec2<double>{fx,y};
	}
	ASSERT__(poly.MatchPoints(samplePoints));
}

M::float3 AverageSpatialPoint(const Table&raw,SampleType t,index_t row, float z, float zRadius)
{
	M::float3 rs;
	count_t numSamples = 0;
	for (int x = -5; x <= 5; x++)
		for (int y = -4; y <= 4; y++)
		{
			M::float3 p(0,row+x,z+zRadius*y/4);
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
			std::array<M::TVec2<double>,TAgeHypothesis::TNCoef>	samplePoints;

			age.temporalLinearBreak = i;
			float last = 0;
			index_t lastX = 0;
			for (index_t at = 0; at < TAgeHypothesis::TNCoef; at++)
			{
				const index_t x = //::vmin(age.maxTemporalDistance,pow(float(at)/(TAgeHypothesis::TNCoef-1),1.2f)*age.maxTemporalDistance);
								at*age.temporalLinearBreak/(TAgeHypothesis::TNCoef-1);
				const auto&row = raw.samples[x];
				const float y = row.IsNotEmpty()? row.First().Get(SampleType::DivergenceDepth) : 0;

				samplePoints[at] = M::TVec2<double>{double(x),y};
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
				err += M::sqr(measured - synthetic) * 1.f/float(pow(j,1.4f));
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
		Buffer0<M::TVec3<double> > points;
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
		M::float3 p1 = AverageSpatialPoint(raw,SampleType::DivergenceDepth,0.9f * age.maxTemporalDistance,lowZ,max*0.15f);
		M::float3 p0 = AverageSpatialPoint(raw,SampleType::DivergenceDepth,0.9f * age.maxTemporalDistance,highZ,max*0.15f);
		M::float3 p2 = AverageSpatialPoint(raw,SampleType::DivergenceDepth,middleY,lowZ,max*0.1f);

		//const float lowX = raw.FindLevelSpatialDistance(SampleType::DivergenceDepth,age.maxTemporalDistance,lowZ)/raw.metrics.spatialSupersampling;
		//const float highX = raw.FindLevelSpatialDistance(SampleType::DivergenceDepth,age.maxTemporalDistance,highZ)/raw.metrics.spatialSupersampling;
		//const float middleX = raw.FindLevelSpatialDistance(SampleType::DivergenceDepth,middleY,lowZ)/raw.metrics.spatialSupersampling;

		//float3 p[3],n;
		//p[1] = float3(lowX,age.maxTemporalDistance,lowZ);
		//p[0] = float3(highX,age.maxTemporalDistance,highZ);
		//p[2] = float3(middleX,middleY,lowZ);
		M::float3 normal;
		M::Obj::triangleNormal(p0,p1,p2,normal);
		M::Vec::normalize(normal);
		float w = M::Vec::dot(normal,(p0+p1+p2)/3);
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
	const auto M::Max = amax.ToVector();

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
					float at = Math::clamp( base +kf*step,vmin[axis],M::Max[axis]);

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
	index_t step = Math::Max(1,mean.GetLength()-50);
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


M::float3	Table::TrainFlatOverestimation(index_t sourceTableID, SampleType t,float threshold)
{
	const auto& source = tables[sourceTableID];


	const float farSpatialBegin = source.FindLevelSpatialDistance(t,source.samples.Count()-1,threshold);
	const float middleSpatialBegin = source.FindLevelSpatialDistance(t,source.samples.Count()/2,threshold);
	const float closeSpatialBegin = source.FindLevelSpatialDistance(t,source.samples.Count()/4,threshold);

	M::float3			linearPlaneFunc;	//x: spatial,y: temporal inclination, z: offset

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
		//float h = M::Max(0.f, (linearPlaneFunc.z + linearPlaneFunc.x * spatialDistance + linearPlaneFunc.y * temporalDistance));
		//return h;
		//if (h == 0)
		//	return 0;
		const auto& source = tables[sourceTableID];
		return source.GetSmoothed(spatialDistance,temporalDistance,t) * 1.1f;
	};

	this->colorFunction = [linearPlaneFunc,sourceTableID](float spatialDistance, float temporalDistance, SampleType t) -> M::float4
	{
		const float h = M::Max(0.f, (linearPlaneFunc.z + linearPlaneFunc.x * spatialDistance + linearPlaneFunc.y * temporalDistance));
		const auto& source = tables[sourceTableID];
		const float s = source.GetSmoothed(spatialDistance,temporalDistance,t);

		//float level = s/h;
		M::float4 rs;
		rs.a = 1.f;
		M::Vec::HSL2RGB(M::float3(fmod(h*20.f,1.f),1.f,0.5f),rs.rgb);
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

	this->colorFunction = [hypothesis](float spatialDistance, float temporalDistance, SampleType t) -> M::TVec4<>
	{
		const float shouldBe = tables[hypothesis.sourceTableID].SmoothGeometryHeightFunctionF(t,spatialDistance,temporalDistance);
		const float error = (shouldBe-hypothesis.Sample(spatialDistance,temporalDistance,t));
		//rs.height = fabs(shouldBe-rs.height);
		M::TVec4<> rs;
		rs.a = 1;
		rs.r = 1;
		rs.gb = 1.f - M::float2(fabs(error)*50);

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
		return M::Clamp01(s0);
	const float s1 = spatialProfile1(spatialDistance);
	return M::Clamp01(Lerp(s0,s1,(temporalDistance-spatialProfile0TAt)/(maxTemporalDistance - spatialProfile0TAt)));

	//return M::Clamp01(spatialProfile(spatialDistance));
}



float TStaticAgeHypothesis::Sample(float spatialDistance, float temporalDistance, float density, float relativeSensorRange) const
{
	M::float3 linearPlaneFunc(pX(density,relativeSensorRange),1,pBase(density,relativeSensorRange));  
	float h = linearPlaneFunc.x * spatialDistance + linearPlaneFunc.y * temporalDistance + linearPlaneFunc.z;
	return M::Max(h,0.f);
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
		return M::Max(h,0.f);
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
			error += M::sqr(Sample(spatial,age));
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

//float Table::Get(index_t spatialDistance, index_t temporalDistance, SampleType t) const
//{
//	if (heightFunction)
//	{
//		float fs = spatialDistance;
//		float ft = temporalDistance;
//		return heightFunction(fs,ft,t);
//	}
//	return GetSample(spatialDistance,temporalDistance).Get(t);
//}

float				TInclinedSurfacePoint::ThicknessToOffsetX(float t) const
{
	float maxInclination = fabs(xInclination);
	float rs = t / sqrt(M::sqr(maxInclination)+1) 
			+ maxInclination * (t * sqrt(1.f - 1.f / (M::sqr(maxInclination)+1)));
	DBG_ASSERT_GREATER_OR_EQUAL__(rs,t);
	return rs;
}

float				TInclinedSurfacePoint::ThicknessToOffsetY(float t) const
{
	float maxInclination = fabs(yInclination);
	float rs = t / sqrt(M::sqr(maxInclination)+1) 
			+ maxInclination * (t * sqrt(1.f - 1.f / (M::sqr(maxInclination)+1)));
	DBG_ASSERT_GREATER_OR_EQUAL__(rs,t);
	return rs;
}


void	Table::StretchSamplesS()
{
	foreach (samples,row)
	{
		if (row->Count() <= 1)
			continue;

		SampleRow newRow;
		newRow.AppendRow(row->Count() * 2 -1);
		for (index_t i = 0; i < row->Count(); i++)
			newRow[i*2] = row->at(i);
		for (index_t i = 0; i+1 < row->Count(); i++)
			newRow[i*2+1].LoadAverage( row->at(i),row->at(i+1));
		row->swap(newRow);
	}

}

void Table::TransposeSamples()
{
	Buffer0<Buffer0<TSample> >	newSamples;
	
	count_t numRows = 0;
	foreach (samples,s)
		numRows = M::Max(numRows,s->Count());
	newSamples.AppendRow(numRows+1);
	for (index_t i = 1; i < newSamples.Count(); i++)
		newSamples[i].AppendRow(samples.Count()-1);

	for (index_t i = 1; i < samples.Count(); i++)
	{
		const auto&row = samples[i];
		for (index_t j = 0; j < row.Count(); j++)
		{
			newSamples[j+1][i-1] = row[j];
		}
	}
	samples.swap(newSamples);
}


TSample	Table::AccumulateAtR(index_t iRow, index_t iCol, index_t r) const
{

	TSample rs;
	{
		const index_t ibegin = iRow > r ? iRow - r : 1;
		const index_t iend = iRow+r < samples.Count() ? iRow+r : samples.Count()-1;

		for (index_t i = ibegin; i <= iend; i++)
		{
			const auto&src = samples[i];

			if (iCol >= r)
				rs.Include(src[iCol-r]);
			if (r != 0 && iCol + r < src.Count())
				rs.Include(src[iCol+r]);
		}
	}
	if (r > 0)
	{
		const index_t ibegin = iRow > r ? iRow - r : 1;
		const index_t iend = iRow+r < samples.Count() ? iRow+r : samples.Count()-1;



		if (iRow > r)
		{
			const auto&row = samples[iRow - r];
			if (row.IsNotEmpty())
			{
				const index_t jbegin = iCol >= r ? iCol - r+1 : 0;
				const index_t jend = std::min(iCol+r - 1, row.Count()-1);
				for (index_t j = jbegin; j <= jend; j++)
					rs.Include(row[j]);
			}
		}
		if (iRow+r < samples.Count())
		{
			const auto&row = samples[iRow + r];
			if (row.IsNotEmpty())
			{
				const index_t jbegin = iCol >= r ? iCol - r+1 : 0;
				const index_t jend = std::min(iCol+r - 1, row.Count()-1);
				for (index_t j = jbegin; j <= jend; j++)
					rs.Include(row[j]);
			}
		}
	}


	return rs;
}

struct NodeAttachment : public M::Graph::NodeAttachment
{
	Table::Mesh::TVertex	vertex;
	bool		mergeable;
	index_t		collapseLevel = 0;

	/**/		NodeAttachment(const Table::Mesh::TVertex&vtx, bool mergeable, index_t collapseLevel=0):vertex(vtx),mergeable(mergeable),collapseLevel(collapseLevel)
	{}
};


void	Table::Blur(const Table&source)
{
	sourceFile = source.sourceFile;
	metrics = source.metrics;

	count_t maxSamples = 0;
	count_t sSize = 0;
	foreach (source.samples,row)
	{
		foreach (*row,s)
			maxSamples = Math::Max(maxSamples,s->numEntities);
		sSize = M::Max(sSize,row->Count());
	}

	const count_t tSize = source.samples.Count();



	const count_t minSamples = maxSamples / 100;

	M::Graph graph;
//	Array2D<M::Graph::PNode>	nodeGrid(sSize,tSize);
	PriorityQueue<M::Graph::WNode,UINT64>	queue;

	static const count_t testSize = 5;
	for (index_t s = 0; s < testSize; s++)
		for (index_t t = 0; t < testSize; t++)
		{
			auto n = graph.AddNode();
	//		nodeGrid.Get(s,t) = n;
			const auto&sample = source.GetSample(s,t);
			bool mergeable = sample.numEntities < minSamples;
			Mesh::TVertex vtx;
			vtx.sample = sample;
			vtx.st = M::float2(float(s) / (testSize-1),float(t)/(testSize-1));
			n->attachment.reset(new NodeAttachment(vtx,mergeable));
		}


	#if 0
	{

	for (index_t s = 0; s < sSize; s++)
		for (index_t t = 0; t < tSize; t++)
		{
			auto n = graph.AddNode();
			nodeGrid.Get(s,t) = n;
			const auto&sample = source.GetSample(s,t);
			bool mergeable = sample.numEntities < minSamples;
			Mesh::TVertex vtx;
			vtx.sample = sample;
			vtx.st = M::float2(float(s) / (sSize-1),float(t)/(tSize-1));
			n->attachment.reset(new NodeAttachment(vtx,mergeable));
			if (mergeable)
				queue.Push(n,sample.numEntities);
		}

	for (index_t s = 1; s+1 < sSize; s++)
	{
		if (s+2 < sSize)
		{
			graph.AddEdge(nodeGrid.Get(s,0),nodeGrid.Get(s+1,0));
			graph.AddEdge(nodeGrid.Get(s,tSize-1),nodeGrid.Get(s+1,tSize-1));
		}

		for (index_t t = 1; t+1 < tSize; t++)
		{
			if (s+2 < sSize)
				graph.AddEdge(nodeGrid.Get(s,t),nodeGrid.Get(s+1,t));
			if (t+2 < tSize)
			{
				if (s+2 < sSize)
				{
					graph.AddEdge(nodeGrid.Get(s,t),nodeGrid.Get(s+1,t+1));
					graph.AddEdge(nodeGrid.Get(s+1,t),nodeGrid.Get(s,t+1));
				}
				graph.AddEdge(nodeGrid.Get(s,t),nodeGrid.Get(s,t+1));
			}
		}
	}

	for (index_t t = 1; t+2 < tSize; t++)
	{
		graph.AddEdge(nodeGrid.Get(0,t),nodeGrid.Get(0,t+1));
		graph.AddEdge(nodeGrid.Get(sSize-1,t),nodeGrid.Get(sSize-1,t+1));
	}

	//if (false)
	{
		M::Graph::WNode n;
		while (queue.PopLeast(n))
		{
			auto node = n.lock();
			if (!node)
				continue;

			M::Graph::PNode candidate;
			const NodeAttachment*candidateAttachment = nullptr;

			const NodeAttachment&a = *(const NodeAttachment*)node->attachment.get();
			node->VisitAllNeighbors([&a,&candidate,&candidateAttachment,node](const M::Graph::PNode&n2)
			{
				const NodeAttachment&b = *(const NodeAttachment*)n2->attachment.get();
				if (!b.mergeable || n2 == node)
					return;
				if (!candidateAttachment || b.vertex.sample.numEntities < candidateAttachment->vertex.sample.numEntities)
				{
					candidate = n2;
					candidateAttachment = &b;
				}
			});
			if (candidate)
			{
				M::Graph::PNode merged = graph.MergeNodes(node,candidate);
				Mesh::TVertex vtx;

				vtx.sample = a.vertex.sample;
				vtx.sample.Include(candidateAttachment->vertex.sample);
				M::float2 mergedAt = a.vertex.st * a.vertex.sample.numEntities + candidateAttachment->vertex.st * candidateAttachment->vertex.sample.numEntities;
				if (vtx.sample.numEntities != 0)
					mergedAt /= vtx.sample.numEntities;
				else
					mergedAt = (a.vertex.st + candidateAttachment->vertex.st) * 0.5f;
				const bool mergedIsMergeable = vtx.sample.numEntities < minSamples;
				const index_t mergedCollapseLevel = std::max(a.collapseLevel , candidateAttachment->collapseLevel) + 1;
				merged->attachment.reset(new NodeAttachment(vtx,mergedIsMergeable,mergedCollapseLevel));
				if (mergedIsMergeable)
					queue.Push(merged,vtx.sample.numEntities);
			}
		}
	}
	}
	#endif /*0*/

	M::Delaunay2D delaunay;
	Array<M::Graph::PNode>	graphNodes;
	graph.ExportNodes(graphNodes);

	for (index_t i = 0; i < graphNodes.Count(); i++)
		delaunay.Add(((const NodeAttachment*)( graphNodes[i]->attachment.get()))->vertex.st);
	delaunay.Triangulate();

	mesh.vertices.SetSize(graphNodes.Count());
	for (index_t i = 0; i < mesh.vertices.Count(); i++)
	{
		const NodeAttachment&a = *(const NodeAttachment*)graphNodes[i]->attachment.get();
		auto&v = mesh.vertices[i];
		v = a.vertex;
		v.color = M::float4(1);
	}
	mesh.triangles = delaunay.GetTriangles();
	delaunay.GetOpenEdges(mesh.edges);

	


	return;





	samples.AppendRow(source.samples.Count());
	for (index_t i = 1; i < samples.Count(); i++)
	{
		auto&dest = samples[i];
		const auto&src = source.samples[i];
		dest.AppendRow(src.Count());
		for (index_t j = 0; j < src.Count(); j++)
		{
			auto&s = dest[j];

			s = TSample{};

			for (index_t r = 0; r <= 2; r++)
			{
				s.Include(source.AccumulateAtR(i,j, r));
				if (s.numEntities*100 >= maxSamples)
					break;
			}
		}
	}

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

	const bool isCSV = f.GetExtension() == "csv";

	bool hasLocationErrorSum = false;
	bool hasMissingEntites = false;
	bool hasInconsistencyAge = false;

	while (file >> line)
	{
		if (isCSV)
		{
			explode(',',line,parts);

			ASSERT__((parts.Count() % 6) == 0);
			const count_t numSamples = parts.Count() / 6;

			Buffer0<TSample>&myLine = samples.Append();
			for (index_t i = 0; i < numSamples; i++)
			{
				myLine.Append().Decode(parts[i*6+5],parts[i*6+1],StringRef(),parts[i*6+3],StringRef(),StringRef());
			}

			continue;
		}


		if (CharFunctions::isalpha(line.FirstChar()))
		{
			//attribute or metric
			index_t eqAt = line.Find('=');
			if (eqAt == InvalidIndex)
			{
				std::cout << "File contains malformatted metric '"<<line<<"'. Ignoring."<<std::endl;
				continue;
			}
			const StringRef svalue = line.SubStringRef(eqAt+1);

			count_t value;
			float fvalue;
			bool haveInt = convert(svalue.pointer(),svalue.GetLength(),value);
			bool haveFloat = convert(svalue.pointer(),svalue.GetLength(),fvalue);
			StringRef metric = line.SubStringRef(index_t(0),eqAt);
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


		if (line.BeginsWith('[') && line.EndsWith(']'))	//flags
		{
			if (mFilter && !mFilter(metrics))
				return false;
			explode(' ',line.GetBetweenRef('[',']'),parts);
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
		Buffer0<TSample>&myLine = samples.Append();

		count_t numParts = 3 + hasLocationErrorSum + hasMissingEntites + hasInconsistencyAge;

		for (index_t i = 0; i*numParts+numParts < parts.Count(); i++)
		{
			const auto&total = parts[i*numParts+1];
			const auto&inc = parts[i * numParts + 2];
			const auto&newlyInc = parts[i*numParts+3];
			myLine.Append().Decode(total,inc,newlyInc
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

	if (isCSV)
	{
		TransposeSamples();
		StretchSamplesS();
	}


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

	float atX = range.y.Derelativate( spatialDistance );
	float atY = range.x.Derelativate( temporalDistance );


	float s00,s01,s11,s10;

	const index_t iX = (index_t)floor(atX);
	const index_t iY = (index_t)floor(atY);

	s00 = GetSample(iX,iY).Get(t);
	s01 = GetSample(iX+1,iY).Get(t);
	s10 = GetSample(iX, iY+1).Get(t);
	s11 = GetSample(iX + 1, iY+1).Get(t);
	float s0 = Interpolate(s00,s01,M::frac(atX));
	float s1 = Interpolate(s10,s11,M::frac(atX));
	return Interpolate(s0,s1,M::frac(atY));
}


float Table::SmoothGeometryHeightFunctionF(SampleType t, float spatialDistance, float temporalDistance) const
{
	return range.z.Relativate(GetSmoothed(spatialDistance,temporalDistance,t));
}


float Relative2TextureHeight(float h)
{
	return h * 1.1f;
}




TSurfacePoint	Table::GetGeometryPointF(SampleType t, float spatialDistance, float temporalDistance) const
{
	TSurfacePoint rs;
	rs.height =  SmoothGeometryHeightFunctionF(t,spatialDistance,temporalDistance);

	if (colorFunction)
		rs.color = colorFunction(spatialDistance,temporalDistance,t);
	else
		rs.color = color;

	return rs;
}

/*static*/ void	Table::BuildRegularSurface(CGS::Constructor<>::Object&obj,const FSurface&surfaceFunction, count_t resolution)
{
	obj.SetVertexOffsetToCurrent();

	for (unsigned ix = 0; ix < resolution; ix++)
	{
		const float fx0 = (float)ix/(resolution-1);
		for (unsigned iy = 0; iy < resolution; iy++)
		{
			float fy = (float)iy/(resolution-1);
			float fx = fx0;
			const auto s = surfaceFunction(fy,fx);

			fx = fx * 2.f - 1.f;
			fy = fy * 2.f - 1.f;

			{
				float h = s.height;
				obj.MakeVertex(M::float3(fx,fy,h),s.color,M::float2(Relative2TextureHeight(h)));
			}
			if (ix && iy && s.cover)
				obj.MakeQuadInv( (ix-1)*resolution + (iy-1), (ix-1)*resolution + (iy),  (ix)*resolution + (iy),  (ix)*resolution + (iy-1));
		}
	}
}


/*static*/ void	Table::BuildMeshSurface(CGS::Constructor<>::Object&obj,const Mesh&mesh, SampleType t)
{
	obj.SetVertexOffsetToCurrent();

	foreach (mesh.vertices,v)
	{
		float x = v->st.x * 2.f - 1.f;
		float y = v->st.y * 2.f - 1.f;
		float h = v->sample.Get(t);
		obj.MakeVertex(M::float3(x,y,h),v->color,M::float2(Relative2TextureHeight(h)));
	}

	foreach (mesh.triangles,t)
		obj.MakeTriangle(t->v[0],t->v[1],t->v[2]);
}


#if 0
/*static*/	void				Table::BuildPlotHoleWalls(CGS::Constructor<>::Object&obj,const std::function<TInclinedSurfacePoint(float&,float&)>&surfaceFunction, UINT xExtent, UINT yExtent, float thickness /*= 0.03f*/)
{
	obj.SetVertexOffsetToCurrent();
	for (unsigned iy = yExtent-1; iy < Resolution; iy++)
	{
		unsigned ix = Resolution-xExtent;
		float fy = (float)iy/(Resolution-1);
		float fx = (float)ix/(Resolution-1);
		auto s = surfaceFunction(fy,fx);
		fx = fx * 2.f - 1.f;
		fy = fy * 2.f - 1.f;
		s.xInclination *= range.x.GetExtent();
		s.yInclination *= range.y.GetExtent();

		{
//			float h = image.get(res-1,iy)[0];
			float h = s.height;
			obj.MakeVertex(M::float3(fx,fy,h),s.color, M::float2(Relative2TextureHeight(h)));
		}
		{
			float t = s.ThicknessToOffsetY(thickness);

			obj.MakeVertex(M::float3(fx,fy,M::Max(s.height-t,0)),s.color,M::float2());
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
		unsigned iy = yExtent-1;
		//float fx1 = (float)(ix+1)/(res);


		M::TVec2<> coords={0,0};
		float fx0 = (float)ix/(Resolution-1);
		float fy = (float)iy/(Resolution-1);
		auto s = surfaceFunction(fy,fx0);
		fy = fy*2.f - 1.f;
		fx0 = fx0*2.f - 1.f;
		s.xInclination *= range.x.GetExtent();
		s.yInclination *= range.y.GetExtent();
		float t = s.ThicknessToOffsetX(thickness);
		obj.MakeVertex(M::float3(fx0,fy,M::Max(s.height-t,0)) ,s.color, coords);
		{
			float h = s.height;
				//image.get(ix,0)[0];
			obj.MakeVertex(M::float3(fx0,fy,h),s.color, M::float2(Relative2TextureHeight(h)));
		}
		if (ix > 0)
			obj.MakeQuad((ix-1)*2,(ix-1)*2+1,(ix)*2+1,(ix)*2);
	}



}

#endif /*0*/

///*static*/ Table::TSurfacePoint			Table::SampleRangePixelAtF(SampleType t, float&spatialDistance, float&temporalDistance, index_t at)
//{
//	TSurfacePoint values[0x100];
//	for (index_t i = 0; i < tables.Count(); i++)
//	{
//		((TSurfacePoint&)values[i]) = tables[i].GetGeometryPointF(t,ix,iy);
//		values[i].color = tables[i].color;
//	}
//	std::sort(values,values+tables.Count());
//	return values[at];
//}

void					Table::TraceLineF(const M::TFloatRange<>&range, const std::function<M::float3(float)>&f, count_t stResolution)
{
	TLineSegment&seg = this->lineSegments.Append();
	seg.color = color;
	seg.color.rgb /= 2.f;
	seg.width = 4;

	count_t res = std::max<count_t>(2,range.GetExtent() * stResolution);

	for (index_t i = 0; i < res; i++)
	{
		const float fx = range.Derelativate( float(i)/(res-1) );
		M::TVec3<>&rel = seg.points.Append();
		rel = f(fx);
		rel.xy *= 2.f;
		rel.xy -= 1.f;
	}
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
	currentRange.range = M::MaxInvalidRange<float>();
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

	if (mesh.IsNotEmpty())
	{
		BuildMeshSurface(obj,mesh,t);
	}
	else
	{
		if (IsRangeTable)
		{
			#ifdef LIVE_SORT
				foreach (tables,table)
				{
					volatile UINT at = UINT(table - tables.pointer());
					const UINT width = Resolution/2-at*M::Max(1,Resolution/32),height = width/8;

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
					const UINT at = UINT(table - tables.pointer());
					const float width = 0.5f-1.f/32.f* at,height = 1.f/8.f;
				

					BuildRegularSurface(obj,	[t,this,at,width,height,&table](float spatialDistance, float temporalDistance)
											{
												TSurfaceSample s;
												(TSurfacePoint&)s = table->GetGeometryPointF(t,spatialDistance,temporalDistance);
												s.cover = at == 0 ? true : temporalDistance > 1.f - width || spatialDistance < height;
												return s;
											},400
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
				FATAL__("Window not supported anymore");
			}
			else
			{
				BuildRegularSurface(obj,	[t,this](float spatialDistance, float temporalDistance)
										{
											TSurfaceSample rs; 
											((TSurfacePoint&)rs) = GetGeometryPointF(t,spatialDistance,temporalDistance);
											rs.cover = true;
											return rs;
										},
					400
			
				);
			}

			TraceLineF(M::FloatRange(0.f,1.f),[this,t](float x)
			{
				float y = 0.f;
				float h = GetGeometryPointF(t,y,x).height;
				return M::float3(x,y,h);
			},400);
			//TraceLine(IntRange<UINT>(0,Resolution),[this,t](UINT x)
			//{
			//	return float3(x,Resolution,GetGeometryPoint(t,x,Resolution-1).height);
			//});
			TraceLineF(M::FloatRange(0.f,1.f),[this,t](float y)
			{
				float x = 1.f;
				float h = GetGeometryPointF(t,y,x).height;
				return M::float3(x,y,h);
			},400);
			TraceLineF(M::FloatRange(0.f,1.f),[this,t](float y)
			{
				float x = 0.f;
				float h = GetGeometryPointF(t,y,x).height;
				return M::float3(x,y,h);
			},400);

		}
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
		tables[i].color = M::float4( 1.f - float( i%2 ),1.f - float((i/2)%2),1.f - float((i/4)%2),1.f) * 0.5f + 0.5f;
}

void LoadTables(const Buffer0<FileSystem::File>&files, bool addBlurredClone)
{
	RemoveAllPlotGeometries();
	tables.Clear();
	foreach (files,file)
	{
		tables.Append().LoadSamples(*file);

		if (addBlurredClone)
		{
			auto&filtered = tables.Append();
			filtered.Blur(tables.GetFromEnd(1));
		}
	}
	UpdateColors();
}


void LoadTables(const std::initializer_list<const FileSystem::File>&files, bool addBlurredClone)
{
	RemoveAllPlotGeometries();
	tables.Clear();
	foreach (files,file)
	{
		tables.Append().LoadSamples(*file);

		if (addBlurredClone)
		{
			auto&filtered = tables.Append();
			filtered.Blur(tables.GetFromEnd(1));
		}
	}
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

