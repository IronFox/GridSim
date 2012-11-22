#include "../../global_root.h"
#include "input.h"

namespace Engine
{
	static TKeyPointer	unbound;
	
	InputMap	input;
	
	
	
	
	void InputMap::cascadeKeyDown(unsigned index)
	{
		if (verbose)
			cout << " input: forward down "<<index<<".";
		if (index >= NumKeys || !binding_stack)
		{
			if (verbose)
				cout << " index invalid or binding stack empty"<<endl;
			return;
		}
		stack_forward_depth++;
		if (verbose)
			cout << " profile is "<<(binding_stack-stack_forward_depth)<<".";
		InputProfile*profile = binding_stack[binding_stack-stack_forward_depth];
		if (!profile)
		{
			if (verbose)
				cout << " undefined profile"<<endl;
			stack_forward_depth--;
			return;
		}
		if (pressed[VK_CONTROL])
		{
			if (verbose)
				cout << " CTRL "<<profile->key[index].ctrl_pntr<<endl;
			
			profile->key[index].ctrl_pntr(profile->key[index].ctrl_parameter);
		}
		else
		{
			if (verbose)
				cout << " "<<profile->key[index].down_pntr << endl;
			profile->key[index].down_pntr(profile->key[index].parameter);
		}
		stack_forward_depth--;
	}
	
	void InputMap::cascadeKeyUp(unsigned index)
	{
		if (verbose)
			cout << " input: forward up "<<index<<".";
		if (index >= NumKeys || !binding_stack)
		{
			if (verbose)
				cout << " index invalid or binding stack empty"<<endl;
			return;
		}
		stack_forward_depth++;
		if (verbose)
			cout << " profile is "<<(binding_stack-stack_forward_depth)<<".";
		
		InputProfile*profile = binding_stack[binding_stack-stack_forward_depth];
		if (!profile)
		{
			if (verbose)
				cout << " undefined profile"<<endl;
			stack_forward_depth--;
			return;
		}
		if (verbose)
			cout << " "<< profile->key[index].up_pntr << endl;
			
		profile->key[index].up_pntr(profile->key[index].parameter);
		stack_forward_depth--;
	}
	
	static void cascadeDown(int key)
	{
		input.cascadeKeyDown(key);
	}
	
	static void cascadeUp(int key)
	{
		input.cascadeKeyUp(key);
	}
	
	bool InputMap::keyDown(unsigned index)
	{
		if (index >= NumKeys)
			return false;
		bool verbose = !pressed[index] && this->verbose;
		if (verbose)
			cout << " input: down "<<resolveKeyName((Key::Name)index)<<"("<<index<<")"<<".";
		bool result;
		if (pressed[VK_CONTROL])
		{
			if (verbose)
			{
				cout << " CTRL "<<key[index].ctrl_pntr << endl;
			}
			result = key[index].ctrl_pntr(key[index].ctrl_parameter);
		}
		else
		{
			if (verbose)
			{
				cout << " "<<key[index].down_pntr << endl;
			}
			result = key[index].down_pntr(key[index].parameter);
		}
		pressed[index]=true;
		return result;
	}

	bool InputMap::keyUp(unsigned index)
	{
		if (verbose)
			cout << " input: up "<<resolveKeyName((Key::Name)index)<<"("<<index<<")"<<".";
		if (index >= NumKeys)
		{
			if (verbose)
				cout << " index invalid"<<endl;
			return false;
		}
		pressed[index]=false;
		if (index == VK_CONTROL)
		{
			if (verbose)
				cout << " index is CTRL"<<endl;
			return false;
		}
		if (verbose)
			cout << " "<<key[index].up_pntr << endl;
		return key[index].up_pntr(key[index].parameter);
	}
	

	InputMap::InputMap():active_profile(&default_profile),prepared(0),stack_forward_depth(0),verbose(false)
	{
		
		
		resetKeys();
		
		empty_source.name = "Undefined";
		empty_source.min = 0;
		empty_source.max = 1;
		empty_source.value = &empty_source.min;
	}
	
	InputMap::~InputMap()
	{}
	
	
	

	void InputMap::resetKeys()
	{
		if (verbose)
			cout << " input: reset keys"<<endl;
		for (unsigned i = 0; i < NumKeys; i++)
		{
			key[i].down_pntr = unbound;
			key[i].up_pntr = unbound;
			key[i].ctrl_pntr = unbound;
			key[i].parameter = i;
			key[i].ctrl_parameter = i;
			pressed[i] = false;
		}
	}
	
	void InputMap::cascadeKeys	()
	{
		if (verbose)
			cout << " input: cascade keys"<<endl;
		for (unsigned i = 0; i < NumKeys; i++)
		{
			key[i].down_pntr = cascadeDown;
			key[i].up_pntr = cascadeUp;
			key[i].ctrl_pntr = unbound;
			key[i].parameter = i;
			key[i].ctrl_parameter = i;
			pressed[i] = false;
		}
	}

	void InputMap::bind(Key::Name name, TKeyPointer cmd)
	{
		if (verbose)
			cout << " input: binding "<<resolveKeyName(name)<<"("<<name<<")"<<" to command "<<cmd<<endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].down_pntr = cmd;
		key[k].up_pntr = unbound;
		key[k].parameter = k;
	}

	void InputMap::bind(Key::Name name, TKeyPointer cmd, int param)
	{
		if (verbose)
			cout << " input: binding "<<resolveKeyName(name)<<"("<<name<<")"<<" to command "<<cmd<<" using parameter "<<param<<endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].down_pntr = cmd;
		key[k].up_pntr = unbound;
		key[k].parameter = (int)param;
	}

	void InputMap::bind(Key::Name name, TKeyPointer cmd, TKeyPointer ucmd)
	{
		if (verbose)
			cout << " input: binding "<<resolveKeyName(name)<<"("<<name<<")"<<" to commands "<<cmd<<"(down) and "<<ucmd<<"(up)"<<endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].down_pntr = cmd;
		key[k].up_pntr = ucmd;
		key[k].parameter = (int)name;
	}

	void InputMap::bind(Key::Name name, TKeyPointer cmd, TKeyPointer ucmd, int param)
	{
		if (verbose)
			cout << " input: binding "<<resolveKeyName(name)<<"("<<name<<")"<<" to commands "<<cmd<<"(down) and "<<ucmd<<"(up) using parameter "<<param<<endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].down_pntr = cmd;
		key[k].up_pntr = ucmd;
		key[k].parameter = (int)param;
	}

	void InputMap::bindCtrl(Key::Name name, TKeyPointer cmd)
	{
		if (verbose)
			cout << " input: binding ctrl+"<<resolveKeyName(name)<<"("<<name<<")"<<" to command "<<cmd<<endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].ctrl_pntr = cmd;
		key[k].ctrl_parameter = (int)name;
	}

	void InputMap::bindCtrl(Key::Name name, TKeyPointer cmd, int param)
	{
		if (verbose)
			cout << " input: binding ctrl+"<<resolveKeyName(name)<<"("<<name<<")"<<" to command "<<cmd<<" using parameter "<<param<<endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].ctrl_pntr = cmd;
		key[k].ctrl_parameter = param;
	}

	void InputMap::unbind(Key::Name name)
	{
		if (verbose)
			cout << " input: unbinding "<<resolveKeyName(name)<<"("<<name<<")"<<endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].down_pntr = unbound;
		key[k].up_pntr = unbound;
		key[k].ctrl_pntr = unbound;
	}
	
	void InputMap::cascade(Key::Name name)
	{
		if (verbose)
			cout << " input: cascading "<<resolveKeyName(name)<<"("<<name<<")"<<endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].down_pntr = cascadeDown;
		key[k].up_pntr = cascadeUp;
		key[k].ctrl_pntr = unbound;
	}


	void InputMap::regInit()
	{
		prepared = 0;
	}

	void InputMap::regKey(const char*name, TKeyPointer cmd, Key::Name kname)
	{
		if (strlen(name) >= 64)
			return;
		unsigned k = ((unsigned)kname)%NumKeys;
		strcpy(preparation[prepared].name,name);
		preparation[prepared].key_id = kname;
		preparation[prepared].function_pointer = cmd;
		preparation[prepared].snd_pointer = unbound;
		preparation[prepared].overwrite = false;
		preparation[prepared].done = false;
		preparation[prepared++].function_param = (int)kname;
	}


	void InputMap::regKey(const char*name, TKeyPointer cmd, Key::Name kname, int param)
	{
		if (strlen(name) >= 64)
			return;
		unsigned k = ((unsigned)kname)%NumKeys;
		strcpy(preparation[prepared].name,name);
		preparation[prepared].key_id = kname;
		preparation[prepared].function_pointer = cmd;
		preparation[prepared].snd_pointer = unbound;
		preparation[prepared].overwrite = true;
		preparation[prepared].done = false;
		preparation[prepared++].function_param = param;
	}

	void InputMap::regKey(const char*name, TKeyPointer cmd, TKeyPointer ucmd, Key::Name kname)
	{
		if (strlen(name) >= 64)
			return;
		unsigned k = ((unsigned)kname)%NumKeys;
		strcpy(preparation[prepared].name,name);
		preparation[prepared].key_id = kname;
		preparation[prepared].function_pointer = cmd;
		preparation[prepared].snd_pointer = ucmd;
		preparation[prepared].overwrite = false;
		preparation[prepared].done = false;
		preparation[prepared++].function_param = (int)kname;
	}


	void InputMap::regKey(const char*name, TKeyPointer cmd, TKeyPointer ucmd, Key::Name kname, int param)
	{
		if (strlen(name) >= 64)
			return;
		unsigned k = ((unsigned)kname)%NumKeys;
		strcpy(preparation[prepared].name,name);
		preparation[prepared].key_id = kname;
		preparation[prepared].function_pointer = cmd;
		preparation[prepared].snd_pointer = ucmd;
		preparation[prepared].overwrite = true;
		preparation[prepared].done = false;
		preparation[prepared++].function_param = param;
	}

	const char* InputMap::keyName(TKeyPointer cmd)
	{
		for (unsigned i = 0; i < NumKeys; i++)
			if (key[i].down_pntr == cmd || key[i].up_pntr == cmd || key[i].ctrl_pntr == cmd)
				return resolveKeyName((Key::Name)i);
		return "";
	}

	const char* InputMap::keyName(TKeyPointer cmd, int param)
	{
		for (unsigned i = 0; i < NumKeys; i++)
			if (((key[i].down_pntr == cmd || key[i].up_pntr == cmd) && key[i].parameter == param) || (key[i].ctrl_pntr == cmd && key[i].ctrl_parameter == param))
				return resolveKeyName((Key::Name)i);
		return "";
	}


/*
	void InputMap::readFromCFG(const String&name)
	{
		for (BYTE i = 0; i < prepared; i++)
		{
			int id;
			if (config.defined(name+"."+preparation[i].name,"Key"))
				id = config.getKey(name+"."+preparation[i].name,preparation[i].key_id);
			else
				id = preparation[i].key_id;

			if (key[id].down_pntr.isset())
				continue;
			key[id].down_pntr = preparation[i].function_pointer;
			key[id].up_pntr = preparation[i].snd_pointer;
			if (preparation[i].overwrite)
				key[id].parameter =preparation[i].function_param;
		}
	}*/

	void InputMap::linkKey(const char*name, Key::Name kname)
	{
		BYTE k = (BYTE)kname;
		for (BYTE i = 0; i < prepared; i++)
			if (!strcmp(preparation[i].name,name))
			{
				key[k].down_pntr = preparation[i].function_pointer;
				key[k].up_pntr = preparation[i].snd_pointer;
				if (preparation[i].overwrite)
					key[k].parameter = preparation[i].function_param;
				preparation[i].done = true;
				return;
			}
		for (BYTE i = 0; i < prepared; i++)
			if (!strcmpi(preparation[i].name,name))
			{
				key[k].down_pntr = preparation[i].function_pointer;
				key[k].up_pntr = preparation[i].snd_pointer;
				if (preparation[i].overwrite)
					key[k].parameter = preparation[i].function_param;
				preparation[i].done = true;
				return;
			}
	}

	void InputMap::linkOthers()
	{
		for (BYTE i = 0; i < prepared; i++)
			if (!preparation[i].done && !key[preparation[i].key_id].down_pntr.isset())
			{
				key[preparation[i].key_id].down_pntr = preparation[i].function_pointer;
				key[preparation[i].key_id].up_pntr = preparation[i].snd_pointer;
				if (preparation[i].overwrite)
					key[preparation[i].key_id].parameter = preparation[i].function_param;
			}
		prepared = 0;
	}	


	InputProfile*	InputMap::currentProfile()
	{
		return active_profile;
	}
	
	InputProfile::InputProfile(bool keyboard_, bool device_buttons_):keyboard(keyboard_),device_buttons(device_buttons_)
	{
		memset(key,0,sizeof(key));
	}
	
	void InputProfile::importFrom(InputMap*from)
	{
		if (from->verbose)
			cout << " input: importing map "<<from<<" into profile "<<this<<endl;
		if (keyboard)
			memcpy(this->key,from->key,0x100*sizeof(TKeyLink));
		if (device_buttons)
			memcpy(this->key+0x100,from->key+0x100,(NumKeys-0x100)*sizeof(TKeyLink));
	}
	
	void InputProfile::exportTo(InputMap*to)
	{
		if (to->verbose)
			cout << " input: exporting profile "<<this<<" to map "<<to<<endl;
		if (keyboard)
			memcpy(to->key,this->key,0x100*sizeof(TKeyLink));
		if (device_buttons)
			memcpy(to->key+0x100,this->key+0x100,(NumKeys-0x100)*sizeof(TKeyLink));
	}


	void InputMap::chooseProfile(InputProfile&profile)
	{
		if (verbose)
			cout << " input: choosing profile "<<&profile<<" over "<<active_profile<<endl;
		if (active_profile == &profile)
			return;
		active_profile->importFrom(this);
		active_profile = &profile;
		active_profile->exportTo(this);
	}

	void InputMap::bindProfile(InputProfile& profile)
	{
		chooseProfile(profile);
	}

	void InputMap::activateProfile(InputProfile& profile)
	{
		chooseProfile(profile);
	}

	void InputMap::pushProfile()
	{
		if (verbose)
			cout << " input: pushing profile "<<active_profile<<endl;
		active_profile->importFrom(this);
		binding_stack.append(active_profile);
	}

	void InputMap::popProfile()
	{
		if (!binding_stack)
			return;
		if (verbose)
			cout << " input: popping profile, thus overwriting "<<active_profile<<endl;
		active_profile->importFrom(this);
		active_profile = binding_stack.drop(binding_stack-1);
		active_profile->exportTo(this);
		if (verbose)
			cout << " input: now active profile is "<<active_profile<<endl;
	}
	
	void	InputMap::regAnalog(const String&name, float&resource, float min, float max)
	{
		TAnalogSource&source = analog_sources.set(name.copyToLowerCase());
		source.value = &resource;
		source.min = min;
		source.max = max;
		source.name = name;
		linear_analog_list << source;
	}
	
	TAnalogSource*		InputMap::findAnalog(const String&name)
	{
		return analog_sources.queryPointer(name.copyToLowerCase());
	}
}

