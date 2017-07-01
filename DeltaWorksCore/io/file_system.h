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

#undef CopyFile
#undef MoveFile
#undef CreateDirectory

namespace DeltaWorks
{

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

		PathString		ExtractFileExt(const PathString&filename);										//!< Extracts the file extension of \b filename without preceeding '.'
		PathString		ExtractFileDir(const PathString&filename);										//!< Extracts the file directory of \b filename without trailing slash
		PathString		ExtractFileName(const PathString&filename);										//!< Extracts the file name of \b filename without preceeding folder or succeeding extension (neither preceeding slashes or succeeding dots either)
		PathString		ExtractFileDirName(const PathString&filename);									//!< Extracts folders and file name of \b filename but no trailing extension (or extension dot)
		PathString		ExtractFileNameExt(const PathString&filename);									//!< Extracts the file name and extension of \b filename but no preceeding folders (or slashes)
		PathString		EscapeSpaces(const PathString&path);											//!< Inserts a backslash before every space character (for unix systems)
	
		ftime_t			GetModificationTime(const PathString&name);										//!< Returns the unix timestamp of the specified file \param name File name \return Unix timestamp
		fsize_t			GetFileSize(const PathString&name);												//!< Returns the size (in bytes) of the specified file
		bool			DoesExist(const PathString&path);												//!< Checks if the specified file or folder exists \param path File/folder name \return true if the specified file/folder exists, false otherwise
		bool			IsFile(const PathString::char_t*path);											//!< Checks if the specified file exists \param path Filename \return true if the entry exists in the file system and is a file, false otherwise
		bool			IsFile(const PathString&path);													//!< Checks if the specified file exists \param path Filename \return true if the entry exists in the file system and is a file, false otherwise
		bool			IsFolder(const PathString&path);												//!< Checks if the specified folder/directory exists (identical to IsDirectory()) \param path Folder name \return true if the entry exists in the file system and is a folder, false otherwise
		bool			IsDirectory(const PathString&path);												//!< Checks if the specified folder/directory exists (identical to IsFolder()) \param path Folder name \return true if the entry exists in the file system and is a folder, false otherwise
		PathString		WorkingDirectory(); 															//!< Returns the current working directory without trailing slash
		PathString		GetWorkingDirectory();	 														//!< Identical to GetWorkingDirectory()
		bool			SetWorkingDirectory(const PathString&path); 									//!< Sets the current working directory \param path new working directory
		bool			SetWorkingDirectory(const Folder&folder); 										//!< Sets the current working directory \param folder new working directory
		count_t			GetDriveList(Drive*target,count_t max);											//!< Queries system drives \param target Pointer to a drive field to write to \param max Number of entries in the drive field \return Total number of drives in the system
		void			GetDriveList(Array<Drive>&out);													//!< Queries system drives \param target Reference to an array to write to
		bool			Copy(const PathString&source, const PathString&destination, File&outFile, bool overwrite_if_exist);		//!< Identical to copyFile(). Copies \b source to \b destination . \param source Source file \param destination Destination file \param overwrite_if_exist Specifies whether or not \b destination should be overwritten if existing \return pointer to a global file object pointing to the destination file or NULL on failure
		const File*		Copy(const PathString&source, const PathString&destination, bool overwrite_if_exist);		//!< Identical to copyFile(). Copies \b source to \b destination . \param source Source file \param destination Destination file \param overwrite_if_exist Specifies whether or not \b destination should be overwritten if existing \return pointer to a global file object pointing to the destination file or NULL on failure
		const File*		Copy(const File*source, const PathString&destination, bool overwrite_if_exist);		//!< Identical to copyFile(). Copies \b source to \b destination . \param source Source file \param destination Destination file \param overwrite_if_exist Specifies whether or not \b destination should be overwritten if existing \return pointer to a global file object pointing to the destination file or NULL on failure
		const File*		Copy(const File&source, const PathString&destination, bool overwrite_if_exist);		//!< Identical to copyFile(). Copies \b source to \b destination . \param source Source file \param destination Destination file \param overwrite_if_exist Specifies whether or not \b destination should be overwritten if existing \return pointer to a global file object pointing to the destination file or NULL on failure
		const File*		CopyFile(const PathString&source, const PathString&destination, bool overwrite_if_exist);	//!< Copies \b source to \b destination . \param source Source file \param destination Destination file \param overwrite_if_exist Specifies whether or not \b destination should be overwritten if existing \return pointer to a global file object pointing to the destination file or NULL on failure
		const File*		CopyFile(const File*source, const PathString&destination, bool overwrite_if_exist);	//!< Copies \b source to \b destination . \param source Source file \param destination Destination file \param overwrite_if_exist Specifies whether or not \b destination should be overwritten if existing \return pointer to a global file object pointing to the destination file or NULL on failure
		const File*		CopyFile(const File&source, const PathString&destination, bool overwrite_if_exist);	//!< Copies \b source to \b destination . \param source Source file \param destination Destination file \param overwrite_if_exist Specifies whether or not \b destination should be overwritten if existing \return pointer to a global file object pointing to the destination file or NULL on failure
		const File*		Move(const PathString&source, const PathString&destination);								//!< Identical to MoveFile(). Moves \b source to \b destination . \param source Source file \param destination Destination file \return pointer to a global file object pointing to the destination file or NULL on failure
		const File*		Move(const File*source, const PathString&destination);								//!< Identical to MoveFile(). Moves \b source to \b destination . \param source Source file \param destination Destination file \return pointer to a global file object pointing to the destination file or NULL on failure
		const File*		Move(const File&source, const PathString&destination);								//!< Identical to MoveFile(). Moves \b source to \b destination . \param source Source file \param destination Destination file \return pointer to a global file object pointing to the destination file or NULL on failure
		const File*		MoveFile(const PathString&source, const PathString&destination);					//!< Moves \b source to \b destination . \param source Source file \param destination Destination file \return pointer to a global file object pointing to the destination file or NULL on failure
		const File*		MoveFile(const File*source, const PathString&destination);						//!< Moves \b source to \b destination . \param source Source file \param destination Destination file \return pointer to a global file object pointing to the destination file or NULL on failure
		const File*		MoveFile(const File&source, const PathString&destination);						//!< Moves \b source to \b destination . \param source Source file \param destination Destination file \return pointer to a global file object pointing to the destination file or NULL on failure
		const File*		CreateFolder(const PathString&folder_name);										//!< Creates one or more folders/directories specified by \b folder_name. Identical to createDirectory().	If \b folder_name is relative then it is created relative to the active working directory. \param folder_name Name of the folder to create \return pointer to a global file object pointing to the newly created folder or NULL on failure
		bool			CreateFolder(const PathString&folder_name, File&out);							//!< Creates one or more folders/directories specified by \b folder_name. Identical to createDirectory().	If \b folder_name is relative then it is created relative to the active working directory. \param folder_name Name of the folder to create \param out File reference to hold the newly created directory (if any). \return true on success
		const File*		CreateDirectory(const PathString&folder_name);									//!< Identical to CreateFolder(). Creates one or more folders/directories specified by \b folder_name. Identical to CreateFolder(). If \b folder_name is relative then it is created relative to the active working directory. \param folder_name Name of the folder to create \return pointer to a global file object pointing to the newly created folder or NULL on failure
		bool			CreateDirectory(const PathString&folder_name, File&out);						//!< Identical to CreateFolder(). Creates one or more folders/directories specified by \b folder_name. Identical to CreateFolder(). If \b folder_name is relative then it is created relative to the active working directory. \param folder_name Name of the folder to create \param out File reference to hold the newly created directory (if any). \return true on success
		bool			UnlinkFile(const PathString&filename);											//!< Unlinks (erases) the specified file \return true on success
		bool			RemoveFolder(const PathString&foldername);										//!< Recursively erases the specified folder and all contained files \return true on success, false if any one file or directory could not be deleted
		bool			RemoveFolderContents(const PathString&foldername);								//!< Recursively erases the contents of the specified folder \return true on success, false if any one file or directory could not be deleted
		PathString		GetRelativePath(const PathString&origin, const PathString&destination);	//!< Creates a relative path from \b origin to \b destination \return relative path
		PathString		GetAbsolutePath(const PathString&relative_path);	//!< Converts the specified relative path to an absolute path based on the current working directory.

		const File*		LocateExecutable(const char*name);				//!< Identifies a program executable depending on the global path variables. Also follows symlinks. Windows .lnk files not supported yet. \param name Name of the executable to locate \return Pointer to a global file object pointing to the executable or NULL if the executable could not be located
		bool			LocateExecutable(const char*name, File&target);	//!< Identifies a program executable depending on the global path variables. Also follows symlinks. Windows .lnk files not supported yet. \param name Name of the executable to locate \param target Target File to store the file target in. \return true on if the executable could be located, false otherwise

	


		/**
			\brief File/folder handle.

		*/
		class File
		{
		private:
			PathString		name,
							location;
			bool			is_folder;
			friend class	Folder;
			friend class	Drive;
			friend bool		FileSystem::Copy(const PathString&source, const PathString&destination, File&outFile, bool overwrite_if_exist);
			friend const File* FileSystem::Move(const PathString&source, const PathString&destination);
			friend const File* FileSystem::Move(const File*, const PathString&);
			friend const File* FileSystem::Move(const File&, const PathString&);
			friend bool  FileSystem::CreateFolder(const PathString&folder_name, File&out);
		public:
			/**/				File();
			/**/				File(const Drive&drive);

			void				swap(File&other)
			{
				name.swap(other.name);
				location.swap(other.location);
				swp(is_folder,other.is_folder);
			}
			friend void swap(File& a, File& b)	{a.swap(b);}

			const File*			GetSibling(const PathString&ext)		const;
			ftime_t				GetModificationTime()				const;		//!< Returns the unix filetime of the local entry \return Unix time stamp
			PathString			GetName()							const;		//!< Returns the name of the local entry (without preceeding folder(s)) \return Filename
			PathString			GetInnerName()						const;		//!< Returns the name of the local entry (without preceeding folder(s)) excluding any trailing extension \return Inner filename
			const PathString&	GetLocation()						const;		//!< Returns the absolute filename (including any preceeding folder(s)) \return Absolute location
			PathString			GetFolder()							const;		//!< Returns the absolute name of the parent folder without trailing slash \return Absolute location of the containing folder
			PathString			GetExtension()						const;		//!< Returns the file extension (without preceeding dot) \return String containing the file extension
			const PathString::char_t*			GetExtensionPointer()				const;		//!< Returns the file extension (without preceeding dot) \return Pointer to the beginning of the local filename's file extension. Does not return NULL
			bool				IsExtension(const PathString&ext)		const;		//!< Checks if the specified extension equals the local file's extension. Comparison is case insensitive. \return true if the specified extension matches the extension of the local file
			bool				IsFolder()							const;		//!< Returns true if the local entry is a folder/directory (identical to IsDirectory())	\return true if folder
			bool				IsDirectory()						const;		//!< Returns true if the local entry is a folder/directory (identical to IsFolder()) \return true if folder
			String				ToString()							const;		//!< Creates a string representation of the local entry \return String representation
			bool				DoesExist()							const;		//!< Queries existence of the local file or folder

			bool				Unlink()							const;		//!< Unlinks (erases) the local file/folder (making the local handle invalid) \return true on success
			
			bool				operator<(const File&other)			const;
			bool				operator>(const File&other)			const;
			bool				operator<=(const File&other)		const;
			bool				operator>=(const File&other)		const;
			bool				operator==(const File&other)		const;
			bool				operator!=(const File&other)		const;

			bool				operator<(const PathString&filename)	const;
			bool				operator>(const PathString&filename)	const;
			bool				operator<=(const PathString&filename)	const;
			bool				operator>=(const PathString&filename)	const;
			bool				operator==(const PathString&filename)	const;
			bool				operator!=(const PathString&filename)	const;
			
			operator const PathString&()								const
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
			PathString				filename;
			bool					created;
			static	System::Mutex	mutex;
		public:
			/**/					TempFile(bool create=false);
			virtual					~TempFile();
			bool					Create();						//!< Creates the associated temporary file if not already created \return true on success
			bool					Destroy();						//!< Destroys the associated temporary file if created \return true on success
			const PathString&		GetLocation();					//!< Retrieves the absolute location of the temporary file. Also creates the associated file if not aready created \return Absolute file location
			operator const 			PathString& ();					//!< Implicit string conversation. Also creates the associated file if not aready created \return Absolute file location
		};



		/**
			\brief Drive handle

		*/
		class Drive
		{
		private:
				PathString		root,
								name;
				friend class	Folder;
				friend class	File;
				friend count_t	FileSystem::GetDriveList(Drive*target,count_t max);

		public:
				PathString		GetName()			const;
				PathString		GetRoot()			const;
				PathString		ToString()			const;
				File			ToFile()			const;
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
				PathString			absolute_folder;
		static	File				file;
				#if SYSTEM == WINDOWS
					HANDLE			find_handle;
					WIN32_FIND_DATAW find_data;
				#elif SYSTEM == UNIX
					DIR				*find_handle;
				#endif

				void				closeScan();
				bool				locate(const PathString&path);
				bool				ResolvePathAndMoveTo(const PathString&path, bool moveToParent);
				bool				ResolvePath(const PathString&path, PathString*finalParent, PathString&final) const;
				static bool			ExitAbs(PathString&, bool check);
		public:



									Folder();
									Folder(const PathString&folder_string);
									Folder(const File&file);
									Folder(const File*file);
									Folder(const Folder&other);
		virtual						~Folder();
				Folder&				operator=(const Folder&other);
				Folder&				operator=(const File&file);
				Folder&				operator=(const File*file);
				Folder&				operator=(const PathString&folder_string);
				bool				MoveTo(const PathString&folder_string);	//!< Leaves the current location and moves to the specified folder. If goTo fails then Folder is in an invalid location. \param folder_string Absolute or relative folder to move to \return true on success
				bool				MoveTo(const File*file);				//!< Leaves current location and moves to the specified folder. If goTo fails then Folder is in an invalid location. \param file Handle to the folder to move to \return true on success
				bool				MoveTo(const File&file);				//!< Leaves current location and moves to the specified folder. If goTo fails then Folder is in an invalid location. \param file Handle to the folder to move to \return true on success
				bool				Exit();									//!< Attempts to drop back to the next superior folder. If no such exists or it cannot be entered then Folder will return to the original state. \return true on success
				bool				Enter(const File&file);					//!< Attempts to enter the specified sub folder. If no such exists or it cannot be entered then Folder will restore the original state. \param file Handle of the respective sub folder \return true on success
				bool				Enter(const File*file);					//!< Attempts to enter the specified sub folder. If no such exists or it cannot be entered then Folder will restore the original state. \param file Handle of the respective sub folder \return true on success
				bool				Enter(const PathString&folder_name);		//!< Attempts to enter the specified sub folder. If no such exists or it cannot be entered then Folder will restore the original state. \param folder_name Name of the respective sub folder \return true on success
				const File*			ParentLocation()	const;				//!< Queries parent location. Returns NULL if folder is top-most folder \return Pointer to a handle pointing to the respective parent folder or NULL if no such exists
				const File*			Location()			const;				//!< Queries location. Returns NULL if the current location is invalid \return Pointer to a handle pointing to the current location or NULL if the current location is invalid
				const PathString&	LocationStr()		const;				//!< Queries location. \return Absolute location
				const PathString&	GetLocation()		const;				//!< Queries location. \return Absolute location
				bool				IsValidLocation()	const;					//!< Queries location validity. \return true if Folder resides in a valid folder (retrieves content of a local variable)
				count_t				CountEntries()		const;				//!< Counts files/folders in the local folder \return Number of entries in the local folder
			
				void				Reset();									//!< Resets the folder cursor to the beginning of the folder
				void				Rewind();									//!< Identical to reset()
				const File*			NextEntry();								//!< Returns the next entry in the local folder \return Pointer to a file container pointing to the next entry in the local folder or NULL if no such exists
				bool				NextEntry(File&out);						//!< Identifies the next entry in the local folder \param out Target file structure to hold the next entry \return true if a next entry exists, false otherwise
				const File*			NextFolder();								//!< Returns the next folder in the local folder \return Pointer to a file container pointing to the next folder in the local folder or NULL if no such exists
				bool				NextFolder(File&out);						//!< Identifies the next folder in the local folder \param out Target file structure to hold the next folder \return true if a next folder exists, false otherwise
				const File*			NextFile();									//!< Returns the next file in the local folder \return Pointer to a file container pointing to the next file in the local folder or NULL if no such exists
				bool				NextFile(File&out);							//!< Identifies the next file in the local folder \param out Target file structure to hold the next file \return true if a next file exists, false otherwise
				const File*			NextFile(const PathString&extension);				//!< Returns the next file in the local folder matching the specified extension \return Pointer to a file container pointing to the next file matching the specified extension in the local folder or NULL of no such exists
				bool				NextFile(const PathString&extension, File&out);	//!< Identifies the next file in the local folder matching the specified extension \param extension File extension to look for (without succeeding dot) \param out Target file structure to hold the next matching file \return true if a next file of the specified extension exists, false otherwise
				const File*			Find(const PathString&path, bool mustExist=true)						const;	//!< Locates the specified file or folder starting from the current folder location. \param path Path to the file or folder that should be located \return Pointer to a file container pointing to the specified entry or NULL if the entry could not be located
				bool				Find(const PathString&path, File&out, bool mustExist=true)			const;	//!< Locates the specified file or folder starting from the current folder location. \param path Path to the file or folder that should be located \param out Out File structure reference. \return true if the entry could be located
				const File*			FindFile(const PathString&path, bool mustExist=true)					const;	//!< Locates the specified file starting from the current folder location \param path Path to the file that should be located \return Pointer to a file container pointing to the specified file or NULL if the file could not be located
				bool				FindFile(const PathString&path, File&out, bool mustExist=true)		const;	//!< Locates the specified file starting from the current folder location. \param path Path to the file or folder that should be located \param out Out File structure reference. \return true if the entry could be located
				const File*			FindFolder(const PathString&path, bool mustExist=true)				const;	//!< Locates the specified folder starting from the current folder location \param path Path to the folder that should be located \return Pointer to a file container pointing to the specified folder or NULL if the folder could not be located
				bool				FindFolder(const PathString&path, File&out, bool mustExist=true)		const;	//!< Locates the specified folder starting from the current folder location. \param path Path to the file or folder that should be located \param out Out File structure reference. \return true if the entry could be located
				const File*			CreateFolder(const PathString&name)				const;	//!< Creates the specified sub folder \param name Name of the sub folder to create \return Pointer to a file container pointing to the newly created folder or NULL if the folder could not be created
				bool				CreateFolder(const PathString&name, File&out)	const;	//!< Creates the specified sub folder \param name Name of the sub folder to create \param out Out File structure reference. \return true if the new folder could be created, false otherwise.
			
				String				ToString()		const;						//!< Creates a string representation of the local folder
			
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
}

#endif
