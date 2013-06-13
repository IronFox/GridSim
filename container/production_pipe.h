#ifndef production_pipeH
#define production_pipeH

#include "buffer.h"
#include "../general/thread_system.h"


/******************************************************************

E:\include\list\production_pipe.h

******************************************************************/


/**
	@brief Pointer queue for scenarios where many threads write and one reads
	
*/
template <class T>
	class ProductionPipe:protected Buffer<shared_ptr<T> >
	{
	protected:
	        Mutex						mutex;
			index_t						readAt;
	public:
			typedef Buffer<shared_ptr<T> >	Super;

	        inline  void                SignalRead();		//!< Signals that the local object mutex should be locked and the read cursor reset for subsequent read access
	        inline  bool                operator>>(shared_ptr<T>&);	//!< Retrieves the object currently apointed by the read cursor. Automatically advances the read cursor 	@return true if the object could be retrieves, false if the read cursor has reached the end of the queue
	        inline  void                ExitRead();			//!< Signals that the local list should be flushed and the local object mutex released
	        inline  ProductionPipe&		operator<<(const shared_ptr<T>&);		//!< Inserts an element to the end of the queue. This operation is mutex protected
	        inline  void                clear();			//!< Erases each object and flushs the queue. This operation is mutex protected
			
			//the following operations are provided for convenience. They are _not_ mutex protected and (except for count() maybe) should be invoked only from inside a signalRead()/exitRead() block.
			Super::count;
			Super::isEmpty;
			Super::isNotEmpty;
			Super::operator[];
			
			typedef T	Type;
			//ReferenceVector<T>::Type;
	};



#include "production_pipe.tpl.h"

#endif
