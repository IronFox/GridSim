#include "../global_root.h"
#include "string_file.h"

/******************************************************************

Simplified string-file-handler.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

//#include "log.h"

//LogFile	StringFile::log_file("stringfile.log",true);

static inline void awrite(const void*data,size_t a,size_t b, FILE*f)
{
	if (fwrite(data,a,b,f)!=b)
		throw IO::DriveAccess::DataWriteFault("StringFile: Unable to write "+String(a*b)+" byte(s) to file");

}


void StringFile::stripComments(String&str)
{
	//log_file << "stripping comments of '"<<str<<"'"<<nl;
    index_t comment_start(1);
    while (true)
    {
        if ((conversion_flags&CM_RECORD_COMMENTS) && in_comment)
        {
            index_t at = str.indexOf(comment_end);
            if (at)
            {
				if (conversion_flags&CM_RECORD_COMMENTS)
					comment += " "+str.subString(comment_start+comment_begin.length()-1,at-comment_start-comment_begin.length());
                str.erase(comment_start-1,at+comment_end.length()-comment_start);
                in_comment = false;
            }
            else
            {
				if (conversion_flags&CM_RECORD_COMMENTS)
					comment += " "+str.subString(comment_start+comment_begin.length()-1);
                str.erase(comment_start-1);
				//log_file << " => '"<<str<<"'"<<nl;
                return;
            }
        }
        index_t lcomment(0),bcomment(0);
        if (conversion_flags&CM_STRIP_LINE_COMMENTS)
            lcomment = str.indexOf(line_comment);
        if (conversion_flags&CM_STRIP_BLOCK_COMMENTS)
            bcomment = str.indexOf(comment_begin);
        if (!bcomment && !lcomment)
		{
			//log_file << " => '"<<str<<"'"<<nl;
            return;
		}
        if (lcomment && (lcomment < bcomment || !bcomment))
        {
			if (conversion_flags&CM_RECORD_COMMENTS)
				comment += " "+str.subString(lcomment+line_comment.length()-1);
            str.erase(lcomment-1);
			//log_file << " => '"<<str<<"'"<<nl;
            return;
        }
        comment_start = bcomment;
        in_comment = true;
    }
	//log_file << " => '"<<str<<"'"<<nl;
}



StringFile::StringFile(const String&filename, unsigned flags, bool create_):active(false),write_mode(false),read_mode(false),had13(false),fclose_on_destruct(true),
                f(NULL),e(&buffer[sizeof(buffer)-1]),line_comment("//"),comment_begin("/*"),comment_end("*/"),conversion_flags(flags),
                root_line(1)
{
    buffer[sizeof(buffer)-1] = 0;
	if (!create_)
		open(filename);
	else
		create(filename);
}

StringFile::StringFile(FILE*file, unsigned flags):active(false),write_mode(false),read_mode(false),had13(false),fclose_on_destruct(true),
                f(NULL),e(&buffer[sizeof(buffer)-1]),line_comment("//"),comment_begin("/*"),comment_end("*/"),conversion_flags(flags),
                root_line(1)
{
    buffer[sizeof(buffer)-1] = 0;
    assign(file);
}

StringFile::StringFile(unsigned flags):active(false),write_mode(false),read_mode(false),had13(false),fclose_on_destruct(true),
                f(NULL),e(&buffer[sizeof(buffer)-1]),line_comment("//"),comment_begin("/*"),comment_end("*/"),conversion_flags(flags),
                root_line(1)
{
    buffer[sizeof(buffer)-1] = 0;
}


StringFile::~StringFile()
{
	close();
}

void StringFile::assign(FILE*file)
{
	close();
    f = file;
    active = f!=NULL;
    write_mode = false;
    read_mode = active;
	fclose_on_destruct = false;
    reset();
}

bool StringFile::open(const String&filename)
{
	//log_file << "reading '"<<filename<<"'"<<nl;

	close();
    f = fopen(filename.c_str(),"rb");
    active = f!=NULL;
    write_mode = false;
    read_mode = active;
	fclose_on_destruct = true;
    return reset();
}

bool StringFile::create(const String&filename)
{
    close();
    f = fopen(filename.c_str(),"wb");
    active = f!=NULL;
    write_mode = active;
    read_mode = false;
	fclose_on_destruct = true;
    return active;
}


bool StringFile::append(const String&filename)
{
    close();
    f = fopen(filename.c_str(),"a+b");
    active = f!=NULL;
    write_mode = active;
    read_mode = false;
	fclose_on_destruct = true;
    return active;
}

void StringFile::close()
{
    if (f && fclose_on_destruct)
        fclose(f);
    f = NULL;
}

bool StringFile::isActive()
{
    return active;
}

bool StringFile::reset()
{
    if (!read_mode)
        return false;
    fseek(f,0,SEEK_SET);
    string = buffer;
    end = string;
    fragment = "";
    in_comment = false;
    root_line = 0;
    had13 = false;
    return true;
}



bool StringFile::operator>>(String&target)
{
	comment = "";
    if (!read_mode)
        return false;

    while (true)
    {
        if (string >= end)
        {
			//log_file << "reached buffer end. reading "<<(e-buffer)<<" more bytes"<<nl;

            string = buffer;
            end = string + fread(string,1,e-string,f);
            (*end) = 0;
            if (end <= string)
            {
				//log_file << "reached end of file. finalizing"<<nl;
                target = fragment;
                fragment = "";
                if (target.length())
                    stripComments(target);
                return target.length()>0;
            }
        }
        char*c = string;
        while (c!=end)
        {
            //#13#10   / \n := 10

            if ((*c) == '\n' && had13)
            {
                string++;
                c++;
                had13 = false;
                continue;
            }

            if ((*c) == '\r' || (*c) == '\n')
            {
                had13 = (*c) == '\r';
                root_line++;
                (*c) = 0;
				//log_file << "reached end of line: "<<fragment<<nl<<"   +"<<string<<nl;
                target = fragment+string;
                string = c+1;
				fragment = "";
                if (target.length())
                    stripComments(target);
                if (target.length()||(conversion_flags&CM_RETURN_EMPTY_LINES))
                {
                    return true;
                }
            }
            else
                had13 = false;
            c++; //love this line :)
        }

        fragment+=string;
        string = end;
    }
	return false;
}


StringFile& StringFile::operator<< (const char*line)
{
    if (!write_mode)
        return *this;
    if (fputs(line,f) == EOF)
		throw IO::DriveAccess::DataWriteFault(__func__ ": Unable to write string '"+String(line)+"' to file");
    return *this;
}

StringFile& StringFile::operator<< (const String&line)
{
    if (!write_mode)
        return *this;
    awrite(line.c_str(),1,line.length(),f);
    return *this;
}


StringFile& StringFile::operator<< (const TEndLine&)
{
	return operator<<(nl);
}

StringFile& StringFile::operator<< (const TNewLine&)
{
    if (!write_mode)
        return *this;
	#if SYSTEM==WINDOWS
		static const char c[2] = {'\r','\n'};
		awrite(c,2,1,f);
	#else
		static const char c = '\n';
		awrite(&c,1,1,f);
	#endif
    return *this;
}

StringFile&  StringFile::operator<<(const TSpace&space)
{
    if (!write_mode || space.length <= 0)
        return *this;
	static const char*space16 = "                ";
	unsigned len = (unsigned)space.length;
	while (len > 16)
	{
		awrite(space16,16,1,f);
		len -= 16;
	}
	awrite(space16,len,1,f);
    return *this;
}

StringFile&  StringFile::operator<<(const TTabSpace&space)
{
    if (!write_mode || space.length <= 0)
        return *this;
	static const char*space16 = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	unsigned len = (unsigned)space.length;
	while (len > 16)
	{
		awrite(space16,16,1,f);
		len -= 16;
	}
	awrite(space16,len,1,f);
    return *this;
}

StringFile& StringFile::operator<< (char c)
{
    if (!write_mode)
        return *this;
    awrite(&c,1,1,f);
    return *this;
}

StringFile& StringFile::operator<< (BYTE item)
{
    String str(item);
    awrite(str.c_str(),1,str.length(),f);
    return *this;
}

StringFile& StringFile::operator<< (long long item)
{
    String str(item);
    awrite(str.c_str(),1,str.length(),f);
    return *this;
}

StringFile& StringFile::operator<< (unsigned long long item)
{
    String str(item);
    awrite(str.c_str(),1,str.length(),f);
    return *this;
}

StringFile& StringFile::operator<< (int item)
{
    String str(item);
    awrite(str.c_str(),1,str.length(),f);
    return *this;
}

StringFile& StringFile::operator<< (unsigned item)
{
    String str(item);
    awrite(str.c_str(),1,str.length(),f);
    return *this;
}

StringFile& StringFile::operator<< (short item)
{
    String str(item);
    awrite(str.c_str(),1,str.length(),f);
    return *this;
}

StringFile& StringFile::operator<< (unsigned short item)
{
    String str(item);
    awrite(str.c_str(),1,str.length(),f);
    return *this;
}

StringFile& StringFile::operator<< (long item)
{
    String str(item);
    awrite(str.c_str(),1,str.length(),f);
    return *this;
}

StringFile& StringFile::operator<< (unsigned long item)
{
    String str(item);
    awrite(str.c_str(),1,str.length(),f);
    return *this;
}

StringFile& StringFile::operator<< (float item)
{
    String str(item);
    awrite(str.c_str(),1,str.length(),f);
    return *this;
}

StringFile& StringFile::operator<< (double item)
{
    String str(item);
    awrite(str.c_str(),1,str.length(),f);
    return *this;
}

void	StringFile::flush()
{
	fflush(f);
}


