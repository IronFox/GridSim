#ifndef boost_pointerH
#define boost_pointerH


#include "../container/strategy.h"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

DECLARE_T__(boost::shared_ptr,Swappable);
DECLARE_T__(boost::weak_ptr,Swappable);

namespace boost
{
	void	no_op(const void*)	{}

};




#endif
