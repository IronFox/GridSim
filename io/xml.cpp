#include "../global_root.h"
#include "xml.h"


/******************************************************************

E:\include\io\xml.cpp

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

#define IS_WHITESPACE(c)	((c)==' ' || (c)=='\t' || (c)=='\n' || (c)=='\r')

//#include "log.h"

//static Log	log_file("xml.log",true);

static char* findCharacter(char*from, char c)
{
	while ((*from) && (*from)!=c)
		from++;
	if (!*from)
		return NULL;
	return from;
}

static String getWord(char*from)
{
	while (IS_WHITESPACE(*from))
		from++;
	char*c = from,quote;
	bool in_string(false);
	while (*c)
	{
		if (*c == '\"' || *c == '\'')
		{
			if (!in_string)
			{
				in_string = true;
				quote = *c;
			}
			else
			{
				in_string = (*c) != quote;
				if (!in_string)
				{
					c++;
					break;
				}
			}
		}
		else
		{
			if ((IS_WHITESPACE(*c) || (*c) == '>' || (*c) == '=') && !in_string)
				break;
		}
		c++;
	}
	char temp = *c;
	(*c) = 0;
	String rs = from;
	(*c) = temp;
	return rs;
}

static unsigned findWord(char*from)
{
	char*c = from,quote;
	while (IS_WHITESPACE(*c))
		c++;
	bool in_string(false),
			found_word(false);
	while (*c)
	{
		if (*c == '\"' || *c == '\'')
		{
			if (!in_string)
			{
				in_string = true;
				quote = *c;
			}
			else
			{
				in_string = (*c) != quote;
				if (!in_string)
				{
					found_word = true;
					c++;
					break;
				}
			}
			found_word = true;
		}
		else
		{
			if ((IS_WHITESPACE(*c) || (*c) == '>' || (*c) == '=' /*|| (*c) == '/'*/) && !in_string)
				break;
			found_word = true;
		}
		c++;
	}
	if (!*c || !found_word)
		return 0;
	return c-from;
}


static String encode(const String&content)
{
	ASSERT_EQUAL1__(content.length(),strlen(content.c_str()),content.c_str());
	
	String rs = content;
	for (unsigned i = 0; i < rs.length(); i++)
	{
		char c = rs.get(i);
		switch (c)
		{
			case '&':
				rs.insert(i+1,"amp;");
			break;
			case '<':
				rs.erase(i,1);
				rs.insert(i,"&lt;");
			break;
			case '>':
				rs.erase(i,1);
				rs.insert(i,"&gt;");
			break;
			case '\'':
			{
				rs.erase(i,1);
				rs.insert(i,"&apos;");
			}
			break;
			case '\"':
				rs.erase(i,1);
				rs.insert(i,"&quot;");
			break;
		}
	}
	ASSERT_EQUAL__(rs.length(), strlen(rs.c_str()));
	return rs;
}

static String decode(const String&content)
{
	if (content.length() < 4)
		return content;
	String rs = content;
	//log_file << "in: "<<content<<nl;
	for (unsigned i = 0; i+3 < rs.length(); i++)
		if (rs.get(i) == '&')
		{
			unsigned end=0;
			//log_file << i+1<<"->"<<rs.length()<<nl;
			for (end = i+1; end < rs.length() && rs.get(end) != ';'; end++);
			
			if (end >= rs.length() || end-i>6)
				break;
			String sub = rs.subString(i+1,end-i);
			rs.erase(i,end-i+1);
			char replace(0);
			if (sub == "amp;")
				replace='&';
			elif (sub == "lt;")
				replace='<';
			elif (sub == "gt;")
				replace='>';
			elif (sub == "quot;")
				replace='\"';
			elif (sub == "apos;")
				replace='\'';
			if (replace)
			{
				//log_file << "replace ("<<String(replace)<<")"<<nl;
				rs.insert(i,replace);
				//log_file << "replaced"<<nl;
			}
			//log_file << i<<"/"<<rs.length()<<nl;
		}
	//log_file << "out: "<<rs<<nl;
	return rs;
}

/*
XML::Node::Node(const Node&other):name(other.name),inner_content(other.inner_content),following_content(other.following_content),parent(NULL)
{
	for (unsigned i = 0; i < other.children; i++)
		children.append(SHIELDED(new Node(*other.children[i])))->parent = this;
	attributes = other.attributes;
}*/

void		XML::Node::swap(Node&other)
{
	name.swap(other.name);
	inner_content.swap(other.inner_content);
	following_content.swap(other.following_content);
	//swp(parent,other.parent);
	children.swap(other.children);
	attributes.swap(other.attributes);
}



XML::Node&		XML::Node::newChild(const String&name)
{
	Node&rs = children.append();
	rs.name = name;
	return rs;
}

XML::TAttribute*	XML::Node::set(const String&parameter_name, const String&parameter_value)
{
	TAttribute* p = attributes.append(parameter_name);
	p->name = parameter_name;
	p->value = parameter_value;
	return p;
}


void				XML::Node::unset(const String&parameter_name)
{
	attributes.erase(parameter_name);
}
/*
String			XML::Node::path()	const
{
	if (!this)
		return "NULL";
	String result = name;
	const XML::Node*at = parent;
	while (at)
	{
		result = at->name+'/'+result;
		at = at->parent;
	}
	return result;
}
*/

bool			XML::Node::query(const String&attrib_name, String&val_out) const
{
	if (!this)
		return false;
	if (const TAttribute*attrib = attributes.lookup(attrib_name))
	{
		val_out = attrib->value;
		return true;
	}
	return false;
}

bool			XML::Node::query(const char*attrib_name, String&val_out)		const
{
	if (!this)
		return false;
	if (const TAttribute*attrib = attributes.lookup(attrib_name))
	{
		val_out = attrib->value;
		return true;
	}
	return false;
}


/*
bool			XML::Node::query(const String&attrib_name, String&val_out) const
{
	if (!this)
		return false;
	if (const TAttribute*attrib = attributes.lookup(attrib_name))
	{
		val_out = attrib->value;
		return true;
	}
	return false;
}*/

void	XML::Container::clear()
{
	root_node.name = "root";
	root_node.inner_content = "";
	root_node.following_content = "";
	root_node.children.clear(0);
	root_node.attributes.clear();
}

void	XML::Container::loadFromCharArray(ArrayData<char>&field)
{
	Buffer<Node*,4>	parse_stack;

	bool in_block(false),/*in_string(false),*/open(false);
	
	Node	*active_entry = NULL,
			*last(NULL);
	
	char	*c = field.pointer(),
			*last_c=NULL;
	while (*c)
	{
		ASSERT__(c!=last_c);
		last_c = c;
		char*next = strchr(c,'<');
		if (!next)
		{
			if (!active_entry)
				return;
			throw IO::DriveAccess::FileFormatFault("XML: Did not return to root level at end of file");
		}
			
		{
			*next = 0;
			if (open)
			{
				if (active_entry)
					active_entry->inner_content += decode(c);
			}
			else
				if (last)
					last->following_content += decode(c);
			*next = '<';
		}
		c = next;

		c++;

		if (*c == '?')
		{
			c = strchr(c,'>');
			if (!c)
				throw IO::DriveAccess::FileFormatFault(globalString("XML: Cannot find enclosing '>' character to '<?...'"));
			c++;
		}
		elif (c[0] == '!' && c[1] == '-' && c[2] == '-')
		{
			c = strstr(c+3,"-->");
			if (!c)
				throw IO::DriveAccess::FileFormatFault(globalString("XML: Cannot find enclosing '-->' character to '<!--...'"));

			c+=3;
		}
		elif (*c == '/')
		{
			String name = getWord(c+1);
			last = active_entry;
			if (active_entry && name == active_entry->name)
				active_entry = parse_stack.pop();

			c = strchr(c,'>');
			if (!c)
				throw IO::DriveAccess::FileFormatFault(globalString("XML: Cannot find enclosing '>' character to '</...'"));

			c++;
			open = false;
		}
		else
		{
			Node		*sub = active_entry?&active_entry->children.append():&root_node;
			//sub->parent = active_entry;
			parse_stack << active_entry;
			active_entry = sub;
			active_entry->name = getWord(c);
//			ShowMessage("opening "+active_entry->name);
			in_block = true;
			c += active_entry->name.length();
			open = true;
			while (unsigned len = findWord(c))
			{
				String pname = getWord(c);
				c += len;
				if (pname == "/")
				{
					last = active_entry;
					active_entry = parse_stack.pop();//active_entry->parent;
//					ShowMessage("breaking down");
					open = false;
					break;
				}
				c = strchr(c,'=');
				if (!c)
					throw IO::DriveAccess::FileFormatFault("XML: Cannot find assignment operator '=' for attribute '"+pname+"'");

				c++;
				len = findWord(c);
				if (!len)
					throw IO::DriveAccess::FileFormatFault("XML: Missing attribute value of attribute '"+pname+"'");

				String pvalue = getWord(c);
				c += len;
				if (pvalue.firstChar() == '\"' || pvalue.firstChar() == '\'')
					pvalue.erase(0,1);
				if (pvalue.length() && (pvalue.lastChar() == '\"' || pvalue.lastChar() == '\''))
					pvalue.erase(pvalue.length()-1);
				active_entry->set(pname,decode(pvalue));
//				ShowMessage("specifying parameter "+pname+" = "+pvalue);
			}
			c = strchr(c,'>');
			if (!c)
				throw IO::DriveAccess::FileFormatFault(globalString("XML: Cannot find enclosing '>' character to '<...'"));

			if (*(c-1) == '/' && open)
			{
				last = active_entry;
				if (active_entry)
					active_entry = parse_stack.pop();//active_entry->parent;
//					ShowMessage("breaking down");
				open = false;
			}
			c++;
		}
	}

}

void	XML::Container::loadFromString(const String&content)
{
	clear();
	
	Array<char>	field(content.c_str(),content.length()+1);
	size_t len = content.length();
	
	field[len] = 0;
	
	loadFromCharArray(field);
}

void	XML::Container::loadFromFile(const String&filename, String*content_out)
{
	FILE*f = fopen(filename.c_str(),"rb");
	if (!f)
		throw IO::DriveAccess::FileOpenFault("XML: Unable to open file '"+filename+"'");

	clear();
	fseek(f,0,SEEK_END);
	unsigned len = ftell(f);
	fseek(f,0,SEEK_SET);
	
	Array<char>	field;
	field.setSize(len+1);
	
	if (fread(field.pointer(),1,len,f)!=len)
	{
		fclose(f);

		throw IO::DriveAccess::DataReadFault("XML: Unable to read data from file");
	}
	field[len] = 0;
	fclose(f);
	
	if (content_out)
		(*content_out) = field;
	
	loadFromCharArray(field);
}

inline static bool isNewLine(char c)
{
	return c == '\n' || c == '\r';
}

inline static void trim(Array<String,Adopt>&field)
{
	for (unsigned i = 0; i < field.length(); i++)
	{
		field[i].trimThis();
		if (!field[i].length())
			field.erase(i--);
	}
}

template <class OutStream>
static void writeToStream(OutStream&outfile, XML::Node		*entry, XML::export_style_t style, unsigned indent=0)
{
	entry->name.trimThis();
	if (entry->name.isEmpty())
		throw IO::StructureCompositionFault("XML: Local entry is empty");

	if (style==XML::Nice)
		outfile << tabSpace(indent);
	outfile << "<" << entry->name;
	entry->attributes.reset();
	while (XML::TAttribute*p = entry->attributes.each())
	{
		if (style != XML::Tidy || p == entry->attributes.first())
			outfile << " ";
		outfile << p->name<<"=\""<<encode(p->value)<<"\"";
	}
	if (entry->children.isEmpty() && entry->inner_content.isEmpty())
	{
		if (style != XML::Tidy)
			outfile << " /";
		else
			outfile << '/';
	}
	outfile << ">";
	//if (style==XML::Raw || !entry->children)
	{
		switch (style)
		{
			case XML::Raw:
				outfile << encode(entry->inner_content);
			break;
			case XML::Nice:
			{
				Array<String,Adopt>	lines;
				explodeCallback(isNewLine,entry->inner_content,lines);
				trim(lines);
				if (lines.count() > 1)
				{
					outfile << nl;
					for (unsigned i = 0; i < lines.count(); i++)
						outfile << tabSpace(indent+1)<<encode(lines[i])<<nl;
					if (entry->children.isEmpty())
						outfile << tabSpace(indent);
				}
				else
				{
					if (lines.isNotEmpty())
						outfile << encode(lines.first());
					if (entry->children.isNotEmpty())
						outfile << nl;
				}
			}
			break;
			case XML::Tidy:
			{
				Array<String,Adopt>	lines;
				explodeCallback(isNewLine,entry->inner_content,lines);
				trim(lines);
				for (unsigned i = 0; i < lines.count(); i++)
				{
					if (i)
						outfile << nl;
					outfile << lines[i];
				}
			}
			break;
		}
	}

	for (index_t i = 0; i < entry->children.count(); i++)
		writeToStream(outfile,entry->children +i,style,indent+1);
	
	if (entry->children.isNotEmpty() || entry->inner_content.isNotEmpty())
	{
		if (style==XML::Nice && entry->children.isNotEmpty())
			outfile << tabSpace(indent);
		outfile << "</" << entry->name << ">";
	}

	{
		switch (style)
		{
			case XML::Raw:
				outfile << encode(entry->following_content);
			break;
			case XML::Nice:
			{
				Array<String,Adopt>	lines;
				explodeCallback(isNewLine,entry->following_content,lines);
				trim(lines);
				outfile << nl;
				for (unsigned i = 0; i < lines.count(); i++)
					outfile << tabSpace(indent)<<encode(lines[i])<<nl;
			}
			break;
			case XML::Tidy:
			{
				Array<String,Adopt>	lines;
				explodeCallback(isNewLine,entry->inner_content,lines);
				trim(lines);
				for (unsigned i = 0; i < lines.count(); i++)
				{
					if (i)
						outfile << nl;
					outfile << lines[i];
				}
			}
			break;
		}
	}
}

void				XML::Container::saveToStringBuffer(StringBuffer&target, export_style_t style)
{
	writeToStream(target,&root_node,style);
}

void				XML::Container::saveToFile(const String&filename, export_style_t style)
{
	StringFile outfile;
	if (!outfile.create(filename))
		throw IO::DriveAccess::FileOpenFault("XML: Unable to open file '"+filename+"' for output");

	
	outfile << "<?xml version=\"1.0\" encoding=\""<<encoding<<"\" ?>"<<nl;
	writeToStream(outfile,&root_node, style);
}

static XML::Node*	findIn(BasicBuffer<XML::Node,SwapStrategy>&children, const String&path)
{
	String local,sub;
	index_t p = path.indexOf('/');
	if (p)
	{
		local = path.subString(0,p-1);
		sub = path.subString(p);
	}
	else
		local = path;

	//cout << "searching for local='"<<local<<"' among "<<children.count()<<" children"<<endl;
	XML::Node*result;
	for (index_t i = 0; i < children.count(); i++)
		if (children[i].name == local)
		{
			if (sub.isEmpty())
				return children+i;
			return findIn(children[i].children,sub);
		}
	return NULL;
}

static const XML::Node*	findInConst(const BasicBuffer<XML::Node,SwapStrategy>&children, const String&path)
{
	String local,sub;
	index_t p = path.indexOf('/');
	if (p)
	{
		local = path.subString(0,p-1);
		sub = path.subString(p);
	}
	else
		local = path;

	//cout << "searching for local='"<<local<<"' among "<<children.count()<<" children"<<endl;
	XML::Node*result;
	for (index_t i = 0; i < children.count(); i++)
		if (children[i].name == local)
		{
			if (sub.isEmpty())
				return children+i;
			return findInConst(children[i].children,sub);
		}
	return NULL;
}

static XML::Node*	findFrom(XML::Node*context, const String&path)
{
	String local,sub;
	index_t p = path.indexOf('/');
	if (p)
	{
		local = path.subString(0,p-1);
		sub = path.subString(p);
	}
	else
		local = path;

	if (context->name != local)
	{
		//cout << "context name is '"<<context->name<<"', local is '"<<local<<"'"<<endl;
		return NULL;
	}
	if (sub.isEmpty())
		return context;
	//cout << "delegating search to "<<context->children.count()<<" children using sub path '"<<sub<<"'"<<endl;
	return findIn(context->children,sub);
}

static const XML::Node*	findFromConst(const XML::Node*context, const String&path)
{
	String local,sub;
	index_t p = path.indexOf('/');
	if (p)
	{
		local = path.subString(0,p-1);
		sub = path.subString(p);
	}
	else
		local = path;

	if (context->name != local)
		return NULL;
	if (sub.isEmpty())
		return context;
	return findInConst(context->children,sub);
}

static XML::Node&	createIn(XML::Node*context, const String&path, const String&inner_content)
{
	ASSERT_NOT_NULL__(context);
	Array<String,Adopt>	components;
	explode('/',path,components);
	if (!components.count())
		throw IO::ParameterFault("Trying to create empty XML segment");

	for (index_t i = 0; i < components.count()-1; i++)
	{
		index_t next = InvalidIndex;
		for (index_t j = 0; j < context->children.count() && next == InvalidIndex; j++)
			if (context->children[j].name == components[i])
				next = j;

		if (next == InvalidIndex)
		{
			context = &context->children.append();
			context->name = components[i];
		}
		else
			context = &context->children[next];
	}
	ASSERT_NOT_NULL__(context);
	context = &context->children.append();
	context->name = components.last();
	context->inner_content = inner_content;
	return *context;
}




XML::Node* XML::Node::find(const String&path)
{
	return findIn(children,path);
}

const XML::Node* XML::Node::find(const String&path) const
{
	return findInConst(children,path);
}


XML::Node*	XML::Container::find(const String&path)
{
	return findFrom(&root_node,path);
}

const XML::Node*	XML::Container::find(const String&path) const
{
	return findFromConst(&root_node,path);
}


XML::Node&	XML::Node::create(const String&path, const String&inner_content)
{
	ASSERT_NOT_NULL__(this);	//kinda dumb, no?
	return createIn(this, path, inner_content);
}
XML::Node&	XML::Container::create(const String&path, const String&inner_content)
{
	String local,sub;
	index_t p = path.indexOf('/');
	if (p)
	{
		local = path.subString(0,p-1);
		sub = path.subString(p);
	}
	else
		local = path;
	
	if (root_node.name=="xml")	//!< empty name
	{
		root_node.name = local;
	}
	elif (root_node.name != local)
		throw IO::ParameterFault("Path component mismatch: '"+local+"'");

	if (sub.isEmpty())
		return root_node;

	return createIn(&root_node, sub, inner_content);
}


XML::ScannerRule::ScannerRule(const String&name_, Scanner*parent_, bool want_content_, pNodeCallback on_enter_, pNodeCallback on_exit_):
								parent(parent_),name(name_),want_content(want_content_),on_enter(on_enter_),on_exit(on_exit_)	{}
								
XML::ScannerRule*	XML::ScannerRule::map(const String&name, bool want_content,pNodeCallback on_enter, pNodeCallback on_exit)
{
	ScannerRule*result;
	if (connections.query(name,result))
	{
		result->want_content = want_content;
		result->on_enter = on_enter;
		result->on_exit = on_exit;
		return result;
	}
	result = parent->rules.append(SHIELDED(new ScannerRule(name,parent,want_content,on_enter,on_exit)));
	connections.set(name,result);
	return result;
}

XML::ScannerRule*	XML::ScannerRule::map(ScannerRule*node)
{
	connections.set(node->name,node);
	return node;
}

bool	XML::ScannerRule::unMap(const String&name)
{
	if (connections.isSet(name))
	{
		connections.unSet(name);
		return true;
	}
	return false;
}

bool	XML::ScannerRule::unMap(ScannerRule*node)
{
	if (connections.isSet(node->name))
	{
		connections.unSet(node->name);
		return true;
	}
	return false;
}
	
	
bool	XML::Scanner::read(char until, StringBuffer&to, FILE*f)
{
	to.reset();
	while (true)
	{
		if (buffer_fill_state)
		{
			char*offset = strnchr(buffer,buffer_fill_state,until);
			if (offset)
			{
				to.Write(buffer,offset-buffer);
				memcpy(buffer,offset+1,buffer_fill_state-(offset-buffer)-1);
				buffer_fill_state -= (offset-buffer)+1;
				return true;
			}
			else
			{
				to.Write(buffer,buffer_fill_state);
				buffer_fill_state = 0;
			}
		}
		buffer_fill_state = fread(buffer,1,sizeof(buffer),f);
		if (!buffer_fill_state)
			return false;
	}
	return false;
}


bool	XML::Scanner::skip(char until, FILE*f)
{
	while (true)
	{
		if (buffer_fill_state)
		{
			char*offset = strnchr(buffer,buffer_fill_state,until);
			if (offset)
			{
				memcpy(buffer,offset+1,buffer_fill_state-(offset-buffer)-1);
				buffer_fill_state -= (offset-buffer)+1;
				return true;
			}
		}
		buffer_fill_state = fread(buffer,1,sizeof(buffer),f);
		if (!buffer_fill_state)
			return false;
	}
	return false;
}

bool	XML::Scanner::skip(const char*until, FILE*f)
{
	size_t len = strlen(until);
	while (true)
	{
		if (buffer_fill_state)
		{
			char*offset = strnstr(buffer,until,buffer_fill_state),
				*first = strnchr(buffer,buffer_fill_state,until[0]);
			if (offset)
			{
				memcpy(buffer,offset+1,buffer_fill_state-(offset-buffer)-1);
				buffer_fill_state -= (offset-buffer)+1;
				return true;
			}
			elif (first && (unsigned)(first-buffer) > (unsigned)(buffer_fill_state-len))
			{
				memcpy(buffer,first,buffer_fill_state-(first-buffer)-1);
				buffer_fill_state -= (first-buffer)+1;
			}
			else
				buffer_fill_state = 0;
		}
		size_t delta = fread(buffer+buffer_fill_state,1,sizeof(buffer)-buffer_fill_state,f);
		buffer_fill_state += delta;
		if (!delta)
			return false;
	}
	return false;
}

XML::Scanner::Scanner():ScannerRule("xml:document",this,false,NULL,NULL),except(NULL),buffer_fill_state(0)
{}


const String&	XML::Scanner::errorStr()						const
{
	return error_string;
}

const String&	XML::Scanner::getError()						const
{
	return error_string;
}


void	XML::Scanner::setExcept(pNodeCallback callback_on_except)
{
	except = callback_on_except;
}

bool	XML::Scanner::scan(const String&filename)
{
	FILE*f = fopen(filename.c_str(),"rb");
	if (!f)
	{
		error_string = "Unable to open file '"+filename+"'";
		error_is_filenotfound = true;
	}
	error_is_filenotfound = false;
	
	List::Vector<TScannerNode>	stack;
	TScannerNode*current = stack.append();
	current->parent = NULL;
	current->rule = this;
	ScannerRule*rule = this;
	
	StringBuffer	buffer;
	bool 	error = false,
			working = true;
	
	if (!skip('<',f))
	{
		error = true;
		working = false;
		error_string = "Empty or non XML file.";
	}	
	while (working)
	{
		//cursor is now behind the '<'-character of the new tag.
		if (!read('>',buffer,f) || !buffer.length())	//read out all data until the end of the tag
		{
			error = true;
			working = false;
			error_string = "Unexpected end of file.";
		}
		
		char*field = buffer.pointer(),						//extract working buffer
			*c = field,
			*end = field+buffer.length();
		while (!IS_WHITESPACE(*c) && c != end)	//find end of name
			c++;
		unsigned name_len = c-field;
		field[name_len] = 0;	//zero terminate name
		if (field[0] == '?')	//skip xml header tags for now
		{
			if (!skip('<',f))	//go to next tag
			{
				error = stack.count()!=1;
				working = false;
				error_string = "Unexpected end of file.";
			}
			continue;
		}
		elif (field[0] == '!' && field[1] == '-' && field[2] == '-')
		{
			if (!skip("-->",f))
			{
				error = stack.count() != 1;
				working = false;
				error_string = "Unexpected end of file.";
			}
		}
			
		if (field[0] == '/')	//closing tag
		{
			String tag_name = field+1;
			if (current->rule && tag_name != current->rule->name)	//tag hierarchy broken
			{
				error_string = "Closing tag '"+tag_name+"' does not match opening tag '"+current->rule->name+"' of this level";
				error = true;
				working = false;
				continue;
			}
			if (rule && rule->on_exit)
				rule->on_exit(current);		//execute rule
			stack.erase(stack.count()-1);	//pop top stack element
			current = stack.last();
			rule = current->rule;
			
			
			if (!skip('<',f))
			{
				error = stack.count()!=1;
				working = false;
				error_string = "Unexpected end of file.";
			}
			
			continue;
		}
		
		String tag_name = field;
		ScannerRule*next=NULL;
		if (rule)
			rule->connections.query(tag_name,next);
		TScannerNode*inner = stack.append();
		inner->parent = current;
		inner->rule = next;
		//read attributes
		char*name_offset = field+name_len+1;
		if (next && name_offset < end)
			while (char*attrib = strnchr(name_offset,end-name_offset,'='))
			{
				while (IS_WHITESPACE(*name_offset))
					name_offset++;
				(*attrib) = 0;
				attrib++;
				field[buffer.length()] = 0;
				char quote = *attrib;
				if ((quote != '\"' && quote != '\'') || attrib-name_offset<2)
				{
					error_string = "Attribute '"+String(name_offset)+"' corrupted ('\"' or '\'' expected but '"+String(*attrib)+"' encountered).";
					working = false;
					error = true;
					break;
				}
				char*data_offset = ++attrib;
				while ((*attrib)!=quote && attrib != end)
					attrib++;
				if (attrib == end)
				{
					error_string = "Attribute quotation not closed.";
					working = false;
					error = true;
					break;
				}
				(*attrib) = 0;
				TAttribute*a = inner->attributes.append(name_offset);
				a->name = name_offset;
				a->value = decode(data_offset);
				name_offset = attrib+1;
			}
		bool do_enter = field[buffer.length()-1] != '/';
		
		if ((next && next->want_content)||(!next && except))
		{
			if (!read('<',buffer,f))
			{
				error = true;
				working = false;
				error_string = "Unexpected end of file.";
			}
			inner->content = decode(buffer.ToStringRef());
		}
		else
			if (!skip('<',f))
			{
				error = true;
				working = false;
				error_string = "Unexpected end of file.";
			}
		
		if (next)
		{
			if (next->on_enter)
				next->on_enter(inner);
		}
		else
			if (except)
				except(inner);
		
		if (do_enter)
		{
			current = inner;
			rule = next;
		}
		else
		{
			if (next && next->on_exit)
				next->on_exit(inner);
			stack.erase(stack.count()-1);	//erase 'inner'
		}
	}
	
	fclose(f);
	if (!error && stack.count()!=1)
	{
		error = true;
		error_string = "Unexpected end of file.";
	}
	return !error;
}
