#include "../global_root.h"
#include "displayConnection.h"


#if SYSTEM_VARIANCE==LINUX
    static Display             *_display(NULL);

    Display* getDisplay()
    {
        if (!_display)
            _display = XOpenDisplay(NULL);
        return _display;
    }

    void closeDisplay()
    {
        if (_display)
            XCloseDisplay(_display);
        _display = NULL;
    }
    
#endif
