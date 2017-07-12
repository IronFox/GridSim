#pragma once

#include <global_string.h>
#include <container/buffer.h>
#include "hypothesis.h"

CONSTRUCT_ENUMERATION10(SampleType, Inconsistency, 
									InconsistencyHypothesis, 
									InconsistencyDelta, 
									ComputedConsistencyDelta, 
									ConsistencyDeltaHypothesis, 
									DivergenceDepth,
									LocationError,
									LocationErrorHypothesis,
									MissingEntities,
									EntityCount
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

	void		Include(const TSample&other);

	float		Get(SampleType) const;

	template <typename T>
	void		ConvertOptional(const D::StringRef&str, T&rsOut)
	{
		if (str.GetLength() > 0)
			ASSERT1__(D::convert(str.pointer(),str.length(),rsOut),str);
	}

	void		Decode(const D::StringRef&tot, const D::StringRef&inc, const D::StringRef&newlyInc, const D::StringRef&locationError, const D::StringRef&missingEntities, const D::StringRef&inconsistencyAge)
	{
		ASSERT1__(D::convert(tot.pointer(),tot.length(),numEntities),tot);
		ASSERT1__(D::convert(inc.pointer(), inc.length(), inconsistentEntities), inc);
		ASSERT1__(D::convert(newlyInc.pointer(), newlyInc.length(), newlyInconsistentEntities), inc);
		ConvertOptional(locationError,this->locationError);
		ConvertOptional(missingEntities,this->missingEntities);
		ConvertOptional(inconsistencyAge,this->inconsistencyAge);
		ASSERT_LESS_OR_EQUAL__(inconsistentEntities,numEntities);
	}
};
