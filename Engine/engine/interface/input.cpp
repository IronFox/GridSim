#include "input.h"


static std::ostream& operator<<(std::ostream&str, const Engine::KeyHandler &func)
{
	str << "function(";
	if (func.IsNotEmpty())
		str << func.GetRawPointer();
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


	
	void InputMap::CascadeKeyPressed(Key::Name index)
	{
		if (verbose)
			std::cout << " input: forward down "<<index<<".";
		if (index < 0 || (index_t)index >= NumKeys || binding_stack.IsEmpty())
		{
			if (verbose)
				std::cout << " index invalid or binding stack empty"<<std::endl;
			return;
		}
		stack_forward_depth++;
		if (verbose)
			std::cout << " profile is "<<(binding_stack.Count()-stack_forward_depth)<<".";
		InputProfile*profile = binding_stack[binding_stack.Count()-stack_forward_depth];
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
				std::cout << " CTRL "<<profile->key[index].onPressedWithCtrl<<std::endl;
			profile->key[index].onPressedWithCtrl(index);
		}
		else
		{
			if (verbose)
				std::cout << " "<<profile->key[index].onPressed << std::endl;
			profile->key[index].onPressed(index);
		}
		stack_forward_depth--;
	}
	
	void InputMap::CascadeKeyReleased(Key::Name index)
	{
		if (verbose)
			std::cout << " input: forward up "<<index<<".";
		if (index < 0 || (index_t)index >= NumKeys || binding_stack.IsEmpty())
		{
			if (verbose)
				std::cout << " index invalid or binding stack empty"<<std::endl;
			return;
		}
		stack_forward_depth++;
		if (verbose)
			std::cout << " profile is "<<(binding_stack.Count()-stack_forward_depth)<<".";
		
		InputProfile*profile = binding_stack[binding_stack.Count()-stack_forward_depth];
		if (!profile)
		{
			if (verbose)
				std::cout << " undefined profile"<<std::endl;
			stack_forward_depth--;
			return;
		}
		if (verbose)
			std::cout << " "<< profile->key[index].onReleased << std::endl;
		
		profile->key[index].onReleased(index);
		stack_forward_depth--;
	}
	
	//static void cascadeDown(int key)
	//{
	//	input.CascadeKeyPressed(key);
	//}
	//
	//static void cascadeUp(int key)
	//{
	//	input.CascadeKeyReleased(key);
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
				std::cout << " CTRL "<<key[index].onPressedWithCtrl << std::endl;
			result = key[index].onPressedWithCtrl((Key::Name)index);
		}
		else
		{
			if (verbose)
				std::cout << " "<<key[index].onPressed << std::endl;
			result = key[index].onPressed((Key::Name)index);
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
		//if (index == VK_CONTROL)
		//{
		//	if (verbose)
		//		std::cout << " index is CTRL"<<std::endl;
		//	return false;
		//}
		bool result;
		if (pressed[VK_CONTROL])
		{
			if (verbose)
				std::cout << " CTRL "<<key[index].onReleasedWithCtrl << std::endl;
			result = key[index].onReleasedWithCtrl((Key::Name)index);
		}
		else
		{
			if (verbose)
				std::cout << " "<<key[index].onReleased << std::endl;
			result = key[index].onReleased((Key::Name)index);
		}
		return result;
	}
	

	InputMap::InputMap():key(NumKeys),active_profile(&default_profile),stack_forward_depth(0),verbose(false)
	{
		ClearAllBindings();
		
		empty_source.name = "Undefined";
		empty_source.min = 0;
		empty_source.max = 1;
		empty_source.value = &empty_source.min;
	}
	
	InputMap::~InputMap()
	{
		ClearAllBindings();
	}
	
	
	

	void InputMap::ClearAllBindings()
	{
		if (verbose)
			std::cout << " input: reset keys"<<std::endl;
		for (unsigned i = 0; i < NumKeys; i++)
		{
			key[i].Clear();
			pressed[i] = false;
		}
	}
	
	void InputMap::CascadeAllKeys	()
	{
		if (verbose)
			std::cout << " input: cascade keys"<<std::endl;
		for (unsigned i = 0; i < NumKeys; i++)
		{
			Cascade((Key::Name)i);
		}
	}


	void	InputMap::BindAll(const Handler&downHandler, bool unboundOnly)
	{
		for (unsigned k = 0; k < NumKeys; k++)
			if (!unboundOnly || (key[k].onPressed.IsEmpty() && key[k].onReleased.IsEmpty()))
			{
				key[k].onPressed = downHandler;
			}
	}

	void	InputMap::BindAll(const Handler&downHandler, const Handler&upHandler, bool unboundOnly)
	{
		for (unsigned k = 0; k < NumKeys; k++)
			if (!unboundOnly || (key[k].onPressed.IsEmpty() && key[k].onReleased.IsEmpty()))
			{
				key[k].onPressed = downHandler;
				key[k].onReleased = upHandler;
			}
	}


	void InputMap::Bind(Key::Name name, const Handler& cmd)
	{
		if (verbose)
			std::cout << " input: binding "<<resolveKeyName(name)<<"("<<name<<")"<<" to command "<<cmd<<std::endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].onPressed = cmd;
		key[k].onReleased = Handler();
	}


	void InputMap::Bind(Key::Name name, const Handler& cmd, const Handler& ucmd)
	{
		if (verbose)
			std::cout << " input: binding "<<resolveKeyName(name)<<"("<<name<<")"<<" to commands "<<cmd<<"(down) and "<<ucmd<<"(up)"<<std::endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].onPressed = cmd;
		key[k].onReleased = ucmd;
	}


	void InputMap::BindCtrl( Key::Name name, const Handler& cmd, const Handler& ucmd )
	{
		if (verbose)
			std::cout << " input: binding ctrl+"<<resolveKeyName(name)<<"("<<name<<")"<<" to command "<<cmd<<std::endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].onPressedWithCtrl = cmd;
		key[k].onReleasedWithCtrl = ucmd;
	}


	void InputMap::Unbind(Key::Name name)
	{
		if (verbose)
			std::cout << " input: unbinding "<<resolveKeyName(name)<<"("<<name<<")"<<std::endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].Clear();
	}
	
	void InputMap::Cascade(Key::Name name)
	{
		if (verbose)
			std::cout << " input: cascading "<<resolveKeyName(name)<<"("<<name<<")"<<std::endl;
		unsigned k = ((unsigned)name)%NumKeys;
		key[k].onPressed = [this](Key::Name k){CascadeKeyPressed(k);};
		key[k].onReleased = [this](Key::Name k){CascadeKeyReleased(k);};
		key[k].onPressedWithCtrl.Clear();
		pressed[k] = false;
	}


	void InputMap::RegInit()
	{
		preparation.reset();
	}

	void InputMap::RegKey(const String&name, const Handler& cmd, Key::Name kname)
	{
		TKeyPreparation&p = preparation.Append();
		p.name = name;
		p.onPressed = cmd;
		//p.onReleased.Clear();
		p.keyID = kname;
		p.done = false;
	}


	void InputMap::RegKey(const String&name, const Handler& cmd, const Handler& ucmd, Key::Name kname)
	{
		TKeyPreparation&p = preparation.Append();
		p.name = name;
		p.onPressed = cmd;
		p.onReleased = ucmd;
		p.keyID = kname;
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

	void InputMap::LinkKey(const String&name, Key::Name kname)
	{
		BYTE k = (BYTE)kname;
		for (index_t i = 0; i < preparation.Count(); i++)
			if (preparation[i].name == name)
			{
				key[k].onPressed = preparation[i].onPressed;
				key[k].onReleased = preparation[i].onReleased;
				preparation[i].done = true;
				return;
			}
		for (index_t i = 0; i < preparation.Count(); i++)
			if (preparation[i].name.EqualsIgnoreCase(name))
			{
				key[k].onPressed = preparation[i].onPressed;
				key[k].onReleased = preparation[i].onReleased;
				preparation[i].done = true;
				return;
			}
	}

	void InputMap::LinkOthers()
	{
		for (index_t i = 0; i < preparation.Count(); i++)
			if (!preparation[i].done && key[preparation[i].keyID].onPressed.IsEmpty())
			{
				key[preparation[i].keyID].onPressed = preparation[i].onPressed;
				key[preparation[i].keyID].onReleased = preparation[i].onReleased;
			}
		preparation.reset();
	}	


	InputProfile*	InputMap::GetCurrentProfile()
	{
		return active_profile;
	}
	
	InputProfile::InputProfile(bool keyboard_, bool device_buttons_):keyboard(keyboard_),device_buttons(device_buttons_),key(NumKeys)
	{
		//memset(key,0,sizeof(key));
	}
	
	void InputProfile::ImportFrom(InputMap*from)
	{
		if (from->verbose)
			std::cout << " input: importing map "<<from<<" into profile "<<this<<std::endl;
		if (keyboard)
			key.copyFrom(from->key.pointer(),0x100);
		if (device_buttons)
			CopyStrategy::copyElements(from->key + 0x100,key + 0x100,(NumKeys-0x100));
	}
	
	void InputProfile::ExportTo(InputMap*to)
	{
		if (to->verbose)
			std::cout << " input: exporting profile "<<this<<" to map "<<to<<std::endl;
		if (keyboard)
			to->key.copyFrom(key.pointer(),0x100);
		if (device_buttons)
			CopyStrategy::copyElements(key + 0x100,to->key + 0x100,(NumKeys-0x100));
	}


	void InputMap::ChooseProfile(InputProfile&profile)
	{
		if (verbose)
			std::cout << " input: choosing profile "<<&profile<<" over "<<active_profile<<std::endl;
		if (active_profile == &profile)
			return;
		active_profile->ImportFrom(this);
		active_profile = &profile;
		active_profile->ExportTo(this);
	}

	void InputMap::BindProfile(InputProfile& profile)
	{
		ChooseProfile(profile);
	}

	void InputMap::ActivateProfile(InputProfile& profile)
	{
		ChooseProfile(profile);
	}

	void InputMap::PushProfile()
	{
		if (verbose)
			std::cout << " input: pushing profile "<<active_profile<<std::endl;
		active_profile->ImportFrom(this);
		binding_stack.append(active_profile);
	}

	void InputMap::PopProfile()
	{
		if (binding_stack.IsEmpty())
			return;
		if (verbose)
			std::cout << " input: popping profile, thus overwriting "<<active_profile<<std::endl;
		active_profile->ImportFrom(this);
		active_profile = binding_stack.Pop();
		active_profile->ExportTo(this);
		if (verbose)
			std::cout << " input: now active profile is "<<active_profile<<std::endl;
	}
	
	void	InputMap::RegAnalog(const String&name, float&resource, float min, float max)
	{
		TAnalogSource&source = analog_sources.Set(name.CopyToLowerCase());
		source.value = &resource;
		source.min = min;
		source.max = max;
		source.name = name;
		linear_analog_list << source;
	}
	
	void	InputMap::UnregAnalog(const String&name)
	{
		analog_sources.Unset(name);
	}


	TAnalogSource*		InputMap::FindAnalog(const String&name)
	{
		return analog_sources.queryPointer(name.CopyToLowerCase());
	}
}

