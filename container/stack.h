#ifndef stackH
#define stackH

/******************************************************************

Collection of different stacks.

******************************************************************/


#include "dynamic_elements.h"

#undef SECURE_BOUNDARIES
#define SECURE_BOUNDARIES

template <class C, size_t Depth> class Stack
{
private:
        C                   stack[Depth];
        size_t				volume;
        
public:
                            Stack();
INLINE  const C&            peek()                  const;
INLINE  const C&            pop();
INLINE  void                push(const C&value);
INLINE  size_t				count()                 const;
INLINE  bool                empty()                 const;
INLINE  bool                full()                  const;
INLINE  void                clear();
};


template <class C, size_t Depth> class CPStack
{
private:
        C                   *stack[Depth];
        size_t              volume;

public:
                            CPStack();
virtual                    ~CPStack();
INLINE  C*                  peek();
INLINE  C*                  pop();
INLINE  void                push(C*value);
INLINE  size_t              count()                 const;
INLINE  bool                full()                  const;
INLINE  bool                empty()                 const;
INLINE  void                clear();
INLINE  void                flush();
};




template <class C>  class DynamicStack
{
private:
        SDynamicPath<C>    *first;
        C                   element;
        size_t              volume;
public:
                            DynamicStack();
virtual                    ~DynamicStack();
INLINE  const C&            peek()                  const;
INLINE  const C&            pop();
INLINE  void                push(const C&value);
INLINE  size_t              count()                 const;
INLINE  bool                empty()                 const;
INLINE  void                clear();
};

template <class C>  class DynamicPStack
{
private:
        SDynamicPath<C*>   *first;
        size_t              volume;
public:
                            DynamicPStack();
virtual                    ~DynamicPStack();
INLINE  C*                  peek();
INLINE  C*                  pop();
INLINE  void                push(C*value);
INLINE  size_t              count()                 const;
INLINE  bool                empty()                 const;
INLINE  void                clear();
INLINE  void                flush();
};


#include "stack.tpl.h"

#undef SECURE_BOUNDARIES

#endif

