#pragma once

#include <global_string.h>
#include <container/buffer.h>
#include <string/converter.h>
#include "hypothesis.h"

CONSTRUCT_ENUMERATION6(SampleType, Inconsistency, 
									InconsistencyDelta, 
									DivergenceDepth,
									LocationError,
									MissingEntities,
									LiveSamples
									);


extern Hypothesis hypothesis;


struct TSample
{
	UINT64		numEntities = 0,
				inconsistentEntities = 0,
				newlyInconsistentEntities = 0,
				missingEntities = 0,
				inconsistencyAge = 0
				;
	double		locationError = 0;

	float		GetInconsistentEntities() const {return inconsistentEntities ? float(inconsistentEntities) / float(numEntities) : 0.f; }
	float		GetDelta(const TSample&other) const {return GetInconsistentEntities() - other.GetInconsistentEntities(); }

	/**
	Calculates and loads the arithmetic average of the specified two samples
	*/
	void		LoadAverage(const TSample&a,const TSample&b)
	{
		numEntities = (a.numEntities + b.numEntities)/2;
		inconsistentEntities = (a.inconsistentEntities + b.inconsistentEntities)/2;
		newlyInconsistentEntities = (a.newlyInconsistentEntities + b.newlyInconsistentEntities)/2;
		missingEntities = (a.missingEntities + b.missingEntities)/2;
		inconsistencyAge = (a.inconsistencyAge + b.inconsistencyAge)/2;
		locationError = (a.locationError + b.locationError)/2;
	}

	void		Include(const TSample&other);
	void		IncludeMean(const TSample&other);

	float		Get(SampleType) const;

	template <typename T>
	void		ConvertOptional(const StringRef&str, T&rsOut)
	{
		if (str.GetLength() > 0)
			ASSERT1__(Convert(str,rsOut),str);
	}

	void		Decode(const StringRef&tot, const StringRef&inc, const StringRef&newlyInc, const StringRef&locationError, const StringRef&missingEntities, const StringRef&inconsistencyAge)
	{
		ASSERT1__(Convert(tot,numEntities),tot);
		ASSERT1__(Convert(inc, inconsistentEntities), inc);
		ConvertOptional(newlyInc,this->newlyInconsistentEntities);
		ConvertOptional(locationError,this->locationError);
		ConvertOptional(missingEntities,this->missingEntities);
		ConvertOptional(inconsistencyAge,this->inconsistencyAge);
		ASSERT_LESS_OR_EQUAL__(inconsistentEntities,numEntities);
	}

private:
	float		_Get(SampleType) const;

};
