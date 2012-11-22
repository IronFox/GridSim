#ifndef rootH
#define rootH



#include "hinstance.h"
#include "../container/array.h"



extern volatile bool application_shutting_down;

#if defined(__DLL__) && __DLL__!=0
	//#define _DLL
#else
	#undef __DLL__
#endif



#ifndef __DLL__
/*
	int 		_cdecl Main();

	int			argc();						//!< Returns the number of arguments passed to this application. Should return at least 1.
	const char*	argv(int index);			//!< Returns a specific argument. The first argument (index 0) is the command name as evoked by the shell.
	int			rootCallMeToLinkMe();		//!< Dummy function, causing certain variables and functions to be linked if evoked by the main program file.
	*/
	#if SYSTEM==UNIX
	    #include <X11/Xlib.h>
	    Display*getDisplay();
	    void    closeDisplay();
	#endif

	#if 0
		#if !defined(__BORLANDC__) && !defined(_DLL)
			static int __local_link_dummy__ = rootCallMeToLinkMe();
		#endif
	#endif




	#undef ProgramMain
	#define ProgramMain    __local_link_dummy__ = rootCallMeToLinkMe(); int _cdecl Main

	/*#undef main
	#define main    __local_link_dummy__ = rootCallMeToLinkMe(); int _cdecl Main		//deprecated*/

	#define ProgamMain	you might want to check your spelling
#else



#endif


#endif

