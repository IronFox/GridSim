#ifndef listH
#define listH

/******************************************************************

List super-include.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include "../global_root.h"

//#define FAKE_ERASE

#ifdef __GNUC__

#define USE_LROOT\
                        using   Super::reset;\
                        using   Super::each;\
                        using   Super::count;\
                        using   Super::get;\
                        using   Super::getConst;\
                        using   Super::drop;\
                        using   Super::erase;\
                        using   Super::flush;\
                        using   Super::clear;\
                        using   Super::begin;\
                        using   Super::end;\
                        using   Super::size;\
                        using   Super::max_size;\
                        using   Super::empty;\
                        using	Super::iterator;\
                        using	Super::const_iterator;
						

#define USE_LIST        USE_LROOT\
                        using   Super::insert;\
                        using   Super::append;\
                        using   Super::import;\
                        using   Super::importAndFlush;\
                        using   Super::getIndexOf;\
                        using   Super::set;\
                        using   Super::include;


#define USE_SORTED      USE_LROOT\
                        using   Super::lookup;\
                        using   Super::lookupIndex;\
                        using   Super::dropByIdent;\
                        using   Super::eraseByIdent;\
                        using   Super::insert;\
                        using   Super::import;\
                        using   Super::importAndFlush;\
                        using   Super::add;\

#define USE_CONTAINER   typedef typename Super::value_type        value_type;\
                        typedef typename Super::reference         reference;\
                        typedef typename Super::const_reference   const_reference;\
                        typedef typename Super::pointer           pointer;\
                        typedef typename Super::difference_type   difference_type;\
                        typedef typename Super::size_type         size_type;\
                        using	Super::iterator;\
                        using	Super::const_iterator;

#else
    #define USE_LROOT
    #define USE_LIST
    #define USE_SORTED
    #define USE_CONTAINER
#endif


#define CONTAINER_TYPES \
                        typedef C*                  value_type;\
                        typedef value_type&         reference;\
                        typedef const value_type&   const_reference;\
                        typedef value_type*         pointer;\
                        typedef int                 difference_type;\
                        typedef size_t				size_type;

#define CONTAINER_MMETHODS \
                        INLINE      size_t                size()          const {return count();}\
                        INLINE      size_t                max_size()      const {return -1;}\
                        INLINE      bool                    empty()         const {return !count();}\
                        INLINE      Type*                   front()         {return first();}\
                        INLINE      Type*                   back()          {return last();}


#define CONTAINER_METHODS CONTAINER_MMETHODS\
                        INLINE      void                    push_back(C*el) {appendElement(el);}\
                        INLINE      void                    push_front(C*el) {insertElement(0U,el);}




template <class List> class ListIterator;
template <class List> class ConstListIterator;

template <class List> class ListIterator
{
private:
        const List                  *list;

public:
        typedef typename List::Type*    Pntr;
        typedef typename List::Type*&   PntrRef;
        typedef ListIterator<List>     Iterator;

        size_t						 index;

                                    ListIterator(const List*list_, size_t index_);

INLINE  Pntr&                       operator*();
INLINE  ListIterator<List>&        operator++();
INLINE  ListIterator<List>         operator++(int);
INLINE  ListIterator<List>&        operator+=(int delta);
INLINE  ListIterator<List>         operator+(int delta);
INLINE  ListIterator<List>&        operator--();
INLINE  ListIterator<List>         operator--(int);
INLINE  ListIterator<List>&        operator-=(int delta);
INLINE  ListIterator<List>         operator-(int delta);
INLINE  int                         operator-(const ListIterator<List>&other);
INLINE  int                         operator-(const ConstListIterator<List>&other);
INLINE  bool                        operator<(const ListIterator<List>&other);
INLINE  bool                        operator<(const ConstListIterator<List>&other);
INLINE  bool                        operator>(const ListIterator<List>&other);
INLINE  bool                        operator>(const ConstListIterator<List>&other);
INLINE  bool                        operator<=(const ListIterator<List>&other);
INLINE  bool                        operator<=(const ConstListIterator<List>&other);
INLINE  bool                        operator>=(const ListIterator<List>&other);
INLINE  bool                        operator>=(const ConstListIterator<List>&other);
INLINE  bool                        operator==(const ListIterator<List>&other);
INLINE  bool                        operator==(const ConstListIterator<List>&other);
INLINE  bool                        operator!=(const ListIterator<List>&other);
INLINE  bool                        operator!=(const ConstListIterator<List>&other);
};

template <class List> class ConstListIterator
{
private:
        const List                  *list;

public:
        typedef typename List::Type*        Pntr;
        typedef ConstListIterator<List>    Iterator;

        size_t                    index;

                                    ConstListIterator(const List*list_, size_t index_);
                                    ConstListIterator(const ListIterator<List>&other);

INLINE  Pntr                        operator*();
INLINE  Iterator&                   operator++();
INLINE  Iterator                    operator++(int);
INLINE  Iterator&                   operator+=(int delta);
INLINE  Iterator                    operator+(int delta);
INLINE  Iterator&                   operator--();
INLINE  Iterator                    operator--(int);
INLINE  Iterator&                   operator-=(int delta);
INLINE  Iterator                    operator-(int delta);
INLINE  int                         operator-(const ListIterator<List>&other);
INLINE  int                         operator-(const Iterator&other);
INLINE  bool                        operator<(const ListIterator<List>&other);
INLINE  bool                        operator<(const Iterator&other);
INLINE  bool                        operator>(const ListIterator<List>&other);
INLINE  bool                        operator>(const Iterator&other);
INLINE  bool                        operator<=(const ListIterator<List>&other);
INLINE  bool                        operator<=(const Iterator&other);
INLINE  bool                        operator>=(const ListIterator<List>&other);
INLINE  bool                        operator>=(const Iterator&other);
INLINE  bool                        operator==(const ListIterator<List>&other);
INLINE  bool                        operator==(const Iterator&other);
INLINE  bool                        operator!=(const ListIterator<List>&other);
INLINE  bool                        operator!=(const Iterator&other);
};


template <class C, size_t Len> INLINE void movePointers(C**from, C**to);      //!< Copies pointers left to right
template <class C, size_t Len> INLINE void movePointersBack(C**from, C**to);  //!< Copies pointers right to left
template <class C> INLINE void movePointers(C**from, C**to, size_t count);    //!< Copies pointers left to right
template <class C> INLINE void movePointersBack(C**from, C**to, size_t count);//!< Copies pointers right to left



#include "list.tpl.h"

#endif
