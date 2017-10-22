#ifndef systemMonitoredProcessH
#define systemMonitoredProcessH

#ifdef WIN32

#include "../general/timer.h"
#include "../global_string.h"
#include "../general/thread_system.h"

namespace DeltaWorks
{

	class MonitoredProcess : private Sys::ThreadObject
	{
	private:

		struct Paths
		{
			PathString	executablePath,
						workingDirectory;
		};

		PROCESS_INFORMATION	infoOut;
		Timer::Time		lastCheck = timer.Now();

		Paths			paths;
		PathString		parameters;

		bool			createWindow = true;

		volatile bool	quitThread=false,terminateOnQuit=false,autoRestart;
		
		mutable Sys::SpinLock	faultLock;
		String			lastFault;




		void			Restart();
		void			Flush();
		void			QuitThread();


		virtual void	ThreadMain() override;
		void			ThreadedStartProcess(const Paths&);
	public:
		/**/			MonitoredProcess(bool autoRestart):autoRestart(autoRestart)	{}

		virtual			~MonitoredProcess() {QuitThread();}

		void			SetAutoRestart(bool autoRestart) {this->autoRestart = autoRestart;}
		bool			DoesAutoRestart() const {return autoRestart;}


		//bool			IsStarted() const {return isStarted;}	//deprecated. do not use. ever. use IsRunning() instead
		void			Start(const PathString&workingDirectory, const PathString&executablePath, const PathString&parametersWithoutExecutableName, bool createWindow=true);

		bool			TryResume(const PathString&workingDirectory, const PathString&executablePath);
		/**
		First checks whether a process with the specified executablePath is already running. Parameters cannot be checked reliably, so it is assumed they match.
		If a process is found then the found process is copied to the local configuration.
		*/
		void			ResumeOrStart(const PathString&workingDirectory, const PathString&executablePath, const PathString&parametersWithoutExecutableName, bool createWindow=true);
		/**
		Terminates the process if currently active
		*/
		void			Terminate();
		/**
		Ends any running observation thread without terminating the monitored application
		*/
		void			EndObservation();
		/**
		Terminates and restarts the monitored process
		*/
		void			ForceRestart();
		bool			IsRunning() const;
		bool			IsFaulted() const;
		String			GetLastFault() const;
		bool			HasAnyVisibleWindows() const;
	};
}


#endif






#endif
