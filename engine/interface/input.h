#ifndef engine_interface_inputH
#define engine_interface_inputH

#include "../../string/keys.h"
//#include "../../io/config.h"
#include "../../container/buffer.h"
#include "../../container/lvector.h"
#include "../../container/hashtable.h"
#include "../../global_string.h"
#include "../../string/string_converter.h"	//for string name lookup
#include <iostream>
#include <functional>

namespace Engine
{




	struct TKeyLink	//! Eve keyboard link container
	{
		std::function<void()>	down_pntr,	//!< Key down event pointer
								up_pntr,	//!< Key up event pointer
								ctrl_pntr;	//!< Key down (if ctrl is being pressed) event pointer
		void					swap(TKeyLink&other)
		{
			down_pntr.swap(other.down_pntr);
			up_pntr.swap(other.up_pntr);
			ctrl_pntr.swap(other.ctrl_pntr);
		}
	};

	struct TKeyPreparation	//! Eve keyboard preparation container
	{
		String		name;			//!< Name of the key event
		std::function<void()>	function_pointer,	//!< Key down event pointer
								snd_pointer;		//!< Key up event pointer
		bool		done;
		Key::Name	key_id;

		void swap(TKeyPreparation&other)
		{
			name.swap(other.name);
			function_pointer.swap(other.function_pointer);
			snd_pointer.swap(other.snd_pointer);

		}
	};

	struct TAnalogSource	//! Analog resource containing
	{
		String		name;	//!< Source name
		float		*value,	//!< Pointer to the resource's current value
					min,	//!< Minimum value
					max;	//!< Maximum value
	};
	
	class InputMap;
	
	class InputProfile
	{
	public:
		static const unsigned	NumKeys = Key::Max+1;	//0x100 + 0x20 joystick buttons + 0x5 mouse buttons		
		bool		keyboard,
					device_buttons;
		//TKeyLink	key[NumKeys];
		Array<TKeyLink>	key;	

					InputProfile(bool keyboard=true, bool device_buttons=true);
		void		importFrom(InputMap*map);
		void		exportTo(InputMap*map);
	};

	
	
	/**
		\brief Generalized input handling interface
	*/
	class InputMap
	{
	private:
			static const unsigned	MaxBindingLayers = 0x100;
			static const unsigned	NumKeys = InputProfile::NumKeys;	//0x100 + 0x20 joystick buttons + 0x5 mouse buttons		
			
			friend class InputProfile;

			//TKeyLink				key[NumKeys];
			Array<TKeyLink>			key;	
			InputProfile			default_profile,
									*active_profile;
			
			Buffer<TKeyPreparation,0,Swap>	preparation;

			List::ReferenceVector<InputProfile>
									binding_stack;
			unsigned				//active_binding,
									stack_forward_depth;
			
			HashTable<TAnalogSource>	analog_sources;
			TAnalogSource				empty_source;
			Buffer<TAnalogSource,0>		linear_analog_list;

	public:
			bool				verbose;			//!< Set true to print input changes and events to the console. False by default
	
			bool				pressed[NumKeys];	//!< Key pressed state. The array provides one state variable per key.
			
			
			bool				keyDown(unsigned key);
			bool				keyDown(Key::Name key)
								{
									return keyDown((unsigned)key);
								}
			bool				keyUp(unsigned key);
			bool				keyUp(Key::Name key)
								{
									return keyUp((unsigned)key);
								}
								
			void 				cascadeKeyDown(unsigned index);		//!< Forwards a key down event to the profile most recently pushed to the profile stack. The method returns if no such exists
			void 				cascadeKeyUp(unsigned index);		//!< Forwards a key up event to the profile most recently pushed to the profile stack. The method returns if no such exists
			
	public:
								InputMap();
	virtual						~InputMap();

			void				bind(Key::Name key, const std::function<void()>& down_handler);						//!< Binds the specified event handler to the specified key. \param key Index of the key to bind. \param down_handler Pointer to a function to execute if the specified key has been pressed
			void				bind(Key::Name key, const std::function<void()>& down_handler, const std::function<void()>& up_handler);				//!< Binds the specified event handlers to the specified key. \param key Index of the key to bind. \param down_handler Pointer to a function to execute if the specified key has been pressed. \param up_handler Pointer to a function to execute if the specified key has been released.
			void				bindCtrl(Key::Name key, const std::function<void()>& ctrl_handler);									//!< Binds the specified event handler to the specified key (+ctrl). \param key Index of the key to bind. \param ctrl_handler Pointer to a function to execute if the specified key has been pressed in combination with the ctrl key.
			void				unbind(Key::Name key);																//!< Removes all bound event handlers from the specified key. \param key Index of the key to unbind.
			void				cascade(Key::Name key);																//!< Sets the key to auto cascade incoming key down and up events to stacked profiles. \param key Index of the key to cascade.
			void				resetKeys();																		//!< Unbinds all keys.
			void				cascadeKeys();																		//!< Binds all keys so that their event is forwarded to the profile stack
			void				regInit();																			//!< Resets key registration
			void				regKey(const String&name, const std::function<void()>& down_handler, Key::Name key);						//!< Registers a key command for import. \param name Name of the command. \param down_handler Pointer to a function to execute if the specified key has been pressed. \param key Index of the key to default to if the command is not bound.
			void				regKey(const String&name, const std::function<void()>& down_handler, const std::function<void()>& up_handler, Key::Name key);				//!< Registers a key command for import. \param name Name of the command. \param down_handler Pointer to a function to execute if the specified key has been pressed. \param up_handler Pointer to a function to execute if the specified key has been released. \param key Index of the key to default to if the command is not bound.
			void				linkKey(const String&name, Key::Name key);																//!< Link a registered command to the specified key. The method returns if the specified command was not registered. After execution the specified key will be bound to registered function pointer(s). \param name Name of the command to link. \param key Index of the key to link to.
			void				linkOthers();								//!< Links all non linked commands to their respective default keys
								/*!
									\brief Links all commands/keys found in the specified group in the currently loaded configuration file.
									\param group_name Configuration group name to scan for key binds.

									readFromCFG() attempts to read key command bindings from the currently loaded ConfigFile class 'config'.
								*/
			void				readFromCFG(const String&group_name);
			
			
								/*!	\brief Activates the specified profile
									\param profile Reference to the binding profile to use.
									
									 Activates the specified binding profile. Any succeeding bind() or linkKey() calls will affect the now active profile.	*/
			void				chooseProfile(InputProfile&profile);			
			void				bindProfile(InputProfile&profile);		//!< Identical to chooseProfile()
			void				activateProfile(InputProfile&profile);	//!< Identical to chooseProfile()
			InputProfile*		currentProfile();					//!< Retrieves the currently bound profile
			void				pushProfile();						//!< Pushes the currently active profile to the stack.
			void				popProfile();						//!< Pops the most recently pushed profile from the stack, replacing the currently active one.
			
			
			void				RegAnalog(const String&name, float&resource, float min, float max);	//!< Registers a new analog resource by the given name \param name Name of the new resource \param resource Reference to the float variable containing the current resource status \param min Minimum value of this resource \param max Maximum value of this resource
			void				UnregAnalog(const String&name);
			TAnalogSource*		FindAnalog(const String&name);											//!< Attempts to locate an analog input source by the specified name. \return Reference to a matching analog source or NULL if no such match was found
	};
	
	


	extern InputMap			input;


}






#endif
