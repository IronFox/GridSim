#ifndef encryptionH
#define encryptionH

/******************************************************************

sha1, md4 and rc4 hashsum-calculators/encryptors.

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
		static const count_t num_bytes = int32_count*4;
		static const count_t NumBytes = int32_count*4;
		static const count_t NumInts = int32_count;
		union
		{
			BYTE	bytes[num_bytes];
			UINT32	ints[int32_count];
		};
		
		bool	operator==(const THash<int32_count>&other)	const
				{
					return !memcmp(bytes,other.bytes,sizeof(bytes));
				}
		bool	operator!=(const THash<int32_count>&other)	const
				{
					return memcmp(bytes,other.bytes,sizeof(bytes)) != 0;
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

class SHA1
{
private:
	    UINT32      h0,h1,h2,h3,h4,
	                sz0,sz1;
	    BYTE        data[64];

        void        transform(UINT32*);

public:
	typedef THash<5>HashContainer;

                    SHA1();
    void			Append(const void*source, size_t size);
    void			Finish(void*target);
    void			Finish(HashContainer&target);
	template <typename T>
		inline void	AppendPOD(const T&object) {Append(&object,sizeof(T));}

	static void		Hash(const void*data, size_t size, HashContainer&out);
	static void		Hash(const void*data, size_t size, void*out);
};


class SHA256
{
protected:
	UINT32			h0,h1,h2,h3,h4,h5,h6,h7;
					
	static const size_t chunk_size = 64;
		
	UINT32			w[64];
	BYTE			*current;
	UINT64			total;
	static UINT32	k[64];
	void 			processChunk();
public:
	typedef THash<8>	HashContainer;
	static const size_t out_bytes = HashContainer::num_bytes;
	static const size_t HashLength = HashContainer::num_bytes;

	static void		hash(const void*data, size_t size, HashContainer&out);
	static void		Hash(const void*data, size_t size, HashContainer&out)	{hash(data,size,out);}
	static void		hash(const void*data, size_t size, void*out);
	static void		Hash(const void*data, size_t size, void*out)	{hash(data,size,out);}

	/**/			SHA256();
	/**/			SHA256(const void*source, size_t size);
	void			reset();
	inline void		Reset()	{reset();}
    void			append(const void*source, size_t size);
	inline void		Append(const void*source, size_t size)	{append(source,size);}
	template <typename T>
		inline void	AppendPOD(const T&object) {append(&object,sizeof(T));}
	inline void		Append(const ArrayData<BYTE>&data) {append(data.pointer(),data.GetLength());}
    void			finish(void*target);
	inline void		Finish(void*target)	{finish(target);}
    void			finish(HashContainer&target);
	inline void		Finish(HashContainer&target)	{finish(target);}
};



using namespace Encryption;


#endif
