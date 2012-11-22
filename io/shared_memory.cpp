#include "shared_memory.h"


namespace SharedMemory
{
	TMapping::TMapping():data(NULL)
		#if SYSTEM==WINDOWS
			,handle(NULL)
		#endif
	{}

	TMapping		map(const char*name, size_t size)
	{
		TMapping result;
		
		#if SYSTEM==WINDOWS
			result.handle = CreateFileMappingA(
					 INVALID_HANDLE_VALUE,
					 NULL,
					 PAGE_READWRITE,
					 0,
					 DWORD(size),
					 name);
			if (!result.handle)
			{
				result.data = NULL;
				return result;
			}
			result.data = (void*) MapViewOfFile(result.handle,
							FILE_MAP_ALL_ACCESS, // read/write permission
							0,                   
							0,                   
							size);
			if (!result.data)
				CloseHandle(result.handle);
		#else
			//#error stub
		#endif
		
		return result;
	}
	
	void			unmap(const TMapping&mapping)
	{
		if (!mapping.data)
			return;
		#if SYSTEM==WINDOWS
			UnmapViewOfFile(mapping.data);
			CloseHandle(mapping.handle);
		#else
			//#error stub
		#endif
	}
}

