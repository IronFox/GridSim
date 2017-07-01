#ifndef mutex_logH
#define mutex_logH

/******************************************************************

Thread-safe Logfile-creation-tool.

******************************************************************/

#include "log.h"
#include "../general/thread_system.h"


class SynchronizedLogFile;

class MutexLogSession
{
protected:
		SynchronizedLogFile	*log_file;
		unsigned	*counter;
public:
		typedef MutexLogSession	Session;
		
					MutexLogSession(SynchronizedLogFile*file);
					MutexLogSession(const MutexLogSession&other);
virtual				~MutexLogSession();
		Session&	operator=(const MutexLogSession&other);
inline  Session&	operator<<(const char*line);
inline  Session&	operator<<(const String&line);
inline  Session&	operator<<(const TNewLine&);
inline  Session&	operator<<(const TLogIndent&indent);
};

/**
    \brief Thread safe log file handle
*/
class SynchronizedLogFile:public LogFile
{
private:
	Mutex      mutex;
	typedef LogFile	Super;
		
	friend class MutexLogSession;
public:
	typedef MutexLogSession	Session;


	/**/		SynchronizedLogFile();
	/**/		SynchronizedLogFile(const PathString&filename, bool makeclear);   //!< Creates a new log object \param filename Name of the file to write to \param makeclear Specifies whether or not the specified file should be cleared before writing
	virtual     ~SynchronizedLogFile();

	void        Clear();                                        //!< Clears the content of the local log file
	bool        Open(const PathString&filename, bool makeclear);   //!< Closes the active file and opens the specified file for write output \param filename Name of the file to write to  \param makeclear Specifies whether or not the specified file should be cleared before writing \return true on success
	void        Close();                                        //!< Closes the active file
    bool        Log(const char*line, bool time_=false);         //!< Writes a line to the end of the log file and flushes the file \param line Pointer to the string that should be written \param time_ Specifies whether or not a timestamp should preceed the output \return true on success
    bool        Log(const String&line, bool time_=false);      //!< Writes a line to the end of the log file and flushes the file \param line String that should be written \param time_ Specifies whether or not a timestamp should preceed the output \return true on success
	inline  Session		operator<<(const char*line);
	inline  Session		operator<<(const String&line);
	inline  Session		operator<<(const TNewLine&);
	inline  Session		operator<<(const TLogIndent&indent);
};




#include "mutex_log.tpl.h"


#endif
