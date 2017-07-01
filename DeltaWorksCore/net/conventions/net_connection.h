#ifndef connection_conventionH
#define connection_conventionH

/*************************************************************************************

UDP based channel usage conventions

*************************************************************************************/

//#include "../additional\base_network.h"
#include "../io/net.h"

#define CON_PORT   6715
#define STC //server_to_client_send
#define CTS //client_to_server_send
//Ezyne



#ifndef CH_START
#define CH_START    0x0
#endif

#define CH_USER_IDENTITY            CTS (CH_START+0)                //currently: [name:strz] goal: (encrypted)[name:strz][password:strz]
#define CH_USER_IDENTITY_REJECTED   STC                 (CH_START+1)//signal - no data
#define CH_USER_IDENTITY_ACCEPTED   STC                 (CH_START+2)//signal - no data
#define CH_SERVER_MESSAGE           STC                 (CH_START+3)//multi-line string: [message:strz]
#define CH_CLIENT_MESSAGE                       (CH_START+4)        //single-line string: [message:strz] (broadcasted by the server)
#define CH_MODULE_DESC                          (CH_START+5)        //request-signal without data if CTS, string if STC: [desc:strz]
#define CH_CLIENT_COUNT                         (CH_START+6)        //request-signal without data if CTS, UINT32 if STC: [count:UINT32]
#define CH_CLIENT_NAMES                         (CH_START+7)        //request-signal without data if CTS, batch of strings if STC: [name0:strz][name1:strz]...[nameN:strz]
#define CH_REMOTE_COMMAND           CTS (CH_START+8)                //single-line command: [command:strz] - currently disabled


#define CH_MODULE   (CH_START+64)       //channel-offset for module-channels


#endif
