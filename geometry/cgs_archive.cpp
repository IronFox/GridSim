#include "../global_root.h"
#include "cgs_archive.h"

/******************************************************************

Visual structure archive.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

namespace CGS
{


TArchiveEntry::TArchiveEntry():data(NULL)
{}

TArchiveEntry::~TArchiveEntry()
{
    dealloc(data);
}

ArchiveResource::ArchiveResource():changed(false)
{}

count_t ArchiveResource::countEntries()
{
    return count();
}

TextureA*ArchiveResource::entry(unsigned index)
{
    return get(index);
}

TextureA*ArchiveResource::retrieve(const __int64&name)
{
    return lookup(name);
}




bool ArchiveResource::readContent(const String&filename)
{
    changed = false;
    clear();
    RiffFile riff;
    if (!riff.open(filename.c_str()))
        return false;
    if (!riff.findFirst("VERS") || riff.getSize() != sizeof(UINT32))
        return false; //riff auto-closes
    UINT32 vers,crc_fails(0);
    riff.get(&vers);
    if (vers != 0x0103)
        return false;
    TextureA*selected(NULL);
    if (riff.first())
        do
        {
            switch (riff.getChunk().info.id)
            {
                case RIFF_ID:
                    if (selected)
                        TextureBuffer::add(selected);

                    if (riff.getSize() == sizeof(__int64))
                    {
                        __int64 name;
                        riff.get(&name);
                        selected = SHIELDED(new TextureA());
                        selected->name = name;
                    }
                    else
                        selected = NULL;
                break;
                case RIFF_FACE:
                {
                    if (!selected)
                        continue;
					index_t index = selected->face_field.length();
					selected->face_field.resizePreserveContent(index+1);	//this has got to be the second most inefficient way to do this :S
					Array<BYTE>&face = selected->face_field[index];
					face.setSize(riff.getSize());
					riff.get(face.pointer());
						
                    UINT32 crc0 = *(UINT32*)face.pointer(),
                           crc1 = CRC32::getChecksum(face+(unsigned)sizeof(UINT32),face.size()-sizeof(UINT32));
                    if (crc0 != crc1)
                    {
						selected->face_field.resizePreserveContent(index);
                        crc_fails++;
                        continue;
                    }
                }
                break;
                default:
                    if (selected)
                        TextureBuffer::add(selected);
                    selected = NULL;
            }
        }
        while (riff.next());
    if (selected)
        TextureBuffer::add(selected);

	for (index_t i = 0; i < TextureBuffer::count(); i++)
		TextureBuffer::get(i)->updateHash();

    if (crc_fails)
        ErrMessage("CRC-Checksum failed "+IntToStr(crc_fails)+" time(s) while importing archive textures");
    return true;
}

bool ArchiveResource::writeContent(const String&filename)
{
    TextureBuffer::comment = "VisualArchive texture container.\nDO NOT MODIFY!!!";
    changed = changed && !TextureBuffer::write(filename);
    return !changed;
}

}
