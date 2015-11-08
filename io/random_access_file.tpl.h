#ifndef random_access_fileTplH
#define random_access_fileTplH

/******************************************************************

File-access tool providing more options for handling volumes.

******************************************************************/



template <class C> inline
	bool RandomAccessFile::Insert(unsigned offset, const C&target)
	{
		return RandomAccessFile::Insert(offset,&target,sizeof(target));
	}

template <class C> inline
	bool RandomAccessFile::Append(const C&target)
	{
		return RandomAccessFile::Append(&target,sizeof(target));
	}

template <class C> inline
	bool RandomAccessFile::Read(unsigned offset, C&target)
	{
		return RandomAccessFile::Extract(offset,&target,sizeof(target));
	}

template <class C> inline
	bool RandomAccessFile::Overwrite(unsigned offset, const C&target)
	{
		return RandomAccessFile::Overwrite(offset,&target,sizeof(target));
	}


#endif
