#ifndef tokenizerH
#define tokenizerH

#include "../global_string.h"
//#include "../container/lvector.h"
#include "../container/buffer.h"
#include "../container/sorter.h"



MAKE_SORT_CLASS(LengthSort, length);

struct TToken
{
	count_t			ident,  index,
					line_offset,char_offset;	//not defined by tokenizer - for custom application usage
	String			content;


	void			swap(TToken&other)
					{
						swp(ident,other.ident);
						swp(index,other.index);
						swp(line_offset,other.line_offset);
						swp(char_offset,other.char_offset);
						content.swap(other.content);
					}
};
DECLARE__(TToken,Swappable);

typedef Buffer<TToken,0,SwapStrategy>	TokenList;


class StringTokenizer
{
public:
	struct Pattern
	{
		String			pattern;
		count_t			ident,
						length;
		bool			pre_open,post_open;

		void			swap(Pattern&other)
						{
							pattern.swap(other.pattern);
							swp(ident,other.ident);
							swp(length,other.length);
							swp(pre_open,other.pre_open);
							swp(post_open,other.post_open);
						}
		int				compareTo(const Pattern&other)
						{
							if (length > other.length)
								return -1;
							if (length < other.length)
								return 1;
							return 0;
						}

	};
	typedef Buffer<Pattern,0,SwapStrategy>	PList;
private:
	PList				list[0x100],
						open[0x100];
	count_t				string_ident;
	bool				sorted;
		
	static void			finish(char*root,const Pattern&p, TokenList&out);
	void				appendStringElement(char*root, char*current, TokenList&out)	const;
	static bool			match(char*root,char*end,const Pattern&p);
	static inline bool	openChar(char c)
						{
							return !isalnum(c) && c != '_';
						}
		
public:
	/**/				StringTokenizer();
	virtual				~StringTokenizer();
	void				clear();
	void				reg(const String&pattern, unsigned ident, bool left_open=false, bool right_open=false);
	void				regWord(const String&pattern, unsigned ident);
	void				setStringIdent(unsigned ident);
	void				parse(const String&string, TokenList&target)	const;
	void				parse(Array<char>&field, TokenList&target)	const;
	String				status();
		
	void				finalizeInitialization();
	bool				isFinalized()		const;
};



#endif
