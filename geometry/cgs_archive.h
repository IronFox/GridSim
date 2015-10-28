#ifndef cgs_archiveH
#define cgs_archiveH

/******************************************************************

CGS structure archive.

******************************************************************/


#include "../math/object.h"
#include "cgs.h"
#include "../io/texture_archive.h"
#include "../structure/texture_buffer.h"

#define RIFF_STRC           0x43525453

#define VISUAL_VERSION      0x0101

namespace CGS
{


template <class> class   GeometryArchive;
class   ArchiveResource;
struct  TArchiveEntry;


struct TArchiveEntry	//!< Customized archive entry class
{
        Riff::RIFF_ADDR     location;		//!< File offset
        UINT32              extracted_size,	//!< Extracted data size in bytes
							size,			//!< Raw (compressed) data size in bytes
                            crc;			//!< Compressed crc32
        __int64             name;			//!< Entry name (64bit)
        BYTE               *data;			//!< Raw (compressed) data if expracted. Data is extracted on demand.

                            TArchiveEntry();
virtual                    ~TArchiveEntry();
};

class ArchiveResource:public TextureBuffer, public TextureResource	//! Geometry archive texture resource
{
private:
        bool                changed;
public:

                            ArchiveResource();
        count_t				countEntries();
        TextureA*           entry(unsigned index);
        TextureA*           retrieve(const __int64&name);

template <class Def>
        void                add(CGS::Geometry<Def>*geometry);	//!< Adds all textures provided by the specified geometry to the local texture resource
        bool                writeContent(const String&filename);	//!< Writes all textures to a separate material archive
        bool                readContent(const String&filename);	//!< Loads all textures from a separate material archive
};


template <class Def> class GeometryArchive:public Archive<TArchiveEntry>	//! Compressed geometry archive. Allows storage and retrieval of geometries
{
protected:
        bool                _changed;

        void                handleChunk(Riff::File&riff,ArchiveFolder<TArchiveEntry>*current);
        void                makeVirtual(ArchiveFolder<TArchiveEntry>*folder);
static  void                putToRiff(ArchiveFolder<TArchiveEntry>*folder, Riff::File&riff);

public:
        ArchiveResource    textures;		//!< Global archive textures
        String             filename;		//!< Most recent filename

                            GeometryArchive();

        bool                open(const String&filename);	//!< Opens an existing geometry archive
        bool                create(const String&filename);	//!< Creates a new geometry archive
        Geometry<Def>*		getData();								//!< Retrieves the currently selected geometry
        bool                getData(CGS::Geometry<Def>&target);	//!< Retrieves the currently selected geometry
        BYTE*               getRaw(TArchiveEntry*entry);					//!< Deprecated
        void                add(CGS::Geometry<Def>*structure,__int64 name);	//!< Adds a geometry to the archive using the specified name

        bool                update();	//!< Writes any changes to the underlying file
};


#include "cgs_archive.tpl.h"

}
#endif
