#include "../global_root.h"
#include "texture_archive.h"

/******************************************************************

Texture-archive.

******************************************************************/



//BYTE TEXTURE_BUFFER[0x100000];


STAface::STAface():extracted(NULL)
{}

STAface::~STAface()
{
    if (extracted)
        DISCARD(extracted);
}

TextureArchive::TextureArchive():Archive<STAentry>(0x0103)
{}

TextureArchive::TextureArchive(const PathString&filename):Archive<STAentry>(0x0103)
{
    Open(filename);
}

void TextureArchive::handleChunk(Riff::File&riff,ArchiveFolder<STAentry>*current)
{
    switch (Riff::TID(riff.GetChunk().info.sid).Numeric())
    {
        case RIFF_ID:
        {
            String name;
			name.resize(riff.GetSize());
			riff.Get(name.mutablePointer());
			name.trimThis();
            selected = current->files.add(name);
            selected->faces = 0;
            selected->custom_attachment = NULL;
        }
        break;
        case RIFF_FACE:
        {
            if (!selected)
                return;
            STAface*face = &selected->face[selected->faces++];
            face->location = riff.GetAddr();
            face->size = riff.GetSize();
            Image::THeader   head;
            riff.Get(&head,sizeof(head));
            face->width = 0x1<<head.x_exp;
            face->height = 0x1<<head.y_exp;
            face->channels = head.channels;
            face->codec = head.type&TEX_CODEC_MASK;
			face->content_type = Image::GetContentType(head);
        }
        break;
    }
}


const Image* TextureArchive::GetData(BYTE face)
{
    if (!selected || face >= selected->faces)
        return NULL;
    if (selected->face[face].extracted)
        return selected->face[face].extracted;
    BYTE*data = RandomAccessFile::Extract(selected->face[face].location,selected->face[face].size);
    if (!data)
        return NULL;
    selected->face[face].extracted = SHIELDED(new Image());
    Image&target = *selected->face[face].extracted;
    target.SetSize(selected->face[face].width,selected->face[face].height,selected->face[face].channels);
    target.setContentType(selected->face[face].content_type);
    switch (selected->face[face].codec)
    {
        case 0:
            target.read(&data[sizeof(Image::THeader)]);
        break;
        case 1:
            ErrMessage("Built-in compression is no longer supported");
            DISCARD(selected->face[face].extracted);
            selected->face[face].extracted = NULL;
            return NULL;

            //target.decompress(&data[sizeof(Image::THeader)],selected->face[face].size-sizeof(Image::THeader));
        break;
        case 2:
        {
            if (!BZ2::decompress(&data[sizeof(Image::THeader)],selected->face[face].size-sizeof(Image::THeader),target.GetData(),target.size()))
            {
                ErrMessage("TextureArchive: BZ2-decompression failed ("+String(BZ2::errorStr())+")");
                DISCARD(selected->face[face].extracted);
                selected->face[face].extracted = NULL;
                return NULL;
            }
        }
        break;
        default:
            ErrMessage("TextureArchive: unexpected codec: "+IntToStr(selected->face[face].codec));
            DISCARD(selected->face[face].extracted);
            selected->face[face].extracted = NULL;
        return NULL;
    }
    DISCARD_ARRAY(data);
    return selected->face[face].extracted;
}

BYTE TextureArchive::GetFaces()
{
	if (!selected)
		return 0;
    return selected->faces;	
}

BYTE TextureArchive::GetAll(const Image* out[])
{
    if (!selected)
        return 0;
    /*unsigned width(0),height(0);
    BYTE     channels(0);*/
    for (BYTE k = 0; k < selected->faces; k++)
        out[k] = GetData(k);

	return selected->faces;
}

void	TextureArchive::GetRaw(const STAface&face, ArrayData<BYTE>&target)
{
	target.SetSize(face.size);
	RandomAccessFile::Extract(face.location,target.pointer(),face.size);
}




