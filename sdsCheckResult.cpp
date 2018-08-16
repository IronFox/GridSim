#include "types.h"
#include "sdsCheckResult.h"


bool			TSDSCheckResult::ShouldRecoverThis_Original() const volatile
{
	return !thisIsConsistent 
		&& 
		(
			rcsPossiblyAvailableFromNeighbor > 0
			|| rcsRestoredFromCache > 0 
			//|| outRCSUpdatable > 0
			|| (missingRCS == 0 && predecessorIsConsistent) 
		);
}

bool			TSDSCheckResult::ShouldRecoverThis2() const volatile
{
	bool rs = !thisIsConsistent 
		&& 
		(
			RCSVeryLikelyAvailableFromNeighbor() > 0
			//RCSVeryLikelyAvailableFromNeighbor(precise) == missingRCS
			|| rcsRestoredFromCache > 0 
			//|| AnyOutRCSUpdateable(precise)
			|| (missingRCS == 0 && predecessorIsConsistent) 
		);
	return rs;
}


bool			TSDSCheckResult::ShouldRecoverThis0() const volatile
{
	bool rs = !thisIsConsistent 
		&& 
		(
			rcsLikelyAvailableFromNeighbor > 0
			|| rcsRestoredFromCache > 0 
			//|| outRCSUpdatable > 0
			|| (missingRCS == 0 && predecessorIsConsistent) 
		);
	return rs;
}


count_t			TSDSCheckResult::RCSVeryLikelyAvailableFromNeighbor() const volatile
{
	return (rcsVeryLikelyAvailableFromNeighbor);
}


bool			TSDSCheckResult::AnyOutRCSUpdateable() const volatile
{
	return (outRCSUpdatable) > 0;
}


void			TSDSCheckResult::AssertEqualTo(const TSDSCheckResult&other) const
{
	ASSERT_EQUAL__(missingRCS,other.missingRCS);
	ASSERT_EQUAL__(rcsPossiblyAvailableFromNeighbor,other.rcsPossiblyAvailableFromNeighbor);
	ASSERT_EQUAL__(rcsLikelyAvailableFromNeighbor,other.rcsLikelyAvailableFromNeighbor);
	ASSERT_EQUAL__(rcsVeryLikelyAvailableFromNeighbor,other.rcsVeryLikelyAvailableFromNeighbor);
	ASSERT_EQUAL__(outRCSUpdatable,other.outRCSUpdatable);
	ASSERT_EQUAL__(rcsAvailableFromDatabase,other.rcsAvailableFromDatabase);
	ASSERT_EQUAL__(rcsRestoredFromCache,other.rcsRestoredFromCache);
	ASSERT_EQUAL__(predecessorIsConsistent,other.predecessorIsConsistent);
	ASSERT_EQUAL__(thisIsConsistent,other.thisIsConsistent);
}
