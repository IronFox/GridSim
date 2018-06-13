#pragma once

#include "types.h"
#include <memory>
#include <container/hashtable.h>
#include <general/enumeration.h>

namespace Database
{
	typedef PCoreShardDomainState	SDS;
	typedef PRCS RCS;

	struct SDS_ID
	{
		TGridCoords		destination;
		index_t			generation;


		String			ToString() const {return destination.ToString()+" g"+String(generation);}
		friend hash_t	Hash(const SDS_ID&id)
		{
			return HashValue() << id.destination << id.generation;
		}

		bool			operator==(const SDS_ID&other) const {return destination == other.destination && generation == other.generation;}
		bool			operator!=(const SDS_ID&other) const {return !operator==(other);}
	};

	static const constexpr size_t SDS_ID_size = sizeof(SDS_ID);

	struct RCS_ID
	{
		TGridCoords		origin,destination;
		index_t			generation;

		friend hash_t	Hash(const RCS_ID&id)
		{
			return HashValue() << id.origin << id.destination << id.generation;
		}

		String			ToString() const {return origin.ToString()+"->"+destination.ToString()+" g"+String(generation);}
		bool			operator==(const RCS_ID&other) const {return origin == other.origin && destination == other.destination && generation == other.generation;}
		bool			operator!=(const RCS_ID&other) const {return !operator==(other);}
	};


	class CoordGenMap
	{
	protected:
		HashTable<TGridCoords,index_t>	newestSDS;

	public:
		index_t		NewestKnownSDSGeneration(const TGridCoords&coords) const;
		bool		Update(const TGridCoords&coords, index_t);
		void		Clear()	{newestSDS.Clear();}
	};

	//class LockedCoordGenMap : private CoordGenMap
	//{
	//	typedef CoordGenMap	Super;
	//	mutable SpinLock	lock;
	//public:
	//	index_t		NewestKnownSDSGeneration(const TGridCoords&coords) const;
	//	bool		Update(const TGridCoords&coords, index_t);
	//	void		Clear();
	//	void		TransferNeighborhood(const TGridCoords&coords, CoordGenMap&targetMap, count_t timestepDelay) const;
	//};

	template <typename T>
		struct Container
		{
			T		entry;
			index_t	creationTimestep;
			HashTable<Client*,index_t> requestedByFor;



			void swap(Container&other)
			{
				entry.swap(other.entry);
				swp(creationTimestep,other.creationTimestep);
				requestedByFor.swap(other.requestedByFor);
			}
		};
	template <typename T>
		using PContainer = std::shared_ptr<Container<T> >;

	template <typename T>
		struct CacheEntry
		{
			T				data;
			index_t			uploadedAt=0,
							requestedAt=0,
							downloadedAt=0;
		};
	template <typename ID, typename T>
		struct ClientChannel
		{
			HashTable<ID, CacheEntry<T>>	cache;

			struct TUpload
			{
				index_t		remainingRounds;
				ID			id;
				T			data;
			};

			Buffer0<TUpload>	uploadList;
			mutable Sys::SpinLock	uploadLock,downloadLock;


			void			Clear() {cache.Clear(); uploadList.Clear();}
		};


	CONSTRUCT_ENUMERATION4(Result,Downloaded, Requested, CannotDownload, SystemLocked);


	void			ProcessDistribution();
	void			AdvanceTimestep(index_t);

	class Client
	{
		index_t			generation = 0;
		bool			active = true;
		ClientChannel<RCS_ID, RCS>	myRCSChannel;
		ClientChannel<SDS_ID, SDS>	mySDSChannel;
		
		index_t			minimumGeneration=0,serverGeneration=0;

		friend void ::DB::ProcessDistribution();

		void			ProcessUploads();
		void			UpdateCoordMap();
	public:
		::Shard*const	owner;

		/**/			Client(Shard*owner);
		/**/			Client() = delete;
		/**/			Client(const Client&) = delete;
		void			operator=(const Client&) = delete;
		/**/			~Client();
		
		index_t			GetGeneration() const {return generation;}

		bool			SDSIsUploading(index_t generation) const;
		bool			SDSExists(index_t generation) const;
		void			AssertSDSExists(index_t generation) const;
		bool			ExistsAnywhere(const SDS_ID&) const;
		bool			ExistsAnywhere(const RCS_ID&) const;
		bool			IsUploading(const SDS_ID&) const;
		bool			IsUploading(const RCS_ID&) const;
		bool			IsCached(const SDS_ID&) const;
		bool			IsCached(const RCS_ID&) const;
		Result			Download(index_t generation, SDS&target);
		Result			Download(const RCS_ID&, RCS&target);
		Result			DownloadInbound(const Shard*source, index_t generation, RCS&);
		bool			Upload(const SDS&, bool instantly=false);
		bool			Upload(const RCS_ID&, const RCS&, bool instantly=false);

		
		void			SignalShutdown();
		void			SignalStartup();

		void			UpdateMinimumGeneration(index_t gen, const TCodeLocation&cause);
		index_t			GetMinimumGeneration() const {return minimumGeneration;}

		void			SignalDownload(const RCS_ID&, const RCS&, index_t uploadedAt, index_t requestedAt);
		void			SignalDownload(const SDS_ID&, const SDS&, index_t uploadedAt, index_t requestedAt);
	};
	SDS_ID				ID(const Shard&, const SDS&);
	SDS_ID				ID(const Shard&, index_t gen);
	RCS_ID				ID(const Shard*source, const Shard*destination, index_t generation);

	void				Lock();
	void				Unlock();
	bool				IsLocked();

	void				Reset(const GridSize&size);
	void				AssertIsWellDefined();


	index_t				NewestKnownSDSGeneration(const TGridCoords&c);
}
