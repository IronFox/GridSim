#ifndef byte_streamTplH
#define byte_streamTplH

/*************************************************************************************

This file is part of the DeltaWorks-Foundation or an immediate derivative core module.
Original Author: Stefan Elsen
Copyright (C) 2006 Computer Science Department of the Univerity of Trier, Germany.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

*************************************************************************************/

template <typename T>
	void		ByteStream::assign(Array<T>&array)
	{
		assign(array.pointer(),array.contentSize());
	}


template <class C> void ByteStream::push(const C&object)
{
    pushData(&object,sizeof(object));
}

template <class C> void ByteStream::push(const C*array, count_t count)
{
    pushData(array,sizeof(C)*count);
}

template <class C> ByteStream& ByteStream::operator<<(const C&object)
{
    pushData(&object,sizeof(object));
    return *this;
}


template <class C>  bool ByteStream::stream(C&out)
{
    return getData(&out,sizeof(out));
}

template <class C>  bool ByteStream::stream(C*array, count_t count)
{
    return getData(array,sizeof(C)*count);
}

template <class C> bool ByteStream::operator>>(C&object)
{
    return getData(&object,sizeof(object));
}

#endif
