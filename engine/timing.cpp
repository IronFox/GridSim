#include "../global_root.h"
#include "timing.h"

/******************************************************************

Engine timing-unit.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

namespace Engine
{

	Timing		timing;

	Timing::Timing():ticks_per_second(0),now64(0),delta64(0),delta(0),now(0)
	{}

	void Timing::update()
	{
	    delta64 = timer.getTimei()-now64;
	    now64+=delta64;
	    delta = (float)delta64/ticks_per_second;
	    now = (double)now64/ticks_per_second;
	}

	void Timing::initialize()
	{
		ticks_per_second = timer.getTicksPerSecond();
		delta = 0.01f;
		delta64 = 1;
	}
}
