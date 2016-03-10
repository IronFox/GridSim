#ifndef production_pipeH
#define production_pipeH

#include "buffer.h"
#include "../general/thread_system.h"


/******************************************************************

E:\include\list\production_pipe.h

******************************************************************/



template <typename T, typename Strategy=typename StrategySelector<T>::Default>
	class WorkPipe : protected Buffer<T,0,Strategy>
	{
		typedef Buffer<T,0,Strategy>Super;
		typedef WorkPipe<T,Strategy>Self;
		volatile bool				locked;
	    //Mutex						mutex;
		SpinLock					lock;
		index_t						readAt;
		Signal						contentSignal;
	public:
		typedef typename Super::iterator		iterator;
		typedef typename Super::const_iterator		const_iterator;

		/**/						WorkPipe():locked(false)	{}
	    inline  void                SignalRead();		//!< Signals that the local object mutex should be locked and the read cursor reset for subsequent read access
	    inline  bool                operator>>(T&);	//!< Retrieves the object currently apointed by the read cursor. Automatically advances the read cursor 	@return true if the object could be retrieves, false if the read cursor has reached the end of the queue
	    inline  void                ExitRead();			//!< Signals that the local list should be flushed and the local object mutex released
	    inline  Self&				operator<<(const T&);		//!< Inserts an element to the end of the queue. This operation is mutex protected
		#if __BUFFER_RVALUE_REFERENCES__
			inline Self&			operator<<(T&&element);
		#endif

		inline  Self&				MoveAppend(T&);		//!< Similar to the << operator but uses strategy move, rather than forced copy
		inline	Self&				MoveAppend(BasicBuffer<T>&buffer);
	    inline  void                clear();			//!< Erases each object and flushs the queue. This operation is mutex protected
		inline	void				Clear() {clear();}
		inline	void				WaitForContent()	{contentSignal.wait();}
		inline  void				OverrideSignalNow() {contentSignal.signal();}
		
		typename Super::iterator		begin()	{ASSERT__(locked); return Super::begin();}
		typename Super::const_iterator	begin()	const {ASSERT__(locked); return Super::begin();}
		typename Super::iterator		end()	{return Super::end();}
		typename Super::const_iterator	end() const	{return Super::end();}

		inline count_t				Count() const {return Super::count();}
		inline count_t				size() const {return Super::size();}
		inline bool					IsEmpty() const {return Super::IsEmpty();}

		//the following operations are provided for convenience. They are _not_ mutex protected and (except for count() maybe) should be invoked only from inside a signalRead()/exitRead() block.
		//Super::IsEmpty;
		Super::IsNotEmpty;
		Super::operator[];
			
		typedef T	Type;
	
	};

/**
	@brief Pointer queue for scenarios where many threads write and one reads
*/
template <class T>
	class ProductionPipe:public WorkPipe<std::shared_ptr<T> >
	{
		typedef WorkPipe<std::shared_ptr<T> >Super;
	protected:
	public:

			//ReferenceVector<T>::Type;
	};



#include "production_pipe.tpl.h"

#endif
