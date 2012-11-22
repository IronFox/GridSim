#ifndef img_importerH
#define img_importerH

#include "../image.h"


/**
	@brief Abstract image format descriptor
*/
/*abstract*/ class ImageFormat
{
protected:
		/**
			@brief Reads data into the target memory section
		 */
		void			read(void*target, size_t size, FILE*f)
		{
			if (fread(target,size,1,f) != 1)
			{
				throw IO::DriveAccess::DataReadFault();
			}
		}

	template <typename T>
		void			read(T&target, FILE*f)
		{
			read(&target,sizeof(T),f);
		}


		/**
			@brief Writes data from the target memory section to the specified file
		 */
		void			write(const void*target, size_t size, FILE*f)
		{
			if (fwrite(target,size,1,f) != 1)
			{
				throw IO::DriveAccess::DataWriteFault();
			}
		}

	template <typename T>
		void			write(const T&target, FILE*f)
		{
			write(&target,sizeof(T),f);
		}


public:
		const String	name;			//!< Image format name (ie. 'JPEG')
		Array<String>	extensions;		//!< Extensions supported by this image format
		Array<BYTE>	magic_bytes;	//!< Magic bytes (if any). May be up to 10 bytes
		
						ImageFormat(const String&name_, const String&ext):name(name_)	{explode(' ',ext,extensions);}
		
		/**
			@brief Attempts to write an image to the specified file
			
			May throw Exception::IO::DriveAccess, Exception::Program::FunctionalityNotImplemented
			
			@param image Image to save
			@param filename File path to write to
		*/
		void			saveToFile(const Image&image, const String&filename)
		{
			FILE*f = fopen(filename.c_str(),"wb");
			if (!f)
			{
				throw IO::DriveAccess::FileOpenFault("File not found or inaccessible for writing: '"+filename+"'");
			}
			try
			{
				saveToFilePointer(image,f);
				fclose(f);
			}
			catch (...)
			{
				fclose(f);
				throw;
			}
		}						
						
		/**
			@brief Attempts to write an image to the specified file
			
			May throw Exception::IO::DriveAccess, Exception::Program::FunctionalityNotImplemented
			
			@param image Image to save
			@param f File pointer to write to
		*/
virtual	void			saveToFilePointer(const Image&image, FILE*f)  =0;
		/**
			@brief Attempts to read an image from the specified file

			May throw Exception::IO::DriveAccess, Exception::Program::FunctionalityNotImplemented

			@param image Image to load
			@param filename File path to read from
		*/
		void			loadFromFile(Image&image, const String&filename) 
		{
			FILE*f = fopen(filename.c_str(),"rb");
			if (!f)
				throw IO::DriveAccess::FileOpenFault("File not found or inaccessible: '"+filename+"'");
			try
			{
				loadFromFilePointer(image,f);
				fclose(f);
			}
			catch (...)
			{
				fclose(f);
				throw;
			}
		}

		/**
			@brief Attempts to read an image from the specified file

			May throw Exception::IO::DriveAccess, Exception::Program::FunctionalityNotImplemented

			@param image Image to load
			@param file File reference to load from
		*/
virtual	void			loadFromFilePointer(Image&image, FILE*file) =0;

		/**
			@brief Queries whether or not the specified extension is supported by this image format
		*/
		bool			supports(const char*extension) const throw()
		{
			for (unsigned i = 0; i < extensions.count(); i++)
				if (!strcmpi(extensions[i].c_str(),extension))
					return true;
			return false;
		}

};



#endif
