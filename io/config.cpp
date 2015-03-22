#include "../global_root.h"
#include "config.h"

/******************************************************************

Extended configuration-file-parser.

******************************************************************/


#define err(x,code) {errors.add(String(x)+" in line "+IntToStr(frame.line)+" of '"+frame.filename+"' "+makePath(file_history));error=code;frame.error_context=frame.filename;}
#define _err(x,code) {err(x,code);return code;}
#define serr(x,code) {err_to=code; err_list.add("syntax-error in section '"+dif+"': "+String(x));}

//(included from '"+file_history.last()+"')
//


namespace Config
{

	Container::Config::Config()
	{
		main_separator="";
		operators = "= := += -= *= /=";
		quotations="\"'";
		quotation_escape_character = '\\';
		recursion_up = "([{";
		recursion_down = ")]}";
		trim_characters = " \t";
	}

	Container::Config						Container::configuration;


	Context::Attribute*		Context::protectedDefine(const String&name, const String&value)
	{
		if (!this || name.IsEmpty())
			return NULL;
		Attribute*result;
		if (attribute_map.query(name,result))
		{
			result->value = value;
			return result;
		}
		result = attributes.append();
		result->name = name;
		result->value = value;
		result->commented = false;
		result->assignment_operator = '=';
		attribute_map.set(name,result);
		return result;
	}

	Context::Attribute*		Context::define(const String&name, const String&value)
	{
		if (!name.contains('/'))
			return protectedDefine(name.trim(),value);

		Array<String>	components;
		explode('/',name,components);
		return protectedDefineContext(components.pointer(),components.count()-1)->protectedDefine(components.last().trim(),value);
	}

	Context*					Context::protectedDefineContext(const String*names, count_t num_names)
	{
		Context*ctx = this;
		for (index_t i = 0; i < num_names; i ++)
			ctx = ctx->protectedDefineContext(names[i].trim());
		return ctx;
	}
	
	Context*					Context::protectedDefineContext(const String&name)
	{
		if (!this || name.IsEmpty())
			return NULL;
		Context*result;
		if (child_map.query(name,result))
			return result;
		result = children.append();
		result->name = name;
		result->Is_mode = false;
		child_map.set(name,result);
		return result;
	}
	Context*					Context::defineContext(const String&name)
	{
		if (!name.contains('/'))
			return protectedDefineContext(name.trim());

		Array<String>	components;
		explode('/',name,components);
		return protectedDefineContext(components.pointer(),components.count());
	}
		
	
	Context*					Context::createContext(const String&name)
	{
		Context*result;
		result = children.append();
		result->name = name;
		result->Is_mode = false;
		child_map.set(name,result);
		return result;
	}
	
	Context*					Context::defineMode(const String&name)
	{
		Context*result;
		if (mode_map.query(name,result))
			return result;
		result = modes.append();
		result->name = name;
		result->Is_mode = true;
		mode_map.set(name,result);
		return result;
	}
	
	Context*					Context::defineModeContext(const String&mode_name, const String&context_name)
	{
		return defineMode(mode_name)->defineContext(context_name);
	}
	
	Context*					Context::createModeContext(const String&mode_name, const String&context_name)
	{
		return defineMode(mode_name)->createContext(context_name);
	}
	
	void						Context::clear()
	{
		children.clear();
		child_map.clear();
		modes.clear();
		mode_map.clear();
		attributes.clear();
		attribute_map.clear();
	}


	void						Context::exportModes(ArrayData<Context*>&out)
	{
		out.setSize(modes);
		for (unsigned i = 0; i < modes; i++)
			out[i] = modes[i];
	}
	
	void						Context::exportModes(ArrayData<const Context*>&out)				const
	{
		out.setSize(modes);
		for (unsigned i = 0; i < modes; i++)
			out[i] = modes[i];
	}
	
	void						Context::exportChildren(ArrayData<Context*>&out)
	{
		out.setSize(children.count());
		for (unsigned i = 0; i < children; i++)
			out[i] = children[i];
	}
	
	void						Context::exportChildren(ArrayData<const Context*>&out)			const
	{
		out.setSize(children.count());
		for (unsigned i = 0; i < children; i++)
			out[i] = children[i];
	}
	
	void						Context::exportAttributes(ArrayData<Attribute*>&out)
	{
		out.setSize(attributes);
		for (unsigned i = 0; i < attributes; i++)
			out[i] = attributes[i];
	}
	
	void						Context::exportAttributes(ArrayData<const Attribute*>&out)		const
	{
		out.setSize(attributes);
		for (unsigned i = 0; i < attributes; i++)
			out[i] = attributes[i];
	}


	Context::Attribute*					Context::GetAttrib(const String&path)
	{
		Array<String,Adopt>	segments;
		explode('/',path,segments);
		if (!segments.count())
			return NULL;
		String mode;
		if (index_t at = segments.first().indexOf(':'))
		{
			mode = segments.first().subString(0,at-1);
			segments.first().erase(0,at).trimThis();
			if (!segments.first().length())
				segments.erase(index_t(0));
			
		}
		//std::cout << "retrieving attribute '"+implode(", ",segments)+"' ("+mode+")"<<std::endl;
		Context*context = (segments.count()>1||mode.length())?GetContext(segments.pointer(),segments.count()-1,mode):this;
		if (!context)
		{
			//std::cout << " context could not be located"<<std::endl;
			return NULL;
		}
		Attribute*result;
		if (context->attribute_map.query(segments.last(),result))
			return result;
		/*std::cout << " attribute '"+segments.last()+"' could not be located:"<<std::endl;
		context->attributes.reset();
		while (Attribute*a = context->attributes.each())
		{
			std::cout << "  "<<a->name<<std::endl;
			if (a->name == segments.last())
				std::cout << "  ERROR: found unmapped attribute '"<<a->name<<"'!!!"<<std::endl;
		}*/

		return NULL;
	}


	const Context::Attribute*				Context::GetAttrib(const String&path)						const
	{
		Array<String,Adopt>	segments;
		explode('/',path,segments);
		if (!segments.count())
			return NULL;
		String mode;
		if (index_t at = segments.first().indexOf(':'))
		{
			mode = segments.first().subString(0,at-1);
			segments.first().erase(0,at).trimThis();
			if (!segments.first().length())
				segments.erase(index_t(0));
			
		}
		const Context*context = (segments.count()>1||mode.length())?GetContext(segments.pointer(),segments.count()-1,mode):this;
		if (!context)
			return NULL;
		Attribute*result;
		if (context->attribute_map.query(segments.last(),result))
			return result;
		return NULL;
	}
	
	const String&				Context::Get(const String&path, const String&except)	const
	{
		const Attribute*rs = GetAttrib(path);
		return rs?rs->value:except;
	}
	bool				Context::QueryString(const String&path, String&outResult)	const
	{
		const Attribute*rs = GetAttrib(path);
		if (rs)
		{
			outResult = rs->value;
			return true;
		}
		return false;
	}
	
	int							Context::GetInt(const String&path, int except)			const
	{
		const Attribute*rs = GetAttrib(path);
		int result;
		if (!rs || !convert(rs->value.c_str(),result))
			return except;
		return result;
	}
	
	unsigned					Context::GetUnsigned(const String&path, unsigned except)	const
	{
		const Attribute*rs = GetAttrib(path);
		unsigned result;
		if (!rs || !convert(rs->value.c_str(),result))
			return except;
		return result;
	}

	float						Context::GetFloat(const String&path, float except)		const
	{
		const Attribute*rs = GetAttrib(path);
		float result;
		if (!rs || !convert(rs->value.c_str(),result))
			return except;
		return result;
	}

	bool						Context::GetBool(const String&path, bool except)		const
	{
		const Attribute*rs = GetAttrib(path);
		bool result;
		if (!rs || !convert(rs->value.c_str(),result))
			return except;
		return result;
	}

	Key::Name					Context::GetKey(const String&path, Key::Name except)		const
	{
		const Attribute*rs = GetAttrib(path);
		Key::Name result;
		if (!rs || !convert(rs->value.c_str(),result))
			return except;
		return result;
	}

	Context*					Context::GetContext(const String&path)
	{
		Array<String,Adopt>	segments;
		explode('/',path,segments);
		if (!segments.count())
			return NULL;
		String mode;
		if (index_t at = segments.first().indexOf(':'))
		{
			mode = segments.first().subString(0,at-1);
			segments.first().erase(0,at);
		}
		return GetContext(segments.pointer(),segments.count(),mode);
	}
	
	Context*					Context::GetContext(const String*path, size_t path_len, const String&mode)
	{
		Context*n;
		if (mode.length() && mode_map.query(mode,n))
			if (Context*rs = n->GetContext(path,path_len,""))
				return rs;
		if (path_len && child_map.query(*path,n))
			return n->GetContext(path+1,path_len-1,mode);
		return path_len || mode.length()?NULL:this;
	}
	
	const Context*					Context::GetContext(const String&path) const
	{
		Array<String,Adopt>	segments;
		explode('/',path,segments);
		if (!segments.count())
			return NULL;
		String mode;
		if (index_t at = segments.first().indexOf(':'))
		{
			mode = segments.first().subString(0,at-1);
			segments.first().erase(0,at);
		}
		return GetContext(segments.pointer(),segments.count(),mode);
	}
	
	const Context*					Context::GetContext(const String*path, size_t path_len, const String&mode) const
	{
		Context*n;
		if (mode.length() && mode_map.query(mode,n))
			if (const Context*rs = ((const Context*)n)->GetContext(path,path_len,""))
				return rs;
		if (path_len && child_map.query(*path,n))
			return ((const Context*)n)->GetContext(path+1,path_len-1,mode);
		return path_len || mode.length()?NULL:this;
	}
	
	const String&					Context::GetOperator(const String&path)						const
	{
		static String empty = "";
		const Attribute*attrib = GetAttrib(path);
		if (!attrib)
			return empty;
		return attrib->assignment_operator;
	}
	
	void		Context::cleanup()
	{
		children.reset();
		while (Context*child = children.each())
		{
			child->cleanup();
			if (child->IsEmpty())
			{
				child_map.unSet(child->name);
				children.erase();
			}
		}
		
		modes.reset();
		while (Context*mode = modes.each())
		{
			mode->cleanup();
			if (mode->IsEmpty())
			{
				mode_map.unSet(mode->name);
				modes.erase();
			}
		}
	}
	
	bool		Context::IsEmpty() const
	{
		return !children && !attributes && !modes;
	}
	
	void		Context::retrieveMaxNameValueLength(size_t&name_len, size_t&value_len, size_t indent)	const
	{
		for (index_t i = 0; i < attributes; i++)
		{
			if (name_len < attributes[i]->name.length()+indent)
				name_len = attributes[i]->name.length()+indent;
			if (value_len < attributes[i]->value.length()+2+indent)
				value_len = attributes[i]->value.length()+2+indent;
		}
		for (index_t i = 0; i < children; i++)
			children[i]->retrieveMaxNameValueLength(name_len,value_len,indent+4);
		
		for (index_t i = 0; i < modes; i++)
		{
			const Context*mode = modes[i];
			if (mode->children == 1 && !mode->attributes)
				mode->children.first()->retrieveMaxNameValueLength(name_len,value_len,indent+4);
			else
				mode->retrieveMaxNameValueLength(name_len,value_len,indent+4);
		}
	}


	void		Context::writeContent(StringBuffer&buffer, size_t name_len, size_t value_len, const String&indent) const
	{
		for (index_t i = 0; i < attributes.count(); i++)
		{
			const Attribute*attrib = attributes[i];
			if (attrib->commented)
				buffer << "//";
			buffer << indent << attrib->name;
			count_t tabs = (name_len-attrib->name.length()-indent.length()*4)/4;
			if ((name_len-attrib->name.length()-indent.length()*4)%4)
				tabs++;
			for (index_t i = 0; i <= tabs; i++)
				buffer << '\t';
			
			buffer<<attrib->assignment_operator<<"\t";
			
			size_t len = attrib->value.length();
			if (!isBool(attrib->value.c_str()) && !isFloat(attrib->value.c_str()))
			{
				String value = attrib->value;
				//std::cout << "'"<<value<<"'"<<std::endl;
				for (index_t i = 0; i < value.length(); i++)
					if (value.get(i) == '\"')
					{
						value.insert(i,'\\');
						//std::cout << "'"<<value<<"'"<<std::endl;
						i++;
					}
				//std::cout << "'"<<value<<"'"<<std::endl;
				buffer << "\""<<value<<"\"";
				len = value.length()+2;
			}
			else
				buffer << attrib->value;
			
			if (attrib->comment.length())
			{
				count_t tabs = (value_len - len)/4;
				if ((value_len - len)%4)
					tabs++;
				for (index_t i = 0; i <= tabs; i++)
					buffer << '\t';
				buffer << "\t//"<<attrib->comment;
			}
			buffer << nl;
		}
		for (index_t i = 0; i < modes.count(); i++)
		{
			const Context*mode = modes[i];
			if (mode->children == 1 && !mode->attributes.count() && !mode->modes.count())
			{
				buffer << indent << "["<<mode->name<<":"<<mode->children.first()->name<<"]"<<nl;
				mode->children.first()->writeContent(buffer,name_len,value_len,indent+"\t");
			}
			else
			{
				buffer << indent << mode->name<<":"<<nl;
				mode->writeContent(buffer,name_len,value_len,indent+"\t");
			}
		}
		for (index_t i = 0; i < children.count(); i++)
		{
			const Context*child = children[i];
			buffer << indent << "["<<child->name<<"]"<<nl;
			child->writeContent(buffer,name_len,value_len,indent+"\t");
		}
	}
	
	
	Container::Container()
	{
	}
	
	Container::Container(const String&filename)
	{
		LoadFromFile(filename);
	
	}
	
	bool	Container::IsOperator(const String&string)
	{
		if (string == '=')
			return true;
		if (string.length() != 2 || string.get(1) != '=')
			return false;
		char c = string.firstChar();
		switch (c)
		{
			case ':':
			case '+':
			case '-':
			case '*':
			case '/':
				return true;
		}
		return false;
	}
	
	unsigned	Container::GetDepth(const char*line)
	{
		unsigned depth = 0;
		for(;;)
		{
			if (*line == ' ')
				depth ++;
			elif (*line == '\t')
				depth += 4;
			else
				break;
			line++;
		}
		return depth/4;
	}
	
	
	void	Container::Clear()
	{
		Context::clear();
		error = "";
	}
	
	LogFile	logfile("configuration.log",true);
	
	bool	Container::LoadFromFile(const String&filename)
	{
		StringFile file(CM_STRIP_COMMENTS|CM_RECORD_COMMENTS|CM_STRIP_LINE_START_COMMENTS);
		if (!file.open(filename))
		{
			error = "File not found: "+filename;
			return false;
		}
		return LoadFromFile(file);
	}

	bool	Container::LoadFromFile(StringFile&file)
	{
		clear();
		List::ReferenceVector<Context>	stack_elements;
		stack_elements.append(this);
		
		//logfile << "Now processing file "<<filename<<nl;
		file.lineStartComment = ";";


		bool errors = false;
		String line;
		StringList		tokens;
		while (file>>line)
		{
			if (!line.trim().length())
				continue;
			//logfile << ' '<<line<<nl;
			unsigned depth = GetDepth(line.c_str());
			String group = line.getBetween('[',']').trimThis();
			if (depth >= stack_elements)
			{
				error += "\nRecursive depth of line "+String(file.root_line)+" exceeds stack depth ("+String(stack_elements)+"):\n "+line;
				errors = true;
				continue;
			}
			
			while (depth < stack_elements-1)
				stack_elements.drop(stack_elements-1);
			
			Context*context = stack_elements.last();
			
			String mode;
			if (line.trim().endsWith(':'))
			{
				group = "";
				mode = line.trim();
				mode.erase(mode.length()-1);
			}
			
			if (index_t at = group.indexOf(':'))
			{
				mode = group.subString(0,at-1);
				mode.trimThis();
				group.erase(0,at).trimThis();
			}
			
			if (mode.length())
			{
				bool exists = false;
				stack_elements.reset();
				while (Context*ctx = stack_elements.each())
					if (ctx->IsMode())
					{
						exists = true;
						break;
					}
				if (exists)
				{
					error += "Mode in mode detected: '"+mode+"' in line "+IntToStr(file.root_line)+":\n "+line;
					errors = true;
				}
				else
				{
					Context*child = context->defineMode(mode);
					if (!group.length())
						stack_elements.append(child);
					context = child;
				}
			}
			if (group.length())
			{
				Context*child = context->createContext(group);
				stack_elements.append(child);
				context = child;
			}
			
			if (!group.length() && !mode.length())
			{
				Tokenizer::tokenize(line,configuration,tokens);
				if (!tokens)
					continue;
				if (tokens != 3 || !IsOperator(tokens[1]))
				{
					error += "\nSyntax error in line "+String(file.root_line)+(IsOperator(tokens[1])?"":" (invalid operator)")+":\n "+line;
					errors = true;
					continue;
				}
				Attribute*attrib = context->define(tokens.first());
				if (attrib->value.length())
				{
					error += "\nRedefining attribute '"+attrib->name+"' to '"+tokens[2]+"' in line "+String(file.root_line)+":\n "+line;
					errors = true;
				}
				attrib->assignment_operator = tokens[1];
				attrib->value = Tokenizer::dequote(tokens[2],configuration);
				attrib->comment = file.comment.trim();
			}
		}
		//logfile << "Reached end of file "<<filename<<nl;
		return !errors;
	}
	
	
	bool						Container::SaveToFile(const String&filename)
	{
		StringFile	file;
		if (!file.create(filename))
		{
			error = "Unable to create/overwrite file '"+filename+"'";
			return false;
		}
		error = "";

		SaveToFile(file);
		return true;
	}


	void						Container::SaveToStringBuffer(StringBuffer&buffer)
	{
		time_t tt = time(NULL);
		const tm*t = localtime(&tt);
		char time_buffer[256];
		strftime(time_buffer,sizeof(time_buffer),"%B %d. %Y %H:%M:%S",t);

		StringList	comments;
		comments << "Extended initialization file generated by <undefined> on "+String(time_buffer)+".";
		
		for (index_t i = 0; i < comments.count(); i++)
		{
			String&line = comments[i];
			while (line.length() > 80)
			{
				const char	*begin = line.c_str(),
							*c = begin+80;
				while (c != begin && (*c) != ' ')
					c--;
				if (c==begin)
					c = begin+line.length()/2;
				comments.insert(i+1,line.subString(c-begin+1));
				line.erase(c-begin);
			}
		}
		
		buffer << "/*";
		buffer << nl;
		for (index_t i = 0; i < comments.count(); i++)
			buffer << "; "<<comments[i]<<nl;
		buffer << "*/"<<nl<<nl;
		
		size_t	name_len = 0,
				value_len = 0;
		retrieveMaxNameValueLength(name_len,value_len);
		if (name_len%4)
			name_len += 4-(name_len%4);
		if (value_len%4)
			value_len += 4-(value_len%4);

		Context::writeContent(buffer,name_len,value_len);
	}


	void						Container::SaveToFile(StringFile&file)
	{
		StringBuffer buffer;
		SaveToStringBuffer(buffer);
		file << buffer.ToStringRef();
		//return true;
	}
	
	bool						Container::HasErrors()	const
	{
		return error.IsNotEmpty();
	
	}
	
	const String&				Container::GetError()	const
	{
		return error;
	}
	
	bool									CXContext::process(String&expression, bool singular, String*error_out)	const
	{
		Buffer<index_t>	segment_start(2);
		Buffer<bool>		Is_segment(2);
		for (index_t i = 0; i < expression.length(); i++)
		{
			if (expression.get(i) == '(')
			{
				Is_segment << !(i+1 == expression.length() || expression.get(i+1)!='$');
				segment_start << i;
			}
			elif (expression.get(i) == ')')
			{
				if (!segment_start.fillLevel())
				{
					if (error_out)
						(*error_out) = "Variable end at offset "+String(i)+" of expression '"+expression+"' lacks beginning";
					return false;
				}
				index_t start = segment_start.pop();
				if (!Is_segment.pop())
					continue;
				String key = expression.subString(start+2,i-start-2);
				if (key == '*')
				{
					if (singular)
					{
						expression.erase(start, i-start+1);
						i = start;
					}
				}
				else
				{
					const Variable*variable = lookup(key,error_out);
					if (!variable)
					{
						if (error_out)
							(*error_out) += "\nVariable not found: '"+key+"'";
						return false;
					}
					if (singular)
					{
						expression.replaceSubString(start, i-start+1, variable->first());
						i = start+variable->first().length();
					}
					else
					{
						String imploded = implode(", ",*variable);
						expression.replaceSubString(start, i-start+1, imploded);
						i = start+imploded.length();
					}
				}
			}
		}
		return true;
	}
	
	CXContext::Variable*					CXContext::innerFindVariable(const ArrayData<String>&segments)
	{
		CXContext*context = this;
		for (index_t i = 0; i+1 < segments.count(); i++)
		{
			context = context->children.lookup(segments[i]);
			if (!context)
				return NULL;
		}
		std::cout << "checking for variable '"<<segments.last()<<"' among "<<context->variables.count()<<" variables"<<std::endl;
		return context->variables.lookup(segments.last());
	}
	
	const CXContext::Variable*					CXContext::innerFindVariable(const ArrayData<String>&segments) const
	{
		const CXContext*context = this;
		for (index_t i = 0; i+1 < segments.count(); i++)
		{
			context = context->children.lookup(segments[i]);
			if (!context)
				return NULL;
		}
		std::cout << "checking for variable '"<<segments.last()<<"' among "<<context->variables.count()<<" variables of "<<context->name<<std::endl;
		return context->variables.lookup(segments.last());
	}
	
	CXContext::Variable*					CXContext::findVariable(const String&path)
	{
		Array<String,Adopt>	segments;
		explodeCallback(IsPathSeparator,path,segments);
		CXContext*context = this;
		while (context->parent)
			context = context->parent;
		return context->innerFindVariable(segments);
	}
	
	const CXContext::Variable*					CXContext::findVariable(const String&path)	const
	{
		Array<String,Adopt>	segments;
		explodeCallback(IsPathSeparator,path,segments);
		const CXContext*context = this;
		while (context->parent)
			context = context->parent;
		return context->innerFindVariable(segments);
	}
	
	
	CXContext*					CXContext::innerFindContext(const ArrayData<String>&segments)
	{
		CXContext*context = this;
		for (index_t i = 0; i < segments.count(); i++)
		{
			context = context->children.lookup(segments[i]);
			if (!context)
				return NULL;
		}
		return context;
	}
	
	const CXContext*					CXContext::innerFindContext(const ArrayData<String>&segments) const
	{
		const CXContext*context = this;
		for (index_t i = 0; i < segments.count(); i++)
		{
			context = context->children.lookup(segments[i]);
			if (!context)
				return NULL;
		}
		return context;
	}
	
	CXContext*					CXContext::findContext(const String&path)
	{
		Array<String,Adopt>	segments;
		explodeCallback(IsPathSeparator,path,segments);
		CXContext*context = this;
		while (context->parent)
			context = context->parent;
		return context->innerFindContext(segments);
	}
	
	const CXContext*					CXContext::findContext(const String&path)	const
	{
		Array<String,Adopt>	segments;
		explodeCallback(IsPathSeparator,path,segments);
		const CXContext*context = this;
		while (context->parent)
			context = context->parent;
		return context->innerFindContext(segments);
	}
	
	
	CXContext::Variable*					CXContext::lookup(String path, String*error_out)
	{
		if (!process(path,true,error_out))
			return NULL;
		Variable*rs = findVariable(path);
		if (!rs && error_out)
			(*error_out) = "Unable to locate variable '"+path+"'";
		return rs;
	}
	
	const CXContext::Variable*				CXContext::lookup(String path, String*error_out)	const
	{
		if (!process(path,true,error_out))
			return NULL;
		const Variable*rs = findVariable(path);
		if (!rs && error_out)
			(*error_out) = "Unable to locate variable '"+path+"'";
		return rs;
	}
	
	void	CXContext::clear()
	{
		variables_finalized = false;
		children.clear();
		variables.clear();
		pre_finalize_children.clear();
		name = "";
	}
	
	bool	CXContext::validNameChar(char c)
	{
		return (c>='a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c=='_' || c=='+' || c == '-' || c == '#' || c == '~' || c == '%' || c == '[' || c == ']';
	}
	
	void	CXContext::parse(const String&content)
	{
		std::cout << "parsing content '"<<content<<"'"<<std::endl;
		Array<String,Adopt>	lines,segments;
		explode(';',content,lines);
		if (!lines.count())
			return;
		lines.last().trimThis();
		if (!lines.last().IsEmpty())
			throw IO::DriveAccess::FileDataFault("; expected at end of line '"+lines.last()+"'");
		
		if (lines.count() == 1)
			return;
		
		for (index_t i = 0; i+1 < lines.count(); i++)
		{
			index_t at = lines[i].indexOf('=');
			if (!at)
				throw IO::DriveAccess::FileDataFault("Expression lacks '=': '"+lines[i]+"'");

			String	name = lines[i].subString(0,at-1).trim(),
					value = lines[i].subString(at).trim();
			if (name.IsEmpty() || !name.isValid(validNameChar))
				throw IO::DriveAccess::FileDataFault("Name validation failed. '"+name+"' Is no valid variable name");

			std::cout << "defining variable '"<<name<<"' in "<<this->name<<std::endl;
			Variable*var = variables.append(name);
			
			if (at = value.indexOf("($*)"))
			{
				if (var->count())
					value.replaceSubString(at-1,4,var->first());
				else
					value.erase(at-1,4);
				value.insert(at-1,"($*),");
			}
			var->setSize(1);
			var->first() = value;
		}

	}
	
	void	CXContext::loadStacked(const XML::Node*node)
	{
		if (!node)
			throw IO::ParameterFault(globalString("Provided node Is NULL"));


		std::cout << "now processing node '"<<node->name<<"'"<<std::endl;
		conditions.setSize(node->attributes.count());
		for (index_t i = 0; i < conditions.count(); i++)
			conditions[i] = *node->attributes.get(i);
		name = node->name;
		parse(node->inner_content);
		
		for (index_t i = 0; i < node->children.count(); i++)
		{
			std::cout << "processing child #"<<i<<" '"<<node->children[i].name<<"'"<<std::endl;
			const XML::Node&xchild = node->children[i];
			CXContext*child = pre_finalize_children.append();
			child->parent = this;
			child->loadStacked(&xchild);
		
			parse(xchild.following_content);
		}
	}
	
	
	void	CXContext::finalizeVariables()
	{
		if (variables_finalized)
			return;
		for (StringMappedList<Variable>::iterator it = variables.begin(); it != variables.end(); ++it)
		{
			//std::cout << "rendering variable #"<<i<<std::endl;
			Variable*variable = *it;
			ASSERT_NOT_NULL__(variable);
			ASSERT__(variable->count() > 0);
			String	value = variable->first();
			String error;
			std::cout << "finalizing variable = '"<<value<<"'"<<std::endl;
			if (!process(value,false,&error))
				throw IO::DriveAccess::FileDataFault("Expression processing failed for expression '"+value+"' ("+error+")");
			
			explode(',',value,*variable);
			for (index_t i = 0; i < variable->count(); i++)
				(*variable)[i].trimThis();
		}
		variables_finalized = true;
	}
	
	void	CXContext::trimVariables()
	{
		for (StringMappedList<Variable>::iterator it = variables.begin(); it != variables.end(); ++it)
		{
			for (unsigned i = 0; i < (*it)->count(); i++)
			{
				(*(*it))[i].trimThis();
				if ((*(*it))[i].IsEmpty() || (*(*it))[i] == "($*)")
				{
					(*it)->erase(i--);
				}
			}
		}
	}
	
	void	CXContext::finalize()
	{
		bool succeeded = true;
		String error_message;
		finalizeVariables();
		for (index_t i = 0; i < pre_finalize_children.count(); i++)
		{
			CXContext*child = pre_finalize_children[i];
			bool do_map = true;
			for (index_t j = 0; j < child->conditions.count(); j++)
			{
				const XML::TAttribute&attrib = child->conditions[j];
				if (attrib.name == "inherit")
				{
					String value = attrib.value;
					String error;
					if (!process(value,false,&error))
					{
						error_message = "Failed to process inheritance '"+value+"' ("+error+")";
						succeeded = false;
						continue;
					}

					Array<String,Adopt>	inherit;
					explode(',',value,inherit);
					child->finalizeVariables();

					for (index_t k = 0; k < inherit.count(); k++)
					{
						inherit[k].trimThis();
						if (inherit[k].IsEmpty())
							continue;
						CXContext*target = findContext(inherit[k]);
						if (!target)
						{
							error_message = "Failed to locate inheritance '"+inherit[k]+"'";
							succeeded = false;
							continue;
						}
						if (target->childOf(child) || child->childOf(target))
							continue;
						Array<String,Adopt>		names;
						Array<Variable*>	variables;
						target->variables.exportTo(names, variables);
						for (index_t i = 0; i < names.count(); i++)
						{
							std::cout << "inheriting variable "<<names[i]<<" = '"<<implode("', '",*(variables[i]))<<"' into "<<child->name<<std::endl;
							if (!child->variables.isSet(names[i]))
								(*child->variables.append(names[i])) = *(variables[i]);
							else
							{
								Variable*var = child->variables[names[i]];
								ASSERT_NOT_NULL__(var);
								std::cout <<" variable already exists with value '"<<implode("', '",*(var))<<"'"<<std::endl;
								index_t at;
								for (index_t j = 0; j < var->count(); j++)
									while (at = (*var)[j].indexOf("($*)"))
									{
										std::cout << "found ($*) in parameter #"<<j<<std::endl;
										(*var)[j].replaceSubString(at-1,4,implode(',',*variables[i]));
										Array<String,Adopt>	sub;
										explode(',',(*var)[j],sub);
										for (index_t k = 0; k < sub.count(); k++)
											sub[k].trimThis();
										var->erase(j);
										var->insertImport(j,sub);
										break;
									}
							}
						}
					}
					continue;
				}
				const Variable*var = findVariable(attrib.name);
				if (!var)
				{
					error_message += "\nUnable to check for compliance of conditional attribute '"+attrib.name+"="+attrib.value+"'";
					succeeded = false;
					do_map = false;
					continue;
				}
				
				if (var->first() != attrib.value)
				{
					do_map = false;
					break;
				}
			}
			if (do_map)
			{
				if (children.isSet(child->name))
				{
					error_message = "Trying to redefine child context '"+child->name+"'";
					DISCARD(child);
					succeeded = false;
				}
				else
				{
					children.append(child->name,child);
					if (succeeded)
						try
						{
							child->finalize();
						}
						catch (const std::exception&except)
						{
							error_message = except.what();
							succeeded = false;
						}
				}
			}
			else
				DISCARD(child);
		}
		pre_finalize_children.flush();
		
		trimVariables();
		
		if (!succeeded)
			throw IO::DriveAccess::FileDataFault(error_message);
	}
	
	
	void	CXContext::loadFromXML(const XML::Container&container, bool stack)
	{
		if (!stack)
			clear();
		loadStacked(&container.root_node);
		if (!stack)
			return finalize();
	}
	
	void	CXContext::loadFromFile(const String&filename, bool stack)
	{
		XML::Container	container;
		container.loadFromFile(filename);
		loadFromXML(container,stack);
	}
	
	bool	CXContext::IsPathSeparator(char c)
	{
		return c == '.' || c == '/';
	}
	
	bool	CXContext::set(const String&variable_name, const String&variable_value)
	{
		Array<String,Adopt>	segments;
		explodeCallback(IsPathSeparator,variable_name,segments);
		CXContext*context = this;
		CXContext*result;
		String var_name = segments.last();
		segments.erase(segments.count()-1);
		while (context)
		{
			if (result = context->innerFindContext(segments))
			{
				Variable*var = result->variables.append(var_name);
				var->setSize(1);
				var->first() = variable_value;
				return true;
			}
			context = context->parent;
		}
		return false;
	}
	
	bool	CXContext::childOf(const CXContext*other)	const
	{
		const CXContext*ctx = this;
		while (ctx && ctx != other)
			ctx = ctx->parent;
		return ctx == other;
	}
	
	void	CXContext::printToCOut(unsigned indent)	const
	{
		std::cout << tabSpace(indent)<<name<<std::endl;
		Array<String,Adopt>		names;
		Array<const Variable*>	variables;
		this->variables.exportTo(names, variables);
		for (index_t i = 0; i < names.count(); i++)
			std::cout << tabSpace(indent+1)<<names[i]<<" := '"<<implode("', '",*(variables[i]))<<"'"<<std::endl;
				
		for (StringMappedList<CXContext>::const_iterator it = children.begin(); it != children.end(); ++it)
			(*it)->printToCOut(indent+1);
	}
	
	
}

