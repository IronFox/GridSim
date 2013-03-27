#ifndef cli_defaultsH
#define cli_defaultsH

#include "cli.h"
#include "../container/hashtable.h"

/******************************************************************

E:\include\string\cli_defaults.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

namespace CLI
{
	typedef	void	(*pPrintln)(const String&);
	typedef	void	(*pOnFocusChange)();
	
	extern pPrintln			println;
	extern pOnFocusChange	focusChange;

	
	/**
	@brief Initializes all standard functions to the specified target interpretor
		
	Currently only one interpretor may use the standard interface at a time.
	@param target Target interpretor to register standard commands to
	@param println Pointer to a print line function that will be used to print results to a console
	@param onFocusChange Function pointer to execute if the cd command was successfuly executed
	*/
	void			InitDefaults(Interpretor&target, pPrintln println, pOnFocusChange onFocusChange=NULL, bool echoSetOperation = true);


	/**
	@brief Registers a help entry for a command or variable
		
	registerHelp() registers the specified message as help message for the specified command or variable. This message will be print out upon invocation of
	the internal 'help <command/variable>' command. Any previously registered help message for the specified object is overwritten.
		
	@param pointer Pointer to the command or variable object. May technically be of any type but should be registered to the main interpretor
	@param message String message to be passed to println if the respective command or variable is queried
	
	*/
	void	RegisterHelp(void*pointer,const String&message);
	
	/**
	@brief Unregisters a help entry.
	*/
	void	UnregisterHelp(void*pointer);
	


}




#endif
