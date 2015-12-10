#ifndef progress_logH
#define progress_logH

/******************************************************************

progress logger unit.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include "../global_string.h"
#include "../container/queue.h"
#include "../container/stack.h"
#include "../math/basic.h"

#include "../gl/gl.h"


#define PROGRESS_LOG_LINES          20


#define PROG_LH                 (1.0f/PROGRESS_LOG_LINES)



namespace Progress	//! Progress log and visualizer namespace
{



    struct  tLogMinor;
    struct  tLogLine;
    struct  tQueueFragment;
    struct  tQueueEnterMinor;
    struct  tQueueExitMinor;
    struct  tQueueSetState;
    struct  tQueueEnterStep;
    struct  tQueueExitStep;
    
    class   Log;
    class   Visualizer;



    void nameLine(tLogLine&line, const String&str);
    void nameLine(tLogLine&line, const char*str);

    class Visualizer	//! Virtual progress log visualizer
    {
    public:
    virtual                    ~Visualizer();
    virtual void                init(const String&caption)=0;	//!< Init visual output \param caption General log caption
    virtual void                update(Log*log)=0;				//!< Update visual output \param log Pointer to the progress log object that should be visualized
    };
    
    struct tLogMinor	//! A 'minor' specifies a minor progress step which is only visualized by a bar
    {
        float       offset,		//!< initial progress bar status
					window;		//!< progress bar range for this minor
        size_t		state,		//!< current progress state
					len;		//!< number of possible different progress states
    };
        
    
    struct  tLogLine	//! Output line in the progress log
    {
        char        line[0x100];	//!< character buffer
        size_t		line_len,		//!< character buffer Fill state
					depth;			//!< line indentation (0 = no indentation)
    };


    class Log	//! Progress log object
    {
    private:
            bool                        active;
            Visualizer                *vis;

    public:

            tLogLine                    line[PROGRESS_LOG_LINES];		//!< line buffer
            unsigned                    lines,							//!< number of visible lines
										step_depth,						//!< current indentation
										bar_line;						//!< index of the line, the progress bar is currently attached to	

            Stack<tLogMinor,0x100>     stack;							//!< sub state stack
            tLogMinor                   top;							//!< current sub state



                                        Log();
            void                        enterMinor(unsigned len);       //!< Enters minor log state \param len Number of steps in the minor
            void                        exitMinor();                    //!< Returns to the next superior log state
            void                        setState(unsigned state);       //!< Updates the local minor state \param state New state. Expected less or equal to the number of steps in the minor
            void                        incState();                                         //!< Increases the local minor state by one
            void                        enterStep(const String&name, bool attach=false);   //!< Enters a new sub step \param name Name of the step \param attach Specfies whether or not the progress bar should be attached to the line
            void                        exitStep();                                         //!< Returns to the next superior step
            void                        addStep(const String&name, bool attach=false);     //!< Adds a new step at the same level \param name Name of the step \param attach Specfies whether or not the progress bar should be attached to the line
    template <class C>
            void                        execute(const String&name, Visualizer&visualizer, void (*func_)(C&), C& param);	//!< Begins visual output and executes the specified function pointer using the specified parameter \param name General caption of the progress log \param visualizer Virtual visualizer to use \param func_ Function pointer to execute once initialized \param param Parameter to pass to \b func_
            void                        begin(const String&name, Visualizer&visualizer);	//!< Initializes visual output \param name Topmost caption \param visualizer Reference to a virtual visualizer to use as graphical output
            void                        close();											//!< Close output
            float                       progress();											//!< Retrieves the current progress \return Current progess (usually in the range 0...1)
    };
}



#include "progress_log.tpl.h"

#endif

