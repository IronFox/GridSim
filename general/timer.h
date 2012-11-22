#ifndef timerH
#define timerH

/******************************************************************

Theoretically plattform independent time-query-class.
Works under linux and windows so far.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

#if SYSTEM==UNIX
	#include <sys/time.h>
#endif

/*!
	\brief Accurate timer class

	The Timer class can be used to meassures time in microseconds.
*/
class Timer
{
private:
		float			fresolution;
		double			dresolution;
		UINT64			frequency,
						started,
						lastcall;
public:
		typedef UINT64	Time;				//!< Time container. Currently using unsigned 64 bit integers

						Timer();
		float			getDeltaf();		//!< Query time since last call in seconds (float) \return Time since last call in seconds
		double			getDeltad();		//!< Query time since last call in seconds (double) \return Time since last call in seconds
		Time			getDeltai();		//!< Query time since last call in ticks \return Time since last call in ticks
		float			getTimef()								const;	//!< Query passed time since object creation \return Time in seconds since object was created
		double			getTimed()								const;	//!< Query passed time since object creation \return Time in seconds since object was created
		Time			getTimei()								const;	//!< Query passed time since object creation \return Time in ticks since object was created
		Time			now()									const;	//!< Query native system time \return Current system time in ticks (zero time unknown)
		Time			netTime()								const;	//!< Query native system time in microseconds \return Current system time in micro seconds
		Time			getTicksPerSecond()						const;	//!< Query number of ticks per second \return Number of ticks per second (always 1000000 on unix)
		float			toSecondsf(const Time&ticks)			const;	//!< Converts ticks into seconds (float) \param ticks Time in ticks \return Time equivalent in seconds
		double			toSecondsd(const Time&ticks)			const;	//!< Converts ticks into seconds (double) \param ticks Time in ticks \return Time equivalent in seconds
		Time			toTicks(double seconds)					const;	//!< Converts seconds into ticks \param seconds Time (in seconds) to convert to ticks \return Time equivalent in ticks
		
		Time			netToLocalTicks(const Time&mseconds)	const;
		Time			localToNetTicks(const Time&ticks)		const;
static	float			netToSecondsf(const Time&mseconds);	//!< Converts net time (micro seconds) into seconds (float) \param mseconds Time in micro seconds \return Time in seconds
static	double			netToSecondsd(const Time&mseconds);	//!< Converts net time (micro seconds) into seconds (double) \param mseconds Time in micro seconds \return Time in seconds
static	Time			netToTicks(double seconds);			//!< Converts net time (in seconds) into ticks (micro seconds)	@param seconds Time (in seconds) to convert to net ticks @return Time equivalent in net ticks
};





extern Timer timer;


#endif
