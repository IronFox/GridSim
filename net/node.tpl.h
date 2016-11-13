#ifndef node_tplH
#define node_tplH


/******************************************************************

Network-interface.

******************************************************************/


template <class Item>
	Item*	Net::Queue<Item>::get()
	{
	    if (first == last)
	        return NULL;
	    Item*result = loop[first];
	    first++;
	    return result;
	}

template <class Item>
	Net::Queue<Item>::Queue():first(0),last(0)
	{}

template <class Item>
	Item*Net::Queue<Item>::peek()
	{
	    if (first == last)
	        return NULL;
	    return loop[first];
	}

template <class Item>
	Item*Net::Queue<Item>::peek(UINT16 element)
	{
	    return loop[first+element];
	}

template <class Item>
	void Net::Queue<Item>::insert(Item*item)
	{
	    if (last+1 == first)
	    {
	        DISCARD(item);
	        return;
	    }
	    loop[last] = item;
	    last++;
	}

template <class Item>
	void Net::Queue<Item>::clear()
	{
	    while (first != last)
	    {
	        DISCARD(loop[first]);
	        first++;
	    }
	}

template <class Item> UINT16 Net::Queue<Item>::elements()
{
    return last-first;
}

#if 0
template <class C> inline void Net::Connection::send(UINT16 channel, BYTE flags, const C*data, unsigned elements)
{
    write(channel,flags,data,sizeof(C)*elements);
}

template <class C> inline void Net::Connection::sendArray(UINT16 channel, BYTE flags, const Array<C>&data)
{
    write(channel,flags,data.pointer(),data.GetContentSize());
}

template <class C> inline void Net::Connection::sendArray(UINT16 channel, const Array<C>&data)
{
    write(channel,NET_CONFIRM|NET_SERIAL,data.pointer(),data.GetContentSize());
}

template <class C> inline void Net::Connection::send(UINT16 channel, BYTE flags, const C&data)
{
    write(channel,flags,&data,sizeof(data));
}

template <class C> inline void Net::Connection::send(UINT16 channel, const C*data, unsigned elements)
{
    write(channel,NET_CONFIRM|NET_SERIAL,data,sizeof(C)*elements);
}

template <class C> inline void Net::Connection::send(UINT16 channel, const C&data)
{
    write(channel,NET_CONFIRM|NET_SERIAL,&data,sizeof(data));
}

template <class C> inline void Net::Server::sendArray(UINT16 channel, BYTE flags, const Array<C>&data)
{
    write(channel,flags,data.pointer(),data.GetContentSize());
}

template <class C> inline void Net::Server::sendArray(UINT16 channel, const Array<C>&data)
{
    write(channel,NET_CONFIRM|NET_SERIAL,data.pointer(),data.GetContentSize());
}


template <class C> inline void Net::Server::send(UINT16 channel, const C*data, unsigned elements)
{
    write(channel,NET_CONFIRM|NET_SERIAL,data,sizeof(C)*elements);
}

template <class C> inline void Net::Server::send(UINT16 channel, const C&data)
{
    write(channel,NET_CONFIRM|NET_SERIAL,&data,sizeof(data));
}
template <class C> inline void Net::Server::send(UINT16 channel, BYTE flags, const C*data, unsigned elements)
{
    write(channel,flags,data,sizeof(C)*elements);
}

template <class C> inline void Net::Server::send(UINT16 channel, BYTE flags, const C&data)
{
    write(channel,flags,&data,sizeof(data));
}
#endif /*0*/

#endif
