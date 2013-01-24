#ifndef string_bufferH
#define string_bufferH

#include "../io/str_stream.h"



class StringBuffer
{
private:
        char           *field,*current;
        size_t          len,remaining;
		
		bool			expand(size_t length, bool decrease_remaining);
public:
        bool            overflow, auto_resize;


                        StringBuffer(size_t len=1024);
						StringBuffer(const StringBuffer&other);
						StringBuffer(const char*string, size_t extra=100);
						StringBuffer(const String&string, size_t extra=100);
virtual                ~StringBuffer();
		StringBuffer&	operator=(const StringBuffer&other);
		StringBuffer&	operator=(const String&string);
		StringBuffer&	operator=(const char*string);
		void			adoptData(StringBuffer&other);
        void            resize(size_t len);		//!< Resizes the buffer to the specified length in characters. Unless @b auto_resize is set false, the buffer will expand its memory as necessary
        StringBuffer&  operator<<(const String&);
        StringBuffer&  operator<<(const char*);
        StringBuffer&  operator<<(const StringBuffer&);
        StringBuffer&  operator<<(const TNewLine&);
        StringBuffer&  operator<<(const TSpace&);
        StringBuffer&  operator<<(const TTabSpace&);
		StringBuffer&  operator<<(char);
		StringBuffer&  operator<<(BYTE);
		StringBuffer&  operator<<(long long);
		StringBuffer&  operator<<(unsigned long long);
		StringBuffer&  operator<<(int);
		StringBuffer&  operator<<(unsigned);
		StringBuffer&  operator<<(long);
		StringBuffer&  operator<<(unsigned long);
		StringBuffer&  operator<<(short);
		StringBuffer&  operator<<(unsigned short);
		StringBuffer&  operator<<(float);
		StringBuffer&  operator<<(double);
		
        bool            write(const char*,size_t len);
		bool			insert(size_t index, char c);	//!< Inserts a singular character before the specified position (0=first character)
		bool			insert(size_t index, const char*str, size_t len);	//!< Inserts a string segment before the specified position (0=first character)
		bool			insert(size_t index, const String&string);			//!< Inserts a string before the specified position (0=first character)
		bool			erase(size_t index, size_t len);					//!< Erases a number of characters from the buffer. @return true if at least one character was removed, false otherwise.
		
inline	char			operator[](size_t index)	const
						{
							return field[index];
						}
		
        void            reset();					//!< Resets the local string to the first character of the buffer without truncating the character array
        void			clear();					//!< @copydoc reset()
		char*     		root();						//!< Retrieves a pointer to the beginning of the local buffer field
        const char*     root()      	const;		//!< @copydoc root()
        char*     		pointer();					//!< @copydoc root()
        const char*     pointer()      	const;		//!< @copydoc root()
        size_t          length()    	const;		//!< Retrieves the current fill state of the local buffer
		ReferenceExpression<char>
						toStringRef()	const;		//!< Returns a string reference to the local content
		String			toString()		const;		//!< Converts the local content to a string
        char            terminator()    const;		//!< Queries the final character of the content in the local buffer
		
};


#endif
