#ifndef ioSharedLogFileH
#define ioSharedLogFileH

//#include "../general/system.h"
//#include "../global_string.h"
#include "../string/string_buffer.h"
#include <mutex>

/**
The shared log file allows synchronized out-access, but also opens the respective file in shared mode.
Other processes can open the file while it is being modified here
*/
class SharedLogFile
{
public:
	enum OversizeHandling
	{
		CloseFile,
		ClearFileAndContinue
	};

private:
	HANDLE	fileHandle;
	LONGLONG	maxFileSize;
	OversizeHandling oversizeHandling;
	std::recursive_mutex	mutex;
	count_t	timesTruncated=0;
	
	/**/	SharedLogFile(const SharedLogFile&)	{}
	void	operator=(const SharedLogFile&)	{}
public:


	/**/	SharedLogFile():
			fileHandle(INVALID_HANDLE_VALUE),maxFileSize(maxFileSize),oversizeHandling(oversizeHandling){}
	virtual ~SharedLogFile() {Close();}
	void	Close()
	{
		mutex.lock();
			if (fileHandle != INVALID_HANDLE_VALUE)
			{
				CloseHandle(fileHandle);
				fileHandle = INVALID_HANDLE_VALUE;
			}
		mutex.unlock();
	}
	bool	Open(const PathString&path, LONGLONG maxFileSize=0, OversizeHandling handling=OversizeHandling::ClearFileAndContinue);
	bool	Write(const String&str, bool toSyslog)	{return Write(str.ref(),toSyslog);}
	bool	Write(const char*str, bool toSyslog)	{return Write(StringRef(str),toSyslog);}
	bool	Write(const StringRef&str, bool toSyslog);
	bool	Write(const StringBuffer&buffer,bool toSyslog)	{return Write(buffer.ToStringRef(),toSyslog);}
	void	WriteAndClear(StringBuffer&buffer,bool toSyslog)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (Write(buffer,toSyslog) || buffer.Count() > 1000000)
			buffer.Clear();
	}
	
	
	
};



#endif
