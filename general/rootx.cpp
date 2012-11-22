#include "../global_root.h"
#include "root.h"

/******************************************************************

E:\include\general\rootx.cpp

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

#if SYSTEM==UNIX
    Display             *_display(NULL);

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
