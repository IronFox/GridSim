#include "../global_root.h"
#include "tokenizer.h"

namespace DeltaWorks
{

	StringTokenizer::StringTokenizer():string_ident(0),sorted(true)
	{}

	StringTokenizer::~StringTokenizer()
	{
		clear();
	}



	void        StringTokenizer::clear()
	{
		for (index_t i = 0; i < ARRAYSIZE(list); i++)
		{
			list[i].reset();
			open[i].reset();
		}
		sorted = true;
	}

	void        StringTokenizer::reg(const String&pattern, unsigned ident, bool left_open, bool right_open)
	{
		if (!pattern.length())
			return;
		Pattern	p;
		p.ident = ident;
		p.pre_open = left_open;
		p.post_open = right_open;
		p.pattern = pattern;
		p.length = p.pattern.length();
		if (p.pre_open)
			open[(BYTE)pattern.FirstChar()] << p;
		else
		{
			list[(BYTE)pattern.FirstChar()] << p;
	//        ShowMessage(String((BYTE)pattern[1])+" is now "+String(list[(BYTE)pattern[1]]));
		}
		sorted = false;
	}

	void        StringTokenizer::regWord(const String&pattern, unsigned ident)
	{
		reg(pattern,ident,true,true);
	}

	void        StringTokenizer::setStringIdent(unsigned ident)
	{
		string_ident = ident;
	}


	void StringTokenizer::finish(char*root,const Pattern&p, TokenList&out)
	{
		TToken&t = out.append();
		char old = root[p.length];
		root[p.length] = 0;
		t.ident = p.ident;
		t.content = root;
		t.index = out.length()-1;
		root[p.length] = old;
	}

	void StringTokenizer::appendStringElement(char*root, char*current, TokenList&out)	const
	{
		char old = *current;
		*current = 0;

		TToken&t = out.append();
		t.index = out.length()-1;
		t.ident = string_ident;
		t.content = root;
		*current = old;
	}

	/*static*/ bool StringTokenizer::match(char*root,char*end,const Pattern&p)
	{
		if ((count_t)(end-root) < p.length)
			return false;
		if (strncmp(root,p.pattern.c_str(),p.length))
			return false;
		if (!p.post_open)
			return true;
		return openChar(root[p.length]);
	}

	void StringTokenizer::finalizeInitialization()
	{
		if (!sorted)
		{
			for (index_t i = 0; i < ARRAYSIZE(list); i++)
			{
				Sorting::ByMethod::quickSort(list[i]);
	//            list[i].revert();
			}
			for (index_t i = 0; i < ARRAYSIZE(open); i++)
			{
				Sorting::ByMethod::quickSort(open[i]);
				//Sort<LengthSort>::quickSortRAL(open[i]);
				//open[i].revert();
			}
			sorted = true;
		}
	}    

	bool	StringTokenizer::isFinalized()	const
	{
		return sorted;
	}

	void        StringTokenizer::parse(const String&string, TokenList&out)	const
	{
		if (!sorted)
			FATAL__("Please call finalize() before parsing");
		size_t len = string.length();
		if (!len)
			return;
        

		Ctr::Array<char>	field(string.c_str(),len+1);
		parse(field,out);
	}

	void	StringTokenizer::parse(Ctr::Array<char>&field,TokenList&out)	const
	{
		char  *current = field.pointer(),
			  *end = field+field.length()-1;
		if (*end != 0)
			end++;
          
		bool last_open = true;
		char  *string_first = current;
    
		while (current<end)
		{
			bool    alpha = !openChar(*current),
					found = false;
			if (last_open)
			{
				const PatternList&patterns = open[(BYTE)*current];
				for (index_t i = 0; i < patterns.Count(); i++)
				{
					const Pattern&p = patterns[i];
					if (match(current,end,p))
					{
	//                    cout << "found matching open pattern: '"<<p->pattern.c_str()<<"'\n";
                    
						if (string_first != current)
							appendStringElement(string_first,current,out);
						finish(current,p,out);
						current+=p.length;
						string_first = current;
						last_open = openChar(*(current-1));
						found = true;
	//                    cout << "current now: '"<<current<<"'\n";
						break;
					}
				}
			}
			if (!found)
			{
				const PatternList&patterns = list[(BYTE)*current];
				for (unsigned i = 0; i < patterns.Count(); i++)
				{
					const Pattern&p = patterns[i];
					if (match(current,end,p))
					{
	//                    cout << "found matching pattern ('"<<(char)*current<<"'): '"<<p->pattern.c_str()<<"'\n";
						if (string_first != current)
							appendStringElement(string_first,current,out);
						finish(current,p,out);
						current+=p.length;
						string_first = current;
						last_open = openChar(*(current-1));
						found = true;
	//                    cout << "current now: '"<<current<<"'\n";
						break;
					}
				}
			}
			if (!found)
			{
				last_open = !alpha;
				current++;
			}
		}
		if (string_first < end)
			appendStringElement(string_first,end,out);
	}

	String     StringTokenizer::status()
	{
		finalizeInitialization();
		String rs;
		for (index_t i = 0; i < ARRAYSIZE(list); i++)
		{
			rs+="row "+String(i)+" ('"+(char)i+"'): ";
			for (index_t j = 0; j < list[i].length(); j++)
			{
				const Pattern&p = list[i][j];
				rs+="'"+p.pattern+"'("+String(p.length)+") ";
			}
			rs+="\n";
		}
		for (index_t i = 0; i < ARRAYSIZE(open); i++)
		{
			rs+="open row "+String(i)+" ('"+(char)i+"'): ";
			for (index_t j = 0; j < list[i].length(); j++)
			{
				const Pattern&p = open[i][j];
				rs+="'"+p.pattern+"'("+String(p.length)+") ";
			}
			rs+="\n";
		}
		return rs;
	}
}
