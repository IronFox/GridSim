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


	Command::Command(const Name&name):callback([](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config) {}),
						NamedItem(name.coreName), fullSpecification(name.fullSpecification)
	{
		construct(NoCompletion);
	}

	Command::Command(const Name&name, const full_function_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback(f)
	{
		construct(completion);
	}
	Command::Command(const Name&name, const function0_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f();})
	{
		construct(completion);

	}
	Command::Command(const Name&name, const function1_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config));})

	{
		construct(completion);
	}
	Command::Command(const Name&name, const function2_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config));})

	{
		construct(completion);
	}
	Command::Command(const Name&name, const function3_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config));})

	{
		construct(completion);
	}
	Command::Command(const Name&name, const function4_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config),Tokenizer::dequote(arguments[4],config));})

	{
		construct(completion);
	}

	Command::Command(const Name&name, const function5_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config),Tokenizer::dequote(arguments[4],config),Tokenizer::dequote(arguments[5],config));})

	{
		construct(completion);
	}
	Command::Command(const Name&name, const function6_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config),Tokenizer::dequote(arguments[4],config),Tokenizer::dequote(arguments[5],config),Tokenizer::dequote(arguments[6],config));})

	{
		construct(completion);
	}


	Command::Command(const Name&name, const full_func_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback(f)
	{
		construct(completion);
	}
	Command::Command(const Name&name, const func0_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f();})
	{
		construct(completion);
	}
	Command::Command(const Name&name, const func1_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config));})

	{
		construct(completion);
	}
	Command::Command(const Name&name, const func2_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config));})

	{
		construct(completion);
	}
	Command::Command(const Name&name, const func3_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config));})

	{
		construct(completion);
	}
	Command::Command(const Name&name, const func4_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config),Tokenizer::dequote(arguments[4],config));})

	{
		construct(completion);
	}

	Command::Command(const Name&name, const func5_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config),Tokenizer::dequote(arguments[4],config),Tokenizer::dequote(arguments[5],config));})

	{
		construct(completion);
	}
	Command::Command(const Name&name, const func6_t&f, eCommandCompletion completion):
					NamedItem(name.coreName), fullSpecification(name.fullSpecification),
					callback([f](const PCommand&command, const StringList&arguments, const Tokenizer::Config&config){f(Tokenizer::dequote(arguments[1],config),Tokenizer::dequote(arguments[2],config),Tokenizer::dequote(arguments[3],config),Tokenizer::dequote(arguments[4],config),Tokenizer::dequote(arguments[5],config),Tokenizer::dequote(arguments[6],config));})

	{
		construct(completion);
	}


	void Command::construct(eCommandCompletion completion)
	{
		this->completion = completion;
	}

	void		Command::Invoke(const StringList&arguments, const Tokenizer::Config&config)
	{
		callback(shared_from_this(),arguments,config);
	}


	Variable::Variable(const String&name_, const String&type_, count_t components_, UINT32 protection_):NamedItem(name_),type(type_),components(components_),protection(protection_)
	{}





	

	Interpretor::Interpretor():root(new Folder(PFolder(),""))
	{
		focused = root;
		lookup.pathConfig.main_separator="/";
		lookup.segmentConfig.quotations = "\"'";
		lookup.segmentConfig.main_separator = " ";
		lookup.segmentConfig.quotation_escape_character = '\\';
		lookup.segmentConfig.recursion_break = false;
		lookup.lineConfig.quotations = "\"'";
		lookup.lineConfig.main_separator = "\n";
		lookup.lineConfig.quotation_escape_character = '\\';
		lookup.lineConfig.recursion_break = false;
		
	}
	
	bool Command::Name::Parse(const String&def)
	{
		StringList	items(def);
		if (!items)
			return false;
		detail::makeValidName(items.first());
		fullSpecification = items.first();
		if (items > 1)
			fullSpecification += " <"+items.fuse(1,"> <")+">";
		
		coreName = items.first();
		return true;
	}
	


	bool	Interpretor::InsertCommand(const PCommand&cmd, const String&targetPath)
	{
		if (!cmd)
			return false;
		PFolder folder = FindFolder(targetPath);
		if (!folder)
			return false;

		PCommand result = folder->commands.Query(cmd->name);
		if (result)
			return false;
		folder->commands.Set(cmd->name, cmd);
		return true;
	}
	
	bool	Interpretor::InsertGlobalCommand(const PCommand&cmd)
	{
		if (!cmd)
			return false;
		PCommand result = globalCommands.Query(cmd->name);
		if (result)
			return false;
		globalCommands.Set(cmd->name, cmd);
		return true;
	}
	

	bool	Interpretor::_EntryLookup(const String&name, bool may_exist)
	{
		lookup.folder.reset();
		lookup.variable.reset();
		lookup.command.reset();
		Tokenizer::tokenize(name,lookup.pathConfig,lookup.pathSegments);
		if (!lookup.pathSegments)
		{
			error = "Path empty";
			return false;
		}
		detail::makeValidName(lookup.pathSegments.last());
		lookup.folder = _Resolve(name.beginsWith('/'));
		if (!lookup.folder)
		{
			error = "Unable to _Resolve parent folder";
			return false;
		}
		lookup.variable = lookup.folder->variables.Query(lookup.pathSegments.last());
		lookup.command = lookup.folder->commands.Query(lookup.pathSegments.last());
		if (!may_exist)
			if (lookup.variable)
			{
				error = "Variable '"+lookup.pathSegments.last()+"' already defined";
				return false;
			}
			elif (lookup.command)
			{
				error = "Command '"+lookup.pathSegments.last()+"' already defined";
				return false;
			
			}
		return true;
			
	}

	PVariable	Interpretor::SetString(const String& name,	unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(), new StringVariable(lookup.pathSegments.last(),"",protection));
		
	}

	PVariable	Interpretor::SetString(const String& name, const String& value, unsigned protection, bool allow_overwrite)
	{
		if (!_EntryLookup(name,allow_overwrite))
			return PVariable();

		if (lookup.command)
		{
			error = "A command of that name ('"+lookup.pathSegments.last()+"') already exists";
			return PVariable();
		}
		
		if (!lookup.variable)
		{
			return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new StringVariable(lookup.pathSegments.last(),value,protection));
		}
		else
		{
			return lookup.variable->Set(value)?lookup.variable:PVariable();
		}
	}


	PVariable	Interpretor::SetFloat(const String& name,	unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT__(lookup.variable);
		ASSERT__(lookup.command);
		
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new FloatVariable(lookup.pathSegments.last(),0,protection));
	}

	PVariable	Interpretor::SetFloat(const String& name, float value, unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new FloatVariable(lookup.pathSegments.last(),value,protection));
	}

	
	PVariable	Interpretor::SetInt(const String& name,	unsigned protection)
	{
	
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new IntVariable(lookup.pathSegments.last(),0,protection));
	}

	PVariable	Interpretor::SetInt(const String& name, int value, unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new IntVariable(lookup.pathSegments.last(),value,protection));
	}

	PVariable	Interpretor::SetBool(const String& name,	unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new BoolVariable(lookup.pathSegments.last(),false,protection));
	}

	PVariable	Interpretor::SetBool(const String& name, bool value, unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new BoolVariable(lookup.pathSegments.last(),value,protection));
	}

	
	
	PVariable	Interpretor::SetVector3f(const String& name, unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new VectorVariable<3>(lookup.pathSegments.last(),NULL,protection));
	}
	
	PVariable	Interpretor::SetVector4f(const String& name, unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new VectorVariable<4>(lookup.pathSegments.last(),NULL,protection));
	}
	
	PVariable	Interpretor::SetVector3d(const String& name, unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new DoubleVectorVariable<3>(lookup.pathSegments.last(),NULL,protection));
	}
	
	PVariable	Interpretor::SetVector4d(const String& name, unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new DoubleVectorVariable<4>(lookup.pathSegments.last(),NULL,protection));
	}
	
	
	PVariable	Interpretor::SetVector3f(const String& name, float value[3], unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new VectorVariable<3>(lookup.pathSegments.last(),value,protection));
	}
	
	PVariable	Interpretor::SetVector4f(const String& name, float value[4], unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new VectorVariable<4>(lookup.pathSegments.last(),value,protection));
	}
	
	PVariable	Interpretor::SetVector3d(const String& name, double value[3], unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new DoubleVectorVariable<3>(lookup.pathSegments.last(),value,protection));
	}
	
	PVariable	Interpretor::SetVector4d(const String& name, double value[3], unsigned protection)
	{
		if (!_EntryLookup(name,false))
			return PVariable();
		ASSERT_IS_NULL__(lookup.variable);
		ASSERT_IS_NULL__(lookup.command);
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),new DoubleVectorVariable<4>(lookup.pathSegments.last(),value,protection));
	}
	

	PVariable	Interpretor::SetVariable(const PVariable&v)
	{
		return focused->SetVariable(v);
	}


	PVariable	Folder::SetVariable(const PVariable&v)
	{
		for (index_t i = 0; i < v->name.length(); i++)
			if (!isAllowedCharacter(v->name.get(i)))
				throw Program::ParameterFault("Character 0x"+IntToHex((int)v->name.get(i),2)+" ('"+v->name.get(i)+"') of variable '"+v->name+"' not allowed");

		if (variables.IsSet(v->name))
			throw Program::UniquenessViolation("Variable '"+v->name+"' already defined in context '"+GetPath()+"'");
		variables.Insert(v->name, v);
		return v;
	}
	
	
	
	
	
	
	
	
	
	PVariable	Interpretor::Set(const String& name, const String& value)
	{
		if (!_EntryLookup(name,true))
			return PVariable();
			
		if (lookup.command || globalCommands.IsSet(lookup.pathSegments.last()))
		{
			error = "The requested variable name ('"+lookup.pathSegments.last()+"') is already assigned to a command";
			return PVariable();
		}
		
		if (lookup.variable)
		{
			if (lookup.variable->protection&WriteProtection)
			{
				error = "Variable '"+name+"' is write-protected";
				return	PVariable();
			}
			if (!lookup.variable->Set(value))
			{
				error = "Assigment failed";
				return PVariable();
			}
			return lookup.variable;
		}
		
		int	ival;
		float vval;
		
		
		if (convertToInt(value.c_str(),ival))
			return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),(new IntVariable(lookup.pathSegments.last(),ival)));
		
		if (convertToFloat(value.c_str(),vval))
			return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),(new FloatVariable(lookup.pathSegments.last(),vval)));
		
		VectorVariable<3>	f3(lookup.pathSegments.last());
		VectorVariable<4>	f4(lookup.pathSegments.last());
		
		if (f3.Set(value))
			return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),(new VectorVariable<3>(f3)));
		if (f4.Set(value))
			return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),(new VectorVariable<4>(f4)));
			
		return lookup.folder->variables.InsertNew(lookup.pathSegments.last(),(new StringVariable(lookup.pathSegments.last(),value)));
	}

	
	
	
	

	PFolder	Interpretor::_Resolve(bool from_root)
	{
		if (!lookup.pathSegments)
		{
			error = "Missing argument";
			return PFolder();
		}
		
		PFolder current = from_root?root:focused;

		for	(index_t i = 0; i < lookup.pathSegments-1; i++)
		{
			if	(lookup.pathSegments[i] == '.')
				continue;
			if	(lookup.pathSegments[i] == "..")
			{
				if	(!current->parent.lock())
				{
					error = "Can't exit folder. Folder is root";
					return	PFolder();
				}
				current = current->parent.lock();
			}
			elif (lookup.pathSegments[i] == '~' || lookup.pathSegments[i] == '-')
			{
				if	(!current->parent.lock())
				{
					error = "Can't exit folder. Folder is root";
					return	PFolder();
				}
				while (current->parent.lock()->parent.lock())
					current = current->parent.lock();
			}
			else
			{
				PFolder folder = current->folders.Query(lookup.pathSegments[i]);
				if	(!folder)
				{
					error = "Folder does not exist: '"+lookup.pathSegments[i]+"'";
					return	PFolder();
				}
				current = folder;
			}
		}
		return	current;
	}
	
	PFolder	Interpretor::_ResolveFull(bool from_root)
	{
		if (!lookup.pathSegments)
		{
			error = "Missing argument";
			return PFolder();
		}
		
		PFolder current = from_root?root:focused;

		for	(index_t i = 0; i < lookup.pathSegments; i++)
		{
			if	(lookup.pathSegments[i] == '.')
				continue;
			if	(lookup.pathSegments[i] == "..")
			{
				if	(!current->parent.lock())
				{
					error = "Can't exit folder. Folder is root";
					return	PFolder();
				}
				current = current->parent.lock();
			}
			elif (lookup.pathSegments[i] == '~' || lookup.pathSegments[i] == '-')
			{
				if	(!current->parent.lock())
				{
					error = "Can't exit folder. Folder is root";
					return	PFolder();
				}
				while (current->parent.lock()->parent.lock())
					current = current->parent.lock();
			}
			else
			{
				PFolder folder = current->folders.Query(lookup.pathSegments[i]);
				if	(!folder)
				{
					error = "Folder does not exist: '"+lookup.pathSegments[i]+"'";
					return	PFolder();
				}
				current = folder;
			}
		}
		return	current;
	}

	bool	Interpretor::Unset(const String&	name)
	{
		if (!_EntryLookup(name,true))
			return false;
		if (!lookup.variable)
		{
			error = "Variable not defined: '"+lookup.pathSegments.last()+"'";
			return false;
		}
	
		
		
		
		if (lookup.variable->protection&EraseProtection)
		{
			error = "Variable '"+name+"' is protected";
			return false;
		}
		lookup.folder->variables.Unset(lookup.variable->name);
		return true;
	}


	void	Interpretor::UnsetIgnoreProtection(const String&	name)
	{
		if (!_EntryLookup(name,true) || !lookup.variable)
			return;
		
		
		lookup.folder->variables.Unset(lookup.variable->name);
	}

	bool	Interpretor::Unset(const PFolder&folder, const PVariable&var, bool ignore_protection)
	{
		if (!var || !folder || !folder->variables.IsSet(var->name))
		{
			error = "Parameters invalid";
			return false;
		}
		if (ignore_protection || !(var->protection&EraseProtection))
		{
			folder->variables.Unset(var->name);
			return true;
		}
		error = "'"+var->name+"' is protected";
		return false;
	}



	unsigned	Interpretor::GetAsUnsigned(const String&	name,	unsigned	exception)
	{
		PVariable v = FindVar(name);
		if (!v)
			return exception;
		if (v->type == "Integer")
			return (unsigned)((IntVariable*)v.get())->content;
		if (v->type == "Float")
			return (unsigned)((FloatVariable*)v.get())->content;

		unsigned val;
		if (!convert(v->ToString().c_str(),val))
			return exception;
		return val;
	}

	int	Interpretor::GetAsInt(const String&	name,	int	exception)
	{
		PVariable v = FindVar(name);
		if (!v)
			return exception;
		if (v->type == "Integer")
			return (int)((IntVariable*)v.get())->content;
		if (v->type == "Float")
			return (int)((FloatVariable*)v.get())->content;
		int val;
		if (!convert(v->ToString().c_str(),val))
			return exception;
		return val;
	}

	float	Interpretor::GetAsFloat(const String&	name,	float	exception)
	{
		PVariable v = FindVar(name);
		if (!v)
			return exception;
		if (v->type == "Integer")
			return ((IntVariable*)v.get())->content;
		if (v->type == "Float")
			return ((FloatVariable*)v.get())->content;
		float val;
		if (!convert(v->ToString().c_str(),val))
			return exception;
		return val;
	}

	Key::Name	Interpretor::GetAsKey(const String&	name,	Key::Name	exception)
	{
		PVariable v = FindVar(name);
		if (!v)
			return exception;
		Key::Name val;
		if (!convert(v->ToString().c_str(),val))
			return exception;
		return val;
	}

	bool	Interpretor::GetAsBool(const String&	name,	bool	exception)
	{
		PVariable v = FindVar(name);
		if (!v)
			return exception;
		if (v->type == "Integer")
			return ((IntVariable*)v.get())->content!=0;
		if (v->type == "Float")
			return ((FloatVariable*)v.get())->content!=0;
		bool val;
		if (!convert(v->ToString().c_str(),val))
			return exception;
		return val;
	}


	const String& Interpretor::GetAsString(const String& name, const String& exception)
	{
		static String content;
		PVariable v = FindVar(name);
		if (!v)
			return exception;
		content = v->ToString();
		return content;
	}

	const	String&	Interpretor::GetErrorStr()	const
	{
		return	error;
	}

	const	String&	Interpretor::GetError()	const
	{
		return	error;
	}


	PCommand Interpretor::Find(const String&name,PFolder*folder_out/*=NULL*/)
	{
		Tokenizer::tokenize(name,lookup.pathConfig,lookup.pathSegments);
		
		if (!lookup.pathSegments)
			return PCommand();
		PFolder folder = _Resolve(name.beginsWith('/'));
		if (!folder)
			return PCommand();
		PCommand rs= folder->commands.Query(lookup.pathSegments.last());
		if (rs)
		{
			if (folder_out)
				(*folder_out) = folder;
			return rs;
		}
		if (lookup.pathSegments==1)
		{
			if (folder_out)
				(*folder_out) = root;
			return globalCommands.Query(lookup.pathSegments.first());
		}
		return PCommand();
	}

	PCommand Interpretor::Find(const char*name,PFolder*folder_out/*=NULL*/)
	{
		return Find(String(name),folder_out);
	}

	PVariable Interpretor::FindVar(const String&name,PFolder*folder_out/*=NULL*/)
	{
		Tokenizer::tokenize(name,lookup.pathConfig,lookup.pathSegments);
		if (!lookup.pathSegments)
			return PVariable();
		PFolder folder = _Resolve(name.beginsWith('/'));
		if (!folder)
			return PVariable();
		if (folder_out)
			(*folder_out) = folder;
		return folder->variables.Query(lookup.pathSegments.last());
	}

	PVariable Interpretor::FindVar(const char*name,PFolder *folder_out/*=NULL*/)
	{
		Tokenizer::tokenize(name,lookup.pathConfig,lookup.pathSegments);
		if (!lookup.pathSegments)
			return PVariable();
		PFolder folder = _Resolve(name[0]=='/');
		if (!folder)
			return PVariable();
		if (folder_out)
			(*folder_out) = folder;
		return folder->variables.Query(lookup.pathSegments.last());
	}

	PFolder 		Interpretor::FindFolder(const String&path)
	{
		Tokenizer::tokenize(path,lookup.pathConfig,lookup.pathSegments);
		if (!lookup.pathSegments)
		{
			if (path == '/')
				return root;
			error = "No segments given in '"+path+"'";
			return PFolder();
		}
		lookup.pathSegments<<".";
		return _Resolve(path.beginsWith('/'));
	}


	bool	Interpretor::Interpret(const String&line, bool allow_global_commands/*=true*/)
	{
		return	Parse(line.c_str(),allow_global_commands);
	}
	
	bool	Interpretor::Interpret(const char*line, bool allow_global_commands/*=true*/)
	{
		return	Parse(line,allow_global_commands);
	}

	bool	Interpretor::Parse(const String&line, bool allow_global_commands/*=true*/)
	{
		return	Parse(line.c_str(),allow_global_commands);
	}

	PCommand		Interpretor::GetExecutingCommand()	const
	{
		return executing;
	}

	PFolder 		Interpretor::GetExecutionContext()	const
	{
		return executingFolder;
	}

	String Interpretor::Complete(const String&line, StringList&out)
	{
		out.clear();
		String longest_common;
		Tokenizer::tokenize(line,lookup.pathConfig,lookup.pathSegments);
		
		if (!lookup.pathSegments)
			return "";
			
		PFolder parent = _Resolve(line.beginsWith('/'));
		if (!parent)
			return "";
		
		String prefix = lookup.pathSegments.fuse(0,lookup.pathSegments-1,'/'),
				inner = lookup.pathSegments.last();
		if (prefix.length())
			prefix+= "/";
				
		if (line.beginsWith('/'))
			prefix = "/"+prefix;
		
		if (lookup.pathSegments==1)
			for (index_t i = 0; i < globalCommands.count(); i++)
				if (globalCommands[i]->name.beginsWith(inner))
					out << prefix+globalCommands[i]->name;
		
		for (index_t i = 0; i < parent->commands.count(); i++)
			if (parent->commands[i]->name.beginsWith(inner))
				out << prefix+parent->commands[i]->name;
		for (index_t i = 0; i < parent->variables.count(); i++)
			if (parent->variables[i]->name.beginsWith(inner))
				out << prefix+parent->variables[i]->name;
		for (index_t i = 0; i < parent->folders.count(); i++)
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

	String Interpretor::CompleteVariables(const String&line,StringList&out)
	{
		out.clear();
		String longest_common;
		Tokenizer::tokenize(line,lookup.pathConfig,lookup.pathSegments);
		
		if (!lookup.pathSegments)
			return "";
			
		PFolder parent = _Resolve(line.beginsWith('/'));
		if (!parent)
			return "";
		
		String prefix = lookup.pathSegments.fuse(0,lookup.pathSegments-1,'/'),
				inner = lookup.pathSegments.last();
		if (prefix.length())
			prefix+= "/";
		if (line.beginsWith('/'))
			prefix = "/"+prefix;
		
		for (index_t i = 0; i < parent->variables.count(); i++)
			if (parent->variables[i]->name.beginsWith(inner))
				out << prefix+parent->variables[i]->name;
		for (index_t i = 0; i < parent->folders.count(); i++)
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



	String Interpretor::CompleteFolders(const String&line, StringList&out)
	{
		out.clear();
		String longest_common;
		Tokenizer::tokenize(line,lookup.pathConfig,lookup.pathSegments);
		
		if (!lookup.pathSegments)
			return "";
			
		PFolder parent = _Resolve(line.beginsWith('/'));
		if (!parent)
			return "";
		
		String prefix = lookup.pathSegments.fuse(0,lookup.pathSegments-1,'/'),
				inner = lookup.pathSegments.last();
		if (prefix.length())
			prefix+= "/";

		if (line.beginsWith('/'))
			prefix = "/"+prefix;
		
		for (index_t i = 0; i < parent->folders.count(); i++)
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

	const StringList&	Interpretor::GetExecutionSegments()		const
	{
		return lookup.cmdSegments;
	}
	
	bool	Interpretor::_ParseLine(const char*line,bool allow_global_commands)
	{
		//cout << "parse("<<line<<")"<<endl;
		Tokenizer::tokenize(line,lookup.segmentConfig,lookup.cmdSegments);
		if (!lookup.cmdSegments)
			return true;
			
		Tokenizer::tokenize(lookup.cmdSegments[0],lookup.pathConfig,lookup.pathSegments);
		if	(!lookup.pathSegments)
			return	true;	//this would be rather odd...
			
		executingFolder = _Resolve(line[0]=='/');
		if	(!executingFolder)
			return	false;
		
		//cout << "cmdSegments:"<<endl;
		//lookup.cmdSegments.printLines(cout," ");
		//cout << "pathSegments:"<<endl;
		//lookup.pathSegments.printLines(cout," ");

		if	(lookup.cmdSegments == 1)
			if (PVariable v = executingFolder->variables.Query(lookup.pathSegments.last()))
			{
				if (onVariableCall)
					onVariableCall(v);
				return true;
			}
		
		executing = executingFolder->commands.Query(lookup.pathSegments.last());
		if (!executing && allow_global_commands)
			executing = globalCommands.Query(lookup.pathSegments.last());
		
		if (executing)
		{
			executing->Invoke(lookup.cmdSegments, lookup.segmentConfig);
			executing.reset();
			executingFolder.reset();
			return true;
		}
		
		executingFolder.reset();
		error = "Command or Variable unknown: '"+lookup.pathSegments.last()+"'";
		return false;
	}

	bool	Interpretor::Parse(const char*line, bool allow_global_commands /*=true*/)
	{
		StringList	lines;
		Tokenizer::tokenize(line,lookup.lineConfig,lines);
		for	(index_t i = 0; i < lines;	i++)
			if	(!_ParseLine(lines[i].c_str(),allow_global_commands))
				return	false;
		return	true;
	}
	


	count_t	Folder::CountCommands() const
	{
		count_t rs = commands.count();
		for (index_t i = 0; i < folders.count(); i++)
			rs += folders[i]->CountCommands();
		return	rs;
	}

	count_t	Folder::CountVariables() const
	{
		count_t rs = variables.count();
		for (index_t i = 0; i < folders.count(); i++)
			rs += folders[i]->CountVariables();
		return	rs;
	}


	Folder::Folder(const PFolder&p,const String&n):parent(p),NamedItem(n),customFlags(0)
	{}

	String Folder::GetPath()	const
	{
		String rs = name;
		PFolder folder = parent.lock();
		while (folder)
		{
			if (folder->parent.lock() || folder->name.length())
				rs = folder->name+'/'+rs;
			folder = folder->parent.lock();
		}
		return rs;
	}
	
	String Folder::GetPath(unsigned max_depth)	const
	{
		String rs = name;
		PFolder folder = parent.lock();
		unsigned depth(0);
		while (folder && ++depth<max_depth)
		{
			if (folder->parent.lock() || folder->name.length())
			{
				//cout << "////concatenating '"<<folder->name<<"' and '"<<rs<<"'"<<endl;
				rs = folder->name+'/'+rs;
				//cout << "////rs now '"<<rs<<"'"<<endl;
			}
			folder = folder->parent.lock();
		}
		if (folder && (folder->parent.lock() || folder->name.length()))
			rs = "(...)/"+rs;
		return rs;
	}
	
	bool Folder::IsChildOf(const PFolder&other)	const
	{
		if (!other)
			return false;
		if (other.get() == this)
			return true;
		const Folder* folder = this;
		for (;;)
		{
			PFolder parent = folder->parent.lock();
			if (parent == other)
				return true;
			if (!parent)
				return false;
			folder = parent.get();
		}
		//return false;
	}


	PFolder 		Interpretor::EnterNewFolder(const String& name, bool enter_existing)
	{
		Tokenizer::tokenize(name,lookup.pathConfig,lookup.pathSegments);
		if (!lookup.pathSegments)
			return PFolder();
		
		PFolder parent = _Resolve(name.beginsWith('/'));
		if (!parent)
			return PFolder();
			
		PFolder result = parent->folders.Query(lookup.pathSegments.last());
		if (result)
		{
			if (enter_existing)
				focused = result;
			return PFolder();
		}
			
		result.reset(new Folder(parent,lookup.pathSegments.last()));
		parent->folders.Insert(result->name,result);
		focused = result;
		return result;
	}

	void				Interpretor::EraseFolder(const PFolder&folder)
	{
		if (!folder)
			return;
		PFolder parent = folder->parent.lock();
		if (!parent)
			return;
		
		if (focused->IsChildOf(parent))
			focused = parent;
		
		parent->folders.Unset(folder->name);
	}
	
	void				Interpretor::EraseFolder(const String& name)
	{
		Tokenizer::tokenize(name,lookup.pathConfig,lookup.pathSegments);
		if (!lookup.pathSegments)
			return;
			
		if (PFolder parent = _Resolve(name.beginsWith('/')))
		{
			if (focused->IsChildOf(parent))
				focused = parent;
			parent->folders.Unset(lookup.pathSegments.last());
		}
	}


	bool	Interpretor::MoveFocus(const String&context)
	{
		Tokenizer::tokenize(context,lookup.pathConfig,lookup.pathSegments);
		lookup.pathSegments << ".";	//dummy
		PFolder new_focus = _Resolve(context.beginsWith('/'));
		if (new_focus)
		{
			focused = new_focus;
			return true;
		}
		return false;
	}

	bool	Interpretor::ExitFolder()
	{
		return ExitFocus();
	}
	
	bool	Interpretor::ExitFocus()
	{
		PFolder parent = focused->parent.lock();
		if (parent)
		{
			focused = parent;
			return true;
		}
		return false;
	}

	void	Interpretor::ResetFocus()
	{
		focused = root;
	}

	PFolder 	Interpretor::GetFocus()	const
	{
		return focused;
	}
	
	void		Interpretor::SetFocus(const PFolder&folder)
	{
		if (folder)
			focused = folder;
		else
		{
			FATAL__("Trying to set NULL focus");
			//focused = root;
		}
	}

	void		Interpretor::PushFocus()
	{
		focusStack << focused;
	}

	void		Interpretor::PushAndReplaceFocus(const PFolder&new_focus)
	{
		PushFocus();
		//if (new_focus)
			SetFocus(new_focus);
	}


	void		Interpretor::PopFocus()
	{
		ASSERT__(focusStack.IsNotEmpty());
		SetFocus(focusStack.pop());
	}


	
	String			Interpretor::GetContext(unsigned depth)	const
	{
		return focused->GetPath(depth);
	}

	const Tokenizer::Config&	Interpretor::GetSegmentizerConfig()	const
	{
		return lookup.segmentConfig;
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
			parser->Parse(script->get(i));
		return	true;
	}
}
