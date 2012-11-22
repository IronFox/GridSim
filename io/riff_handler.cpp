#include "../global_root.h"
#include "riff_handler.h"



#ifdef DOUBLE_CHECK_DATA
unsigned char check_buffer[1000000];
#endif
/*
C_String c42ansi(const char*c4)
{
C_String rs;
for (unsigned char i = 0; i < 4; i++)
    rs+=c4[i];
return rs;
}
*/

//rule: now.addr is at the beginning of its data-block/children

char			RiffChunk::str_out[5] = "    ";


static bool ischar(char c)
{
    return isupper((BYTE)c) || isdigit((BYTE)c) || c == ' ';
}


static RIFF_SIZE	castSize(size_t size)
{
	if (size > 0xFFFFFFFFULL)
		FATAL__("Parameter size exceeds maximum allowed RIFF size");
	return (RIFF_SIZE)size;
}

RIFF_ADDR SRiffChunk::blockEnd()
{
    return addr+info.size;
}

RIFF_ADDR SRiffChunk::root()
{
    return addr-RIFF_CHUNK_HEAD_SIZE;
}

void RiffFile::mark()
{
        mark_stack[mark_stacked++] = now;
}

void RiffFile::recall()
{
        if (!mark_stacked)
            return;
        now = mark_stack[--mark_stacked];
}

void RiffFile::forget()
{
    mark_stacked--;
}


const char*RiffFile::mainID()
{
    memcpy(str_out,&history[0].info.id,sizeof(RIFF_INDEX_ID));
    return str_out;
}

#ifdef RIFF_SENSITIVE
    const char*RiffFile::subID()
    {
        memcpy(str_out,&sub_id,sizeof(RIFF_INDEX_ID));
        return str_out;
    }
#endif

RIFF_SIZE RiffFile::mainSize()
{
    return history[0].info.size;
}

bool RiffFile::multipleOf(size_t size)
{
    return now.info.size && !(now.info.size%size);
}

RIFF_SIZE RiffFile::getSize()
{
    return now.info.size;
}

const char*RiffFile::getID()
{
    memcpy(str_out,&now.info.id,sizeof(RIFF_INDEX_ID));
    return str_out;
}

const SRiffChunk& RiffFile::getChunk()
{
    return now;
}

RiffFile::RiffFile():mark_stacked(0),husage(0),streaming(false),ignore_main_size_(false)
{
    str_out[4] = 0;
}

RiffFile::RiffFile(const char*filename):mark_stacked(0),husage(0),streaming(false),ignore_main_size_(false)
{
    str_out[4] = 0;
    open(filename);
}

RiffFile::~RiffFile()
{
    close();
}

bool RiffFile::open(const char*filename)
{
    close();
    if (!RandomAccessFile::open(filename,RandomAccessFile::Direct))
        return false;
    reset();
    return true;
}

void RiffFile::assign(BYTE*data, ULONG size)
{
    close();
    RandomAccessFile::assign(data,size);
    reset();
}

bool RiffFile::reopen()
{
    close();
    if (!RandomAccessFile::reopen(RandomAccessFile::Direct))
        return false;
    reset();
    return true;
}

bool RiffFile::create(const char*filename)
{
    close();
    if (!RandomAccessFile::create(filename,RandomAccessFile::Direct))
        return false;
    RandomAccessFile::append("RIFF",4);
    RandomAccessFile::append((BYTE)0,4);
    reset();
    return true;
}


void RiffFile::close()
{
    if (streaming)
        FATAL__("closing in stream");
    RandomAccessFile::close();
}

void RiffFile::reset()
{
    streaming = false;
    if (!RandomAccessFile::isActive())
        return;

    RandomAccessFile::read(0,now.info);
    if (ignore_main_size_)
        now.info.size = RandomAccessFile::size()-RIFF_CHUNK_HEAD_SIZE;
    now.addr = RIFF_CHUNK_HEAD_SIZE;
    now.index = 0;
    #ifdef  RIFF_SENSITIVE
        if (now.info.id == RIFF_RIFF && now.info.size >= 8)
        {
            RIFF_INDEX_ID test;
            char*ctest = (char*)&test;
            RandomAccessFile::read(RIFF_CHUNK_HEAD_SIZE+4,test);
            if (ischar(ctest[0]) && ischar(ctest[1]) && ischar(ctest[2]) && ischar(ctest[3]))
            {
                RandomAccessFile::read(RIFF_CHUNK_HEAD_SIZE,sub_id);
                now.info.size-=4;
                now.addr+=4;
            }
            else
                sub_id = RIFF_RIFF;
        }
    #endif
    history[0] = now;
    husage = 0;
    mark_stacked = 0;
    enter();
}

bool RiffFile::findFirst(RIFF_INDEX_ID ID)
{
    mark();
    if (first())
        do
            if (isID(ID))
            {
                forget();
                return true;
            }
        while (next());
    recall();
    return false;
}

bool RiffFile::findFirst(RIFF_STRING_ID ID)
{
    mark();
    if (first())
        do
            if (isID(ID))
            {
                forget();
                return true;
            }
        while (next());
    recall();
    return false;
}



bool RiffFile::findNext(RIFF_INDEX_ID ID)
{
    mark();
    while (next())
        if (isID(ID))
        {
            forget();
            return true;
        }
    recall();
    return false;
}

bool RiffFile::findNext(RIFF_STRING_ID ID)
{
    mark();
    while (next())
        if (isID(ID))
        {
            forget();
            return true;
        }
    recall();
    return false;
}

bool RiffFile::select(unsigned index)
{
    if (index < now.index) first();
    while (now.index != index && next());
    return now.index == index;
}

unsigned RiffFile::getIndex()
{
    return now.index;
}


bool RiffFile::isID(RIFF_INDEX_ID ID)
{
    return ID == now.info.id;
}

bool RiffFile::isID(RIFF_STRING_ID ID)
{
    return isID(*(RIFF_INDEX_ID*)ID);
}

RIFF_ADDR RiffFile::tell()
{
    return now.addr+streaming?streaming_at:0;
}


bool RiffFile::next()
{
    RIFF_SIZE size = now.info.size+now.info.size%2;
    if (now.addr+size+RIFF_CHUNK_HEAD_SIZE > history[husage-1].blockEnd())
        return false;
    now.addr+=size+RIFF_CHUNK_HEAD_SIZE;
    RandomAccessFile::read(now.addr-RIFF_CHUNK_HEAD_SIZE,now.info);
    now.index++;
    return true;
}

bool RiffFile::first()
{
    if (streaming)
        FATAL__("seeking first while streaming");
    if (!history[husage-1].info.size||!RandomAccessFile::isActive())
        return false;
    RandomAccessFile::read(history[husage-1].addr,now.info);
    now.addr = history[husage-1].addr+RIFF_CHUNK_HEAD_SIZE;
    now.index = 0;
    return true;
}

bool RiffFile::enter()
{
    if (streaming)
        FATAL__("trying to enter while streaming");
    if ((!isID("LIST") && !isID("RIFF") && husage)) return false;
    history[husage++] = now;
    RandomAccessFile::read(now.addr,now.info);
    now.addr+=RIFF_CHUNK_HEAD_SIZE;
    now.index = 0;
    return true;
}

bool RiffFile::dropBack()
{
    if (streaming)
        FATAL__("trying to drop back while streaming");
    if (husage <= 1)
        return false;
    now = history[--husage];
//    ShowMessage("dropping back "+IntToStr(now.addr));
    return true;
}

RIFF_SIZE RiffFile::get(void*out)
{
    if (streaming)
        FATAL__("get does not work during streaming-operation");
    RandomAccessFile::extract(now.addr,out,now.info.size);
    return now.info.size;
}

RIFF_SIZE RiffFile::get(void*out, size_t max)
{
    if (streaming)
        FATAL__("get does not work during streaming-operation");
    RIFF_SIZE reading = now.info.size<max?now.info.size:castSize(max);
    RandomAccessFile::extract(now.addr,out,reading);
    return now.info.size;
}

RIFF_ADDR RiffFile::getAddr()
{
    return now.addr;
}

const char* RiffFile::getFileName()
{
    return RandomAccessFile::filename;
}


void RiffFile::openStream()
{
    if (streaming)
        FATAL__("already streaming");
    streaming = true;
    streaming_at = 0;
    mark();
}

void RiffFile::closeStream()
{
    if (!streaming)
        FATAL__("trying to close non-existing stream");
    streaming = false;
    recall();
}

bool RiffFile::skip(size_t data_size)
{
    if (!streaming)
        FATAL__("no stream open");
    if (streaming_at+data_size>now.info.size)
        return false;
    streaming_at+=castSize(data_size);
    return true;
}

bool	RiffFile::canStreamBytes(size_t bytes)	const
{
    if (!streaming)
		return false;
    return ((size_t)streaming_at+bytes <= (size_t)now.info.size);
}

bool RiffFile::streamPointer(void*target, size_t data_size)
{
    if (!streaming)
        FATAL__("no stream open");
    if ((size_t)streaming_at+data_size > (size_t)now.info.size)
        return false;
    RandomAccessFile::extract(now.addr+streaming_at,target,castSize(data_size));
    streaming_at+=castSize(data_size);
    return true;
}

bool RiffFile::appendBlock(RIFF_STRING_ID ID, const void*data, size_t size)
{
     return appendBlock(*(RIFF_INDEX_ID*)ID,data,size);
}

bool RiffFile::appendBlock(RIFF_INDEX_ID ID, const void*data, size_t size)
{
//    bool catch_newone = !history[husage-1].info.size;
    RIFF_SIZE isize = RIFF_CHUNK_HEAD_SIZE+castSize(size+size%2);
    BYTE*insert = SHIELDED_ARRAY(new BYTE[isize],isize);
    memcpy(insert,&ID,sizeof(ID));
    memcpy(&insert[sizeof(ID)],&size,sizeof(size));
    memcpy(&insert[RIFF_CHUNK_HEAD_SIZE],data,size);
    RIFF_ADDR offset = history[husage-1].addr+history[husage-1].info.size+history[husage-1].info.size%2;
    memcpy(str_out,&ID,4);
//    ShowMessage(C_String(str_out)+" "+IntToStr(offset));
    if (!RandomAccessFile::insert(offset,insert,isize))
    {
        DISCARD_ARRAY(insert);
        return false;
    }
    for (BYTE i = 0; i < husage; i++)
    {
//        ShowMessage("adding: "+IntToStr(isize)+" to "+IntToStr(i));
        history[i].info.size+=isize;
        RandomAccessFile::overwrite(history[i].root(),history[i].info);
    }
/*    if (catch_newone)
    {*/
        now.info.id = ID;
        now.info.size = castSize(size);
        now.addr = offset+RIFF_CHUNK_HEAD_SIZE;
        now.index = UNSIGNED_UNDEF;
//    }
    DISCARD_ARRAY(insert);
    
    return true;
}

bool RiffFile::insertBlock(RIFF_STRING_ID ID, const void*data, size_t size)
{
     return insertBlock(*(RIFF_INDEX_ID*)ID,data,size);
}

bool RiffFile::insertBlock(RIFF_INDEX_ID ID, const void*data, size_t size)
{
    if (!history[husage-1].info.size)
        return appendBlock(ID,data,size);
    RIFF_SIZE isize = RIFF_CHUNK_HEAD_SIZE+castSize(size+size%2);
    BYTE*insert = SHIELDED_ARRAY(new BYTE[isize],isize);
    memcpy(insert,&ID,sizeof(ID));
    memcpy(&insert[sizeof(ID)],&size,sizeof(size));
    memcpy(&insert[RIFF_CHUNK_HEAD_SIZE],data,size);
    if (!RandomAccessFile::insert(now.addr-RIFF_CHUNK_HEAD_SIZE,insert,isize))
    {
        DISCARD_ARRAY(insert);
        return false;
    }
    now.info.id = ID;
    now.info.size = castSize(size);
    for (BYTE i = 0; i < husage; i++)
    {
        history[i].info.size+=isize;
        RandomAccessFile::overwrite(history[i].root(),history[i].info);
    }
    DISCARD_ARRAY(insert);
    return true;
}

bool RiffFile::dropBlock()
{
    if (!history[husage-1].info.size || !RandomAccessFile::isActive() || streaming)
        return false;
    RIFF_SIZE total_size = now.info.size+now.info.size%2+RIFF_CHUNK_HEAD_SIZE;
    if (!RandomAccessFile::erase(now.addr-RIFF_CHUNK_HEAD_SIZE,total_size))
        return false;
    for (BYTE i = 0; i < husage; i++)
    {
        history[i].info.size-=total_size;
        RandomAccessFile::overwrite(history[i].root(),history[i].info);
    }
    first();    
    return true;
}

bool RiffFile::overwrite(const void*data, size_t check_size)
{
    if (size_t(now.info.size) != check_size || !RandomAccessFile::isActive() || streaming)
        return false;
    return RandomAccessFile::overwrite(now.addr,data,castSize(check_size));
}

bool RiffFile::resizeBlock(size_t size)
{
    bool success;
    RIFF_ADDR end = now.addr+now.info.size+now.info.size%2;
    int difference = castSize((size+size%2)-(now.info.size+now.info.size%2));
    if (difference > 0)
        success = RandomAccessFile::insert(end,(BYTE)0,difference);
    else
        if (difference < 0)
            success = RandomAccessFile::erase(end+difference,-difference);
        else
            return true;
    if (!success)
        return false;
    now.info.size = castSize(size);
    RandomAccessFile::overwrite(now.root(),now.info);
    for (BYTE i = 0; i < husage; i++)
    {
        history[i].info.size+=difference;
        RandomAccessFile::overwrite(history[i].root(),history[i].info);
    }
    return true;
}

bool RiffFile::swapBlocks(unsigned index0, unsigned index1)
{
    if (index0 > index1)
    {
        unsigned save = index0;
        index0 = index1;
        index1 = save;
    }
    mark();
    if (!select(index0))
    {
        recall();
        return false;
    }
    SRiffChunk first = now;
    BYTE*buffer = RandomAccessFile::extract(now.addr-RIFF_CHUNK_HEAD_SIZE,now.info.size+RIFF_CHUNK_HEAD_SIZE);
    if (!select(index1))
    {
        recall();
        dealloc(buffer);
        return false;
    }
    SRiffChunk second = now;
    BYTE*second_buffer = RandomAccessFile::extract(now.addr-RIFF_CHUNK_HEAD_SIZE,now.info.size+RIFF_CHUNK_HEAD_SIZE);
    resizeBlock(first.info.size);
    RandomAccessFile::overwrite(second.addr-RIFF_CHUNK_HEAD_SIZE,buffer,first.info.size+RIFF_CHUNK_HEAD_SIZE);
    select(index0);
    resizeBlock(second.info.size);
    RandomAccessFile::overwrite(first.addr-RIFF_CHUNK_HEAD_SIZE,second_buffer,second.info.size+RIFF_CHUNK_HEAD_SIZE);
    recall();
    dealloc(buffer);
    dealloc(second_buffer);
    return true;
}


bool RiffFile::appendBlocks(RiffFile&other)
{
    mark();
    other.mark();
    if (other.first())
        do
        {
            RIFF_SIZE size = other.getSize();
            char*buffer = alloc<char>(size);
            other.get(buffer);
            if (!appendBlock(other.now.info.id,buffer,size))
            {
                dealloc(buffer);
                recall();
                other.recall();
                return false;
            }
            dealloc(buffer);
        }
        while (other.next());
    other.recall();
    recall();
    return true;
}


//------------------------------------------------------------------------------

RiffChunk::RiffChunk():_data(NULL),_first(NULL),_next(NULL),_current(NULL),_previous(NULL),_parent(NULL),_index(0),_streaming(false)
{
    _info.id = RIFF_RIFF;
}

RiffChunk::RiffChunk(RiffChunk*parent):_data(NULL),_first(NULL),_next(NULL),_current(NULL),_previous(NULL),_parent(parent),_index(0),_streaming(false)
{
    _info.id = RIFF_RIFF;
}

RiffChunk::RiffChunk(RiffChunk*parent, RIFF_INDEX_ID id, const void*data, size_t size):_first(NULL),_next(NULL),_current(NULL),_previous(NULL),_parent(parent),_index(0),_streaming(false)
{
    _info.id = id;
    _info.size = castSize(size);
    RIFF_SIZE i_size = castSize(size + size%2);
    alloc(_data,i_size);
    memcpy(_data,data,size);
}


RiffChunk::~RiffChunk()
{
    dealloc(_data);
    if (_first)
        DISCARD(_first);
    if (_next)
        DISCARD(_next);
}

    
bool RiffChunk::loadFromFile(const char*filename)
{
    clear();
    FILE*f = fopen(filename,"rb");
    if (!f)
        return false;
    clear();
    fseek(f,0,SEEK_END);
    RIFF_SIZE max_len = ftell(f);
    fseek(f,0,SEEK_SET); 
    fromFile(f,max_len,true);
    fclose(f);
    return true;
}

RIFF_SIZE RiffChunk::fromFile(FILE*f, size_t size, bool force_list)
{
    clear();
    if (size < sizeof(SRiffInfo))
        return 0;
    if (!fread(&_info,sizeof(_info),1,f))
		return 0;
    if (size-sizeof(SRiffInfo) < _info.size)
	{
		throw IO::DriveAccess::FileFormatFault("RIFF size violation");
        return sizeof(SRiffInfo);
	}
    RIFF_SIZE local = _info.size;
    _previous = NULL;
    _index = 0;
    _current = NULL;
    #ifdef RIFF_SENSITIVE
        if (_info.id == RIFF_RIFF && _info.size >= 8)
        {
            RIFF_INDEX_ID test;
            char*ctest = (char*)&test;
            fseek(f,4,SEEK_CUR);
            if (!fread(&test,sizeof(test),1,f))
				return 0;
            if (ischar(ctest[0]) && ischar(ctest[1]) && ischar(ctest[2]) && ischar(ctest[3]))
                _info.size-=4;
            else
                fseek(f,-4,SEEK_CUR);
            fseek(f,-4,SEEK_CUR);
        }
    #endif
    if (_info.id == RIFF_LIST || force_list)
        while (local >= sizeof(SRiffInfo))
        {
            if (!_current)
            {
                _current = SHIELDED(new RiffChunk(this));
                _first = _current;
            }
            else
            {
                _current->_next = SHIELDED(new RiffChunk(this));
                _current = _current->_next;
            }
            local-=_current->fromFile(f,local);
        }
    else
    {
        RIFF_SIZE i_size = local+local%2;
        re_alloc(_data,i_size);
        if (fread(_data,1,i_size,f)!=i_size)
			return 0;
    }
    _current = NULL;
    return _info.size+_info.size%2+sizeof(SRiffInfo);
}

void RiffChunk::loadFromData(const void*data, size_t size)
{
    clear();
    fromData((const BYTE*)data,size, true);
}

RIFF_SIZE RiffChunk::fromData(const BYTE*data, size_t size, bool force_list)
{
    clear();
    if (size < sizeof(SRiffInfo))
        return 0;
    memcpy(&_info,data,sizeof(_info));
    if (size-sizeof(SRiffInfo) < _info.size)
        return sizeof(SRiffInfo);
    RIFF_SIZE local = _info.size;
    _previous = NULL;
    _index = 0;
    _current = NULL;
    RIFF_ADDR offset = sizeof(SRiffInfo);
    if (_info.id == RIFF_LIST || force_list)
        while (local >= sizeof(SRiffInfo))
        {
            if (!_current)
            {
                _current = SHIELDED(new RiffChunk(this));
                _first = _current;
            }
            else
            {
                _current->_next = SHIELDED(new RiffChunk(this));
                _current = _current->_next;
            }
            offset+=_current->fromData(&data[offset],local-offset);
        }
    else
    {
        RIFF_SIZE i_size = local+local%2;
        re_alloc(_data,i_size);
        memcpy(_data,&data[offset],i_size);
    }
    _current = NULL;
    return _info.size+_info.size%2+sizeof(SRiffInfo);
}


bool RiffChunk::saveToFile(const char*filename)
{
    FILE*f = fopen(filename,"wb");
    if (!f)
        return false;
    resolveSize(true);
    toFile(f, true);
    fclose(f);
    return true;
}

void RiffChunk::toFile(FILE*f, bool force_list)
{
	if (!fwrite(&_info,sizeof(_info),1,f))
		return;
    if (_info.id == RIFF_LIST || force_list)
    {
        RiffChunk*chunk = _first;
        while (chunk)
        {
            chunk->toFile(f);
            chunk = chunk->_next;
        }
    }
    else
        ASSERT__(fwrite(_data,1,_info.size+_info.size%2,f)==_info.size+_info.size%2);
}

RIFF_SIZE RiffChunk::saveToData(void*out)
{
    resolveSize(true);
    toData((BYTE*)out,true);
    return sizeof(_info)+_info.size+_info.size%2;
}

void RiffChunk::toData(BYTE*out,bool force_list)
{
    memcpy(out,&_info,sizeof(_info));
    RIFF_ADDR offset = sizeof(SRiffInfo);
    if (_info.id == RIFF_LIST || force_list)
    {
        RiffChunk*chunk = _first;
        while (chunk)
        {
            chunk->toData(&out[offset]);
            offset+=chunk->_info.size+chunk->_info.size%2+sizeof(SRiffInfo);
            chunk = chunk->_next;
        }
    }
    else
        memcpy(&out[offset],_data,_info.size + _info.size%2);
}

RIFF_SIZE RiffChunk::resolveSize(bool force_list)
{
    if (_info.id == RIFF_LIST || force_list)
    {
        _info.size = 0;
        RiffChunk*chunk = _first;
        while (chunk)
        {
            _info.size += chunk->resolveSize(false);
            chunk = chunk->_next;
        }
        return sizeof(_info) + _info.size;
    }
    else
        return sizeof(_info) + _info.size + _info.size%2;
}


void RiffChunk::clear()
{
    if (_first)
        DISCARD(_first);
    _first = NULL;
    _current = NULL;
    _previous = NULL;
    _index = 0;
    dealloc(_data);
    _data = NULL;
    _info.size = 0;
}

RiffChunk*RiffChunk::findFirst(RIFF_INDEX_ID ID)
{
    if (RiffChunk*chunk = first())
        do
        {
            if (chunk->_info.id == ID)
                return chunk;
        }
        while (chunk = next());
    return NULL;
}

RiffChunk*RiffChunk::findFirst(RIFF_STRING_ID ID)
{
    return findFirst(*(RIFF_INDEX_ID*)ID);
}

RiffChunk*RiffChunk::findNext(RIFF_INDEX_ID  ID)
{
    while (RiffChunk*chunk = next())
        if (chunk->_info.id == ID)
            return chunk;
    return NULL;
}

RiffChunk*RiffChunk::findNext(RIFF_STRING_ID  ID)
{
    return findNext(*(RIFF_INDEX_ID*)ID);
}

RiffChunk*RiffChunk::select(unsigned index)
{
    if (index < _index)
        first();
    while (_index < index && next());

    if (_index == index)
        return _current;
    return NULL;
}

RiffChunk*RiffChunk::first()
{
    _current = _first;
    _previous = NULL;
    _index = 0;
    return _current;    
}

RiffChunk*RiffChunk::next()
{
    if (!_current)
        return NULL;
    _previous = _current;
    _current = _current->_next;
    _index++;
    return _current;    
}

void RiffChunk::setID(RIFF_INDEX_ID ID)
{
    _info.id = ID;
}

void RiffChunk::setID(RIFF_STRING_ID ID)
{
    _info.id = *(RIFF_INDEX_ID*)ID;
}

RIFF_INDEX_ID RiffChunk::getID()	const
{
    return _info.id;
}

const char*RiffChunk::getIDString()	const
{
    memcpy(str_out,&_info.id,sizeof(RIFF_INDEX_ID));
    return str_out;
}


RIFF_SIZE RiffChunk::get(void*out)	const
{
    memcpy(out,_data,_info.size);
    return _info.size;
}

RIFF_SIZE RiffChunk::get(void*out,size_t max)	const
{
    RIFF_SIZE size = _info.size;
    if (size > max)
        size = castSize(max);
    memcpy(out,_data,size);
    return size;        
}

unsigned RiffChunk::currentIndex()	const
{
    return _index;
}

RiffChunk*RiffChunk::current()
{
    return _current;
}


bool RiffChunk::isID(RIFF_INDEX_ID ID) const
{
    return _info.id == ID;
}

bool RiffChunk::isID(RIFF_STRING_ID ID) const
{
    return _info.id == *(RIFF_INDEX_ID*)ID;
}

RIFF_SIZE RiffChunk::size() const
{
    return _info.size;
}

bool RiffChunk::multipleOf(size_t size) const
{
    return _info.size && !(_info.size%size);
}

void RiffChunk::openStream()
{
    _offset = 0;
    _streaming = true;
}

bool RiffChunk::streamPointer(void*target, size_t data_size)
{
    if (data_size+_offset > _info.size)
        return false;
    memcpy(target,&_data[_offset],data_size);
    _offset += castSize(data_size);
    return true;
}

void RiffChunk::closeStream()
{
    _streaming = false;
}
        
bool RiffChunk::eraseCurrent()
{
    if (!_current)
        return false;
    if (_previous)
        _previous->_next = _current->_next;
    else
        _first = _current->_next;
    _current->_next = NULL;
    DISCARD(_current);
    _current = _previous->_next;
    return true;
}

bool RiffChunk::resizeCurrent(size_t size)
{
    if (!_current)
        return false;
    if (size == _current->_info.size)
        return true;
    BYTE*temp = alloc<BYTE>(size+size%2);
    RIFF_SIZE move = castSize(size);
    if (move > _current->_info.size)
        move = _current->_info.size;
    memcpy(temp,_current->_data,move);
    dealloc(_current->_data);
    _current->_data = temp;
    _current->_info.size = castSize(size);
    return true;
}

RiffChunk*RiffChunk::insertBlock(RIFF_INDEX_ID ID, const void*data, size_t size)
{
    if (!_current)
        return appendBlock(ID,data,size);
    RiffChunk*chunk = SHIELDED(new RiffChunk(this,ID,data,size));
    if (!_previous)
    {
        chunk->_next = _first;
        _first = chunk;
    }
    else
    {
        chunk->_next = _previous->_next;
        _previous->_next = chunk;
    }
    _current = chunk;
    return chunk;
}

RiffChunk*RiffChunk::insertBlock(RIFF_STRING_ID ID, const void*data, size_t size)
{
    return insertBlock(*(RIFF_INDEX_ID*)ID,data,size);
}

RiffChunk*RiffChunk::appendBlock(RIFF_INDEX_ID ID, const void*data, size_t size)
{
    if (!_current)
        first();
    while (next());
    RiffChunk*chunk = SHIELDED(new RiffChunk(this,ID,data,size));
    if (_previous)
        _previous->_next = chunk;
    else
        _first = chunk;
    _current = chunk;
    return chunk;
}

RiffChunk*RiffChunk::appendBlock(RIFF_STRING_ID ID, const void*data, size_t size)
{
    return appendBlock(*(RIFF_INDEX_ID*)ID,data,size);
}

bool RiffChunk::overwrite(const void*data, size_t check_size)
{
    if (_info.size != check_size)
        return false;
    memcpy(_data,data,check_size);
    return true;
}
