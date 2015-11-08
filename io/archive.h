#ifndef archiveH
#define archiveH


/******************************************************************

Universal riff-archive-structure. Can be used for any type
of data-content in recursive folders with 8-byte-names.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include "../io/riff_handler.h"
#include "../string/int_name.h"
#include "../global_string.h"
#include "../container/lvector.h"
#include "../container/sortedlist.h"


#define ARCHIVE_ERROR_FILE_NOT_FOUND	(-1)
#define ARCHIVE_ERROR_VERSION_MISMATCH	(-2)
#define ARCHIVE_ERROR_UNABLE_TO_REOPEN	(-3)

#define RIFF_FLDR	0x52444C46	//"FLDR"

template <class> class ArchiveFolder;
template <class> class Archive;

/*!
	\brief Archive folder entry class

	The ArchiveFolder class maps all contained file and folder entries.
	Folder entries can be directly queried from the class itself, files need to be queried from the \b files list.
	The template Entry type is required to provide a public int64_t name variable.
*/
template <class Entry> class ArchiveFolder:public Named<::List::Vector<ArchiveFolder<Entry> > >
{
public:
		String						name;	//!< Name of this folder
		Named<::List::Vector<Entry> > files;	//!< List of all files located in this folder
};

/*!
	\brief	Generalized archive reader for RIFF based archives.
	
	Generalized archive reader for the RIFF based archive format.
	The template Entry type is required to provide a public int64_t name variable.
	Format specification (RIFF chunks):
<pre>
RIFF
{
	VERS
	{
		version: UINT32 
			version-integer of the file. This chunk may be located anywhere
			on the top most level. The version is considered 0 if no version-chunk
			is found .If the found version does not match the expected version,
			then the loading-procedure will fail.
	}
	CONT
	{
		content string: char[]
			May be any custom string. Use '\\n' to separate lines - zero-termination
			not recommended.
	}
	<file>
	{
		(...)
			Unknown chunks are passed to the client-class which may or may not
			load their content. Files may exist on any level.
	}
	FLDR
	{
		folder name: int64_t
			Folders with identical names are fused. A folder is initialized by a
			FLDR-chunk followed by a LIST-chunk. FLDR- and/or LIST-chunks that
			violate this rule are not recognized as folders and ignored.
	}
	LIST
	{
		(...) 
			a valid folder consist of a FLDR-chunk followed by a LIST-chunk containing
			files and/or sub-folders. Each folder may hold any number of files and
			subfolders in any order.
	}
}
</pre>
*/
template <class Entry> class Archive:protected RandomAccessFile
{

private:
			void					scanFolder(Riff::File&riff);

protected:

			Entry					*selected;			//!< Currently selected archive entry
			String					info;				//!< Archive info
			ArchiveFolder<Entry>	root_folder,		//!< Root folder
									*folder[0x100];		//!< Recursive folder path containing the currently selected folders. The entry at \b depth is the current working directory.
			String					name_path[0x100];	//!< Name path to the current working directory
			BYTE					depth;				//!< Folder stack depth
			UINT32					expected_version;	//!< Version to expect when loading an archive file.

virtual		void					handleChunk(Riff::File&riff,ArchiveFolder<Entry>*current);

public:
			typedef ArchiveFolder<Entry>	Folder;		//!< Folder type
			
			int						error_;								//!< Index of the last occured error (if any)
			
									Archive(UINT32 expected_version);	//!< Default constructor \param expected_version Version to expect when loading an archive file
virtual								~Archive();
			bool					Open(const PathString&filename);		//!< Attempts to load an archive file. The method fails if the file is no valid archive or the expected version does not match the found version. \param filename File to load. \return true if the specified archive file could be loaded, false otherwise. Use errorStr() to get a string representation of the last occured error if open() failed.
			void					Close();							//!< Closes the currently loaded archive. All loaded entries and folders will be erased.

inline		const String&			GetContent()								const;	//!< Retrieves the content description of the currently loaded archive file. \return Zero terminated content string.
inline		const String&			GetInfo()									const;	//!< Identical to getContent(). Retrieves the content description of the currently loaded archive file. \return Zero terminated content string.
inline		Entry*					Select(const String&name);							//!< Selects an entry in the current working directory via its name \param name Name of the entry to select
inline		Entry*					Select(unsigned index);								//!< Selects an entry in the current working directory via its index \param index Index of the entry to retrieve (0=first entry in the current working directory) \return Now selected entry (NULL if the specified index was invalid for the current working directory).
inline		bool					Select(Entry*entry);								//!< Attempts to select the specified entry. \param entry Pointer to an entry to select. The entry is required to be in the current working directory or the method will fail. \return true if the specified entry could be found in the current working directory, false otherwise. No entry is selected if the method returns false.
inline		Entry*					Select64(const tName&name);							//!< Selects an entry in the current working directory via its 64bit name \param name 64 bit integer name of the entry to select \return Now selected entry (NULL if the specified name could not be found in the current working directory).
inline		Entry*					GetSelected()								const;	//!< Retrieves a pointer to the currently selected entry \return Pointer to the currently selected entry or NULL if no entry is selected
inline		Folder*					GetLocation()									const;	//!< Queries the current working directory \return Pointer to the current working directory
inline		bool					SetPath(const Array<tName>&path);						//!< Attempts to locate a folder via its absolute name path \param path 64 bit integer name path (from the root folder). All elements are required to be sub-folders of the respective previous folder. The method fails if any of the specified names could not be found. \return true if the specified folder could be selected as the current working directory, false otherwise.
inline		bool					SetPath(const Array<String>&path);					//!< Attempts to locate a folder via its absolute name path \param pathz String name path (from the root folder). All elements are required to be sub-folders of the respective previous folder. The method fails if any of the specified names could not be found. \return true if the specified folder could be selected as the current working directory, false otherwise.
inline		bool					Enter(const tName&folder);							//!< Attempts to enter the specified sub folder from the current working directory \param folder Name of the sub folder to enter. If the folder exists then the current working directory will move to the specified sub folder. \return true if the specified sub folder exists and could be made the current working directory, false otherwise.
inline		bool					Enter(const String&folder);						//!< Attempts to enter the specified sub folder from the current working directory \param folder Name of the sub folder to enter. If the folder exists then the current working directory will move to the specified sub folder. \return true if the specified sub folder exists and could be made the current working directory, false otherwise.
inline		bool					DropBack();											//!< Returns to the next superior folder of the working directory. The method fails if no such exists. \return true if a superior folder existed and could be made the current working directory, false otherwise.
inline		void					DropToRoot();										//!< Returns to the archive root folder
inline		void					GetPath(Array<tName>&path)					const;	//!< Writes the name path to the current working directory to the specified name array. \param path Name array to write to. 
inline		void					GetPath(Array<String>&path)				const;	//!< Writes the name path to the current working directory to the specified name array. \param path Name array to write to. 
inline		String					GetLocationString()							const;	//!< Queries a string representation of the path to the current working directory. \return String containg the path to the current working directory. The folder names are trimmed and separated via '/'
inline		BYTE					GetPathDepth()									const;	//!< Queries the recursive folder depth of the current working directory. \return Current folder depth (1 = root folder) or 0 if no archive is currently loaded.
virtual		String					GetErrorStr()									const;	//!< Queries a string representation of the last occured error (if any). \return Error string.

									using RandomAccessFile::IsActive;
};

#include "archive.tpl.h"
#endif
