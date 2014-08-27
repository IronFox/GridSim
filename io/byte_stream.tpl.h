#ifndef byte_streamTplH
#define byte_streamTplH


template <typename T>
	void		ByteStream::Assign(Array<T>&array)
	{
		Assign(array.pointer(),array.contentSize());
	}


template <class C> void ByteStream::push(const C&object)
{
	_PushData(&object, sizeof(object));
}

template <class C> void ByteStream::push(const C*array, count_t count)
{
	_PushData(array, sizeof(C)*count);
}

template <class C> ByteStream& ByteStream::operator<<(const C&object)
{
    _PushData(&object,sizeof(object));
    return *this;
}


template <class C>  bool ByteStream::stream(C&out)
{
    return _GetData(&out,sizeof(out));
}

template <class C>  bool ByteStream::stream(C*array, count_t count)
{
	return _GetData(array, sizeof(C)*count);
}

template <class C> bool ByteStream::operator>>(C&object)
{
	return _GetData(&object, sizeof(object));
}

#endif
