#ifndef function_pointerH
#define function_pointerH

/******************************************************************

E:\include\general\function_pointer.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

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
