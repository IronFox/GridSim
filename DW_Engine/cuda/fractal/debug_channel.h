#ifndef debug_channelH
#define debug_channelH

#include "../../io/shared_memory.h"

#define DEBUG_POINT(_CONTEXT_)							//Debug::set((#_CONTEXT_)[0],__LINE__);
#define ARRAY_DEBUG_POINT(_CONTEXT_,_INDEX_)			//Debug::setArray((#_CONTEXT_)[0],__LINE__,_INDEX_);
#define ARRAY_DEBUG_POINT2(_CONTEXT_,_INDEX0_,_INDEX1_)	//Debug::setArray((#_CONTEXT_)[0],__LINE__,_INDEX0_,_INDEX1_);

namespace Debug
{
	void		init();
	void		set(char context, unsigned line);
	void		setArray(char context, unsigned line, unsigned array_index);
	void		setArray(char context, unsigned line, unsigned index0, unsigned index1);
}





#endif
