#ifndef global_stringH
#define global_stringH

#include "global_root.h"

/******************************************************************

Global string-header.

******************************************************************/

//#ifndef DSTRING_H
    #include <string.h>
    #include "string/str_class.h"
    
    #ifndef strncmpi
    
        #if SYSTEM==WINDOWS
            #define strncmpi    strnicmp
        #elif SYSTEM==UNIX
            #define strncmpi    strncasecmp
            #define strcmpi     strcasecmp
        #endif
    #endif
    

    
/*
#else
    typedef AnsiString              String;
    #define ShowMessage(message)    MessageBox(NULL,String(message).c_str(),"message",MB_OK)
    #define ErrMessage(message)     MessageBox(NULL,String(message).c_str(),"error",MB_OK)
    
#endif
*/
#ifdef DSTRING_H
    #define ShowMessage(message)    MessageBox(NULL,String(message).c_str(),"message",MB_OK)
    #define ErrMessage(message)     MessageBox(NULL,String(message).c_str(),"error",MB_OK)
#endif

#define CONSOLE_DEBUG_POINT		cout<<"["<<__FILE__<<"] "<<__func__<<" ("<<__LINE__<<")\n";

#define ShowOnce(message)	{static bool shown = false; if (!shown) { shown = true; ShowMessage(message);} }

class StrContainer
{
private:
        char        *string;
        unsigned    length;
public:
                    StrContainer():string(NULL),length(0) {}
virtual            ~StrContainer()     {dealloc(string);}
        char*       get()               {return string;}
        char*       get(unsigned long len)   {if (len != length) re_alloc(string,len); len = length; return string;}
};



inline int system(const String&str)
{
    return system(str.c_str());
}


#endif
