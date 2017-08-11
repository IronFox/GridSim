#ifndef cliTplH
#define cliTplH


/******************************************************************

E:\include\string\cli.tpl.h

******************************************************************/

template <typename F>
	CLI::PCommand				CLI::Interpreter::DefineCommand(const String& def, const F&f, eCommandCompletion completion/*=NoCompletion*/)
	{
		Tokenizer::tokenize(def,lookup.pathConfig,lookup.pathSegments);
		if (!lookup.pathSegments)
			return NULL;
		PFolder parent = _Resolve(def.BeginsWith('/'));
		if (!parent)
			return NULL;
		Command::Name name;
		if (!name.Parse(lookup.pathSegments.last()))
			return false;

		PCommand result = parent->commands.Query(name.coreName);
		if (result)
			return result;
		PCommand command (new Command(name,f, completion));
		parent->commands.Insert(command->name, command);
		return command;
	}

template <typename F>
	CLI::PCommand				CLI::Interpreter::DefineGlobalCommand(const String& def, const F&f, eCommandCompletion completion/*=NoCompletion*/)
	{
		Command::Name name;
		if (!name.Parse(def))
			return false;
		PCommand result = globalCommands.Query(name.coreName);
		if (result)
			return result;
		PCommand command (new Command(name,f, completion));
		globalCommands.Insert(command->name, command);
		return command;
	}



template <typename T, count_t Components>
	count_t				DecodeVectorContent(T*content,const String&value)
	{
		if (value.IsEmpty())
			return 0;
		index_t	begin = value.Find('('),
				end = value.Find(')');
		if (begin ==InvalidIndex || end == InvalidIndex)
		{
			begin = value.Find('{');
			end = value.Find('}');
		}
		
		if (begin ==InvalidIndex || end == InvalidIndex)
		{
			begin = value.Find('[');
			end = value.Find(']');
		}
		
		if (begin ==InvalidIndex|| end==InvalidIndex)
		{
			begin = 0;
			end = value.length()-1;
		}
		else
		{
			begin++;
			end--;
		}
		index_t	last = begin,
				count = 0;
		for (index_t i = begin; i <= end && count < Components; i++)
			if (value.GetChar(i)==',' || value.GetChar(i)==';')
			{
				String sub = value.subString(last,i-last);
				sub.TrimThis();
				if (!convert(sub.c_str(),content[count++]))
					return 0;
				last = i+1;
			}
		{
			String sub = value.subString(last,end-last+1);
			sub.TrimThis();
			if (!convert(sub.c_str(),content[count++]))
				return 0;
		}
		return count;
	}

template <typename T, count_t Components>
	bool				SetVectorVariableContent(T*content, const String&component, const String&value)
	{
		T temp[Components];
		count_t count = DecodeVectorContent<T,Components>(temp,value);
		if (count == 0)
			return false;

		if (count == 1 && component.length()>1)
		{
			count = M::vmin(component.length(),Components);

			for (index_t i = 1; i < count; i++)
				temp[i] = temp[0];
		}
		index_t at = 0;
		for (index_t i = 0; i < component.length() && at < count; i++)
		{
			switch (component.GetChar(i))
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
	

template <typename T, count_t Components>
	bool				SetVectorVariableContent(T*content, const String&value)
	{
		count_t count = DecodeVectorContent<T,Components>(content,value);
		if (count == 1)
		{
			for (index_t i = 1; i < Components; i++)
				content[i] = content[0];
			return true;
		}
		return count == Components;
	}

	
	
template <count_t Components>
	VectorVariable<Components>::VectorVariable(const String&name, float content_[Components], unsigned protection):Variable(name,"FloatVector",Components,protection)
	{
		if (content_)
			M::VecUnroll<Components>::copy(content_,content);
	}
	
template <count_t Components>
	String			VectorVariable<Components>::ConvertToString()					const
	{
		return M::VecUnroll<Components>::toString(content);
	}
	
template <count_t Components>
	bool			VectorVariable<Components>::Set(const String&value)
	{
		return SetVectorVariableContent<float,Components>(content,value);
	}

template <count_t Components>	
	bool	VectorVariable<Components>::Set(const String&component, const String&value)
	{
		return SetVectorVariableContent<float,Components>(content,component,value);
	}


template <count_t Components>
	DoubleVectorVariable<Components>::DoubleVectorVariable(const String&name, double content_[Components], unsigned protection):Variable(name,"DoubleVector",Components,protection)
	{
		if (content_)
			M::VecUnroll<Components>::copy(content_,content);
	}
	
template <count_t Components>
	String		DoubleVectorVariable<Components>::ConvertToString()					const
	{
		return M::VecUnroll<Components>::toString(content);
	}

template <count_t Components>	
	bool		DoubleVectorVariable<Components>::Set(const String&value)
	{
		return SetVectorVariableContent<double,Components>(content,value);
	}

template <count_t Components>	
	bool		DoubleVectorVariable<Components>::Set(const String&component, const String&value)
	{
		return SetVectorVariableContent<double,Components>(content,component,value);
	}	


#endif
