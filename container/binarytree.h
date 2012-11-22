#ifndef binarytreeH
#define binarytreeH

/******************************************************************

Implementation of the AVLTree-class.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include "list.h"
#include "sort_classes.h"

namespace List
{

	template <class, class> class BinaryTree;
	template <class, class> class BinaryNode;
	template <class, class> class BinaryIterator;
	template <class, class> class ConstBinaryIterator;


	template <class C, class Class> class BinaryIterator
	{
	public:
						typedef BinaryIterator<C,Class>	Iterator;
					
								BinaryNode<C,Class>	*node;

														BinaryIterator(BinaryNode<C,Class>*node);
						INLINE	Iterator&				operator++();
						INLINE	Iterator				operator++(int);
						INLINE	C*&						operator*();
						INLINE	bool					operator==(const Iterator&other);
						INLINE	bool					operator==(const ConstBinaryIterator<C,Class>&other);
						INLINE	bool					operator!=(const Iterator&other);
						INLINE	bool					operator!=(const ConstBinaryIterator<C,Class>&other);
	};

	template <class C, class Class> class ConstBinaryIterator
	{
	public:
						typedef ConstBinaryIterator<C,Class>	Iterator;
					
						const BinaryNode<C,Class>		*node;
					
														ConstBinaryIterator(const BinaryNode<C,Class>*node);
														ConstBinaryIterator(const BinaryIterator<C,Class>&iterator);
						INLINE	Iterator&				operator++();
						INLINE	Iterator				operator++(int);
						INLINE	C*						operator*();
						INLINE	bool					operator==(const Iterator&other);
						INLINE	bool					operator==(const BinaryIterator<C,Class>&other);
						INLINE	bool					operator!=(const Iterator&other);
						INLINE	bool					operator!=(const BinaryIterator<C,Class>&other);
	};


	/*!
		\brief AVL Binary tree

		Contained elements are automatically balanced to ensure access in O(log n).
		The respective sorting-key should not be modified as long as an element is stored within the tree.
	*/
	template <class C, class Class> class BinaryTree:protected Class
	{
	public:
						typedef C						Type;
						typedef C*						Data;
						typedef BinaryNode<C,Class>	Node;
						typedef void					(*_walkFunc)(Type*,size_t);		//!< Function type to use with walk and walkBack. The first parameter is the element executed, the second its index.
						typedef void					(*_printFunc)(Type*);
	protected:
	friend						class					BinaryNode<C,Class>;
								Node					*root,*seeker,*last_location;
								size_t					entries;

	public:
						typedef BinaryIterator<C,Class>		iterator;
						typedef ConstBinaryIterator<C,Class>	const_iterator;
								CONTAINER_TYPES

														BinaryTree();
	template <class CL>									BinaryTree(const BinaryTree<C,CL>&other);
	virtual												~BinaryTree();
	template <class CL> INLINE	BinaryTree<C,Class>&	operator=(const BinaryTree<C,CL>&other);

	template <class ID> INLINE	Type*					add(const ID&ident);
						INLINE	Node*					insert(Type*element);
	template <class CL> INLINE	void					import(BinaryTree<C,CL>&other);
	template <class CL> INLINE	void					importAndFlush(BinaryTree<C,CL>&other);
	template <class Lst>INLINE	void					import(Lst&list);
	template <class Lst>INLINE	void					importAndFlush(Lst&list);
						INLINE	Type*					drop(Type*element);
						INLINE	Type*					drop(size_t index);
						INLINE	iterator				drop(const iterator&it);
						INLINE	Node*					drop(Node*node);							//!< Drops the specified node's element from the tree. 
						INLINE	bool					erase(Type*element);
						INLINE	bool					erase(size_t index);
						INLINE	iterator				erase(const iterator&it);
						INLINE	Node*					erase(Node*node);
	template <class ID> INLINE	Type*					dropByIdent(const ID&ident);
	template <class ID> INLINE	void					eraseByIdent(const ID&ident);

						INLINE	void					walk(_walkFunc func);
						INLINE	void					walkBack(_walkFunc func);
								void					printHierarchy(_printFunc func);

						INLINE	size_t					count()								const;
						INLINE	void					clear();
						INLINE	void					flush();

	template <class ID> INLINE	Node*					lookupNode(const ID&ident);
	template <class ID> INLINE	const Node*				lookupNode(const ID&ident)			const;
	template <class ID> INLINE	Type*					lookup(const ID&ident);
	template <class ID> INLINE	const Type*				lookup(const ID&ident)				const;
	template <class ID> INLINE	size_t					lookupIndex(const ID&ident)			const;

						INLINE	Type*					first();
						INLINE	const Type*				first()								const;
						INLINE	Type*					last();
						INLINE	const Type*				last()								const;
						INLINE	Type*					get(size_t index);
						INLINE	const Type*				get(size_t index)						const;
						INLINE	const Type*				getConst(size_t index)				const;
						INLINE	Data&					getReference(size_t index);
						INLINE	Node*					getNode(size_t index);
						INLINE	const Node*				getNode(size_t index)					const;
						INLINE	iterator				getIterator(size_t index);
						INLINE	const_iterator			getIterator(size_t index)				const;
						INLINE	size_t					getIndexOf(const Type*element)		const;
						INLINE	size_t					getIndexOf(const Node*node)			const;
						INLINE	Type*					operator[](size_t index);
						INLINE	const Type*				operator[](size_t index)				const;
						INLINE	size_t					operator()(const Type*element)		const;
						INLINE	operator				size_t()							const;		//!< Implict cast to size_t	\return Number of elements in the list

						INLINE	iterator				begin();
						INLINE	const_iterator			begin()								const;
						INLINE	iterator				end();
						INLINE	const_iterator			end()								const;

						INLINE	void					reset();
						INLINE	Type*					each();
						INLINE	Type*					drop();
						INLINE	void					erase();
						INLINE	size_t					totalSize()							const;

								void					checkSyntax() const;
								CONTAINER_MMETHODS
	};

	/*!
		\brief AVL Binary tree (reference)

		Identical to BinaryTree except that it does not erase its elements on destruction.
	*/

	template <class C, class Class> class ReferenceBinaryTree		:public BinaryTree<C,Class>
	{
	public:
						typedef BinaryTree<C,Class>	List;
						USE_SORTED
						typedef C						Type;
						typedef C*						Data;
					
														ReferenceBinaryTree		();
	template <class CL>									ReferenceBinaryTree		(const BinaryTree<C,CL>&other);
	virtual												~ReferenceBinaryTree		();
	template <class CL> INLINE	BinaryTree<C,Class>&	operator=(const BinaryTree<C,CL>&other);


						INLINE	Type*					operator[](size_t index);
						INLINE	const Type*				operator[](size_t index)				const;
						INLINE	size_t					operator()(const Type*element)		const;
	};

	/*!
		\brief Binary tree node (AVL)
	
		Element carrier of BinaryTree.
		Each node has one parent and at most two children. A node that has no children is considered a leaf.
		Nodes are automatically balanced so that the difference in depths of the left and right child is not greater than one at any time.
	*/
	template <class C, class Class> class BinaryNode:protected Class
	{
	public:
						typedef C						Type;
						typedef BinaryNode<C,Class>	Node;
						typedef BinaryTree<C,Class>	Tree;
						typedef void					(*_walkFunc)(Type*,size_t);
						typedef void					(*_printFunc)(Type*);
	protected:
	friend						class					BinaryTree<C,Class>;


								int						resolveDepth() const;
						INLINE	void					change(Node*from, Node*to);
						INLINE	Node*					unaryChild();
						INLINE	Node*					leftMost();
								void					rotate(BYTE direction);
						INLINE	void					updateDepth();
						INLINE	int						getDepth(BYTE direction) const;
						INLINE	int						maxDepth() const;
				static	INLINE	int						lmax(int,int);
						INLINE	Node*					insertNode(Node*node, BYTE dir);
						INLINE	Node*					insertRec(Type*node, BYTE dir);
						INLINE	void					rebalanceUpper(Node*top);
								Node*					terminate(); //returns replacement
								void					balance();

								Tree					*tree;
								Node					*parent;
								Type					*element;
								int						depth;
								size_t				governing;
								union
								{
									Node				*child[2];
									struct
									{
										Node			*child0,*child1;
									};
									struct
									{
										Node			*left,*right;
									};
								};



														BinaryNode(Tree*tree_);
														BinaryNode(Tree*tree_,Node*parent_,Type*pntr_);
	virtual												~BinaryNode();
								Node*					insertNode(Node*node);
								Node*					insertRec(Type*element);
	template <class ID>			bool					eraseRec(const ID&ident);
	template <class ID>			Type*					dropRec(const ID&ident);
	template <class CL>			void					handOverRec(BinaryTree<C,CL>*target);
								void					eraseElementRec(size_t index);
								Type*					dropElementRec(size_t index);
								void					flushRec();
								Type*					getElementRec(size_t index);

								void					walk(_walkFunc func, size_t&id);
								void					walkBack(_walkFunc func, size_t&id);

								void					checkSyntax(size_t level=0) const;
								void					printHierarchy(char dir, size_t depth, _printFunc func);
								size_t				countRec()		const;
							
	public:
								Type*					getElement()				//! Retrieve carried element	\return carried element
								{
									return element;
								}
								const Type*				getElement()	const		//! Const version of getElement()	\return carried element
								{
									return element;
								}
								void					setElement(Type*el)			//! Update carried element. The previously carried element is \b not deleted. \param el Element to carry instead of the currently carried one.
								{
									element = el;
								}
								Tree*					getTree()					//! Query parent tree \return parent tree
								{
									return tree;
								}
								const Tree*				getTree()	const			//! Const version of getTree()	\return parent tree
								{
									return tree;
								}
	};



	/*!
		\fn void		BinaryTree::flush()
		\brief	Clears the local list without deleting the contained elements.

		\fn void BinaryTree::clear()
		\brief The clear()-method deletes all contained elements and clears the local list.

		\fn void BinaryTree::import(BinaryTree<C,CL>&other)
		\brief Imports all elements from another binary tree
		\param other List to import all elements from

		\fn void BinaryTree::import(Lst&list)
		\brief Imports all elements from another STL conform list
		\param list List to import all elements from (must be STL container conform)

		\fn void BinaryTree::importAndFlush(Lst&list)
		\brief Imports all elements another list and flushes that list
		\param list List to import all elements from and flush afterwards

		\fn Node* BinaryTree::insert(Type*element)
		\brief Inserts the specified element into the list
		\param element Element to insert
		\return Tree node containing the newly inserted element or NULL if an identical element already exists.
	
		\fn Type * 	BinaryTree::add (const ID &ident)
		\brief Insures existence of an element of the specified id.
		\param ident Id to use for the new element
		\return Pointer to the new or existing object.

		The add()-method ensures that an element matching the given identifier exists in the list.
		If no matching element was found a new object is created and inserted with the
		given identifier assigned to it. Operates in O(log n)

		\fn Type* BinaryTree::dropByIdent(const ID &ident)
		\brief Searches the list for an occurrence of identifier and drops the element if found.
		\return Pointer to dropped element or NULL if the element could not be found.
	
		Operates in O(log n).

		\fn void BinaryTree::eraseByIdent(const ID &ident)
		\brief Searches the list for an occurrence of identifier and erasess the element if found.

		Operates in O(log n).

		\fn void 	BinaryTree::walk (_walkFunc func)
		\brief Executes func once for each element stored in the tree in ascending order.
		\param func Function to execute
	
		\fn void 	BinaryTree::walkBack (_walkFunc func)
		\brief Executes func once for each element stored in the tree in descending order.
		\param func Function to execute

		\fn Type* BinaryTree::drop(size_t index)
		\brief Drops the element at the specified index from the list (without deleting it)
		\param index Index of the element to drop
		\return pointer to the dropped element or NULL if index is invalid
		(may also return NULL if the specified element contained NULL).

		Drops the specified element from the list without deleting it.
		Operates in O(log n).

		\fn iterator BinaryTree::drop(const iterator&it)
		\brief Drops the element at the specified iterator position from the list (without deleting it)
		\param it Valid iterator targeting the immediate successor of the dropped element.
		\return iterator pointing to the object immediately after the dropped element

		Drops the iterator-target from the list. This operation may de-validate certain
		or all iterators hooked to the list. Operates in O(1).

		\fn Type* BinaryTree::drop(Type*element)
		\brief Drops the specified element from the list (without deleting it)
		\param element Element to drop
		\return element on success, NULL otherwise

		\fn bool BinaryTree::erase(size_t index)
		\brief Erases the element at the specified index from the list (also deletes the object)
		\param index Index of the element to erase
		\return true on success, false if the specified element was not found

		Erases the element specified by index. The list remains unchanged if index is invalid.
		Operates in O(log n).


		\fn iterator BinaryTree::erase(const iterator&it)
		\brief Erases the element at the specified iterator position from the list (also deletes the object)
		\param it Iterator of the element to erase
		\return Iterator pointing to the object immediately following the erased element

		Erases the iterator-target from the list. This operation may de-validate certain or all
		iterators hooked to the list.
		Operates in O(1).

		\fn bool BinaryTree::erase(Type*element)
		\brief Erases the specified element from the list (also deletes the object)
		\param element Element to erase
		\return true on success, false if the specified element was not found


		\fn void BinaryTree::swap(BinaryTree<C,Size>&other)
		\brief Swaps the content of the local list with the remote list
		\param other List to swap contents with
		Exchanges the contents of the local list with the given list. The given list
		must be of the same list- and data-type as the local list.

		\fn void BinaryTree::revert()
		\brief Reverts the order of elements within the list

		\fn Type* BinaryTree::get(size_t index)
		\brief Retrieves an element from the list (optimized)
		\param index Index of the element to retrieve (0 = first element)
		\return Pointer to the respective object or NULL if the index is invalid

		The get()-method retrieves the element specified by \a index
		Operates in O(log n).

		\fn Type* BinaryTree::getConst(size_t index)			const
		\brief Retrieves an element from the list (const)
		\param index Index of the element to retrieve (0 = first element)
		\return Pointer to the respective object or NULL if the index is invalid

		The getConst()-method retrieves the element specified by \a index. Identical to the get() method invoked on a const list object. Deprecated.
		Operates in O(log n).

		\fn Data& BinaryTree::getReference(size_t index)		const
		\brief Retrieves the reference to an element from the list (const)
		\param index Index of the element to retrieve the reference from (0 = first element)
		\return Reference to the pointer of the respective object

		The list may throw an error if the specified index is invalid.
		The runtime-behaviour is identical to that of getConst()

		\fn iterator BinaryTree::getIterator(size_t index)			const
		\brief Retrieves an iterator pointing to the specified list position
		\param index Index of the object the iterator should point to
		\return Iterator to the specified position

		The returned iterator is undefined if the specified index is invalid.
		Operates in O(log n)
	
		\fn Node * BinaryTree::getNode (size_t index) const
		\brief Retrieves the node at the specified position
		\param index Index of the node to retrieve (0 = first element)
		\return Pointer to the node at the specified location or NULL if the index is invalid


		\fn Node* BinaryTree::lookupNode (const ID &ident) const
		\brief Tries to find the node of an element via its id
		\param ident Id to find
		\return Node of the element specified by identifier or NULL if the identifier could not be found

		\fn size_t BinaryTree::lookupIndex (const ID &ident) const
		\brief Tries to find the index of an element via its id
		\param ident Id to find
		\return index of the element specified by identifier (with 1 being the first element) or 0 if the
		element could not be found.

		\fn Type * BinaryTree::lookup (const ID &ident) const
		\brief Tries to find an element by its id
		\param ident Id to find
		\return Pointer to the element specified by identifier or NULL if the element could not be found.

		This method searches the local list-content for an occurrence of identifier. If the identifier was not found,
		lookup() will return NULL.
		Element-lookup operates in O(log n).


		\fn size_t BinaryTree::getIndexOf(const Node*node)			const
		\brief Retrieves the index of the specified node
		\param node Tree node to retrieve the position of
		\return index of the respective node+1 or 0 if the node could not be found

		Operates in O(log n)
	
		\fn size_t BinaryTree::getIndexOf(const Type*element)	const
		\brief Retrieves the index of the specified element
		\param element Element to retrieve the position of
		\return index of the respective object+1 or 0 if the object could not be found

		Operates in O(log n)

		\fn Type* BinaryTree::first() const
		\brief Retrieves the first element in the list
		\return Pointer to the first object in the list or NULL if the list is empty

		\fn Type* BinaryTree::last() const
		\brief Retrieves the last element in the list
		\return Pointer to the last object in the list or NULL if the list is empty

		\fn iterator BinaryTree::begin() const
		\brief Retrieves an iterator pointing to the first object in the list
		\return Iterator pointing to the first element

		\fn iterator BinaryTree::end() const
		\brief Retrieves an iterator pointing one past the last object in the list
		\return Iterator pointing one past the last element

		\fn Type* BinaryTree::operator[](size_t index)
		\brief Retrieves an element from the list (optimized)
		\param index Index of the element to return (0 = first element)
		\return Pointer to the respective object or NULL if the index is invalid

		\fn size_t BinaryTree::operator()(const Type*)			const
		\brief Retrieves the index of the specified element
		\param element Element to retrieve the position of
		\return index of the respective object+1 or 0 if the object could not be found

		\fn BinaryTree::operator				size_t()						const

		This implict type conversation can be quite handy. Assuming one has a list
		called 'objects', the following operations are valid:<br><ul>
		<li>if (objects) //if objects contains at least one object</li>
		<li>for (size_t i = 0; i < objects; i++) //iterate through all elements</li></ul>

		\fn size_t BinaryTree::count()							const
		\brief Returns the number of elements in the list
		\return Number of elements in the list

		\fn size_t BinaryTree::totalSize()						const
		\brief Calculates the size in bytes that the list consumes
		\returns Size in bytes (not counting the size of the elements)

		\fn void BinaryTree::reset()
		\brief Resets the 'each' cursor to the first element in the list

		Must be called before calling each()!

		\fn Type* BinaryTree::each()
		\brief Retrieves the current 'each' element of the list and advances the cursor by one element
		\return Current 'each' element or NULL if all elements have been processed

		each() sequentially returns the elements of the list beginning with the first
		and ending with the last element. The method returns NULL if the cursor reached
		the end of the list.<br />
		reset() must be called to initialize each() or the result will be undefined/corrupted.<br />
		Example:<br />
		<div>list.reset();<br />while ( \p Type *element = list.each())<br />(...)</div>

		\fn Type* BinaryTree::drop()
		\brief Drops the element at the current 'each' cursor position from the list (without deleting it)
		\return Element at the current 'each' cursor position

		Drops the element returned by the last each()-call from the list.
		The next each()-call will return the element following the dropped one.
		Operates in O(log n).

		\fn void BinaryTree::erase()
		\brief Erases the element at the current 'each' cursor position from the list (also deletes the object)

		Erases the element returned by the last each()-call from the list.
		The next each()-call will return the element following the erased one.
		Operates in O(log n).

		\fn Type* BinaryTree::include()
		\brief Inserts a new element at the current 'each' cursor position
		\return Pointer to a new object which has been inserted into the list

		Inserts a new  \p Type -object as the immediate predecessor of the element
		returned by the last each()-call. The next each()-call will return the
		element following the current with neither executing the new element
		nor re-executing the current.

		\fn void BinaryTree::include(Type*element)
		\brief Inserts the specified element at the current 'each' cursor position

		Inserts \a element as the immediate predecessor of the element returned by
		the last each()-call. The next each()-call will return the element following
		the current with neither executing the new element
		nor re-executing the current.

	*/





	#include "binarytree.tpl.h"
}

#endif

