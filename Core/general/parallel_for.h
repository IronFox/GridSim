#ifndef general_parallel_forH
#define general_parallel_forH

#if SYSTEM==WINDOWS
	#include <ppl.h>
#else
	#include <tbb/parallel_for.h>

	namespace Concurrency=tbb;
#endif





#endif

