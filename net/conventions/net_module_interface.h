#ifndef moduleConventionH
#define moduleConventionH

/*************************************************************************************

This file is part of the DeltaWorks-Foundation or an immediate derivative core module.
Original Author: Stefan Elsen
Copyright (C) 2006 Computer Science Department of the Univerity of Trier, Germany.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

*************************************************************************************/

#include "../io/net.h"

#define MIN_REQUEST_DELAY   1

class   NetUser
{
private:
    char           *name;
public:
    NetConnection *connection;
    int             level;
    __int64         last_request,connected;
    unsigned        request_count;

//    void            setName(const char*new_name);
                    NetUser(NetConnection*conn):name(NULL),connection(conn),level(0),request_count(0),connected(timer.now())
                    {
                        setName("<generic>");
                    }
virtual            ~NetUser()
                    {}
    bool            mayRequest()
                    {
                        __int64 time = timer.now();
                        if (request_count && timer.toSecondsf(time-last_request) < MIN_REQUEST_DELAY)
                            return false;
                        request_count++;
                        last_request = time;
                        return true;
                    }
    void            setName(const char*new_name)
                    {
                        unsigned len = strlen(new_name);
                        realloc(name,len+1);
                        memcpy(name,new_name,len+1);
                    }
    const char*     GetName()
                    {
                        return name;
                    }
};

typedef void    (*_NetSendToPointer)(NetConnection*connection, UINT16 channel, UINT16 flags, const void*data, unsigned len);
typedef void    (*_NetSendToAllPointer)(UINT16 channel, UINT16 flags, const void*data, unsigned len);
typedef void    (*_NetOpenChannelPointer)(UINT16 channel, _netReceive receive);
typedef void    (*_NetCloseChannelPointer)(UINT16 channel);
typedef void    (*_ConsolePrintPointer)(const char*msg);



#endif
