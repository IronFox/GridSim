#ifndef sorterTplH
#define sorterTplH


/******************************************************************

Global list interface specifications and classes.

******************************************************************/
#if 0

template <class Iterator, class Type, class Class> static void mergeLists(const Iterator&lower, const Iterator&center, const Iterator&upper, Type*buffer, Class&sort)
{
    Type *end = buffer, *middle = NULL;
    for (Iterator i = lower; i != upper; ++i)
    {
        if (i == center)
            middle = end;
        (*end++) = *i;
    }

    Type        *c0 = buffer, *c1 = middle;
    Iterator    out = lower;

    while (c0 != middle && c1 != end)
        if (sort.greaterObject(*c0,*c1))
            (*out++) = (*c1++);
        else
            (*out++) = (*c0++);

    while (c0 != middle)
        (*out++) = (*c0++);
    while (c1 != end)
        (*out++) = (*c1++);
}

template <class Iterator, class Type, class Class> static void mergeListsField(const Iterator&lower, const Iterator&center, const Iterator&upper, Type*buffer, Class&sort)
{
    Type *end = buffer, *middle = NULL;
    for (Iterator i = lower; i != upper; ++i)
    {
        if (i == center)
            middle = end;
        (*end++) = *i;
    }

    Type        *c0 = buffer, *c1 = middle;
    Iterator    out = lower;

    while (c0 != middle && c1 != end)
        if (sort.greaterObject(c0,c1))
            (*out++) = (*c1++);
        else
            (*out++) = (*c0++);

    while (c0 != middle)
        (*out++) = (*c0++);
    while (c1 != end)
        (*out++) = (*c1++);
}



template <class Iterator, class Type, class Class> static void mergeSortSTL(const Iterator&lower, const Iterator&upper, Type*buffer, Class&sort)
{
    if (upper == lower)
        return;
    Iterator u = lower,
             center = lower;
    bool inc(false);
    while (u != upper)
    {
        ++u;
        if (inc)
            ++center;
        inc = !inc;
    }
    if (center == lower)
        return;

    mergeSortSTL(lower,center,buffer,sort);
    mergeSortSTL(center,upper,buffer,sort);
    mergeLists(lower,center,upper,buffer,sort);
}

template <class Class>
template <class List>  void    Sort<Class>::mergeSortSTL(List&list)
{
	if (list.empty())
		return;

    size_t len = (size_t)list.size();
    typename List::value_type*buffer;
    alloc(buffer,len);
    Class sort;
    ::mergeSortSTL(list.begin(),list.end(),buffer,sort);
    dealloc(buffer);
}

template <class List, class Class>  static void  mergeSortRAL(List&list, size_t lower, size_t upper, typename List::Type**buffer, Class&sort)
{
    if (lower+1 >= upper)
        return;
//    cout << "["<<lower<<", "<<upper<<")\n";
    size_t mid = (lower+upper)/2;
    mergeSortRAL(list,lower,mid,buffer,sort);
    mergeSortRAL(list,mid,upper,buffer,sort);
//    cout << " merging ("<<lower<<", "<<mid<<", "<<upper<<")\n";
    mergeLists(list.getIterator(lower), list.getIterator(mid), list.getIterator(upper), buffer, sort);
//    cout << " done.\n";
}

template <class Class>
template <class List>  void    Sort<Class>::mergeSortRAL(List&list)
{
	if (list.empty())
		return;
    typedef typename List::Type Type;
    Type**buffer;
    alloc(buffer,list.count());
    Class sort;
    ::mergeSortRAL(list, 0, list.count(), buffer, sort);
    dealloc(buffer);
}

template <class Type, class Class>  static void  mergeSortField(Type field[], size_t lower, size_t upper, Type buffer[], Class&sort)
{
    if (lower+1 >= upper)
        return;
    size_t mid = (lower+upper)/2;
    mergeSortField(field,lower,mid,buffer,sort);
    mergeSortField(field,mid,upper,buffer,sort);
    mergeListsField(field+lower, field+mid, field+upper, buffer, sort);
}


template <class Class>
template <class Type>  void    Sort<Class>::mergeSortField(Type field[], size_t len)
{
	if (!len)
		return;
    Type*buffer = alloc<Type>(len);
    Class sort;
    ::mergeSortField(field,0,len,buffer,sort);
    dealloc(buffer);
}

template <class Class>
template <class Type>  void    Sort<Class>::mergeSortField(Array<Type>&field)
{
	mergeSortField(field.pointer(),field.length());
}


template <class Class>
template <class List>  void    Sort<Class>::bubbleSortSTL(List&list)
{
    if (list.empty())
        return;
    Class sort;
    for (typename List::iterator end = list.end(); end != list.begin(); --end)
        for (typename List::iterator i = list.begin(), prev = i++; i != end; ++i, ++prev)
            if (sort.greaterObject(*prev,*i))
                swp(*prev,*i);
}

template <class Class>
template <class List>  void    Sort<Class>::bubbleSortRAL(List&list)
{
    if (list.empty())
        return;
    Class sort;
    for (size_t i = 0; i < list.count(); i++)
        for (size_t j=0; j < list.count()-1-i; j++)
            if (sort.greaterObject(list[j],list[j+1]))
                list.swap(j,j+1);
}

template <class Class>
template <class Type>  void    Sort<Class>::bubbleSortField(Type field[], size_t len)
{
    Class sort;
    for (size_t i = 0; i < len; i++)
        for (size_t j=0; j < len-1-i; j++)
            if (sort.greaterObject(field+j,field+j+1))
                swp(field[j],field[j+1]);
}

template <class Class>
template <class Type>  void    Sort<Class>::bubbleSortField(Array<Type>&field)
{
	bubbleSortField(field.pointer(),field.length());
}

template <class Iterator, class Class>  static void    quickSortSTL(const Iterator&first, const Iterator&last, Class&sort)
{
    if (first == last)
        return;
    Iterator i0 = first,
             i1 = last,
             pivot = first,
             lower_boundary = first;
    ++lower_boundary;
    ++i0;
    do
    {
        while (!sort.greaterObject(*i0,pivot) && i0 != last)
            ++i0;
        while (sort.greaterObject(*i1,pivot) && i1 != lower_boundary)
            --i1;

        if (i0 < i1)
            swp(*i0,*i1);
    }
    while (i0 < i1);
    swp(*first,*i1);
    i0 = i1;
    if (first != i0)
    {
        --i0;
        quickSortSTL(first,i0,sort);
    }
    if (last != i1)
    {
        ++i1;
        quickSortSTL(i1,last,sort);
    }
}

template <class Class>
template <class List>  void    Sort<Class>::quickSortSTL(List&list)
{
    if (list.empty())
        return;
    Class sort;
    ::quickSortSTL(list.begin(),--list.end(),sort);
}


template <class List, class Class> static void quickSortRAL(List&list, index_t lo, index_t hi, Class&sort)
{
    if (lo >= hi)
        return;

    index_t pivot_index = (lo+hi+1)/2;
    list.swap(pivot_index,hi);
    typename List::Type*pivot = list[hi];
    size_t write=lo;

    for (index_t i = lo; i < hi; i++)
        if (!sort.greaterObject(list[i],pivot))
            list.swap(write++,i);

    list.swap(write,hi);

    if (write>1)
        quickSortRAL(list,lo,write-1,sort);
    quickSortRAL(list,write+1,hi,sort);

}

template <class Class>
template <class List>  void    Sort<Class>::quickSortRAL(List&list)
{
	if (list.empty())
		return;
    Class sort;
    ::quickSortRAL(list,0,list.count()-1,sort);
}


template <class Type, class Class> static void quickSortField(Type field[], size_t lo, size_t hi, Class&sort)
{
    if (lo >= hi)
        return;

    index_t pivot_index = (lo+hi+1)/2;
    swp(field[pivot_index],field[hi]);
    Type*pivot = field+hi;
    size_t write=lo;

    for (size_t i = lo; i < hi; i++)
        if (!sort.greaterObject(field+i,pivot))
            swp(field[write++],field[i]);

    swp(field[write],field[hi]);

    if (write>1)
        quickSortField(field,lo,write-1,sort);
    quickSortField(field,write+1,hi,sort);
}

template <class Class>
template <class Type>  void    Sort<Class>::quickSortField(Type field[], size_t len)
{
	if (!len)
		return;
    Class sort;
    ::quickSortField(field,0,len-1,sort);
}

template <class Class>
template <class Type>  void    Sort<Class>::quickSortField(Array<Type>&field)
{
	quickSortField(field.pointer(),field.length());
}

template <class List, class Class> static void downheap(List&list, Class&sort, index_t v, size_t max)
{
        index_t w=2*v+1;    // first descendant of v
        while (w<max)
        {
            if (w+1<max && sort.greaterObject(list[w+1],list[w]))
                w++;
            // w is the descendant of v with maximum label

            if (!sort.greaterObject(list[w],list[v]))
                return;  // v has heap property
            // otherwise
            list.swap(v, w);
            v=w;        // continue
            w=2*v+1;
        }
}

template <class Type, class Class> static void downheapField(Type field[], Class&sort, index_t v, size_t max)
{
        index_t w=2*v+1;    // first descendant of v
        while (w<max)
        {
            if (w+1<max && sort.greaterObject(field+(w+1),field+w))
                w++;
            // w is the descendant of v with maximum label

            if (!sort.greaterObject(field+w,field+v))
                return;  // v has heap property
            // otherwise
            swp(field[v], field[w]);
            v=w;        // continue
            w=2*v+1;
        }
}



template <class List, class Class>  static void buildHeap(List&list, Class&sort)
{
    for (index_t i = list.count()/2-1; i != index_t(-1); i--)
        downheap(list,sort,i, list.count());
}

template <class Type, class Class>  static void buildFieldHeap(Type field[], size_t len, Class&sort)
{
    for (index_t i = len/2-1; i != index_t(-1); i--)
        downheapField(field,sort,i, len);
}


template <class Class>
template <class List>  void    Sort<Class>::heapSortRAL(List&list)
{
	if (list.empty())
		return;
    Class sort;
    buildHeap(list,sort);
    size_t remaining = list.count();
    while (--remaining)
    {
        list.swap(0,remaining);
        downheap(list,sort,0,remaining);
    }
}

template <class Class>
template <class Type>  void    Sort<Class>::heapSortField(Type field[], size_t len)
{
	if (!len)
		return;
    Class sort;
    buildFieldHeap(field,len,sort);
    size_t remaining = len;
    while (--remaining)
    {
        swp(field[0],field[remaining]);
        downheapField(field,sort,0,remaining);
    }
}

template <class Class>
template <class Type>  void    Sort<Class>::heapSortField(Array<Type>&field)
{
	heapSortField(field.pointer(),field.length());
}


template <class Class>
template <class Type>  bool    Sort<Class>::isSorted(const Type field[], size_t len)
{
    Class sort;
    for (size_t i = 1; i < len; i++)
        if (sort.greaterObject(field+i-1,field+i))
            return false;
    return true;
}

template <class Class>
template <class Type>  bool    Sort<Class>::isSortedArray(const Array<Type>&field)
{
    Class sort;
    for (size_t i = 1; i < field.length(); i++)
        if (sort.greaterObject(field+i-1,field+i))
            return false;
    return true;
}

template <class Class>
template <class List>  bool    Sort<Class>::isSorted(const List&list)
{
    if (list.empty())
        return true;
    typename List::const_iterator   next = list.begin(),
                                    prev = next;
    Class sort;
    ++next;
    while (next != list.end())
        if (sort.greaterObject(*prev++,*next++))
            return false;
    return true;
}

#endif /*0*/

#endif
