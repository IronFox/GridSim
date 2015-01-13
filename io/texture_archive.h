#ifndef texture_databaseH
#define texture_databaseH

/******************************************************************

Texture-archive.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/



#include "archive.h"
#include "../image/image.h"
#include "../remote/bz2/bz2.h"


#define RIFF_ID     0x20204449		//"ID  "
#define RIFF_FACE   0x45434146		//"FACE"
#define RIFF_NORM   0x4D524F4E		//"NORM"




class TextureArchive;
struct STAface;
struct STAentry;



//---------------------------------------------------------------------------


struct STAface	//! Texture archive face entry
{
        Riff::RIFF_SIZE     size;				//!< Size of this face in bytes in the file (may be compressed)
        Riff::RIFF_ADDR     location;			//!< Absolute file offset of this face
        UINT32              width,				//!< Width of this face in pixels
							height,				//!< Height of this face in pixels
                            crc;				//!< crc32 checksum of this face
        BYTE                channels,			//!< Number of color channels (i.e. 3 for an opaque face or 4 for an alpha-mapped one)
							codec;				//!< Used compression codec
		PixelType			content_type;		//!< Image content type
        Image             *extracted;			//!< Hook to the extracted image content (or NULL) to prevent multiple extraction of the same face. Automatically deleted by the STAface destructor if not NULL.
		
                            STAface();
virtual                    ~STAface();
};

struct STAentry	//!< Texture archive texture entry
{
        String             name;				//!< 64 bit name of this texture
        STAface             face[0xFF];			//!< Map to the faces of this texture (up to 255 faces per texture allowed)
        BYTE                faces;				//!< Number of faces actually used (up to 255). This value may be 0 (no faces).
		DW::Object			*custom_attachment;	//!< Custom reference pointer to attach additional data to a texture entry. Initially set to NULL but not used by TextureArchive and deleted by the STAentry destructor if not NULL
		
							STAentry():faces(0),custom_attachment(NULL)
							{}
virtual						~STAentry()	{if (custom_attachment) DISCARD(custom_attachment);}
};


/*!
	\brief Multi Texture Archive access interface.
	
	The TextureArchive class serves as a simplified method to extract images from an MTA texture archive.
	The underlying file is read-only and referenced until it is explicitly closed again or the object erased.
	External modifications to the MTA file while the TextureArchive object operates on it are fatal.
*/
class TextureArchive:public Archive<STAentry>
{
private:
        void                handleChunk(Riff::File&riff,ArchiveFolder<STAentry>*current);

public:
                            TextureArchive();
                            TextureArchive(const String&filename);	//!< Archive load constructor. \param filename Name of the archive file to load.
		BYTE				getFaces();						//!< Queries the number of faces attached to the currently selected texture. \return Number of faces attached to the currently selected texture
        const Image*       getData(BYTE face=0);			//!< Extracts one image (face) from the currently selected texture. \param face Index of the face to extract. \return Pointer to the internal Image data containing the extracted image or NULL if no texture was selected or the face index invalid. <b>Must not be deleted</b>.
        BYTE                getAll(const Image* out[]);	//!< Extracts all faces of the currently selected texture. \param out Image pointer field to write the faces to. Query getFaces() to insure the field has enough entries. The Image pointers written to the out field <b>must not be deleted</b> \return Number of faces attached to the currently selected texture
        void				getRaw(const STAface&face, ArrayData<BYTE>&target);			//!< Extracts the raw unmodified (compressed) data of the specified face. \param face Reference to the face to extract the data of. \param target Byte output
};
#endif
