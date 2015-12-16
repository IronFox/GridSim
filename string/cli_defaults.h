#ifndef cli_defaultsH
#define cli_defaultsH

#include "cli.h"
#include "../container/hashtable.h"

/******************************************************************

E:\include\string\cli_defaults.h

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


	


}




#endif
