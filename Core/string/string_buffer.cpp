#include "../global_root.h"
#include "string_buffer.h"


/******************************************************************

E:\include\string\string_buffer.cpp

******************************************************************/

namespace DeltaWorks
{

	StringBuffer::StringBuffer(size_t l):Super(l)
	{}

	StringBuffer::StringBuffer(const char*string, size_t extra):Super(0)
	{
		size_t strLen = strlen(string);
		memcpy(Super::AppendRow(strLen),string,strLen);
	}

	StringBuffer::StringBuffer(const String&string, size_t extra):Super(string.GetLength()+extra)
	{
		memcpy(Super::AppendRow(string.GetLength()),string.c_str(),string.GetLength());
	}


	StringBuffer&	StringBuffer::operator=(const StringBuffer&other)
	{
		Super::operator=(other);
		return *this;
	}

	StringBuffer&	StringBuffer::operator=(const char*string)
	{
		Super::clear();
		return operator<<(string);
	}


	StringBuffer&	StringBuffer::operator=(const String&other)
	{
		Super::clear();
		return operator<<(other);
	}



	template <typename T, typename UT>
		static void _WriteSigned(StringBuffer*buffer, T value)
		{
			bool negative = false;
			if (value < 0)
			{
				value*=-1;
				negative = true;
			}

			char	char_buffer[256],
					*end = char_buffer+ARRAYSIZE(char_buffer),
					*c = end;
			UT uval = value;
			while (uval && c != char_buffer)
			{
				(*(--c)) = (char)('0'+(uval%10));
				uval/=10;
			}
			if (c==end)
				(*(--c)) = (char)'0';
			if (negative)
				buffer->Write("-",1);
			buffer->Write(c,end-c);
		}

	template <typename T>	
		static void _WriteUnsigned(StringBuffer*buffer, T value)
		{
			char	char_buffer[256],
					*end = char_buffer+ARRAYSIZE(char_buffer),
					*c = end;
			while (value && c != char_buffer)
			{
				(*(--c)) = (char)('0'+(value%10));
				value/=10;
			}
			if (c==end)
				(*(--c)) = (char)'0';
			buffer->Write(c,end-c);
		}

	template <typename T>
		static void _WriteFloat(StringBuffer*buffer, T value, unsigned precision)
		{
			char	char_buffer[256],
					*str = String::FloatToStr(value, precision, false, char_buffer+ARRAYSIZE(char_buffer), char_buffer);

			buffer->Write(str,char_buffer+ARRAYSIZE(char_buffer)-str);
		}


	StringBuffer&  StringBuffer::operator<<(char value)
	{
		Super::append(value);
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(BYTE value)
	{
		_WriteUnsigned(this,value);
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(long long value)
	{
		_WriteSigned<long long, unsigned long long>(this,value);
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(unsigned long long value)
	{
		_WriteUnsigned(this,value);
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(int value)
	{
		_WriteSigned<int,unsigned>(this,value);
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(unsigned value)
	{
		_WriteUnsigned(this,value);
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(long value)
	{
		_WriteSigned<long, unsigned long>(this,value);
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(unsigned long value)
	{
		_WriteUnsigned(this,value);
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(short value)
	{
		_WriteSigned<short,unsigned short>(this,value);
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(unsigned short value)
	{
		_WriteUnsigned(this,value);
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(float value)
	{
		_WriteFloat(this,value,5);
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(double value)
	{
		_WriteFloat(this,value,8);
		return *this;
	}


	StringBuffer&		StringBuffer::operator<<(const Ctr::ArrayData<char>&data)
	{
		if (data.IsEmpty())
			return *this;
		if (!data.Last())	//terminating zero
			Write(data.pointer(),data.GetLength()-1);
		else
			Super::AppendAll(data);
		return *this;
	}

	StringBuffer&		StringBuffer::operator<<(const StringRef&ref)
	{
		Write(ref.pointer(),ref.GetLength());
		return *this;
	}




	StringBuffer&  StringBuffer::operator<<(const StringBuffer&buffer)
	{
		Super::AppendAll(buffer);
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(const String&str)
	{
		Write(str.c_str(),str.GetLength());
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(const char*str)
	{
		Write(str,strlen(str));
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(const TNewLine&)
	{
		Super::append('\n');
		return *this;
	}


	StringBuffer&  StringBuffer::operator<<(const TSpace&space)
	{
		char*out = appendRow(space.length);
		for (size_t i = 0; i < space.length; i++)
			(*out++) = ' ';
		return *this;
	}

	StringBuffer&  StringBuffer::operator<<(const TTabSpace&space)
	{
		char*out = appendRow(space.length);
		for (size_t i = 0; i < space.length; i++)
			(*out++) = '\t';
		return *this;
	}


	String	 StringBuffer::CopyToString()	const
	{
		return String(Super::pointer(),Super::Count());
	}

	StringRef	StringBuffer::ToStringRef()	const
	{
		return StringRef(Super::pointer(),Super::Count());
	}


	void			StringBuffer::Insert(index_t index, const char*str, size_t len)
	{
		memcpy(Super::insertRow(index,len),str,len);
	}

	void			StringBuffer::Insert(size_t index, const String&string)
	{
		Insert(index,string.c_str(),string.GetLength());
	}
}
