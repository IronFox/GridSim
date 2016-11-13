#ifndef binarytreeTplH
#define binarytreeTplH

/******************************************************************

Implementation of the AVLTree-class.

******************************************************************/



//#define CHECK(item)		{cout << " <" << __func__ << "(" << __LINE__ << ")> "; if (item) item->checkSyntax(); else cout << "NULL ";}
//#define CHECK_SYNTAX	CHECK(this)



#define CHECK_SYNTAX
#define CHECK(item)





template <class C, class Class> BinaryIterator<C,Class>::BinaryIterator(BinaryNode<C,Class>*node_):node(node_)
{}

template <class C, class Class> INLINE BinaryIterator<C,Class>& BinaryIterator<C,Class>::operator++()
{
	if (node->right)
	{
		node = node->right->leftMost();
		return *this;
	}
	while (node->parent && node->parent->left != node)
		node = node->parent;
	node = node->parent;
	return *this;
}

template <class C, class Class> INLINE	BinaryIterator<C,Class>	BinaryIterator<C,Class>::operator++(int)
{
	BinaryNode<C,Class>*rs = node;
	
	if (node->right)
	{
		node = node->right->leftMost();
		return rs;
	}
	while (node->parent && node->parent->left != node)
		node = node->parent;
	node = node->parent;
	return rs;
}

template <class C, class Class> INLINE	C*& BinaryIterator<C,Class>::operator*()
{
	return node->element;
}

template <class C, class Class> INLINE	bool BinaryIterator<C,Class>::operator==(const Iterator&other)
{
	return node == other.node;
}

template <class C, class Class> INLINE	bool BinaryIterator<C,Class>::operator==(const ConstBinaryIterator<C,Class>&other)
{
	return node == other.node;
}

template <class C, class Class> INLINE	bool BinaryIterator<C,Class>::operator!=(const Iterator&other)
{
	return node != other.node;
}

template <class C, class Class> INLINE	bool BinaryIterator<C,Class>::operator!=(const ConstBinaryIterator<C,Class>&other)
{
	return node != other.node;
}



template <class C, class Class> ConstBinaryIterator<C,Class>::ConstBinaryIterator(const BinaryNode<C,Class>*node_):node(node_)
{}

template <class C, class Class> ConstBinaryIterator<C,Class>::ConstBinaryIterator(const BinaryIterator<C,Class>&iterator):node(iterator.node)
{}

template <class C, class Class> INLINE ConstBinaryIterator<C,Class>& ConstBinaryIterator<C,Class>::operator++()
{
	if (node->right)
	{
		node = node->right->leftMost();
		return *this;
	}
	while (node->parent && node->parent->left != node)
		node = node->parent;
	node = node->parent;
	return *this;
}

template <class C, class Class> INLINE	ConstBinaryIterator<C,Class> ConstBinaryIterator<C,Class>::operator++(int)
{
	BinaryNode<C,Class>*rs = node;

	if (node->right)
	{
		node = node->right->leftMost();
		return rs;
	}
	while (node->parent && node->parent->left != node)
		node = node->parent;
	node = node->parent;
	return rs;
}

template <class C, class Class> INLINE	C*	ConstBinaryIterator<C,Class>::operator*()
{
	return node->element;
}

template <class C, class Class> INLINE	bool ConstBinaryIterator<C,Class>::operator==(const Iterator&other)
{
	return node == other.node;
}

template <class C, class Class> INLINE	bool ConstBinaryIterator<C,Class>::operator==(const BinaryIterator<C,Class>&other)
{
	return node == other.node;
}

template <class C, class Class> INLINE	bool ConstBinaryIterator<C,Class>::operator!=(const Iterator&other)
{
	return node != other.node;
}

template <class C, class Class> INLINE	bool ConstBinaryIterator<C,Class>::operator!=(const BinaryIterator<C,Class>&other)
{
	return node != other.node;
}











template <class C, class Class> BinaryTree<C,Class>::BinaryTree():root(NULL),entries(0)
{}

template <class C, class Class>
template <class CL> BinaryTree<C,Class>::BinaryTree(const BinaryTree<C,CL>&other):root(NULL),entries(0)
{
	insert(other);
}

template <class C, class Class> BinaryTree<C,Class>::~BinaryTree()
{
	clear();
}

template <class C, class Class>
template <class CL> INLINE	BinaryTree<C,Class>&	BinaryTree<C,Class>::operator=(const BinaryTree<C,CL>&other)
{
	clear();
	insert(other);
	return *this;
}


template <class C, class Class>
template <class C0> INLINE	C* BinaryTree<C,Class>::add(const C0&ident)
{
	Node*node = SHIELDED(new Node(this));
	Class::create(node->element,ident);
	if (!root)
	{
		root = node;
		entries = 1;
		return node->element;
	}
	Node*other = root->insertNode(node);
	if (other==node)
	{
		entries++;
		return node->element;
	}
	DISCARD(node);
#pragma warning(suppress: 6011)
	return other->element;
}


template <class C, class Class> INLINE	BinaryNode<C,Class>* BinaryTree<C,Class>::insert(Type*element)
{
	if (!root)
	{
		root = SHIELDED(new Node(this,NULL,element));
		entries = 1;
		return root;
	}
	Node*n = root->insertRec(element);
	if (n->element == element)
		entries++;
	else
		return NULL;
	CHECK(root);
	return n;
}

template <class C, class Class>
template <class CL> INLINE	void BinaryTree<C,Class>::import(BinaryTree<C,CL>&other)
{
	if (&other == this)
	{
	#if DEBUG_LEVEL >= 1
		FATAL__("trying to re-import local content");
	#else
		return;
	#endif
	}
	if (other.root)
		other.root->handOverRec(this);
}

template <class C, class Class>
template <class CL> INLINE	void BinaryTree<C,Class>::importAndFlush(BinaryTree<C,CL>&other)
{
	if (&other == this)
	{
	#if DEBUG_LEVEL >= 1
		FATAL__("trying to re-import local content");
	#else
		return;
	#endif
	}
	if (other.root)
		other.root->handOverRec(this);
	other.flush();
}


template <class C, class Class>
template <class Lst>INLINE	void BinaryTree<C,Class>::import(Lst&list)
{
	if (&list == this)
	{
	#if DEBUG_LEVEL >= 1
		FATAL__("trying to re-import local content");
	#else
		return;
	#endif
	}
	for (typename Lst::iterator it = list.begin(); it != list.end(); it++)
		insert(*it);
}

template <class C, class Class>
template <class Lst>INLINE	void BinaryTree<C,Class>::importAndFlush(Lst&list)
{
	if (&list == this)
	{
	#if DEBUG_LEVEL >= 1
		FATAL__("trying to re-import local content");
	#else
		return;
	#endif
	}
	for (typename Lst::iterator it = list.begin(); it != list.end(); it++)
		insert(*it);
	list.flush();
}



template <class C, class Class>
template <class ID> INLINE	C* BinaryTree<C,Class>::dropByIdent(const ID&ident)
{
	Node*n = lookupNode(ident);
	if (!n)
		return NULL;
	Type*result = n->element;
	n->element = NULL;
	n->terminate();
	entries--;
	return result;
}

template <class C, class Class>
template <class ID> INLINE	void BinaryTree<C,Class>::eraseByIdent(const ID&ident)
{
	if (Node*n = lookupNode(ident))
	{
		n->terminate();
		entries--;
	}
}

template <class C, class Class>
template <class ID> INLINE	BinaryNode<C,Class>* BinaryTree<C,Class>::lookupNode(const ID&ident)
{
	Node*n = root;
	while (n)
	{
		if (Class::greater(n->element,ident))
			n = n->left;
		else
			if (Class::less(n->element,ident))
				n = n->right;
			else
				return n;
	}
	return NULL;
}

template <class C, class Class>
template <class ID> INLINE	const BinaryNode<C,Class>* BinaryTree<C,Class>::lookupNode(const ID&ident) const
{
	const Node*n = root;
	while (n)
	{
		if (Class::greater(n->element,ident))
			n = n->left;
		else
			if (Class::less(n->element,ident))
				n = n->right;
			else
				return n;
	}
	return NULL;
}

template <class C, class Class>
template <class ID> INLINE	C* BinaryTree<C,Class>::lookup(const ID&ident)
{
	Node*n = root;
	while (n)
	{
		if (Class::greater(n->element,ident))
			n = n->left;
		else
			if (Class::less(n->element,ident))
				n = n->right;
			else
				return n->element;
	}
	return NULL;
}

template <class C, class Class>
template <class ID> INLINE	const C* BinaryTree<C,Class>::lookup(const ID&ident) const
{
	const Node*n = root;
	while (n)
	{
		if (Class::greater(n->element,ident))
			n = n->left;
		else
			if (Class::less(n->element,ident))
				n = n->right;
			else
				return n->element;
	}
	return NULL;
}

template <class C, class Class>
template <class ID> INLINE	size_t BinaryTree<C,Class>::lookupIndex(const ID&ident) const
{
	const BinaryNode<C,Class>*node=root;
	size_t id = 1;
	while (node)
	{
		if (Class::greater(node->element,ident))
			node = node->left;
		else
			if (Class::less(node->element,ident))
			{
				id++;
				if (node->left)
					id+=node->left->governing;
				node = node->right;
			}
			else
				return id;
	}
	return 0;
}

template <class C, class Class> INLINE	size_t BinaryTree<C,Class>::getIndexOf(const Node*node) const
{
	size_t left = node->left?node->left->governing:0;
	while (node->parent)
	{
		if (node->parent->right == node)
			left+= node->parent->governing - node->governing;
		node = node->parent;
	}
	if (root != node)
		return 0;
	return left+1;
}


template <class C, class Class> INLINE	size_t BinaryTree<C,Class>::getIndexOf(const Type*element) const
{
	const BinaryNode<C,Class>*node=root;
	size_t id = 1;
	while (node)
	{
		if (Class::greaterObject(node->element,element))
			node = node->left;
		else
			if (Class::lesserObject(node->element,element))
			{
				id++;
				if (node->left)
					id+=node->left->governing;
				node = node->right;
			}
			else
				return id;
	}
	return 0;
}


template <class C, class Class> INLINE	C* BinaryTree<C,Class>::drop()
{
	Node*top = seeker->parent;
	Type*rs = seeker->element;
	seeker->element = NULL;
	seeker->terminate();
	seeker = last_location;
	top->rebalanceUpper(NULL);
	entries--;
	CHECK(root);
	return rs;
}

template <class C, class Class> INLINE C*	BinaryTree<C,Class>::drop(Type*element)
{
	BinaryNode<C,Class>*node=root;
	while (node)
	{
		if (Class::greaterObject(node->element,element))
			node = node->left;
		else
			if (Class::lesserObject(node->element,element))
				node = node->right;
			else
			{
				Type*result = node->element;
				drop(node);
				return result;
			}
	}
	return NULL;
}

template <class C, class Class> INLINE bool BinaryTree<C,Class>::erase(Type*element)
{
	C*found = drop(element);
	if (!found)
		return false;
	DISCARD(found);
	return true;
}

template <class C, class Class> INLINE BinaryNode<C,Class>* BinaryTree<C,Class>::drop(Node*node)
{
	Node*top = node->parent;
	node->element = NULL;
	Node*replacement = node->terminate();
	top->rebalanceUpper(NULL);
	entries--;
	CHECK(root);
	return replacement;
}

template <class C, class Class> INLINE BinaryNode<C,Class>* BinaryTree<C,Class>::erase(Node*node)
{
	DISCARD(node->element);
	Node*top = node->parent;
	node->element = NULL;
	Node*replacement = node->terminate();
	top->rebalanceUpper(NULL);
	entries--;
	CHECK(root);
	return replacement;
}


template <class C, class Class> INLINE BinaryIterator<C,Class> BinaryTree<C,Class>::drop(const iterator&it)
{
	it.node->element = NULL;
	return drop(it.node);
}


template <class C, class Class> INLINE	void BinaryTree<C,Class>::erase()
{
	Node*top = seeker->parent;
	seeker->terminate();
	seeker = last_location;
	top->rebalanceUpper(NULL);
	entries--;
	CHECK(root);
}


template <class C, class Class> INLINE BinaryIterator<C,Class> BinaryTree<C,Class>::erase(const iterator&it)
{
	return drop(it.node);
}

template <class C, class Class> INLINE	void BinaryTree<C,Class>::walk(_walkFunc func)
{
	size_t id(0);
	if (root)
		root->walk(func,id);
}

template <class C, class Class> INLINE	void BinaryTree<C,Class>::walkBack(_walkFunc func)
{
	size_t id(entries-1);
	if (root)
		root->walkBack(func,id);
}

template <class C, class Class> INLINE size_t BinaryTree<C,Class>::count() const
{
	return entries;
}

template <class C, class Class> void BinaryTree<C,Class>::checkSyntax() const
{
	if (root)
	{
		root->checkSyntax();
		if (entries != root->countRec())
			FATAL__("entries mismatch ");//+IntToStr(entries)+" != "+IntToStr(root->countRec()));
	}
	else
		if (entries)
			FATAL__("list is not empty but root is NULL");
}

template <class C, class Class> INLINE	void BinaryTree<C,Class>::clear()
{
	if (root)
		DISCARD(root);
	root = NULL;
	entries = 0;
}

template <class C, class Class> INLINE	void BinaryTree<C,Class>::flush()
{
	if (root)
	{
		root->flushRec();
		DISCARD(root);
	}
	root = NULL;
	entries = 0;
}

template <class C, class Class> INLINE	const C* BinaryTree<C,Class>::getConst(size_t index) const
{
	CHECK_SYNTAX;
	if (index >= entries)
		return NULL;
	const Node*el = root;
	while (el)
	{
		size_t l = el->left?el->left->governing:0;
		if (index < l)
		{
			el = el->left;
			continue;
		}
		if (index == l)
			return el->element;
		index-=l+1;
		el = el->right;
	}
	FATAL__("lookup failed");
}


template <class C, class Class> INLINE	BinaryNode<C,Class>* BinaryTree<C,Class>::getNode(size_t index)
{
	CHECK_SYNTAX;
	if (index >= entries)
		return NULL;
	Node*el = root;
	while (el)
	{
		size_t l = el->left?el->left->governing:0;
		if (index < l)
		{
			el = el->left;
			continue;
		}
		if (index == l)
			return el;
		index-=l+1;
		el = el->right;
	}
	FATAL__("lookup failed");
}

template <class C, class Class> INLINE	const BinaryNode<C,Class>* BinaryTree<C,Class>::getNode(size_t index) const
{
	CHECK_SYNTAX;
	if (index >= entries)
		return NULL;
	const Node*el = root;
	while (el)
	{
		size_t l = el->left?el->left->governing:0;
		if (index < l)
		{
			el = el->left;
			continue;
		}
		if (index == l)
			return el;
		index-=l+1;
		el = el->right;
	}
	FATAL__("lookup failed");
}

template <class C, class Class> INLINE	BinaryIterator<C,Class> BinaryTree<C,Class>::getIterator(size_t index)
{
	return getNode(index);
}

template <class C, class Class> INLINE	ConstBinaryIterator<C,Class> BinaryTree<C,Class>::getIterator(size_t index)				const
{
	return getNode(index);
}


template <class C, class Class> INLINE C* BinaryTree<C,Class>::get(size_t index)
{
	CHECK_SYNTAX;
	if (index >= entries)
		return NULL;
	Node*el = root;
	while (el)
	{
		size_t l = el->left?el->left->governing:0;
		if (index < l)
		{
			el = el->left;
			continue;
		}
		if (index == l)
			return el->element;
		index-=l+1;
		el = el->right;
	}
	FATAL__("lookup failed");
}

template <class C, class Class> INLINE const C* BinaryTree<C,Class>::get(size_t index)	const
{
	CHECK_SYNTAX;
	if (index >= entries)
		return NULL;
	const Node*el = root;
	while (el)
	{
		size_t l = el->left?el->left->governing:0;
		if (index < l)
		{
			el = el->left;
			continue;
		}
		if (index == l)
			return el->element;
		index-=l+1;
		el = el->right;
	}
	FATAL__("lookup failed");
}

template <class C, class Class> INLINE C*& BinaryTree<C,Class>::getReference(size_t index)
{
	CHECK_SYNTAX;
	if (index >= entries)
		FATAL__("bad reference-lookup");
	Node*el = root;
	while (el)
	{
		size_t l = el->left?el->left->governing:0;
		if (index < l)
		{
			el = el->left;
			continue;
		}
		if (index == l)
			return el->element;
		index-=l+1;
		el = el->right;
	}
	FATAL__("lookup failed");
}

template <class C, class Class> INLINE	size_t	BinaryTree<C,Class>::totalSize()	const
{
	return sizeof(*this) + sizeof(BinaryNode<C,Class>)*entries;
}

template <class C, class Class> INLINE	C* BinaryTree<C,Class>::first()
{
	if (!root)
		return NULL;
	Node*n(root);
	while (n->left)
		n = n->left;
	return n->element;
}

template <class C, class Class> INLINE	const C* BinaryTree<C,Class>::first()								const
{
	if (!root)
		return NULL;
	const Node*n(root);
	while (n->left)
		n = n->left;
	return n->element;
}

template <class C, class Class> INLINE	C* BinaryTree<C,Class>::last()
{
	if (!root)
		return NULL;
	Node*n(root);
	while (n->right)
		n = n->right;
	return n->element;
}

template <class C, class Class> INLINE	const C* BinaryTree<C,Class>::last()								const
{
	if (!root)
		return NULL;
	const Node*n(root);
	while (n->right)
		n = n->right;
	return n->element;
}

template <class C, class Class> INLINE BinaryIterator<C,Class> BinaryTree<C,Class>::begin()
{
	if (!root)
		return NULL;
	Node*n(root);
	while (n->left)
		n = n->left;
	return n;
}

template <class C, class Class> INLINE ConstBinaryIterator<C,Class> BinaryTree<C,Class>::begin()	const
{
	if (!root)
		return NULL;
	const Node*n(root);
	while (n->left)
		n = n->left;
	return n;
}

template <class C, class Class> INLINE BinaryIterator<C,Class> BinaryTree<C,Class>::end()
{
	return NULL;
}

template <class C, class Class> INLINE ConstBinaryIterator<C,Class> BinaryTree<C,Class>::end() const
{
	return NULL;
}

template <class C, class Class> INLINE C* BinaryTree<C,Class>::operator[](size_t index)
{
	return get(index);
}

template <class C, class Class> INLINE const C* BinaryTree<C,Class>::operator[](size_t index) const
{
	return get(index);
}

template <class C, class Class> INLINE size_t BinaryTree<C,Class>::operator()(const Type*element) const
{
	return getIndexOf(element);
}

template <class C, class Class> INLINE	BinaryTree<C,Class>::operator				size_t()							const
{
	return entries;
}

template <class C, class Class> void BinaryTree<C,Class>::reset()
{
	last_location = seeker = NULL;
	
}

template <class C, class Class> C* BinaryTree<C,Class>::each()
{
	last_location = seeker;
	if (!seeker)
		if (root)
		{
			seeker = root->leftMost();
			return seeker->element;
		}
		else
			return NULL;
	if (seeker->right)
	{
		seeker = seeker->right->leftMost();
		return seeker->element;
	}
	while (seeker->parent && seeker->parent->left != seeker)
		seeker = seeker->parent;
	seeker = seeker->parent;
	return seeker?seeker->element:NULL;
}

template <class C, class Class> INLINE	C* BinaryTree<C,Class>::drop(size_t index)
{
	if (index >= entries)
		return NULL;
	if (!root)
		FATAL__("list entry-count broken");
	entries--;
	return root->dropElementRec(index);
}

template <class C, class Class> INLINE	bool BinaryTree<C,Class>::erase(size_t index)
{
	if (index >= entries)
		return false;
	if (!root)
		FATAL__("list entry-count broken");
	root->eraseElementRec(index);
	entries--;
	return true;
}


template <class C, class Class> void BinaryTree<C,Class>::printHierarchy(_printFunc func)
{
	if (root)
		root->printHierarchy(' ',0,func);
}



template <class C, class Class> ReferenceBinaryTree<C,Class>::ReferenceBinaryTree		()
{}

template <class C, class Class>
template <class CL>			ReferenceBinaryTree<C,Class>::ReferenceBinaryTree		(const BinaryTree<C,CL>&other):BinaryTree<C,Class>(other)
{}

template <class C, class Class> ReferenceBinaryTree<C,Class>::~ReferenceBinaryTree		()
{
	Super::flush();
}

template <class C, class Class>
template <class CL> INLINE	BinaryTree<C,Class>&	ReferenceBinaryTree<C,Class>::operator=(const BinaryTree<C,CL>&other)
{
	Super::flush();
	insert(other);
	return *this;
}


template <class C, class Class> INLINE	C* ReferenceBinaryTree<C,Class>::operator[](size_t index)
{
	return Super::get(index);
}

template <class C, class Class> INLINE	const C* ReferenceBinaryTree<C,Class>::operator[](size_t index)				const
{
	return Super::get(index);
}

template <class C, class Class> INLINE	size_t ReferenceBinaryTree<C,Class>::operator()(const Type*element)		const
{
	return getIndexOf(element);
}






















template <class C, class Class> BinaryNode<C,Class>::BinaryNode(Tree*tree_):tree(tree_),parent(NULL),element(NULL),
														depth(1),governing(1),left(NULL),right(NULL)
{}

template <class C, class Class> BinaryNode<C,Class>::BinaryNode(Tree*tree_,Node*parent_,Type*pntr_):tree(tree_),parent(parent_),element(pntr_),
														depth(1),governing(1),left(NULL),right(NULL)
{}


template <class C, class Class> BinaryNode<C,Class>::~BinaryNode()
{
	if (left)
		DISCARD(left);
	if (right)
		DISCARD(right);
	if (element)
		DISCARD(element);
}


template <class C, class Class> void BinaryNode<C,Class>::flushRec()
{
	if (left)
		left->flushRec();
	if (right)
		right->flushRec();
	element = NULL;
}

template <class C, class Class> size_t BinaryNode<C,Class>::countRec() const
{
	size_t cnt(1);
	if (left)
		cnt+=left->countRec();
	if (right)
		cnt+=right->countRec();
	return cnt;
}

template <class C, class Class> C* BinaryNode<C,Class>::getElementRec(size_t id)
{
	size_t l = left?left->governing:0;
	if (id < l)
		return left->getElementRec(id);
	if (id == l)
		return element;
	if (!right)
		FATAL__("illegal list lookup ");//("+IntToStr(id)+" <" +IntToStr(l+1)+">/"+IntToStr(governing)+")");
	return right->getElementRec(id-l-1);
}


template <class C, class Class> INLINE void BinaryNode<C,Class>::change(Node*from, Node*to)
{
	if (!this)
	{
		from->tree->root = to;
		return;
	}
	if (child0 == from)
		child0 = to;
	else
		if (child1 == from)
			child1 = to;
	updateDepth();
}

template <class C, class Class> INLINE BinaryNode<C,Class>*BinaryNode<C,Class>::unaryChild()
{
	if (child0 && !child1)
		return child0;
	if (child1 && !child0)
		return child1;
	return NULL;
}

template <class C, class Class> INLINE BinaryNode<C,Class>* BinaryNode<C,Class>::leftMost()
{
	Node*node = this;
	while (node->left)
		node = node->left;
	return node;
}

template <class C, class Class> void BinaryNode<C,Class>::rotate(BYTE d)
{
	parent->change(this,child[!d]);
	Node*temp = child[!d]->child[d];
	child[!d]->child[d] = this;
	child[!d]->parent = parent;
	parent = child[!d];
	child[!d] = temp;
	if (temp)
		temp->parent = this;
	updateDepth();
	if (parent)
		parent->updateDepth();
}

template <class C, class Class> INLINE	void BinaryNode<C,Class>::updateDepth()
{
	depth = maxDepth()+1;
	governing = 1;
	if (left)
		governing+=left->governing;
	if (right)
		governing+=right->governing;
}


template <class C, class Class> INLINE	int BinaryNode<C,Class>::getDepth(BYTE direction) const
{
	if (child[direction])
		return child[direction]->depth;
	return 0;
}

template <class C, class Class> INLINE	int BinaryNode<C,Class>::lmax(int a,int b)
{
	return a > b?a:b;
}


template <class C, class Class> INLINE	int BinaryNode<C,Class>::maxDepth() const
{
	return lmax(getDepth(0),getDepth(1));
}

template <class C, class Class> int BinaryNode<C,Class>::resolveDepth() const
{
	int a = child0?child0->resolveDepth():0,
		b = child1?child1->resolveDepth():0;
	return lmax(a,b)+1;
}



template <class C, class Class> void BinaryNode<C,Class>::balance()
{
	updateDepth();
	int delta = getDepth(0)-getDepth(1);
	BYTE dir;
	if (delta >= 2)
		dir = 0;
	else
		if (delta <= -2)
			dir = 1;
		else
			return;
	if (child[dir]->getDepth(!dir) > child[dir]->getDepth(dir))
		child[dir]->rotate(dir);
	rotate(!dir);
}

template <class C, class Class> BinaryNode<C,Class>* BinaryNode<C,Class>::insertNode(Node*node, BYTE dir)
{
	CHECK_SYNTAX;
	Node*rs;
	if (child[dir])
	{
		rs = child[dir]->insertNode(node);
		if (child[dir]->depth-getDepth(!dir) >= 2)
		{
			if (child[dir]->getDepth(!dir) > child[dir]->getDepth(dir))
				child[dir]->rotate(dir);
			rotate(!dir);
		}
		updateDepth();
		return rs;
	}
	rs = child[dir] = node;
	node->parent = this;
	node->depth = 1;
	updateDepth();
	CHECK_SYNTAX;

	return rs;
}


template <class C, class Class> BinaryNode<C,Class>* BinaryNode<C,Class>::insertRec(C*el, BYTE dir)
{
	CHECK_SYNTAX;
	Node*rs;
	if (child[dir])
	{
		rs = child[dir]->insertRec(el);
		if (child[dir]->depth-getDepth(!dir) >= 2)
		{
			if (child[dir]->getDepth(!dir) > child[dir]->getDepth(dir))
				child[dir]->rotate(dir);
			rotate(!dir);
		}
		updateDepth();
		CHECK_SYNTAX;
		return rs;
	}
	rs = child[dir] = SHIELDED(new Node(tree,this,el));
	updateDepth();
	CHECK_SYNTAX;

	return rs;
}


template <class C, class Class> BinaryNode<C,Class>* BinaryNode<C,Class>::insertNode(Node*node)
{
	CHECK_SYNTAX;
	if (Class::greaterObject(element,node->element))
		return insertNode(node,0);
	if (Class::lesserObject(element,node->element))
		return insertNode(node,1);
	return this;
}

template <class C, class Class> BinaryNode<C,Class>* BinaryNode<C,Class>::insertRec(Type*el)
{
	CHECK_SYNTAX;
	if (Class::greaterObject(element,el))
		return insertRec(el,0);
	if (Class::lesserObject(element,el))
		return insertRec(el,1);
	return this;
}


template <class C, class Class> BinaryNode<C,Class>* BinaryNode<C,Class>::terminate()
{
	CHECK_SYNTAX;
	if (!child0 && !child1)
	{
		parent->change(this,NULL);		//updates depth
		parent = NULL;
		CHECK(parent);
		DISCARD(this);
		return NULL;
	}

	if (Node*unary = unaryChild())
	{
		unary->parent = parent;
		parent->change(this,unary);
		child0 = child1 = NULL;
		parent = NULL;
		CHECK(unary);
		DISCARD(this);
		return unary;
	}

	Node*left_most = child1->leftMost();
	if (left_most == child1)
	{
		child1->child0 = child0;
		child0->parent = child1;
		child1->parent = parent;
		child1->updateDepth();
		parent->change(this,child1);
		child1->balance();

		child0 = child1 = NULL;
		parent = NULL;
		CHECK(child1);
		DISCARD(this);
		return left_most;
	}


	if (left_most->child1)
	{
		left_most->child1->parent = left_most->parent;
		left_most->parent->child0 = left_most->child1;
	}
	else
		left_most->parent->child0 = NULL;

	left_most->parent->rebalanceUpper(this);

	left_most->child0 = child0;
	left_most->child1 = child1;
	left_most->parent = parent;
	left_most->updateDepth();
	child0->parent = left_most;
	child1->parent = left_most;
	parent->change(this,left_most);
	child0 = child1 = NULL;
	parent = NULL;
	left_most->balance();
	CHECK(left_most);
	DISCARD(this);
	return left_most;
}

template <class C, class Class> INLINE void BinaryNode<C,Class>::rebalanceUpper(Node*limit) //limit can be null to indicate entire path
{
	Node*node = this;
	while (node != limit)
	{
		node->updateDepth();
		node->balance();
		#pragma warning(suppress: 6011)	//useless piece of crap. analysis doesn't work
		node = node->parent;
		if (!node && limit)
			FATAL__("boundary breached");
	}
}


template <class C, class Class>
template <class C0> bool BinaryNode<C,Class>::eraseRec(const C0&ident)
{
	CHECK_SYNTAX;
	if (Class::greater(element,ident))
	{
		if (!left || !left->eraseRec(ident))
			return false;
		balance();
		CHECK_SYNTAX;
		return true;
	}
	if (Class::less(element,ident))
	{
		if (!right || !right->eraseRec(ident))
			return false;
		balance();
		CHECK_SYNTAX;
		return true;
	}
	terminate();
	return true;
}


template <class C, class Class>
template <class C0>			C*						BinaryNode<C,Class>::dropRec(const C0&ident)
{
	CHECK_SYNTAX;
	if (Class::greater(element,ident))
		return dropRec(ident,0);
	if (Class::less(element,ident))
		return dropRec(ident,1);

	C*result = element;
	element = NULL;
	terminate();
	return result;
}

template <class C, class Class> void BinaryNode<C,Class>::eraseElementRec(size_t id)
{
	CHECK_SYNTAX;
	size_t l = left?left->governing:0;
	if (id < l)
	{
		left->eraseElementRec(id);
		balance();
		CHECK_SYNTAX;
		return;
	}
	if (id == l)
	{
		terminate();
		return;
	}
	if (!right)
		FATAL__("illegal list-lookup");
	right->eraseElementRec(id-l-1);
	balance();
	CHECK_SYNTAX;
}

template <class C, class Class> C* BinaryNode<C,Class>::dropElementRec(size_t id)
{
	CHECK_SYNTAX;
	size_t l = left?left->governing:0;
	Type*pntr;
	if (id < l)
	{
		pntr = left->dropElementRec(id);
		balance();
		CHECK_SYNTAX;
		return pntr;
	}
	if (id == l)
	{
		pntr = element;
		element = NULL;
		terminate();
		return pntr;
	}
	if (!right)
		FATAL__("illegal list-lookup");
	pntr = right->eraseElementRec(id-l-1);
	balance();
	CHECK_SYNTAX;
	return pntr;
}

template <class C, class Class>
template <class CL> void BinaryNode<C,Class>::handOverRec(BinaryTree<C,CL>*target)
{
	if (left)
		left->handOverRec(target);
	target->insert(element);
	if (right)
		right->handOverRec(target);
}


template <class C, class Class> void BinaryNode<C,Class>::walk(_walkFunc func, size_t&id)
{
	if (left)
		left->walk(func,id);
	func(element,id++);
	if (right)
		right->walk(func,id);
}

template <class C, class Class> void BinaryNode<C,Class>::walkBack(_walkFunc func, size_t&id)
{
	if (right)
		right->walkBack(func,id);
	func(element,id--);
	if (left)
		left->walk(func,id);
}


template <class C, class Class> void BinaryNode<C,Class>::checkSyntax(size_t level) const
{
	#undef ERROR
//	#define ERROR(line) {cout << endl << line << " (depth "<<level<<")"<<endl; system("PAUSE"); exit(0);}
	#define ERROR(line) FATAL__("syntax broken")

	if (left && Class::greaterObject((const C*)left->element,(const C*)element))
		ERROR("left order broken");
	if (right && Class::greaterObject((const C*)element,(const C*)right->element))
		ERROR("right order broken");
	if (resolveDepth() != depth)
		ERROR("depth mismatch (resolved "<<resolveDepth()<<", stored "<<depth<<")");
	if (countRec() != governing)
		ERROR("governing mismatch (resolved "<<countRec()<<", stored "<<governing<<")");
	{
		int delta = getDepth(0) - getDepth(1);
		if (delta < 0)
			delta *=-1;
		if (delta > 1)
			ERROR("delta too large ("<<getDepth(0)<<" "<<getDepth(1)<<")");
	}
	for (BYTE k = 0; k < 2; k++)
		if (child[k])
			if (child[k]->parent != this)
				ERROR("this-pointer broken")
			else
				child[k]->checkSyntax(level+1);
/*	if (!level)
		cout << "check done" << endl;*/
	#undef ERROR
}

template <class C, class Class> void BinaryNode<C,Class>::printHierarchy(char dir, size_t depth, _printFunc func)
{
	/*
	for (size_t i = 0; i < depth; i++)
		cout << " |";
	cout << dir << " ";*/
	func(element);
/*	cout <<endl;*/
	if (left)
		left->printHierarchy('<',depth+1,func);
	if (right)
		right->printHierarchy('>',depth+1,func);
}

#undef CHECK_SYNTAX
#undef CHECK



#endif

