#include "../global_root.h"
#include "tcp.h"





namespace TCP
{
	bool			verbose=false;

	template class Channel<String,0,0>;
	template class ObjectSender<String,0>;
	template class ConstrainedObject<String,32>;



	void	NoExceptionHandler(const TCodeLocation&loc,const std::exception&ex)
	{

	}
	void	NoArbExceptionHandler(const TCodeLocation&loc)
	{

	}

	std::function<void(const TCodeLocation&,const std::exception&)>	onIgnoredException = NoExceptionHandler;
	std::function<void(const TCodeLocation&)>	onIgnoredArbitraryException = NoArbExceptionHandler;



	void	SwapCloseSocket(volatile SOCKET&socket)
	{
		if (socket == INVALID_SOCKET)
			return;
		SOCKET socket_to_close = socket;
		socket = INVALID_SOCKET;
		closesocket(socket_to_close);
	}

	String	ToString(const addrinfo&address)
	{
		sockaddr_storage temp;
		memcpy(&temp, address.ai_addr, address.ai_addrlen);
		return ToString(temp);
/*

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
					
		return buffer;*/
	}

	String	ToString(const sockaddr_storage&address)
	{
		char ipbuff[INET_ADDRSTRLEN];
		inet_ntop(address.ss_family, &(((struct sockaddr_in *)&address)->sin_addr), ipbuff, INET_ADDRSTRLEN);
		return String(ipbuff);
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



	bool RootChannel::SendObject(Destination&target, const ISerializable&object, unsigned minUserLevel)
	{
		return target.SendObject(id,object,minUserLevel);
	}

	bool RootChannel::SendObject(Destination&target, const PPeer&exclude, const ISerializable&object, unsigned minUserLevel)
	{
		return target.SendObject(id,exclude,object,minUserLevel);
	}


	void	Dispatcher::HandleObject(RootChannel*receiver, const TDualLink&sender, const PSerializableObject&object)
	{
		if (!object)
			return;
		if (async)
		{
			receiver->Handle(object,sender);
			//if (eventLock.PermissiveLock())
			//{
			//	eventLock.PermissiveUnlock();
			//}
		}
		else
		{
			TCommonEvent inbound;
			inbound.type = TCommonEvent::Object;
			inbound.object = object;
			inbound.sender = sender;
			inbound.receiver = receiver;
			if (eventLock.ExclusiveLock(CLOCATION))
			{
				queue << inbound;
				eventLock.ExclusiveUnlock();
			}
		}
	}
	
	void	Dispatcher::HandlePeerDeletion(const PPeer&peer)
	{
		if (!peer || !peer->Destroy())
			return;
		//if (async)
		//{
		//	peer->Join();
		//}
		//else
		{
			wasteMutex.lock();
				wasteBucket.Append(peer);
			wasteMutex.release();
		}
	}
	
	void	Dispatcher::HandleEvent(event_t event, const TDualLink&peer)
	{
		if (event == Event::None)
			return;
		if (async)
		{
			if (eventLock.PermissiveLock(CLOCATION))
			{
				if (onEvent)
				{
					PPeer p = peer.s.lock();
					Peer*ptr = p ? p.get() : peer.p;
					if (ptr)
						onEvent(event,*ptr);
					else
					{
						DBG_FATAL__("peer reference lost in transit for event "+String(event2str(event)));
					}
				}
				eventLock.PermissiveUnlock();
			}
		}
		else
		{
			TCommonEvent  sevent;
			sevent.type = TCommonEvent::NetworkEvent;
			sevent.event = event;
			sevent.sender = peer;
			if (eventLock.ExclusiveLock(CLOCATION))
			{
				queue << sevent;
				eventLock.ExclusiveUnlock();
			}
		}
	}

	void	Dispatcher::HandleSignal(UINT32 signal, const TDualLink&peer)
	{
		if (async)
		{
			if (eventLock.PermissiveLock(CLOCATION))
			{
				if (onSignal)
				{
					PPeer p = peer.s.lock();
					Peer*ptr = p ? p.get() : peer.p;
					if (ptr)
						onSignal(signal,*ptr);
					else
					{
						DBG_FATAL__("peer reference lost in transit for signal on channel "+String(signal));
					}
				}
				eventLock.PermissiveUnlock();
			}
		}
		else
		{
			TCommonEvent tsignal;
			tsignal.type = TCommonEvent::Signal;
			tsignal.channel = signal;
			tsignal.sender = peer;

			if (eventLock.ExclusiveLock(CLOCATION))
			{
				queue << tsignal;
				eventLock.ExclusiveUnlock();
			}
			
		}
	}


	void Dispatcher::Resolve()
	{
		if (eventLock.ExclusiveLock(CLOCATION))
		{
			TCP_TRY
			{
				TCommonEvent ev;
				//TSignal signal;
				while (queue >> ev)
				{
					TCP_TRY
					{
						switch (ev.type)
						{
							case TCommonEvent::NetworkEvent:
								if (onEvent)
								{
									PPeer p = ev.sender.s.lock();
									Peer*ptr = p ? p.get() : ev.sender.p;
									if (ptr)
										onEvent(ev.event,*ptr);
									else
									{
										throw Exception(CLOCATION,"peer reference lost in transit for event "+String(event2str(ev.event)));
									}
								}
							break;
							case TCommonEvent::Signal:
								if (onSignal)
								{
									PPeer p = ev.sender.s.lock();
									Peer*ptr = p ? p.get() : ev.sender.p;
									if (ptr)
										onSignal(ev.channel,*ptr);
									else
									{
										throw Exception(CLOCATION,"peer reference lost in transit for signal on channel "+String(ev.channel));
									}
								}
							break;
							case TCommonEvent::Object:
								ev.receiver->Handle(ev.object,ev.sender);
							break;
						}
					}
					TCP_CATCH
				}
			}
			TCP_CATCH
			eventLock.ExclusiveUnlock();
		}
		//mutex.release();
		FlushWaste();
	}

	void Dispatcher::FlushWaste()
	{
		wasteMutex.lock();
			foreach(wasteBucket, w)
			{
				TCP_TRY
				{
					(*w)->Join(/*1000*/);
				}
				TCP_CATCH
				TCP_TRY
				{
					(*w)->Dissolve();
				}
				TCP_CATCH
			}
			wasteBucket.Clear();
		wasteMutex.release();
	}
	
	void Dispatcher::FlushPendingEvents()
	{
		FlushWaste();
		if (eventLock.ExclusiveLock(CLOCATION))
		{
			queue.Clear();
			eventLock.ExclusiveUnlock();
		}
	}
	
	RootChannel*	Dispatcher::getReceiver(UINT32 channel_id, unsigned user_level)
	{
		RootChannel*channel;
		if (!channel_map.query(channel_id,channel))	//not protecting this against invalid operations because netRead operations don't interfere with each other
			return NULL;
		return channel;
	}
	
	Dispatcher::Dispatcher():async(true),is_locked(false),onEvent(NULL),onSignal(NULL),onIgnorePackage(NULL),onDeserializationFailed(NULL)
	{}
	
	Dispatcher::~Dispatcher()
	{
		//synchronized(mutex)
		//{
		//	TInbound inbound;
		//	while (object_queue >> inbound)
		//		DISCARD(inbound.object);
		//}
	}
	
	
	void	Dispatcher::OpenSignalChannel(UINT32 id, unsigned min_user_level)
	{
		signal_map.set(id,min_user_level);
	}
	
	void	Dispatcher::CloseSignalChannel(UINT32 id)
	{
		signal_map.unSet(id);
	}
	
	void	Dispatcher::InstallChannel(RootChannel&channel)
	{
		channel_map.set(channel.id,&channel);
	}
	
	void	Dispatcher::UninstallChannel(RootChannel&channel)
	{
		channel_map.unSet(channel.id);
	}



	

	
	
	void ConnectionAttempt::ThreadMain()
	{

		if (verbose)
			std::cout << "ConnectionAttempt::ThreadMain() enter"<<std::endl;
		client->Disconnect();
		client->Join();
		if (!Net::initNet())
		{
			client->setError("Failed to initialize network ("+String(lastSocketError())+")");
			client->HandleEvent(Event::ConnectionFailed,TDualLink(client));
			if (verbose)
				std::cout << "ConnectionAttempt::ThreadMain() exit: failed to initialize network"<<std::endl;
			return;
		}
		const String&url = connect_target;
		index_t separator = url.indexOf(':');
		if (!separator)
		{
			client->setError("Missing port in address line '"+url+"'");
			client->HandleEvent(Event::ConnectionFailed,TDualLink(client));
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
			client->HandleEvent(Event::ConnectionFailed,TDualLink(client));
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
			client->HandleEvent(Event::ConnectionFailed,TDualLink(client));
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
			host += ToString(*actual_address);
			actual_address = actual_address->ai_next;
			SwapCloseSocket(socketHandle);
		}

		if (actual_address == NULL)
		{
			freeaddrinfo(remote_address);
			client->setError("'"+host+"' does not answer on port "+s_port);
			client->HandleEvent(Event::ConnectionFailed,TDualLink(client));
			if (verbose)
				std::cout << "ConnectionAttempt::ThreadMain() exit: connection failed"<<std::endl;
			return;
		}
		memcpy(&client->address, actual_address->ai_addr, actual_address->ai_addrlen);
		freeaddrinfo(remote_address);
		try
		{
			client->socketAccess->SetSocket(socketHandle);
		}
		catch (const std::exception&exception)
		{
			client->setError("Socket set operation to '"+host+"' failed: "+exception.what());
			client->HandleEvent(Event::ConnectionFailed,TDualLink(client));
			if (verbose)
				std::cout << "ConnectionAttempt::ThreadMain() exit: connection failed"<<std::endl;
			return;
		}
		catch (...)
		{
			client->setError("Socket set operation to '"+host+"' failed (no compatible exception given)");
			client->HandleEvent(Event::ConnectionFailed,TDualLink(client));
			if (verbose)
				std::cout << "ConnectionAttempt::ThreadMain() exit: connection failed"<<std::endl;
			return;
		}
		client->writer.Begin(client->socketAccess);

		if (verbose)
			std::cout << "ConnectionAttempt::ThreadMain(): starting client thread"<<std::endl;
		client->Start();
		if (verbose)
			std::cout << "ConnectionAttempt::ThreadMain() exit: connection established"<<std::endl;
		if (verbose)
			std::cout << "ConnectionAttempt::ThreadMain(): sending 'connection established' event"<<std::endl;
		client->HandleEvent(Event::ConnectionEstablished,TDualLink(client));

	}


	bool	Client::Connect(const String&url)
	{
		if (attempt.IsRunning())
		{
			attempt.Join();
			return !socketAccess->IsClosed();
		}
		ConnectAsync(url);
		attempt.Join();
		ASSERT__(!attempt.IsRunning());
		return !socketAccess->IsClosed();
	}
	
	void	Client::ConnectAsync(const String&url)
	{
		ASSERT__(!IsSelf());
		if (attempt.IsRunning())
		{
			if (!attempt.IsDone())
				return;
			attempt.Join();
		}
			
		attempt.connect_target = url;
//		attempt.client = this;
		attempt.Start();
	}

	void	Peer::handleUnexpectedSendResult(int result)
	{
		writer.Terminate();
		if (!result || result == SOCKET_ERROR)
		{
			if (socketAccess->IsClosed())
			{
				if (verbose)
					std::cout << "Peer::succeeded() exit: socket handle reset by remote operation"<<std::endl;
				return;
			}
			owner->setError("");
			owner->HandleEvent(Event::ConnectionClosed,LinkFromThis());
			socketAccess->CloseSocket();
			owner->OnDisconnect(this,Event::ConnectionClosed);
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
		owner->HandleEvent(Event::ConnectionLost,LinkFromThis());
		owner->OnDisconnect(this, Event::ConnectionLost);
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
		return writer.Write(channel_id,data,size);
		//UINT32 size32 = (UINT32)size;
		//synchronized(write_mutex)
		//{

		//	if (socketAccess->Write(&channel_id,4)!=4)
		//		return false;
		//	if (socketAccess->Write(&size32,4)!=4)
		//		return false;
		//	if (socketAccess->Write(data,size)!=(int)size)
		//		return false;
		//}
		//return true;
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
				writer.Terminate();
				if (socketAccess->IsClosed())
				{
					if (verbose)
						std::cout << "Peer::netRead() exit: socket handle reset by remote operation"<<std::endl;
					return false;
				}
				socketAccess->CloseSocket();
				owner->setError("Connection lost to "+ToString()+" ("+lastSocketError()+")");
				owner->HandleEvent(Event::ConnectionLost,LinkFromThis());
				if (!destroyed)
					owner->OnDisconnect(this,Event::ConnectionLost);
				if (verbose)
					std::cout << "Peer::netRead() exit: invalid size value received: "<<size<<std::endl;
				return false;
			}
			if (!size)
			{
				writer.Terminate();
				if (socketAccess->IsClosed())
				{
					if (verbose)
						std::cout << "Peer::netRead() exit: socket handle reset by remote operation"<<std::endl;
					return false;
				}
				socketAccess->CloseSocket();
				owner->setError("");
				owner->HandleEvent(Event::ConnectionClosed,LinkFromThis());
				if (!destroyed)
					owner->OnDisconnect(this, Event::ConnectionClosed);
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

	
	///*virtual override*/ bool	Peer::Write(const void*target, serial_size_t size)
	//{
	//	if (verbose)
	//		std::cout << "Peer::write() enter"<<std::endl;
	//	if (!size)
	//		return true;
	//	if (verbose)
	//		std::cout << "Peer::write(): sending "<<size<<" byte(s)"<<std::endl;
	//	if (size > remaining_write_size)
	//	{
	//		FATAL__("Send-size ("+String(size)+" byte(s)) exceeds remaining available write size ("+String(remaining_write_size)+" byte(s))");
	//	}
	//	if (!succeeded(socketAccess->Write(target,size),size))
	//	{
	//		if (verbose)
	//			std::cout << "Peer::write(): failed to write data to socket"<<std::endl;
	//		return false;
	//	}
	//	remaining_write_size -= size;
	//	if (verbose)
	//		std::cout << "Peer::write() exit: "<<remaining_write_size<<" byte(s) remaining to write"<<std::endl;
	//	return true;
	//}
	
	static BYTE	dump_buffer[2048];
	bool	Peer::SendObject(UINT32 channel_id, const ISerializable&object, unsigned minUserLevel)
	{
		if (this->userLevel < minUserLevel)
			return false;
		if (writer.connectionLost)
			return false;
		writer.Write(channel_id,object);
		return !writer.connectionLost;
	}


	void						PeerWriter::Begin(SocketAccess*access)
	{
		Terminate();
		connectionLost = false;
		pipe.clear();
		accessPointerLock.lock();
		this->accessPointer = access;
		accessPointerLock.unlock();
		this->Start();
	}

	void						PeerWriter::Update(SocketAccess*access)
	{
		accessPointerLock.lock();

		this->accessPointer = access;
		accessPointerLock.unlock();
	}
	void						PeerWriter::Terminate()
	{
		TCP_TRY
		{
			accessPointerLock.lock();
				accessPointer = nullptr;
			accessPointerLock.unlock();
		}
		TCP_CATCH

		connectionLost = true;
		TCP_TRY
		{
			pipe.clear();
		}
		TCP_CATCH
		TCP_TRY
		{
			pipe.OverrideSignalNow();
		}
		TCP_CATCH
		this->Join();
	}


	void	PeerWriter::ThreadMain()
	{
		Buffer<Array<BYTE>,0,Swap>	storage;
		for (;;)
		{

			if (accessPointer && !connectionLost)
				pipe.WaitForContent();

			accessPointerLock.lock();
			if (!accessPointer || connectionLost /*|| pipe.IsEmpty()*/)	//through weird synchronization events, the pipe might be empty
			{
				connectionLost = true;
				accessPointer = nullptr;
				if (verbose)
					std::cout << __func__<<" exit"<<std::endl;
				accessPointerLock.unlock();
				return;
			}
			storage.Clear();
			pipe.SignalRead();
			if (verbose)
				std::cout << __func__<<" loop "<<std::endl;
	
			storage.MoveAppend(pipe.begin(),pipe.Count());
			pipe.ExitRead();


			for (auto package : storage)
			{
				bool did_write = false;
				{
					UINT32*data = (UINT32*)package.pointer();
					int rs = accessPointer->Write(package.pointer(),package.size());
					if ((size_t)rs != package.size())
					{
						if (verbose)
							std::cout << __func__ << " exit: failed to send package chunk"<<std::endl;
						connectionLost = true;
						accessPointer = nullptr;
						accessPointerLock.unlock();
						parent->handleUnexpectedSendResult(rs);
						return;
					}
				}
			
			}
			accessPointerLock.unlock();

			storage.Clear();
			if (verbose)
				std::cout << __func__ << " success "<<std::endl;
		}
	}
	
	bool	Peer::SendSignal(UINT32 channel_id)
	{
		if (verbose)
			std::cout << "Peer::sendSignal() enter: channel_id="<<channel_id<<std::endl;
		//synchronized(write_mutex)
		{
			if (!writer.WriteSignal(channel_id))
			{
				if (verbose)
					std::cout << __func__<<" exit: failed to send channel id"<<std::endl;
				return false;
			}
			if (verbose)
				std::cout << __func__ << " exit"<<std::endl;
			return true;
		}
	}
	
	void	Peer::ThreadMain()
	{
		if (verbose)
			std::cout << "Peer::ThreadMain() enter"<<std::endl;
		AssertIsSelf();	//this should really be implied. as it turns out due to whatnot kind of errors, sometimes it hicks up
		while (socketAccess && !socketAccess->IsClosed())
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
				writer.Terminate();
				if (socketAccess->IsClosed())
				{
					if (verbose)
						std::cout << "Peer::ThreadMain() exit: socket handle reset by remote operation"<<std::endl;
					return;
				}
				socketAccess->CloseSocket();
				DBG_FATAL__("Maximum safe package size ("+String(owner->safe_package_size/1024)+"KB) exceeded by "+String((remaining_size-owner->safe_package_size)/1024)+"KB");
				owner->setError("Maximum safe package size ("+String(owner->safe_package_size/1024)+"KB) exceeded by "+String((remaining_size-owner->safe_package_size)/1024)+"KB");
				owner->HandleEvent(Event::ConnectionClosed,LinkFromThis());
				owner->OnDisconnect(this,Event::ConnectionClosed);
				if (verbose)
					std::cout << "Peer::ThreadMain() exit: received invalid package size"<<std::endl;
				return;
			}
			UINT32	channel_index = header[0];
			//std::cout << "has package "<<channel_index<<"/"<<remaining_size<<std::endl;

			unsigned min_user_level=0;
			if (!remaining_size && owner->signal_map.query(channel_index,min_user_level))
			{
				if (userLevel >= min_user_level)
				{
					owner->HandleSignal(channel_index,LinkFromThis());
				}
				else
				{
					if (verbose)
						std::cout << "Peer::ThreadMain(): insufficient user level. ignoring package"<<std::endl;
					if (owner->onIgnorePackage)
					{
						owner->onIgnorePackage(channel_index,0,*this);
					}
				}
			}
			else
			{
					
				RootChannel*receiver = owner->getReceiver(channel_index,userLevel);
				
				if (receiver)
				{
					if (verbose)
						std::cout << "Peer::ThreadMain(): deserializing"<<std::endl;
				
					PSerializableObject object = receiver->Deserialize(*this,remaining_size,*this);
					if (object)
					{
						if (verbose)
							std::cout << "Peer::ThreadMain(): deserialization succeeded, dispatching object"<<std::endl;
						owner->HandleObject(receiver,LinkFromThis(),object);
					}
					else
					{
						if (owner->onDeserializationFailed)
							owner->onDeserializationFailed(channel_index,*this);
						#ifdef _DEBUG
							else if (!this->destroyed && !this->socketAccess->IsClosed())
								FATAL__("deserialization failed");	//for now, this is appropriate
						#endif
					}
					//elif (verbose)
					//{
					//	std::cout << "Peer::ThreadMain(): deserialization failed, or refuses to return an object"<<std::endl;
					//}

				}
				else
				{
					#ifdef _DEBUG
						String dbg;
						bool found = false;
						owner->channel_map.visitAllEntries([&dbg,&found,channel_index](index_t index, RootChannel*channel)
						{
							dbg += String(index)+", ";
							if (index == channel_index)
								found = true;
						});
						ASSERT__(!owner->channel_map.IsSet(channel_index));
						ASSERT__(!found);
						if (!remaining_size)
						{
							min_user_level = 0;
							bool mapped = owner->signal_map.query(channel_index,min_user_level);
							ASSERT__(!mapped);
							ASSERT__(min_user_level == 0);
							String dbg2;
							owner->signal_map.visitAllEntries([&dbg2,&found,channel_index](index_t index, unsigned minLevel)
							{
								dbg2 += String(index)+", ";
								if (index == channel_index)
									found = true;
							});
							ASSERT__(!found);
							FATAL__("channel/signal channel is not known "+String(channel_index)+" (known channels: "+dbg+", known signal channels: "+dbg2+")");	//for now, this is appropriate
						}
						FATAL__("channel is not known "+String(channel_index)+" (known channels: "+dbg+")");	//for now, this is appropriate
					#endif
					if (verbose)
						std::cout << "Peer::ThreadMain(): no receiver available (nothing installed on this channel). ignoring package"<<std::endl;
					if (owner->onIgnorePackage)
					{
						owner->onIgnorePackage(channel_index,UINT32(remaining_size),*this);
					}
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
	
	void Peer::Disconnect()
	{
		if (verbose)
			std::cout << "Peer::disconnect() enter"<<std::endl;
		writer.Terminate();
		if (!socketAccess->IsClosed())
		{
			if (verbose)
				std::cout << "Peer::disconnect(): graceful shutdown: invoking handlers and closing socket"<<std::endl;
			owner->setError("");
			owner->HandleEvent(Event::ConnectionClosed,LinkFromThis());
			socketAccess->CloseSocket();
			owner->OnDisconnect(this,Event::ConnectionClosed);
		}
		elif (verbose)
			std::cout << "Peer::disconnect(): socket handle reset by remote operation"<<std::endl;
		if (verbose)
			std::cout << "Peer::disconnect() exit"<<std::endl;
	}

	void		Server::ThreadMain()
	{
#if 1
		if (verbose)
			std::cout << "Server::ThreadMain() enter"<<std::endl;
		sockaddr_storage	addr;
		while (socket_handle != INVALID_SOCKET)
		{
			if (async)
			{
				FlushWaste();
			}


			SOCKET handle;
			socklen_t size = (socklen_t)sizeof(addr);
			handle = accept( socket_handle, (sockaddr*)&addr,&size);

			if (handle != INVALID_SOCKET)
			{
				int i = 1;
				if (setsockopt( handle, IPPROTO_TCP, TCP_NODELAY, (const char *)&i, sizeof(i)) == SOCKET_ERROR )
				{
					closesocket(handle);
					handle = INVALID_SOCKET;
				}
			}

			if (handle == INVALID_SOCKET)
			{
				if (socket_handle == INVALID_SOCKET)
				{
					if (verbose)
						std::cout << "Server::ThreadMain() exit: socket handle reset by remote operation"<<std::endl;
					return;
				}
				Fail("accept() call failed");
				if (verbose)
					std::cout << "Server::ThreadMain() exit: accept() operation failed"<<std::endl;
				return;
			}
			PPeer peer(new Peer(this,true));
			peer->self = peer;
			peer->address = addr;
			peer->SetCloneOfSocketAccess(socketAccess);
			peer->socketAccess->SetSocket(handle);

			if (verbose)
				std::cout << "Server::ThreadMain(): acquiring write lock for client create"<<std::endl;
			if (clientMutex.BeginWrite())
			{
				if (this->is_shutting_down)
				{
					clientMutex.EndWrite();
					continue;
				}
				clientList << peer;
				clientMutex.EndWrite();

				if (verbose)
					std::cout << "Server::ThreadMain(): released write lock"<<std::endl;
				setError("");
				peer->writer.Begin(peer->socketAccess);

				if (verbose)
					std::cout << "Server::ThreadMain():	starting peer thread"<<std::endl;
				peer->Start();
				HandleEvent(Event::ConnectionEstablished,TDualLink(peer));
			}
			else
				if (verbose)
					std::cout << "Server::ThreadMain():	ignoring client"<<std::endl;
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
		Peer::Disconnect();
	}
	
	void		Server::Fail(const String&message)
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
			HandleEvent(Event::ConnectionLost,TDualLink(&centralPeer));
			SwapCloseSocket(socket_handle);
		}
		if (verbose)
			std::cout << "Server::fail(): terminating service"<<std::endl;
		eventLock.Block(CLOCATION);
			EndService();
		eventLock.Unblock(CLOCATION);
		if (verbose)
			std::cout << "Server::fail() exit"<<std::endl;
	}

	bool		Server::StartService(USHORT port, bool limitToLocalhost/*=false*/, USHORT*outPort/*=nullptr*/)
	{
		if (verbose)
			std::cout << "Server::startService() enter"<<std::endl;
		if (IsRunning())
		{
			if (!IsDone())
			{
				setError("connection already active");
				if (verbose)
					std::cout << "Server::startService() exit: service is already online"<<std::endl;
				return false;
			}
			Join();
		}
		is_shutting_down = false;
		if (!Net::initNet())
		{
			setError("Net failed to initialize");
			if (verbose)
				std::cout << "Server::startService() exit: failed to initialize the network"<<std::endl;
			return false;
		}
		socket_handle = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); //and create a new socket
		if (socket_handle == INVALID_SOCKET)
		{
			Fail("socket creation failed");
			if (verbose)
				std::cout << "Server::startService() exit: failed to create listen socket"<<std::endl;
			return false;
		}
		this->port = port;
		
		sockaddr_in self;
		self.sin_family = AF_INET;
		self.sin_addr.s_addr = limitToLocalhost ? inet_addr("127.0.0.1") : INADDR_ANY;//inet_addr("127.0.0.1");
		self.sin_port = htons(port);
		if (bind(socket_handle, (sockaddr*)&self,sizeof(self)))
		{
			Fail("socket bind failed");
			if (verbose)
				std::cout << "Server::startService() exit: failed to bind listen socket"<<std::endl;
			return false;
		}
		if (listen(socket_handle,0))
		{
			Fail("socket listen failed");
			if (verbose)
				std::cout << "Server::startService() exit: failed to start listen operation"<<std::endl;
			return false;
		}

		if (outPort)
		{
			struct sockaddr_in sin;
			socklen_t len = sizeof(sin);
			if (getsockname(socket_handle, (struct sockaddr *)&sin, &len) == -1)
			{
				Fail("Unable to fetch sock-name on new socket");
				return false;
			}
			else
			{
				(*outPort) = ntohs(sin.sin_port);
				if (port != 0 && port != *outPort)
				{
					Fail("Read port does not match expected port");
					return false;
				}
			}
		}

		if (verbose)
			std::cout << "Server::startService(): socket created, bound, and now listening. starting thread"<<std::endl;
		Start();
		if (verbose)
			std::cout << "Server::startService() exit: service is online"<<std::endl;
		return true;
	}

	PPeer		Server::GetSharedPointerOfClient(Peer*peer)
	{
		if (verbose)
			std::cout << __func__ " enter" << std::endl;
		if (peer)
		{
			if (clients_locked)
			{
				if (verbose)
					std::cout << __func__ " exit: clients are locked" << std::endl;
				return PPeer();
			}
			if (verbose)
				std::cout << __func__": acquiring read lock for client lookup" << std::endl;
			clientMutex.BeginRead();
			if (verbose)
				std::cout << __func__ ": lock acquired. searching" << std::endl;
			index_t at = InvalidIndex;
			foreach(clientList, cl)
				if (cl->get() == peer)
				{
					at = cl - clientList.pointer();
					break;
				}
			PPeer p;
			if (at != InvalidIndex)
			{
				if (verbose)
					std::cout << __func__ " client found" << std::endl;
				p = clientList[at];
			}
			clientMutex.EndRead();
			if (verbose)
				std::cout << __func__": released read lock. returning result" << std::endl;
			return p;
		}
		if (verbose)
			std::cout << __func__ " exit" << std::endl;
		return PPeer();
	}

	
	void		Server::OnDisconnect(const Peer*peer, event_t event)
	{
		if (verbose)
			std::cout << "Server::onDisconnect() enter"<<std::endl;
		if (peer)
		{
			if (clients_locked)
			{
				if (verbose)
					std::cout << "Server::onDisconnect() exit: clients are locked" << std::endl;
				return;
			}
			if (verbose)
				std::cout << "Server::onDisconnect(): acquiring write lock for client disconnect" << std::endl;
			if (!clientMutex.BeginWrite())
			{
				if (verbose)
					std::cout << "Server::onDisconnect(): Server locked down for termination. Aborting event handling" << std::endl;
				return;
			}
			if (verbose)
				std::cout << "Server::onDisconnect(): lock acquired. dropping peer" << std::endl;
			index_t at = InvalidIndex;
			foreach(clientList, cl)
				if (cl->get() == peer)
				{
					at = cl - clientList.pointer();
					break;
				}
			PPeer p;
			if (at != InvalidIndex)
			{
				p = clientList[at];
				clientList.erase(at);
			}
			clientMutex.EndWrite();
			if (verbose)
				std::cout << "Server::onDisconnect(): released write lock. discarding peer" << std::endl;
			if (p)
				HandlePeerDeletion(p);
		}
		if (verbose)
			std::cout << "Server::onDisconnect() exit"<<std::endl;
	}

	void		Server::EndService()
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
		eventLock.Block(CLOCATION);
		TCP_TRY
		{
			if (verbose)
				std::cout << "Server::endService(): acquiring write lock for service termination"<<std::endl;
			ASSERT__(clientMutex.BeginWrite(true));
				if (verbose)
					std::cout << "Server::endService(): lock acquired. erasing client peers"<<std::endl;
				clients_locked = true;
				foreach (clientList,cl)
				{
					(*cl)->Destroy();
					(*cl)->Join();
				}
				clientList.Clear();
				clients_locked = false;
			clientMutex.EndWrite();
			if (verbose)
				std::cout << "Server::endService(): released write lock"<<std::endl;
			
			if (socket_handle != INVALID_SOCKET)
			{
				if (verbose)
					std::cout << "Server::endService(): closing listen socket"<<std::endl;
				SwapCloseSocket(socket_handle);
			}
			if (verbose)
				std::cout << "Server::endService(): awaiting listen thread termination"<<std::endl;
			FlushPendingEvents();
			Join(/*1000*/);
		}
		TCP_CATCH
		if (eventLock.Unblock(CLOCATION))	//Server::Fail() blocks events and calls this method, thus generating recursion. Let's not clear error messages then
			setError("");
		if (verbose)
			std::cout << "Server::endService(): sending connection closed event"<<std::endl;
		HandleEvent(Event::ConnectionClosed,TDualLink(&centralPeer));
		if (verbose)
			std::cout << "Server::endService() exit: service is offline"<<std::endl;
	}
	
	
	bool		Server::SendObject(UINT32 channel, const ISerializable&object, unsigned minUserLevel)
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
		{
			#ifdef _DEBUG
				Array<BYTE>	testBuffer(10000000);
				ISerializable::serial_size_t rs = SerializeToMemory(object,testBuffer.pointer(),testBuffer.GetContentSize(),false);
				if (rs)
				{
					FATAL__("Failed to serialize data structure on channel "+String(channel)+". Expected serial size "+String(size)+" but serialized to "+String(rs));
				}
				else
					FATAL__("Failed to serialize data structure on channel "+String(channel));
			#endif
			return false;
		}
		if (verbose)
			std::cout << "Server::sendObject(): acquiring read lock for message send"<<std::endl;
		clientMutex.BeginRead();
			if (verbose)
				std::cout << "Server::sendObject(): lock acquired"<<std::endl;
			for (index_t i = 0; i < clientList.Count(); i++)
			{
				const PPeer peer = clientList[i];	//should be copy? let's assume so for now
				if (peer->socketAccess->IsClosed())
					continue;	//we assume this case is already handled
				if (peer->userLevel < minUserLevel)
					continue;
				if (!peer->sendData(channel,out_buffer.pointer(),size))
				{
					clientMutex.EndRead();
					if (verbose)
					{
						std::cout << "Server::sendObject(): released read lock"<<std::endl;
						std::cout << "Server::sendObject(): acquiring write lock for peer termination"<<std::endl;
					}
					if (clientMutex.BeginWrite())
					{
						clients_locked = true;
						if (verbose)
							std::cout << "Server::sendObject(): erasing peer "<<peer->ToString()<<std::endl;
						clientList.FindAndErase(peer);
						clients_locked = false;
						clientMutex.EndWrite();
					}
					else
						FATAL__("TCP Server: Failed to acquire write-lock. This should not happen");

					if (verbose)
					{
						std::cout << "Server::sendObject(): released write lock"<<std::endl;
						std::cout << "Server::sendObject(): acquiring read lock for continued message send"<<std::endl;
					}
					HandlePeerDeletion(peer);
					clientMutex.BeginRead();
					i--;
				}
			}
		clientMutex.EndRead();
		if (verbose)
		{
			std::cout << "Server::sendObject(): released read lock"<<std::endl;
			std::cout << "Server::sendObject() exit"<<std::endl;
		}
	
		return true;
	}
	
	bool		Server::SendObject(UINT32 channel, const PPeer&exclude, const ISerializable&object, unsigned minUserLevel)
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
		clientMutex.BeginRead();
			for (index_t i = 0; i < clientList.Count(); i++)
			{
				const PPeer peer = clientList[i];
				if (peer == exclude)
					continue;
				if (peer->socketAccess->IsClosed())
					continue;	//we assume this case is already handled
				if (peer->userLevel < minUserLevel)
					continue;
				if (!peer->sendData(channel,out_buffer.pointer(),size))
				{
					clientMutex.EndRead();
					if (verbose)
					{
						std::cout << "Server::sendObject(): released read lock"<<std::endl;
						std::cout << "Server::sendObject(): acquiring write lock for peer termination"<<std::endl;
					}
					if (clientMutex.BeginWrite())
					{
						clients_locked = true;
						if (verbose)
							std::cout << "Server::sendObject(): erasing peer "<<peer->ToString()<<std::endl;
						clientList.FindAndErase(peer);
						clients_locked = false;
						clientMutex.EndWrite();
					}
					else
						FATAL__("TCP Server: Failed to acquire write-lock. This should not happen");

					if (verbose)
					{
						std::cout << "Server::sendObject(): released write lock"<<std::endl;
						std::cout << "Server::sendObject(): acquiring read lock for continued message send"<<std::endl;
					}
					clientMutex.BeginRead();
					i--;
				}
			}
		clientMutex.EndRead();
		if (verbose)
		{
			std::cout << "Server::sendObject(): released read lock"<<std::endl;
			std::cout << "Server::sendObject() exit"<<std::endl;
		}
		return true;
	}



 }
 