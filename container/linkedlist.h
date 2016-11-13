#ifndef linkedlistH
#define linkedlistH

/******************************************************************

Implementation of the LinkedList-class.

******************************************************************/


#include "dynamic_elements.h"
#include "list.h"


template <class C> class LinkedList;

/*!
	\brief Bdirectional linked pointer carrier
	
	Chains of linked pointer carriers form the element string of linked lists.
	Each carrier is linked to its succeeding and preceding neighbor.

*/
template <class C> class LinkedCarrier
{
private:
		LinkedCarrier<C>	*next,           //!< Link to next carrier
							*prev;           //!< Link to previous carrier
		C					*element;        //!< Link to element

		friend class LinkedList<C>;
		
							LinkedCarrier()	{}
							~LinkedCarrier()	{}
		void				swap(LinkedCarrier<C>&other)
							{
								swp(next,other.next);
								swp(prev,other.prev);
								swp(element,other.element);
							}
public:
		C*					getElement()	//! Retrieve carried element	\return carried element
							{
								return element;
							}
		const C*			getElement()	const	//! Const version of getElement()	\return carried element
							{
								return element;
							}
		void				setElement(C*el)	//! Update carried element. The previously carried element is \b not deleted. \param el Element to carry instead of the currently carried one.
							{
								element = el;
							}
		LinkedCarrier<C>*	getNext()
							{
								return next;
							}
		const LinkedCarrier<C>*	getNext()	const
							{
								return next;
							}
		LinkedCarrier<C>*	getPrev()
							{
								return prev;
							}
		const LinkedCarrier<C>*	getPrev()	const
							{
								return prev;
							}
};


/*!
	\brief Bidirectional linked list
	
	The linked list stores elements in one long chain of bedirectionally linked carriers. If located items can be erased or inserted in O(1).
	Sequential index lookup is optimized via an internal carrier cursor, non sequential index lookup is inherently slow.
*/
template <class C> class LinkedList
{
public:
		typedef LinkedCarrier<C>	Carrier;
		
protected:
		Carrier				pivot,	//!< Root element that is located between the last and the first element (the linked list is actually a ring). The pivot element is never actually part of the list and holds no data
							*seeker,	//!< Seeker element used to navigate forward and backward through the list. Only used by non-const access methods. May be NULL indicating recent changes to the list have invalidated the seeker's location
							*cursor,
							*carrier;
		size_t				elements,seeker_index;

inline	bool				seek(size_t index);
inline	Carrier*			drop(Carrier*carrier);

public:
		typedef C			Type;
		typedef C*			Data;
		
		CONTAINER_TYPES
		typedef LinkedIterator<Carrier,C*>				iterator;
		typedef LinkedIterator<const Carrier,const C*>	const_iterator;

									LinkedList();
									LinkedList(const LinkedList<C>&other);
virtual								~LinkedList();
INLINE		LinkedList<C>&			operator=(const LinkedList<C>&other);
			void					adoptData(LinkedList<C>&other);

INLINE		void					flush();
INLINE		void					clear();

INLINE		void					push_front(Type*element);
INLINE		void					push_back(Type*element);
INLINE		Type*					append(Type*element);
INLINE		Type*					append();
INLINE		void					import(LinkedList<C>&vector);
template <class List>
INLINE		void					import(List&list);
template <class List>
INLINE		void					importAndFlush(List&list);
INLINE		Type*					insert(size_t index, Type*element);
INLINE		Type*					insert(size_t index);
INLINE		iterator				insert(const iterator&it, Type*element);
INLINE		Type*					drop(size_t index);
INLINE		iterator				drop(const iterator&it);
INLINE		Type*					drop(Type*element);
INLINE		bool					erase(size_t index);
INLINE		iterator				erase(const iterator&it);
INLINE		iterator				erase(const iterator&from, const iterator&to);
INLINE		bool					erase(Type*element);
INLINE		void					set(size_t index, Type*element);
INLINE		void					swap(size_t index0, size_t index1);
INLINE		void					swap(LinkedList<C>&other);
INLINE		void					revert();
INLINE		C*						dropFirst();
INLINE		C*						dropLast();
INLINE		C*						insertBeforeCarrier(Carrier*carrier, C*element);	//!< Inserts the specified element before the specified carrier. \param carrier Carrier to insert before \param element Element to insert \return Pointer to element
INLINE		C*						insertBehindCarrier(Carrier*carrier, C*element);	//!< Inserts the specified element behind the specified carrier. \param carrier Carrier to insert behind \param element Element to insert \return Pointer to element
INLINE		C*						insertBeforeCarrier(Carrier*carrier);	//!< Inserts a new element before the specified carrier. \param carrier Carrier to insert before \return Pointer to the new element
INLINE		C*						insertBehindCarrier(Carrier*carrier);	//!< Inserts a new element behind the specified carrier. \param carrier Carrier to insert behind \return Pointer to the new element
INLINE		void					eraseCarrier(Carrier*c);
INLINE		C*						dropCarrier(Carrier*c);
static INLINE	void				relocate(Carrier*carrier, Carrier*before);	//!< Relocates \b carrier so that it preceeds \b before. \param carrier Carrier to relocate \param before Carrier to insert the carrier before.

INLINE		Type*					get(size_t index);
INLINE		const Type*				get(size_t index)					const;
INLINE		const Type*				getConst(size_t index)			const;
INLINE		Data&					getReference(size_t index);
INLINE		iterator				getIterator(size_t index);
INLINE		const_iterator			getIterator(size_t index)			const;
INLINE		size_t					getIndexOf(const Type*)			const;
INLINE		Carrier*				getCarrier();
INLINE		const Carrier*			getCarrier()					const;
INLINE		Carrier*				getCarrier(size_t index);
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
INLINE		size_t					operator()(const Type*)			const;
INLINE		operator				size_t()						const;

INLINE		size_t					count()							const;
INLINE		size_t					totalSize()						const;

INLINE		void					reset();
INLINE		Type*					each();
INLINE		Carrier*				eachCarrier();
INLINE		Type*					drop();
INLINE		void					erase();
INLINE		Type*					include();
INLINE		void					include(Type*element);


			void					check();

			CONTAINER_MMETHODS
};


/*!
	\brief Bidirectional linked reference list
	
	Identical to LinkedList except that it does not erase its elements on destruction.
*/
template <class C> class ReferenceLinkedList:public	LinkedList<C>
{
public:
		typedef LinkedList<C>		Super;
		typedef C					Type;
		typedef C*					Data;
		//USE_LIST
		//USE_CONTAINER

									~ReferenceLinkedList();
inline	C*							operator[](size_t Index);
inline	const C*					operator[](size_t Index)	const;
inline	size_t						operator()(const C*element) const;
INLINE	LinkedList<C>&				operator=(const LinkedList<C>&list);
};



/*!
	\fn void		LinkedList::flush()
	\brief	Clears the local list without deleting the contained elements.

	\fn void LinkedList::clear()
	\brief Deletes all contained elements and clears the local list.

	\fn void LinkedList::push_front(Type*element)
	\brief Inserts the specified element before the first element in the list
	\param element Element to insert

	push_front operates in O(1).

	\fn void LinkedList::push_back(Type*element)
	\brief Appends the specified element to the end of the list.
	\param element Element to insert

	push_back operates in O(1).

	\fn Type*	LinkedList::append(Type*element)
	\brief Appends the specified element to the end of the list.
	\param element Element to append

	append operates in O(1).


	\fn Type* LinkedList::append()
	\brief Appends a new element to the end of the list.
	\return Pointer to a new object of type \b Type which has been appended to the list

	\fn void LinkedList::import(LinkedList< C >&other)
	\brief Imports all elements from another array list
	\param other List to import all elements from

	\fn void LinkedList::import(List&list)
	\brief Imports all elements from another STL conform list
	\param list List to import all elements from (must be STL container conform)

	\fn void LinkedList::importAndFlush(List&list)
	\brief Imports all elements from another list and flushes that list
	\param list List to import all elements from and flush afterwards

	\fn Type* LinkedList::insert(size_t index, Type*element)
	\brief Inserts the specified element before the specified index
	\param index Index to insert before (0 = first element)
	\param element Element to insert
	\return element

	Inserts \a element before the specified index. If \a index is equal or greater count() then
	append() will be invoked instead.
	insert() operates in O(n).


	\fn Type* LinkedList::insert(size_t index)
	\brief Inserts a new element before the specified index
	\param index Index to insert before
	\return Pointer to a new element of type \b Type which has been inserted into the list

	Inserts a new <type>-object before the specified \a index (providing 0 as \a index
	results in inserting before the first element). If \a index is equal or greater count()
	an insert()-call results in an append()-call.
	insert() operates in O(n).

	\fn iterator	LinkedList::insert(const iterator&it, Type*element)
	\brief Inserts the specified element at the specified iterator position
	\param it Iterator position
	\param element Element to insert
	\return Iterator that points to the newly inserted element

	insert() inserts \a element as the immediate predecessor of the given iterator's
	target.

	\fn Type* LinkedList::drop(size_t index)
	\brief Drops the element at the specified index from the list (without deleting it)
	\param index Index of the element to drop
	\return pointer to the dropped element or NULL if \b index is invalid
	(may also return NULL if the specified element contained NULL).

	Drops the specified element from the list without deleting it.
	Operates in O(n).

	\fn iterator LinkedList::drop(const iterator&it)
	\brief Drops the element at the specified iterator position from the list (without deleting it)
	\param it Valid iterator targeting the immediate successor of the dropped element.
	\return iterator pointing to the object immediately after the dropped element

	Drops the iterator-target from the list. This operation may de-validate certain
	or all iterators hooked to the list. Operates in O(1).

	\fn Type* LinkedList::drop(Type*element)
	\brief Drops the specified element from the list (without deleting it)
	\param element Element to drop
	\return \b element on success, NULL otherwise
	
	\fn Type* LinkedList::dropLast()
	\brief Drops the last element of the list.
	\return Dropped element or NULL if the list was empty.

	\fn Type* LinkedList::dropFirst()
	\brief Drops the first element of the list.
	\return Dropped element or NULL if the list was empty.

	\fn void LinkedList::eraseCarrier(Carrier*c)
	\brief Erases the specified carrier including the element it carries.
	\param c Carrier of the element to erase

	Operates in O(1).

	\fn C* LinkedList::dropCarrier(Carrier*c)
	\brief Erases the specified carrier without deleting the data it carries.
	\param c Carrier of the element to drop
	\return Pointer to the dropped element

	Operates in O(1).

	\fn bool LinkedList::erase(size_t index)
	\brief Erases the element at the specified index from the list (also deletes the object)
	\param index Index of the element to erase
	\return true on success, false if the specified element was not found

	Erases the element specified by index. No operation is performed if \a index is invalid.
	Operates in O(n).


	\fn iterator LinkedList::erase(const iterator&it)
	\brief Erases the element at the specified iterator position from the list (also deletes the object)
	\param it Iterator of the element to erase
	\return Iterator pointing to the object immediately following the erased element

	Erases the iterator-target from the list. This operation may de-validate certain or all
	iterators hooked to the list.
	Operates in O(1).

	\fn iterator	LinkedList::erase(const iterator&from, const iterator&to)
	\brief Erases the specified iterator interval from the list (also deletes all objects within)
	\param from Iterator pointing to the first element to erase
	\param to Iterator pointing one past the last element to erase
	\return Iterator pointing to the object immediately after the erased element(s)

	Erases all elements in the range [\a from, \a to) ( \a to is not erased). The method may fail or cause undesired
	results if \a to is no successor of \a from. The operation may de-validate certain or all iterators
	hooked to the list. Operates in O(m) (m = number of elements being erased).

	\fn bool LinkedList::erase(Type*element)
	\brief Erases the specified element from the list (also deletes the object)
	\param element Element to erase
	\return true on success, false if the specified element was not found


	\fn void LinkedList::set(size_t index, Type*element)
	\brief Replaces the element at the specified index
	\param index Index of the element that should be replaced
	\param element Pointer to use as replacement

	set() changes the element specified by \a index to \a element. No operation is performed if \a index is invalid.
	Operates in O(n).

	\fn void LinkedList::swap(size_t index0, size_t index1)
	\brief Swaps two elements in the list
	\param index0 First index
	\param index1 Second index

	Swaps two list entries. The method fails if either \a index0 or \a index1 are
	invalid or \a index0 is identical to \a index1.
	Operates in O(n).


	\fn void LinkedList::swap(LinkedList< C >&other)
	\brief Swaps the content of the local list with the remote list
	\param other List to swap contents with
	
	Exchanges the contents of the local list with the given list. The given list
	must be of the same list- and data-type as the local list.

	\fn void LinkedList::revert()
	\brief Reverts the order of elements within the list

	\fn Type* LinkedList::get(size_t index)
	\brief Retrieves an element from the list (optimized)
	\param index Index of the element to retrieve (0 = first element)
	\return Pointer to the respective object or NULL if the index is invalid

	The get()-method retrieves the element specified by \a index
	Operates in O(n).

	\fn const Type* LinkedList::getConst(size_t index)			const
	\brief Retrieves an element from the list (const)
	\param index Index of the element to retrieve (0 = first element)
	\return Pointer to the respective object or NULL if the index is invalid

	The getConst()-method retrieves the element specified by \a index. Identical to the get() method invoked on a const list object. Deprecated.
	Operates in O(n).

	\fn Data& LinkedList::getReference(size_t index)
	\brief Retrieves a reference to the specified element (const)
	\param index Index of the element to retrieve the reference of (0 = first element)
	\return Reference to the pointer of the respective object

	The list may throw an error if the specified index is invalid.
	The runtime-behaviour is identical to that of getConst()

	\fn iterator LinkedList::getIterator(size_t index)		const
	\brief Retrieves an iterator pointing to the specified list position
	\param index Index of the object the iterator should point to
	\return Iterator to the specified position

	The returned iterator is undefined if the specified index is invalid.
	Operates in O(n)

	\fn LinkedList::Carrier*		LinkedList::getCarrier()					const
	\brief Queries carrier of last inserted element
	\return Pointer to carrier of last inserted element
	
	The method returns a pointer to the new carrier containing the element
	of the last call of: <br />
	<ul>
	<li>append([element])</li>
	<li>push_back(element)</li>
	<li>push_front(element)</li>
	<li>insert(index[,element])</li>
	<li>insert(iterator[,element])</li>
	<li>include([element])</li>
	<li>insertBeforeCarrier(carrier[,element])</li>
	<li>insertBehindCarrier(carrier[,element])</li></ul><br />
	The result is undefined if no carrier was created or the created carrier deleted in the meantime.
	
	\fn LinkedList::Carrier*		LinkedList::getCarrier(size_t index)
	\brief Queries carrier of the specified element index
	\return Pointer to the carrier holding the specified element or NULL if the index is invalid
	

	\fn size_t LinkedList::getIndexOf(const Type*element)	const
	\brief Retrieves the index of the specified element
	\param element Element to retrieve the position of
	\return index of the respective object+1 or 0 if the object could not be found

	Operates in O(n)

	\fn Type* LinkedList::first()
	\brief Retrieves the first element in the list
	\return Pointer to the first object in the list or NULL if the list is empty

	\fn Type* LinkedList::last()
	\brief Retrieves the last element in the list
	\return Pointer to the last object in the list or NULL if the list is empty

	\fn iterator LinkedList::begin()
	\brief Retrieves an iterator pointing to the first object in the list
	\return Iterator pointing to the first element

	\fn iterator LinkedList::end()
	\brief Retrieves an iterator pointing one past the last object in the list
	\return Iterator pointing one past the last element

	\fn Type* LinkedList::operator[](size_t index)
	\brief Retrieves an element from the list (optimized)
	\param index Index of the element to return (0 = first element)
	\return Pointer to the respective object or NULL if the index is invalid

	\fn size_t LinkedList::operator()(const Type*)		const
	\brief Retrieves the index of the specified element
	\param element Element to retrieve the position of
	\return index of the respective object+1 or 0 if the object could not be found

	\fn LinkedList::operator				size_t()						const

	This implicit type conversation can be quite handy. Assuming one has a list
	called 'objects', the following operations are valid:<br><ul>
	<li>if (objects) //if objects contains at least one object</li>
	<li>for (size_t i = 0; i < objects; i++) //iterate through all elements</li></ul>

	\fn size_t LinkedList::count()						const
	\brief Returns the number of elements in the list
	\return Number of elements in the list

	\fn size_t LinkedList::totalSize()					const
	\brief Determines the current list size without its elements
	\return Size in bytes (not counting the size of the elements)

	\fn void LinkedList::reset()
	\brief Resets the 'each' cursor to the first element in the list

	Must be called before calling each()!

	\fn LinkedList::Carrier*		LinkedList::eachCarrier()
	\brief Retrieves the current 'each' element's carrier of the list and advances the cursor by one element
	\return pointer to the sequentially next carrier-object or NULL if all elements have been processed.

	Similar to each(). The method sequentially returns each carrier managed by the list, starting with the first
	and ending with the last carrier. The method may be slightly slower than each().
	reset() must be called to initialize eachCarrier() or the result will be undefined/corrupted.

	\fn Type* LinkedList::each()
	\brief Retrieves the current 'each' element of the list and advances the cursor by one element
	\return Current 'each' element or NULL if all elements have been processed

	each() sequentially returns the elements of the list beginning with the first
	and ending with the last element. The method returns NULL if the cursor reached
	the end of the list.<br />
	reset() must be called to initialize each() or the result will be undefined/corrupted.<br />
	Example:<br />
	<div>list.reset();<br />while (<type>*element = list.each())<br />(...)</div>
	

	\fn Type* LinkedList::drop()
	\brief Drops the element at the current 'each' cursor position from the list (without deleting it)
	\return Element at the current 'each' cursor position

	Drops the element returned by the last each()-call from the list.
	The next each()-call will return the element following the dropped one.
	Operates in O(1).

	\fn void LinkedList::erase()
	\brief Erases the element at the current 'each' cursor position from the list (also deletes the object)

	Erases the element returned by the last each()-call from the list.
	The next each()-call will return the element following the erased one.
	Operates in O(1).

	\fn Type* LinkedList::include()
	\brief Inserts a new element at the current 'each' cursor position
	\return Pointer to a new object which has been inserted into the list

	Inserts a new <type>-object as the immediate predecessor of the element
	returned by the last each()-call. The next each()-call will return the
	element following the current with neither executing the new element
	nor re-executing the current.

	\fn void LinkedList::include(Type*element)
	\brief Inserts the specified element at the current 'each' cursor position

	Inserts \a element as the immediate predecessor of the element returned by
	the last each()-call. The next each()-call will return the element following
	the current with neither executing the new element
	nor re-executing the current.

*/


#include "linkedlist.tpl.h"


#endif


