#include "cli_defaults.h"

/******************************************************************

E:\include\string\cli_defaults.cpp

******************************************************************/

namespace CLI
{

	pPrintln		println=NULL;

	Interpreter		*main_interpretor(NULL);
	pOnFocusChange	onFocusChange=NULL;
	bool echoSetOperation = true;
	
	
	
	

	static void	printHelp(const String&p)
	{
		String parameter = p;
		if (!parameter.length())
			parameter = "help";
		
		{
			String message;
			if (PVariable var = main_interpretor->FindVar(parameter))
			{
				String supported = var->GetSupportedValues();
				if (var->help)
					message = var->name+" ("+var->type+"):\n"+*var->help;
				else
				{
					message = "No help text defined for variable '"+var->name+"' of type "+var->type;
					if (supported.IsNotEmpty())
						message += '.';
				}
				if (supported.IsNotEmpty())
				{
					message += "\nSupported values are "+supported;
				}
			}
			elif (PCommand cmd = main_interpretor->Find(parameter))
			{
				if (cmd->help)
					message = cmd->fullSpecification+":\n"+*cmd->help;
				else
					message = "No help text defined for command '"+cmd->fullSpecification+"'";
			}
			elif (PFolder folder = main_interpretor->FindFolder(parameter))
			{
				if (folder->help)
					message = folder->name+":\n"+*folder->help;
				else
					message = "No help text defined for folder '"+folder->name+"'";
			}
			
			if (!message.length())
			{
				message ="Unable to find requested entry '"+parameter+"'";
			}
			println(message);
		}
	}


	static void	printCommands(CLI::ItemTable<CLI::Command>&commands, const String&appendix)
	{
		PointerTable<bool>	listed_commands;

		for (index_t i = 0; i < commands.count(); i++)
		{
			CLI::PCommand cmd = commands[i];
			if (!listed_commands.isSet(cmd.get()))
			{
				Buffer<CLI::PCommand>	temp;
				temp.append(cmd);
				for (index_t j = i+1; j < commands.count(); j++)
					if (commands[j] == cmd)
						temp.append(commands[j]);
				String line = " *";
				for (index_t j = 0; j < temp.count()-1; j++)
					line+=temp[j]->name+"/";
				line+=temp.last()->fullSpecification+appendix;
				println(line);
				listed_commands.set(cmd.get(),true);
			}
		}
	}

	static void 	list(const String&f)
	{
		CLI::PFolder folder = f.IsNotEmpty() ? main_interpretor->FindFolder(f) : main_interpretor->GetFocus();
		if (!folder)
		{
			println("'"+f+"' is not a folder");
			return;
		}
		
		//println(" ["+folder->path()+"]:");
		println("-");
		for (index_t i = 0; i < folder->variables.count(); i++)
			println(" "+folder->variables[i]->name);
		if (folder->variables.count())
			println("-");
		
		printCommands(folder->commands,"");
		if (folder->parent.expired())
			printCommands(main_interpretor->globalCommands," (global)");
		if (folder->commands.count())
			println("-");
		if (folder->folders.count() < 6)
			for (index_t i = 0; i < folder->folders.count(); i++)
				println(" ["+folder->folders[i]->name+"]");
		else
		{
			count_t groups = folder->folders.count()/3;
			for (index_t i = 0; i < groups; i++)
				println(" ["+folder->folders[i*3]->name+"] ["+folder->folders[i*3+1]->name+"] ["+folder->folders[i*3+2]->name+"]");
			if (folder->folders.count()%3)
				if (groups*3+1 == folder->folders.count())
					println(" ["+folder->folders[groups*3]->name+"]");
				else
					println(" ["+folder->folders[groups*3]->name+"] ["+folder->folders[groups*3+1]->name+"]");
		}
		if (folder->folders.count())
			println("-");
	}

	static void	cd(const String&folder)
	{
		if (!main_interpretor->MoveFocus(folder))
			println(main_interpretor->GetErrorStr());
		elif (onFocusChange)
			onFocusChange();
		
	}



	static void	set(const String&variable, const String&value)
	{
		CLI::PVariable v;
		if (index_t p = variable.GetIndexOf('.'))
		{
			String var = variable.subString(0,p-1),
					component = variable.subString(p);
			v = main_interpretor->FindVar(var);
			if (!v)
			{
				println("Unknown variable: '"+var+"'. Variable must be set for component access");
				return;
			}
			if (!v->Set(component,value))
			{
				println("Failed to update '"+variable+"'");
				return;
			}
		}
		else
			if (!(v=main_interpretor->Set(variable,value)))
			{
				println(main_interpretor->GetErrorStr());
				return;
			}
		if (echoSetOperation)
			println(v->name+" set to "+v->ToString());
	}

	static void	variableCallBack(const CLI::PVariable&v)
	{
		String msg = v->name+": "+v->ToString();
		println(msg);
	}

	static void	showTypeof(const String&var)
	{
		CLI::PVariable v = main_interpretor->FindVar(var);
		if (!v)
		{
			if (main_interpretor->FindFolder(var))
				println("'"+var+"' is of type folder");
			else
				println("Undefined variable: '"+var+"'");
			return;
		}
		String msg = "'"+v->name+"' is of type "+v->type;
		if (v->components != 1)
			msg += "["+String(v->components)+"]";
		//println(msg);
		
		msg += " and ";
		if (v->protection != NoProtection)
		{
			if ( (v->protection & EraseProtection) && (v->protection & WriteProtection))
				msg += "fully protected (You may neither modify nor unset this variable)";
			elif (v->protection & EraseProtection)
				msg += "may be modified but not unset";
			else
				msg += "may be unset but not modified";
		}
		else
			msg += "may be modified and/or unset as required";
		msg += '.';
		String supported = v->GetSupportedValues();
		if (supported.IsNotEmpty())
			msg += "\nSupported values are "+supported;
		println(msg);

	}

	static void	unset(const String&variable)
	{
		if (!main_interpretor->Unset(variable))
			println(main_interpretor->GetErrorStr());
	}

	typedef std::shared_ptr<const String>	PString;
	
	static PString	list_help = PString(new String("Displays the content of the current folder.\nCommands are displayed with a leading asterisk (e.g. '*command'), folders in brackets (e.g. '[folder]'), and variables just plain (e.g. 'variable').")),
							general_help = PString(new String("Enter 'ls' or 'list' to list all available commands.\nAdditionally 'help <variable/command>' will query the respective help entry.")),
							set_help = PString(new String("Registers a new variable or updates the value of an existing one.\nVariables may be read-only preventing you from changing their value, others may execute arbitrary code upon modification.")),
							unset_help = PString(new String("Unregisters a variable. Variables may be protected against this kind of operation.")),
							cd_help = PString(new String("Changes the active folder. Use 'cd <folder>' to change into a specific folder.\nEntering 'cd ..' will change into the respective parent folder and 'cd /' will change the active directory to the root folder. You may also specify an entire path (e.g. 'cd ../path0/subpath/../othersubpath'). The respective target folder(s) must exist for this operation to be successful.")),
							typeof_help = PString(new String("Queries the type and protection of a variable.\nVariable types may vary from primitive types like integers, booleans, strings, or floats to very complex ones."));
							
							
	
	void			InitDefaults(Interpreter&parser, pPrintln out, pOnFocusChange focusChange, bool echoSetOperation_)
	{
	
		println = out;
		onFocusChange = focusChange;
		echoSetOperation = echoSetOperation_;
		
		main_interpretor = &parser;
		PCommand help = parser.DefineGlobalCommand("help command/variable/folder",printHelp,CLI::CommandCompletion);
		parser.DefineGlobalCommand("ls [folder]",list,CLI::FolderCompletion)->help = list_help;
		parser.DefineGlobalCommand("list [folder]",list,CLI::FolderCompletion)->help = list_help;
		//parser.defineGlobalCommand("list",list);

		parser.DefineGlobalCommand("set variable value",set,CLI::VariableCompletion)->help = set_help;
		parser.DefineGlobalCommand("unset variable",unset,CLI::VariableCompletion)->help = unset_help;
		parser.DefineGlobalCommand("cd folder",cd,CLI::FolderCompletion)->help = cd_help;
		parser.DefineGlobalCommand("typeof variable",showTypeof,CLI::VariableCompletion)->help = typeof_help;
		parser.onVariableCall = variableCallBack;
		
		help->help = general_help;
	}

}
