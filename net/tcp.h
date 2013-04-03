#ifndef tcpH
#define tcpH

#include "node.h"
#include "../container/buffer.h"
#include "../io/byte_stream.h"

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

/**
	@brief TCP/IP related constructs
*/
namespace TCP
{
	#if SYSTEM==WINDOWS
		typedef int	socklen_t;
	#endif


	void	swapCloseSocket(volatile SOCKET&socket);
	String	addressToString(const addrinfo&address);
	



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
			swapCloseSocket(socketHandle);
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
		virtual	bool	deserialize(IReadStream&stream, serial_size_t fixed_size)	override
						{
							if (fixed_size > MaxSize)
								return false;
							return Serializable::deserialize(stream,fixed_size);
						}
		};
	
	


	/**
		@brief Signal serializable that does not hold any actual data.
		
		VoidSerializable is always of size 0 and ommits any decoding or encoding but returns success upon doing so
	*/
	class VoidSerializable:public SerializableObject
	{
	public:
	virtual	serial_size_t	serialSize(bool) const	override	{return 0;};
	virtual	bool			serialize(IWriteStream&stream,bool) const		override	{return true;}
	virtual	bool			deserialize(IReadStream&stream,serial_size_t)	override	{return true;}
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
	virtual	bool			sendObject(UINT32 channel, const ISerializable&serializable)=0;
	virtual	bool			sendObject(UINT32 channel, Peer*exclude, const ISerializable&serializable)	{return sendObject(channel,serializable);};
	};

	/**
		@brief Abstract channel installation to handle outgoing and incoming packages
		
		This structure is for inheritance only and does not provide public members.
	*/

	class RootChannel
	{
	protected:
			const UINT32	id;			//!< Channel id. Unmutable
			friend class 	Peer;
			friend class 	Dispatcher;
			Mutex			send_mutex;	//!< Mutex used to shield outgoing serializations to the tcp socket
			
									RootChannel(UINT32 id_):id(id_)
									{}
			
			/**
				@brief Sends an object to the specified destination
			*/
			bool					sendObject(Destination*destination, const ISerializable&object);
			bool					sendObject(Destination*destination, Peer*exclude, const ISerializable&object);
							
									/**
										@brief Deserializes the stream into a new object

										This method is called asynchronously by the respective connection's worker thread.
									*/
	virtual	SerializableObject*		deserialize(IReadStream&stream,serial_size_t fixed_size,Peer*sender)
									{
										return NULL;
									}
									/**
										@brief Dispatches a successfully deserialized object (returned by deserialize() )

										The method is invoked only, if deserialize() returned a new non-NULL object.
										Depending on whether the connection is synchronous or asynchronous, this method is called my the main thread, or the connection's worker thread.
									*/
	virtual	void					handle(SerializableObject*serializable,Peer*sender)	{FATAL__("Channel failed to override handle()");};
	public:
	};


	/**
		@brief Dedicated send-only channel
	*/
	template <class SerializableObject, unsigned Channel>
		class ObjectSender:public RootChannel
		{
		protected:
		virtual	SerializableObject*		deserialize(IReadStream&stream,serial_size_t fixed_size,Peer*sender)	override
										{
											return NULL;
										}
		public:
								ObjectSender():RootChannel(Channel)
								{}
				bool			sendTo(Destination*destination, const SerializableObject&object)
								{
									return sendObject(destination,object);
								}
				/**
					@brief Special overload form of sendTo for servers only
					
					Allows to send objects to all clients except a specific client
					
					@param destination Transfer destination. The method will fail if @a destination is NULL
					@param exclude Peer to specifically exclude
					@param object Serializable object to send
					@return true on success, false otherwise. Note that a positive result only indicates that the object was successfuly serialized to the tcp stream. It does not mean that the package was received or accepted.
				*/
				bool			sendTo(Destination*destination, Peer*exclude, const SerializableObject&object)
								{
									return sendObject(destination,exclude,object);
								}
												
		};
	
	/**
		@brief Dedicated send-only data-less channel
	*/
	template <unsigned ChannelID>
		class SignalSender:public RootChannel
		{
		protected:
		virtual	SerializableObject*		deserialize(IReadStream&stream,serial_size_t fixed_size,Peer*sender)	override
										{
											return NULL;
										}
		public:
								SignalSender():RootChannel(ChannelID)
								{}
				/**
					@brief Sends a signal (data-less package) to the specified destination
				*/
				bool			sendTo(Destination*destination)
								{
									VoidSerializable object;
									return sendObject(destination,object);
								}
				/**
					@brief Special overload form of sendTo for servers only
					
					Allows to send objects to all clients except a specific client
					
					@param destination Transfer destination. The method will fail if @a destination is NULL
					@param exclude Peer to specifically exclude
					@param object Serializable object to send
					@return true on success, false otherwise. Note that a positive result only indicates that the object was successfuly serialized to the tcp stream. It does not mean that the package was received or accepted.
				*/
				bool			sendTo(Destination*destination, Peer*exclude)
								{
									VoidSerializable object;
									return sendObject(destination,exclude,object);
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
			

			struct TInbound
			{
				SerializableObject		*object;
				Peer					*sender;
				RootChannel				*receiver;
			};
			
			struct TEvent
			{
				event_t					event;
				Peer					*sender;
			};
			
			struct TSignal
			{
				UINT32					channel;
				Peer					*sender;
			};
			
			Queue<TInbound>				object_queue;
			Queue<TEvent>				event_queue;
			Queue<TSignal>				signal_queue;
			List::Vector<Peer>			erase_queue;
			Mutex						mutex;
			bool						async;
			volatile bool				is_locked;
			volatile unsigned			block_events;
			IndexTable<RootChannel*>	channel_map;
			IndexTable<unsigned>		signal_map;
			
			friend class Peer;
			
			void						handlePeerDeletion(Peer*);
			void						handleSignal(UINT32 signal, Peer*sender);
			void						handleObject(RootChannel*receiver, Peer*sender, SerializableObject*object);
			void						handleEvent(event_t event, Peer*origin);
			RootChannel*				getReceiver(UINT32 channel_id, unsigned user_level);
			
	public:
			void (*onEvent)(event_t event, Peer*origin);	//!< Callback hook for events (connection, disconnection, etc)	@param event Event that occured @param origin Event origin
			void (*onSignal)(UINT32 signal, Peer*origin);	//!< Callback hook for signals (data-less packages) @param signal Channel that the data-less package was received on @param origin Origin of the signal
			void (*onIgnorePackage)(UINT32 channel,UINT32 size,Peer*origin);	//!< Callback hook for ignored packages. Always async. @param channel Channel the package or signal was received on @param size Package size in bytes @param origin Receiving peer
	
										Dispatcher();
	virtual								~Dispatcher();
			void						resolve();		//!< Resolves all events that occured since the last resolve() invokation. The client application must call this method frequently if the local dispatcher is set to synchronous. Never otherwise.
			void						installChannel(RootChannel&channel);	//!< Installs a channel in/out processor. If the used channel id is already in use then it will be overwritten with the provided channel handler
			void						uninstallChannel(RootChannel&channel);		//!< Uninstalls a channel in/out processor. The used channel id will be closed
			void						openSignalChannel(UINT32 id, unsigned min_user_level);	//!< Opens a signal channel for receiving	@param id Channel id to listen for signals on @param min_user_level Minimum required user level for this signal to be accepted
			void						closeSignalChannel(UINT32 id);							//!< Closes a signal channel
			
			void						setAsync(bool is_async)		//! Changes the socket asynchronous status. Dispatchers run in async mode by default. @param is_async Set true if the dispatcher should perform operations asynchronously from this point on, false otherwise
										{
											async = is_async;
										}
			bool						isAsync()	const			//! Queries whether or not this dispatcher is currently asynchronously. Dispatchers run in async mode by default.
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
	virtual	void						onDisconnect(Peer*peer, event_t event)	{};	//!< Abstract disconnection even called if the local connection has been lost or closed
			const char*					error()	const	//! Queries the last occured error
										{
											return current_error.c_str();
										}
	};
	
	/**
		@brief TCP socket handler
		
		A peer handles incoming and outgoing packages from/to a specific IP address over a specific socket handle. A server would manage one peer structure per client, a client would inherit from Peer.
	
	*/
	class Peer:public ThreadObject, protected IReadStream, protected IWriteStream, public Destination //, public IToString
	{
	protected:
		Connection					*owner;			//!< Pointer to the owning connection to handle incoming packages and report errors to
		Mutex						write_mutex,	//!< Mutex to synchronize socket write operations
									counter_mutex;	//!< Currently not in use
		//volatile SOCKET				socket_handle;	//!< Handle to the occupied socket
		serial_size_t				remaining_size,			//!< Size remaining for reading streams 
									remaining_write_size;	//!< Size remaining for writing streams in the announced memory frame
		ByteStream					serial_buffer;		//!< Buffer used to serialize package data
		SocketAccess				*socketAccess;

		friend class Server;
		friend class RootChannel;
			
		void						ThreadMain();			//!< Socket read thread main
		bool						sendData(UINT32 channel_id, const void*data, size_t size);	//!< Sends raw data to the TCP stream
		bool						succeeded(int result, size_t desired);							//!< Handles the result of a TCP socket send operation. @param result Actual value returned by send() @param desired Valued expected to be returned by send() @return true if both values match, false otherwise. The connection is automatically closed, events triggered and error values set if the operation failed.
		void						handleUnexpectedSendResult(int result);
		bool						read(void*target, serial_size_t size);								//!< IInStream override for direct TCP stream input
		bool						write(const void*target, serial_size_t size);						//!< IOutStream override for direct TCP stream output
		bool						netRead(BYTE*current, size_t size);							//!< Continuously reads a sequence of bytes from the TCP stream. The method does not return until either the requested amount of bytes was received or an error occured
		bool						sendObject(UINT32 channel, const ISerializable&object);		//!< Sends a serializable object to the TCP stream on the specified channel
			
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
		shared_ptr<Attachment>		attachment;
		addrinfo					*root_address,					//!< Remote peer address set (root pointer to linked list describing the remote address)
									*actual_address;					//!< Part of the @a root_address linked list that actually describes the remote address
		unsigned					user_level;							//!< Current user level. Anonymous by default
			
		
				
		/**/						Peer(Connection*connection):owner(connection),socketAccess(new DefaultSocketAccess()),root_address(NULL),actual_address(NULL),user_level(User::Anonymous)
									{
										ASSERT_NOT_NULL__(owner);
									}
		virtual						~Peer()
									{
										disconnect();
										if (root_address)
											freeaddrinfo(root_address);
										delete socketAccess;
									}

		template <class SocketAccessClass>
			void					SetSocketAccess()
									{
										ASSERT__(socketAccess->IsClosed());
										delete socketAccess;
										socketAccess = new SocketAccessClass();
									}
		void						SetCloneOfSocketAccess(SocketAccess*access)
									{
										ASSERT__(socketAccess->IsClosed());
										delete socketAccess;
										socketAccess = access->CloneClass();
									}

		void						disconnect();			//!< Disconnects the local peer. If this peer is element of a peer collection (ie. a Server instance) then the owner is automatically notified that the client on this peer is no longer available. The local data is erased immediately if the respective dispatcher is set to @b async, or when its resolve() method is next executed.
		bool						isConnected()	const	//! Queries whether or not the local peer is currently connected @return true if the local peer is currently connected, false otherwise
									{
										return !socketAccess->IsClosed();
									}
		String						ToString()	const	//! Converts the local address into a string. If the local object is NULL then the string "NULL" is returned instead.
									{
										return this&&actual_address?addressToString(*actual_address):"NULL";
									}
		bool						sendSignal(UINT32 channel);		//!< Sends a data-less package to the other end of this peer
			
		bool						validHandle()	const	{return !socketAccess->IsClosed();}
		//SOCKET						handle()	const	{return socket_handle;}
	};

	class Client;
	
	/**
		@brief Asynchronous connection attempt
	*/
	class ConnectionAttempt:public ThreadObject
	{
	protected:
			void				ThreadMain();
	public:
			String				connect_target;	//!< Target address
			Client				*client;		//!< Client to connect
	};

	/**
		@brief Single socket TCP connection client
		
		The Client object establishes and maintains a connection to a server. Multiple connections to different servers require different instances of Client.
	*/
	class Client:public Connection, public Peer
	{
	protected:
		ConnectionAttempt	attempt;
		bool				is_connected;	//!< True if the client has an active connection to a server

		friend class ConnectionAttempt;

		void				fail(const String&message);
	public:
					
		/**/					Client():Peer(this),is_connected(false)
								{}
		virtual					~Client()
								{
									attempt.awaitCompletion();
									disconnect();
									awaitCompletion();
								}
		bool					connect(const String&url);		//!< Attempts to connect to a remote server and waits until a connection was established (or not establishable). Depending on the local connection this may lag for a few seconds. @return true if a connection could be established, false otherwise.
		void					connectAsync(const String&url);	//!< Starts the process of connecting to a remote host. Due to the asynchronous nature of this method, No immediate result is available. Hook a function into the local object's inherited @a onEvent callback pointer to receive information about success or failure
			
		bool					isAttemptingToConnect()	const	//!< Queries whether or not the client is currently attempting to connect
								{
									return attempt.isActive();
								}
	};
	
	/**
		@brief Multi socket TCP connection server
		
		The Server object sets up a local listen port and automatically accepts incoming connection requests.
		Dispatcher for channel handling is inherited via Connection
	*/
	class Server: public Connection, public ThreadObject, public Destination
	{
	private:
			volatile SOCKET		socket_handle;
			volatile bool		clients_locked,
								is_shutting_down;

		SocketAccess		*socketAccess;

			//Array<BYTE>		out_buffer;
			
			void				ThreadMain();
	
	protected:
			void				onDisconnect(Peer*peer, event_t event);	//!< Executed by a peer if its connection died
			void				fail(const String&message);					//!< Executed if an operation failed
			bool				sendObject(UINT32 channel, const ISerializable&object);
			bool				sendObject(UINT32 channel, Peer*exclude, const ISerializable&object);
		
	public:
			ReadWriteMutex		client_mutex;	//!< Mutex to shield operations on the client list. This mutex should be locked before clients are queried
			List::Vector<Peer>	clients;		//!< List of all currently connected clients
			USHORT				port;			//!< Read only variable which is updated during startService()
	
								Server():socket_handle(INVALID_SOCKET),clients_locked(false),is_shutting_down(false),socketAccess(new DefaultSocketAccess())
								{}
	virtual						~Server()
								{
									client_mutex.signalWrite();
										for (index_t i = 0; i < clients.count(); i++)
											DISCARD(clients[i]);
										clients.flush();
									client_mutex.exitWrite();
									delete socketAccess;
								}
		template <typename F>
			void				forEachPeerAttachment(const F&f)
								{
									client_mutex.signalRead();
									{
										clients.reset();
										while (Peer*p = clients.each())
											if (p->attachment)
												f(p->attachment);
									}
									client_mutex.exitRead();
								}
		template <class SocketAccessClass>
			void				SetSocketAccess()
								{
									ASSERT__(socketAccess->IsClosed());
									delete socketAccess;
									socketAccess = new SocketAccessClass();
								}

			/**
				@brief Starts the local service on the specified port
				
				This method attempts to bind the specified port to the local object and listen for any incoming
				connections. Query the result of error() to retrieve a more detailed error discription in case
				this method returns false.
				@param port Port to bind this service to
				@return true on success, false otherwise.
			*/
			bool				startService(USHORT port);
			/**
				@brief Terminates the service.
				
				Any currently connected clients are automatically disconnected.
				This operation executes the ConnectionClosed event once for each remaining client and once for itself
				passing NULL as @b peer parameter.
			*/
			void				endService();
			void				stopService()	{endService();};	//!< @overload
			void				shutdown()		{endService();};		//!< @overload
			bool				isOnline()	const	//!< Queries the current execution status
								{
									return isActive();
								}
	};





	/**
		@brief Abstract template channel structure to handle all traffic on a specific channel
		
		Instances must override the onReceive() method or specify an onObjectReceive function pointer.
		Otherwise incoming packages are ignored.
		
		@param Object the type of object being sent and received on this channel. Must be a derivative of @a ISerializable
		@param MinUserLevel Minimum remote user level required for an incoming package to be decoded on this channel. If the remote user level does not satisfy this level then the incoming package will be ignored
		@param Channel Channel that this channel structure should operate on
	*/
	template <class Object, unsigned MinUserLevel, UINT32 ChannelID>
		class Channel:public RootChannel
		{
		public:
				void			(*onObjectReceive)(Object*object, Peer*sender);
				void			(*onSimpleObjectReceive)(Object*object);
		protected:
				
		static	Object*			create()
								{
									return new Object();
								}
		static	void			discard(Object*object)
								{
									delete object;
								}
				
				
		virtual	void			handle(SerializableObject*serializable,Peer*sender)
								{
									if (sender->user_level >= MinUserLevel)
										onReceive((Object*)serializable,sender);
									discard((Object*)serializable);
								}
								
								
		virtual	SerializableObject*		deserialize(IReadStream&stream,serial_size_t fixed_size,Peer*sender)	override
										{
											Object*result = create();
											if (!result->deserialize(stream,fixed_size))
											{
												discard(result);
												return NULL;
											}
											return result;
										}
								
		virtual	void			onReceive(Object*object, Peer*sender)
								{
									if (onObjectReceive)
										onObjectReceive(object,sender);
									elif (onSimpleObjectReceive)
										onSimpleObjectReceive(object);
								}
		
		public:
					
								Channel():RootChannel(ChannelID),onObjectReceive(NULL),onSimpleObjectReceive(NULL)
								{}
								
								Channel(void(*onObjectReceive_)(Object*object, Peer*sender)):RootChannel(ChannelID),onObjectReceive(onObjectReceive_),onSimpleObjectReceive(NULL)
								{}
								Channel(void(*onObjectReceive_)(Object*object)):RootChannel(ChannelID),onObjectReceive(NULL),onSimpleObjectReceive(onObjectReceive_)
								{}
								
								
				/**
					@brief Sends an object to the specified destination
					
					@param destination Transfer destination. The method will fail if @a destination is NULL
					@param object Serializable object to send
					@return true on success, false otherwise. Note that a positive result only indicates that the object was successfuly serialized to the tcp stream. It does not mean that the package was received or accepted.
				*/
				bool			sendTo(Destination*destination, const Object&object)
								{
									return sendObject(destination,object);
								}
												
				/**
					@brief Special overload form of sendTo for servers only
					
					Allows to send objects to all clients except a specific client
					
					@param destination Transfer destination. The method will fail if @a destination is NULL
					@param exclude Peer to specifically exclude
					@param object Serializable object to send
					@return true on success, false otherwise. Note that a positive result only indicates that the object was successfuly serialized to the tcp stream. It does not mean that the package was received or accepted.
				*/
				bool			sendTo(Destination*destination, Peer*exclude, const Object&object)
								{
									return sendObject(destination,exclude,object);
								}
				
				

		};



}






#endif
