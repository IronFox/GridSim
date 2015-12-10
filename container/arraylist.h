#ifndef arraylistH
#define arraylistH


/******************************************************************

Implementation of the ArrayList-class.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include "list.h"


namespace Container
{

#define B_ArrayList			ArrayList
#define ArrayList			ArrayList
#define C_ArrayList			ArrayList
#define C_ArrayReference	ReferenceArrayList




template <class C, size_t Size> struct SArraySection	//! Field container
{
			C						*field[Size];		//!< Pointer field
			SArraySection<C,Size>	*next,				//!< Next pointer field
									*prev;				//!< Previous pointer field
};

/*!
	\brief Array-list pointer container
	
	The ArrayList object is a standardized container in compliance with the
	STL container specification and the Delta-Works RAL specification.
	It stores pointers in large arrays which again are linked to one another.<br />
	The array list is best used when appending massive data at the end.
	Otherwise the vector list is superior.
*/
template <class C, size_t Size> class ArrayList	//RSize used for both Size and Multiplier
{
protected:
typedef		SArraySection<C,Size>*	Section;
			typedef C**				Iterator;

			Section					map[Size],				//!< Section map for fast lookup
									top,					//!< Pointer to the top most section
									lookup,					//!< Section link used for lookups
									cursor;					//!< Section link used for 'each' operations
									
			UINT32					lookup_id,				//!< Index used for section lookup
									elements,				//!< Number of elements currently held by the list
									sections;				//!< Number of sections currently managed by the list
			Iterator				root,					//!< First entry in the top section
									Fill,					//!< Current entry in the top section
									ceiling,				//!< Top most entry in the top section
									cursor_id,				//!< Entry link used in 'each' operations
									cursor_limit;			//!< Top most entry link in the current cursor link

INLINE		Section					section(size_t sid);	//!< Locates a section
INLINE		const Section			section(size_t sid)	const;	//!< Locates a section

public:

			CONTAINER_TYPES

			typedef C				Type;
			typedef C*				Data;

			typedef ListIterator<ArrayList<C,Size> >		iterator;
			typedef ConstListIterator<ArrayList<C,Size> >	const_iterator;


									ArrayList();
template <size_t RSize>				ArrayList(const ArrayList<C,RSize>&other);
virtual								~ArrayList();
template <size_t RSize>
INLINE		ArrayList<C,Size>&		operator=(const ArrayList<C,RSize>&other);

INLINE		void					flush();
INLINE		void					clear();
INLINE		void					push_front(Type*element);
inline		void					push_back(Type*element);
INLINE		Type*					append(Type*element);
INLINE		Type*					append();
template <size_t RSize>
INLINE		void					import(ArrayList<C,RSize>&other);
template <class List>
INLINE		void					import(List&list);
template <class List>
INLINE		void					importAndFlush(List&list);
INLINE		Type*					insert(UINT32 index, Type*element);
INLINE		Type*					insert(UINT32 index);
INLINE		iterator				insert(const iterator&it, Type*element);
INLINE		Type*					drop(UINT32 index);
INLINE		iterator				drop(const iterator&it);
INLINE		Type*					drop(Type*element);
INLINE		bool					erase(UINT32 index);
INLINE		iterator				erase(const iterator&it);
INLINE		iterator				erase(const iterator&from, const iterator&to);
INLINE		bool					erase(Type*element);
INLINE		void					set(UINT32 index, Type*element);
INLINE		void					swap(UINT32 index0, UINT32 index1);
INLINE		void					swap(ArrayList<C,Size>&other);
INLINE		void					revert();

INLINE		Type*					get(UINT32 index);
INLINE		const Type*				get(UINT32 index)					const;
INLINE		const Type*				getConst(UINT32 index)			const;
INLINE		Data&					getReference(UINT32 index);
INLINE		iterator				getIterator(UINT32 index);
INLINE		const_iterator			getIterator(UINT32 index)			const;
INLINE		UINT32					getIndexOf(const Type*element)	const;
INLINE		Type*					first();
INLINE		const Type*				first()							const;
INLINE		Type*					last();
INLINE		const Type*				last()							const;
INLINE		iterator				begin();
INLINE		const_iterator			begin()							const;
INLINE		iterator				end();
INLINE		const_iterator			end()							const;
INLINE		Type*					operator[](size_t index);
INLINE		const Type*				operator[](size_t index)		const;
INLINE		UINT32					operator()(const Type*)			const;
INLINE		operator				size_t()						const;		//!< Implict cast to size_t	\return Number of elements in the list


INLINE		UINT32					count()							const;
INLINE		size_t					totalSize()						const;

INLINE		void					reset();
INLINE		Type*					each();
INLINE		Type*					drop();
INLINE		void					erase();
INLINE		Type*					include();
INLINE		void					include(Type*element);


			CONTAINER_MMETHODS
};


/*!
	\fn void		ArrayList::flush()
	\brief	Clears the local list without deleting the contained elements.
	
	\fn void ArrayList::clear()
	\brief Deletes all contained elements and clears the local list.
	
	\fn void ArrayList::push_front(Type*element)
	\brief Inserts the specified element before the first element in the list
	\param element Element to insert
	
	push_front operates in O(n).
	
	\fn void ArrayList::push_back(Type*element)
	\brief Appends the specified element to the end of the list.
	\param element Element to insert
	
	push_back operates in O(1).
	
	\fn Type*	ArrayList::append(Type*element)
	\brief Appends the specified element to the end of the list.
	\param element Element to append
	
	append operates in O(1).

	
	\fn Type* ArrayList::append()
	\brief Appends a new element to the end of the list.
	\return Pointer to a new object of type \b Type which has been appended to the list
	
	\fn void ArrayList::import(ArrayList< C, RSize >&other)
	\brief Imports all elements from another array list
	\param other List to import all elements from
	
	\fn void ArrayList::import(List&list)
	\brief Imports all elements from another STL conform list
	\param list List to import all elements from (must be STL container conform)
	
	\fn void ArrayList::importAndFlush(List&list)
	\brief Imports all elements from another list and flushes that list
	\param list List to import all elements from and flush afterwards
	
	\fn Type* ArrayList::insert(UINT32 index, Type*element)
	\brief Inserts the specified element before the specified index
	\param index Index to insert before (0 = first element)
	\param element Element to insert
	\return element
	
	Inserts \a element before the specified index. If \a index is equal or greater count() then
	append() will be invoked instead.
	insert() operates in O(n).

	
	\fn Type* ArrayList::insert(UINT32 index)
	\brief Inserts a new element before the specified index
	\param index Index to insert before
	\return Pointer to a new element of type \b Type which has been inserted into the list
	
	Inserts a new \p Type -object before the specified \a index (providing 0 as \a index
	results in inserting before the first element). If \a index is equal or greater count()
	an insert()-call results in an append()-call.
	insert() operates in O(n).
	
	\fn iterator	ArrayList::insert(const iterator&it, Type*element)
	\brief Inserts the specified element at the specified iterator position
	\param it Iterator position
	\param element Element to insert
	\return Iterator that points to the newly inserted element
	
	insert() inserts \a element as the immediate predecessor of the given iterator's
	target.
	
	\fn Type* ArrayList::drop(UINT32 index)
	\brief Drops the element at the specified index from the list (without deleting it)
	\param index Index of the element to drop
	\return pointer to the dropped element or NULL if \b index is invalid
	(may also return NULL if the specified element contained NULL).
	
	Drops the specified element from the list without deleting it.
	Operates in O(n).

	\fn iterator ArrayList::drop(const iterator&it)
	\brief Drops the element at the specified iterator position from the list (without deleting it)
	\param it Valid iterator targeting the immediate successor of the dropped element.
	\return iterator pointing to the object immediately after the dropped element

	Drops the iterator-target from the list. This operation may de-validate certain
	or all iterators hooked to the list. Operates in O(n).

	\fn Type* ArrayList::drop(Type*element)
	\brief Drops the specified element from the list (without deleting it)
	\param element Element to drop
	\return \b element on success, NULL otherwise
	
	\fn bool ArrayList::erase(UINT32 index)
	\brief Erases the element at the specified index from the list (also deletes the object)
	\param index Index of the element to erase
	\return true on success, false if the specified element was not found
	
	Erases the element specified by index. No operation is performed if \a index is invalid.
	Operates in O(n).
	

	\fn iterator ArrayList::erase(const iterator&it)
	\brief Erases the element at the specified iterator position from the list (also deletes the object)
	\param it Iterator of the element to erase
	\return Iterator pointing to the object immediately following the erased element
	
	Erases the iterator-target from the list. This operation may de-validate certain or all
	iterators hooked to the list.
	Operates in O(1).
	
	\fn iterator	ArrayList::erase(const iterator&from, const iterator&to)
	\brief Erases the specified iterator interval from the list (also deletes all objects within)
	\param from Iterator pointing to the first element to erase
	\param to Iterator pointing one past the last element to erase
	\return Iterator pointing to the object immediately after the erased element(s)
	
	Erases all elements in the range [\a from, \a to) ( \a to is not erased). The method may fail or cause undesired
	results if \a to is no successor of \a from. The operation may de-validate certain or all iterators
	hooked to the list. Operates in O(m*n) (m = number of elements being erased).
	
	\fn bool ArrayList::erase(Type*element)
	\brief Erases the specified element from the list (also deletes the object)
	\param element Element to erase
	\return true on success, false if the specified element was not found
	
	
	\fn void ArrayList::set(UINT32 index, Type*element)
	\brief Replaces the element at the specified index
	\param index Index of the element that should be replaced
	\param element Pointer to use as replacement
	
	set() changes the element specified by \a index to \a element. No operation is performed if \a index is invalid.
	Operates in O(1) or O(n) depending on the Fill-state of the list.
	
	\fn void ArrayList::swap(UINT32 index0, UINT32 index1)
	\brief Swaps two elements in the list
	\param index0 First index
	\param index1 Second index
	
	Swaps two list entries. The method fails if either \a index0 or \a index1 are
	invalid or \a index0 is identical to \a index1.
	Operates in O(1) or O(n) depending on the Fill-state of the list.
	

	\fn void ArrayList::swap(ArrayList<C,Size>&other)
	\brief Swaps the content of the local list with the remote list
	\param other List to swap contents with

	Exchanges the contents of the local list with the given list. The given list
	must be of the same list- and data-type as the local list.
	
	\fn void ArrayList::revert()
	\brief Reverts the order of elements within the list

	\fn Type* ArrayList::get(UINT32 index)
	\brief Retrieves an element from the list (optimized)
	\param index Index of the element to retrieve (0 = first element)
	\return Pointer to the respective object or NULL if the index is invalid
	
	The get()-method retrieves the element specified by \a index
	Operates in O(1) or O(n) depending on the present state of the list.
	
	\fn Type* ArrayList::getConst(UINT32 index)			const
	\brief Retrieves an element from the list (const)
	\param index Index of the element to retrieve (0 = first element)
	\return Pointer to the respective object or NULL if the index is invalid
	
	The getConst()-method retrieves the element specified by \a index. Identical to the get() method invoked on a const list object. Deprecated.
	Operates in O(1) or O(n) depending on the present state of the list.

	\fn Data& ArrayList::getReference(UINT32 index)		const
	\brief Retrieves the reference to an element from the list (const)
	\param index Index of the element to retrieve the reference from (0 = first element)
	\return Reference to the pointer of the respective object
	
	The list may throw an error if the specified index is invalid.
	The runtime-behaviour is identical to that of getConst()
	
	\fn iterator ArrayList::getIterator(UINT32 index)			const
	\brief Retrieves an iterator pointing to the specified list position
	\param index Index of the object the iterator should point to
	\return Iterator to the specified position
	
	The returned iterator is undefined if the specified index is invalid.
	Operates in O(1)

	\fn UINT32 ArrayList::getIndexOf(const Type*element)	const
	\brief Retrieves the index of the specified element
	\param element Element to retrieve the position of
	\return index of the respective object+1 or 0 if the object could not be found
	
	Operates in O(n)
	
	\fn Type* ArrayList::first() const
	\brief Retrieves the first element in the list
	\return Pointer to the first object in the list or NULL if the list is empty
	
	\fn Type* ArrayList::last() const
	\brief Retrieves the last element in the list
	\return Pointer to the last object in the list or NULL if the list is empty
	
	\fn iterator ArrayList::begin() const
	\brief Retrieves an iterator pointing to the first object in the list
	\return Iterator pointing to the first element
	
	\fn iterator ArrayList::end() const
	\brief Retrieves an iterator pointing one past the last object in the list
	\return Iterator pointing one past the last element
	
	\fn Type* ArrayList::operator[](size_t index)
	\brief Retrieves an element from the list (optimized)
	\param index Index of the element to return (0 = first element)
	\return Pointer to the respective object or NULL if the index is invalid
	
	\fn UINT32 ArrayList::operator()(const Type*element)			const
	\brief Retrieves the index of the specified element
	\param element Element to retrieve the position of
	\return index of the respective object+1 or 0 if the object could not be found
	
	\fn ArrayList::operator				size_t()						const

	This implicit type conversation can be quite handy. Assuming one has a list
	called 'objects', the following operations are valid:<br><ul>
	<li>if (objects) //if objects contains at least one object</li>
	<li>for (size_t i = 0; i < objects; i++) //iterate through all elements</li></ul>

	\fn UINT32 ArrayList::count()							const
	\brief Returns the number of elements in the list
	\return Number of elements in the list
	
	\fn size_t ArrayList::totalSize()						const
	\brief Calculates the size in bytes that the list consumes
	\returns Size in bytes (not counting the size of the elements)

	\fn void ArrayList::reset()
	\brief Resets the 'each' cursor to the first element in the list
	
	Must be called before calling each()!
	
	\fn Type* ArrayList::each()
	\brief Retrieves the current 'each' element of the list and advances the cursor by one element
	\return Current 'each' element or NULL if all elements have been processed
	
	each() sequentially returns the elements of the list beginning with the first
	and ending with the last element. The method returns NULL if the cursor reached
	the end of the list.<br />
	reset() must be called to initialize each() or the result will be undefined/corrupted.<br />
	Example:<br />
	<div>list.reset();<br />while ( \p Type *element = list.each())<br />(...)</div>
	
	\fn Type* ArrayList::drop()
	\brief Drops the element at the current 'each' cursor position from the list (without deleting it)
	\return Element at the current 'each' cursor position
	
	Drops the element returned by the last each()-call from the list.
	The next each()-call will return the element following the dropped one.
	Operates in O(n).

	\fn void ArrayList::erase()
	\brief Erases the element at the current 'each' cursor position from the list (also deletes the object)

	Erases the element returned by the last each()-call from the list.
	The next each()-call will return the element following the erased one.
	Operates in O(n).

	\fn Type* ArrayList::include()
	\brief Inserts a new element at the current 'each' cursor position
	\return Pointer to a new object which has been inserted into the list
	
	Inserts a new  \p Type -object as the immediate predecessor of the element
	returned by the last each()-call. The next each()-call will return the
	element following the current with neither executing the new element
	nor re-executing the current.

	\fn void ArrayList::include(Type*element)
	\brief Inserts the specified element at the current 'each' cursor position

	Inserts \a element as the immediate predecessor of the element returned by
	the last each()-call. The next each()-call will return the element following
	the current with neither executing the new element
	nor re-executing the current.

*/


/*!
	\brief Array-list reference pointer container
	
	Identical to ArrayList except that it does not erase its elements on destruction.
*/
template <class C, size_t Size> class ReferenceArrayList:public ArrayList<C,Size>
{
public:
		typedef ArrayList<C,Size>	List,Super;
		typedef C					Type;
		typedef C*					Data;
		
									ReferenceArrayList();
template <size_t RSize>				ReferenceArrayList(const ArrayList<C,RSize>&other);
virtual								~ReferenceArrayList();
template <size_t RSize>
INLINE		ArrayList<C,Size>&		operator=(const ArrayList<C,RSize>&other);
INLINE		Type*					operator[](size_t index);
INLINE		const Type*				operator[](size_t index)		const;
INLINE		UINT32					operator()(const Type*)			const;
};


#include "arraylist.tpl.h"
}

#endif
