#include "../global_root.h"
#include "string_file.h"

/******************************************************************

Simplified string-file-handler.

******************************************************************/
namespace DeltaWorks
{

	void StringFile::awrite(const void*data,size_t numBytes)
	{
		if (!f)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): File is not opened for write access");

		const size_t written = fwrite(data,1,numBytes,f);
		if (written != numBytes)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Failed to write all of "+String(numBytes)+" byte(s) to file. Operation did write "+String(written)+" byte(s)");

	}


	void StringFile::stripComments(String&str)
	{
		//log_file << "stripping comments of '"<<str<<"'"<<nl;
		index_t comment_start(1);
		while (true)
		{
			if (/*(conversion_flags&CM_RECORD_COMMENTS) && */in_comment)
			{
				index_t at = str.GetIndexOf(comment_end);
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
			index_t lcomment(0),lscomment(0),bcomment(0);
			if (conversion_flags&CM_STRIP_LINE_COMMENTS)
				lcomment = str.GetIndexOf(lineComment);
			if (conversion_flags&CM_STRIP_LINE_START_COMMENTS)
			{
				index_t candidate = str.GetIndexOf(lineStartComment);
				bool valid = candidate > 0;
				for (index_t i = 0; i+1 < candidate; i++)
				{
					if (!IsWhitespace(str.get(i)))
					{
						valid = false;
						break;
					}
				}
				if (valid)
					lscomment = candidate;
			}
			if (conversion_flags&CM_STRIP_BLOCK_COMMENTS)
				bcomment = str.GetIndexOf(comment_begin);
			if (!bcomment && !lcomment && !lscomment)
			{
				//log_file << " => '"<<str<<"'"<<nl;
				return;
			}
			if (lscomment)
			{
				if (conversion_flags&CM_RECORD_COMMENTS)
					comment += " "+str.subString(lscomment+lineStartComment.length()-1);
				str.erase(lscomment-1);
				//log_file << " => '"<<str<<"'"<<nl;
				return;
			}
			if (lcomment && (lcomment < bcomment || !bcomment))
			{
				if (conversion_flags&CM_RECORD_COMMENTS)
					comment += " "+str.subString(lcomment+lineComment.length()-1);
				str.erase(lcomment-1);
				//log_file << " => '"<<str<<"'"<<nl;
				return;
			}
			comment_start = bcomment;
			in_comment = true;
		}
		//log_file << " => '"<<str<<"'"<<nl;
	}



	StringFile::StringFile(const PathString&filename, unsigned flags, bool create_):active(false),write_mode(false),read_mode(false),had13(false),fclose_on_destruct(true),
					f(NULL),e(&buffer[sizeof(buffer)-1]),lineComment("//"),lineStartComment("#"), comment_begin("/*"),comment_end("*/"),conversion_flags(flags),
					root_line(1)
	{
		buffer[sizeof(buffer)-1] = 0;
		if (!create_)
			Open(filename);
		else
			Create(filename);
	}

	StringFile::StringFile(FILE*file, unsigned flags):active(false),write_mode(false),read_mode(false),had13(false),fclose_on_destruct(true),
					f(NULL),e(&buffer[sizeof(buffer)-1]),lineComment("//"),lineStartComment("#"),comment_begin("/*"),comment_end("*/"),conversion_flags(flags),
					root_line(1)
	{
		buffer[sizeof(buffer)-1] = 0;
		Assign(file);
	}

	StringFile::StringFile(unsigned flags):active(false),write_mode(false),read_mode(false),had13(false),fclose_on_destruct(true),
					f(NULL),e(&buffer[sizeof(buffer)-1]),lineComment("//"),lineStartComment("#"),comment_begin("/*"),comment_end("*/"),conversion_flags(flags),
					root_line(1)
	{
		buffer[sizeof(buffer)-1] = 0;
	}


	StringFile::~StringFile()
	{
		Close();
	}

	void StringFile::Assign(FILE*file)
	{
		Close();
		filename = "Assign()";
		f = file;
		active = f!=NULL;
		write_mode = false;
		read_mode = active;
		fclose_on_destruct = false;
		Rewind();
	}

	void StringFile::Open(const PathString&filename)
	{
		//log_file << "reading '"<<filename<<"'"<<nl;

		Close();
		f = FOPEN(filename.c_str(),"rb");
		if (f == nullptr)
			throw Except::IO::DriveAccess::FileOpenFault(CLOCATION,"Failed to append to file "+String(filename));
		active = true;
		write_mode = false;
		read_mode = true;
		fclose_on_destruct = true;
		this->filename = filename;
		Rewind();
	}

	void StringFile::Create(const PathString&filename)
	{
		Close();
		f = FOPEN(filename.c_str(),"wb");
		if (f == nullptr)
			throw Except::IO::DriveAccess::FileOpenFault(CLOCATION,"Failed to create file "+String(filename));
		active = true;
		write_mode = true;
		read_mode = false;
		fclose_on_destruct = true;
		this->filename = filename;
	}


	void StringFile::Append(const PathString&filename)
	{
		Close();
		f = FOPEN(filename.c_str(),"a+b");
		if (f == nullptr)
			throw Except::IO::DriveAccess::FileOpenFault(CLOCATION,"Failed to append to file "+String(filename));
		
		active = true;
		write_mode = true;
		read_mode = false;
		fclose_on_destruct = true;
		this->filename = filename;
	}

	void StringFile::Close()
	{
		if (f && fclose_on_destruct)
			fclose(f);
		string = buffer;
		end = string;
		f = NULL;
		active = false;
		this->filename = "";
	}

	bool StringFile::IsActive()
	{
		return active;
	}

	void StringFile::Rewind()
	{
		if (!read_mode)
			return;
		fseek(f,0,SEEK_SET);
		string = buffer;
		end = string;
		fragment = "";
		in_comment = false;
		root_line = 0;
		had13 = false;
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
				end = string + (f != NULL ? fread(string,1,e-string,f) : 0);
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
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Unable to write string '"+String(line)+"' to file");
		return *this;
	}

	StringFile& StringFile::operator<< (const String&line)
	{
		return operator<<(line.ref());
	}

	StringFile& StringFile::operator<< (const StringRef&line)
	{
		if (!write_mode)
			return *this;
		awrite(line.pointer(),line.length());
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
			awrite(c,2);
		#else
			static const char c = '\n';
			awrite(&c,1);
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
			awrite(space16,16);
			len -= 16;
		}
		awrite(space16,len);
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
			awrite(space16,16);
			len -= 16;
		}
		awrite(space16,len);
		return *this;
	}

	StringFile& StringFile::operator<< (char c)
	{
		if (!write_mode)
			return *this;
		awrite(&c,1);
		return *this;
	}

	StringFile& StringFile::operator<< (BYTE item)
	{
		if (!write_mode)
			return *this;
		if (fprintf(f,"%hhu",item) <= 0)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Unable to write number "+String(item)+" to file");
		return *this;
	}

	StringFile& StringFile::operator<< (long long item)
	{
		if (!write_mode)
			return *this;
		if (fprintf(f,"%lli",item) <= 0)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Unable to write number "+String(item)+" to file");
		return *this;
	}

	StringFile& StringFile::operator<< (unsigned long long item)
	{
		if (!write_mode)
			return *this;
		if (fprintf(f,"%llu",item) <= 0)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Unable to write number "+String(item)+" to file");
		return *this;
	}

	StringFile& StringFile::operator<< (int item)
	{
		if (!write_mode)
			return *this;
		if (fprintf(f,"%i",item) <= 0)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Unable to write number "+String(item)+" to file");
		return *this;
	}

	StringFile& StringFile::operator<< (unsigned item)
	{
		if (!write_mode)
			return *this;
		if (fprintf(f,"%u",item) <= 0)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Unable to write number "+String(item)+" to file");
		return *this;
	}

	StringFile& StringFile::operator<< (short item)
	{
		if (!write_mode)
			return *this;
		if (fprintf(f,"%hi",item) <= 0)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Unable to write number "+String(item)+" to file");
		return *this;
	}

	StringFile& StringFile::operator<< (unsigned short item)
	{
		if (!write_mode)
			return *this;
		if (fprintf(f,"%hu",item) <= 0)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Unable to write number "+String(item)+" to file");
		return *this;
	}

	StringFile& StringFile::operator<< (long item)
	{
		if (!write_mode)
			return *this;
		if (fprintf(f,"%li",item) <= 0)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Unable to write number "+String(item)+" to file");
		return *this;
	}

	StringFile& StringFile::operator<< (unsigned long item)
	{
		if (!write_mode)
			return *this;
		if (fprintf(f,"%lu",item) <= 0)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Unable to write number "+String(item)+" to file");
		return *this;
	}

	StringFile& StringFile::operator<< (float item)
	{
		if (!write_mode)
			return *this;
		if (fprintf(f,"%f",item) <= 0)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Unable to write number "+String(item)+" to file");
		return *this;
	}

	StringFile& StringFile::operator<< (double item)
	{
		if (!write_mode)
			return *this;
		if (fprintf(f,"%f",item) <= 0)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Unable to write number "+String(item)+" to file");
		return *this;
	}

	StringFile& StringFile::operator<< (long double item)
	{
		if (!write_mode)
			return *this;
		if (fprintf(f,"%Lf",item) <= 0)
			throw Except::IO::DriveAccess::DataWriteFault("StringFile("+String(filename)+"): Unable to write number "+String(item)+" to file");
		return *this;
	}

	void	StringFile::Flush()
	{
		fflush(f);
	}

}
