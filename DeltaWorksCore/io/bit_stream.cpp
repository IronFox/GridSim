#include "../global_root.h"
#include "bit_stream.h"

/******************************************************************

Bitstream-class. Helps compressing/decompressing data.

******************************************************************/


namespace DeltaWorks
{

	BitStream::BitStream(void*Stream, unsigned Len):max_len(Len*8),stream((unsigned char*)Stream),read_stream((unsigned char*)Stream),offset(0),markedn(0),error_(false)
	{}

	BitStream::BitStream(const void*Stream, unsigned Len):max_len(Len*8),stream(NULL),read_stream((unsigned char*)Stream),offset(0),markedn(0),error_(false)
	{}

	void BitStream::assign(void*field, unsigned len)
	{
		read_stream = stream = (BYTE*)field;
		offset = 0;
		markedn = 0;
		max_len = len*8;
		error_ = false;
	}

	void BitStream::assignRead(const void*field, unsigned len)
	{
		read_stream = (BYTE*)field;
		stream = NULL;
		offset = 0;
		markedn = 0;
		max_len = len*8;
		error_ = false;
	}

	void BitStream::push(unsigned char bit)
	{
		if (offset>=max_len || !stream)
		{
			error_ = true;
			return;
		}
		stream[offset/8]<<=1;
		stream[offset/8]+=bit;
		offset++;
	}


	unsigned char BitStream::showByte()
	{
		return read_stream[offset/8];
	}

	void BitStream::seek(unsigned addr)
	{
		offset = addr;
	}


	/*
	void BitStream::pushCheckUnsigned(unsigned value, unsigned bits)
	{
	unsigned test = 0;
	mark();
	push(&value,bits);
	finalize();
	positionRecall();
	get(&test,bits);
	if (value!=test)
	   ShowMessage("write-test failed: "+IntToStr(value)+" changed to "+IntToStr(test)+" at "+IntToStr(offset)+" with len "+IntToStr(bits));
	definalize();   
	}
	*/

	void BitStream::push(const void*pntr, unsigned bits)
	{
		if (!stream)
		{
			error_ = true;
			return;
		}
		const BYTE*tpntr = reinterpret_cast<const BYTE*>(pntr);
		BYTE    bitb[8],
				pos = 0;
		while (bits)
		{
			BYTE byte = tpntr[pos];
			for (unsigned i = 0; i < 8; i++)
			{
				if (bits < 8)
					bitb[i] = byte/0x80;
				else
					push(byte/0x80);
				byte <<= 1;
			}
			if (bits < 8)
			   for (BYTE i = 8-bits; i<8; i++)
				   push(bitb[i]);
			pos++;
			if (bits > 8)
				bits-=8;
			else
				bits = 0;
		}
	}

	void BitStream::drop()
	{
		if (!stream)
		{
			error_ = true;
			return;
		}
		offset--;
		stream[offset/8]>>=1;
	}

	void BitStream::drop(unsigned bits)
	{
		if (!stream)
		{
			error_ = true;
			return;
		}
		if (bits > offset)
			bits = offset;
		for (unsigned i = 0; i < bits; i++)
			drop();
	}

	void BitStream::dropTo(unsigned length)
	{
		while (length<offset)
			drop();
	}

	void BitStream::finalize()
	{
		if (stream && (offset%8))
		   stream[offset/8] <<= (unsigned char)(8-offset%8);
	}

	void BitStream::definalize()
	{
		if (stream && (offset%8))
		   stream[offset/8] >>= (unsigned char)(8-offset%8);
	}



	void BitStream::reset()
	{
		error_ = false;
		offset = 0;
	}

	void BitStream::mark()
	{
		marked[markedn++] = offset;
	}

	void BitStream::dropRecall()
	{
		markedn--;
		while (offset > marked[markedn])
			  drop();
	}

	void BitStream::positionRecall()
	{
		offset = marked[--markedn];
	}

	void BitStream::ignoreRecall()
	{
		markedn--;
	}

	unsigned BitStream::tellSize()
	{
		return offset/8+(offset%8?1:0);
	}

	unsigned BitStream::tellByte()
	{
		return offset/8;
	}


	unsigned BitStream::tellBits()
	{
		return offset;
	}

	void BitStream::get(void*pntr, unsigned bits)
	{
		unsigned char*tpntr = reinterpret_cast<unsigned char*>(pntr);
		for (unsigned i = 0; i < bits; i++)
		{
			tpntr[i/8]<<=1;
			tpntr[i/8] += get();
		};
	}

	void BitStream::get(unsigned Offset, void*pntr, unsigned bits)
	{
		offset = Offset;
		get(pntr,bits);
	}

	bool BitStream::get()
	{
		unsigned char byte = read_stream[offset/8],
					  bit = offset%8;
		byte <<= bit;
		bit = byte/0x80;
		offset++;
		return bit!=0;
	}

	bool BitStream::get(unsigned Offset)
	{
		offset = Offset;
		return get();
	}
}