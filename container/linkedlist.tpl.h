#ifndef linkedlistTplH
#define linkedlistTplH

/******************************************************************

Implementation of the LinkedList-class.

******************************************************************/


#define MAKE_CARRIER		SHIELDED_ARRAY(new Carrier[1],1)
#define UNMAKE_CARRIER(c)	DISCARD_ARRAY(c)


template <class C> INLINE void LinkedList<C>::clear()
{
	Carrier*carrier = pivot.next;
	while (carrier != &pivot)
	{
		Carrier*next = carrier->next;
		DISCARD(carrier->element);
		UNMAKE_CARRIER(carrier);
		carrier = next;
	}
	pivot.next = &pivot;
	pivot.prev = &pivot;
	seeker = NULL;
	elements = 0;
}


template <class C> INLINE bool LinkedList<C>::seek(size_t index)
{
	if (index >= elements)
		return false;
	if (!seeker)
	{
		seeker = pivot.next;
		seeker_index = 0;
	}

	while (seeker_index < index)
	{
		seeker = seeker->next;
		seeker_index++;
	}
	while (seeker_index > index)
	{
		seeker = seeker->prev;
		seeker_index--;
	}
	return true;
}

template <class C> INLINE typename LinkedList<C>::Carrier*	LinkedList<C>::drop(Carrier*carrier)
{
	carrier->prev->next = carrier->next;
	carrier->next->prev = carrier->prev;

	Carrier*next = carrier->next;
	UNMAKE_CARRIER(carrier);
	elements--;
	
	seeker = NULL;	//must invalidate here since seeker_index cannot be checked/corrected

	return next;
}





template <class C> LinkedList<C>::LinkedList():seeker(NULL),cursor(&pivot),elements(0),seeker_index(-1)
{
	pivot.next = &pivot;
	pivot.prev = &pivot;
	pivot.element = NULL;
}

template <class C> LinkedList<C>::LinkedList(const LinkedList<C>&other):seeker(NULL),cursor(&pivot),elements(0),seeker_index(-1)
{
	append(other);
}

template <class C> LinkedList<C>::~LinkedList()
{
	clear();
}

			
template <class C>
	void				LinkedList<C>::adoptData(LinkedList<C>&other)
	{
		pivot.next = other.pivot.next;
		pivot.prev = other.pivot.prev;
		pivot.next->prev = &pivot;
		pivot.prev->next = &pivot;
		
		other.pivot.next = &other.pivot;
		other.pivot.prev = &other.pivot;
		
		seeker = other.seeker;
		cursor = other.cursor;
		carrier = other.carrier;
		elements = other.elements;
		seeker_index = other.seeker_index;
		
		other.seeker = NULL;
		other.cursor = &pivot;
		other.elements = 0;
		other.seeker_index = -1;
		other.carrier = NULL;
	}

template <class C> INLINE LinkedList<C>& LinkedList<C>::operator=(const LinkedList<C>&other)
{
	clear();
	append(other);
	return *this;
}


template <class C> INLINE	C* LinkedList<C>::append(C*Element)
{
	carrier = MAKE_CARRIER;
	carrier->element = Element;

	carrier->prev = pivot.prev;
	pivot.prev->next = carrier;
	carrier->next = &pivot;
	pivot.prev = carrier;

	elements++;

	return Element;
}

template <class C> INLINE	void LinkedList<C>::push_back(C*Element)
{
	carrier = MAKE_CARRIER;
	carrier->element = Element;

	carrier->prev = pivot.prev;
	pivot.prev->next = carrier;
	carrier->next = &pivot;
	pivot.prev = carrier;

	elements++;
}


template <class C> INLINE	void LinkedList<C>::push_front(C*Element)
{
	carrier = MAKE_CARRIER;
	carrier->element = Element;

	carrier->prev = &pivot;
	pivot.next->prev = carrier;
	carrier->next = pivot.next;
	pivot.next = carrier;

	elements++;
}


template <class C> INLINE	C* LinkedList<C>::append()
{
	return append(SHIELDED(new Type()));
}

template <class C> INLINE void LinkedList<C>::import(LinkedList<C>&other)
{
	if (&other == this)
	{
	#if DEBUG_LEVEL >= 1
		FATAL__("trying to re-import local content");
	#else
		return;
	#endif
	}
	Carrier*c = other.pivot->next;
	while (c != &other.pivot)
	{
		push_back(c->element);
		c = c->next;
	}
}

template <class C>
template <class List> INLINE void LinkedList<C>::import(List&list)
{
	if (&list == this)
	{
	#if DEBUG_LEVEL >= 1
		FATAL__("trying to re-import local content");
	#else
		return;
	#endif
	}
	for (typename List::iterator i = list.begin(); i != list.end(); ++i)
		push_back(*i);
}


template <class C>
template <class List> INLINE void LinkedList<C>::importAndFlush(List&list)
{
	if (&list == this)
	{
	#if DEBUG_LEVEL >= 1
		FATAL__("trying to re-import local content");
	#else
		return;
	#endif
	}
	import(list);
	list.flush();
}


template <class C> INLINE	C* LinkedList<C>::insert(size_t index, C*Element)
{
	if (!seek(index))
		return append(Element);
	carrier = MAKE_CARRIER;
	carrier->element = Element;

	carrier->prev = seeker->prev;
	seeker->prev->next = carrier;

	carrier->next = seeker;
	seeker->prev = carrier;

	seeker = carrier;
	elements++;

	return Element;
}

template <class C> INLINE	C* LinkedList<C>::insert(size_t index)
{
	return insert(index,SHIELDED(new Type()));
}

template <class C> INLINE	C* LinkedList<C>::last()
{
	return pivot.prev->element;
}

template <class C> INLINE	const C* LinkedList<C>::last() const
{
	return pivot.prev->element;
}




template <class C> INLINE C* LinkedList<C>::first()
{
	return pivot.next->element;
}

template <class C> INLINE const C* LinkedList<C>::first() const
{
	return pivot.next->element;
}


template <class C> INLINE	C* LinkedList<C>::get(size_t Index)
{
	if (!seek(Index))
		return NULL;
	return seeker->element;
}

template <class C> INLINE const C*		LinkedList<C>::get(size_t index)					const
{
	return getConst(index);
}


template <class C> INLINE typename LinkedList<C>::Carrier* LinkedList<C>::getCarrier(size_t index)
{
	if (!seek(index))
		return NULL;
	return seeker;
}

template <class C> INLINE LinkedIterator<LinkedCarrier<C>,C*>	 LinkedList<C>::getIterator(size_t index)
{
	return getCarrier(index);
}

template <class C> INLINE LinkedIterator<const LinkedCarrier<C>,const C*>	 LinkedList<C>::getIterator(size_t index)			const
{
	if (index >= elements)
		return NULL;
	if (seeker)
	{
		Carrier*carrier = seeker;
		size_t at = seeker_index;

		while (at < index)
		{
			carrier = carrier->next;
			at++;
		}
		while (at > index)
		{
			carrier = carrier->prev;
			at--;
		}
		return carrier;
	}
	Carrier*carrier = pivot.next;
	size_t _index = 0;
	while (_index < index)
	{
		_index++;
		carrier = carrier->next;
	}
	return carrier;
}


template <class C> INLINE	const C* LinkedList<C>::getConst(size_t Index) const
{
	if (Index >= elements)
		return NULL;
	
	Carrier*carrier = pivot.next;
	size_t index = 0;
	while (index < Index)
	{
		index++;
		carrier = carrier->next;
	}
	return carrier->element;
}

template <class C> INLINE	size_t LinkedList<C>::getIndexOf(const C*Element) const
{
	Carrier*carrier = pivot.next;
	size_t index = 0;
	while (carrier != &pivot)
	{
		index++;
		if (carrier->element == Element)
			return index;
		carrier = carrier->next;
	}
	return 0;
}

template <class C> INLINE	void LinkedList<C>::set(size_t Index, C*Element)
{
	if (seek(Index))
		seeker->element = Element;
}

template <class C> INLINE	size_t LinkedList<C>::count() const
{
	return elements;
}

template <class C> INLINE	void	LinkedList<C>::flush()
{
	Carrier*carrier = pivot.next;
	while (carrier != &pivot)
	{
		Carrier*next = carrier->next;
		UNMAKE_CARRIER(carrier);
		carrier = next;
	}
	pivot.next = &pivot;
	pivot.prev = &pivot;
	seeker = NULL;
	elements = 0;
}


template <class C> INLINE	bool LinkedList<C>::erase(size_t Index)
{
	if (seek(Index))
	{
		DISCARD(seeker->element);
		drop(seeker);
		return true;
	}
	return false;
}

template <class C> INLINE	bool LinkedList<C>::erase(C*element)
{
	Carrier*carrier = pivot.next;
	while (carrier != &pivot && carrier->element != element)
		carrier = carrier->next;
	if (carrier->element != element)
		return false;

	DISCARD(element);
	drop(carrier);
	return true;
}

template <class C> INLINE	C* LinkedList<C>::drop(size_t Index)
{
	if (seek(Index))
	{
		C*el = seeker->element;
		drop(seeker);
		return el;
	}
	return NULL;
}

template <class C> INLINE	C* LinkedList<C>::drop(C*element)
{
	Carrier*carrier = pivot.next;
	while (carrier != &pivot && carrier->element != element)
		carrier = carrier->next;
	if (carrier->element != element)
		return NULL;

	drop(carrier);
	return element;
}

template <class C> INLINE	C* LinkedList<C>::dropFirst()
{
	if (!elements)
		return NULL;
	C*rs = pivot.next->element;
	drop(pivot.next);
	return rs;
}

template <class C> INLINE	C* LinkedList<C>::dropLast()
{
	if (!elements)
		return NULL;
	C*rs = pivot.prev->element;
	drop(pivot.prev);
	return rs;
}

template <class C> INLINE	void LinkedList<C>::swap(size_t Index1, size_t Index2)
{
	if (Index1 == Index2 || !seek(Index1))
		return;
	Carrier*carrier = seeker;
	if (!seek(Index2))
		return;
	C*s = carrier->element;
	carrier->element = seeker->element;
	seeker->element = s;
}

template <class C> INLINE		void	LinkedList<C>::revert()
{
	Carrier	*c0 = pivot.next,
						*c1 = pivot.prev;
	size_t steps = elements/2;
	for (size_t i = 0; i < steps; i++)
	{
		swp(c0->element,c1->element);
		c0 = c0->next;
		c1 = c1->prev;
	}
}

template <class C> INLINE void LinkedList<C>::swap(LinkedList<C>&other)
{
	pivot.swap(other.pivot);
	if (pivot.next != &other.pivot)
	{
		pivot.prev->next = &pivot;
		pivot.next->prev = &pivot;
	}
	else
	{
		pivot.prev = &pivot;
		pivot.next = &pivot;
	}

	if (other.pivot.next != &pivot)
	{
		other.pivot.next->prev = &other.pivot;
		other.pivot.prev->next = &other.pivot;
	}
	else
	{
		other.pivot.prev = &other.pivot;
		other.pivot.next = &other.pivot;
	}

	seeker = NULL;
	other.seeker = NULL;
	cursor = &pivot;
	other.cursor = &other.pivot;

	carrier = NULL;
	other.carrier = NULL;

	swp(elements,other.elements);
	swp(seeker_index,other.seeker_index);

	if (!elements)
	{
		ASSERT_EQUAL__(&pivot,pivot.next);
		ASSERT_EQUAL__(&pivot,pivot.prev);
	}
	if (!other.elements)
	{
		ASSERT_EQUAL__(&other.pivot,other.pivot.next);
		ASSERT_EQUAL__(&other.pivot,other.pivot.prev);
	}

}

template <class C> INLINE	void LinkedList<C>::reset()
{
	cursor = &pivot;
}

template <class C> INLINE	C* LinkedList<C>::each()
{
	cursor = cursor->next;
	return cursor->element;
}

template <class C> INLINE	typename LinkedList<C>::Carrier* LinkedList<C>::eachCarrier()
{
	cursor = cursor->next;
	return cursor == &pivot?cursor:NULL;
}


template <class C> INLINE	C* LinkedList<C>::drop()
{
	cursor = drop(cursor)->prev;
}

template <class C> INLINE	void LinkedList<C>::erase()
{
	DISCARD(cursor->element);
	cursor = drop(cursor)->prev;
}

template <class C> INLINE	C* LinkedList<C>::include()
{
	return include(SHIELDED(new Type()));
}


template <class C> INLINE	void LinkedList<C>::include(Type*element)
{
	carrier = MAKE_CARRIER;
	carrier->element = element;

	carrier->prev = cursor->prev;
	cursor->prev->next = carrier;

	carrier->next = cursor;
	cursor->prev = carrier;

	cursor = carrier;
	elements++;
}

template <class C> INLINE	LinkedIterator<LinkedCarrier<C>,C*>	 LinkedList<C>::insert(const iterator&it, C*element)
{
	carrier = MAKE_CARRIER;
	carrier->element = element;
	elements++;
	Carrier*c = it.carrier;

	carrier->prev = c->prev;
	c->prev->next = carrier;
	carrier->next = c;
	c->prev = carrier;
	
	return carrier;
	
/*
	_Node* __tmp = _M_create_node(__x);
	__tmp->_M_next = __position._M_node;
	__tmp->_M_prev = __position._M_node->_M_prev;
	__position._M_node->_M_prev->_M_next = __tmp;
	__position._M_node->_M_prev = __tmp;
	return __tmp;*/
}

template <class C> INLINE void	LinkedList<C>::relocate(Carrier*carrier, Carrier*before)
{
	carrier->next->prev = carrier->prev;
	carrier->prev->next = carrier->next;
	
	carrier->next = before;
	carrier->prev = before->prev;
	
	before->prev->next = carrier;
	before->prev = carrier;
}


template <class C> INLINE	C* LinkedList<C>::insertBeforeCarrier(Carrier*c, C*element)
{
	carrier = MAKE_CARRIER;
	carrier->element = element;

	carrier->prev = c->prev;
	c->prev->next = carrier;

	carrier->next = c;
	c->prev = carrier;

	elements++;
	return element;
}

template <class C> INLINE	C* LinkedList<C>::insertBehindCarrier(Carrier*c, C*element)
{
	carrier = MAKE_CARRIER;
	carrier->element = element;

	carrier->next = c->next;
	carrier->next->prev = carrier;

	carrier->prev = c;
	c->next = carrier;

	elements++;
	return element;
}

template <class C> INLINE	C* LinkedList<C>::insertBeforeCarrier(Carrier*carrier)
{
	return insertBeforeCarrier(carrier,SHIELDED(new Type()));
}

template <class C> INLINE	C* LinkedList<C>::insertBehindCarrier(Carrier*carrier)
{
	return insertBehindCarrier(carrier, SHIELDED(new Type()));
}



template <class C> INLINE	C* LinkedList<C>::operator[](size_t Index)
{
	return get(Index);
}

template <class C> INLINE	const C* LinkedList<C>::operator[](size_t Index) const
{
	return getConst(Index);
}

template <class C> INLINE	size_t LinkedList<C>::operator()(const C*element) const
{
	return getIndexOf(element);
}

template <class C> INLINE		LinkedList<C>::operator size_t()				const
{
	return elements;
}


template <class C> INLINE size_t LinkedList<C>::totalSize()	const
{
	return sizeof(*this)+(size_t)elements*sizeof(Carrier);
}

template <class C> INLINE	typename LinkedList<C>::Carrier*	LinkedList<C>::getCarrier()
{
	return carrier;
}

template <class C> INLINE	const typename LinkedList<C>::Carrier*	LinkedList<C>::getCarrier() const
{
	return carrier;
}

template <class C> INLINE	void LinkedList<C>::eraseCarrier(Carrier*c)
{
	DISCARD(c->element);
	drop(c);
}

template <class C> INLINE C*	LinkedList<C>::dropCarrier(Carrier*c)
{
	C* rs = c->element;
	drop(c);
	return rs;
}

template <class C> INLINE	LinkedIterator<LinkedCarrier<C>,C*>	 LinkedList<C>::erase(const iterator&it)
{
	seeker = NULL;
	Carrier*carrier = it.carrier;
	carrier->prev->next = carrier->next;
	carrier->next->prev = carrier->prev;

	Carrier*next = carrier->next;
	UNMAKE_CARRIER(carrier);
	elements--;
	return next;
}

template <class C> INLINE	LinkedIterator<LinkedCarrier<C>,C*>	 LinkedList<C>::erase(const iterator&from, const iterator&to)
{
	seeker = NULL;
	Carrier*c0 = from.carrier,
						*c1 = to.carrier;
	while (c0 != c1 && c0 != &pivot)
	{
		Carrier*n = c0->next;
		eraseCarrier(c0);
		c0 = n;
	}
	return iterator(c1);
}

template <class C> INLINE	LinkedIterator<LinkedCarrier<C>,C*>	 LinkedList<C>::drop(const iterator&it)
{
	Carrier*next = it.carrier->next;
	dropElement(it.carrier);
	return next;
}


template <class C> INLINE	LinkedIterator<const LinkedCarrier<C>,const C*>	 LinkedList<C>::begin() const
{
	return pivot.next;
}

template <class C> INLINE	LinkedIterator<const LinkedCarrier<C>,const C*>	 LinkedList<C>::end() const
{
	return &pivot;
}

template <class C> INLINE	LinkedIterator<LinkedCarrier<C>,C*>	 LinkedList<C>::begin()
{
	return pivot.next;
}

template <class C> INLINE	LinkedIterator<LinkedCarrier<C>,C*>	 LinkedList<C>::end()
{
	return &pivot;
}



template <class C> void LinkedList<C>::check()
{
	/*
	bool seeker_found(false);
	size_t at(0);
	Carrier*	carrier = pivot.next;
	while (carrier != &pivot)
	{
		if (carrier == seeker)
		{
			seeker_found = true;
			if (at != seeker_index)
				cout << "seeker-index incorrect (should be "<<at<<" but is "<<seeker_index<<")\n";
		}
		if (carrier->prev->next != carrier)
		{
			cout << "list-syntax broken at "<<at<<endl;
		}
		carrier = carrier->next;
		at++;
	}
	if (at != elements)
		cout << "count incorrect (should be "<<at<<" but is "<<elements<<")\n";
	if (!seeker_found && elements && seeker != &pivot)
		cout << "seeker not found\n";
	*/
}

template <class C> ReferenceLinkedList<C>::~ReferenceLinkedList()
{
	LinkedList<C>::flush();
}

template <class C> INLINE LinkedList<C>& ReferenceLinkedList<C>::operator=(const LinkedList<C>&other)
{
	LinkedList<C>::flush();
	LinkedList<C>::append(other);
	return *this;
}

template <class C> INLINE	C*ReferenceLinkedList<C>::operator[](size_t Index)
{
	return LinkedList<C>::get(Index);
}

template <class C> INLINE	const C*ReferenceLinkedList<C>::operator[](size_t Index) const
{
	return LinkedList<C>::get(Index);
}

template <class C> INLINE	size_t ReferenceLinkedList<C>::operator()(const C*element) const
{
	return LinkedList<C>::getIndexOf(element);
}



#endif

