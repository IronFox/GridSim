#ifndef sharedLibrarH
#define sharedLibrarH

#include "file_system.h"

namespace DeltaWorks
{
	class SharedLibrary	   //! Shared library handler (for DLLs/SOs)
	{
	private:
		#if SYSTEM==WINDOWS
			HMODULE	 	module_handle;
		#elif SYSTEM==UNIX
			void	   *module_handle;
		#endif
		String		lastError;

					SharedLibrary(const SharedLibrary&other) = delete;
		void		operator=(const SharedLibrary&other)=delete;
	public:
		/**/		SharedLibrary();
		void		swap(SharedLibrary&other)
		{
			using std::swap;
			swap(module_handle,other.module_handle);
			// lastError.swap(other.lastError);
		}
		/**
		Loads the specified path string.
		Similar to Open() but may throw exceptions
		*/
		/**/		SharedLibrary(const PathString&filename);
		virtual		~SharedLibrary();
		/**
		Closes if open, and loads the specified shared library.
		Note that especially on unix-like systems this method may fail to find shared objects if the provided path is not full/absolute (starting with a slash on unix systems)
		@param filename Full path to the shared library file (including extension) 
		@return true on success, false otherwise		
		*/
		bool		Open(const PathString&filename);
		void		Close();							//!< Closes a loaded shared library
		bool		IsLoaded();						   //!< Queries load state \return true, if a shared library is currently loaded
		void*	 	FindFunction(const char*funcname) const;		//!< Locates a symbol in the currently loaded library \param funcname Name of the function that should be located \return Pointer to the function
		/*!
			\brief Locates a symbol and automatically casts it to the result type.
			\param funcname Name of the function that should be located
			\param target Out pointer target
			\return true on success
		*/
		template <class FuncType>
		bool		FindFunction(FuncType&target,const char*funcname) const
					{
						target = (FuncType)FindFunction(funcname);
						return target!=NULL;
					}
		template <class FuncType>
		bool		FindFunction(const char*funcname, FuncType&target) const
					{
						target = (FuncType)FindFunction(funcname);
						return target!=NULL;
					}
		String		GetOpenError() const {return lastError;}
		void		adoptData(SharedLibrary&other);

		friend void	swap(SharedLibrary&a, SharedLibrary&b)
		{
			a.swap(b);
		}
	};
}

#endif