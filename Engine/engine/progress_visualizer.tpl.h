#ifndef engine_progress_visualizerTplH
#define engine_progress_visualizerTplH

/******************************************************************

Eve progress-visualizer.

******************************************************************/

namespace Engine
{

	template <class Font, class GL> ProgressVisualizer<Font,GL>::ProgressVisualizer(Display<GL>&display_):display(display_)
	{
	    bg.select(EveStructure::Quads);
	    bg.Resize(3);

	    bg.color(0,0,0);
	    bg.vertex(0,0);
	    bg.vertex(1,0);
	    bg.vertex(1,1.0f-PROG_LH);
	    bg.vertex(0,1.0f-PROG_LH);

	    bg.color(1.0f,0.75f,0);
	    bg.vertex(0,1);
	    bg.vertex(0,1.0f-PROG_LH);
	    bg.vertex(1,1.0f-PROG_LH);
	    bg.vertex(1,1);

	    bg.vertex(1,0);
	    bg.vertex(1,0);
	    bg.vertex(0,0);
	    bg.vertex(0,0);

	    bg.select(EveStructure::Lines);
	    bg.Resize(3);
	    bg.vertex(0,1);
	    bg.vertex(0,0);
	    bg.vertex(1,0);
	    bg.vertex(1,1);
	    bg.vertex(0,0);
	    bg.vertex(1,0);
	    
	    bg.seal();
	    
	    bar.select(EveStructure::Quads);
	    bar.Resize(2);
	    bar.color(0,0,0);
	    bar.vertex(0,0.1);
	    bar.vertex(1,0.1);
	    bar.vertex(1,0.8);
	    bar.vertex(0,0.8);
	    
	    bar.color(1.0f,0.75f,0);
	    bar.vertex(0,0.1);
	    bar.vertex(0,0.1);
	    bar.vertex(0,0.8);
	    bar.vertex(0,0.8);
	    
	    bar.select(EveStructure::Lines);
	    bar.Resize(3);
	    bar.vertex(0,0.1);
	    bar.vertex(1,0.1);
	    bar.vertex(0,0.8);
	    bar.vertex(1,0.8);
	    bar.vertex(1,0.1);
	    bar.vertex(1,0.8);
	    
	    bar.seal();
	}

	template <class Font, class GL> void ProgressVisualizer<Font,GL>::init(const String&caption_)
	{
	    caption = caption_;
	    EveOrthographicAspect<float>            aspect;
	    aspect.make(1, 1.9, -1, 1);
	    aspect.region = M::TFloatRect(0.2,0.2,0.8,0.8);
	    _v3(aspect.location,0.5,0.5,0);
	    aspect.build();
	    display.Pick(aspect);
	    textout.setScale(PROG_LH,PROG_LH);
	    textout.color(1.0f,0.75f,0);
	    __eye4(sub_system);
	    sub_system[5] = PROG_LH;
	}

	template <class Font, class GL> void ProgressVisualizer<Font,GL>::update(Progress::Log*log)
	{
	   // display.nextFrameNoClr();

	    float progress = log->progress();
	    bool bar_visible = log->bar_line < log->lines && progress;
	    GL::render(bg);

	    if (bar_visible)
	    {
	        sub_system[12] = 0.1f*log->line[log->bar_line].depth+textout.unscaledLength(log->line[log->bar_line].line)*PROG_LH+0.05f;
	        sub_system[0] = 0.95f-sub_system[12];
	        sub_system[13] = 1-PROG_LH*(log->bar_line+2);
	        bar.field[EveStructure::Quads][3].position[0] = progress;
	        bar.field[EveStructure::Quads][0].position[0] = progress;
	        bar.field[EveStructure::Quads][5].position[0] = progress;
	        bar.field[EveStructure::Quads][6].position[0] = progress;
	        GL::enterSubSystem(sub_system);
	            GL::render(bar);
	        GL::exitSubSystem();
	    }
	    textout.locate(0.05f,1.0f-0.25f*PROG_LH);
	    textout.color(0.3f,0.1f,0);
	    textout.setScale(PROG_LH*0.75,PROG_LH*0.75);
	    textout.print(caption);
	    for (unsigned i = 0; i < log->lines; i++)
	    {
	        textout.state.line = 0;
	        if (log->line[i].depth)
	        {
	            textout.color(0.8f,0.6f,0);
	            textout.setScale(PROG_LH*0.8,PROG_LH*0.8);
	            textout.locate(0.1f*log->line[i].depth,1-PROG_LH*(i+1)-PROG_LH*0.1);
	        }
	        else
	        {
	            textout.color(0.95f,0.7f,0);
	            textout.setScale(PROG_LH,PROG_LH);
	            textout.locate(0.1f*log->line[i].depth,1-PROG_LH*(i+1));
	        }
	        textout.print(log->line[i].line);
	    }
	    display.nextFrameNoClr();
	    
	    
	    
	    GL::render(bg);

	    if (bar_visible)
	    {
	        sub_system[12] = 0.1f*log->line[log->bar_line].depth+textout.unscaledLength(log->line[log->bar_line].line)*PROG_LH+0.05f;
	        sub_system[0] = 0.95f-sub_system[12];
	        sub_system[13] = 1-PROG_LH*(log->bar_line+2);
	        bar.field[EveStructure::Quads][3].position[0] = progress;
	        bar.field[EveStructure::Quads][0].position[0] = progress;
	        bar.field[EveStructure::Quads][5].position[0] = progress;
	        bar.field[EveStructure::Quads][6].position[0] = progress;
	        GL::enterSubSystem(sub_system);
	            GL::render(bar);
	        GL::exitSubSystem();
	    }
	    textout.locate(0.05f,1.0f-0.25f*PROG_LH);
	    textout.color(0.3f,0.1f,0);
	    textout.setScale(PROG_LH*0.75,PROG_LH*0.75);
	    textout.print(caption);
	    for (unsigned i = 0; i < log->lines; i++)
	    {
	        textout.state.line = 0;
	        if (log->line[i].depth)
	        {
	            textout.color(0.8f,0.6f,0);
	            textout.setScale(PROG_LH*0.8,PROG_LH*0.8);
	            textout.locate(0.1f*log->line[i].depth,1-PROG_LH*(i+1)-PROG_LH*0.1);
	        }
	        else
	        {
	            textout.color(0.95f,0.7f,0);
	            textout.setScale(PROG_LH,PROG_LH);
	            textout.locate(0.1f*log->line[i].depth,1-PROG_LH*(i+1));
	        }
	        textout.print(log->line[i].line);
	    }
	    display.nextFrameNoClr();
	}

}

#endif

