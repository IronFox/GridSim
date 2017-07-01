#include "../global_root.h"
#include "progress_log.h"

namespace DeltaWorks
{


	Progress::Log::Log():active(false),vis(NULL),lines(0),step_depth(0),bar_line(0)
	{
		top.window = 1;
		top.offset = 0;
		top.len = 1;
		top.state = 0;
	}

	void Progress::Log::EnterMinor(unsigned len)
	{
		if (!this || !active)
			return;
		float offset = GetProgress(),
			  window = top.len?top.window /(float)top.len:top.window;
		stack.Append(top);
		top.window = window;
		top.state = 0;
		top.len = len;
		top.offset = offset;
		vis->update(this);
	}


	void Progress::Log::ExitMinor()
	{
		if (!this || !active)
			return;
		top = stack.pop();
	//    vis->update(this);
	}

	void Progress::Log::SetState(unsigned state)
	{
		if (!this || !active)
			return;
		top.state = state;
		vis->update(this);
	}

	void Progress::Log::IncState()
	{
		if (!this || !active)
			return;
		top.state++;
		vis->update(this);
	}

	void Progress::NameLine(tLogLine&line, const String&str)
	{
		line.line_len = Math::clamped(str.length(),0,sizeof(line.line)-1);
		memcpy(line.line,str.c_str(),line.line_len);
		line.line[line.line_len] = 0;
	}

	void Progress::NameLine(tLogLine&line, const char*str)
	{
		line.line_len = (unsigned)Math::clamped(strlen(str),0,sizeof(line.line)-1);
		memcpy(line.line,str,line.line_len);
		line.line[line.line_len] = 0;
	}



	void Progress::Log::EnterStep(const String&name, bool attach)
	{
		if (!this || !active)
			return;
		if (lines == PROGRESS_LOG_LINES-2)
		{
			for (unsigned i = 0; i < PROGRESS_LOG_LINES-1; i++)
				line[i] = line[i+1];
			lines--;
			if (bar_line)
				bar_line--;
		}
		if (attach)
			bar_line = lines;
		NameLine(line[lines],name);
		line[lines++].depth = step_depth++;
		vis->update(this);
	}


	void Progress::Log::ExitStep()
	{
		if (!this || !active)
			return;
		step_depth--;
	}

	void Progress::Log::AddStep(const String&name, bool attach)
	{
		if (!this || !active)
			return;
		if (lines == PROGRESS_LOG_LINES-2)
		{
			for (unsigned i = 0; i < PROGRESS_LOG_LINES-1; i++)
				line[i] = line[i+1];
			lines--;
			if (bar_line)
				bar_line--;
		}
		if (attach)
			bar_line = lines;
		NameLine(line[lines],name);
		line[lines++].depth = step_depth;
		vis->update(this);
	}




	float Progress::Log::GetProgress()
	{
		if (!top.len)
			return top.offset;
		return top.offset+top.window*(float)top.state/(float)top.len;
	}

	void Progress::Log::Begin(const String&name, Visualizer&visualizer)
	{
		if (!this)
			return;
		vis = &visualizer;
		lines = 0;
		bar_line = 0;
		step_depth = 0;
		stack.clear();
		top.window = 1;
		top.offset = 0;
		top.len = 1;
		top.state = 0;
		active = true;

		vis->init(name);
		vis->update(this);
	}

	void Progress::Log::Close()
	{
		if (!this || !active)
			return;
		active = false;
	}

	Progress::Visualizer::~Visualizer()
	{}

}
