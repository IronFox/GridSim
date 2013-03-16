#ifndef iin_streamH
#define iin_streamH

#include "common.h"



template <bool LocalIsLittleEndian>
	inline serial_size_t	decodeLittleEndian(serial_size_t size)
	{
		return size;
	}

template <>
	inline serial_size_t	decodeLittleEndian<false>(serial_size_t size)
	{
		//return (size>>56)|(((size>>48)&0xFF)<<8)|(((size>>40)&0xFF)<<16)|(((size>>32)&0xFF)<<24)|(((size>>24)&0xFF)<<32)|(((size>>16)&0xFF)<<40)|(((size>>8)&0xFF)<<48)|((size&0xFF)<<56);

		return (size>>24)|(((size>>16)&0xFF)<<8)|(((size>>8)&0xFF)<<16)|((size&0xFF)<<24);
	}

	inline serial_size_t	decodeLittleEndianRT(serial_size_t size)
	{
		if (isLittleEndian())
			return decodeLittleEndian<true>(size);
		else
			return decodeLittleEndian<false>(size);
	}


/*!
	@brief Generic stream data source
	
	Derivatives of IReadStream provide an interface to load blocks of data. Such a stream could (for instance) be a file, tcp stream, or pipe.
*/
interface IReadStream
{
protected:
	virtual			~IReadStream()	{};
public:
	virtual	bool	read(void*target_data, serial_size_t size)=0;	//!< Loads a chunk of binary data from the stream @param target_data Pointer to the memory section that read data should be written to @param size Number of bytes that should be read @return true if the requested amount of bytes could be read from the stream, false otherwise
	virtual bool	read(volatile void*data, serial_size_t size)	{return read(const_cast<void*>(data),size);}

	template <typename T>
		bool	readPrimitive(T&element)
		{
			return read(&element,sizeof(element));
		}
	template <typename T>
		bool	readPrimitives(T*elements, count_t num_elements)
		{
			return read(elements,sizeof(T)*num_elements);
		}
	template <typename T>
		bool	readSize(T&size_out)	//! Reads the content of a size variable in little endian 
		{
			BYTE b[4];
			if (!read(b,1))
				return false;
			BYTE flags = b[0] & 0xC0;

			switch (flags)
			{
				case 0:
					size_out = b[0];
					return true;
				break;
				case 0x40:
					if (!read(b+1,1))
						return false;
					size_out = (((T)(b[0] & 0x3F)) << 8) | b[1];
					return true;
				break;
				case 0x80:
					if (!read(b+1,2))
						return false;
					size_out = (((T)(b[0] & 0x3F)) << 16) | (((T)b[1])<<8) | (((T)b[2]));
					return true;
				break;
				case 0xC0:
					if (!read(b+1,3))
						return false;
					size_out = (((T)(b[0] & 0x3F)) << 24) | (((T)b[1])<<16) | (((T)b[2])<<8) | (((T)b[3]));
					return true;
				break;
				default:
					FATAL__("Logical issue");
					return false;
			}
		}
};




/*!
	@brief Binary memory section in-stream
	
	MemReadStream allows stream reading from a memory section. MemReadStream does not delete any assigned data on destruction.
*/
class MemReadStream: public IReadStream
{
protected:
		const BYTE	*field,	//!< Pointer to the current memory location that is next read from. Auto incremented as needed
					*end;	//!< Pointer one past the last available byte. Data can be read up to but not past this pointer.
public:

					MemReadStream(const void*data=NULL,serial_size_t size = 0)
					{
						assign(data,size);
					}
		void		assign(const void*data, serial_size_t size)	//!< Assigns a new field to the local memory stream
					{
						field = (const BYTE*)data;
						end = field+size;
					}
		const BYTE*	current()	const
					{
						return field;
					}

					
		bool		read(void*target_data, serial_size_t size)
					{
						if (field+size > end)
							return false;
						memcpy(target_data,field,size);
						field+=size;
						return true;
					}
};

#endif
