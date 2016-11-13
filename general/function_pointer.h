#ifndef function_pointerH
#define function_pointerH

/******************************************************************

E:\include\general\function_pointer.h

******************************************************************/

namespace FunctionPointer   //! Various predefined function pointers
{
    typedef void (*_voidFunc)();
    typedef void (*_intFunc)(int);
    typedef void (*_voidPFunc)(void*);
    typedef const char* (*_stringQueryFunc)();


}

using namespace FunctionPointer;



#endif
