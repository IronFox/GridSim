#ifndef ioSharedLogFileH
#define ioSharedLogFileH

//#include "../general/system.h"
//#include "../global_string.h"
#include "../string/string_buffer.h"



class SharedLogFile
{
	HANDLE	fileHandle;
	
	/**/	SharedLogFile(const SharedLogFile&)	{}
	void	operator=(const SharedLogFile&)	{}
public:
	/**/	SharedLogFile():fileHandle(INVALID_HANDLE_VALUE){}
	virtual ~SharedLogFile() {Close();}
	void	Close()
	{
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(fileHandle);
			fileHandle = INVALID_HANDLE_VALUE;
		}
	}
	bool	Open(const PathString&path)
	{
		Close();
		fileHandle = CreateFileW(path.c_str(),GENERIC_WRITE,FILE_SHARE_READ,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
		return fileHandle != INVALID_HANDLE_VALUE;
	}

	void	Write(const StringBuffer&buffer,bool toSyslog)
	{
		DWORD at;
		WriteFile(fileHandle,buffer.pointer(),(DWORD)buffer.GetLength(),&at,NULL);
	}
	
	void	WriteAndClear(StringBuffer&buffer,bool toSyslog)
	{
		Write(buffer,toSyslog);
		buffer.Clear();
	}
	
	
	
};



#endif
