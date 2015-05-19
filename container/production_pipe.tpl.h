#ifndef production_pipeTplH
#define production_pipeTplH



/******************************************************************

E:\include\list\production_pipe.tpl.h

******************************************************************/

template <typename T,typename Strategy>
	inline void WorkPipe<T,Strategy>::SignalRead()
	{
		DBG_ASSERT__(!locked);
		mutex.lock();
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
		mutex.release();
	}

template <typename T,typename Strategy>
	inline WorkPipe<T,Strategy>&    WorkPipe<T,Strategy>::operator<<(const T&pntr)
	{
		mutex.lock();
			Super::Append(pntr);
			contentSignal.signal();
		mutex.release();
		return *this;
	}
template <typename T,typename Strategy>
	inline WorkPipe<T,Strategy>&    WorkPipe<T,Strategy>::MoveAppend(T&pntr)
	{
		mutex.lock();
			Super::MoveAppend(pntr);
			contentSignal.signal();
		mutex.release();
		return *this;
	}

template <typename T,typename Strategy>
	inline	WorkPipe<T,Strategy>&    WorkPipe<T,Strategy>::MoveAppend(BasicBuffer<T>&buffer)
	{
		mutex.lock();
			Super::MoveAppend(buffer);
			contentSignal.signal();
		mutex.release();
		return *this;
	}



template <typename T,typename Strategy>
	inline void WorkPipe<T,Strategy>::clear()
	{
		mutex.lock();
			Super::clear();
		mutex.release();
	}


#endif
