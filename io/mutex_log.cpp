#include "../global_root.h"
#include "mutex_log.h"

/******************************************************************

Thread-safe Logfile-creation-tool.

******************************************************************/



MutexLogSession::MutexLogSession(SynchronizedLogFile*file):log_file(file),counter(SHIELDED(new unsigned(1)))
{
	file->mutex.lock();
}

MutexLogSession::~MutexLogSession()
{
	(*counter)--;
	if (!(*counter))
	{
		log_file->mutex.release();
		DISCARD(counter);
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
		DISCARD(counter);
	}
	counter = other.counter;
	log_file = other.log_file;
	(*counter)++;
	return *this;
}



SynchronizedLogFile::SynchronizedLogFile()
{}



SynchronizedLogFile::SynchronizedLogFile(const String&filename, bool makeclear):LogFile(filename,makeclear)
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

bool        SynchronizedLogFile::log(const char*line, bool time_)
{
//    ShowMessage(line);
    if (!begin())
        return false;
    if (time_)
    {
        time_t tt = time(NULL);
        const tm*t = localtime(&tt);
        char output[256];
        unsigned len = (unsigned)strftime(output,sizeof(output),format.c_str(),t);
        mutex.lock();
        if (fwrite(output,1,len,f)!=len)
		{
			end();
			mutex.release();
			return false;
		}
    }
    else
        mutex.lock();
	size_t len = strlen(line);
    if (fwrite(line,1,len,f)!=len)
	{
		end();
		mutex.release();
		return false;
	}
	end();
	mutex.release();
    return true;
}

bool        SynchronizedLogFile::log(const String&line, bool time_)
{
//    cout << "'"<<line.c_str()<<"' ("<<line.length()<<")\n";
    if (!begin())
        return false;
    if (time_)
    {
        time_t tt = time(NULL);
        const tm*t = localtime(&tt);
        char output[256];
        unsigned len = (unsigned)strftime(output,sizeof(output),format.c_str(),t);
        mutex.lock();
        if (fwrite(output,1,len,f)!=len)
		{
			end();
			mutex.release();
			return false;
		}
    }
    else
        mutex.lock();
    if (fwrite(line.c_str(),1,line.length(),f)!=line.length())
	{
		end();
		mutex.release();
		return false;
	}
	end();
	mutex.release();
	return true;
}

