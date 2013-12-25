#include "../global_root.h"
#include "file_system.h"

/******************************************************************

Plattform-independent file-browsing-module.

******************************************************************/


static	String addSlashes(const String&in)
{
	String rs = in;
	for (index_t i = 0; i < rs.length(); i++)
		if (rs.get(i) == '\\' || rs.get(i) == '\'' || rs.get(i) == '\"')
		{
			rs.insert(i,'\\');
			i++;
		}
	return rs;
}


namespace FileSystem
{

	Mutex	TempFile::mutex;

	File::File()
	{}

	File::File(const Drive&drive):name(drive.root),location(drive.root),is_folder(true)
	{}

	String	 File::getName()  const
	{
		return name;
	}

	String	 File::getInnerName()  const
	{
		return extractFileName(name);
	}
	
	String	 File::getLocation() const
	{
		return location;
	}

	bool		File::isFolder() const
	{
		return is_folder;
	}

	bool		File::isDirectory() const
	{
		return is_folder;
	}


	String	 File::getFolder()		 const
	{
		return extractFilePath(location);
	}

	const char* File::getExtensionPointer()	const
	{
		index_t at = name.length();
		const char*field = name.c_str();
		while (at!=InvalidIndex && field[at] != '.')
			at--;
		if (at == InvalidIndex)
			return field+name.length();
		return field+at+1;
	}
	
	String  File::getExtension()	  const
	{
		return getExtensionPointer();
	}
	
	bool	File::isExtension(const String&ext)		const
	{
		return !strcmpi(ext.c_str(),getExtensionPointer());
	}
	
	bool	File::isExtension(const char*ext)			const
	{
		return !strcmpi(ext,getExtensionPointer());
	}


	bool	File::doesExist()							const
	{
		return FileSystem::doesExist(location);
	}


	ftime_t				File::fileTime()				  const
	{
		return FileSystem::fileTime(location);
	}

	

	bool		  File::operator<(const File&other)  const
	{
    	#if SYSTEM==WINDOWS
			return strcmpi(location.c_str(),other.location.c_str())<0;
        #else
        	return location < other.location;
        #endif
	}

	bool		  File::operator>(const File&other)  const
	{
    	#if SYSTEM==WINDOWS
			return strcmpi(location.c_str(),other.location.c_str())>0;
        #else
        	return location > other.location;
        #endif
	}

	bool		  File::operator<=(const File&other)  const
	{
    	#if SYSTEM==WINDOWS
			return strcmpi(location.c_str(),other.location.c_str())<=0;
        #else
        	return location <= other.location;
        #endif
	}

	bool		  File::operator>=(const File&other)  const
	{
    	#if SYSTEM==WINDOWS
			return strcmpi(location.c_str(),other.location.c_str())>=0;
        #else
        	return location >= other.location;
        #endif
	}

	bool		  File::operator==(const File&other)  const
	{
    	#if SYSTEM==WINDOWS
        	return !strcmpi(location.c_str(),other.location.c_str());
        #else
			return location == other.location;
        #endif
	}

	bool		  File::operator!=(const File&other)  const
	{
    	#if SYSTEM==WINDOWS
        	return strcmpi(location.c_str(),other.location.c_str())!=0;
        #else
			return location != other.location;
        #endif

	}


	bool	 File::operator<(const String&filename)  const
	{
    	#if SYSTEM==WINDOWS
        	return strcmpi(location.c_str(),filename.c_str())<0;
        #else
			return name < filename;
        #endif
	}

	bool	 File::operator>(const String&filename)  const
	{
    	#if SYSTEM==WINDOWS
        	return strcmpi(location.c_str(),filename.c_str())>0;
        #else
			return name > filename;
        #endif
	}

	bool	 File::operator<=(const String&filename)  const
	{
    	#if SYSTEM==WINDOWS
        	return strcmpi(location.c_str(),filename.c_str())<=0;
        #else
			return name <= filename;
        #endif
	}

	bool	 File::operator>=(const String&filename)  const
	{
    	#if SYSTEM==WINDOWS
        	return strcmpi(location.c_str(),filename.c_str())>=0;
        #else
			return name >= filename;
        #endif
	}

	bool	 File::operator==(const String&filename)  const
	{
    	#if SYSTEM==WINDOWS
        	return strcmpi(location.c_str(),filename.c_str())==0;
        #else
			return name == filename;
        #endif
	}

	bool	 File::operator!=(const String&filename)  const
	{
    	#if SYSTEM==WINDOWS
        	return strcmpi(location.c_str(),filename.c_str())!=0;
        #else
			return name != filename;
        #endif
	}

	static File  fs_file_sibling;

	const File*	  File::sibling(const String&ext)  const
	{
		String sibling_location = extractFileName(location)+"."+ext;
		if (!FileSystem::doesExist(sibling_location))
			return NULL;
		fs_file_sibling = *this;
		fs_file_sibling.name = extractFileName(name)+"."+ext;
		fs_file_sibling.location = sibling_location;
		return &fs_file_sibling;
	}




	String File::ToString() const
	{
		return is_folder?"Folder: "+location:"File: "+location;
	}

	bool File::unlink() const
	{
		if (!this)
			return false;
		#if SYSTEM==UNIX
			if (is_folder)
				return !rmdir(location.c_str());
			return !::unlink(location.c_str());
		#elif SYSTEM==WINDOWS
			if (is_folder)
				return !!RemoveDirectoryA(location.c_str());
			return !!DeleteFileA(location.c_str());
		#else
			#error not supported
		#endif
	}





	TempFile::TempFile(bool docreate):created(docreate&&create())
	{}


	TempFile::~TempFile()
	{
		if (created)
		{
			unlinkFile(filename);
	//		cout << " - "<<filename.c_str()<<endl;
		}

	}


	bool				TempFile::create()
	{
		MutexLock	lock(mutex);
		String tmp_folder = getTempFolder();
		#if SYSTEM==WINDOWS

			static const size_t buffer_size=512;

			char	szTempName[buffer_size];
	/*		UINT	uRetVal;
			BOOL fSuccess;*/


		// Create a temporary file.
			if (!GetTempFileNameA(tmp_folder.c_str(), // directory for tmp files
								  "generic",		// temp file name prefix
								  0,			// create unique name
								  szTempName))  // buffer for name
				return false;


			filename = szTempName;
		#elif SYSTEM==UNIX
			static unsigned subsequent_call;
			srand(time(NULL)+(subsequent_call++));
			unsigned loops(0);
			do
			{
				filename = tmp_folder+"/generic."+IntToHex(rand(),4);
			}
			while (isFile(filename) && loops++ < 512);
			if (isFile(filename))
				return false;
			//filename = mktemp("/tmp/generic.XXXXXX");
			//filename = mkstemp("/tmp/generic.XXXXXX");
		#else
			#error not supported
		#endif

		FILE*tmp = fopen(filename.c_str(),"wb");
		if (!tmp)
			return false;
		fclose(tmp);
		created = true;

		#if SYSTEM==WINDOWS
			SetFileAttributesA(filename.c_str(),FILE_ATTRIBUTE_TEMPORARY);
		#endif

	//	cout << " + "<<filename.c_str()<<endl;
		return true;
	}

	bool	TempFile::destroy()
	{
		if (created)
		{
			MutexLock	lock(mutex);
			unlinkFile(filename);
			created = false;
			return true;
		}
		return false;
	}


	const String&	  TempFile::getLocation()
	{
		if (!created && !create())
			FATAL__("Unable to create temporary file");
		return filename;
	}

	TempFile::operator const String& ()
	{
		if (!created && !create())
			FATAL__("Unable to create temporary file");
		return filename;
	}



	String Drive::getName()			const
	{
		return name;
	}

	String Drive::getRoot()			const
	{
		return root;
	}

	String Drive::ToString() const
	{
		return name+" ("+root+")";
	}

	File Drive::toFile()			const
	{
		File result;
		result.location = root;
		result.name = root;
		result.is_folder = true;
		return result;
	}



	Folder::Folder():valid_location(true),absolute_folder(workingDirectory()),find_handle(NULL)
	{}

	Folder::Folder(const String&folder_string):valid_location(true),absolute_folder(workingDirectory()),find_handle(NULL)
	{
		moveTo(folder_string);
	}

	Folder::Folder(const File&file):valid_location(false),find_handle(NULL)
	{
		locate(file.location);
	}

	Folder::Folder(const File*file):valid_location(false),find_handle(NULL)
	{
		if (file)
			locate(file->location);
	}

	Folder::Folder(const Folder&folder):valid_location(folder.valid_location),absolute_folder(folder.absolute_folder),find_handle(NULL)
	{}

	Folder::~Folder()
	{
		closeScan();
	}

	Folder& Folder::operator=(const Folder&other)
	{
		closeScan();
		valid_location = other.valid_location;
		absolute_folder = other.absolute_folder;
	    return *this;
	}

	Folder& Folder::operator=(const File&file)
	{
		closeScan();
		valid_location = false;
		locate(file.location);
	    return *this;
	}

	Folder& Folder::operator=(const File*file)
	{
		closeScan();
		valid_location = false;
		locate(file->location);
	    return *this;
	}

	Folder& Folder::operator=(const String&folder_string)
	{
		closeScan();
		valid_location = false;
		moveTo(folder_string);
	    return *this;
	}




	void Folder::closeScan()
	{
		if (!find_handle)
			return;
		#if SYSTEM==WINDOWS
			FindClose(find_handle);
		#elif SYSTEM==UNIX
			closedir(find_handle);
		#else
			#error not supported
		#endif
		find_handle = NULL;
	}

	bool Folder::resolvePath(const String&path_string, String&final)
	{
		if (!path_string.length())
			return false;
		String local = path_string;
		#if SYSTEM==WINDOWS
			if (isalpha(path_string.firstChar()) && path_string.get(1) == ':')
			{
				absolute_folder = path_string.subString(0,3);
				local = path_string.subString(3);
				valid_location = true;
			}
			else
				if (path_string.firstChar() == '\\')
				{
					absolute_folder = workingDirectory().subString(0,3);
					local = path_string.subString(1);
					valid_location = true;
				}
		#elif SYSTEM==UNIX
			if (path_string.firstChar() == '/')
			{
				absolute_folder = "/";
				local = path_string.subString(1);
				valid_location = true;
			}
		#else
			#error not supported
		#endif

		if (!valid_location)
		{
			absolute_folder = workingDirectory();
			valid_location = true;
		}

		final = absolute_folder;

		while (local.length())
		{
			if (!isFolder(final))
				return false;
			String step;
			index_t	at0 = local.indexOf('/'),
					at1 = local.indexOf('\\'),
					at = !at0?at1:(!at1?at0:(min(at0,at1)));

			
			if (at)
			{
				step = local.subString(0,at-1);
				local.erase(0,at);
			}
			else
			{
				step = local;
				local = "";
			}
			if (step == '.' || !step.length())
				continue;
			if (step == "..")
			{
				if (!exit())
					return false;
				continue;
			}
			if (absolute_folder.lastChar() != '/'
				&&
				absolute_folder.lastChar() != '\\')
				absolute_folder+=FOLDER_SLASH;
			absolute_folder += step;
		}
		final = absolute_folder;
		return true;
	}




	bool Folder::locate(const String&folder_string)
	{
		absolute_folder = folder_string;
		if (absolute_folder.length() > 1 && (absolute_folder.lastChar() == '/' || absolute_folder.lastChar() == '\\'))
			absolute_folder.erase(absolute_folder.length()-1);
		valid_location = isFolder(absolute_folder);
		return valid_location;
	}

	bool Folder::moveTo(const String&folder_string)
	{
		closeScan();
		String final;
		valid_location = resolvePath(folder_string, final) && locate(final);
		return valid_location;
	}

	bool Folder::moveTo(const File*file)
	{
		if (file)
			return locate(file->location);
		return false;
	}

	bool Folder::moveTo(const File&file)
	{
		return locate(file.location);
	}

	bool Folder::exit()
	{
		if (!valid_location)
			return false;
		#if SYSTEM==UNIX
			if (absolute_folder == '/' || absolute_folder == '\\')
				return false;
		#endif

		String current = absolute_folder,
				dir = extractFileNameExt(absolute_folder),
				super = extractFilePath(absolute_folder);
		#if SYSTEM!=UNIX
			if (!super.length())
				return false;
		#else
			if (!super.length())
			{
				absolute_folder = FOLDER_SLASH;
				return true;
			}
		#endif
		if (!locate(super))
		{
			absolute_folder = current;
			valid_location = true;
			return false;
		}
		return true;
	}

	bool Folder::enter(const String&folder)
	{
		if (!valid_location)
			return false;
		String current = absolute_folder,
				target;
		if (!resolvePath(folder,target) || !locate(target))
		{
			absolute_folder = current;
			valid_location = true;
			return false;
		}
		return true;
	}

	bool Folder::enter(const File&file)
	{
		if (!valid_location)
			return false;
		String current = absolute_folder;
		if (!locate(file.location))
		{
			absolute_folder = current;
			valid_location = true;
			return false;
		}
		return true;
	}

	bool Folder::enter(const File*file)
	{
		return file && enter(*file);
	}

	bool Folder::validLocation() const
	{
		return valid_location;
	}

	count_t  Folder::countEntries() const
	{
		if (!valid_location)
			return 0;
		count_t	count = 0;

		#if SYSTEM==WINDOWS

			WIN32_FIND_DATAA result;
			HANDLE handle = FindFirstFileA(String(absolute_folder+"\\*").c_str(),&result);
			if (handle)
			{
				do
				{
					if (result.cFileName[0] != '.')
						count++;
				}
				while (FindNextFileA(handle,&result));
				FindClose(handle);
			}
		#elif SYSTEM==UNIX
			DIR*dir = opendir(absolute_folder.c_str());
			if (!dir)
				return 0;
			while (dirent*entry = readdir(dir))
				if (entry->d_name[0] != '.')
					count++;

			closedir(dir);
		#else
			#error not supported
		#endif
		return count;
	}

	void		 Folder::reset()
	{
		#if SYSTEM==WINDOWS
			if (find_handle)
				FindClose(find_handle);
			find_handle = FindFirstFileA(String(absolute_folder+"\\*").c_str(),&find_data);
		#elif SYSTEM==UNIX
			if (find_handle)
				rewinddir(find_handle);
			else
				find_handle = opendir(absolute_folder.c_str());
		#else
			#error not supported
		#endif
	}

	void 	Folder::rewind()
	{
		reset();
	}

	const Folder::File*  Folder::nextEntry()
	{
		if (nextEntry(file))
			return &file;
		return NULL;
	}

	bool Folder::nextEntry(File&file)
	{
		if (!find_handle)
			return false;
		bool retry=false;
		#if SYSTEM==WINDOWS
			file.name = find_data.cFileName;
			file.location = absolute_folder;
			if (file.location.lastChar() != '/' && file.location.lastChar() != '\\')
				file.location+=FOLDER_SLASH;
			file.location+= find_data.cFileName;
			file.is_folder = (find_data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0;
			if (!FindNextFileA(find_handle,&find_data))
			{
				FindClose(find_handle);
				find_handle = NULL;
			}
		#elif SYSTEM==UNIX
			dirent*entry = readdir(find_handle);
			if (!entry)
			{
				closeScan();
				return false;
			}
			file.name = entry->d_name;

			file.location = absolute_folder;
			if (file.location.lastChar() != '/' && file.location.lastChar() != '\\')
				file.location+=FOLDER_SLASH;
			file.location += entry->d_name;
			struct stat s;
			if (stat(file.location.c_str(),&s))
				retry = true;
			file.is_folder = s.st_mode&S_IFDIR;
		#else
			#error not supported
		#endif
		if (retry || !file.name.length() || file.name.firstChar() == '.')
			return nextEntry(file);
		return true;
	}


	const Folder::File*	 Folder::nextFolder()
	{
		const File*file;
		do
		{
			file = nextEntry();
		}
		while (file && !file->is_folder);
		return file;
	}

	bool	Folder::nextFolder(File&target)
	{
		while (nextEntry(target))
			if (target.is_folder)
				return true;
		return false;
	}

	const Folder::File*	 Folder::nextFile()
	{
		const File*file;
		do
		{
			file = nextEntry();
		}
		while (file && file->is_folder);
		return file;
	}

	bool	Folder::nextFile(File&target)
	{
		while (nextEntry(target))
			if (!target.is_folder)
				return true;
		return false;
	}


	const Folder::File*	 Folder::nextFile(const String&extension)
	{
		const char*ext = extension.c_str();
		const File*file;
		do
		{
			file = nextEntry();
		}
		while (file && (file->is_folder || !file->getLocation().endsWith(extension)));
		//while (file && (file->is_folder || strcmpi(file->getExtensionPointer(),ext)));
		return file;
	}

	bool	Folder::nextFile(const String&extension, File&target)
	{
		const char*ext = extension.c_str();
		while (nextEntry(target))
			if (!target.is_folder && target.getLocation().endsWith(extension))
				return true;
		return false;
	}



	const Folder::File*	 Folder::find(const String&folder_str)	const
	{
		if (find(folder_str,file))
			return &file;
		return NULL;
	}

	bool	 Folder::find(const String&folder_str, File&out)	const
	{
		Folder	temporary = *this;
		if (!temporary.resolvePath(folder_str,out.location))
			return false;
		out.name = extractFileNameExt(out.location);

		#if SYSTEM==WINDOWS
			DWORD attributes = GetFileAttributesA(out.location.c_str());
			if (attributes == INVALID_FILE_ATTRIBUTES)
				return false;
			out.is_folder = (attributes&FILE_ATTRIBUTE_DIRECTORY)!=0;
		#elif SYSTEM==UNIX
			struct stat s;
			if (stat(out.location,&s))
				return false;
			out.is_folder = s.st_mode&S_IFDIR;
		#else
			#error not supported
		#endif
		return true;
	}

	const Folder::File*	 Folder::findFile(const String&folder_str)	const
	{
		const File*rs = find(folder_str);
		if (!rs || rs->is_folder)
			return NULL;
		return rs;
	}

	bool	 Folder::findFile(const String&folder_str, File&out)	const
	{
		return find(folder_str,out) && !out.is_folder;
	}

	bool	 Folder::FindFolder(const String&folder_str, File&out)	const
	{
		return find(folder_str,out) && out.is_folder;
	}

	const Folder::File*	 Folder::FindFolder(const String&folder_str)	const
	{
		const File*rs = find(folder_str);
		if (!rs || !rs->is_folder)
			return NULL;
		return rs;
	}

	const Folder::File*	 Folder::createFolder(const String&name)	const
	{
		if (!valid_location)
			return NULL;
		String location = absolute_folder;
		if (location.lastChar() != '/' && location.lastChar() != '\\')
			location += FOLDER_SLASH;
		location += name;
		return FileSystem::createFolder(location);
	}

	bool	 Folder::createFolder(const String&name, File&out)	const
	{
		if (!valid_location)
			return false;
		String location = absolute_folder;
		if (location.lastChar() != '/' && location.lastChar() != '\\')
			location += FOLDER_SLASH;
		location += name;
		return FileSystem::createFolder(location, out);
	}





	String		 Folder::ToString() const
	{
		if (!valid_location)
			return "Folder: "+absolute_folder+"(invalid)";
		return "Folder: "+absolute_folder;
	}

	const String&  Folder::locationStr() const
	{
		return absolute_folder;
	}

	const String&  Folder::getLocation() const
	{
		return absolute_folder;
	}


	const Folder::File* Folder::location() const
	{
		if (!valid_location)
			return NULL;
		file.name = extractFileNameExt(absolute_folder);
		file.location = absolute_folder;
		file.is_folder = true;
		return &file;
	}

	const Folder::File* Folder::parentLocation()	  const
	{
		if (!valid_location)
			return NULL;
		#if SYSTEM==UNIX
			if (absolute_folder == '/' || absolute_folder == '\\')
				return false;
		#endif

		String	 super = extractFilePath(absolute_folder);
		#if SYSTEM==WINDOWS
			if (!super.length())
				return NULL;
		#elif SYSTEM==UNIX
			if (!super.length())
			{
				file.location = FOLDER_SLASH;
				file.name = FOLDER_SLASH;
				file.is_folder = true;
				return &file;
			}
		#else
			#error not supported
		#endif
		file.location = super;
		file.name = extractFileNameExt(super);
		file.is_folder = true;
		return &file;
	}


	bool				Folder::operator<(const Folder&other)  const
	{
		return absolute_folder < other.absolute_folder;
	}

	bool				Folder::operator>(const Folder&other)  const
	{
		return absolute_folder > other.absolute_folder;
	}

	bool				Folder::operator<=(const Folder&other)  const
	{
		return absolute_folder <= other.absolute_folder;
	}

	bool				Folder::operator>=(const Folder&other)  const
	{
		return absolute_folder >= other.absolute_folder;
	}

	bool				Folder::operator==(const Folder&other)  const
	{
		return absolute_folder == other.absolute_folder;
	}

	bool				Folder::operator!=(const Folder&other)  const
	{
		return absolute_folder != other.absolute_folder;
	}



	String workingDirectory()
	{
		char buffer[0x1000];
		#if SYSTEM==WINDOWS
			if (!GetCurrentDirectoryA(sizeof(buffer),buffer))
				return "";
			return buffer;
		#elif SYSTEM==UNIX
			if (!getcwd(buffer,sizeof(buffer)))
				return "";
			return buffer;
		#else
			#error not supported
		#endif
	}

	String getWorkingDirectory()
	{
		char buffer[0x1000];
		#if SYSTEM==WINDOWS
			if (!GetCurrentDirectoryA(sizeof(buffer),buffer))
				return "";
			return buffer;
		#elif SYSTEM==UNIX
			if (!getcwd(buffer,sizeof(buffer)))
				return "";
			return buffer;
		#else
			#error not supported
		#endif
	}

	bool  setWorkingDirectory(const String&path)
	{
		#if SYSTEM==WINDOWS
			return !!SetCurrentDirectoryA(path.c_str());
		#elif SYSTEM==UNIX
			return !chdir(path.c_str());
		#else
			#error not supported
		#endif
	}

	bool  setWorkingDirectory(const Folder&folder)
	{
		return setWorkingDirectory(folder.getLocation());
	}

	void		getDriveList(Array<Drive>&out)
	{
    	count_t cnt = getDriveList(NULL,0);
        out.setSize(cnt);
        getDriveList(out.pointer(),out.length());
	}
	count_t getDriveList(Drive*target, count_t max)
	{
		#if SYSTEM==UNIX
			if (max)
			{
				target->root="/";
				target->name="unix root-folder";
			}
			return 1;
		#elif SYSTEM==WINDOWS
			count_t cnt(0);
			char	volume_name[0x100],drive[] = "a:\\";
			for (char d = 'a'; d <= 'z'; d++)
			{
				drive[0] = d;
				UINT type = GetDriveTypeA(drive);
				bool valid = type == DRIVE_FIXED || type == DRIVE_REMOTE || type == DRIVE_CDROM || type == DRIVE_RAMDISK;
				if (!valid || !GetVolumeInformationA(drive, volume_name, sizeof(volume_name), NULL, NULL, NULL, NULL, 0))
					continue;
				if (cnt < max)
				{
					target[cnt].root = "a:";
					target[cnt].root.set(0,d);
					target[cnt].name=volume_name;
				}
				cnt++;
			}
			return cnt;
		#else
			#error not supported
		#endif
	}

	static File  fs_result;
	Folder::File  Folder::file;


	template<typename T>
		static StringTemplate<T>	_extractFileName(const StringTemplate<T>&filename)
		{
			index_t last_dot(filename.length()),last_slash(0);
			for (index_t i = 0; i < filename.length(); i++)
			{
				if (filename.get(i) == (T)'/' || filename.get(i) == (T)'\\')
					last_slash = i+1;
				if (filename.get(i) == (T)'.')
					last_dot = i;
			}
			if (last_dot < last_slash)
				last_dot = filename.length();
			return filename.subString(last_slash,last_dot-last_slash);
		}

	String	extractFileName(const String&filename)
	{
		return _extractFileName(filename);
	}
	WString	extractFileName(const WString&filename)
	{
		return _extractFileName(filename);
	}

	template <typename T>
		static StringTemplate<T>	_extractFileExt(const StringTemplate<T>&filename)
		{
			index_t at(filename.length()-1);
			while (filename.get(at) != (T)'/' && filename.get(at) != (T)'\\' && filename.get(at) != (T)'.' && --at < filename.length());
			if (at >= filename.length() || filename.get(at) != (T)'.')
				return StringTemplate<T>();
			return filename.subString(at+1);
		}

	String	extractFileExt(const String&filename)
	{
		return _extractFileExt(filename);
	}
	WString	extractFileExt(const WString&filename)
	{
		return _extractFileExt(filename);
	}

	template <typename T>
		static StringTemplate<T>	_extractFilePath(const StringTemplate<T>&filename)
		{
			if (!filename.length())
				return StringTemplate<T>();
			index_t at = filename.length()-1;
			while (filename.get(at) != (T)'\\' && filename.get(at) != (T)'/' && --at < filename.length());
			if (at >= filename.length())
				return (T)'.';
			/*if (at)
				at--;*/
			return filename.subString(0,at);
		}

	String	extractFilePath(const String&filename)	{return _extractFilePath(filename);}
	WString	extractFilePath(const WString&filename)	{return _extractFilePath(filename);}

	template <typename T>
		static StringTemplate<T>	 _extractFilePathName(const StringTemplate<T>&filename)
		{
			index_t at(filename.length()-1);
			if (at >= filename.length())
				return StringTemplate<T>();
			while (filename.get(at) != (T)'/' && filename.get(at) != (T)'\\' && filename.get(at) != (T)'.' && --at < filename.length());
			if (at >= filename.length() || filename.get(at) != (T)'.')
				return filename;
			return filename.subString(0,at);
		}

	String	 extractFilePathName(const String&filename)	{return _extractFilePathName(filename);}
	WString	 extractFilePathName(const WString&filename)	{return _extractFilePathName(filename);}

	template <typename T>
		static StringTemplate<T>	 _extractFileNameExt(const StringTemplate<T>&filename)
		{
			index_t at = filename.length()-1;
			if (at>= filename.length())
				return StringTemplate<T>();
			while (filename.get(at) != (T)'/' && filename.get(at) != (T)'\\' && --at < filename.length());
			return filename.subString(at+1);
		}

	String	 extractFileNameExt(const String&filename)	{return _extractFileNameExt(filename);}
	WString	 extractFileNameExt(const WString&filename)	{return _extractFileNameExt(filename);}


	template <typename T>
		static StringTemplate<T>	_escapeSpaces(StringTemplate<T> path)
		{
			while (index_t at = path.indexOf((T)"\\"))
				path.erase(at-1,1);
			StringTemplate<T> final;
			for (index_t i = 0; i < path.length(); i++)
			{
				if (path.get(i) == (T)' ')
					final += (T)'\\';
				final += path.get(i);
			}
			return final;
		}
	String	 escapeSpaces(const String &path)	{return _escapeSpaces(path);}
	WString	 escapeSpaces(const WString &path)	{return _escapeSpaces(path);}


	bool isDirectory(const String&name)
	{
		return isFolder(name);
	}

	bool isFolder(const String&name)
	{
		#if SYSTEM==WINDOWS
			DWORD attribs = GetFileAttributesA(name.c_str());
			return attribs != INVALID_FILE_ATTRIBUTES && (attribs&FILE_ATTRIBUTE_DIRECTORY);
		#elif SYSTEM==UNIX
			struct ::stat s;
			return !stat(name.c_str(),&s) && (s.st_mode&S_IFDIR);
		#else
			#error not supported
		#endif
	}

	bool isFile(const String&name)
	{
		#if SYSTEM==WINDOWS
			DWORD attribs = GetFileAttributesA(name.c_str());
			return attribs != INVALID_FILE_ATTRIBUTES && !(attribs&FILE_ATTRIBUTE_DIRECTORY);
		#elif SYSTEM==UNIX
			struct ::stat s;
			return !stat(name.c_str(),&s) && !(s.st_mode&S_IFDIR);
		#else
			#error not supported
		#endif
	}

	bool doesExist(const String&name)
	{
		#if SYSTEM==WINDOWS
			return GetFileAttributesA(name.c_str()) != INVALID_FILE_ATTRIBUTES;
		#elif SYSTEM==UNIX
			struct ::stat s;
			return !stat(name.c_str(),&s);
		#else
			#error not supported
		#endif
	}

	ftime_t	fileDate(const String&name)
	{
		return fileTime(name);
	}

	fsize_t		fileSize(const String&name)
	{
		#if SYSTEM==WINDOWS
			WIN32_FILE_ATTRIBUTE_DATA	data;
			if (!GetFileAttributesExA(name.c_str(),GetFileExInfoStandard,&data))
			{
				return 0;
			}
			return ((fsize_t)data.nFileSizeHigh) << 32 | (fsize_t)data.nFileSizeLow;
		#elif SYSTEM==UNIX
			struct ::stat s;
			if (stat(name.c_str(),&s))
				return 0;
			return s.st_size;
		#else
			#error not supported
		#endif
	}


	ftime_t fileTime(const String&name)
	{
		#if SYSTEM==WINDOWS
			WIN32_FILE_ATTRIBUTE_DATA	data;
			if (!GetFileAttributesExA(name.c_str(),GetFileExInfoStandard,&data))
			{
				return 0;
			}
			UINT64		ctime,
						wtime;
			memcpy(&ctime,&data.ftCreationTime,sizeof(ctime));
			memcpy(&wtime,&data.ftLastWriteTime,sizeof(wtime));

			wtime = (wtime - 116444736000000000ULL) / 10000000ULL;
			ctime = (ctime - 116444736000000000ULL) / 10000000ULL;
			return max(wtime,ctime);
		#elif SYSTEM==UNIX
			struct ::stat s;
			if (stat(name.c_str(),&s))
				return 0;
			return s.st_ctime;
		#else
			#error not supported
		#endif
	}


	const File*  moveFile(const String&source, const String&destination)
	{
		return move(source,destination);
	}


	const File*  move(const String&source, const String&destination)
	{
		#if SYSTEM==WINDOWS
			if (!MoveFileA(source.c_str(),destination.c_str()))
				return NULL;
		#elif SYSTEM==UNIX
			if (rename(source.c_str(),destination.c_str()))
				return NULL;
		#else
			#error not supported
		#endif
		fs_result.name = extractFileNameExt(destination);
		fs_result.location = destination;
		fs_result.is_folder = isFolder(destination);
		return &fs_result;
	}

	const File*  copy(const String&source, const String&destination, bool overwrite_if_exist)
	{
		if (doesExist(destination) && !overwrite_if_exist)
			return NULL;
		#if SYSTEM==WINDOWS
			String my_destination = destination;
			if (isFolder(destination) && !isFolder(source))
				my_destination += FOLDER_SLASH+extractFileNameExt(source);

			if (!CopyFileA(source.c_str(),my_destination.c_str(),!overwrite_if_exist) && system("copy \""+addSlashes(source)+"\" \""+addSlashes(destination)+"\""))
				return NULL;
		#elif SYSTEM==UNIX
			/*FILE*fsource = fopen(escapeSpaces(source),"rb");
			if (!source)
				return NULL;
			FILE*ftarget = fopen(escapeSpaces(destination),"wb");
			if (!ftarget)
			{
				fclose(fsource);
				return NULL;
			}
			
			BYTE buffer[1024];
			int read;
			do
			{
				read = fread(buffer,1,sizeof(buffer),fsource);
				fwrite(buffer,1,read,ftarget);
			}
			while (read == sizeof(buffer));
			
			fclose(fsource);
			fclose(ftarget);*/
		
			
			String call = ("cp "+escapeSpaces(source)+" "+escapeSpaces(destination));
			if (system(call))
				return NULL;
			//sleep(500);
		#else
			#error not supported
		#endif
		fs_result.name = extractFileNameExt(destination);
		fs_result.location = destination;
		fs_result.is_folder = isFolder(destination);
		return &fs_result;
	}

	const File*  copy(const File*source, const String&destination, bool overwrite)
	{
		if (!source)
			return NULL;
		return copy(source->location,destination,overwrite);
	}

	const File*  copy(const File&source, const String&destination, bool overwrite)
	{
		return copy(source.location,destination,overwrite);
	}


	const File*  copyFile(const String&source, const String&destination, bool overwrite_if_exist)
	{
		return copy(source,destination,overwrite_if_exist);
	}

	const File*  copyFile(const File*source, const String&destination, bool overwrite)
	{
		return copy(source,destination,overwrite);
	}

	const File*  copyFile(const File&source, const String&destination, bool overwrite)
	{
		return copy(source,destination,overwrite);
	}


	const File*  move(const File*source, const String&destination)
	{
		return move(source->getLocation(),destination);
	}

	const File*  move(const File&source, const String&destination)
	{
		return move(source.getLocation(),destination);
	}


	const File*  moveFile(const File*source, const String&destination)
	{
		return move(source->getLocation(),destination);
	}

	const File*  moveFile(const File&source, const String&destination)
	{
		return move(source.getLocation(),destination);
	}

	const File*  createDirectory(const String&folder_name)
	{
		return createFolder(folder_name);
	}

	const File*  createFolder(const String&folder_name)
	{
		if (createFolder(folder_name,fs_result))
			return &fs_result;
		return NULL;
	}

	bool  createDirectory(const String&folder_name, File&out)
	{
		return createFolder(folder_name,out);
	}

	bool  createFolder(const String&folder_name, File&out)
	{
		if (!folder_name.length())
			return false;
		out.location = folder_name;
		if (out.location.lastChar() == '/' || out.location.lastChar() == '\\')
			out.location.erase(out.location.length()-1);
		out.is_folder = true;
		#if SYSTEM==WINDOWS
			out.location.replace('/','\\');
			Array<String>	segments;
			explode('\\',out.location,segments);
			for (index_t i = 1; i <= segments.count(); i++)
			{
				String path = implode('\\',segments.pointer(),i);
				if (!CreateDirectoryA(path.c_str(),NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
					return false;
			}
		#elif SYSTEM==UNIX
			if (mkdir(out.location.c_str(), S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) && errno != EEXIST)
				return false;
		#else
			#error not supported
		#endif
		return true;
	}

	bool			unlinkFile(const String&location)
	{
		#if SYSTEM==UNIX
			return !::unlink(location.c_str());
		#elif SYSTEM==WINDOWS
			int retry = 0;
			String loc = location;
			loc.replace('/','\\');
			while (!DeleteFileA(loc.c_str()))
			{
				if (++retry > 10)
					return false;
				Sleep(100);
			}
			return true;
		#else
			#error not supported
		#endif


	}

	bool			removeFolder(const String&location)
	{
		#if SYSTEM==UNIX
			return !rmdir(location.c_str());
		#elif SYSTEM==WINDOWS
			return !!RemoveDirectoryA(location.c_str());
		#else
			#error not supported
		#endif


	}


	static const File* resolveLink(const File*file)
	{
		#if SYSTEM==UNIX
			if (!file)
				return NULL;

			char buffer[0x200];
			int len = readlink(file->getLocation(),buffer,sizeof(buffer));
			if (len == -1 || len ==sizeof(buffer))
				return file;
			buffer[len] = 0;	//appearingly readlink does not terminate the string by itself
	//		ShowMessage("symlink pointing to "+String(buffer));
			Folder folder(file->getFolder());
			return folder.findFile(buffer);
		#else

			return file;
		#endif
	}

	static bool resolveLink(File&file)
	{
		#if SYSTEM==UNIX
			char buffer[0x200];
			int len = readlink(file.getLocation(),buffer,sizeof(buffer));
			if (len == -1 || len >= sizeof(buffer))
				return true;
			buffer[len] = 0;	//appearingly readlink does not terminate the string by itself
	//		ShowMessage("symlink pointing to "+String(buffer));
			Folder folder(file.getFolder());
			return folder.findFile(buffer,file);
		#else

			return true;
		#endif
	}

	const File*  locateExecutable(const char*name)
	{
		if (locateExecutable(name,fs_result))
			return &fs_result;
		return NULL;
	}

	bool  locateExecutable(const char*name, File&target)
	{
		String wd = workingDirectory();
		Folder folder(wd);

		String filename=name;

		#if SYSTEM==UNIX
			if (filename.Pos('/'))
		#endif
		{
			#if SYSTEM==WINDOWS
				if (folder.findFile(filename,target))
					return resolveLink(target);
				if (folder.findFile(filename+".exe",target))
					return resolveLink(target);
				if (folder.findFile(wd+FOLDER_SLASH_STR+filename,target))
					return resolveLink(target);
				if (folder.findFile(wd+FOLDER_SLASH_STR+filename+".exe",target))
					return resolveLink(target);
			#else
				if (folder.findFile(filename,target))
					return resolveLink(target);
				if (folder.findFile(wd+FOLDER_SLASH_STR+filename,target))
					return resolveLink(target);
			#endif
		}

		{
			Array<char> field(getenv("PATH"));
			char*segment_begin = field.pointer();
			for (index_t i = 1; i < field.length()-1; i++)
				if (field[i] == ENV_PATH_SEPARATOR)
				{
					field[i] = 0;
					if (field[i-1] == '/' || field[i-1] == '\\')
						field[i-1] = 0;
					filename = String(segment_begin)+FOLDER_SLASH_STR+name;
					if (folder.findFile(filename,target))
						return resolveLink(target);
					if (folder.findFile(filename+".exe",target))
						return resolveLink(target);
					segment_begin = field+i+1;
				}
			{
				filename = String(segment_begin)+FOLDER_SLASH_STR+name;
				if (folder.findFile(filename,target))
					return resolveLink(target);
				if (folder.findFile(filename+".exe",target))
					return resolveLink(target);
			}
		}
		return false;
	}

	String			getAbsolutePath(const String&relative_path)
	{
		Folder	folder(getWorkingDirectory());
		File file;
		if (folder.find(relative_path,file))
			return file.getLocation();
		return relative_path;
	}

	String			getRelativePath(const String&origin, const String&destination)
	{
		const char	*o = origin.c_str(),
					*d = destination.c_str(),
					*of = origin.c_str(),
					*df = destination.c_str();
		while (*o && (*o == *d || (*o == '/' && *d == '\\') || (*o == '\\' && *d == '/')))
		{
			if (*o == '/' || *o == '\\')
			{
				of = o+1;
				df = d+1;
			}
			o++;
			d++;
		}
		if (!*o && ((*d == '/') || (*d == '\\')))
		{
			of = o;
			df = d+1;
		}
		if (!*d && ((*o == '/') || (*o == '\\')))
		{
			df = d;
			of = o+1;
		}
		
		if (of == origin.c_str())
			return destination; //no relative path found
	//	unsigned down(0);
		String rs;
		o = of;
		while (*o)
		{
			if ((*o == '/') || (*o == '\\'))
				rs  += "../";
			o++;
		}
		return rs + df;
	}

}
