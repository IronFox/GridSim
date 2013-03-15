#ifndef file_systemH
#define file_systemH



#if SYSTEM == WINDOWS
	#include <winbase.h>
	#include <stdio.h>
#elif SYSTEM == UNIX
	#include <unistd.h>
	#include <dirent.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <errno.h>
#endif

#include <ctype.h>
#include "../global_string.h"
#include "../general/system.h"
#include "../general/thread_system.h"


/**
	\brief Plattform independent file system abstraction
	
	The FileSystem module attempts to create a more abstract view on the local file system, avoiding system specific peculiarities.
*/
namespace FileSystem
{
	class File;
	class Drive;
	class Folder;

	
	typedef unsigned long long ftime_t;	//!< File time type
	typedef unsigned long long fsize_t;
	
	
	#if SYSTEM==WINDOWS
	static const int		MaxDrives=26;
	#else
	static const int		MaxDrives=1;
	#endif

	String			extractFileExt(const String&filename);										//!< Extracts the file extension of \b filename without preceeding '.'
	String			extractFilePath(const String&filename);									//!< Extracts the file path of \b filename without trailing slash
	String			extractFileName(const String&filename);									//!< Extracts the file name of \b filename without preceeding folder or succeeding extension (neither preceeding slashes or succeeding dots either)
	String			extractFilePathName(const String&filename);								//!< Extracts folders and file name of \b filename but no trailing extension (or extension dot)
	String			extractFileNameExt(const String&filename);									//!< Extracts the file name and extension of \b filename but no preceeding folders (or slashes)
	String			escapeSpaces(String path);													//!< Inserts a backslash before every space character (for unix systems)
	
	ftime_t			fileTime(const String&name);												//!< Returns the unix timestamp of the specified file \param name File name \return Unix timestamp
	ftime_t			fileDate(const String&name);												//!< Identical to fileTime()
	fsize_t			fileSize(const String&name);												//!< Returns the size (in bytes) of the specified file
	bool			doesExist(const String&path);												//!< Checks if the specified file or folder exists \param path File/folder name \return true if the specified file/folder exists, false otherwise
	bool			isFile(const String&path);													//!< Checks if the specified file exists \param path Filename \return true if the entry exists in the file system and is a file, false otherwise
	bool			isFolder(const String&path);												//!< Checks if the specified folder/directory exists (identical to isDirectory()) \param path Folder name \return true if the entry exists in the file system and is a folder, false otherwise
	bool			isDirectory(const String&path);											//!< Checks if the specified folder/directory exists (identical to isFolder()) \param path Folder name \return true if the entry exists in the file system and is a folder, false otherwise
	String			workingDirectory(); 														//!< Returns the current working directory without trailing slash
	String			getWorkingDirectory(); 														//!< Identical to getWorkingDirectory()
	bool			setWorkingDirectory(const String&path); 									//!< Sets the current working directory \param path new working directory
	bool			setWorkingDirectory(const Folder&folder); 										//!< Sets the current working directory \param folder new working directory
	count_t			getDriveList(Drive*target,count_t max);											//!< Queries system drives \param target Pointer to a drive field to write to \param max Number of entries in the drive field \return Total number of drives in the system
	void			getDriveList(Array<Drive>&out);													//!< Queries system drives \param target Reference to an array to write to
	const File*	copy(const String&source, const String&destination, bool overwrite_if_exist);		//!< Identical to copyFile(). Copies \b source to \b destination . \param source Source file \param destination Destination file \param overwrite_if_exist Specifies whether or not \b destination should be overwritten if existing \return pointer to a global file object pointing to the destination file or NULL on failure
	const File*	copy(const File*source, const String&destination, bool overwrite_if_exist);		//!< Identical to copyFile(). Copies \b source to \b destination . \param source Source file \param destination Destination file \param overwrite_if_exist Specifies whether or not \b destination should be overwritten if existing \return pointer to a global file object pointing to the destination file or NULL on failure
	const File*	copy(const File&source, const String&destination, bool overwrite_if_exist);		//!< Identical to copyFile(). Copies \b source to \b destination . \param source Source file \param destination Destination file \param overwrite_if_exist Specifies whether or not \b destination should be overwritten if existing \return pointer to a global file object pointing to the destination file or NULL on failure
	const File*	copyFile(const String&source, const String&destination, bool overwrite_if_exist);	//!< Copies \b source to \b destination . \param source Source file \param destination Destination file \param overwrite_if_exist Specifies whether or not \b destination should be overwritten if existing \return pointer to a global file object pointing to the destination file or NULL on failure
	const File*	copyFile(const File*source, const String&destination, bool overwrite_if_exist);	//!< Copies \b source to \b destination . \param source Source file \param destination Destination file \param overwrite_if_exist Specifies whether or not \b destination should be overwritten if existing \return pointer to a global file object pointing to the destination file or NULL on failure
	const File*	copyFile(const File&source, const String&destination, bool overwrite_if_exist);	//!< Copies \b source to \b destination . \param source Source file \param destination Destination file \param overwrite_if_exist Specifies whether or not \b destination should be overwritten if existing \return pointer to a global file object pointing to the destination file or NULL on failure
	const File*	move(const String&source, const String&destination);								//!< Identical to moveFile(). Moves \b source to \b destination . \param source Source file \param destination Destination file \return pointer to a global file object pointing to the destination file or NULL on failure
	const File*	move(const File*source, const String&destination);								//!< Identical to moveFile(). Moves \b source to \b destination . \param source Source file \param destination Destination file \return pointer to a global file object pointing to the destination file or NULL on failure
	const File*	move(const File&source, const String&destination);								//!< Identical to moveFile(). Moves \b source to \b destination . \param source Source file \param destination Destination file \return pointer to a global file object pointing to the destination file or NULL on failure
	const File*	moveFile(const String&source, const String&destination);					//!< Moves \b source to \b destination . \param source Source file \param destination Destination file \return pointer to a global file object pointing to the destination file or NULL on failure
	const File*	moveFile(const File*source, const String&destination);					//!< Moves \b source to \b destination . \param source Source file \param destination Destination file \return pointer to a global file object pointing to the destination file or NULL on failure
	const File*	moveFile(const File&source, const String&destination);					//!< Moves \b source to \b destination . \param source Source file \param destination Destination file \return pointer to a global file object pointing to the destination file or NULL on failure
	const File*	createFolder(const String&folder_name);									//!< Creates one or more folders/directories specified by \b folder_name. Identical to createDirectory().	If \b folder_name is relative then it is created relative to the active working directory. \param folder_name Name of the folder to create \return pointer to a global file object pointing to the newly created folder or NULL on failure
	bool			createFolder(const String&folder_name, File&out);						//!< Creates one or more folders/directories specified by \b folder_name. Identical to createDirectory().	If \b folder_name is relative then it is created relative to the active working directory. \param folder_name Name of the folder to create \param out File reference to hold the newly created directory (if any). \return true on success
	const File*	createDirectory(const String&folder_name);									//!< Identical to createFolder(). Creates one or more folders/directories specified by \b folder_name. Identical to createFolder(). If \b folder_name is relative then it is created relative to the active working directory. \param folder_name Name of the folder to create \return pointer to a global file object pointing to the newly created folder or NULL on failure
	bool			createDirectory(const String&folder_name, File&out);					//!< Identical to createFolder(). Creates one or more folders/directories specified by \b folder_name. Identical to createFolder(). If \b folder_name is relative then it is created relative to the active working directory. \param folder_name Name of the folder to create \param out File reference to hold the newly created directory (if any). \return true on success
	bool			unlinkFile(const String&filename);											//!< Unlinks (erases) the specified file \return true on success
	bool			removeFolder(const String&foldername);										//!< Erases the specified folder \return true on success
	
	String			getRelativePath(const String&origin, const String&destination);	//!< Creates a relative path from \b origin to \b destination \return relative path
	String			getAbsolutePath(const String&relative_path);	//!< Converts the specified relative path to an absolute path based on the current working directory.

	const File*	locateExecutable(const char*name);				//!< Identifies a program executable depending on the global path variables. Also follows symlinks. Windows .lnk files not supported yet. \param name Name of the executable to locate \return Pointer to a global file object pointing to the executable or NULL if the executable could not be located
	bool			locateExecutable(const char*name, File&target);	//!< Identifies a program executable depending on the global path variables. Also follows symlinks. Windows .lnk files not supported yet. \param name Name of the executable to locate \param target Target File to store the file target in. \return true on if the executable could be located, false otherwise

	




	/**
		\brief File/folder handle.

	*/
	class File
	{
	private:
			String			name,
							location;
			bool			is_folder;
			friend class	Folder;
			friend class	Drive;
			friend const File* FileSystem::copy(const String&source, const String&destination, bool overwrite_if_exist);
			friend const File* FileSystem::copy(const File*, const String&, bool);
			friend const File* FileSystem::copy(const File&, const String&, bool);
			friend const File* FileSystem::move(const String&source, const String&destination);
			friend const File* FileSystem::move(const File*, const String&);
			friend const File* FileSystem::move(const File&, const String&);
			friend bool  FileSystem::createFolder(const String&folder_name, File&out);
	public:
								File();
								File(const Drive&drive);
	//virtual					~File(){};

			const File*		sibling(const String&ext)			const;
			ftime_t				fileTime()							const;		//!< Returns the unix filetime of the local entry \return Unix time stamp
			String				getName()							const;		//!< Returns the name of the local entry (without preceeding folder(s)) \return Filename
			String				getInnerName()						const;		//!< Returns the name of the local entry (without preceeding folder(s)) excluding any trailing extension \return Inner filename
			String				getLocation()						const;		//!< Returns the absolute filename (including any preceeding folder(s)) \return Absolute location
			String				getFolder()							const;		//!< Returns the absolute name of the parent folder without trailing slash \return Absolute location of the containing folder
			String				getExtension()						const;		//!< Returns the file extension (without preceeding dot) \return String containing the file extension
			const char*			getExtensionPointer()				const;		//!< Returns the file extension (without preceeding dot) \return Pointer to the beginning of the local filename's file extension. Does not return NULL
			bool				isExtension(const String&ext)		const;		//!< Checks if the specified extension equals the local file's extension. Comparison is case insensitive. \return true if the specified extension matches the extension of the local file
			bool				isExtension(const char*ext)			const;		//!< Checks if the specified extension equals the local file's extension. Comparison is case insensitive. \return true if the specified extension matches the extension of the local file
			bool				isFolder()							const;		//!< Returns true if the local entry is a folder/directory (identical to isDirectory())	\return true if folder
			bool				isDirectory()						const;		//!< Returns true if the local entry is a folder/directory (identical to isFolder()) \return true if folder
			String				toString()							const;		//!< Creates a string representation of the local entry \return String representation
			bool				doesExist()							const;		//!< Queries existence of the local file or folder

			bool				unlink()							const;		//!< Unlinks (erases) the local file/folder (making the local handle invalid) \return true on success
			
			bool				operator<(const File&other)		const;
			bool				operator>(const File&other)		const;
			bool				operator<=(const File&other)		const;
			bool				operator>=(const File&other)		const;
			bool				operator==(const File&other)		const;
			bool				operator!=(const File&other)		const;

			bool				operator<(const String&filename)	const;
			bool				operator>(const String&filename)	const;
			bool				operator<=(const String&filename)	const;
			bool				operator>=(const String&filename)	const;
			bool				operator==(const String&filename)	const;
			bool				operator!=(const String&filename)	const;
			
			operator const String&()								const
			{
				return location;
			}
	};

	/**
		\brief	Handle to a temporary file

	The temporary file is implicitly created when the handle is first queried as a string and is again erased if the handle object is erased.
	Alternatively it may be created explicitly by providing true as constructor parameter or calling create().
	Do not copy the object!
	*/
	class TempFile:public File
	{
	private:
			String				filename;
			bool				created;
	static	Mutex				mutex;
	public:
								TempFile(bool create=false);
	virtual						~TempFile();
			bool				create();						//!< Creates the associated temporary file if not already created \return true on success
			bool				destroy();						//!< Destroys the associated temporary file if created \return true on success
			const String&		getLocation();					//!< Retrieves the absolute location of the temporary file. Also creates the associated file if not aready created \return Absolute file location
			operator const 		String& ();					//!< Implicit string conversation. Also creates the associated file if not aready created \return Absolute file location
	};



	/**
		\brief Drive handle

	*/
	class Drive
	{
	private:
			String			root,
							name;
			friend class	Folder;
			friend class	File;
			friend count_t	FileSystem::getDriveList(Drive*target,count_t max);

	public:
			String			getName()			const;
			String			getRoot()			const;
			String			toString()			const;
			File			toFile()			const;
	};


	/**
		\brief Folder handle
	*/
	class Folder
	{
	public:
			typedef File	File;			//!< File entry
			typedef Drive	Drive;			//!< Drive entry
	private:
			bool				valid_location;
			String				absolute_folder;
	static	File				file;
			#if SYSTEM == WINDOWS
				HANDLE			find_handle;
				WIN32_FIND_DATAA find_data;
			#elif SYSTEM == UNIX
				DIR				*find_handle;
			#endif

			void				closeScan();
			bool				locate(const String&path);
			bool				resolvePath(const String&path, String&final);
	public:



								Folder();
								Folder(const String&folder_string);
								Folder(const File&file);
								Folder(const File*file);
								Folder(const Folder&other);
	virtual						~Folder();
			Folder&			operator=(const Folder&other);
			Folder&			operator=(const File&file);
			Folder&			operator=(const File*file);
			Folder&			operator=(const String&folder_string);
			bool				moveTo(const String&folder_string);	//!< Leaves the current location and moves to the specified folder. If goTo fails then Folder is in an invalid location. \param folder_string Absolute or relative folder to move to \return true on success
			bool				moveTo(const File*file);				//!< Leaves current location and moves to the specified folder. If goTo fails then Folder is in an invalid location. \param file Handle to the folder to move to \return true on success
			bool				moveTo(const File&file);				//!< Leaves current location and moves to the specified folder. If goTo fails then Folder is in an invalid location. \param file Handle to the folder to move to \return true on success
			bool				exit();									//!< Attempts to drop back to the next superior folder. If no such exists or it cannot be entered then Folder will return to the original state. \return true on success
			bool				enter(const File&file);					//!< Attempts to enter the specified sub folder. If no such exists or it cannot be entered then Folder will restore the original state. \param file Handle of the respective sub folder \return true on success
			bool				enter(const File*file);					//!< Attempts to enter the specified sub folder. If no such exists or it cannot be entered then Folder will restore the original state. \param file Handle of the respective sub folder \return true on success
			bool				enter(const String&folder_name);		//!< Attempts to enter the specified sub folder. If no such exists or it cannot be entered then Folder will restore the original state. \param folder_name Name of the respective sub folder \return true on success
			const File*			parentLocation()	const;				//!< Queries parent location. Returns NULL if folder is top-most folder \return Pointer to a handle pointing to the respective parent folder or NULL if no such exists
			const File*			location()			const;				//!< Queries location. Returns NULL if the current location is invalid \return Pointer to a handle pointing to the current location or NULL if the current location is invalid
			const String&		locationStr()		const;				//!< Queries location. \return Absolute location
			const String&		getLocation()		const;				//!< Queries location. \return Absolute location
			bool				validLocation()	const;					//!< Queries location validity. \return true if Folder resides in a valid folder (retrieves content of a local variable)
			count_t				countEntries()		const;				//!< Counts files/folders in the local folder \return Number of entries in the local folder
			
			void				reset();									//!< Resets the folder cursor to the beginning of the folder
			void				rewind();									//!< Identical to reset()
			const File*			nextEntry();								//!< Returns the next entry in the local folder \return Pointer to a file container pointing to the next entry in the local folder or NULL if no such exists
			bool				nextEntry(File&out);						//!< Identifies the next entry in the local folder \param out Target file structure to hold the next entry \return true if a next entry exists, false otherwise
			const File*			nextFolder();								//!< Returns the next folder in the local folder \return Pointer to a file container pointing to the next folder in the local folder or NULL if no such exists
			bool				nextFolder(File&out);						//!< Identifies the next folder in the local folder \param out Target file structure to hold the next folder \return true if a next folder exists, false otherwise
			const File*			nextFile();									//!< Returns the next file in the local folder \return Pointer to a file container pointing to the next file in the local folder or NULL if no such exists
			bool				nextFile(File&out);							//!< Identifies the next file in the local folder \param out Target file structure to hold the next file \return true if a next file exists, false otherwise
			const File*			nextFile(const String&extension);				//!< Returns the next file in the local folder matching the specified extension \return Pointer to a file container pointing to the next file matching the specified extension in the local folder or NULL of no such exists
			bool				nextFile(const String&extension, File&out);	//!< Identifies the next file in the local folder matching the specified extension \param extension File extension to look for (without succeeding dot) \param out Target file structure to hold the next matching file \return true if a next file of the specified extension exists, false otherwise
			const File*			find(const String&path)						const;	//!< Locates the specified file or folder starting from the current folder location. \param path Path to the file or folder that should be located \return Pointer to a file container pointing to the specified entry or NULL if the entry could not be located
			bool				find(const String&path, File&out)			const;	//!< Locates the specified file or folder starting from the current folder location. \param path Path to the file or folder that should be located \param out Out File structure reference. \return true if the entry could be located
			const File*			findFile(const String&path)					const;	//!< Locates the specified file starting from the current folder location \param path Path to the file that should be located \return Pointer to a file container pointing to the specified file or NULL if the file could not be located
			bool				findFile(const String&path, File&out)		const;	//!< Locates the specified file starting from the current folder location. \param path Path to the file or folder that should be located \param out Out File structure reference. \return true if the entry could be located
			const File*			findFolder(const String&path)				const;	//!< Locates the specified folder starting from the current folder location \param path Path to the folder that should be located \return Pointer to a file container pointing to the specified folder or NULL if the folder could not be located
			bool				findFolder(const String&path, File&out)		const;	//!< Locates the specified folder starting from the current folder location. \param path Path to the file or folder that should be located \param out Out File structure reference. \return true if the entry could be located
			const File*			createFolder(const String&name)				const;	//!< Creates the specified sub folder \param name Name of the sub folder to create \return Pointer to a file container pointing to the newly created folder or NULL if the folder could not be created
			bool				createFolder(const String&name, File&out)	const;	//!< Creates the specified sub folder \param name Name of the sub folder to create \param out Out File structure reference. \return true if the new folder could be created, false otherwise.
			
			String				toString()		const;						//!< Creates a string representation of the local folder
			
			bool				operator<(const Folder&other)	const;
			bool				operator>(const Folder&other)	const;
			bool				operator<=(const Folder&other)	const;
			bool				operator>=(const Folder&other)	const;
			bool				operator==(const Folder&other)	const;
			bool				operator!=(const Folder&other)	const;

	};


}

typedef FileSystem::File		CFSFile;
typedef FileSystem::Folder		CFSFolder;
typedef FileSystem::Drive		CFSDrive;
typedef FileSystem::TempFile	CFSTempFile;

#endif
