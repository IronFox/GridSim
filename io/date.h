#ifndef dateH
#define dateH

#ifdef __GNUC__
	#include <sys/time.h>
#endif
#include <time.h>

class Date
{
private:
		char			buffer[0x100];
public:
		const char*		toString(const char*format);
		const char*		toString();
};
extern Date  date;


#endif
