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


namespace Engine
{


	struct TKeyPointer
	{
		class Context
		{};
		
		typedef void (Context::*pPlainMethod)();
		typedef void (Context::*pByteMethod)(BYTE);
		typedef void (Context::*pIntMethod)(int);
	
		union
		{
			void	(*plainEvent)();
			void	(*byteEvent)(BYTE);
			void	(*intEvent)(int);
			pPlainMethod	plainMethod;
			pByteMethod		byteMethod;
			pIntMethod		intMethod;
		};
		enum Type
		{
			TypeUndef,
			TypePlain,
			TypeByte,
			TypeInt,
			TypePlainMethod,
			TypeByteMethod,
			TypeIntMethod
		};
		
		Type		type;
		Context		*context;
					
					TKeyPointer():plainEvent(NULL),type(TypeUndef)
					{}
					TKeyPointer(void (*event)()):plainEvent(event),type(TypePlain)
					{}
					TKeyPointer(void (*event)(BYTE)):byteEvent(event),type(TypeByte)
					{}
					TKeyPointer(void (*event)(int)):intEvent(event),type(TypeInt)
					{}
					TKeyPointer(Context*object, pPlainMethod event):plainMethod(event),type(TypePlainMethod),context(object)
					{}
					TKeyPointer(Context*object, pByteMethod event):byteMethod(event),type(TypeByteMethod),context(object)
					{}
					TKeyPointer(Context*object, pIntMethod event):intMethod(event),type(TypeIntMethod),context(object)
					{}
		
		bool		operator()(int parameter)	const
					{
						switch (type)
						{
							case TypePlain:
								plainEvent();
							return true;
							case TypeByte:
								byteEvent((BYTE)parameter);
							return true;
							case TypeInt:
								intEvent(parameter);
							return true;
							case TypePlainMethod:
								(context->*plainMethod)();
							return true;
							case TypeByteMethod:
								(context->*byteMethod)((BYTE)parameter);
							return true;
							case TypeIntMethod:
								(context->*intMethod)(parameter);
							return true;
						}
						return false;
					}
		bool		operator==(const TKeyPointer&other)	const
					{
						return type == other.type && (type == TypeUndef || plainEvent == other.plainEvent) && (type < TypePlainMethod || context == other.context);
					}
		bool		isset()	const
					{
						return type != TypeUndef;
					}
		void		print(std::ostream&stream)	const
					{
						switch (type)
						{
							case TypeUndef:
								stream << "unassigned";
							break;
							case TypePlain:
								stream << plainEvent << "()";
							break;
							case TypeByte:
								stream << byteEvent << "(byte)";
							break;
							case TypeInt:
								stream << intEvent << "(int)";
							break;
							case TypePlainMethod:
								stream << context << "->"<<plainMethod<<"()";
							break;
							case TypeByteMethod:
								stream << context << "->"<<byteMethod<<"(byte)";
							break;
							case TypeIntMethod:
								stream << context << "->"<<intMethod<<"(int)";
							break;
							default:
								stream << "unknown type";
							break;
						}
					}
	};



	struct TKeyLink	//! Eve keyboard link container
	{
			TKeyPointer	down_pntr,	//!< Key down event pointer
						up_pntr,	//!< Key up event pointer
						ctrl_pntr;	//!< Key down (if ctrl is being pressed) event pointer
			int			parameter,		//!< Parameter to pass to the key down and up event pointers
						ctrl_parameter;	//!< Parameter to pass to the ctrl event pointer
						
						
	};

	struct TKeyPreparation	//! Eve keyboard preparation container
	{
			char		name[64];			//!< Name of the key event (max 64 characters long)
			TKeyPointer	function_pointer,	//!< Key down event pointer
						snd_pointer;		//!< Key up event pointer
			int			function_param;		//!< Parameter to pass to the pointers
			bool		overwrite,
						done;
			Key::Name	key_id;
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
			TKeyLink	key[NumKeys];
			
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

			TKeyLink				key[NumKeys];
			InputProfile			default_profile,
									*active_profile;
			
			TKeyPreparation			preparation[0x100];
			BYTE					prepared;

			List::ReferenceVector<InputProfile>
									binding_stack;
			unsigned				//active_binding,
									stack_forward_depth;
			
			HashTable<TAnalogSource>	analog_sources;
			TAnalogSource				empty_source;
			Buffer<TAnalogSource>		linear_analog_list;

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

			void				bind(Key::Name key, TKeyPointer down_handler);						//!< Binds the specified event handler to the specified key. \param key Index of the key to bind. \param down_handler Pointer to a function to execute if the specified key has been pressed
			void				bind(Key::Name key, TKeyPointer down_handler, int param);				//!< Binds the specified event handler to the specified key using a custom parameter. \param key Index of the key to bind. \param down_handler Pointer to a function to execute if the specified key has been pressed. \param param Custom parameter to pass to the event handler function.
			void				bind(Key::Name key, TKeyPointer down_handler, TKeyPointer up_handler);				//!< Binds the specified event handlers to the specified key. \param key Index of the key to bind. \param down_handler Pointer to a function to execute if the specified key has been pressed. \param up_handler Pointer to a function to execute if the specified key has been released.
			void				bind(Key::Name key, TKeyPointer down_handler, TKeyPointer up_handler, int param);	//!< Binds the specified event handlers to the specified key using a custom parameter. \param key Index of the key to bind. \param down_handler Pointer to a function to execute if the specified key has been pressed. \param up_handler Pointer to a function to execute if the specified key has been released. \param param Custom parameter to pass to the event handler functions.
			void				bindCtrl(Key::Name key, TKeyPointer ctrl_handler);									//!< Binds the specified event handler to the specified key (+ctrl). \param key Index of the key to bind. \param ctrl_handler Pointer to a function to execute if the specified key has been pressed in combination with the ctrl key.
			void				bindCtrl(Key::Name key, TKeyPointer ctrl_handler, int param);						//!< Binds the specified event handler to the specified key (+ctrl) using a custom parameter. \param key Index of the key to bind. \param ctrl_handler Pointer to a function to execute if the specified key has been pressed in combination with the ctrl key. \param param Custom parameter to pass to the event handler function.
			void				unbind(Key::Name key);																//!< Removes all bound event handlers from the specified key. \param key Index of the key to unbind.
			void				cascade(Key::Name key);																//!< Sets the key to auto cascade incoming key down and up events to stacked profiles. \param key Index of the key to cascade.
			void				resetKeys();																		//!< Unbinds all keys.
			void				cascadeKeys();																		//!< Binds all keys so that their event is forwarded to the profile stack
			void				regInit();																			//!< Resets key registration
			void				regKey(const char*name, TKeyPointer down_handler, Key::Name key);						//!< Registers a key command for import. \param name Name of the command. \param down_handler Pointer to a function to execute if the specified key has been pressed. \param key Index of the key to default to if the command is not bound.
			void				regKey(const char*name, TKeyPointer down_handler, Key::Name key, int param);				//!< Registers a key command for import using a custom parameter. \param name Name of the command. \param down_handler Pointer to a function to execute if the specified key has been pressed. \param key Index of the key to default to if the command is not bound. \param param Custom parameter to pass to the event handler function.
			void				regKey(const char*name, TKeyPointer down_handler, TKeyPointer up_handler, Key::Name key);				//!< Registers a key command for import. \param name Name of the command. \param down_handler Pointer to a function to execute if the specified key has been pressed. \param up_handler Pointer to a function to execute if the specified key has been released. \param key Index of the key to default to if the command is not bound.
			void				regKey(const char*name, TKeyPointer down_handler, TKeyPointer up_handler, Key::Name key, int param);	//!< Registers a key command for import using a custom parameter. \param name Name of the command. \param down_handler Pointer to a function to execute if the specified key has been pressed. \param up_handler Pointer to a function to execute if the specified key has been released. \param key Index of the key to default to if the command is not bound. \param param Custom parameter to pass to the event handler functions.
			void				linkKey(const char*name, Key::Name key);																//!< Link a registered command to the specified key. The method returns if the specified command was not registered. After execution the specified key will be bound to registered function pointer(s). \param name Name of the command to link. \param key Index of the key to link to.
			void				linkOthers();								//!< Links all non linked commands to their respective default keys
								/*!
									\brief Links all commands/keys found in the specified group in the currently loaded configuration file.
									\param group_name Configuration group name to scan for key binds.

									readFromCFG() attempts to read key command bindings from the currently loaded ConfigFile class 'config'.
								*/
			void				readFromCFG(const String&group_name);
			const char*			keyName(TKeyPointer handler);					//!< Queries the name of the key, that is currently bound to the specified handler. \param handler Function pointer to search for. The specified pointer may be an up, down, or ctrl event handler. \return Name of the bound key or an empty string if the specified handler could not be found.
			const char*			keyName(TKeyPointer handler, int param);		//!< Queries the name of the key, that is currently bound to the specified handler using the specified parameter. \param handler Function pointer to search for. The specified pointer may be an up, down, or ctrl event handler. \param param Parameter to look for. \return Name of the key that is currently bound to the specified handler using the specified parameter or an empty string if no such could be found.

			
								/*!	\brief Activates the specified profile
									\param profile Reference to the binding profile to use.
									
									 Activates the specified binding profile. Any succeeding bind() or linkKey() calls will affect the now active profile.	*/
			void				chooseProfile(InputProfile&profile);			
			void				bindProfile(InputProfile&profile);		//!< Identical to chooseProfile()
			void				activateProfile(InputProfile&profile);	//!< Identical to chooseProfile()
			InputProfile*		currentProfile();					//!< Retrieves the currently bound profile
			void				pushProfile();						//!< Pushes the currently active profile to the stack.
			void				popProfile();						//!< Pops the most recently pushed profile from the stack, replacing the currently active one.
			
			
			void				regAnalog(const String&name, float&resource, float min, float max);	//!< Registers a new analog resource by the given name \param name Name of the new resource \param resource Reference to the float variable containing the current resource status \param min Minimum value of this resource \param max Maximum value of this resource
			TAnalogSource*		findAnalog(const String&name);											//!< Attempts to locate an analog input source by the specified name. \return Reference to a matching analog source or NULL if no such match was found
	};
	
	


	extern InputMap			input;


}


inline std::ostream&	operator<<(std::ostream&stream, const Engine::TKeyPointer&binding)
{
	binding.print(stream);
	return stream;
}




#endif
