#include "../global_root.h"
#include "exception.h"
#include "thread_system.h"
#ifdef __CUDACC__
	#include <cstdlib>
#endif


namespace Except
{
	static void defaultFatal(const FatalDescriptor&discriptor)
	{
		DISPLAY__(discriptor.ToString());
	}

	void (*onFatal)(const FatalDescriptor&exception) = defaultFatal;


	static RecursiveMutex	fatal_mutex;
	static volatile	bool	fatal_occured = false;


	void	terminateApplication()
	{
		#ifdef __CUDACC__
			exit(-1);
		#else
			#ifdef _DEBUG
				std::abort();
			#else
				#ifdef WIN32
					if (IsDebuggerPresent())
						DebugBreak();
					TerminateProcess(GetCurrentProcess(),0);	//self-terminate
				#else
					exit(-1);
				#endif
			#endif
		#endif
			/*
		#elif SYSTEM == WINDOWS
			TerminateProcess(GetCurrentProcess(), 0);
		#elif SYSTEM==UNIX
			raise(SIGKILL);
		#else
			exit(-1);
		#endif*/
	}

	void	endFatal()
	{
		fatal_mutex.release();
	}

	bool	enterFatalPhase()
	{
		if (!fatal_mutex.tryLock())
			return false;
		if (fatal_occured)
		{
			fatal_mutex.unlock();
			return false;
		}
		fatal_occured = true;
		return true;
	}

	bool	isInFatalPhase()
	{
		return fatal_mutex.isLockedByMe();
	}

	void	holyShit(const char*message)
	{
		static Mutex	mutex;

		mutex.lock();	//no unlock. display message. end program. now.

			if (message != NULL)
				DISPLAY__(message);
			
			terminateApplication();

	}

}
