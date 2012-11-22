#ifndef jpegH
#define jpegH

/******************************************************************

JPEG-Format handler.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

//typedef unsigned char    boolean;
//typedef int boolean;
#undef main
#if defined(__BORLANDC__) || defined(_MSC_VER)
	#define boolean int
	#undef FAR
	#define FAR
#else
	#if SYSTEM == WINDOWS
		#undef boolean
		#define boolean int
	#else
		typedef int	boolean;
	#endif
#endif
#include "../../remote/jpeg/jpeglib.h"
#include "img_format.h"

#include "../../general/root.h"	//redefine "main"


class CJPEG:public ImageFormat
{
public:
					CJPEG():ImageFormat("JPEG","jpg jpeg")
					{
						magic_bytes.set4(0xff,0xd8,0xff,0xe0);
					}
					
virtual	void		saveToFilePointer(const Image&image, FILE*file);
virtual	void		loadFromFilePointer(Image&image, FILE*file);
};

extern CJPEG		jpeg;


#endif
