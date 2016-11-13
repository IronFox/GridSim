#ifndef sort_classesTplH
#define sort_classesTplH

/******************************************************************

Sort-classes specifying how to sort the content of lists.

******************************************************************/



template <class Type, class Ident>  inline void  PointerSort::create(Type*&target,const Ident&input)
{
    target = SHIELDED(new Type(*input));
}

template <class C0, class C1> inline  bool PointerSort::greater(const C0*n0, const C1&n1)
{
    return n0 > (const void*)n1;
}

template <class C0, class C1> inline  bool PointerSort::less(const C0*n0, const C1&n1)
{
    return n0 < (const void*)n1;
}

template <class C0> inline  bool PointerSort::greaterObject(const C0*n0, const C0*n1)
{
    return n0>n1;
}

template <class C0> inline  bool PointerSort::lesserObject(const C0*n0, const C0*n1)
{
    return n0<n1;
}


template <class Type, class Ident> inline void OperatorSort::create(Type*&target, const Ident&input)
{
    target = SHIELDED(new Type(input));
}

template <class C> inline  bool OperatorSort::greaterObject(const C*n0, const C*n1)
{
    return (*n0)>(*n1);
}

template <class C> inline  bool OperatorSort::lesserObject(const C*n0, const C*n1)
{
    return (*n0)<(*n1);
}

template <class C0, class C1> inline  bool OperatorSort::greater(const C0*n0, const C1&n1)
{
    return (*n0)>(n1);
}

template <class C0, class C1> inline  bool OperatorSort::less(const C0*n0, const C1&n1)
{
    return (*n0)<(n1);
}




#endif

