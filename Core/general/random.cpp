#include "../global_root.h"
#include "random.h"
#include <random>


/******************************************************************

E:\include\general\random.cpp

******************************************************************/

namespace DeltaWorks
{

	template class RNG<SimpleRandomSource>;
	template class RNG<std::mt19937>;
	template class RNG<std::mt19937_64>;


	std::atomic<index_t>		randomRandomizationModifier = time(NULL);



	void        SimpleRandomSource::Advance()
	{
		last = last * 1103515245 + 12345;
	}


	void        SimpleRandomSource::discard(count_t queries)
	{
		for (index_t i = 0; i < queries; i++)
			Advance();
	}

	UINT32   SimpleRandomSource::NextSeed()
	{
		Advance();
		return last;
	}

	UINT    SimpleRandomSource::operator()()
	{
		Advance();
		return((UINT)(last>>16) & Mask);
	}
}
