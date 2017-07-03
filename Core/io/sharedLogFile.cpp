#include <global_root.h>
#include "sharedLogFile.h"

namespace DeltaWorks
{

	bool	SharedLogFile::Open(const PathString&path, LONGLONG maxFileSize/*=0*/, OversizeHandling handling/*=OversizeHandling::ClearFileAndContinue*/)
	{
		mutex.lock();
			Close();
			this->maxFileSize = maxFileSize;
			oversizeHandling = handling;
			timesTruncated=0;
			fileHandle = CreateFileW(path.c_str(),GENERIC_WRITE,FILE_SHARE_READ,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
			bool rs = fileHandle != INVALID_HANDLE_VALUE;
		mutex.unlock();
		return rs;
	}



	bool	SharedLogFile::Write(const StringRef&ref,bool toSyslog)
	{
		if (fileHandle == INVALID_HANDLE_VALUE)
			return false;
		std::lock_guard<std::recursive_mutex> lock(mutex);
		DWORD at;
		if (maxFileSize != 0)
		{
			LARGE_INTEGER size;
			if (GetFileSizeEx(fileHandle,&size))
			{
				if (size.QuadPart >= maxFileSize)
				{
					switch (oversizeHandling)
					{
						case CloseFile:
						{
							String toWrite = "File size threshold ("+String(maxFileSize)+") reached. Halting output\r\n";
							WriteFile(fileHandle,toWrite.c_str(),(DWORD)toWrite.length(),&at,NULL);
							Close();
							return false;
						}
						case ClearFileAndContinue:
						{
							if (SetFilePointer(fileHandle,0,NULL,FILE_BEGIN)!=0)
							{
								Close();
								return false;
							}
							if (!SetEndOfFile(fileHandle))
							{
								Close();
								return false;
							}
							timesTruncated++;
							String toWrite = "File size threshold ("+String(maxFileSize)+") reached. Cleared file. This happend "+String(timesTruncated)+" time(s)\r\n";
							WriteFile(fileHandle,toWrite.c_str(),(DWORD)toWrite.length(),&at,NULL);
						}
						break;
						default:
							Close();
							return false;
					}
				}
			}
		}
		return WriteFile(fileHandle,ref.pointer(),(DWORD)ref.GetLength(),&at,NULL) == TRUE;
	}
}
