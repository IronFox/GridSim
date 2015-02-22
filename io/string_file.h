#ifndef string_fileH
#define string_fileH

/******************************************************************

Simplified string-file-handler.

******************************************************************/


#include "str_stream.h"
#include <stdio.h>
#include "../math/basic.h"



#define CM_IDENTITY						0x0
#define CM_STRIP_LINE_COMMENTS			0x1
#define CM_STRIP_LINE_START_COMMENTS	0x2
#define CM_STRIP_BLOCK_COMMENTS			0x4
#define CM_STRING_SENSITIVE				0x8
#define CM_RETURN_EMPTY_LINES			0x10
#define CM_RECORD_COMMENTS				0x20

#define CM_STRIP_COMMENTS       (CM_STRIP_LINE_COMMENTS|CM_STRIP_BLOCK_COMMENTS)
#define CM_CODE                 (CM_STRIP_COMMENTS|CM_STRING_SENSITIVE)


/*!
	\brief Simplified string access and creation interface

	The StringFile class provides a simple interface to reading and writing string files.
*/
class StringFile
{
private:
//static	LogFile	log_file;
        bool    active,write_mode,read_mode,in_comment,in_string,had13,fclose_on_destruct;
        FILE    *f;
        String fragment;
        char    buffer[0x1001],*string,*end;
const   char    *e;

        void    stripComments(String&str);

public:
        String	lineComment,	//!< Line comment string (e.g. "//")
				lineStartComment,	//!< Line comment that is valid only as the first non-whitespace character (e.g. "#")
                comment_begin,	//!< Block comment begin string (e.g. "/*")
                comment_end,	//!< Block comment end string (e.g. "*/")
				comment;		//!< Most recently extracted comment (only available if CM_RECORD_COMMENTS is ORed to \b conversion_flags )
        UINT32	conversion_flags;	//!< String conversion configuration
        UINT    root_line;		//!< Beginning of the currently returned line (from the beginning of the file)

                StringFile(const String&filename, unsigned flags=CM_IDENTITY, bool create=false);	//!< Load constructor. File is opened in readonly mode. \param filename File to open \param flags Filter configuration. Common are CM_STRIP_COMMENTS to strip all comments or CM_IDENTITY to disable filtering.
				StringFile(FILE*f, unsigned flags=CM_IDENTITY);					//!< Assign constructor. File is used for reading. \param f Stream to read from. May be NULL. \param flags Filter configuration. Common are CM_STRIP_COMMENTS to strip all comments or CM_IDENTITY to disable filtering.
                StringFile(unsigned flags=CM_IDENTITY);							//!< Empty constructor. No file is opened.\param flags Filter configuration. Common are CM_STRIP_COMMENTS to strip all comments or CM_IDENTITY to disable filtering.
virtual        ~StringFile();

        void    assign(FILE*file);					//!< Assigns an already open file for reading. \param file File to read from. May be NULL.
        bool    open(const String&filename);		//!< Opens a text file in readonly mode.  \return true if the file could be opened, false otherwise.
        bool    create(const String&filename);		//!< (Re)creates a text file for output. If the file exists then it is overwritten. \return true if the file could be created/overwritten, false otherwise.
        bool    append(const String&filename);		//!< Opens a text file for output. If the file exists then any succeeding content will be appended to it. Otherwise a new file is created.
        void    close();							//!< Closes the local file handle (if open)
        bool    isActive();							//!< Queries the current file state. \return true if a file is currently opened.
        bool    reset();							//!< Resets the file pointer to the beginning of the file. \return true on success.

        StringFile&    operator<<(const char*);
        StringFile&    operator<<(const TNewLine&);
        StringFile&    operator<<(const TEndLine&);
        StringFile&    operator<<(const TSpace&);
        StringFile&    operator<<(const TTabSpace&);
        StringFile&    operator<<(const String&);
        //StringFile&    operator<<(const String&);
        StringFile&    operator<<(char);
        StringFile&    operator<<(BYTE);
        StringFile&    operator<<(long long);
        StringFile&    operator<<(unsigned long long);
        StringFile&    operator<<(int);
        StringFile&    operator<<(unsigned);
        StringFile&    operator<<(long);
        StringFile&    operator<<(unsigned long);
        StringFile&    operator<<(short);
        StringFile&    operator<<(unsigned short);
        StringFile&    operator<<(float);
        StringFile&    operator<<(double);

		/*!
			\brief Reads one line from the currently opened file

			The stream read operator extracts one line from the opened file. Unless \a conversion_flags contains
			\a CM_RETURN_EMPTY_LINES empty lines are ignored. The returned string may be stripped of
			comments automatically by adding \a CM_STRIP_BLOCK_COMMENTS and/or \a CM_STRIP_LINE_COMMENTS to
			\a conversion_flags. <br>
			Common usage:<br>
			String line;<br>
			StringFile file(...);<br>
			while (file >> line)<br>
				(...)

			\param target String target to store the extracted line in (if any)
			\return true if a line could be extracted. The content of \a target remains unchanged if no line could be extracted.

		*/
        bool    operator>>(String&target);
			
		void	flush();	//!< Flushes the file
};


#endif
