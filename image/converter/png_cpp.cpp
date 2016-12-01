#include "../../global_root.h"
#include "png.h"
#include "../../container/buffer.h"

PNG	png;

namespace 
{

	png_byte	_GetColorType(BYTE numChannels)
	{
		switch (numChannels)
		{
			case 1:
				return PNG_COLOR_TYPE_GRAY;
			break;
			case 2:
				return PNG_COLOR_TYPE_GRAY_ALPHA;
			break;
			case 3:
				return PNG_COLOR_TYPE_RGB;
			break;
			case 4:
				return PNG_COLOR_TYPE_RGB_ALPHA;
			break;
			default:
				throw IO::DriveAccess::FileFormatFault(globalString("Unexpected number of channels"));
			break;
		}
	}

	void pngError()
	{}

	void PNGAPI readError(png_structp strct, png_const_charp err_str)
	{
		throw IO::DriveAccess::DataReadFault(err_str);
	}

	void PNGAPI writeError(png_structp strct, png_const_charp err_str)
	{
		throw IO::DriveAccess::DataWriteFault(err_str);
	}


	Buffer<BYTE>	out_buffer;
	IWriteStream	*out_stream = NULL;
	IReadStream		*inStream = NULL;
	const	BYTE	*global_source = NULL;
	index_t			global_at = 0;
	size_t			global_size = 0;

	void writeSection(png_structp png, png_bytep data, png_size_t size)
	{
		out_buffer.append(data,(unsigned)size);
	}

	void writeSectionToStream(png_structp png, png_bytep data, png_size_t size)
	{
		if (!out_stream->Write(data,(serial_size_t)size))
			throw IO::DriveAccess::DataWriteFault("Stream refused to accept streaming data");
	}

	void	flushSection(png_structp png)
	{}

	void readSection(png_structp png, png_bytep data, png_size_t size)
	{
		if (global_at + size > global_size)
			throw IO::DriveAccess::DataReadFault("Unexpected end of file encountered");
		else
			for (png_size_t i = 0; i < size; i++)
			{
				BYTE v = global_source[global_at++];
				data[i] = v;
			}
	}

	void readSectionFromStream(png_structp png, png_bytep data, png_size_t size)
	{
		if (!inStream->Read(data,(serial_size_t)size))
			throw IO::DriveAccess::DataReadFault("Stream refused to read requested amount of bytes");
	}
}


void PNGAPI handleWarning(png_structp, png_const_charp)
{}

static 	inline void  _c3(const BYTE v[3], BYTE w[3])
	{
		w[0] = v[0];
		w[1] = v[1];
		w[2] = v[2];
	}


void PNG::SaveToFileQ(const Image&resource, const PathString&filename)
{
    FILE*f = FOPEN(filename.c_str(),"wb");
    if (!f)
		throw IO::DriveAccess::FileOpenFault("File not found or inaccessible for writing: '"+filename+"'");

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,(void*)pngError,writeError,handleWarning);
    png_init_io(png,f);
	png_infop info= png_create_info_struct(png);
	png_set_compression_level(png,2);	//fast compression
	//png_set_write_fn(png,NULL,writeSection,flushSection);

	//log_file << "configuring info...\r\n";
	info->width = resource.GetWidth();
	info->height = resource.GetHeight();
	info->valid = 0;
	info->rowbytes = resource.GetWidth()*resource.GetChannels();
	info->palette = NULL;
	info->num_palette = 0;
	info->num_trans = 0;
	info->bit_depth = 8;
	info->color_type = _GetColorType(resource.GetChannels());
	info->compression_type = PNG_COMPRESSION_TYPE_BASE;
	info->filter_type = PNG_FILTER_TYPE_BASE;
	info->interlace_type = PNG_INTERLACE_NONE;
	info->channels = resource.GetChannels();
	info->pixel_depth = resource.GetChannels()*8;
	info->spare_byte = 0;
	memcpy(info->signature,"PNG     ",8);

	//log_file << "creating map...\r\n";

	png_bytepp image = new png_bytep[resource.GetHeight()];
	for (unsigned i = 0; i < resource.GetHeight(); i++)
	{
		image[i] = new png_byte[resource.GetWidth()*resource.GetChannels()];
		for (unsigned j = 0; j < resource.GetWidth(); j++)
		{
			const BYTE*pixel = resource.get(j,resource.GetHeight()-1-i);
			for (BYTE k = 0; k < info->channels; k++)
				image[i][j*info->channels+k] = pixel[k];
		}
	}

	//log_file << "sending map...\r\n";

	png_set_rows(png,info,image);
	//log_file << "writing png...\r\n";

	png_write_png(png,info,PNG_TRANSFORM_IDENTITY,NULL);
	//log_file << "cleaning up...\r\n";
	png_destroy_info_struct(png,&info);
	//log_file << "cleaning up...\r\n";
	png_destroy_write_struct(&png,NULL);


	for (unsigned i = 0; i < resource.GetHeight(); i++)
	{
		delete[] image[i];
	}
	delete[] image;
	fclose(f);

}


void PNG::SaveToFilePointer(const Image&resource, FILE*f)
{

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,(void*)pngError,writeError,handleWarning);
    png_init_io(png,f);
	png_infop info= png_create_info_struct(png);
	png_set_compression_level(png,9);	//maximum compression
	//png_set_write_fn(png,NULL,writeSection,flushSection);

	//log_file << "configuring info...\r\n";
	info->width = resource.GetWidth();
	info->height = resource.GetHeight();
	info->valid = 0;
	info->rowbytes = resource.GetWidth()*resource.GetChannels();
	info->palette = NULL;
	info->num_palette = 0;
	info->num_trans = 0;
	info->bit_depth = 8;
	info->color_type = _GetColorType(resource.GetChannels());
	info->compression_type = PNG_COMPRESSION_TYPE_BASE;
	info->filter_type = PNG_FILTER_TYPE_BASE;
	info->interlace_type = PNG_INTERLACE_NONE;
	info->channels = resource.GetChannels();
	info->pixel_depth = resource.GetChannels()*8;
	info->spare_byte = 0;
	memcpy(info->signature,"PNG     ",8);

	//log_file << "creating map...\r\n";

	png_bytepp image = new png_bytep[resource.GetHeight()];
	for (unsigned i = 0; i < resource.GetHeight(); i++)
	{
		image[i] = new png_byte[resource.GetWidth()*resource.GetChannels()];
		for (unsigned j = 0; j < resource.GetWidth(); j++)
		{
			const BYTE*pixel = resource.get(j,resource.GetHeight()-1-i);
			for (BYTE k = 0; k < info->channels; k++)
				image[i][j*info->channels+k] = pixel[k];
		}
	}

	//log_file << "sending map...\r\n";

	png_set_rows(png,info,image);
	//log_file << "writing png...\r\n";

	png_write_png(png,info,PNG_TRANSFORM_IDENTITY,NULL);
	//log_file << "cleaning up...\r\n";
	png_destroy_info_struct(png,&info);
	//log_file << "cleaning up...\r\n";
	png_destroy_write_struct(&png,NULL);


	for (unsigned i = 0; i < resource.GetHeight(); i++)
	{
		delete[] image[i];
	}
	delete[] image;
}


void PNG::LoadFromFilePointer(Image&target, FILE*f)
{
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,(void*)pngError,readError,handleWarning);
    if (!png)
		FATAL__("PNG read struct creation failed");

    png_infop info= png_create_info_struct(png);
    if (!info)
    {
		FATAL__("PNG info struct creation failed");
        png_destroy_struct(png);
        return;
    }
	
    //target.SetSize(10,10,3);
    png_init_io(png,f);
    png_read_png(png,info,PNG_TRANSFORM_IDENTITY,NULL);
	BYTE bits_per_channel = info->bit_depth;
  //  png_start_read_image(png);
    png_bytepp image = png_get_rows(png,info);
    bool palette = info->color_type&PNG_COLOR_MASK_PALETTE;
	target.SetContentType(PixelType::Color);
    target.SetSize(info->width,info->height,palette?3:info->channels);
	//ShowMessage(info->channels);

	
	switch (bits_per_channel)
	{
		case 8:
		    for (unsigned x = 0; x < info->width; x++)
		        for (unsigned y = 0; y < info->height; y++)
		            if (palette)
		                target.set(x,y,&info->palette[image[y][x]*info->channels].red);
		            else
		                target.set(x,y,&image[y][x*info->channels]);
		break;
		case 16:
		    for (unsigned x = 0; x < info->width; x++)
		        for (unsigned y = 0; y < info->height; y++)
				{
					BYTE*pixel = target.get(x,y);
		            if (palette)
						_c3(&info->palette[image[y][x]*info->channels].red,pixel);
		            else
					{
						for (BYTE k = 0; k < info->channels; k++)
						{
							pixel[k] = ((const USHORT*)(image[y]+(x*info->channels*2)))[k]/256;
							//ShowMessage(IntToStr(k)+": "+String(((const USHORT*)(image[y]+(x*info->channels*2)))[k]));
						}
					}
				}
		break;
	}
            /*
    unsigned pass = png_set_interlace_handling(png);
    for (unsigned i = 0; i < pass; i++)
        for (unsigned j = 0; j < info->height; j++)
            png_read_row(png,target.get(0,j),NULL);
*/
    png_destroy_info_struct(png,&info);
	png_destroy_read_struct(&png,NULL,NULL);
    target.FlipVertically();
//    target.resize(128,128);

}




/*static*/	void		PNG::CompressToStream(const Image&source_image, IWriteStream&stream)
{
	out_stream = &stream;
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,(void*)pngError,writeError,handleWarning);
	png_infop info= png_create_info_struct(png);
	png_set_compression_level(png,9);	//maximum compression
	png_set_write_fn(png,NULL,writeSectionToStream,flushSection);

	png_bytepp image = NULL;
	try
	{
		info->width = source_image.GetWidth();
		info->height = source_image.GetHeight();
		info->valid = 0;
		info->rowbytes = source_image.GetWidth()*source_image.GetChannels();
		info->palette = NULL;
		info->num_palette = 0;
		info->num_trans = 0;
		info->bit_depth = 8;
		info->color_type = _GetColorType(source_image.GetChannels());
		info->compression_type = PNG_COMPRESSION_TYPE_BASE;
		info->filter_type = PNG_FILTER_TYPE_BASE;
		info->interlace_type = PNG_INTERLACE_NONE;
		info->channels = source_image.GetChannels();
		info->pixel_depth = source_image.GetChannels()*8;
		info->spare_byte = 0;
		memcpy(info->signature,"PNG     ",8);
		
		image = new png_bytep[source_image.GetHeight()];
		for (UINT32 i = 0; i < source_image.GetHeight(); i++)
		{
			image[i] = new png_byte[source_image.GetWidth()*source_image.GetChannels()];
			for (UINT32 j = 0; j < source_image.GetWidth(); j++)
			{
				const BYTE*pixel = source_image.get(j,i);
				for (BYTE k = 0; k < info->channels; k++)
					image[i][j*info->channels+k] = pixel[k];
			}
		}
			
		png_set_rows(png,info,image);
		png_write_png(png,info,PNG_TRANSFORM_IDENTITY,NULL);
		png_destroy_info_struct(png,&info);
		png_destroy_write_struct(&png,NULL);
			
			
		for (UINT32 i = 0; i < source_image.GetHeight(); i++)
		{
			delete[] image[i];
		}
		delete[] image;
	}
	catch (const std::exception&)
	{
		png_destroy_info_struct(png,&info);
		png_destroy_write_struct(&png,NULL);
			
			
		if (image)
		{
			for (UINT32 i = 0; i < source_image.GetHeight(); i++)
			{
				delete[] image[i];
			}
			delete[] image;
		}
		throw;
	}
	
	//log_file << "copying compressed image ("<<out_buffer.fillLevel()<<" byte(s))...\r\n";
}


/*static*/	void		PNG::CompressToArray(const Image&source_image, Array<BYTE>&data)
{
	out_buffer.reset();
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,(void*)pngError,writeError,handleWarning);
	png_infop info= png_create_info_struct(png);
	png_set_compression_level(png,9);	//maximum compression
	png_set_write_fn(png,NULL,writeSection,flushSection);
	png_bytepp image = NULL;
	try
	{
		info->width = source_image.GetWidth();
		info->height = source_image.GetHeight();
		info->valid = 0;
		info->rowbytes = source_image.GetWidth()*source_image.GetChannels();
		info->palette = NULL;
		info->num_palette = 0;
		info->num_trans = 0;
		info->bit_depth = 8;
		info->color_type = _GetColorType(source_image.GetChannels());
		info->compression_type = PNG_COMPRESSION_TYPE_BASE;
		info->filter_type = PNG_FILTER_TYPE_BASE;
		info->interlace_type = PNG_INTERLACE_NONE;
		info->channels = source_image.GetChannels();
		info->pixel_depth = source_image.GetChannels()*8;
		info->spare_byte = 0;
		memcpy(info->signature,"PNG     ",8);
		
		image = new png_bytep[source_image.GetHeight()];
		for (UINT32 i = 0; i < source_image.GetHeight(); i++)
		{
			image[i] = new png_byte[source_image.GetWidth()*source_image.GetChannels()];
			for (UINT32 j = 0; j < source_image.GetWidth(); j++)
			{
				const BYTE*pixel = source_image.get(j,i);
				for (BYTE k = 0; k < info->channels; k++)
					image[i][j*info->channels+k] = pixel[k];
			}
		}
			
		png_set_rows(png,info,image);
		png_write_png(png,info,PNG_TRANSFORM_IDENTITY,NULL);
		png_destroy_info_struct(png,&info);
		png_destroy_write_struct(&png,NULL);
			


		for (UINT32 i = 0; i < source_image.GetHeight(); i++)
		{
			delete[] image[i];
		}
		delete[] image;
	
		//log_file << "copying compressed image ("<<out_buffer.fillLevel()<<" byte(s))...\r\n";
		out_buffer.copyToArray(data);
	}
	catch (const std::exception&)
	{
		png_destroy_info_struct(png,&info);
		png_destroy_write_struct(&png,NULL);
			
		if (image)
		{
			for (UINT32 i = 0; i < source_image.GetHeight(); i++)
			{
				delete[] image[i];
			}
			delete[] image;
		}
		throw;
	}
}


/*static*/	void		PNG::DecompressData(Image&out_image, const void*data, size_t data_size)
{
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,(void*)pngError,readError,handleWarning);
	png_infop info= png_create_info_struct(png);
	png_set_read_fn(png,NULL,readSection);
			
	global_source = reinterpret_cast<const BYTE*>(data);
	global_at = 0;
	global_size = data_size;
	
	try
	{
		png_read_png(png,info,PNG_TRANSFORM_IDENTITY,NULL);
			
		png_bytepp image = png_get_rows(png,info);
		bool palette = info->color_type&PNG_COLOR_MASK_PALETTE;
		out_image.SetSize(info->width,info->height,palette?3:info->channels);
		out_image.SetContentType(PixelType::Color);

		for (UINT32 x = 0; x < info->width; x++)
			for (UINT32 y = 0; y < info->height; y++)
				if (palette)
					out_image.set(x,y,&info->palette[image[y][x]*info->channels].red);
				else
					out_image.set(x,y,&image[y][x*info->channels]);

		png_destroy_info_struct(png,&info);
		png_destroy_read_struct(&png,NULL,NULL);
	}
	catch (std::exception&)
	{
		png_destroy_info_struct(png,&info);
		png_destroy_read_struct(&png,NULL,NULL);
		throw;
	}
}

/*static*/	void		PNG::DecompressArray(Image&out_image, const Array<BYTE>&data)
{
	DecompressData(out_image,data.pointer(),data.size());

}


/*static*/	void		PNG::DecompressStream(Image&out_image, IReadStream&stream)
{
	inStream = &stream;

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,(void*)pngError,readError,handleWarning);
	png_infop info= png_create_info_struct(png);
	png_set_read_fn(png,NULL,readSectionFromStream);
	
	try
	{
		png_read_png(png,info,PNG_TRANSFORM_IDENTITY,NULL);
			
		png_bytepp image = png_get_rows(png,info);
		bool palette = info->color_type&PNG_COLOR_MASK_PALETTE;
		out_image.SetSize(info->width,info->height,palette?3:info->channels);
		out_image.SetContentType(PixelType::Color);

		for (UINT32 x = 0; x < info->width; x++)
			for (UINT32 y = 0; y < info->height; y++)
				if (palette)
					out_image.set(x,y,&info->palette[image[y][x]*info->channels].red);
				else
					out_image.set(x,y,&image[y][x*info->channels]);

		png_destroy_info_struct(png,&info);
		png_destroy_read_struct(&png,NULL,NULL);
	}
	catch (const std::exception&)
	{
		png_destroy_info_struct(png,&info);
		png_destroy_read_struct(&png,NULL,NULL);
		throw;
	}
}
