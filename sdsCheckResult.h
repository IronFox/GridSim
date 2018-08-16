#pragma once

#include <global_root.h>
using namespace DeltaWorks;


/**
Storage for various metrics of an inconsistent SDS stored by a shard.
Methods are implemented in sdsCheckResult.coo
*/
struct TSDSCheckResult
{
	count_t			missingRCS=0;
	count_t			rcsPossiblyAvailableFromNeighbor=0,
					rcsLikelyAvailableFromNeighbor=0,
					rcsVeryLikelyAvailableFromNeighbor=0,
					outRCSUpdatable=0,
					rcsAvailableFromDatabase=0,
					rcsRestoredFromCache = 0;
	bool			predecessorIsConsistent = false,
					thisIsConsistent = false;

	bool			AnyOutRCSUpdateable() const volatile;
	count_t			RCSVeryLikelyAvailableFromNeighbor() const volatile;
	bool			AllThere() const {return missingRCS == 0;}
	bool			MissingAvailableFromNeighbors() const {return missingRCS == rcsPossiblyAvailableFromNeighbor;}
	bool			MissingAvailableFromAnywhere() const {return missingRCS == rcsPossiblyAvailableFromNeighbor + rcsAvailableFromDatabase;}
	bool			AnyAvailableFromNeighbors() const volatile {return rcsPossiblyAvailableFromNeighbor > 0;}
	bool			AnyAvailableFromAnywhere() const {return rcsPossiblyAvailableFromNeighbor > 0 || rcsAvailableFromDatabase > 0;}
	bool			ShouldRecoverThis0() const volatile;	//!< Ignores potential updates from neighbors unless they are very likely
	bool			ShouldRecoverThis_Original() const volatile;	//!< Original test
	bool			ShouldRecoverThis2() const volatile;	//!< Ignores potential updates from neighbors unless their makeup makes it likely those are produced (oracle mode)

	void			AssertEqualTo(const TSDSCheckResult&other) const;
};
