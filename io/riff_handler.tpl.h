#ifndef riff_handlerTplH
#define riff_handlerTplH


template <class C>
bool RiffChunk::stream(C&obj)
{
    return streamPointer(&obj,sizeof(obj));
}

template <class C>
bool RiffChunk::stream(C*obj, count_t num)
{
    return streamPointer(obj,num*sizeof(C));
}

template <class C>
bool RiffFile::stream(C&obj)
{
    return streamPointer(&obj,sizeof(obj));
}

template <class C>
bool RiffFile::stream(C*obj, count_t num)
{
    return streamPointer(obj,num*sizeof(C));
}

template <class C>
unsigned RiffFile::get(ArrayData<C>&out)
{
    if (streaming)
        FATAL__("get does not work during streaming-operation");
	
	unsigned count = now.info.size/sizeof(C);
	out.setSize(count);
	RandomAccessFile::extract(now.addr,out.pointer(),(unsigned)out.contentSize());
    return count;
}


inline bool RiffFile::exit()
{
    return dropBack();
}

template <typename ID, typename T>
bool	RiffFile::insertBlock(ID id, const ArrayData<T>&data)
{
	return insertBlock(id,data.pointer(),(RIFF_SIZE)data.contentSize());
}

template <typename ID, typename T>
bool	RiffFile::appendBlock(ID id, const ArrayData<T>&data)
{
	return appendBlock(id,data.pointer(),(RIFF_SIZE)data.contentSize());
}

template <typename T>
unsigned	RiffChunk::get(ArrayData<T>&out) const
{
	unsigned count = _info.size/sizeof(T);
	out.setSize(count);
	memcpy(out.pointer(),_data,out.contentSize());
    return count;
}

template<typename ID, typename T>
RiffChunk*		RiffChunk::insertBlock(ID id, const ArrayData<T>&data)
{
	return insertBlock(id,data.pointer(),data.contentSize());
}

template<typename ID, typename T>
RiffChunk*		RiffChunk::appendBlock(ID id, const ArrayData<T>&data)
{
	return appendBlock(id,data.pointer(),data.contentSize());
}


#endif
