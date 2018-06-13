#include "Database.h"
#include <container/hashtable.h>
#include <mutex>
#include "shard.h"
#include <ppl.h>
#include "InconsistencyCoverage.h"

namespace Database
{

	#ifdef DBG_SHARD_HISTORY
		#define DB_DBG_PARAMETER	,String&dbgOut
		#define DB_DBG_FORWARD		,dbgOut
	#else
		#define DB_DBG_PARAMETER	
		#define DB_DBG_FORWARD
	#endif



	count_t currentTimestep = 0,distributionTimestep=0;
	volatile bool locked = false;

	//static const count_t UploadTime = 4;
	//static const count_t DownloadTime = 8;

	static const constexpr float DownloadDelayMultiplier = 2;	//as opposed to upload
	#ifdef D3
		static const constexpr float TransferDelayPerICCell = 0.005f;
	#else
		static const constexpr float TransferDelayPerICCell = 0.05f;
	#endif

	static const count_t MapTransferDelay = 1;


	count_t GetUploadTime(const InconsistencyCoverage&ic)
	{
		count_t rs = std::max<count_t>(1, M::Round(TransferDelayPerICCell * ic.GetGrid().Count()));
		return rs;
	}

	count_t GetDownloadTime(const InconsistencyCoverage&ic)
	{
		count_t rs = std::max<count_t>(1, M::Round(TransferDelayPerICCell * DownloadDelayMultiplier * ic.GetGrid().Count()));

		return rs;
	}


	template <typename ID, typename T>
		class Requestable
		{
			T		data;
			ID		myID;
			index_t	uploadedAt=0;

			struct TRequest
			{
				count_t	remainingRounds=0,
						requestedAt=0,
						clientGeneration=0;
			};
			HashTable<Client*,TRequest> requestedByFor;

		public:
			/**/	Requestable()	{}
			/**/	Requestable(const ID&id, const T&data):myID(id),data(data),uploadedAt(distributionTimestep)	{}

			const ID&GetID() const {return myID;}
			const T&GetData() const {return data;}
			void	swap(Requestable<ID,T>&other)
			{
				swp(myID,other.myID);
				data.swap(other.data);
				requestedByFor.swap(other.requestedByFor);
			}

			void	Distribute()
			{
				requestedByFor.FilterEntries([this](Client*cl, TRequest&req)
				{
					if (req.clientGeneration != cl->GetGeneration())
						return false;
					if (req.remainingRounds > 0)
						req.remainingRounds--;
					if (!req.remainingRounds)
					{
						cl->SignalDownload(myID,data,uploadedAt,req.requestedAt);
						return false;
					}
					return true;
				});
			}

			void	Request(Client*cl DB_DBG_PARAMETER)
			{
				auto ptr = requestedByFor.QueryPointer(cl);
				if (ptr)
				{
					if (ptr->clientGeneration == cl->GetGeneration())
					{
						#ifdef DBG_SHARD_HISTORY
							dbgOut = "Request already set";
						#endif
						return;	//all good
					}
					ptr->requestedAt = distributionTimestep;
					ptr->remainingRounds = GetDownloadTime(data->ic);
					ptr->clientGeneration = cl->GetGeneration();

					#ifdef DBG_SHARD_HISTORY
						dbgOut = " recycled: dt"+String(ptr->remainingRounds)+" u"+String(uploadedAt);
					#endif
					return;
				}
				{
					auto&entry = requestedByFor.Set(cl);
					entry.requestedAt = distributionTimestep;
					entry.remainingRounds = GetDownloadTime(data->ic);
					entry.clientGeneration = cl->GetGeneration();
					#ifdef DBG_SHARD_HISTORY
						dbgOut = " dt"+String(entry.remainingRounds)+" u"+String(uploadedAt);
					#endif
				}
			}

			count_t	CountRequests() const
			{
				return requestedByFor.Count();
			}

			void			SignalShutdown(Client*cl)
			{
				requestedByFor.Unset(cl);
			}

		};




	class RCSStack
	{
		mutable Sys::SpinLock	lock;



		Buffer0<Requestable<RCS_ID,RCS>,Swap> stack,dump;


		index_t			Find(const index_t gen) const
		{
			if (stack.IsEmpty())
				return 0;
			if (gen < stack.First().GetID().generation)
				return 0;
			if (gen > stack.Last().GetID().generation)
				return stack.Count();

			index_t begin = 0;
			index_t end = stack.Count();

			while (begin < end)
			{
				const index_t middle = (begin+end)/2;
				const index_t compare = stack[middle].GetID().generation;
				if (gen < compare)
				{
					end = middle;
					continue;
				}
				if (gen > compare)
				{
					begin = middle+1;
					continue;
				}
				return middle;
			}
			return begin;
		}
	public:

		void			DistributeNoLock()
		{
			//lock.lock();
				foreach (stack,st)
					st->Distribute();
				for (index_t d = 0; d < dump.Count(); d++)
				{
					dump[d].Distribute();
					if (dump[d].CountRequests() == 0)
					{
						dump.Erase(d);
						d--;
					}
				}
			//lock.unlock();
		}

		void			Insert(const RCS_ID&id, const RCS&rcs)
		{
			lock.lock();
				const index_t at = Find(id.generation);
				if (at >= stack.Count() || stack[at].GetID().generation != id.generation)
					stack.Insert(at,Requestable<RCS_ID,RCS>(id ,rcs));
				
				for (index_t i = 1; i < stack.Count(); i++)
					ASSERT_GREATER__(stack[i].GetID().generation,stack[i-1].GetID().generation);


			lock.unlock();
		}

		bool			Get(index_t gen, RCS&out) const
		{
			lock.lock();
				const index_t at = Find(gen);
				bool rs = at < stack.Count() && stack[at].GetID().generation == gen;
				if (rs)
					out = stack[at].GetData();
			lock.unlock();
			return rs;
		}

		bool			Has(index_t gen) const
		{
			lock.lock();
				const index_t at = Find(gen);
				bool rs = at < stack.Count() && stack[at].GetID().generation == gen;
			lock.unlock();
			return rs;
		}

		void			Trim(index_t minGeneration)
		{
			lock.lock();
				index_t remove = 0;
				while (remove < stack.Count() && stack[remove].GetID().generation < minGeneration)
				{
					if (stack[remove].CountRequests())
						dump.Append().swap(stack[remove]);
					remove++;
				}
				stack.Erase(0,remove);
			lock.unlock();
		}

		void			Request(Client*cl, index_t generation DB_DBG_PARAMETER)
		{
			lock.lock();
				const index_t at = Find(generation);
				bool rs = at < stack.Count() && stack[at].GetID().generation == generation;
				if (rs)
					stack[at].Request(cl DB_DBG_FORWARD);
				#ifdef DBG_SHARD_HISTORY
					else
						dbgOut = "Bad Request";
				#endif
			lock.unlock();
		}

		void			SignalShutdown(Client*cl)
		{
			lock.lock();
				foreach (stack,st)
					st->SignalShutdown(cl);
			lock.unlock();
		}


	};

	class ShardState
	{
		mutable Sys::SpinLock		lock;

		Requestable<SDS_ID,SDS>	current;
		Buffer0<Requestable<SDS_ID,SDS> > dump;

		TGridCoords			myCoords;
		GridArray<RCSStack>	stacks;
	public:

		static const constexpr count_t RCSStackCount =
			#ifdef D3
				3*3*3
			#else
				3*3
			#endif
			;
		
		/**/			ShardState()
		{
			#ifdef D3
				stacks.SetSize(3,3,3);
			#else
				stacks.SetSize(3,3);
			#endif
		}

		void			Setup(const TGridCoords&myCoords)
		{
			this->myCoords = myCoords;
		}

		void			DistributeSDSNoLock()
		{
			//lock.lock();
				current.Distribute();

				for (index_t d = 0; d < dump.Count(); d++)
				{
					dump[d].Distribute();
					if (dump[d].CountRequests() == 0)
					{
						dump.Erase(d);
						d--;
					}
				}

			//lock.unlock();
		}


		void			DistributeRCSNoLock(index_t stack)
		{
			stacks[stack].DistributeNoLock();
		}


		void			Update(const SDS&newSDS)
		{
			lock.lock();
				if (!current.GetData() || current.GetID().generation < newSDS->generation)
				{
					if (current.CountRequests())
						dump.Append().swap(current);
					SDS_ID newID;
					newID.destination = myCoords;
					newID.generation = newSDS->generation;
					current = Requestable<SDS_ID,SDS>(newID,newSDS);

					foreach (stacks,s)
						s->Trim(newSDS->generation+1);
				}
			lock.unlock();
		}


		RCSStack&		GetStack(const TGridCoords&originCoords)
		{
			const TGridCoords delta = originCoords - myCoords;
			#ifdef D3
				return stacks.Get(delta.x+1,delta.y+1,delta.z+1);
			#else
				return stacks.Get(delta.x+1,delta.y+1);
			#endif
		}

		bool			RCSExists(const TGridCoords&originCoords, index_t gen)
		{
			return GetStack(originCoords).Has(gen);
		}

		bool			Exists(index_t generation) const
		{
			lock.lock();
				bool rs = current.GetData() && current.GetID().generation == generation;
			lock.unlock();
			return rs;
		}

		void			AssertExists(index_t generation) const
		{
			lock.lock();
				ASSERT__(current.GetData());
				ASSERT__(current.GetData()->ic.IsSealedAndConsistent());
				ASSERT__(current.GetID().destination==myCoords);	//just in case
				ASSERT_EQUAL__(current.GetID().generation,generation);
			lock.unlock();
		}

		bool			Exists() const
		{
			lock.lock();
				bool rs = current.GetData() != nullptr;
			lock.unlock();
			return rs;
		}

		index_t			GetSDSGeneration() const
		{
			lock.lock();
				index_t rs = GetSDSGenerationNoLock();
			lock.unlock();
			return rs;
		}

		index_t			GetSDSGenerationNoLock() const
		{
			return current.GetData() ? current.GetID().generation : 0;
		}


		void			Request(Client*cl, index_t generation DB_DBG_PARAMETER)
		{
			lock.lock();
				if (current.GetData() && current.GetID().generation == generation)
					current.Request(cl DB_DBG_FORWARD);
				#ifdef DBG_SHARD_HISTORY
					else
						dbgOut = "Bad request";
				#endif
			lock.unlock();
		}


		void			RequestRCS(Client*cl, const TGridCoords&senderCoords, index_t generation DB_DBG_PARAMETER)
		{
			GetStack(senderCoords).Request(cl,generation DB_DBG_FORWARD);
		}


		void			SignalShutdown(Client*cl)
		{
			lock.lock();
				foreach (dump,d)
					d->SignalShutdown(cl);
				current.SignalShutdown(cl);
			lock.unlock();

			foreach (stacks,st)
				st->SignalShutdown(cl);
		}
	};


	GridArray<ShardState>	globalStore;



	Sys::SpinLock		clientListLock;
	Buffer0<Client*>	clientList;


	void			StoreParallel(const std::function<void(ShardState&)>&f)
	{
		Concurrency::parallel_for(index_t(0),globalStore.Count(),
		[f](index_t i)
		{
			f(globalStore[i]);
		});


		//for (index_t i = 0; i < globalStore.Count(); i++)
		//	f(globalStore[i]);
	}

	ShardState&		GetGlobal(const TGridCoords&c)
	{
		#ifdef D3
			return globalStore.Get(c.x,c.y,c.z);
		#else
			return globalStore.Get(c.x,c.y);
		#endif
	}


	index_t	CoordGenMap::NewestKnownSDSGeneration(const TGridCoords&coords)	const
	{
		const index_t*ptr = newestSDS.QueryPointer(coords);
		if (ptr)
			return *ptr;
		return 0;
	}

	bool		CoordGenMap::Update(const TGridCoords&coords, index_t gen)
	{
		index_t*current = newestSDS.QueryPointer(coords);
		if (!current)
		{
			newestSDS.Set(coords,gen);
			return true;
		}
		if (*current < gen)
		{
			*current = gen;
			return true;
		}
		return false;
	}







	template <typename ID, typename T>
		bool	_IsUploading(const Client*client, const ID&id, const ClientChannel<ID,T>&clChannel)
		{
			clChannel.uploadLock.lock();
			foreach (clChannel.uploadList,up)
				if (up->id == id)
				{
					clChannel.uploadLock.unlock();
					return true;
				}
			clChannel.uploadLock.unlock();
			return false;
		}

	bool			Client::ExistsAnywhere(const SDS_ID&id) const
	{
		ASSERT__(active);
		return mySDSChannel.cache.IsSet(id) ||
			GetGlobal(id.destination).Exists(id.generation);
	}
	bool			Client::ExistsAnywhere(const RCS_ID&id) const
	{
		ASSERT__(active);
		return myRCSChannel.cache.IsSet(id) ||
			GetGlobal(id.destination).RCSExists(id.origin,id.generation);
	}

	bool			Client::IsUploading(const SDS_ID&id) const
	{
		ASSERT__(active);
		return _IsUploading(this,id,mySDSChannel);
	}
	bool			Client::IsUploading(const RCS_ID&id) const
	{
		ASSERT__(active);
		return _IsUploading(this,id,myRCSChannel);
	}

	bool			Client::SDSExists(index_t generation) const
	{
		ASSERT__(active);
		return GetGlobal(owner->gridCoords).Exists(generation);
	}
	void			Client::AssertSDSExists(index_t generation) const
	{
		ASSERT__(active);
		GetGlobal(owner->gridCoords).AssertExists(generation);
	}
	bool			Client::SDSIsUploading(index_t generation) const
	{
		ASSERT__(active);
		return _IsUploading(this,ID(*owner,generation),mySDSChannel);
	}



	bool			Client::IsCached(const SDS_ID&id) const
	{
		ASSERT__(active);
		return this->mySDSChannel.cache.IsSet(id);
	}

	bool			Client::IsCached(const RCS_ID&id) const
	{
		ASSERT__(active);
		return this->myRCSChannel.cache.IsSet(id);
	}




	template <typename ID, typename T>
	Result			_Download(Client*cl, const ID&id, const ClientChannel<ID, T>&clChannel, T&target DB_DBG_PARAMETER)
	{
		ASSERT__(!locked);
		if (locked)
			return Result::SystemLocked;

		ASSERT__(!_IsUploading(cl,id,clChannel));

		auto e = clChannel.cache.QueryPointer(id);
		if (e)
		{
			target = e->data;
			ASSERT__(target->ic.IsSealedAndConsistent());
			#ifdef DBG_SHARD_HISTORY
				dbgOut = "d"+String(e->downloadedAt)+ " r"+String(e->requestedAt)+" u"+String(e->uploadedAt);
			#endif
			return Result::Downloaded;
		}
		if (id.generation < cl->GetMinimumGeneration())
			return Result::CannotDownload;

		return Result::Requested;

		//std::lock_guard<std::mutex> lock(channel.mutex);
		//auto p = channel.Query(id);
		//
		//if (!p)
		//{
		//	//ASSERT_GREATER__(id.generation,globalMap.NewestKnownSDSGeneration(id.destination));
		//}

		//if (p && !p->requestedByFor.IsSet(cl))
		//	p->requestedByFor.Set(cl,GetDownloadTime(p->entry->ic));
		//return Result::Requested;
	}

	Result			Client::Download(index_t generation, SDS&target)
	{
		ASSERT__(active);
		#ifdef DBG_SHARD_HISTORY
			String dbgOut;
		#endif
		Result rs= _Download(this,ID(*owner,generation),mySDSChannel, target DB_DBG_FORWARD);
		if (rs == Result::Requested)
		{
			GetGlobal(owner->gridCoords).Request(this,generation DB_DBG_FORWARD);
			#ifdef DBG_SHARD_HISTORY
				owner->LogEvent("SDS Download Request t="+String(distributionTimestep)+" "+owner->gridCoords.ToString()+" g"+String(generation)+" "+dbgOut);
			#endif
		}
		#ifdef DBG_SHARD_HISTORY
			elif (rs == Result::Downloaded)
			{
				owner->LogEvent("SDS Downloaded t="+String(distributionTimestep)+": "+owner->gridCoords.ToString()+" g"+String(generation)+" "+dbgOut);
			}
		#endif

		return rs;
	}

	Result			Client::DownloadInbound(const Shard*source, index_t generation, RCS&ref)
	{
		ASSERT__(active);
		#ifdef DBG_SHARD_HISTORY
			String dbgOut;
		#endif
		auto rs = _Download(this,ID(source,owner,generation),myRCSChannel,ref DB_DBG_FORWARD);
		if (rs == Result::Requested)
		{
			GetGlobal(owner->gridCoords).RequestRCS(this,source->gridCoords,generation DB_DBG_FORWARD);
			#ifdef DBG_SHARD_HISTORY
				owner->LogEvent("RCS Download Request t="+String(distributionTimestep)+""+source->gridCoords.ToString()+"->" +owner->gridCoords.ToString()+" g"+String(generation)+" "+dbgOut);
			#endif
		}
		#ifdef DBG_SHARD_HISTORY
			elif (rs == Result::Downloaded)
			{
				owner->LogEvent("RCS Downloaded t="+String(distributionTimestep)+": "+source->gridCoords.ToString()+"->" +owner->gridCoords.ToString()+" g"+String(generation)+" "+dbgOut);
			}
		#endif
		return rs;
	}


	Result		Client::Download(const RCS_ID&id, RCS&target)
	{
		ASSERT__(active);
		#ifdef DBG_SHARD_HISTORY
			String dbgOut;
		#endif
		Result rs= _Download(this,id,myRCSChannel,target DB_DBG_FORWARD);

		if (rs == Result::Requested)
		{
			GetGlobal(owner->gridCoords).RequestRCS(this,id.origin,id.generation DB_DBG_FORWARD);
			#ifdef DBG_SHARD_HISTORY
				owner->LogEvent("RCS Download Request t="+String(distributionTimestep)+""+id.ToString()+" "+dbgOut);
			#endif
		}
		#ifdef DBG_SHARD_HISTORY
			elif (rs == Result::Downloaded)
			{
				owner->LogEvent("RCS Downloaded t="+String(distributionTimestep)+": "+id.ToString()+" "+dbgOut);
			}
		#endif
		return rs;
	}

	Client::Client(Shard*owner):owner(owner)
	{
		clientListLock.lock();
			clientList << this;
		clientListLock.unlock();
	}

	Client::~Client()
	{
		if (!application_shutting_down)
		{
			clientListLock.lock();
				clientList.FindAndErase(this);
			clientListLock.unlock();
		}
	}

	
	template <typename ID, typename T>
	void		_Upload(Client*sender, const ID&id, const T&element, ClientChannel<ID, T>&clChannel)
	{
		if (locked)
			return;
		if (_IsUploading(sender,id,clChannel))
			return;

		ASSERT__(element->ic.IsSealedAndConsistent());


		clChannel.uploadLock.lock();
		clChannel.uploadList.Append().remainingRounds = GetUploadTime(element->ic);
		clChannel.uploadList.Last().id = id;
		clChannel.uploadList.Last().data = element;
		clChannel.uploadLock.unlock();
	}

	bool			Client::Upload(const SDS&sds, bool instantly/*=false*/)
	{
		ASSERT__(active);
		ASSERT__(sds);
		ASSERT__(sds->IsFullyConsistent());
		sds->ic.Seal();
		if (sds->generation < minimumGeneration)
		{
			ASSERT__(!instantly);
			return false;
		}
		if (instantly)
		{
			UpdateMinimumGeneration(sds->generation+1,CLOCATION);
			GetGlobal(owner->gridCoords).Update(sds);
		}
		else
		{
			const SDS_ID id = ID(*owner,sds);
			#ifdef DBG_SHARD_HISTORY
				owner->LogEvent("Upload SDS "+id.ToString());
			#endif
			_Upload(this,id, sds,mySDSChannel);
		}
		return true;
	}
	bool			Client::Upload(const RCS_ID&id, const RCS&rcs, bool instantly/*=false*/)
	{
		ASSERT__(active);
		ASSERT__(rcs);
		ASSERT__(rcs->ic.IsFullyConsistent());
		rcs->ic.Seal();
		if (instantly)
		{
			auto&global =GetGlobal(owner->gridCoords);
			if (id.generation > global.GetSDSGeneration())
			{
				#ifdef DBG_SHARD_HISTORY
					owner->LogEvent("Upload RCS "+id.ToString());
				#endif
				global.GetStack(id.origin).Insert(id,rcs);
			}
			#ifdef DBG_SHARD_HISTORY
				else
					owner->LogEvent("Reject RCS Upload "+id.ToString()+" ("+String(global.GetSDSGeneration()));
			#endif
		}
		else
			_Upload(this,id, rcs,myRCSChannel);
		return true;
	}


	void	Upload(const SDS_ID&id,const SDS&sds)
	{
		GetGlobal(id.destination).Update(sds);
	}
	void	Upload(const RCS_ID&id,const RCS&rcs)
	{
		auto&gl = GetGlobal(id.destination);
		if (id.generation > gl.GetSDSGeneration())
			gl.GetStack(id.origin).Insert(id,rcs);
	}

	template <typename ID, typename T>
		void _Process(Client*client, ClientChannel<ID,T>&clChannel,bool inclusive)
		{
			for (index_t i = 0; i < clChannel.uploadList.Count(); i++)
			{
				auto&up = clChannel.uploadList[i];
				ASSERT__(up.data->ic.IsSealedAndConsistent());
				if (up.remainingRounds)
					up.remainingRounds --;
				if (!up.remainingRounds)
				{
					const index_t newestGen = NewestKnownSDSGeneration(up.id.destination);
					if (up.id.generation > newestGen)
					{
						Upload(up.id,up.data);
					}
					else
					{
//						coordMap.Update(up.id.destination,newestGen);
					}
					clChannel.uploadList.Erase(i);
					i--;
				}
			}



			index_t min = client->GetMinimumGeneration();
			//if (!inclusive)
			//	min++;
			clChannel.cache.FilterKeys([inclusive,client,min](const ID&id)
			{
					//coordMap.NewestKnownSDSGeneration(id.destination);
				if (id.generation < min)
				{
					#ifdef DBG_SHARD_HISTORY
						client->owner->LogEvent("Curbed from cache: "+id.ToString()+", min="+String(min));
					#endif
					return false;
				}
				return true;
			});
		}



	void			Client::ProcessUploads()
	{
		_Process(this,mySDSChannel,true);
		_Process(this,myRCSChannel,false);
	}

	void			Client::UpdateCoordMap()
	{
		const index_t globalState = GetGlobal(owner->gridCoords).GetSDSGenerationNoLock();
		if (globalState > MapTransferDelay)
		{
			serverGeneration = globalState - MapTransferDelay;
		}
		else
			serverGeneration = 0;

	}

	SDS_ID			ID(const Shard&sender, const SDS&sds)
	{
		SDS_ID rs;
		rs.generation = sds->generation;
		rs.destination = sender.gridCoords;
		return rs;
	}

	SDS_ID			ID(const Shard&sender, index_t gen)
	{
		SDS_ID rs;
		rs.generation = gen;
		rs.destination = sender.gridCoords;
		return rs;
	}

	RCS_ID			ID(const Shard*source, const Shard*destination, index_t generation)
	{
		RCS_ID rs;
		rs.generation = generation;
		rs.destination = destination->gridCoords;
		rs.origin = source->gridCoords;
		return rs;
	}


	void		Client::SignalDownload(const RCS_ID&id, const RCS&data, index_t uploadedAt, index_t requestedAt)
	{
		ASSERT__(active);
		ASSERT__(!locked);
		const index_t minGen = GetMinimumGeneration();
		if (id.generation < minGen)
		{
			#ifdef DBG_SHARD_HISTORY
				owner->LogEvent("RCS delivery t="+String(distributionTimestep)+" "+id.ToString()+" rejected. minGen="+String(minGen));
			#endif
			return;
		}
		myRCSChannel.downloadLock.lock();
		auto&entry = myRCSChannel.cache.Set(id);
		entry.data = data;
		entry.downloadedAt = distributionTimestep;
		entry.uploadedAt = uploadedAt;
		entry.requestedAt = requestedAt;
		#ifdef DBG_SHARD_HISTORY
			owner->LogEvent("RCS delivery t="+String(distributionTimestep)+" "+id.ToString());
		#endif
		myRCSChannel.downloadLock.unlock();
		ASSERT__(!IsUploading(id));
		ASSERT__(data->ic.IsSealedAndConsistent());
	}


	void		Client::SignalDownload(const SDS_ID&id, const SDS&data, index_t uploadedAt, index_t requestedAt)
	{
		ASSERT__(active);
		ASSERT__(!locked);

		//coordMap.Update(id.destination,id.generation);
		const index_t minGen = GetMinimumGeneration();
		if (id.generation < minGen)
		{
			#ifdef DBG_SHARD_HISTORY
				owner->LogEvent("SDS delivery t="+String(distributionTimestep)+" "+id.ToString()+" rejected. minGen="+String(minGen));
			#endif
			return;
		}
		mySDSChannel.downloadLock.lock();
		auto&entry = mySDSChannel.cache.Set(id);
		entry.data = data;
		entry.downloadedAt = distributionTimestep;
		entry.uploadedAt = uploadedAt;
		entry.requestedAt = requestedAt;
		#ifdef DBG_SHARD_HISTORY
			owner->LogEvent("SDS delivery t="+String(distributionTimestep)+" "+id.ToString());
		#endif
		mySDSChannel.downloadLock.unlock();
		//ASSERT__(!IsUploading(id));
		ASSERT__(data->ic.IsSealedAndConsistent());
	}



	void			Client::SignalShutdown()
	{
		ASSERT__(!locked);

		#ifdef DBG_SHARD_HISTORY
			owner->LogEvent("DB shutdown");
		#endif

		generation++;

		active = false;
		myRCSChannel.Clear();
		mySDSChannel.Clear();
		minimumGeneration = 0;
		serverGeneration = 0;
	}

	void			Client::SignalStartup()
	{
		#ifdef DBG_SHARD_HISTORY
			owner->LogEvent("DB startup");
		#endif
		active = true;
		generation++;
	}

	void Client::UpdateMinimumGeneration(index_t gen, const TCodeLocation&cause)
	{
		auto old = minimumGeneration;
		minimumGeneration = M::Max(minimumGeneration,gen);
		#ifdef DBG_SHARD_HISTORY
			if (old != minimumGeneration)
				owner->LogEvent("Updated min generation: "+String(old)+"->"+String(minimumGeneration)+ " called by "+String(cause));
		#endif
	}



	void			ProcessDistribution()
	{
		if (locked)
			return;
		distributionTimestep++;

		StoreParallel([](ShardState&st)
		{
			st.DistributeSDSNoLock();
		});

		const count_t numStacks = ShardState::RCSStackCount * globalStore.Count();
		Concurrency::parallel_for(index_t(0),numStacks,[](index_t i)
		{
			const index_t shardIndex = i / ShardState::RCSStackCount;
			const index_t stackIndex = i % ShardState::RCSStackCount;
			globalStore[shardIndex].DistributeRCSNoLock(stackIndex);
		});


		clientListLock.lock();
			//Concurrency::parallel_for(index_t(0),clientList.Count(), [](index_t i)
			for (index_t i = 0; i < clientList.Count(); i++)
			{
				clientList[i]->ProcessUploads();
			}
			//);
//			Concurrency::parallel_for(index_t(0),clientList.Count(), [](index_t i)
			for (index_t i = 0; i < clientList.Count(); i++)
			{
				clientList[i]->UpdateCoordMap();
			}
			//);
		clientListLock.unlock();
	}

	void AdvanceTimestep(index_t nextTimestep)
	{
		currentTimestep = nextTimestep;

		#ifdef DBG_SHARD_HISTORY
			clientListLock.lock();
				//Concurrency::parallel_for(index_t(0),clientList.Count(), [](index_t i)
				for (index_t i = 0; i < clientList.Count(); i++)
				{
					clientList[i]->owner->LogEvent("Timestep ->"+String(nextTimestep));
				}
			clientListLock.unlock();
		#endif
	}


	void Lock()
	{
		locked = true;
	}

	void Unlock()
	{
		locked = false;
	}

	void			Reset(const GridSize&size)
	{
		ASSERT__(!locked);
		globalStore.Clear();	//make sure nothing is left over
		globalStore.SetSize(size);
		#ifdef D3
			for (index_t x = 0; x < size.width; x++)
			for (index_t y = 0; y < size.height; y++)
			for (index_t z = 0; z < size.depth; z++)
				globalStore.Get(x,y,z).Setup(TGridCoords((int)x,(int)y,(int)z));
		#else
			for (index_t x = 0; x < size.width; x++)
			for (index_t y = 0; y < size.height; y++)
				globalStore.Get(x,y).Setup(TGridCoords((int)x,(int)y));
		#endif

		distributionTimestep = 0;
		locked = false;
	}


	void AssertIsWellDefined()
	{
		foreach (globalStore,st)
		{
			ASSERT__(st->Exists());

		}
	}

	bool	IsLocked()
	{
		return locked;
	}

	index_t	NewestKnownSDSGeneration(const TGridCoords&c)
	{
		return GetGlobal(c).GetSDSGeneration();
	}
}

