#ifndef systemMonitoredProcessH
#define systemMonitoredProcessH

#ifdef WIN32

#include "timer.h"
#include "../global_string.h"

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

	bool			Restart();
	void			Flush();
public:
	bool			IsStarted() const {return isStarted;}
	bool			Start(const PathString&workingDirectory, const PathString&executablePath, const PathString&parametersWithoutExecutableName, bool createWindow=true);
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
};

#endif






#endif
