#include "../global_root.h"
#include "random_access_file.h"

/******************************************************************

File-access tool providing more options for handling volumes.

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


#undef FOPEN
#undef STRLEN
#if SYSTEM==WINDOWS
	#define STRLEN(_STR_)	wcslen(_STR_)
	#define FOPEN(_FILE_,_MODE_)	_wfopen((_FILE_),L##_MODE_)
#else
	#define STRLEN(_STR_)	strlen(_STR_)
	#define FOPEN(_FILE_,_MODE_)	fopen((_FILE_),_MODE_)
#endif


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
        f = FOPEN(filename,"w+b");
        writable = write_access = f != NULL;
        if (!f)
            f = FOPEN(filename,"rb");
    }
    else
    {
        f = FOPEN(filename, "rb");
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
    f = FOPEN(filename,"r+b");
    writable = write_access = f != NULL;
    if (!f)
        f = FOPEN(filename,"rb");
    active = f != NULL;
    return write_access;
}


RandomAccessFile::RandomAccessFile():f(NULL),buffer(NULL),write_access(false),writable(false),referenced(false),fsize(0),buffer_size(0),open_mode(Direct)
{
    filename[0] = 0;
}

RandomAccessFile::~RandomAccessFile()
{
    Close();
    if (!referenced)
        dealloc(buffer);
}

bool RandomAccessFile::HasWriteAccess()
{
    return write_access||writable;
}

bool RandomAccessFile::IsActive()
{
    return active;
}

bool RandomAccessFile::Resize(unsigned size)
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
                BYTE*buffer = Extract(0,size);
                fclose(f);
                if (!openStream(true))
                {
                    dealloc(buffer);
                    active = false;
                    return false;
                }
                Append(buffer,size);
                dealloc(buffer);
            }
            fsize = size;
        return true;
    }
    return false;
}

bool RandomAccessFile::Create(const char_t*Filename, OpenMode mode)
{
    Close();
	if (!Filename)
		return false;
	size_t len = STRLEN(Filename);
    if (!len || len+1>MAX_PATH)
        return false;
	memcpy(filename,Filename,(len+1)*sizeof(char_t));
    //strcpy(filename,Filename);
    return ReCreate(mode);
}

bool RandomAccessFile::ReCreate(OpenMode mode)
{
    FILE*temp = FOPEN(filename,"wb");      //sort of cheating here but never mind
    if (!temp)
        return false;
    fclose(temp);
    changed = true;
    return ReOpen(mode);
}

void RandomAccessFile::Assign(BYTE*data, unsigned size)
{
    Close();
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

void RandomAccessFile::Assign(const RandomAccessFile&other)
{
    Close();
    if (!referenced)
        dealloc(buffer);
    (*this) = other;
    referenced = true;
    if (open_mode == Buffered)
        writable = write_access = false;
}

bool RandomAccessFile::Open(const char_t*Filename, OpenMode mode)
{
    Close();

	if (!Filename)
		return false;
	size_t len = STRLEN(Filename);
    if (!len || len+1>MAX_PATH)
        return false;
	memcpy(filename,Filename,(len+1)*sizeof(char_t));
    return ReOpen(mode);
}

bool RandomAccessFile::ReOpen(OpenMode mode)
{
    Close();
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

void RandomAccessFile::Close()
{
    if (!active||referenced)
        return;
    Update();
    if (f)
        fclose(f);
    f = NULL;
    active = false;
}

bool RandomAccessFile::Update()
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

BYTE*RandomAccessFile::Extract(unsigned offset, unsigned size)
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

bool RandomAccessFile::Extract(unsigned offset, void*out, unsigned size)
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

bool RandomAccessFile::Erase(unsigned offset, unsigned size)
{
    if (!write_access)
        return false;
    unsigned m_size = fsize-(offset+size);
    BYTE*temp =Extract(offset+size,m_size);
    if (!Resize(fsize-size))
    {
        dealloc(temp);
        return false;
    }
    if (temp)
    {
        Overwrite(offset,temp,m_size);
        DISCARD_ARRAY(temp);
    }
    changed = true;
    return true;
}

bool RandomAccessFile::Overwrite(unsigned offset, const void*data, unsigned size)
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

bool RandomAccessFile::Overwrite(unsigned offset, BYTE value, unsigned size)
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

bool RandomAccessFile::Insert(unsigned offset, const void*data, unsigned size)
{
    if (!write_access)
        return false;
    changed = true;
    if (offset >= fsize)
        return Append(data,size);
    unsigned m_size = fsize-offset;
    BYTE*temp =Extract(offset,m_size);
    if (!Resize(fsize+size))
    {
        dealloc(temp);
        return false;
    }
    if (temp)
    {
        Overwrite(offset+size,temp,m_size);
        DISCARD_ARRAY(temp);
    }
    Overwrite(offset,data,size);
    return true;
}

bool RandomAccessFile::Insert(unsigned offset, BYTE value, unsigned size)
{
    if (!write_access)
        return false;
    changed = true;
    if (offset >= fsize)
        return Append(value,size);
    unsigned m_size = fsize-offset;
    BYTE*temp =Extract(offset,m_size);
    if (!Resize(fsize+size))
    {
        dealloc(temp);
        return false;
    }
    if (temp)
    {
        Overwrite(offset+size,temp,m_size);
        DISCARD_ARRAY(temp);
    }
    Overwrite(offset,value,size);
    return true;
}

bool RandomAccessFile::Append(const void*data, unsigned size)
{
    unsigned at = fsize;
    return Resize(fsize+size) && Overwrite(at,data,size);
}

bool RandomAccessFile::Append(BYTE value, unsigned size)
{
    unsigned at = fsize;
    return Resize(fsize+size) && Overwrite(at,value,size);
}

UINT RandomAccessFile::GetSize()
{
    return fsize;
}
