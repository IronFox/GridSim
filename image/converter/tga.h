#ifndef tgaH
#define tgaH

/******************************************************************

TGA-Format handler.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

#include "img_format.h"

class CTGA:public ImageFormat
{
public:
					CTGA():ImageFormat("TGA","tga")
					{
						magic_bytes.free();//no magic bytes
					}
					
virtual	void		saveToFilePointer(const Image&image, FILE*file);
virtual	void		loadFromFilePointer(Image&image, FILE*file);
};

extern CTGA		tga;


#endif
