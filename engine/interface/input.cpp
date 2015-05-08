#include "../../global_root.h"
#include "input.h"


static std::ostream& operator<<(std::ostream&str, const std::function<void()> &func)
{
	str << "function(";
	if (func)
		str << func.target<void()>();
	else
		str << "<empty>";
	str << ")";
	return str;
}


namespace Engine
{
	//std::function<void()>	unbound;
	
	InputMap	input;
	
	
	
	void InputMap::ReleasePressedKeys()
	{
		for (unsigned i = 0; i < NumKeys; i++)
			if (pressed[i])
				keyUp((Key::Name)i);
	}


	
	void InputMap::cascadeKeyDown(unsigned index)
	{
		if (verbose)
			std::cout << " input: forward down "<<index<<".";
		if (index >= NumKeys || !binding_stack)
		{
			if (verbose)
				std::cout << " index invalid or binding stack empty"<<std::endl;
			return;
		}
		stack_forward_depth++;
		if (verbose)
			std::cout << " profile is "<<(binding_stack-stack_forward_depth)<<".";
		InputProfile*profile = binding_stack[binding_stack-stack_forward_depth];
		if (!profile)
		{
			if (verbose)
				std::cout << " undefined profile"<<std::endl;
			stack_forward_depth--;
			return;
		}
		if (pressed[VK_CONTROL])
		{
			if (verbose)
				std::cout << " CTRL "<<profile->key[index].ctrl_pntr<<std::endl;
			
			if (profile->key[index].ctrl_pntr)
				profile->key[index].ctrl_pntr();
		}
		else
		{
			if (verbose)
				std::cout << " "<<profile->key[index].down_pntr << std::endl;
			if (profile->key[index].down_pntr)
				profile->key[index].down_pntr();
		}
		stack_forward_depth--;
	}
	
	void InputMap::cascadeKeyUp(unsigned index)
	{
		if (verbose)
			std::cout << " input: forward up "<<index<<".";
		if (index >= NumKeys || !binding_stack)
		{
			if (verbose)
				std::cout << " index invalid or binding stack empty"<<std::endl;
			return;
		}
		stack_forward_depth++;
		if (verbose)
			std::cout << " profile is "<<(binding_stack-stack_forward_depth)<<".";
		
		InputProfile*profile = binding_stack[binding_stack-stack_forward_depth];
		if (!profile)
		{
			if (verbose)
				std::cout << " undefined profile"<<std::endl;
			stack_forward_depth--;
			return;
		}
		if (verbose)
			std::cout << " "<< profile->key[index].up_pntr << std::endl;
		
		if (profile->key[index].up_pntr)
			profile->key[index].up_pntr();
		stack_forward_depth--;
	}
	
	//static void cascadeDown(int key)
	//{
	//	input.cascadeKeyDown(key);
	//}
	//
	//static void cascadeUp(int key)
	//{
	//	input.cascadeKeyUp(key);
	//}
	
	bool InputMap::keyDown(unsigned index)
	{
		if (index >= NumKeys)
			return false;
		bool verbose = !pressed[index] && this->verbose;
		if (verbose)
			std::cout << " input: down "<<resolveKeyName((Key::Name)index)<<"("<<index<<")"<<".";
		bool result;
		if (pressed[VK_CONTROL])
		{
			if (verbose)
			{
				std::cout << " CTRL "<<key[index].ctrl_pntr << std::endl;
			}
			result = key[index].ctrl_pntr ? true : false;
			if (result)
				key[index].ctrl_pntr();
		}
		else
		{
			if (verbose)
			{
				std::cout << " "<<key[index].down_pntr << std::endl;
			}
			result = key[index].down_pntr ? true : false;
			if (result)
				key[index].down_pntr();
		}
		pressed[index]=true;
		return result;
	}

	bool InputMap::keyUp(unsigned index)
	{
		if (verbose)
			std::cout << " input: up "<<resolveKeyName((Key::Name)index)<<"("<<index<<")"<<".";
		if (index >= NumKeys)
		{
			if (verbose)
				std::cout << " index invalid"<<std::endl;
			return false;
		}
		pressed[index]=false;
		if (index == VK_CONTROL)
		{
			if (verbose)
				std::cout << " index is CTRL"<<std::endl;
			return false;
		}
		if (verbose)
			std::cout << " "<<key[index].up_pntr << std::endl;
		if (!key[index].up_pntr)
			return false;
		key[index].up_pntr();
		return true;
	}
	

	InputMap::InputMap():key(NumKeys),active_profile(&default_profile),stack_forward_depth(0),verbose(false)
	{
		resetKeys();
		
		empty_source.name = "Undefined";
		empty_source.min = 0;
		empty_source.max = 1;
		empty_source.value = &empty_source.min;
	}
	
	InputMap::~InputMap()
	{
		resetKeys();
	}
	
	
	

	void InputMap::resetKeys()
	{
		if (verbose)
			std::cout << " input: reset keys"<<std::endl;
		for (unsigned i = 0; i < NumKeys; i++)
		{
			key[i].down_pntr = std::function<void()>();
			key[i].up_pntr = std::function<void()>();
			key[i].ctrl_pntr = std::function<void()>();
			pressed[i] = false;
		}
	}
	
	void InputMap::cascadeKeys	()
	{
		if (verbose)
			std::cout << " input: cascade keys"<<std::endl;
		for (unsigned i = 0; i < NumKeys; i++)
		{
			key[i].down_pntr = std::bind(&InputMap::cascadeKeyDown,this,i);
			key[i].up_pntr = std::bind(&InputMap::cascadeKeyUp,this,i);
			key[i].ctrl_pntr = std::function<void()>();
			pressed[i] = false;
		}
	}

	void InputMap::bind(Key::Name name, const std::function<void()>& cmd)
	{
		if (verbose)
			std::cout << " input: binding "<<resolveKeyName(name)<<"("<<name<<")"<<" to command "<<cmd<<std::endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].down_pntr = cmd;
		key[k].up_pntr = std::function<void()>();
	}


	void InputMap::bind(Key::Name name, const std::function<void()>& cmd, const std::function<void()>& ucmd)
	{
		if (verbose)
			std::cout << " input: binding "<<resolveKeyName(name)<<"("<<name<<")"<<" to commands "<<cmd<<"(down) and "<<ucmd<<"(up)"<<std::endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].down_pntr = cmd;
		key[k].up_pntr = ucmd;
	}


	void InputMap::bindCtrl(Key::Name name, const std::function<void()>& cmd)
	{
		if (verbose)
			std::cout << " input: binding ctrl+"<<resolveKeyName(name)<<"("<<name<<")"<<" to command "<<cmd<<std::endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].ctrl_pntr = cmd;
	}


	void InputMap::unbind(Key::Name name)
	{
		if (verbose)
			std::cout << " input: unbinding "<<resolveKeyName(name)<<"("<<name<<")"<<std::endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].down_pntr = std::function<void()>();
		key[k].up_pntr = std::function<void()>();
		key[k].ctrl_pntr = std::function<void()>();
	}
	
	void InputMap::cascade(Key::Name name)
	{
		if (verbose)
			std::cout << " input: cascading "<<resolveKeyName(name)<<"("<<name<<")"<<std::endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].down_pntr = std::bind(&InputMap::cascadeKeyDown,this,k);
		key[k].up_pntr = std::bind(&InputMap::cascadeKeyUp,this,k);
		key[k].ctrl_pntr = std::function<void()>();
	}


	void InputMap::regInit()
	{
		preparation.reset();
	}

	void InputMap::regKey(const String&name, const std::function<void()>& cmd, Key::Name kname)
	{
		TKeyPreparation&p = preparation.append();
		p.name = name;
		p.function_pointer = cmd;
		p.snd_pointer = std::function<void()>();
		p.key_id = kname;
		p.done = false;
	}


	void InputMap::regKey(const String&name, const std::function<void()>& cmd, const std::function<void()>& ucmd, Key::Name kname)
	{
		TKeyPreparation&p = preparation.append();
		p.name = name;
		p.function_pointer = cmd;
		p.snd_pointer = ucmd;
		p.key_id = kname;
		p.done = false;
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

	void InputMap::linkKey(const String&name, Key::Name kname)
	{
		BYTE k = (BYTE)kname;
		for (index_t i = 0; i < preparation.count(); i++)
			if (preparation[i].name == name)
			{
				key[k].down_pntr = preparation[i].function_pointer;
				key[k].up_pntr = preparation[i].snd_pointer;
				preparation[i].done = true;
				return;
			}
		for (index_t i = 0; i < preparation.count(); i++)
			if (preparation[i].name.equalsIgnoreCase(name))
			{
				key[k].down_pntr = preparation[i].function_pointer;
				key[k].up_pntr = preparation[i].snd_pointer;
				preparation[i].done = true;
				return;
			}
	}

	void InputMap::linkOthers()
	{
		for (index_t i = 0; i < preparation.count(); i++)
			if (!preparation[i].done && !key[preparation[i].key_id].down_pntr)
			{
				key[preparation[i].key_id].down_pntr = preparation[i].function_pointer;
				key[preparation[i].key_id].up_pntr = preparation[i].snd_pointer;
			}
		preparation.reset();
	}	


	InputProfile*	InputMap::currentProfile()
	{
		return active_profile;
	}
	
	InputProfile::InputProfile(bool keyboard_, bool device_buttons_):keyboard(keyboard_),device_buttons(device_buttons_),key(NumKeys)
	{
		//memset(key,0,sizeof(key));
	}
	
	void InputProfile::importFrom(InputMap*from)
	{
		if (from->verbose)
			std::cout << " input: importing map "<<from<<" into profile "<<this<<std::endl;
		if (keyboard)
			key.copyFrom(from->key.pointer(),0x100);
		if (device_buttons)
			CopyStrategy::copyElements(from->key + 0x100,key + 0x100,(NumKeys-0x100));
	}
	
	void InputProfile::exportTo(InputMap*to)
	{
		if (to->verbose)
			std::cout << " input: exporting profile "<<this<<" to map "<<to<<std::endl;
		if (keyboard)
			to->key.copyFrom(key.pointer(),0x100);
		if (device_buttons)
			CopyStrategy::copyElements(key + 0x100,to->key + 0x100,(NumKeys-0x100));
	}


	void InputMap::chooseProfile(InputProfile&profile)
	{
		if (verbose)
			std::cout << " input: choosing profile "<<&profile<<" over "<<active_profile<<std::endl;
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
			std::cout << " input: pushing profile "<<active_profile<<std::endl;
		active_profile->importFrom(this);
		binding_stack.append(active_profile);
	}

	void InputMap::popProfile()
	{
		if (!binding_stack)
			return;
		if (verbose)
			std::cout << " input: popping profile, thus overwriting "<<active_profile<<std::endl;
		active_profile->importFrom(this);
		active_profile = binding_stack.drop(binding_stack-1);
		active_profile->exportTo(this);
		if (verbose)
			std::cout << " input: now active profile is "<<active_profile<<std::endl;
	}
	
	void	InputMap::RegAnalog(const String&name, float&resource, float min, float max)
	{
		TAnalogSource&source = analog_sources.set(name.copyToLowerCase());
		source.value = &resource;
		source.min = min;
		source.max = max;
		source.name = name;
		linear_analog_list << source;
	}
	
	void	InputMap::UnregAnalog(const String&name)
	{
		analog_sources.unset(name);
	}


	TAnalogSource*		InputMap::FindAnalog(const String&name)
	{
		return analog_sources.queryPointer(name.copyToLowerCase());
	}
}

