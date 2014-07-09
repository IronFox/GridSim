#if !defined(queueH) && !defined(__CUDACC__)
#define queueH

/******************************************************************

Collection of different queues.

******************************************************************/


#include "dynamic_elements.h"

template <class Entry>
	class QueueIterator
	{
	private:
			Entry			*field_begin,
							*current,
							*field_end;
	public:
			typedef QueueIterator<Entry>	It;
			
							QueueIterator(Entry*begin,Entry*end,Entry*c);
			It&				operator++();
			It				operator++(int);
			It				operator+(int delta)	const;
			It&				operator--();
			It				operator--(int);
			It				operator-(int delta)	const;
			bool			operator==(const It&other)	const;
			bool			operator!=(const It&other)	const;
			Entry&			operator*();
			Entry*			operator->();
			
			size_t		index()	const;
	};

template <class Entry, class Strategy=typename StrategySelector<Entry>::Default>
	class Queue:protected Array<Entry,Strategy>
	{
	protected:
			Entry			*section_begin,
							*section_end,
							*field_end;
			typedef Array<Entry,Strategy>	Array;

			void			increaseSize(count_t new_size);
	public:
			typedef QueueIterator<Entry>	iterator;
			typedef QueueIterator<const Entry>	const_iterator;
			
	
							Queue(size_t size=1024);

			iterator		begin();
			iterator		end();
			const_iterator	begin()	const;
			const_iterator	end() const;
			bool			pop(Entry&out);						//!< Pops the oldest element from the queue, and writes it to @b out , decreasing the number of stored elements by one. Note that the stored object is not destroyed. @param out Target reference to write to @return true if the queue was not empty and an element was written to @b out , false otherwise
			Entry&			pop();								//!< Pops the oldest element from the queue, decreasing the number of stored elements by one. Note that the stored object is not destroyed.
			count_t			pop(Entry*out_field, count_t count);			//!< Pops up to @a count of the oldest elements to the specified out field @return Number of elements that were actually popped (which may be less than the resquested number if not enough elements are stored)
			void			push(const ArrayData<Entry>&entries);	//!< Pushes multiple elements into the queue, increasing the number of stored elements by <em>entries.count()</em>. The queue automatically increases the size of its data field if appropriate
			void			push(const Entry*, count_t count);	//!< Pushes multiple elements into the queue, increasing the number of stored elements by <em>count</em>. The queue automatically increases the size of its data field if appropriate
			void			push(const Entry&data);				//!< Pushes an element into the queue, increasing the number of stored elements by one. The queue automatically increases the size of its data field if appropriate
			Entry&			push();								//!< Pushes a new empty element into the queue.

			bool			isEmpty()					const;	//!< Identical to length()==0
			bool			isNotEmpty()				const;	//!< Identical to length()!=0
	inline 	bool			IsEmpty()					const	{return isEmpty();}
	inline 	bool			IsNotEmpty()				const	{return isNotEmpty();}
			count_t			length()					const;	//!< Returns the current number of element stored in the queue
	inline	count_t			size()						const {return length();}		//!< Identical to length()
			bool			operator>>(Entry&entry);			//!< Identical to pop()
			Queue<Entry,Strategy>&	operator<<(const Entry&entry);		//!< Identical to push() @return *this
			Entry&			peek();								//!< Returns a reference to the last (oldest) element in the queue @return last element in the queue
			const Entry&	peek()						const;	//!< Returns a reference to the last (oldest) element in the queue @return last element in the queue
			Entry&			oldest();							//!< Returns a reference to the last (oldest) element in the queue. Identical to peek() @return last element in the queue
			const Entry&	oldest()					const;	//!< Returns a reference to the last (oldest) element in the queue. Identical to peek() @return last element in the queue
			Entry&			newest();							//!< Returns a reference to the first (newest) element in the queue @return first element in the queue
			const Entry&	newest()					const;	//!< Returns a reference to the first (newest) element in the queue @return first element in the queue
			Entry&			operator[](size_t index);			//!< Returns the nth element from the queue. @param index Index of the element to return ranging [0, length()-1] @return Reference to the requested object
			const Entry&	operator[](size_t index)	const;	//!< Returns the nth element from the queue. @param index Index of the element to return ranging [0, length()-1] @return Reference to the requested object
			void			clear();							//!< Clears the local queue of all entries. No objects are actually erased

			bool			Contains(const Entry&entry)	const;	//!< Checks if the queue contains the specified element
			bool			contains(const Entry&entry)	const	/** @copydoc Contains()*/ {return Contains(entry);}

			void			operator=(const Queue<Entry,Strategy>&other);	//!< Copies all elements from the remote queue to the local queue. The local buffer size is resized to match the remote buffer size but only actually used elements are copied
			void			swap(Queue<Entry,Strategy>&other);
			void			adoptData(Queue<Entry,Strategy>&other);
			
	inline 	size_t			count()						const	{return length();};	//!< Returns the current number of element stored in the queue
	};

template <class Entry, class Priority, class EntryStrategy=typename StrategySelector<Entry>::Default>
	class PriorityQueue
	{
	protected:
			typedef	Array<Priority>			PriorityArray;
			typedef Array<Entry,EntryStrategy>	EntryArray;

			EntryArray					entry_field;
			PriorityArray				priority_field;
			index_t						section_begin,
										section_end;
	inline	void						dec(size_t&offset);
			bool						checkIntegrity();
			void						eraseAddr(index_t index);				//!< Erases the specified element (0 pointing to the element with least priority and count()-1 to the element with the highest priority in the queue)

	public:
			typedef QueueIterator<Entry>	iterator;
	
								PriorityQueue(size_t size=1024);
			iterator			begin();
			iterator			end();
			bool				find(const Priority&priority, iterator&target);
			bool				find(const Entry&data, const Priority&priority, iterator&target);
			bool				pop(Entry&out, Priority&pout);		//!< Pops the element of greatest priority from the queue and writes it to @b out decreasing the number of stored elements by one. Note that the stored object is not destroyed. @param out Target reference to write to @param pout Priority of the popped element @return true if the queue was not empty and an element was written to @b out , false otherwise
			bool				pop(Entry&out);						//!< Pops the element of greatest priority from the queue and writes it to @b out decreasing the number of stored elements by one. Note that the stored object is not destroyed. @param out Target reference to write to @return true if the queue was not empty and an element was written to @b out , false otherwise
			Entry&				pop();								//!< Pops the element of greatest priority from the queue decreasing the number of stored elements by one. Note that the stored object is not destroyed
			bool				popLeast(Entry&out, Priority&pout);	//!< Pops the element of least priority from the queue and writes it to @b out decreasing the number of stored elements by one. Note that the stored object is not destroyed. @param out Target reference to write to @param pout Priority of the popped element @return true if the queue was not empty and an element was written to @b out , false otherwise
			bool				popLeast(Entry&out);				//!< Pops the element of least priority from the queue and writes it to @b out decreasing the number of stored elements by one. Note that the stored object is not destroyed. @param out Target reference to write to @return true if the queue was not empty and an element was written to @b out , false otherwise
			Entry&				popLeast();							//!< Pops the element of least priority from the queue decreasing the number of stored elements by one. Note that the stored object is not destroyed
			void				push(const Entry&data, const Priority&priority);		//!< Pushes an element into the queue increasing the number of stored elements by one. The queue automatically increases the size of its data field if appropriate @param data Element to push into the queue @param priority Priority of the newly inserted element
			bool				alterPriority(const Entry&data, const Priority&old_priority, const Priority&new_priority);
			bool				isEmpty()					const;	//!< Identical to length()==0
			bool				isNotEmpty()				const;	//!< Identical to length()!=0
			count_t				length()					const;	//!< Returns the current number of element stored in the queue
	inline 	count_t				count()						const	{return length();};	//!< Returns the current number of element stored in the queue
	inline	count_t				size()						const {return length();}		//!< Identical to length()
			bool				operator>>(Entry&entry);			//!< Identical to pop()
			Entry&				peek();								//!< Returns a reference to the next returned element (of greatest priority) in the queue @return next element in the queue
			const Entry&		peek()						const;	//!< Returns a reference to the next returned element (of greatest priority) in the queue @return next element in the queue
			const Priority&		peekPriority()				const;	//!< Returns the priority of the next returned element (of greatest priority) in the queue @return priority of the next element in the queue
			Entry&				peekLeast();						//!< Returns a reference to the next returned element (of greatest priority) in the queue @return next element in the queue
			const Entry&		peekLeast()					const;	//!< Returns a reference to the next returned element (of greatest priority) in the queue @return next element in the queue
			const Priority&		peekLeastPriority()			const;	//!< Returns the priority of the next returned element (of greatest priority) in the queue @return priority of the next element in the queue

			void				erase(const iterator&it);			//!< Erases the specified element (the iterator stays valid but should probably be decremented after the operation)
			const Priority&		priorityOf(const iterator&it)	const;
			void				clear();							//!< Clears the local queue of all entries. No objects are actually erased
			
	};


/*
template <class C, class IndexType> class Queue
{
private:
        C               loop[0x1<<(sizeof(IndexType)*8)],
                        element;
        IndexType       first,last;
public:
                        Queue();
        const C&        peek()                  const;
        const C&        get();
        const C&        peek(IndexType element) const;
        void            insert(const C&item);
        void            clear();
        IndexType       count()                 const;
        bool            full()                  const;
        bool            empty()                 const;
};

template <class C, class IndexType> class CPQueue
{
private:
        C              *loop[0x1<<(sizeof(IndexType)*8)];
        IndexType       first,last;
public:
                        CPQueue();
        C*              peek();
        C*              get();
        C*              peek(IndexType element);
        void            insert(C*item);
        void            clear();
        void            flush();
        IndexType       count()                 const;
        bool            full()                  const;
        bool            empty()                 const;
};

template <class C>  class Queue8:public Queue<C,unsigned char>
{};

template <class C>  class Queue16:public Queue<C,unsigned short>
{};

template <class C>  class CPQueue8:public CPQueue<C,unsigned char>
{};

template <class C>  class CPQueue16:public CPQueue<C,unsigned short>
{};


template <class C>  class DynamicQueue
{
private:
        SDynamicPath<C> *first,*last;
        size_t        volume;
        C               element;
public:
                        DynamicQueue();
virtual                ~DynamicQueue();
        const C&        peek()                  const;
        const C&        get();
        void            insert(const C&item);
        void            clear();
        size_t        count()                 const;
        bool            empty()                 const;
};


template <class C>  class DynamicPQueue
{
private:
        SDynamicPath<C*>*first,*last;
        size_t        volume;
public:
                        DynamicPQueue();
virtual                ~DynamicPQueue();
        C*              peek();
        C*              get();
        void            insert(C*item);
        void            clear();
        void            flush();
        size_t        count()                 const;
        bool            empty()                 const;
};
*/

#include "queue.tpl.h"


#endif


