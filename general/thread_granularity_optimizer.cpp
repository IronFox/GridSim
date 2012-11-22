#include "../global_root.h"
#include "thread_granularity_optimizer.h"



void			ThreadGranularityOptimizer::updateLocations()
{
	unsigned	min = focus>>2,
				max = focus<<2;
	for (unsigned i = 0; i < Resolution; i++)
	{
		float at = (float)i/(Resolution-1);
		location[i] = (unsigned)round((float)min + (float)(max-min)*at);
		time_sum[i] = 0;
	}


}

void			ThreadGranularityOptimizer::reset(unsigned initial_job_size)
{
	focus = initial_job_size;
	updateLocations();
}

unsigned		ThreadGranularityOptimizer::begin()
{
	started = timer.now();
	return location[current_samples%Resolution];
}

void			ThreadGranularityOptimizer::end(unsigned iterations, const TCodeLocation&clocation)
{
	Timer::Time delta = (timer.now()-started)*100/iterations;
	time_sum[(current_samples++)%Resolution] += delta;
	
	/*
		f(x) = ax² + bx + c
		
		y1 = ax1² + bx1 + c
		=> c = y1 - ax1² - bx1
		=> y1-y2 + a*(x2²-x1²) + b*(x2-x1) = 0
		=> b(x2-x1) = y2-y1 + a(x1²-x2²)
		=> b	= (y2-y1 + a(x1²-x2²))/(x2-x1)
			= (y3-y2 + a(x2²-x3²))/(x3-x2)
		=> (y2-y1)(x3-x2) + a(x1²-x2²)(x3-x2) = (y3-y2)(x2-x1) + a(x2²-x3²)(x2-x1)
		=> a((x1²-x2²)(x3-x2) - (x2²-x3²)(x2-x1)) = (y3-y2)(x2-x1) - (y2-y1)(x3-x2) 
			(y3-y2)(x2-x1) - (y2-y1)(x3-x2) 
		=> a = -------------------------------------------------
			(x1²-x2²)(x3-x2) - (x2²-x3²)(x2-x1)
	*/
	
	if (current_samples >= RequireSamples*Resolution)
	{
		cout << "optimizing "<<clocation.toString()<<"\n";
		for (unsigned i = 0; i < Resolution; i++)
			cout << " " <<location[i]<<"="<<time_sum[i]<<endl;
		
		double weight_sum = 0,
				total = 0;
		/*Timer::Time min_val = time_sum[0];
		unsigned min_loc0 = 0,
				min_loc1 = min_loc0;*/
		for (unsigned i = 1; i < Resolution; i++)
		{
			double weight = 1.0/time_sum[i];
			weight_sum += weight;
			total += weight * location[i];
			
			/*if (time_sum[i] < min_val)
			{
				min_loc0 = min_loc1;
				min_val = time_sum[i];
				min_loc1 = i;
			}*/
		}
		//unsigned	index = (unsigned)round((float)min_loc0*0.25 + (float)min_loc1*0.75);
		unsigned optimum = (unsigned)round(total/weight_sum);
		
		cout << "optimum: "<<optimum<<endl;
		current_samples = 0;
	}
}

