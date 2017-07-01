#include "../global_root.h"
#include "mutex_log.h"

/******************************************************************

Thread-safe Logfile-creation-tool.

******************************************************************/


void        SynchronizedLogFile::Clear()
{
    mutex.lock();
        LogFile::Clear();
    mutex.release();
}

bool        SynchronizedLogFile::Open(const PathString&filename, bool makeclear)
{
    mutex.lock();
        bool result = LogFile::Open(filename,makeclear);
    mutex.release();
    return result;
}

void        SynchronizedLogFile::Close()
{
    mutex.lock();
        LogFile::close();
    mutex.release();
}

MutexLogSession::MutexLogSession(SynchronizedLogFile*file):log_file(file),counter(SignalNew(new unsigned(1)))
{
	file->mutex.lock();
}

MutexLogSession::~MutexLogSession()
{
	(*counter)--;
	if (!(*counter))
	{
		log_file->mutex.release();
		Discard(counter);
	}
}


MutexLogSession::MutexLogSession(const MutexLogSession&other):log_file(other.log_file),counter(other.counter)
{
	(*counter)++;
}

MutexLogSession&	MutexLogSession::operator=(const MutexLogSession&other)
{
	if (&other == this)
		return *this;
	(*counter)--;
	if (!(*counter))
	{
		log_file->mutex.release();
		Discard(counter);
	}
	counter = other.counter;
	log_file = other.log_file;
	(*counter)++;
	return *this;
}



SynchronizedLogFile::SynchronizedLogFile()
{}



SynchronizedLogFile::SynchronizedLogFile(const PathString&filename, bool makeclear):LogFile(filename,makeclear)
{
    /*mutex.lock();
        if (makeclear)
            f = fopen(filename.c_str(),"wb");
        else
            f = fopen(filename.c_str(),"ab");
        memset(space_buffer,' ',sizeof(space_buffer));
    mutex.release();*/
}


SynchronizedLogFile::~SynchronizedLogFile()
{
    close();
}

bool        SynchronizedLogFile::Log(const char*line, bool time_)
{
	mutex.lock();
	bool rs = Super::Log(line, time_);
	mutex.unlock();
	return rs;
}

bool        SynchronizedLogFile::Log(const String&line, bool time_)
{
	mutex.lock();
	bool rs = Super::Log(line, time_);
	mutex.unlock();
	return rs;
}

