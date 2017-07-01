#ifndef archiveTplH
#define archiveTplH

/******************************************************************

Universal riff-archive-structure. Can be used for any type
of data-content in recursive folders with 8-byte-names.

******************************************************************/



template <class Entry> void Archive<Entry>::handleChunk(Riff::File&riff,Folder*current)
{}

template <class Entry> Archive<Entry>::Archive(UINT32 version):selected(NULL),depth(0),expected_version(version)
{
    folder[0] = &root_folder;
}

template <class Entry> Archive<Entry>::~Archive()
{}

template <class Entry> void Archive<Entry>::scanFolder(Riff::File&riff)
{
    Folder*fldr(NULL);
    String name;
    if (riff.First())
        do
        {
            switch (Riff::TID(riff.GetChunk().info.sid).Numeric())
            {
                case RIFF_FLDR:
					name.resize(riff.GetSize());
                    riff.Get(name.mutablePointer());
					name.trimThis();
                    fldr = folder[depth]->add(name); //if two folders with the same name occurr they are fused
                break;
                case RIFF_LIST:
                    if (fldr)
                    {
                        folder[++depth] = fldr;
                        riff.Enter();
                            scanFolder(riff);
                        riff.DropBack();
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
        while (riff.Next());
}

template <class Entry> bool Archive<Entry>::Open(const PathString&filename)
{
    Riff::File  riff;
    if (!riff.Open(filename.c_str()))
    {
        error_ = ARCHIVE_ERROR_FILE_NOT_FOUND;
        return false;
    }
    UINT32   version = 0;
    if (riff.FindFirst("VERS") && riff.GetSize() == sizeof(version))
        riff.Get(&version);
    if (version != expected_version)
    {
        riff.Close();
        error_ = ARCHIVE_ERROR_VERSION_MISMATCH;
        return false;
    }

    if (riff.FindFirst("CONT") || riff.FindFirst("INFO"))
    {
		info.resize(riff.GetSize());
		riff.Get(info.mutablePointer());
		info.trimThis();
    }
    folder[0]->clear();
    folder[0]->files.clear();
    depth = 0;
    selected = NULL;
    scanFolder(riff);
    riff.Close();
    error_ = ARCHIVE_ERROR_UNABLE_TO_REOPEN;
    return RandomAccessFile::Open(filename.c_str());
}

template <class Entry> void Archive<Entry>::Close()
{
    RandomAccessFile::Close();
    folder[0]->clear();
    folder[0]->files.clear();
    depth = 0;
}

template <class Entry> inline const String&Archive<Entry>::GetContent()   const
{
    return info;
}

template <class Entry> inline const String&Archive<Entry>::GetInfo()   const
{
    return info;
}

template <class Entry> inline Entry*Archive<Entry>::Select(const String&name)
{
    selected = folder[depth]->files.lookup(name);
    return selected;
}

template <class Entry> inline Entry*Archive<Entry>::Select(unsigned index)
{
    selected = folder[depth]->files.Get(index);
    return selected;
}

template <class Entry> inline bool Archive<Entry>::Select(Entry*entry)
{
    if (folder[depth]->files.getIndexOf(entry))
        selected = entry;
    else
        selected = NULL;
    return selected;
}

template <class Entry> inline Entry*Archive<Entry>::Select64(const int64_t&name)
{
    selected = folder[depth]->files.lookup(name2str(name));
    return selected;
}

template <class Entry> inline Entry*Archive<Entry>::GetSelected() const
{
    return selected;
}


template <class Entry> inline ArchiveFolder<Entry>*Archive<Entry>::GetLocation()   const
{
    return folder[depth];
}

template <class Entry> inline bool Archive<Entry>::SetPath(const Array<tName>&path)
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

template <class Entry> inline bool Archive<Entry>::SetPath(const Array<String>&path)
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

template <class Entry> inline bool Archive<Entry>::Enter(const tName&folder_name)
{
	return enter(name2str(folder_name));
}

template <class Entry> inline bool Archive<Entry>::Enter(const String&folder_name)
{
    if (Folder*f = folder[depth]->lookup(folder_name))
    {
        name_path[depth] = folder_name;
        folder[++depth] = f;
        return true;
    }
    return false;
}

template <class Entry> inline bool Archive<Entry>::DropBack()
{
    if (!depth)
        return false;
    depth--;
    return true;
}

template <class Entry> inline void Archive<Entry>::DropToRoot()
{
	depth = 0;
}

template <class Entry> inline void Archive<Entry>::GetPath(Array<tName>&path)  const
{
	path.SetSize(depth);
    for (BYTE k = 0; k < depth; k++)
		path[k] = str2name(name_path[k]);
}

template <class Entry> inline void Archive<Entry>::GetPath(Array<String>&path)  const
{
	path.SetSize(depth);
    for (BYTE k = 0; k < depth; k++)
		path[k] = name_path[k];
}


template <class Entry> inline BYTE Archive<Entry>::GetPathDepth() const
{
    return depth;
}


template <class Entry> inline String Archive<Entry>::GetLocationString()  const
{
    String result;
    for (BYTE k = 0; k < depth; k++)
        result+="/"+name_path[k];
    if (!depth)
        result = '/';
    return result;
}



template <class Entry> inline String Archive<Entry>::GetErrorStr()    const
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
