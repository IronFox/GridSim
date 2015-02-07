#ifndef queueTplH
#define queueTplH

/******************************************************************

Collection of different queues.

******************************************************************/


template <class Entry, class Element, class Strategy>
	QueueIterator<Entry,Element,Strategy>::QueueIterator(Element*begin,Element*end,Element*c):field_begin(begin),field_end(end),current(c)
	{}
	
template <class Entry, class Element, class Strategy>
	QueueIterator<Entry,Element,Strategy>&	QueueIterator<Entry,Element,Strategy>::operator++()
	{
		current++;
		if (current == field_end)
			current = field_begin;
		return *this;
	}
	
template <class Entry, class Element, class Strategy>
	QueueIterator<Entry,Element,Strategy>	QueueIterator<Entry,Element,Strategy>::operator++(int)
	{
		It rs(*this);
		operator++();
		return rs;
	}

template <class Entry, class Element, class Strategy>
	QueueIterator<Entry,Element,Strategy>	QueueIterator<Entry,Element,Strategy>::operator+(int delta)	const
	{
		It rs(*this);
		rs.current += delta;
		while (rs.current >= field_end)
			rs.current -= (field_end-field_begin);
		while (rs.current < field_begin)
			rs.current += (field_end-field_begin);
		return rs;
	}
	
template <class Entry, class Element, class Strategy>
	QueueIterator<Entry,Element,Strategy>&	QueueIterator<Entry,Element,Strategy>::operator--()
	{
		current--;
		if (current < field_begin)
			current = field_end-1;
		return *this;
	}

template <class Entry, class Element, class Strategy>
	QueueIterator<Entry,Element,Strategy>	QueueIterator<Entry,Element,Strategy>::operator--(int)
	{
		It rs(*this);
		operator--();
		return rs;
	}

template <class Entry, class Element, class Strategy>
	QueueIterator<Entry,Element,Strategy>	QueueIterator<Entry,Element,Strategy>::operator-(int delta)	const
	{
		It rs(*this);
		rs.current -= delta;
		while (rs.current >= field_end)
			rs.current -= (field_end-field_begin);
		while (rs.current < field_begin)
			rs.current += (field_end-field_begin);
		return rs;
	}
		
	
template <class Entry, class Element, class Strategy>
	size_t		QueueIterator<Entry,Element,Strategy>::index()	const
	{
		return current-field_begin;
	}
	


template <class Entry,class Strategy>
	Queue<Entry,Strategy>::Queue(size_t size):Array(size)
	{
		section_begin = section_end = Array::pointer();
		field_end = section_begin + Array::length();
	}

template <class Entry,class Strategy>
	void Queue<Entry,Strategy>::operator=(const Queue<Entry,Strategy>&other)
	{
		Array::setSize(other.Array::count());
		section_begin = section_end = Array::pointer();
		field_end = section_begin + Array::length();
		for (index_t i = 0; i < other.count(); i++)
			(*section_end++).Copy(other[i]);
	}

template <class Entry,class Strategy>
	void Queue<Entry,Strategy>::swap(Queue<Entry,Strategy>&other)
	{
		Array::swap(other);
		std::swap(section_begin,other.section_begin);
		std::swap(section_end,other.section_end);
		std::swap(field_end,other.field_end);
	}
template <class Entry,class Strategy>
	void Queue<Entry,Strategy>::adoptData(Queue<Entry,Strategy>&other)
	{
		Array::adoptData(other);
		section_begin = other.section_begin;
		section_end = other.section_end;
		field_end = other.field_end;

		other.field_end = other.section_begin = other.section_end = NULL;
	}



template <class Entry,class Strategy>
	typename Queue<Entry,Strategy>::iterator	Queue<Entry,Strategy>::begin()
	{
		return iterator(Array::data,field_end,section_begin);
	}

template <class Entry,class Strategy>
	typename Queue<Entry,Strategy>::iterator	Queue<Entry,Strategy>::end()
	{
		return iterator(Array::data,field_end,section_end);
	}

template <class Entry,class Strategy>
	typename Queue<Entry,Strategy>::const_iterator	Queue<Entry,Strategy>::begin()	const
	{
		return const_iterator(Array::data,field_end,section_begin);
	}

template <class Entry,class Strategy>
	typename Queue<Entry,Strategy>::const_iterator	Queue<Entry,Strategy>::end()	const
	{
		return const_iterator(Array::data,field_end,section_end);
	}

template <class Entry,class Strategy>
	bool			Queue<Entry,Strategy>::Contains(const Entry&entry)	const
	{
		foreach (*this,it)
			if (*it == entry)
				return true;
		return false;
	}


template <class Entry,class Strategy>
	count_t	Queue<Entry,Strategy>::Pop(Entry*out_field, count_t count)
	{
		count_t written = 0;
		while (section_begin != section_end && count > 0)
		{
			Strategy::move(section_begin->Cast(),*out_field);
			section_begin->Destruct();
			section_begin++;
			out_field++;
			if (section_begin >= field_end)
				section_begin = Array::pointer();
			count--;
			written++;
		}
		return written;
	}

template <class Entry,class Strategy>
	bool	Queue<Entry,Strategy>::Pop(Entry&out)
	{
		if (section_begin == section_end)
			return false;
		Strategy::move(section_begin->Cast(),out);
		section_begin->Destruct();
		section_begin++;
		if (section_begin >= field_end)
			section_begin = Array::pointer();
		return true;
	}

template <class Entry,class Strategy>
	Entry	Queue<Entry,Strategy>::Pop()
	{
		if (section_begin == section_end)
			return Entry();
		Entry out;
		Strategy::move(section_begin->Cast(),out);
		section_begin->Destruct();
		section_begin++;
		if (section_begin >= field_end)
			section_begin = Array::pointer();
		return out;
	}

template <class Entry,class Strategy>
	void	Queue<Entry,Strategy>::EraseFront()
	{
		if (section_begin == section_end)
			return;
		section_begin->Destruct();
		section_begin++;
		if (section_begin >= field_end)
			section_begin = Array::pointer();
	}

template <class Entry,class Strategy>
	void	Queue<Entry,Strategy>::EraseBack()
	{
		if (section_begin == section_end)
			return;
		if (section_end == Array::pointer())
			section_end = field_end-1;
		else
			section_end--;
		section_end->Destruct();
	}

template <class Entry, class Strategy>
	void	Queue<Entry,Strategy>::increaseSize(count_t new_size)
	{
		size_t old_usage = length();
		Array	new_field(new_size);
		Element*out = new_field.pointer();
		while (section_begin != section_end)
		{
			out->adoptData(section_begin);
			out++;
			section_begin++;
			//Strategy::move(*section_begin++,*out++);
			if (section_begin >= field_end)
				section_begin = Array::pointer();
		}
		

		Array::adoptData(new_field);
		section_begin = Array::pointer();
		section_end = section_begin+old_usage;
		field_end = section_begin+Array::length();
	}

template <class Entry, class Strategy>
	void	Queue<Entry,Strategy>::Push(const ArrayData<Entry>&entries)
	{
		Push(entries.pointer(),entries.count());
	}

template <class Entry, class Strategy>
	void	Queue<Entry,Strategy>::Push(const Entry*data, count_t count)
	{
		if (!count)
			return;
		count_t target = Array::length(),
				need = length()+count;
				
		while (target < need)
			target <<= 1;
		if (target != Array::length())
		{
			increaseSize(target);
			for (index_t i = 0; i < count; i++)
			{
				section_end->Construct(data[i]);
				//Strategy::move(data[i],section_end->Cast());
				section_end++;
			}
			//::Strategy::Adopt::copyRange(data,data+count,section_end);	//easy case
			//section_end += count;
		}
		else
		{
			while (count--)
			{
				section_end->Construct(*data);
				data ++;
				section_end++;
				//(*section_end++) = (*data++);
				if (section_end >= field_end)
					section_end = Array::pointer();
			}
		}
	}


template <class Entry, class Strategy>
	void	Queue<Entry,Strategy>::Push(const Entry&data)
	{
		Push() = data;
	}

template <class Entry, class Strategy>
	Entry&	Queue<Entry,Strategy>::Push()
	{
		section_end++;
		if (section_end >= field_end)
			section_end = Array::pointer();
		if (section_end == section_begin)
		{
			size_t old_len = Array::length();
			Array	new_field(Array::length()*2);
			Element*out = new_field.pointer();
			do
			{
				out->adoptData(*section_begin);
				section_begin++;
				out++;
				if (section_begin >= field_end)
					section_begin = Array::pointer();
			}
			while (section_begin != section_end);
			Array::adoptData(new_field);
			section_begin = Array::pointer();
			section_end = section_begin+old_len;
			field_end = section_begin+Array::length();
		}
		if (section_end > Array::data)
			return (section_end-1)->ConstructAndCast();
		return Array::data[Array::elements-1].ConstructAndCast();
	}

//
//template <class Entry, class Strategy>
//	void	Queue<Entry,Strategy>::push(const ArrayData<Entry>&entries)
//	{
//		push(entries.pointer(),entries.count());
//	}
//
//template <class Entry, class Strategy>
//	void	Queue<Entry,Strategy>::push(const Entry*data, count_t count)
//	{
//		if (!count)
//			return;
//		count_t target = Array::length(),
//				need = length()+count;
//				
//		while (target < need)
//			target <<= 1;
//		if (target != Array::length())
//		{
//			increaseSize(target);
//			Strategy::copyRange(data,data+count,section_end);	//easy case
//			section_end += count;
//		}
//		else
//		{
//			while (count--)
//			{
//				(*section_end++) = (*data++);
//				if (section_end >= field_end)
//					section_end = Array::pointer();
//			}
//		}
//	}
//

template <class Entry, class Strategy>
	void	Queue<Entry,Strategy>::PushFront(const Entry&data)
	{
		PushFront() = data;
	}

template <class Entry, class Strategy>
	Entry&	Queue<Entry,Strategy>::PushFront()
	{
		if (section_begin > Array::pointer())
			section_begin --;
		else
			section_begin = Array::IsNotEmpty() ? field_end-1 : field_end;
		if (section_end == section_begin)
		{
			size_t old_len = Array::length();
			Array	new_field(Array::length()*2);
			Element*out = new_field.pointer();
			do
			{
				out->adoptData(*section_begin);
				section_begin++;
				out++;
				//Strategy::move(*section_begin++,*out++);
				if (section_begin >= field_end)
					section_begin = Array::pointer();
			}
			while (section_begin != section_end);
			Array::adoptData(new_field);
			section_begin = Array::pointer();
			section_end = section_begin+old_len;
			field_end = section_begin+Array::length();
		}
		return section_begin->ConstructAndCast();
	}



template <class Entry,class Strategy>
	void	Queue<Entry,Strategy>::clear()
	{
		while (section_begin != section_end)
		{
			section_begin->Destruct();
			section_begin++;
			if (section_begin >= field_end)
				section_begin = Array::pointer();
		}
		//section_end = section_begin;
	}
	

template <class Entry,class Strategy>
	bool	Queue<Entry,Strategy>::IsEmpty()					const
	{
		return section_begin == section_end;
	}
template <class Entry,class Strategy>
	bool	Queue<Entry,Strategy>::IsNotEmpty()					const
	{
		return section_begin != section_end;
	}

template <class Entry,class Strategy>	
	size_t	Queue<Entry,Strategy>::length()					const
	{
		return section_end >= section_begin?section_end-section_begin:section_end+Array::length()-section_begin;
	}

template <class Entry,class Strategy>	
	bool	Queue<Entry,Strategy>::operator==(const Self&other) const
	{
		auto i0 = other.begin();
		auto i1 = begin();
		while (i0 != other.end() && i1 != end())
		{
			if (*i0 != *i1)
				return false;
			++i0; ++i1;
		}
		return i0 == other.end() && i1 == end();
	}
template <class Entry,class Strategy>	
	bool	Queue<Entry,Strategy>::operator!=(const Self&other) const
	{
		auto i0 = other.begin();
		auto i1 = begin();
		while (i0 != other.end() && i1 != end())
		{
			if (*i0 == *i1)
				return false;
			++i0; ++i1;
		}
		return i0 != other.end() || i1 != end();
	}

	
template <class Entry,class Strategy>
	bool		Queue<Entry,Strategy>::operator>>(Entry&entry)
	{
		return Pop(entry);
	}
	
template <class Entry,class Strategy>
	Queue<Entry,Strategy>&		Queue<Entry,Strategy>::operator<<(const Entry&entry)
	{
		Push(entry);
		return *this;
	}
	
template <class Entry,class Strategy>
	Entry&			Queue<Entry,Strategy>::GetOldest()
	{
		return section_begin->Cast();
	}
	
template <class Entry,class Strategy>
	const Entry&	Queue<Entry,Strategy>::GetOldest()						const
	{
		return section_begin->Cast();
	}

template <class Entry,class Strategy>
	Entry&			Queue<Entry,Strategy>::Peek()
	{
		return section_begin->Cast();
	}

template <class Entry,class Strategy>
	const Entry&	Queue<Entry,Strategy>::Peek()						const
	{
		return section_begin->Cast();
	}

template <class Entry, class Strategy>
	void		Queue<Entry,Strategy>::CopyToArray(ArrayData<Entry>&out) const
	{
		out.SetSize(CountEntries());
		for (index_t i = 0; i < out.Count(); i++)
			out[i] = operator[](i);
	}



template <class Entry,class Strategy>
	Entry&			Queue<Entry,Strategy>::GetNewest()
	{
		return (section_end!=Array::data?(section_end-1):field_end-1)->Cast();
	}
	
template <class Entry,class Strategy>
	const Entry&	Queue<Entry,Strategy>::GetNewest()						const
	{
		return (section_end!=Array::data?(section_end-1):field_end-1)->Cast();
	}
	
template <class Entry,class Strategy>
	Entry&			Queue<Entry,Strategy>::operator[](size_t index)
	{
		if (index >= index_t(field_end-section_begin))
		{
			return Array::at(index- (field_end-section_begin)).Cast();
		}
		return section_begin[index].Cast();
	}
	
template <class Entry,class Strategy>
	const Entry&			Queue<Entry,Strategy>::operator[](size_t index)	const
	{
		if (index >= index_t(field_end-section_begin))
		{
			return Array::at(index- (field_end-section_begin)).Cast();
		}
		return section_begin[index].Cast();
	}


template <class Entry, class Priority, class Strategy>
	PriorityQueue<Entry, Priority, Strategy>::PriorityQueue(size_t size):entry_field(size),priority_field(size),section_begin(0),section_end(0)
	{}
			
template <class Entry, class Priority, class Strategy>
	typename PriorityQueue<Entry,Priority,Strategy>::iterator	PriorityQueue<Entry, Priority, Strategy>::begin()
	{
		return iterator(entry_field.pointer(),entry_field.pointer()+entry_field.length(),entry_field+section_begin);
	}
		
template <class Entry, class Priority, class Strategy>
	typename PriorityQueue<Entry,Priority,Strategy>::iterator	PriorityQueue<Entry, Priority, Strategy>::end()
	{
		return iterator(entry_field.pointer(),entry_field.pointer()+entry_field.length(),entry_field+section_end);
	}

template <class Entry, class Priority, class Strategy>
	bool	PriorityQueue<Entry, Priority, Strategy>::pop(Entry&out, Priority&pout)
	{
		if (section_begin == section_end)
			return false;
		Strategy::move(entry_field[section_begin].Cast(),out);
		entry_field[section_begin].Destruct();
		PriorityArray::AppliedStrategy::move(priority_field[section_begin],pout);
		section_begin++;
		if (section_begin >= entry_field.length())
			section_begin = 0;
		return true;
	}
	
template <class Entry, class Priority, class Strategy>
	bool	PriorityQueue<Entry, Priority, Strategy>::pop(Entry&out)
	{
		if (section_begin == section_end)
			return false;
		Strategy::move(entry_field[section_begin].Cast(),out);
		entry_field[section_begin].Destruct();
		section_begin++;
		if (section_begin >= entry_field.length())
			section_begin = 0;
		return true;
	}
	
template <class Entry, class Priority, class Strategy>
	Entry				PriorityQueue<Entry, Priority, Strategy>::Pop()
	{
		if (section_begin == section_end)
			return Entry();
		Entry rs = entry_field[section_begin].Cast();
		entry_field[section_begin].Destruct();
		section_begin++;
		if (section_begin >= entry_field.length())
			section_begin = 0;
		return rs;
	}

template <class Entry, class Priority, class Strategy>
	void				PriorityQueue<Entry, Priority, Strategy>::EraseFront()
	{
		if (section_begin == section_end)
			return;
		entry_field[section_begin].Destruct();
		section_begin++;
		if (section_begin >= entry_field.length())
			section_begin = 0;
		return rs;
	}



template <class Entry, class Priority, class Strategy>
	bool	PriorityQueue<Entry, Priority, Strategy>::PopLeast(Entry&out, Priority&pout)
	{
		if (section_begin == section_end)
			return false;
		index_t index = section_end?section_end-1:entry_field.count()-1;
		Strategy::move(entry_field[index].Cast(),out);
		PriorityArray::AppliedStrategy::move(priority_field[index],pout);
		entry_field[index].Destruct();
		section_end = index;
		return true;
	}
	
template <class Entry, class Priority, class Strategy>
	bool	PriorityQueue<Entry, Priority, Strategy>::PopLeast(Entry&out)
	{
		if (section_begin == section_end)
			return false;
		index_t index = section_end?section_end-1:entry_field.count()-1;
		Strategy::move(entry_field[index].Cast(),out);
		entry_field[index].Destruct();
		section_end = index;
		return true;
	}
	
template <class Entry, class Priority, class Strategy>
	Entry				PriorityQueue<Entry, Priority, Strategy>::PopLeast()
	{
		if (section_begin == section_end)
			return Entry();
		index_t index = section_end?section_end-1:entry_field.count()-1;
		Entry rs = entry_field[index].Cast();
		entry_field[index].Destruct();
		section_end = index;
		return rs;
	}

template <class Entry, class Priority, class Strategy>
	void				PriorityQueue<Entry, Priority, Strategy>::EraseLeast()
	{
		if (section_begin == section_end)
			return;
		index_t index = section_end?section_end-1:entry_field.count()-1;
		entry_field[index].Destruct();
		section_end = index;
	}

template <class Entry, class Priority, class Strategy>
	inline void			PriorityQueue<Entry, Priority, Strategy>::dec(size_t&offset)
	{
		if (offset)
			offset--;
		else
			offset = entry_field.length()-1;
	}

template <class Entry, class Priority, class Strategy>
	void			PriorityQueue<Entry, Priority, Strategy>::eraseAddr(index_t index)
	{
		if (index >= entry_field.length())
			return;
		if (section_begin < section_end)
		{
			if (index < section_begin || index >= section_end)
				return;
			section_end--;
			for (index_t i = index; i < section_end; i++)
			{
				Strategy::move(entry_field[i+1],entry_field[i]);
				PriorityArray::AppliedStrategy::move(priority_field[i+1],priority_field[i]);
			}
			return;
		}
		if (index < section_end)
		{
			section_end--;
			for (index_t i = index; i < section_end; i++)
			{
				Strategy::move(entry_field[i+1],entry_field[i]);
				PriorityArray::AppliedStrategy::move(priority_field[i+1],priority_field[i]);
			}
			return;
		}
		if (index < section_begin)
			return;
		if (section_end)
			section_end--;
		else
			section_end = entry_field.length()-1;
		
		for (index_t i = index; i != section_end;)
		{
			index_t next = (i+1)%entry_field.length();
			Strategy::move(entry_field[next],entry_field[i]);
			PriorityArray::AppliedStrategy::move(priority_field[next],priority_field[i]);
			i = next;
		}
	}


template <class Entry, class Priority, class Strategy>
	void			PriorityQueue<Entry, Priority, Strategy>::erase(const iterator&it)
	{
		eraseAddr(it.index());

	}

template <class Entry, class Priority, class Strategy>
	bool				PriorityQueue<Entry,Priority,Strategy>::find(const Priority&priority, iterator&it)
	{
		size_t len = entry_field.length();
		size_t end = section_end;
		if (end < section_begin)
			end+=entry_field.length();
		size_t half = (section_begin+end)/2;
		
		size_t	lower = section_begin;
		{
			size_t		num = end-section_begin,
						upper = end;
			while (lower< upper && upper <= end)
			{
		        size_t el = (lower+upper)/2;
				size_t real = el%len;
				
		        const Priority&p = priority_field[real];
		        if (p < priority)
		            upper = el;
		        else
					lower = el+1;
		    }
		}
		index_t real = lower%len;
		if (priority_field[real] == priority)
		{
			it = iterator(entry_field.pointer(),entry_field.pointer()+entry_field.length(),entry_field+real);
			return true;
		}
		return false;
	}

template <class Entry, class Priority, class Strategy>
	bool				PriorityQueue<Entry,Priority,Strategy>::find(const Entry&data, const Priority&priority, iterator&it)
	{
		if (!find(priority,it))
			return false;
		if (*it == data)
			return true;
		iterator origin = it;
		--it;
		while (it != begin()-1 && priorityOf(it) == priority)
		{
			if (*it == data)
				return true;
			--it;
		}
		it = origin;
		++it;
		while (it != end() && priorityOf(it) == priority)
		{
			if (*it == data)
				return true;
			++it;
		}
		return false;
	}

template <class Entry, class Priority, class Strategy>
	bool				PriorityQueue<Entry,Priority,Strategy>::alterPriority(const Entry&data, const Priority&old_priority, const Priority&new_priority)
	{
		iterator it;
		if (!find(data,old_priority,it))
			return false;
		erase(it);
		push(data,new_priority);
	}


template <class Entry, class Priority, class Strategy>
	void				PriorityQueue<Entry, Priority, Strategy>::Push(const Entry&data, const Priority&priority)
	{
		
		size_t len = entry_field.length();
		size_t end = section_end;
		if (end < section_begin)
			end+=entry_field.length();
		size_t half = (section_begin+end)/2;
		
		size_t	lower = section_begin;
		{
			size_t	num = end-section_begin,
						upper = end;
			while (lower< upper && upper <= end)
			{
		        size_t el = (lower+upper)/2;
				size_t real = el%len;
				
		        const Priority&p = priority_field[real];
		        if (p < priority)
		            upper = el;
		        else
					lower = el+1;
		    }
		}

		if (end-section_begin==len-1)
		{
			//cout << "queue has reached capacity ("<<len<<")"<<endl;
			size_t l = lower%len;
			EntryArray		new_entry_field(len<<1);
			PriorityArray	new_priority_field(len<<1);
			//cout << "new fields have length "<<new_entry_field.length()<<endl;
			//cout << "element is "<<l<<endl;
			Element*out = new_entry_field.pointer();
			Priority*pout = new_priority_field.pointer();
			
			while (section_begin != l)
			{
				//cout << "processing element "<<section_begin<<endl;
				out->adoptData(entry_field[section_begin]);
				out++;
				PriorityArray::AppliedStrategy::move(priority_field[section_begin],*pout++);
				section_begin++;
				section_begin%=len;
			}
			out->Construct(data);
			out++;
			//(*out++) = data;
			(*pout++) = priority;
			while (section_begin != section_end)
			{
				//cout << "processing element "<<section_begin<<endl;
				out->adoptData(entry_field[section_begin]);
				out++;
//				Strategy::move(entry_field[section_begin],*out++);
				PriorityArray::AppliedStrategy::move(priority_field[section_begin],*pout++);
				section_begin++;
				section_begin%=len;
			}
			
			entry_field.adoptData(new_entry_field);
			priority_field.adoptData(new_priority_field);
			section_begin = 0;
			section_end = len;
			//cout << "new section is "<<section_begin<<"..."<<section_end<<endl;
		}
		else
		{
			//cout << "simple insert job at "<<lower<<endl;
			if (end-lower < lower-section_begin)
			{
				for (size_t i = end; i > lower; i--)
				{
					entry_field[i%len].adoptData(entry_field[(i-1)%len]);
					//Strategy::move(entry_field[(i-1)%len],entry_field[i%len]);
					PriorityArray::AppliedStrategy::move(priority_field[(i-1)%len],priority_field[i%len]);
				}
				section_end++;
				section_end%=len;

				const index_t at = lower%len;
				entry_field[at].Construct(data);

				//entry_field[lower%len] = data;
				priority_field[lower%len] = priority;
			}
			else
			{
				//ShowMessage("section is "+String(section_begin)+", "+String(end));
				//ShowMessage("lower is "+String(lower));
				
				if (section_begin != lower%len)
				{
					//ShowMessage("copying first");
					if (section_begin)
					{
						entry_field[section_begin-1].adoptData(entry_field[section_begin]);
						//Strategy::move(entry_field[section_begin],entry_field[section_begin-1]);
						PriorityArray::AppliedStrategy::move(priority_field[section_begin],priority_field[section_begin-1]);
					}
					else
					{
						entry_field.last().adoptData(entry_field.first());
						//Strategy::move(entry_field.first(),entry_field.last());
						PriorityArray::AppliedStrategy::move(priority_field.first(),priority_field.last());
					}
				}
				for (size_t i = section_begin; i+1 < lower; i++)
				{
					entry_field[i%len].adoptData(entry_field[(i+1)%len]);
					//Strategy::move(entry_field[(i+1)%len],entry_field[i%len]);
					PriorityArray::AppliedStrategy::move(priority_field[(i+1)%len],priority_field[i%len]);
				}
				dec(section_begin);
				dec(lower);
				const index_t at = lower%len;
				entry_field[at].Construct(data);
				priority_field[at] = priority;
			}
			//cout << "section end now "<<section_end<<" (len="<<len<<")"<<endl;
		}
	}
	
template <class Entry, class Priority, class Strategy>	
	bool				PriorityQueue<Entry, Priority, Strategy>::checkIntegrity()
	{
		iterator b = begin();
		while (b != end())
		{
			iterator a = b++;
			if (b != end() && priorityOf(a)<priorityOf(b))
				return false;
		}
		return true;
	
	}
	
template <class Entry, class Priority, class Strategy>
	void	PriorityQueue<Entry, Priority, Strategy>::clear()
	{
		const count_t len = entry_field.length();
		while (section_begin != section_end)
		{
			entry_field[section_begin].Destruct();
			section_begin++;
			section_begin%=len;
		}

		//section_end = section_begin;
	}
	
template <class Entry, class Priority, class Strategy>
	bool				PriorityQueue<Entry, Priority, Strategy>::isEmpty()					const
	{
		return section_begin == section_end;
	}
template <class Entry, class Priority, class Strategy>
	bool				PriorityQueue<Entry, Priority, Strategy>::isNotEmpty()					const
	{
		return section_begin != section_end;
	}
	
	
template <class Entry, class Priority, class Strategy>
	size_t			PriorityQueue<Entry, Priority, Strategy>::length()					const
	{
		if (section_end >= section_begin)
			return section_end-section_begin;
		return section_end+entry_field.length()-section_begin;
	}
	
template <class Entry, class Priority, class Strategy>
	bool				PriorityQueue<Entry, Priority, Strategy>::operator>>(Entry&entry)
	{
		return pop(entry);
	}
	
	
template <class Entry, class Priority, class Strategy>
	Entry&				PriorityQueue<Entry, Priority, Strategy>::peek()	
	{
		return entry_field[section_begin];
	}

template <class Entry, class Priority, class Strategy>
	const Priority&				PriorityQueue<Entry, Priority, Strategy>::peekPriority()	const
	{
		return priority_field[section_begin];
	}

	
template <class Entry, class Priority, class Strategy>
	const Entry&		PriorityQueue<Entry, Priority, Strategy>::peek()						const
	{
		return entry_field[section_begin];
	}
	
template <class Entry, class Priority, class Strategy>
	Entry&				PriorityQueue<Entry, Priority, Strategy>::peekLeast()
	{
		return section_end?entry_field[section_end-1]:entry_field.last();
	}
	
template <class Entry, class Priority, class Strategy>
	const Entry&		PriorityQueue<Entry, Priority, Strategy>::peekLeast()						const
	{
		return section_end?entry_field[section_end-1]:entry_field.last();
	}

	
template <class Entry, class Priority, class Strategy>
	const Priority&		PriorityQueue<Entry, Priority, Strategy>::peekLeastPriority()						const
	{
		return section_end?priority_field[section_end-1]:priority_field.last();
	}
	
template <class Entry, class Priority, class Strategy>
	const Priority&			PriorityQueue<Entry, Priority, Strategy>::priorityOf(const iterator&it)	const
	{
		return priority_field[it.index()];
	
	
	}

/*

template <class C, class IndexType> const C&Queue<C,IndexType>::get()
{
    if (first == last)
        return NULL;
    element = loop[first];
    first++;
    return element;
}

template <class C, class IndexType> Queue<C,IndexType>::Queue():first(0),last(0)
{}

template <class C, class IndexType> const C&Queue<C,IndexType>::peek()  const
{
    if (first == last)
        return NULL;
    return loop[first];
}

template <class C, class IndexType> const C&Queue<C,IndexType>::peek(IndexType element)  const
{
    return loop[first+element];
}

template <class C, class IndexType> void Queue<C,IndexType>::insert(const C&item)
{
    if (last+1 == first)
        return;
    loop[last] = item;
    last++;
}

template <class C, class IndexType> void Queue<C,IndexType>::clear()
{
    first = last;
}

template <class C, class IndexType> IndexType Queue<C, IndexType>::count() const
{
    return last-first;
}

template <class C, class IndexType> bool Queue<C,IndexType>::full() const
{
    return last+1 == first;
}

template <class C, class IndexType> bool Queue<C,IndexType>::empty() const
{
    return last == first;
}





template <class C, class IndexType> C*CPQueue<C,IndexType>::get()
{
    if (first == last)
        return NULL;
    C*result = loop[first];
    first++;
    return result;
}

template <class C, class IndexType> CPQueue<C,IndexType>::CPQueue():first(0),last(0)
{}

template <class C, class IndexType> C*CPQueue<C,IndexType>::peek()
{
    if (first == last)
        return NULL;
    return loop[first];
}

template <class C, class IndexType> C*CPQueue<C,IndexType>::peek(IndexType element)
{
    return loop[first+element];
}

template <class C, class IndexType> void CPQueue<C,IndexType>::insert(C*item)
{
    if (last+1 == first)
    {
        DISCARD(item);
        return;
    }
    loop[last] = item;
    last++;
}

template <class C, class IndexType> void CPQueue<C,IndexType>::clear()
{
    while (first != last)
    {
        DISCARD(loop[first]);
        first++;
    }
}

template <class C, class IndexType> void CPQueue<C,IndexType>::flush()
{
    first = last;
}

template <class C, class IndexType> IndexType CPQueue<C, IndexType>::count() const
{
    return last-first;
}

template <class C, class IndexType> bool CPQueue<C,IndexType>::full() const
{
    return last+1 == first;
}

template <class C, class IndexType> bool CPQueue<C,IndexType>::empty() const
{
    return last == first;
}







template <class C> DynamicQueue<C>::DynamicQueue():first(NULL),last(NULL),volume(0)
{}

template <class C> DynamicQueue<C>::~DynamicQueue()
{
    clear();
}


template <class C> const C& DynamicQueue<C>::peek()                  const
{
    return first?first->element:element;
}

template <class C> const C& DynamicQueue<C>::get()
{
    if (!first)
        return element;
    element = first->element;
    SDynamicPath<C>*next = first->next;
    DISCARD(first);
    if (!next)
        last = NULL;
    first = next;
    volume--;
    return element;
}

template <class C> void DynamicQueue<C>::insert(const C&item)
{
    SDynamicPath<C>*append = SHIELDED(new SDynamicPath<C>());
    append->element = item;
    append->next = NULL;
    if (last)
        last->next = append;
    else
        first = append;
    last = append;
    volume++;
}

template <class C> void DynamicQueue<C>::clear()
{
    SDynamicPath<C>*path = first;
    while (path)
    {
        SDynamicPath<C>*next = path->next;
        DISCARD(path);
        path = next;
    }
    first = NULL;
    last = NULL;
    volume = 0;
}

template <class C> size_t DynamicQueue<C>::count()              const
{
    return volume;
}

template <class C> bool DynamicQueue<C>::empty()                 const
{
    return !volume;
}


template <class C> DynamicPQueue<C>::DynamicPQueue():first(NULL),last(NULL),volume(0)
{}

template <class C> DynamicPQueue<C>::~DynamicPQueue()
{
    clear();
}

template <class C> C*DynamicPQueue<C>::peek()
{
    return first?first->element:NULL;
}

template <class C> C*DynamicPQueue<C>::get()
{
    if (!first)
        return NULL;
    C*element = first->element;
    SDynamicPath<C*>*next = first->next;
    DISCARD(first);
    if (!next)
        last = NULL;
    first = next;
    volume--;
    return element;
}

template <class C> void DynamicPQueue<C>::insert(C*item)
{
    SDynamicPath<C*>*append = SHIELDED(new SDynamicPath<C*>());
    append->element = item;
    append->next = NULL;
    if (last)
        last->next = append;
    else
        first = append;
    last = append;
    volume++;
}

template <class C> void DynamicPQueue<C>::clear()
{
    SDynamicPath<C*>*path = first;
    while (path)
    {
        SDynamicPath<C*>*next = path->next;
        DISCARD(path->element);
        DISCARD(path);
        path = next;
    }
    first = NULL;
    last = NULL;
    volume = 0;
}

template <class C> void DynamicPQueue<C>::flush()
{
    SDynamicPath<C*>*path = first;
    while (path)
    {
        SDynamicPath<C*>*next = path->next;
        DISCARD(path);
        path = next;
    }
    first = NULL;
    last = NULL;
    volume = 0;
}


template <class C> size_t DynamicPQueue<C>::count()              const
{
    return volume;
}

template <class C> bool DynamicPQueue<C>::empty()                 const
{
    return !volume;
}

*/

#endif


