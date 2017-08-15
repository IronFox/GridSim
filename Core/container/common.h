#pragma once

#include "../global_root.h"
#include <memory>	//shared_ptr

namespace std
{
	template <typename T>
		inline bool		operator==(const shared_ptr<T>&a, const weak_ptr<T>&b)
		{
			return a == b.lock();
		}

	template <typename T>
		inline bool		operator==(const weak_ptr<T>&a, const shared_ptr<T>&b)
		{
			return b == a.lock();
		}
}


namespace DeltaWorks
{
	namespace Container
	{}
	
	namespace Ctr = Container;
	
	using namespace Container;	//I really don't think we need to use Container:: or Ctr:: all the time. Just good for destinction
}
