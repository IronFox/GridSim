#include "../global_root.h"
#include "monitoredProcess.h"
#include "../io/file_system.h"
#include "../container/buffer.h"

#ifdef WIN32

#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <winternl.h>
//#include <iostream>


static DWORD		MapPathToProcess(const FileSystem::File&file)
{
	FileSystem::Folder folder;
	PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	//StringW out;

    if (Process32First(snapshot, &entry) == TRUE)
    {
        do
        {
			if (file.GetName().equalsIgnoreCase(entry.szExeFile))
			{
				MODULEENTRY32 mentry;
				mentry.dwSize = sizeof(MODULEENTRY32);
				HANDLE msnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,entry.th32ProcessID);
				if (Module32First(msnapshot,&mentry) == TRUE)
				{
					FileSystem::File mfile;
					if (folder.FindFile(mentry.szExePath,mfile))
					{
						if (mfile.GetLocation().equalsIgnoreCase(file.GetLocation()))
						{
							CloseHandle(msnapshot);
							CloseHandle(snapshot);
							return mentry.th32ProcessID;
						}
					}
					else
						ShowMessage(String(mentry.szExePath));
				}
				CloseHandle(msnapshot);
			}
			//out += entry.szExeFile;
			//out += L'\n';
			//std::wcout << entry.szExeFile<<std::endl;
        }
		while  (Process32Next(snapshot, &entry) == TRUE);
    }
    CloseHandle(snapshot);
	//ShowMessage("not found: "+String(file.GetName()));
	//ShowMessage(String(out));
	return NULL;
}


struct TEnumParameter
{
    DWORD	process;
	Buffer0<HWND>	*outWindows;
};

static BOOL CALLBACK Enumerate(HWND hWnd, LPARAM lParam)
{
    TEnumParameter*p = (TEnumParameter*)lParam;
    DWORD process = 0x0;
    GetWindowThreadProcessId( hWnd, &process );
    if (p->process == process)
	{
        p->outWindows->Append(hWnd);
    }
    return TRUE;
}

static void EnumerateWindows(DWORD process, Buffer0<HWND>&outWindows)
{
    TEnumParameter p;
	p.process = process;
	p.outWindows = &outWindows;
    EnumWindows( Enumerate, (LPARAM)&p );
}



bool		MonitoredProcess::ResumeOrStart(const PathString&workingDirectory, const PathString&executablePath, const PathString&parametersWithoutExecutableName, bool createWindow/*=true*/)
{
	FileSystem::Folder folder;
	FileSystem::File file;
	ASSERT1__(folder.FindFile(executablePath,file),executablePath);
	DWORD process = MapPathToProcess(file);
	if (process == NULL)
		return Start(workingDirectory,executablePath,parameters, createWindow);

	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process);
	if (processHandle == INVALID_HANDLE_VALUE)
	{
		return Start(workingDirectory,executablePath,parameters, createWindow);
	}
	Terminate();
	Flush();
	infoOut.dwProcessId = process;
	infoOut.hProcess = processHandle;
	this->executablePath = file.GetLocation();
	this->workingDirectory = file.GetFolder();
	this->createWindow = createWindow;
	this->isStarted = true;
	return true;
}


bool		MonitoredProcess::Start(const PathString&workingDirectory, const PathString&executablePath, const PathString&parametersWithoutExecutableName, bool createWindow)
{
	Terminate();
	FileSystem::Folder f(workingDirectory);
	if (!f.IsValidLocation())
		return false;
	FileSystem::File found;
	if (!f.FindFile(executablePath,found))
		return false;
	parameters = '"'+FileSystem::ExtractFileNameExt(executablePath)+"\" "+parametersWithoutExecutableName;
	this->executablePath = found.GetLocation();
	this->workingDirectory = f.GetLocation();
	this->createWindow = createWindow;
	return Restart();
}


bool	MonitoredProcess::Restart()
{
	ASSERT__(!isStarted);
	isStarted = true;
	
//	char parameterBuffer[32768];

	//sprintf_s(parameterBuffer,"\"Console Client" PLATFORM_TARGET_NAME_EXTENSION_STR ".exe\" aiAccessToken='%s' udpListen=false serverURL=localhost",accessToken.ToFullString().c_str());


	STARTUPINFOW	infoIn;
	ZeroMemory(&infoIn,sizeof(infoIn));
	infoIn.cb = sizeof(infoIn);

	if (!createWindow)
	{
		infoIn.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		infoIn.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		infoIn.hStdOutput =  GetStdHandle(STD_OUTPUT_HANDLE);
		infoIn.hStdError = GetStdHandle(STD_ERROR_HANDLE);
		infoIn.wShowWindow = SW_HIDE;
	}

	//infoIn.wShowWindow = SW_MINIMIZE;
	//infoIn.dwFlags |= STARTF_USESHOWWINDOW;	//dx11 doesn't start well minimized

	infoIn.dwXSize = 100;
	infoIn.dwYSize = 100;
	//infoIn.dwFlags |= STARTF_USESIZE;

	DWORD flags = 0;
	#ifdef _DEBUG
		//flags |= DEBUG_PROCESS;
	#endif
	flags |= CREATE_NEW_CONSOLE;
	if (!createWindow)
		flags |= CREATE_NO_WINDOW;
			
	BOOL rs = CreateProcessW(executablePath.c_str(),parameters.mutablePointer(),NULL,NULL,FALSE,flags,NULL,workingDirectory.c_str(),&infoIn,&infoOut);
	return rs != 0;
}


bool		MonitoredProcess::IsRunning()	const
{
	if (!isStarted)
		return false;
	DWORD code;
	return (GetExitCodeProcess(infoOut.hProcess,&code)!=0 &&  code == STILL_RUNNING);
}

void		MonitoredProcess::Check()
{
	if (timer.GetSecondsSince(lastCheck) > 1)
	{
		CheckNow();
	}	
}

void	MonitoredProcess::CheckNow()
{
	if (!IsRunning())
	{
		bool wasStarted = isStarted;
		Flush();
		if (!wasStarted)
			return;
		Restart();
	}
	lastCheck = timer.Now();
}

void	MonitoredProcess::ForceRestart()
{
	Terminate();
	Restart();
}


void MonitoredProcess::Flush()
{
	CloseHandle(infoOut.hProcess);
	CloseHandle(infoOut.hThread);
	infoOut.hProcess = INVALID_HANDLE_VALUE;
	infoOut.hThread = INVALID_HANDLE_VALUE;
	infoOut.dwThreadId = 0;
	infoOut.dwProcessId = 0;
	isStarted = false;
}

void		MonitoredProcess::Terminate()
{
	if (isStarted)
	{
		isStarted = false;
		Buffer0<HWND>	windows;
		EnumerateWindows(infoOut.dwProcessId,windows);
		foreach (windows,w)
		{
			DWORD threadID = GetWindowThreadProcessId(*w,NULL);
			PostThreadMessageA(threadID,WM_DESTROY,0,0);
		}
		if (WaitForSingleObject(infoOut.hProcess,2000) != 0)
			TerminateProcess(infoOut.hProcess,0);
		Flush();
	}
}

#endif

