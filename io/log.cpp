#include "../global_root.h"
#include "log.h"


//namespace IO
//{

	TLogIndent  indent(BYTE characters)
	{
		TLogIndent  rs;
		rs.length = characters;
		return rs;
	}

	char LogFile::space_buffer[0x100];


	LogFile::LogFile():f(NULL),format("[%Y %B %d. %H:%M:%S] "),indentation(0),indent_(false)
	{}

	LogFile::LogFile(const String&filename, bool makeclear):f(NULL),do_open(true),last(filename),format("[%Y %B %d. %H:%M:%S] "),indentation(0),indent_(false),make_clear(makeclear)
	{
		memset(space_buffer,' ',sizeof(space_buffer));
	}

	LogFile::~LogFile()
	{
		close();
	}

	bool LogFile::begin()
	{
		if (f)
			return true;
		if (!do_open)
			return false;
		if (make_clear)
			f = fopen(last.c_str(),"wb");
		else
			f = fopen(last.c_str(),"ab");
		do_open = false;
		return f != NULL;
	}

	bool LogFile::open(const String&filename, bool makeclear, bool delayed)
	{
		close();
		last = filename;
		do_open = true;
		make_clear = makeclear;
		if (!delayed)
			return begin();
		return true;
	}

	bool LogFile::create(const String&filename, bool delayed)
	{
		return open(filename,true,delayed);
	}

	bool LogFile::isActive()
	{
		return f!=NULL || do_open;
	}

	void LogFile::close()
	{
		if (f)
			fclose(f);
		f = NULL;
		do_open = false;
	}
    


	void LogFile::clear()
	{
		close();
		open(last,true,false);
	}

	void LogFile::timeStamp()
	{
		if (!begin())
			return;
		time_t tt = time(NULL);
		const tm*t = localtime(&tt);
		char output[256];
		size_t len = strftime(output,sizeof(output),format.c_str(),t);
		ASSERT__(fwrite(output,1,len,f)==len);
	}

	bool LogFile::log(const char*line, bool time_)
	{
		if (!begin())
			return false;
		if (indent_)
		{
			for (unsigned i = 0; i < indentation; i++)
			{
				static const char c = '\t';
				if (!fwrite(&c,1,1,f))
					return false;
			}
			indent_ = false;
		}
		if (time_)
			timeStamp();
		const char*nl = strchr(line,'\n');
		if (nl)
		{
			if (fwrite(line,1,nl-line+1,f)!=nl-line+1)
				return false;
			indent_ = true;
			return !strlen(nl+1) || log(nl+1,false);
		}
		size_t len = strlen(line);
		if (fwrite(line,1,len,f)!=len)
			return false;
		char nl_ = '\n';
		if (fwrite(&nl_,1,1,f)!=1)
			return false;
		fflush(f);
		return true;
	}


	bool LogFile::log(const String&line, bool time_)
	{
		if (!begin())
			return false;
		if (indent_)
		{
			for (unsigned i = 0; i < indentation; i++)
			{
				static const char c = '\t';
				if (fwrite(&c,1,1,f)!=1)
					return false;
			}
			indent_ = false;
		}
		if (time_)
			timeStamp();
		if (index_t at = line.indexOf('\n'))
		{
			if (fwrite(line.c_str(),1,at,f)!=at)
				return false;
			indent_ = true;
			return at == line.length() || log(line.c_str()+at,false);
		}
		if (fwrite(line.c_str(),1,line.length(),f)!=line.length())
			return false;
		char nl_ = '\n';
		if (fwrite(&nl_,1,1,f)!=1)
			return false;
		fflush(f);
		return true;
	}

	LogFile& LogFile::operator<<(const TEndLine&)
	{
		return operator<<(nl);
	}

	LogFile& LogFile::operator<<(const TNewLine&)
	{
		if (!begin())
			return *this;
		static const char c = '\n';
		size_t written = fwrite(&c,1,1,f);
	
		fflush(f);
		indent_ = true;
	
		return *this;
	}

	LogFile& LogFile::operator<<(const char*line)
	{
		log(line);
		return *this;
	}

	LogFile& LogFile::operator<<(const String&line)
	{
		log(line);
		return *this;
	}

	LogFile& LogFile::operator<<(const TLogIndent&indent)
	{
		if (!begin())
			return *this;
		size_t written = fwrite(space_buffer,1,indent.length,f);
		fflush(f);
		indent_ = false;
		return *this;
	}

	void	LogFile::indent()
	{
		indentation++;
	}

	void	LogFile::outdent()
	{
		if (indentation)
			indentation--;
	}

        
//}

