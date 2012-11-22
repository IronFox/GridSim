#ifndef engine_progress_visualizerH
#define engine_progress_visualizerH

/******************************************************************

Eve progress-visualizer.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include "../general/progress_log.h"
#include "renderer/renderer.h"
#include "display.h"

namespace Engine
{

	template <class Font, class GL>
		class ProgressVisualizer:public Progress::Visualizer
		{
		private:
				String				 	caption;
				Display<GL>			&display;
				EveStructure			bg,bar;
				float					sub_system[16];
				

		public:
				Textout<Font>			textout;

										EveVisualizer(Display<GL>&display_);
				void					init(const String&caption);
				void					update(Progress::Log*log);
		};
}

/*
	top.window = 1;
	top.offset = 0;
	top.len = 1;
	top.state = 0;

	bg.select(EVE_QUADS);
	bg.resize(5);

	bg.color(0,0,0);
	bg.vertex(0,EVE_PROG_LH);
	bg.vertex(1,EVE_PROG_LH);
	bg.vertex(1,1.0f-EVE_PROG_LH);
	bg.vertex(0,1.0f-EVE_PROG_LH);

	bg.color(1.0f,0.75f,0);
	bg.vertex(0,1);
	bg.vertex(0,1.0f-EVE_PROG_LH);
	bg.vertex(1,1.0f-EVE_PROG_LH);
	bg.vertex(1,1);

	bg.vertex(1,0);
	bg.vertex(1,EVE_PROG_LH);
	bg.vertex(0,EVE_PROG_LH);
	bg.vertex(0,0);

	bg.select(EVE_LINES);
	bg.resize(2);
	bg.vertex(0,1);
	bg.vertex(0,0);
	bg.vertex(1,0);
	bg.vertex(1,1);

	EveParallelAspect<float>			aspect;
	aspect.make(1, 1.9, -1, 1);
	aspect.region = TFloatRect(0.2,0.2,0.8,0.8);
	_v3(aspect.location,0.5,0.5,0);
	aspect.build();
	eve.pick(aspect);

		GL::render(bg);
		eve.finishFrame(false);
*/

#include "progress_visualizer.tpl.h"

#endif

