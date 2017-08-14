#if !defined(queueH) && !defined(__CUDACC__)
#define queueH

/******************************************************************

Collection of different queues.

******************************************************************/


//#include "dynamic_elements.h"
#include "common.h"
#include "array.h"

namespace DeltaWorks
{
	namespace Container
	{

		template <class T, class MyStrategy>
			class QueueElement
			{
			public:
				char	data[sizeof(T)];
				bool	isConstructed;

				/**/	QueueElement():isConstructed(false)
				{}
				/**/	~QueueElement()
				{
					if (isConstructed)
						Destruct();
				}


				void	Copy(const QueueElement<T,MyStrategy>&other)
				{
					if (!other.isConstructed)
					{
						if (isConstructed)
							Destruct();
						return;
					}
					Copy(other.Cast());
				}

				void	Copy(const T&data)
				{
					if (!isConstructed)
						Construct();
					Cast() = data;
				}

				T&		Cast()	{DBG_ASSERT__(isConstructed); return *(T*)data;}
				const T&Cast() const {DBG_ASSERT__(isConstructed); return *(const T*)data;}
		
				void	Construct()
				{
					DBG_ASSERT__(!isConstructed);
					new (data) T;
					isConstructed = true;
				}
				void	Construct(const T&initData)
				{
					DBG_ASSERT__(!isConstructed);
					new (data) T(initData);
					isConstructed = true;
				}
				void	Destruct()
				{
					//DBG_ASSERT__(isConstructed);//in Cast()
					Cast().~T();
					isConstructed = false;
				}

				T&		ConstructAndCast()
				{
					Construct();
					return Cast();
				}

				void	adoptData(QueueElement<T,MyStrategy>&other)
				{
					//if (isConstructed)
						//Destruct();
					if (other.isConstructed)
					{
						if (isConstructed)
							MyStrategy::move(*(T*)other.data,*(T*)data);
						else
						{
							MyStrategy::constructSingleFromFleetingData((T*)data,*(T*)other.data);
							isConstructed = true;
						}
						other.Destruct();
					}
					else
						if (isConstructed)
							Destruct();
					//isConstructed = other.isConstructed;
					//other.isConstructed = false;
				}

	
			};

		template <class Entry, class MyStrategy>
			class Queue;

		template <class Entry, class Element, class MyStrategy>
			class QueueIterator
			{
			private:
				//typedef QueueElement<Entry,Strategy>	Element;

				Element			*field_begin = nullptr,
								*current = nullptr,
								*field_end = nullptr;

				friend class Queue<Entry,MyStrategy>;
			public:
				typedef QueueIterator<Entry,Element,MyStrategy>	It;
			
				/**/			QueueIterator()	{};
				/**/			QueueIterator(Element*begin,Element*end,Element*c);
				///**/			QueueIterator(const QueueIterator<typename std::remove_const<Entry>::type,typename std::remove_const<Element>::type,Strategy>&other):field_begin(other.field_begin),current(other.current),field_end(other.field_end)	{}
				It&				operator++();
				It				operator++(int);
				It				operator+(int delta)	const;
				It&				operator--();
				It				operator--(int);
				It				operator-(int delta)	const;
				bool			operator==(const It&other)	const	{return current == other.current;}
				bool			operator!=(const It&other)	const	{return current != other.current;}
				Entry&			operator*()	{return current->Cast();}
				Entry*			operator->()	{return &current->Cast();}
			
				size_t			index()	const;
			};


		template <class Entry, class MyStrategy=typename StrategySelector<Entry>::Default>
			class Queue:protected Ctr::Array<QueueElement<Entry,MyStrategy>,Strategy::Adopt>
			{
			private:
				typedef QueueElement<Entry,MyStrategy>	Element;
				typedef Ctr::Array<Element,Strategy::Adopt>	Super;
				Element				*section_begin,
									*section_end,
									*field_end;
				void				increaseSize(count_t new_size);
				Element*			Increment(Element*el)	const;
			public:
				typedef Queue<Entry,MyStrategy>	Self;
				typedef QueueIterator<Entry,Element,MyStrategy>	iterator;
				typedef QueueIterator<const Entry,const Element,MyStrategy>	const_iterator;
			
				/**/				Queue(const Self&other);
				/**/				Queue(Self&&other);
				/**/				Queue(size_t size=0);
				/**/				~Queue()	{clear();}

				iterator			begin();
				iterator			end();
				const_iterator		begin()	const;
				const_iterator		end() const;
				bool				Pop(Entry&out);						//!< Pops the oldest element from the queue, and writes it to @b out , decreasing the number of stored elements by one. Note that the stored object IS destroyed. @param out Target reference to write to @return true if the queue was not empty and an element was written to @b out , false otherwise
				Entry				Pop();								//!< Pops the oldest element from the queue, decreasing the number of stored elements by one. Note that the stored object IS destroyed.
				count_t				Pop(Entry*out_field, count_t count);			//!< Pops up to @a count of the oldest elements to the specified out field @return Number of elements that were actually popped (which may be less than the resquested number if not enough elements are stored)
				void				EraseFront();							//!< Erases the (oldest) element out-front. Identical to Pop() but without any copy constructors
				void				EraseBack();							//!< Erases the (newest) element out-back
				void				Push(const Ctr::ArrayData<Entry>&entries);	//!< Pushes multiple elements into the queue, increasing the number of stored elements by <em>entries.count()</em>. The queue automatically increases the size of its data field if appropriate
				void				Push(const Entry*, count_t count);	//!< Pushes multiple elements into the queue, increasing the number of stored elements by <em>count</em>. The queue automatically increases the size of its data field if appropriate
				void				Push(const Entry&data);				//!< Pushes an element into the queue, increasing the number of stored elements by one. The queue automatically increases the size of its data field if appropriate
				Entry&				Push();								//!< Pushes a new empty element into the queue.

				void				PushFront(const Entry&data);
				Entry&				PushFront();

				bool				IsEmpty()					const;	//!< Identical to length()==0
				bool				IsNotEmpty()				const;	//!< Identical to length()!=0
				count_t				length()					const;	//!< Returns the current number of element stored in the queue
				inline count_t		GetLength()					const {return length();}
				inline count_t		CountEntries()				const {return length();}
				inline count_t		size()						const {return length();}		//!< Identical to length()
				bool				operator>>(Entry&entry);			//!< Identical to pop()
				Queue<Entry,MyStrategy>&	operator<<(const Entry&entry);		//!< Identical to push() @return *this
				Entry&				Peek();								//!< Returns a reference to the last (oldest) element in the queue @return last element in the queue
				const Entry&		Peek()						const;	//!< Returns a reference to the last (oldest) element in the queue @return last element in the queue
				Entry&				GetOldest();							//!< Returns a reference to the last (oldest) element in the queue. Identical to peek() @return last element in the queue
				const Entry&		GetOldest()					const;	//!< Returns a reference to the last (oldest) element in the queue. Identical to peek() @return last element in the queue
				Entry&				GetNewest();							//!< Returns a reference to the first (newest) element in the queue @return first element in the queue
				const Entry&		GetNewest()					const;	//!< Returns a reference to the first (newest) element in the queue @return first element in the queue
				Entry&				operator[](size_t index);			//!< Returns the nth element from the queue. @param index Index of the element to return ranging [0, length()-1] @return Reference to the requested object
				const Entry&		operator[](size_t index)	const;	//!< Returns the nth element from the queue. @param index Index of the element to return ranging [0, length()-1] @return Reference to the requested object
				void				clear();							//!< Clears the local queue of all entries. No objects are actually erased
				inline void			Clear()	{clear();}
				bool				Contains(const Entry&entry)	const;	//!< Checks if the queue contains the specified element

				void				operator=(const Queue<Entry,MyStrategy>&other);	//!< Copies all elements from the remote queue to the local queue. The local buffer size is resized to match the remote buffer size but only actually used elements are copied
				void				swap(Self&other);
				friend void			swap(Self&a, Self&b)	{a.swap(b);}
				void				adoptData(Self&other);

				void				CopyToArray(Ctr::ArrayData<Entry>&out) const;
				void				MoveToArray(Ctr::ArrayData<Entry>&out, bool clearSelfWhenDone=true);
			
				inline count_t		count()						const	{return length();};	//!< Returns the current number of element stored in the queue
				inline count_t		Count()						const	{return length();}

				bool				operator==(const Self&other) const;
				bool				operator!=(const Self&other) const;
			};

		template <class Entry, class Priority, class EntryStrategy=typename StrategySelector<Entry>::Default>
			class PriorityQueue
			{
			protected:
					typedef QueueElement<Entry,EntryStrategy>	Element;

					typedef	Ctr::Array<Priority>			PriorityArray;
					typedef Ctr::Array<Element,Strategy::Swap>	EntryArray;

					EntryArray					entry_field;
					PriorityArray				priority_field;
					index_t						section_begin,
												section_end;
			inline	void						dec(size_t&offset);
					bool						checkIntegrity();
					void						eraseAddr(index_t index);				//!< Erases the specified element (0 pointing to the element with least priority and count()-1 to the element with the highest priority in the queue)

			public:
				typedef QueueIterator<Entry,Element,EntryStrategy>	iterator;
	
				/**/				PriorityQueue(size_t size=1024);
				/**/				~PriorityQueue()	{clear();}

				iterator			begin();
				iterator			end();
				bool				find(const Priority&priority, iterator&target);
				bool				find(const Entry&data, const Priority&priority, iterator&target);
				bool				pop(Entry&out, Priority&pout);		//!< Pops the element of greatest priority from the queue and writes it to @b out decreasing the number of stored elements by one. Note that the stored object IS destroyed. @param out Target reference to write to @param pout Priority of the popped element @return true if the queue was not empty and an element was written to @b out , false otherwise
				bool				pop(Entry&out);						//!< Pops the element of greatest priority from the queue and writes it to @b out decreasing the number of stored elements by one. Note that the stored object IS destroyed. @param out Target reference to write to @return true if the queue was not empty and an element was written to @b out , false otherwise
				Entry				Pop();								//!< Pops the element of greatest priority from the queue, decreasing the number of stored elements by one. Note that the stored object IS destroyed
				void				EraseFront();						//!< Erases the element of greatest priority from the queue, decreasing the number of stored elements by one. Identical to Pop() but without copy constructors
				bool				PopLeast(Entry&out, Priority&pout);	//!< Pops the element of least priority from the queue and writes it to @b out decreasing the number of stored elements by one. Note that the stored object IS destroyed. @param out Target reference to write to @param pout Priority of the popped element @return true if the queue was not empty and an element was written to @b out , false otherwise
				bool				PopLeast(Entry&out);				//!< Pops the element of least priority from the queue and writes it to @b out decreasing the number of stored elements by one. Note that the stored object IS destroyed. @param out Target reference to write to @return true if the queue was not empty and an element was written to @b out , false otherwise
				Entry				PopLeast();							//!< Pops the element of least priority from the queue decreasing the number of stored elements by one. Note that the stored object IS destroyed
				void				EraseLeast();						//!< Erases the element of least priority from the queue, decreasing the number of stored elements by one. Identical to PopLeast() but without copy constructors
				void				Push(const Entry&data, const Priority&priority);		//!< Pushes an element into the queue increasing the number of stored elements by one. The queue automatically increases the size of its data field if appropriate @param data Element to push into the queue @param priority Priority of the newly inserted element
				bool				alterPriority(const Entry&data, const Priority&old_priority, const Priority&new_priority);
				bool				IsEmpty()					const;	//!< Identical to length()==0
				bool				IsNotEmpty()				const;	//!< Identical to length()!=0
				count_t				length()					const;	//!< Returns the current number of element stored in the queue
				inline 	count_t		count()						const	{return length();};	//!< Returns the current number of element stored in the queue
				inline	count_t		size()						const {return length();}		//!< Identical to length()
				bool				operator>>(Entry&entry);			//!< Identical to pop()
				Entry&				peek();								//!< Returns a reference to the next returned element (of greatest priority) in the queue @return next element in the queue
				const Entry&		peek()						const;	//!< Returns a reference to the next returned element (of greatest priority) in the queue @return next element in the queue
				const Priority&		peekPriority()				const;	//!< Returns the priority of the next returned element (of greatest priority) in the queue @return priority of the next element in the queue
				Entry&				peekLeast();						//!< Returns a reference to the next returned element (of greatest priority) in the queue @return next element in the queue
				const Entry&		peekLeast()					const;	//!< Returns a reference to the next returned element (of greatest priority) in the queue @return next element in the queue
				const Priority&		peekLeastPriority()			const;	//!< Returns the priority of the next returned element (of greatest priority) in the queue @return priority of the next element in the queue

				void				Erase(const iterator&it);			//!< Erases the specified element (the iterator stays valid but should probably be decremented after the operation)
				const Priority&		priorityOf(const iterator&it)	const;
				void				clear();							//!< Clears the local queue of all entries. No objects are actually erased
			
			};



		#include "queue.tpl.h"
	}
}

#endif


