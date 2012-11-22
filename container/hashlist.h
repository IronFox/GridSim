#ifndef hashlistH
#define hashlistH

#include "hashtable.h"
#include "linkedlist.h"


/******************************************************************

E:\include\list\hashlist.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


template <class K, class C, class Hash=StdHash, class KeyStrategy=typename StrategySelector<K>::Default>
	class GenericHashList:protected LinkedList<C>, protected GenericHashTable<K,typename LinkedList<C>::Carrier*,Hash,KeyStrategy,PrimitiveStrategy>
	{
	protected:
			typedef LinkedList<C>														List;
			typedef typename List::Carrier												ListCarrier;
			typedef GenericHashTable<K,ListCarrier*,Hash,KeyStrategy,PrimitiveStrategy>	Table;
			typedef typename Table::Carrier												TableCarrier;
	public:
			typedef GenericHashList<K,C,Hash,KeyStrategy>	ThisType;
			typedef K						KeyType;	//!< Key type
			typedef C						Type;		//!< Data type
			
											GenericHashList()	{};
											GenericHashList(const ThisType&other);
			ThisType&						operator=(const ThisType&other);
			

		template <class Entry>
			inline	void					exportTo(ArrayData<Entry*>&values);					//!< Exports all entries. \param values Reference to an array containing all entries after execution.
		template <class Key, class Entry>
			inline	void					exportTo(ArrayData<Key>&keys, ArrayData<Entry*>&values);	//!< Exports keys and values to the respective arrays. \param keys Reference to an array containing all associated keys after execution. \param values Reference to an object array containing all contained data elements after execution. \b keys and \b values will be of the same size with each entry of \b keys associated with the entry in \b values of the same index.
		template <class Key, class Entry>
			inline	void					exportTo(ArrayData<Key>&keys, ArrayData<const Entry*>&values)	const;	//!< Exports keys and values to the respective arrays. \param keys Reference to an array containing all associated keys after execution. \param values Reference to an object array containing all contained data elements after execution. \b keys and \b values will be of the same size with each entry of \b keys associated with the entry in \b values of the same index.
			inline	void					import(ThisType&list);	//!< Moves the content of the specified other hashlist in addition to the already contained entries. Duplicate entries are left in the original list. \param list Hashlist to import entries from.
			inline	void					importCopies(ThisType&list);	//!< Duplicates the content of the specified other hashlist in addition to the already contained entries. Duplicate entries are not added. \param list Hashlist to import entries from.
			inline	void					clear();									//!< Resets the internal table to the initial size, deletes all contained entries and sets their pointers to NULL.
			inline	void					flush();									//!< Resets the internal table to the initial size and sets all contained entries to NULL (does not delete the objects).
		template <class Key>
			inline	bool					query(const Key&ident, Type*&target)	const;	//!< Requests the content associated with the specified key without setting it. \param ident Key to look for \param target Out reference to copy the respective content to \return True if an entry matching the specified key could be found, false otherwise. \b target remains unchanged if \b key could not be found.
		template <class Key>
			inline	Type*					drop(const Key&ident);						//!< Drops the data associated with the specified key without deleting it. Succeeding calls to lookup(ident) will return NULL. \param ident Key to look for. \return Pointer to the associated data or NULL if \b ident could not be found.
		template <class Key>
			inline	bool					erase(const Key&ident);
		template <class Key>
			inline	Type*					append(const Key&ident, C*element);		//!< Appends the specified pointer associated with the specified key. \param ident Key to define \param element Pointer to the data that should be associated. \return \b element if the specified key was not defined and \b element could be associated with it, NULL otherwise.
		template <class Key>
			inline	Type*					append(const Key&ident);					//!< Relocates any existing pointer to the end of the list. If none exists then a new pointer associated  with the specified key is appended to the end of the list. \param ident Key to define \return Pointer to the data associated with the specified key or to a new object if the key was not previously defined.
		
		template <class Key0, class Key1>
			inline	Type*					insert(const Key0&before, const Key1&key);	//!< Relocates an existing pointer to the specified position or inserts a new pointer associated with the specified key. \param before Existing key to insert before. \param key Associated key. \return Pointer to the data associated with the specified key or to a new object if the key was not previously defined.
		template <class Key0, class Key1>
			inline	Type*					insert(const Key0&before, const Key1&key, Type*element);		//!< Inserts the specified pointer associated with the specified key. \param before Existing key to insert before. \param key Non existing key to associate the provided pointer with. \param element Pointer to the data that should be inserted. \return \b element if the specified key was not defined and \b element could be associated with it, NULL otherwise.
			
		template <class Key0, class Key1>
			inline	bool					relocate(const Key0&key, const Key1&before);	//!< Relocates an existing entry. The method will fail if either \b key or \b before are not defined. \param key Key of the element to relocate. \param before Key to insert the element before \return true if both elements were found and the entry could be relocated, false otherwise.

		template <class Key>
			inline	Type*					lookup(const Key&ident);					//!< Identical to operator[](ident)
		template <class Key>
			inline	const Type*				lookup(const Key&ident)				const;	//!< Identical to operator[](ident)
			
		template <class Key>
			inline	Type*					operator[](const Key&ident);				//!< Standard access. If the specified key could not be found then NULL is returned. \param ident Key to look for \return Reference to the data associated with the specified key or NULL if the key is not defined
		template <class Key>
			inline	const Type*				operator[](const Key&ident)			const;	//!< @overload
			inline	operator 				size_t()							const;	//!< Implicit conversion to size_t. \return Returns the number of entries in the table.

			void							adoptData(ThisType&other)
											{
												List::adoptData(other);
												Table::adoptData(other);
											}
			void							swap(ThisType&other)
											{
												List::swap(other);
												Table::swap(other);
											}


			List::get;
			List::getConst;
			List::getReference;
			List::getIterator;
			List::getIndexOf;
			List::first;
			List::last;
			List::begin;
			List::end;
			#ifndef __BORLANDC__
			List::operator();
	        #endif
			List::count;
			List::reset;
			List::each;
			
			Table::isSet;
			Table::findKeyOf;
			Table::exportKeys;

			#ifndef _MSC_VER
				List::iterator;
				List::const_iterator;
			#endif
	};


template <class C> class StringMappedList:public GenericHashList<String,C,StdHash,SwapStrategy>
{};

template <class C> class IndexMappedList:public GenericHashList<unsigned,C,StdHash,PrimitiveStrategy>
{};

template <class C> class PointerMappedList:public GenericHashList<const void*,C,StdHash,PrimitiveStrategy>
{};


#include "hashlist.tpl.h"

#endif
