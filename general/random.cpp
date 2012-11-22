#include "../global_root.h"
#include "random.h"


/******************************************************************

E:\include\general\random.cpp

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

unsigned    Random::random_mod((unsigned)time(NULL));

Random::Random()
{
    randomize();
}

Random::Random(unsigned seed):last(seed)
{}

void        Random::randomize()
{
    last = (unsigned)time(NULL)+random_mod++;
}

void        Random::setSeed(unsigned seed)
{
    last = seed;
}

void        Random::advance(unsigned queries)
{
    for (unsigned i = 0; i < queries; i++)
        last = last * 1103515245 + 12345;
}

unsigned    Random::currentSeed() const
{
	return last;
}

unsigned    Random::getSeed()
{
    last = last * 1103515245 + 12345;
    return last;
}

unsigned    Random::get()
{
    last = last * 1103515245 + 12345;
    return((unsigned)(last>>16) & mask);
}

unsigned    Random::get(unsigned max)
{
    return get()*(max+1)/(resolution);
}

index_t    Random::getIndex(index_t max)
{
    return get()*(max+1)/(resolution);
}

int    Random::get(int min, int max)
{
    return min+get()*(max-min+1)/(resolution);
}

float       Random::getFloat()
{
    return (float)get()/(float)(resolution-1);
}

float       Random::getFloat(float max)
{
    return (float)get()*max/(float)(resolution-1);
}

float       Random::getFloat(float min, float max)
{
    return min+(float)get()*(max-min)/(float)(resolution-1);
}

double       Random::getDouble()
{
    return (double)get()/(double)(resolution-1);
}

double       Random::getDouble(double max)
{
    return (double)get()*max/(double)(resolution-1);
}

double       Random::getDouble(double min, double max)
{
    return min+(double)get()*(max-min)/(double)(resolution-1);
}
