#ifndef magicH
#define magicH

/******************************************************************

Auto-Format handler.

******************************************************************/

#include "bitmap.h"
#include "jpeg.h"
#include "png.h"
#include "tga.h"





namespace Magic //! Multi format image access namespace
{
	extern	ImageFormat*	format[];

	void			GetExtensions(ArrayData<PathString>&extensions);	//!< Fills the specified array with all supported file extensions (lower case) \param extensions String array to write to
	void			GetExtensions(ArrayData<PathString>&extensions, ArrayData<ImageFormat*>&formats);	//!< Similar to the above but also exports the respective matching format descriptors \param extensions String array to write all supported extensions to \param formats Field to write the respective matching format descriptors to. Both fields are filled with the same number of items

	/**
	@brief Attempts to load an image file from the specified filename
			
	May throw Exception::IO::DriveAccess,Exception::Program::FunctionalityNotImplemented, Program::UnsupportedRequest

	The method will first attempt to match the retrieved magic bytes. Should that fail it will
	attempt to find a matching format via extension. If magic byte matching failed and no extension
	is available, loadFromFile() will sequentially try all known formats.
			
	\param filename File to load from. If the filename does not contain any supported extensions then all format descriptors will be tried
	\param target Out image to write to
	\param error_out Pointer to write an error description to or NULL if no error description is required. The contents are left unchanged if the function returns true
	\return true on success
	*/
	void			LoadFromFile(Image&target, const PathString&filename);
	/**
	@brief Extension specific version of the above.
			
	If the specified extension is NULL then this function behaves identical to the above

	May throw Exception::IO::DriveAccess,Exception::Program::FunctionalityNotImplemented, Program::UnsupportedRequest

			
	\param filename File to load from.
	\param extensions_override Pointer to a 0-terminated string describing the extension to use instead of any extension found in the filename. May be NULL. \param target Out image to write to
	\param error_out Pointer to write an error description to or NULL if no error description is required. The contents are left unchanged if the function returns true
	\return true on success
	*/
    void        	LoadFromFile(Image&target, const PathString&filename, const PathString::char_t*extension_override);
    void        	SaveToFile(const Image&image, const PathString&filename);	//!< Attempts to save an image to the specified file \param filename File to save to. The function fails if \b filename does not contain any supported extension \param image Image container to read image data from \param error_out Pointer to write an error description to or NULL if no error description is required. The contents are left unchanged if the function returns true \return true on success
    void        	SaveToFile(const Image&image, const PathString&filename, const PathString::char_t*extension_override);//!< Extension specific version of the above. If the specified extension is NULL then this function behaves identical to the above, \param filename File to save to \param extension_override Pointer to a zero terminated string describing the extension to use instead of any extension found in the filename. May be NULL. \param image Image container to read image data from \param error_out Pointer to write an error description to or NULL if no error description is required. The contents are left unchanged if the function returns true \return true on success
}



#endif
