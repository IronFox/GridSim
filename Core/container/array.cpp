#include "array.h"

namespace DeltaWorks
{
	namespace Container
	{
		template class Array<int>;
		template class MutableArrayRef<int>;
		template class FixedArray<int,10>;
	}
}
