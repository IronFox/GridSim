#ifndef str_streamH
#define str_streamH

#include "../global_string.h"


namespace StrStream
{
    struct TNewLine {};
	struct TSpace
	{
		unsigned	length;
	};
	
	
	inline TSpace	space(int len)
	{
		TSpace result = {(unsigned)len};
		return result;
	}

	struct TTabSpace
	{
		unsigned	length;
	};
	
	
	inline TTabSpace	tabSpace(int len)
	{
		TTabSpace result = {(unsigned)len};
		return result;
	}

static const TNewLine  nl  = TNewLine();
static const char  nl_  = '\n';
static const char  n_  = '\n';
static const char  tab = '\t';

	typedef std::ostream& (*TEndLine)(std::ostream&);
}

using namespace StrStream;


inline std::ostream& operator<<(std::ostream&stream, const TNewLine&)
{
	return stream << std::endl;
}

inline std::ostream& operator<<(std::ostream&stream, const TSpace&space)
{
	for (unsigned i = 0; i < space.length; i++)
		stream << ' ';
	return stream;
}

inline std::ostream& operator<<(std::ostream&stream, const TTabSpace&space)
{
	for (unsigned i = 0; i < space.length; i++)
		stream << '\t';
	return stream;
}



#endif
