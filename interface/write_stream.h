#ifndef iout_streamH
#define iout_streamH

#include "common.h"


template <bool LocalIsLittleEndian>
	inline serial_size_t	encodeLittleEndian(serial_size_t size)
	{
		return size;
	}

template <>
	inline serial_size_t	encodeLittleEndian<false>(serial_size_t size)
	{
		//return (size>>56)|(((size>>48)&0xFF)<<8)|(((size>>40)&0xFF)<<16)|(((size>>32)&0xFF)<<24)|(((size>>24)&0xFF)<<32)|(((size>>16)&0xFF)<<40)|(((size>>8)&0xFF)<<48)|((size&0xFF)<<56);
		return (size>>24)|(((size>>16)&0xFF)<<8)|(((size>>8)&0xFF)<<16)|((size&0xFF)<<24);
	}

	inline serial_size_t	encodeLittleEndianRT(serial_size_t size)
	{
		if (isLittleEndian())
			return encodeLittleEndian<true>(size);
		else
			return encodeLittleEndian<false>(size);
	}

/*!
	@brief Generic data output stream
	
	Derivatives of IOutStream provide an interface to write blocks of data. Such a stream could (for instance) be a file, tcp stream, or pipe.
*/
interface IWriteStream
{
protected:
virtual			~IWriteStream()	{};
public:
virtual	bool	write(const void*data, serial_size_t size)=0;	//!< Writes a chunk of binary data to the stream @param data Pointer to the memory section that should be written @param size Number of bytes that should be written @return true if the specified amount of bytes could be written to the stream, false otherwise

	template <typename T>
		bool	writePrimitive(const T&element)
		{
			return write(&element,sizeof(element));
		}
	template <typename T>
		bool	writePrimitives(const T*elements, count_t num_elements)
		{
			return write(elements,sizeof(T)*num_elements);
		}

		bool	writeSize(serial_size_t size)	//! Reads the content of a size variable in little endian 
		{
			BYTE b[4];
			if (size <= 0x3F)
			{
				b[0] = ((BYTE)size);
				return write(b,1);
			}
			if (size <= 0x3FFF)
			{
				b[0] = (BYTE)((size >> 8)&0x3F) | 0x40;
				b[1] = (BYTE)(size&0xFF);
				return write(b,2);
			}
			if (size <= 0x3FFFFF)
			{
				b[0] = (BYTE)((size >> 16)&0x3F) | 0x80;
				b[1] = (BYTE)((size>>8)&0xFF);
				b[2] = (BYTE)((size)&0xFF);
				return write(b,3);
			}
			b[0] = (BYTE)((size >> 24)&0x3F) | 0xC0;
			b[1] = (BYTE)((size >> 16)&0xFF);
			b[2] = (BYTE)((size >> 8)&0xFF);
			b[3] = (BYTE)((size)&0xFF);
			return write(b,4);
		}

		bool	writeSize(UINT64 size)
		{
			if (size > 0xFFFFFFFF)
				throw Program::ParameterFault("Attempting to write serial size larger than can be expressed in 32 bit");
			return writeSize((serial_size_t)size);
		}

};


/*!
	@brief Binary memory section write-stream
	
	MemWriteStream allows stream writing to a memory section. MemWriteStream does not delete any assigned data on destruction.
*/
class MemWriteStream: public IWriteStream
{
protected:
		BYTE	*field,	//!< Pointer to the current memory location that is next written to. Auto incremented as needed
				*end;	//!< Pointer one past the last available byte of the out-buffer. Data can be written up to but not past this pointer.

public:
					MemWriteStream(void*data=NULL,serial_size_t size = 0)
					{
						assign(data,size);
					}
		void		assign(void*data, serial_size_t size)	//!< Assigns a new field to the local memory stream
					{
						field = (BYTE*)data;
						end = field+size;
					}
		
		BYTE*		current()	const
					{
						return field;
					}
				
		bool		write(const void*data, serial_size_t size)
					{
						if (field+size > end)
							return false;
						memcpy(field,data,size);
						field+=size;
						return true;
					}
};

#endif
