#ifndef arrayListTplH
#define arrayListTplH

/******************************************************************

Implementation of the ArrayList-class.

******************************************************************/



#define MAKE_SECTION				SHIELDED(new SArraySection<C,Size>())
#define UNMAKE_SECTION(section)		DISCARD(section)

static inline size_t lmin(size_t v0, size_t v1)
{
	return v0 < v1?v0:v1;
}


template <class C, size_t Size> INLINE void ArrayList<C,Size>::flush()
{
	Section s = map[0];
	for (size_t i = 0; i < sections; i++)
	{
		Section next = s->next;
		UNMAKE_SECTION(s);
		s = next;
	}
	sections = 0;
	elements = 0;
	Fill = ceiling = NULL;
	lookup = NULL;
	top = NULL;
}

template <class C, size_t Size> INLINE void ArrayList<C,Size>::clear()
{
	Section s = map[0];
	size_t el(0);
	for (size_t i = 0; i < sections; i++)
	{
		Section next = s->next;
		for (size_t j = 0; j < Size && el < elements; j++, el++)
			DISCARD(s->field[j]);
		UNMAKE_SECTION(s);
		s = next;
	}
	sections = 0;
	elements = 0;
	Fill = ceiling = NULL;
	lookup = NULL;
	top = NULL;
}


template <class C, size_t Size> INLINE typename ArrayList<C,Size>::Section	ArrayList<C,Size>::section(size_t sid)
{
	if (sid < Size)
		return map[sid];

	if (!lookup)
	{
		lookup = map[Size-1];
		lookup_id = Size-1;
	}
	while (sid > lookup_id)
	{
		lookup = lookup->next;
		++lookup_id;
	}
	while (sid < lookup_id)
	{
		lookup = lookup->prev;
		--lookup_id;
	}
	return lookup;
}


template <class C, size_t Size> INLINE const typename ArrayList<C,Size>::Section	ArrayList<C,Size>::section(size_t sid) const
{
	if (sid < Size)
		return map[sid];
		
	const Section s = map[Size-1]->next;
	sid-=Size;
	while (sid--)
		s = s->next;
	return s;
}

template <class C, size_t Size> INLINE void ArrayList<C,Size>::push_front(Type*element)
{
	insert(0U,element);
}

template <class C, size_t Size> inline void ArrayList<C,Size>::push_back(C*element)
{
	if (Fill == ceiling)
	{
//		cout << "	new section ("<<sections<<")\n";
		if (!top)
		{
//			cout << "	(root)\n";
			top = MAKE_SECTION;
			map[0] = top;
//			cout << "	done\n";
		}
		else
		{
//			cout << "	creating...\n";
			Section section = MAKE_SECTION;
//			cout << "	created\n";
			top->next = section;
			section->prev = top;
//			cout << "	linked\n";
			top = section;
//			cout << "	set\n";
			if (sections < Size)
			{
				map[sections] = section;
//				cout << "	mapped\n";
			}
		}
		++sections;
		Fill = root = top->field;
		ceiling = top->field+Size;
//		cout << "	done\n";
	}
	(*Fill) = element;
	++Fill;
	elements++;
//	cout << "	done ("<<(Fill-root)<<" / "<<elements<<")\n";
}

template <class C, size_t Size> INLINE		C* ArrayList<C,Size>::append(C*element)
{
	if (Fill == ceiling)
	{
		if (!top)
		{
			top = MAKE_SECTION;
			map[0] = top;
		}
		else
		{
			Section section = MAKE_SECTION;
			top->next = section;
			section->prev = top;
			top = section;
			if (sections < Size)
				map[sections] = section;
		}
		++sections;
		Fill = root = top->field;
		ceiling = top->field+Size;
	}
	(*Fill) = element;
	++Fill;
	elements++;
	return element;
}

template <class C, size_t Size>
template <size_t RSize> INLINE void ArrayList<C,Size>::import(ArrayList<C,RSize>&other)
{
	if (&other == this)
	{
	#if DEBUG_LEVEL >= 1
		FATAL__("trying to re-import local content");
	#else
		return;
	#endif
	}


	size_t el(0);
	SArraySection<C,RSize>*s = other.map[0];
	for (size_t i = 0; i < other.sections; i++)
	{
		for (size_t j = 0; j < Size && el < other.elements; j++, el++)
			push_back(s->field[j]);
		s = s->next;
	}
}

template <class C, size_t Size>
template <class List> INLINE	void ArrayList<C,Size>::import(List&list)
{
	if (&list == this)
	{
	#if DEBUG_LEVEL >= 1
		FATAL__("trying to re-import local content");
	#else
		return;
	#endif
	}

	for (typename List::const_iterator i = list.begin(); i != list.end(); ++i)
		push_back(*i);
}


template <class C, size_t Size>
template <class List> INLINE	void ArrayList<C,Size>::importAndFlush(List&list)
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




template <class C, size_t Size> INLINE	C* ArrayList<C,Size>::append()
{
	Type*rs = SHIELDED(new Type());
	push_back(rs);
	return rs;
}

template <class C, size_t Size> INLINE	C*ArrayList<C,Size>::insert(UINT32 index, C*element)
{
	if (index >= elements)
		return append(element);
		
	UINT32 main_ = index/Size,
			sub = index%Size;
//	cout << "inserting at "<<index<<" / "<<elements<<endl;
	push_back(NULL);
//	cout << "appended\n";
	Section s = section(main_);
//	cout << " got secion\n";
	if (s == top)
	{
//		cout << " top section\n";
		movePointersBack(s->field+sub,s->field+sub+1,elements-index-1);
		s->field[sub] = element;
//		cout << "	done\n";
		return element;
	}

//	cout << " inter section\n";
	Type*backup0 = s->field[Size-1];
	movePointersBack(s->field+sub,s->field+sub+1,Size-sub-1);
//	cout << " copy0\n";
	s->field[sub] = element;
	s = s->next;
	while (s != top)
	{
		Type*backup1 = s->field[Size-1];
		movePointersBack<C,Size-1>(s->field,s->field+1);
		s->field[0] = backup0;
		backup0 = backup1;
		s = s->next;
//		cout << " lopped\n";
	}
	if (elements%Size)
		movePointersBack(s->field,s->field+1,(elements%Size)-1);
	s->field[0] = backup0;
//	cout << "	done\n";
	return element;
}

template <class C, size_t Size> INLINE C* ArrayList<C,Size>::insert(UINT32 index)
{
	return insert(index, SHIELDED(new Type()));
}

template <class C, size_t Size> INLINE	ListIterator<ArrayList<C,Size> > ArrayList<C,Size>::insert(const iterator&it, C*element)
{
	insert(it.index,element);
	return it;
}


template <class C, size_t Size> INLINE C* ArrayList<C,Size>::drop(UINT32 index)
{
	if (index >= elements)
		return NULL;

	UINT32 main_ = index/Size,
			sub = index%Size;

	Section s = section(main_);
	Type*result = s->field[sub];

//	cout << "killing "<< index << endl;
	if (s == top)
	{
//		cout << "top-entry\n";
		movePointers(s->field+sub+1,s->field+sub,elements-index-1);
//		cout << "copied\n";
	}
	else
	{
//		cout << "inter-entry\n";
		movePointers(s->field+sub+1,s->field+sub,Size-sub-1);
//		cout << "moved0\n";
		s->field[Size-1] = s->next->field[0];
		s = s->next;
//		cout << "increased\n";
		while (s != top)
		{
//			cout << " beginning step\n";
			movePointers<C,Size-1>(s->field+1,s->field);
			s->field[Size-1] = s->next->field[0];
			s = s->next;
//			cout << " step done\n";
		}
		if (elements%Size)
			movePointers(s->field+1,s->field,(elements%Size)-1);
//		cout << "copied\n";
	}
	--Fill;
	if (Fill == root)
	{
//		cout << "decreasing ("<<sections<<")";
		if (--sections)
		{
//			cout << " to "<<sections<<endl;
			if (lookup == top)
			{
				lookup = lookup->prev;
				--lookup_id;
			}
			top = top->prev;
			UNMAKE_SECTION(top->next);
			root = top->field;
			ceiling = top->field+Size;
			Fill = ceiling;
		}
		else
		{
//			cout << " to zero\n";
			UNMAKE_SECTION(top);
			top = NULL;
			root = Fill = ceiling = NULL;
			lookup = NULL;
		}
	}
	--elements;
//		cout << "elements now " << elements << endl;
	return result;
}

template <class C, size_t Size> INLINE ListIterator<ArrayList<C,Size> > ArrayList<C,Size>::drop(const iterator&it)
{
	drop(it.index);
	return it;
}

template <class C, size_t Size> INLINE C* ArrayList<C,Size>::drop(Type*element)
{
	UINT32 index = getIndexOf(element);
	if (!index)
		return NULL;
	return drop(index-1);
}

template <class C, size_t Size> INLINE bool ArrayList<C,Size>::erase(UINT32 index)
{
	C*element = drop(index);
	if (!element)
		return false;
	DISCARD(element);
	return true;
}

template <class C, size_t Size> INLINE ListIterator<ArrayList<C,Size> > ArrayList<C,Size>::erase(const iterator&it)
{
	erase(it.index);
	return it;
}

template <class C, size_t Size> INLINE ListIterator<ArrayList<C,Size> > ArrayList<C,Size>::erase(const iterator&from, const iterator&to)
{
	if (to <= from)
		return from;

	UINT32 count = to-from,
		index = from.index;
	while (elements > index && count)
	{
		erase(index);
		--count;
	}
	return from;
}

template <class C, size_t Size> INLINE bool ArrayList<C,Size>::erase(Type*element)
{
	return erase(getIndexOf(element)-1);
}




template <class C, size_t Size> ArrayList<C,Size>::ArrayList():top(NULL),lookup(NULL),cursor(NULL),elements(0),sections(0),root(NULL),Fill(NULL),ceiling(NULL)
{}

template <class C, size_t Size>
template <size_t RSize> ArrayList<C,Size>::ArrayList(const ArrayList<C,RSize>&other):top(NULL),lookup(NULL),cursor(NULL),elements(0),sections(0),root(NULL),Fill(NULL),ceiling(NULL)
{
	append(other);
}


template <class C, size_t Size> ArrayList<C,Size>::~ArrayList()
{
	clear();
}

template <class C, size_t Size>
template <size_t RSize> ArrayList<C,Size>& ArrayList<C,Size>::operator=(const ArrayList<C,RSize>&other)
{
	clear();
	append(other);
	return *this;
}



template <class C, size_t Size> INLINE		void ArrayList<C,Size>::set(UINT32 index, Type*element)
{
	if (index >= elements)
		return;
	section(index/Size)->field[index&Size] = element;
}


template <class C, size_t Size> INLINE void ArrayList<C,Size>::swap(UINT32 index0, UINT32 index1)
{
	if (index0 >= elements || index1 >= elements || index0 == index1)
		return;
	Type*temp = get(index0);
	set(index0,get(index1));
	set(index1,temp);
}

template <class C, size_t Size> INLINE void ArrayList<C,Size>::revert()
{
	for (size_t i = 0; i < elements/2; i++)
	{
		Type*temp = get(i);
		set(i,get(elements-i-1));
		set(elements-i-1,temp);
	}
}


template <class C, size_t Size> INLINE void ArrayList<C,Size>::swap(ArrayList<C,Size>&other)
{
	swp(top,other.top);
	swp(lookup,other.lookup);
	swp(cursor,other.cursor);
	swp(lookup_id,other.lookup_id);
	swp(cursor_id,other.cursor_id);
	swp(elements,other.elements);
	swp(sections,other.sections);
	swp(root,other.root);
	swp(Fill,other.Fill);
	swp(ceiling,other.ceiling);
	
	bool left = other.sections < sections;
	size_t	doswap = left?other.sections:sections,
				domove = (sections+other.sections)-2*doswap;
	if (doswap > Size)
		doswap = Size;
	if (doswap + domove > Size)
		domove = Size-doswap;
		
	for (size_t i = 0; i < doswap; i++)
		swp(map[i],other.map[i]);
		
	if (left)
		memcpy(map+doswap,other.map+doswap,domove*sizeof(C*));
	else
		memcpy(other.map+doswap,map+doswap,domove*sizeof(C*));
}


template <class C, size_t Size> INLINE C* ArrayList<C,Size>::get(UINT32 index)
{
	if (index >= elements)
		return NULL;
	return section(index/Size)->field[index%Size];
}

template <class C, size_t Size> INLINE const C* ArrayList<C,Size>::get(UINT32 index) const
{
	if (index >= elements)
		return NULL;
	return section(index/Size)->field[index%Size];
}


template <class C, size_t Size> INLINE	const C* ArrayList<C,Size>::getConst(UINT32 index)			const
{
	if (index >= elements)
		return NULL;
	UINT32 sid = index/Size;
	
	if (sid < Size)
		return map[sid]->field[index%Size];
		
	Section s = map[Size-1]->next;
	sid-=Size;
	while (sid--)
		s = s->next;
	return s->field[index%Size];
}


template <class C, size_t Size> INLINE C*& ArrayList<C,Size>::getReference(UINT32 index)
{
	if (index >= elements)
		FATAL__("bad reference-lookup!");
	UINT32 sid = index/Size;

	if (sid < Size)
		return map[sid]->field[index%Size];

	Section s = map[Size-1]->next;
	sid-=Size;
	while (sid--)
		s = s->next;
	return s->field[index%Size];
}

template <class C, size_t Size> INLINE	ListIterator<ArrayList<C,Size> >	ArrayList<C,Size>::getIterator(UINT32 index)
{
	return iterator(this,index);
}

template <class C, size_t Size> INLINE	ConstListIterator<ArrayList<C,Size> >	ArrayList<C,Size>::getIterator(UINT32 index)			const
{
	return const_iterator(this,index);
}

template <class C, size_t Size> INLINE		UINT32 ArrayList<C,Size>::getIndexOf(const Type*element) const
{
	Section s = map[0];
	size_t el(0);
	for (size_t i = 0; i < sections; i++)
	{
		for (size_t j = 0; j < Size && el < elements; j++, el++)
			if (s->field[j] == element)
				return el+1;
		s = s->next;
	}
	return 0;
}

template <class C, size_t Size> INLINE		C*	ArrayList<C,Size>::first()
{
	if (!elements)
		return NULL;
	return map[0]->field[0];
}

template <class C, size_t Size> INLINE		const C*	ArrayList<C,Size>::first()							const
{
	if (!elements)
		return NULL;
	return map[0]->field[0];
}

template <class C, size_t Size> INLINE		C*	ArrayList<C,Size>::last()
{
	if (!elements)
		return NULL;
	return *(Fill-1);
}

template <class C, size_t Size> INLINE		const C*	ArrayList<C,Size>::last()							const
{
	if (!elements)
		return NULL;
	return *(Fill-1);
}


template <class C, size_t Size> INLINE		ListIterator<ArrayList<C,Size> > ArrayList<C,Size>::begin()
{
	return iterator(this,0);
}

template <class C, size_t Size> INLINE		ConstListIterator<ArrayList<C,Size> > ArrayList<C,Size>::begin() const
{
	return const_iterator(this,0);
}

template <class C, size_t Size> INLINE		ListIterator<ArrayList<C,Size> > ArrayList<C,Size>::end()
{
	return iterator(this,elements);
}

template <class C, size_t Size> INLINE		ConstListIterator<ArrayList<C,Size> > ArrayList<C,Size>::end() const
{
	return const_iterator(this,elements);
}

template <class C, size_t Size> INLINE		C* ArrayList<C,Size>::operator[](size_t index)
{
	if (index >= elements)
		return NULL;
	return section(index/Size)->field[index%Size];
}

template <class C, size_t Size> INLINE		const C* ArrayList<C,Size>::operator[](size_t index) const
{
	if (index >= elements)
		return NULL;
	return section(index/Size)->field[index%Size];
}

template <class C, size_t Size> INLINE		UINT32 ArrayList<C,Size>::operator()(const Type*el)			const
{
	return getIndexOf(el);
}

template <class C, size_t Size> INLINE		ArrayList<C,Size>::operator				size_t()							const
{
	return elements;
}



template <class C, size_t Size> INLINE		UINT32	ArrayList<C,Size>::count()							const
{
	return elements;
}

template <class C, size_t Size> INLINE		size_t	ArrayList<C,Size>::totalSize()						const
{
	return sizeof(*this)+sizeof(SArraySection<C,Size>)*(size_t)sections;
}

template <class C, size_t Size> INLINE		void ArrayList<C,Size>::reset()
{
	if (!top)
	{
		cursor = NULL;
		cursor_limit = NULL;
		cursor_id = NULL;
	}
	else
	{
		cursor = map[0];
		cursor_id = cursor->field;
		cursor_limit = cursor_id+Size;
	}
}

template <class C, size_t Size> INLINE		C* ArrayList<C,Size>::each()
{
	if (cursor_id == cursor_limit)
	{
		if (cursor == top)
			return NULL;
		cursor = cursor->next;
		cursor_id = cursor->field;
		cursor_limit = cursor_id+Size;
	}
	return *cursor_id++;
}

template <class C, size_t Size> INLINE		C* ArrayList<C,Size>::drop()
{
	Type*result = *(cursor_id-1);
	if (cursor == top)
		movePointers(cursor_id,cursor_id-1,Fill-cursor_id);
	else
	{
		size_t index = cursor_id - cursor->field;
		movePointers(cursor_id,cursor_id-1,Size-index);
		Section s = cursor;
		s->field[Size-1] = s->next->field[0];
		s = s->next;
		while (s != top)
		{
			movePointers<C,Size-1>(s->field+1,s->field);
			s->field[Size-1] = s->next->field[0];
			s = s->next;
		}
		if (elements%Size)
			movePointers(s->field+1,s->field,(elements%Size)-1);
	}
	--Fill;
	if (Fill == root)
	{
		if (--sections)
		{
			if (lookup == top)
			{
				lookup = lookup->prev;
				--lookup_id;
			}
			top = top->prev;
			UNMAKE_SECTION(top->next);
			root = top->field;
			ceiling = top->field+Size;
			Fill = ceiling;
		}
		else
		{
			UNMAKE_SECTION(top);
			top = NULL;
			root = Fill = ceiling = NULL;
			lookup = NULL;
		}
	}
	--elements;
	return result;
}

template <class C, size_t Size> INLINE		void ArrayList<C,Size>::erase()
{
	DISCARD(drop());
}

template <class C, size_t Size> INLINE		C* ArrayList<C,Size>::include()
{
	insert(cursor_id);
	return each();
}

template <class C, size_t Size> INLINE		void ArrayList<C,Size>::include(Type*element)
{
	insert(cursor_id,element);
	return each();
}













/*








template <class C, size_t RSize>INLINE SArraySheet<C,Size>* ArrayList<C,RSize>::getSheet(size_t sheet_id)
{
	if (sheet_id < Multiplier)
		return direct_link[sheet_id];
	if (!last_found_id || last_found_id > sheet_id)
	{
		last_found = direct_link[Multiplier-1];
		last_found_id = Multiplier-1;
	}
	while (last_found_id < sheet_id)
	{
		if (!last_found->next)
			return NULL;
		last_found = last_found->next;
		last_found_id++;
	}
	return last_found;
}

template <class C, size_t RSize>ArrayList<C,RSize>::ArrayList():last_found(NULL),last_found_id(0),elements(0),writing(false),reading(0)
{
	direct_link[0] = NULL;
}

template <class C, size_t RSize>
template <size_t LSize> ArrayList<C,RSize>::ArrayList(const ArrayList<C,LSize>& other):last_found(NULL),last_found_id(0),elements(0),writing(false),reading(0)
{
	SArraySheet<C,Size>*sheet = other.direct_link[0];
	while (sheet)
	{
		for (size_t i = 0; i < LSize; i++)
			appendElement(new C(*sheet->array[i]));
		sheet = sheet->next;
	}
}

template <class C, size_t RSize>ArrayList<C,RSize>::~ArrayList()
{
	eraseElements();
}

template <class C, size_t RSize>INLINE C*ArrayList<C,RSize>::appendElement()
{
	return appendElement(new C());
}

template <class C, size_t RSize>inline C*ArrayList<C,RSize>::appendElement(C*Element)
{

	WRITE_BARRIER;
		size_t	sheet_id = elements/Size,
					element_id = elements%Size;
		elements++;
		if (sheet_id < Multiplier)
		{
			if (!element_id)
			{
				direct_link[sheet_id] = new SArraySheet<C,Size>();
				direct_link[sheet_id]->next = NULL;
				if (sheet_id)
					direct_link[sheet_id-1]->next = direct_link[sheet_id];
			}
			direct_link[sheet_id]->array[element_id] = Element;
			return Element;
		}
		if (!last_found_id)
		{
			last_found = direct_link[Multiplier-1];
			last_found_id = Multiplier-1;
		}
		while (last_found_id < sheet_id)
		{
			if (!last_found->next)
			{
				last_found->next = new SArraySheet<C,Size>();
				last_found->next->next = NULL;
			}
			last_found = last_found->next;
			last_found_id++;
		}
		last_found->array[element_id] = Element;
	WRITE_RELEASE;
	return Element;
}//should be ok

template <class C, size_t RSize>INLINE C* ArrayList<C,RSize>::getLastElement()
{
	return getElement(count()-1);
}

template <class C, size_t RSize>INLINE const C*ArrayList<C,RSize>::getLastElementConst() const
{
	return getElementConst(count()-1);
}


template <class C, size_t RSize>inline C*& ArrayList<C,RSize>::getReference(size_t Index)
{
	READ_BARRIER;
		if (Index >= elements)
			return NULL;
		size_t sheet_id = Index/Size,
					element_id = Index%Size;
		if (sheet_id < Multiplier)
		{
			Data&rs = direct_link[sheet_id]->array[element_id];
			READ_RELEASE;
			return rs;
		}
	READ_RELEASE;

	WRITE_BARRIER;


		if (!last_found_id||last_found_id > sheet_id)
		{
			last_found = direct_link[Multiplier-1];
			last_found_id = Multiplier-1;
		}
		while (last_found_id < sheet_id)
		{
			last_found = last_found->next;
			last_found_id++;
		}

		Data&rs=	last_found->array[element_id];

	WRITE_RELEASE;
	return rs;
}//should be ok

template <class C, size_t RSize>inline C* ArrayList<C,RSize>::getElement(size_t Index)
{
	C*rs;
	READ_BARRIER;
		if (Index >= elements)
			return NULL;
		size_t sheet_id = Index/Size,
					element_id = Index%Size;
		if (sheet_id < Multiplier)
		{
			rs = direct_link[sheet_id]->array[element_id];
			READ_RELEASE;
			return rs;
		}
	READ_RELEASE;
	
	WRITE_BARRIER;


		if (!last_found_id||last_found_id > sheet_id)
		{
			last_found = direct_link[Multiplier-1];
			last_found_id = Multiplier-1;
		}
		while (last_found_id < sheet_id)
		{
			last_found = last_found->next;
			last_found_id++;
		}
		
		rs=	last_found->array[element_id];
		
	WRITE_RELEASE;
	return rs;
}//should be ok

template <class C, size_t RSize>inline const C* ArrayList<C,RSize>::getElementConst(size_t Index) const
{
	READ_BARRIER;
		if (Index >= elements)
			return NULL;
		size_t sheet_id = Index/Size,
				element_id = Index%Size;
		if (sheet_id < Multiplier)
			return direct_link[sheet_id]->array[element_id];
		SArraySheet<C,Size>*el = direct_link[Multiplier-1]->next;
		for (size_t sheet = Multiplier; sheet < sheet_id && el; sheet++)
			el = el->next;
		C*rs = el?el->array[element_id]:NULL;
	READ_RELEASE;
	return rs;
}

template <class C, size_t RSize>inline size_t ArrayList<C,RSize>::getIndexOf(const C*Element)
{
	for (size_t i = 0; i < elements; i++)
		if (getElement(i) == Element)
			return i+1;
	return 0;
}//too simple to be buggy

template <class C, size_t RSize>inline size_t ArrayList<C,RSize>::getIndexOfConst(const C*Element) const
{
	for (size_t i = 0; i < elements; i++)
		if (getElementConst(i) == Element)
			return i+1;
	return 0;
}//too simple to be buggy

template <class C, size_t RSize>inline void ArrayList<C,RSize>::setElement(size_t Index, C*Element)
{
	READ_BARRIER;
		if (Index >= elements)
			return;
		size_t sheet_id = Index/Size,
					element_id = Index%Size;
		if (sheet_id < Multiplier)
		{
			direct_link[sheet_id]->array[element_id] = Element;
			READ_RELEASE;
			return;
		}
	READ_RELEASE;
	WRITE_BARRIER;
		if (!last_found_id||last_found_id > sheet_id)
		{
			last_found = direct_link[Multiplier-1];
			last_found_id = Multiplier-1;
		}
		while (last_found_id < sheet_id)
		{
			last_found = last_found->next;
			last_found_id++;
		}
		last_found->array[element_id] = Element;
	WRITE_RELEASE;
}//should be ok


template <class C, size_t RSize>INLINE	size_t ArrayList<C,RSize>::count() const
{
	return elements;
}//too simple to be buggy

template <class C, size_t RSize>inline void ArrayList<C,RSize>::dropElements()
{
	WRITE_BARRIER;
		SArraySheet<C,Size>*pntr = direct_link[0];
		while (pntr)
		{
			SArraySheet<C,Size>*old = pntr;
			pntr = pntr->next;
			delete old;
		}
		direct_link[0] = NULL;
		elements = 0;
		last_found = NULL;
		last_found_id = 0;
	WRITE_RELEASE;

}//simple - clean

template <class C, size_t RSize>inline void ArrayList<C,RSize>::eraseElements()
{
	WRITE_BARRIER;
		SArraySheet<C,Size>*pntr = direct_link[0];
		size_t cnt(0);
		while (pntr)
		{
			SArraySheet<C,Size>*old = pntr;
			pntr = pntr->next;
			for (size_t el = 0; el < Size && ++cnt <= elements; el++)
				if (old->array[el])
					delete old->array[el];
			delete old;
		}
		direct_link[0] = NULL;
		elements = 0;
		last_found = NULL;
		last_found_id = 0;
	WRITE_RELEASE;
}//simple - clean

template <class C, size_t RSize>inline void ArrayList<C,RSize>::eraseElement(size_t Index)
{
	delete dropElement(Index);
}//should be ok


template <class C, size_t RSize>inline bool ArrayList<C,RSize>::eraseElement(C*element)
{
	size_t index = getIndexOf(element);
	if (index)
	{
		eraseElement(index-1);
		return true;
	}
	return false;
}

template <class C, size_t RSize>INLINE C* ArrayList<C,RSize>::insertElement(size_t index)
{
	return insertElement(index, new C());
}

template <class C, size_t RSize>inline C* ArrayList<C,RSize>::insertElement(size_t Index, C*element)
{
	WRITE_BARRIER;
		if (Index >= elements)
			return appendElement(element);
		size_t sheet_id = Index/Size,
					element_id = Index%Size;
		SArraySheet<C,Size>*sheet;//,*before;
		if (sheet_id < Multiplier)
			sheet = direct_link[sheet_id];
		else
		{
			sheet = direct_link[Multiplier-1];
			for (size_t i = Multiplier-1; i < sheet_id; i++)
			{
				//before = sheet;
				sheet = sheet->next;
			}
		}
		//now we got the sheet where the element is in
		appendElement(NULL); //dummy-element
		C*over_element = sheet->array[Size-1];

		size_t	local = Size-element_id,
					global = elements-Index;
		movePointersBack(&sheet->array[element_id],&sheet->array[element_id+1],(global<local?global:local)-1);


		sheet->array[element_id] = element;
		sheet = sheet->next;
		while (sheet)
		{
			C*new_over_element = sheet->array[Size-1];
			if (sheet->next)
				movePointersBack(sheet->array,&sheet->array[1],Size-1);
			else
			{
				size_t local = elements%Size;
				if (local)
					movePointersBack(sheet->array,&sheet->array[1],(elements%Size)-1);
			}
			sheet->array[0] = over_element;
			over_element = new_over_element;
			sheet = sheet->next;
		}
	WRITE_RELEASE;
	return element;
}

template <class C, size_t RSize>inline C* ArrayList<C,RSize>::dropCurrent()
{

		seeking_at--;
		seek_cnt--;
		
		return dropElement(seek_cnt);
}

template <class C, size_t RSize>INLINE void ArrayList<C,RSize>::eraseCurrent()
{
	delete dropCurrent();
}

template <class C, size_t RSize>INLINE ListIterator<C,ArrayList<C,RSize> > ArrayList<C,RSize>::erase(const iterator&it)
{
	#ifndef FAKE_ERASE
		eraseElement(it.index);
	#else
		dropElement(it.index);
	#endif
	return iterator(this,it.index);
}

template <class C, size_t RSize>inline ListIterator<C,ArrayList<C,RSize> > ArrayList<C,RSize>::erase(const iterator&from, const iterator&to)
{
	int cnt = to-from;
	if (cnt < 0)
		return to;
	while (cnt--)
		eraseElement(from.index);
	return iterator(from.index);
}

template <class C, size_t RSize>inline ListIterator<C,ArrayList<C,RSize> > ArrayList<C,RSize>::drop(const iterator&it)
{
	dropElement(it.index);
	return it;
}


template <class C, size_t RSize>inline C* ArrayList<C,RSize>::dropElement(size_t Index)
{
	WRITE_BARRIER;
		if (Index >= elements)
			return NULL;
		size_t sheet_id = Index/Size,
					element_id = Index%Size;
		SArraySheet<C,Size>*sheet,*before;
		if (sheet_id < Multiplier)
		{
			sheet = direct_link[sheet_id];
			before = sheet_id?direct_link[sheet_id-1]:NULL;
		}
		else
		{
			sheet = direct_link[Multiplier-1];
			before = direct_link[Multiplier-2];
			for (size_t i = Multiplier-1; i < sheet_id; i++)
			{
				before = sheet;
				sheet = sheet->next;
			}
		}
		last_found_id = 0;
	//now we got the sheet where the element is in
		C*element = sheet->array[element_id];

		size_t	local = Size-element_id,
					global = elements-Index;
		movePointers(&sheet->array[element_id+1],&sheet->array[element_id],(local < global?local:global)-1);
//		memcpy(&sheet->array[element_id],&sheet->array[element_id+1],sizeof(C*)*(Size-element_id-1));
		while (sheet->next)
		{
			sheet->array[Size-1] = sheet->next->array[0];
			before = sheet;
			sheet = sheet->next;
			if (sheet->next)
				movePointers(&sheet->array[1],sheet->array,Size-1);
			else
			{
				local = elements%Size;
				if (local)
					movePointers(&sheet->array[1],sheet->array,local-1);

			}
//		memcpy(sheet->array,&sheet->array[1],(Size-1)*sizeof(C*));
			sheet->array[Size-1] = NULL;//double check here - in case !elements%Size
		}
		elements--;
		if (!(elements%Size))
		{
			if (before)
				before->next = NULL;
			delete sheet;
			size_t sid = elements/Size;
			if (sid < Multiplier)
				direct_link[sid] = NULL;
		}
	WRITE_RELEASE;
	return element;
}//should be ok

template <class C, size_t RSize>inline C* ArrayList<C,RSize>::dropElement(C*element)
{
	size_t index = getIndexOf(element);
	return dropElement(index-1);
}

template <class C, size_t RSize>inline void ArrayList<C,RSize>::swapElements(size_t Index1, size_t Index2)
{
	C*temp = getElement(Index1);
	setElement(Index1,getElement(Index2));
	setElement(Index2,temp);
}

template <class C, size_t RSize>INLINE C* ArrayList<C,RSize>::operator[](size_t Index)
{
	return getElement(Index);
}

template <class C, size_t RSize>INLINE size_t ArrayList<C,RSize>::operator[](const C*element)
{
	return getIndexOf(element);
}

template <class C, size_t RSize>
template <size_t LSize> inline ArrayList<C,RSize>& ArrayList<C,RSize>::operator=(const ArrayList<C,LSize>&other)
{
	eraseElements();
	SArraySheet<C,Size>*sheet = other.direct_link[0];
	while (sheet)
	{
		for (size_t i = 0; i < LSize; i++)
			appendElement(new C(*sheet->array[i]));
		sheet = sheet->next;
	}
	return *this;
}

template <class C, size_t RSize>inline void ArrayList<C,RSize>::reset()
{
	seeking_in = direct_link[0];
	seeking_at = 0;
	seek_cnt = 0;
} //no barriers here. each is insecure anyway. should not be used in multithreaded lists.

template <class C, size_t RSize>inline C* ArrayList<C,RSize>::each()
{
	seek_cnt++;
	if (seek_cnt > elements)
		return NULL;
	if (seeking_at >= Size)
	{
		seeking_at = 0;
		seeking_in = seeking_in->next;
	}
	return seeking_in->array[seeking_at++];
}


template <class C, size_t RSize>INLINE C* ArrayList<C,RSize>::insertBeforeCurrent(C*element)
{
	return insertElement(seek_cnt-1,element);
}

template <class C, size_t RSize>INLINE C* ArrayList<C,RSize>::insertBehindCurrent(C*element)
{
	return insertElement(seek_cnt,element);
}

template <class C, size_t RSize>INLINE C* ArrayList<C,RSize>::insertBeforeCurrent()
{
	return insertElement(seek_cnt-1);
}

template <class C, size_t RSize>INLINE C* ArrayList<C,RSize>::insertBehindCurrent()
{
	return insertElement(seek_cnt);
}

template <class C, size_t RSize>INLINE ListIterator<C,ArrayList<C,RSize> > ArrayList<C,RSize>::insert(const iterator&it, C*element)
{
	insertElement(it.index,element);
	return iterator(this,it.index);
}

template <class C, size_t RSize>INLINE ListIterator<C,ArrayList<C,RSize> > ArrayList<C,RSize>::insertBehindIterator(const iterator&it, C*element)
{
	insertElement(it.index+1,element);
	return iterator(this,it.index+1);
}

template <class C, size_t RSize>INLINE C* ArrayList<C,RSize>::insert(const iterator&it)
{
	return insertElement(it.index);
}

template <class C, size_t RSize>INLINE C* ArrayList<C,RSize>::insertBehindIterator(const iterator&it)
{
	return insertElement(it.index+1);
}

template <class C, size_t RSize>INLINE ListIterator<C,ArrayList<C,RSize> > ArrayList<C,RSize>::begin()
{
	return iterator(this,0);
}

template <class C, size_t RSize>INLINE ListIterator<C,ArrayList<C,RSize> > ArrayList<C,RSize>::end()
{
	return iterator(this,count());
}

template <class C, size_t RSize>inline size_t ArrayList<C,RSize>::totalSize()		const
{
	size_t num_sheets = elements/RSize;
	if (elements%RSize)
		num_sheets++;
	return sizeof(*this)+num_sheets*sizeof(SArraySheet<C,RSize>);
}

*/

//------------------------------------------------------------------------------------------------------------


template <class C, size_t Size>ReferenceArrayList<C,Size>::ReferenceArrayList()
{}

template <class C, size_t Size>
template <size_t RSize> ReferenceArrayList<C,Size>::ReferenceArrayList(const ArrayList<C,RSize>&other):ArrayList<C,Size>(other)
{}

template <class C, size_t Size>ReferenceArrayList<C,Size>::~ReferenceArrayList()
{
	ArrayList<C,Size>::flush();
}

template <class C, size_t Size>
template <size_t RSize>
INLINE ArrayList<C,Size>& ReferenceArrayList<C,Size>::operator=(const ArrayList<C,RSize>&other)
{
	ArrayList<C,Size>::flush();
	ArrayList<C,Size>::append(other);
	return *this;
}

template <class C, size_t Size> INLINE		C* ReferenceArrayList<C,Size>::operator[](size_t index)
{
	return ArrayList<C,Size>::get(index);
}

template <class C, size_t Size> INLINE		const C* ReferenceArrayList<C,Size>::operator[](size_t index) const
{
	return ArrayList<C,Size>::get(index);
}

template <class C, size_t Size> INLINE		UINT32 ReferenceArrayList<C,Size>::operator()(const Type*el)			const
{
	return ArrayList<C,Size>::getIndexOf(el);
}



#endif




