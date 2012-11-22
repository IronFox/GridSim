#ifndef testableH
#define testableH

#include "../global_string.h"


/******************************************************************

E:\include\general\testable.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

class Testable
{
protected:
        unsigned    iteration;
        String     report;
public:
        enum Result{Idle,Busy,Finished,Error};
        
virtual unsigned    beginTest()=0;      //returns iterations (0 for unknown)
virtual Result      test()=0;
virtual String     getName()=0;
        const String&     getReport() {return report;};
};

#endif
