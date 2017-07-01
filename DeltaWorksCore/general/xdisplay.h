#ifndef generalXDisplayH
#define generalXDisplayH

	#if SYSTEM==UNIX
	    #include <X11/Xlib.h>
	    Display*getDisplay();
	    void    closeDisplay();
	#endif

#endif
