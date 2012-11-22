#include "../global_root.h"
#include "random_access_file.h"

/******************************************************************

File-access tool providing more options for handling volumes.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


/*
O_RDONLY	Open for reading only.
O_WRONLY	Open for writing only.
O_RDWR	Open for reading and writing.

Other Access Flags

O_NDELAY	Not used; for UNIX compatibility.
O_APPEND	If set, the file pointer will be set to the end of the file prior to each write.
O_CREAT	If the file exists, this flag has no effect. If the file does not exist, the file is created, and the bits of mode are used to set the file attribute bits as in chmod.
O_TRUNC	If the file exists, its length is truncated to 0. The file attributes remain unchanged.
O_EXCL	Used only with O_CREAT. If the file already exists, an error is returned.

O_BINARY	Can be given to explicitly open the file in binary mode.
*/


void RandomAccessFile::applyBufferSize(unsigned new_size)
{
    unsigned new_buffer_step = new_size/0x100+new_size%0x100?1:0;
    if (buffer_size != new_buffer_step)
    {
        BYTE*new_buffer = alloc<BYTE>(new_buffer_step+0x100);
        memcpy(new_buffer,buffer,new_size<fsize?new_size:fsize);
        dealloc(buffer);
        buffer = new_buffer;
        buffer_size = new_buffer_step;
    }
    fsize = new_size;
}


bool RandomAccessFile::openStream(bool truncate)
{
    if (truncate)
    {
        f = fopen(filename,"w+b");
        writable = write_access = f != NULL;
        if (!f)
            f = fopen(filename,"rb");
    }
    else
    {
        f = fopen(filename, "rb");
        writable = true;
        write_access = false;
    }
        
    return f!=NULL;
}

bool RandomAccessFile::beginWrite()
{
    if (write_access)
        return true;
    if (!writable || !f || !active)
        return false;
//    long at = ftell(f);
    fclose(f);
    f = fopen(filename,"r+b");
    writable = write_access = f != NULL;
    if (!f)
        f = fopen(filename,"rb");
    active = f != NULL;
    return write_access;
}


RandomAccessFile::RandomAccessFile():f(NULL),buffer(NULL),write_access(false),writable(false),referenced(false),fsize(0),buffer_size(0),open_mode(Direct)
{
    filename[0] = 0;
}

RandomAccessFile::~RandomAccessFile()
{
    close();
    if (!referenced)
        dealloc(buffer);
}

bool RandomAccessFile::writeAccess()
{
    return write_access||writable;
}

bool RandomAccessFile::isActive()
{
    return active;
}

bool RandomAccessFile::resize(unsigned size)
{
    if (!beginWrite())
        return false;
    changed = true;
    if (size == fsize)
        return true;
    switch (open_mode)
    {
        case Buffered:
            applyBufferSize(size);
        return true;
        case Direct:
            if (size > fsize)
            {
                fseek(f,0,SEEK_END);
                for (unsigned i = 0; i < size-fsize; i++)
                {
                    BYTE dummy;
                    ASSERT__(fwrite(&dummy,1,1,f)==1);
                }
            }
            else
            {
                BYTE*buffer = extract(0,size);
                fclose(f);
                if (!openStream(true))
                {
                    dealloc(buffer);
                    active = false;
                    return false;
                }
                append(buffer,size);
                dealloc(buffer);
            }
            fsize = size;
        return true;
    }
    return false;
}

bool RandomAccessFile::create(const char*Filename, OpenMode mode)
{
    close();
    if (!Filename ||!strlen(Filename) || strlen(Filename)>0x99)
        return false;
    strcpy(filename,Filename);
    return recreate(mode);
}

bool RandomAccessFile::recreate(OpenMode mode)
{
    FILE*temp = fopen(filename,"wb");      //sort of cheating here but never mind
    if (!temp)
        return false;
    fclose(temp);
    changed = true;
    return reopen(mode);
}

void RandomAccessFile::assign(BYTE*data, unsigned size)
{
    close();
    if (!referenced)
        dealloc(buffer);
    buffer = data;
    buffer_size = size;
    fsize = size;
    open_mode = Buffered;
    write_access = false;
    writable = false;
    changed = false;
    referenced = true;
    active = true;
}

void RandomAccessFile::assign(const RandomAccessFile&other)
{
    close();
    if (!referenced)
        dealloc(buffer);
    (*this) = other;
    referenced = true;
    if (open_mode == Buffered)
        writable = write_access = false;
}

bool RandomAccessFile::open(const char*Filename, OpenMode mode)
{
    close();
    if (!Filename ||!strlen(Filename) || strlen(Filename)>0x99)
    {
        return false;
    }
    strcpy(filename,Filename);
    return reopen(mode);
}

bool RandomAccessFile::reopen(OpenMode mode)
{
    close();
    changed = false;
    if (referenced)
    {
        buffer = NULL;
        buffer_size = 0;
        referenced = false;
    }
    open_mode = mode;
    unsigned nsize;
    active = false;
    switch (open_mode)
    {
        case Buffered:
            if (!openStream())
                return false;
            fseek(f,0,SEEK_END);
            nsize = ftell(f);
            fseek(f,0,SEEK_SET);
            applyBufferSize(nsize);
            if (fread(buffer,1,fsize,f)!=fsize)
			{
				dealloc(buffer);
				buffer = NULL;
				fclose(f);
				f = NULL;
				active = false;
				return false;
			}
            fclose(f);
            f = NULL;
        break;
        case Direct:
            dealloc(buffer);
            buffer = NULL;
            if (!openStream())
                return false;
            fseek(f,0,SEEK_END);
            fsize = ftell(f);
            fseek(f,0,SEEK_SET);
        break;
        default:
            return false;
    }
    active = true;
    return true;
}

void RandomAccessFile::close()
{
    if (!active||referenced)
        return;
    update();
    if (f)
        fclose(f);
    f = NULL;
    active = false;
}

bool RandomAccessFile::update()
{
    if (!changed)
        return write_access;
    changed = false;
	if (open_mode == Buffered)
    {
		if (openStream(true))
		{
			if (write_access)
				if (fwrite(buffer,1,fsize,f)!=fsize)
				{
					fclose(f);
					return false;
				}
			fclose(f);
			return write_access;
		}
		else
			return false;
    }
    return false;
}

BYTE*RandomAccessFile::extract(unsigned offset, unsigned size)
{
    BYTE*out = alloc<BYTE>(size);
    switch (open_mode)
    {
        case Buffered:
            memcpy(out,&buffer[offset],size);
        return out;
        case Direct:
            fseek(f,offset,SEEK_SET);
            if (fread(out,1,size,f)!=size)
			{
				dealloc(out);
				return NULL;
			}
        return out;
    }
    return NULL;
}

bool RandomAccessFile::extract(unsigned offset, void*out, unsigned size)
{
    switch (open_mode)
    {
        case Buffered:
            if (offset+size > fsize)
                return false;
            memcpy(out,&buffer[offset],size);
        return true;
        case Direct:
            if (!f)
                return false;
            fseek(f,offset,SEEK_SET);
        return fread(out,size,1,f) > 0;
    }
    return false;
}

bool RandomAccessFile::erase(unsigned offset, unsigned size)
{
    if (!write_access)
        return false;
    unsigned m_size = fsize-(offset+size);
    BYTE*temp =extract(offset+size,m_size);
    if (!resize(fsize-size))
    {
        dealloc(temp);
        return false;
    }
    if (temp)
    {
        overwrite(offset,temp,m_size);
        DISCARD_ARRAY(temp);
    }
    changed = true;
    return true;
}

bool RandomAccessFile::overwrite(unsigned offset, const void*data, unsigned size)
{
    if (!write_access)
        return false;
    changed = true;
    switch (open_mode)
    {
        case Buffered:
            if (offset+size > fsize)
                return false;
            memcpy(&buffer[offset],data,size);
        return true;
        case Direct:
            if (!f)
                return false;
            fseek(f,offset,SEEK_SET);
        return fwrite(data,size,1,f) > 0;
    }
    return false;
}

bool RandomAccessFile::overwrite(unsigned offset, BYTE value, unsigned size)
{
    if (!write_access)
        return false;
    changed = true;
    switch (open_mode)
    {
        case Buffered:
            if (offset+size > fsize)
                return false;
            memset(&buffer[offset],value,size);
        return true;
        case Direct:
            if (!f)
                return false;
            fseek(f,offset,SEEK_SET);
            for (unsigned i = 0; i < size; i++)
                if (fwrite(&value,1,1,f) != 1)
					return false;
        return true;
    }
    return false;
}

bool RandomAccessFile::insert(unsigned offset, const void*data, unsigned size)
{
    if (!write_access)
        return false;
    changed = true;
    if (offset >= fsize)
        return append(data,size);
    unsigned m_size = fsize-offset;
    BYTE*temp =extract(offset,m_size);
    if (!resize(fsize+size))
    {
        dealloc(temp);
        return false;
    }
    if (temp)
    {
        overwrite(offset+size,temp,m_size);
        DISCARD_ARRAY(temp);
    }
    overwrite(offset,data,size);
    return true;
}

bool RandomAccessFile::insert(unsigned offset, BYTE value, unsigned size)
{
    if (!write_access)
        return false;
    changed = true;
    if (offset >= fsize)
        return append(value,size);
    unsigned m_size = fsize-offset;
    BYTE*temp =extract(offset,m_size);
    if (!resize(fsize+size))
    {
        dealloc(temp);
        return false;
    }
    if (temp)
    {
        overwrite(offset+size,temp,m_size);
        DISCARD_ARRAY(temp);
    }
    overwrite(offset,value,size);
    return true;
}

bool RandomAccessFile::append(const void*data, unsigned size)
{
    unsigned at = fsize;
    return resize(fsize+size) && overwrite(at,data,size);
}

bool RandomAccessFile::append(BYTE value, unsigned size)
{
    unsigned at = fsize;
    return resize(fsize+size) && overwrite(at,value,size);
}

UINT RandomAccessFile::size()
{
    return fsize;
}
