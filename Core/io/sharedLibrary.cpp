#include <global_root.h>

#include "sharedLibrary.h"

namespace DeltaWorks
{

	SharedLibrary::SharedLibrary():module_handle(NULL)
	{}


	SharedLibrary::SharedLibrary(const PathString&filename):module_handle(NULL)
	{
		if (!Open(filename))
			throw Except::IO::DriveAccess::FileOpenFault(CLOCATION,GetOpenError());
	}

	SharedLibrary::~SharedLibrary()
	{
		if (!application_shutting_down)
			Close();	//this can, apparently, freeze
	}
	
	void SharedLibrary::adoptData(SharedLibrary&other)
	{
		Close();
		module_handle = other.module_handle;
		other.module_handle = NULL;
	}

	bool SharedLibrary::Open(const PathString&filename)
	{
		Close();
		if (filename.IsEmpty())
		{
			lastError = "Filename was empty";
			return false;
		}
		lastError = "";
		#if SYSTEM==WINDOWS
			module_handle = LoadLibrary(filename.c_str());
		#elif SYSTEM==UNIX
			module_handle = dlopen(filename,RTLD_LAZY);
		#endif
		if (module_handle == nullptr)
		{
			#if SYSTEM==LINUX
				lastError = dlerror();
			#else
				lastError = Sys::GetLastErrorString();
			#endif
		}
		return module_handle != nullptr;
	}


	bool SharedLibrary::IsLoaded()
	{
		return module_handle!=NULL;
	}

	void SharedLibrary::Close()
	{
		if (!module_handle)
			return;
		#if SYSTEM==WINDOWS
			FreeLibrary(module_handle);
		#elif SYSTEM==UNIX
			dlclose(module_handle);
		#endif
		module_handle = NULL;
	}

	void* SharedLibrary::FindFunction(const char*funcname) const
	{
		if (!module_handle)
			return NULL;
		#if SYSTEM==WINDOWS
			return (void*)GetProcAddress(module_handle,funcname);
		#elif SYSTEM==UNIX
			return dlsym(module_handle,funcname);
		#else
			#error unsupported
		#endif
	}
}

