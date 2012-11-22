#ifndef rpp_netH
#define rpp_netH

#include <ctype.h>
#include "../global_string.h"
#include "../net/node.h"
#include "../list/binarytree.h"
#include "../list/sorter.h"
#include "../general/system.h"
#include "../structure/ref.h"
#include "../list/production_pipe.h"
#include "../general/testable.h"


/******************************************************************

E:\include\io\tcp.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

#define TCP_PORT            2049
#define TCP_ENCODE_CHAR     '%'



namespace TCP
{
    class Client
    {
    public:
            typedef void (*Event)();


            Event   OnDisconnect;

            bool    send(const String&line)
            {
                return false;
            }

            bool    connectTo(const String&addr)
            {
                return false;
            }
            bool    online()
            {
                return false;
            }
            




    };



/*
    class Connection;
    class Server;
    class Client;
    class Event;
    class MessageHandle;
    class Interpretor;


    typedef void(*_link)(const String*parameter, Client*origin);
    typedef void(*_clientDisconnected)(Client*);
    typedef void(*_clientConnected)(Client*);
    typedef void(*_event)();


    class InterpretorRoot:public virtual Testable
    {
    private:
            BinaryTree<MessageHandle,NameSort>  list;
            String                 dialect_str;
            ProductionPipe<Event> event_pipe;
            Vector<Event>         hold_queue;
            bool                    hold;

            unsigned                test_iteration;
            String                 test_report;
            static int              test_handled;
            static void             testLink(const String*p, Client*origin);

            friend class Interpretor;
    public:
                            InterpretorRoot();
    virtual                ~InterpretorRoot();

            void            link(const String&name, _link function, bool holdable=true);
            void            link(const String&name, const String&p0, _link function, bool holdable=true);
            void            link(const String&name, const String&p0, const String&p1, _link function, bool holdable=true);
            void            link(const String&name, const String&p0, const String&p1, const String&p2, _link function, bool holdable=true);
            void            link(const String&name, const String&p0, const String&p1, const String&p2, const String&p3, _link function, bool holdable=true);
            void            link(const String&name, const String&p0, const String&p1, const String&p2, const String&p3, const String&p4, _link function, bool holdable=true);
            void            dispatchEvents();
            void            holdEvents();           //enable holding
            void            dispatchHeldEvents();   //disable holding and release buffered events
            String         dialect();

            Testable::Result   test();
            unsigned            beginTest();
            String             getName();
    };

    class Interpretor:public virtual Testable
    {
    private:
            InterpretorRoot       &root;

            char                    net_buffer[32768];
            unsigned                oversize;

            friend class Client;
            friend class Server;
            friend class Connection;
            friend class InterpretorRoot;

            Event*         resolve(char*buffer_section,Client*origin);

    public:
                            Interpretor(InterpretorRoot&);
    virtual                ~Interpretor();

            void            resolveTest(const String&message);

            void            parseEvents(unsigned buffer_usage, Client*origin);

            Testable::Result   test();
            unsigned            beginTest();
            String             getName();
    };


    class Connection:private Interpretor,public InterpretorRoot, public virtual Testable
    {
    private:
            SOCKET          node;
            Thread         read_thread;
            sockaddr_in     address;
            bool            disconnected;

            bool            connectTo(const sockaddr_in&target);
    static  int             read(void*);
    public:
            enum            Error{NoError,InitFailed,URILookupFailed,SocketCreationFailed,ConnectFailed};
            Error           error;
            _event          OnConnectionLost;

                            Connection();
    virtual                ~Connection();
            bool            connectTo(const String&url, USHORT port=RPP_PORT);
            String         errorStr();
            void            disconnect();
            void            send(const String&message);
            void            send(const char*message);
            void            send(const char*message, unsigned len);
            bool            online();
            void            resolve();
            UINT32          getIP();
            String         addressStr();

            Interpretor::resolveTest;

            Testable::Result   test();
            unsigned            beginTest();
            String             getName();

    };


    class ClientData
    {
    public:
    virtual                    ~ClientData();
    };

    class Client:private Interpretor, public Reference<ClientData>
    {
    private:
            sockaddr_in         address;
            SOCKET              node;
            Server            *server;
            Thread             read_thread;
            Mutex              mutex;

    static  int                 read(void*);
    friend  class   Server;
    public:
                                Client(SOCKET, const sockaddr_in&, Server*);
    virtual                    ~Client();
            void                send(const String&str);
            void                send(const char*message);
            void                send(const char*message, unsigned len);

            const sockaddr_in&  getAddress();
    };

    class Server:public InterpretorRoot, public virtual Testable
    {    private:
            Vector<Client>    clients;
            ReferenceVector<Client>   disconnected;
            ReferenceVector<Client>   connected;
            SOCKET              node;
            Thread             read_thread;
            Mutex              mutex,connect_mutex;
            USHORT              connected_port;

    static  int                 read(void*);
            void                notifyDisconnect(Client*);

            friend class Client;

    public:
            enum            Error{NoError,InitFailed,SocketCreationFailed,SocketBindFailed};
            Error           error;

            _clientDisconnected OnDisconnect;
            _clientConnected    OnConnect;

                            Server();
    virtual                ~Server();
            bool            startService(USHORT port=RPP_PORT);
            String         errorStr();
            void            disconnect();
            bool            online();
            void            resolve();
            USHORT          getPort();

            unsigned        countClients();
            Client*        getClient(unsigned index);
            void            signalClientRead();
            Client*            getClientSignaled(unsigned index);
            void            exitClientRead();

            Testable::Result   test();
            unsigned            beginTest();
            String             getReport();
            String             getName();
    };


    class Event
    {
    private:
            _link           target;
            String        *data;
            Client        *origin;
            bool            holdable;
            friend class MessageHandle;
    public:
                            Event(_link link, unsigned sections, bool holdable, Client*origin);
    virtual                ~Event();
            bool            execute(bool hold);
    };

    class MessageHandle
    {
    private:
            struct Section
            {
                String     name;
                unsigned    index;
            };

            _link           link;
            Section        *section;
            unsigned        sections;
            bool            holdable;
            friend class Interpretor;
            friend class InterpretorRoot;


            unsigned        indexOf(const char*str);
            Event*         resolve(char*buffer_section, Client*origin);
    public:
            String         name;

                            MessageHandle(const String&name, _link target, bool holdable);
                            MessageHandle(const String&name, const String&p0,_link target, bool holdable);
                            MessageHandle(const String&name, const String&p0, const String&p1,_link target, bool holdable);
                            MessageHandle(const String&name, const String&p0, const String&p1, const String&p2,_link target, bool holdable);
                            MessageHandle(const String&name, const String&p0, const String&p1, const String&p2, const String&p3,_link target, bool holdable);
                            MessageHandle(const String&name, const String&p0, const String&p1, const String&p2, const String&p3, const String&p4, _link target, bool holdable);
    virtual                ~MessageHandle();

            String         toString();
    };




    String     encode(const String&str);
    String     decode(const String&str);
    void        testMessage();
    
    */
}


#endif
