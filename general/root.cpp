#include "../global_root.h"
#include "root.h"

/******************************************************************

E:\include\general\root.cpp

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

bool volatile application_shutting_down = false;

#ifndef __DLL__
	#undef main

	static char const*const*				___argv=NULL;
	static int						___argc=0;
	

	int rootCallMeToLinkMe()	//gcc only links .o files which are referenced. so in order for WinMain or main to be linked an application must reference a function or object from this file
	{
		return 0;
	}


	int			argc()
	{
		return ___argc;
	}

	const char*	argv(int index)
	{
		return index >= 0 && index<___argc ? ___argv[index] : "";
	}

	/*#if SYSTEM==WINDOWS
	int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
	{
		___argv = __argv;
		___argc = __argc;
		
		int result = Main();
		application_shutting_down = true;
		return result;
	}

	#endif*/
	/*
	int main(int argc, char *argv[])
	{
		___argv = argv;
		___argc = argc;
		
		int result = Main();
		application_shutting_down = true;
		return result;
	}*/
	//#endif

#endif
