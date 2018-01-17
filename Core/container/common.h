#pragma once

#include "../global_root.h"
#include <memory>	//shared_ptr


template <typename T>
	inline bool operator==(const std::shared_ptr<T>&a, const std::weak_ptr<T>&b)
	{
		return a == b.lock();
	}
template <typename T>
	inline bool operator==(const std::weak_ptr<T>&a, const std::shared_ptr<T>&b)
	{
		return a.lock() == b;
	}
template <typename T>
	inline bool operator==(const std::weak_ptr<T>&a, const std::weak_ptr<T>&b)
	{
		return a.lock() == b.lock();
	}


template <typename T>
	inline bool operator!=(const std::shared_ptr<T>&a, const std::weak_ptr<T>&b)
	{
		return a != b.lock();
	}
template <typename T>
	inline bool operator!=(const std::weak_ptr<T>&a, const std::shared_ptr<T>&b)
	{
		return a.lock() != b;
	}
template <typename T>
	inline bool operator!=(const std::weak_ptr<T>&a, const std::weak_ptr<T>&b)
	{
		return a.lock() != b.lock();
	}



namespace DeltaWorks
{
	namespace Container
	{}
	
	namespace Ctr = Container;
	
	using namespace Container;	//I really don't think we need to use Container:: or Ctr:: all the time. Just good for destinction
}
