#ifndef cliTplH
#define cliTplH


/******************************************************************

E:\include\string\cli.tpl.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

template <typename F>
	CLI::Command*				CLI::Interpretor::defineCommand(const String& def, const F&f, eCommandCompletion completion/*=NoCompletion*/)
	{
		Tokenizer::tokenize(def,lookup.path_config,lookup.path_segments);
		if (!lookup.path_segments)
			return NULL;
		Folder*parent = resolve(def.beginsWith('/'));
		if (!parent)
			return NULL;
		
		StringList	items(lookup.path_segments.last());
		if (!items)
			return NULL;
		detail::makeValidName(items.first());
		Command*result = parent->commands.lookup(items.first());
		if (result)
			return result;
		String description = items.first();
		if (items > 1)
			description += " <"+items.fuse(1,"> <")+">";
		
		Command*command = SHIELDED(new Command(items.first(),description,f, completion));
		parent->commands.insert(command);
		return command;
	}

template <typename F>
	CLI::Command*				CLI::Interpretor::defineGlobalCommand(const String& def, const F&f, eCommandCompletion completion/*=NoCompletion*/)
	{
		StringList	items(def);
		if (!items)
			return NULL;
		detail::makeValidName(items.first());
		Command*result = global_commands.lookup(items.first());
		if (result)
			return result;
		String description = items.first();
		if (items > 1)
			description += " <"+items.fuse(1,"> <")+">";
		
		Command*command = SHIELDED(new Command(items.first(),description,f, completion));
			
		global_commands.insert(command);
		return command;
	}


template <typename T, unsigned Components>
	bool				setVectorVariableContent(T*content, const String&component, const String&value)
	{
		T temp[Components];
		
		index_t	begin = value.indexOf('('),
				end = value.indexOf(')');
		if (!begin || !end)
		{
			begin = value.indexOf('{');
			end = value.indexOf('}');
		}
		
		if (!begin || !end)
		{
			begin = value.indexOf('[');
			end = value.indexOf(']');
		}
		
		if (!begin || !end)
		{
			begin = 1;
			end = value.length();
		}
		else
		{
			begin++;
			end--;
		}
		index_t	last = begin,
				count = 0;
		for (index_t i = begin; i <= end && count < Components; i++)
			if (value.get(i-1)==',' || value.get(i-1)==';')
			{
				String sub = value.subString(last-1,i-last);
				sub.trimThis();
				if (!convert(sub.c_str(),temp[count++]))
					return false;
				last = i+1;
			}
		{
			String sub = value.subString(last-1,end-last+1);
			sub.trimThis();
			if (!convert(sub.c_str(),temp[count++]))
				return false;
		}
		if (count == 1 && component.length()>1)
		{
			count = vmin(component.length(),Components);

			for (index_t i = 1; i < count; i++)
				temp[i] = temp[0];
		}
		index_t at = 0;
		for (index_t i = 0; i < component.length() && at < count; i++)
		{
			switch (component.get(i))
			{
				case 'r':
				case 'R':
				case 'x':
				case 'X':
				case 'u':
				case 'U':
					if (Components < 1)
						return false;
					content[0] = temp[at++];
				break;
				case 'g':
				case 'G':
				case 'y':
				case 'Y':
				case 'v':
				case 'V':
					if (Components < 2)
						return false;
					content[1] = temp[at++];
				break;
				case 'b':
				case 'B':
				case 'z':
				case 'Z':
					if (Components < 3)
						return false;
					content[2] = temp[at++];
				break;
				case 'a':
				case 'A':
				case 'w':
				case 'W':
					if (Components < 4)
						return false;
					content[3] = temp[at++];
				break;
			}
		}
		return true;
	}
	

template <typename T, unsigned Components>
	bool				setVectorVariableContent(T*content, const String&value)
	{
		index_t		begin = value.indexOf('('),
					end = value.indexOf(')');
		if (!begin || !end)
		{
			begin = value.indexOf('{');
			end = value.indexOf('}');
		}
		
		if (!begin || !end)
		{
			begin = value.indexOf('[');
			end = value.indexOf(']');
		}
				
		if (!begin || !end)
		{
			begin = 1;
			end = value.length();
		}
		else
		{
			begin++;
			end--;
		}
		
		
		index_t 	last = begin,
					index = 0;
		for (index_t i = begin; i <= end && index < Components; i++)
			if (value.get(i-1)==',' || value.get(i-1)==';')
			{
				String sub = value.subString(last-1,i-last);
				sub.trimThis();
				if (!convert(sub.c_str(),content[index++]))
					return false;
				last = i+1;
			}
		{
			String sub = value.subString(last-1,end-last+1);
			sub.trimThis();
			if (!convert(sub.c_str(),content[index++]))
				return false;
		}
		if (index == 1)
		{
			for (index_t i = 1; i < Components; i++)
				content[i] = content[0];
			return true;
		}
		return index == Components;
	}

	
	
template <unsigned Components>
	VectorVariable<Components>::VectorVariable(const String&name, float content_[Components], unsigned protection):Variable(name,"FloatVector",Components,protection)
	{
		if (content_)
			VecUnroll<Components>::copy(content_,content);
	}
	
template <unsigned Components>
	String			VectorVariable<Components>::toString()					const
	{
		return VecUnroll<Components>::toString(content);
	}
	
template <unsigned Components>
	bool			VectorVariable<Components>::set(const String&value)
	{
		return setVectorVariableContent<float,Components>(content,value);
	}

template <unsigned Components>	
	bool	VectorVariable<Components>::set(const String&component, const String&value)
	{
		return setVectorVariableContent<float,Components>(content,component,value);
	}


template <unsigned Components>
	DoubleVectorVariable<Components>::DoubleVectorVariable(const String&name, double content_[Components], unsigned protection):Variable(name,"DoubleVector",Components,protection)
	{
		if (content_)
			VecUnroll<Components>::copy(content_,content);
	}
	
template <unsigned Components>
	String		DoubleVectorVariable<Components>::toString()					const
	{
		return VecUnroll<Components>::toString(content);
	}

template <unsigned Components>	
	bool		DoubleVectorVariable<Components>::set(const String&value)
	{
		return setVectorVariableContent<double,Components>(content,value);
	}

template <unsigned Components>	
	bool		DoubleVectorVariable<Components>::set(const String&component, const String&value)
	{
		return setVectorVariableContent<double,Components>(content,component,value);
	}	


#endif
