#ifndef hashtableH
#define hashtableH

#include "../global_string.h"
#include "../interface/hashable.h"






/**
	@brief Basic hash interface

	Question: should 'const String*' produce the same hash value as 'const String&' or as 'const void*'?
	Same for 'const ArrayData<...>*' and 'const IHashable*'...

	Pros: can use certain objects as pointers rather than copies and still apply data comparison instead of pointer comparison
	Cons:	*pointer tables cease to work as defined for these types (identical hash value yet different)
			*must always retrieve hash value for entire object which may be very expensive and unwanted (see above)

	Conclusion: treat like 'const void*'
*/
class StdHash
{
public:
static inline hash_t			memHash(const void*memory, size_t size);

static inline hash_t 			hash(const String&ident);
static inline hash_t 			hash(const StringW&ident);
static inline hash_t 			hash(const std::string&ident);
static inline hash_t 			hash(const std::wstring&ident);
static inline hash_t 			hash(const ReferenceExpression<char>&ident);
static inline hash_t 			hash(const char*ident);
static inline hash_t 			hash(const wchar_t*ident);
static inline hash_t 			hash(const void*ident);
static inline hash_t 			hash(char 	ident);
static inline hash_t 			hash(short 	ident);
static inline hash_t 			hash(int 	ident);
static inline hash_t 			hash(long	ident);
static inline hash_t 			hash(long long	ident);
static inline hash_t 			hash(unsigned char 	ident);
static inline hash_t 			hash(unsigned short 	ident);
static inline hash_t 			hash(unsigned int 	ident);
static inline hash_t 			hash(unsigned long	ident);
static inline hash_t 			hash(unsigned long long	ident);
static inline hash_t			hash(const IHashable&hashable);
static inline hash_t			hash(const IHashable*hashable);

template <typename T>
static inline hash_t 			hash(const ArrayData<T>&ident);



};




template <class K, class KeyStrategy>
	struct THashSetCarrier	//! Carrier type used to store a single entry of a hash set
	{
			typedef K					Key;

			K							key;		//!< Key of this entry
			hash_t						hashed;		//!< Hashed key
			bool						occupied;	//!< Set true if this entry is occupied

										THashSetCarrier():occupied(false)
										{}

			void						adoptData(THashSetCarrier<K,KeyStrategy>&other)
										{
											KeyStrategy::move(other.key,key);
											hashed = other.hashed;
											occupied = other.occupied;
										}
			void						occupy()
										{
											occupied = true;
										}
			void						free()
										{
											occupied = false;
										}
	};

template <class K, class C, class KeyStrategy, class DataStrategy>
	struct THashTableCarrier:public THashSetCarrier<K, KeyStrategy>	//! Carrier type used to store a single entry of a hash table
	{
			typedef THashTableCarrier<K,C,KeyStrategy,DataStrategy>	ThisType;
			typedef THashSetCarrier<K, KeyStrategy>	Super;
			typedef C					Data;

			ALIGN16 BYTE				entry_data[sizeof(C)];
			//C							entry;		//!< Local copy of the inserted data

										~THashTableCarrier()
										{
											if (occupied)
												reinterpret_cast<C*>(entry_data)->~C();
										}

			ThisType&					operator=(const ThisType&other)
										{
											if (occupied)
											{
												if (other.occupied)
													reinterpret_cast<C*>(entry_data)->operator=(*reinterpret_cast<const C*>(other.entry_data));
												else
													reinterpret_cast<C*>(entry_data)->~C();
											}
											else
												if (other.occupied)
													new (entry_data) C(*reinterpret_cast<const C*>(other.entry_data));
											Super::operator=(other);
											return *this;
										}

			void						adoptData(THashTableCarrier<K,C,KeyStrategy,DataStrategy>&other)
										{
											if (other.occupied)
											{
												if (occupied)
													DataStrategy::move(*reinterpret_cast<C*>(other.entry_data),*reinterpret_cast<C*>(entry_data));
												else
													DataStrategy::constructSingleFromFleetingData(reinterpret_cast<C*>(entry_data),*reinterpret_cast<C*>(other.entry_data));
											}
											else
												if (occupied)
													reinterpret_cast<C*>(entry_data)->~C();
											Super::adoptData(other);
										}
			C&							cast()
										{
											return *reinterpret_cast<C*>(entry_data);
										}
			const C&					cast() const
										{
											return *reinterpret_cast<const C*>(entry_data);
										}
			void						free()
										{
											if (occupied)
												reinterpret_cast<C*>(entry_data)->~C();
											occupied = false;
										}
			void						occupy()
										{
											new (entry_data) C();
											occupied = true;
										}
	};


template <class Carrier>
	class GenericHashBase
	{
	protected:
			static const size_t 			InitialSize = 4;	//!< Initial entries in the table.
			
			Array<Carrier,AdoptStrategy>	array;				//!< Entry table
			size_t							entries;			//!< Number of entries that are occupied in the table
			
											GenericHashBase();
											
		template <class Key>
			inline Carrier*					find(hash_t hash, const Key&key, bool occupy,bool*did_occupy=NULL)	//!< Carrier lookup via a key and its hash-value. \param hash Hashvalue of the provided string (usually the result of hashString(key)) \param key Key to look for \param occupy Forces the hashtable to occupy a new carrier for the specified key if it could not be found. \param did_occupy If non NULL then \b did_occupy will be set true if a carrier was occupied, false otherwise. \return Pointer to the occupied carrier if the key could be found or \b occupy was set true. Otherwise to a non-occupied carrier.
											{
												if (occupy && entries+1 >= array.length()*0.8)
													resize(array.length()*2);
												size_t offset = hash%array.length();
										//		cout << "attempting to find '"<<key<<"' ("<<hash<<") at "<<entries<<"/"<<array.length()<<endl;
												while (array[offset].occupied && (array[offset].hashed != hash || array[offset].key != key))
													offset = (offset+1)%array.length();
										//		cout << "done searching\n";
												if (occupy && !array[offset].occupied)
												{
													array[offset].hashed = hash;
													array[offset].key = typename Carrier::Key(key);
													array[offset].occupy();
													entries++;
													if (entries == array.length())
														FATAL__("hashtable overflow");
													if (did_occupy)
														(*did_occupy) = true;
												}
												else
													if (did_occupy)
														(*did_occupy) = false;


												return array+offset;
											}		
		template <class Key>
			inline const Carrier*			find(hash_t hash, const Key&key)			const	//!< Carrier lookup via a key and its hash-value. \param hash Hashvalue of the provided string (usually the result of hashString(key)) \param key Key to look for \return Pointer to the occupied carrier if the key could be found. Otherwise to a non-occupied carrier.
											{
										//		cout << "attempting to find '"<<key<<"' ("<<hash<<") at "<<entries<<"/"<<array.length()<<endl;


												size_t offset = hash%array.length();
												while (array[offset].occupied && (array[offset].hashed != hash || array[offset].key != key))
													offset = (offset+1)%array.length();
										//		cout << "done searching\n";



												return array+offset;
											}
			inline void						resize(size_t new_size);							//!< Resizes the hashtable to the specified size and remaps all table entries \param new_size New number of entries to use
			inline void 					remove(Carrier*c);									//!< Removes the specified carrier from the table. Also reduces the size of the table if necessary. \param c Carrier to remove.
	
	public:
			virtual							~GenericHashBase()	{};
	
			inline size_t					totalSize()							const;	//!< Returns the total size of the set/table in bytes. \return Total size of the local set/table in bytes.
			inline GenericHashBase<Carrier>&operator=(const GenericHashBase<Carrier>&other);
			inline size_t					count()								const;	//!< Returns the number of entries currently stored in the set/table. \return Number of entries in the set/table.
			inline operator 				size_t()							const;	//!< Implicit conversion to size_t. \return Returns the number of entries in the set/table.
			inline	void					clear();									//!< Resizes back to the initial set size and un-occupies all carriers. If no resizing took place then the respective objects are not reinitialized.
			inline	bool					isEmpty()							const;	//!< Checks if the local table is empty
			inline	bool					isNotEmpty()						const;	//!< Checks if the local table contains at least one element
			inline	void					Clear()										/** @copydoc clear() */ {clear();}
			inline	bool					IsEmpty()							const	/** @copydoc isEmpty() */ {return isEmpty();}
			inline	bool					IsNotEmpty()						const	/** @copydoc isNotEmpty() */ {return isNotEmpty();}
			inline	void					import(GenericHashBase<Carrier>&list);		//!< Imports the content of the specified other hashset/hashtable in addition to the already contained entries. Existing entries are overwritten. \param list Hashset to import entries from.
		template <class Key>
			inline	void					exportKeys(ArrayData<Key>&keys)	const;			//!< Exports the keys to the specified array. \param keys Reference to an array containing all associated keys after execution. 

			inline	void					calculateUnion(const GenericHashBase<Carrier>&other);	//!< Calculates the union of the local and the remote hash table/set. Duplicate entries are taken once from the local table/set. @param other Hash table/set to calculate the union with.
			inline	void					calculateIntersection(const GenericHashBase<Carrier>&other);	//!< Calculates the intersection of the local and the remote hash table/set. Local entries that are not also contained by the remote table/set are dropped. @param other Hash table/set to calculate the intersection with.
			inline	void					calculateDifference(const GenericHashBase<Carrier>&other);		//!< Calculates the difference (this - other). Removes all entries of the remote table/set from the local table/set. @param other Hash table/set to remove from the local one

			inline	void					adoptData(GenericHashBase<Carrier>&other);
			inline	void					swap(GenericHashBase<Carrier>&other);
	};

template <class Carrier, class Hash>
	class ExtendedHashBase:public GenericHashBase<Carrier>
	{
	protected:
			typedef GenericHashBase<Carrier>	Base;
	public:
		template <class Key>
			inline	bool					isSet(const Key&ident)				const;	//!< Queries whether ot not data is associated with the specified key without setting it. \param ident Key to look for \return True if the specified key could be found, false otherwise.
		template <class Key>
			inline	bool					unSet(const Key&ident);						//!< Un-sets any data associated with the specifed key. Potentially resizes the internal set. \param ident Key to look for \return True if the specified key could be found and un-set, false otherwise.
	};

/*!
	\brief Root Hashset

	The generic hashset is a type-invariant root class of all other sets in this file.
	Instances of GenericHashSet are natively copyable and require no additional copy constructor or assignment operator.
*/
template <class K, class Hash=StdHash, typename KeyStrategy = typename StrategySelector<K>::Default>
	class GenericHashSet:public ExtendedHashBase<THashSetCarrier<K,KeyStrategy>,Hash>
	{
	protected:
			typedef THashSetCarrier<K,KeyStrategy>		Carrier;
		
			typedef ExtendedHashBase<THashSetCarrier<K,KeyStrategy>,Hash>	Base;
	public:
			typedef K						KeyType;	//!< Key type

		template <class Key>
			inline	void					set(const Key&ident);						//!< Sets the specified key (if not set already). \param ident Key to set
		template <class Key>
			inline	void					setAll(const ArrayData<Key>&idents);				//!< Sets the specified range of keys (if not set already). @param idents Array of keys to set
	};

	
typedef GenericHashSet<String>		HashSet;				//!< String based 'hash' set
typedef GenericHashSet<index_t>		IndexSet;				//!< Index set
typedef GenericHashSet<const void*>	PointerSet;			//!< Pointer set

/*!
	\brief Root Hashtable

	The generic hashtable is a type-invariant root class of all other tables and containers in this file.
	Instances of GenericHashTable are natively copyable and require no additional copy constructor or assignment operator.
*/
template <class K, class C, class Hash=StdHash, typename KeyStrategy = typename StrategySelector<K>::Default, typename DataStrategy = typename StrategySelector<C>::Default>
	class GenericHashTable:public ExtendedHashBase<THashTableCarrier<K,C,KeyStrategy,DataStrategy>,Hash>
	{
	protected:
		
			typedef THashTableCarrier<K,C,KeyStrategy,DataStrategy>	Carrier;

			typedef ExtendedHashBase<THashTableCarrier<K,C, KeyStrategy,DataStrategy>,Hash>	Base;

	public:
			typedef K						KeyType;	//!< Key type
			typedef C						DataType;		//!< Data type


		template <typename F>
			inline	void					visitAllElements(const F&f);	//!< Passes each occupied element to the specified functor of type <tt>void f(DataType&)</tt>. The functor may receive a copy, const, or non-const reference, even alter data if needed
		template <typename F>
			inline	void					visitAllElements(const F&f)	const;	//!< Constant version of visitAllElements(). Only copies or const references may be received by the functor.
		template <typename F>
			inline	void					visitAllEntries(const F&f);	//!< Passes each occupied element to the specified functor of type <tt>void f(const KeyType&, DataType&)</tt>. The functor may receive a copy, const, or non-const reference, even alter data if needed
		template <typename F>
			inline	void					visitAllEntries(const F&f)	const;	//!< Constant version of visitAllEntries(). Only copies or const references may be received by the functor.
		template <class Key, class Entry>
			inline	void					exportTo(ArrayData<Key>&keys, ArrayData<Entry>&values)	const;	//!< Exports keys and values to the respective arrays. \param keys Reference to an array containing all associated keys after execution. \param values Reference to an object array containing all contained data elements after execution. \b keys and \b values will be of the same size with each entry of \b keys associated with the entry in \b values of the same index.
		template <class Entry>
			inline	void					exportTo(ArrayData<Entry>&values)		const;	//!< Exports all entries. \param values Reference to an array containing all entries after execution.
		template <class Key, class Entry>
			inline	void					exportAddressesTo(ArrayData<const Key*>&keys, ArrayData<Entry*>&values);
		template <class Entry>
			inline	void					exportAddressesTo(ArrayData<Entry*>&values);
		template <class Key, class Entry>
			inline	void					exportAddressesTo(ArrayData<const Key*>&keys, ArrayData<const Entry*>&values)const;
		template <class Entry>
			inline	void					exportAddressesTo(ArrayData<const Entry*>&values)	const;
		template <class Key, class Entry>
			inline	bool					findKeyOf(const Entry&entry, Key&key)const;	//!< Finds the key of the specified entry and stores it in \b key. Operates in O(n). \param entry Entry to look for. Comparison will be done via operator== . \param key Out key variable. Found key will be assigned via operator= . \return True if the requested element could be found, false otherwise.
		template <class Key>
			inline	bool					query(const Key&ident, DataType&target)	const;	//!< Requests the content associated with the specified key without setting it. \param ident Key to look for \param target Out reference to copy the respective content to \return True if an entry matching the specified key could be found, false otherwise. \b target remains unchanged if \b key could not be found.
		template <class Key>
			inline	bool					queryAndUnSet(const Key&ident, DataType&target);	//!< Requests the content associated with the specified key and un-sets it. \param ident Key to look for \param target Out reference to copy the respective content to \return True if an entry matching the specified key could be found, false otherwise. \b target remains unchanged if \b key could not be found.
		template <class Key>
			inline	DataType*				queryPointer(const Key&ident);				//!< Requests the content associated with the specified key without setting it. The method returns a pointer to the element ot NULL if no such could be found. \param ident Key to look for \return Pointer to the object matching the specified key or NULL if no such could be found
		template <class Key>
			inline	const DataType*			queryPointer(const Key&ident)		const;	//!< Requests the content associated with the specified key without setting it. The method returns a pointer to the element ot NULL if no such could be found. \param ident Key to look for \return Pointer to the object matching the specified key or NULL if no such could be found
			inline	bool					owns(const DataType*data)	const;
		template <class Key>
			inline	DataType*				setNew(const Key&ident);					//!< Sets the specified key if it is currently not set and returns the pointer its content. If the key is already set then NULL is returned @param ident Key to set @return Pointer to the set content associated with the specified key, or NULL if the key was previously set
		template <class Entry>
			inline	bool					unSetEntry(const Entry&entry);				//!< Un-sets the key associated with the specified entry. Potentially resizes the internal table. \param entry Entry to look for \return True if the specified entry could be found and un-set, false otherwise.
		template <class Key>
			inline	void					set(const Key&ident, const DataType&v);		//!< Sets the specified key (if not set already) and assigns \b v to the associated data. Identical to <table>[ident] = v; \param ident Key to set \param v Value to assign to the associated data.
		template <class Key>
			inline	DataType&				set(const Key&ident);						//!< Sets the specified key if it is currently not set and returns a reference to it. No change occurs if the specified key is already set (merely returns a reference to it)
		template <class Key>
			inline	DataType&				get(const Key&ident, DataType&except);		//!< Returns a (mutable) reference to the data associated with the specified key, or @a except if no such could be found.
		template <class Key>
			inline	const C&				get(const Key&ident, const C&except)	const;			//!< Returns a (const) reference to the data associated with the specified key, or @a except if no such could be found.
		template <class Key>
			inline	DataType&				require(const Key&ident);					//!< Returns a (mutable) reference to the data associated with the specified key. An assertion will fire, if the requested element does not exist
		template <class Key>
			inline	const C&				require(const Key&ident)	const;			//!< Returns a (const) reference to the data associated with the specified key. An assertion will fire, if the requested element does not exist

		template <class Key>
			inline	DataType&				operator[](const Key&ident);				//!< Standard access. If the specified key could not be found then an exception of type Program::MemberNotFound will be triggered. \param ident Key to look for \return Reference to the data associated with the specified key.
		template <class Key>
			inline	const DataType&			operator[](const Key&ident)			const;	//!< Standard const access. If the specified key could not be found then an exception of type Program::MemberNotFound will be triggered. \param ident Key to look for \return Reference to the data associated with the specified key.
	};


/*!
	\brief Standard Hashtable

	The hashtable stores copies of inserted objects mapped via strings. Anything that can be cast into a string can be used as a key.
*/
template <class C, typename DataStrategy = typename StrategySelector<C>::Default>
	class HashTable:public GenericHashTable<String,C,StdHash,SwapStrategy,DataStrategy>
	{};


/*!
	\brief Index table

	The indextable stores copies of inserted objects mapped via indices. Anything that can be cast into an integer can be used as a key.
*/
template <class C, typename DataStrategy = typename StrategySelector<C>::Default>
	class IndexTable:public GenericHashTable<index_t,C,StdHash,PrimitiveStrategy,DataStrategy>
	{};


/*!
	\brief Pointer table

	The pointertable stores copies of inserted objects mapped via pointers. Anything that can be cast into a pointer can be used as a key.
*/
template <class C, typename DataStrategy = typename StrategySelector<C>::Default>
	class PointerTable:public GenericHashTable<const void*,C, StdHash, PrimitiveStrategy, DataStrategy>
	{};

/*!
	\brief Generic mapped container
	
	The GenericHashContainer class acts as a generic super class to all pointer managing mapped tables.
*/
template <class K, class C,class Hash=StdHash, typename KeyStrategy = typename StrategySelector<K>::Default>
	class GenericHashContainer:protected GenericHashTable<K,C*,Hash,KeyStrategy,PrimitiveStrategy>
	{
	protected:
			typedef GenericHashTable<K,C*,Hash,KeyStrategy,PrimitiveStrategy>	Root;
			typedef typename Root::Carrier								Carrier;
			

	public:
			typedef typename Root::DataType	DataType;		//!< Data type (pointer to)
			typedef typename Root::KeyType	KeyType;
			


											GenericHashContainer()	{};
			virtual							~GenericHashContainer();

			inline	void					importAndFlush(GenericHashContainer<K,C,Hash,KeyStrategy>&list);		//!< Imports the content of the specified other hashcontainer and flushs it. \param list Hashcontainer to import entries from.

			inline	void					clear();									//!< Resets the internal table to the initial size, deletes all contained entries and sets their pointers to NULL.
			inline	void					Clear()	/**@copydoc clear()*/	{clear();}
			inline	void					flush();									//!< Resets the internal table to the initial size and sets all contained entries to NULL (does not delete the objects).

		template <class Key>
			inline	DataType				define(const Key&ident, DataType element);		//!< Defines the pointer associated with the specified key. \param ident Key to define \param element Pointer to the data that should be associated. \return \b element if the specified key was not defined and \b element could be associated with it, NULL otherwise.
		template <class Key>
			inline	DataType				define(const Key&ident);					//!< Defines the pointer associated with the specified key. \param ident Key to define \return Pointer to the data associated with the specified key or to a new object if the key was not previously defined.
		template <class Key>
			inline	DataType				drop(const Key&ident);						//!< Drops the data associated with the specified key without deleting it. Succeeding calls to lookup(ident) will return NULL. \param ident Key to look for. \return Pointer to the associated data or NULL if \b ident could not be found.
		template <class Key>
			inline	bool					erase(const Key&ident);

		template <class Key>
			inline	DataType				lookup(const Key&ident);					//!< Identical to lookup(ident)
		template <class Key>
			inline	const DataType			lookup(const Key&ident)				const;	//!< Identical to lookup(ident)

		template <class Key>
			inline	DataType				operator[](const Key&ident);				//!< Identical to lookup(ident)
		template <class Key>
			inline	const DataType			operator[](const Key&ident)			const;	//!< Identical to lookup(ident)
			inline	operator 				size_t()							const;	//!< Implicit conversion to size_t. \return Returns the number of entries in the table.
			
			Root::isSet;
			Root::count;
			Root::get;
			Root::totalSize;
			#ifndef __BORLANDC__
			Root::exportTo;
			#else
		template <class Key, class Entry>
			inline	void					exportTo(ArrayData<Key>&keys, ArrayData<Entry>&values)	const
			{
				Root::exportTo(keys,values);
			}
		template <class Entry>
			inline	void					exportTo(ArrayData<Entry>&values)		const
			{
				Root::exportTo(values);
			}
			#endif
			Root::query;
			Root::findKeyOf;
			
			
	};

/*!
	\brief String hash container

	The hashcontainer stores object pointers mapped via string keys.
	The container manages the inserted pointers, deleting them if the hashcontainer is deleted.
*/

template <class C>
	class HashContainer:public GenericHashContainer<String,C,StdHash,SwapStrategy>
	{};


/*!
	\brief Indexed container

	The index container stores object pointers mapped via indices. Anything that can be cast into an integer can be used as a key.
	The container manages the inserted pointers, deleting them if the hashcontainer is deleted.
*/

template <class C>
	class IndexContainer:public GenericHashContainer<index_t,C,StdHash,PrimitiveStrategy>
	{};


/*!
	\brief Pointer	container

	The pointer container stores object pointers mapped via pointers. Anything that can be cast into a pointer can be used as a key.
	The container manages the inserted pointers, deleting them if the hashcontainer is deleted.
*/

template <class C>
	class PointerContainer:public GenericHashContainer<const void*,C, StdHash,PrimitiveStrategy>
	{};



#include "hashtable.tpl.h"

#endif
