#ifndef log_streamH
#define log_streamH

#include "../global_string.h"			//strings
#include "../general/thread_system.h"	//mutexes
#include "../string/string_buffer.h"	//string builder


class LogStream;

/**
	@brief Temporary output session until << operator stream ends
	
	During the existence of a LogSession the respective parent log stream is mutex locked
*/
class LogSession
{
protected:
		friend class			LogStream;
		
		LogStream				*parent;
		unsigned				*counter;
		
		
								LogSession(LogStream*parent_);
public:
								LogSession(const LogSession&);
virtual							~LogSession();
		LogSession&			operator=(const LogSession&);


	template <typename T>
		LogSession&			operator<<(const T&obj);
		
inline	LogSession&			operator<<(const TEndLine&);
};



/**
	@brief Redirectable string out stream
	
	LogStream allows to redirect messages to any source.
*/
class LogStream
{
protected:
		StringBuffer			segment;
		Mutex					mutex;
		
		friend class			LogSession;
public:

		void(*redirectTarget)(const String&segment);	//!< String output target (cout by default)

static	void					standardRedirect(const String&segment)
								{
									cout << '>'<<segment;
								}

								LogStream():redirectTarget(standardRedirect)
								{}
virtual							~LogStream()	{};

	template <typename T>
		LogSession				operator<<(const T&obj)
								{
									LogSession session(this);
									segment.reset();
									session << obj;
									return session;
								}
};


template <typename T>
	LogSession&			LogSession::operator<<(const T&obj)
	{
		parent->segment<<obj;
		return *this;
	}


inline
	LogSession&			LogSession::operator<<(const TEndLine&)
	{
		parent->segment<<nl;
		return *this;
	}



extern LogStream		lout;





#endif
