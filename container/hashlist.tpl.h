#ifndef hashlistTplH
#define hashlistTplH

/******************************************************************

E:\include\list\hashlist.tpl.h

******************************************************************/

template <class K, class C, class Hash, class KeyStrategy>
	GenericHashList<K,C,Hash,KeyStrategy>::GenericHashList(const GenericHashList<K,C,Hash,KeyStrategy>&other)
	{
		for (typename List::const_iterator i = other.List::begin(); i != other.List::end(); ++i)
		{
			K key;
			if (other.Table::findKeyOf(i.getCarrier(),key))
				append(key,SHIELDED(new C(**i)));
		}
	}

template <class K, class C, class Hash, class KeyStrategy>
	void	GenericHashList<K,C,Hash,KeyStrategy>::operator=(const ThisType&other)
	{
		clear();
		for (typename List::const_iterator i = other.List::begin(); i != other.List::end(); ++i)
		{
			K key;
			if (other.Table::findKeyOf(i.getCarrier(),key))
				append(key,SHIELDED(new C(**i)));
		}
	}

template <class K, class C, class Hash, class KeyStrategy> template <class Entry>
	inline	void	GenericHashList<K,C,Hash,KeyStrategy>::exportTo(ArrayData<Entry*>&values)
	{
		values.reset(List::count());
		for (size_t i = 0; i < List::count(); i++)
			values[i] = List::get(i);
	}
	
template <class K, class C, class Hash, class KeyStrategy> template <class Key, class Entry>
	inline	void	GenericHashList<K,C,Hash,KeyStrategy>::exportTo(ArrayData<Key>&keys, ArrayData<Entry*>&values)
	{
		Array<ListCarrier*>	elements;
		Table::exportTo(keys,elements);
		values.SetSize(elements.count());
		for (size_t i = 0; i < elements.count(); i++)
			values[i] = elements[i]->getElement();
	}
	
template <class K, class C, class Hash, class KeyStrategy> template <class Key, class Entry>
	inline	void	GenericHashList<K,C,Hash,KeyStrategy>::exportTo(ArrayData<Key>&keys, ArrayData<const Entry*>&values) const
	{
		Array<const ListCarrier*>	elements;
		Table::exportTo(keys,elements);
		values.SetSize(elements.count());
		for (size_t i = 0; i < elements.count(); i++)
			values[i] = elements[i]->getElement();
	}
	

template <class K, class C, class Hash, class KeyStrategy>
	inline void GenericHashList<K,C,Hash,KeyStrategy>::import(GenericHashList<K,C,Hash,KeyStrategy>&list)
	{
		for (size_t i = 0; i < list.Table::array.length(); i++)
			if (list.Table::array[i].occupied)
			{
				TableCarrier*remote = list.array+i;
				bool did_occupy;
				TableCarrier*local = find(remote->hashed,remote->key,true,&did_occupy);
				if (did_occupy)
				{
					List::append(list.List::dropCarrier(remote->cast()));
					local->cast() = List::getCarrier();
					remote->occupied = false;
					remote->cast() = NULL;
				}
			}
	}

template <class K, class C, class Hash, class KeyStrategy>
	inline void GenericHashList<K,C,Hash,KeyStrategy>::importCopies(GenericHashList<K,C,Hash,KeyStrategy>&list)
	{
		for (size_t i = 0; i < list.Table::array.length(); i++)
			if (list.Table::array[i].occupied)
			{
				TableCarrier*remote = list.array+i;
				bool did_occupy;
				TableCarrier*local = find(remote->hashed,remote->key,true,&did_occupy);
				if (did_occupy)
				{
					List::append(SHIELDED(new C(remote->cast()->getElement())));
					local->cast() = List::getCarrier();
				}
			}
	}
	
	
template <class K, class C, class Hash, class KeyStrategy>
	inline	void GenericHashList<K,C,Hash,KeyStrategy>::clear()
	{
		Table::clear();
		List::clear();
	}

template <class K, class C, class Hash, class KeyStrategy>	
	inline	void GenericHashList<K,C,Hash,KeyStrategy>::flush()
	{
		Table::clear();
		List::flush();
	}
	
template <class K, class C, class Hash, class KeyStrategy>	template <class Key>
	inline	bool	GenericHashList<K,C,Hash,KeyStrategy>::query(const Key&ident, C*&target)	const
	{
		ListCarrier*c;
		if (Table::query(ident,c))
		{
			target = c->element;
			return true;
		}
		return false;
	}
	
template <class K, class C, class Hash, class KeyStrategy>	template <class Key>
	inline	C*	GenericHashList<K,C,Hash,KeyStrategy>::drop(const Key&ident)
	{
	
		TableCarrier*inner = Table::find(Hash::ComputeHash(ident),ident,false,NULL);

		if (!inner->occupied)
			return NULL;
		ListCarrier*c = inner->cast();
		Table::remove(inner);
		return List::dropCarrier(c);
	}
	
template <class K, class C, class Hash, class KeyStrategy>	template <class Key>
	inline	bool	GenericHashList<K,C,Hash,KeyStrategy>::erase(const Key&ident)
	{
		C*entry = drop(ident);
		if (!entry)
			return false;
		DISCARD(entry);
		return true;
	}
	
template <class K, class C, class Hash, class KeyStrategy>	template <class Key>
	inline	C* GenericHashList<K,C,Hash,KeyStrategy>::append(const Key&ident, C*element)
	{
		bool did_occupy;
		TableCarrier*inner = Table::find(Hash::ComputeHash(ident),ident,true,&did_occupy);
		if (did_occupy)
		{
			List::append(element);
			inner->cast() = List::getCarrier();
			return element;
		}
		return NULL;
	}
	
template <class K, class C, class Hash, class KeyStrategy>	template <class Key>
	inline	C* GenericHashList<K,C,Hash,KeyStrategy>::append(const Key&ident)
	{
		bool did_occupy;
		TableCarrier*inner = Table::find(Hash::ComputeHash(ident),ident,true,&did_occupy);
		if (did_occupy)
		{
			C*result = List::append();
			inner->cast() = List::getCarrier();
			return result;
		}
		return inner->cast()->getElement();
	}
	
template <class K, class C, class Hash, class KeyStrategy>	template <class Key0, class Key1>
	inline	C*	GenericHashList<K,C,Hash,KeyStrategy>::insert(const Key0&before, const Key1&key)
	{
		ListCarrier*c;
		if (!Table::query(before,c))
			return NULL;
		bool did_occupy;
		TableCarrier*inner = Table::find(Hash::ComputeHash(key),key,true,&did_occupy);
		if (did_occupy)
		{
			C*result = List::insertBeforeCarrier(c);
			inner->cast() = List::getCarrier();
			return result;
		}
		return inner->cast()->getElement();
	}
	
template <class K, class C, class Hash, class KeyStrategy>	template <class Key0, class Key1>
	inline	C*	GenericHashList<K,C,Hash,KeyStrategy>::insert(const Key0&before, const Key1&key, Type*element)
	{
		ListCarrier*c;
		if (!Table::query(before,c))
			return NULL;
		bool did_occupy;
		TableCarrier*inner = Table::find(Hash::ComputeHash(key),key,true,&did_occupy);
		if (did_occupy)
		{
			C*result = List::insertBeforeCarrier(c,element);
			inner->cast() = List::getCarrier();
			return result;
		}
		return NULL;
	}
	
		
template <class K, class C, class Hash, class KeyStrategy>	template <class Key0, class Key1>
	inline	bool	GenericHashList<K,C,Hash,KeyStrategy>::relocate(const Key0&key, const Key1&before)
	{
		ListCarrier*c0,*c1;
		if (!Table::query(key,c0) || !Table::query(before,c1))
			return false;
		List::relocate(c0,c1);
		return true;
	}

	

template <class K, class C, class Hash, class KeyStrategy> template <class Key>
	inline	C*	GenericHashList<K,C,Hash,KeyStrategy>::lookup(const Key&ident)
	{
		TableCarrier*inner = Table::find(Hash::ComputeHash(ident),ident,false,NULL);
		if (!inner->occupied)
			return NULL;
		return inner->cast()->getElement();
	}
	
template <class K, class C, class Hash, class KeyStrategy> template <class Key>
	inline	const C*	GenericHashList<K,C,Hash,KeyStrategy>::lookup(const Key&ident)			const
	{
		const TableCarrier*inner = Table::find(Hash::ComputeHash(ident),ident);
		if (!inner->occupied)
			return NULL;
		return inner->cast()->getElement();
	}

template <class K, class C, class Hash, class KeyStrategy> template <class Key>
	inline	C*	GenericHashList<K,C,Hash,KeyStrategy>::operator[](const Key&ident)
	{
		TableCarrier*inner = Table::find(Hash::ComputeHash(ident),ident,false,NULL);
		if (!inner->occupied)
			return NULL;
		return inner->cast()->getElement();
	}
	
template <class K, class C, class Hash, class KeyStrategy> template <class Key>
	inline	const C*	GenericHashList<K,C,Hash,KeyStrategy>::operator[](const Key&ident)			const
	{
		const TableCarrier*inner = Table::find(Hash::ComputeHash(ident),ident);
		if (!inner->occupied)
			return NULL;
		return inner->cast()->getElement();
	}
	
template <class K, class C, class Hash, class KeyStrategy>
	inline	GenericHashList<K,C,Hash,KeyStrategy>::operator size_t()							const
	{
		return Table::entries;
	}





#endif
