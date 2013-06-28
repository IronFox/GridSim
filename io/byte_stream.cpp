#include "../global_root.h"
#include "byte_stream.h"



void ByteStream::pushData(const void*pntr, size_t size)
{
	if (current+size > end)
	{
		size_t		index = current-begin,
					len = (end-begin)*2;
		
		if (!len)
			len = 4;
		while (index+size > len)
			len <<= 1;

		char	*new_field = alloc<char>(len);
		memcpy(new_field,begin,end-begin);
		dealloc(begin);
		begin = new_field;
		end = begin+len;
		current = begin+index;		
	}
	memcpy(current,pntr,size);
	current += size;
}


void ByteStream::pushZero(size_t size)
{
	if (current+size > end)
	{
		size_t		index = current-begin,
					len = (end-begin)*2;
		
		if (!len)
			len = 4;
		while (index+size > len)
			len <<= 1;

		char	*new_field = alloc<char>(len);
		memcpy(new_field,begin,end-begin);
		dealloc(begin);
		begin = new_field;
		end = begin+len;
		current = begin+index;		
	}
	memset(current,0,size);
	current += size;
}



bool ByteStream::getData(void*pntr, size_t size)
{
    if (current+size > end)
        return false;
    memcpy(pntr,current,size);
    current+=size;
    return true;
}

ByteStream::ByteStream(size_t len):local(true)
{
	current = begin = alloc<char>(len);
	end = begin+len;
}

ByteStream::~ByteStream()
{
    if (local)
        dealloc(begin);
}

void ByteStream::reset()
{
    current = begin;
}

size_t	ByteStream::fillLevel()	const
{
	return current-begin;
}

void ByteStream::resize(size_t len)
{
	if ((size_t)(end-begin) ==len)
	{
		current = begin;
		return;
	}
	re_alloc(begin,len);
	end = begin+len;
	current = begin;
}

void ByteStream::Assign(void*pntr, size_t size)
{
    if (local)
        dealloc(begin);
    if (size)
        begin = (char*)pntr;
    else
        begin = NULL;
	current = begin;
	end = begin + size;
    local = false;
}

char*	ByteStream::pointer()
{
	return begin;
}

const char*	ByteStream::pointer() const
{
	return begin;
}


const void* ByteStream::data() const
{
    return begin;
}

size_t    ByteStream::size()	const
{
    return end-begin;
}
