#ifndef file_streamH
#define file_streamH

#include <fcntl.h>


#if SYSTEM!=WINDOWS
	#define _close	::close
	#define	_open	::open
	#define	_read	::read
	#define	_write	::write
	
	#define	__O_SEQUENTIAL	0	//not supported
	#define	__O_RANDOM		0	//not supported
	#define __O_APPEND		O_APPEND
	#define	__O_CREAT		O_CREAT
	#define __O_RDONLY		O_RDONLY
	#define __O_WRONLY		O_WRONLY
	#define __O_RDWR		O_RDWR
	#define	__O_TRUNC		O_TRUNC
	#define	__O_EXCL		O_EXCL
	#define	__O_NONBLOCK	O_NONBLOCK
	#define	__O_SHLOCK		0
	#define	__O_EXLOCK		0
	#define	__O_NOFOLLOW	O_NOFOLLOW
	#define __O_SYMLINK		0
#else
	#include <io.h>

	#define __O_APPEND		_O_APPEND|_O_BINARY
	#define	__O_CREAT		_O_CREAT|_O_BINARY
	#define __O_RDONLY		_O_RDONLY|_O_BINARY
	#define __O_WRONLY		_O_WRONLY|_O_BINARY
	#define __O_RDWR		_O_RDWR|_O_BINARY
	#define	__O_TRUNC		_O_TRUNC
	#define	__O_EXCL		_O_EXCL
	#define	__O_NONBLOCK	0	//not supported
	#define	__O_SHLOCK		0	//not supported
	#define	__O_EXLOCK		0	//not supported
	#define	__O_NOFOLLOW	0	//not supported
	#define __O_SYMLINK		0	//not supported
	#define	__O_SEQUENTIAL	_O_SEQUENTIAL
	#define	__O_RANDOM		_O_RANDOM
#endif


/**
	@brief Abstracted IO file class
*/
class FileStream:public IReadStream, public IWriteStream
{
protected:
		int				handle;
		
						FileStream(const FileStream&):handle(-1)
						{}
		FileStream&	operator=(const FileStream&)
						{
							return *this;
						}
public:
		enum flag_t	//!< Renamed and predefined flag combinations
		{
			ReadOnly = __O_RDONLY,        //!< open for reading only
			WriteOnly = __O_WRONLY,		//!< open for writing only
			ReadWrite = __O_RDWR,			//!< open for reading and writing
			NonBlock = __O_NONBLOCK,		//!< do not block on open or for data to become available
			Append = __O_APPEND,			//!< append on each write
			Create = __O_CREAT,			//!< create file if it does not exist
			Truncate = __O_TRUNC,			//!< truncate size to 0
			ForceNew = __O_EXCL,			//!< error if O_CREAT and the file exists
			SharedLock = __O_SHLOCK,		//!< atomically obtain a shared lock
			ExclusiveLock = __O_EXLOCK,	//!< atomically obtain an exclusive lock
			NoSymlinks = __O_NOFOLLOW,	//!< do not follow symlinks
			Symlinks = __O_SYMLINK,		//!< allow open of symlinks
			Sequential = __O_SEQUENTIAL,	//!< sequential access optimization
			Random = __O_RANDOM,			//!< random access optimization
			
			StandardRead = ReadOnly | SharedLock | Symlinks | Sequential,							//!< Standard read flag combination. File is opened for read only on a shared lock following symlinks
			StandardWrite = WriteOnly | Create | Truncate | ExclusiveLock | Symlinks | Sequential,	//!< Standard write flag combination. File is opened for write only on a new or truncated file and an exclusive lock following symlinks
			StandardAppend = WriteOnly | Create | Append | ExclusiveLock | Symlinks	| Sequential	//!< Standard append flag combination. File is opened for write only on a new or existing file and an exclusive lock following symlinks
		};

						FileStream():handle(-1){};
						FileStream(const char*filename, int flags):handle(-1)
						{
							open(filename,flags);
						}
						FileStream(const wchar_t*filename, int flags):handle(-1)
						{
							open(filename,flags);
						}
virtual					~FileStream()
						{
							close();
						}
		
		bool			open(const char*filename, int flags);	//!< Opens a file in the specified mode. @param filename Filename to open @return true on success
		bool			open(const wchar_t*filename, int flags);//!< Opens a file in the specified mode. @param filename Filename to open @return true on success
		
		bool			OpenCreate(const char*filename)	{ return open(filename, StandardWrite); }
		bool			OpenAppend(const char*filename) { return open(filename, StandardAppend); }
		bool			OpenRead(const char*filename) { return open(filename, StandardRead); }
		bool			OpenCreate(const wchar_t*filename)	{ return open(filename, StandardWrite); }
		bool			OpenAppend(const wchar_t*filename) { return open(filename, StandardAppend); }
		bool			OpenRead(const wchar_t*filename) { return open(filename, StandardRead); }

		bool			isOpen()	const	//!< Queries whether or not the local file is open for reading or writing
						{
							return handle != -1;
						}
		bool			isActive()	const	//!< @overload
						{
							return handle != -1;
						}

		void			close();
	
		uint64_t		size()	const
						{
							return isOpen() ? _filelengthi64(handle) : 0;
						}
		
	virtual bool		Write(const void*data, serial_size_t size) override;	//!< Writes a section of binary data to the local file. File must be opened in write mode or this operation will fail
	virtual bool		Read(void*data, serial_size_t size) override;			//!< Reads a section of binary data from the local file. File must be opened in read mode or this operation will fail
	serial_size_t		GetRemainingBytes() const override {return isOpen()? (serial_size_t)(_filelengthi64(handle) - _telli64(handle)): 0;}


	template <typename T>
		inline	bool	readPrimitive(T&target)
						{
							return read(&target,sizeof(target));
						}
	template <typename T>
		inline	bool	writePrimitive(const T&primitive)
						{
							return write(&primitive,sizeof(primitive));
						}
};


#endif
