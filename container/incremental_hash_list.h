#ifndef incremental_hash_listH
#define incremental_hash_listH

#include <list>

#include "buffer.h"
#include "hashtable.h"




template <size_t ElementSize>
	class IncrementalPool
	{
	protected:
	static	Buffer<void*>		unused_elements;

	public:
	static inline void*			allocate()
								{
									if (unused_elements.isEmpty())
									{
										return malloc(ElementSize);
										//BYTE*address = (BYTE*)malloc(ElementSize*4);	//for some reason, block allocation is slower. Definately not faster. After a while it doesn't matter anyway
										//unused_elements.appendVA<BYTE*>(3,address+ElementSize*3,address+ElementSize*2,address+ElementSize);
										//return address;
									}
									return unused_elements.pop();
								}
			
	static inline void			deallocate(void*p)
								{
									if (application_shutting_down)	//buffer no longer available when commencing application shutdown
									{
										//free(p); //deallocated anyway. possibly gaining a bit of shutdown speed
									}
									else
										unused_elements << p;
								}
	};

template <size_t ElementSize>
	Buffer<void*>	IncrementalPool<ElementSize>::unused_elements;


/**
	@brief Buffered allocator to be used by list<...> types. Not to be used by any other std container type.

	This type must NEVER EVER be used FOR ANYTHING BUT list<...>. No, not even then.
*/
template <typename T>
	class IncrementalAllocator
	{
	public:
			typedef	T*			pointer;
			typedef const T*	const_pointer;
			typedef T&			reference;
			typedef const T&	const_reference;
			typedef T			value_type;
			typedef size_t		size_type;
			typedef ptrdiff_t	difference_type;




								IncrementalAllocator()
								{}
								IncrementalAllocator(const IncrementalAllocator<T>&other)
								{}
							template <typename T1>
								IncrementalAllocator(const IncrementalAllocator<T1>&other)
								{}

	static	inline size_t		max_size()
								{
									return -1;
								}
	static	inline T*			allocate(size_t n, const void* hint = NULL)	//ignoring n because list<...> never allocates more than 1 element at a time
								{
									return (T*)IncrementalPool<sizeof(T)>::allocate();
								}

	static	inline void			deallocate(T* p, size_t n)	//ignoring n because list<...> never allocates more than 1 element at a time
								{
									IncrementalPool<sizeof(T)>::deallocate(p);

								}


		template <typename T1>
			static inline void	construct(T1*p, const T&t)
								{
									//cout << "constructing (p=0x"<<p<<", &t=0x"<<&t<<")"<<endl; flush(cout);
									new ((void*) p) T1(t);
									//cout << " constructed"<<endl; flush(cout);
								}
 
		template <typename T1>
			static inline void	destroy(T1*p)
								{
									//cout << "destroying (p=0x"<<p<<")"<<endl; flush(cout);
									((T1*)p)->~T1();
									//cout << " destroyed"<<endl; flush(cout);
								}

		template <typename T1>
			struct rebind
			{
				typedef IncrementalAllocator<T1>	other;
			};
	};



	/*
		this thing is dangerous because it creates uninitialized carrier data. unless on primitives better stay clear
template <typename T>
	class IncrementalNoInitAllocator:public IncrementalAllocator<T>
	{
	public:
		template <typename T1>
			static inline void	construct(T1*p, const T&t)
								{}
 
		template <typename T1>
			static inline void	destroy(T1*p)
								{}
	};

	*/


template <typename K, typename C>
	struct THashPair
	{
			K			key;
			C			data;
	};

template <typename K, typename C, typename Hash=StdHash, typename Allocator = IncrementalAllocator<THashPair<K,C> > >
	class IncrementalHashList:
			protected std::list<THashPair<K,C>,Allocator>,
			protected GenericHashTable<K,typename std::list<THashPair<K,C>,Allocator>::iterator>
	{
	public:
			typedef typename std::list<THashPair<K,C>,Allocator>	List;
			
			typedef typename List::iterator			iterator;
			typedef typename List::const_iterator	const_iterator;

			typedef THashPair<K,C>							Pair;
			typedef GenericHashTable<K,iterator>			Table;


			using List::begin;
			using List::end;

			using Table::isset;
			using Table::query;
			using Table::queryPointer;

	inline	bool							isEmpty()	const
			{
				return List::empty();
			}
	inline	count_t							count()	const
			{
				return List::size();
			}
	inline	Pair&							first()
			{
				return List::front();
			}
	inline	const Pair&						first() const
			{
				return List::front();
			}
	inline	Pair&							last()
			{
				return List::back();
			}
	inline	const Pair&						last() const
			{
				return List::back();
			}

			void							clear()
			{
				List::clear();
				Table::clear();
			}
	
		template <typename K1>
			C*								set(const K1&key, const C&data)		//! Sets the data associated with the specified key. The method fails if data is already associated with the specified key
			{
				//cout << "setNew(key="<<key<<", data="<<data<<")"<<endl; flush(cout);
				iterator*it = Table::setNew(key);
				if (!it)
				{
					//cout << " failed"<<endl; flush(cout);
					return NULL;
				}
				//cout << " creating pair p"<<endl; flush(cout);
				Pair p = {(K)key,data};
				//cout << "  with key="<<p.key<<" and data="<<p.data<<endl; flush(cout);
				//cout << " inserting..."<<endl;  flush(cout);
				(*it) = List::insert(List::end(),p);
				//cout << " inserted."<<endl; flush(cout);
				return &((*it)->data);
			}

		template <typename Key>
			bool							unset(const Key&key)	//! Removes an element from the local hash list
			{
				iterator it;
				if (Table::queryAndUnset(key,it))
				{
					List::erase(it);
					return true;
				}
				return false;
			}

		template <typename Key>
			bool							erase(const Key&key)	//! Same like unset()
			{
				iterator it;
				if (Table::queryAndUnset(key,it))
				{
					List::erase(it);
					return true;
				}
				return false;
			}
	};







#endif
