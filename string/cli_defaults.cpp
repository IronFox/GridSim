#include "cli_defaults.h"

/******************************************************************

E:\include\string\cli_defaults.cpp

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

namespace CLI
{

	pPrintln		println=NULL;

	Interpretor	*main_interpretor(NULL);
	pOnFocusChange	onFocusChange=NULL;
	bool echoSetOperation = true;
	
	PointerTable<String>	help_map;
	
	
	void	registerHelp(void*pointer,const String&message)
	{
		help_map.set(pointer,message);
	}
	
	void	unregisterHelp(void*pointer)
	{
		help_map.unSet(pointer);
	}
	

	static void	printHelp(const String&p)
	{
		String parameter = p;
		if (!parameter.length())
			parameter = "help";
		
		{
			String message;
			if (Variable*var = main_interpretor->findVar(parameter))
			{
				if (help_map.query(var,message))
					message = var->name+" ("+var->type+"):\n"+message;
				else
					message = "No help definitions found for variable '"+var->name+"' of type "+var->type;
			}
			elif (Command*cmd = main_interpretor->find(parameter))
			{
				if (help_map.query(cmd,message))
					message = cmd->description+":\n"+message;
				else
					message = "No help definitions found for command '"+cmd->description+"'";
			}
			elif (Folder*folder = main_interpretor->findFolder(parameter))
			{
				if (help_map.query(folder,message))
					message = folder->name+":\n"+message;
				else
					message = "No help definitions found for folder '"+folder->name+"'";
			}
			
			if (!message.length())
			{
				message ="Unable to find requested entry '"+parameter+"'";
			}
			println(message);
		}
	}


	static void	printCommands(Sorted<List::Vector<CLI::Folder::Command>,NameSort>&commands, const String&appendix)
	{
		PointerTable<bool>	listed_commands;

		for (index_t i = 0; i < commands.count(); i++)
		{
			CLI::Folder::Command*cmd = commands[i];
			if (!listed_commands.isSet(cmd))
			{
				List::ReferenceVector<CLI::Folder::Command>	temp;
				temp.append(cmd);
				for (index_t j = i+1; j < commands; j++)
					if (commands[j] == cmd)
						temp.append(commands[j]);
				String line = " *";
				for (index_t j = 0; j < temp.count()-1; j++)
					line+=temp[j]->name+"/";
				line+=temp.last()->description+appendix;
				println(line);
				listed_commands.set(cmd,true);
			}
		}
	}

	static void 	list()
	{
		CLI::Folder*folder = main_interpretor->getFocus();
		
		//println(" ["+folder->path()+"]:");
		println("-");
		for (index_t i = 0; i < folder->variables.count(); i++)
			println(" "+folder->variables[i]->name);
		if (folder->variables.count())
			println("-");
		
		printCommands(folder->commands,"");
		if (!folder->parent)
			printCommands(main_interpretor->global_commands," (global)");
		if (folder->commands.count())
			println("-");
		if (folder->folders < 6)
			for (index_t i = 0; i < folder->folders.count(); i++)
				println(" ["+folder->folders[i]->name+"]");
		else
		{
			count_t groups = folder->folders/3;
			for (index_t i = 0; i < groups; i++)
				println(" ["+folder->folders[i*3]->name+"] ["+folder->folders[i*3+1]->name+"] ["+folder->folders[i*3+2]->name+"]");
			if (folder->folders%3)
				if (groups*3+1 == folder->folders)
					println(" ["+folder->folders[groups*3]->name+"]");
				else
					println(" ["+folder->folders[groups*3]->name+"] ["+folder->folders[groups*3+1]->name+"]");
		}
		if (folder->folders.count())
			println("-");
	}

	static void	cd(const String&folder)
	{
		if (!main_interpretor->moveFocus(folder))
			println(main_interpretor->getErrorStr());
		elif (onFocusChange)
			onFocusChange();
		
	}



	static void	set(const String&variable, const String&value)
	{
		CLI::Variable*v;
		if (index_t p = variable.indexOf('.'))
		{
			String var = variable.subString(0,p-1),
					component = variable.subString(p);
			v = main_interpretor->findVar(var);
			if (!v)
			{
				println("Unknown variable: '"+var+"'. Variable must be set for component access");
				return;
			}
			if (!v->set(component,value))
			{
				println("Failed to update '"+variable+"'");
				return;
			}
		}
		else
			if (!(v=main_interpretor->set(variable,value)))
			{
				println(main_interpretor->getErrorStr());
				return;
			}
		if (echoSetOperation)
			println(v->name+" set to "+v->toString());
	}

	static void	variableCallBack(CLI::Interpretor::Variable*v)
	{
		String msg = v->name+": "+v->toString();
		println(msg);
	}

	static void	showTypeof(const String&var)
	{
		CLI::Variable*v = main_interpretor->findVar(var);
		if (!v)
		{
			if (main_interpretor->findFolder(var))
				println("'"+var+"' is of type folder");
			else
				println("Undefined variable: '"+var+"'");
			return;
		}
		String msg = v->name+" is of type "+v->type;
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
		println(msg);

	}

	static void	unset(const String&variable)
	{
		if (!main_interpretor->unset(variable))
			println(main_interpretor->getErrorStr());
	}

	
	static const String	list_help = "Displays the content of the current folder.\nCommands are displayed with a leading asterisk (e.g. '*command'), folders in brackets (e.g. '[folder]'), and variables just plain (e.g. 'variable').",
							general_help = "Enter 'ls' or 'list' to list all available commands.\nAdditionally 'help <variable/command>' will query the respective help entry.",
							set_help = "Registers a new variable or updates the value of an existing one.\nVariables may be read-only preventing you from changing their value, others may execute arbitrary code upon modification.",
							unset_help = "Unregisters a variable. Variables may be protected against this kind of operation.",
							cd_help = "Changes the active folder. Use 'cd <folder>' to change into a specific folder.\nEntering 'cd ..' will change into the respective parent folder and 'cd /' will change the active directory to the root folder. You may also specify an entire path (e.g. 'cd ../path0/subpath/../othersubpath'). The respective target folder(s) must exist for this operation to be successful.",
							typeof_help = "Queries the type and protection of a variable.\nVariable types may vary from primitive types like integers, booleans, strings, or floats to very complex ones.";
							
							
	
	void			initDefaults(Interpretor&parser, pPrintln out, pOnFocusChange focusChange, bool echoSetOperation_)
	{
	
		println = out;
		onFocusChange = focusChange;
		echoSetOperation = echoSetOperation_;
		
		main_interpretor = &parser;
		Command*help = parser.defineGlobalCommand("help command/variable/folder",printHelp,CLI::CommandCompletion);
		registerHelp(parser.defineGlobalCommand("ls",list),list_help);
		registerHelp(parser.defineGlobalCommand("list",list),list_help);
		//parser.defineGlobalCommand("list",list);

		registerHelp(parser.defineGlobalCommand("set variable value",set,CLI::VariableCompletion),set_help);
		registerHelp(parser.defineGlobalCommand("unset variable",unset,CLI::VariableCompletion),unset_help);
		registerHelp(parser.defineGlobalCommand("cd folder",cd,CLI::FolderCompletion),cd_help);
		registerHelp(parser.defineGlobalCommand("typeof variable",showTypeof,CLI::VariableCompletion),typeof_help);
		parser.exec_on_variable_call = variableCallBack;
		
		registerHelp(help,general_help);
	}

}
