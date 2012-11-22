#ifndef visual_archiveTplH
#define visual_archiveTplH


/******************************************************************

Visual structure archive.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


template <class Def> void ArchiveResource::add(Geometry<Def>*structure)
{
/*    for (unsigned i = 0; i < structure->textures.countEntries(); i++)
        add(structure->textures.entry(i));*/
    unsigned cnt = countEntries();
    for (unsigned i = 0; i < structure->materials; i++)
    {
        for (unsigned j = 0; j < structure->material[i].info.layers; j++)
        {
            structure->material[i].info.layer[j].source         =       TextureBuffer::addDuplicate(structure->material[i].info.layer[j].source);
            if (structure->material[i].info.layer[j].source)
                structure->material[i].info.layer[j].source_name    =       structure->material[i].info.layer[j].source->name;
        }
    }
    changed = changed || cnt != countEntries();
}






template <class Def> GeometryArchive<Def>::GeometryArchive(): ::Archive<TArchiveEntry>(VISUAL_VERSION),_changed(false)
{}

template <class Def> void GeometryArchive<Def>::handleChunk(RiffFile&riff,ArchiveFolder<TArchiveEntry>*current)
{
    switch (riff.getChunk().info.id)
    {
        case RIFF_STRC:
        {
            __int64 name;
            riff.openStream();
                riff.stream(name);
                selected = current->files.add(name);
                riff.stream(selected->crc);
                riff.stream(selected->extracted_size);
            riff.closeStream();
        }
        break;
        case RIFF_DATA:
        {
            if (!selected)
                return;
            selected->location = riff.getAddr();
            selected->size = riff.getSize();
            selected = NULL;
        }
        break;
        default:
            selected = NULL;
    }
}


template <class Def> bool GeometryArchive<Def>::open(const String&filename_)
{
    filename = filename_;
    _changed = false;
    textures.readContent(filename+".mta");
    return ::Archive<TArchiveEntry>::open(filename+".va");
}

template <class Def> bool GeometryArchive<Def>::create(const String&filename_)
{
    _changed = false;
    filename = filename_;
    RiffFile riff;
    if (!riff.create(String(filename+".va").c_str()))
        return false;
    UINT32  version = VISUAL_VERSION;
    riff.appendBlock("VERS",&version,sizeof(version));
    riff.close();
    textures.clear();
    return ::Archive<TArchiveEntry>::open(filename+".va");
}

template <class Def> Geometry<Def>*GeometryArchive<Def>::getData()
{
    Geometry<Def>*structure = SHIELDED(new Geometry<Def>());
    if (!getData(*structure))
    {
        DISCARD(structure);
        return NULL;
    }
    return structure;
}

template <class Def> bool GeometryArchive<Def>::getData(Geometry<Def>&target)
{
    if (!selected)
        return false;
    if (!selected->data)
        selected->data = RandomAccessFile::extract(selected->location,selected->size);
    if (!selected->data)
    {
        selected->size = 0;
        return false;
    }
    UINT32  crc = CRC32::getChecksum(selected->data,selected->size);
    if (crc != selected->crc)
    {
        selected->size = 0;
        return false;
    }
    BYTE*fbuffer = SHIELDED_ARRAY(new BYTE[selected->extracted_size],selected->extracted_size);
    unsigned extracted = BZ2::decompress(selected->data, selected->size, fbuffer, selected->extracted_size);
    if (!extracted)
    {
        ErrMessage("decompression failed while processing <"+name2str(selected->name)+">:\n\""+BZ2::errorStr()+"\"");
        DISCARD_ARRAY(fbuffer);
        selected->size = 0;
        return false;
    }
    selected->extracted_size = extracted;
    RiffFile riff;
    riff.assign(fbuffer,selected->extracted_size);
    target.loadEmbedded(riff,&textures);
    DISCARD_ARRAY(fbuffer);
    return true;
}

template <class Def> BYTE* GeometryArchive<Def>::getRaw(TArchiveEntry*entry)
{
    return RandomAccessFile::extract(entry->location,entry->size);
}


template <class Def> void GeometryArchive<Def>::add(Geometry<Def>*structure,__int64 name)
{
    _changed = true;
    while (folder[depth]->files.lookup(name))
        incrementName(name);
    textures.add(structure);
    TArchiveEntry*entry = folder[depth]->files.add(name);
    RiffChunk chunk;
    structure->saveEmbedded(chunk,false);
    entry->extracted_size = chunk.resolveSize(true);
    BYTE*data = alloc<BYTE>(entry->extracted_size);
    entry->data = alloc<BYTE>(entry->extracted_size);   //virtual link is larger than actual used memory... hmm
    chunk.toData(data,true);
    entry->size = entry->extracted_size;
    size_t temp_size = BZ2::compress(data,entry->extracted_size,entry->data,entry->size);
	entry->size = temp_size;
	if (!temp_size)
	{
		dealloc(entry->data);
		entry->data = NULL;
	}
    entry->crc = CRC32::getChecksum(entry->data,entry->size);
    dealloc(data);
}

template <class Def> void GeometryArchive<Def>::makeVirtual(ArchiveFolder<TArchiveEntry>*folder)
{
    folder->files.reset();
    while (TArchiveEntry*entry = folder->files.each())
        if (!entry->size)
            folder->files.erase();
        else
            if (!entry->data)
                entry->data = getRaw(entry);
    folder->reset();
    while (ArchiveFolder<TArchiveEntry>*sub = folder->each())
        makeVirtual(sub);
}

template <class Def> void GeometryArchive<Def>::putToRiff(ArchiveFolder<TArchiveEntry>*folder, RiffFile&riff)
{
    struct
    {
        __int64 name;
        UINT32  crc,extracted_size;
    }   out;
    folder->files.reset();
    while (TArchiveEntry*entry = folder->files.each())
    {
        out.name = entry->name;
        out.crc = entry->crc;
        out.extracted_size = entry->extracted_size;
        riff.appendBlock(RIFF_STRC,&out,sizeof(out));
        riff.appendBlock(RIFF_DATA,entry->data,entry->size);
    }
    folder->reset();
    while (ArchiveFolder<TArchiveEntry>*sub = folder->each())
    {
        riff.appendBlock(RIFF_FLDR,&sub->name,sizeof(sub->name));
        riff.appendBlock(RIFF_LIST);
        riff.enter();
            putToRiff(sub,riff);
        riff.dropBack();
    }
}

template <class Def> bool GeometryArchive<Def>::update()
{
    if (!_changed)
        return true;
    makeVirtual(folder[0]);
    RiffFile riff;
    if (!riff.create(String(filename+".va").c_str()))
        return false;
    UINT32  version = VISUAL_VERSION;
    riff.appendBlock("VERS",&version,sizeof(version));
    putToRiff(folder[0],riff);
    riff.close();
    return textures.write(filename+".mta");
}

#endif
