#ifndef string_listTplH
#define string_listTplH



INLINE String StringList::fuse(size_t index, size_t count, const char*glue) const
{
	return fuse(index,count,glue,(size_t)strlen(glue));
}

INLINE String StringList::fuse(size_t index, size_t count, char glue) const
{
	return fuse(index,count,&glue,1);
}

INLINE String StringList::fuse(size_t index, size_t count, const String&glue) const
{
	return fuse(index,count,glue.c_str(),glue.length());
}

INLINE String StringList::fuse(size_t index, const String&glue) const
{
	return fuse(index, Count(), glue.c_str(), glue.length());
}

INLINE String StringList::fuse(size_t index, const char*glue) const
{
	return fuse(index, Count(), glue, (size_t)strlen(glue));
}

INLINE String StringList::fuse(size_t index, char glue) const
{
	return fuse(index, Count(), &glue, 1);
}

INLINE String StringList::implode(const String&glue)									const
{
	return fuse(0,Count(),glue.c_str(),glue.length());
}

INLINE String StringList::implode(const char*glue)									const
{
	return fuse(0,Count(),glue,(size_t)strlen(glue));
}

INLINE String StringList::implode(char glue)											const
{
	return fuse(0,Count(),&glue,1);
}


INLINE const String&StringList::operator[](size_t index)   const
{
	return Get(index);
}

INLINE String&StringList::operator[](size_t index)
{
	return Get(index);
}






#endif
