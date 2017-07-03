#include "hinstance.h"

/******************************************************************

E:\include\general\hinstance.cpp

******************************************************************/

namespace DeltaWorks
{

	static HINSTANCE	instance(NULL);

	HINSTANCE	GetInstance()
	{
		#if SYSTEM==WINDOWS
			if (!instance)
				instance = GetModuleHandle(NULL);
		#endif
		return instance;
	}

	void		SetInstance(HINSTANCE hinst)
	{
		instance = hinst;
	}
}
