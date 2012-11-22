#include "hinstance.h"

/******************************************************************

E:\include\general\hinstance.cpp

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

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
