#ifndef riff_handlerTplH
#define riff_handlerTplH


template <class C>
bool Chunk::Stream(C&obj)
{
    return StreamPointer(&obj,sizeof(obj));
}

template <class C>
bool Chunk::Stream(C*obj, count_t num)
{
    return StreamPointer(obj,num*sizeof(C));
}

template <class C>
bool File::Stream(C&obj)
{
    return StreamPointer(&obj,sizeof(obj));
}

template <class C>
bool File::Stream(C*obj, count_t num)
{
    return StreamPointer(obj,num*sizeof(C));
}

template <class C>
count_t File::Get(Ctr::ArrayData<C>&out)
{
    if (streaming)
        FATAL__("get does not work during streaming-operation");
	
	count_t count = (size_t)now.info.size/sizeof(C);
	out.SetSize(count);
	file.Extract(now.addr,out.pointer(),(unsigned)out.GetContentSize());
    return count;
}


inline bool File::Exit()
{
    return DropBack();
}

template <typename T>
bool	File::InsertBlock(TID id, const Ctr::ArrayData<T>&data)
{
	return InsertBlock(id,data.pointer(),(RIFF_SIZE)data.GetContentSize());
}

template <typename T>
bool	File::AppendBlock(TID id, const Ctr::ArrayData<T>&data)
{
	return AppendBlock(id,data.pointer(),(RIFF_SIZE)data.GetContentSize());
}

template <typename T>
count_t	Chunk::Get(Ctr::ArrayData<T>&out) const
{
	count_t count = count_t(_info.size)/sizeof(T);
	out.SetSize(count);
	memcpy(out.pointer(),_data,out.GetContentSize());
    return count;
}

template<typename T>
Chunk*		Chunk::InsertBlock(TID id, const Ctr::ArrayData<T>&data)
{
	return insertBlock(id,data.pointer(),data.GetContentSize());
}

template<typename T>
Chunk*		Chunk::AppendBlock(TID id, const Ctr::ArrayData<T>&data)
{
	return AppendBlock(id,data.pointer(),data.GetContentSize());
}


#endif
