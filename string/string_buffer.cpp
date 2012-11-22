#include "../global_root.h"
#include "string_buffer.h"


/******************************************************************

E:\include\string\string_buffer.cpp

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

StringBuffer::StringBuffer(size_t l):field(alloc<char>(l)),len(l),remaining(l),overflow(false),auto_resize(true)
{
	current = field;
}

StringBuffer::StringBuffer(const char*string, size_t extra):overflow(false),auto_resize(true)
{
	len = strlen(string);
	field = alloc<char>(len+extra);
	memcpy(field,string,len);
	current = field+len;
	len += extra;
	remaining = extra;
}

StringBuffer::StringBuffer(const String&string, size_t extra):overflow(false),auto_resize(true)
{
	len = string.length();
	field = alloc<char>(len+extra);
	memcpy(field,string.c_str(),len);
	current = field+len;
	len += extra;
	remaining = extra;
}



StringBuffer::~StringBuffer()
{
	dealloc(field);
}




StringBuffer::StringBuffer(const StringBuffer&other):field(alloc<char>(other.len)),len(other.len),remaining(other.remaining),overflow(false),auto_resize(other.auto_resize)
{
	current = field+(other.current-other.field);
	memcpy(field,other.field,current-field);
}


StringBuffer&	StringBuffer::operator=(const StringBuffer&other)
{
	reloc(field, len, other.len);
	remaining = other.remaining;
	overflow = false;
	auto_resize = other.auto_resize;
	memcpy(field,other.field,current-field);
	current = field+(other.current-other.field);
	return *this;
}

StringBuffer&	StringBuffer::operator=(const char*string)
{
	reset();
	operator<<(string);
	return *this;
}


StringBuffer&	StringBuffer::operator=(const String&other)
{
	reset();
	operator<<(other);
	return *this;
}

void			StringBuffer::adoptData(StringBuffer&other)
{
	dealloc(field);
	field = other.field;
	current = other.current;
	remaining = other.remaining;
	auto_resize = other.auto_resize;
	len = other.len;
	
	other.field = NULL;
	other.current = NULL;
	other.len = 0;
	other.remaining = 0;

}


template <typename T, typename UT>
	static void writeSigned(StringBuffer*buffer, T value)
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
			buffer->write("-",1);
		buffer->write(c,end-c);
	}

template <typename T>	
	static void writeUnsigned(StringBuffer*buffer, T value)
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
		buffer->write(c,end-c);
	}

template <typename T>
	static void writeFloat(StringBuffer*buffer, T value, unsigned precision)
	{
		char	char_buffer[256],
				*str = String::floatToStr(value, precision, false, char_buffer+ARRAYSIZE(char_buffer), char_buffer);

		buffer->write(str,char_buffer+ARRAYSIZE(char_buffer)-str);
	}


StringBuffer&  StringBuffer::operator<<(char value)
{
	write(&value,1);
	return *this;
}

StringBuffer&  StringBuffer::operator<<(BYTE value)
{
	writeUnsigned(this,value);
	return *this;
}

StringBuffer&  StringBuffer::operator<<(long long value)
{
	writeSigned<long long, unsigned long long>(this,value);
	return *this;
}

StringBuffer&  StringBuffer::operator<<(unsigned long long value)
{
	writeUnsigned(this,value);
	return *this;
}

StringBuffer&  StringBuffer::operator<<(int value)
{
	writeSigned<int,unsigned>(this,value);
	return *this;
}

StringBuffer&  StringBuffer::operator<<(unsigned value)
{
	writeUnsigned(this,value);
	return *this;
}

StringBuffer&  StringBuffer::operator<<(long value)
{
	writeSigned<long, unsigned long>(this,value);
	return *this;
}

StringBuffer&  StringBuffer::operator<<(unsigned long value)
{
	writeUnsigned(this,value);
	return *this;
}

StringBuffer&  StringBuffer::operator<<(short value)
{
	writeSigned<short,unsigned short>(this,value);
	return *this;
}

StringBuffer&  StringBuffer::operator<<(unsigned short value)
{
	writeUnsigned(this,value);
	return *this;
}

StringBuffer&  StringBuffer::operator<<(float value)
{
	writeFloat(this,value,5);
	return *this;
}

StringBuffer&  StringBuffer::operator<<(double value)
{
	writeFloat(this,value,8);
	return *this;
}




StringBuffer&  StringBuffer::operator<<(const StringBuffer&buffer)
{
	write(buffer.field,buffer.current-buffer.field);
	return *this;
}

StringBuffer&  StringBuffer::operator<<(const String&str)
{
	write(str.c_str(),str.length());
	return *this;
}

StringBuffer&  StringBuffer::operator<<(const char*str)
{
	write(str,strlen(str));
	return *this;
}

StringBuffer&  StringBuffer::operator<<(const TNewLine&)
{
	static const char c='\n';
	write(&c,1);
	return *this;
}


StringBuffer&  StringBuffer::operator<<(const TSpace&space)
{
	if (space.length <= 0 || !expand(space.length,true))
		return *this;
	for (size_t i = 0; i < space.length; i++)
		(*current++) = ' ';
	return *this;
}

StringBuffer&  StringBuffer::operator<<(const TTabSpace&space)
{
	if (space.length <= 0 || !expand(space.length,true))
		return *this;
	for (size_t i = 0; i < space.length; i++)
		(*current++) = '\t';
	return *this;
}


String	 StringBuffer::toString()	const
{
	return String(field,current-field);
}

ReferenceExpression<char>	StringBuffer::toStringRef()	const
{
	return ReferenceExpression<char>(field,current-field);
}

void			StringBuffer::resize(size_t l)
{
	len = l;
	re_alloc(field,len);
	current = field;
	remaining = len;
	overflow = false;
}

bool			StringBuffer::expand(size_t l, bool decrease_remaining)
{
	//cout << "Expanding area by "<<l<<" byte(s)"<<endl;
	if (l < remaining)
	{
		//cout << "Still enough available"<<endl;
		if (decrease_remaining)
		{
			remaining -= l;
			//cout << "Decremented. Now remaining are "<<remaining<<" byte(s)"<<endl;
		}
		return true;
	}
	if (!auto_resize)
	{
		//cout << "Auto-resize is disabled. Aborting"<<endl;
		overflow = true;
		return false;
	}
		
	   
	size_t new_len = len<<1;
	if (!new_len)
		new_len = 256;
	size_t target_load = (current-field)+l;
	while (new_len < target_load)
	{
		size_t next_new_len = new_len << 1;
		if (next_new_len < new_len)  //integer-overflow
		{
			//cout << "Integer overflow detected. Aborting"<<endl;
			overflow = true;
			return false;
		}
		new_len = next_new_len;
	}
	//cout << "new_len="<<new_len<<endl;
	
		
	char*new_buffer = alloc<char>(new_len);
	if (field)
	{
		//cout << "copying "<<(current-field)<<" byte(s) to new field"<<endl;
		memcpy(new_buffer,field,current-field);
		current = new_buffer+(current-field);
		dealloc(field);
		field = new_buffer;
	}
	else
	{
		field = new_buffer;
		current = field;
	}
	len = new_len;
	remaining = len-(current-field);
	//cout << "remaining bytes are "<<remaining<<endl;
	if (decrease_remaining)
	{
		remaining -= l;
		//cout << "decreased to "<<remaining<<endl;
	}
		
	return true;
}

bool			StringBuffer::write(const char*str,size_t l)
{
	if (!expand(l,true))
		return false;

	memcpy(current,str,l);
	current+=l;
	return true;
}

bool			StringBuffer::insert(size_t index, char c)
{
	if (!expand(1,true))
		return false;
	if (index > (size_t)(current-field))
		index = (size_t)(current-field);
	char*ch = current;
	while (ch > field+index)
	{
		*ch = *(ch-1);
		ch--;
	}
	field[index] = c;
	current ++;
	return true;
}

bool			StringBuffer::insert(size_t index, const char*str, size_t len)
{
	if (!len)
		return true;
	if (!expand(len,true))
		return false;
	if (index > size_t(current-field))
		index = size_t(current-field);
	char*ch = current-1+len;
	while (ch > field+index-1+len)
	{
		*ch = *(ch-len);
		ch--;
	}
	memcpy(field+index,str,len);
	
	current += len;
	return true;
}

bool			StringBuffer::insert(size_t index, const String&string)
{
	return insert(index,string.c_str(),string.length());
}

bool			StringBuffer::erase(size_t index, size_t len)
{
	size_t length = current-field;
	if (index >= length)
		return false;
	if (index+len > length)
		len = length-index;
	remaining += len;
	
	for (size_t i = index; i < length-len; i++)
		field[i] = field[i+len];
	
	current -= len;
	return true;

}




void			StringBuffer::reset()
{
	current = field;
	remaining = len;
	overflow = false;
}

const char*	 StringBuffer::root()	  const
{
	return field;
}

char*			StringBuffer::root()
{
	return field;
}

size_t		StringBuffer::length()	const
{
	return current-field;
}

char			StringBuffer::terminator()	const
{
	if (current==field)
		return 0;
	return *(current-1);
}
