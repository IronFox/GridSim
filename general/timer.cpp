#include "../global_root.h"
#include "timer.h"

/******************************************************************

Theoretically plattform independent time-query-class.
Works under linux and windows so far.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/



Timer timer;


Timer::Timer()
{
    #if SYSTEM==WINDOWS
    
        DWORD oldmask=SetThreadAffinityMask(GetCurrentThread(), 1);
        if (!QueryPerformanceFrequency((LARGE_INTEGER*)&frequency))
        {
            MessageBoxA(NULL,"not available", "timer-error",MB_OK);
            return;
        }
        QueryPerformanceCounter((LARGE_INTEGER*)&started);
        SetThreadAffinityMask(GetCurrentThread(), oldmask);
        dresolution = 1.0/(double)frequency;
        fresolution = (float)dresolution;
    #elif SYSTEM==UNIX
        frequency = 1000000;
        dresolution = 0.000001;
        fresolution = 0.000001;
        
        timeval t;
        gettimeofday(&t,NULL);
        started = frequency*t.tv_sec + t.tv_usec;
    #endif
    lastcall = started;
}


float Timer::getDeltaf()
{
    Time   time,delta;
    #if SYSTEM==WINDOWS
        DWORD oldmask=SetThreadAffinityMask(GetCurrentThread(), 1);
        QueryPerformanceCounter((LARGE_INTEGER*)&time);
        SetThreadAffinityMask(GetCurrentThread(), oldmask);
    #elif SYSTEM==UNIX
        timeval t;
        gettimeofday(&t,NULL);
        time = frequency*t.tv_sec + t.tv_usec;
    #endif
    delta=time-lastcall;
    lastcall = time;
    return fresolution*delta;
}

double  Timer::getDeltad()
{
    Time   time,delta;
    #if SYSTEM==WINDOWS
        DWORD oldmask=SetThreadAffinityMask(GetCurrentThread(), 1);
        QueryPerformanceCounter((LARGE_INTEGER*)&time);
        SetThreadAffinityMask(GetCurrentThread(), oldmask);
    #elif SYSTEM==UNIX
        timeval t;
        gettimeofday(&t,NULL);
        time = frequency*t.tv_sec + t.tv_usec;
    #endif

    delta=time-lastcall;
    lastcall = time;
    return dresolution*delta;
}

Timer::Time Timer::getDeltai()
{
    Time   time,delta;
    #if SYSTEM==WINDOWS
        DWORD oldmask=SetThreadAffinityMask(GetCurrentThread(), 1);
        QueryPerformanceCounter((LARGE_INTEGER*)&time);
        SetThreadAffinityMask(GetCurrentThread(), oldmask);
    #elif SYSTEM==UNIX
        timeval t;
        gettimeofday(&t,NULL);
        time = frequency*t.tv_sec + t.tv_usec;
    #endif
    delta = time-lastcall;
    lastcall = time;
    return delta;
}

float   Timer::getTimef()	const
{
    #if SYSTEM==WINDOWS
        Time   time;
        DWORD oldmask=SetThreadAffinityMask(GetCurrentThread(), 1);
        QueryPerformanceCounter((LARGE_INTEGER*)&time);
        SetThreadAffinityMask(GetCurrentThread(), oldmask);
        return fresolution*(time-started);
    #elif SYSTEM==UNIX
        timeval t;
        gettimeofday(&t,NULL);
        return fresolution * (frequency*t.tv_sec + t.tv_usec - started);
    #endif
}

double  Timer::getTimed()	const
{
    #if SYSTEM==WINDOWS
        Time time;
        DWORD oldmask=SetThreadAffinityMask(GetCurrentThread(), 1);
        QueryPerformanceCounter((LARGE_INTEGER*)&time);
        SetThreadAffinityMask(GetCurrentThread(), oldmask);
        return dresolution*(time-started);
    #elif SYSTEM==UNIX
        timeval t;
        gettimeofday(&t,NULL);
        return dresolution * (frequency*t.tv_sec + t.tv_usec - started);
    #endif
}

Timer::Time Timer::getTimei()	const
{
    #if SYSTEM==WINDOWS
        Time time;
        DWORD oldmask=SetThreadAffinityMask(GetCurrentThread(), 1);
        QueryPerformanceCounter((LARGE_INTEGER*)&time);
        SetThreadAffinityMask(GetCurrentThread(), oldmask);
        return time-started;
    #elif SYSTEM==UNIX
        timeval t;
        gettimeofday(&t,NULL);
        return frequency*t.tv_sec + t.tv_usec - started;
    #endif
}

Timer::Time Timer::now()	const
{
    #if SYSTEM==WINDOWS
        Time time;
        DWORD oldmask=SetThreadAffinityMask(GetCurrentThread(), 1);
        QueryPerformanceCounter((LARGE_INTEGER*)&time);
        SetThreadAffinityMask(GetCurrentThread(), oldmask);
        return time;
    #elif SYSTEM==UNIX
        timeval t;
        gettimeofday(&t,NULL);
        return frequency*t.tv_sec + t.tv_usec;
    #endif
}

Timer::Time Timer::netTime()	const
{
    #if SYSTEM==WINDOWS
        Time time;
        DWORD oldmask=SetThreadAffinityMask(GetCurrentThread(), 1);
        QueryPerformanceCounter((LARGE_INTEGER*)&time);
        SetThreadAffinityMask(GetCurrentThread(), oldmask);
        return time*1000000/frequency;
    #elif SYSTEM==UNIX
        timeval t;
        gettimeofday(&t,NULL);
        return frequency*t.tv_sec + t.tv_usec;
    #endif
}

Timer::Time			Timer::netToLocalTicks(const Time&mseconds)	const
{
	return static_cast<Time>(static_cast<double>(mseconds)*static_cast<double>(frequency) /1000000.0);

}
Timer::Time			Timer::localToNetTicks(const Time&ticks)		const
{
	return static_cast<Time>(static_cast<double>(ticks)*1000000.0*dresolution);
}


/*static*/ float           Timer::netToSecondsf(const Time&ticks)
{
    return (float)ticks/1000000.0f;
}

/*static*/ double          Timer::netToSecondsd(const Time&ticks)
{
    return (double)ticks/1000000.0;
}

/*static*/ Timer::Time          Timer::netToTicks(double seconds)
{
    return static_cast<Time>(seconds*1000000.0);
}



Timer::Time Timer::getTicksPerSecond()	const
{
    return frequency;
}

float Timer::toSecondsf(const Time&ticks)	const
{
    return fresolution*ticks;
}

Timer::Time	Timer::toTicks(double seconds)	const
{
	return (Time)(seconds*frequency);
}


double Timer::toSecondsd(const Time&ticks)	const
{
    return dresolution*ticks;
}

