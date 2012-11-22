#ifndef random_access_fileTplH
#define random_access_fileTplH

/******************************************************************

File-access tool providing more options for handling volumes.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/



template <class C> inline
	bool RandomAccessFile::insert(unsigned offset, const C&target)
	{
		return RandomAccessFile::insert(offset,&target,sizeof(target));
	}

template <class C> inline
	bool RandomAccessFile::append(const C&target)
	{
		return RandomAccessFile::append(&target,sizeof(target));
	}

template <class C> inline
	bool RandomAccessFile::read(unsigned offset, C&target)
	{
		return RandomAccessFile::extract(offset,&target,sizeof(target));
	}

template <class C> inline
	bool RandomAccessFile::overwrite(unsigned offset, const C&target)
	{
		return RandomAccessFile::overwrite(offset,&target,sizeof(target));
	}


#endif
