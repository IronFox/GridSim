#ifndef production_pipeTplH
#define production_pipeTplH



/******************************************************************

E:\include\list\production_pipe.tpl.h

******************************************************************/

template <typename T,typename Strategy>
	inline void WorkPipe<T,Strategy>::SignalRead()
	{
		DBG_ASSERT__(!locked);
		//mutex.lock();
		lock.lock();
		readAt = 0;
		locked = true;
	}

template <typename T,typename Strategy>
	inline bool WorkPipe<T,Strategy>::operator>>(T&out)
	{
		DBG_ASSERT__(locked);
		if (readAt >= Super::count())
			return false;
		Strategy::move(Super::at(readAt++), out);
		return true;
	}

template <typename T,typename Strategy>
	inline void  WorkPipe<T,Strategy>::ExitRead()
	{
		DBG_ASSERT__(locked);
		Super::clear();
		locked = false;
		lock.unlock();
	}
#if __BUFFER_RVALUE_REFERENCES__
template <typename T,typename Strategy>
	inline WorkPipe<T,Strategy>&    WorkPipe<T,Strategy>::operator<<(T&&element)
	{
		lock.lock();
			Super::Append(std::forward<T&&>(element));
			contentSignal.signal();
		lock.unlock();
		return *this;

	}
#endif

template <typename T,typename Strategy>
	inline WorkPipe<T,Strategy>&    WorkPipe<T,Strategy>::operator<<(const T&pntr)
	{
		lock.lock();
			Super::Append(pntr);
			contentSignal.signal();
		lock.unlock();
		return *this;
	}
template <typename T,typename Strategy>
	inline WorkPipe<T,Strategy>&    WorkPipe<T,Strategy>::MoveAppend(T&pntr)
	{
		lock.lock();
			Super::MoveAppend(pntr);
			contentSignal.signal();
		lock.unlock();
		return *this;
	}

template <typename T,typename Strategy>
	inline	WorkPipe<T,Strategy>&    WorkPipe<T,Strategy>::MoveAppend(BasicBuffer<T>&buffer)
	{
		lock.lock();
			Super::MoveAppend(buffer);
			contentSignal.signal();
		lock.unlock();
		return *this;
	}



template <typename T,typename Strategy>
	inline void WorkPipe<T,Strategy>::clear()
	{
		lock.lock();
			Super::clear();
		lock.unlock();
	}


#endif
