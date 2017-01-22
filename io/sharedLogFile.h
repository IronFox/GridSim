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

	bool	Write(const StringBuffer&buffer,bool toSyslog)
	{
		if (fileHandle == INVALID_HANDLE_VALUE)
			return false;
		DWORD at;
		return WriteFile(fileHandle,buffer.pointer(),(DWORD)buffer.GetLength(),&at,NULL) == TRUE;
	}
	
	void	WriteAndClear(StringBuffer&buffer,bool toSyslog)
	{
		if (Write(buffer,toSyslog) || buffer.Count() > 1000000)
			buffer.Clear();
	}
	
	
	
};



#endif
