#include "../global_root.h"
#include "log_stream.h"


LogSession::LogSession(LogStream*stream):parent(stream),counter(SHIELDED(new unsigned(1)))
{
	stream->mutex.lock();
}


LogSession::~LogSession()
{
	(*counter)--;
	if (!(*counter))
	{
		parent->redirectTarget(parent->segment.ToStringRef());
		parent->mutex.release();
		DISCARD(counter);
	}
}


LogSession::LogSession(const LogSession&other):parent(other.parent),counter(other.counter)
{
	(*counter)++;
}

LogSession&	LogSession::operator=(const LogSession&other)
{
	if (&other == this)
		return *this;
	(*counter)--;
	if (!(*counter))
	{
		parent->redirectTarget(parent->segment.ToStringRef());
		parent->mutex.release();
		DISCARD(counter);
	}
	counter = other.counter;
	parent = other.parent;
	(*counter)++;
	return *this;
}

LogStream	lout;

