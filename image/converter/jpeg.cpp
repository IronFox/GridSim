#include "../../global_root.h"
#include "jpeg.h"

/******************************************************************

JPEG-Format handler.

******************************************************************/


JPEG	jpeg;

int jpeg_error;
char jpeg_detail[JMSG_LENGTH_MAX];

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  int setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;



void my_error_exit (j_common_ptr cinfo_ptr)
{
	jpeg_decompress_struct* cinfo_tmp = reinterpret_cast<jpeg_decompress_struct*>(cinfo_ptr);
	jpeg_decompress_struct& cinfo = *cinfo_tmp;
	const char* msg1 = cinfo.err->jpeg_message_table[cinfo.err->msg_code - cinfo.err->first_addon_message];
	const char* msg2 = "";
	if (cinfo.err->addon_message_table != NULL)
	{
		msg2 = cinfo.err->addon_message_table[cinfo.err->msg_code - cinfo.err->first_addon_message];
		//MessageBox(NULL,msg2,NULL,MB_OK);
	}
	throw Except::IO::DriveAccess::DataReadFault(String(msg1) + '\n' + String(msg2));
//	
//	//const char* msg2 = "";
//	MessageBox(NULL,msg1,NULL,MB_OK);
//	if (cinfo.err->addon_message_table != NULL)
//	{
//	msg2 = cinfo.err->addon_message_table[cinfo.err->msg_code - cinfo.err->first_addon_message];
//	MessageBox(NULL,msg2,NULL,MB_OK);
//	}
//	jpeg_destroy_decompress(&cinfo);
//
//
////	my_error_ptr myerr = (my_error_ptr) cinfo->err;
//	//char buffer[JMSG_LENGTH_MAX];
//	//(*cinfo->err->format_message) (cinfo, jpeg_detail);
//	//longjmp(myerr->setjmp_buffer, 1);
//	
//	throw IO::DriveAccess::DataReadFault();
}



void j_putRGBScanline(BYTE *jpegline, 
					 int widthPix,
					 BYTE *outBuf,
					 int row)
{
	int offset = row * widthPix * 3;
	int count;
	for (count=0;count<widthPix;count++) 
	{
		*(outBuf + offset + count * 3 + 0) = *(jpegline + count * 3 + 0);
		*(outBuf + offset + count * 3 + 1) = *(jpegline + count * 3 + 1);
		*(outBuf + offset + count * 3 + 2) = *(jpegline + count * 3 + 2);
	}
}

//
//	stash a gray scanline
//

void j_putGrayScanlineToRGB(BYTE *jpegline, 
							 int widthPix,
							 BYTE *outBuf,
							 int row)
{
	int offset = row * widthPix * 3;
	int count;
	for (count=0;count<widthPix;count++) {

		BYTE iGray;

		// get our grayscale value
		iGray = *(jpegline + count);

		*(outBuf + offset + count * 3 + 0) = iGray;
		*(outBuf + offset + count * 3 + 1) = iGray;
		*(outBuf + offset + count * 3 + 2) = iGray;
	}
}

void	JPEG::LoadFromFilePointer(Image&result, FILE*f)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
/*	if (setjmp(jerr.setjmp_buffer))
    {
        jpeg_error = JPEG_DATA_ERROR;
		jpeg_destroy_decompress(&cinfo);
        fclose(f);
        return NULL;
	}*/
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, f);
	(void) jpeg_read_header(&cinfo, TRUE);
	(void) jpeg_start_decompress(&cinfo);
	result.SetContentType(PixelType::Color);
    result.SetSize(cinfo.output_width,cinfo.output_height,3);
	
	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;

	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	while (cinfo.output_scanline < cinfo.output_height)
    {
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		if (cinfo.out_color_components==3)
        {
			j_putRGBScanline(buffer[0],
								result.GetWidth(),
								result.GetData(),
								cinfo.output_scanline-1);

		}
        else
            if (cinfo.out_color_components==1)
            {
    			j_putGrayScanlineToRGB(buffer[0],
	    							result.GetWidth(),
		    						result.GetData(),
			    					cinfo.output_scanline-1);
    		}
	}
    result.FlipVertically();
	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
}


void	JPEG::SaveToFilePointer(const Image&result, FILE*f)
{
	throw Except::Program::FunctionalityNotImplemented();

	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;
	//JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */
	
	
	#if 0

    FILE*f = fopen(filename,"wb");
    if (!f)
    {
        jpeg_error = JPEG_FILE_IO_ERROR;
        return false;
    }
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
/*	if (setjmp(jerr.setjmp_buffer))
    {
        jpeg_error = JPEG_DATA_ERROR;
		jpeg_destroy_decompress(&cinfo);
        fclose(f);
        return NULL;
	}*/
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, f);
	
	cout << "compress created\n";
	jpeg_set_defaults(&cinfo);
	cout << "defaults set\n";
	jpeg_set_colorspace(&cinfo,JCS_RGB);
	cout << "colorspace set\n";
	jpeg_set_quality(&cinfo,75,TRUE);
	cout << "quality set\n";
	
	JSAMPROW row_pointer[1];
	
	
	
// 	jpeg_read_header(&cinfo, TRUE);
	
//  	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, resource->GetWidth()*resource->GetChannels()*100, 1);
	
	cinfo.image_width = resource->GetWidth();
	cinfo.image_height = resource->GetHeight();
	cinfo.input_components = resource->GetChannels();
	cinfo.in_color_space = JCS_RGB;
	
	
	cout << cinfo.image_width<<"*"<<cinfo.image_height<<"*"<<cinfo.input_components<<endl;
	cinfo.raw_data_in = TRUE;
	
	jpeg_start_compress(&cinfo, TRUE);
	cout << "compress started\n";
	cout << cinfo.image_width<<"*"<<cinfo.image_height<<"*"<<cinfo.input_components<<endl;

	//cinfo.next_scanline = 0;
	cout << "starting with line "<<cinfo.next_scanline<<endl;
	
	row_stride = cinfo.image_width*cinfo.input_components;
	Array<JSAMPLE>	sample(row_stride);

	while (cinfo.next_scanline < cinfo.image_height)
	{
		int source_row_index = (resource->GetHeight()-cinfo.next_scanline-1);
		const BYTE*source_row = resource->GetData()+source_row_index*resource->GetWidth()*resource->GetChannels();
		for (unsigned i = 0; i < (unsigned)row_stride; i++)
		{
			sample[i] = JSAMPLE(((float)source_row[i]/255.0f)*MAXJSAMPLE);
			cout << (int)sample[i]<<" ";
		}
		cout << "writing "<<cinfo.next_scanline<<" ("<<source_row_index<<"/"<<resource->GetHeight()<<")... ";
		flush(cout);
		row_pointer[0] = sample;
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
		cout << "done\n";
		flush(cout);
	}
	cout << "done writing scanlines...\n";
	jpeg_finish_compress(&cinfo);
	cout << "compression finished\n";
	jpeg_destroy_compress(&cinfo);
	cout << "and destroyed\n";
	fclose(f);
	return true;
	#endif
}
