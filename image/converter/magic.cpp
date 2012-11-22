#include "../../global_root.h"
#include "magic.h"


/******************************************************************

Auto-Format handler.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


ImageFormat*	Magic::format[] = {&bitmap,&jpeg,&png,&tga};

void			Magic::getExtensions(ArrayData<String>&extensions)
{
	count_t cnt = 0;
	for (index_t i = 0; i < ARRAYSIZE(format); i++)
		cnt += format[i]->extensions.count();
	extensions.setSize(cnt);
	String*to = extensions.pointer();
	for (index_t i = 0; i < ARRAYSIZE(format); i++)
	{
		for (index_t j = 0; j < format[i]->extensions.count(); j++)
			(*to++) = format[i]->extensions[j];
	}
	ASSERT_CONCLUSION(extensions,to);
}

void			Magic::getExtensions(ArrayData<String>&extensions, ArrayData<ImageFormat*>&formats)
{
	count_t cnt = 0;
	for (index_t i = 0; i < ARRAYSIZE(format); i++)
		cnt += format[i]->extensions.count();
	extensions.setSize(cnt);
	formats.setSize(cnt);
	String*sto = extensions.pointer();
	ImageFormat**fto = formats.pointer();
	
	for (index_t i = 0; i < ARRAYSIZE(format); i++)
	{
		for (index_t j = 0; j < format[i]->extensions.count(); j++)
		{
			(*sto++) = format[i]->extensions[j];
			(*fto++) = format[i];
		}
	}
	ASSERT_CONCLUSION(extensions,sto);
	ASSERT_CONCLUSION(formats,fto);
}

static const char* extractExt(const String&name)
{
    index_t offset = name.length();
	const char*cstr = name.c_str();
    while (--offset && cstr[offset] != '.');
    if (offset)
        return cstr + (offset+1);
    return NULL;
}


void		Magic::loadFromFile(Image&target, const String&filename)
{
	loadFromFile(target,filename,NULL);
}

void        Magic::loadFromFile(Image&target, const String&filename, const char*file_ext)
{
	if (!file_ext)
		file_ext = extractExt(filename);

	BYTE	magic_bytes[10];
	FILE*file = fopen(filename.c_str(),"rb");
	if (!file)
	{
		throw IO::DriveAccess::FileOpenFault("File not found or inaccessible: '"+filename+"'");
		return;
	}


	size_t actual_bytes = fread(magic_bytes,1,10,file);
	FILE_READ_ASSERT_ZERO__(fseek(file,0,SEEK_SET));

	/*cout << "interpreting image bytes";
	for (index_t i = 0; i < actual_bytes; i++)
		cout << " 0x"<<std::hex<<(int)magic_bytes[i];
	cout << "..."<<endl;*/

	for (index_t i = 0; i < ARRAYSIZE(format); i++)
	{
		ImageFormat*format = Magic::format[i];
		if (format->magic_bytes.isEmpty() || format->magic_bytes.count()>actual_bytes)
			continue;	//process later or ignore
		if (memcmp(format->magic_bytes.pointer(),magic_bytes,format->magic_bytes.count())!=0)
			continue;
		//cout << " found direct match #"<<i<<" '"<<format->name<<"'"<<endl;
		try
		{
			format->loadFromFilePointer(target,file);
			fclose(file);
			return;
		}
		catch (...)
		{
			fclose(file);
			throw;
		}
	}
	//cout << " no direct match found. comparing extensions"<<endl;

	for (index_t i = 0; i < ARRAYSIZE(format); i++)
	{
		ImageFormat*format = Magic::format[i];
		if (file_ext)
		{
			if (!format->supports(file_ext))
				continue;
			//cout << " found extension match #"<<i<<" '"<<format->name<<"'"<<endl;
		}
		else
		{
			if (!format->magic_bytes.isEmpty())
				continue;
			//cout << " trying format #"<<i<<" '"<<format->name<<"'"<<endl;
		}
		try
		{
			format->loadFromFilePointer(target,file);
			fclose(file);
			return;
		}
		catch (...)
		{
			fclose(file);
			throw;
		}
	}

	fclose(file);
	throw Program::UnsupportedRequest("'"+filename+"': Unable to find matching image format");
}

void			Magic::saveToFile(const Image&image, const String&filename)
{
	return saveToFile(image,filename,NULL);
}

void        	Magic::saveToFile(const Image&image, const String&filename, const char*file_ext)
{
	if (!file_ext)
		file_ext = extractExt(filename);

	if (file_ext)
	{
		for (index_t i = 0; i < ARRAYSIZE(format); i++)
			if (format[i]->supports(file_ext))
			{
				format[i]->saveToFile(image,filename);
				return;
			}
	}
	throw Program::UnsupportedRequest("Extension not supported: '"+String(file_ext)+"'");
}
