#include "../global_root.h"
#include "bitmap_handler.h"

/*********************************************************************

Image-interface for communication with Borland paint-areas and images.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

*********************************************************************/


#ifdef __BORLANDC__


IntStream::IntStream():TStream(),stream(NULL),cursor(0)
{
    Position = 0;
}

__fastcall IntStream::~IntStream()
{
    dealloc(stream);
}

Image*IntStream::getImage()
{
    Image*image = SHIELDED(new Image(info.biWidth,info.biHeight,info.biBitCount/8));
    image->read(stream);
    image->SwapChannels(0,2);
    return image;
}

void IntStream::setImage(const Image*origin)
{
    unsigned row_size = 3*origin->GetWidth();
    unsigned padding = (row_size%4);
    if (padding)
    {
        padding = 4-padding;
        row_size += padding;
    }

    unsigned total = row_size*origin->GetHeight();
    info.biSize = 0x28;
    info.biWidth = origin->GetWidth();
    info.biHeight = origin->GetHeight();
    info.biBitCount = 24;
    info.biPlanes = 1;
    info.biCompression = 0;
    info.biSizeImage = total;
    info.biXPelsPerMeter = 2800;
    info.biYPelsPerMeter = 2800;
    info.biClrUsed = 0;
    info.biClrImportant = 0;

    file.bfType = *(USHORT*)"BM";
    file.bfSize = 0x36+total;
    file.bfReserved1 = 0;
    file.bfReserved2 = 0;
    file.bfOffBits = 0x36;
    dealloc(stream);
    stream_size = total+14+40;
    stream = SHIELDED_ARRAY(new BYTE[stream_size],stream_size);
    memcpy(stream,&file,14);
    memcpy(&stream[14],&info,sizeof(info));
    BYTE*p = stream+14+40;
    for (unsigned row = 0; row < origin->GetHeight(); row++)
    {
        for (unsigned col = 0; col < origin->GetWidth(); col++)
        {
            const BYTE*pixel = origin->get(col,row);
            (*p++) = pixel[2];
            (*p++) = pixel[1];
            (*p++) = pixel[0];
        }
        for (unsigned i = 0; i < padding; i++)
            (*p++) = 0;
    }

    Position = 0;
    cursor = 0;
}

void IntStream::rewind()
{
    Position = 0;
    cursor = 0;
}


int __fastcall IntStream::Read(void *out, int request)
{
    memcpy(out,&stream[cursor],request);
    cursor+=request;
    return request;
}

int __fastcall IntStream::Write(const void *data, int len)
{
    cursor++;
    if (cursor == 1)
    {
        if (len != 14)
            return 0;
        memcpy(&file,data,len);
        if (file.bfType != *(USHORT*)"BM")
            return 0;
    }
    if (cursor==2)
    {
        if (len != sizeof(info))
            return 0;
        memcpy(&info,data,len);
    }
    if (cursor==3)
    {
        realloc(stream,len);
        memcpy(stream,data,len);
    }
    return len;
}

int __fastcall IntStream::Seek(int offset, Word whence)
{
    switch (whence)
    {
        case soFromBeginning:
            cursor = offset;
        break;
        case soFromCurrent:
            cursor+=offset;
        break;
        case soFromEnd:
            cursor = stream_size+offset-1;
        break;
    }
    return cursor;
}

__int64 __fastcall IntStream::Seek(const __int64 Offset, TSeekOrigin Origin)
{
    return Seek((int)Offset,(Word)Origin);
}


#endif

void _BMO_Slice(Image**IMG,en_Parameter_Type Type, float Left, float Bottom, float Right, float Top)
{
    if (Type == PT_Percent)
    {
        Left = Round(Left/100*(float)(*IMG)->GetWidth());
        Top = Round(Top/100*(float)(*IMG)->GetHeight());
        Right = Round(Right/100*(float)(*IMG)->GetWidth());
        Bottom = Round(Bottom/100*(float)(*IMG)->GetHeight());
    }
    Image*New = SHIELDED(new Image((UINT32)(Right-Left),(UINT32)(Top-Bottom),(*IMG)->GetChannels()));

    for (unsigned x = 0; x < New->GetWidth(); x++)
        for (unsigned y = 0; y < New->GetHeight(); y++)
            New->set(x,y,(*IMG)->get((unsigned)(x+Left),(unsigned)(y+Bottom)));
    DISCARD(*IMG);
    (*IMG) = New;
}

Image*_BMG_ColorFade(unsigned Width, unsigned Height,float Red1, float Green1, float Blue1, float Alpha1, float Red2, float Green2, float Blue2, float Alpha2, en_Fade_Type Type)
{
    Image*result = SHIELDED(new Image(Width,Height,4));
    TVec4<> cl1={Red1,Green1,Blue1,Alpha1},cl2={Red2,Green2,Blue2,Alpha2};
    for (unsigned x = 0; x < Width; x++)
        for (unsigned y = 0; y < Height; y++)
        {
            float factor;
            switch (Type)
            {
                case FT_Hrz:
                    factor = (float)x/(Width-1);
                break;
                case FT_Vert:
                    factor = (float)y/(Height-1);
                break;
                case FT_Explosion:
                    factor = sqrt(sqr((float)y-(float)Height/2)/sqr(Height/2)+sqr((float)x-(float)Width/2)/sqr(Width/2));
                    if (factor > 1)
                        factor = 1;
                break;
            }
            TVec4<> buffer0,buffer1;
            Vec::multiply(cl1,factor,buffer0);
            Vec::multiply(cl2,1-factor,buffer1);
            Vec::add(buffer0,buffer1);
            result->setf(x,y,buffer0.v);
        }
    return result;
}

