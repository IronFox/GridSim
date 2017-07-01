#include "debug_channel.h"


namespace Debug
{
	static SharedMemory::TMapping	mapping_;
	static SharedMemory::TDebugInfo	*info_;

	void		init()
	{
		/*mapping_ = SharedMemory::map("Global\\DebugChannel",sizeof(SharedMemory::TDebugInfo));
		if (!mapping_.data)
			FATAL__("Failed to open debug output channel. Terminating application.");
		
		info_ = (SharedMemory::TDebugInfo*)mapping_.data;*/
	}
	
	void		set(char context, unsigned line)
	{
		/*info_->domain[0] = context;
		info_->line = line;*/
	}
	
	void		setArray(char context, unsigned line, unsigned array_index)
	{
		/*info_->domain[0] = context;
		info_->line = line;
		info_->array_index0 = array_index;*/
	}

	void		setArray(char context, unsigned line, unsigned index0, unsigned index1)
	{
		/*info_->domain[0] = context;
		info_->line = line;
		info_->array_index0 = index0;
		info_->array_index1 = index1;*/
	}
}


