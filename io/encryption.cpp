#include "../global_root.h"
#include "encryption.h"

/******************************************************************

sha1, md4 and rc4 hashsum-calculators/encryptors.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


UINT32 swap32(UINT32 value)
{
    BYTE*inpntr=(BYTE*)&value;
    BYTE out[4];
    out[0] = inpntr[3];
    out[1] = inpntr[2];
    out[2] = inpntr[1];
    out[3] = inpntr[0];
    return *(UINT32*)out;
}


CMD5::CMD5():_a(0x67452301UL),_b(0xEFCDAB89UL),_c(0x98BADCFEUL),_d(0x10325476UL),_total(0)
{}

void CMD5::transform(UINT32*data)
{
    UINT32 A, B, C, D, X[16];
    for (BYTE k = 0; k < 16; k++)
        X[k] = data[k];       //swap here
    #define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

    #define P(a,b,c,d,k,s,t)                \
    {                           \
        a += F(b,c,d) + X[k] + t; a = S(a,s) + b;       \
    }
    A = _a;
    B = _b;
    C = _c;
    D = _d;

    #define F(x,y,z) (z ^ (x & (y ^ z)))

        P( A, B, C, D,  0,  7, 0xD76AA478 );
        P( D, A, B, C,  1, 12, 0xE8C7B756 );
        P( C, D, A, B,  2, 17, 0x242070DB );
        P( B, C, D, A,  3, 22, 0xC1BDCEEE );
        P( A, B, C, D,  4,  7, 0xF57C0FAF );
        P( D, A, B, C,  5, 12, 0x4787C62A );
        P( C, D, A, B,  6, 17, 0xA8304613 );
        P( B, C, D, A,  7, 22, 0xFD469501 );
        P( A, B, C, D,  8,  7, 0x698098D8 );
        P( D, A, B, C,  9, 12, 0x8B44F7AF );
        P( C, D, A, B, 10, 17, 0xFFFF5BB1 );
        P( B, C, D, A, 11, 22, 0x895CD7BE );
        P( A, B, C, D, 12,  7, 0x6B901122 );
        P( D, A, B, C, 13, 12, 0xFD987193 );
        P( C, D, A, B, 14, 17, 0xA679438E );
        P( B, C, D, A, 15, 22, 0x49B40821 );

    #undef F

    #define F(x,y,z) (y ^ (z & (x ^ y)))

        P( A, B, C, D,  1,  5, 0xF61E2562 );
        P( D, A, B, C,  6,  9, 0xC040B340 );
        P( C, D, A, B, 11, 14, 0x265E5A51 );
        P( B, C, D, A,  0, 20, 0xE9B6C7AA );
        P( A, B, C, D,  5,  5, 0xD62F105D );
        P( D, A, B, C, 10,  9, 0x02441453 );
        P( C, D, A, B, 15, 14, 0xD8A1E681 );
        P( B, C, D, A,  4, 20, 0xE7D3FBC8 );
        P( A, B, C, D,  9,  5, 0x21E1CDE6 );
        P( D, A, B, C, 14,  9, 0xC33707D6 );
        P( C, D, A, B,  3, 14, 0xF4D50D87 );
        P( B, C, D, A,  8, 20, 0x455A14ED );
        P( A, B, C, D, 13,  5, 0xA9E3E905 );
        P( D, A, B, C,  2,  9, 0xFCEFA3F8 );
        P( C, D, A, B,  7, 14, 0x676F02D9 );
        P( B, C, D, A, 12, 20, 0x8D2A4C8A );

    #undef F

    #define F(x,y,z) (x ^ y ^ z)

        P( A, B, C, D,  5,  4, 0xFFFA3942 );
        P( D, A, B, C,  8, 11, 0x8771F681 );
        P( C, D, A, B, 11, 16, 0x6D9D6122 );
        P( B, C, D, A, 14, 23, 0xFDE5380C );
        P( A, B, C, D,  1,  4, 0xA4BEEA44 );
        P( D, A, B, C,  4, 11, 0x4BDECFA9 );
        P( C, D, A, B,  7, 16, 0xF6BB4B60 );
        P( B, C, D, A, 10, 23, 0xBEBFBC70 );
        P( A, B, C, D, 13,  4, 0x289B7EC6 );
        P( D, A, B, C,  0, 11, 0xEAA127FA );
        P( C, D, A, B,  3, 16, 0xD4EF3085 );
        P( B, C, D, A,  6, 23, 0x04881D05 );
        P( A, B, C, D,  9,  4, 0xD9D4D039 );
        P( D, A, B, C, 12, 11, 0xE6DB99E5 );
        P( C, D, A, B, 15, 16, 0x1FA27CF8 );
        P( B, C, D, A,  2, 23, 0xC4AC5665 );

    #undef F

    #define F(x,y,z) (y ^ (x | ~z))

        P( A, B, C, D,  0,  6, 0xF4292244 );
        P( D, A, B, C,  7, 10, 0x432AFF97 );
        P( C, D, A, B, 14, 15, 0xAB9423A7 );
        P( B, C, D, A,  5, 21, 0xFC93A039 );
        P( A, B, C, D, 12,  6, 0x655B59C3 );
        P( D, A, B, C,  3, 10, 0x8F0CCC92 );
        P( C, D, A, B, 10, 15, 0xFFEFF47D );
        P( B, C, D, A,  1, 21, 0x85845DD1 );
        P( A, B, C, D,  8,  6, 0x6FA87E4F );
        P( D, A, B, C, 15, 10, 0xFE2CE6E0 );
        P( C, D, A, B,  6, 15, 0xA3014314 );
        P( B, C, D, A, 13, 21, 0x4E0811A1 );
        P( A, B, C, D,  4,  6, 0xF7537E82 );
        P( D, A, B, C, 11, 10, 0xBD3AF235 );
        P( C, D, A, B,  2, 15, 0x2AD7D2BB );
        P( B, C, D, A,  9, 21, 0xEB86D391 );

    #undef F
    #undef P
    #undef S

    _a += A;
    _b += B;
    _c += C;
    _d += D;
}

void CMD5::update(const void*source_, size_t len)
{
    const BYTE*source = (const BYTE*)source_;
    size_t left, fill;
    if (!len)
        return;
    left = (_total & 0x3F);
    fill = 64 - left;
    _total += len;

    if(left && len >= fill )
    {
        memcpy(&_data[left], source, fill);
        transform((UINT32*)_data);
        len -= fill;
        source += fill;
        left = 0;
    }

    while(len >= 64)
    {
        transform((UINT32*)source);
        len -= 64;
        source += 64;
    }

    if(len)
    {
        memcpy(&_data[left],source,len);
    }
}


void CMD5::finish(void*target)
{
    static BYTE padding[64] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    UINT32*digest = (UINT32*)target;

    UINT32 last, padn,
           msglen[2];
    msglen[0] = ((_total <<  3) & 0xFFFFFFFF); //swap here
    msglen[1] = ((_total >> 29) & 0xFFFFFFFF); //swap here

    last = (_total & 0x3F);
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    update(padding, padn );
    update(msglen, sizeof(msglen));

    digest[0] = _a; //swap here
    digest[1] = _b; //swap here
    digest[2] = _c; //swap here
    digest[3] = _d; //swap here
}




/*
 * General purpose 48-byte transformation, using two 32-byte salts (generally,
 * a client and server salt) and a global salt value used for padding.
 * Both SHA1 and MD5 algorithms are used.
 */
void Encryption::hash48(const void*in, const void*salt, const void*pepper, BYTE base, void*out)
{
	BYTE shasig[20];
	BYTE pad[4],*pntr = (BYTE*)out;
	for (BYTE k = 0; k < 3; k++)
	{
		memset(pad, base + k, k + 1);
        CSHA1 sha;
        sha.update(pad,k+1);
        sha.update(in,48);
        sha.update(salt,32);
        sha.update(pepper,32);
        sha.finish(shasig);

        CMD5 md5;
        md5.update(in,48);
        md5.update(shasig,20);
        md5.finish(&pntr[k*16]);
	}
}

/*
 * Weaker 16-byte transformation, also using two 32-byte salts, but
 * only using a single round of MD5.
 */
void Encryption::hash16(const void*in, const void*salt, const void*pepper, void*out)
{
    CMD5   md5;
    md5.update(in,16);
    md5.update(salt,32);
    md5.update(pepper,32);
    md5.finish(out);
}





CSHA1::CSHA1():h0(0x67452301),h1(0xEFCDAB89),h2(0x98BADCFE),h3(0x10325476),h4(0xC3D2E1F0),sz0(0),sz1(0)
{}

void CSHA1::transform(UINT32*data)
{
    UINT32 temp, W[16], A, B, C, D, E;
    for (BYTE k = 0; k < 16; k++)
        W[k] = swap32(data[k]);

    #define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

    #define R(t)                                            \
    (                                                       \
        temp = W[(t -  3) & 0x0F] ^ W[(t - 8) & 0x0F] ^     \
               W[(t - 14) & 0x0F] ^ W[ t      & 0x0F],      \
        ( W[t & 0x0F] = S(temp,1) )                         \
    )

    #define P(a,b,c,d,e,x)                                  \
    {                                                       \
        e += S(a,5) + F(b,c,d) + K + x; b = S(b,30);        \
    }

    A = h0;
    B = h1;
    C = h2;
    D = h3;
    E = h4;

    #define F(x,y,z) (z ^ (x & (y ^ z)))
    #define K 0x5A827999

        P( A, B, C, D, E, W[0]  );
        P( E, A, B, C, D, W[1]  );
        P( D, E, A, B, C, W[2]  );
        P( C, D, E, A, B, W[3]  );
        P( B, C, D, E, A, W[4]  );
        P( A, B, C, D, E, W[5]  );
        P( E, A, B, C, D, W[6]  );
        P( D, E, A, B, C, W[7]  );
        P( C, D, E, A, B, W[8]  );
        P( B, C, D, E, A, W[9]  );
        P( A, B, C, D, E, W[10] );
        P( E, A, B, C, D, W[11] );
        P( D, E, A, B, C, W[12] );
        P( C, D, E, A, B, W[13] );
        P( B, C, D, E, A, W[14] );
        P( A, B, C, D, E, W[15] );
        P( E, A, B, C, D, R(16) );
        P( D, E, A, B, C, R(17) );
        P( C, D, E, A, B, R(18) );
        P( B, C, D, E, A, R(19) );

    #undef K
    #undef F

    #define F(x,y,z) (x ^ y ^ z)
    #define K 0x6ED9EBA1

        P( A, B, C, D, E, R(20) );
        P( E, A, B, C, D, R(21) );
        P( D, E, A, B, C, R(22) );
        P( C, D, E, A, B, R(23) );
        P( B, C, D, E, A, R(24) );
        P( A, B, C, D, E, R(25) );
        P( E, A, B, C, D, R(26) );
        P( D, E, A, B, C, R(27) );
        P( C, D, E, A, B, R(28) );
        P( B, C, D, E, A, R(29) );
        P( A, B, C, D, E, R(30) );
        P( E, A, B, C, D, R(31) );
        P( D, E, A, B, C, R(32) );
        P( C, D, E, A, B, R(33) );
        P( B, C, D, E, A, R(34) );
        P( A, B, C, D, E, R(35) );
        P( E, A, B, C, D, R(36) );
        P( D, E, A, B, C, R(37) );
        P( C, D, E, A, B, R(38) );
        P( B, C, D, E, A, R(39) );

    #undef K
    #undef F

    #define F(x,y,z) ((x & y) | (z & (x | y)))
    #define K 0x8F1BBCDC

        P( A, B, C, D, E, R(40) );
        P( E, A, B, C, D, R(41) );
        P( D, E, A, B, C, R(42) );
        P( C, D, E, A, B, R(43) );
        P( B, C, D, E, A, R(44) );
        P( A, B, C, D, E, R(45) );
        P( E, A, B, C, D, R(46) );
        P( D, E, A, B, C, R(47) );
        P( C, D, E, A, B, R(48) );
        P( B, C, D, E, A, R(49) );
        P( A, B, C, D, E, R(50) );
        P( E, A, B, C, D, R(51) );
        P( D, E, A, B, C, R(52) );
        P( C, D, E, A, B, R(53) );
        P( B, C, D, E, A, R(54) );
        P( A, B, C, D, E, R(55) );
        P( E, A, B, C, D, R(56) );
        P( D, E, A, B, C, R(57) );
        P( C, D, E, A, B, R(58) );
        P( B, C, D, E, A, R(59) );

    #undef K
    #undef F

    #define F(x,y,z) (x ^ y ^ z)
    #define K 0xCA62C1D6

        P( A, B, C, D, E, R(60) );
        P( E, A, B, C, D, R(61) );
        P( D, E, A, B, C, R(62) );
        P( C, D, E, A, B, R(63) );
        P( B, C, D, E, A, R(64) );
        P( A, B, C, D, E, R(65) );
        P( E, A, B, C, D, R(66) );
        P( D, E, A, B, C, R(67) );
        P( C, D, E, A, B, R(68) );
        P( B, C, D, E, A, R(69) );
        P( A, B, C, D, E, R(70) );
        P( E, A, B, C, D, R(71) );
        P( D, E, A, B, C, R(72) );
        P( C, D, E, A, B, R(73) );
        P( B, C, D, E, A, R(74) );
        P( A, B, C, D, E, R(75) );
        P( E, A, B, C, D, R(76) );
        P( D, E, A, B, C, R(77) );
        P( C, D, E, A, B, R(78) );
        P( B, C, D, E, A, R(79) );

    #undef K
    #undef F
    #undef S
    #undef R
    #undef P

    h0 += A;
    h1 += B;
    h2 += C;
    h3 += D;
    h4 += E;
}


void CSHA1::update(const void*source_, size_t size)
{
    const BYTE*source = (const BYTE*)source_;
    size_t left, fill;

    if (!size)
        return;

    left = sz0 & 0x3F;
    fill = 64 - left;

    sz0 += (UINT32)size;
    sz0 &= 0xFFFFFFFF;

    if(sz0 < size)
        sz1++;

    if(left && size >= fill)
    {
        memcpy(&data[left],source, fill);
        transform((UINT32*)data);
        size -= fill;
        source  += fill;
        left = 0;
    }

    while(size >= 64)
    {
        transform((UINT32*)source);
        size   -= 64;
        source  += 64;
    }

    if(size)
        memcpy(&data[left],source,size);
}

void CSHA1::finish(void*target)
{
    UINT32  last, padn;
    UINT32  high, low;
    UINT32  msglen[2];

    high = ( sz0 >> 29 )
         | ( sz1 <<  3 );
    low  = ( sz0 <<  3 );

    static BYTE sha1_padding[64] =
    {
     0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    msglen[0] = swap32(high);
    msglen[1] = swap32(low);

    last = sz0 & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    update(sha1_padding, padn );
    update(msglen, 8 );

    UINT32*digest = (UINT32*)target;
    digest[0] = swap32(h0);
    digest[1] = swap32(h1);
    digest[2] = swap32(h2);
    digest[3] = swap32(h3);
    digest[4] = swap32(h4);
}



void Encryption::sign(const void*key, size_t keylen, const void*data, size_t datalen, void*signature, BYTE signature_len)
{
	BYTE    shasig[20];
	BYTE    md5sig[16];
	BYTE    lenhdr[4];
    static BYTE pad_54[40] = {
	    54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
    	54, 54, 54,
    	54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
    	54, 54, 54
    };
    static BYTE pad_92[48] = {
    	92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,
    	92, 92, 92, 92, 92, 92, 92,
    	92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,
    	92, 92, 92, 92, 92, 92, 92
    };
    CMD5   md5;
    CSHA1  sha;

    (*(UINT32*)lenhdr) = (UINT32)datalen;

	sha.update(key, keylen);
	sha.update(pad_54, 40);
	sha.update(lenhdr, 4);
	sha.update(data, datalen);
	sha.finish(shasig);

	md5.update(key, keylen);
	md5.update(pad_92, 48);
	md5.update(shasig, 20);
	md5.finish(md5sig);
    if (signature_len > 16)
        signature_len = 16;

	memcpy(signature, md5sig, signature_len);
}


CRC4::CRC4():_x(0),_y(0)
{}

CRC4::CRC4(const void*data, size_t size)
{
    set(data,size);
}

void CRC4::apply(const void*source, void*target, size_t size)
{
    UINT32*d,x,y,tx,ty;
    x=_x;
    y=_y;
    d=_data;
    const BYTE*indata = (const BYTE*)source;
    BYTE*outdata = (BYTE*)target;


    #define LOOP(in,out) \
		x=((x+1)&0xff); \
		tx=d[x]; \
		y=(tx+y)&0xff; \
		d[x]=ty=d[y]; \
		d[y]=tx; \
		(out) = d[(tx+ty)&0xff]^ (in);

    #ifndef RC4_INDEX
        #define RC4_LOOP(a,b,i)	LOOP(*((a)++),*((b)++))
    #else
        #define RC4_LOOP(a,b,i)	LOOP(a[i],b[i])
    #endif

	int i=((int)size>>3L);
	if (i)
		{
		for (;;)
			{
			RC4_LOOP(indata,outdata,0);
			RC4_LOOP(indata,outdata,1);
			RC4_LOOP(indata,outdata,2);
			RC4_LOOP(indata,outdata,3);
			RC4_LOOP(indata,outdata,4);
			RC4_LOOP(indata,outdata,5);
			RC4_LOOP(indata,outdata,6);
			RC4_LOOP(indata,outdata,7);
    #ifdef RC4_INDEX
			indata+=8;
			outdata+=8;
    #endif
			if (--i == 0) break;
			}
		}
	i=(int)size&0x07;
	if (i)
		{
		for (;;)
			{
			RC4_LOOP(indata,outdata,0); if (--i == 0) break;
			RC4_LOOP(indata,outdata,1); if (--i == 0) break;
			RC4_LOOP(indata,outdata,2); if (--i == 0) break;
			RC4_LOOP(indata,outdata,3); if (--i == 0) break;
			RC4_LOOP(indata,outdata,4); if (--i == 0) break;
			RC4_LOOP(indata,outdata,5); if (--i == 0) break;
			RC4_LOOP(indata,outdata,6); if (--i == 0) break;
			}
		}               
	_x=x;
	_y=y;
    #undef LOOP
    #undef RC4_LOOP
}

void CRC4::set(const void*data, size_t size)
{
    UINT32 tmp,*d=_data;
    const BYTE*pntr = (const BYTE*)data;
    unsigned id1(0), id2(0);

    for (USHORT k = 0; k < 0x100; k++)
        d[k] = k;

    _x = 0;
    _y = 0;

    #define SK_LOOP(n) { \
		tmp=d[(n)]; \
		id2 = (pntr[id1] + tmp + id2) & 0xff; \
		if (++id1 == size) id1=0; \
		d[(n)]=d[id2]; \
		d[id2]=tmp; }

	for (USHORT k = 0; k < 0x100; k+=4)
    {
		SK_LOOP(k+0);
		SK_LOOP(k+1);
		SK_LOOP(k+2);
		SK_LOOP(k+3);
    }
}


/*static*/ void		SHA256::hash(const void*data, size_t size, THash<8>&out)
{
	SHA256	obj(data,size);
	obj.finish(out);
}

/*static*/ void		SHA256::hash(const void*data, size_t size, void*out)
{
	SHA256	obj(data,size);
	obj.finish(out);
}


SHA256::SHA256(const void*data, size_t size)
{
	reset();
	append(data,size);
}

SHA256::SHA256()
{
	reset();
}

void SHA256::reset()
{


	h0 = 0x6a09e667;
	h1 = 0xbb67ae85;
	h2 = 0x3c6ef372;
	h3 = 0xa54ff53a;
	h4 = 0x510e527f;
	h5 = 0x9b05688c;
	h6 = 0x1f83d9ab;
	h7 = 0x5be0cd19;
	current = (BYTE*)w;
	total = 0;
	//memset(w,0,sizeof(w));
}


template <typename T, typename U>
	static inline T rotateL(T x, U y)
	{
		return (x << y) | (x >> ((sizeof(T) * 8) - y));
	}


	static inline UINT32 rotateR(UINT32 x, UINT32 y)
	{
		//return (x << y) | (x >> (32 - y));
		return (x >> y) | (x << (32 - y));
	}
	
static inline UINT32 rotateRight(UINT32 n,UINT32 x)
{
	return ((x >> n) | (x << (32 - n)));
}

static inline UINT32 choice(UINT32 x,UINT32 y,UINT32 z)
{
	return ((x & y) ^ (~x & z));
}
static inline UINT32 majority(UINT32 x,UINT32 y,UINT32 z)
{
	return ((x & y) ^ (x & z) ^ (y & z));
}

static inline UINT32 sha256_Sigma0(UINT32 x)
{
	return (rotateRight(2, x) ^ rotateRight(13, x) ^ rotateRight(22, x));
}
static inline UINT32 sha256_Sigma1(UINT32 x)
{
	return (rotateRight(6, x) ^ rotateRight(11, x) ^ rotateRight(25, x));
}

static inline UINT32 sha256_sigma0(UINT32 x)
{
	return (rotateRight(7, x) ^ rotateRight(18, x) ^ (x >> 3));
}

static inline UINT32 sha256_sigma1(UINT32 x)
{
	return (rotateRight(17, x) ^ rotateRight(19, x) ^ (x >> 10));
}

static inline UINT32 sha256_expand(UINT32*w, UINT32 j)
{
	return (w[j&0x0f] += sha256_sigma1(w[(j+14)&0x0f]) + w[(j+9)&0x0f] + sha256_sigma0(w[(j+1)&0x0f]));
}

void SHA256::processChunk()
{
	UINT32 w2[16];
	BYTE*buffer = (BYTE*)w;
	for(unsigned i=0; i<16; i++)
		w2[i] = ((buffer[(i<<2)+3]) | (buffer[(i<<2)+2] << 8) | (buffer[(i<<2)+1] << 16) | (buffer[i<<2] << 24));
	
	UINT32	a = h0,
			b = h1,
			c = h2,
			d = h3,
			e = h4,
			f = h5,
			g = h6,
			h = h7;
	
	for(unsigned j=0; j<64; j++)
	{
		UINT32 t1 = h + sha256_Sigma1(e) + choice(e, f, g) + k[j];
		if (j < 16)
			t1 += w2[j];
		else
			t1 += sha256_expand(w2, j);
		UINT32 t2 = sha256_Sigma0(a) + majority(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
    }

    h0 +=  a;
    h1 +=  b;
    h2 +=  c;
    h3 +=  d;
    h4 +=  e;
    h5 +=  f;
    h6 +=  g;
    h7 +=  h;
	
	current = (BYTE*)w;
}

void SHA256::append(const void*data, size_t size)
{
	UINT32 i, index, curpos = 0;
	/* Compute number of bytes mod 64 */
	index = ((total >> 3) & 63);
	UINT32 remainder = (size & 63);

	/* Update number of bits */
	total += size*8;
	BYTE*buffer = (BYTE*)w;
	/* Transform as many times as possible */
	const BYTE*read = (const BYTE*)data;
	for(i=0; i+63<size; i+=64)
	{
		for(unsigned j=index; j<64; j++)
			buffer[j] = read[curpos++];
		processChunk();
		index = 0;
	}

	/* Buffer remaining input */
	for(unsigned j=0; j<remainder; j++)
		buffer[j] = read[curpos++];


/*


	const BYTE	*base = (const BYTE*)data,
				*at = base,
				*end = base+size;
	BYTE*chunk_end = ((BYTE*)w)+64;
				
	while (at < end)
	{
		size_t copy = chunk_end-current;
		if (copy > end-at)
			copy = end-at;
		
		memcpy(current,data,copy);
		at += copy;
		current += copy;
		if (current == chunk_end)
			processChunk();
	}
	
	total += size*8;*/
}

void SHA256::finish(HashContainer&target)
{
	finish(target.bytes);
}

static void swapBytes(UINT32 in, UINT32&out)
{
	out =	(in>>24)
			|
			(((in >> 16)&0xFF)<<8)
			|
			(((in >> 8)&0xFF)<<16)
			|
			(((in)&0xFF)<<24);
}

void SHA256::finish(void*target)
{
	UINT32 index = ((total >> 3) & 0x3f);
	BYTE*buffer = (BYTE*)w;
	buffer[index++] = 0x80;
    if(index <= 56)
	{
		for(unsigned i=index; i<56; i++)
			buffer[i] = 0;
	}
	else
	{
		for(unsigned i=index; i<64; i++)
			buffer[i] = 0;
        processChunk();
		for(unsigned i=0; i<56; i++)
			buffer[i] = 0;
	}
	buffer[56] = (total >> 56) & 0xff;
	buffer[57] = (total >> 48) & 0xff;
	buffer[58] = (total >> 40) & 0xff;
	buffer[59] = (total >> 32) & 0xff;
	buffer[60] = (total >> 24) & 0xff;
	buffer[61] = (total >> 16) & 0xff;
	buffer[62] = (total >> 8) & 0xff;
	buffer[63] = total & 0xff;
    processChunk();
		
	
	/*


	UINT64 ctotal = total;
	BYTE one = 1;
	append(&one,1);
	unsigned mod = (total%512);
	unsigned pad = mod<=448?448-mod:512+448-mod;
	ASSERT__(!(pad%8));
	pad/=8;
	BYTE zero = 0;
	for (unsigned i = 0; i < pad; i++)
		append(&zero,1);
	
	ASSERT1__(!((total+64)%512),total);
	
	append(&ctotal,sizeof(ctotal));
	ASSERT1__(!(total%512),total);
	ASSERT_EQUAL__(current,(BYTE*)w);
	*/
	UINT32*out = (UINT32*)target;
	swapBytes(h0,out[0]);
	swapBytes(h1,out[1]);
	swapBytes(h2,out[2]);
	swapBytes(h3,out[3]);
	swapBytes(h4,out[4]);
	swapBytes(h5,out[5]);
	swapBytes(h6,out[6]);
	swapBytes(h7,out[7]);
}
	

UINT32	SHA256::k[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
   };
 