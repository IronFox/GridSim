#ifndef bit_streamH
#define bit_streamH

/******************************************************************

Bitstream-class. Helps compressing/decompressing data.

******************************************************************/

namespace DeltaWorks
{
	class BitStream;

	//---------------------------------------------------------------------------
	class BitStream
	{
	private:
        unsigned    max_len;
        BYTE		*stream;
		const BYTE	*read_stream;
        UINT32      offset,marked[1024],markedn;
        BYTE        buffer[8];
	public:
        bool        error_;
        
                    BitStream(void*field, unsigned max_len);
                    BitStream(const void*read_field, unsigned max_len);
        void        assign(void*field, unsigned max_len);
        void        assignRead(const void*field, unsigned max_len);
        void        push(unsigned char bit);
        void        push(const void*pntr, unsigned bits);
//        void     pushCheckUnsigned(unsigned value, unsigned bits);
        void        drop();
        void        drop(unsigned bits);
        void        dropTo(unsigned length);
        void        finalize();
        void        definalize();
        void        reset();
        void        mark();
        void        dropRecall();
        void        positionRecall();
        void        ignoreRecall();
        void        seek(unsigned addr);
        unsigned    tellSize();
        unsigned    tellByte();
        unsigned    tellBits();
        BYTE        showByte();
        void        get(void*pntr, unsigned bits);
        void        get(unsigned offset, void*pntr, unsigned bits);
        bool        get();
        bool        get(unsigned offset);
	};
}

#endif
