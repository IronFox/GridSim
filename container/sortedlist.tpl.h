#ifndef sortedlistTplH
#define sortedlistTplH

/******************************************************************

General sorted list (usable in combination with all lists
complying with the RAL-specifications)

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/





template <class List, class Class> Sorted<List,Class>::Sorted()
{}

template <class List, class Class>
template <class Lst> Sorted<List,Class>::Sorted(const Lst&list)
{
    insert(list);
}

template <class List, class Class>
template <class Lst>INLINE  Sorted<List,Class>& Sorted<List,Class>::operator=(const Lst&list)
{
    clear();
    insert(list);
    return *this;
}



template <class List, class Class>
template <class C1>inline typename List::Type*  Sorted<List,Class>::add(const C1&ident)
{
    size_t lower = 0,
             num = List::count(),
             upper = num;
    while (lower< upper && upper <= num)
    {
        size_t el = (lower+upper)/2;
        Type*element = List::get(el);
        if (Class::greater(element,ident))
            upper = el;
        else
            if (Class::less(element,ident))
                lower = el+1;
            else
                return element;
    }
    Type*result;
    Class::create(result,ident);
    List::insert(lower,result);
    return result;
}

template <class List, class Class> inline  typename List::Type* Sorted<List,Class>::drop(const Type*element)
{
    return List::drop(getIndexOf(element)-1);
}

template <class List, class Class> inline  void Sorted<List,Class>::erase(const Type*element)
{
    List::erase(getIndexOf(element)-1);
}


template <class List, class Class> bool Sorted<List, Class>::checkIntegrity()
{
    for (size_t i = 1; i < count(); i++)
        if (Class::greaterObject(List::get(i-1),List::get(i)))
            return false;
    return true;
}

template <class List, class Class> size_t Sorted<List, Class>::duplicateEntry()
{
    for (size_t i = 0; i < count()-1; i++)
    {
        Type*entry = get(i);
        for (size_t j = i+1; j < count(); j++)
            if (entry == get(j))
                return i+1;
    }
    return 0;
}

template <class List, class Class> inline  typename List::Type*           Sorted<List, Class>::drop(size_t index)
{
    return List::drop(index);
}

template <class List, class Class> inline  void Sorted<List, Class>::erase(size_t index)
{
    List::erase(index);
}

template <class List, class Class> inline  typename List::Type*           Sorted<List,Class>::drop()
{
    return List::drop();
}

template <class List, class Class> inline  void Sorted<List,Class>::erase()
{
    List::erase();
}


template <class List, class Class>inline typename List::Type* Sorted<List, Class>::insert(Type*item)
{
    size_t lower = 0,
             num = List::count(),
             upper = num;
    while (lower< upper && upper <= num)
    {
        size_t el = (lower+upper)/2;
        const Type*element = List::get(el);
        if (Class::greaterObject(element,(const Type*)item))
            upper = el;
        else
            if (Class::lesserObject(element,(const Type*)item))
                lower = el+1;
            else
                return NULL;
    }
    List::insert(lower,item);
    if (!checkIntegrity())
        FATAL__("sort-state corrupted");

    return item;
}



template <class List, class Class>
template <class C1>inline typename List::Type* Sorted<List, Class>::lookup(const C1&ident)
{
    size_t lower = 0,
             num = List::count(),
             upper = num;
    while (lower< upper && upper <= num)
    {
        size_t el = (lower+upper)/2;
        Type*element = List::get(el);
        if (Class::greater(element,ident))
            upper = el;
        else
            if (Class::less(element,ident))
                lower = el+1;
            else
                return element;
    }
    return NULL;
}


template <class List, class Class>
template <class C1>inline size_t Sorted<List, Class>::lookupIndex(const C1&ident)
{
    size_t lower = 0,
             num = List::count(),
             upper = num;
    while (lower< upper && upper <= num)
    {
        size_t el = (lower+upper)/2;
        Type*element = List::get(el);
        if (Class::greater(element,ident))
            upper = el;
        else
            if (Class::less(element,ident))
                lower = el+1;
            else
                return el+1;
    }
    return 0;
}

template <class List, class Class> inline size_t Sorted<List, Class>::getIndexOf(const Type*element)
{
    size_t lower = 0,
             num = List::count(),
             upper = num;
    while (lower< upper && upper <= num)
    {
        size_t el = (lower+upper)/2;
        Type*pntr = List::get(el);
        if (Class::greaterObject(pntr,element))
            upper = el;
        else
            if (Class::lesserObject(pntr,element))
                lower = el+1;
            else
                return el+1;
    }
    return 0;
}

template <class List, class Class> inline	size_t	Sorted<List, Class>::operator()(const Type*element)
{
	return getIndexOf(element);
}


template <class List, class Class>
template <class C1>inline typename List::Type* Sorted<List,Class>::dropByIdent(const C1&ident)
{
    size_t index = lookupIndex(ident);
    if (index)
        return List::drop(index-1);
    return NULL;
}

template <class List, class Class>
template <class C1>inline void Sorted<List, Class>::eraseByIdent(const C1&ident)
{
    erase(lookupIndex(ident)-1);
}

template <class List, class Class>
template <class Lst> inline  void Sorted<List, Class>::import(Lst&other)
{
    if ((const Sorted<List, Class>*)&other == (const Sorted<List, Class>*)this)
    {
    #if DEBUG_LEVEL >= 1
        FATAL__("trying to re-import local content");
    #else
        return;
    #endif
    }
    for (typename Lst::iterator it = other.begin(); it != other.end(); ++it)
        insert(*it);
}

template <class List, class Class>
template <class Lst> inline  void Sorted<List, Class>::importAndFlush(Lst&other)
{
    if ((const Sorted<List, Class>*)&other == this)
    {
    #if DEBUG_LEVEL >= 1
        FATAL__("trying to re-import local content");
    #else
        return;
    #endif
    }
    for (typename Lst::iterator it = other.begin(); it != other.end(); ++it)
        insert(*it);
    other.flush();
}


template <class List, class Class>inline typename List::Type* Sorted<List, Class>::operator[](size_t index)
{
    return List::get(index);
}

template <class List, class Class>inline const typename List::Type* Sorted<List, Class>::operator[](size_t index) const
{
    return List::getConst(index);
}


template <class RList> inline typename RList::Type* Named<RList>::operator[](size_t index)
{
    return RList::get(index);
}



#endif
