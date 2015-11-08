#include "../../global_root.h"
#include "bitmap.h"

/******************************************************************

Bitmap-Format handler.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/



#define BM_RLE8     0xFF
#define BM_RLE4     0xFE

Bitmap	bitmap;



//#if SYSTEM==WINDOWS
static void    bmpResolveBitCompression(BYTE*data,unsigned data_size,BYTE*color_map,unsigned colors,Image*result,BYTE bpp)
{
    if (colors < (unsigned)(0x1<<bpp))
		throw IO::DriveAccess::FileFormatFault(globalString("Insufficient color table size"));

    unsigned 	//offset = 0,
    			x(0),y(0),remaining(data_size);
    BYTE    pixels = 8/bpp,mask(0);
    for (BYTE k = 0; k < bpp; k++)
        mask |= 0x1<<k;
    while (remaining)
    {
        remaining--;
        BYTE len = *data++;
        if (!len)
        {
            if (!remaining)
				throw IO::DriveAccess::FileFormatFault(globalString("Unexpected decompression end-of-line"));

            remaining--;
            switch (*data++)
            {
                case 0:
                    x = 0;
                    y++;
                continue;
                case 1:
                    return;
                case 2:
                    if (remaining < 2)
						throw IO::DriveAccess::FileFormatFault(globalString("Unexpected decompression end-of-line"));
                    x+=*data++;
                    y+=*data++;
                    remaining-=2;
                continue;
                default:
                    if (!remaining)
						throw IO::DriveAccess::FileFormatFault(globalString("Unexpected decompression end-of-line"));
                    len = *data++;
                    remaining--;
                break;
            }
        }
        if ((unsigned)(len/pixels) > remaining)
			throw IO::DriveAccess::FileFormatFault(globalString("Unexpected decompression end-of-line"));
        remaining-=len/pixels;
        for (BYTE k = 0; k < len/pixels; k++)
        {
            BYTE source = *data++;
            if (x>= result->getWidth() || y >= result->getHeight())
				throw IO::DriveAccess::FileFormatFault(globalString("Decompression pixel out of range"));

            for (BYTE j = 0; j < pixels; j++)
            {
                BYTE index = (source>>(8-bpp))&mask;
                source<<=bpp;
                BYTE color[3] = {color_map[index*4+2],color_map[index*4+1],color_map[index*4]};
                result->set(x,y,color);
                x++;
            }
        }
    }
}
//#endif


static 	inline void  _c3(const BYTE v[3], BYTE w[3])
	{
		w[0] = v[0];
		w[1] = v[1];
		w[2] = v[2];
	}

void	Bitmap::LoadFromFilePointer(Image&target, FILE*f)
{
#if SYSTEM!=WINDOWS

	#define BI_RGB	0
	#define BI_RLE8	1
	#define BI_RLE4	2
	#define BI_BITFIELDS	3

#endif

    FILE_READ_ASSERT_ZERO__(fseek(f,0,SEEK_END));
    ULONG true_size = ftell(f);
    FILE_READ_ASSERT_ZERO__(fseek(f,0,SEEK_SET));
    BYTE magic[2];
    read(magic,2,f);
    if (magic[0] != 'B' || magic[1] != 'M')
		throw IO::DriveAccess::FileFormatFault(globalString("Bitmap header broken: Magic bytes not 'BM'"));

    
    ULONG filesize,dummy,data_offset,header_size,width,height,compression,colors,bitmap_data_size,vres,hres;
    USHORT  planes,bpp;
	Array<BYTE>	color_map;
    read(filesize,f);
    if (filesize != true_size)
    {
			//let's tollerate this
    }
    read(dummy,f);
    read(data_offset,f);
    read(header_size,f);
    if (header_size != 0x28)
		throw IO::DriveAccess::FileFormatFault("Bitmap header broken: Header size mismatch (should be 0x28 but is 0x"+IntToHex((int)header_size,4)+")");

    read(width,f);
    read(height,f);
    read(planes,f);
    read(bpp,f);
    read(compression,f);
    switch (compression)
    {
        case BI_RGB:
        break;
        case BI_RLE8:
            bpp = BM_RLE8;
        break;
        case BI_RLE4:
            bpp = BM_RLE4;
        break;
        default:
			throw IO::DriveAccess::FileFormatFault("Bitmap uses unsupported data format "+IntToStr(compression));
    }
    read(bitmap_data_size,f);
    read(hres,f);
    read(vres,f);
    read(dummy,f);
    read(colors,f);
    if (bpp==1)
        colors = 2;
    elif (bpp==4)
        colors = 16;
    elif (bpp==8)
        colors = 256;
    if (colors)
    {
        color_map.setSize(colors*4);
        read(color_map.pointer(),4*colors,f);
    }
    if (ftell(f) != (long)data_offset)
		throw IO::DriveAccess::FileFormatFault("Data offset mismatch (should be "+IntToStr((long)data_offset)+" but is "+IntToStr(ftell(f))+")");

    if (!bitmap_data_size)
    {
        bitmap_data_size = true_size-ftell(f);
        if (!bitmap_data_size)
			throw IO::DriveAccess::FileFormatFault("Data size mismatch ("+IntToStr(bitmap_data_size)+")");
	}
	Array<BYTE>	data(bitmap_data_size);
    fread(data.pointer(),bitmap_data_size,1,f);
	target.setContentType(PixelType::Color);
    target.setDimensions(width,height,3);
    BYTE 	pixel[3],
			*out_field = target.getData();
    switch (bpp)
    {
        case 1:
        {
            if (color_map.isEmpty() || colors < 2)
				throw IO::DriveAccess::FileFormatFault(globalString("Insufficient color table size"));

            unsigned offset(0);
            for (unsigned y = 0; y < height; y++)
            {
                for (unsigned x = 0; x < width/8; x++)
                {
                    BYTE byte = data[offset++];
                    for (BYTE k = 0; k < 8; k++)
                    {
                        BYTE bit = byte&0x80;
                        byte <<= 1;
                        _c3(&color_map[(bit?1:0)*4],pixel);
                        swp(pixel[0],pixel[2]);
                        target.set(x*8+k,y,pixel);
                    }
                }
                offset = aligned(offset,4);
            }
        }
        break;
        case 4:
            if (color_map.isEmpty() || colors < 16)
				throw IO::DriveAccess::FileFormatFault(globalString("Insufficient color table size"));

            for (unsigned i = 0; i < bitmap_data_size; i++)
            {
                BYTE byte = data[i];
                for (BYTE k = 0; k < 2; k++)
                {
                    unsigned index = (i*2+k)*3;
                    if (index >= target.size())
                    {
                        return;
                    }
                    BYTE bit4 = byte&0xF0;
                    byte <<=4;
                    bit4 >>=4;
                    _c3(&color_map[(bit4&0x0F)*4],pixel);
                    swp(pixel[0],pixel[2]);
                    _c3(pixel,out_field+index);
                }
            }
        break;
        case 8:
        {
            if (color_map.isEmpty() || colors < 256)
				throw IO::DriveAccess::FileFormatFault(globalString("Insufficient color table size"));

            unsigned batch_size = height*aligned(width,4);
            if (bitmap_data_size<batch_size)
				throw IO::DriveAccess::FileFormatFault("Data size mismatch ("+IntToStr(bitmap_data_size)+")");

            unsigned offset = 0;
            for (unsigned y = 0; y < height; y++)
            {
                for (unsigned x = 0; x < width; x++)
                {
                    _c3(&color_map[data[offset++]*4],pixel);
                    swp(pixel[0],pixel[2]);
                    target.set(x,y,pixel);
                }
                offset = aligned(offset,4);
            }
        }
        break;
        case 16:
        {
            unsigned batch_size = height*aligned(width*2,4);
            if (bitmap_data_size<batch_size)
				throw IO::DriveAccess::FileFormatFault("Data size mismatch ("+IntToStr(bitmap_data_size)+")");

            unsigned offset=0;
            for (unsigned y = 0; y < height; y++)
            {
                for (unsigned x = 0; x < width; x++)
                {
                    USHORT  color = *(USHORT*)&data[offset];
                    BYTE  red = ((color&0xF800)>>11)&0x1F,
                          green = ((color&0x0FE0)>>5)&0x3F,
                          blue = (color&0x001F);
                    pixel[0] = blue *0xFF/0x1F;
                    pixel[1] = green * 0xFF/0x3F;
                    pixel[2] = red *0xFF/0x1F;
                    target.set(x,y,pixel);
                    offset+=2;
                }
                offset = aligned(offset,4);
            }
        }
        break;
        case 24:
        {
            unsigned batch_size = height*aligned(width*3,4);
            if (bitmap_data_size<batch_size)
				throw IO::DriveAccess::FileFormatFault("Data size mismatch ("+IntToStr(bitmap_data_size)+")");

            unsigned offset = 0;
            for (unsigned y = 0; y < height; y++)
            {
                for (unsigned x = 0; x < width; x++)
                {
					target.setChannel(x,y,0,data[offset+2]);
					target.setChannel(x,y,1,data[offset+1]);
					target.setChannel(x,y,2,data[offset]);
                    offset+=3;
                }
                offset = aligned(offset,4);
            }
        }
        break;
        case 32:
            if (bitmap_data_size<width*height*4)
				throw IO::DriveAccess::FileFormatFault("Data size mismatch ("+IntToStr(bitmap_data_size)+")");

            for (unsigned i = 0; i < width*height; i++)
            {
                out_field[i*3] = data[i*3+2];
                out_field[i*3+1] = data[i*3+1];
                out_field[i*3+2] = data[i*3];
            }
        break;
        case BM_RLE8:
            memset(out_field,0,target.size());
            bmpResolveBitCompression(data.pointer(),bitmap_data_size,(BYTE*)color_map.pointer(),colors,&target,8);
        break;
        case BM_RLE4:
            memset(out_field,0,target.size());
            bmpResolveBitCompression(data.pointer(),bitmap_data_size,(BYTE*)color_map.pointer(),colors,&target,4);
        break;
        default:
			throw IO::DriveAccess::FileFormatFault("Bit count not supported ("+IntToStr(bpp)+")");
    }
}



void	Bitmap::SaveToFilePointer(const Image&resource, FILE*f)
{
//#if SYSTEM==WINDOWS
    BYTE magic[2]={'B','M'};
    write(magic,2,f);

    unsigned row_size = 3*resource.getWidth();
    unsigned padding = (row_size%4);
    if (padding)
    {
        padding = 4-padding;
        row_size += padding;
    }
    unsigned image_size = row_size*resource.getHeight();

    ULONG bitmap_data_size(image_size),filesize(0x36+bitmap_data_size),dummy,data_offset(0x36),header_size(0x28),/*width,height,*/compression(0),colors(0),vres(1024),hres(1024);
    USHORT  planes(1),bpp(24);
	UINT32	image_width = resource.getWidth(),
			image_height = resource.getHeight();
    write(filesize,f);
    write(dummy,f);
    write(data_offset,f);
    write(header_size,f);
    write(image_width,f);
    write(image_height,f);
    write(planes,f);
    write(bpp,f);
    write(compression,f);
    write(bitmap_data_size,f);
    write(hres,f);
    write(vres,f);
    write(dummy,f);
    write(colors,f);
    ASSERT_EQUAL__ (ftell(f),(long)data_offset);

    BYTE pad = 0;
    for (unsigned y = 0; y < resource.getHeight(); y++)
    {
        for (unsigned x = 0; x < resource.getWidth(); x++)
        {
            const BYTE*pixel = resource.get(x,y);
            write(&pixel[2],1,f);
            write(&pixel[1],1,f);
            write(&pixel[0],1,f);
        }
        for (unsigned i = 0; i < padding; i++)
            write(&pad,1,f);
    }
}

