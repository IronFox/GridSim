#include "../global_root.h"
#include "file_stream.h"
#include <sys/stat.h>

namespace DeltaWorks
{

	bool	FileStream::open(const char*filename, int flags)
	{
		if (handle != -1)
			close();
		handle = _open(filename, flags, _S_IREAD | _S_IWRITE);

		return handle != -1;
	}

	bool	FileStream::open(const wchar_t*filename, int flags)
	{
		if (handle != -1)
			close();
		#if SYSTEM==WINDOWS
			handle = _wopen(filename,flags);
		#else
			handle = -1;
		#endif
		return handle != -1;
	}


	/*virtual override*/ void	FileStream::Write(const void*data, serial_size_t size)
	{
		if (handle == -1)
			throw Except::IO::DriveAccess(CLOCATION,"Cannot write data: File is not open");
		unsigned remaining = (unsigned)size;
		const BYTE*write = (const BYTE*)data;
		while (remaining > 0)
		{
			int written = _write(handle,write,remaining);
			if (written < 0)
			{
				int err = errno;
				switch (err)
				{
					case EBADF:
						throw Except::IO::DriveAccess(CLOCATION, "Cannot write data: File handle is invalid or not accessible for writing");
					case ENOSPC:
						throw Except::IO::DriveAccess(CLOCATION,"Cannot write data: Insufficient space on storage");
					case EINVAL:
						throw Except::Program::ParameterFault(CLOCATION,"Cannot write data: Invalid parameters");
				}
				char errs[0x100];
				sprintf_s(errs,"Cannot write data: Unexpected error code: %d",err);
				throw Except::IO::DriveAccess(CLOCATION,errs);

			}
			write += written;
			remaining -= written;
		}
	}

	/*virtual override*/ void	FileStream::Read(void*data, serial_size_t size)
	{
		if (handle == -1)
			throw Except::IO::DriveAccess(CLOCATION,"Cannot read data: File is not open");
		unsigned remaining = (unsigned)size;
		BYTE*readPtr = (BYTE*)data;
		while (remaining > 0)
		{
			int read = _read(handle,readPtr,remaining);
			if (read == 0)
				throw Except::IO::DriveAccess(CLOCATION,"Cannot read data: EOF reached");
			if (read < 0)
			{
				int err = errno;
				switch (err)
				{
					case EBADF:
						throw Except::IO::DriveAccess(CLOCATION, "Cannot read data: File handle is invalid");
				}
				char errs[0x100];
				sprintf_s(errs,"Cannot read data: Unexpected error code: %d",err);
				throw Except::IO::DriveAccess(CLOCATION,errs);

			}
			readPtr += read;
			remaining -= read;
		}
	}

	void FileStream::close()
	{
		if (handle != -1)
		{
			_close(handle);
			handle = -1;
		}
	}
}

