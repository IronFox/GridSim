#ifndef systemMonitoredProcessH
#define systemMonitoredProcessH

#ifdef WIN32

#include "timer.h"
#include "../global_string.h"

namespace DeltaWorks
{

	class MonitoredProcess
	{
	private:
		PROCESS_INFORMATION	infoOut;
		Timer::Time		lastCheck = timer.Now();
		bool			isStarted = false;
		PathString		parameters,
						executablePath,
						workingDirectory;
		bool			createWindow = true;

		void			Restart();
		void			Flush();
	public:
		//bool			IsStarted() const {return isStarted;}	//deprecated. do not use. ever. use IsRunning() instead
		void			Start(const PathString&workingDirectory, const PathString&executablePath, const PathString&parametersWithoutExecutableName, bool createWindow=true);

		bool			TryResume(const PathString&workingDirectory, const PathString&executablePath);
		/**
		First checks whether a process with the specified executablePath is already running. Parameters cannot be checked reliably, so it is assumed they match.
		If a process is found then the found process is copied to the local configuration.
		*/
		void			ResumeOrStart(const PathString&workingDirectory, const PathString&executablePath, const PathString&parametersWithoutExecutableName, bool createWindow=true);
		/**
		Checks whether or not the monitored process is still running. If not, it is restarted.
		Can be called with high frequency. Will only actually check at most once per second
		*/
		void			Check();
		/**
		Checks whether or not the monitored process is still running. If not, it is restarted.
		Actually checks now. Can be slow.
		*/
		void			CheckNow();
		void			Terminate();
		/**
		Terminates and restarts the monitored process
		*/
		void			ForceRestart();
		bool			IsRunning() const;
		bool			HasAnyVisibleWindows() const;
	};
}


#endif






#endif
