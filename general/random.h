#ifndef prandomH
#define prandomH

#include <time.h>
#include <math.h>
#include "../global_root.h"

/******************************************************************

E:\include\general\random.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

/*!
	\brief Random generator class
	
	The Random class generates a sequence of pseudo random number in the range
	[0,Random::resolution).
	Each Random object will auto randomize using the current time unless a custom
	seed is specified. The returned random sequence is deterministic, meaning it will produce the exact
	same sequence if starting at the same seed position.
	Although Random can produce only 2^15 different outputs, its internal number of states ranges at at least 2^32
*/
class Random
{
private:
static	unsigned	random_mod;
		unsigned	last;						//!< Up to 2^32 different internal states.
public:
static	const UINT	resolution=32768,			//!< Number of different random values.
					mask = resolution-1;		//!< Bits allowed as random values


					Random();					//!< Constructs a new randomized Random object
					Random(unsigned seed);		//!< Constructs a new Random object \param seed Custom seed to start from
		void		randomize();				//!< Rerandomizes using the current time
		void		setSeed(unsigned seed);		//!< Relocates seed \param seed New custom seed to continue from
		
		void		advance(unsigned queries);	//!< Skip the next n random queries \param queries Number of queries to skip in the sequence

		unsigned	currentSeed()	const;		//!< Query seed \return Current seed
		unsigned	getSeed();					//!< Query and advance seed \return Current seed
		unsigned	get();						//!< Query random value \return Random value ranging [0,resolution-1]
		unsigned	get(unsigned max);			//!< Query random value \param max Maximum random value \return Random value ranging [0,max]
		index_t		getIndex(index_t max);			//!< Query random value \param max Maximum random value \return Random value ranging [0,max]
		int			get(int min, int max);		//!< Query random value \param min Minimum random value \param max Maximum random value \return Random value ranging [min,max]
		


		float		getFloat();						//!< Query random float value \return Random float value ranging [0.0f,1.0f]
		float		getFloat(float max);			//!< Query random float value \param max Maximum random value \return Random float value ranging [0,max]
		float		getFloat(float min, float max); //!< Query random float value \param min Minimum random value \param max Maximum random value \return Random float value ranging [min,max]

		double		getDouble();						//!< Query random double value \return Random double value ranging [0.0,1.0]
		double		getDouble(double max);				//!< Query random double value \param max Maximum random value \return Random double value ranging [0,max]
		double		getDouble(double min, double max);	//!< Query random double value \param min Minimum random value \param max Maximum random value \return Random double value ranging [min,max]

	template <class Type>
		Type		getType()							//!< Query custom type random value \return Random value ranging [0,1]
		{
			return (Type)get()/(Type)(resolution-1);
		}
	template <class Type>
		Type		getType(Type max)					//!< Query custom type random value \param max Maximum random value \return Random value ranging [0,max]
		{
			return (Type)get()*max/(Type)(resolution-1);
		}
	template <class Type>
		Type		getType(Type min, Type max)			//!< Query custom type random value \param min Minimum random value \param max Maximum random value \return Random value ranging [min,max]
		{
			return min+(Type)get()*(max-min)/(Type)(resolution-1);
		}

	template <typename Type>
		void		getGaussian(Type&y0, Type&y1)	//! Retrieves two gaussian distributed random values
		{
			Type w,x0,x1;
			do
			{
				x0 = Type(2) * getType<Type>() - Type(1);
				x1 = Type(2) * getType<Type>() - Type(1);
				w = x0*x0 + x1*x1;
			}
			while (w >= 1);
			w = sqrt((Type(-2) * log(w))/w);
			y0 = x0*w;
			y1 = x1*w;

		}

};





#endif
