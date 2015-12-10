#ifndef namendlistH
#define namendlistH


/******************************************************************

General sorted list (usable in combination with all lists
complying with the RAL-specifications)

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include "sort_classes.h"

/*!
	\brief Sorted list
	\param List Root list class (possibly Vector, ArrayList, or LinkedList)
	\param Class Sort class to use

	Sorted maps items using the provided sort class. Sorted lists store unique elements, meaning they cannot hold two (or more) elements
	classified as identical by the sort class. Sort classes usually compare elements via certain member variables, local operators or the pointer itself.<br />
	Example:<br />
	Sorted&lt;Vector&lt;MyClass&gt;, NameSort&gt; will create a uniquely sorted list
	allowing insert and lookup via the 'name' member of 'MyClass'.
	Identifier lookups will always query the 'name' member of list elements.
*/

template <class List, class Class> class Sorted:protected List,protected Class
{
public:
		typedef List					Super;
		typedef typename List::Type		Type;	//!< List entry type (derived from the underlying list)
					
										Sorted();
	template <class Lst>				Sorted(const Lst&list);
	template <class Lst>
		inline	Sorted<List,Class>&	operator=(const Lst&list);

		inline  Type*					insert(Type*element);	//!< Inserts an element into the sorted list. The method fails if an element matching the new element in regard to the sort class already exists in the list. \param element Pointer to an object to insert into the list \return \b element if it could be inserted, NULL otherwise.
	template <class Lst>
		inline  void					import(Lst&list);				//!< Imports all elements from another list in addition to the elements already contained by the local list. \param list STL conform container list. Certain elements may not be inserted if equivalent entries already exist.
	template <class Lst>
		inline  void					importAndFlush(Lst&list);		//!< Imports all elements from another list in addition to the elements already contained by the local list. Flushes the remote list after finishing. \param list STL conform container list. Certain elements may not be inserted if equivalent entries already exist.
	template <class ID>
		inline  Type*		   			add(const ID&ident);			//!< Insures existence of the specified identifier. A new entry is created if the specified identifier could not be found. \param ident Identifier. The kind of operation performed with this identifier depends on the used sort class. \return Pointer to an existing or newly inserted entry matching the specified identifier.
	template <class ID>
		inline  size_t					lookupIndex(const ID&ident);	//!< Queries the index of an element via its identifier. Operates in O(log n) if random access on the underlying list can be performed in O(1). \param ident Identifier to look for. \return Index of the item matching the specified identifier (1 = first element in the list) or 0 if the specified identifier could not be found.
	template <class ID>
		inline  Type*					lookup(const ID&ident);			//!< Queries the pointer to an element via its identifier. Operates in O(log n) if random access on the underlying list can be performed in O(1). \param ident Identifier to look for. \return Pointer to the item matching the specified identifier or NULL if the identifier could not be found.
	template <class ID>
		inline  Type*					dropByIdent(const ID&ident);	//!< Drops an element from the list via its identifier. The respective element is not deleted. \param ident Identifier to look for. \return Pointer to the removed entry or NULL if the specified identifier could not be found.
	template <class ID>
		inline  void					eraseByIdent(const ID&ident);	//!< Erases an element from the list via its identifier. The respective entry is automatically deleted if found. \param ident Identifier to look for.
		inline  size_t					getIndexOf(const Type*element);	//!< Queries the index of an element via its identifier. Operates in O(log n) if random access on the underlying list can be performed in O(1). \param ident Identifier to look for. \return Index of the item matching the specified identifier (1 = first element in the list) or 0 if the specified identifier could not be found.
		inline  Type*					drop(const Type*element);		//!< Drops the specified element from the list. \return Dropped entry or NULL if \b element could not be found.
		inline  void					erase(const Type*element);		//!< Erases an element from the list. The respective entry is automatically deleted if found.
		inline  Type*					drop(size_t index);			//!< Drops an element from the list via its index (0 = first list entry). The respective element is not deleted. \param index Index of the element to drop in the range [0,count() ). \return Pointer to the removed entry or NULL if the specified index is invalid.
		inline  void					erase(size_t index);			//!< Erases an element from the list via its index (0 = first list entry). The respective entry is automatically deleted if found. \param index Index of the element to drop in the range [0,count() ). 
		inline  Type*					drop();							//!< Drops the entry returned by the last each() call. The respective element is not deleted. \return Dropped entry.
		inline  void					erase();						//!< Erases the entry returned by the last each() call. The respective entry is automatically deleted.
		inline  Type*					operator[](size_t index);					//!< Identical to List::get()
		inline  const Type*				operator[](size_t index)	const;			//!< Identical to List::getConst()
		inline	size_t					operator()(const Type*element);				//!< Identical to getIndexOf()
        inline 							operator size_t()	const {return count();}	//!< Queries the number of entries in the list. \return Number of entries.
		bool							checkIntegrity();
		size_t							duplicateEntry();

		using Super::each;
		using Super::reset;
		using Super::count;
		using Super::get;
		using Super::clear;
		using Super::flush;
		using Super::getIndexOf;
		using Super::begin;
		using Super::end;
		using Super::totalSize;
		using Super::first;
		using Super::last;
		using typename Super::iterator;
		using typename Super::const_iterator;
		using Super::IsEmpty;
		using Super::IsNotEmpty;
//		Super::operator size_t;
		

};


template <class RList> class Named:public Sorted<RList,NameSort>
{
public:
					typedef typename RList::Type	 Type;
					typedef Sorted<RList,NameSort>  Super;
	//				USE_SORTED

					inline  Type*		   operator[](size_t index);
};









#include "sortedlist.tpl.h"
#endif
