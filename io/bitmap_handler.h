#ifndef bitmap_handlerH
#define bitmap_handlerH

/*********************************************************************

Image-interface for communication with Borland paint-areas and images.

*********************************************************************/


#include "../image/image.h"
#include "../math/vector.h"

#if SYSTEM==WINDOWS
    #include <wingdi.h>
#endif


class IntStream;
enum  en_Parameter_Type {PT_TrueCoords,PT_Percent};
enum  en_Fade_Type      {FT_Hrz,FT_Vert,FT_Explosion};






//Bitmap_Operations

void _BMO_Slice(Image**IMG,en_Parameter_Type Type, float Left, float Top, float Right, float Bottom);


//Bitmap_Generators

Image*_BMG_ColorFade(unsigned Width, unsigned Height,float Red1, float Green1, float Blue1, float Alpha1, float Red2, float Green2, float Blue2, float Alpha2, en_Fade_Type Type);

//Classes

#ifdef __BORLANDC__

class IntStream:TStream
{
private:
        tagBITMAPINFOHEADER info;
        tagBITMAPFILEHEADER file;
        unsigned cursor;
public:
        unsigned    stream_size;
        BYTE       *stream;
                    IntStream();
virtual __fastcall ~IntStream();
	int __fastcall  Read(void *Buffer, int Count);
	int __fastcall  Write(const void *data, int len);
	int __fastcall  Seek(int Offset, Word Origin);
    __int64 __fastcall Seek(const __int64 Offset, TSeekOrigin Origin);

    Image         *getImage();
    void            setImage(const Image*origin);
    void            rewind();
};
#endif

#endif
