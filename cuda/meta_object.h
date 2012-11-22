#ifndef cuda_meta_objectH
#define cuda_meta_objectH
#include <cstdlib>
#include <iostream>

#include "../global_string.h"
 
using namespace std;

namespace CUDA
{

	class CMetaObject;

	
	/**
		@brief Abstract storage space
	*/
	class CMetaChannel
	{
	public:
			CMetaObject	*first,	//!< Link to the least recently referenced meta object in allocated memory
						*last;	//!< Link to the most recently referenced meta object in allocated memory
			UINT64		total_allocated,
						shifted_bytes,
						memory_limit;
			UINT32		restored,
						suspended;
			
						CMetaChannel():first(NULL),last(NULL),total_allocated(0),shifted_bytes(0),memory_limit(0),restored(0),suspended(0)
						{}
						
			void		freeMemory();

			void		allocate(size_t bytes)
						{
							total_allocated += bytes;
							if (memory_limit > 0)
								while (total_allocated > memory_limit)
									freeMemory();
							ASSERT2__(total_allocated >= (unsigned)bytes,total_allocated,(unsigned)bytes);
						}
			void		delocate(size_t bytes)
						{
							total_allocated -= bytes;
						}
	};



	/**
		@brief Persistent object that can be moved between host and device memory as needed
	*/
	/*virtual*/ class CMetaObject
	{
	protected:
			friend class CMetaChannel;
			
			CMetaObject	*prev,	//!< Object preceeding the current one or NULL if no such exists
						*next;	//!< Object succeeding the current one or NULL if no such exists
			CMetaChannel*channel;
	
						CMetaObject(CMetaChannel*channel_=NULL):prev(NULL),next(NULL),channel(channel_)
						{}
	virtual				~CMetaObject()
						{
							unlink();
						}
	virtual	void		suspend()=0;		//!< Transfers the object to offline space, effectivly clearing any occupied online memory
	virtual	void		restore()=0;		//!< Transfers the object to online space. This operation may cause other objects to be suspended to offline memory
	virtual	size_t		contentSize()	const=0;		//! Returns the summarized size of all contained elements in bytes \return Size of all elements
	
	
			void		reregister(bool move_to_top)			//!< (Re)registers the local object as the most recently referenced object.
						{
							if (!channel)
								return;
							if ((next || prev) && !move_to_top)
								return;
							if (channel->last == this)
								return;
							unlink();
							prev = channel->last;
							if (prev)
								prev->next = this;
							else
								channel->first = this;
							channel->last = this;
						}
						
			void		unlink()
						{
							if (prev)
								prev->next = next;
							elif (channel && channel->first == this)
								channel->first = next;
							
							if (next)
								next->prev = prev;
							elif (channel && channel->last == this)
								channel->last = prev;
							prev = NULL;
							next = NULL;
						}


	};

}

#endif