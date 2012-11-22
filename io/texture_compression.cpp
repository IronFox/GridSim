#include "../global_root.h"
#include "texture_compression.h"

/******************************************************************

Simplified interface to extract image-date from compressed
.tex-files and texture-blocks.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


static String	ext_error;


static void pngError()
{}

static void PNGAPI handleError(png_structp strct, png_const_charp err_str)
{
	
}

static void PNGAPI handleWarning(png_structp, png_const_charp err_str)
{
	
}

static Buffer<BYTE>	out_buffer;

static const BYTE*global_source = NULL;
static unsigned global_at = 0,
				global_size = 0;


static void writeSection(png_structp png, png_bytep data, png_size_t size)
{
	out_buffer.append(data,(unsigned)size);
}

static void	flushSection(png_structp png)
{

}

static void readSection(png_structp png, png_bytep data, png_size_t size)
{
	for (unsigned i = 0; i < size && global_at < global_size; i++)
	{
		BYTE v = global_source[global_at++];
		data[i] = v;
	}
}




PixelType	TextureCompression::extractContentType(const ArrayData<BYTE>&source)
{
	if (source.size() < sizeof(Image::THeader))
		return PixelType::None;
	return Image::getContentType(*(const Image::THeader*)source.pointer());
}

Image*    TextureCompression::decompress(const BYTE*data, size_t size)
{
	if (size <= sizeof(Image::THeader))
	{
		ext_error = "Field is empty";
		return NULL;
	}
    const Image::THeader*head = (Image::THeader*)data;
    UINT32 crc = CRC32::getChecksum(&head->settings,size-4);
    if (crc != head->crc)
	{
		ext_error = "CRC mismatch";
        return NULL;
	}

    Image*result = SHIELDED(new Image(*head));


    switch (head->type&TEX_CODEC_MASK)
    {
        case NoCompression:
            result->read(&data[sizeof(Image::THeader)]);
        break;
        case SectorCompression:
            ext_error = ("Built-in compression is no longer supported");
            DISCARD(result);
            return NULL;
        break;
        case BZ2Compression:
        {
            if (!BZ2::decompress(&data[sizeof(Image::THeader)],size-sizeof(Image::THeader),result->getData(),result->size()))
            {
				ext_error = "Decompression failed ("+String(BZ2::errorStr())+")";
                DISCARD(result);
                return NULL;
            }
        }
        break;
		case PNGCompression:
		{
		    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,(void*)pngError,handleError,handleWarning);
		    png_infop info= png_create_info_struct(png);
			png_set_read_fn(png,NULL,readSection);
			
			global_source = data+sizeof(Image::THeader);
			global_at = 0;
			global_size = (unsigned)(size-sizeof(Image::THeader));
			
		    png_read_png(png,info,PNG_TRANSFORM_IDENTITY,NULL);
			
		    png_bytepp image = png_get_rows(png,info);
		    bool palette = info->color_type&PNG_COLOR_MASK_PALETTE;
		    result->setDimensions(info->width,info->height,palette?3:info->channels);

		    for (unsigned x = 0; x < info->width; x++)
		        for (unsigned y = 0; y < info->height; y++)
		            if (palette)
		                result->set(x,y,&info->palette[image[y][x]*info->channels].red);
		            else
		                result->set(x,y,&image[y][x*info->channels]);

			png_destroy_info_struct(png,&info);
		    png_destroy_read_struct(&png,NULL,NULL);
		}
		break;
        default:
			ext_error = "Codec not supported ("+IntToStr(head->type&TEX_CODEC_MASK)+")";
            DISCARD(result);
            return NULL;
    }
    return result;
}

Image*    TextureCompression::decompress(const ArrayData<BYTE>&data)
{
	return decompress(data.pointer(),data.length());
}

bool TextureCompression::decompress(const BYTE*data, size_t size, Image&to)
{
	if (size <= sizeof(Image::THeader))
	{
		ext_error = "Field is empty";
		return false;
	}
    Image::THeader*head = (Image::THeader*)data;
    UINT32 crc = CRC32::getChecksum(&head->settings,size-4);
    if (crc != head->crc)
	{
		ext_error = "CRC mismatch";
        return false;
	}

    to.applyHeader(*head);
	
    switch (head->type&TEX_CODEC_MASK)
    {
        case NoCompression:
            to.read(&data[sizeof(Image::THeader)]);
        break;
        case SectorCompression:
            ext_error = ("Built-in compression is no longer supported");
            return false;
        break;
        case BZ2Compression:
        {
            if (!BZ2::decompress(&data[sizeof(Image::THeader)],size-sizeof(Image::THeader),to.getData(),to.size()))
			{
				ext_error = "Decompression failed ("+String(BZ2::errorStr())+")";
                return false;
			}
        }
        break;
		case PNGCompression:
		{
		    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,(void*)pngError,handleError,handleWarning);
		    png_infop info= png_create_info_struct(png);
			png_set_read_fn(png,NULL,readSection);
			
			global_source = data+sizeof(Image::THeader);
			global_at = 0;
			global_size = (unsigned)(size-sizeof(Image::THeader));
			
		    png_read_png(png,info,PNG_TRANSFORM_IDENTITY,NULL);
			
		    png_bytepp image = png_get_rows(png,info);
		    bool palette = info->color_type&PNG_COLOR_MASK_PALETTE;
		    to.setDimensions(info->width,info->height,palette?3:info->channels);

		    for (unsigned x = 0; x < info->width; x++)
		        for (unsigned y = 0; y < info->height; y++)
		            if (palette)
		                to.set(x,y,&info->palette[image[y][x]*info->channels].red);
		            else
		                to.set(x,y,&image[y][x*info->channels]);

			png_destroy_info_struct(png,&info);
		    png_destroy_read_struct(&png,NULL,NULL);
		}
		break;
        default:
			ext_error = "Codec not supported ("+IntToStr(head->type&TEX_CODEC_MASK)+")";
            return false;
    }
    return true;
}


bool TextureCompression::decompress(const ArrayData<BYTE>&data, Image&to)
{
	return decompress(data.pointer(),data.length(),to);
}

static size_t autoCompressImage(const Image&source, Array<BYTE>&buffer)	//this is extremely inefficient but i can't be bothered right now.
{
	Array<BYTE>	temp;
	size_t current = TextureCompression::compress(source,buffer,TextureCompression::NoCompression);
	for (unsigned i = 1; i < TextureCompression::RegularCodecCount; i++)
	{
		size_t s = TextureCompression::compress(source,temp,(TextureCompression::Codec)i);
		if (s > 0 && s < current)
		{
			buffer = temp;
			current = s;
		}
	}
	return current;
}

size_t TextureCompression::compress(const Image&source, Array<BYTE>&buffer, Codec codec)
{
	if (codec == SmallestCompression)
		return autoCompressImage(source,buffer);
	
	Image::THeader	header;
	bool exact = source.writeHeader(header);
	
	header.type |= codec;
	
		
	Image local;
	const Image*from;
	if (exact)
		from = &source;
	else
	{
		local = source;
		local.resample((1<<header.x_exp),(1<<header.y_exp));
		from = &local;
	}
	
	size_t compressed_size = 0;
	static LogFile	log_file("compression.log",true);

	log_file << "compressing "<<from->getWidth()<<"*"<<from->getHeight()<<"*"<<from->getChannels()<<nl;

	switch (codec)
	{
		case NoCompression:
			compressed_size = from->size();
			buffer.setSize((unsigned)(compressed_size+sizeof(Image::THeader)));
			memcpy(buffer+sizeof(Image::THeader),from->getData(),compressed_size);
		break;
		case SectorCompression:
		{
            ext_error = ("Built-in compression is no longer supported");
            return 0;
		}
		break;
		case BZ2Compression:
		{
			Array<BYTE>	temp((unsigned)(from->size()));
			compressed_size = BZ2::compress(from->getData(), from->size(), temp.pointer(), temp.contentSize());
			if (!compressed_size)
			{
				ext_error = "Compression failed ("+String(BZ2::errorStr())+")";
				return 0;
			}	
			buffer.setSize((unsigned)(compressed_size+sizeof(Image::THeader)));
			memcpy(buffer+sizeof(Image::THeader),temp.pointer(),compressed_size);
		}
		break;
		case PNGCompression:
		{
			out_buffer.reset();
			png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,(void*)pngError,handleError,handleWarning);
			png_infop info= png_create_info_struct(png);
			png_set_compression_level(png,9);	//maximum compression
			png_set_write_fn(png,NULL,writeSection,flushSection);

			log_file << "configuring info...\r\n";
			info->width = from->getWidth();
			info->height = from->getHeight();
			info->valid = 0;
			info->rowbytes = from->getWidth()*from->getChannels();
			info->palette = NULL;
			info->num_palette = 0;
			info->num_trans = 0;
			info->bit_depth = 8;
			info->color_type = from->getChannels()==3?PNG_COLOR_TYPE_RGB:PNG_COLOR_TYPE_RGBA;
			info->compression_type = PNG_COMPRESSION_TYPE_BASE;
			info->filter_type = PNG_FILTER_TYPE_BASE;
			info->interlace_type = PNG_INTERLACE_NONE;
			info->channels = from->getChannels();
			info->pixel_depth = from->getChannels()*8;
			info->spare_byte = 0;
			memcpy(info->signature,"PNG     ",8);
			
			log_file << "creating map...\r\n";
			
			png_bytepp image = new png_bytep[from->getHeight()];
			for (unsigned i = 0; i < from->getHeight(); i++)
			{
				image[i] = new png_byte[from->getWidth()*from->getChannels()];
				for (unsigned j = 0; j < from->getWidth(); j++)
				{
					const BYTE*pixel = from->get(j,i);
					for (BYTE k = 0; k < info->channels; k++)
						image[i][j*info->channels+k] = pixel[k];
				}
			}
			
			log_file << "sending map...\r\n";
			
			png_set_rows(png,info,image);
			log_file << "writing png...\r\n";
			
			png_write_png(png,info,PNG_TRANSFORM_IDENTITY,NULL);
			log_file << "cleaning up...\r\n";
		    png_destroy_info_struct(png,&info);
			log_file << "cleaning up...\r\n";
			png_destroy_write_struct(&png,NULL);
			
			
			for (unsigned i = 0; i < from->getHeight(); i++)
			{
				delete[] image[i];
			}
			delete[] image;
	
			log_file << "copying compressed image ("<<out_buffer.fillLevel()<<" byte(s))...\r\n";
	
			compressed_size = out_buffer.fillLevel();
			buffer.setSize(sizeof(Image::THeader)+out_buffer.fillLevel());
			log_file << "buffer resized to "<<buffer.contentSize()<<" byte(s)\r\n";
			memcpy(buffer.pointer()+sizeof(Image::THeader),out_buffer.pointer(),compressed_size);
		}
	}
	if (!compressed_size)
		return 0;
		
	(*(Image::THeader*)buffer.pointer()) = header;
	Image::THeader*head = (Image::THeader*)buffer.pointer();
	head->crc = CRC32::getChecksum(&head->settings,buffer.size()-4);
	
	log_file << "all done\r\n";
	
	return buffer.size();
	/*}
	catch (...)
	{
		return 0;
	}*/
}

const String&	TextureCompression::getError()
{
	return ext_error;
}

const String&	TextureCompression::errorStr()
{
	return ext_error;
}
