#pragma once

#include "types.h"
#include "entity.h"
#include <container/hashtable.h>
#include <io/cryptographic_hash.h>
#include "InconsistencyCoverage.h"
#include "EntityStorage.h"
#include <engine/renderer/opengl.h>
#include "DataSize.h"


extern const PRCS edgeInboundRCS;


/**
Core shard domain state, including generation counter, entity states, and IC, but nothing else
*/
class CoreShardDomainState
{
public:
	/**
	Contained entity states
	*/
	EntityStorage		entities;
	/**
	Current inconsistency ic
	*/
	IC					ic;
	ExtHGrid			hGrid;
	const index_t		generation=0, createdInTimestep=0;
	count_t				migrationCounter=0,
						mergeCounter=0;
	const TCodeLocation	precomputeCaller,birthPlace;
	TCodeLocation		finalizeCaller;

	/**/				CoreShardDomainState(index_t gen, index_t createdInTimestep, const TCodeLocation&birthPlace, const TCodeLocation&precomputeCaller):generation(gen),createdInTimestep(createdInTimestep),birthPlace(birthPlace),precomputeCaller(precomputeCaller)
	{
		ic.AllocateDefaultSize();
		ic.FlushInconsistency();
		ASSERT_GREATER_OR_EQUAL__(createdInTimestep,gen);
	}
	void				Hash(Hasher&)	const;


	bool				IsFullyConsistent() const {return this && ic.IsFullyConsistent();}
	bool				operator==(const CoreShardDomainState&other) const {return generation == other.generation && entities == other.entities && ic == other.ic;}
};

struct TPermanence
{
	/**/				TPermanence(){}
	/**/				TPermanence(const TPermanence&other)
	{
		wasRequired = (count_t)other.wasRequired;
	}

	void				operator=(const TPermanence&other)
	{
		wasRequired = (count_t)other.wasRequired;
	}

	void				Request() const
	{
		wasRequired++;
	}

	void				FlushRequests()
	{
		//ASSERT__(wasRequired==0);
		wasRequired = 0;
	}


	count_t				GetDemand(count_t age, bool isStoredInDatabase)
	{
		if (!isStoredInDatabase)
			return 0;
		return wasRequired ? 10+age : 1;
	}

	count_t				GetSDSDemand(count_t ageToBottom, bool isStoredInDatabase)
	{
		wasRequired = true;
		return GetDemand(ageToBottom,isStoredInDatabase) * NumNeighbors;
	}

	friend void swap(TPermanence&a, TPermanence&b)
	{
		count_t ac = (count_t)a.wasRequired;
		count_t bc = (count_t)b.wasRequired;
		a.wasRequired = bc;
		b.wasRequired = ac;
	}
private:
	mutable std::atomic<count_t>		wasRequired=0;
};

class RemoteChangeSet
{
public:
	PRCS					verifiedWith;
	ChangeSet				cs;
	InconsistencyCoverage	ic;
	HGrid					hGrid;
	const TCodeLocation		birthPlace;

	/**/					RemoteChangeSet(const TCodeLocation&birthPlace):birthPlace(birthPlace){}

	DataSize				GetDataSize() const
	{
		return DataSize(1,cs.CountInstructions());
	}

	void					VerifyIntegrity(const TCodeLocation&loc) const;
	void					Verify(const PRCS&consistentMatch);
};


class FullShardDomainState
{
	index_t					generation = 0;
	PCoreShardDomainState	output;

public:


	TPermanence				permanence;
	/**
	Holds all processed entities. Recreated during PrecomputeSuccessor()
	*/
	EntityStorage			processed;
	index_t					lastTouched = InvalidIndex;
	bool					significantInboundChange = false;
	ChangeSet				localCS;
	FixedArray<TPermanence,NumNeighbors> inRCSPermanence;

	/**
	Indicates that the entity states derived from the predecessor SDS was consistent and complete
	*/
	bool					inputConsistent=true;
	Hasher::HashContainer	inputHash = Hasher::HashContainer::Empty;

	std::shared_ptr<HashProcessGrid>	pGrid;

	/**
	'userMessages' are a serious problem.
	the way it stands, they may be lost if the shard dies.
	worse yet, their non-existence is not detectable at this point.

	theoretical solution:
	entities are tagged as interactive or non-interactive.
	if interactive, a no-op user message is expected each sds.
		if this message does not exist, that entity and the space it inhabits must be flagged inconsistent.
	if the entity is non-interactive, any user-message directed to it are disregarded.

	entities may freely change their mode any time, as long as this decision remains deterministic.
	a user logging off may thus deactivate his avatars.

	contrary to the current implementation, user-messages must be archived in the database.
	single-layer implementations may regard missing user-messages as no-ops.
	multi-layer implementations must treat those as inconsistency, but may revert this state if merge-operations confirm or fix this state
	*/
	Buffer0<Op::Message>	userMessages;



	struct OutRemoteChangeSet
	{
		bool				confirmed=false;
		PRCS				ref;

		void				swap(OutRemoteChangeSet&other)
		{
			swp(confirmed,other.confirmed);
			ref.swap(other.ref);
		}

		friend void			swap(OutRemoteChangeSet&a, OutRemoteChangeSet&b)
		{
			a.swap(b);
		}
	};



	FixedArray<OutRemoteChangeSet,NumNeighbors>	outboundRCS;
	FixedArray<PRCS,NumNeighbors>				inboundRCS;

	void				swap(FullShardDomainState&other)
	{
		using std::swap;
		swap(generation,other.generation);
		swap(permanence,other.permanence);
		swp(significantInboundChange,other.significantInboundChange);
		swp(lastTouched,other.lastTouched);
		inRCSPermanence.swap(other.inRCSPermanence);
		processed.swap(other.processed);
		output.swap(other.output);
		inboundRCS.swap(other.inboundRCS);
		outboundRCS.swap(other.outboundRCS);
		localCS.swap(other.localCS);
		swp(inputConsistent,other.inputConsistent);
		swp(inputHash,other.inputHash);
		userMessages.swap(other.userMessages);

		if (output)
			ASSERT_EQUAL__(generation,output->generation);
		if (other.output)
			ASSERT_EQUAL__(other.output->generation,other.GetGeneration());
	}

	//void				operator=(const SDS&other)
	//{
	//	significantInboundChange = other.significantInboundChange;
	//	lastTouched = other.lastTouched;
	//	processed = other.processed;
	//	GetOutput() = other.GetOutput();
	//	inboundRCS = other.inboundRCS;
	//	outboundRCS = other.outboundRCS;
	//	localCS = other.localCS;
	//	inputConsistent = other.inputConsistent;
	//	outputConsistent = other.outputConsistent;
	//	inputHash = other.inputHash;
	//	userMessages = other.userMessages;
	//}


	/**/				FullShardDomainState(){}


	/**
	Sets all inbound RCS to which neighbors do not exist and hence data can never be received
	*/
	void				SetOpenEdgeRCS(const Shard&owner);

	index_t				GetGeneration() const {return generation;}
	void				InitGeneration(index_t gen, index_t currentTimestep, bool reallocateOutput, const TCodeLocation&birthPlace, const TCodeLocation&precomputeCaller);

	const PCoreShardDomainState&	GetOutput() const {return output;}
	void				OverrideSetOutput(Shard&parent, const PCoreShardDomainState&out, const TCodeLocation&caller);
	void				AllocateOutput(index_t currentTimestep, const TCodeLocation&loc)
	{
		ASSERT__(!output);
		output.reset(new CoreShardDomainState(generation,currentTimestep,loc,loc));
	}
	DB::Result			DownloadOutput(Shard&s);

	/**
	Executes all entity logic processes, and adds automated operations.
	@param[out] outLocal Puts all resulting operations here
	@param motionSpace Outer edge of the simulated space
	*/
	void				ExecuteLogic(CS&outLocal, index_t generation, const TGridCoords&gridCoords, const TBoundaries&motionSpace);

	void				LosePermanence(bool isFirst);
	void				ClearDemand();
	void				RegisterDemand(DownloadDemandRegistry&reg,count_t generationsToTop, count_t generationsToBottom);

	bool				IsConsistentAndConfirmed() const;
	bool				AllConfirmed() const;
	bool				AllReceived() const;
	bool				AllRCSAreLocalAndConsistent() const;
	TSDSCheckResult		CheckMissingRCS(Shard&);

	void				PrecomputeSuccessor(Shard&shard, SDS&rs,const TBoundaries&motionSpace, index_t currentTimestep, const TCodeLocation&caller)	const;
	void				FinalizeComputation(Shard&shard, const TCodeLocation&caller);


//	void				AssertEquality(const SDS&other) const;
	/**
	Asserts total equality between this and other.
	The method assumes both the local and remote SDS are fully consistent
	*/
	void				AssertTotalEquality(const SDS&other, const TGridCoords&shardOffset)const;
	/**
	Asserts equality of space segments where both this and other are consistent.
	Segments where either this or other are inconsistent are ignored.
	*/
	void				AssertSelectiveEquality(const SDS&other, const TGridCoords&shardOffset)const;
	static void			AssertSelectiveEquality(const PCoreShardDomainState&, const PCoreShardDomainState&, const TGridCoords&shardOffset);

	void				SynchronizeWithSibling(Shard&myShard,Shard&siblingShard, SDS&sibling, const IC::Comparator&, index_t currentTimestep);
};

DECLARE_DEFAULT_STRATEGY(FullShardDomainState,Swap);

