#ifndef hashtableTplH
#define hashtableTplH

template <typename Tfrom, typename Tto>
	static inline Tto	force_cast(const Tfrom&value)
	{
		union
		{
			Tfrom	in;
			Tto		to;
		};
		to = 0;
		in = value;
		return to;
	}



inline hash_t StdHash::hash(const IHashable&hashable)
{
	return hashable.hashCode();
}


inline hash_t StdHash::memHash(const void*memory, size_t size)
{
	return stdMemHash(memory,size);
}

template <typename T>
	inline hash_t 			StdHash::hash(const ArrayData<T>&field)
	{
		hash_t	result = 0;
		for (size_t i = 0; i < field.length(); i++)
		{
			result *= 17;
			result += hash(field[i]);
		}
		return result;
	}

template <typename T0, typename T1>
	/*static*/ inline hash_t 	StdHash::hash(const std::pair<T0,T1>&ident)
	{
		hash_t	result = hash(ident.first);
		result *= 17;
		result += hash(ident.second);
		return result;
	}


template <>
	inline hash_t 			StdHash::hash(const ArrayData<const void*>&field)
	{
		return stdMemHash(field.pointer(),field.GetContentSize());
	}

template <>
	inline hash_t 			StdHash::hash(const ArrayData<void*>&field)
	{
		return stdMemHash(field.pointer(),field.GetContentSize());
	}

template <>
	inline hash_t 			StdHash::hash(const ArrayData<char>&field)
	{
		return stdCharHash(field.pointer(),field.GetContentSize());
	}
template <>
	inline hash_t 			StdHash::hash(const ArrayData<wchar_t>&field)
	{
		return stdCharHash(field.pointer(),field.GetContentSize());
	}
template <>
	inline hash_t 			StdHash::hash(const ArrayData<short>&field)
	{
		return stdMemHash(field.pointer(),field.GetContentSize());
	}
template <>
	inline hash_t 			StdHash::hash(const ArrayData<int>&field)
	{
		return stdMemHash(field.pointer(),field.GetContentSize());
	}
template <>
	inline hash_t 			StdHash::hash(const ArrayData<long>&field)
	{
		return stdMemHash(field.pointer(),field.GetContentSize());
	}
template <>
	inline hash_t 			StdHash::hash(const ArrayData<long long>&field)
	{
		return stdMemHash(field.pointer(),field.GetContentSize());
	}

template <>
	inline hash_t 			StdHash::hash(const ArrayData<unsigned char>&field)
	{
		return stdMemHash(field.pointer(),field.GetContentSize());
	}

template <>
	inline hash_t 			StdHash::hash(const ArrayData<unsigned short>&field)
	{
		return stdMemHash(field.pointer(),field.GetContentSize());
	}

template <>
	inline hash_t 			StdHash::hash(const ArrayData<unsigned int>&field)
	{
		return stdMemHash(field.pointer(),field.GetContentSize());
	}

template <>
	inline hash_t 			StdHash::hash(const ArrayData<unsigned long>&field)
	{
		return stdMemHash(field.pointer(),field.GetContentSize());
	}

template <>
	inline hash_t 			StdHash::hash(const ArrayData<unsigned long long>&field)
	{
		return stdMemHash(field.pointer(),field.GetContentSize());
	}



inline hash_t StdHash::hash(const char*ident)
{
	return stdCharHash(ident,strlen(ident));
}
inline hash_t StdHash::hash(const wchar_t*ident)
{
	return stdCharHash(ident,Template::strlen(ident));
}



inline hash_t StdHash::hash(const String&ident)
{
	return stdCharHash(ident.c_str(),ident.length());
}
inline hash_t StdHash::hash(const StringW&ident)
{
	return stdCharHash(ident.c_str(),ident.length());
}
inline hash_t 			StdHash::hash(const std::string&ident)
{
	return stdCharHash(ident.c_str(),ident.length());
}
inline hash_t 			StdHash::hash(const std::wstring&ident)
{
	return stdCharHash(ident.c_str(),ident.length());
}
/*static*/ inline hash_t 			StdHash::hash(const StringRef&ident)
{
	return stdCharHash(ident.pointer(),ident.length());

}
/*static*/ inline hash_t 			StdHash::hash(const StringRefW&ident)
{
	return stdCharHash(ident.pointer(),ident.length());
}



inline hash_t StdHash::hash(const void*ident)
{
	return reinterpret_cast<hash_t>(ident);
}
	

inline hash_t StdHash::hash(int ident)	//identity
{
	return (hash_t)ident;
}

inline hash_t StdHash::hash(char ident)	//identity
	{
		return (hash_t)(BYTE)ident;
	}

inline hash_t StdHash::hash(short ident)	//identity
	{
		return (hash_t)(USHORT)ident;
	}

inline hash_t StdHash::hash(long ident)	//identity
	{
		return (hash_t)ident;
	}

inline hash_t StdHash::hash(long long ident)	//identity
	{
		return (hash_t)ident;	//need to fix this
	}




inline hash_t StdHash::hash(unsigned int ident)	//identity
{
	return (hash_t)ident;
}

inline hash_t StdHash::hash(unsigned char ident)	//identity
	{
		return (hash_t)(BYTE)ident;
	}

inline hash_t StdHash::hash(unsigned short ident)	//identity
	{
		return (hash_t)(USHORT)ident;
	}

inline hash_t StdHash::hash(unsigned long ident)	//identity
	{
		return (hash_t)ident;
	}

inline hash_t StdHash::hash(unsigned long long ident)	//identity
	{
		return (hash_t)ident;
	}



template <class Carrier>
	inline void					GenericHashBase<Carrier>::resize(size_t new_size)
	{


		Array<Carrier>	old_array;
		old_array.adoptData(array);
		array.SetSize(new_size);
			
		for (size_t i = 0; i < old_array.length(); i++)
			if (old_array[i].occupied)
			{
				Carrier	*c = old_array+i,
						*n = find(c->hashed,c->key,false);
				n->adoptData(*c);
				//(*n) = (*c);
			}
	}

template <class Carrier>
	inline	void				GenericHashBase<Carrier>::calculateUnion(const GenericHashBase<Carrier>&other)
	{
		for (index_t i = 0; i < other.array.length(); i++)
		{
			const Carrier*c = other.array+i;
			if (c->occupied)
			{
				bool did_occupy;
				Carrier*local = find(c->hashed,c->key,true,&did_occupy);
				if (did_occupy)
					*local = *c;
			}
		}
	}



template <class Carrier>
	inline	void					GenericHashBase<Carrier>::calculateIntersection(const GenericHashBase<Carrier>&other)
	{
		Array<THashSetCarrier<typename Carrier::Key, typename Carrier::AppliedKeyStrategy> >	occupied_keys(entries);
		index_t at = 0;
		for (index_t i = 0; i < array.length(); i++)
		{
			Carrier*c = array+i;
			if (c->occupied)
				occupied_keys[at++] = *c;
		}
		ASSERT_EQUAL__(at,entries);
		for (index_t i = 0; i < occupied_keys.count(); i++)
		{
			const THashSetCarrier<typename Carrier::Key, typename Carrier::AppliedKeyStrategy>&c = occupied_keys[i];
			if (!other.find(c.hashed,c.key)->occupied)
				remove(find(c.hashed,c.key,false));
		}
	}

template <class Carrier>
	inline	void					GenericHashBase<Carrier>::calculateDifference(const GenericHashBase<Carrier>&other)
	{
		Array<THashSetCarrier<typename Carrier::Key, typename Carrier::AppliedKeyStrategy> >	occupied_keys(entries);
		index_t at = 0;
		for (index_t i = 0; i < array.length(); i++)
		{
			Carrier*c = array+i;
			if (c->occupied)
				occupied_keys[at++] = *c;
		}
		ASSERT_EQUAL__(at,entries);
		for (index_t i = 0; i < occupied_keys.count(); i++)
		{
			const THashSetCarrier<typename Carrier::Key, typename Carrier::AppliedKeyStrategy>&c = occupied_keys[i];
			if (other.find(c.hashed,c.key)->occupied)
			{
				remove(find(c.hashed,c.key,false));
			}
		}
	}


template <class Carrier>
	inline	void				GenericHashBase<Carrier>::adoptData(GenericHashBase<Carrier>&other)
	{
		array.adoptData(other.array);
		entries = other.entries;

		other.array.SetSize(InitialSize);
		other.entries = 0;

		for (index_t i = 0; i < InitialSize; i++)
			other.array[i].free();
	}

template <class Carrier>
	inline	void				GenericHashBase<Carrier>::swap(GenericHashBase<Carrier>&other)
	{
		array.swap(other.array);
		swp(entries,other.entries);
	}

template <class Carrier>
	inline void					GenericHashBase<Carrier>::remove(Carrier*c)
	{
		if (!c->occupied)
			return;


		size_t	index = c-array.pointer(),
				current = index;
		
		while (true)
		{
			current = (current+1)%array.length();
			if (!array[current].occupied)
				break;
			size_t should_be = array[current].hashed%array.length();
			if (
				(current > index && (should_be <= index || should_be > current))
				||
	            (current < index && (should_be <= index && should_be > current))
				)
			{
				array[index].adoptData(array[current]);
				index = current;
			}
		}
		array[index].free();
		--entries;
		Tidy();
	}



template <class Carrier>
	inline bool					GenericHashBase<Carrier>::Tidy()
	{
		if (entries*5 >= array.length() || array.length() <= InitialSize)
			return false;
		count_t newSize = array.length() >> 1;
		while (entries*5 < newSize && newSize > InitialSize)
			newSize >>= 1;
		resize(newSize);
		return true;
	}
	

	

template <class Carrier>
	GenericHashBase<Carrier>::GenericHashBase():array(InitialSize),entries(0)
	{}

template <class Carrier>
	inline	GenericHashBase<Carrier>&	GenericHashBase<Carrier>::operator=(const GenericHashBase<Carrier>&other)
	{
		array = other.array;
		entries = other.entries;
		//if (array.pointer() && array.pointer() == other.array.pointer())	//this happens when copying self, but the assignment operator of array already handles that, so let's not add more redundant logic
		//	FATAL__("copy exception");
		return *this;
	
	}
	
template <class Carrier>
    inline  GenericHashBase<Carrier>::operator 				size_t()							const
	{
		return entries;
	}
	
template <class Carrier>
    inline	size_t				GenericHashBase<Carrier>::totalSize()							const
	{
		return sizeof(*this)+array.GetContentSize();
	}
	

template <class Carrier>
    inline  size_t				GenericHashBase<Carrier>::count()								const
	{
		return entries;
	}

template <class Carrier>
	inline	void	GenericHashBase<Carrier>::clear()
	{
		entries = 0;
		array.SetSize(InitialSize);
		for (size_t i = 0; i < InitialSize; i++)
			array[i].free();
	}

template <class Carrier>
	inline  void  GenericHashBase<Carrier>::import(GenericHashBase<Carrier>&list)
	{
		for (size_t i = 0; i < list.array.length(); i++)
			if (list.array[i].occupied)
			{
				Carrier*remote = list.array+i;
				find(remote->hashed,remote->key,true)->cast() = remote->entry;
			}
	}

template <class Carrier>
	template <class Key>
		inline	void	GenericHashBase<Carrier>::exportKeys(ArrayData<Key>&keys)	const
		{
			keys.SetSize(entries);
			if (!entries)
				return;
			size_t at = 0;
			for (size_t i = 0; i < array.length(); i++)
				if (array[i].occupied)
				{
					keys[at] = array[i].key;
					at++;
				}
		}
	







template <class Carrier, class Hash>
	template <class Key>
		inline	bool		ExtendedHashBase<Carrier,Hash>::isSet(const Key&ident)				const
		{
			return Base::find(Hash::hash(ident),ident)->occupied;
		}
	
template <class Carrier, class Hash>
	template <class Key>
		inline  bool        ExtendedHashBase<Carrier,Hash>::Unset(const Key&ident)
		{
			Carrier*c = Base::find(Hash::hash(ident),ident,false);
			if (c->occupied)
			{
				Base::remove(c);
				return true;
			}
			return false;
		}
	


template <class Carrier, class Hash>
	template <typename F>
		inline	void		ExtendedHashBase<Carrier,Hash>::visitAllKeys(const F&f) const
		{
			//index_t at = 0;
			for (index_t i = 0; i < Base::array.length(); i++)
				if (Base::array[i].occupied)
					f(Base::array[i].key);
		}





template <class K, class Hash, class KeyStrategy>
	template <class Key>
		inline	void		GenericHashSet<K,Hash,KeyStrategy>::set(const Key&ident)						//!< Sets the specified key (if not set already). The data associated with this key will not be (re)initialized. \param ident Key to set
		{
			Base::find(Hash::hash(ident),ident,true);
		}	

template <class K, class Hash, class KeyStrategy>
	template <class Key>
		inline	bool		GenericHashSet<K,Hash,KeyStrategy>::SetNew(const Key&ident)
		{
			bool did_occupy = false;
			Base::find(Hash::hash(ident),ident,true,&did_occupy);
			return did_occupy;
		}	

	

template <class K, class Hash, class KeyStrategy>
	template <class Key>
		inline	void		GenericHashSet<K,Hash,KeyStrategy>::setAll(const ArrayData<Key>&idents)						//!< Sets the specified key (if not set already). The data associated with this key will not be (re)initialized. \param ident Key to set
		{
			for (index_t i = 0; i < idents.count(); i++)
				Base::find(Hash::hash(idents[i]),idents[i],true);
		}	





template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	template <typename F>
		inline	void					GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::visitAllEntries(const F&f)
		{
			for (index_t i = 0; i < Base::array.length(); i++)
				if (Base::array[i].occupied)
					f((const K&)Base::array[i].key, Base::array[i].cast());
		}

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	template <typename F>
		inline	void					GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::visitAllEntries(const F&f)	const
		{
			for (index_t i = 0; i < Base::array.length(); i++)
				if (Base::array[i].occupied)
					f((const K&)Base::array[i].key, Base::array[i].cast());
		}


template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	template <typename F>
		inline	void				GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::visitAllValues(const F&f)
		{
			index_t at = 0;
			for (index_t i = 0; i < Base::array.length(); i++)
				if (Base::array[i].occupied)
					f(Base::array[i].cast());
		}

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	template <typename F>
		inline	void				GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::visitAllValues(const F&f)	const
		{
			for (index_t i = 0; i < Base::array.length(); i++)
				if (Base::array[i].occupied)
					f(Base::array[i].cast());
		}

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	template <typename F>
		inline	void				GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::FilterEntries(const F&f)
		{
			bool anyChanged = false;
			for (index_t i = 0; i < Base::array.length(); i++)
				if (Base::array[i].occupied && !f((const K&)Base::array[i].key,Base::array[i].cast()))
				{
					Base::array[i].free();
					--entries;
					anyChanged = true;
				}

			if (anyChanged && !Base::Tidy())
				Base::resize(array.Count());//will sort elements to where they should be
		}

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
		template <class Entry>
			inline	void					GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::exportAddressesTo(ArrayData<const K*>&keys, ArrayData<Entry*>&values)
			{
				keys.SetSize(Base::entries);
				values.SetSize(Base::entries);
				if (!Base::entries)
					return;
				index_t at = 0;
				for (index_t i = 0; i < Base::array.length(); i++)
					if (Base::array[i].occupied)
					{
						keys[at] = &Base::array[i].key;
						values[at] = &Base::array[i].cast();
						at++;
					}

			}

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
		template <class Entry>
			inline	void					GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::exportAddressesTo(ArrayData<Entry*>&values)
			{
				values.SetSize(Base::entries);
				if (!Base::entries)
					return;
				size_t at = 0;
				for (size_t i = 0; i < Base::array.length(); i++)
					if (Base::array[i].occupied)
					{
						values[at] = &Base::array[i].cast();
						at++;
					}
			}

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
		template <class Entry>
			inline	void					GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::exportAddressesTo(ArrayData<const K*>&keys, ArrayData<const Entry*>&values)const
			{
				keys.SetSize(Base::entries);
				values.SetSize(Base::entries);
				if (!Base::entries)
					return;
				index_t at = 0;
				for (index_t i = 0; i < Base::array.length(); i++)
					if (Base::array[i].occupied)
					{
						keys[at] = &Base::array[i].key;
						values[at] = &Base::array[i].cast();
						at++;
					}

			}

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
		template <class Entry>
			inline	void					GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::exportAddressesTo(ArrayData<const Entry*>&values)	const
			{
				values.SetSize(Base::entries);
				if (!Base::entries)
					return;
				size_t at = 0;
				for (size_t i = 0; i < Base::array.length(); i++)
					if (Base::array[i].occupied)
					{
						values[at] = &Base::array[i].cast();
						at++;
					}

			}


template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	template <class Entry>
	inline  void  GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::exportTo(ArrayData<K>&keys, ArrayData<Entry>&values)	const 
	{
		keys.SetSize(Base::entries);
		values.SetSize(Base::entries);
		if (!Base::entries)
			return;
		index_t at = 0;
		for (index_t i = 0; i < Base::array.length(); i++)
			if (Base::array[i].occupied)
			{
				keys[at] = Base::array[i].key;
				values[at] = Base::array[i].cast();
				at++;
			}
	}
	

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>	template <class Entry>
	inline  void  GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::exportTo(ArrayData<Entry>&values)	const
	{
		values.SetSize(Base::entries);
		if (!Base::entries)
			return;
		size_t at = 0;
		for (size_t i = 0; i < Base::array.length(); i++)
			if (Base::array[i].occupied)
			{
				values[at] = Base::array[i].cast();
				at++;
			}
	}

	
	

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy> template <class Entry>
	inline	bool		GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::findKeyOf(const Entry&entry, K&key)const
	{
		for (size_t i = 0; i < Base::array.length(); i++)
			if (Base::array[i].occupied && Base::array[i].cast() == entry)
			{
				key = Base::array[i].key;
				return true;
			}
		return false;
	}
	
template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	inline	C*	GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::queryPointer(const K&ident)
	{
		Carrier*c = Base::find(Hash::hash(ident),ident,false,NULL);
		if (!c->occupied)
			return NULL;
		return &c->cast();
	}

	
template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	inline	bool	GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::Owns(const DataType*data)	const
	{
		static const size_t	delta = ((const Carrier*)NULL)->entry_data - (BYTE*)NULL;
		return Base::array.Owns((const Carrier*)(((const BYTE*)data)-delta));
	}

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	inline	C&					GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::get(const K&ident, C&except)
	{
		C*ptr = queryPointer(ident);
		return ptr ? *ptr : except;
	}
template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	inline	const C&				GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::get(const K&ident, const C&except)	const
	{
		const C*ptr = queryPointer(ident);
		return ptr ? *ptr : except;
	}
template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	inline	C&					GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::require(const K&ident)
	{
		C*ptr = queryPointer(ident);
		ASSERT_NOT_NULL__(ptr/*,ident*/);	//not all 'ident' may be casted to string
		return *ptr;
	}
template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	inline	const C&				GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::require(const K&ident)	const
	{
		const C*ptr = queryPointer(ident);
		ASSERT_NOT_NULL1__(ptr,ident);
		return *ptr;
	}


template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	inline	const C*	GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::queryPointer(const K&ident) const
	{
		const Carrier*c = Base::find(Hash::hash(ident),ident);
		if (!c->occupied)
			return NULL;
		return &c->cast();
	}

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	inline	bool	GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::query(const K&ident, DataType&target)	const
	{
		const Carrier*c = Base::find(Hash::hash(ident),ident);
		if (!c->occupied)
			return false;
		target = c->cast();
		return true;
	}

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	inline	bool	GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::queryAndUnset(const K&ident, DataType&target)
	{
		Carrier*c = Base::find(Hash::hash(ident),ident,false);
		if (!c->occupied)
			return false;
		DataStrategy::move(c->cast(),target);
		//target = c->cast();
		Base::remove(c);
		return true;
	}




template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
template <class Entry>
	inline	bool				GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::unsetEntry(const Entry&entry)
	{
		K key;
		return findKeyOf(entry,key) && Base::Unset(key);
	}
	
		
	
template <class K, class C, class Hash, class KeyStrategy, class DataStrategy> 
	inline	C&					GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::set(const K&ident, const DataType&v)
	{
		C&rs = Base::find(Hash::hash(ident),ident,true)->cast();
		rs = v;
		return rs;
	}

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	inline	C&				GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::Reference(const K&ident, const DataType&initValue)
	{
		bool didOccupy;
		C&rs = Base::find(Hash::hash(ident),ident,true,&didOccupy)->cast();
		if (didOccupy)
			rs = initValue;
		return rs;
	}


template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
	inline	C*					GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::setNew(const K&ident)
	{
		bool did_occupy;
		Carrier*c = Base::find(Hash::hash(ident),ident,true,&did_occupy);
		if (!did_occupy)
			return NULL;
		return &c->cast();
	}

template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
    inline  typename GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::DataType&           		GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::set(const K&ident)
	{
		Carrier*c = Base::find(Hash::hash(ident),ident,true);
		return c->cast();
	}

	
	
template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
    inline  typename GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::DataType&           		GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::operator[](const K&ident)
	{
		Carrier*c = Base::find(Hash::hash(ident),ident,false);
		if (!c->occupied)
			throw Except::Program::MemberNotFound();
		return c->cast();
	}
	
template <class K, class C, class Hash, class KeyStrategy, class DataStrategy>
    inline  const typename GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::DataType&           	GenericHashTable<K,C,Hash,KeyStrategy,DataStrategy>::operator[](const K&ident)	const
	{
		const Carrier*c = Base::find(Hash::hash(ident),ident);
		if (!c->occupied)
			throw Except::Program::MemberNotFound();
		return c->cast();
		//return Base::find(Hash::hash(ident),ident)->cast();
	}



	
	

	
template <class K, class C, class Hash, class KeyStrategy>
	GenericHashContainer<K,C,Hash,KeyStrategy>::~GenericHashContainer()
	{

		for (size_t i = 0; i < Root::array.length(); i++)
			if (Root::array[i].occupied && Root::array[i].cast() != NULL)
				DISCARD(Root::array[i].cast());

	}


template <class K, class C, class Hash, class KeyStrategy>
	inline  void	GenericHashContainer<K,C,Hash,KeyStrategy>::importAndFlush(GenericHashContainer<K,C,Hash,KeyStrategy>&list)
	{

		Root::import(list);
		list.flush();

	}
	
template <class K, class C, class Hash, class KeyStrategy>
	inline	void	GenericHashContainer<K,C,Hash,KeyStrategy>::clear()
	{

		for (size_t i = 0; i < Root::array.length(); i++)
			if (Root::array[i].occupied && Root::array[i].cast() != NULL)
				DISCARD(Root::array[i].cast());
		Root::clear();

	}
	
template <class K, class C, class Hash, class KeyStrategy>
	inline	void	GenericHashContainer<K,C,Hash,KeyStrategy>::flush()
	{

		Root::clear();

	}

template <class K, class C, class Hash, class KeyStrategy> template <class Key>
	inline  typename GenericHashContainer<K,C,Hash,KeyStrategy>::DataType	GenericHashContainer<K,C,Hash,KeyStrategy>::define(const Key&ident, DataType element)
	{

		bool did_occupy;
		Carrier*c = Root::find(Hash::hash(ident),ident,true,&did_occupy);

		if (did_occupy)
		{
			c->cast() = element;
			return element;
		}
		return NULL;
	}

template <class K, class C, class Hash, class KeyStrategy> template <class Key>
	inline  typename GenericHashContainer<K,C,Hash,KeyStrategy>::DataType	GenericHashContainer<K,C,Hash,KeyStrategy>::define(const Key&ident)
	{

		bool did_occupy;
		Carrier*c = Root::find(Hash::hash(ident),ident,true,&did_occupy);

		if (did_occupy)
			c->cast() = SHIELDED(new C());
		return c->cast();
	}



	
template <class K, class C, class Hash, class KeyStrategy> template <class Key>
	inline  typename GenericHashContainer<K,C,Hash,KeyStrategy>::DataType        GenericHashContainer<K,C,Hash,KeyStrategy>::drop(const Key&ident)
	{

		Carrier*c = Root::find(Hash::hash(ident),ident,false,NULL);

		if (!c->occupied)
			return NULL;
		DataType result = c->cast();
		Root::remove(c);
		return result;
	}
	
template <class K, class C, class Hash, class KeyStrategy> template <class Key>
	inline  bool GenericHashContainer<K,C,Hash,KeyStrategy>::erase(const Key&ident)
	{

		DataType t = drop(ident);

		if (!t)
			return false;
		DISCARD(t);
		return true;
	}
	
	
	
template <class K, class C, class Hash, class KeyStrategy> template <class Key>
    inline  typename GenericHashContainer<K,C,Hash,KeyStrategy>::DataType           		GenericHashContainer<K,C,Hash,KeyStrategy>::operator[](const Key&ident)
	{

		Carrier*c = Root::find(Hash::hash(ident),ident,false);

		return c->occupied?c->cast():NULL;
	}
	
template <class K, class C, class Hash, class KeyStrategy> template <class Key>
    inline  const typename GenericHashContainer<K,C,Hash,KeyStrategy>::DataType           	GenericHashContainer<K,C,Hash,KeyStrategy>::operator[](const Key&ident)				const
	{

		const Carrier*c = Root::find(Hash::hash(ident),ident);

		return c->occupied?c->cast():NULL;
	}
	
template <class K, class C, class Hash, class KeyStrategy> template <class Key>
    inline  typename GenericHashContainer<K,C,Hash,KeyStrategy>::DataType           		GenericHashContainer<K,C,Hash,KeyStrategy>::lookup(const Key&ident)
	{

		Carrier*c = Root::find(Hash::hash(ident),ident,false);

		return c->occupied?c->cast():NULL;
	}
	
template <class K, class C, class Hash, class KeyStrategy> template <class Key>
    inline  const typename GenericHashContainer<K,C,Hash,KeyStrategy>::DataType           	GenericHashContainer<K,C,Hash,KeyStrategy>::lookup(const Key&ident)				const
	{

		const Carrier*c = Root::find(Hash::hash(ident),ident);

		return c->occupied?c->cast():NULL;
	}
		
template <class K, class C, class Hash, class KeyStrategy> 	
	inline  GenericHashContainer<K,C,Hash,KeyStrategy>::operator 				size_t()							const	//!< Implicit conversion to size_t. \return Returns the number of entries in the table.
	{

		return Root::entries;
	}

	


#endif
