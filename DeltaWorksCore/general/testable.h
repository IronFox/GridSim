#ifndef testableH
#define testableH

#include "../global_string.h"


/******************************************************************

E:\include\general\testable.h

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
virtual String     GetName()=0;
        const String&     getReport() {return report;};
};

#endif
