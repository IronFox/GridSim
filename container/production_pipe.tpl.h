#ifndef production_pipeTplH
#define production_pipeTplH



/******************************************************************

E:\include\list\production_pipe.tpl.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

template <class Type> inline void ProductionPipe<Type>::signalRead()
{
    mutex.lock();
    Super::reset();
}

template <class Type> inline bool ProductionPipe<Type>::operator>>(Type*&out)
{
    out = Super::each();
    return out!=NULL;
}

template <class Type> inline void                ProductionPipe<Type>::exitRead()
{
    Super::flush();
    mutex.release();
}

template <class Type> inline ProductionPipe<Type>&    ProductionPipe<Type>::operator<<(Type*pntr)
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

template <class Type> inline void ProductionPipe<Type>::flush()
{
    mutex.lock();
    Super::flush();
    mutex.release();
}



#endif
