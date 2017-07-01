#ifndef conventionH
#define conventionH

/******************************************************************

E:\include\net\conventions\infworlds.h

******************************************************************/

namespace Channel	//! Infinite Worlds global channel convention
{
	enum ID	//! Channel ids
	{
		system=0,
			SystemMessage=system,		//!< Messages the client is required to print (server to client)
			
		Module=128,
			ModuleMessage=Module,		//!< Module-wide message (server to client)
			ModuleCurrent=Module+1,		//!< Query current Module id of the requesting User
			ModuleName=Module+2,		//!< Query name of a currently loaded server-side Module via its id (two way)
			ModuleList=Module+3,		//!< Query list of ids of all currently loaded server-side Modules (two way)
		
		User=256,
			UserMessage=User,			//!< Global or individual client messages. From one client to another or an entire Session. (two way)
			UserPrivateMessage=User+1,	//!< Individual client message from one user to another - forwarded from a received UserMessage event (server to client)
			UserSetName=User+2,			//!< Set active User name (two way)
			UserID=User+3,				//!< Query own User id (two way)
			UserLookup=User+4,			//!< Query the id of a User via his/her name (two way)
			UserWhoIs=User+5,			//!< Query the full name of a User via his/her id (two way)
		
		Session=512,
			SessionMessage=Session,			//!< Message the client is required to print (server to client)
			SessionCurrent=Session+1,		//!< Query current Session id of the requesting User
			SessionCreate=Session+2,		//!< Request a new Session (client->server) of the specified module or inform all clients of a new Session (server->client)
			SessionChange=Session+3,		//!< Change the active Session ([session id], client->server) or confirm Session change of a client ([session id, module id, module name] server->client). 
			SessionList=Session+4,			//!< Contains a list of all public Session ids (server->client) sent to new clients.
			SessionUserCount=Session+5,		//!< Request/receive number of Users in this Session
			SessionUserList=Session+6,		//!< Request/receive a list of User ids
			SessionModule=Session+7,		//!< Queries the module id of the specified session
			SessionUserConnect=Session+8,	//!< Notifies all clients of a session that a user has connected to the session (server to client) - contains the respective user id and name [id,name]
			SessionUserDisconnect=Session+9	//!< Notifies all clients of a session that a user has disconnect from the session (server to client) - contains the respective user id 
		
	};
}

#endif
