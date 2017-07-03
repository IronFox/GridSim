#include "../global_root.h"
#include "log_stream.h"

namespace DeltaWorks
{

	LogSession::LogSession(LogStream*stream):parent(stream),counter(SignalNew(new count_t(1)))
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
			Discard(counter);
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
			Discard(counter);
		}
		counter = other.counter;
		parent = other.parent;
		(*counter)++;
		return *this;
	}

	LogStream	lout;

}
