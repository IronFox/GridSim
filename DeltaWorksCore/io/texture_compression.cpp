#include "../global_root.h"
#include "texture_compression.h"

/******************************************************************

Simplified interface to extract image-date from compressed
.tex-files and texture-blocks.

******************************************************************/

#undef LOG_TEXTURE_COMPRESSION
//#define LOG_TEXTURE_COMPRESSION	//define this to write compression operations to compression.log in working directory


#undef LOG_TEXTURE_COMPRESSION_STAGE
#ifdef LOG_TEXTURE_COMPRESSION
	#include "../io/log.h"

	#define LOG_TEXTURE_COMPRESSION_STAGE(STAGE)	log_file << STAGE << nl;
#else
	#define LOG_TEXTURE_COMPRESSION_STAGE(STAGE)
#endif


namespace DeltaWorks
{
	static String	ext_error;


	static void pngError()
	{}

	static void PNGAPI handleError(png_structp strct, png_const_charp err_str)
	{
	
	}

	static void PNGAPI handleWarning(png_structp, png_const_charp err_str)
	{
	
	}

	static Ctr::Buffer<BYTE>	out_buffer;

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
		return Image::GetContentType(*(const Image::THeader*)source.pointer());
	}

	Image*    TextureCompression::decompress(const BYTE*data, size_t size)
	{
		if (size <= sizeof(Image::THeader))
		{
			ext_error = "Field is empty";
			return NULL;
		}
		const Image::THeader*head = (Image::THeader*)data;
		UINT32 crc = CRC32::Sequence().Append(&head->settings,size-4).Finish();
		if (crc != head->crc)
		{
			ext_error = "CRC mismatch";
			return NULL;
		}

		Image*result = SignalNew(new Image(*head));


		switch (head->type&TEX_CODEC_MASK)
		{
			case NoCompression:
				result->read(&data[sizeof(Image::THeader)]);
			break;
			case SectorCompression:
				ext_error = ("Built-in compression is no longer supported");
				Discard(result);
				return NULL;
			break;
			case BZ2Compression:
			{
				if (!BZ2::decompress(&data[sizeof(Image::THeader)],size-sizeof(Image::THeader),result->GetData(),result->size()))
				{
					ext_error = "Decompression failed ("+String(BZ2::errorStr())+")";
					Discard(result);
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
				result->SetSize(info->width,info->height,palette?3:info->channels);

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
				Discard(result);
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
		UINT32 crc = CRC32::Sequence().Append(&head->settings,size-4).Finish();
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
				if (!BZ2::decompress(&data[sizeof(Image::THeader)],size-sizeof(Image::THeader),to.GetData(),to.size()))
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
				to.SetSize(info->width,info->height,palette?3:info->channels);

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
			local.Resample((1<<header.x_exp),(1<<header.y_exp));
			from = &local;
		}
	
		size_t compressed_size = 0;
		#ifdef LOG_TEXTURE_COMPRESSION
			static LogFile	log_file("compression.log",true);
		#endif

		LOG_TEXTURE_COMPRESSION_STAGE("compressing "<<from->GetWidth()<<"*"<<from->GetHeight()<<"*"<<from->GetChannels());


		switch (codec)
		{
			case NoCompression:
				compressed_size = from->size();
				buffer.SetSize((unsigned)(compressed_size+sizeof(Image::THeader)));
				memcpy(buffer+sizeof(Image::THeader),from->GetData(),compressed_size);
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
				compressed_size = BZ2::compress(from->GetData(), from->size(), temp.pointer(), temp.GetContentSize());
				if (!compressed_size)
				{
					ext_error = "Compression failed ("+String(BZ2::errorStr())+")";
					return 0;
				}	
				buffer.SetSize((unsigned)(compressed_size+sizeof(Image::THeader)));
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

				LOG_TEXTURE_COMPRESSION_STAGE("configuring info");

				info->width = from->GetWidth();
				info->height = from->GetHeight();
				info->valid = 0;
				info->rowbytes = from->GetWidth()*from->GetChannels();
				info->palette = NULL;
				info->num_palette = 0;
				info->num_trans = 0;
				info->bit_depth = 8;
				info->color_type = from->GetChannels()==3?PNG_COLOR_TYPE_RGB:PNG_COLOR_TYPE_RGBA;
				info->compression_type = PNG_COMPRESSION_TYPE_BASE;
				info->filter_type = PNG_FILTER_TYPE_BASE;
				info->interlace_type = PNG_INTERLACE_NONE;
				info->channels = from->GetChannels();
				info->pixel_depth = from->GetChannels()*8;
				info->spare_byte = 0;
				memcpy(info->signature,"PNG     ",8);
			
				LOG_TEXTURE_COMPRESSION_STAGE("creating map");

				png_bytepp image = new png_bytep[from->GetHeight()];
				for (unsigned i = 0; i < from->GetHeight(); i++)
				{
					image[i] = new png_byte[from->GetWidth()*from->GetChannels()];
					for (unsigned j = 0; j < from->GetWidth(); j++)
					{
						const BYTE*pixel = from->get(j,i);
						for (BYTE k = 0; k < info->channels; k++)
							image[i][j*info->channels+k] = pixel[k];
					}
				}
			
				LOG_TEXTURE_COMPRESSION_STAGE("sending map");
			
				png_set_rows(png,info,image);
				LOG_TEXTURE_COMPRESSION_STAGE("writing png");
			
				png_write_png(png,info,PNG_TRANSFORM_IDENTITY,NULL);
				LOG_TEXTURE_COMPRESSION_STAGE("cleaning up 1");
				png_destroy_info_struct(png,&info);
				LOG_TEXTURE_COMPRESSION_STAGE("cleaning up 2");
				png_destroy_write_struct(&png,NULL);
			
			
				for (unsigned i = 0; i < from->GetHeight(); i++)
				{
					delete[] image[i];
				}
				delete[] image;
	
				LOG_TEXTURE_COMPRESSION_STAGE("copying compressed image ("<<out_buffer.Count()<<" byte(s))");
	
				compressed_size = out_buffer.Count();
				buffer.SetSize(sizeof(Image::THeader)+out_buffer.Count());
				LOG_TEXTURE_COMPRESSION_STAGE("buffer resized to "<<buffer.GetContentSize()<<" byte(s)");
				memcpy(buffer.pointer()+sizeof(Image::THeader),out_buffer.pointer(),compressed_size);
			}
		}
		if (!compressed_size)
			return 0;
		
		(*(Image::THeader*)buffer.pointer()) = header;
		Image::THeader*head = (Image::THeader*)buffer.pointer();
		head->crc = CRC32::Sequence().Append(&head->settings,buffer.size()-4).Finish();
	
		LOG_TEXTURE_COMPRESSION_STAGE("all done");
	
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
}
