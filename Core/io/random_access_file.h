#ifndef fileH
#define fileH

/******************************************************************

File-access tool providing more options for handling volumes.

******************************************************************/

#include "../global_root.h"

#include <stdio.h>
#if SYSTEM==WINDOWS
    #include <io.h>
#endif
#include <fcntl.h>



/*
WINMMAPI HMMIO WINAPI mmioOpen( IN OUT LPSTR pszFileName, IN OUT LPMMIOINFO pmmioinfo, IN DWORD fdwOpen);
WINMMAPI MMRESULT WINAPI mmioClose( IN HMMIO hmmio, IN UINT fuClose);
WINMMAPI LONG WINAPI mmioRead( IN HMMIO hmmio, OUT HPSTR pch, IN LONG cch);
WINMMAPI LONG WINAPI mmioWrite( IN HMMIO hmmio, IN const char _huge* pch, IN LONG cch);
WINMMAPI LONG WINAPI mmioSeek( IN HMMIO hmmio, IN LONG lOffset, IN int iOrigin);
WINMMAPI MMRESULT WINAPI mmioGetInfo( IN HMMIO hmmio, OUT LPMMIOINFO pmmioinfo, IN UINT fuInfo);
WINMMAPI MMRESULT WINAPI mmioSetInfo( IN HMMIO hmmio, IN LPCMMIOINFO pmmioinfo, IN UINT fuInfo);
WINMMAPI MMRESULT WINAPI mmioSetBuffer( IN HMMIO hmmio, IN LPSTR pchBuffer, IN LONG cchBuffer,
    IN UINT fuBuffer);
WINMMAPI MMRESULT WINAPI mmioFlush( IN HMMIO hmmio, IN UINT fuFlush);
WINMMAPI MMRESULT WINAPI mmioAdvance( IN HMMIO hmmio, IN OUT LPMMIOINFO pmmioinfo, IN UINT fuAdvance);
WINMMAPI MMRESULT WINAPI mmioDescend( IN HMMIO hmmio, IN OUT LPMMCKINFO pmmcki,
    IN const MMCKINFO FAR* pmmckiParent, IN UINT fuDescend);
WINMMAPI MMRESULT WINAPI mmioAscend( IN HMMIO hmmio, IN LPMMCKINFO pmmcki, IN UINT fuAscend);
WINMMAPI MMRESULT WINAPI mmioCreateChunk(IN HMMIO hmmio, IN LPMMCKINFO pmmcki, IN UINT fuCreate);
*/

#include "../global_root.h"

namespace DeltaWorks
{

	class RandomAccessFile //! Address orientated data file
	{
	public:
		#if SYSTEM==WINDOWS
			typedef wchar_t	char_t;
		#else
			typedef char char_t;
		#endif

		enum OpenMode       //! File open mode
		{
			Direct,         //!< IO operations are directly applied to the underlying file
			Buffered        //!< IO processes operate on a local buffer rather than the actual file. modifications are applied only when closing the file or manually calling update()
		};
	private:
			FILE       *f;
			BYTE       *buffer;
			bool        write_access,writable,active,changed,referenced;
			UINT        fsize,buffer_size;
			OpenMode    open_mode;

			bool        openStream(bool Truncate=false);
			bool        beginWrite();
			void        applyBufferSize(unsigned new_size); //resizes buffer if necessary. Also applies new_size to fsize

	protected:
			char_t      filename[MAX_PATH];      

	public:
						RandomAccessFile();
	virtual            ~RandomAccessFile();
			const char_t*	GetFilename() const {return filename;}
			bool        Create(const char_t*filename, OpenMode mode=Direct);      //!< Closes if active and creates the specified file. \param filename specifies the name of the file to create \param mode specifies how the file should be handled. \return true if the file could be created and opened, false otherwise.
			bool        ReCreate(OpenMode mode=Direct);                         //!< Closes if active and recreates/overwrites the file specified by the stored filename. \param mode specifies how the file should be handled. \return true if the file could be created and opened, false otherwise.
			bool        Open(const char_t*filename, OpenMode mode=Direct);        //!< Closes if active and opens the specified file in the specified mode. If \b mode is Buffered and the file is accessible, the entire data is read to a local buffer. \param filename specifies the name of the file to open \param mode specifies how the file should be handled. \return true if the file could be opened, false otherwise.
			void        Assign(BYTE*data, unsigned size);                       //!< Closes if active and assigns data of length size bytes as read-only buffer. \param data Pointer to the external data field \param size Size in bytes of the external data field
			void        Assign(const RandomAccessFile&other);
			bool        ReOpen(OpenMode mode);                                  //!< Closes if active and reopens it in the specified mode.
			bool        Update();
			bool        Resize(unsigned new_size);                              //!< Changes the size of the file to \b new_size bytes. If the file-size is increased then the file will be padded using unspecified data. \return true if the file could be resized, false otherwise.
			bool        IsActive();								//!< Queries file status \return true if a file is currently opened, false otherwise
			bool        HasWriteAccess();								//!< Queries write access availability \return true if write access is available or active, false otherwise
			void        Close();									//!< Closes any active file handle. If the file is buffered then it will be updated before closing.
			BYTE*       Extract(unsigned offset, unsigned size);
			bool        Extract(unsigned offset, void*out, unsigned size);
			bool        Erase(unsigned offset, unsigned size);
			bool        Overwrite(unsigned offset, const void*data, unsigned size);
			bool        Overwrite(unsigned offset, BYTE value, unsigned size);
			bool        Insert(unsigned offset, const void*data, unsigned size);
			bool        Insert(unsigned offset, BYTE value, unsigned size);
			bool        Append(const void*data, unsigned size);
			bool        Append(BYTE value, unsigned size);
			UINT        GetSize();									//!< Queries file size \return File size in bytes
	template <class C>
			bool        Insert(unsigned offset, const C&target);
	template <class C>
			bool        Append(const C&target);
	template <class C>
			bool        Read(unsigned offset, C&target);
	template <class C>
			bool        Overwrite(unsigned offset, const C&target);
	};


	/*!

	\fn void RandomAccessFile::Assign(const RandomAccessFile&other)
	\brief Closes if active and duplicates the file-access-information from the other file-object.

	Should the other file operate in buffered mode, then this file will operate on the same buffer in readonly mode.
	Otherwise this file will operate on the same file in the same mode.

	\fn bool RandomAccessFile::update()
	\brief Rewrites the buffer to the open file.

	Use this to explicitly update the file-content when working with buffers.
	\return true if a file is opened in buffered-mode and the contents could be written to it, false otherwise.


	\fn BYTE* RandomAccessFile::extract(unsigned offset, unsigned size)
	\brief Extracts a data-section from the file or buffer.
	\param offset Index of the first byte (0=first byte in the file)
	\param size Length of the section in bytes
	\return Pointer to an array of BYTEs if a file was opened and the extracted section was of size > 0.
	NULL otherwise. The application is responsible for erasing the returned array.

	\fn bool  RandomAccessFile::extract(unsigned offset, void*out, unsigned size)
	\brief Extracts a data-section from the file or buffer and writes it to \b out
	\param offset Index of the first byte (0=first byte in the file)
	\param out Pointer to the output field. Must be at least \b size bytes long.
	\param size Length of the section in bytes
	\return true on success

	\fn bool  RandomAccessFile::Erase(unsigned offset, unsigned size)
	\brief Erases a section from the open file. Any trailing data will be moved to the beginning of the section,
	causing the file to shrink by size bytes.
	\param offset Index of the first byte (0=first byte in the file)
	\param size Length of the section in bytes
	\return true if the section could be removed from the file, false otherwise.

	\fn bool  RandomAccessFile::overwrite(unsigned offset, const void*data, unsigned size)
	\brief Overwrites a section of the open file with the provided data.
	\param offset Index of the first byte (0=first byte in the file)
	\param size Length of the section in bytes
	\param data Pointer to the first byte of the data field
	\return true if the section could successfully be overwritten, false otherwise.

	\fn bool  RandomAccessFile::overwrite(unsigned offset, BYTE value, unsigned size)
	\brief Overwrites a section of the open file by repeating the specified value.
	\param offset Index of the first byte (0=first byte in the file)
	\param value Byte value to repeat
	\param size Length of the section in bytes
	\return true if the section could successfully be overwritten, false otherwise.

	\fn bool  RandomAccessFile::Insert(unsigned offset, const void*data, unsigned size)
	\brief Inserts a data-section of size bytes into the file before the specified offset (offset 0 = before the first byte).
	Inserting at offsets >= file size will result in an append-call.
	\param offset Index of the first byte (0=first byte in the file)
	\param size Length of the section in bytes
	\param data Pointer to the first byte of the data field
	\return true if the section could successfully be inserted, false otherwise.

	\fn bool  RandomAccessFile::Insert(unsigned offset, BYTE value, unsigned size)
	\brief Inserts a sequence of size times value into the file before the specified offset (offset 0 = before the first
	byte).
	Inserting at offsets >= file size will result in an append-call.
	\param offset Index of the first byte (0=first byte in the file)
	\param value Byte value to repeat
	\param size Length of the section in bytes
	\return true if the sequence could successfully be inserted, false otherwise.

	\fn bool  RandomAccessFile::append(const void*data, unsigned size)
	\brief Appends a data-section to the file.
	\param size Length of the section in bytes
	\param data Pointer to the first byte of the data field
	\return true if the section could successfully be appended, false otherwise.

	\fn bool  RandomAccessFile::append(BYTE value, unsigned size)
	\brief Appends a sequence of size times value to the end of the file.
	\param value Byte value to repeat
	\param size Length of the section in bytes
	\return true if the sequence could successfully be appended, false otherwise.


	\fn bool RandomAccessFile::Insert(unsigned offset, const C&target)
	\brief Inserts the given item into the file at the specified offset.
	Identical to \a Insert(offset, &target, sizeof(target));
	\param offset Index of the first byte (0=first byte in the file)
	\param target Reference to the item to insert
	\return true if the item could be inserted at the specified offset, false otherwise.

	\fn bool RandomAccessFile::append(const C&target)
	\brief Appends the given item to the end of the file.
	Identical to \a append(&target, sizeof(target));
	\param target Reference to the item to append
	\return true if the item could be appended, false otherwise.

	\fn bool RandomAccessFile::read(unsigned offset, C&target)
	\brief Reads target from the specified offset.
	Identical to \a extract(offset, &target, sizeof(target));
	\param offset Index of the first byte (0=first byte in the file)
	\param target Reference to the item to read
	\return true if the target could be read, false otherwise.

	\fn bool RandomAccessFile::overwrite(unsigned offset, const C&target)
	\brief Overwrites the data at the specified offset with the given item.
	Identical to \a overwrite(offset, &item, sizeof(item));
	\param offset Index of the first byte (0=first byte in the file)
	\param target Reference to the item to overwrite
	\return true if the item could be written, false otherwise.


	*/


	#include "random_access_file.tpl.h"
}

#endif

