#include <global_root.h>
#include "sample.h"
#include "hypothesis.h"
#include <io/string_file.h>


Hypothesis hypothesis;



float		TSample::Get(SampleType t) const
{ 
	switch (t)
	{
		case SampleType::DivergenceDepth:
		{
			const count_t living = GetLivingEntities();
			return inconsistencyAge ? float(inconsistencyAge) / float(living) : 0.f;
		}
		case SampleType::Inconsistency:
		{
			const count_t living = GetLivingEntities();
			return (inconsistentEntities ? float(inconsistentEntities) / float(living) : 0.f);
		}
		case SampleType::InconsistencyDelta:
			return newlyInconsistentEntities ? float(newlyInconsistentEntities) / float(numEntities) : 0.f;
		case SampleType::EntityCount:
			return (float)numEntities;
		case SampleType::LocationError:
		{
			const count_t living = GetLivingEntities();
			return living > 0 ? locationError / living : 0;
		}
		//case SampleType::LocationErrorHypothesis:
		//	return numEntities > 0 ? pow(float(inconsistencyAge) / float(numEntities),0.5) : 0;
		case SampleType::BadnessEstimation:
		{
			const float loc = Get(SampleType::LocationError);
			return loc;
			//const float m = Get(SampleType::MissingEntities);
			//const float p = 1.f - m;
			//	//Get(SampleType::Inconsistency);
			//return loc + loc / p * m;
		}
		case SampleType::MissingEntities:
		{
			return numEntities > 0 ? float(missingEntities) / numEntities: 0;
		}
	}
	return 0.f;
}
//float		TSample::Get() const { return newlyInconsistentEntities ? float(newlyInconsistentEntities) / float(numEntities) : 0.f; }
//float		TSample::Get() const {return numSamples ? 0.1f+float(inconsistentEntities) / float(numEntities) : 0.f;}
//float		TSample::Get() const { return (float)numEntities / 10000; }
//float		TSample::Get() const {return (float)numSamples/10000;}
//float		TSample::Get() const {return numSamples <100 ? 0 : (inconsistentEntities == 0 ? 0.01f : 1.f);}


void		TSample::Include(const TSample&other)
{
	numEntities += other.numEntities;
	inconsistentEntities += other.inconsistentEntities;
	newlyInconsistentEntities += other.newlyInconsistentEntities;
	missingEntities += other.missingEntities;
	locationError += other.locationError;

}

