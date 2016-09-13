#include "../global_root.h"
#include "image.h"



#define decompressionError(message) ErrMessage("decompression_error: "#message)
#define compressionError(message) ErrMessage("compression_error: "#message)


//static unsigned fuseColors(unsigned long*id_map, unsigned row_len, unsigned left, unsigned top, unsigned dimension);
//static unsigned sectorField(unsigned long*id_map,unsigned row_len, BitStream*stream,unsigned left,unsigned top, unsigned dimension,unsigned bits_per_id,unsigned min_size);
static void sectorReLink(unsigned long*id_map,unsigned row_len, BitStream*stream,unsigned left,unsigned top, unsigned dimension,unsigned bits_per_id,unsigned min_size);





#ifdef LOG_COMPRESSION

FILE*log_out_f(NULL);
void openLog(const char*filename)
{
   log_out_f = fopen(filename,"wb");
}

void closeLog()
{
   if (log_out_f) fclose(log_out_f);
   log_out_f = NULL;
}

void out(const char*line)
{
   if (!log_out_f) return;
   fwrite(line,1,strlen(line),log_out_f);
   fwrite("\n",1,1,log_out_f);
}
#endif

template GenericImage<TIntImageNature<BYTE> >;
template GenericImage<TIntImageNature<UINT16> >;
template GenericImage<TFloatImageNature<float> >;
template GenericImage<TUnclampedFloatImageNature<float> >;


bool					BaseImage::IsEmpty()															const
{
	return image_width == 0 || image_height == 0 || image_channels == 0;
}

bool					BaseImage::IsNotEmpty()														const
{
	return image_width != 0 && image_height != 0 && image_channels != 0;
}


UINT32					BaseImage::GetWidth()															const
{
	return image_width;
}

UINT32					BaseImage::width()															const
{
	return image_width;
}

UINT32					BaseImage::GetDimension(BYTE index)											const
{
	return index?image_height:image_width;
}

UINT32					BaseImage::dimension(BYTE index)											const
{
	return index?image_height:image_width;
}

UINT32					BaseImage::GetHeight()															const
{
	return image_height;
}

UINT32					BaseImage::height()															const
{
	return image_height;
}

float					BaseImage::pixelAspect()															const
{
	return float(image_width)/float(image_height);
}

float					BaseImage::GetPixelAspect()															const
{
	return float(image_width)/float(image_height);
}


BYTE					BaseImage::channels()														const
{
	return image_channels;
}

BYTE					BaseImage::GetChannels()														const
{
	return image_channels;
}

PixelType		BaseImage::GetContentType()													const
{
	return content_type;
}

PixelType		BaseImage::contentType()													const
{
	return content_type;
}

String					BaseImage::contentTypeString()												const
{
	return content_type.ToString();
}

String					BaseImage::GetContentTypeString()												const
{
	return content_type.ToString();
}



void					BaseImage::setContentType(PixelType type)
{
	content_type = type;
}



PixelType				BaseImage::contentType(const THeader&header)
{
	return GetContentType(header);
}

PixelType				BaseImage::GetContentType(const THeader&header)
{
	PixelType type = PixelType::Color;
	if (header.type&TEX_NORMAL_BIT)
		if (header.type&TEX_OBJECT_SPACE_BIT)
			type = PixelType::ObjectSpaceNormal;
		else
			type = PixelType::TangentSpaceNormal;
	return type;
}


bool					BaseImage::writeHeader(THeader&header)											const
{
    header.x_exp = (BYTE)(ceil(log((float)image_width)/log(2.0f)));
    header.y_exp = (BYTE)(ceil(log((float)image_height)/log(2.0f)));
    header.channels = image_channels;
	header.type = 0;
	switch (content_type)
	{
		case PixelType::ObjectSpaceNormal:
			header.type |= TEX_OBJECT_SPACE_BIT;
		case PixelType::TangentSpaceNormal:
			header.type |= TEX_NORMAL_BIT;
		break;
	}
	return image_width == (1<<header.x_exp) && image_height == (1<<header.y_exp);
}

BaseImage::THeader					BaseImage::GetHeader()															const
{
	THeader h;
	writeHeader(h);
	return h;
}


bool	BaseImage::IsColorMap()	const
{
	return content_type == PixelType::Color || content_type == PixelType::StrictColor || content_type == PixelType::BGRA || content_type == PixelType::PureLuminance;
}

bool	BaseImage::IsNormalMap() const
{
	return content_type == PixelType::ObjectSpaceNormal || content_type == PixelType::TangentSpaceNormal;
}


String BaseImage::ToString()                                                             const
{
	return "Image<"+String(content_type.ToString())+"> ("+IntToStr(image_width)+"*"+IntToStr(image_height)+"*"+IntToStr(image_channels)+")";
}


