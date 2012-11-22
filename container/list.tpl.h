#ifndef listTplH
#define listTplH


/*************************************************************************************

This file is part of the DeltaWorks-Foundation or an immediate derivative core module.
Original Author: Stefan Elsen
Copyright (C) 2006 Computer Science Department of the Univerity of Trier, Germany.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

*************************************************************************************/

template <class List> ListIterator<List>::ListIterator(const List*list_, size_t index_):list(list_),index(index_)
{}

template <class List> INLINE  typename List::Type*&    ListIterator<List>::operator*()
{
    return list->getReference(index);
}

template <class List> INLINE  ListIterator<List>&      ListIterator<List>::operator++()
{
    index++;
    return *this;
}

template <class List> INLINE  ListIterator<List>       ListIterator<List>::operator++(int)
{
    return Iterator(list,index++);
}

template <class List> INLINE  ListIterator<List>&      ListIterator<List>::operator+=(int delta)
{
    index+=delta;
    return *this;
}

template <class List> INLINE  ListIterator<List>       ListIterator<List>::operator+(int delta)
{
    return Iterator(list,index+delta);
}

template <class List> INLINE  ListIterator<List>&      ListIterator<List>::operator--()
{
    index--;
    return *this;
}

template <class List> INLINE  ListIterator<List>       ListIterator<List>::operator--(int)
{
    return Iterator(list,index--);
}

template <class List> INLINE  ListIterator<List>&      ListIterator<List>::operator-=(int delta)
{
    index -= delta;
    return *this;
}

template <class List> INLINE  ListIterator<List>       ListIterator<List>::operator-(int delta)
{
    return Iterator(list,index-delta);
}

template <class List> INLINE  int                         ListIterator<List>::operator-(const ListIterator<List>&other)
{
    return index - other.index;
}

template <class List> INLINE  int                         ListIterator<List>::operator-(const ConstListIterator<List>&other)
{
    return index - other.index;
}

template <class List> INLINE  bool                        ListIterator<List>::operator<(const ListIterator<List>&other)
{
    return index < other.index;
}

template <class List> INLINE  bool                        ListIterator<List>::operator<(const ConstListIterator<List>&other)
{
    return index < other.index;
}

template <class List> INLINE  bool                        ListIterator<List>::operator>(const ListIterator<List>&other)
{
    return index > other.index;
}

template <class List> INLINE  bool                        ListIterator<List>::operator>(const ConstListIterator<List>&other)
{
    return index > other.index;
}

template <class List> INLINE  bool                        ListIterator<List>::operator<=(const ListIterator<List>&other)
{
    return index <= other.index;
}

template <class List> INLINE  bool                        ListIterator<List>::operator<=(const ConstListIterator<List>&other)
{
    return index <= other.index;
}

template <class List> INLINE  bool                        ListIterator<List>::operator>=(const ListIterator<List>&other)
{
    return index >= other.index;
}

template <class List> INLINE  bool                        ListIterator<List>::operator>=(const ConstListIterator<List>&other)
{
    return index >= other.index;
}

template <class List> INLINE  bool                        ListIterator<List>::operator==(const ListIterator<List>&other)
{
    return index == other.index;
}

template <class List> INLINE  bool                        ListIterator<List>::operator==(const ConstListIterator<List>&other)
{
    return index == other.index;
}

template <class List> INLINE  bool                        ListIterator<List>::operator!=(const ListIterator<List>&other)
{
    return index != other.index;
}

template <class List> INLINE  bool                        ListIterator<List>::operator!=(const ConstListIterator<List>&other)
{
    return index != other.index;
}

template <class List> ConstListIterator<List>::ConstListIterator(const List*list_, size_t index_):list(list_),index(index_)
{}

template <class List> ConstListIterator<List>::ConstListIterator(const ListIterator<List>&other):list(other.list),index(other.index)
{}

template <class List> INLINE  typename List::Type*                        ConstListIterator<List>::operator*()
{
    return list->getConst(index);
}

template <class List> INLINE  ConstListIterator<List>&                   ConstListIterator<List>::operator++()
{
    index++;
    return *this;
}

template <class List> INLINE  ConstListIterator<List>                    ConstListIterator<List>::operator++(int)
{
    return Iterator(list,index++);
}

template <class List> INLINE  ConstListIterator<List>&                   ConstListIterator<List>::operator+=(int delta)
{
    index+=delta;
    return *this;
}

template <class List> INLINE  ConstListIterator<List>                    ConstListIterator<List>::operator+(int delta)
{
    return Iterator(list,index+delta);
}

template <class List> INLINE  ConstListIterator<List>&                   ConstListIterator<List>::operator--()
{
    index--;
    return *this;
}

template <class List> INLINE  ConstListIterator<List>                    ConstListIterator<List>::operator--(int)
{
    return Iterator(list,index--);
}

template <class List> INLINE  ConstListIterator<List>&                   ConstListIterator<List>::operator-=(int delta)
{
    index-=delta;
    return *this;
}

template <class List> INLINE  ConstListIterator<List>                    ConstListIterator<List>::operator-(int delta)
{
    return Iterator(list,index-delta);
}

template <class List> INLINE  int                         ConstListIterator<List>::operator-(const ListIterator<List>&other)
{
    return index - other.index;
}

template <class List> INLINE  int                         ConstListIterator<List>::operator-(const Iterator&other)
{
    return index - other.index;
}

template <class List> INLINE  bool                        ConstListIterator<List>::operator<(const ListIterator<List>&other)
{
    return index < other.index;
}

template <class List> INLINE  bool                        ConstListIterator<List>::operator<(const Iterator&other)
{
    return index < other.index;
}

template <class List> INLINE  bool                        ConstListIterator<List>::operator>(const ListIterator<List>&other)
{
    return index > other.index;
}

template <class List> INLINE  bool                        ConstListIterator<List>::operator>(const Iterator&other)
{
    return index > other.index;
}

template <class List> INLINE  bool                        ConstListIterator<List>::operator<=(const ListIterator<List>&other)
{
    return index <= other.index;
}

template <class List> INLINE  bool                        ConstListIterator<List>::operator<=(const Iterator&other)
{
    return index <= other.index;
}

template <class List> INLINE  bool                        ConstListIterator<List>::operator>=(const ListIterator<List>&other)
{
    return index >= other.index;
}

template <class List> INLINE  bool                        ConstListIterator<List>::operator>=(const Iterator&other)
{
    return index >= other.index;
}

template <class List> INLINE  bool                        ConstListIterator<List>::operator==(const ListIterator<List>&other)
{
    return index == other.index;
}

template <class List> INLINE  bool                        ConstListIterator<List>::operator==(const Iterator&other)
{
    return index == other.index;
}

template <class List> INLINE  bool                        ConstListIterator<List>::operator!=(const ListIterator<List>&other)
{
    return index != other.index;
}

template <class List> INLINE  bool                        ConstListIterator<List>::operator!=(const Iterator&other)
{
    return index != other.index;
}





template <class C, size_t Len> INLINE void movePointers(C**from, C**to)
{
    memcpy(to,from,Len*sizeof(C*));
}

template <class C, size_t Len> INLINE void movePointersBack(C**from, C**to)
{
    for (size_t i = Len-1; i < Len; i--)
        to[i] = from[i];
}


template <class C> INLINE void movePointers(C**from, C**to, size_t count)
{
    memcpy(to,from,count*sizeof(C*));


/*    size_t lc = count/8;
    while (lc--)
    {
        to[0] = from[0];
        to[1] = from[1];
        to[2] = from[2];
        to[3] = from[3];
        to[4] = from[4];
        to[5] = from[5];
        to[6] = from[6];
        to[7] = from[7];
        to += 8;
        from+=8;
    }
    lc = count%8;
    while (lc--)
        (*to++) = (*from++);
  */
}

template <class C> INLINE void movePointersBack(C**from, C**to, size_t count)
{
    //memcpy(to,from,count*sizeof(C*));
    C   **e0 = from+count-1,
        **e1 = to+count-1;
    size_t lc = count/8;
    while (lc--)
    {
        (*e1--) = (*e0--);
        (*e1--) = (*e0--);
        (*e1--) = (*e0--);
        (*e1--) = (*e0--);
        (*e1--) = (*e0--);
        (*e1--) = (*e0--);
        (*e1--) = (*e0--);
        (*e1--) = (*e0--);
    }
    lc = count%8;
    while (lc--)
        (*e1--) = (*e0--);
}



#endif
