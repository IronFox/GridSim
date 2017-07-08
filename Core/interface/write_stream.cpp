#include "write_stream.h"


namespace DeltaWorks
{
	void		IWriteStream::WriteSize(serial_size32_t size)
	{
		WriteSize((serial_size64_t)size);
	}

	void		IWriteStream::WriteSize(serial_size64_t size)
	{
		BYTE b[8];
		if (size <= 0x3F)
		{
			b[0] = ((BYTE)size);
			Write(b,1);
			return;
		}
		if (size <= 0x3FFF)
		{
			b[0] = (BYTE)((size >> 8)&0x3F) | 0x40;
			b[1] = (BYTE)(size&0xFF);
			Write(b,2);
			return;
		}
		if (size <= 0x3FFFFF)
		{
			b[0] = (BYTE)((size >> 16)&0x3F) | 0x80;
			b[1] = (BYTE)((size>>8)&0xFF);
			b[2] = (BYTE)((size)&0xFF);
			Write(b,3);
			return;
		}
		if (size < (1U<<29))
		{
			b[0] = (BYTE)((size >> 24)&0x1F) | 0xC0;
			b[1] = (BYTE)((size >> 16)&0xFF);
			b[2] = (BYTE)((size >> 8)&0xFF);
			b[3] = (BYTE)((size)&0xFF);
			Write(b,4);
			return;
		}
		if (size >= (1ULL<<61))
			throw Except::Memory::SerializationFault(CLOCATION, "Trying to serialize size exceeding maximum size of 2^61-1");
		b[0] = (BYTE)((size >> 56)&0x3F) | 0xE0;
		b[1] = (BYTE)((size >> 48)&0xFF);
		b[2] = (BYTE)((size >> 40)&0xFF);
		b[3] = (BYTE)((size >> 32)&0xFF);
		b[4] = (BYTE)((size >> 24)&0xFF);
		b[5] = (BYTE)((size >> 16)&0xFF);
		b[6] = (BYTE)((size >> 8)&0xFF);
		b[7] = (BYTE)((size)&0xFF);
		Write(b,8);
	}

}
