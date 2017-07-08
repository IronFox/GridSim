#include "read_stream.h"


namespace DeltaWorks
{
	void					IReadStream::ReadSize(serial_size64_t&sizeOut)
	{
		BYTE b[8];
		Read(b,1);
		const BYTE flags2 = b[0] & 0xC0;
		BYTE flags3 = b[0] & 0xE0;
		if (flags2 != 0xC0)
			flags3 &= 0xC0;


		typedef serial_size64_t T;

		switch (flags3)
		{
			case 0:
				sizeOut = b[0];
				return;
			break;
			case 0x40:
				Read(b+1,1);
				sizeOut = (((T)(b[0] & 0x3F)) << 8) | b[1];
				return;
			break;
			case 0x80:
				Read(b+1,2);
				sizeOut = (((T)(b[0] & 0x3F)) << 16) | (((T)b[1])<<8) | (((T)b[2]));
				return;
			break;
			case 0xC0:
				Read(b+1,3);
				sizeOut = (((T)(b[0] & 0x1F)) << 24) | (((T)b[1])<<16) | (((T)b[2])<<8) | (((T)b[3]));
				return;
			break;
			case 0xE0:
				Read(b+1,7);
				sizeOut = (((T)(b[0] & 0x1F)) << 56) 
						| (((T)b[1])<<48)
						| (((T)b[2])<<40)
						| (((T)b[3])<<32)
						| (((T)b[4])<<24)
						| (((T)b[5])<<16)
						| (((T)b[6])<<8)
						| (((T)b[7]));
				return;
			default:
				FATAL__("Logical issue");	//this should be impossible due to the mask above
				return;
		}
	}

}