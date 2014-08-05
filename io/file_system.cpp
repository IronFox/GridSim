#include "../global_root.h"
#include "file_system.h"
#include <algorithm>

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

	String	 File::GetName()  const
	{
		return name;
	}

	String	 File::GetInnerName()  const
	{
		return ExtractFileName(name);
	}
	
	String	 File::GetLocation() const
	{
		return location;
	}

	bool		File::IsFolder() const
	{
		return is_folder;
	}

	bool		File::IsDirectory() const
	{
		return is_folder;
	}


	String	 File::GetFolder()		 const
	{
		return ExtractFileDir(location);
	}

	const char* File::GetExtensionPointer()	const
	{
		index_t at = name.length();
		const char*field = name.c_str();
		while (at!=InvalidIndex && field[at] != '.')
			at--;
		if (at == InvalidIndex)
			return field+name.length();
		return field+at+1;
	}
	
	String  File::GetExtension()	  const
	{
		return GetExtensionPointer();
	}
	
	bool	File::IsExtension(const String&ext)		const
	{
		return !strcmpi(ext.c_str(),GetExtensionPointer());
	}
	
	bool	File::IsExtension(const char*ext)			const
	{
		return !strcmpi(ext,GetExtensionPointer());
	}


	bool	File::DoesExist()							const
	{
		return FileSystem::DoesExist(location);
	}


	ftime_t				File::GetModificationTime()				  const
	{
		return FileSystem::GetModificationTime(location);
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

	const File*	  File::GetSibling(const String&ext)  const
	{
		String sibling_location = ExtractFileName(location)+"."+ext;
		if (!FileSystem::DoesExist(sibling_location))
			return NULL;
		fs_file_sibling = *this;
		fs_file_sibling.name = ExtractFileName(name)+"."+ext;
		fs_file_sibling.location = sibling_location;
		return &fs_file_sibling;
	}




	String File::ToString() const
	{
		return is_folder?"Folder: "+location:"File: "+location;
	}

	bool File::Unlink() const
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





	TempFile::TempFile(bool docreate):created(docreate&&Create())
	{}


	TempFile::~TempFile()
	{
		if (created)
		{
			UnlinkFile(filename);
	//		cout << " - "<<filename.c_str()<<endl;
		}

	}


	bool				TempFile::Create()
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

	bool	TempFile::Destroy()
	{
		if (created)
		{
			MutexLock	lock(mutex);
			UnlinkFile(filename);
			created = false;
			return true;
		}
		return false;
	}


	const String&	  TempFile::GetLocation()
	{
		if (!created && !Create())
			FATAL__("Unable to create temporary file");
		return filename;
	}

	TempFile::operator const String& ()
	{
		if (!created && !Create())
			FATAL__("Unable to create temporary file");
		return filename;
	}



	String Drive::GetName()			const
	{
		return name;
	}

	String Drive::GetRoot()			const
	{
		return root;
	}

	String Drive::ToString() const
	{
		return name+" ("+root+")";
	}

	File Drive::ToFile()			const
	{
		File result;
		result.location = root;
		result.name = root;
		result.is_folder = true;
		return result;
	}



	Folder::Folder():valid_location(true),absolute_folder(GetWorkingDirectory()),find_handle(NULL)
	{}

	Folder::Folder(const String&folder_string) : valid_location(true), absolute_folder(GetWorkingDirectory()), find_handle(NULL)
	{
		MoveTo(folder_string);
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
		MoveTo(folder_string);
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
					absolute_folder = GetWorkingDirectory().subString(0, 3);
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
			absolute_folder = GetWorkingDirectory();
			valid_location = true;
		}

		final = absolute_folder;

		while (local.length())
		{
			if (!IsFolder(final))
				return false;
			String step;
			index_t	at0 = local.indexOf('/'),
					at1 = local.indexOf('\\'),
					at = !at0?at1:(!at1?at0:(std::min(at0,at1)));

			
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
				if (!Exit())
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
		valid_location = IsFolder(absolute_folder);
		return valid_location;
	}

	bool Folder::MoveTo(const String&folder_string)
	{
		closeScan();
		String final;
		valid_location = resolvePath(folder_string, final) && locate(final);
		return valid_location;
	}

	bool Folder::MoveTo(const File*file)
	{
		if (file)
			return locate(file->location);
		return false;
	}

	bool Folder::MoveTo(const File&file)
	{
		return locate(file.location);
	}

	bool Folder::Exit()
	{
		if (!valid_location)
			return false;
		#if SYSTEM==UNIX
			if (absolute_folder == '/' || absolute_folder == '\\')
				return false;
		#endif

		String current = absolute_folder,
				dir = ExtractFileNameExt(absolute_folder),
				super = ExtractFileDir(absolute_folder);
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

	bool Folder::Enter(const String&folder)
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

	bool Folder::Enter(const File&file)
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

	bool Folder::Enter(const File*file)
	{
		return file && Enter(*file);
	}

	bool Folder::IsValidLocation() const
	{
		return valid_location;
	}

	count_t  Folder::CountEntries() const
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

	void		 Folder::Reset()
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

	void 	Folder::Rewind()
	{
		Reset();
	}

	const Folder::File*  Folder::NextEntry()
	{
		if (NextEntry(file))
			return &file;
		return NULL;
	}

	bool Folder::NextEntry(File&file)
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
			return NextEntry(file);
		return true;
	}


	const Folder::File*	 Folder::NextFolder()
	{
		const File*file;
		do
		{
			file = NextEntry();
		}
		while (file && !file->is_folder);
		return file;
	}

	bool	Folder::NextFolder(File&target)
	{
		while (NextEntry(target))
			if (target.is_folder)
				return true;
		return false;
	}

	const Folder::File*	 Folder::NextFile()
	{
		const File*file;
		do
		{
			file = NextEntry();
		}
		while (file && file->is_folder);
		return file;
	}

	bool	Folder::NextFile(File&target)
	{
		while (NextEntry(target))
			if (!target.is_folder)
				return true;
		return false;
	}


	const Folder::File*	 Folder::NextFile(const String&extension)
	{
		const char*ext = extension.c_str();
		const File*file;
		do
		{
			file = NextEntry();
		}
		while (file && (file->is_folder || !file->GetLocation().endsWith(extension)));
		//while (file && (file->is_folder || strcmpi(file->GetExtensionPointer(),ext)));
		return file;
	}

	bool	Folder::NextFile(const String&extension, File&target)
	{
		const char*ext = extension.c_str();
		while (NextEntry(target))
			if (!target.is_folder && target.GetLocation().endsWith(extension))
				return true;
		return false;
	}



	const Folder::File*	 Folder::Find(const String&folder_str)	const
	{
		if (Find(folder_str,file))
			return &file;
		return NULL;
	}

	bool	 Folder::Find(const String&folder_str, File&out)	const
	{
		Folder	temporary = *this;
		if (!temporary.resolvePath(folder_str,out.location))
			return false;
		out.name = ExtractFileNameExt(out.location);

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

	const Folder::File*	 Folder::FindFile(const String&folder_str)	const
	{
		const File*rs = Find(folder_str);
		if (!rs || rs->is_folder)
			return NULL;
		return rs;
	}

	bool	 Folder::FindFile(const String&folder_str, File&out)	const
	{
		return Find(folder_str,out) && !out.is_folder;
	}

	bool	 Folder::FindFolder(const String&folder_str, File&out)	const
	{
		return Find(folder_str,out) && out.is_folder;
	}

	const Folder::File*	 Folder::FindFolder(const String&folder_str)	const
	{
		const File*rs = Find(folder_str);
		if (!rs || !rs->is_folder)
			return NULL;
		return rs;
	}

	const Folder::File*	 Folder::CreateFolder(const String&name)	const
	{
		if (!valid_location)
			return NULL;
		String location = absolute_folder;
		if (location.lastChar() != '/' && location.lastChar() != '\\')
			location += FOLDER_SLASH;
		location += name;
		return FileSystem::CreateFolder(location);
	}

	bool	 Folder::CreateFolder(const String&name, File&out)	const
	{
		if (!valid_location)
			return false;
		String location = absolute_folder;
		if (location.lastChar() != '/' && location.lastChar() != '\\')
			location += FOLDER_SLASH;
		location += name;
		return FileSystem::CreateFolder(location, out);
	}





	String		 Folder::ToString() const
	{
		if (!valid_location)
			return "Folder: "+absolute_folder+"(invalid)";
		return "Folder: "+absolute_folder;
	}

	const String&  Folder::LocationStr() const
	{
		return absolute_folder;
	}

	const String&  Folder::GetLocation() const
	{
		return absolute_folder;
	}


	const Folder::File* Folder::Location() const
	{
		if (!valid_location)
			return NULL;
		file.name = ExtractFileNameExt(absolute_folder);
		file.location = absolute_folder;
		file.is_folder = true;
		return &file;
	}

	const Folder::File* Folder::ParentLocation()	  const
	{
		if (!valid_location)
			return NULL;
		#if SYSTEM==UNIX
			if (absolute_folder == '/' || absolute_folder == '\\')
				return false;
		#endif

		String	 super = ExtractFileDir(absolute_folder);
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
		file.name = ExtractFileNameExt(super);
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



	String WorkingDirectory()
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

	String GetWorkingDirectory()
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

	bool  SetWorkingDirectory(const String&path)
	{
		#if SYSTEM==WINDOWS
			return !!SetCurrentDirectoryA(path.c_str());
		#elif SYSTEM==UNIX
			return !chdir(path.c_str());
		#else
			#error not supported
		#endif
	}

	bool  SetWorkingDirectory(const Folder&folder)
	{
		return SetWorkingDirectory(folder.GetLocation());
	}

	void		GetDriveList(Array<Drive>&out)
	{
    	count_t cnt = GetDriveList(NULL,0);
        out.setSize(cnt);
        GetDriveList(out.pointer(),out.length());
	}
	count_t GetDriveList(Drive*target, count_t max)
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

	String	ExtractFileName(const String&filename)
	{
		return _extractFileName(filename);
	}
	WString	ExtractFileName(const WString&filename)
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

	String	ExtractFileExt(const String&filename)
	{
		return _extractFileExt(filename);
	}
	WString	ExtractFileExt(const WString&filename)
	{
		return _extractFileExt(filename);
	}

	template <typename T>
		static StringTemplate<T>	_extractFileDir(const StringTemplate<T>&filename)
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

	String	ExtractFileDir(const String&filename)	{return _extractFileDir(filename);}
	WString	ExtractFileDir(const WString&filename)	{return _extractFileDir(filename);}

	template <typename T>
		static StringTemplate<T>	 _extractFileDirName(const StringTemplate<T>&filename)
		{
			index_t at(filename.length()-1);
			if (at >= filename.length())
				return StringTemplate<T>();
			while (filename.get(at) != (T)'/' && filename.get(at) != (T)'\\' && filename.get(at) != (T)'.' && --at < filename.length());
			if (at >= filename.length() || filename.get(at) != (T)'.')
				return filename;
			return filename.subString(0,at);
		}

	String	 ExtractFileDirName(const String&filename)	{return _extractFileDirName(filename);}
	WString	 ExtractFileDirName(const WString&filename)	{return _extractFileDirName(filename);}

	template <typename T>
		static StringTemplate<T>	 _extractFileNameExt(const StringTemplate<T>&filename)
		{
			index_t at = filename.length()-1;
			if (at>= filename.length())
				return StringTemplate<T>();
			while (filename.get(at) != (T)'/' && filename.get(at) != (T)'\\' && --at < filename.length());
			return filename.subString(at+1);
		}

	String	 ExtractFileNameExt(const String&filename)	{return _extractFileNameExt(filename);}
	WString	 ExtractFileNameExt(const WString&filename)	{return _extractFileNameExt(filename);}


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
	String	 EscapeSpaces(const String &path)	{return _escapeSpaces(path);}
	WString	 EscapeSpaces(const WString &path)	{return _escapeSpaces(path);}


	bool IsDirectory(const String&name)
	{
		return IsFolder(name);
	}

	bool IsFolder(const String&name)
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

	bool DoesExist(const String&name)
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

	ftime_t	FileDate(const String&name)
	{
		return GetModificationTime(name);
	}

	fsize_t		GetFileSize(const String&name)
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


	ftime_t GetModificationTime(const String&name)
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
			return std::max(wtime,ctime);
		#elif SYSTEM==UNIX
			struct ::stat s;
			if (stat(name.c_str(),&s))
				return 0;
			return s.st_ctime;
		#else
			#error not supported
		#endif
	}


	const File*  MoveFile(const String&source, const String&destination)
	{
		return Move(source, destination);
	}


	const File*  Move(const String&source, const String&destination)
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
		fs_result.name = ExtractFileNameExt(destination);
		fs_result.location = destination;
		fs_result.is_folder = IsFolder(destination);
		return &fs_result;
	}

	const File*  Copy(const String&source, const String&destination, bool overwrite_if_exist)
	{
		if (DoesExist(destination) && !overwrite_if_exist)
			return NULL;
		#if SYSTEM==WINDOWS
			String my_destination = destination;
			if (IsFolder(destination) && !IsFolder(source))
				my_destination += FOLDER_SLASH+ExtractFileNameExt(source);

			if (!CopyFileA(source.c_str(),my_destination.c_str(),!overwrite_if_exist) && system("copy \""+addSlashes(source)+"\" \""+addSlashes(destination)+"\""))
				return NULL;
		#elif SYSTEM==UNIX
			/*FILE*fsource = fopen(EscapeSpaces(source),"rb");
			if (!source)
				return NULL;
			FILE*ftarget = fopen(EscapeSpaces(destination),"wb");
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
		
			
			String call = ("cp "+EscapeSpaces(source)+" "+EscapeSpaces(destination));
			if (system(call))
				return NULL;
			//sleep(500);
		#else
			#error not supported
		#endif
		fs_result.name = ExtractFileNameExt(destination);
		fs_result.location = destination;
		fs_result.is_folder = IsFolder(destination);
		return &fs_result;
	}

	const File*  Copy(const File*source, const String&destination, bool overwrite)
	{
		if (!source)
			return NULL;
		return Copy(source->location,destination,overwrite);
	}

	const File*  Copy(const File&source, const String&destination, bool overwrite)
	{
		return Copy(source.location,destination,overwrite);
	}


	const File*  CopyFile(const String&source, const String&destination, bool overwrite_if_exist)
	{
		return Copy(source,destination,overwrite_if_exist);
	}

	const File*  CopyFile(const File*source, const String&destination, bool overwrite)
	{
		return Copy(source,destination,overwrite);
	}

	const File*  CopyFile(const File&source, const String&destination, bool overwrite)
	{
		return Copy(source,destination,overwrite);
	}


	const File*  Move(const File*source, const String&destination)
	{
		return Move(source->GetLocation(),destination);
	}

	const File*  Move(const File&source, const String&destination)
	{
		return Move(source.GetLocation(),destination);
	}


	const File*  MoveFile(const File*source, const String&destination)
	{
		return Move(source->GetLocation(),destination);
	}

	const File*  MoveFile(const File&source, const String&destination)
	{
		return Move(source.GetLocation(),destination);
	}

	const File*  CreateDirectory(const String&folder_name)
	{
		return CreateFolder(folder_name);
	}

	const File*  CreateFolder(const String&folder_name)
	{
		if (CreateFolder(folder_name,fs_result))
			return &fs_result;
		return NULL;
	}

	bool  CreateDirectory(const String&folder_name, File&out)
	{
		return CreateFolder(folder_name,out);
	}

	bool  CreateFolder(const String&folder_name, File&out)
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

	bool			UnlinkFile(const String&location)
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

	bool			RemoveFolder(const String&location)
	{
		#if SYSTEM==UNIX
			return !rmdir(location.c_str());
		#elif SYSTEM==WINDOWS
			return !!RemoveDirectoryA(location.c_str());
		#else
			#error not supported
		#endif


	}


	static const File* ResolveLink(const File*file)
	{
		#if SYSTEM==UNIX
			if (!file)
				return NULL;

			char buffer[0x200];
			int len = readlink(file->GetLocation(),buffer,sizeof(buffer));
			if (len == -1 || len ==sizeof(buffer))
				return file;
			buffer[len] = 0;	//appearingly readlink does not terminate the string by itself
	//		ShowMessage("symlink pointing to "+String(buffer));
			Folder folder(file->GetFolder());
			return folder.FindFile(buffer);
		#else

			return file;
		#endif
	}

	static bool ResolveLink(File&file)
	{
		#if SYSTEM==UNIX
			char buffer[0x200];
			int len = readlink(file.GetLocation(),buffer,sizeof(buffer));
			if (len == -1 || len >= sizeof(buffer))
				return true;
			buffer[len] = 0;	//appearingly readlink does not terminate the string by itself
	//		ShowMessage("symlink pointing to "+String(buffer));
			Folder folder(file.GetFolder());
			return folder.FindFile(buffer,file);
		#else

			return true;
		#endif
	}

	const File*  LocateExecutable(const char*name)
	{
		if (LocateExecutable(name,fs_result))
			return &fs_result;
		return NULL;
	}

	bool  LocateExecutable(const char*name, File&target)
	{
		String wd = GetWorkingDirectory();
		Folder folder(wd);

		String filename=name;

		#if SYSTEM==UNIX
			if (filename.Pos('/'))
		#endif
		{
			#if SYSTEM==WINDOWS
				if (folder.FindFile(filename,target))
					return ResolveLink(target);
				if (folder.FindFile(filename+".exe",target))
					return ResolveLink(target);
				if (folder.FindFile(wd+FOLDER_SLASH_STR+filename,target))
					return ResolveLink(target);
				if (folder.FindFile(wd+FOLDER_SLASH_STR+filename+".exe",target))
					return ResolveLink(target);
			#else
				if (folder.FindFile(filename,target))
					return ResolveLink(target);
				if (folder.FindFile(wd+FOLDER_SLASH_STR+filename,target))
					return ResolveLink(target);
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
					if (folder.FindFile(filename,target))
						return ResolveLink(target);
					if (folder.FindFile(filename+".exe",target))
						return ResolveLink(target);
					segment_begin = field+i+1;
				}
			{
				filename = String(segment_begin)+FOLDER_SLASH_STR+name;
				if (folder.FindFile(filename,target))
					return ResolveLink(target);
				if (folder.FindFile(filename+".exe",target))
					return ResolveLink(target);
			}
		}
		return false;
	}

	String			getAbsolutePath(const String&relative_path)
	{
		Folder	folder(GetWorkingDirectory());
		File file;
		if (folder.Find(relative_path,file))
			return file.GetLocation();
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
