#ifndef mutex_logTplH
#define mutex_logTplH

/******************************************************************

Thread-safe Logfile-creation-tool.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


 

inline  void        SynchronizedLogFile::clear()
{
    mutex.lock();
        LogFile::clear();
    mutex.release();
}

inline  bool        SynchronizedLogFile::open(const String&filename, bool makeclear)
{
    mutex.lock();
        bool result = LogFile::open(filename,makeclear);
    mutex.release();
    return result;
}

inline  void        SynchronizedLogFile::close()
{
    mutex.lock();
        LogFile::close();
    mutex.release();
}


inline  MutexLogSession	SynchronizedLogFile::operator<<(const char*line)
{
	MutexLogSession	result(this);
	result << line;
	return result;
}

inline  MutexLogSession	SynchronizedLogFile::operator<<(const String&line)
{
	MutexLogSession	result(this);
	result << line;
	return result;
}

inline  MutexLogSession	SynchronizedLogFile::operator<<(const TNewLine&nl)
{
	MutexLogSession	result(this);
	result << nl;
	return result;
}

inline  MutexLogSession	SynchronizedLogFile::operator<<(const TLogIndent&indent)
{
	MutexLogSession	result(this);
	result << indent;
	return result;
}




inline  MutexLogSession&       MutexLogSession::operator<<(const char*line)
{
    log_file->LogFile::log(line);
    return *this;
}

inline  MutexLogSession&       MutexLogSession::operator<<(const String&line)
{
    log_file->LogFile::log(line);
    return *this;
}

inline  MutexLogSession&       MutexLogSession::operator<<(const TNewLine&l)
{
	log_file->LogFile::operator<<(l);
    return *this;
}

inline  MutexLogSession&       MutexLogSession::operator<<(const TLogIndent&indent)
{
    log_file->LogFile::operator<<(indent);
    return *this;
}

#endif
