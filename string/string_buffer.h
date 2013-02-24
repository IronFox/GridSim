#ifndef string_bufferH
#define string_bufferH

#include "../io/str_stream.h"
#include "../container/buffer.h"


class StringBuffer : public Buffer<char>
{
public:
	typedef Buffer<char>	Super;
	/**/				StringBuffer(size_t len=1024);
	/**/				StringBuffer(const char*string, size_t extra=100);
	/**/				StringBuffer(const String&string, size_t extra=100);
	StringBuffer&		operator=(const StringBuffer&other);
	StringBuffer&		operator=(const String&string);
	StringBuffer&		operator=(const char*string);
    StringBuffer&		operator<<(const String&);
    StringBuffer&		operator<<(const char*);
    StringBuffer&		operator<<(const StringBuffer&);
    StringBuffer&		operator<<(const TNewLine&);
    StringBuffer&		operator<<(const TSpace&);
    StringBuffer&		operator<<(const TTabSpace&);
    StringBuffer&		operator<<(const ArrayData<char>&);
	StringBuffer&		operator<<(const StringRef&);
	StringBuffer&		operator<<(char);
	StringBuffer&		operator<<(BYTE);
	StringBuffer&		operator<<(long long);
	StringBuffer&		operator<<(unsigned long long);
	StringBuffer&		operator<<(int);
	StringBuffer&		operator<<(unsigned);
	StringBuffer&		operator<<(long);
	StringBuffer&		operator<<(unsigned long);
	StringBuffer&		operator<<(short);
	StringBuffer&		operator<<(unsigned short);
	StringBuffer&		operator<<(float);
	StringBuffer&		operator<<(double);
		
	void				Insert(index_t index, const char*str, size_t len);	//!< Inserts a string segment before the specified position (0=first character)
	void				Insert(index_t index, const String&string);			//!< Inserts a string before the specified position (0=first character)
		
	inline	char		operator[](index_t index)	const{return Super::operator[](index);}
		
	StringRef			ToStringRef()	const;		//!< Returns a string reference to the local content
	String				ToString()		const;		//!< Converts the local content to a string

	inline void			Write(const char*str, size_t strLen)	{memcpy(Super::appendRow(strLen),str,strLen);}
};


#endif
