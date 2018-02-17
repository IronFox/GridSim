#include "cli_defaults.h"

/******************************************************************

E:\include\string\cli_defaults.cpp

******************************************************************/

namespace DeltaWorks
{

	namespace CLI
	{

		pPrintln		println=NULL;

		Interpreter		*main_interpretor(NULL);
		pOnFocusChange	onFocusChange=NULL;
		bool echoSetOperation = true;
	
	
	
	
		static void Describe(const CLI::Variable&v, bool more)
		{
			String msg = "'"+v.name+"' ("+v.type;
			if (v.components != 1)
				msg += "["+String(v.components)+"]";
			msg += ") ";
			//println(msg);
			if (v.protection != NoProtection)
			{
				if ( (v.protection & EraseProtection) && (v.protection & WriteProtection))
					msg += "is fully protected (You may neither modify nor unset this variable)";
				elif (v.protection & EraseProtection)
					msg += "may be modified but not unset";
				else
					msg += "may be unset but not modified";
			}
			else
				msg += "may be modified and/or unset as required";
			String supported = v.GetSupportedValues();
			if (supported.IsNotEmpty())
			{
				msg += '.';
				if ((v.protection & WriteProtection) == 0)
					msg += "\nSupported values are "+supported;
				else
					msg += "\nPossible values are "+supported;
			}
			if (more)
				msg += '.';
			println(msg);
		}

		static void	printHelp(const String&p)
		{
			String parameter = p;
			if (!parameter.GetLength())
				parameter = "help";
		
			{
				String message;
				if (PVariable var = main_interpretor->FindVar(parameter))
				{
					Describe(*var,true);
					String supported = var->GetSupportedValues();
					if (var->help)
						message = *var->help;
					else
					{
						message = "No additional information available :/";
					}
				}
				elif (PCommand cmd = main_interpretor->Find(parameter))
				{
					if (cmd->help)
						message = cmd->fullSpecification+":\n"+*cmd->help;
					else
						message = "No additional information available for command '"+cmd->fullSpecification+"' :/";
				}
				elif (PFolder folder = main_interpretor->FindFolder(parameter))
				{
					if (folder->help)
						message = folder->name+":\n"+*folder->help;
					else
						message = "No additional information available for folder '"+folder->name+"' :/";
				}
			
				if (!message.GetLength())
				{
					message ="Unable to find requested entry '"+parameter+"' :/";
				}
				println(message);
			}
		}


		static void	printCommands(CLI::ItemTable<CLI::Command>&commands, const String&appendix)
		{
			PointerTable<bool>	listed_commands;

			for (index_t i = 0; i < commands.Count(); i++)
			{
				CLI::PCommand cmd = commands[i];
				if (!listed_commands.IsSet(cmd.get()))
				{
					Buffer<CLI::PCommand>	temp;
					temp.append(cmd);
					for (index_t j = i+1; j < commands.Count(); j++)
						if (commands[j] == cmd)
							temp.append(commands[j]);
					String line = " *";
					for (index_t j = 0; j < temp.Count()-1; j++)
						line+=temp[j]->name+"/";
					line+=temp.Last()->fullSpecification+appendix;
					println(line);
					listed_commands.Set(cmd.get(),true);
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
			for (index_t i = 0; i < folder->variables.Count(); i++)
				println(" "+folder->variables[i]->name);
			if (folder->variables.Count())
				println("-");
		
			printCommands(folder->commands,"");
			if (folder->parent.expired())
				printCommands(main_interpretor->globalCommands," (global)");
			if (folder->commands.Count())
				println("-");
			if (folder->folders.Count() < 6)
				for (index_t i = 0; i < folder->folders.Count(); i++)
					println(" ["+folder->folders[i]->name+"]");
			else
			{
				count_t groups = folder->folders.Count()/3;
				for (index_t i = 0; i < groups; i++)
					println(" ["+folder->folders[i*3]->name+"] ["+folder->folders[i*3+1]->name+"] ["+folder->folders[i*3+2]->name+"]");
				if (folder->folders.Count()%3)
					if (groups*3+1 == folder->folders.Count())
						println(" ["+folder->folders[groups*3]->name+"]");
					else
						println(" ["+folder->folders[groups*3]->name+"] ["+folder->folders[groups*3+1]->name+"]");
			}
			if (folder->folders.Count())
				println("-");
		}

		static void	cd(const String&folder)
		{
			if (!main_interpretor->MoveFocus(folder))
				println(main_interpretor->GetErrorStr());
			elif (onFocusChange)
				onFocusChange();
		
		}


		static bool UpdateComponent(const String&variable, const String&value)
		{
			index_t p = variable.Find('.');
			if (p == InvalidIndex)
				return false;

			String var = variable.SubString(0,p);
			StringRef component = variable.SubStringRef(p+1);
			CLI::PVariable v = main_interpretor->FindVar(var);
			if (!v)
			{
				println("Unknown variable: '"+var+"'. Variable must be set for component access");
				return true;
			}
			if (v->IsWriteProtected())
			{
				println("Variable '"+var+"' is write-protected");
				return true;
			}
			if (!v->Set(component,value))
			{
				println("Failed to update '"+variable+"'");
				return true;
			}
			if (echoSetOperation)
				println(v->name+" set to "+v->ConvertToString());
			return true;
		}
		


		static void	Set(const String&variable, const String&value)
		{
			if (UpdateComponent(variable,value))
				return;
			CLI::PVariable v;
			if (!(v=main_interpretor->Set(variable,value)))
			{
				println(main_interpretor->GetErrorStr());
				return;
			}

			if (echoSetOperation)
				println(v->name+" set to "+v->ConvertToString());
		}

		static void	variableCallBack(const CLI::PVariable&v)
		{
			String msg = v->name+": "+v->ConvertToString();
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
			Describe(*v,false);
		}

		static void	Unset(const String&variable)
		{
			if (!main_interpretor->Unset(variable))
				println(main_interpretor->GetErrorStr());
		}

		typedef std::shared_ptr<const String>	PString;
	
		static PString	list_help = PString(new String("Displays the content of the current folder.\nCommands are displayed with a leading asterisk (e.g. '*command'), folders in brackets (e.g. '[folder]'), and variables just plain (e.g. 'variable').")),
								general_help = PString(new String("Enter 'ls' or 'list' to list all available commands.\nAdditionally, 'help <variable/command>' will query the respective help entry.")),
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

			parser.DefineGlobalCommand("set variable value",Set,CLI::VariableCompletion)->help = set_help;
			parser.DefineGlobalCommand("unset variable",Unset,CLI::VariableCompletion)->help = unset_help;
			parser.DefineGlobalCommand("cd folder",cd,CLI::FolderCompletion)->help = cd_help;
			parser.DefineGlobalCommand("typeof variable",showTypeof,CLI::VariableCompletion)->help = typeof_help;
			parser.onVariableCall = variableCallBack;
		
			help->help = general_help;
		}

	}
}
