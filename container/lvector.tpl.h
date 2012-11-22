#ifndef lvectorTplH
#define lvectorTplH



/*************************************************************************************

This file is part of the DeltaWorks-Foundation or an immediate derivative core module.
Original Author: Stefan Elsen
Copyright (C) 2006 Computer Science Department of the Univerity of Trier, Germany.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

*************************************************************************************/

#if DEBUG_LEVEL < 2
    #define VCOPY_FIELD(source, target, count)              movePointers(source,target,count)
    #define VMOVE_FIELD                                     VCOPY_FIELD
    #define VCOPY_FIELD_BACK(source, target, count)         movePointersBack(source,target,count)
    #define VMAKE_FIELD(size)                               SHIELDED_ARRAY(new Data[size],size)
    #define VFREE_FIELD(field)                              DISCARD_ARRAY(field)
    #define VSET(at, value)                                 (*(at)) = value
    #define VDELETE(at)                                     DISCARD(*(at))
    #define VGET(at)                                        (*(at))

#else
    #define VCOPY_FIELD(source, target, count)              {for (size_t i_ = 0; i_ < (count); i_++) VSET((target)+i_,VGET((source)+i_));}
    #define VMOVE_FIELD(source, target, count)              {for (size_t i_ = 0; i_ < (count); i_++) VMOVE((target),(target)+i_,VGET((source)+i_));}
    #define VCOPY_FIELD_BACK(source, target, count)         {for (size_t i_ = (count)-1; i_ != -1; i_--) VSET((target)+i_,VGET((source)+i_));}
    #define VMOVE(rt,at, value)                             {SHIELD.identify(list_id).verify((const void**)(this),(void**)(rt),(void**)(at)); (*(at)) = (Data)(value);}
    #define VMAKE_FIELD(size)                               (Data*)SHIELD.identify(list_id).allocate((void**)(this),size)
    #define VFREE_FIELD(field)                              SHIELD.identify(list_id).delocate((void**)(this),(void**)(field))
    #define VSET(at, value)                                 {SHIELD.identify(list_id).verify((const void**)(this),(void**)(root),(void**)(at)); (*(at)) = (Data)(value);}
    #define VDELETE(at)                                     {SHIELD.identify(list_id).verify((const void**)(this),(void**)(root),(void**)(at)); DISCARD(*(at));}
    #define VGET(at)                                        (Data)SHIELD.identify(list_id).retrieve((const void**)(this),(void**)(root),(void**)(at))

#endif

#if DEBUG_LEVEL >= 3
    #define VCHECK                                          {Data*pntr = root; while (*pntr) {if (!SHIELD.shielded((void*)*pntr)) badPointer(*pntr, pntr-root);pntr++;}}
#else
    #define VCHECK
#endif



template <class C> void badPointer(C*pntr, size_t index)
{
    FATAL__("bad pointer");
}


template <class C> INLINE void Vector<C>::inc()
{
    VCHECK

    if (fill == imark)
    {
        C** new_field = VMAKE_FIELD(cells<<1);
        VMOVE_FIELD(root,new_field,cells);
        VFREE_FIELD(root);
        root = new_field;
        fill = root+cells;
        cells <<=1;
        imark = root+cells-1;
    }
    else
        ++fill;
    VSET(fill,NULL);

    VCHECK

//    elements++;
}

template <class C> INLINE void Vector<C>::dec()
{
    if (fill == root+cells/4)
    {
        cells >>= 1;
        C** new_field = VMAKE_FIELD(cells);
        VMOVE_FIELD(root,new_field,fill-root);
        VFREE_FIELD(root);
        root = new_field;
        imark = root+cells-1;
        fill = root+cells/2-1;
    }
    else
        --fill;
    VSET(fill,NULL);
    VCHECK
}


template <class C> INLINE void Vector<C>::decToZero()
{
    if (cells > 4)
    {
        cells = 2;
        C** new_field = VMAKE_FIELD(2);
        VFREE_FIELD(root);
        root = new_field;
        imark = root+1;
        VSET(root,NULL);
    }
    else
        VSET(root,NULL);
    fill = root;
    VCHECK
}


template <class C> Vector<C>::Vector():/*elements(0),*/
    #if DEBUG_LEVEL >= 2
        list_id(global_list_id++),
    #endif
    cells(2),root(VMAKE_FIELD(2)),fill(root),imark(root+1)
{
    VSET(fill,NULL);
}

template <class C> Vector<C>::Vector(const Vector<C>&other):
    #if DEBUG_LEVEL >= 2
        list_id(global_list_id++),
    #endif
    cells(other.cells),root(VMAKE_FIELD(other.cells))
{
    fill = root+(other.fill-other.root);
    imark = root+(other.imark-other.root);
    movePointers(other.root,root,fill-root);
    VCHECK
}

template <class C> Vector<C>::~Vector()
{
    for (iterator it = root; it != fill; ++it)
        VDELETE(it);
    VFREE_FIELD(root);
}

template <class C> INLINE Vector<C>& Vector<C>::operator=(const Vector<C>&other)
{
    for (iterator i = begin(); i != end(); i++)
        VDELETE(i);
    if (cells != other.cells)
    {
        VFREE_FIELD(root);
        cells = other.cells;
        root = VMAKE_FIELD(cells);
    }
    fill = root+(other.fill-other.root);
    imark = root+(other.imark-other.root);
    movePointers(other.root,root,fill-root);
    VCHECK
    return *this;
}

template <class C> INLINE		void			Vector<C>::adoptData(Vector<C>&other)
{
	clear();
	
	swp(cells,other.cells);
	swp(root,other.root);
	swp(fill,other.fill);
	swp(imark,other.imark);
	swp(cursor,other.cursor);
}


template <class C> INLINE  void Vector<C>::reset()
{
    cursor = root;
}

template <class C> INLINE  C* Vector<C>::each()
{
    return VGET(cursor++);
}

template <class C> INLINE  size_t Vector<C>::count()                         const
{
    return fill-root;
}

/*template <class C> INLINE		bool					Vector<C>::empty()							const
{
	return fill == root;
}*/

template <class C> INLINE		bool					Vector<C>::isEmpty()						const
{
	return fill == root;
}
template <class C> INLINE		bool					Vector<C>::isNotEmpty()					const
{
	return fill != root;
}


template <class C> INLINE C* Vector<C>::get(size_t index)
{
    if (index >= (size_t)(fill-root))
        return NULL;
    return VGET(root + index);
}

template <class C> INLINE const C* Vector<C>::get(size_t index)	const
{
    if (index >= (size_t)(fill-root))
        return NULL;
    return VGET(root + index);
}

template <class C> INLINE const C* Vector<C>::getConst(size_t index) const
{
    if (index >= (size_t)(fill-root))
        return NULL;
    return VGET(root + index);
}

template <class C> INLINE C*& Vector<C>::getReference(size_t index)
{
    return VGET(root + index);
}

template <class C> INLINE C** Vector<C>::getIterator(size_t index)
{
    return root+index;
}

template <class C> INLINE const C** Vector<C>::getIterator(size_t index)    const
{
    return root+index;
}

template <class C> INLINE C* Vector<C>::first()
{
    return *root;
}

template <class C> INLINE const C* Vector<C>::first() const
{
    return *root;
}

template <class C> INLINE C* Vector<C>::last()
{
    return fill != root ? *(fill-1): NULL;
}

template <class C> INLINE const C* Vector<C>::last() const
{
    return fill != root ? *(fill-1): NULL;
}


template <class C> INLINE C* Vector<C>::drop()
{
    VCHECK
    size_t index = cursor-root;
    C*result = *(cursor-1);
    VCOPY_FIELD(cursor,cursor-1,fill-cursor);
    dec();
    cursor = root+index-1;
    VCHECK
    return result;
}

template <class C> INLINE void Vector<C>::erase()
{
    VCHECK
    size_t index = cursor-root;
    VDELETE(cursor-1);
    VCOPY_FIELD(cursor,cursor-1,fill-cursor);
    dec();
    cursor = root+index-1;
    VCHECK
}

template <class C> INLINE bool Vector<C>::erase(size_t index)
{
    if (index >= (size_t)(fill-root))
        return false;
    VCHECK
    iterator it = root + index;
    VDELETE(it);
    VCOPY_FIELD(it+1,it,fill-it-1);
    dec();
    VCHECK
	return true;
}

template <class C> INLINE bool Vector<C>::erase(C*element)
{
    return erase(getIndexOf(element)-1);
}

template <class C> INLINE C* Vector<C>::drop(C*element)
{
    return drop(getIndexOf(element)-1);
}


template <class C> INLINE C* Vector<C>::drop(size_t index)
{
    VCHECK
    iterator it = root + index;
    if (it >= fill || it < root)
        return NULL;
    Type*el = VGET(it);
    VCOPY_FIELD(it+1,it,fill-it-1);
    dec();
    VCHECK
    return el;
}
    
template <class C> INLINE void Vector<C>::flush()
{
    decToZero();
}

template <class C> INLINE void Vector<C>::clear()
{
    VCHECK
    for (iterator it = root; it != fill; it++)
        VDELETE(it);
    decToZero();
}


template <class C> INLINE C* Vector<C>::insert(size_t index,C*element)
{
    VCHECK
    if (index >= (size_t)(fill-root))
    {
        push_back(element);
        return element;
    }
    inc();
    iterator it = root + index;
    VCOPY_FIELD_BACK(it,it+1,fill-it-1);
    VSET(it,element);
    VCHECK
    return element;
}

template <class C> INLINE C* Vector<C>::insert(size_t index)
{
    return insert(index, SHIELDED(new Type()));
}

template <class C> INLINE  C** Vector<C>::insert(const iterator&it, C*element)
{
    size_t index = it-root;
    if (index >= (size_t)(fill-root))
    {
        push_back(element);
        return root+index;
    }
    inc();
    iterator i = root+index;
    VCOPY_FIELD_BACK(i,i+1,fill-i-1);
    VSET(i,element);
    return i;
}


template <class C> INLINE C* Vector<C>::append(C*element)
{
    VCHECK
    VSET(fill,element);
    if (fill == imark)
    {
        C** new_field = VMAKE_FIELD(cells<<1);
        VMOVE_FIELD(root,new_field,cells);
        VFREE_FIELD(root);
        root = new_field;
        fill = root+cells;
        cells <<=1;
        imark = root+cells-1;
    }
    else
        ++fill;
    VSET(fill,NULL);
    VCHECK
    return element;
}

template <class C>
template <class List>
INLINE      void Vector<C>::import(List&list)
{
    if ((const Vector<C>*)&list == this)
    {
    #if DEBUG_LEVEL >= 1
        FATAL__("trying to re-import local content");
    #else
        return;
    #endif
    }
    for (typename List::iterator i = list.begin(); i != list.end(); ++i)
        push_back(*i);
}

template <class C>
template <class List>
INLINE      void Vector<C>::importAndFlush(List&list)
{
    if ((const Vector<C>*)&list == this)
    {
    #if DEBUG_LEVEL >= 1
        FATAL__("trying to re-import local content");
    #else
        return;
    #endif
    }
    for (typename List::iterator i = list.begin(); i != list.end(); ++i)
        push_back(*i);
    list.flush();
}




template <class C> INLINE void Vector<C>::push_front(C*element)
{
    VCHECK
    inc();
    movePointersBack(root,root+1,fill-root-1);
    VSET(root,element);
    VCHECK
}

template <class C> INLINE void Vector<C>::push_back(C*element)
{


/* could someone (anyone?) tell me why this crap is faster than my method below??? this doesn't even remotely make any sense.
    const size_type __len = __old_size != 0 ? 2 * __old_size : 1;
    iterator __new_start = _M_allocate(__len);
    iterator __new_finish = __new_start;
    __STL_TRY {
      __new_finish = uninitialized_copy(_M_start, __position, __new_start);
      construct(__new_finish, __x);
      ++__new_finish;
      __new_finish = uninitialized_copy(__position, _M_finish, __new_finish);
    }
    __STL_UNWIND((destroy(__new_start,__new_finish),
                  _M_deallocate(__new_start,__len)));
    destroy(begin(), end());
    _M_deallocate(_M_start, _M_end_of_storage - _M_start);
    _M_start = __new_start;
    _M_finish = __new_finish;
    _M_end_of_storage = __new_start + __len;
    */

    VCHECK
    VSET(fill,element);
    if (fill == imark)
    {
        C** new_field = VMAKE_FIELD(cells<<1);
        VMOVE_FIELD(root,new_field,cells);
        VFREE_FIELD(root);
        root = new_field;
        fill = root+cells;
        cells <<=1;
        imark = root+cells-1;
    }
    else
        ++fill;
    VSET(fill,NULL);
    VCHECK
}

template <class C> INLINE C* Vector<C>::append()
{
    return append(SHIELDED(new Type()));
}

template <class C> INLINE size_t    Vector<C>::getIndexOf(const C*element)  const
{
    for (iterator i = root; i != fill; i++)
        if ((*i) == element)
            return (i-root)+1;
    return 0;
}

template <class C> INLINE void        Vector<C>::set(size_t index, Type*element)
{
    if (index >= (size_t)(fill-root))
        return;
    VSET(root+index,element);
}

template <class C> INLINE C*          Vector<C>::include()
{
    return insert(cursor-root);
}

template <class C> INLINE void          Vector<C>::include(C*element)
{
    insert(cursor-root,element);
}


template <class C> INLINE  C** Vector<C>::begin()
{
    return root;
}

template <class C> INLINE  const C** Vector<C>::begin()   const
{
    return (const C**)root;
}

template <class C> INLINE  C** Vector<C>::end()
{
    return fill;
}

template <class C> INLINE  const C** Vector<C>::end()   const
{
    return (const C**)fill;
}

template <class C> INLINE C**  Vector<C>::erase(const iterator&it)
{
    VCHECK
    size_t index = it-root;
    VCOPY_FIELD(it+1,it,fill-it-1);
    dec();
    VCHECK
    return root+index;
}

template <class C> INLINE   C** Vector<C>::erase(const iterator&from, const iterator&to)
{
    if (to > from)
        return from;
    size_t index = from-root,
             num = to-from;
    for (size_t i = 0; i < num; i++)
        drop(index);
    return root+index;
}

template <class C> INLINE  C** Vector<C>::drop(const iterator&it)
{
    size_t index = it-root;
    drop(index);
    return root+index;
}

template <class C> INLINE  C*  Vector<C>::operator[](size_t index)
{
    if (index >= (size_t)(fill-root))
        return NULL;
    return VGET(root+index);
}

template <class C> INLINE  const C*  Vector<C>::operator[](size_t index)	const
{
    if (index >= (size_t)(fill-root))
        return NULL;
    return VGET(root+index);
}

template <class C> INLINE  size_t    Vector<C>::operator()(const Type*element) const
{
    return getIndexOf(element);
}

template <class C> INLINE      Vector<C>::operator                size_t()                         const

{
    return fill - root;
}

template <class C> INLINE  size_t    Vector<C>::totalSize()                     const
{
    return sizeof(*this)+ (size_t)(cells)*sizeof(C*);
}


template <class C> INLINE bool  Vector<C>::discrete()                      const
{
    VCHECK
    Data*s = root;
    while (*s)
    {
        Data*t = s;
        t++;
        while (*t)
            if ((*t++) == (*s))
                return false;
        s++;
    }
    VCHECK
    return true;
}


template <class C> INLINE void  Vector<C>::swap(size_t index0, size_t index1)
{
    swp(root[index0],root[index1]);
}

template <class C> INLINE void  Vector<C>::revert()
{
    size_t len = fill-root,
            steps = len/2;
    for (size_t i = 0; i < steps; i++)
        swp(root[i],root[len-i-1]);
}

template <class C> INLINE void Vector<C>::swap(Vector<C>&other)
{
//    Swap::swap(elements,other.elements);
    swp(cells,other.cells);
    swp(root,other.root);
    swp(fill,other.fill);
    swp(imark,other.imark);
}



template <class C> ReferenceVector<C>::ReferenceVector()
{}

template <class C> ReferenceVector<C>::ReferenceVector(const Vector<C>&other):Vector<C>(other)
{}

template <class C> ReferenceVector<C>::~ReferenceVector()
{
    Vector<C>::flush();
}

template <class C> Vector<C>& ReferenceVector<C>::operator=(const Vector<C>&other)
{
    Vector<C>::flush();
    Vector<C>::appendAll(other);
    return *this;
}

template <class C> INLINE      C* ReferenceVector<C>::operator[](size_t index)
{
    return Vector<C>::get(index);
}

template <class C> INLINE      const C* ReferenceVector<C>::operator[](size_t index) const
{
    return Vector<C>::get(index);
}

template <class C> INLINE      size_t   ReferenceVector<C>::operator()(const Type*el)         const
{
    return Vector<C>::getIndexOf(el);
}










#endif
