#include "../global_root.h"
#include "file_stream.h"



bool	FileStream::open(const char*filename, int flags)
{
	if (handle != -1)
		_close(handle);
	handle = _open(filename,flags);
	return handle != -1;
}

bool	FileStream::open(const wchar_t*filename, int flags)
{
	if (handle != -1)
		_close(handle);
	#if SYSTEM==WINDOWS
		handle = _wopen(filename,flags);
	#else
		handle = -1;
	#endif
	return handle != -1;
}


bool	FileStream::write(const void*data, serial_size_t size)
{
	if (handle == -1)
		return false;
	return _write(handle,data,(unsigned)size)==size;
}

bool	FileStream::read(void*data, serial_size_t size)
{
	if (handle == -1)
		return false;
	return _read(handle,data,(unsigned)size)==size;
}


