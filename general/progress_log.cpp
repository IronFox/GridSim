#include "../global_root.h"
#include "progress_log.h"




Progress::Log::Log():active(false),vis(NULL),lines(0),step_depth(0),bar_line(0)
{
    top.window = 1;
    top.offset = 0;
    top.len = 1;
    top.state = 0;
}

void Progress::Log::enterMinor(unsigned len)
{
    if (!this || !active)
        return;
    float offset = progress(),
          window = top.len?top.window /(float)top.len:top.window;
    stack.push(top);
    top.window = window;
    top.state = 0;
    top.len = len;
    top.offset = offset;
    vis->update(this);
}


void Progress::Log::exitMinor()
{
    if (!this || !active)
        return;
    top = stack.pop();
//    vis->update(this);
}

void Progress::Log::setState(unsigned state)
{
    if (!this || !active)
        return;
    top.state = state;
    vis->update(this);
}

void Progress::Log::incState()
{
    if (!this || !active)
        return;
    top.state++;
    vis->update(this);
}

void Progress::nameLine(tLogLine&line, const String&str)
{
    line.line_len = clamped(str.length(),0,sizeof(line.line)-1);
    memcpy(line.line,str.c_str(),line.line_len);
    line.line[line.line_len] = 0;
}

void Progress::nameLine(tLogLine&line, const char*str)
{
    line.line_len = (unsigned)clamped(strlen(str),0,sizeof(line.line)-1);
    memcpy(line.line,str,line.line_len);
    line.line[line.line_len] = 0;
}



void Progress::Log::enterStep(const String&name, bool attach)
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
    nameLine(line[lines],name);
    line[lines++].depth = step_depth++;
    vis->update(this);
}


void Progress::Log::exitStep()
{
    if (!this || !active)
        return;
    step_depth--;
}

void Progress::Log::addStep(const String&name, bool attach)
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
    nameLine(line[lines],name);
    line[lines++].depth = step_depth;
    vis->update(this);
}




float Progress::Log::progress()
{
    if (!top.len)
        return top.offset;
    return top.offset+top.window*(float)top.state/(float)top.len;
}

void Progress::Log::begin(const String&name, Visualizer&visualizer)
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

void Progress::Log::close()
{
    if (!this || !active)
        return;
    active = false;
}

Progress::Visualizer::~Visualizer()
{}


