#include	"../global_root.h"
#include	"cli.h"




namespace CLI
{
	static inline bool isAllowedCharacter(char c)
	{
		return (isalnum(c) || c == '_') && (c != '\n' && c != '\r' && c != ' ');
	}

	namespace detail
	{
		void	makeValidName(String&name)
		{
			for (index_t i = 0; i < name.length(); i++)
				if (!isAllowedCharacter(name.get(i)))
					name.set(i,'_');
		}
	}


	Command::Command():callback([](Command*command, const StringList&arguments, const Tokenizer::Config&config) {})
	{
		construct("", "", NoCompletion);
	}

	Command::Command(const String&name, const String&description, const full_function_t&f, eCommandCompletion completion):
					callback(f)
	{
		construct(name,description,completion);
	}
	Command::Command(const String&name, const String&description, const function0_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f();})
	{
		construct(name,description,completion);

	}
	Command::Command(const String&name, const String&description, const function1_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config));})

	{
		construct(name,description,completion);
	}
	Command::Command(const String&name, const String&description, const function2_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config));})

	{
		construct(name,description,completion);
	}
	Command::Command(const String&name, const String&description, const function3_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config));})

	{
		construct(name,description,completion);
	}
	Command::Command(const String&name, const String&description, const function4_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config),Tokenizer::dequote(arguments[4],config));})

	{
		construct(name,description,completion);
	}

	Command::Command(const String&name, const String&description, const function5_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config),Tokenizer::dequote(arguments[4],config),Tokenizer::dequote(arguments[5],config));})

	{
		construct(name,description,completion);
	}
	Command::Command(const String&name, const String&description, const function6_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config),Tokenizer::dequote(arguments[4],config),Tokenizer::dequote(arguments[5],config),Tokenizer::dequote(arguments[6],config));})

	{
		construct(name,description,completion);
	}


	Command::Command(const String&name, const String&description, const full_func_t&f, eCommandCompletion completion):
					callback(f)
	{
		construct(name,description,completion);
	}
	Command::Command(const String&name, const String&description, const func0_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f();})
	{
		construct(name,description,completion);

	}
	Command::Command(const String&name, const String&description, const func1_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config));})

	{
		construct(name,description,completion);
	}
	Command::Command(const String&name, const String&description, const func2_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config));})

	{
		construct(name,description,completion);
	}
	Command::Command(const String&name, const String&description, const func3_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config));})

	{
		construct(name,description,completion);
	}
	Command::Command(const String&name, const String&description, const func4_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config),Tokenizer::dequote(arguments[4],config));})

	{
		construct(name,description,completion);
	}

	Command::Command(const String&name, const String&description, const func5_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config),Tokenizer::dequote(arguments[4],config),Tokenizer::dequote(arguments[5],config));})

	{
		construct(name,description,completion);
	}
	Command::Command(const String&name, const String&description, const func6_t&f, eCommandCompletion completion):
					callback([f](Command*command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config),Tokenizer::dequote(arguments[4],config),Tokenizer::dequote(arguments[5],config),Tokenizer::dequote(arguments[6],config));})

	{
		construct(name,description,completion);
	}


	void Command::construct(const String&name, const String&description, eCommandCompletion completion)
	{
		this->name = name;
		this->description = description;
		this->completion = completion;
	}

	void		Command::invoke(const StringList&arguments, const Tokenizer::Config&config)
	{
		callback(this,arguments,config);
	}


	Variable::Variable(const String&name_, const String&type_, count_t components_, UINT32 protection_):name(name_),type(type_),components(components_),protection(protection_)
	{}





	

	Interpretor::Interpretor():Folder(NULL,""),exec_on_variable_call(NULL),focused(this),executing(NULL),executing_folder(NULL)
	{
		lookup.path_config.main_separator="/";
		lookup.segment_config.quotations = "\"'";
		lookup.segment_config.main_separator = " ";
		lookup.segment_config.quotation_escape_character = '\\';
		lookup.segment_config.recursion_break = false;
		lookup.line_config.quotations = "\"'";
		lookup.line_config.main_separator = "\n";
		lookup.line_config.quotation_escape_character = '\\';
		lookup.line_config.recursion_break = false;
		
	}
	
	
	Command*	Interpretor::defineCommand(const String& def, Command*cmd)
	{
		if (!cmd)
			return NULL;
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
			return NULL;
		cmd->description = items.first();
		if (items > 1)
			cmd->description += " <"+items.fuse(1,"> <")+">";
		
		cmd->name = items.first();
		parent->commands.insert(cmd);
		return cmd;
	}
	
	Command*	Interpretor::defineGlobalCommand(const String& def, Command*cmd)
	{
		if (!cmd)
			return NULL;
		StringList	items(def);
		if (!items)
			return NULL;
		detail::makeValidName(items.first());
		Command*result = global_commands.lookup(items.first());
		if (result)
			return NULL;
		cmd->description = items.first();
		if (items > 1)
			cmd->description += " <"+items.fuse(1,"> <")+">";
		
		cmd->name = items.first();
		
		global_commands.insert(cmd);
		return cmd;
	}
	

	bool	Interpretor::entryLookup(const String&name, bool may_exist)
	{
		lookup.folder = NULL;
		lookup.variable = NULL;
		lookup.command = NULL;
		Tokenizer::tokenize(name,lookup.path_config,lookup.path_segments);
		if (!lookup.path_segments)
		{
			error = "Path empty";
			return false;
		}
		detail::makeValidName(lookup.path_segments.last());
		lookup.folder = resolve(name.beginsWith('/'));
		if (!lookup.folder)
		{
			error = "Unable to resolve parent folder";
			return false;
		}
		lookup.variable = lookup.folder->variables.lookup(lookup.path_segments.last());
		lookup.command = lookup.folder->commands.lookup(lookup.path_segments.last());
		if (!may_exist)
			if (lookup.variable)
			{
				error = "Variable '"+lookup.path_segments.last()+"' already defined";
				return false;
			}
			elif (lookup.command)
			{
				error = "Command '"+lookup.path_segments.last()+"' already defined";
				return false;
			
			}
		return true;
			
	}

	Interpretor::Variable*	Interpretor::setString(const String& name,	unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new StringVariable(lookup.path_segments.last(),"",protection)));
		
	}

	Interpretor::Variable*	Interpretor::setString(const String& name, const String& value, unsigned protection, bool allow_overwrite)
	{
		if (!entryLookup(name,allow_overwrite))
			return NULL;

		if (lookup.command)
		{
			error = "A command of that name ('"+lookup.path_segments.last()+"') already exists";
			return NULL;
		}
		
		if (!lookup.variable)
		{
			return lookup.folder->variables.insert(SHIELDED(new StringVariable(lookup.path_segments.last(),value,protection)));
		}
		else
		{
			return lookup.variable->set(value)?lookup.variable:NULL;
		}
	}


	Interpretor::Variable*	Interpretor::setFloat(const String& name,	unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		
		return lookup.folder->variables.insert(SHIELDED(new FloatVariable(lookup.path_segments.last(),0,protection)));
	}

	Interpretor::Variable*	Interpretor::setFloat(const String& name, float value, unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new FloatVariable(lookup.path_segments.last(),value,protection)));
	}

	
	Interpretor::Variable*	Interpretor::setInt(const String& name,	unsigned protection)
	{
	
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new IntVariable(lookup.path_segments.last(),0,protection)));
	}

	Interpretor::Variable*	Interpretor::setInt(const String& name, int value, unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new IntVariable(lookup.path_segments.last(),value,protection)));
	}

	Interpretor::Variable*	Interpretor::setBool(const String& name,	unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new BoolVariable(lookup.path_segments.last(),false,protection)));
	}

	Interpretor::Variable*	Interpretor::setBool(const String& name, bool value, unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new BoolVariable(lookup.path_segments.last(),value,protection)));
	}

	
	
	Interpretor::Variable*	Interpretor::setVector3f(const String& name, unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new VectorVariable<3>(lookup.path_segments.last(),NULL,protection)));
	}
	
	Interpretor::Variable*	Interpretor::setVector4f(const String& name, unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new VectorVariable<4>(lookup.path_segments.last(),NULL,protection)));
	}
	
	Interpretor::Variable*	Interpretor::setVector3d(const String& name, unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new DoubleVectorVariable<3>(lookup.path_segments.last(),NULL,protection)));
	}
	
	Interpretor::Variable*	Interpretor::setVector4d(const String& name, unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new DoubleVectorVariable<4>(lookup.path_segments.last(),NULL,protection)));
	}
	
	
	Interpretor::Variable*	Interpretor::setVector3f(const String& name, float value[3], unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new VectorVariable<3>(lookup.path_segments.last(),value,protection)));
	}
	
	Interpretor::Variable*	Interpretor::setVector4f(const String& name, float value[4], unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new VectorVariable<4>(lookup.path_segments.last(),value,protection)));
	}
	
	Interpretor::Variable*	Interpretor::setVector3d(const String& name, double value[3], unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new DoubleVectorVariable<3>(lookup.path_segments.last(),value,protection)));
	}
	
	Interpretor::Variable*	Interpretor::setVector4d(const String& name, double value[3], unsigned protection)
	{
		if (!entryLookup(name,false))
			return NULL;
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.insert(SHIELDED(new DoubleVectorVariable<4>(lookup.path_segments.last(),value,protection)));
	}
	

	Interpretor::Variable*	Interpretor::setVariable(Variable*v)
	{
		return focused->setVariable(v);
	}


	Interpretor::Variable*	Folder::setVariable(Variable*v)
	{
		for (index_t i = 0; i < v->name.length(); i++)
			if (!isAllowedCharacter(v->name.get(i)))
				throw Program::ParameterFault("Character 0x"+IntToHex((int)v->name.get(i),2)+" ('"+v->name.get(i)+"') of variable '"+v->name+"' not allowed");

		if (variables.lookup(v->name))
			throw Program::UniquenessViolation("Variable '"+v->name+"' already defined in context '"+path()+"'");
		return variables.insert(v);
	}
	
	
	
	
	
	
	
	
	
	Interpretor::Variable*	Interpretor::set(const String& name, const String& value)
	{
		if (!entryLookup(name,true))
			return NULL;
			
		if (lookup.command || global_commands.lookup(lookup.path_segments.last()))
		{
			error = "The requested variable name ('"+lookup.path_segments.last()+"') is already assigned to a command";
			return NULL;
		}
		
		if (lookup.variable)
		{
			if (lookup.variable->protection&WriteProtection)
			{
				error = "Variable '"+name+"' is write-protected";
				return	NULL;
			}
			if (!lookup.variable->set(value))
			{
				error = "Assigment failed";
				return NULL;
			}
			return lookup.variable;
		}
		
		int	ival;
		float vval;
		
		
		if (convertToInt(value.c_str(),ival))
			return lookup.folder->variables.insert(SHIELDED(new IntVariable(lookup.path_segments.last(),ival)));
		
		if (convertToFloat(value.c_str(),vval))
			return lookup.folder->variables.insert(SHIELDED(new FloatVariable(lookup.path_segments.last(),vval)));
		
		VectorVariable<3>	f3(lookup.path_segments.last());
		VectorVariable<4>	f4(lookup.path_segments.last());
		
		if (f3.set(value))
			return lookup.folder->variables.insert(SHIELDED(new VectorVariable<3>(f3)));
		if (f4.set(value))
			return lookup.folder->variables.insert(SHIELDED(new VectorVariable<4>(f4)));
			
		return lookup.folder->variables.insert(SHIELDED(new StringVariable(lookup.path_segments.last(),value)));
	}

	
	
	
	

	Folder*	Interpretor::resolve(bool from_root)
	{
		if (!lookup.path_segments)
		{
			error = "Missing argument";
			return NULL;
		}
		
		Folder*current = from_root?this:focused;

		for	(index_t i = 0; i < lookup.path_segments-1; i++)
		{
			if	(lookup.path_segments[i] == '.')
				continue;
			if	(lookup.path_segments[i] == "..")
			{
				if	(!current->parent)
				{
					error = "Can't exit folder. Folder is root";
					return	NULL;
				}
				current = current->parent;
			}
			elif (lookup.path_segments[i] == '~' || lookup.path_segments[i] == '-')
			{
				if	(!current->parent)
				{
					error = "Can't exit folder. Folder is root";
					return	NULL;
				}
				while (current->parent->parent)
					current = current->parent;
			}
			else
			{
				Folder*folder = current->folders.lookup(lookup.path_segments[i]);
				if	(!folder)
				{
					error = "Folder does not exist: '"+lookup.path_segments[i]+"'";
					return	NULL;
				}
				current = folder;
			}
		}
		return	current;
	}
	
	Folder*	Interpretor::resolveFull(bool from_root)
	{
		if (!lookup.path_segments)
		{
			error = "Missing argument";
			return NULL;
		}
		
		Folder*current = from_root?this:focused;

		for	(index_t i = 0; i < lookup.path_segments; i++)
		{
			if	(lookup.path_segments[i] == '.')
				continue;
			if	(lookup.path_segments[i] == "..")
			{
				if	(!current->parent)
				{
					error = "Can't exit folder. Folder is root";
					return	NULL;
				}
				current = current->parent;
			}
			elif (lookup.path_segments[i] == '~' || lookup.path_segments[i] == '-')
			{
				if	(!current->parent)
				{
					error = "Can't exit folder. Folder is root";
					return	NULL;
				}
				while (current->parent->parent)
					current = current->parent;
			}
			else
			{
				Folder*folder = current->folders.lookup(lookup.path_segments[i]);
				if	(!folder)
				{
					error = "Folder does not exist: '"+lookup.path_segments[i]+"'";
					return	NULL;
				}
				current = folder;
			}
		}
		return	current;
	}

	bool	Interpretor::unset(const String&	name)
	{
		if (!entryLookup(name,true))
			return false;
		if (!lookup.variable)
		{
			error = "Variable not defined: '"+lookup.path_segments.last()+"'";
			return false;
		}
	
		
		
		
		if (lookup.variable->protection&EraseProtection)
		{
			error = "Variable '"+name+"' is protected";
			return false;
		}
		index_t index = lookup.folder->variables.getIndexOf(lookup.variable);
		ASSERT__(index>0);
		
		lookup.folder->variables.erase(index-1);
		return true;
	}


	void	Interpretor::unsetIgnoreProtection(const String&	name)
	{
		if (!entryLookup(name,true) || !lookup.variable)
			return;
		
		
		index_t	index = lookup.folder->variables.getIndexOf(lookup.variable);
		if	(!index)
			return;
		
		lookup.folder->variables.erase(index-1);
	}

	bool	Interpretor::unset(Folder*folder, Variable*var, bool ignore_protection)
	{
		if (!var || !folder || !folder->variables(var))
		{
			error = "Parameters invalid";
			return false;
		}
		if (ignore_protection || !(var->protection&EraseProtection))
		{
			folder->variables.erase(var);
			return true;
		}
		error = "'"+var->name+"' is protected";
		return false;
	}



	unsigned	Interpretor::getAsUnsigned(const String&	name,	unsigned	exception)
	{
		Variable*v = findVar(name);
		if (!v)
			return exception;
		if (v->type == "Integer")
			return (unsigned)((IntVariable*)v)->content;
		if (v->type == "Float")
			return (unsigned)((FloatVariable*)v)->content;

		unsigned val;
		if (!convert(v->toString().c_str(),val))
			return exception;
		return val;
	}

	int	Interpretor::getAsInt(const String&	name,	int	exception)
	{
		Variable*v = findVar(name);
		if (!v)
			return exception;
		if (v->type == "Integer")
			return (int)((IntVariable*)v)->content;
		if (v->type == "Float")
			return (int)((FloatVariable*)v)->content;
		int val;
		if (!convert(v->toString().c_str(),val))
			return exception;
		return val;
	}

	float	Interpretor::getAsFloat(const String&	name,	float	exception)
	{
		Variable*v = findVar(name);
		if (!v)
			return exception;
		if (v->type == "Integer")
			return ((IntVariable*)v)->content;
		if (v->type == "Float")
			return ((FloatVariable*)v)->content;
		float val;
		if (!convert(v->toString().c_str(),val))
			return exception;
		return val;
	}

	Key::Name	Interpretor::getAsKey(const String&	name,	Key::Name	exception)
	{
		Variable*v = findVar(name);
		if (!v)
			return exception;
		Key::Name val;
		if (!convert(v->toString().c_str(),val))
			return exception;
		return val;
	}

	bool	Interpretor::getAsBool(const String&	name,	bool	exception)
	{
		Variable*v = findVar(name);
		if (!v)
			return exception;
		if (v->type == "Integer")
			return ((IntVariable*)v)->content!=0;
		if (v->type == "Float")
			return ((FloatVariable*)v)->content!=0;
		bool val;
		if (!convert(v->toString().c_str(),val))
			return exception;
		return val;
	}


	const String& Interpretor::getAsString(const String& name, const String& exception)
	{
		static String content;
		Variable*v = findVar(name);
		if (!v)
			return exception;
		content = v->toString();
		return content;
	}

	const	String&	Interpretor::getErrorStr()	const
	{
		return	error;
	}

	const	String&	Interpretor::getError()	const
	{
		return	error;
	}


	Interpretor::Command*Interpretor::find(const String&name,Folder**folder_out/*=NULL*/)
	{
		Tokenizer::tokenize(name,lookup.path_config,lookup.path_segments);
		
		if (!lookup.path_segments)
			return NULL;
		Folder*folder = resolve(name.beginsWith('/'));
		if (!folder)
			return NULL;
		Command*rs= folder->commands.lookup(lookup.path_segments.last());
		if (rs)
		{
			if (folder_out)
				(*folder_out) = folder;
			return rs;
		}
		if (lookup.path_segments==1)
		{
			if (folder_out)
				(*folder_out) = this;
			return global_commands.lookup(lookup.path_segments.first());
		}
		return NULL;
	}

	Interpretor::Command*Interpretor::find(const char*name,Folder**folder_out/*=NULL*/)
	{
		return find(String(name),folder_out);
	}

	Interpretor::Variable*Interpretor::findVar(const String&name,Folder**folder_out/*=NULL*/)
	{
		Tokenizer::tokenize(name,lookup.path_config,lookup.path_segments);
		if (!lookup.path_segments)
			return NULL;
		Folder*folder = resolve(name.beginsWith('/'));
		if (!folder)
			return NULL;
		if (folder_out)
			(*folder_out) = folder;
		return folder->variables.lookup(lookup.path_segments.last());
	}

	Interpretor::Variable*Interpretor::findVar(const char*name,Folder**folder_out/*=NULL*/)
	{
		Tokenizer::tokenize(name,lookup.path_config,lookup.path_segments);
		if (!lookup.path_segments)
			return NULL;
		Folder*folder = resolve(name[0]=='/');
		if (!folder)
			return NULL;
		if (folder_out)
			(*folder_out) = folder;
		return folder->variables.lookup(lookup.path_segments.last());
	}

	Folder*		Interpretor::findFolder(const String&path)
	{
		Tokenizer::tokenize(path,lookup.path_config,lookup.path_segments);
		if (!lookup.path_segments)
		{
			if (path == '/')
				return this;
			error = "No segments given in '"+path+"'";
			return NULL;
		}
		lookup.path_segments<<".";
		return resolve(path.beginsWith('/'));
	}


	bool	Interpretor::interpret(const String&line, bool allow_global_commands/*=true*/)
	{
		return	parse(line.c_str(),allow_global_commands);
	}
	
	bool	Interpretor::interpret(const char*line, bool allow_global_commands/*=true*/)
	{
		return	parse(line,allow_global_commands);
	}

	bool	Interpretor::parse(const String&line, bool allow_global_commands/*=true*/)
	{
		return	parse(line.c_str(),allow_global_commands);
	}

	Interpretor::Command*		Interpretor::getExecutingCommand()	const
	{
		return executing;
	}

	Folder*		Interpretor::getExecutionContext()	const
	{
		return executing_folder;
	}

	String Interpretor::complete(const String&line, StringList&out)
	{
		out.clear();
		String longest_common;
		Tokenizer::tokenize(line,lookup.path_config,lookup.path_segments);
		
		if (!lookup.path_segments)
			return "";
			
		Folder*parent = resolve(line.beginsWith('/'));
		if (!parent)
			return "";
		
		String prefix = lookup.path_segments.fuse(0,lookup.path_segments-1,'/'),
				inner = lookup.path_segments.last();
		if (prefix.length())
			prefix+= "/";
				
		if (line.beginsWith('/'))
			prefix = "/"+prefix;
		
		if (lookup.path_segments==1)
			for (index_t i = 0; i < global_commands; i++)
				if (global_commands[i]->name.beginsWith(inner))
					out << prefix+global_commands[i]->name;
		
		for (index_t i = 0; i < parent->commands; i++)
			if (parent->commands[i]->name.beginsWith(inner))
				out << prefix+parent->commands[i]->name;
		for (index_t i = 0; i < parent->variables; i++)
			if (parent->variables[i]->name.beginsWith(inner))
				out << prefix+parent->variables[i]->name;
		for (index_t i = 0; i < parent->folders; i++)
			if (parent->folders[i]->name.beginsWith(inner))
				out << prefix+parent->folders[i]->name+"/";

		if (!out.count())
			return "";
		
		if (out.count() == 1)
		{
			return out.first();
		}
		count_t longest = prefix.length()+1;
		bool same(true);
		while (same)
		{
			longest_common = out.first().subString(0,longest);
			if (longest>longest_common.length())
				same = false;
			for (index_t i = 1; i < out.count() && same; i++)
				same = out[i].beginsWith(longest_common);
			if (same)
				longest++;
		}
		longest_common.erase(longest-1);
		
		
		return longest_common;
	}

	String Interpretor::completeVariables(const String&line,StringList&out)
	{
		out.clear();
		String longest_common;
		Tokenizer::tokenize(line,lookup.path_config,lookup.path_segments);
		
		if (!lookup.path_segments)
			return "";
			
		Folder*parent = resolve(line.beginsWith('/'));
		if (!parent)
			return "";
		
		String prefix = lookup.path_segments.fuse(0,lookup.path_segments-1,'/'),
				inner = lookup.path_segments.last();
		if (prefix.length())
			prefix+= "/";
		if (line.beginsWith('/'))
			prefix = "/"+prefix;
		
		for (index_t i = 0; i < parent->variables; i++)
			if (parent->variables[i]->name.beginsWith(inner))
				out << prefix+parent->variables[i]->name;
		for (index_t i = 0; i < parent->folders; i++)
			if (parent->folders[i]->name.beginsWith(inner))
				out << prefix+parent->folders[i]->name+'/';
		

		if (!out.count())
			return "";
		
		if (out.count() == 1)
		{
			return out.first();
		}
		count_t longest = prefix.length()+1;
		bool same(true);
		while (same)
		{
			longest_common = out.first().subString(0,longest);
			if (longest>longest_common.length())
				same = false;
			for (index_t i = 1; i < out.count() && same; i++)
				same = out[i].beginsWith(longest_common);
			if (same)
				longest++;
		}
		longest_common.erase(longest-1);
		
		return longest_common;
	}



	String Interpretor::completeFolders(const String&line, StringList&out)
	{
		out.clear();
		String longest_common;
		Tokenizer::tokenize(line,lookup.path_config,lookup.path_segments);
		
		if (!lookup.path_segments)
			return "";
			
		Folder*parent = resolve(line.beginsWith('/'));
		if (!parent)
			return "";
		
		String prefix = lookup.path_segments.fuse(0,lookup.path_segments-1,'/'),
				inner = lookup.path_segments.last();
		if (prefix.length())
			prefix+= "/";

		if (line.beginsWith('/'))
			prefix = "/"+prefix;
		
		for (index_t i = 0; i < parent->folders; i++)
			if (parent->folders[i]->name.beginsWith(inner))
				out << prefix+parent->folders[i]->name+"/";
		

		if (!out.count())
			return "";
		
		if (out.count() == 1)
		{
			return out.first();
		}
		count_t longest = prefix.length()+1;
		bool same(true);
		while (same)
		{
			longest_common = out.first().subString(0,longest);
			if (longest>longest_common.length())
				same = false;
			for (index_t i = 1; i < out.count() && same; i++)
				same = out[i].beginsWith(longest_common);
			if (same)
				longest++;
		}
		longest_common.erase(longest-1);
		
		return longest_common;
	}

	const StringList&	Interpretor::getExecutionSegments()		const
	{
		return lookup.cmd_segments;
	}
	
	bool	Interpretor::parseLine(const char*line,bool allow_global_commands)
	{
		//cout << "parse("<<line<<")"<<endl;
		Tokenizer::tokenize(line,lookup.segment_config,lookup.cmd_segments);
		if (!lookup.cmd_segments)
			return true;
			
		Tokenizer::tokenize(lookup.cmd_segments[0],lookup.path_config,lookup.path_segments);
		if	(!lookup.path_segments)
			return	true;	//this would be rather odd...
			
		executing_folder = resolve(line[0]=='/');
		if	(!executing_folder)
			return	false;
		
		//cout << "cmd_segments:"<<endl;
		//lookup.cmd_segments.printLines(cout," ");
		//cout << "path_segments:"<<endl;
		//lookup.path_segments.printLines(cout," ");

		if	(lookup.cmd_segments == 1)
			if (Variable*v = executing_folder->variables.lookup(lookup.path_segments.last()))
			{
				if (exec_on_variable_call)
					exec_on_variable_call(v);
				return true;
			}
		
		executing = executing_folder->commands.lookup(lookup.path_segments.last());
		if (!executing && allow_global_commands)
			executing = global_commands.lookup(lookup.path_segments.last());
		
		if (executing)
		{
			executing->invoke(lookup.cmd_segments, lookup.segment_config);
			executing = NULL;
			executing_folder = NULL;
			return true;
		}
		
		executing_folder = NULL;
		error = "Command or Variable unknown: '"+lookup.path_segments.last()+"'";
		return false;
	}

	bool	Interpretor::parse(const char*line, bool allow_global_commands /*=true*/)
	{
		StringList	lines;
		Tokenizer::tokenize(line,lookup.line_config,lines);
		for	(index_t i = 0; i < lines;	i++)
			if	(!parseLine(lines[i].c_str(),allow_global_commands))
				return	false;
		return	true;
	}
	
	void			Folder::attach(Attachable*attachable,const Signature&signature)
	{
		attachment.reference = attachable;
		attachment.signature = &signature;
	}
	
	
	Attachable*	Folder::getAttachment(const Signature&signature)
	{
		if (attachment.signature == &signature)
			return attachment.reference;
		return NULL;
	}


	count_t	Folder::countCommands() const
	{
		count_t rs = commands.count();
		for (index_t i = 0; i < folders; i++)
			rs += folders[i]->countCommands();
		return	rs;
	}

	count_t	Folder::countVariables() const
	{
		count_t rs = variables.count();
		for (index_t i = 0; i < folders; i++)
			rs += folders[i]->countVariables();
		return	rs;
	}


	Folder::Folder(Folder*p,const String&n):parent(p),name(n),custom_flags(0)
	{}

	String Folder::path()	const
	{
		String rs = name;
		const Folder*folder = parent;
		while (folder)
		{
			if (folder->parent || folder->name.length())
				rs = folder->name+'/'+rs;
			folder = folder->parent;
		}
		return rs;
	}
	
	String Folder::path(unsigned max_depth)	const
	{
		String rs = name;
		const Folder*folder = parent;
		unsigned depth(0);
		while (folder && ++depth<max_depth)
		{
			if (folder->parent || folder->name.length())
			{
				//cout << "////concatenating '"<<folder->name<<"' and '"<<rs<<"'"<<endl;
				rs = folder->name+'/'+rs;
				//cout << "////rs now '"<<rs<<"'"<<endl;
			}
			folder = folder->parent;
		}
		if (folder && (folder->parent || folder->name.length()))
			rs = "(...)/"+rs;
		return rs;
	}
	
	bool Folder::isChildOf(const Folder*other)	const
	{
		if (!other)
			return false;
		if (other == this)
			return true;
		const Folder*folder = this;
		while (folder->parent && folder->parent != other)
			folder = folder->parent;
		return folder->parent == other;
	}


	Folder*		Interpretor::enterNewFolder(const String& name, bool enter_existing)
	{
		Tokenizer::tokenize(name,lookup.path_config,lookup.path_segments);
		if (!lookup.path_segments)
			return NULL;
		
		Folder*parent = resolve(name.beginsWith('/'));
		if (!parent)
			return NULL;
			
		Folder*result = parent->folders.lookup(lookup.path_segments.last());
		if (result)
		{
			if (enter_existing)
				focused = result;
			return NULL;
		}
			
		result = SHIELDED(new Folder(parent,lookup.path_segments.last()));
		parent->folders.insert(result);
		focused = result;
		return result;
	}

	void				Interpretor::eraseFolder(Folder*folder)
	{
		if (!folder)
			return;
		Folder*parent = folder->parent;
		
		if (focused->isChildOf(parent))
			focused = parent;
		
		parent->folders.erase(folder);
	}
	
	void				Interpretor::eraseFolder(const String& name)
	{
		Tokenizer::tokenize(name,lookup.path_config,lookup.path_segments);
		if (!lookup.path_segments)
			return;
			
		if (Folder*parent = resolve(name.beginsWith('/')))
		{
			if (focused->isChildOf(parent))
				focused = parent;
			parent->folders.eraseByIdent(lookup.path_segments.last());
		}
	}


	bool	Interpretor::moveFocus(const String&context)
	{
		Tokenizer::tokenize(context,lookup.path_config,lookup.path_segments);
		lookup.path_segments << ".";	//dummy
		Folder*new_focus = resolve(context.beginsWith('/'));
		if (new_focus)
		{
			focused = new_focus;
			return true;
		}
		return false;
	}

	bool	Interpretor::exitFolder()
	{
		return exitFocus();
	}
	
	bool	Interpretor::exitFocus()
	{
		if (focused->parent)
		{
			focused = focused->parent;
			return true;
		}
		return false;
	}

	void	Interpretor::resetFocus()
	{
		focused = this;
	}

	Folder*	Interpretor::getFocus()
	{
		return focused;
	}
	
	void		Interpretor::setFocus(Folder*folder)
	{
		focused = folder;
	}

	void		Interpretor::pushFocus()
	{
		focus_stack << focused;
	}

	void		Interpretor::pushAndReplaceFocus(Folder*new_focus)
	{
		pushFocus();
		if (new_focus)
			setFocus(new_focus);
	}


	void		Interpretor::popFocus()
	{
		ASSERT__(focus_stack.isNotEmpty());
		setFocus(focus_stack.pop());
	}


	const Folder*	Interpretor::getFocus() const
	{
		return focused;
	}
	
	String			Interpretor::getContext(unsigned depth)	const
	{
		return focused->path(depth);
	}

	const Tokenizer::Config&	Interpretor::getSegmentizerConfig()	const
	{
		return lookup.segment_config;
	}


	Script::Script(const	String&Name):StringList(64),name(Name)
	{}

	Script::Script(const	String&Name,	const	String&filename):StringList(64),name(Name)
	{
		load(filename);
	}

	void	Script::loadFromStream(char*buffer,	size_t len)
	{
		char*pntr=buffer;
		do
		{
			char*next = strstr(pntr,"\n");
			if	(next)
			{
				if	(pntr[next-pntr-1] == 13)
					pntr[next-pntr-1] = 0;
				else
					next[0] = 0;
			}
			add(pntr);
			if	(next)
				pntr = &next[1];
			else
				pntr = NULL;
		}
		while	(pntr);
	}

	bool	Script::load(const	String&filename)
	{
		clear();
		FILE*f = fopen(filename.c_str(),"rb");
		if	(!f)
			return	false;
		fseek(f,0,SEEK_END);
		size_t size = ftell(f);
		fseek(f,0,SEEK_SET);
		char*buffer = SHIELDED_ARRAY(new char[size+1],size+1);
		fread(buffer,size,1,f);
		buffer[size] = 0;
		fclose(f);
		loadFromStream(buffer,size+1);
		DISCARD_ARRAY(buffer);
		return	true;
	}


	bool	ScriptList::load(const	String&filename,	const	String&alias)
	{
		Script*script = find(alias);
		if	(!script)
			script = append(SHIELDED(new	Script(alias)));
		return	script->load(filename);
	}

	bool	ScriptList::erase(const	String&alias)
	{
		for	(index_t i = 0; i < count(); i++)
			if (get(i)->name == alias)
			{
				erase(i);
				return	true;
			}
		return	false;
	}


	Script*ScriptList::find(const	String&alias)
	{
		for	(index_t i = 0;	i < count(); i++)
			if (get(i)->name == alias)
				return	get(i);
		return	NULL;
	}

	bool	ScriptList::execute(const	String&alias,Interpretor*parser)
	{
		Script*script = find(alias);
		if	(!script)
			return	false;
		for	(index_t i = 0;	i < script->count(); i++)
			parser->parse(script->get(i));
		return	true;
	}
}
