#ifndef thread_granularity_optimizerH
#define thread_granularity_optimizerH


#include "thread_system.h"
#include "timer.h"
#include "../math/basic.h"


class ThreadGranularityOptimizer
{
protected:
		Timer::Time	started;
		
		static const unsigned Resolution = 50;
		static const unsigned RequireSamples = 10;	//samples per location to resolve results
		
		Timer::Time	time_sum[Resolution];
		unsigned		location[Resolution];
		
		unsigned		focus,
						current_samples;
		
		void			updateLocations();
		
public:
						ThreadGranularityOptimizer(unsigned initial_job_size=128)
						{
							reset(initial_job_size);
						}
		
		void			reset(unsigned initial_job_size);
		unsigned		begin();
		void			end(unsigned iterations, const TCodeLocation&location);
};





#endif
