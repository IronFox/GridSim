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

template<class T, bool isEnum=false>
struct GenericVTableTest
{
	class TestClass : public T
	{
		virtual void SomeVirtualMethod()
		{}
	};
	static const bool HasVTable = (sizeof(T) == sizeof(TestClass));
};

template<class T>
struct GenericVTableTest<T,true>
{
	static const bool HasVTable = false;
};



#define PRIMITIVE_NO_VTABLE(TYPE)\
	template<> struct GenericVTableTest<TYPE,false> {static const bool HasVTable = false;};

PRIMITIVE_NO_VTABLE(bool)
PRIMITIVE_NO_VTABLE(char)
PRIMITIVE_NO_VTABLE(unsigned char)
PRIMITIVE_NO_VTABLE(signed char)
PRIMITIVE_NO_VTABLE(short)
PRIMITIVE_NO_VTABLE(unsigned short)
PRIMITIVE_NO_VTABLE(int)
PRIMITIVE_NO_VTABLE(unsigned int)
PRIMITIVE_NO_VTABLE(long)
PRIMITIVE_NO_VTABLE(unsigned long)
PRIMITIVE_NO_VTABLE(long long)
PRIMITIVE_NO_VTABLE(unsigned long long)
PRIMITIVE_NO_VTABLE(float)
PRIMITIVE_NO_VTABLE(double)
PRIMITIVE_NO_VTABLE(long double)

template <bool HasVTable>
class VTableAssertion
{
public:
	static inline void	AssertNot()
	{
	}

};
template <>
class VTableAssertion<true>
{
//public:	//private, so compilation will fail outright
	static inline void	AssertNot()
	{
		FATAL__("VTable detected. Cannot interpret this type as primitive.")
	}

};


#define OBJECT_HAS_VTABLE(type) HasVTable<type>::Value

/*!
	@brief Generic data output stream
	
	Derivatives of IOutStream provide an interface to write blocks of data. Such a stream could (for instance) be a file, tcp stream, or pipe.
*/
interface IWriteStream
{
protected:
	virtual			~IWriteStream()	{};
public:
	virtual	bool	Write(const void*data, serial_size_t size)=0;	//!< Writes a chunk of binary data to the stream @param data Pointer to the memory section that should be written @param size Number of bytes that should be written @return true if the specified amount of bytes could be written to the stream, false otherwise
	virtual bool	Write(const volatile void*data, serial_size_t size)	{return Write(const_cast<const void*>(data),size);}

	template <typename T>
		bool		WritePrimitive(const T&element)
		{
			VTableAssertion<GenericVTableTest<T,std::is_enum<T>::value>::HasVTable>::AssertNot();
			return Write(&element, (serial_size_t)sizeof(element));
		}
	template <typename T>
		bool		WritePrimitives(const T*elements, count_t num_elements)
		{
			VTableAssertion<GenericVTableTest<T,std::is_enum<T>::value>::HasVTable>::AssertNot();
			return Write(elements, (serial_size_t)(sizeof(T)*num_elements));
		}

		bool		WriteSize(serial_size_t size)	//! Reads the content of a size variable in little endian 
		{
			BYTE b[4];
			if (size <= 0x3F)
			{
				b[0] = ((BYTE)size);
				return Write(b,1);
			}
			if (size <= 0x3FFF)
			{
				b[0] = (BYTE)((size >> 8)&0x3F) | 0x40;
				b[1] = (BYTE)(size&0xFF);
				return Write(b,2);
			}
			if (size <= 0x3FFFFF)
			{
				b[0] = (BYTE)((size >> 16)&0x3F) | 0x80;
				b[1] = (BYTE)((size>>8)&0xFF);
				b[2] = (BYTE)((size)&0xFF);
				return Write(b,3);
			}
			if (size >= (1U<<29))
				return false;
			b[0] = (BYTE)((size >> 24)&0x3F) | 0xC0;
			b[1] = (BYTE)((size >> 16)&0xFF);
			b[2] = (BYTE)((size >> 8)&0xFF);
			b[3] = (BYTE)((size)&0xFF);
			return Write(b,4);
		}

		bool	WriteSize(UINT64 size)
		{
			if (size > 0xFFFFFFFF)
				throw Except::Program::ParameterFault("Attempting to write serial size larger than can be expressed in 32 bit");
			return WriteSize((serial_size_t)size);
		}

};


/*!
	@brief Binary memory section write-stream
	
	MemWriteStream allows stream writing to a memory section. MemWriteStream does not delete any assigned data on destruction.
*/
class MemWriteStream: public IWriteStream
{
protected:
	BYTE			*field,	//!< Pointer to the current memory location that is next written to. Auto incremented as needed
					*end;	//!< Pointer one past the last available byte of the out-buffer. Data can be written up to but not past this pointer.

public:
	/**/			MemWriteStream(void*data=NULL,serial_size_t size = 0)
					{
						Assign(data,size);
					}
	void			Assign(void*data, serial_size_t size)	//!< Assigns a new field to the local memory stream
					{
						field = (BYTE*)data;
						end = field+size;
					}
		
	BYTE*			GetCurrent()	const
					{
						return field;
					}
				
	bool			Write(const void*data, serial_size_t size)	override
					{
						if (field+size > end)
							return false;
						memcpy(field,data,size);
						field+=size;
						return true;
					}
};

#endif
