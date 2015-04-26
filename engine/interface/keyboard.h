#ifndef engine_interface_keyboardH
#define engine_interface_keyboardH

/******************************************************************

engine keyboard-interface.

******************************************************************/


#include "../timing.h"
#include "../../io/config.h"

#include "../../string/keys.h"

#if SYSTEM==UNIX
	#include <X11/Xlib.h>
#endif

#include "input.h"


namespace Engine
{



	
	/*!
		\brief 	Keyboard interface class.
		
		The Keyboard class acts as an interface helping with the interaction between an application and the user keyboard.
		It is currently used by the Eve engine and the hive multi-window interface.
	*/

	class Keyboard
	{
	private:
			char				buffer[0x100],
								translation[0x100];
			bool				block;
			Timer::Time		block_start;
			float				block_len;
			BYTE				buffer_len,
								on_char_input;
			unsigned			delay_started,delay;
	public:
			void				keyDown(Key::Name key);		//!< Triggers key down event handling for the specified key. Usually invoked automatically by the surrounding event handling system. \param key Key-index
			void				keyUp(Key::Name key);		//!< Triggers key up event handling for the specified key. Usually invoked automatically by the surrounding event handling system. \param key Key-index
			void				input(char c);			//!< Triggers char event handling for the specified character.  Usually invoked automatically by the surrounding event handling system. \param c Character
			char				convertInput(char c);	//!< Converts a character using the internal conversion map. If the character is not valid or not mapped then 0 is returned

	#if SYSTEM==WINDOWS
	friend	LRESULT CALLBACK	WndProc(HWND hWnd, UINT Msg, WPARAM wParam,LPARAM lParam);
	#elif SYSTEM==UNIX
	friend	void				ExecuteEvent(XEvent&event);
	#endif

	public:
			typedef void	(*charReader)	(char);
			
			
			InputMap			&map;
			bool				read;					//!< Species whether or not character inputs should be handled.
			charReader			reader,					//!< Pointer to an external character handling function. Set NULL to internally handle character input (if \b read is set true)
								onCharHandled;			//!< Pointer to be notified when a char has been handled. Effective only if @a reader is NULL.


								Keyboard(InputMap&map);
	virtual						~Keyboard();
			const char*			getInput();				//!< Zero terminates and returns the internal character buffer content. \return Character array pointer pointing to the internal character buffer. The returned string is zero terminated.
			BYTE				getInputLen();			//!< Queries buffer fill state. \return Number of characters currently in the internal buffer (not counting any trailing zero character).
			void				flushInput();					//!< Clears the internal character buffer.
			void				fillInput(const char*strn);		//!< Fills the specified string into the internal character buffer overwriting any previous buffer content. \param strn String to fill in. Only the first 255 characters are copied.
			void				fillInput(const String&strn);	//!< Fills the specified string into the internal character buffer overwriting any previous buffer content. \param strn String to fill in. Only the first 255 characters are copied.
			void				dropLastCharacter();			//!< Drops one character from the end of the internal character buffer reducing the buffer fill state by one. The method has no effect if the internal buffer is empty.
			void				truncateIfLonger(BYTE len);										//!< Drops a number of characters from the end of the internal character buffer so that its content is at most \b len characters long after execution. \param len Maximum number of characters, the internal buffer should contain after execution.
			void				replaceInputSection(BYTE offset, BYTE len, const String&string);	//!< Replaces a section of the internal character buffer with the specified string. \param offset Index of the first character to remove (0=first character in the buffer) \param len Number of characters to remove \param strn String to replace the removed section with. The string is not required to be of the same length as the section it replaces.
			void				replaceInputSection(BYTE offset, BYTE len, const char*string);	//!< Replaces a section of the internal character buffer with the specified string. \param offset Index of the first character to remove (0=first character in the buffer) \param len Number of characters to remove \param strn String to replace the removed section with. The string is not required to be of the same length as the section it replaces.
			void				delayRead(float seconds=0.1);									//!< Enables character input handling after the specified time interval has passed. \param seconds Seconds to wait before enabling character handling.

			void				resetTranslation();					//!< Resets the internal input character translation to the internal default.
			void				filter(const char*characters);		//!< Modifies the internal input character translation to only allow the specified characters. \param characters Characters to allow. All non-specified characters will be blocked.
			void				allow(const char*characters);		//!< Modifies the internal input character translation to allow the specified characters. \param characters Characters to allow. Non-specified character translations are not modified.
			void				disallow(const char*characters);			//!< Modifies the internal input character translation to disallow the specified characters. \param characters Characters to block. Non-specified character translations are not modified.
			void				translate(const char*from, const char*to);	//!< Modifies the internal input character translation. \param from Character field to translate from \param to Character field to translate to. Must be of the same length as \b from.
	};


	extern Keyboard keyboard;
}

#endif
