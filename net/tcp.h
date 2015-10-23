#ifndef tcpH
#define tcpH

#include "node.h"
#include "../container/buffer.h"
#include "../io/byte_stream.h"
#include "../container/production_pipe.h"

#include <thread>
#include <sstream>
#include <mutex>

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#undef StartService

/**
@brief TCP/IP related constructs
*/
namespace TCP
{
	#if SYSTEM==WINDOWS
		typedef int	socklen_t;
	#endif


	#undef TCP_TRY
	#undef TCP_CATCH

	#define TCP_TRY  try
	#define TCP_CATCH\
				catch (const std::exception&ex)\
				{\
					::TCP::onIgnoredException(CLOCATION,ex);\
				}\
				catch (...)\
				{\
					::TCP::onIgnoredArbitraryException(CLOCATION);\
				}



	extern std::function<void(const TCodeLocation&,const std::exception&)>	onIgnoredException;
	extern std::function<void(const TCodeLocation&)>	onIgnoredArbitraryException;


	void	SwapCloseSocket(volatile SOCKET&socket);
	String	ToString(const addrinfo&address);
	String	ToString(const sockaddr_storage&address);
	
	typedef std::shared_ptr<SerializableObject>	PSerializableObject;


	/**
	Special mutex for events that allows to completely kick out threads before they enter a critical section
	*/
	class EventLock
	{
		struct TimedLock
		{
			std::recursive_timed_mutex	mutex;
			TCodeLocation		lastLockedBy;

			bool			TryLock(const TCodeLocation&locker)
			{
				#ifdef _DEBUG
					static const size_t toleranceMS = 30000;
				#else
					static const size_t toleranceMS = 1000;
				#endif
				if (!mutex.try_lock_for(std::chrono::milliseconds(toleranceMS)))
				{
					FATAL__(String(locker.ToString())+": Failed to lock thread after "+String(toleranceMS)+"ms. Deadlock assumed. Last locker was "+lastLockedBy.ToString());
					return false;
				}
				lastLockedBy = locker;
				return true;
			}

			void			Unlock()
			{
				mutex.unlock();
			}

		};


		TimedLock			innerLock,permissiveLock;
		volatile count_t	blockLevel,permissiveCount;
		



	public:
		/**/		EventLock():blockLevel(0)	{}


		void		Block(const TCodeLocation&locker)
		{
			if (!innerLock.TryLock(locker))
				return;
			blockLevel++;
			innerLock.Unlock();
		}

		bool		Unblock(const TCodeLocation&locker)
		{
			if (!innerLock.TryLock(locker))
				return false;
			ASSERT__(blockLevel > 0);
			blockLevel--;
			bool rs = blockLevel == 0;
			innerLock.Unlock();
			return rs;
		}

		bool		PermissiveLock(const TCodeLocation&locker)
		{
			if (!permissiveLock.TryLock(locker))
				return false;
			if (permissiveCount == 0)
			{
				if (!innerLock.TryLock(locker))
				{
					permissiveLock.Unlock();
					return false;
				}
				if (blockLevel != 0)
				{
					innerLock.Unlock();
					permissiveLock.Unlock();
					return false;
				}
			}
			permissiveCount++;
			permissiveLock.Unlock();
			return true;
		}

		void		PermissiveUnlock(const TCodeLocation&locker)
		{
			if (!permissiveLock.TryLock(locker))
				return;
			ASSERT__(permissiveCount > 0);
			permissiveCount--;
			if (permissiveCount == 0)
			{
				innerLock.Unlock();
			}
			permissiveLock.Unlock();
		}

		bool		ExclusiveLock(const TCodeLocation&loc)
		{
			if (!innerLock.TryLock(loc))
				return false;
			if (blockLevel != 0)
			{
				innerLock.Unlock();
				return false;
			}
			return true;
		}


		void		ExclusiveUnlock()
		{
			innerLock.Unlock();
		}


	};


	class SocketAccess
	{
	public:
		virtual					~SocketAccess()	{};
		virtual void			SetSocket(SOCKET socket) = 0;
		virtual void			CloseSocket() = 0;
		virtual bool			IsClosed() const = 0;
		virtual SocketAccess*	CloneClass() const = 0;

		virtual	int				Read(void*data, size_t dataSize) = 0;
		virtual int				Write(const void*data, size_t dataSize) = 0;
	};


	class Exception : public std::exception
	{
	public:
		String message;
		TCodeLocation loc;


		Exception(const TCodeLocation&loc):loc(loc)	{}
		Exception(const TCodeLocation&loc,const String&message):loc(loc),message(message)	{}



		const char* what() const override {return (String(loc.ToString())+": "+message).c_str();}
	};


	class DefaultSocketAccess : public SocketAccess
	{
	public:
		SOCKET			socketHandle;

		/**/			DefaultSocketAccess():socketHandle(INVALID_SOCKET){}

		virtual SocketAccess*	CloneClass() const
		{
			return new DefaultSocketAccess();
		}

		virtual bool	IsClosed() const override
		{
			return socketHandle == INVALID_SOCKET;
		}

		virtual void	CloseSocket() override
		{
			SwapCloseSocket(socketHandle);
		}
		
		virtual void	SetSocket(SOCKET socket)	override
		{
			socketHandle = socket;
		}

		virtual	int		Read(void*data, size_t dataSize) override
		{
			return recv(socketHandle,(char*)data,(int)dataSize,0);
		}

		virtual int		Write(const void*data, size_t dataSize) override
		{
			const char* at = (const char*)data,
					* end = at + dataSize;
			while (at < end)
			{
				#if 0
					//this works ... a little too well:
					timeval tv;
					tv.tv_sec = 0;
					tv.tv_usec = 100000;
					FD_SET set;
					FD_ZERO(&set);
					FD_SET(socketHandle,&set);
					select(1,nullptr,&set,nullptr,&tv);
					if (!FD_ISSET(socketHandle,&set))
					{
						return -1;
					}
				#endif /*0*/

				int sent = send(socketHandle,at,end-at,0);
				if (sent <= 0)
					return sent;
				at += sent;
			}
			return (int)dataSize;
		}
	};


	extern bool			verbose;	//!< Set true to automatically print events to the console. false by default

	namespace User	//! User level definitions for use by the client application
	{
		enum user_t	//! User level definitions for use by the client application
		{
			Anonymous,		//!< Non-authenticated user
			Authenticated,	//!< General authenticated user
			Privileged,		//!< Administrator grade authenticated user
			Root,			//!< Authenticated super user
			
			Any=Anonymous
		};
	}
	
	typedef User::user_t	user_t;


	class Peer;
	typedef std::shared_ptr<Peer>	PPeer;
	typedef std::weak_ptr<Peer>	WPeer;

	struct TDualLink
	{
		Peer					*p;	//cannot avoid pointer sometimes
		WPeer					s;

		/**/					TDualLink():p(NULL){}
		explicit				TDualLink(Peer*_p):p(_p)	{}
		explicit				TDualLink(const PPeer&_s):p(NULL),s(_s)	{}

		//Peer*					Reference() const
		//{
		//	PPeer pp = s.lock();
		//	if (pp)
		//		re
		//	DBG_ASSERT__(s || p);
		//	return s ? *s : *p;
		//}
	};


	/**
		@brief Size constrained serializable
		
		This class imposes a maximum size to the underlying serializable class.
	*/
	template <class Serializable, serial_size_t MaxSize>
		class ConstrainedObject:public Serializable
		{
		public:
				/**
					@brief Deserializes data from an abstract stream source into the local object as defined by the abstract ISerializable interface.
				*/
			virtual	bool	Deserialize(IReadStream&stream, serial_size_t fixed_size)	override
							{
								if (fixed_size > MaxSize)
									return false;
								return Serializable::Deserialize(stream,fixed_size);
							}
		};
	
	

	template <typename T>
		class Serial
		{
		private:
			bool		sealed;
			T			element;
			Array<BYTE>	serialized;
		public:
			/**/		Serial():sealed(false)
			{}

			T&			Ref()
			{
				ASSERT__(!sealed);
				return element;
			}

			void		Seal()
			{
				if (sealed)
					return;
				sealed = true;
				serialized.SetSize(element.GetSerialSize(false));
				ASSERT__(SerializeToMemory(element,serialized.pointer(),(serial_size_t)serialized.GetContentSize(),false) != 0);
			}

			const Array<BYTE>&	Get() const
			{
				ASSERT__(sealed);
				return serialized;
			}

		};


	/**
	@brief Signal serializable that does not hold any actual data.
		
	VoidSerializable is always of size 0 and ommits any decoding or encoding but returns success upon doing so
	*/
	class VoidSerializable:public SerializableObject
	{
	public:
		virtual	serial_size_t	GetSerialSize(bool) const	override	{return 0;};
		virtual	bool			Serialize(IWriteStream&stream,bool) const		override	{return true;}
		virtual	bool			Deserialize(IReadStream&stream,serial_size_t)	override	{return true;}
	};
	
	/**
		@brief Abstract data destination
		
		Peer (client) or Server (all clients) are of this type and implement the abstract sendObject() method
	*/
	class Destination
	{
	public:
	virtual					~Destination()	{};
			/**
				@brief Abstract data send method
				
				sendObject() sends a serializable data structure on the specified channel to the remote destination described by the local structure
				@param channel Channel to send the data on
				@param serializable Data to send on the specified channel
				@return true on success, false otherwise
			*/
	virtual	bool			SendObject(UINT32 channel, const ISerializable&serializable, unsigned minUserLevel)=0;
	virtual	bool			SendObject(UINT32 channel, const PPeer&exclude, const ISerializable&serializable, unsigned minUserLevel)	{return SendObject(channel,serializable,minUserLevel);};
	};
	typedef std::shared_ptr<Destination>	PDestination;
	/**
		@brief Abstract channel installation to handle outgoing and incoming packages
		
		This structure is for inheritance only and does not provide public members.
	*/
	class RootChannel 
	{
	protected:
		const UINT32		id;			//!< Channel id. Unmutable
		friend class 		Peer;
		friend class 		Dispatcher;
		Mutex				sendMutex;	//!< Mutex used to shield outgoing serializations to the tcp socket
			
							RootChannel(UINT32 id_):id(id_)
							{}
			
		/**
		@brief Sends an object to the specified destination
		*/
		bool				SendObject(Destination&destination, const ISerializable&object, unsigned minUserLevel=0);
		bool				SendObject(Destination&destination, const PPeer&exclude, const ISerializable&object, unsigned minUserLevel=0);
							
		/**
		@brief Deserializes the stream into a new object

		This method is called asynchronously by the respective connection's worker thread.
		*/
		virtual	PSerializableObject	Deserialize(IReadStream&stream,serial_size_t fixed_size,Peer&sender)
		{
			return PSerializableObject();
		}
		/**
		@brief Dispatches a successfully deserialized object (returned by Deserialize() )

		The method is invoked only, if Deserialize() returned a new non-NULL object.
		Depending on whether the connection is synchronous or asynchronous, this method is called my the main thread, or the connection's worker thread.
		*/
		virtual	void				Handle(const PSerializableObject&serializable,const TDualLink&sender)	{FATAL__("Channel failed to override handle()");};
	public:
	};


	/**
		@brief Dedicated send-only channel
	*/
	template <class SerializableObject, unsigned Channel>
		class ObjectSender:public RootChannel
		{
		protected:
			virtual	PSerializableObject	Deserialize(IReadStream&stream,serial_size_t fixed_size,Peer&sender)	override
			{
				return PSerializableObject();
			}
		public:
			/**/					ObjectSender():RootChannel(Channel)
									{}
			bool					SendTo(Destination&destination, const SerializableObject&object, unsigned minUserLevel=0)
			{
				return SendObject(destination,object,minUserLevel);
			}
			bool					SendTo(const PDestination&destination, const SerializableObject&object, unsigned minUserLevel=0)
			{
				if (!destination)
					return false;
				return SendObject(*destination,object,minUserLevel);
			}
			/**
			@brief Special overload form of sendTo for servers only
					
			Allows to send objects to all clients except a specific client
					
			@param destination Transfer destination. The method will fail if @a destination is NULL
			@param exclude Peer to specifically exclude
			@param object Serializable object to send
			@return true on success, false otherwise. Note that a positive result only indicates that the object was successfuly serialized to the tcp stream. It does not mean that the package was received or accepted.
			*/
			bool				SendTo(const PDestination&destination, const PPeer&exclude, const SerializableObject&object, unsigned minUserLevel=0)
			{
				if (!destination)
					return false;
				return SendObject(*destination,exclude,object,minUserLevel);
			}
			bool				SendTo(Destination&destination, const PPeer&exclude, const SerializableObject&object, unsigned minUserLevel=0)
			{
				return SendObject(destination,exclude,object);
			}


			bool					SendTo(Destination&destination, const Serial<SerializableObject>&object, unsigned minUserLevel=0)
			{
				return SendObject(destination,object.Get(),minUserLevel);
			}
			bool					SendTo(const PDestination&destination, const Serial<SerializableObject>&object, unsigned minUserLevel=0)
			{
				if (!destination)
					return false;
				return SendObject(*destination,object.Get(),minUserLevel);
			}
			/**
			@brief Special overload form of sendTo for servers only
					
			Allows to send objects to all clients except a specific client
					
			@param destination Transfer destination. The method will fail if @a destination is NULL
			@param exclude Peer to specifically exclude
			@param object Serializable object to send
			@return true on success, false otherwise. Note that a positive result only indicates that the object was successfuly serialized to the tcp stream. It does not mean that the package was received or accepted.
			*/
			bool				SendTo(const PDestination&destination, const PPeer&exclude, const Serial<SerializableObject>&object, unsigned minUserLevel=0)
			{
				if (!destination)
					return false;
				return SendObject(*destination,exclude,object.Get(),minUserLevel);
			}
			bool				SendTo(Destination&destination, const PPeer&exclude, const Serial<SerializableObject>&object, unsigned minUserLevel=0)
			{
				return SendObject(destination,exclude,object.Get(),minUserLevel);
			}
												
		};
	
	/**
		@brief Dedicated send-only data-less channel
	*/
	template <unsigned ChannelID>
		class SignalSender:public RootChannel
		{
		protected:
			virtual	PSerializableObject	Deserialize(IReadStream&stream,serial_size_t fixed_size,const PPeer&sender)	override
			{
				return PSerializableObject();
			}
		public:
			/**/				SignalSender():RootChannel(ChannelID)
								{}
			/**
			@brief Sends a signal (data-less package) to the specified destination
			*/
			bool				SendTo(Destination&destination, unsigned minUserLevel=0)
			{
				VoidSerializable object;
				return SendObject(destination,object,minUserLevel);
			}
			bool				SendTo(const PDestination&destination, unsigned minUserLevel=0)
			{
				if (!destination)
					return false;
				VoidSerializable object;
				return SendObject(*destination,object,minUserLevel);
			}
			/**
			@brief Special overload form of sendTo for servers only
					
			Allows to send objects to all clients except a specific client
					
			@param destination Transfer destination. The method will fail if @a destination is NULL
			@param exclude Peer to specifically exclude
			@param object Serializable object to send
			@return true on success, false otherwise. Note that a positive result only indicates that the object was successfuly serialized to the tcp stream. It does not mean that the package was received or accepted.
			*/
			bool				SendTo(const PDestination&destination, const PPeer&exclude, unsigned minUserLevel=0)
			{
				if (!destination)
					return false;
				VoidSerializable object;
				return SendObject(*destination,exclude,object,minUserlevel);
			}
			bool				SendTo(Destination&destination, const PPeer&exclude, unsigned minUserLevel=0)
			{
				VoidSerializable object;
				return SendObject(destination,exclude,object,minUserlevel);
			}
												
		};



	namespace Event	//! Socket event
	{
		#undef None
		enum event_t	//! Socket event
		{
			None,					//!< Void
			ConnectionFailed,		//!< The local connection attempt has failed
			ConnectionEstablished,	//!< The local connection attempt has succeeded
			ConnectionClosed,		//!< The local connection has been closed
			ConnectionLost			//!< Connection closed unexpectedly
		};
	}
	
	typedef Event::event_t	event_t;
	
	const char*	event2str(event_t);	//!< Retrieves a string representation of the specified event (for logging or other debug output)
	
	/**
		@brief Package handler
		
		The dispatcher handles incoming objects and events. Objects and events are enqueued if @b async is false. Otherwise they are directly executed and deleted if appropriate.
		If @b async is false then the client application must invoke resolve() from time to time to handle incoming events and objects
	*/
	class Dispatcher
	{
	protected:

		//struct TInbound
		//{
		//	PSerializableObject		object;
		//	RootChannel				*receiver;
		//	TDualLink				sender;
		//};
		//	
		//struct TEvent
		//{
		//	event_t					event;
		//	TDualLink				sender;
		//};
		//	
		//struct TSignal
		//{
		//	UINT32					channel;
		//	TDualLink				sender;
		//};

		struct TCommonEvent
		{
			enum type_t
			{
				NetworkEvent,
				Signal,
				Object
			};

			type_t					type;

			event_t					event;	//only if type==NetworkEvent

			UINT32					channel;	//only if type==Signal

			PSerializableObject		object;		//only if type==Object
			RootChannel				*receiver;	//only if type==Object
			
			TDualLink				sender;
		};

			
		//Queue<TInbound>				object_queue;
		//Queue<TEvent>				event_queue;
		//Queue<TSignal>				signal_queue;
		Queue<TCommonEvent>			queue;


		Buffer0<PPeer>				wasteBucket;

		EventLock					eventLock;

		Mutex						wasteMutex;
		bool						async;
		volatile bool				is_locked;
		//volatile unsigned			block_events;
		IndexTable<RootChannel*>	channel_map;
		IndexTable<unsigned>		signal_map;
			
		friend class Peer;
			
		void						HandlePeerDeletion(const PPeer&);
		void						HandleSignal(UINT32 signal, const TDualLink&sender);
		void						HandleObject(RootChannel*receiver, const TDualLink&sender, const PSerializableObject&object);
		void						HandleEvent(event_t event, const TDualLink&sender);
		RootChannel*				getReceiver(UINT32 channel_id, unsigned user_level);
			
		void						FlushWaste();
	public:
		void (*onEvent)(event_t event, Peer&origin);	//!< Callback hook for events (connection, disconnection, etc). NULL by default. @param event Event that occured @param origin Event origin
		void (*onSignal)(UINT32 signal, Peer&origin);	//!< Callback hook for signals (data-less packages). NULL by default.  @param signal Channel that the data-less package was received on @param origin Origin of the signal
		void (*onIgnorePackage)(UINT32 channel,UINT32 size,Peer&origin);	//!< Callback hook for ignored packages. Always async. NULL by default. @param channel Channel the package or signal was received on @param size Package size in bytes @param origin Receiving peer
		void (*onDeserializationFailed)(UINT32 channel, Peer&origin);	//!< Callback hook for deserialization failures. NULL by default. If not set, such errors cause a fatal exception in debug mode, and are ignored in release mode.
	
		/**/						Dispatcher();
		virtual						~Dispatcher();
		void						Resolve();		//!< Resolves all events that occured since the last Resolve() invokation. The client application must call this method frequently if the local dispatcher is set to synchronous. Never otherwise.
		void						FlushPendingEvents();	//!< Dumps all remaining, pending events without executing them. The client application may call this method if the local dispatcher is set to synchronous. Nothing happens otherwise.
		void						InstallChannel(RootChannel&channel);	//!< Installs a channel in/out processor. If the used channel id is already in use then it will be overwritten with the provided channel handler
		void						UninstallChannel(RootChannel&channel);		//!< Uninstalls a channel in/out processor. The used channel id will be closed
		void						OpenSignalChannel(UINT32 id, unsigned minUserLevel);	//!< Opens a signal channel for receiving	@param id Channel id to listen for signals on @param min_user_level Minimum required user level for this signal to be accepted
		void						CloseSignalChannel(UINT32 id);							//!< Closes a signal channel
			
		void						SetAsync(bool is_async)		//! Changes the socket asynchronous status. Dispatchers run in async mode by default. @param is_async Set true if the dispatcher should perform operations asynchronously from this point on, false otherwise
									{
										async = is_async;
									}
		bool						IsAsync()	const			//! Queries whether or not this dispatcher is currently asynchronously. Dispatchers run in async mode by default.
									{
										return async;
									}
	};
	
	
	/**
	@brief Abstract connection prototype
		
	A connection represents the largest most abstract network structure. Only server/client objects qualify as Connection derivatives.
	*/
	class Connection:public Dispatcher
	{
	private:
		String						last_error,
									current_error;
		Mutex						error_mutex;
		friend class Peer;
	protected:
		serial_size_t				safe_package_size;	//!< Maximum allowed package size. ~64MB by default
			
									Connection():safe_package_size(64000000)
									{}
			
		void						setError(const String&message)	//!< Updates the local error string. The local structure maintains two string objects to reduce the risk of invalid pointer access
									{
										error_mutex.lock();
											last_error.adoptData(current_error);
											current_error = message;
										error_mutex.release();
									}
	public:
		virtual	void				OnDisconnect(const Peer*, event_t event)	{};	//!< Abstract disconnection even called if the local connection has been lost or closed
		String						GetError(bool flushError)	//! Queries the last occured error
									{
										error_mutex.lock();
											String copy = current_error;
											if (flushError)
												current_error = "";
										error_mutex.release();
										return copy;
									}
	};

	typedef std::shared_ptr<Connection>	PConnection;


	class TCPThreadObject
	{
	private:
		std::thread			thread;
		volatile bool		done = true;
		Semaphore			sem;
		SpinLock			joinLock;


		static void			_Start(TCPThreadObject*self, Semaphore*sem)
		{
			sem->enter();
			self->ThreadMain();
			self->done = true;
		}
	public:



		virtual void		ThreadMain()=0;



		void				Start()
		{
			done = false;
			//Semaphore			sem;	//perhaps better not to have this in stack
			thread.swap(std::thread(_Start,this,&sem));
			sem.leave();
		}

		bool				IsDone() const
		{
			return done;
		}

		bool				IsRunning() const
		{
			return thread.joinable();
		}

		bool				IsSelf() const
		{
			return std::this_thread::get_id() == thread.get_id();
		}

		void				AssertIsSelf()
		{
			std::thread::id self = std::this_thread::get_id();
			std::thread::id local = thread.get_id();
			std::stringstream ss;
			ss << self<<"!="<<local;
			//string mystring = ;
			ASSERT1__(self == local,ss.str().c_str());
		
		}

		void				Join()
		{
			if (IsSelf())
				return;
			joinLock.lock();
			TCP_TRY
			{
				if (thread.joinable())
					thread.join();
				thread.swap(std::thread());
			}
			TCP_CATCH
			joinLock.unlock();
		}
	};



	class PeerWriter : public TCPThreadObject
	{
		WorkPipe<Array<BYTE> >		pipe;
		SocketAccess				*volatile accessPointer;
		SpinLock					accessPointerLock;
	public:
		volatile bool				connectionLost;
		Peer						*const parent;
		
		/**/						PeerWriter(Peer*parent):accessPointer(nullptr),connectionLost(true),parent(parent){}

		void						Begin(SocketAccess*access);
		void						Update(SocketAccess*access);
		void						Terminate();

		void						ThreadMain() override;			//!< Socket write thread main


		bool						Write(UINT32 channel, const ISerializable&s)
		{
			if (connectionLost)
				return false;
			serial_size_t size = s.GetSerialSize(false);
			Array<BYTE>	data(8 + size);
			(*(UINT32*)data.pointer()) = channel;
			(*(((UINT32*)data.pointer())+1)) = (UINT32)size;

			serial_size_t actual;
			SerializeToMemory(s,data.pointer()+8,size,false,&actual);
			ASSERT_EQUAL__(actual,size);
			pipe.MoveAppend(data);
			return true;
		}
		bool						Write(UINT32 channel, const void*rawData, size_t numBytes)
		{
			if (connectionLost)
				return false;
			Array<BYTE>	data(8 + numBytes);
			(*(UINT32*)data.pointer()) = channel;
			(*(((UINT32*)data.pointer())+1)) = (UINT32)numBytes;
			memcpy(data.pointer()+8,rawData,numBytes);
			pipe.MoveAppend(data);
			return true;
		}

		bool						Write(const Array<BYTE>&packet)
		{
			if (connectionLost)
				return false;
			pipe << packet;
			return true;
		}

		bool						WriteSignal(UINT32 signal)
		{
			if (connectionLost)
				return false;
			Array<BYTE>	data(8);
			(*(UINT32*)data.pointer()) = signal;
			(*(((UINT32*)data.pointer())+1)) = (UINT32)0;
			pipe.MoveAppend(data);
			return true;
		}

	};

	
	/**
	@brief TCP socket handler
		
	A peer handles incoming and outgoing packages from/to a specific IP address over a specific socket handle. A server would manage one peer structure per client, a client would inherit from Peer.
	
	*/
	class Peer : public TCPThreadObject, protected IReadStream, /*protected IWriteStream, */public Destination //, public IToString
	{
	protected:
		PeerWriter					writer;
		Connection					*owner;			//!< Pointer to the owning connection to handle incoming packages and report errors to
		Mutex						//write_mutex,	//!< Mutex to synchronize socket write operations
									counter_mutex;	//!< Currently not in use
		//volatile SOCKET				socket_handle;	//!< Handle to the occupied socket
		serial_size_t				remaining_size,			//!< Size remaining for reading streams 
									remaining_write_size;	//!< Size remaining for writing streams in the announced memory frame
		ByteStream					serial_buffer;		//!< Buffer used to serialize package data
		SocketAccess				*socketAccess;
		std::weak_ptr<Peer>			self;
		const bool					canDoSharedFromThis;
		friend class Server;
		friend class RootChannel;
		friend class PeerWriter;
			
		void						ThreadMain() override;			//!< Socket read thread main
		bool						sendData(UINT32 channel_id, const void*data, size_t size);	//!< Sends raw data to the TCP stream. Used by server
		bool						succeeded(int result, size_t desired);							//!< Handles the result of a TCP socket send operation. @param result Actual value returned by send() @param desired Valued expected to be returned by send() @return true if both values match, false otherwise. The connection is automatically closed, events triggered and error values set if the operation failed.
		void						handleUnexpectedSendResult(int result);
		bool						Read(void*target, serial_size_t size) override;								//!< IInStream override for direct TCP stream input
		//bool						Write(const void*target, serial_size_t size) override;						//!< IOutStream override for direct TCP stream output
		serial_size_t				GetRemainingBytes() const override;
		bool						netRead(BYTE*current, size_t size);							//!< Continuously reads a sequence of bytes from the TCP stream. The method does not return until either the requested amount of bytes was received or an error occured
			
		TDualLink					LinkFromThis()
		{
			PPeer p = self.lock();
			if (p)
				return TDualLink(p);
			return TDualLink(this);
		}
		//{
		//	try
		//	{
		//		return canDoSharedFromThis ? shared_from_this() : PPeer();
		//	}
		//	catch (...)
		//	{
		//		return PPeer();	//weak ptr exception
		//	}
		//}

	public:
		/**
			@brief Abstract peer attachment class
		*/
		class Attachment
		{
		public:
		virtual						~Attachment()	{};
		};
		/**
			@brief Peer attachment
				
			Useful to attach client data to a peer. If @a delete_attachment_on_disconnect is set true then the attached data structure will be erased automatically before this peer object is deleted.
				
		*/
		std::shared_ptr<Attachment>	attachment;
		sockaddr_storage			address;
		unsigned					userLevel;							//!< Current user level. Anonymous (0) by default
		volatile bool				destroyed;
		
				
		/**/						Peer(Connection*connection, bool canDoSharedFromThis):owner(connection),socketAccess(new DefaultSocketAccess()),
									userLevel(User::Anonymous),canDoSharedFromThis(canDoSharedFromThis),destroyed(false),
									writer(this)
									{
										memset(&address,0,sizeof(address));
										ASSERT_NOT_NULL__(owner);
										//writer.Begin(socketAccess);
									}
		virtual						~Peer()
									{}
		/**
		Basically the destructor, except not a destructor, so the destruction won't throw exceptions.
		*/
		void						Dissolve()
									{
										writer.Terminate();
										ASSERT_NOT_NULL__(socketAccess);
										ASSERT__(socketAccess->IsClosed());
										SocketAccess*a = socketAccess;
										socketAccess = nullptr;
										if (a)
											delete a;
									}

		/**
		Pre-desructor shut-down
		*/
		bool						Destroy()
									{
										if (!destroyed)
										{
											writer.Terminate();
											destroyed = true;
											Disconnect();
											return true;
										}
										return false;
										//if (root_address)
										//	freeaddrinfo(root_address);
									}

		template <class SocketAccessClass>
			void					SetSocketAccess()
									{
										ASSERT__(socketAccess->IsClosed());
										delete socketAccess;
										socketAccess = new SocketAccessClass();
										writer.Update(socketAccess);
									}
		void						SetCloneOfSocketAccess(SocketAccess*access)
									{
										ASSERT__(socketAccess->IsClosed());
										delete socketAccess;
										socketAccess = access->CloneClass();
										writer.Update(socketAccess);
									}

		void						Disconnect();			//!< Disconnects the local peer. If this peer is element of a peer collection (ie. a Server instance) then the owner is automatically notified that the client on this peer is no longer available. The local data is erased immediately if the respective dispatcher is set to @b async, or when its resolve() method is next executed.
		bool						IsConnected()	const	//! Queries whether or not the local peer is currently connected @return true if the local peer is currently connected, false otherwise
									{
										return !socketAccess->IsClosed();
									}
		String						ToString()	const	//! Converts the local address into a string. If the local object is NULL then the string "NULL" is returned instead.
									{
										return this?TCP::ToString(address):"NULL";
									}
		bool						SendSignal(UINT32 channel);		//!< Sends a data-less package to the other end of this peer
			
		bool						HandleIsValid()	const	{return !socketAccess->IsClosed();}
		bool						SendObject(UINT32 channel, const ISerializable&object, unsigned minUserLevel=0) override;		//!< Sends a serializable object to the TCP stream on the specified channel
	};

	typedef std::shared_ptr<Peer::Attachment>	PAttachment;

	class Client;
	
	/**
		@brief Asynchronous connection attempt
	*/
	class ConnectionAttempt:public TCPThreadObject
	{
	protected:
		void				ThreadMain();
	public:
		String				connect_target;	//!< Target address
		Client				*const client;		//!< Client to connect

		/**/				ConnectionAttempt(Client*parent):client(parent)	{}
	};

	/**
		@brief Single socket TCP connection client
		
		The Client object establishes and maintains a connection to a server. Multiple connections to different servers require different instances of Client.
	*/
	class Client : public Connection, public Peer
	{
	protected:
		ConnectionAttempt	attempt;
		bool				is_connected;	//!< True if the client has an active connection to a server

		friend class ConnectionAttempt;

		void				fail(const String&message);
	public:
					
		/**/				Client():Peer(this,false),is_connected(false),attempt(this)
							{}
		virtual				~Client()
							{
								attempt.Join();
								Disconnect();
								Join();
							}
		bool				Connect(const String&url);		//!< Attempts to connect to a remote server and waits until a connection was established (or not establishable). Depending on the local connection this may lag for a few seconds. @return true if a connection could be established, false otherwise.
		void				ConnectAsync(const String&url);	//!< Starts the process of connecting to a remote host. Due to the asynchronous nature of this method, No immediate result is available. Hook a function into the local object's inherited @a onEvent callback pointer to receive information about success or failure
			
		//bool				IsConnected() const {return is_connected;}	//implemented by Peer

		bool				IsAttemptingToConnect()	const	//!< Queries whether or not the client is currently attempting to connect
							{
								
								return attempt.IsRunning() && !attempt.IsDone();
							}
		void				Disconnect();

	};
	
	/**
	@brief Multi socket TCP connection server
		
	The Server object sets up a local listen port and automatically accepts incoming connection requests.
	Dispatcher for channel handling is inherited via Connection
	*/
	class Server : public Connection, public TCPThreadObject, public Destination
	{
	private:
		volatile SOCKET		socket_handle;
		volatile bool		clients_locked,
							is_shutting_down;

		Peer				centralPeer;	//used for server-events to not pass null

		SocketAccess		*socketAccess;

		//Array<BYTE>		out_buffer;
			
		void				ThreadMain() override;
	protected:
		void				OnDisconnect(const Peer*, event_t) override;	//!< Executed by a peer if its connection died
		void				Fail(const String&message);					//!< Executed if an operation failed
		bool				SendObject(UINT32 channel, const ISerializable&object, unsigned minUserLevel) override;
		bool				SendObject(UINT32 channel, const PPeer&exclude, const ISerializable&object, unsigned minUserLevel) override;
		
		ReadWriteMutex		clientMutex;	//!< Mutex to shield operations on the client list. This mutex should be locked before clients are queried
		Buffer0<PPeer>		clientList;		//!< List of all currently connected clients
		USHORT				port;			//!< Read only variable which is updated during StartService()
	public:

	
							Server():socket_handle(INVALID_SOCKET),clients_locked(false),is_shutting_down(false),socketAccess(new DefaultSocketAccess()),centralPeer(this,false)
							{}
		virtual				~Server()
							{
								EndService();
								//clientMutex.BeginWrite();
								//	clientList.Clear();
								//clientMutex.EndWrite();
								delete socketAccess;
							}
		bool				HasAnyClients() const
		{
			return clientList.Count() != 0;
		}
		template <typename F>
			void			VisitAllPeerAttachments(const F&f)
							{
								clientMutex.BeginRead();
								{
									foreach (clientList,client)
									{
										const PPeer&p = *client;
										if (p->attachment)
											f(p->attachment);
									}
								}
								clientMutex.EndRead();
							}
		template <typename F>
			void			VisitAllPeers(const F&f)
							{
								clientMutex.BeginRead();
								{
									foreach (clientList,client)
									{
										f(*client);
									}
								}
								clientMutex.EndRead();
							}
		template <class SocketAccessClass>
			void			SetSocketAccess()
							{
								ASSERT__(socketAccess->IsClosed());
								delete socketAccess;
								socketAccess = new SocketAccessClass();
							}

		UINT16				GetPort() const {return port;}
		/**
		@brief Starts the local service on the specified port
				
		This method attempts to bind the specified port to the local object and listen for any incoming
		connections. Query the result of error() to retrieve a more detailed error discription in case
		this method returns false.
		@param port Port to bind this service to
		@param limitToLocalhost If true, only clients connecting from localhost will be permitted
		@param[out] outPort Actually used port. Useful only if @a port is set to 0 such that a dynamic port is assigned
		@return true on success, false otherwise.
		*/
		bool				StartService(USHORT port, bool limitToLocalhost=false, USHORT*outPort=nullptr);
		/**
		@brief Terminates the service.
				
		Any currently connected clients are automatically disconnected.
		This operation executes the ConnectionClosed event once for each remaining client and once for itself
		passing NULL as @b peer parameter.
		*/
		void				EndService();
		void				StopService()	{EndService();};	//!< @overload
		void				Shutdown()		{EndService();};		//!< @overload
		bool				IsOnline()	const	//!< Queries the current execution status
							{
								return IsRunning() && !IsDone();
							}

		PPeer				GetSharedPointerOfClient(Peer*);
	};





	/**
		@brief Abstract template channel structure to handle all traffic on a specific channel
		
		Instances must override the OnReceive() method or specify an onObjectReceive function pointer.
		Otherwise incoming packages are ignored.
		
		@param Object the type of object being sent and received on this channel. Must be a derivative of @a ISerializable
		@param MinUserLevel Minimum remote user level required for an incoming package to be decoded on this channel. If the remote user level does not satisfy this level then the incoming package will be ignored
		@param Channel Channel that this channel structure should operate on
	*/
	template <class Object, unsigned MinUserLevel, UINT32 ChannelID>
		class Channel:public ObjectSender<Object,ChannelID>
		{
		public:
			typedef ObjectSender<Object,ChannelID>	Super;
			void					(*onObjectReceive)(Object&object, Peer&sender);
			void					(*onSimpleObjectReceive)(Object&object);
		protected:
				
				
			virtual	void			Handle(const PSerializableObject&serializable,const TDualLink&sender)	override
			{
				PPeer p = sender.s.lock();
				Peer*ptr = p ? p.get() : sender.p;
				if (ptr && ptr->userLevel >= MinUserLevel)
				{
					OnReceive((Object&)*serializable,*ptr);
				}
				else if (!ptr)
				{
					DBG_FATAL__("peer reference lost in transit for data on channel "+String(ChannelID));
				}
				//discard((Object*)serializable);
			}
								
								
			virtual	PSerializableObject	Deserialize(IReadStream&stream,serial_size_t fixed_size,Peer&sender)	override
			{
				PSerializableObject result (new Object());
				if (!result->Deserialize(stream,fixed_size))
				{
					result.reset();
				}
				return result;
			}
								
			virtual	void			OnReceive(Object&object, Peer&sender)
			{
				if (onObjectReceive)
					onObjectReceive(object,sender);
				elif (onSimpleObjectReceive)
					onSimpleObjectReceive(object);
			}
		
		public:
					
			/**/			Channel():onObjectReceive(NULL),onSimpleObjectReceive(NULL)
							{}
								
			/**/			Channel(void(*onObjectReceive_)(Object&object, Peer&sender)):onObjectReceive(onObjectReceive_),onSimpleObjectReceive(NULL)
							{}
			/**/			Channel(void(*onObjectReceive_)(Object&object)):onObjectReceive(NULL),onSimpleObjectReceive(onObjectReceive_)
							{}
	
		};



}






#endif
