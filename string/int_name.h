#ifndef int_nameH
#define int_nameH

/******************************************************************

64-bit integer-name handler.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

#include "../global_string.h"
#ifndef _MSC_VER
	#include <stdint.h>
#else
	typedef __int64 int64_t;
#endif



typedef int64_t	tName;
typedef int64_t	name64_t;

String     name2str(const int64_t&name);   //transform an int64-name into a string
int64_t     str2name(const String&str);    //transform a string into an int64-name
int64_t     str2name(const char*str);       //same for a const char-array
void        incrementName(int64_t&name);     //numerically increment the given name.
                                            /*examples: "name" -> "name0"
                                            "name0" -> "name1"
                                            "name109" -> "name110"*/


#endif
