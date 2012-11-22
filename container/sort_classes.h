#ifndef sort_classesH
#define sort_classesH

/******************************************************************

Sort-classes specifying how to sort the content of lists.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/




#include "list.h"

#define CLASS   struct


#define MAKE_SORT_CLASS(name, key)\
    CLASS name\
    {\
    template <class Type, class Ident> static inline void    create(Type*&target,const Ident&input)\
    {\
        target = SHIELDED(new Type());\
        target->key = input;\
    }\
    template <class Type> static inline bool greaterObject(const Type*obj0, const Type*obj1)\
    {\
        return ((obj0->key) > (obj1->key));\
    }\
    template <class Type, class Ident> static  inline bool greater(const Type*obj, const Ident&id)\
    {\
        return ((obj->key) > (id));\
    }\
    template <class Type> static inline bool lesserObject(const Type*obj0, const Type*obj1)\
    {\
        return ((obj0->key) < (obj1->key));\
    }\
    template <class Type, class Ident> static  inline bool less(const Type*obj, const Ident&id)\
    {\
        return ((obj->key) < (id));\
    }\
};

MAKE_SORT_CLASS(NameSort, name)
MAKE_SORT_CLASS(KeySort, key)
MAKE_SORT_CLASS(IdentSort, ident)

CLASS PointerSort
{
template <class Type, class Ident>  static inline void  create(Type*&target,const Ident&input);
template <class Type, class Ident>  static inline bool  greater(const Type*obj, const Ident&id);
template <class Type>               static inline bool  greaterObject(const Type*obj0, const Type*obj1);
template <class Type, class Ident>  static inline bool  less(const Type*obj, const Ident&id);
template <class Type>               static inline bool  lesserObject(const Type*obj0, const Type*obj1);
};

CLASS OperatorSort
{
template <class Type, class Ident>  static inline void  create(Type*&target,const Ident&input);
template <class C>                  static inline bool  greaterObject(const C*n0, const C*n1);
template <class C0, class C1>       static inline bool  greater(const C0*n0, const C1&n1);
template <class C>                  static inline bool  lesserObject(const C*n0, const C*n1);
template <class C0, class C1>       static inline bool  less(const C0*n0, const C1&n1);
};




#include "sort_classes.tpl.h"
#endif

