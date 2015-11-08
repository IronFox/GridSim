#ifndef tgaH
#define tgaH

/******************************************************************

TGA-Format handler.

******************************************************************/

#include "img_format.h"

class TGA:public ImageFormat
{
public:
					TGA():ImageFormat("TGA","tga")
					{
						magic_bytes.free();//no magic bytes
					}
					
	virtual	void	SaveToFilePointer(const Image&image, FILE*file)	override;
	virtual	void	LoadFromFilePointer(Image&image, FILE*file)	override;
};

extern TGA		tga;


#endif
