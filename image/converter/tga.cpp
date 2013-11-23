#include "../../global_root.h"
#include "tga.h"


/******************************************************************

TGA-Format handler.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

CTGA	tga;


#define TGA_EMPTY       0
#define TGA_MAPPED      1
#define TGA_RGB         2
#define TGA_WB          3
#define TGA_RLE_MAPPED  9
#define TGA_RLE_RGB     10
#define TGA_RLE_WB      11
#define TGA_CPR_MAPPED  32
#define TGA_CPR2_MAPPED 33



static void tgaColor(const Array<BYTE>&color, index_t offset, BYTE bpp, BYTE*out)
{
    if (bpp >= 24)
    {
        out[0] = color[offset+2];
        out[1] = color[offset+1];
        out[2] = color[offset+0];
		if (bpp>24)
			out[3] = color[offset+3];	//this will require testing
    }
    else
    {
		ASSERT_LESS__(offset+1,color.length());
        USHORT col = *(USHORT*)(color + offset);
        out[0] = ((col>>10)&(0x1F))*255/0x1F;
        out[1] = ((col>>5)&(0x1F))*255/0x1F;
        out[2] = ((col & 0x1F))*255/0x1F;
    }
}

static USHORT tgaIndex(BYTE*index, BYTE bpp)
{
    if (bpp > 8)
        return (*(USHORT*)index >>(16-bpp))&((0x1<<bpp)-1);
    return ((*index) >> (8-bpp)) & ((0x1<<bpp)-1);
}

static void tgaLinearPixel(unsigned index,Image&target,BYTE*color)
{
    unsigned x = index % target.getWidth(),
             y = index / target.getWidth();
    if (y >= target.getHeight())
        return;
    target.set(x,y,color);
}





void	CTGA::loadFromFilePointer(Image&target, FILE*f)
{
	Array<BYTE>	color_map,
					final_map,
					image;

    FILE_READ_ASSERT_ZERO__(fseek(f,0,SEEK_END));
    unsigned fsize = ftell(f);
    FILE_READ_ASSERT_ZERO__(fseek(f,0,SEEK_SET));
    if (fsize <18)
		throw IO::DriveAccess::FileFormatFault(globalString("File size mismatch"));

    BYTE type,ident_size,map_type,/*image_type,*/map_bpp,image_bpp,image_flags,
         attrib_bits,orientation,interleaving,map_stride;
    USHORT map_first,map_len,x_base,y_base,width,height;
    read(ident_size,f);
    read(map_type,f);
    read(type,f);
    if (type == TGA_EMPTY)
    {
		target.setContentType(PixelType::Color);
        target.setDimensions(0,0,3);
        return;
    }
    bool valid = (type < 4 || (type > 8 && type < 12) || type == 32 || type==33);
    if (!valid)
		throw IO::DriveAccess::FileFormatFault("Unsupported type: "+String(type));

    bool mapped = type == TGA_MAPPED || type == TGA_RLE_MAPPED || type > 12;
    if (mapped && !map_type)
		throw IO::DriveAccess::FileFormatFault(globalString("Color map missing"));

    
    read(map_first,f);
    read(map_len,f);
    read(map_bpp,f);
    map_stride = map_bpp/8;
    if (map_type && mapped && (map_bpp%8 || map_stride<2 || map_stride>4))
		throw IO::DriveAccess::FileFormatFault("Invalid bit per color value: "+IntToStr(map_bpp));

    read(x_base,f);
    read(y_base,f);
    read(width,f);
    read(height,f);
    read(image_bpp,f);
    read(image_flags,f);
    attrib_bits = (image_flags>>0x5)&(0x7);
    orientation = (image_flags>>0x3)&(0x1);
    interleaving = (image_flags&0x3);
    FILE_READ_ASSERT_ZERO__(fseek(f,ident_size+(unsigned)map_first*map_stride,SEEK_CUR));
    unsigned map_size = (unsigned)map_len*map_stride;
    if (map_size+18 > fsize)
		throw IO::DriveAccess::FileFormatFault(globalString("File size mismatch"));

    if (map_type)
    {
        color_map.setSize(map_size);
        final_map.setSize(map_len);
        read(color_map.pointer(),map_size,f);
        for (unsigned i = 0; i < map_len; i++)
            tgaColor(color_map,i*3,map_bpp,&final_map[i*map_stride]);
    }
    else
        FILE_READ_ASSERT_ZERO__(fseek(f,map_size,SEEK_CUR));
        
    BYTE pixel_bpp = image_bpp+attrib_bits,
         pixel_stride = pixel_bpp/8;
    if (pixel_bpp%8)
        pixel_stride++;
    if ((mapped && pixel_stride > 2) || pixel_stride>4 || !pixel_stride)
		throw IO::DriveAccess::FileFormatFault("Invalid bit per pixel value: "+String(pixel_stride));

    unsigned size = (unsigned)width*height*pixel_stride;
    if (type > 3)
        size = fsize-18-map_size;
	image.setSize(size);
    read(image.pointer(),size,f);
	target.setContentType(PixelType::Color);
    target.setDimensions(width,height,image_bpp<=24?3:4);
    switch (type)
    {
        case TGA_MAPPED:
            for (unsigned x = 0; x < width; x++)
                for (unsigned y = 0; y < height; y++)
                {
                    USHORT index = tgaIndex(&image[(y*width+x)*pixel_stride],image_bpp);
                    if (index > map_len)
                        throw IO::DriveAccess::FileFormatFault("Index out of bounds ("+String(index)+")");
                    target.set(x,y,&final_map[index*3]);
                }
        break;
        case TGA_RGB:
            for (unsigned x = 0; x < width; x++)
                for (unsigned y = 0; y < height; y++)
                {
                    BYTE color[4];
                    tgaColor(image,(y*width+x)*pixel_stride,image_bpp, color);
                    target.set(x,y,color);
                }
        break;
        case TGA_RLE_MAPPED:
        {
            unsigned offset(0),
                     pixel(0);
            while (offset < size)
            {
                bool run = (image[offset]>>7)&1;
                BYTE count = (image[offset]&0x7F)+1;
                     //color[3];
                offset++;
                if (run)
                {
                    if (offset + pixel_stride > size)
                        throw IO::DriveAccess::FileFormatFault(globalString("File size error"));
                    USHORT index = tgaIndex(&image[offset],image_bpp);
                    if (index > map_len)
                        throw IO::DriveAccess::FileFormatFault("Index out of bounds ("+String(index)+")");
                    offset+=pixel_stride;
                    for (BYTE k = 0; k < count; k++)
                        tgaLinearPixel(pixel++,target,&final_map[index]);
                }
                else
                {
                    if (offset + count * pixel_stride > size)
                        throw IO::DriveAccess::FileFormatFault(globalString("File size error"));
                    for (BYTE k = 0; k < count; k++)
                    {
                        USHORT index = tgaIndex(&image[offset],image_bpp);
                        if (index > map_len)
                            throw IO::DriveAccess::FileFormatFault("Index out of bounds ("+String(index)+")");
                        offset+= pixel_stride;
                        tgaLinearPixel(pixel++,target,&final_map[index]);
                    }
                }
            }
        }
        break;
        case TGA_RLE_RGB:
        {
            unsigned offset(0),
                     pixel(0);
            while (offset < size)
            {
                bool run = (image[offset]>>7)&1;
                BYTE count = (image[offset]&0x7F)+1,
                     color[4];
                offset++;
                if (run)
                {
                    if (offset + pixel_stride > size)
                        return;
                    tgaColor(image,offset,image_bpp,color);
                    offset+=pixel_stride;
                    for (BYTE k = 0; k < count; k++)
                        tgaLinearPixel(pixel++,target,color);
                }
                else
                {
                    if (offset + count * pixel_stride > size)
                        return;
                    for (BYTE k = 0; k < count; k++)
                    {
                        tgaColor(image,offset,image_bpp,color);
                        offset+= pixel_stride;
                        tgaLinearPixel(pixel++,target,color);
                    }
                }
            }
        }
        break;
        default:
			throw IO::DriveAccess::FileFormatFault("Compression not supported: "+String(type));
    }
}


void CTGA::saveToFilePointer(const Image&resource, FILE*)
{
	throw Program::FunctionalityNotImplemented();
}

