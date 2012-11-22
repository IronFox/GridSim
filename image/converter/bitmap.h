#ifndef bitmapH
#define bitmapH
#include <stdio.h>


/******************************************************************

Bitmap-Format handler.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

#include "img_format.h"

class Bitmap:public ImageFormat
{
public:
					Bitmap():ImageFormat("Windows Bitmap","bmp")
					{
						magic_bytes.set2(0x42,0x4d);
					}
					
virtual	void		saveToFilePointer(const Image&image, FILE*file);
virtual	void		loadFromFilePointer(Image&image, FILE*file);
};

extern Bitmap	bitmap;


#endif
