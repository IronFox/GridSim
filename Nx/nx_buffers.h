#ifndef nx_buffersH
#define nx_buffersH

#include <NxStream.h>
#include "../container/buffer.h"

/**
	@brief Write-only NxStream compatible write buffer
*/
class NxWriteBuffer:public Buffer<BYTE>, public NxStream
{
public:
		virtual		NxU8			readByte()								const
		{
			FATAL__("Attempting to read from a native NxBuffer");
			return 0;
		}
	
		virtual		NxU16			readWord()								const
		{
			FATAL__("Attempting to read from a native NxBuffer");
			return 0;
		}

		virtual		NxU32			readDword()								const
		{
			FATAL__("Attempting to read from a native NxBuffer");
			return 0;
		}

		virtual		NxF32			readFloat()								const
		{
			FATAL__("Attempting to read from a native NxBuffer");
			return 0;
		}

		virtual		NxF64			readDouble()							const
		{
			FATAL__("Attempting to read from a native NxBuffer");
			return 0;
		}

		virtual		void			readBuffer(void* buffer, NxU32 size)	const
		{
			FATAL__("Attempting to read from a native NxBuffer");
		}


		virtual		NxStream&		storeByte(NxU8 b)
		{
			Buffer<BYTE>::operator<<((BYTE)b);
			return *this;
		}
	
		virtual		NxStream&		storeWord(NxU16 w)
		{
			Buffer<BYTE>::append((const BYTE*)&w,sizeof(w));
			return *this;
		}
	
		virtual		NxStream&		storeDword(NxU32 d)
		{
			Buffer<BYTE>::append((const BYTE*)&d,sizeof(d));
			return *this;
		}

		virtual		NxStream&		storeFloat(NxF32 f)
		{
			Buffer<BYTE>::append((const BYTE*)&f,sizeof(f));
			return *this;
		}
		
		virtual		NxStream&		storeDouble(NxF64 f)
		{
			Buffer<BYTE>::append((const BYTE*)&f,sizeof(f));
			return *this;
		}
	
		virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size)
		{
			Buffer<BYTE>::append((const BYTE*)buffer,size);
			return *this;
		}

};

/**
	@brief Strictly read-only byte stream
*/
class NxReader:public NxStream
{
protected:
		const BYTE					*begin,	//!< Pointer to the first element of the client region
									*end;	//!< Pointer one past the last element of the client region
		mutable	const BYTE			*current;	//!< Pointer to the next read element of the client region
public:
		
									NxReader():begin(NULL),end(NULL),current(NULL)
									{}
									NxReader(const BYTE*section, size_t length):begin(section),end(section+length),current(section)
									{}
									NxReader(const BYTE*section_begin, const BYTE*section_end):begin(section_begin),end(section_end),current(section_begin)
									{}
								template <typename T>
									NxReader(const Buffer<T>&buffer):begin((const BYTE*)buffer.pointer()),end((const BYTE*)(buffer.pointer()+buffer.length())),current((const BYTE*)buffer.pointer())
									{}
								template <typename T>
									NxReader(const Array<T>&buffer):begin((const BYTE*)buffer.pointer()),end((const BYTE*)(buffer.pointer()+buffer.length())),current((const BYTE*)buffer.pointer())
									{}

		void						reset()
		{
			current = begin;
		}
		
		unsigned					remaining()
		{
			return end-current;
		}
		

		virtual		NxU8			readByte()								const
		{
			ASSERT__(current < end);
			return *current++;
		}
	
		virtual		NxU16			readWord()								const
		{
			NxU16 rs;
			readBuffer(&rs,sizeof(rs));
			return rs;
		}

		virtual		NxU32			readDword()								const
		{
			NxU32 rs;
			readBuffer(&rs,sizeof(rs));
			return rs;
		}

		virtual		NxF32			readFloat()								const
		{
			NxF32 rs;
			readBuffer(&rs,sizeof(rs));
			return rs;
		}

		virtual		NxF64			readDouble()							const
		{
			NxF64 rs;
			readBuffer(&rs,sizeof(rs));
			return rs;
		}

		virtual		void			readBuffer(void* buffer, NxU32 size)	const
		{
			ASSERT3__(current+size<=end,current-begin,size,end-begin);
			memcpy(buffer,current,size);
			current += size;
		}


		virtual		NxStream&		storeByte(NxU8 b)
		{
			FATAL__("Attempting to write to a native NxReadBuffer");
			return *this;
		}
	
		virtual		NxStream&		storeWord(NxU16 w)
		{
			FATAL__("Attempting to write to a native NxReadBuffer");
			return *this;
		}
	
		virtual		NxStream&		storeDword(NxU32 d)
		{
			FATAL__("Attempting to write to a native NxReadBuffer");
			return *this;
		}

		virtual		NxStream&		storeFloat(NxF32 f)
		{
			FATAL__("Attempting to write to a native NxReadBuffer");
			return *this;
		}
		
		virtual		NxStream&		storeDouble(NxF64 f)
		{
			FATAL__("Attempting to write to a native NxReadBuffer");
			return *this;
		}
	
		virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size)
		{
			FATAL__("Attempting to write to a native NxReadBuffer");
			return *this;
		}

};


#endif
