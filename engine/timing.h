#ifndef engine_timingH
#define engine_timingH

/******************************************************************

Engine timing-unit.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

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
