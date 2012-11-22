#include "../global_root.h"
#include "texture_buffer.h"

/******************************************************************

Texture-buffer allowing to buffer/write simple texture-collections.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


char    TextureSort::last(0);



CGS::TextureA*TextureBuffer::lookupData(CGS::TextureA*data)
{
    return data_tree.lookup(data);
}


void TextureBuffer::add(CGS::TextureA*texture)
{
    if (!texture)
        return;
    if (texture->signature == this && texture->reference)
        return;
    texture->reference = data_tree.lookup(texture);
    texture->signature = this;
    if (!texture->reference)
    {
        while (lookup(texture->name))
            incrementName(texture->name);
        insert(texture);
        data_tree.insert(texture);
        texture->reference = texture;
    }
}



CGS::TextureA* TextureBuffer::addDuplicate(CGS::TextureA*texture)
{
    if (!texture)
        return NULL;
    if (texture->signature == this && texture->reference)
        return (CGS::TextureA*)texture->reference;
    texture->reference = data_tree.lookup(texture);
    texture->signature = this;
    if (!texture->reference)
    {
        CGS::TextureA*clone = SHIELDED(new CGS::TextureA(*texture));
        while (lookup(clone->name))
            incrementName(clone->name);
        insert(clone);
        data_tree.insert(clone);
        clone->reference = clone;
        clone->signature = this;
        texture->reference = clone;
    }
    return (CGS::TextureA*)texture->reference;
}

void TextureBuffer::flush()
{
    data_tree.flush();
    Named<Vector<CGS::TextureA> >::flush();
}

void TextureBuffer::clear()
{
    data_tree.clear();
    Named<Vector<CGS::TextureA> >::clear();
}


bool TextureBuffer::write(const String&filename)
{
    RiffFile  riff;
	if (comment.length() > TypeInfo<RIFF_SIZE>::max)
		return false;
    if (!riff.create(filename.c_str()))
        return false;
    UINT32 version = 0x0103;
    riff.appendBlock("VERS",&version,sizeof(version));
    riff.appendBlock("CONT",comment.c_str(),RIFF_SIZE(comment.length()));
    reset();
    while (CGS::TextureA*texture = each())
    {
        riff.appendBlock("ID  ",&texture->name,sizeof(texture->name));
        for (BYTE k = 0; k < texture->face_field.length(); k++)
		{
			if (texture->face_field[k].size() > TypeInfo<RIFF_SIZE>::max)
			{
				riff.close();
				return false;
			}
            riff.appendBlock("FACE",texture->face_field[k].pointer(),RIFF_SIZE(texture->face_field[k].size()));
		}
    }
    riff.close();
    return true;
}

String TextureBuffer::state()
{
    String rs = String(count())+" entry/ies\n";
    reset();
    for (unsigned i = 0; i < count(); i++)
    {
		CGS::TextureA*texture = each();
        rs+=" ("+String(i)+"): \""+name2str(texture->name)+"\" "+String(texture->face_field.length())+" face(s)\n";
    }
    return rs;
}
