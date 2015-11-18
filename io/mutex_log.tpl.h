#ifndef mutex_logTplH
#define mutex_logTplH

/******************************************************************

Thread-safe Logfile-creation-tool.

******************************************************************/


 



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
    log_file->LogFile::Log(line);
    return *this;
}

inline  MutexLogSession&       MutexLogSession::operator<<(const String&line)
{
    log_file->LogFile::Log(line);
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
