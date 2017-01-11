#ifndef bufferTplH
#define bufferTplH



#undef min

#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
	#define CHK_FILLSTATE //if (fillLevel != usage_end - storage_begin) FATAL__("Invalid buffer state")
#else
	#define CHK_FILLSTATE
#endif


#if defined(_DEBUG) && __BUFFER_DBG_RANGE_CHECK__
	#if __BUFFER_DBG_fillLevel__
		#define BUFFER_CHECK_RANGE(_INDEX_)	if ((_INDEX_) >= fillLevel) FATAL__("Index out of range for buffer access");
	#else
		#define BUFFER_CHECK_RANGE(_INDEX_)	if ((_INDEX_) >= fillState()) FATAL__("Index out of range for buffer access");
	#endif
	#define BUFFER_ASSERT_NOT_EMPTY()	if (usage_end == storage_begin)	FATAL__("Cannot access element: buffer is empty");
#else
	#define BUFFER_CHECK_RANGE(_INDEX_)
	#define BUFFER_ASSERT_NOT_EMPTY()
#endif





template <typename T, typename Strategy>
	inline void		BasicBuffer<T, Strategy>::destructAndFree(T*range_begin, T*range_end)
	{
		Strategy::destructRange(range_begin,range_end);
		free(range_begin);
	}


template <typename T, typename Strategy>
	inline T*	BasicBuffer<T, Strategy>::allocate(count_t len)
	{
		if (!len)
			return NULL;
		T*rs = (T*)malloc(sizeof(T)*len);
		if (!rs)
			throw std::bad_alloc(/*"Unable to resize buffer instance: Insufficient memory"*/);
		return rs;
	}


template <typename T, typename Strategy>
	inline T*	BasicBuffer<T, Strategy>::allocateNotEmpty(count_t len)
	{
		T*rs = (T*)malloc(sizeof(T)*len);
		if (!rs)
			throw std::bad_alloc(/*"Unable to resize buffer instance: Insufficient memory"*/);
		return rs;
	}


template <typename T, typename Strategy>
	inline void BasicBuffer<T, Strategy>::ensureHasSpace(count_t elements)
	{
		count_t	current_size = storage_end-storage_begin,
				target_size = current_size,
				Fill = usage_end-storage_begin,
				want_size = Fill+elements;
		if (!target_size)
			target_size = 4;

		while (target_size < want_size)
		{
			count_t new_target_size = target_size << 1;
			if (new_target_size < target_size)	//overflow
				throw std::bad_alloc(/*"Unable to resize buffer instance: Size overflow"*/);
			target_size = new_target_size;
		}

		if (target_size != current_size)
		{
			try
			{
				T	*new_field = allocateNotEmpty(target_size);

				Strategy::constructRangeFromFleetingData(new_field,new_field+current_size,storage_begin);
				destructAndFree(storage_begin,usage_end);

				storage_begin = new_field;
				storage_end = storage_begin+target_size;
				usage_end = storage_begin+Fill;
				CHK_FILLSTATE

			}
			catch (std::bad_alloc& exception)
			{
				storage_begin = storage_end = usage_end = NULL;
				#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
					fillLevel = 0;
					CHK_FILLSTATE
				#endif
				throw;
			}
		}

	}

template <typename T, typename Strategy>
	inline void BasicBuffer<T, Strategy>::ensureHasSpace()
	{
		if (usage_end == storage_end)
		{
			index_t	current_index = usage_end-storage_begin;
			count_t	len = (storage_end-storage_begin)*2;

			if (len < (count_t)(storage_end-storage_begin))
				throw std::bad_alloc(/*"Unable to resize buffer instance: Size overflow"*/);

			if (!len)
				len = 4;
			try
			{
				T	*new_field = allocateNotEmpty(len);//alloc<T>(len);

				Strategy::constructRangeFromFleetingData(new_field,new_field+current_index,storage_begin);
				destructAndFree(storage_begin,usage_end);

				storage_begin = new_field;
				storage_end = storage_begin+len;
				usage_end = storage_begin+current_index;
				CHK_FILLSTATE
			}
			catch (std::bad_alloc& exception)
			{
				storage_begin = storage_end = usage_end = NULL;
				#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
					fillLevel = 0;
					CHK_FILLSTATE
				#endif
				throw;
			}
		}
	}

template <typename T, typename Strategy>
	BasicBuffer<T, Strategy>::BasicBuffer(count_t len)
	{
		try
		{
			usage_end = storage_begin = allocate(len);
			storage_end = storage_begin+len;
			#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
				fillLevel = 0;
				CHK_FILLSTATE
			#endif
		}
		catch (std::bad_alloc& exception)
		{
			usage_end = storage_begin = storage_end = NULL;
			#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
				fillLevel = 0;
				CHK_FILLSTATE
			#endif
			throw;
		}
	}



template <typename T, typename Strategy>
	BasicBuffer<T, Strategy>::~BasicBuffer()
	{
		//deloc(storage_begin);
		destructAndFree(storage_begin,usage_end);
	}

template <typename T, typename Strategy>
	void	BasicBuffer<T, Strategy>::clear(count_t len)
	{
		//deloc(storage_begin);
		Strategy::destructRange(storage_begin,usage_end);
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel = 0;
		#endif

		if (storage_end-storage_begin != len)
		{
			free(storage_begin);
			try
			{
				usage_end = storage_begin = allocate(len);
				storage_end = storage_begin+len;
			}
			catch (std::bad_alloc& exception)
			{
				usage_end = storage_begin = storage_end = NULL;
				throw;
			}
		}
		else
			usage_end = storage_begin;
		CHK_FILLSTATE
			//alloc<T>(len);
	}

template <typename T, typename Strategy>
	BasicBuffer<T, Strategy>::BasicBuffer(std::initializer_list<T> items)
	{
		count_t	Fill = items.size();
		//alloc(storage_begin,Fill);
		try
		{
			storage_begin = allocate(Fill);
		}
		catch (std::bad_alloc& exception)
		{
			storage_begin = storage_end = usage_end = NULL;
			#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
				fillLevel = 0;
				CHK_FILLSTATE
			#endif

			throw;
		}

		storage_end = usage_end = storage_begin+Fill;
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel = Fill;
			CHK_FILLSTATE
		#endif


		std::initializer_list<T>::const_iterator it = items.begin();
		for (index_t i = 0; i < Fill; i++, ++it)
		{
			new ((storage_begin+i)) T(*it);
		}
	}


template <typename T, typename Strategy>
	BasicBuffer<T, Strategy>::BasicBuffer(const BasicBuffer<T, Strategy>&other)
	{
		count_t	Fill = other.usage_end-other.storage_begin;
		//alloc(storage_begin,Fill);
		try
		{
			storage_begin = allocate(Fill);
		}
		catch (std::bad_alloc& exception)
		{
			storage_begin = storage_end = usage_end = NULL;
			#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
				fillLevel = 0;
				CHK_FILLSTATE
			#endif

			throw;
		}

		storage_end = usage_end = storage_begin+Fill;
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel = Fill;
			CHK_FILLSTATE
		#endif

		for (index_t i = 0; i < Fill; i++)
		{
			new ((storage_begin+i)) T(other.storage_begin[i]);
			//storage_begin[i] = other.storage_begin[i];
		}
	}


#if __BUFFER_RVALUE_REFERENCES__
template <typename T, typename Strategy>
	BasicBuffer<T, Strategy>::BasicBuffer(BasicBuffer<T, Strategy>&&other):storage_begin(other.storage_begin),storage_end(other.storage_end),usage_end(other.usage_end)
	{
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel = other.fillLevel;
			other.fillLevel = 0;
			CHK_FILLSTATE
		#endif

		other.storage_begin = other.storage_end = other.usage_end = NULL;
	}

template <typename T, typename Strategy>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::operator=(BasicBuffer<T, Strategy>&&other)
	{
		adoptData(other);
		return *this;
	}
#endif

template <typename T, typename Strategy>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::operator=(const BasicBuffer<T, Strategy>&other)
	{
		Strategy::destructRange(storage_begin,usage_end);
		count_t	Fill = other.usage_end-other.storage_begin;
		if (count_t(storage_end-storage_begin) < Fill)
		{
			free(storage_begin);
			try
			{
				storage_begin = allocate(Fill);
			}
			catch (std::bad_alloc& exception)
			{
				storage_begin = storage_end = usage_end = NULL;
				#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
					fillLevel = 0;
					CHK_FILLSTATE
				#endif
				throw;
			}
			storage_end = usage_end = storage_begin+Fill;
		}
		else
			usage_end = storage_begin+Fill;

		//	deloc(storage_begin);
			//alloc(storage_begin,Fill);
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel = Fill;
			CHK_FILLSTATE
		#endif

		for (index_t i = 0; i < Fill; i++)
			new ((storage_begin+i)) T(other.storage_begin[i]);
		return *this;
	}

template <typename T, typename Strategy>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::operator=(const ArrayData<T>&other)
	{
		Clear();
		T*field = AppendRow(other.Count());
		Strategy::template copyRange<const T,T>(other.begin(),other.end(),field);
		return *this;
	}

template <typename T, typename Strategy>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::operator=(std::initializer_list<T> items)
	{
		Clear();
		T*field = AppendRow(items.size());
		Strategy::copyRange(items.begin(),items.end(),field);
		return *this;
	}

	
template <typename T, typename Strategy>
	void	BasicBuffer<T, Strategy>::revert()
	{
		Strategy::revert(storage_begin,usage_end);
	}


template <typename T, typename Strategy>
	void	BasicBuffer<T, Strategy>::resizePreserveContent(count_t new_len)
	{
		if ((count_t)(storage_end-storage_begin) == new_len)
			return;

		count_t preserve = std::min<count_t>(new_len,usage_end-storage_begin);

		try
		{
			//count_t len = usage_end-storage_begin;
			T	*new_field = allocate(new_len);

			Strategy::constructRangeFromFleetingData(new_field,new_field+preserve,storage_begin);
			destructAndFree(storage_begin,usage_end);

			storage_begin = new_field;
			storage_end = storage_begin+new_len;
			usage_end = storage_begin+preserve;
			#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
				fillLevel = preserve;
				CHK_FILLSTATE
			#endif

		}
		catch (std::bad_alloc& exception)
		{
			storage_begin = usage_end = storage_end = NULL;
			#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
				fillLevel = 0;
				CHK_FILLSTATE
			#endif
			throw;
		}
	}


template <typename T, typename Strategy>
	void	BasicBuffer<T, Strategy>::Fill(const T&pattern)
	{
		for (T*c = storage_begin; c < usage_end; c++)
			(*c) = pattern;

		/*for (register T*c = usage_end; c < storage_end; c++)
			new (c) T(pattern);*/
	}

template <typename T, typename Strategy>
	inline T*				BasicBuffer<T, Strategy>::appendRow(count_t elements, const T&init_pattern)
	{
		ensureHasSpace(elements);

		T*result = usage_end;
		usage_end += elements;
		Strategy::constructRange(result,usage_end,init_pattern);
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel+=elements;
			CHK_FILLSTATE
		#endif
		return result;
	}

	
template <typename T, typename Strategy>
	inline T*				BasicBuffer<T, Strategy>::appendRow(count_t elements)
	{
		ensureHasSpace(elements);

		T*result = usage_end;
		usage_end += elements;
		Strategy::constructRange(result,usage_end);
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel+=elements;
			CHK_FILLSTATE
		#endif
		return result;
	}
	
template <typename T, typename Strategy>
	template <typename T2>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::appendVA(count_t elements, ...)
	{
		if (!elements)
			return *this;

		ensureHasSpace(elements);
		va_list vl;
		va_start(vl,elements);
		for (index_t i=0; i<elements; i++)
		{
			new (usage_end++) T(va_arg(vl,T2));
		}
		va_end(vl);
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel+=elements;
			CHK_FILLSTATE
		#endif

		return *this;
	}


template <typename T, typename Strategy>
	inline T&				BasicBuffer<T, Strategy>::append()
	{
		ensureHasSpace();

		new (usage_end) T;
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel++;
		#endif
		return (*usage_end++);
	}


	
template <typename T, typename Strategy>
	inline T&				BasicBuffer<T, Strategy>::insert(index_t before)
	{
		if (usage_end == storage_end)
		{
			index_t	current_index = usage_end-storage_begin;
			count_t	len = (storage_end-storage_begin)*2;
			if (!len)
				len = 4;
			try
			{
				T	*new_field = allocateNotEmpty(len),//alloc<T>(len),
					*write = new_field,
					*read = storage_begin;

				count_t construct_before = std::min(before,current_index),	//elements to construct before the inserted element
						construct_after = current_index - construct_before;	//elements to construct after the inserted element
					//*barrier = vmin(storage_begin+before,usage_end);

				Strategy::constructRangeFromFleetingData(write, write+construct_before, read); write+= construct_before; read+= construct_before;
				T*result = write;
				new (write++) T;
				Strategy::constructRangeFromFleetingData(write, write+construct_after, read);

					//(*write++) = (*read++);
				//deloc(storage_begin);
				destructAndFree(storage_begin,usage_end);

				storage_begin = new_field;
				storage_end = storage_begin+len;
				usage_end = storage_begin+current_index+1;
				#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
					fillLevel++;
					CHK_FILLSTATE
				#endif

				return *result;
				//return std::min(storage_begin+before,usage_end-1);	//logically correct, but 'min' may fail for very large 'before' values (e.g. -1)
					//*(storage_begin+before);
			}
			catch (std::bad_alloc& exception)
			{
				storage_begin = storage_end = usage_end = NULL;
				#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
					fillLevel = 0;
					CHK_FILLSTATE
				#endif
				throw;
			}
		}
		else
		{
			new (usage_end) T;
			//T*el = std::min(storage_begin+before,usage_end);	//logically correct, but 'min' may fail for very large 'before' values (e.g. -1)
			T*el = storage_begin+std::min(before,count());
			for (T*c = usage_end; c > el; c--)
				Strategy::move(*(c-1),*c);
			usage_end++;
			#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
				fillLevel++;
				CHK_FILLSTATE
			#endif

			return *el;
		}
	}

template <typename T, typename Strategy>
	inline T*				BasicBuffer<T, Strategy>::insertRow(index_t before, count_t length)
	{
		if (usage_end+length > storage_end)
		{
			count_t	current_fill_level = usage_end-storage_begin;
			count_t	len = (storage_end-storage_begin);
			if (!len)
				len = 4;
			while (len < current_fill_level + length)
			{
				len <<= 1;
				if (len < current_fill_level)
					throw new std::bad_alloc();
			}

			try
			{
				T	*new_field = allocateNotEmpty(len),//alloc<T>(len),
					*write = new_field,
					*read = storage_begin;

				count_t construct_before = std::min(before,current_fill_level),	//elements to construct before the inserted elements
						construct_after = current_fill_level - construct_before;	//elements to construct after the inserted elements
					//*barrier = vmin(storage_begin+before,usage_end);

				Strategy::constructRangeFromFleetingData(write, write+construct_before, read); write+= construct_before; read+= construct_before;
				Strategy::constructRange(write,write+length);	 write+= length;
				Strategy::constructRangeFromFleetingData(write, write+construct_after, read);

				destructAndFree(storage_begin,usage_end);

				storage_begin = new_field;
				storage_end = storage_begin+len;
				usage_end = storage_begin+current_fill_level+length;
				#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
					fillLevel+=length;
					CHK_FILLSTATE
				#endif

				return std::min(storage_begin+before,usage_end-length);
					//*(storage_begin+before);
			}
			catch (std::bad_alloc& exception)
			{
				storage_begin = storage_end = usage_end = NULL;
				#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
					fillLevel = 0;
					CHK_FILLSTATE
				#endif
				throw;
			}
		}
		else
		{
			Strategy::constructRange(usage_end,usage_end + length);
			T*el = std::min(storage_begin+before,usage_end);
			for (T*c = usage_end+length-1; c >= el+length; c--)
				Strategy::move(*(c-length),*c);
			usage_end+=length;
			#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
				fillLevel+=length;
				CHK_FILLSTATE
			#endif

			return el;
		}

	}



template <typename T, typename Strategy>
template <typename T2>
	inline	T&				BasicBuffer<T, Strategy>::insert(index_t before, const T2&data)
	{
		if (usage_end == storage_end)
		{
			index_t	current_index = usage_end-storage_begin;
			count_t	len = (storage_end-storage_begin)*2;
			if (!len)
				len = 4;
			try
			{
				T	*new_field = allocateNotEmpty(len),//alloc<T>(len),
					*write = new_field,
					*read = storage_begin;

				count_t construct_before = std::min(before,current_index),	//elements to construct before the inserted element
						construct_after = current_index - construct_before;	//elements to construct after the inserted element
					//*barrier = vmin(storage_begin+before,usage_end);

				Strategy::constructRangeFromFleetingData(write, write+construct_before, read); write+= construct_before; read+= construct_before;
				new (write++) T(data);
				Strategy::constructRangeFromFleetingData(write, write+construct_after, read);

					//(*write++) = (*read++);
				//deloc(storage_begin);
				destructAndFree(storage_begin,usage_end);

				storage_begin = new_field;
				storage_end = storage_begin+len;
				usage_end = storage_begin+current_index+1;
				#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
					fillLevel++;
					CHK_FILLSTATE
				#endif


				return *std::min(storage_begin+before,usage_end-1);
					//*(storage_begin+before);
			}
			catch (std::bad_alloc& exception)
			{
				storage_begin = storage_end = usage_end = NULL;
				#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
					fillLevel = 0;
					CHK_FILLSTATE
				#endif
				throw;
			}
		}
		else
		{
			new (usage_end) T;
			T*el = std::min(storage_begin+before,usage_end);
			for (T*c = usage_end; c > el; c--)
				Strategy::move(*(c-1),*c);
			(*el) = data;
			//el->operator=(data);
			usage_end++;
			#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
				fillLevel++;
				CHK_FILLSTATE
			#endif
			return *el;
		}
	}

template <typename T, typename Strategy>
	inline void				BasicBuffer<T, Strategy>::eraseLast()
	{
		BUFFER_ASSERT_NOT_EMPTY();
		usage_end--;
		usage_end->~T();
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel--;
			CHK_FILLSTATE
		#endif
	}

template <typename T, typename Strategy>
	inline T				BasicBuffer<T, Strategy>::pop()
	{
		BUFFER_ASSERT_NOT_EMPTY();
		usage_end--;
		T data;
		Strategy::move(*usage_end,data);
		usage_end->~T();
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel--;
			CHK_FILLSTATE
		#endif
		return data;
	}

	/*	Not working ... laaaame

template <typename T>
	inline T				BasicBuffer<T, CopyStrategy>::pop()
	{
		usage_end--;
		T data(*usage_end);
		usage_end->~T();
		return data;
	}

template <typename T>
	inline T				BasicBuffer<T, PrimitiveStrategy>::pop()
	{
		usage_end--;
		return *usage_end;
	}
	*/

template <typename T, typename Strategy>
	inline	T&				BasicBuffer<T, Strategy>::last()
	{
		BUFFER_ASSERT_NOT_EMPTY();
		return *(usage_end-1);
	}
	
template <typename T, typename Strategy>
	inline	const T&		BasicBuffer<T, Strategy>::last()	const
	{
		BUFFER_ASSERT_NOT_EMPTY();
		return *(usage_end-1);
	}

template <typename T, typename Strategy>
	inline	T&				BasicBuffer<T, Strategy>::first()
	{
		BUFFER_ASSERT_NOT_EMPTY();
		return *storage_begin;
	}
	
template <typename T, typename Strategy>
	inline	const T&		BasicBuffer<T, Strategy>::first()	const
	{
		BUFFER_ASSERT_NOT_EMPTY();
		return *storage_begin;
	}

template <typename T, typename Strategy>
	inline BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::operator<<(const T&el)
	{
		ensureHasSpace();

		new (usage_end++) T(el);
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel++;
			CHK_FILLSTATE
		#endif

		return *this;
	}

	#if __BUFFER_RVALUE_REFERENCES__
		template <typename T, typename Strategy>
			inline BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::operator<<(T&&el)
			{
				ensureHasSpace();

				Strategy::constructSingleFromFleetingData(usage_end++, el);
				#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
					fillLevel++;
					CHK_FILLSTATE
				#endif
				return *this;
			}
	#endif

template <typename T, typename Strategy>
	inline T&		BasicBuffer<T, Strategy>::append(const T&el)
	{
		ensureHasSpace();

		new (usage_end++) T(el);
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel++;
			CHK_FILLSTATE
		#endif

		return *(usage_end-1);
	}

template <typename T, typename Strategy>
	inline	index_t			BasicBuffer<T,Strategy>::appendIfNotFound(const T&el)
	{
		index_t index = GetIndexOf(el);
		if (index == InvalidIndex)
		{
			index = count();
			append(el);
		}
		return index;
	}


template <typename T, typename Strategy>
	inline T&		BasicBuffer<T, Strategy>::moveAppend(T&el)
	{
		ensureHasSpace();

		Strategy::constructSingleFromFleetingData(usage_end++, el);
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel++;
			CHK_FILLSTATE
		#endif

		return *(usage_end-1);
	}


	#if __BUFFER_RVALUE_REFERENCES__
		template <typename T, typename Strategy>
			inline T&		BasicBuffer<T, Strategy>::append(T&&el)
			{
				ensureHasSpace();

				new (usage_end++) T(std::move(el));
				#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
					fillLevel++;
					CHK_FILLSTATE
				#endif

				return *(usage_end-1);
			}
	#endif


template <typename T, typename Strategy>
	inline index_t		BasicBuffer<T, Strategy>::push_back(const T&el)
	{
		ensureHasSpace();

		new (usage_end++) T(el);
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel++;
			CHK_FILLSTATE
		#endif

		return fillLevel()-1;
	}

template <typename T, typename Strategy> template <typename T2>
	inline BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::appendAddresses(T2*data, count_t elements)
	{
		ensureHasSpace(elements);
		for (index_t i = 0; i < elements; i++)
			new (usage_end++) T(data+i);
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel+=elements;
			CHK_FILLSTATE
		#endif

		return *this;
	}

template <typename T, typename Strategy> template <typename Strategy2>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::moveAppend(BasicBuffer<T,Strategy2>&buffer, bool clearSourceOnCompletion/*=true*/)
	{
		if (IsEmpty() && clearSourceOnCompletion)
		{
			swap(buffer);
			return *this;
		}

		moveAppend(buffer.pointer(),buffer.fillLevel());
		if (clearSourceOnCompletion)
			buffer.reset();
		return *this;
	}

template <typename T, typename Strategy>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::moveAppend(ArrayData<T>&array, bool clearSourceOnCompletion/*=true*/)
	{
		moveAppend(array.pointer(),array.length());
		if (clearSourceOnCompletion)
			array.free();
		return *this;
	}

template <typename T, typename Strategy>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::moveAppend(T*data, count_t elements)
	{
		ensureHasSpace(elements);
		Strategy::constructRangeFromFleetingData(usage_end,usage_end+elements,data);
		usage_end += elements;
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel+=elements;
			CHK_FILLSTATE
		#endif

		return *this;
	}

template <typename T, typename Strategy>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::MoveAppend(std::initializer_list<T> items)
	{
		return moveAppend(items.begin(),items.size());
	}

template <typename T, typename Strategy> template <typename T2>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::Append(std::initializer_list<T2> items)
	{
		return append(items.begin(),items.size());
	}


template <typename T, typename Strategy> template <typename T2>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::append(const T2*data, count_t elements)
	{
		ensureHasSpace(elements);
		for (index_t i = 0; i < elements; i++)
			new (usage_end++) T(data[i]);
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel+=elements;
			CHK_FILLSTATE
		#endif

		return *this;
	}

template <typename T, typename Strategy> template <typename T2, typename Strategy2>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::AppendSubList(const BasicBuffer<T2,Strategy2>&other, index_t offset, count_t maxElements)
	{
		if (offset >= other.Count())
			return *this;
		maxElements = std::min(maxElements,other.Count() - offset);
		if (!maxElements)
			return *this;
		return append(other.pointer()+offset,maxElements);
	}


template <typename T, typename Strategy> template <typename T2>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::append(const ArrayData<T2>&data)
	{
		return append(data.pointer(),data.count());
	}

template <typename T, typename Strategy> template <typename T2, typename Strategy2>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::append(const BasicBuffer<T2,Strategy2>&data)
	{
		return append(data.pointer(),data.count());
	}


template <typename T, typename Strategy> template <typename T2>
	BasicBuffer<T, Strategy>&		BasicBuffer<T, Strategy>::appendAddresses(ArrayData<T2>&data)
	{
		return appendAddresses(data.pointer(),data.count());
	}


template <typename T, typename Strategy>
	void		BasicBuffer<T, Strategy>::compact()
	{
		if (usage_end == storage_end)	// already compacted
			return;
		try
		{
			count_t len = usage_end-storage_begin;
			T	*new_field = allocate(len);

			Strategy::constructRangeFromFleetingData(new_field, new_field+len, storage_begin);
			destructAndFree(storage_begin,usage_end);

			storage_begin = new_field;
			storage_end = usage_end = storage_begin+len;
			CHK_FILLSTATE
		}
		catch (std::bad_alloc& exception)
		{
			usage_end = storage_begin = storage_end = NULL;
			#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
				fillLevel = 0;
				CHK_FILLSTATE
			#endif
			throw;
		}
	}




template <typename T, typename Strategy>
	inline void		BasicBuffer<T, Strategy>::reset()
	{
		Strategy::destructRange(storage_begin,usage_end);
		usage_end = storage_begin;
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel = 0;
			CHK_FILLSTATE
		#endif
	}

template <typename T, typename Strategy>
	inline void		BasicBuffer<T, Strategy>::clear()
	{
		reset();
	}

template <typename T, typename Strategy>
	inline	bool			BasicBuffer<T, Strategy>::empty()						const
	{
		return usage_end==storage_begin;
	}
template <typename T, typename Strategy>
	inline bool	BasicBuffer<T, Strategy>::IsEmpty()					const
	{
		return usage_end==storage_begin;
	}

template <typename T, typename Strategy>
	inline bool	BasicBuffer<T, Strategy>::IsNotEmpty()					const
	{
		return usage_end!=storage_begin;
	}

template <typename T, typename Strategy>
	inline count_t	BasicBuffer<T, Strategy>::fillLevel()					const
	{
		return usage_end-storage_begin;
	}

template <typename T, typename Strategy>
	inline	count_t		BasicBuffer<T, Strategy>::storageSize()				const
	{
		return storage_end-storage_begin;
	}


	
template <typename T, typename Strategy>
	inline count_t	BasicBuffer<T, Strategy>::length()					const
	{
		return fillLevel();
	}
	
template <typename T, typename Strategy>
	inline count_t	BasicBuffer<T, Strategy>::size()						const
	{
		return fillLevel();
	}
	
template <typename T, typename Strategy>
	inline count_t	BasicBuffer<T, Strategy>::count()						const
	{
		return fillLevel();
	}

template <typename T, typename Strategy>
	inline count_t	BasicBuffer<T, Strategy>::Count()						const
	{
		return fillLevel();
	}

template <typename T, typename Strategy>
	inline T&			BasicBuffer<T, Strategy>::operator[](index_t index)
	{
		BUFFER_CHECK_RANGE(index);
		return storage_begin[index];
	}

template <typename T, typename Strategy>
	inline const T&	BasicBuffer<T, Strategy>::operator[](index_t index)	const
	{
		BUFFER_CHECK_RANGE(index);
		return storage_begin[index];
	}

template <typename T, typename Strategy>
	inline bool	BasicBuffer<T, Strategy>::Owns(const T*element)	const
	{
		return element >= storage_begin && element < usage_end;
	}

template <typename T, typename Strategy>
	inline T&			BasicBuffer<T, Strategy>::at(index_t index)
	{
		BUFFER_CHECK_RANGE(index);
		return storage_begin[index];
	}

template <typename T, typename Strategy>
	inline const T&	BasicBuffer<T, Strategy>::at(index_t index)	const
	{
		BUFFER_CHECK_RANGE(index);
		return storage_begin[index];
	}

	template <typename T, typename Strategy>
	inline T&			BasicBuffer<T, Strategy>::fromEnd(index_t index)
	{
		BUFFER_CHECK_RANGE(index);
		return *(usage_end-index-1);
	}

	template <typename T, typename Strategy>
	inline const T&	BasicBuffer<T, Strategy>::fromEnd(index_t index)	const
	{
		BUFFER_CHECK_RANGE(index);
		return *(usage_end-index-1);
	}

template <typename T, typename Strategy>
	inline T*				BasicBuffer<T, Strategy>::pointer()
	{
		return storage_begin;
	}

template <typename T, typename Strategy>
	inline const T*		BasicBuffer<T, Strategy>::pointer()			const
	{
		return storage_begin;
	}

template <typename T, typename Strategy>
	template <typename IndexType>
		inline T*				BasicBuffer<T, Strategy>::operator+(IndexType delta)
		{
			BUFFER_CHECK_RANGE((index_t)delta);
			return storage_begin+delta;
		}

template <typename T, typename Strategy>
	template <typename IndexType>
		inline const T*		BasicBuffer<T, Strategy>::operator+(IndexType delta)		const
		{
			BUFFER_CHECK_RANGE((index_t)delta);
			return storage_begin+delta;
		}

template <typename T, typename Strategy>
	bool			BasicBuffer<T, Strategy>::Truncate(count_t fillLevel)
	{
		count_t cfill = usage_end-storage_begin;
		if (fillLevel < cfill)
		{
			Strategy::destructRange(storage_begin+fillLevel,usage_end);
			usage_end = storage_begin+fillLevel;
			#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
				this->fillLevel = fillLevel;
				CHK_FILLSTATE
			#endif

			return true;
		}
		return false;
	}

template <typename T, typename Strategy>
	void			BasicBuffer<T, Strategy>::adoptData(BasicBuffer<T,Strategy>&other)
	{
		if (&other == this)
			return;
		destructAndFree(storage_begin,usage_end);

		storage_begin = other.storage_begin;
		usage_end = other.usage_end;
		storage_end = other.storage_end;
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel = other.fillLevel;
			other.fillLevel = 0;
			CHK_FILLSTATE
		#endif

		other.storage_begin = other.usage_end = other.storage_end = NULL;
	}

template <typename T, typename Strategy>
	template <typename S2>
		void			BasicBuffer<T, Strategy>::swap(BasicBuffer<T,S2>&other)
		{
			swp(storage_begin,other.storage_begin);
			swp(usage_end,other.usage_end);
			swp(storage_end,other.storage_end);
			#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
				swp(fillLevel,other.fillLevel);
				CHK_FILLSTATE
			#endif
		}

template <typename T, typename Strategy>
	void			BasicBuffer<T, Strategy>::copyToArray(ArrayData<T>&target)	const
	{
		target.SetSize(usage_end-storage_begin);
		const T*from = storage_begin;
		T*to = target.pointer();
		Strategy::template copyRange<const T,T>(from,usage_end,to);
		/*while (from != usage_end)
			(*to++) = (*from++);
		ASSERT_CONCLUSION(target,to);*/
	}

template <typename T, typename Strategy>
	Array<T,Strategy>			BasicBuffer<T, Strategy>::copyToArray()	const
	{
		return Array<T,Strategy>(storage_begin,usage_end-storage_begin);
	}


template <typename T, typename Strategy>
	void			BasicBuffer<T, Strategy>::moveToArray(ArrayData<T>&target, bool reset_buffer)
	{
		target.SetSize(usage_end-storage_begin);
		T*from = storage_begin;
		T*to = target.pointer();
		Strategy::moveRange(from,usage_end,to);
		/*while (from != usage_end)
			Strategy::move(*from++,*to++);
		ASSERT_CONCLUSION(target,to);*/
		if (reset_buffer)
			reset();
	}

template <typename T, typename Strategy>
	inline bool				BasicBuffer<T, Strategy>::operator==(const BasicBuffer<T,Strategy>&other) const
	{
		count_t len = usage_end - storage_begin;
		if (len != other.usage_end - other.storage_begin)
			return false;
		for (index_t i = 0; i < len; i++)
			if (storage_begin[i] != other.storage_begin[i])
				return false;
		return true;
	}
template <typename T, typename Strategy>
	inline bool				BasicBuffer<T, Strategy>::operator!=(const BasicBuffer<T,Strategy>&other) const
	{
		return !operator==(other);
	}

	
template <typename T, typename Strategy>
	template <typename T2>
		inline	bool		BasicBuffer<T, Strategy>::contains(const T2&element)	const
		{
			const T*at = storage_begin;
			while (at != usage_end)
			{
				if (*at == element)
					return true;
				at++;
			}
			return false;
		}

template <typename T, typename Strategy>
	template <typename T2>
		inline	index_t		BasicBuffer<T, Strategy>::GetIndexOf(const T2&element)	const
		{
			const T*at = storage_begin;
			while (at != usage_end)
			{
				if (*at == element)
					return at-storage_begin;
				at++;
			}
			return InvalidIndex;
		}

template <typename T, typename Strategy>
	template <typename T2>
			bool			BasicBuffer<T, Strategy>::findAndErase(const T2&element)
			{
				index_t index = GetIndexOf(element);
				if (index == InvalidIndex)
					return false;
				erase(index);
				return true;
			}

template <typename T, typename Strategy>
	inline	typename BasicBuffer<T,Strategy>::iterator		BasicBuffer<T, Strategy>::erase(iterator it)
	{
		index_t index = it - storage_begin;
		if (index >= (count_t)(usage_end-storage_begin))
			return end();
		erase(index);
		return storage_begin + index;
	}


template <typename T, typename Strategy>
	inline	void			BasicBuffer<T, Strategy>::erase(index_t index)
	{
		if (index >= (count_t)(usage_end-storage_begin))
			return;
		T*c = storage_begin+index;
		while (c+1 != usage_end)
		{
			Strategy::move(*(c+1),*c);
			c++;
		}
		usage_end--;
		usage_end->~T();
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel--;
			CHK_FILLSTATE
		#endif

	}
	

template <typename T, typename Strategy>
	inline	void			BasicBuffer<T, Strategy>::erase(index_t index, index_t elements)
	{
		if (index >= (count_t)(usage_end-storage_begin) || !elements)
			return;
		if (index+elements > (count_t)(usage_end-storage_begin))
			elements = (count_t)(usage_end-storage_begin) - index;

		T*c = storage_begin+index;
		while (c+elements != usage_end)
		{
			Strategy::move(*(c+elements),*c);
			c++;
		}

		Strategy::destructRange(c,usage_end);
		usage_end-=elements;
		#if defined(_DEBUG) && __BUFFER_DBG_fillLevel__
			fillLevel-=elements;
			CHK_FILLSTATE
		#endif

	}



#endif
