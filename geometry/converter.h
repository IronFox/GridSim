#ifndef importerH
#define importerH

#include "cgs.h"
#include "../general/timer.h"

/******************************************************************

E:\include\importer\importer.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

/*
	todo:
		.md2 importer
		.map importer (quake3)
*/


namespace Converter	//! Geometry converter namespace
{

	/*!
		\brief Converter progress callback used to inform the client interface of process updates
	*/
	class ProgressCallback
	{
	public:
	virtual	void			converterProgressUpdate(const String&message)		//! Signals a new process step and indicates that the progress bar should be hidden (if any) \param message Message to print
							{};
	virtual	void			converterProgressBarUpdate(float progress)			//! Signals that the progress bar should be made visible (if any) and updated to the specified progress value (0-1) \param progress New progress bar state (0-1)
							{};
	virtual	void			hideConverterProgressBar()							//! Signals that the progress bar should be hidden
							{};
	virtual	void			dropLastConverterProgressUpdate()					//! Signals that the most recent progress update should be dropped (if the client application uses some sort of update history)
							{};
	};

	/*!
		\brief Root converter class
		
		All format dependent converter classes are required to inherit this root class.
		Conversion processes of this class and its derivatives are NOT thread safe. To import or export geometries parallely use multiple converter instances.
	*/
	class Root
	{
	protected:
			String				last_error;			//!< String representation of the last occured error. Filled in by the inheriting converter class
			ProgressCallback	default_callback,	//!< Internal (mute) callback instance
								*current_callback;	//!< Effective callback instance
			float				seconds;			//!< Number of seconds passed during the last load/save operation
	
	virtual	bool				read(CGS::Geometry<>&target, const String&filename)=0;		//!< Virtual abstract load method (defined by the inheriting converter class)
	virtual	bool				write(const CGS::Geometry<>&source, const String&filename)=0;	//!< Virtual abstract save method (defined by the inheriting converter class)
	public:
		bool				positionObjects;	//!< Indicates that loaded objects should be located in the center of the extracted vertices. True by default
	
		/**/				Root() :current_callback(&default_callback), positionObjects(true)
								{}
								
			bool            	loadFromFile(CGS::Geometry<>&structure, const String&filename, ProgressCallback*callback=NULL)	//! Attempts to import the specified geometrical file. \param structure Geometry to import into. Any existing geometrical data will be cleared \param filename Filename to load from \param callback Custom progress callback instance to report to or NULL to not generate any reports \return true on success, false otherwise
								{
									if (callback)
										current_callback = callback;
									else
										current_callback = &default_callback;
									last_error = "No Error";
									Timer::Time t = timer.now();
									bool result = read(structure,filename);
									seconds = timer.toSecondsf(timer.now()-t);
									return result;
								}
								
	        CGS::Geometry<>*  	loadFromFile(const String&filename,ProgressCallback*callback=NULL)	//! Attempts to import the specified geometrical file into a new geometry \param callback Custom progress callback instance to report to or NULL to not generate any reports \param filename Filename to load from 
		                        {
		                            CGS::Geometry<>*struc = SHIELDED(new CGS::Geometry<>());
		                            if (loadFromFile(*struc,filename,callback))
		                                return struc;
		                            DISCARD(struc);
		                            return NULL;
		                        }
			bool            	saveToFile(const CGS::Geometry<>&structure, const String&filename,ProgressCallback*callback=NULL)	//! Attempts to export the specified geometry to the specified file \param structure Geometry to export \param filename Filename to save to  \param callback Custom progress callback instance to report to or NULL to not generate any reports
								{
									if (callback)
										current_callback = callback;
									else
										current_callback = &default_callback;
									last_error = "No Error";
									Timer::Time t = timer.now();
									bool result = write(structure,filename);
									seconds = timer.toSecondsf(timer.now()-t);
									return result;
								}
	virtual const String&     	errorStr() const					//! Returns a string representation of the last occured error (if any)
								{
									return last_error;
								}
			const String&		getError()	const					//! Returns a string representation of the last occured error (if any)
								{
									return errorStr();
								}
			float				getSecondsf()	const				//! Returns the number of seconds passed during the last load/save operation
								{
									return seconds;
								}
	};
}


#endif
