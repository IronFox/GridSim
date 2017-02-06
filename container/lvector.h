#ifndef lvectorH
#define lvectorH

#include "list.h"



/*************************************************************************************

This file is part of the DeltaWorks-Foundation or an immediate derivative core module.

*************************************************************************************/

namespace List
{
	/*!
		\brief Optimized vector list

		The list stores object pointers in one large array. The size of the array is altered (halved or doubled)	if the current Fill state leaves the working range.
	*/
	template <class C> class Vector
	{
	private:

	#if DEBUG_LEVEL >= 2
				size_t					list_id;
	#endif

				size_t					cells;
				C						**root,**Fill,**imark,**cursor;
		
	INLINE		void					decToZero();
	INLINE		void					inc();
	INLINE		void					dec();

	public:
		//26 E:\include\list\lvector.h `typedef class KeyEntry Vector<KeyEntry>::Type' is inaccessible

	typedef		Vector<C>				Self;
	typedef		C						Type;		//!< Contained object type
	typedef		C*						Data;		//!< Contained object pointer type
	typedef		Data*					iterator;		//!< STL container iterator type
	typedef		const C**				const_iterator;	//!< STL container const iterator type
				CONTAINER_TYPES
			

										Vector();
										Vector(const Vector<C>&other);
	virtual								~Vector();
	INLINE		void					adoptData(Vector<C>&other);
	INLINE		Vector<C>&				operator=(const Vector<C>&other);


	INLINE		void					flush();
	INLINE		void					clear();

	INLINE		void					push_front(Type*element);
	INLINE		void					push_back(Type*element);
	INLINE		Type*					append(Type*element);
	INLINE		Type*					append();
	template <class List>
	INLINE		void					import(List&list);
	template <class List>
	INLINE		void					importAndFlush(List&list);
	INLINE		Type*					insert(index_t index, Type*element);
	INLINE		Type*					insert(index_t index);
	INLINE		iterator				insert(const iterator&it, Type*element);
	INLINE		Type*					drop(index_t index);
	INLINE		iterator				drop(const iterator&it);
	INLINE		Type*					drop(Type*element);
	INLINE		bool					erase(index_t index);
	INLINE		iterator				erase(const iterator&it);
	INLINE		iterator				erase(const iterator&from, const iterator&to);
	INLINE		bool					erase(Type*element);
	INLINE		void					set(index_t index, Type*element);
	INLINE		void					swap(index_t index0, index_t index1);
	INLINE		void					swap(Vector<C>&other);
	friend		void					swap(Self&a, Self&b)	{a.swap(b);}
	INLINE		void					revert();

	INLINE		Type*					get(index_t index);
	INLINE		const Type*				get(index_t index)					const;
	INLINE		const Type*				getConst(index_t index)			const;
	INLINE		Data&					getReference(index_t index);
	INLINE		iterator				getIterator(index_t index);
	INLINE		const_iterator			getIterator(index_t index)			const;
	INLINE		size_t					getIndexOf(const Type*)			const;
	INLINE		Type*					first();
	INLINE		const Type*				first()							const;
	INLINE		Type*					last();
	INLINE		const Type*				last()							const;
	INLINE		iterator				begin();
	INLINE		const_iterator			begin()							const;
	INLINE		iterator				end();
	INLINE		const_iterator			end()							const;
	INLINE		Type*					operator[](index_t index);
	INLINE		const Type*				operator[](index_t index)		const;
	INLINE		index_t					operator()(const Type*)			const;
	INLINE		operator				size_t()						const;		//!< Implict cast to size_t	\return Number of elements in the list

	INLINE		bool					IsEmpty()						const;
	INLINE		bool					IsNotEmpty()					const;
	INLINE		size_t					count()							const;
	INLINE		size_t					totalSize()						const;
	INLINE		bool					discrete()						const;		//!< Verifies discrete list state \return true if no element is listed multiple times

	INLINE		void					reset();
	INLINE		Type*					each();
	INLINE		Type*					drop();
	INLINE		void					erase();
	INLINE		Type*					include();
	INLINE		void					include(Type*element);


				CONTAINER_MMETHODS
	};


	/*!
		\brief Optimized vector list containing non proprietary object pointers

		The ReferenceVector behaves identical to Vector except for its pointer management.
		On destruction contained elements will be flushed, not deleted. ReferenceVector should be used to store proprietary elements
		of another Vector (or any other) list.
	*/
	template <class C> class ReferenceVector:public Vector<C>
	{
	public:
			typedef Vector<C>			Super;	//!< Base list type
			typedef C					Type;	//!< Base object type
			typedef C*					Data;	//!< Base pointer type
			//USE_LIST
			//USE_CONTAINER

										ReferenceVector();
										ReferenceVector(const Vector<C>&other);
	virtual								~ReferenceVector();
				Vector<C>&				operator=(const Vector<C>&);
	INLINE		Type*					operator[](index_t index);
	INLINE		const Type*				operator[](index_t index)		const;
	INLINE		size_t					operator()(const Type*)			const;
	};






	/*

	template <class C> class BufferedVector
	{
	protected:
			size_t	array_exp,
						elements,len;
			C			**field,**back_field,**cursor;

			void		resize(size_t size);

	public:

			CONTAINER_TYPES

	typedef C			Type;
	typedef C*			Data;
	typedef C**			iterator;

						BufferedVector();
	virtual			~BufferedVector();
	inline	void		reset();
	inline	C*			each();
	inline	size_t	count()							const;
	inline	C*			getElement(index_t index);
	inline	const C*	getElementConst(index_t index) const;
			C*			dropCurrent();
			void		eraseCurrent();
			void		eraseElement(index_t index);
			C*			dropElement(index_t index);
			void		dropElements();
			void		eraseElements();

			C*			insertElement(index_t index,C*element);
			C*			insertElement(index_t index);
			C*			appendElement(C*element);
			C*			appendElement();
			size_t	getIndexOf(C*element);
			void		setElement(index_t index, C*element);
			C*			insertBeforeCurrent();
			C*			insertBehindCurrent();

	inline	iterator	begin();
	inline	iterator	end();
	inline	iterator	erase(const iterator&it);
	inline	iterator	erase(const iterator&from, const iterator&to);
	inline	iterator	drop(const iterator&it);

	inline	C*			operator[](index_t index);
	inline	size_t	operator[](const C*element);

			CONTAINER_METHODS
	};

	*/





	/*!
		\fn void		Vector::flush()
		\brief	Clears the local list without deleting the contained elements.

		\fn void Vector::clear()
		\brief Deletes all contained elements and clears the local list.

		\fn void Vector::push_front(Type*element)
		\brief Inserts the specified element before the first element in the list
		\param element Element to insert

		push_front operates in O(n).

		\fn void Vector::push_back(Type*element)
		\brief Appends the specified element to the end of the list.
		\param element Element to insert

		push_back operates in O(1).

		\fn Type*	Vector::append(Type*element)
		\brief Appends the specified element to the end of the list.
		\param element Element to append

		append operates in O(1).


		\fn Type* Vector::append()
		\brief Appends a new element to the end of the list.
		\return Pointer to a new object of type \b Type which has been appended to the list

		\fn void Vector::import(List&list)
		\brief Imports all elements from another STL conform list
		\param list List to import all elements from (must be STL container conform)

		\fn void Vector::importAndFlush(List&list)
		\brief Imports all elements from another list and flushes that list
		\param list List to import all elements from and flush afterwards

		\fn Type* Vector::insert(index_t index, Type*element)
		\brief Inserts the specified element before the specified index
		\param index Index to insert before (0 = first element)
		\param element Element to insert
		\return element

		Inserts \a element before the specified index. If \a index is equal or greater count() then
		append() will be invoked instead.
		insert() operates in O(n).


		\fn Type* Vector::insert(index_t index)
		\brief Inserts a new element before the specified index
		\param index Index to insert before
		\return Pointer to a new element of type \b Type which has been inserted into the list

		Inserts a new <type>-object before the specified \a index (providing 0 as \a index
		results in inserting before the first element). If \a index is equal or greater count()
		an insert()-call results in an append()-call.
		insert() operates in O(n).

		\fn iterator	Vector::insert(const iterator&it, Type*element)
		\brief Inserts the specified element at the specified iterator position
		\param it Iterator position
		\param element Element to insert
		\return Iterator that points to the newly inserted element

		insert() inserts \a element as the immediate predecessor of the given iterator's
		target.

		\fn Type* Vector::drop(index_t index)
		\brief Drops the element at the specified index from the list (without deleting it)
		\param index Index of the element to drop
		\return pointer to the dropped element or NULL if \b index is invalid
		(may also return NULL if the specified element contained NULL).

		Drops the specified element from the list without deleting it.
		Operates in O(n).

		\fn iterator Vector::drop(const iterator&it)
		\brief Drops the element at the specified iterator position from the list (without deleting it)
		\param it Valid iterator targeting the immediate successor of the dropped element.
		\return iterator pointing to the object immediately after the dropped element

		Drops the iterator-target from the list. This operation may de-validate certain
		or all iterators hooked to the list. Operates in O(n).

		\fn Type* Vector::drop(Type*element)
		\brief Drops the specified element from the list (without deleting it)
		\param element Element to drop
		\return \b element on success, NULL otherwise

		\fn bool Vector::erase(index_t index)
		\brief Erases the element at the specified index from the list (also deletes the object)
		\param index Index of the element to erase
		\return true on success

		Erases the element specified by index. No operation is performed if \a index is invalid.
		Operates in O(n).


		\fn iterator Vector::erase(const iterator&it)
		\brief Erases the element at the specified iterator position from the list (also deletes the object)
		\param it Iterator of the element to erase
		\return Iterator pointing to the object immediately following the erased element

		Erases the iterator-target from the list. This operation may de-validate certain or all
		iterators hooked to the list.
		Operates in O(n).

		\fn iterator	Vector::erase(const iterator&from, const iterator&to)
		\brief Erases the specified iterator interval from the list (also deletes all objects within)
		\param from Iterator pointing to the first element to erase
		\param to Iterator pointing one past the last element to erase
		\return Iterator pointing to the object immediately after the erased element(s)

		Erases all elements in the range [\a from, \a to) ( \a to is not erased). The method may fail or cause undesired
		results if \a to is no successor of \a from. The operation may de-validate certain or all iterators
		hooked to the list. Operates in O(m*n) (m = number of elements being erased).

		\fn bool Vector::erase(Type*element)
		\brief Erases the specified element from the list (also deletes the object)
		\param element Element to erase
		\return true on success, false if the specified element was not found

		\fn void Vector::set(index_t index, Type*element)
		\brief Replaces the element at the specified index
		\param index Index of the element that should be replaced
		\param element Pointer to use as replacement

		set() changes the element specified by \a index to \a element. No operation is performed if \a index is invalid.
		Operates in O(1).

		\fn void Vector::swap(index_t index0, index_t index1)
		\brief Swaps two elements in the list
		\param index0 First index
		\param index1 Second index

		Swaps two list entries. The method fails if either \a index0 or \a index1 are
		invalid or \a index0 is identical to \a index1.
		Operates in O(1).


		\fn void Vector::swap(Vector< C >&other)
		\brief Swaps the content of the local list with the remote list
		\param other List to swap contents with

		Exchanges the contents of the local list with the given list. The given list
		must be of the same list- and data-type as the local list.

		\fn void Vector::revert()
		\brief Reverts the order of elements within the list

		\fn Type* Vector::get(index_t index)
		\brief Retrieves an element from the list (optimized)
		\param index Index of the element to retrieve (0 = first element)
		\return Pointer to the respective object or NULL if the index is invalid

		The get()-method retrieves the element specified by \a index
		Operates in O(1).

		\fn Type* Vector::getConst(index_t index)			const
		\brief Retrieves an element from the list (const)
		\param index Index of the element to retrieve (0 = first element)
		\return Pointer to the respective object or NULL if the index is invalid

		The getConst()-method retrieves the element specified by \a index. Identical to the get() method invoked on a const list object. Deprecated.
		Operates in O(1).

		\fn Data& Vector::getReference(index_t index)		const
		\brief Retrieves the reference to an element from the list (const)
		\param index Index of the element to retrieve the reference from (0 = first element)
		\return Reference to the pointer of the respective object

		The list may throw an error if the specified index is invalid.
		The runtime-behaviour is identical to that of getConst()

		\fn iterator Vector::getIterator(index_t index)
		\brief Retrieves an iterator pointing to the specified list position
		\param index Index of the object the iterator should point to
		\return Iterator to the specified position

		The returned iterator is undefined if the specified index is invalid.
		Operates in O(1)

		\fn size_t Vector::getIndexOf(const Type*element)	const
		\brief Retrieves the index of the specified element
		\param element Element to retrieve the position of
		\return index of the respective object+1 or 0 if the object could not be found

		Operates in O(n)

		\fn Type* Vector::first()
		\brief Retrieves the first element in the list
		\return Pointer to the first object in the list or NULL if the list is empty

		\fn Type* Vector::last()
		\brief Retrieves the last element in the list
		\return Pointer to the last object in the list or NULL if the list is empty

		\fn iterator Vector::begin()
		\brief Retrieves an iterator pointing to the first object in the list
		\return Iterator pointing to the first element

		\fn iterator Vector::end()
		\brief Retrieves an iterator pointing one past the last object in the list
		\return Iterator pointing one past the last element

		\fn Type* Vector::operator[](index_t index)
		\brief Retrieves an element from the list (optimized)
		\param index Index of the element to return (0 = first element)
		\return Pointer to the respective object or NULL if the index is invalid

		\fn size_t Vector::operator()(const Type*)			const
		\brief Retrieves the index of the specified element
		\param element Element to retrieve the position of
		\return index of the respective object+1 or 0 if the object could not be found

		\fn Vector::operator				size_t()						const

		This implicit type conversation can be quite handy. Assuming one has a list
		called 'objects', the following operations are valid:<br><ul>
		<li>if (objects) //if objects contains at least one object</li>
		<li>for (size_t i = 0; i < objects; i++) //iterate through all elements</li></ul>

		\fn size_t Vector::count()							const
		\brief Returns the number of elements in the list
		\return Number of elements in the list

		\fn size_t Vector::totalSize()						const
		\brief Calculates the size in bytes that the list consumes
		\returns Size in bytes (not counting the size of the elements)

		\fn void Vector::reset()
		\brief Resets the 'each' cursor to the first element in the list

		Must be called before calling each()!

		\fn Type* Vector::each()
		\brief Retrieves the current 'each' element of the list and advances the cursor by one element
		\return Current 'each' element or NULL if all elements have been processed

		each() sequentially returns the elements of the list beginning with the first
		and ending with the last element. The method returns NULL if the cursor reached
		the end of the list.<br />
		reset() must be called to initialize each() or the result will be undefined/corrupted.<br />
		Example:<br />
		<div>list.reset();<br />while (<type>*element = list.each())<br />(...)</div>

		\fn Type* Vector::drop()
		\brief Drops the element at the current 'each' cursor position from the list (without deleting it)
		\return Element at the current 'each' cursor position

		Drops the element returned by the last each()-call from the list.
		The next each()-call will return the element following the dropped one.
		Operates in O(n).

		\fn void Vector::erase()
		\brief Erases the element at the current 'each' cursor position from the list (also deletes the object)

		Erases the element returned by the last each()-call from the list.
		The next each()-call will return the element following the erased one.
		Operates in O(n).

		\fn Type* Vector::include()
		\brief Inserts a new element at the current 'each' cursor position
		\return Pointer to a new object which has been inserted into the list

		Inserts a new <type>-object as the immediate predecessor of the element
		returned by the last each()-call. The next each()-call will return the
		element following the current with neither executing the new element
		nor re-executing the current.

		\fn void Vector::include(Type*element)
		\brief Inserts the specified element at the current 'each' cursor position

		Inserts \a element as the immediate predecessor of the element returned by
		the last each()-call. The next each()-call will return the element following
		the current with neither executing the new element
		nor re-executing the current.

	*/


	#include "lvector.tpl.h"
}
#endif
