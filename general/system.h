#ifndef systemH
#define systemH

/*****************************************************************************

Classes for platform-independent handling of platform-specific system aspects.
Supported aspects are Pipes, and Shared-Libraries.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

*****************************************************************************/

#include "../global_root.h"
#include <stdlib.h>

#pragma comment(lib,"psapi.lib")


#if SYSTEM == WINDOWS
	#include <winbase.h>
	#include <Psapi.h>
#elif SYSTEM == UNIX
	#include <unistd.h>
	#include <pthread.h>
	#include <sys/sem.h>
	#include <sys/ipc.h>
	#include <sys/types.h>
	#include <dlfcn.h>
	#include <string.h>
	#include <fcntl.h>
	#include <stdarg.h>
	#include <signal.h>
	#include <sched.h>
	#include <sys/ioctl.h>
	//#include <signal.h>
	#include <termios.h>
	#include <sys/time.h>
	#include <sys/resource.h>

	#ifndef SEM_R
		#define SEM_A		0200	/* alter permission */
		#define SEM_R	   0400	/* read permission */	
	#endif
#endif


namespace System //! Translation namespace for common system tasks
{


	class SharedLibrary	   //! Shared library handler (for DLLs/SOs)
	{
	private:
		#if SYSTEM==WINDOWS
			HMODULE	 	module_handle;
		#elif SYSTEM==UNIX
			void	   *module_handle;
		#endif

						SharedLibrary(const SharedLibrary&other)	{};
			void		operator=(const SharedLibrary&other)	{};
	public:
						SharedLibrary();
						SharedLibrary(const char*filename);	//!< Load constructor \param filename Full path to the shared library file (including extension)
	virtual				~SharedLibrary();
			bool		load(const char*filename);			//!< Closes if open and loads the specified shared library. Note that especially on unix systems this method may fail to find shared objects if the provided path is not full/absolute (starting with with a slash on unix systems) \param filename Full path to the shared library file (including extension) \return true on success, false otherwise
			bool		loadFromFile(const char*filename);	//!< Identical to load()
			bool		open(const char*filename);			//!< Identical to load()
			void		close();							//!< Closes a loaded shared library
			bool		loaded();						   //!< Queries load state \return true, if a shared library is currently loaded
			bool		isActive();						 //!< Identical to loaded() - Queries load state \return true, if a shared library is currently loaded
			void*	 	locate(const char*funcname);		//!< Locates a symbol in the currently loaded library \param funcname Name of the function that should be located \return Pointer to the function
			/*!
				\brief Locates a symbol and automatically casts it to the result type.
				\param funcname Name of the function that should be located
				\param target Out pointer target
				\return true on success
			*/
			template <class FuncType>
			bool		locate(FuncType&target,const char*funcname)
						{
							target = (FuncType)locate(funcname);
							return target!=NULL;
						}
			template <class FuncType>
			bool		locate(const char*funcname, FuncType&target)
						{
							target = (FuncType)locate(funcname);
							return target!=NULL;
						}
			const char*	errorStr();
			const char*	getError()	{return errorStr();}

			void		adoptData(SharedLibrary&other);
	};


	


	class Pipe //! System independent (non blocking) pipe handler
	{
	private:
		#if SYSTEM==WINDOWS
			HANDLE	  		read_handle,write_handle;
		#elif SYSTEM==UNIX
			int		 		handle[2];
		#endif

	public:
							Pipe(DWORD size=0);						//!< Constructs a new pipe. \param size Pipe size in bytes. Only applicable when using windows. Set 0 for default.
	virtual					~Pipe();
			bool			write(const void*data, unsigned bytes);	 //!< Writes unformated data to the pipe \param data Pointer to the data to read from \param bytes Number of bytes to write to the pipe
			bool			read(void*target, unsigned bytes);		  //!< Reads unformated data from the stream \param target Pointer to the data to write to \param bytes Number of bytes to read from the pipe
		template <class C>
			bool			write(const C&data)						 //!< Writes one unit of formated data \param data Reference to the object to read from
							{
								return write(&data,sizeof(C));
							}
		template <class C>
			Pipe&	  		operator<<(const C&data)					//!< Streaming operator: Writes one unit of formated data \param data Reference to the object to read from
							{
								write(&data,sizeof(C));
								return *this;
							}
		template <class C>
			bool			read(C&data)								//!< Reads one unit of formated data \param data Reference to the object to write to \return true on success
							{
								return read(&data,sizeof(C));
							}
		template <class C>
			bool			operator>>(C&data)						  //!< Streaming operator: Reads one unit of formated data \param data Reference to the object to write to \return true on success
							{
								return read(&data,sizeof(C));
							}
	};

	class BlockingPipe //! System independent (blocking) pipe handler
	{
	protected:
		#if SYSTEM==WINDOWS
			HANDLE			read_handle,write_handle;
		#elif SYSTEM==UNIX
			int				handle[2];
		#endif

							BlockingPipe(bool);
	public:
							BlockingPipe(DWORD size=0);			   //!< Constructs a new (blocking) pipe. \param size Pipe size in bytes. Only applicable when using windows. Set 0 for default.
	virtual					~BlockingPipe();
		#if SYSTEM==WINDOWS
			HANDLE			writeHandle()	const	{return write_handle;}
		#elif SYSTEM==UNIX
			int				writeHandle()	const	{return handle[1];}
		#endif

			bool			write(const void*data, unsigned bytes);	  //!< Writes unformated data to the pipe \param data Pointer to the data to read from \param bytes Number of bytes to write to the pipe
			/*!
				\brief Reads unformated data from the stream
				\param target Pointer to the data to write to
				\param bytes Number of bytes to read from the pipe
				
				The function blocks until the specified number of bytes were read from the pipe
				or an error occured.
			*/
			bool			read(void*target, unsigned bytes);
		template <class C>
			bool			write(const C&data)						 //!< Writes one unit of formated data \param data Reference to the object to read from
							{
								return write(&data,sizeof(C));
							}
		template <class C>
			bool			writeArray(const Array<C>&data)						 //!< Writes one unit of formated data \param data Reference to the object to read from
							{
								return write(data.pointer(),data.contentSize());
							}
							
		template <class C>
			BlockingPipe&	operator<<(const C&data)				  //!< Streaming operator: Writes one unit of formated data \param data Reference to the object to read from
							{
								write(&data,sizeof(C));
								return *this;
							}
			/*!
				\brief Reads one unit of formated data
				\param data Reference to the data to write to
				\return true on success

				The function blocks until the entire object was read from the pipe
				or an error occured.
			*/
		template <class C>
			bool			read(C&data)
							{
								return read(&data,sizeof(C));
							}
		template <class C>
			C				read()
							{
								C rval;
								read(&rval,sizeof(C));
								return rval;
							}
		template <class C>
			bool			readArray(Array<C>&data)
							{
								return read(data.pointer(),data.contentSize());
							}
			/*!
				\brief Streaming operator: Reads one unit of formated data
				\param data Reference to the data to write to
				\return true on success

				The function blocks until the entire object was read from the pipe
				or an error occured.
			*/
		template <class C>
			bool			operator>>(C&data)
							{
								return read(&data,sizeof(C));
							}
	};


	class PipeFeed
	{
	public:
		#if SYSTEM==WINDOWS
			typedef HANDLE	handle_t;
		#elif SYSTEM==UNIX
			typedef int		handle_t;
		#endif
	private:
		handle_t			write_handle;
	public:
		PipeFeed()			:write_handle(INVALID_HANDLE_VALUE)	{};

		void				set(handle_t handle)	{write_handle = handle;}
		bool				isSet()	const	{return write_handle != INVALID_HANDLE_VALUE;}
		bool				write(const void*data, unsigned bytes);	  //!< Writes unformated data to the pipe \param data Pointer to the data to read from \param bytes Number of bytes to write to the pipe
		template <class C>
		bool				write(const C&data)						 //!< Writes one unit of formated data \param data Reference to the object to read from
							{
								return write(&data,sizeof(C));
							}
		template <class C>
		bool				writeArray(const Array<C>&data)						 //!< Writes one unit of formated data \param data Reference to the object to read from
							{
								return write(data.pointer(),data.contentSize());
							}
							
		template <class C>
		PipeFeed&			operator<<(const C&data)				  //!< Streaming operator: Writes one unit of formated data \param data Reference to the object to read from
							{
								write(&data,sizeof(C));
								return *this;
							}

	};
	
	class NamedPipeClient:public BlockingPipe //! System independent (blocking) named pipe handler
	{
	public:
							NamedPipeClient();			   			//!< Constructs a new (blocking) named pipe client. The pipe is not automatically connected.
	
			bool			connectTo(const char*pipe_name, unsigned timeout=500);
	
			bool			isActive()			const;					//!< Queries the current status of the named pipe
			bool			isConnected()		const;					//!< Queries the current status of the named pipe
	};
	class NamedPipeServer:public BlockingPipe //! System independent (blocking) named pipe handler
	{
	public:
							NamedPipeServer();			   			//!< Constructs a new (blocking) named pipe client. The pipe is not automatically connected.
	
			bool			start(const char*pipe_name);
			void			acceptClient();								//!< Checks and connects any pending connection request. Blocks until a client has connected	
			bool			isActive()			const;					//!< Queries the current status of the named pipe
			bool			isConnected()		const;					//!< Queries the current status of the named pipe
	};

	const char*				getFirstEnv(const char**field, unsigned len, const char*except);	//!< Returns the first non-NULL environment variable or \b except if no such could be found \param field Field containing key-names to check for \param len Number of entries in the name field \param except String to return if no matching key was found \return First non-NULL result of getenv(...) or \b except if none was found
	
	unsigned				getProcessorCount();	//!< Queries the number of processors available on the local system \return number of processors
	const char*				getSystemName();		//!< Queries the name of the currently used operating system \return System name or 'unknown' if the name was not determinable
	const char*				getUserName();			//!< Queries the name of the currently logged in user \return User name or 'unknown' if the name was not determinable
	const char*				getUserDomain();		//!< Queries the domain, the currently logged in user is member of \return User domain name or '' if the domain was not determinable
	const char*				getUserHomeFolder();	//!< Queries the home folder of the currently logged in user \return User home folder or '' if the home folder was not determinable
	const char*				getTempFolder();		//!< Queries the system temp folder \return System temp folder
	const char*				getHostName();			//!< Queries the resolved address of the local computer or at least its name \return computer addr/name

	size_t					getPhysicalMemory();	//!< Queries the total available physical memory (in bytes)
	size_t					getMemoryConsumption();	//!< Memory currently consumed by this application (in bytes)
	
	bool					copyToClipboard(HWND window, const char*line);
	bool					getFromClipboardIfText(HWND window, char*buffer, size_t buffer_size);
	
	int						getConsoleWidth();		//!< Retrieves the visual width of the console in characters
	
	#if SYSTEM==WINDOWS
		#define sleep(milsec)   Sleep(milsec)
		const char*			getLastError();

	#elif SYSTEM==UNIX
		#define sleep(milsec)   usleep(milsec*1000)
	#endif
	
	

//	void					setEnv(const String&name, const String&value);



}

using namespace System;
 
#endif
