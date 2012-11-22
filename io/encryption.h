#ifndef encryptionH
#define encryptionH

/******************************************************************

sha1, md4 and rc4 hashsum-calculators/encryptors.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include "../math/basic.h"

UINT32 swap32(UINT32 value);

class CSHA256;
class CSHA1;
class CMD5;
class CRC4;



namespace Encryption
{
        void        hash48(const void*in, const void*salt, const void*pepper, BYTE base, void*out);
        void        hash16(const void*in, const void*salt, const void*pepper, void*out);
        void        sign(const void*key, size_t keylen, const void*data, size_t datalen, void*signature, BYTE signature_len);
}


template <UINT32 int32_count>
	struct THash
	{
		union
		{
			BYTE	bytes[int32_count*4];
			UINT32	ints[int32_count];
		};
		
		
		bool	operator==(const THash<int32_count>&other)	const
				{
					return !memcmp(bytes,other.bytes,sizeof(bytes));
				}
	};



class CRC4
{
protected:
        UINT32      _x,
                    _y,
                    _data[0x100];

public:
                    CRC4();
                    CRC4(const void*data, size_t size);
        void        set(const void*data, size_t size);
        void        apply(const void*source, void*target, size_t size);
};


class CMD5
{
protected:
        UINT32      _a,_b,_c,_d;
        UINT64      _total;

        BYTE        _data[64];

        void        transform(UINT32*in);

public:
                    CMD5();
        void        update(const void*source, size_t size);
        void        finish(void*target);
};

class CSHA1
{
private:
	    UINT32      h0,h1,h2,h3,h4,
	                sz0,sz1;
	    BYTE        data[64];

        void        transform(UINT32*);

public:
                    CSHA1();
        void        update(const void*source, size_t size);
        void        finish(void*target);
};


class CSHA256
{
protected:
	    UINT32      h0,h1,h2,h3,h4,h5,h6,h7;
					
		static const size_t chunk_size = 64;
		
		UINT32		w[64];
		BYTE		*current;
		UINT64		total;
		
static	UINT32		k[64];


		void 		processChunk();

public:
		typedef THash<8>	Hash;


                    CSHA256();
		void		reset();
        void        append(const void*source, size_t size);
        void        finish(void*target);
        void        finish(Hash&target);
};



using namespace Encryption;


#endif
