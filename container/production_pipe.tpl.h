#ifndef production_pipeTplH
#define production_pipeTplH



/******************************************************************

E:\include\list\production_pipe.tpl.h

******************************************************************/

template <class Type> inline void ProductionPipe<Type>::SignalRead()
{
    mutex.lock();
	readAt = 0;
}

template <class Type> inline bool ProductionPipe<Type>::operator>>(shared_ptr<Type>&out)
{
	if (readAt >= Super::count())
		return false;
    out = Super::at(readAt++);
    return true;
}

template <class Type> inline void                ProductionPipe<Type>::ExitRead()
{
    Super::clear();
    mutex.release();
}

template <class Type> inline ProductionPipe<Type>&    ProductionPipe<Type>::operator<<(const shared_ptr<Type>&pntr)
{
    mutex.lock();
    Super::append(pntr);
    mutex.release();
    return *this;
}


template <class Type> inline void ProductionPipe<Type>::clear()
{
    mutex.lock();
    Super::clear();
    mutex.release();
}


#endif
