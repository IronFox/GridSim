#include "../global_root.h"
#include "file_system.h"
#include <algorithm>

/******************************************************************

Plattform-independent file-browsing-module.

******************************************************************/





namespace FileSystem
{
	#if SYSTEM==WINDOWS
		#define ABS_MARKER	L"\\\\?\\"
		#define ABS_MARKER_LENGTH	(ARRAYSIZE(ABS_MARKER)-1)
	#endif

	static	PathString addSlashes(const PathString&in)
	{
		PathString rs = in;
		for (index_t i = 0; i < rs.length(); i++)
			if (rs.get(i) == '\\' || rs.get(i) == '\'' || rs.get(i) == '\"')
			{
				rs.insert(i,'\\');
				i++;
			}
		return rs;
	}




	Mutex	TempFile::mutex;

	File::File()
	{}

	File::File(const Drive&drive):name(drive.root),location(drive.root),is_folder(true)
	{}

	PathString	 File::GetName()  const
	{
		return name;
	}

	PathString	 File::GetInnerName()  const
	{
		return ExtractFileName(name);
	}
	
	PathString	 File::GetLocation() const
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


	PathString	 File::GetFolder()		 const
	{
		return ExtractFileDir(location);
	}

	const PathString::char_t* File::GetExtensionPointer()	const
	{
		index_t at = name.length();
		const PathString::char_t*field = name.c_str();
		while (at!=InvalidIndex && field[at] != '.')
			at--;
		if (at == InvalidIndex)
			return field+name.length();
		return field+at+1;
	}
	
	PathString  File::GetExtension()	  const
	{
		return GetExtensionPointer();
	}
	
	bool	File::IsExtension(const PathString&ext)		const
	{
		return ext == GetExtensionPointer();
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
        return location < other.location;
	}

	bool		  File::operator>(const File&other)  const
	{
        return location > other.location;
	}

	bool		  File::operator<=(const File&other)  const
	{
        return location <= other.location;
	}

	bool		  File::operator>=(const File&other)  const
	{
        return location >= other.location;
	}

	bool		  File::operator==(const File&other)  const
	{
		return location == other.location;
	}

	bool		  File::operator!=(const File&other)  const
	{
		return location != other.location;
	}


	bool	 File::operator<(const PathString&filename)  const
	{
		return location < filename;
	}

	bool	 File::operator>(const PathString&filename)  const
	{
		return location > filename;
	}

	bool	 File::operator<=(const PathString&filename)  const
	{
		return location <= filename;
	}

	bool	 File::operator>=(const PathString&filename)  const
	{
		return location >= filename;
	}

	bool	 File::operator==(const PathString&filename)  const
	{
		return location == filename;
	}

	bool	 File::operator!=(const PathString&filename)  const
	{
 		return location != filename;
	}

	static File  fs_file_sibling;

	const File*	  File::GetSibling(const PathString&ext)  const
	{
		PathString sibling_location = ExtractFileName(location)+L"."+ext;
		if (!FileSystem::DoesExist(sibling_location))
			return NULL;
		fs_file_sibling = *this;
		fs_file_sibling.name = ExtractFileName(name)+"."+ext;
		fs_file_sibling.location = sibling_location;
		return &fs_file_sibling;
	}




	String File::ToString() const
	{
		return is_folder?"Folder: "+String(location):"File: "+String(location);
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
				return !!RemoveDirectoryW(location.c_str());
			return !!DeleteFileW(location.c_str());
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
		PathString tmp_folder = getTempFolder();
		#if SYSTEM==WINDOWS

			static const size_t buffer_size=512;

			PathString::char_t	szTempName[buffer_size];
	/*		UINT	uRetVal;
			BOOL fSuccess;*/


		// Create a temporary file.
			if (!GetTempFileNameW(tmp_folder.c_str(), // directory for tmp files
								  L"generic",		// temp file name prefix
								  0,			// create unique name
								  szTempName))  // buffer for name
				return false;


			filename = szTempName;
			FILE*tmp = _wfopen(filename.c_str(),L"wb");
		#elif SYSTEM==UNIX
			static unsigned subsequent_call;
			srand(time(NULL)+(subsequent_call++));
			unsigned loops(0);
			do
			{
				filename = tmp_folder+"/generic."+IntToHex(rand(),4);
			}
			while (IsFile(filename) && loops++ < 512);
			if (IsFile(filename))
				return false;
			//filename = mktemp("/tmp/generic.XXXXXX");
			//filename = mkstemp("/tmp/generic.XXXXXX");
			FILE*tmp = fopen(filename.c_str(),"wb");
		#else
			#error not supported
		#endif

		if (!tmp)
			return false;
		fclose(tmp);
		created = true;

		#if SYSTEM==WINDOWS
			SetFileAttributesW(filename.c_str(),FILE_ATTRIBUTE_TEMPORARY);
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


	const PathString&	  TempFile::GetLocation()
	{
		if (!created && !Create())
			FATAL__("Unable to create temporary file");
		return filename;
	}

	TempFile::operator const PathString& ()
	{
		if (!created && !Create())
			FATAL__("Unable to create temporary file");
		return filename;
	}



	PathString Drive::GetName()			const
	{
		return name;
	}

	PathString Drive::GetRoot()			const
	{
		return root;
	}

	PathString Drive::ToString() const
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

	Folder::Folder(const PathString&folder_string) : valid_location(true), absolute_folder(GetWorkingDirectory()), find_handle(NULL)
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
		if (file)
			locate(file->location);
	    return *this;
	}

	Folder& Folder::operator=(const PathString&folder_string)
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

	bool Folder::ResolvePath(const PathString&path_string, PathString*finalParent, PathString&final) const
	{
		if (!path_string.length())
			return false;
		PathString local = path_string;
		bool valid_location = false;
		#if SYSTEM==WINDOWS
			
			bool isAbs = path_string.beginsWith(ABS_MARKER);
			if (isAbs || (   isalpha(path_string.firstChar()) && path_string.get(1) == ':'))
			{
				if (isAbs)
				{
					local = path_string.subString(ABS_MARKER_LENGTH+3);
					final = path_string.subString(0,ABS_MARKER_LENGTH+3);
				}
				else
				{
					local = path_string.subString(3);
					final = ABS_MARKER + path_string.subString(0,3);
				}
				valid_location = true;
			}
			else
				if (path_string.firstChar() == '\\')
				{
					local = path_string.subString(1);
					final = GetWorkingDirectory().subString(0, 3);
					valid_location = true;
				}
				else
				{
					final = absolute_folder;
					valid_location = this->valid_location;
				}
		#elif SYSTEM==UNIX
			if (path_string.firstChar() == '/')
			{
				local = path_string.subString(1);
				final = "/";
				valid_location = true;
			}
			else
			{
				final = absolute_folder;
				valid_location = this->valid_location;
			}
		#else
			#error not supported
		#endif

		if (!valid_location)
		{
			final = GetWorkingDirectory();
			valid_location = true;
		}
		if (finalParent)
			*finalParent = final;
		
		while (local.IsNotEmpty())
		{
			PathString step;
			index_t	at0 = local.GetIndexOf('/'),
					at1 = local.GetIndexOf('\\'),
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
				if (!ExitAbs(final,false))
					return false;
				continue;
			}
			if (local.IsEmpty())
				if (finalParent)
					*finalParent = final;
			if (final.lastChar() != '/'
				&&
				final.lastChar() != '\\')
				final += FOLDER_SLASH;
			final += step;
		}
		//final = absolute_folder;
		return DoesExist(final);
	}

	bool				Folder::ResolvePathAndMoveTo(const PathString&path, bool moveToParent)
	{
		PathString parent,str;
		if (ResolvePath(path,&parent,str))
		{
			absolute_folder = moveToParent ? parent : str;
			valid_location = moveToParent || IsFolder(absolute_folder);
			if (!valid_location)
			{
				absolute_folder = parent;
				valid_location = true;
			}
			return true;
		}
		return false;
	}




	bool Folder::locate(const PathString&folder_string)
	{
		absolute_folder = folder_string;
		if (absolute_folder.length() > 1 && (absolute_folder.lastChar() == '/' || absolute_folder.lastChar() == '\\'))
			absolute_folder.erase(absolute_folder.length()-1);
		#if SYSTEM==WINDOWS
			if (!absolute_folder.beginsWith(ABS_MARKER))
			{
				absolute_folder = ABS_MARKER + absolute_folder;
			}
		#endif
		valid_location = IsFolder(absolute_folder);
		return valid_location;
	}

	bool Folder::MoveTo(const PathString&folder_string)
	{
		closeScan();
		//PathString final;
		valid_location = ResolvePathAndMoveTo(folder_string,false);
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
		return ExitAbs(absolute_folder,true);
		//#if SYSTEM==UNIX
		//	if (absolute_folder == '/' || absolute_folder == '\\')
		//		return false;
		//#endif

		//PathString current = absolute_folder,
		//		dir = ExtractFileNameExt(absolute_folder),
		//		super = ExtractFileDir(absolute_folder);
		//#if SYSTEM!=UNIX
		//	if (!super.length())
		//		return false;
		//#else
		//	if (!super.length())
		//	{
		//		absolute_folder = FOLDER_SLASH;
		//		return true;
		//	}
		//#endif
		//if (!locate(super))
		//{
		//	absolute_folder = current;
		//	valid_location = true;
		//	return false;
		//}
		//return true;
	}

	/*static*/ bool Folder::ExitAbs(PathString&path, bool check)
	{
		if (check && !IsDirectory(path))
			return false;
		#if SYSTEM!=WINDOWS
			if (absolute_folder == '/' || absolute_folder == '\\')
				return false;
		#endif

		PathString current = path,
				dir = ExtractFileNameExt(path),
				super = ExtractFileDir(path);
		if (super.IsEmpty())
			return false;
		if (!check || IsDirectory(super))
		{
			path = super;
			return true;
		}
		return false;
	}

	bool Folder::Enter(const PathString&folder)
	{
		if (!valid_location)
			return false;
	//	PathString current = absolute_folder;
			//	target;
		return ResolvePathAndMoveTo(folder,false);
	
	}

	bool Folder::Enter(const File&file)
	{
		if (!valid_location)
			return false;
		PathString current = absolute_folder;
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

			WIN32_FIND_DATAW result;
			HANDLE handle = FindFirstFileW(PathString(absolute_folder+L"\\*").c_str(),&result);
			if (handle)
			{
				do
				{
					if (result.cFileName[0] != '.')
						count++;
				}
				while (FindNextFileW(handle,&result));
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
			find_handle = FindFirstFileW(PathString(absolute_folder+L"\\*").c_str(),&find_data);
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
			if (!FindNextFileW(find_handle,&find_data))
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


	const Folder::File*	 Folder::NextFile(const PathString&extension)
	{
		const PathString::char_t*ext = extension.c_str();
		const File*file;
		do
		{
			file = NextEntry();
		}
		while (file && (file->is_folder || !file->GetLocation().endsWith(extension)));
		//while (file && (file->is_folder || strcmpi(file->GetExtensionPointer(),ext)));
		return file;
	}

	bool	Folder::NextFile(const PathString&extension, File&target)
	{
		const PathString::char_t*ext = extension.c_str();
		while (NextEntry(target))
			if (!target.is_folder && target.GetLocation().endsWith(extension))
				return true;
		return false;
	}



	const Folder::File*	 Folder::Find(const PathString&folder_str, bool mustExist/*=true*/)	const
	{
		if (Find(folder_str,file,mustExist))
			return &file;
		return NULL;
	}

	bool	 Folder::Find(const PathString&folder_str, File&out, bool mustExist/*=true*/)	const
	{
		if (!valid_location)
			return false;
		if (!ResolvePath(folder_str,nullptr,out.location) && mustExist)
			return false;
		out.name = ExtractFileNameExt(out.location);
		out.is_folder = false;
		#if SYSTEM==WINDOWS
			DWORD attributes = GetFileAttributesW(out.location.c_str());
			if (attributes == INVALID_FILE_ATTRIBUTES)
				return !mustExist;
			out.is_folder = (attributes&FILE_ATTRIBUTE_DIRECTORY)!=0;
		#elif SYSTEM==UNIX
			struct stat s;
			if (stat(out.location.c_str(),&s))
				return !mustExist;
			out.is_folder = s.st_mode&S_IFDIR;
		#else
			#error not supported
		#endif
		return true;
	}

	const Folder::File*	 Folder::FindFile(const PathString&folder_str, bool mustExist/*=true*/)	const
	{
		const File*rs = Find(folder_str,mustExist);
		if (!rs || rs->is_folder)
			return NULL;
		return rs;
	}

	bool	 Folder::FindFile(const PathString&folder_str, File&out, bool mustExist/*=true*/)	const
	{
		return Find(folder_str,out,mustExist) && !out.is_folder;
	}

	bool	 Folder::FindFolder(const PathString&folder_str, File&out, bool mustExist/*=true*/)	const
	{
		//return Find(folder_str,out,mustExist) && out.is_folder;
		if (Find(folder_str,out,mustExist))
		{
			if (mustExist)
				return out.is_folder;
			return !IsFile(out.location);
		}
		return false;
	}

	const Folder::File*	 Folder::FindFolder(const PathString&folder_str, bool mustExist/*=true*/)	const
	{
		const File*rs = Find(folder_str,mustExist);
		if (!rs)
			return NULL;

		if (mustExist)
			return rs->is_folder ? rs : NULL;
		return IsFile(rs->location) ? NULL : rs;
	}

	const Folder::File*	 Folder::CreateFolder(const PathString&name)	const
	{
		if (!valid_location)
			return NULL;
		PathString location = absolute_folder;
		if (location.lastChar() != '/' && location.lastChar() != '\\')
			location += FOLDER_SLASH;
		location += name;
		//locate(
		PathString final;
		ResolvePath(location,nullptr,final);
		return FileSystem::CreateFolder(final);
	}

	bool	 Folder::CreateFolder(const PathString&name, File&out)	const
	{
		if (!valid_location)
			return false;
		PathString location = absolute_folder;
		if (location.lastChar() != '/' && location.lastChar() != '\\')
			location += FOLDER_SLASH;
		location += name;
		ResolvePath(location,nullptr,location);
		return FileSystem::CreateFolder(location, out);
	}





	String		 Folder::ToString() const
	{
		if (!valid_location)
			return "Folder: "+String(absolute_folder)+"(invalid)";
		return "Folder: "+String(absolute_folder);
	}

	const PathString&  Folder::LocationStr() const
	{
		return absolute_folder;
	}

	const PathString&  Folder::GetLocation() const
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
				return nullptr;
		#endif

		PathString	 super = ExtractFileDir(absolute_folder);
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



	PathString WorkingDirectory()
	{
		return GetWorkingDirectory();
	}

	PathString GetWorkingDirectory()
	{
		#if SYSTEM==WINDOWS
			//static const size_t len = 0x2000;
			wchar_t buffer[0x2000];
			if (!GetCurrentDirectoryW(ARRAYSIZE(buffer)-1,buffer))
				return L"";
			PathString rs = buffer;
			if (!rs.beginsWith(ABS_MARKER))
				rs = ABS_MARKER + rs;
//			size_t len = ABS_MARKER_LENGTH;
			return rs;
		#elif SYSTEM==UNIX
			char buffer[0x1000];
			if (!getcwd(buffer,ARRAYSIZE(buffer)))
				return "";
			return buffer;
		#else
			#error not supported
		#endif
	}


	bool  SetWorkingDirectory(const PathString&path)
	{
		#if SYSTEM==WINDOWS
			return !!SetCurrentDirectoryW(path.c_str());
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
				if (!valid || !GetVolumeInformationA(drive, volume_name, ARRAYSIZE(volume_name), NULL, NULL, NULL, NULL, 0))
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

	PathString	ExtractFileName(const PathString&filename)
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

	PathString	ExtractFileExt(const PathString&filename)
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

	PathString	ExtractFileDir(const PathString&filename)	{return _extractFileDir(filename);}

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

	PathString	 ExtractFileDirName(const PathString&filename)	{return _extractFileDirName(filename);}

	template <typename T>
		static StringTemplate<T>	 _extractFileNameExt(const StringTemplate<T>&filename)
		{
			index_t at = filename.length()-1;
			if (at>= filename.length())
				return StringTemplate<T>();
			while (filename.get(at) != (T)'/' && filename.get(at) != (T)'\\' && --at < filename.length());
			return filename.subString(at+1);
		}

	PathString	 ExtractFileNameExt(const PathString&filename)	{return _extractFileNameExt(filename);}


	template <typename T>
		static StringTemplate<T>	_escapeSpaces(StringTemplate<T> path)
		{
			while (index_t at = path.GetIndexOf((T)'\\'))
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
	PathString	 EscapeSpaces(const PathString &path)	{return _escapeSpaces(path);}


	bool IsDirectory(const PathString&name)
	{
		return IsFolder(name);
	}


	bool IsFolder(const PathString&name)
	{
		#if SYSTEM==WINDOWS
			if (name.contains('/'))//this is apparently bad when using extended path strings
			{
				StringW copy(name);
				copy.replace((PathString::char_t)'/',(PathString::char_t)'\\');
				return IsFolder(copy);
			}
			DWORD attribs = GetFileAttributesW(name.c_str());
			return attribs != INVALID_FILE_ATTRIBUTES && (attribs&FILE_ATTRIBUTE_DIRECTORY);
		#elif SYSTEM==UNIX
			struct ::stat s;
			return !stat(name.c_str(),&s) && (s.st_mode&S_IFDIR);
		#else
			#error not supported
		#endif
	}

	bool IsFile(const PathString&name)
	{
		return IsFile(name.c_str());
	}

	bool IsFile(const PathString::char_t*name)
	{
		#if SYSTEM==WINDOWS
			const PathString::char_t*chk = name;
			while (*chk)
			{
				if (*chk == (PathString::char_t)'/')
				{
					//this is apparently bad when using extended path strings
					StringW copy(name);
					copy.replace((PathString::char_t)'/',(PathString::char_t)'\\');
					return IsFile(copy.c_str());
				}
				chk++;
			}
			DWORD attribs = GetFileAttributesW(name);
			return attribs != INVALID_FILE_ATTRIBUTES && !(attribs&FILE_ATTRIBUTE_DIRECTORY);
		#elif SYSTEM==UNIX
			struct ::stat s;
			return !stat(name,&s) && !(s.st_mode&S_IFDIR);
		#else
			#error not supported
		#endif
	}


	bool DoesExist(const PathString&name)
	{
		#if SYSTEM==WINDOWS
			if (name.contains('/'))//this is apparently bad when using extended path strings
			{
				StringW copy(name);
				copy.replace((PathString::char_t)'/',(PathString::char_t)'\\');
				return DoesExist(copy);
			}
			return GetFileAttributesW(name.c_str()) != INVALID_FILE_ATTRIBUTES;
		#elif SYSTEM==UNIX
			struct ::stat s;
			return !stat(name.c_str(),&s);
		#else
			#error not supported
		#endif
	}


	fsize_t		GetFileSize(const PathString&name)
	{
		#if SYSTEM==WINDOWS
			if (name.contains('/'))//this is apparently bad when using extended path strings
			{
				StringW copy(name);
				copy.replace((PathString::char_t)'/',(PathString::char_t)'\\');
				return GetFileSize(copy);
			}
			WIN32_FILE_ATTRIBUTE_DATA	data;
			if (!GetFileAttributesExW(name.c_str(),GetFileExInfoStandard,&data))
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


	ftime_t GetModificationTime(const PathString&name)
	{
		#if SYSTEM==WINDOWS
			if (name.contains('/'))//this is apparently bad when using extended path strings
			{
				StringW copy(name);
				copy.replace((PathString::char_t)'/',(PathString::char_t)'\\');
				return GetModificationTime(copy);
			}

			WIN32_FILE_ATTRIBUTE_DATA	data;
			if (!GetFileAttributesExW(name.c_str(),GetFileExInfoStandard,&data))
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


	const File*  MoveFile(const PathString&source, const PathString&destination)
	{
		return Move(source, destination);
	}


	const File*  Move(const PathString&source, const PathString&destination)
	{
		#if SYSTEM==WINDOWS
			if (source.contains('/') || destination.contains('/'))//this is apparently bad when using extended path strings
			{
				StringW copy0(source),copy1(destination);
				copy0.replace((PathString::char_t)'/',(PathString::char_t)'\\');
				copy1.replace((PathString::char_t)'/',(PathString::char_t)'\\');
				return Move(copy0,copy1);
			}

			if (!MoveFileW(source.c_str(),destination.c_str()))
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

	const File*  Copy(const PathString&source, const PathString&destination, bool overwrite_if_exist)
	{
		if (!Copy(source,destination,fs_result,overwrite_if_exist))
			return NULL;
		return &fs_result;
	}


	bool		Copy(const PathString&source, const PathString&destination, File&outFile, bool overwrite_if_exist)
	{
		if ((IsFile(destination)) && !overwrite_if_exist)
			return false;
		#if SYSTEM==WINDOWS
			PathString my_destination = destination;
			if (IsFolder(destination) && !IsFolder(source))
				my_destination += FOLDER_SLASH+ExtractFileNameExt(source);
			if (IsFolder(source))
			{
				if (!DoesExist(destination))
				{
					File dummy;
					if (!CreateFolder(destination,dummy))
					{
						return false;
					}
				}
				if (IsFile(destination))
					return false;
				Folder scan(source);
				Folder write(destination);
				File found,dest,copied;
				scan.Rewind();
				while (scan.NextEntry(found))
				{
					if (write.Find(found.GetName(),dest,false))
						Copy(found.GetLocation(),dest.GetLocation(),copied,overwrite_if_exist);
				}
			}
			else
			{
				if (!CopyFileW(source.c_str(),my_destination.c_str(),!overwrite_if_exist) && _wsystem((L"copy \""+addSlashes(source)+L"\" \""+addSlashes(destination)+L"\"").c_str()))
					return false;
			}
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
		
			
			PathString call = ("cp "+EscapeSpaces(source)+" "+EscapeSpaces(destination));
			if (system(call))
				return false;
			//sleep(500);
		#else
			#error not supported
		#endif
		outFile.name = ExtractFileNameExt(destination);
		outFile.location = destination;
		outFile.is_folder = IsFolder(destination);
		return true;
	}

	const File*  Copy(const File*source, const PathString&destination, bool overwrite)
	{
		if (!source)
			return NULL;
		return Copy(source->GetLocation(),destination,overwrite);
	}

	const File*  Copy(const File&source, const PathString&destination, bool overwrite)
	{
		return Copy(source.GetLocation(),destination,overwrite);
	}


	const File*  CopyFile(const PathString&source, const PathString&destination, bool overwrite_if_exist)
	{
		return Copy(source,destination,overwrite_if_exist);
	}

	const File*  CopyFile(const File*source, const PathString&destination, bool overwrite)
	{
		return Copy(source,destination,overwrite);
	}

	const File*  CopyFile(const File&source, const PathString&destination, bool overwrite)
	{
		return Copy(source,destination,overwrite);
	}


	const File*  Move(const File*source, const PathString&destination)
	{
		return Move(source->GetLocation(),destination);
	}

	const File*  Move(const File&source, const PathString&destination)
	{
		return Move(source.GetLocation(),destination);
	}


	const File*  MoveFile(const File*source, const PathString&destination)
	{
		return Move(source->GetLocation(),destination);
	}

	const File*  MoveFile(const File&source, const PathString&destination)
	{
		return Move(source.GetLocation(),destination);
	}

	const File*  CreateDirectory(const PathString&folder_name)
	{
		return CreateFolder(folder_name);
	}

	const File*  CreateFolder(const PathString&folder_name)
	{
		if (CreateFolder(folder_name,fs_result))
			return &fs_result;
		return NULL;
	}


	bool  CreateDirectory(const PathString&folder_name, File&out)
	{
		return CreateFolder(folder_name,out);
	}

	bool  CreateFolder(const PathString&folder_name, File&out)
	{
		if (!folder_name.length())
			return false;
		out.location = folder_name;
		if (out.location.lastChar() == '/' || out.location.lastChar() == '\\')
			out.location.erase(out.location.length()-1);
		out.is_folder = true;
		#if SYSTEM==WINDOWS
			bool absoluteWide = folder_name.beginsWith(ABS_MARKER);
			out.location.replace(L'/',L'\\');
			Array<PathString>	segments;
			explode(L'\\',out.location,segments);
			for (index_t i = 1; i <= segments.count(); i++)
			{
				PathString path = implode(L'\\',segments.pointer(),i);
				if (path.IsEmpty() || path == L"\\\\?" || path.endsWith(L':') || path==L"\\")
					continue;
				//if (absoluteWide)
					//path = absMarker + path;
				if (IsDirectory(path))
					continue;
				if (!CreateDirectoryW(path.c_str(),NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
					return false;
			}
		#elif SYSTEM==UNIX
			if (mkdir(out.location.c_str(), S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) && errno != EEXIST)
				return false;
		#else
			#error not supported
		#endif
		//PathString copy = out.location;
		//;
		return Folder().Find(out.location,out);
	}

#if SYSTEM==WINDOWS
	void PrintLastError()
	{
		DWORD code = GetLastError();
		LPVOID lpMsgBuf;
		FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPWSTR)&lpMsgBuf,
			0,
			NULL
			);
		//unsigned len = strlen((char*)lpMsgBuf);
		ErrMessage((const wchar_t*)lpMsgBuf);

		LocalFree(lpMsgBuf);
	}

#endif

	bool			UnlinkFile(const PathString&location)
	{
		#if SYSTEM==UNIX
			return !::unlink(location.c_str());
		#elif SYSTEM==WINDOWS
			if (!IsFile(location))
				return true;
			int retry = 0;
			PathString loc = location;
			loc.replace(L'/',L'\\');
			while (!DeleteFileW(loc.c_str()))
			{
				if (retry > 10)
				{
					PrintLastError();
					return false;
				}
				if (retry == 0)
					SetFileAttributesW(loc.c_str(), FILE_ATTRIBUTE_NORMAL);
				else
					Sleep(10);
				retry++;
			}
			return true;
		#else
			#error not supported
		#endif


	}

	bool			RemoveFolder(const PathString&location)
	{
		#if SYSTEM==UNIX
			return !rmdir(location.c_str());
		#elif SYSTEM==WINDOWS
			return !!RemoveDirectoryW(location.c_str());
		#else
			#error not supported
		#endif


	}


	static const File* ResolveLink(const File*file)
	{
		#if SYSTEM==UNIX
			if (!file)
				return NULL;

			char buffer[0x2000];
			int len = readlink(file->GetLocation().c_str(),buffer,ARRAYSIZE(buffer));
			if (len == -1 || len ==ARRAYSIZE(buffer))
				return file;
			buffer[len] = 0;	//appearingly readlink does not terminate the string by itself
	//		ShowMessage("symlink pointing to "+PathString(buffer));
			Folder folder(file->GetFolder());
			return folder.FindFile(buffer);
		#else

			return file;
		#endif
	}

	static bool ResolveLink(File&file)
	{
		#if SYSTEM==UNIX
			char buffer[0x2000];
			int len = readlink(file.GetLocation().c_str(),buffer,ARRAYSIZE(buffer));
			if (len == -1 || len >= ARRAYSIZE(buffer))
				return true;
			buffer[len] = 0;	//appearingly readlink does not terminate the string by itself
	//		ShowMessage("symlink pointing to "+PathString(buffer));
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
		PathString wd = GetWorkingDirectory();
		Folder folder(wd);

		PathString filename=name;

		#if SYSTEM==UNIX
			if (filename.GetIndexOf('/'))
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
					filename = PathString(segment_begin)+FOLDER_SLASH_STR+name;
					if (folder.FindFile(filename,target))
						return ResolveLink(target);
					if (folder.FindFile(filename+".exe",target))
						return ResolveLink(target);
					segment_begin = field+i+1;
				}
			{
				filename = PathString(segment_begin)+FOLDER_SLASH_STR+name;
				if (folder.FindFile(filename,target))
					return ResolveLink(target);
				if (folder.FindFile(filename+".exe",target))
					return ResolveLink(target);
			}
		}
		return false;
	}

	PathString			GetAbsolutePath(const PathString&relative_path)
	{
		Folder	folder(GetWorkingDirectory());
		File file;
		if (folder.Find(relative_path,file))
			return file.GetLocation();
		return relative_path;
	}

	PathString			GetRelativePath(const PathString&origin, const PathString&destination)
	{
		const PathString::char_t	*o = origin.c_str(),
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
		PathString rs;
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
