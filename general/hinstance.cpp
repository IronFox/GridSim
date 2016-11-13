#include "hinstance.h"

/******************************************************************

E:\include\general\hinstance.cpp

******************************************************************/

static HINSTANCE	instance(NULL);

HINSTANCE	getInstance()
{
	#if SYSTEM==WINDOWS
		if (!instance)
			instance = GetModuleHandle(NULL);
	#endif
	return instance;
}

void		setInstance(HINSTANCE hinst)
{
	instance = hinst;
}
