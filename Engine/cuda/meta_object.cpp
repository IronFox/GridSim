#include "../global_root.h"
#include "meta_object.h"


void	CUDA::CMetaChannel::freeMemory()
{
	if (!first)
		FATAL__("Failed to free sufficient device memory for allocation");
	
	suspended++;
	//cout << "reducing load of "<<total_allocated<<" by "<<first->contentSize()<<" bytes"<<endl;
	//total_allocated-=first->contentSize();
	shifted_bytes += first->contentSize();
	first->suspend();
	first->unlink();
}

