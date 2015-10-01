#ifndef bufferH
#define bufferH


#include <exception>
#include <new>
#include <algorithm>

#include "strategy.h"


#define __BUFFER_RVALUE_REFERENCES__	1		//!< Allows && references in constructors and assignment operators
#define __BUFFER_DBG_RANGE_CHECK__		1		//!< Checks index ranges before access and issues a FATAL__ in case of a violation (DEBUG MODE ONLY)
#define __BUFFER_DBG_FILL_STATE__		1		//!< Maintains a count_t variable that provides the number of elements in the buffer (DEBUG MODE ONLY)

namespace Container
{


	template <typename T, typename Strategy=typename ::StrategySelector<T>::Default>
		class BasicBuffer
		{
		protected:
			T						*storage_begin,		//!< Pointer to the first element of the storage area. May be NULL indicating an empty buffer
									*storage_end,		//!< Pointer one past the last valid element of the storage area. May be NULL indicating an empty buffer. (storage_end-storage_begin) stays valid even if the buffer is empty
									*usage_end;			//!< Pointer one past the last allocated element of the storage area. May be NULL indicating an empty buffer. (usage_end-storage_begin) stays valid even if the buffer is empty
			#if defined(_DEBUG) && __BUFFER_DBG_FILL_STATE__
				count_t				fill_state;			//!< Current fill state in elements. Maintained in debug mode only
			#endif


			inline static	T*		allocate(count_t len);		//!< Allocates the specified number of elements via malloc. No constructors are called. The function returns NULL if len is 0. std::bad_alloc may be thrown if allocation fails
			inline static	T*		allocateNotEmpty(count_t len);	//!< Identical to the above but without length check. @a len must not be 0. passing 0 causes undefined behavior


			inline void				ensureHasSpace(count_t elements);	//!< Ensures that at least the specified number of non-allocated elements is available from @a usage_end to @a storage_end . The local storage may be resized/allocated if necessary. Element movement behavior is described by the passed @a Behavior struct. The requested elements are not constructed. std::bad_alloc may be thrown
			inline void				ensureHasSpace();					//!< Similar to the above. Ensures that at least one more non-allocated element is available.

	
			inline void				genericCompactify();				//!< Reduces storage size to the number of currently allocated elements. If the storage must be reallocated then elements are moved as described in the specified @a Behavior struct. std::bad_alloc may be thrown
	
			static inline void		destructAndFree(T*range_begin, T*range_end);	//!< Invokes destructors for all elements from @a range_begin to @a range_end excluding @a range_end . Then invokes free(@a range_begin ). @a range_begin may be NULL if @a range_end is too.

		public:
			typedef BasicBuffer<T,Strategy>		Self;
			typedef	Strategy						AppliedStrategy;
			typedef T*								iterator;
			typedef const T*						const_iterator;

			explicit				BasicBuffer(count_t len);			//!< Constructor \param len Initial buffer size in elements.
			/**/					BasicBuffer(const BasicBuffer<T,Strategy>&other);
			#if __BUFFER_RVALUE_REFERENCES__
				/**/				BasicBuffer(BasicBuffer<T,Strategy>&&other);
			#endif
			virtual				   ~BasicBuffer();
			Self&					operator=(const ArrayData<T>&array);
			Self&					operator=(const Self&other);
			#if __BUFFER_RVALUE_REFERENCES__
				Self&				operator=(Self&&other);
			#endif

			inline Self&			operator<<(const T&element);			//!< Appends \b element to the end of the buffer, advancing the buffer cursor by one. The buffer will automatically be doubled in size if it is full at the time this operator is invoked.
			#if __BUFFER_RVALUE_REFERENCES__
				inline Self&		operator<<(T&&element);			//!< Appends \b element to the end of the buffer, advancing the buffer cursor by one. The buffer will automatically be doubled in size if it is full at the time this operator is invoked.
			#endif
			inline index_t			push_back(const T&element);				//!< @copydoc operator<<() @return Index of the appended element (buffer fill state prior to appending)
			inline void				clear(count_t len);						//!< Reallocates the buffer to the specified size and resets the buffer cursor. Any stored data is destructed.
			inline void				clear();								//!< @copydoc reset()
			inline void				Clear(count_t len)	/**@copydoc clear(count_t len)*/ {clear(len);}
			inline void				Clear()				/**@copydoc clear()*/ {clear();}
			void					setSize(count_t len, bool occupy);		//!< Resizes the local buffer size @param len New length (in elements) that the buffer storage should hold @param occupy Pass true to call the constructor on all unconstructed elements and set fill state to the specified size, false to clear all data and set fill state to 0
			inline void				SetSize(count_t len, bool occupy)		/**@copydoc setSize()*/ {setSize(len,occupy);}
			void					resizePreserveContent(count_t len);		//!< Resizes the local buffer size but preserves the old content and fill state where possible. If the old fill state exceeds the new size then all new elements will be occupied and initialized
			inline void				ResizePreserveContent(count_t len)		/**@copydoc resizePreserveContent()*/ {resizePreserveContent(len);}
			void					fill(const T&pattern);					//!< Copies \b pattern to each element of the local buffer
			inline void				Fill(const T&pattern)					/**@copydoc fill()*/ {Fill(pattern);}
			template <typename Strategy2>
				Self&				moveAppend(BasicBuffer<T,Strategy2>&buffer, bool clearSourceOnCompletion=true);	//!< Appends all elements in the specified other buffer to the end of the local buffer. The elements will be moved, leaving the parameter buffer empty upon completion.
			template <typename Strategy2>
				inline Self&		MoveAppend(BasicBuffer<T,Strategy2>&buffer, bool clearSourceOnCompletion=true)	/**@copydoc moveAppend()*/ {return moveAppend(buffer,clearSourceOnCompletion);}
			Self&					moveAppend(ArrayData<T>&array, bool clearSourceOnCompletion=true);					//!< Appends all elements in the specified array to the end of the local buffer. The elements will be moved, leaving the parameter array empty upon completion.	
			inline Self&			MoveAppend(ArrayData<T>&array, bool clearSourceOnCompletion=true)	/**@copydoc moveAppend()*/ {return moveAppend(array,clearSourceOnCompletion);}
			Self&					moveAppend(T*data, count_t elements);			//!< Appends all elements in the specified range to the end of the local buffer. The elements will be moved, leaving the individual objects of the parameter field empty upon completion.
			inline Self&			MoveAppend(T*data, count_t elements)	/**@copydoc moveAppend()*/ {return moveAppend(data,elements);}
			template <typename T2, typename Strategy2>
				Self&				AppendSubList(const BasicBuffer<T2,Strategy2>&other, index_t offset, count_t maxElements);
			template <typename T2>
				Self&				append(const T2*data, count_t elements);	//!< Appends a number of elements to the end of the buffer, advancing the buffer cursor by the specified number of elements. The buffer will automatically be resized if necessary.
			template <typename T2>
				inline Self&		Append(const T2*data, count_t elements)		/**@copydoc append()*/ {return append(data,elements);}
			template <typename T2>
				Self&				append(const ArrayData<T2>&array);				//!< Appends a number of elements to the end of the buffer, advancing the buffer cursor by the specified array's contained of elements. The buffer will automatically be resized if necessary.
			template <typename T2>
				inline Self&		Append(const ArrayData<T2>&array)			/**@copydoc append()*/ {return append(array);}
			template <typename T2, typename Strategy2>
				Self&				append(const BasicBuffer<T2,Strategy2>&buffer);	//!< Appends a number of elements to the end of the buffer, advancing the buffer cursor by the specified array's contained of elements. The buffer will automatically be resized if necessary.
			template <typename T2, typename Strategy2>
				inline Self&		Append(const BasicBuffer<T2,Strategy2>&buffer)	/**@copydoc append()*/ {return append(buffer);}
			template <typename T2>
				Self&				appendAddresses(T2*data, count_t elements);	//!< Appends the addresses of the specified elements to the end of the buffer (assuming T is a pointer type fo T2). The buffer will automatically be resized if necessary.
			template <typename T2>
				inline Self&		AppendAddresses(T2*data, count_t elements)	/**@copydoc appendAddresses()*/ {return appendAddresses(data,elements);}
			template <typename T2>
				Self&				appendAddresses(ArrayData<T2>&array);	//!< Appends the addresses of the specified array's elements to the end of the buffer (assuming T is a pointer type fo T2). The buffer will automatically be resized if necessary.
			template <typename T2>
				inline Self&		AppendAddresses(ArrayData<T2>&array)	/**@copydoc appendAddresses()*/ {return appendAddresses(array);}
			template <typename T2>
				Self&				appendVA(count_t elements, ...);		//!< Appends a number of elements to the buffer
			inline void				reset();							//!< Resets the buffer cursor to the beginning. Does \b not resize the local buffer.
			inline void				Reset()								/**@copydoc reset()*/	{reset();}
			inline count_t			fillLevel()					const;	//!< Returns the current buffer cursor relative to the buffer beginning.
			inline count_t			GetFillLevel()				const	/**@copydoc fillLevel()*/ {return fillLevel();}
			inline count_t			length()					const;	//!< @copydoc fillLevel()
			inline count_t			GetLength()					const	/**@copydoc fillLevel()*/ {return fillLevel();}
			inline count_t			size()						const;	//!< @copydoc fillLevel()
			inline count_t			count()						const;	//!< @copydoc fillLevel()
			inline count_t			Count()						const;	//!< @copydoc fillLevel()
			inline count_t			operator()()				const	{ return fillLevel(); }
			inline count_t			storageSize()				const;	//!< Queries the number of elements held in total. This also includes unconstructed incremental storage
			inline count_t			GetStorageSize()			const	/**@copydoc storageSize()*/ {return storageSize();}
			inline size_t			GetContentSize()			const {return fillLevel() * sizeof(T);}
			inline bool				empty()						const;	//!< @copydoc isEmpty()
			inline bool				isEmpty()					const;	//!< Returns true if the buffer holds no elements
			inline bool				IsEmpty()					const	/**@copydoc isEmpty()*/ {return isEmpty();}
			inline bool				isNotEmpty()				const;	//!< Returns true if the buffer holds at least one element
			inline bool				IsNotEmpty()				const	/**@copydoc isNotEmpty()*/ {return isNotEmpty();}
			bool					truncate(count_t fill_state);		//!< Decrements the local buffer counter to the specified fill state. The method fails if the local buffer fill state is less or equal the specified fill state. The actually allocated buffer size remains unchanged.
			inline bool				Truncate(count_t fill_state)		/**@copydoc truncate()*/ {return truncate(fill_state);}
			inline T*				appendRow(count_t length);			//!< Appends a number of elements and returns a pointer to the first element. The method returns NULL, if length is 0 @param length Number of elements to append. Must be greater 0 @return Pointer to the first of the appended elements or NULL, if an error occured
			inline T*				AppendRow(count_t length)			/**@copydoc appendRow()*/ {return appendRow(length);}
			inline T*				appendRow(count_t length, const T&pattern);			//!< Appends a number of elements and returns a pointer to the first element. The method returns NULL, if length is 0 @param length Number of elements to append. Must be greater 0 @param pattern Data to fill newly appended elements with @return Pointer to the first of the appended elements or NULL, if an error occured
			inline T*				AppendRow(count_t length, const T&pattern)			/**@copydoc appendRow()*/ {return appendRow(length,pattern);}
			inline T&				append();							//!< Appends a single element and returns a reference to it
			inline T&				Append()							/**@copydoc append()*/	{return append();}
			inline T&				append(const T&);					//!< Appends a single element via copy, and returns a reference to it in buffer storage
			inline T&				Append(const T&element)				/**@copydoc append()*/	{return append(element);}
			inline index_t			appendIfNotFound(const T&);			//!< Appends a single element via copy, assuming an equal element does not already exist in the local buffer (compared via == operator) @return Index of the first found or the newly appended element
			inline index_t			AppendIfNotFound(const T&element)	/**@copydoc appendIfNotFound()*/	{return appendIfNotFound(element);}
			inline T&				moveAppend(T&);						//!< Appends a single element via move, and returns a reference to it in buffer storage
			inline T&				MoveAppend(T&element)				/**@copydoc moveAppend()*/	{return moveAppend(element);}
			#if __BUFFER_RVALUE_REFERENCES__
				inline T&			append(T&&);						//!< Appends a single element via move, and returns a reference to it in buffer storage
				inline T&			Append(T&&element)					/**@copydoc append()*/ {return append(std::move(element));}
				inline T&			MoveAppend(T&&element)				/**@copydoc moveAppend()*/ { return append(std::remove(element)); }
			#endif

			inline T&				insert(index_t before_element);		//!< Inserts an element into the buffer and returns a reference to it
			inline T&				Insert(index_t before_element)		/**@copydoc insert()*/ {return insert(before_element);}
			template <typename T2>
				inline T&			insert(index_t before_element, const T2&init_data);	//!< Inserts an element into the buffer and returns a reference to it. @a init_data is passed to the constructor or assignment operator of the new element
			template <typename T2>
				inline T&			Insert(index_t before_element, const T2&init_data)		/**@copydoc insert()*/ {return insert(before_element,init_data);}
			inline T*				insertRow(index_t before_element, count_t length);		//!< Appends a number of elements and returns a pointer to the first element. The method returns NULL, if length is 0 @param length Number of elements to append. Must be greater 0 @return Pointer to the first of the appended elements or NULL, if an error occured
			inline T*				InsertRow(index_t before_element, count_t length)		/**@copydoc insertRow()*/ {return insertRow(before_element,length);}

			inline T				pop();								//!< Pops the last element from the buffer (the internally allocated memory section is not resized, only the local fill state decremented and the last object destructed). The method behavior is undefined if the buffer is empty.
			inline T				Pop()								/**@copydoc pop()*/ {return pop();}
			inline void				eraseLast();						//!< Simplified void-version of pop(). Can be more efficient if the contained type is complex and the returned object not used anyway. The method behavior is undefined if the buffer is empty.
			inline void				EraseLast()							/**@copydoc eraseLast()*/ {eraseLast();}
			inline T&				first();							//!< Retrieves a reference to the first element in the buffer. The behavior of this method is undefined if the local buffer is empty
			inline const T&			first()	const;						//!< @overload
			inline T&				last();								//!< Retrieves a reference to the last element in the buffer. The behavior of this method is undefined if the local buffer is empty
			inline const T&			last()	const;						//!< @overload
			inline void				erase(index_t index);				//!< Removes the specified element from the buffer. The buffer's contained element count decreases by one if the specified index is valid.
			inline void				Erase(index_t index)				/**@copydoc erase()*/ {erase(index);}
			inline void				erase(index_t index, index_t elements);		//!< Removes a range of elements from the buffer. The buffer's contained element count decreases by one if the specified index is valid. @a elements may be reduced if the range surpasses the end of the consumed buffer space
			inline void				Erase(index_t index, index_t elements)		/**@copydoc erase()*/ {erase(index,elements);}

			inline iterator			erase(iterator it);

			inline iterator			begin()	{return storage_begin;}
			inline const_iterator	begin()	const {return storage_begin;}
			inline iterator			end()	{return usage_end;}
			inline const_iterator	end() const {return usage_end;}
			inline void				revert();
			inline void				Revert()							/**@copydoc revert()*/	{revert();}
			inline T*				pointer();							//!< Returns a pointer to the beginning of the buffer
			inline const T*			pointer()					const;	//!< Returns a pointer to the beginning of the buffer
			inline T&				operator[](index_t);
			inline const T&			operator[](index_t)			const;
			inline T&				at(index_t);
			inline const T&			at(index_t)			const;

			inline bool				Owns(const T*element)	const;	//! Queries if the specified entry pointer was taken from the local buffer. Actual pointer address is checked, not what it points to.

			inline bool				operator==(const BasicBuffer<T,Strategy>&other) const;
			inline bool				operator!=(const BasicBuffer<T,Strategy>&other) const;

			inline T&				fromEnd(index_t);					//!< Retrieves the nth element from the end of the consumed buffer space. fromEnd(0) is identical to last()
			inline const T&			fromEnd(index_t)			const;	//!< @copydoc fromEnd()
			void					compact();							//!< Reduces the local buffer size to the exact fill state and copies all contained elements. Any succeeding push operation will automatically increase buffer size again. The method returns if the stack is already of compact size
			inline void				Compact()							/**@copydoc compact()*/	{compact();}
			Array<T,Strategy>		copyToArray()						const;	//!< Exports the local data up to the current fill state to the returned array.
			inline Array<T,Strategy>CopyToArray()						const	/**@copydoc copyToArray()*/	{return copyToArray();}
			void					copyToArray(ArrayData<T>&target)	const;	//!< Exports the local data up to the current fill state to the specified array. The target array will be resized if necessary
			inline void				CopyToArray(ArrayData<T>&target)	const	/**@copydoc copyToArray()*/	{copyToArray(target);}
			void					moveToArray(ArrayData<T>&target, bool reset_buffer=true);		//!< Moves local data up to the current fill state to the specified array. Move behavior is defined by the used Strategy class. @param reset_buffer Set true to automatically reset the buffer once element movement is completed
			inline void				MoveToArray(ArrayData<T>&target, bool reset_buffer=true)	/**@copydoc moveToArray()*/	{moveToArray(target,reset_buffer);}
			template <typename T2>
				inline bool			contains(const T2&element)	const;	//!< Determines whether or not an equivalent to the specified element is currently stored in the active region of the buffer. Comparison is done via the ==operator.
			template <typename T2>
				inline bool			Contains(const T2&element)	const	/**@copydoc contains()*/	{return contains(element);}
			template <typename T2>
				inline index_t		indexOf(const T2&element)	const;	//!< Determines the index of the specified element via == operator. If no match was found then index_t(-1)/InvalidIndex is returned
			template <typename T2>
				inline index_t		GetIndexOf(const T2&element)const	/**@copydoc indexOf()*/	{return indexOf(element);}
			template <typename T2>
				bool				findAndErase(const T2&element);		//!< Attempts to find and erase the specified element @return true if the specified element could be found (via ==operator), false otherwise
			template <typename T2>
				inline bool			FindAndErase(const T2&element)		/**@copydoc findAndErase()*/	{return findAndErase(element);}

			void					adoptData(BasicBuffer<T,Strategy>&other);	//!< Adopts all attributes of the specified other buffer leaving it empty. Any existing local data is deleted.
			template <typename S2>
				void				swap(BasicBuffer<T,S2>&other);			//!< Swaps data with the other buffer
			template <typename IndexType>
				inline T*			operator+(IndexType delta);				//!< Returns a pointer to the beginning of the buffer plus the specified delta
			template <typename IndexType>
				inline const T*		operator+(IndexType delta)		const;	//!< Returns a pointer to the beginning of the buffer plus the specified delta


			friend void swap(Self& a, Self& b)
			{
				a.swap(b);
			}
		};



		//! General buffer structure. Stores object copies rather than pointers making it extremely fast for primitive types
	template <typename T, count_t InitialLength=128, typename Strategy=typename ::StrategySelector<T>::Default>
		class Buffer:public BasicBuffer<T,Strategy>
		{
		public:
			typedef BasicBuffer<T,Strategy>				Super;
			typedef Buffer<T,InitialLength,Strategy>	Self;
			typedef	Strategy							AppliedStrategy;


			explicit			Buffer(count_t len=InitialLength):Super(len)
								{}
								Buffer(const Super&other):Super(other)
								{}
								Buffer(const Self&other):Super(other)
								{}
			#if __BUFFER_RVALUE_REFERENCES__
				template<count_t Len>
								Buffer(Buffer<T,Len,Strategy>&&other):Super(std::move(other))
								{}
								Buffer(Self && other) :Super(std::move(other))
								{}
								Buffer(Super&&other) :Super(std::move(other))
								{}
				Self&			operator=(Self&&other) { Super::adoptData(other); return *this; }
				Self&			operator=(Super&&other) { Super::adoptData(other); return *this; }
			#endif
			Self&				operator=(const ArrayData<T>&array){Super::operator=(array); return *this;}
			Self&				operator=(const Self&other) { Super::operator=(other); return *this; }
			Self&				operator=(const Super&other) { Super::operator=(other); return *this; }

			friend void swap(Self& a, Self& b)
			{
				a.swap(b);
			}

			using Super::operator<<;
			using Super::operator[];
			using Super::operator+;
		};

	template <typename T, typename Strategy=typename ::StrategySelector<T>::Default>
		class Buffer0:public Buffer<T,0,Strategy>
		{
		public:
			typedef Buffer<T, 0, Strategy>				Super;
			typedef Buffer0<T,Strategy>					Self;
			typedef	Strategy							AppliedStrategy;


			explicit			Buffer0() :Super(0)
								{}
								Buffer0(const Super&other):Super(other)
								{}
								Buffer0(const Self&other):Super(other)
								{}
			#if __BUFFER_RVALUE_REFERENCES__
				template<count_t Len>
								Buffer0(Buffer0<T,Strategy>&&other):Super(std::move(other))
								{}
								Buffer0(Self&&other):Super(std::move(other))
								{}
								Buffer0(Super&&other):Super(std::move(other))
								{}
				Self&			operator=(Self&&other){Super::adoptData(other); return *this;}
				Self&			operator=(Super&&other){Super::adoptData(other); return *this;}
			#endif
			Self&				operator=(const ArrayData<T>&array){Super::operator=(array); return *this;}
			Self&				operator=(const Self&other) { Super::operator=(other); return *this; }
			Self&				operator=(const Super&other) { Super::operator=(other); return *this; }

			friend void swap(Self& a, Self& b)
			{
				a.swap(b);
			}

			using Super::operator<<;
			using Super::operator[];
			using Super::operator+;
		};
	template <count_t InitialLength=1024>
		class WriteBuffer:public Buffer<BYTE,InitialLength,PrimitiveStrategy>, public IWriteStream
		{
		public:
			typedef Buffer<BYTE,InitialLength,PrimitiveStrategy>	Super;

			virtual	bool		write(const void*data, serial_size_t size)
								{
									Super::append((const BYTE*)data,static_cast<size_t>(size));
									return true;
								}		
		};


	#include "buffer.tpl.h"
}

template <typename T, typename Strategy>
	class StrategySelector<Container::BasicBuffer<T, Strategy> >
	{
	public:
		typedef	SwapStrategy		Default;
	};

template <typename T, count_t InitialLength, typename Strategy>
	class StrategySelector<Container::Buffer<T, InitialLength, Strategy> >
	{
	public:
		typedef	SwapStrategy		Default;
	};

template <typename T, typename Strategy>
	class StrategySelector<Container::Buffer0<T, Strategy> >
	{
	public:
		typedef	SwapStrategy		Default;
	};


template <count_t InitialLength>
	class StrategySelector<Container::WriteBuffer<InitialLength> >
	{
	public:
		typedef	SwapStrategy		Default;
	};


using namespace Container;

#endif
