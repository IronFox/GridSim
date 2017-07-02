#include "timing.h"

/******************************************************************

Engine timing-unit.

******************************************************************/

namespace Engine
{

	Timing		timing;

	Timing::Timing():ticks_per_second(0),now64(0),delta64(0),delta(0),now(0),timerReference(timer.Now())
	{}

	void Timing::update()
	{
	    delta64 = timer.now()-now64;
	    now64+=delta64;
	    delta = (float)delta64/ticks_per_second;
	    now = (double)(now64 - timerReference) /ticks_per_second;
	}

	void Timing::initialize()
	{
		ticks_per_second = timer.getTicksPerSecond();
		delta = 0.01f;
		delta64 = 1;
		now64 = timer.now();
		timerReference = timer.Now();
	}
}
