#ifndef X11_displayConnectionH
#define X11_displayConnectionH

#if SYSTEM_VARIANCE==LINUX
    #include <X11/Xlib.h>

    extern Display* getDisplay();
    extern void closeDisplay();
    
#endif




#endif
