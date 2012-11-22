#ifndef production_pipeH
#define production_pipeH

#include "lvector.h"


/******************************************************************

E:\include\list\production_pipe.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


/**
	@brief Pointer queue for scenarios where many threads write and one reads
	
*/
template <class T>
	class ProductionPipe:protected List::ReferenceVector<T>
	{
	protected:
	        Mutex                      mutex;
	public:
			typedef ::List::ReferenceVector<T>	Super;

	        inline  void                signalRead();		//!< Signals that the local object mutex should be locked and the read cursor reset for subsequent read access
	        inline  bool                operator>>(T*&);	//!< Retrieves the object currently apointed by the read cursor. Automatically advances the read cursor 	@return true if the object could be retrieves, false if the read cursor has reached the end of the queue
	        inline  void                exitRead();			//!< Signals that the local list should be flushed and the local object mutex released
	        inline  ProductionPipe&		operator<<(T*);		//!< Inserts an element to the end of the queue. This operation is mutex protected
	        inline  void                flush();			//!< Flushes the queue without erasing the objects. This operation is mutex protected
	        inline  void                clear();			//!< Erases each object and flushs the queue. This operation is mutex protected
			
			//the following operations are provided for convenience. They are _not_ mutex protected and (except for count() maybe) should be invoked only from inside a signalRead()/exitRead() block.
			Super::count;
			Super::revert;
			Super::swap;
			Super::empty;
			Super::operator[];
			
			typedef T	Type;
			//ReferenceVector<T>::Type;
	};



#include "production_pipe.tpl.h"

#endif
