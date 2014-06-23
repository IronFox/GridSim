#include "../global_root.h"
#include "tcp.h"



namespace TCP
{
	bool			verbose=false;


	void	swapCloseSocket(volatile SOCKET&socket)
	{
		if (socket == INVALID_SOCKET)
			return;
		SOCKET socket_to_close = socket;
		socket = INVALID_SOCKET;
		closesocket(socket_to_close);
	}
	String	addressToString(const addrinfo&address)
	{
		static char buffer[INET6_ADDRSTRLEN];
		if (address.ai_addrlen == sizeof(sockaddr_in6))
		{
			sockaddr_in6 copy = *(const sockaddr_in6*)address.ai_addr;
			inet_ntop(AF_INET6,&copy.sin6_addr,buffer,sizeof(buffer));
		}
		else
			if (address.ai_addrlen == sizeof(sockaddr_in))
			{
				sockaddr_in copy = *(const sockaddr_in*)address.ai_addr;
				inet_ntop(AF_INET,&copy.sin_addr,buffer,sizeof(buffer));
			}
			else
				strcpy_s(buffer,sizeof(buffer),"[Unknown address type]");
					
		return buffer;
	}


	static const char* lastSocketError()
	{
		#if SYSTEM==WINDOWS
			return Net::err2str(WSAGetLastError());
		#else
			return Net::err2str(errno);
		#endif
	}

	const char*	event2str(event_t event)
	{
		#undef ECASE
		#define ECASE(symbol)	case Event::symbol: return "Event::"#symbol;
		switch (event)
		{
			ECASE(None);
			ECASE(ConnectionFailed);
			ECASE(ConnectionEstablished);
			ECASE(ConnectionClosed);
			ECASE(ConnectionLost);
		}
		return "Unknown event";
		#undef ECASE
		
	
	
	}
	/*
	void	closesocket(SOCKET socket)
	{
		std::cout << "closing socket "<<socket<<std::endl;
		if (::closesocket(socket))
			std::cout << "failed to close socket"<<std::endl;
	}*/



	bool RootChannel::sendObject(Destination*target, const ISerializable&object)
	{
		return target?target->sendObject(id,object):false;
	}

	bool RootChannel::sendObject(Destination*target, Peer*exclude, const ISerializable&object)
	{
		return target?target->sendObject(id,exclude,object):false;
	}


	void	Dispatcher::handleObject(RootChannel*receiver, Peer*sender, SerializableObject*object)
	{
		if (!object)
			return;
		if (async)
		{
			receiver->handle(object,sender);
		}
		else
		{
			TInbound inbound;
			inbound.object = object;
			inbound.sender = sender;
			inbound.receiver = receiver;
			mutex.lock();
				object_queue << inbound;
			mutex.release();
		}
	}
	
	void	Dispatcher::handlePeerDeletion(Peer*peer)
	{
		if (!peer)
			return;
		if (async)
		{
			DISCARD(peer);
		}
		else
		{
			mutex.lock();
				erase_queue.append(peer);
			mutex.release();
		}
	}
	
	void	Dispatcher::handleEvent(event_t event, Peer*peer)
	{
		if (event == Event::None || block_events)
			return;
		if (async)
		{
			if (onEvent)
			{
				//std::cout << "sending event "<<event2str(event)<<" | "<<(void*)peer<<" to "<<(void*)onEvent<<std::endl;
				onEvent(event,peer);
			}
		}
		else
		{
			TEvent sevent;
			sevent.event = event;
			sevent.sender = peer;
			synchronized(mutex)
			{
				event_queue << sevent;
			}
		}
	}

	void	Dispatcher::handleSignal(UINT32 signal, Peer*peer)
	{
		if (async)
		{
			if (onSignal)
				onSignal(signal,peer);
		}
		else
		{
			TSignal tsignal;
			tsignal.channel = signal;
			tsignal.sender = peer;
			synchronized(mutex)
			{
				signal_queue << tsignal;
			}
		}
	}


	void Dispatcher::resolve()
	{
		mutex.lock();
			TSignal signal;
			while (signal_queue >> signal)
				if (onSignal)
					onSignal(signal.channel,signal.sender);
			TInbound	inbound;
			while (object_queue>>inbound)
			{
				inbound.receiver->handle(inbound.object,inbound.sender);
			}
			TEvent		event;
			while (event_queue>>event)
			{
				if (onEvent)
					onEvent(event.event,event.sender);
			}
			erase_queue.clear();
		mutex.release();
	}
	
	
	RootChannel*	Dispatcher::getReceiver(UINT32 channel_id, unsigned user_level)
	{
		RootChannel*channel;
		if (!channel_map.query(channel_id,channel))	//not protecting this against invalid operations because netRead operations don't interfere with each other
			return NULL;
		return channel;
	}
	
	Dispatcher::Dispatcher():async(true),is_locked(false),block_events(0),onEvent(NULL),onSignal(NULL),onIgnorePackage(NULL)
	{}
	
	Dispatcher::~Dispatcher()
	{
		synchronized(mutex)
		{
			TInbound inbound;
			while (object_queue >> inbound)
				DISCARD(inbound.object);
		}
	}
	
	
	void	Dispatcher::openSignalChannel(UINT32 id, unsigned min_user_level)
	{
		signal_map.set(id,min_user_level);
	}
	
	void	Dispatcher::closeSignalChannel(UINT32 id)
	{
		signal_map.unSet(id);
	}
	
	void	Dispatcher::installChannel(RootChannel&channel)
	{
		channel_map.set(channel.id,&channel);
	}
	
	void	Dispatcher::uninstallChannel(RootChannel&channel)
	{
		channel_map.unSet(channel.id);
	}



	

	
	
	void ConnectionAttempt::ThreadMain()
	{

		if (verbose)
			std::cout << "ConnectionAttempt::ThreadMain() enter"<<std::endl;
		client->disconnect();
		if (!Net::initNet())
		{
			client->setError("Failed to initialize network ("+String(lastSocketError())+")");
			client->handleEvent(Event::ConnectionFailed,client);
			if (verbose)
				std::cout << "ConnectionAttempt::ThreadMain() exit: failed to initialize network"<<std::endl;
			return;
		}
		const String&url = connect_target;
		index_t separator = url.indexOf(':');
		if (!separator)
		{
			client->setError("Missing port in address line '"+url+"'");
			client->handleEvent(Event::ConnectionFailed,client);
			if (verbose)
				std::cout << "ConnectionAttempt::ThreadMain() exit: provided URL lacks port"<<std::endl;
			return;
		}
		String addr = url.subString(0,separator-1),
				s_port = url.subString(separator);
		USHORT port;
		if (!convert(s_port.c_str(),port))
		{
			client->setError("Failed to parse port number '"+s_port+"'");
			client->handleEvent(Event::ConnectionFailed,client);
			if (verbose)
				std::cout << "ConnectionAttempt::ThreadMain() exit: provided port is not parsable"<<std::endl;
			return;
		}



		addrinfo hints,*remote_address;
		memset(&hints,0,sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		if (getaddrinfo(addr.c_str(),s_port.c_str(),&hints,&remote_address) != 0)
		{
			client->setError("Unable to resolve address '"+String(addr)+"'");
			client->handleEvent(Event::ConnectionFailed,client);
			if (verbose)
				std::cout << "ConnectionAttempt::ThreadMain() exit: unable to decode IP address"<<std::endl;
			return;
		}

		addrinfo*actual_address = remote_address;

		String host = "";

		SOCKET socketHandle = INVALID_SOCKET;

		while (actual_address != NULL)
		{
			socketHandle = socket(actual_address->ai_family,actual_address->ai_socktype,actual_address->ai_protocol); //and create a new socket
			if (socketHandle != INVALID_SOCKET)
			{
				int i = 1;
				if (setsockopt( socketHandle, IPPROTO_TCP, TCP_NODELAY, (const char *)&i, sizeof(i)) == SOCKET_ERROR )
				{
					closesocket(socketHandle);
					socketHandle = INVALID_SOCKET;
				}
			}


			if (socketHandle == INVALID_SOCKET)
			{
				client->fail("Socket creation failed");
				if (verbose)
					std::cout << "ConnectionAttempt::ThreadMain() exit: unable to create socket"<<std::endl;
				return;
			}

			if (connect(socketHandle,actual_address->ai_addr,(int)actual_address->ai_addrlen) == 0)
				break;
			if (host.isNotEmpty())
				host += '/';
			host += addressToString(*actual_address);
			actual_address = actual_address->ai_next;
			swapCloseSocket(socketHandle);
		}

		if (actual_address == NULL)
		{
			client->setError("'"+host+"' does not answer on port "+s_port);
			client->handleEvent(Event::ConnectionFailed,client);
			if (verbose)
				std::cout << "ConnectionAttempt::ThreadMain() exit: connection failed"<<std::endl;
			return;
		}

		if (client->root_address)
			freeaddrinfo(client->root_address);
		client->root_address = remote_address;
		client->actual_address = actual_address;
		try
		{
			client->socketAccess->SetSocket(socketHandle);
		}
		catch (const std::exception&exception)
		{
			client->setError("Socket set operation to '"+host+"' failed: "+exception.what());
			client->handleEvent(Event::ConnectionFailed,client);
			if (verbose)
				std::cout << "ConnectionAttempt::ThreadMain() exit: connection failed"<<std::endl;
			return;
		}


		if (verbose)
			std::cout << "ConnectionAttempt::ThreadMain(): sending 'connection established' event"<<std::endl;
		client->handleEvent(Event::ConnectionEstablished,client);
		if (verbose)
			std::cout << "ConnectionAttempt::ThreadMain(): starting client thread"<<std::endl;
		client->start();
		if (verbose)
			std::cout << "ConnectionAttempt::ThreadMain() exit: connection established"<<std::endl;

	}


	bool	Client::connect(const String&url)
	{
		if (attempt.isActive())
		{
			attempt.awaitCompletion();
			return !socketAccess->IsClosed();
		}
		connectAsync(url);
		attempt.awaitCompletion();
		ASSERT__(!attempt.isActive());
		return !socketAccess->IsClosed();
	}
	
	void	Client::connectAsync(const String&url)
	{
		ASSERT__(!isSelf());
		if (attempt.isActive())
			return;
			
		attempt.connect_target = url;
		attempt.client = this;
		attempt.start();
	}

	void	Peer::handleUnexpectedSendResult(int result)
	{
		if (!result || result == SOCKET_ERROR)
		{
			if (socketAccess->IsClosed())
			{
				if (verbose)
					std::cout << "Peer::succeeded() exit: socket handle reset by remote operation"<<std::endl;
				return;
			}
			owner->setError("");
			owner->handleEvent(Event::ConnectionClosed,this);
			socketAccess->CloseSocket();
			owner->onDisconnect(this,Event::ConnectionClosed);
			if (verbose)
				std::cout << "Peer::succeeded() exit: result is 0"<<std::endl;
			return;
		}
		if (socketAccess->IsClosed())
		{
			if (verbose)
				std::cout << "Peer::succeeded() exit: socket handle reset by remote operation"<<std::endl;
			return;
		}
		owner->setError("Connection lost to "+ToString()+" ("+lastSocketError()+")");
		socketAccess->CloseSocket();
		owner->handleEvent(Event::ConnectionLost,this);
		owner->onDisconnect(this,Event::ConnectionLost);
		if (verbose)
			std::cout << "Peer::succeeded() exit: result was unexpected. socket closed"<<std::endl;
	}
	
	bool	Peer::succeeded(int result, size_t desired)
	{
		if (verbose)
			std::cout << "Peer::succeed() enter: result="<<result<<", desired="<<desired<<std::endl;
		if (result == (int)desired)
		{
			if (verbose)
				std::cout << "Peer::succeed() exit: match"<<std::endl;
			return true;
		}
		handleUnexpectedSendResult(result);
		return false;
	}
	
	bool	Peer::sendData(UINT32 channel_id, const void*data, size_t size)
	{
		if (socketAccess->IsClosed())
			return false;
		UINT32 size32 = (UINT32)size;
		synchronized(write_mutex)
		{

			if (socketAccess->Write(&channel_id,4)!=4)
				return false;
			if (socketAccess->Write(&size32,4)!=4)
				return false;
			if (socketAccess->Write(data,size)!=(int)size)
				return false;
		}
		return true;
	}
	
	
	bool	Peer::netRead(BYTE*current, size_t size)
	{
		if (verbose)
			std::cout << "Peer::netRead() enter: size="<<size<<std::endl;
		BYTE*end = current+size;
		while (current < end)
		{
			int size = socketAccess->Read(current,end-current);
			if (size == SOCKET_ERROR)
			{
				if (socketAccess->IsClosed())
				{
					if (verbose)
						std::cout << "Peer::netRead() exit: socket handle reset by remote operation"<<std::endl;
					return false;
				}
				socketAccess->CloseSocket();
				owner->setError("Connection lost to "+ToString()+" ("+lastSocketError()+")");
				owner->handleEvent(Event::ConnectionLost,this);
				owner->onDisconnect(this,Event::ConnectionLost);
				if (verbose)
					std::cout << "Peer::netRead() exit: invalid size value received: "<<size<<std::endl;
				return false;
			}
			if (!size)
			{
				if (socketAccess->IsClosed())
				{
					if (verbose)
						std::cout << "Peer::netRead() exit: socket handle reset by remote operation"<<std::endl;
					return false;
				}
				socketAccess->CloseSocket();
				owner->setError("");
				owner->handleEvent(Event::ConnectionClosed,this);
				owner->onDisconnect(this,Event::ConnectionClosed);
				if (verbose)
					std::cout << "Peer::netRead() exit: 0 size value received"<<std::endl;
				return false;
			}
			current += size;
		}
		if (verbose)
			std::cout << "Peer::netRead() exit"<<std::endl;
		return true;
	}

	/*virtual override*/ bool	Peer::Read(void*target, serial_size_t size)
	{
		if (!size)
			return true;
		if (size > remaining_size)
			return false;
		if (!netRead((BYTE*)target,size))
			return false;
		remaining_size -= size;
		return true;
	}

	/*virtual override*/ serial_size_t				Peer::GetRemainingBytes() const
	{
		return remaining_size;
	}

	
	/*virtual override*/ bool	Peer::Write(const void*target, serial_size_t size)
	{
		if (verbose)
			std::cout << "Peer::write() enter"<<std::endl;
		if (!size)
			return true;
		if (verbose)
			std::cout << "Peer::write(): sending "<<size<<" byte(s)"<<std::endl;
		if (size > remaining_write_size)
		{
			FATAL__("Send-size ("+String(size)+" byte(s)) exceeds remaining available write size ("+String(remaining_write_size)+" byte(s))");
		}
		if (!succeeded(socketAccess->Write(target,size),size))
		{
			if (verbose)
				std::cout << "Peer::write(): failed to write data to socket"<<std::endl;
			return false;
		}
		remaining_write_size -= size;
		if (verbose)
			std::cout << "Peer::write() exit: "<<remaining_write_size<<" byte(s) remaining to write"<<std::endl;
		return true;
	}
	
	static BYTE	dump_buffer[2048];
	bool	Peer::sendObject(UINT32 channel_id, const ISerializable&object)
	{
		if (verbose)
			std::cout << "Peer::sendObject() enter: channel_id="<<channel_id<<std::endl;
		if (socketAccess->IsClosed())
		{
			if (verbose)
				std::cout << "Peer::sendObject() exit: socket handle reset by remote operation"<<std::endl;
			return false;
		}
		UINT32 size32 = (UINT32)object.GetSerialSize(false);
		if (verbose)
			std::cout << "Peer::sendObject() exit: package size determined as "<<size32<<" byte(s)"<<std::endl;
		softsync(write_mutex)
		{
			serial_buffer.reset();
			serial_buffer << (UINT32)channel_id;
			serial_buffer << (UINT32)0;
			bool did_serialize = object.Serialize(serial_buffer,false);
			bool did_write = false;
			if (did_serialize)
			{
				UINT32*data = (UINT32*)serial_buffer.data();
				data[1] = (UINT32)(serial_buffer.fillLevel() - 8);

				int rs = socketAccess->Write(serial_buffer.data(),serial_buffer.fillLevel());
				if (rs <= 0)
				{
					if (verbose)
						std::cout << "Peer::sendObject() exit: failed to send package chunk"<<std::endl;
					handleUnexpectedSendResult(rs);
					return false;
				}


			}
			//this->write(serial_buffer.data(),(serial_size_t)serial_buffer.fillLevel());

			
			if (verbose)
				std::cout << "Peer::sendObject() exit: success "<<std::endl;
			return true;
		}
		FATAL__("Architectural flaw");
		return false;
	}
	
	bool	Peer::sendSignal(UINT32 channel_id)
	{
		if (verbose)
			std::cout << "Peer::sendSignal() enter: channel_id="<<channel_id<<std::endl;
		synchronized(write_mutex)
		{
			UINT32 packet[2];
			packet[0] = channel_id;
			packet[1] = 0;

			if (!succeeded(socketAccess->Write(packet,sizeof(packet)),sizeof(packet)))
			{
				if (verbose)
					std::cout << "Peer::sendSignal() exit: failed to send channel id"<<std::endl;
				return false;
			}
			if (verbose)
				std::cout << "Peer::sendSignal() exit"<<std::endl;
			return true;
		}
		FATAL__("Architectural flaw");
		return false;
	}
	
	void	Peer::ThreadMain()
	{
		if (verbose)
			std::cout << "Peer::ThreadMain() enter"<<std::endl;
		ASSERT__(isSelf());	//this should really be implied
		while (!socketAccess->IsClosed())
		{
			UINT32	header[2];
			if (!netRead((BYTE*)header,sizeof(header)))
			{
				if (verbose)
					if (socketAccess->IsClosed())
						std::cout << "Peer::ThreadMain() exit: socket handle reset by remote operation"<<std::endl;
					else
						std::cout << "Peer::ThreadMain() exit: netRead() invocation failed"<<std::endl;
				return;
			}
			if (verbose)
				std::cout << "Peer::ThreadMain(): received header: channel="<<header[0]<<" size="<<header[1]<<std::endl;
			
			remaining_size = (serial_size_t)header[1];
			if (remaining_size > owner->safe_package_size)
			{
				if (socketAccess->IsClosed())
				{
					if (verbose)
						std::cout << "Peer::ThreadMain() exit: socket handle reset by remote operation"<<std::endl;
					return;
				}
				socketAccess->CloseSocket();
				FATAL__("Maximum safe package size ("+String(owner->safe_package_size/1024)+"KB) exceeded by "+String((remaining_size-owner->safe_package_size)/1024)+"KB");
				owner->setError("Maximum safe package size ("+String(owner->safe_package_size/1024)+"KB) exceeded by "+String((remaining_size-owner->safe_package_size)/1024)+"KB");
				owner->handleEvent(Event::ConnectionClosed,this);
				owner->onDisconnect(this,Event::ConnectionClosed);
				if (verbose)
					std::cout << "Peer::ThreadMain() exit: received invalid package size"<<std::endl;
				return;
			}
			UINT32	channel_index = header[0];
			//std::cout << "has package "<<channel_index<<"/"<<remaining_size<<std::endl;

			unsigned min_user_level;
			if (!remaining_size && owner->signal_map.query(channel_index,min_user_level) && user_level >= min_user_level)
			{
				owner->handleSignal(channel_index,this);
			}
			else
			{
					
				RootChannel*receiver = owner->getReceiver(channel_index,user_level);
				
				if (receiver)
				{
					if (verbose)
						std::cout << "Peer::ThreadMain(): deserializing"<<std::endl;
				
					SerializableObject*object = receiver->Deserialize(*this,remaining_size,this);
					if (object)
					{
						if (verbose)
							std::cout << "Peer::ThreadMain(): deserialization succeeded, dispatching object"<<std::endl;
						owner->handleObject(receiver,this,object);
					}
					else
					{
						FATAL__("deserialization failed");	//for now, this is appropriate
					}
					//elif (verbose)
					//{
					//	std::cout << "Peer::ThreadMain(): deserialization failed, or refuses to return an object"<<std::endl;
					//}

				}
				elif (owner->onIgnorePackage)
				{
					FATAL__("ignoring packet");	//for now, this is appropriate
					if (verbose)
						std::cout << "Peer::ThreadMain(): no receiver available (nothing installed on this channel). ignoring package"<<std::endl;
					owner->onIgnorePackage(channel_index,UINT32(remaining_size),this);
				}
				
				while (remaining_size > sizeof(dump_buffer))
					if (netRead(dump_buffer,sizeof(dump_buffer)))
						remaining_size -= sizeof(dump_buffer);
					else
					{
						if (verbose)
							std::cout << "Peer::ThreadMain() exit: failed to read ignored appendix data of package"<<std::endl;
						return;
					}
				if (remaining_size && !netRead(dump_buffer,remaining_size))
				{
					if (verbose)
						std::cout << "Peer::ThreadMain() exit: failed to read ignored appendix data of package"<<std::endl;
					return;
				}
			}
		}
		if (verbose)
			std::cout << "Peer::ThreadMain() exit: socket handle reset by remote operation"<<std::endl;
	}
	
	void Peer::disconnect()
	{
		if (verbose)
			std::cout << "Peer::disconnect() enter"<<std::endl;
		if (!socketAccess->IsClosed())
		{
			if (verbose)
				std::cout << "Peer::disconnect(): graceful shutdown: invoking handlers and closing socket"<<std::endl;
			owner->setError("");
			owner->handleEvent(Event::ConnectionClosed,this);
			socketAccess->CloseSocket();
			if (!isSelf())
				awaitCompletion();	//can't wait for self.
			owner->onDisconnect(this,Event::ConnectionClosed);
			
			/*if (!owner->block_events)	//i really can't recall why i did this
				owner->block_events++;	*/
		}
		elif (verbose)
			std::cout << "Peer::disconnect(): socket handle reset by remote operation"<<std::endl;
		if (verbose)
			std::cout << "Peer::disconnect() exit"<<std::endl;
	}

	void		Server::ThreadMain()
	{
#if 0
		if (verbose)
			std::cout << "Server::ThreadMain() enter"<<std::endl;
		sockaddr_in	addr;
		while (socket_handle != INVALID_SOCKET)
		{
			SOCKET handle;
			socklen_t size = (socklen_t)sizeof(addr);
			handle = accept( socket_handle, (sockaddr*)&addr,&size);
			if (handle == INVALID_SOCKET)
			{
				if (socket_handle == INVALID_SOCKET)
				{
					if (verbose)
						std::cout << "Server::ThreadMain() exit: socket handle reset by remote operation"<<std::endl;
					return;
				}
				fail("accept() call failed");
				if (verbose)
					std::cout << "Server::ThreadMain() exit: accept() operation failed"<<std::endl;
				return;
			}
			Peer*peer = SHIELDED(new Peer(this));
			peer->address = addr;
			peer->SetCloneOfSocketAccess(socketAccess);
			peer->socketAccess->SetSocket(handle);

			if (verbose)
				std::cout << "Server::ThreadMain(): acquiring write lock for client create"<<std::endl;
			client_mutex.signalWrite();
				clients.append(peer);
			client_mutex.exitWrite();
			if (verbose)
				std::cout << "Server::ThreadMain(): released write lock"<<std::endl;
			setError("");
			handleEvent(Event::ConnectionEstablished,peer);
			if (verbose)
				std::cout << "Server::ThreadMain():	starting peer thread"<<std::endl;
			peer->start();
		}
		if (verbose)
			std::cout << "Server::ThreadMain() exit: socket handle reset by remote operation"<<std::endl;
#endif /*0*/
	}
	
	void		Client::fail(const String&message)
	{
		if (verbose)
			std::cout << "Client::fail() invoked: "<<message<<std::endl;
		Connection::setError(message+" ("+lastSocketError()+")");
		Peer::disconnect();
	}
	
	void		Server::fail(const String&message)
	{
		if (verbose)
			std::cout << "Server::fail() enter: "<<message<<std::endl;
		if (is_shutting_down)
		{
			if (verbose)
				std::cout << "Server::fail(): status: shutting down. message ignored"<<std::endl;
			return;
		}
		setError(message+" ("+lastSocketError()+")");
		if (socket_handle != INVALID_SOCKET)
		{
			if (verbose)
				std::cout << "Server::fail(): closing listen socket"<<std::endl;
			handleEvent(Event::ConnectionLost,NULL);
			swapCloseSocket(socket_handle);
		}
		if (verbose)
			std::cout << "Server::fail(): terminating service"<<std::endl;
		block_events++;
			endService();
		block_events--;
		if (verbose)
			std::cout << "Server::fail() exit"<<std::endl;
	}


	bool		Server::startService(USHORT port)
	{
		if (verbose)
			std::cout << "Server::startService() enter"<<std::endl;
		if (isActive())
		{
			setError("connection already active");
			if (verbose)
				std::cout << "Server::startService() exit: service is already online"<<std::endl;
			return false;
		}
		is_shutting_down = false;
		if (!Net::initNet())
		{
			setError("Net failed to initialize");
			if (verbose)
				std::cout << "Server::startService() exit: failed to initialize the network"<<std::endl;
			return false;
		}
		block_events = 0;
		socket_handle = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); //and create a new socket
		if (socket_handle == INVALID_SOCKET)
		{
			fail("socket creation failed");
			if (verbose)
				std::cout << "Server::startService() exit: failed to create listen socket"<<std::endl;
			return false;
		}
		this->port = port;
		
		sockaddr_in self;
		self.sin_family = AF_INET;
		self.sin_addr.s_addr = INADDR_ANY;//inet_addr("127.0.0.1");
		self.sin_port = htons(port);
		if (bind(socket_handle, (sockaddr*)&self,sizeof(self)))
		{
			fail("socket bind failed");
			if (verbose)
				std::cout << "Server::startService() exit: failed to bind listen socket"<<std::endl;
			return false;
		}
		if (listen(socket_handle,0))
		{
			fail("socket listen failed");
			if (verbose)
				std::cout << "Server::startService() exit: failed to start listen operation"<<std::endl;
			return false;
		}
		if (verbose)
			std::cout << "Server::startService(): socket created, bound, and now listening. starting thread"<<std::endl;
		start();
		if (verbose)
			std::cout << "Server::startService() exit: service is online"<<std::endl;
		return true;
	}
	
	void		Server::onDisconnect(Peer*peer, event_t event)
	{
		if (verbose)
			std::cout << "Server::onDisconnect() enter"<<std::endl;
		if (clients_locked)
		{
			if (verbose)
				std::cout << "Server::onDisconnect() exit: clients are locked"<<std::endl;
			return;
		}
		if (verbose)
			std::cout << "Server::onDisconnect(): acquiring write lock for client disconnect"<<std::endl;
		client_mutex.signalWrite();
			if (verbose)
				std::cout << "Server::onDisconnect(): lock acquired. dropping peer"<<std::endl;
			clients.drop(peer);
		client_mutex.exitWrite();
		if (verbose)
			std::cout << "Server::onDisconnect(): released write lock. discarding peer"<<std::endl;
		handlePeerDeletion(peer);
		if (verbose)
			std::cout << "Server::onDisconnect() exit"<<std::endl;
	}

	void		Server::endService()
	{
		if (verbose)
			std::cout << "Server::endService() enter"<<std::endl;
		if (is_shutting_down)
		{
			if (verbose)
				std::cout << "Server::endService() exit: already shutting down"<<std::endl;
			return;
		}
		is_shutting_down = true;
		block_events++;
			if (verbose)
				std::cout << "Server::endService(): acquiring write lock for service termination"<<std::endl;
			client_mutex.signalWrite();
				if (verbose)
					std::cout << "Server::endService(): lock acquired. erasing client peers"<<std::endl;
				clients_locked = true;
				clients.clear();
				clients_locked = false;
			client_mutex.exitWrite();
			if (verbose)
				std::cout << "Server::endService(): released write lock"<<std::endl;
			
			if (socket_handle != INVALID_SOCKET)
			{
				if (verbose)
					std::cout << "Server::endService(): closing listen socket"<<std::endl;
				swapCloseSocket(socket_handle);
			}
			if (verbose)
				std::cout << "Server::endService(): awaiting listen thread termination"<<std::endl;
			awaitCompletion();
		block_events--;
		if (!block_events)//...??? must have had something to do with the incrementation in Peer::diconnect()
			setError("");
		if (verbose)
			std::cout << "Server::endService(): sending connection closed event"<<std::endl;
		handleEvent(Event::ConnectionClosed,NULL);
		if (verbose)
			std::cout << "Server::endService() exit: service is offline"<<std::endl;
	}
	
	
	bool		Server::sendObject(UINT32 channel, const ISerializable&object)
	{
		if (verbose)
			std::cout << "Server::sendObject() enter: channel="<<channel<<std::endl;

		if (is_shutting_down)
		{
			if (verbose)
				std::cout << "Server::sendObject() exit: service is being shut down"<<std::endl;
			return false;
		}
		serial_size_t size = object.GetSerialSize(false);
		Array<BYTE>		out_buffer(size);
		if (!SerializeToMemory(object,out_buffer.pointer(),size,false))
			return false;
		if (verbose)
			std::cout << "Server::sendObject(): acquiring read lock for message send"<<std::endl;
		client_mutex.signalRead();
			if (verbose)
				std::cout << "Server::sendObject(): lock acquired"<<std::endl;
			for (unsigned i = 0; i < clients; i++)
			{
				Peer*peer = clients[i];
				if (peer->socketAccess->IsClosed())
					continue;	//we assume this case is already handled
				if (!peer->sendData(channel,out_buffer.pointer(),size))
				{
					client_mutex.exitRead();
					if (verbose)
					{
						std::cout << "Server::sendObject(): released read lock"<<std::endl;
						std::cout << "Server::sendObject(): acquiring write lock for peer termination"<<std::endl;
					}
					client_mutex.signalWrite();
						clients_locked = true;
						if (verbose)
							std::cout << "Server::sendObject(): erasing peer "<<peer->ToString()<<std::endl;
						clients.erase(peer);
						clients_locked = false;
					client_mutex.exitWrite();
					if (verbose)
					{
						std::cout << "Server::sendObject(): released write lock"<<std::endl;
						std::cout << "Server::sendObject(): acquiring read lock for continued message send"<<std::endl;
					}
					client_mutex.signalRead();
					i--;
				}
			}
		client_mutex.exitRead();
		if (verbose)
		{
			std::cout << "Server::sendObject(): released read lock"<<std::endl;
			std::cout << "Server::sendObject() exit"<<std::endl;
		}
	
		return true;
	}
	
	bool		Server::sendObject(UINT32 channel, Peer*exclude, const ISerializable&object)
	{
		if (verbose)
			std::cout << "Server::sendObject() enter: channel="<<channel<<", exclude="<<exclude->ToString()<<std::endl;
		if (is_shutting_down)
		{
			if (verbose)
				std::cout << "Server::sendObject() exit: service is being shut down"<<std::endl;
			return false;
		}
		serial_size_t size = object.GetSerialSize(false);
		Array<BYTE>		out_buffer(size);
		if (!SerializeToMemory(object,out_buffer.pointer(),size,false))
			return false;
		if (verbose)
			std::cout << "Server::sendObject(): acquiring read lock for message send"<<std::endl;
		client_mutex.signalRead();
			for (unsigned i = 0; i < clients; i++)
			{
				Peer*peer = clients[i];
				if (peer == exclude)
					continue;
				if (peer->socketAccess->IsClosed())
					continue;	//we assume this case is already handled
				if (!peer->sendData(channel,out_buffer.pointer(),size))
				{
					client_mutex.exitRead();
					if (verbose)
					{
						std::cout << "Server::sendObject(): released read lock"<<std::endl;
						std::cout << "Server::sendObject(): acquiring write lock for peer termination"<<std::endl;
					}
					client_mutex.signalWrite();
						clients_locked = true;
						if (verbose)
							std::cout << "Server::sendObject(): erasing peer "<<peer->ToString()<<std::endl;
						clients.erase(peer);
						clients_locked = false;
					client_mutex.exitWrite();
					if (verbose)
					{
						std::cout << "Server::sendObject(): released write lock"<<std::endl;
						std::cout << "Server::sendObject(): acquiring read lock for continued message send"<<std::endl;
					}
					client_mutex.signalRead();
					i--;
				}
			}
		client_mutex.exitRead();
		if (verbose)
		{
			std::cout << "Server::sendObject(): released read lock"<<std::endl;
			std::cout << "Server::sendObject() exit"<<std::endl;
		}
		return true;
	}



 }
 