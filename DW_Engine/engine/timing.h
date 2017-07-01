#ifndef engine_timingH
#define engine_timingH

/******************************************************************

Engine timing-unit.

******************************************************************/


#include "../general/timer.h"

namespace Engine
{

	class Timing
	{
	public:
	        Timer::Time		ticks_per_second,
							now64,
			                delta64,
							timerReference;
	        float   		delta;
			double			now;

							Timing();
	        void    		update();
	        void    		initialize();
	};

	extern Timing timing;

}

#endif
