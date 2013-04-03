#ifndef archiveTplH
#define archiveTplH

/******************************************************************

Universal riff-archive-structure. Can be used for any type
of data-content in recursive folders with 8-byte-names.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/



template <class Entry> void Archive<Entry>::handleChunk(RiffFile&riff,Folder*current)
{}

template <class Entry> Archive<Entry>::Archive(UINT32 version):selected(NULL),depth(0),expected_version(version)
{
    folder[0] = &root_folder;
}

template <class Entry> Archive<Entry>::~Archive()
{}

template <class Entry> void Archive<Entry>::scanFolder(RiffFile&riff)
{
    Folder*fldr(NULL);
    String name;
    if (riff.first())
        do
        {
            switch (riff.getChunk().info.id)
            {
                case RIFF_FLDR:
					name.resize(riff.getSize());
                    riff.get(name.mutablePointer());
					name.trimThis();
                    fldr = folder[depth]->add(name); //if two folders with the same name occurr they are fused
                break;
                case RIFF_LIST:
                    if (fldr)
                    {
                        folder[++depth] = fldr;
                        riff.enter();
                            scanFolder(riff);
                        riff.dropBack();
                        depth--;
                        fldr = NULL;
                    }
                break;
                default:
                    fldr = NULL;
                    handleChunk(riff,folder[depth]);
                break;
            }
        }
        while (riff.next());
}

template <class Entry> bool Archive<Entry>::open(const String&filename)
{
    RiffFile  riff;
    if (!riff.open(filename.c_str()))
    {
        error_ = ARCHIVE_ERROR_FILE_NOT_FOUND;
        return false;
    }
    UINT32   version = 0;
    if (riff.findFirst("VERS") && riff.getSize() == sizeof(version))
        riff.get(&version);
    if (version != expected_version)
    {
        riff.close();
        error_ = ARCHIVE_ERROR_VERSION_MISMATCH;
        return false;
    }

    if (riff.findFirst("CONT") || riff.findFirst("INFO"))
    {
		info.resize(riff.getSize());
		riff.get(info.mutablePointer());
		info.trimThis();
    }
    folder[0]->clear();
    folder[0]->files.clear();
    depth = 0;
    selected = NULL;
    scanFolder(riff);
    riff.close();
    error_ = ARCHIVE_ERROR_UNABLE_TO_REOPEN;
    return RandomAccessFile::open(filename.c_str());
}

template <class Entry> void Archive<Entry>::close()
{
    RandomAccessFile::close();
    folder[0]->clear();
    folder[0]->files.clear();
    depth = 0;
}

template <class Entry> inline const String&Archive<Entry>::getContent()   const
{
    return info;
}

template <class Entry> inline const String&Archive<Entry>::getInfo()   const
{
    return info;
}

template <class Entry> inline Entry*Archive<Entry>::select(const String&name)
{
    selected = folder[depth]->files.lookup(name);
    return selected;
}

template <class Entry> inline Entry*Archive<Entry>::select(unsigned index)
{
    selected = folder[depth]->files.get(index);
    return selected;
}

template <class Entry> inline bool Archive<Entry>::select(Entry*entry)
{
    if (folder[depth]->files.getIndexOf(entry))
        selected = entry;
    else
        selected = NULL;
    return selected;
}

template <class Entry> inline Entry*Archive<Entry>::select64(const int64_t&name)
{
    selected = folder[depth]->files.lookup(name2str(name));
    return selected;
}

template <class Entry> inline Entry*Archive<Entry>::getSelected() const
{
    return selected;
}


template <class Entry> inline ArchiveFolder<Entry>*Archive<Entry>::location()   const
{
    return folder[depth];
}

template <class Entry> inline bool Archive<Entry>::path(const Array<tName>&path)
{
    depth = 0;
	Folder*f;
    while (depth < path.length() && (f = folder[depth]->lookup(name2str(path[depth]))))
    {
		name_path[depth] = name2str(path[depth]);
        folder[++depth] = f;
    }
    return depth == path.length();
}

template <class Entry> inline bool Archive<Entry>::path(const Array<String>&path)
{
    depth = 0;
	Folder*f;
    while (depth < path.length() && (f = folder[depth]->lookup(path[depth])))
    {
        name_path[depth] = path[depth];
        folder[++depth] = f;
    }
    return depth == path.length();
}

template <class Entry> inline bool Archive<Entry>::enter(const tName&folder_name)
{
	return enter(name2str(folder_name));
}

template <class Entry> inline bool Archive<Entry>::enter(const String&folder_name)
{
    if (Folder*f = folder[depth]->lookup(folder_name))
    {
        name_path[depth] = folder_name;
        folder[++depth] = f;
        return true;
    }
    return false;
}

template <class Entry> inline bool Archive<Entry>::dropBack()
{
    if (!depth)
        return false;
    depth--;
    return true;
}

template <class Entry> inline void Archive<Entry>::dropToRoot()
{
	depth = 0;
}

template <class Entry> inline void Archive<Entry>::GetPath(Array<tName>&path)  const
{
	path.resize(depth);
    for (BYTE k = 0; k < depth; k++)
		path[k] = str2name(name_path[k]);
}

template <class Entry> inline void Archive<Entry>::GetPath(Array<String>&path)  const
{
	path.resize(depth);
    for (BYTE k = 0; k < depth; k++)
		path[k] = name_path[k];
}


template <class Entry> inline BYTE Archive<Entry>::pathDepth() const
{
    return depth;
}


template <class Entry> inline String Archive<Entry>::getLocationString()  const
{
    String result;
    for (BYTE k = 0; k < depth; k++)
        result+="/"+name_path[k];
    if (!depth)
        result = '/';
    return result;
}



template <class Entry> inline String Archive<Entry>::errorStr()    const
{
    #define ecase(token)    case token: return #token;
    switch (error_)
    {
        ecase(ARCHIVE_ERROR_FILE_NOT_FOUND)
        ecase(ARCHIVE_ERROR_VERSION_MISMATCH)
        ecase(ARCHIVE_ERROR_UNABLE_TO_REOPEN)
    }
    return "undefined archive-error";
    #undef ecase
}


#endif
