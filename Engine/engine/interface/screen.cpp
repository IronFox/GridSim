#include "screen.h"

/******************************************************************

Collection of screen-query containers.

******************************************************************/

namespace Engine
{

	#if SYSTEM==WINDOWS
	String    mode2str(const DEVMODE&mode)
	{
	    String result;
	    if (mode.dmFields & DM_PELSWIDTH)
	        result+="width="+IntToStr(mode.dmPelsWidth)+"\n";
	    if (mode.dmFields & DM_PELSHEIGHT)
	        result+="height="+IntToStr(mode.dmPelsHeight)+"\n";
	    if (mode.dmFields & DM_DISPLAYFREQUENCY)
	        result+="frequency="+IntToStr(mode.dmDisplayFrequency)+"\n";
	    return result;
	}
	#endif



	ScreenResolution::ScreenResolution():width(0),height(0),frequency(NULL),frequencies(0)
	{}

	#if SYSTEM==WINDOWS

	    ScreenResolution::ScreenResolution(const DEVMODE&mode):width(mode.dmPelsWidth),height(mode.dmPelsHeight),frequency(NULL),frequencies(0)
	    {}

	#endif

	ScreenResolution::~ScreenResolution()
	{
	    TDisplayFrequency*f = frequency;
	    while (f)
	    {
	        frequency = f;
	        f = f->next;
	        Discard(frequency);
	    }
	}

	void ScreenResolution::insertFrequency(short rate)
	{
	    TDisplayFrequency*f = frequency,
	                       *last = NULL;
	    while (f && f->rate > rate)
	    {
	        last = f;
	        f = f->next;
	    }
	    if (f && f->rate == rate)
	        return;
	    if (last)
	    {
	        last->next = SignalNew(new TDisplayFrequency());
	        last->next->rate = rate;
	        last->next->next = f;
	    }
	    else
	    {
	        frequency = SignalNew(new TDisplayFrequency());
	        frequency->rate = rate;
	        frequency->next = f;
	    }
	    frequencies++;
	}


	bool ScreenResolution::supported(short rate) const
	{
	    TDisplayFrequency*f = frequency;
	    while (f && f->rate > rate)
	        f = f->next;
	    return f && f->rate == rate;
	}



	bool ScreenResolution::operator>(const ScreenResolution&other)  const
	{
	    return width > other.width || (width == other.width && height > other.height);
	}

	bool ScreenResolution::operator<(const ScreenResolution&other)  const
	{
	    return width < other.width || (width == other.width && height < other.height);
	}

	short ScreenResolution::closestPossibleRate(short target)           const
	{
		if (!target)
			return frequency?frequency->rate:0;
	    TDisplayFrequency*f = frequency,*final=frequency;
	    while (f && f->rate >= target)
	    {
	        final = f;
	        f = f->next;
	    }
	    return final->rate;
	}

	TDisplayMode ScreenResolution::createDisplayMode(short frequency)
	{
	    TDisplayMode result;
	    result.frequency = closestPossibleRate(frequency);
	    result.width = width;
	    result.height = height;
	    return result;
	}

	#if SYSTEM==WINDOWS



	    bool ScreenResolution::operator>(const DEVMODE&mode)  const
	    {
	        return width > mode.dmPelsWidth || (width == mode.dmPelsWidth && height > mode.dmPelsHeight);
	    }
	    
	    bool ScreenResolution::operator<(const DEVMODE&mode)  const
	    {
	        return width < mode.dmPelsWidth || (width == mode.dmPelsWidth && height < mode.dmPelsHeight);
	    }

	    void ResolutionList::insert(const DEVMODE&mode)
	    {
			Append(ScreenResolution(mode)).insertFrequency(mode.dmDisplayFrequency);
	    }

	#endif


	String ResolutionList::list()
	{
	    String result;
		foreach (*this,res)
	    {
	        result += IntToStr(res->width)+"*"+IntToStr(res->height)+" (";
	        TDisplayFrequency*f = res->frequency;
	        while (f)
	        {
	            result+=IntToStr(f->rate);
	            if (f->next)
	                result+=", ";
	            f = f->next;
	        }
	        result+=")\n";
	    }
	    return result;
	}




	FrequencyList::FrequencyList():fcnt(0)
	{}


	void  FrequencyList::insert(short frequency)
	{
	    if (fcnt == 0xFF)
	        FATAL__("frequency-list-capacity exceeded");

	    BYTE    lower = 0,
	            upper = fcnt;
	    while (lower< upper && upper <= fcnt)
	    {
	        unsigned el = (lower+upper)/2;
	        short element = flist[el];
	        if (element>frequency)
	            upper = el;
	        else
	            if (element<frequency)
	                lower = el+1;
	            else
	                return;
	    }
	    fcnt++;
	    for (BYTE k = fcnt; k > lower; k--)
	        flist[k] = flist[k-1];
	    flist[lower] = frequency;
	}

	unsigned  FrequencyList::find(short frequency)
	{
	    BYTE    lower = 0,
	            upper = fcnt;
	    while (lower< upper && upper <= fcnt)
	    {
	        unsigned el = (lower+upper)/2;
	        short element = flist[el];
	        if (element>frequency)
	            upper = el;
	        else
	            if (element<frequency)
	                lower = el+1;
	            else
	                return el;
	    }
	    return UNSIGNED_UNDEF;
	}

	short  FrequencyList::get(unsigned index)
	{
	    if (index >= (unsigned)fcnt)
	        FATAL__("illegal index-lookup");
	    return flist[index];
	}

	String  FrequencyList::list()
	{
	    String result;
	    for (BYTE k = 0; k < fcnt-1; k++)
	        result+= IntToStr(flist[k])+" hz\n";
	    if (fcnt)
	        result+=IntToStr(flist[fcnt-1])+" hz";
	    return result;
	}

	unsigned FrequencyList::Count()
	{
	    return fcnt;
	}
}
