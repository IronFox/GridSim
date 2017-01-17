#include "../global_root.h"
#include "string_list.h"
#include <functional>


namespace Tokenizer
{
	void	ItemSet::addTokens(const char*token_string)
	{
		const char*at = token_string;
		while (*at)
		{
			first_char[(BYTE)*at] = true;
			if (const char*found = strchr(at,' '))
			{
				size_t len = found-at;
				set(StringTemplate<char>(at,len));
				at = found +1;
				if (len > max_len)
					max_len = len;
			}
			else
			{
				String str = at;
				if (str.length() > max_len)
					max_len = str.length();
				set(str);
				return;
			}
		}
	}


	template <typename Receiver>
	static bool			tokenize(const char*composition, size_t length, const Configuration&config, Receiver&receiver)
	{
		if (!*composition)
			return true;
		
		#undef ADD_ENTRY
		#define ADD_ENTRY(_SEGMENT_)	receiver << StringRef(_SEGMENT_.first,_SEGMENT_.last-_SEGMENT_.first+1);
		//#define ADD_ENTRY(_SEGMENT_)	target.add(_SEGMENT_);

		
			
		#undef END_INCLUSIVE
		#undef END_EXCLUSIVE
		#define END_INCLUSIVE\
				{\
						TSegment segment = {token_start,ch-1};\
						if (trimSegment(segment,config))\
							ADD_ENTRY(segment);\
						token_start = ch;\
				}
		#define END_EXCLUSIVE\
				{\
						TSegment segment = {token_start,ch-2};\
						if (trimSegment(segment,config))\
							ADD_ENTRY(segment);\
						token_start = ch-1;\
				}

		const char	*ch = composition,
					*token_start = composition;
		
		const char	*const end = composition+length;

		TemporaryStatus	status;
		
		bool escaped = false;
		bool double_escaped = false;
		while (ch != end)
		{
			char c = *ch++;
			if (config.convert_tabs && c=='\t')
				c = ' ';
			if (status.in_string)
			{
				size_t qindex;
				if (config.quotation_escape_character == c)
					escaped = !escaped;
				elif (!escaped && (qindex = config.quotations.query(c)) && qindex == status.in_string)
				{
					status.in_string = 0;
					if (!status.recursion_level && config.string_break)
					{
						END_INCLUSIVE
						continue;
					}
				}
			}
			else
			{
				size_t rindex;
				if ((status.in_string = config.quotations.query(c)))
				{
					escaped = false;
					if (!status.recursion_level && config.string_break)
					{
						END_EXCLUSIVE
						continue;
					}
				}
				elif ((rindex = config.recursion_up.query(c)))
				{
					if (!status.recursion_level && config.recursion_break)
						END_EXCLUSIVE
					//if (status.recursion_level < ARRAYSIZE(status.recursion_buffer))
						status.recursion_buffer[status.recursion_level++] = rindex;
				}
				elif (status.recursion_level)
				{
					if (config.recursion_down.query(c) == status.recursion_buffer[status.recursion_level-1])
					{
						status.recursion_level--;
						if (!status.recursion_level)
						{
							if (config.recursion_break)
								END_INCLUSIVE
							continue;
						}
					}
				}
				else
				{
					if (config.operators.itemBeginsWith(c))
					{
						size_t max = size_t(end-ch) >= config.operators.maxLength() ? config.operators.maxLength() : (end-ch);
						bool found = false;
						for (size_t j = max; j > 0; j--)
							if (config.operators.isSet(ReferenceExpression<char>(ch-1,j)))
							{
								END_EXCLUSIVE
								ch += j-1;
								END_INCLUSIVE
								found = true;
								break;
							}
						if (found)
							continue;
					}
					if (config.main_separator.isset(c))
					{
						END_EXCLUSIVE
						token_start++;
						continue;
					}
				}
			}
			if (double_escaped)
			{
				double_escaped = false;
				escaped = false;
			}
			else
				if (escaped)
					double_escaped = true;
		}
		END_INCLUSIVE
		return !status.in_string && !status.recursion_level;
	}
	
	
	bool			tokenize(const char*composition, const Configuration&config, StringList&target, bool clear_list)
	{
		if (clear_list)
			target.clear();
		return tokenize(composition,strlen(composition),config,target);
	}
	
	bool			tokenize(const String&composition, const Configuration&config, StringList&target, bool clear_list)
	{
		if (clear_list)
			target.clear();
		return tokenize(composition.c_str(),composition.length(),config,target);
	}

	bool			Tokenize(const StringRef&composition, const Configuration&config, BasicBuffer<StringRef>&target, bool clear_list)
	{
		if (clear_list)
			target.clear();
		return tokenize(composition.pointer(),composition.length(),config,target);
	}
	
	
	template <typename Receiver>
	static void			tokenizeSegment(const char*composition_segment, size_t length, const Configuration&config, Status&status, Receiver&receiver)
	{
		if (!*composition_segment)
			return;
			
			
		#undef END_INCLUSIVE
		#undef END_EXCLUSIVE
		#define END_INCLUSIVE\
				{\
					if (status.buffer.IsNotEmpty())\
					{\
						status.buffer.append(token_start,ch-token_start);\
						TSegment segment = {status.buffer.pointer(),status.buffer.pointer()+status.buffer.Count()-1};\
						if (trimSegment(segment,config))\
							ADD_ENTRY(segment);\
						status.buffer.reset();\
					}\
					else\
					{\
						TSegment segment = {token_start,ch-1};\
						if (trimSegment(segment,config))\
							ADD_ENTRY(segment);\
					}\
					token_start = ch;\
				}
		#define END_EXCLUSIVE\
				{\
					if (status.buffer.IsNotEmpty())\
					{\
						status.buffer.append(token_start,ch-token_start-1);\
						TSegment segment = {status.buffer.pointer(),status.buffer.pointer()+status.buffer.Count()-1};\
						if (trimSegment(segment,config))\
							ADD_ENTRY(segment);\
						status.buffer.reset();\
					}\
					else\
					{\
						TSegment segment = {token_start,ch-2};\
						if (trimSegment(segment,config))\
							ADD_ENTRY(segment);\
					}\
					token_start = ch-1;\
				}

		const char	*ch = composition_segment,
					*token_start = composition_segment;
		
		
		const char	*const end = composition_segment+length;


		bool escaped = false;
		bool double_escaped = false;
		while (ch != end)
		{
			char c = *ch++;
			if (config.convert_tabs && c=='\t')
				c = ' ';
			if (status.in_string)
			{
				size_t qindex;
				if (config.quotation_escape_character == c)
					escaped = !escaped;
				elif (!escaped && (qindex = config.quotations.query(c)) && qindex == status.in_string)
				{
					status.in_string = 0;
					if (!status.recursion_level && config.string_break)
					{
						END_INCLUSIVE
						continue;
					}
				}
			}
			else
			{
				size_t rindex;
				if ((status.in_string = config.quotations.query(c)))
				{
					escaped = false;
					if (!status.recursion_level && config.string_break)
					{
						END_EXCLUSIVE
						continue;
					}
				}
				elif ((rindex = config.recursion_up.query(c)))
				{
					if (!status.recursion_level && config.recursion_break)
						END_EXCLUSIVE
					//if (status.recursion_level < ARRAYSIZE(status.recursion_buffer))
						status.recursion_buffer[status.recursion_level++] = rindex;
				}
				elif (status.recursion_level)
				{
					if (config.recursion_down.query(c) == status.recursion_buffer[status.recursion_level-1])
					{
						status.recursion_level--;
						if (!status.recursion_level)
						{
							if (config.recursion_break)
								END_INCLUSIVE
							continue;
						}
					}
				}
				else
				{
					if (config.operators.itemBeginsWith(c))
					{
						size_t max = size_t(end-ch) >= config.operators.maxLength() ? config.operators.maxLength() : size_t(end-ch);
						bool found = false;
						for (size_t j = max; j > 0; j--)
							if (config.operators.isSet(ReferenceExpression<char>(ch-1,j)))
							{
								END_EXCLUSIVE
								ch += j-1;
								END_INCLUSIVE
								found = true;
								break;
							}
						if (found)
							continue;
					}
					
					if (config.main_separator.isset(c))
					{
						END_EXCLUSIVE
						token_start++;
						continue;
					}
				}
			}
			if (double_escaped)
			{
				double_escaped = false;
				escaped = false;
			}
			else
				if (escaped)
					double_escaped = true;
		}
		
		status.buffer.append(token_start,ch-token_start);
	}
	
	void			tokenizeSegment(const char*composition_segment, const Configuration&config, Status&status, StringList&target)
	{
		tokenizeSegment(composition_segment,strlen(composition_segment),config,status,target);
	}

	void			tokenizeSegment(const String&composition_segment, const Configuration&config, Status&status, StringList&target)
	{
		tokenizeSegment(composition_segment.c_str(),composition_segment.length(),config,status,target);
	}

	
	
	bool			finish(const Configuration&config, Status&status, StringList&target)
	{
		TSegment segment = {status.buffer.pointer(),status.buffer.pointer()+status.buffer.Count()-1};
		if (trimSegment(segment,config))
			//target.add(segment);
			target.add(String(segment.first,segment.last-segment.first+1));
		status.buffer.reset();
		return !status.in_string && !status.recursion_level;
	}
	
	
	String			trim(const String&string, const Configuration&config)
	{
		const char	*begin = string.c_str(),*start = begin,
					*end = begin+string.length()-1;
		while (begin < end && config.trim_characters.isset(*begin))
			begin++;
		while (begin < end && config.trim_characters.isset(*end))
			end--;
		if (begin <= end)
			return string.subString(begin-start,end-begin+1);
		return "";
	}
	
	TSegment		trim(const TSegment&segment, const Configuration&config)
	{
		TSegment	result = segment;
		while (result.first < result.last && config.trim_characters.isset(*result.first))
			result.first++;
		while (result.first < result.last && config.trim_characters.isset(*result.last))
			result.last--;
		return result;
	}
	
	bool			trimSegment(TSegment&segment, const Configuration&config)
	{
		while (segment.first < segment.last && config.trim_characters.isset(*segment.first))
			segment.first++;
		while (segment.first < segment.last && config.trim_characters.isset(*segment.last))
			segment.last--;
		return segment.first <= segment.last;
	}
	

	size_t		checkQuotation(const String&line, const Configuration&config)
	{
		if (line.length()<2)
			return 0;
		size_t result = 1;
		bool escaped=false;
			
		size_t quote_index = config.quotations.query(line.firstChar());
		if (!quote_index)
			return 0;
		for (size_t i = 1; i+1 < line.length(); i++)
		{
			char c = line.get(i);
			if (config.quotation_escape_character == c)
			{
				escaped=!escaped;
				if (!escaped)
					result++;
			}
			else
				if (config.quotations.query(c) && !escaped)
					return 0;
				else
				{
					if (escaped && !config.quotations.query(line.get(i)))
						result++;
					escaped = false;
					result++;
				}
		}
		if (config.quotations.query(line.lastChar()) == quote_index)
			return result;
		return 0;
	}

	static void innerDequote(const char*in_str, size_t in_len, char*out_str,size_t out_len, const Configuration&config)
	{
		bool escaped = false;
		char*out = out_str;
		for (size_t i = 1; i+1 < in_len; i++)
		{
			char c = in_str[i];
			bool is_escape = config.quotation_escape_character == c;
			if (is_escape && !escaped)
				escaped=true;
			else
			{
				if (escaped && !is_escape && !config.quotations.query(c))
					(*out++) = config.quotation_escape_character;
				escaped = false;
				(*out++) = c;
			}
		}
		ASSERT_EQUAL__((void*)out,(void*)(out_str+out_len));
	}

	bool dequote(const String&line, const Configuration&config, String&result)
	{
		size_t	len = checkQuotation(line,config);
					
		if (&line != &result)
		{
			if (!len)
			{
				result = line;
				return false;
			}
			len--;
			result.resize(len);
			innerDequote(line.c_str(),line.length(),result.mutablePointer(),len,config);
		}
		else
		{
			if (!len)
				return false;
			len--;
			String rs;
			rs.resize(len);//= TStringLength(len);
			innerDequote(line.c_str(),line.length(),rs.mutablePointer(),len,config);
			result = rs;
		}
		return true;
	}
	
	String			dequote(const String&string, const Configuration&config)
	{
		String result;
		dequote(string,config,result);
		return result;
	}
	

	bool			dequote(StringList&list, const Configuration&config)
	{
		bool result = true;
		String rs;
		for (size_t i = 0; i < list.count(); i++)
			result &= dequote(list[i],config,list[i]);
		return result;
	}


}

//namespace List
//{
	String					 	StringList::empty_string;


	void StringList::add(const Tokenizer::TSegment&segment)
	{
		Super::operator<<(String(segment.first,segment.last-segment.first+1));
	}

	void StringList::add(const ReferenceExpression<char>&segment)
	{
		Super::operator<<(segment);
		//	String(segment.pointer(),segment.length()));
	}



	/*
	StringList& StringList::operator=(const StringList&other)
	{
		clear();
		addEntries(other);
		return *this;
	}

	*/

	void StringList::addEntries(const StringList&other)
	{
		Super::append(other);
	}

	StringList&  StringList::operator<<(String&&string)
	{
		Super::operator<<(string);
		return *this;
	}

	StringList&  StringList::operator<<(const String&string)
	{
		Super::operator<<(string);
		return *this;
	}

	StringList&  StringList::operator<<(const char*string)
	{
		add(string);
		return *this;
	}

	StringList&  StringList::operator<<(const Tokenizer::TSegment&segment)
	{
		add(segment);
		return *this;
	}

	StringList&  StringList::operator<<(const ReferenceExpression<char>&segment)
	{
		add(segment);
		return *this;
	}
/*
	bool		StringList::operator>>(String&target)
	{
		String*str = Super::each();
		if (!str)
			return false;
		target = *str;
		return true;
	}
*/
	StringList::operator size_t()												  const
	{
		return Super::count();
	}


	void StringList::eraseOccurrences(const String&string, bool case_sensitive)
	{
		if (case_sensitive)
		{
			for (index_t i = 0; i < Super::count(); i++)
				if (Super::at(i).operator==(string))
					Super::erase(i--);
		}
		else
		{
			const char*cstr = string.c_str();
			for (index_t i = 0; i < Super::count(); i++)
				if (!strcmpi(cstr,Super::at(i).c_str()))
					Super::erase(i--);
		}
	}

	String& StringList::getReverse(size_t index)
	{
		return get(count()-index-1);
	}

	const String& StringList::getReverse(size_t index) const
	{
		return get(count()-index-1);
	}

	String StringList::fuse(size_t index, size_t length, const char*glue, size_t glue_len)  const
	{
		size_t		len;
		size_t	cnt(count());
		if (index > cnt)
			index = cnt;
		if (length > cnt-index)
			length = cnt-index;
		if (!length)
			return "";
		len = ((size_t)length-1)*glue_len;
		for (size_t i = 0; i < length; i++)
			len+=Super::at(i+index).length();

		String result = String(TStringLength(len));
		char	*at = result.mutablePointer();
		const String&first = Super::at(index);
		memcpy(at,first.c_str(),first.length());
		at+=first.length();
		for (size_t i = 1; i < length; i++)
		{
			memcpy(at,glue,glue_len);
			at+=glue_len;
			const String&str = Super::at(i+index);
			memcpy(at,str.c_str(),str.length());
			at+=str.length();
		}
		//(*at++) = 0;
		ASSERT_EQUAL__((void*)at,(void*)(result.c_str()+len));
		return result;
	}


	size_t  StringList::countOccurrences(const String&string) const
	{
		size_t cnt = 0;
		for (size_t i = 0; i < count(); i++)
			if (Super::at(i).operator==(string))
				cnt++;
		return cnt;
	}

	void	StringList::add(String&&line)
	{
		Super::operator<<(line);
	}

	void StringList::add(const char*line)
	{
		Super::operator<<( String(line) );
	}

	void	StringList::add(const String&l)
	{
		Super::operator<<(l);
	}

	void	StringList::insert(size_t index, const char*line)
	{
		Super::insert(index,line);
	}

	void	StringList::insert(size_t index, const String&l)
	{
		Super::insert(index,l);
	}


	String&StringList::get(size_t index)
	{
		if (index < Super::count())
			return Super::at(index);
		empty_string = "";
		return empty_string;
	}

	const String&StringList::get(size_t index) const
	{
		if (index < Super::count())
			return Super::at(index);
		empty_string = "";
		return empty_string;
	}

	size_t	StringList::find(const String&line)	const
	{
		size_t rs = findCaseSensitive(line);
		if (!rs)
			rs = findCaseIgnore(line);
		return rs;
	}

	size_t	StringList::findCaseSensitive(const String&line) const
	{
		for (size_t i = 0; i < count(); i++)
			if (Super::at(i)==line)
				return i+1;
		return 0;
	}

	size_t StringList::findCaseIgnore(const String&line) const
	{
		for (size_t i = 0; i < count(); i++)
		{
			const String&str = Super::at(i);
			if (!strcmpi(str.c_str(),line.c_str()))
				return i+1;
		}
		return 0;
	}

	String StringList::list() const
	{
		String as="StringList ("+String(count())+")";
		for (size_t i = 0; i < count(); i++)
			as+="\n "+Super::at(i);
		return as;
	}

	std::ostream&	StringList::printLines(std::ostream&stream, const char*indent)					const
	{
		for (size_t i = 0; i < count(); i++)
			stream << indent << "("<<i<<") "<<Super::at(i)<<std::endl;
		return stream;
	}

	std::ostream&	StringList::printLines(std::ostream&stream, const String&indent)					const
	{
		for (size_t i = 0; i < count(); i++)
			stream << indent << "("<<i<<") "<<Super::at(i)<<std::endl;
		return stream;
	}








	StringList::StringList()
	{}

	StringList::StringList(const String&line)
	{
		const char*at = line.c_str();
		while (*at)
		{
			if (const char*found = strchr(at,' '))
			{
				add(StringTemplate<char>(at,found-at));
				at = found +1;
			}
			else
			{
				add(at);
				return;
			}
		}
	}

	StringList::StringList(const char*line)
	{
		if (line)
		{
			const char*at = line;
			while (*at)
			{
				if (const char*found = strchr(at,' '))
				{
					add(StringTemplate<char>(at,found-at));
					at = found +1;
				}
				else
				{
					add(at);
					return;
				}
			}
		}
	}

	StringList::StringList(const StringList&other)
	{
		Super::append(other);
	}


	size_t	StringList::charactersInCommon(const String&sample)					const
	{
		if (!count())
			return 0;
		size_t	max_len = sample.length();

		for (size_t len = 0; len < max_len; len++)
		{
			char test = sample.get(len);
			for (size_t i = 0; i < count(); i++)
				if (Super::at(i).get(len) != test)
					return len;
		}
		return max_len;
	}

	size_t	StringList::charactersInCommon()					const
	{
		if (Super::IsEmpty())
			return 0;
		const String&sample = Super::first();
		size_t	max_len = sample.length();

		for (size_t len = 0; len < max_len; len++)
		{
			char test = sample.get(len);
			for (size_t i = 1; i < count(); i++)
				if (Super::at(i).get(len) != test)
					return len;
		}
		return max_len;
	}

	const String& StringList::longest() const
	{
		empty_string = "";
		size_t cnt = Super::count();
		if (!cnt)
			return empty_string;
		const String*result = &Super::first();
		size_t len = result->length();
		for (size_t i = 1; i < cnt; i++)
		{
			const String&str = Super::at(i);
			if (str.length() > len)
			{
				result = &str;
				len = str.length();
			}
		}
		return *result;
	}

	String& StringList::longest()
	{
		empty_string = "";
		size_t cnt = Super::count();
		if (!cnt)
			return empty_string;
		String*result = &Super::first();
		size_t len = result->length();
		for (size_t i = 1; i < cnt; i++)
		{
			String&str = Super::at(i);
			if (str.length() > len)
			{
				result = &str;
				len = str.length();
			}
		}
		return *result;
	}

	String& StringList::shortest()
	{
		empty_string = "";
		size_t cnt = Super::count();
		if (!cnt)
			return empty_string;
		String*result = &Super::first();
		size_t len = result->length();
		for (size_t i = 1; i < cnt; i++)
		{
			String&str = Super::at(i);
			if (str.length() < len)
			{
				result = &str;
				len = str.length();
			}
		}
		return *result;
	}

	const String& StringList::shortest()	const
	{
		empty_string = "";
		size_t cnt = Super::count();
		if (!cnt)
			return empty_string;
		const String*result = &Super::first();
		size_t len = result->length();
		for (size_t i = 1; i < cnt; i++)
		{
			const String&str = Super::at(i);
			if (str.length() < len)
			{
				result = &str;
				len = str.length();
			}
		}
		return *result;
	}

	String& StringList::max()
	{
		empty_string = "";
		size_t cnt = Super::count();
		if (!cnt)
			return empty_string;
		String*result = &Super::first();
		for (size_t i = 1; i < cnt; i++)
		{
			String&str = Super::at(i);
			if (str > *result)
				result = &str;
		}
		return *result;
	}

	const String& StringList::max()	const
	{
		empty_string = "";
		size_t cnt = Super::count();
		if (!cnt)
			return empty_string;
		const String*result = &Super::first();
		for (size_t i = 1; i < cnt; i++)
		{
			const String&str = Super::at(i);
			if (str > *result)
				result = &str;
		}
		return *result;
	}

	String& StringList::min()	
	{
		empty_string = "";
		size_t cnt = Super::count();
		if (!cnt)
			return empty_string;
		String*result = &Super::first();
		for (size_t i = 1; i < cnt; i++)
		{
			String&str = Super::at(i);
			if (str < *result)
				result = &str;
		}
		return *result;
	}

	const String& StringList::min()		const
	{
		empty_string = "";
		size_t cnt = Super::count();
		if (!cnt)
			return empty_string;
		const String*result = &Super::first();
		for (size_t i = 1; i < cnt; i++)
		{
			const String&str = Super::at(i);
			if (str < *result)
				result = &str;
		}
		return *result;
	}

	String& StringList::first()
	{
		if (IsNotEmpty())
			return Super::first();
		empty_string = "";
		return empty_string;
	}

	const String& StringList::first()													 const
	{
		if (IsNotEmpty())
			return Super::first();
		empty_string = "";
		return empty_string;
	}

	String& StringList::last()
	{
		if (IsNotEmpty())
			return Super::last();
		empty_string = "";
		return empty_string;
	}

	const String& StringList::last()													  const
	{
		if (IsNotEmpty())
			return Super::last();
		empty_string = "";
		return empty_string;
	}


	bool  StringList::isSorted() const
	{
		return Sort<OperatorSort>::isSorted(Super::pointer(),Super::Count());
	}

	void StringList::radixSort()
	{
		size_t iterations = longest().length();

		SBucket *first[0x100],*last[0x100],*buckets = alloc<SBucket>(count());
		memset(last,0,sizeof(last));
		for (size_t i = iterations-1; i <= iterations; i--)
		{
			SBucket*bucket = buckets;
			for (index_t i = 0; i < Super::count(); i++)
			{
				String&str = Super::operator[](i);
				BYTE c = (BYTE)(i <= str.length()?str.get(i-1):0);
				SBucket*&l = last[c];
				if (!l)
				{
					l = first[c] = bucket++;
					l->entry = &str;
				}
				else
				{
					l->next = bucket++;
					l = last[c]->next;
					l->entry = &str;
				}
			}

			Super::iterator it = begin();
			for (size_t j = 0; j < 0x100; j++)
				if (last[j])
				{
					SBucket*b = first[j];
					while (b != last[j])
					{
						(*it++) = *b->entry;
						b = b->next;
					}
					(*it++) = *b->entry;
					last[j] = NULL;
				}
			if (it != end())
				FATAL__("radix-error");
		}
		dealloc(buckets);
	}



	void StringList::mergeSort()
	{
		Sort<OperatorSort>::mergeSortField(Super::pointer(),Super::Count());
	}

	void StringList::bubbleSort()
	{
		Sort<OperatorSort>::bubbleSortField(Super::pointer(),Super::Count());
	}

	void StringList::quickSort()
	{
		if (count() > 1)
			ByOperator::quickSort<String*,SwapStrategy>(pointer(), 0,count()-1);
	}

	void StringList::heapSort()
	{
		Sort<OperatorSort>::heapSortField(Super::pointer(),Super::Count());
	}



	size_t StringList::lookup(const String&line)								  const
	{
		size_t rs = lookupCaseSensitive(line);
		if (!rs)
			rs = lookupCaseIgnore(line);
		return rs;
	}

	size_t StringList::lookupCaseSensitive(const String&line)					 const
	{
			size_t	lower = 0,
					num = count(),
					upper = num;
			const char*l = line.c_str();
			while (lower< upper && upper <= num)
			{
				size_t el = (lower+upper)/2;
				const String&element = Super::at(el);
				char cmp = strcmp(element.c_str(),l);
				if (cmp > 0)
					upper = el;
				else
					if (cmp < 0)
						lower = el+1;
					else
						return el+1;
			}
			return 0;
	}

	size_t StringList::lookupCaseIgnore(const String&line)						const
	{
			size_t lower = 0,
					 num = count(),
					 upper = num;
			const char*l = line.c_str();
			while (lower< upper && upper <= num)
			{
				size_t el = (lower+upper)/2;
				const String&element = Super::at(el);
				char cmp = strcmpi(element.c_str(),l);
				if (cmp > 0)
					upper = el;
				else
					if (cmp < 0)
						lower = el+1;
					else
						return el+1;
			}
			return 0;
	}
//}


