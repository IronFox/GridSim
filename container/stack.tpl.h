#ifndef stackTplH
#define stackTplH


/******************************************************************

Collection of different stacks.

******************************************************************/



template <class C, size_t Depth> Stack<C,Depth>::Stack():volume(0)
{}

template <class C, size_t Depth> INLINE const C& Stack<C,Depth>::peek() const
{
    #ifdef SECURE_BOUNDARIES
        if (!volume)
            return stack[0];
    #endif
    return stack[volume-1];
}

template <class C, size_t Depth> INLINE const C& Stack<C,Depth>::pop()
{
    #ifdef SECURE_BOUNDARIES
        if (volume)
            volume--;
        return stack[volume];
    #else
        return stack[--volume];
    #endif
}

template <class C, size_t Depth> INLINE void Stack<C,Depth>::push(const C&value)
{
    #ifdef SECURE_BOUNDARIES
        if (volume < Depth)
    #endif
        stack[volume++] = value;
}

template <class C, size_t Depth> INLINE size_t Stack<C,Depth>::count() const
{
    return volume;
}

template <class C, size_t Depth> INLINE bool Stack<C,Depth>::full() const
{
    return volume == Depth;
}

template <class C, size_t Depth> INLINE bool Stack<C,Depth>::empty() const
{
    return !volume;
}

template <class C, size_t Depth> INLINE void Stack<C,Depth>::clear()
{
    volume = 0;
}


template <class C, size_t Depth> CPStack<C,Depth>::CPStack():volume(0)
{}

template <class C, size_t Depth> CPStack<C,Depth>::~CPStack()
{
    for (size_t i = 0; i < volume; i++)
        DISCARD(stack[i]);
}

template <class C, size_t Depth> INLINE C* CPStack<C,Depth>::peek()
{
    #ifdef SECURE_BOUNDARIES
        if (!volume)
            return NULL;
    #endif
    return stack[volume-1];
}

template <class C, size_t Depth> INLINE C* CPStack<C,Depth>::pop()
{
    #ifdef SECURE_BOUNDARIES
        if (!volume)
            return NULL;
    #endif
    return stack[--volume];
}

template <class C, size_t Depth> INLINE void CPStack<C,Depth>::push(C*value)
{
    #ifdef SECURE_BOUNDARIES
        if (volume < Depth)
    #endif
        stack[volume++] = value;
}

template <class C, size_t Depth> INLINE size_t CPStack<C,Depth>::count()   const
{
    return volume;
}

template <class C, size_t Depth> INLINE bool CPStack<C,Depth>::full()        const
{
    return volume == Depth;
}

template <class C, size_t Depth> INLINE bool CPStack<C,Depth>::empty()       const
{
    return !volume;
}

template <class C, size_t Depth> INLINE void CPStack<C,Depth>::clear()
{
    for (size_t i = 0; i < volume; i++)
        DISCARD(stack[i]);
    volume = 0;
}

template <class C, size_t Depth> INLINE void CPStack<C,Depth>::flush()
{
    volume = 0;
}



template <class C> DynamicStack<C>::DynamicStack():first(NULL),volume(0)
{}

template <class C> DynamicStack<C>::~DynamicStack()
{
    clear();
}

template <class C> INLINE  const C& DynamicStack<C>::peek()                  const
{
    return first?first->element:element;
}

template <class C> INLINE const C& DynamicStack<C>::pop()
{
    if (first)
    {
        element = first->element;
        SDynamicPath<C>*next = first->next;
        DISCARD(first);
        first = next;
        volume--;
    }
    return element;
}

template <class C> INLINE  void DynamicStack<C>::push(const C&value)
{
    SDynamicPath<C>*path = SHIELDED(new SDynamicPath<C>());
    path->element = value;
    path->next = first;
    first = path;
    volume++;
}

template <class C> INLINE  size_t DynamicStack<C>::count()                 const
{
    return volume;
}

template <class C> INLINE  bool DynamicStack<C>::empty()                 const
{
    return !volume;
}

template <class C> INLINE  void DynamicStack<C>::clear()
{
    SDynamicPath<C>*el = first;
    while (el)
    {
        SDynamicPath<C>*next = el->next;
        DISCARD(el);
        el = next;
    }
    first = NULL;
    volume = 0;
}

template <class C> DynamicPStack<C>::DynamicPStack():first(NULL),volume(0)
{}


template <class C> DynamicPStack<C>::~DynamicPStack()
{
    clear();
}

template <class C> INLINE  C* DynamicPStack<C>::peek()
{
    return first?first->element:NULL;
}

template <class C> INLINE  C* DynamicPStack<C>::pop()
{
    if (!first)
        return NULL;
    SDynamicPath<C>*next = first->next;
    C*result = first->element;
    DISCARD(first);
    first = next;
    volume--;
    return result;
}

template <class C> INLINE  void DynamicPStack<C>::push(C*value)
{
    SDynamicPath<C>*path = SHIELDED(new SDynamicPath<C>());
    path->element = value;
    path->next = first;
    first = path;
    volume++;
}

template <class C> INLINE  size_t DynamicPStack<C>::count()  const
{
    return volume;
}

template <class C> INLINE  bool DynamicPStack<C>::empty()      const
{
    return !volume;
}

template <class C> INLINE  void DynamicPStack<C>::clear()
{
    SDynamicPath<C>*el = first;
    while (el)
    {
        SDynamicPath<C>*next = el->next;
        DISCARD(el->element);
        DISCARD(el);
        el = next;
    }
    first = NULL;
    volume = 0;
}

template <class C> INLINE  void DynamicPStack<C>::flush()
{
    SDynamicPath<C>*el = first;
    while (el)
    {
        SDynamicPath<C>*next = el->next;
        DISCARD(el);
        el = next;
    }
    first = NULL;
    volume = 0;
}


#endif
