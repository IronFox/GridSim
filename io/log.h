#ifndef logH
#define logH


#include <stdio.h>
#ifndef __BORLANDC__
	#ifdef __GNUC__
		#include <sys/time.h>
	#endif
    #include <time.h>
#else
    #include <time.h>
#endif

#include "str_stream.h"

//namespace IO
//{

	struct TLogIndent
	{
			BYTE        length;
	};

	TLogIndent  indent(BYTE characters);

	/**
		\brief Generalized output log file handler
	*/
	class LogFile
	{
	protected:
		bool			do_open;
		FILE			*f;                     //!< Out file handle
		String			last;                   //!< Last filename
		static  char    space_buffer[0x100];    //!< Space buffer for indentation
		unsigned		indentation;			//!< Additional tabs following a newline
		bool			indent_,				//!< True if the next output requires indention
						make_clear;				//!< Valid only if do_open is true. Specifies whether or not a file opened in delayed mode should be cleared before writing to it

		bool			begin();
		void			timeStamp();            //!< Writes a time stamp to the file
	public:
		String			format;                 //!< Time stamp format
        
		/**/			LogFile();
		/**/			LogFile(const String&filename, bool makeclear);   //!< Creates a new log object. The specified file is opened in delayed mode, meaning it's actually opened the first time data is written to it \param filename Name of the file to write to \param makeclear Specifies whether or not the specified file should be cleared before writing
		virtual			~LogFile();

		void			clear();                                        //!< Clears the content of the local log file
		inline void		Clear()	{clear();}
		bool			open(const String&filename, bool makeclear, bool delayed=false);   //!< Closes the active file and opens the specified file for write output \param filename Name of the file to write to  \param makeclear Specifies whether or not the specified file should be cleared before writing @param delayed Specifies that the file should not be opened immediately but rather when data is written to it the first time \return true on success
		inline bool		Open(const String&filename, bool makeClear, bool delayed=false)	{return open(filename,makeClear,delayed);}
		bool			create(const String&filename, bool delayed=false);					//!< Closes the active file and opens the specified file for write output \param filename Name of the file to write to  @param delayed Specifies that the file should not be opened immediately but rather when data is written to it the first time \return true on success
		inline bool		Create(const String&filename, bool delayed=false)	{return create(filename,delayed);}
		void			close();                                        //!< Closes the active file
		inline void		Close()	{close();}
		bool			isActive();                                     //!< Queries log state \return true if the file is currently open
		inline bool		IsActive()	{return isActive();}
		bool			log(const char*line, bool add_time_stamp=false);         //!< Writes a line to the end of the log file and flushes the file \param line Pointer to the string that should be written \param add_time_stamp Specifies whether or not a timestamp should preceed the output \return true on success
		inline bool		Log(const char*line, bool addTimeStamp=false)	{return log(line,addTimeStamp);}
		bool			log(const String&line, bool add_time_stamp=false);      //!< Writes a line to the end of the log file and flushes the file \param line String that should be written \param add_time_stamp Specifies whether or not a timestamp should preceed the output \return true on success
		inline bool		Log(const String&line, bool addTimeStamp=false)	{return log(line,addTimeStamp);}
		LogFile&		operator<<(const char*line);
		LogFile&		operator<<(const String&line);
		LogFile&		operator<<(const TNewLine&);
		LogFile&		operator<<(const TEndLine&);
		LogFile&		operator<<(const TLogIndent&indent);
		void			indent();
		inline void		Indent()	{indent();}
		void			outdent();
		inline void		Outdent()	{outdent();}
	};








//}

#endif
