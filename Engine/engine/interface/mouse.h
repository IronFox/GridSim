#ifndef engine_interface_mouseH
#define engine_interface_mouseH

/******************************************************************

eve mouse-interface.

******************************************************************/

#include <global_root.h>

#if SYSTEM==UNIX
	#ifndef Status
		typedef int	Status;
	#endif
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/cursorfont.h>
	#include "../../general/undef.h"
#endif

#include "input.h"
#include <math/vector.h>

namespace Engine
{




	typedef M::TVec2<>	TMouseDelta;	//! Mouse movement vector

	struct TMousePosition	//! Mouse position container
	{
		M::TVec2<>			windowRelative;
		M::TVec2<int>		absolute;
	};

	struct TMouseButtons	//! Mouse button state
	{
		union
		{
			struct
			{
				bool	left_down,		//!< True if the left mouse button is currently being pressed.
						middle_down,	//!< True if the middle mouse button is currently being pressed.
						right_down,		//!< True if the right mouse button is currently being pressed.
						x1_down,		//!< True if the X1 mouse button is currently being pressed.
						x2_down;		//!< True if the X2 mouse button is currently being pressed.
			};
			bool		down[5];		//!< State vector (0=left, 1=middle, 2=right).
		};
		bool			pressed;		//!< True if the any mouse button is currently being pressed.
	};



	/*!
		\brief Eve mouse interface
		
		The Mouse class acts as an interface helping with the interaction between an application and the user pointing device.
		It is currently used by the Eve engine only.
	*/
	class Mouse
	{
	public:
		typedef void (*FWheelLink)(short);	//!< Pointer to a wheel delta event handler. The provided short contains the wheel delta.

	private:
		#if SYSTEM==UNIX
			Display				*display;
			Window				window_handle,last_root;
			int				 	screen;
			Cursor				blank_cursor;
		#elif SYSTEM==WINDOWS
			RECT			 	initial_clip, no_clip;
		#endif
		POINT					sp;
		float					sx,sy;
		bool					locked,focus,cursor_visible,force_invisible;
		unsigned				mx,	my;
		RECT			 		screen_clip,window;
		BYTE					button_id;
		M::TVec2<long>				lastPosition;
		bool					hasLastPosition;
		FWheelLink				wheel_link;		//!< Mouse wheel event handler. Altered by the working context - should not be written to by the client application.
			
		friend	class			Context;
		#if SYSTEM==UNIX
			friend void			ExecuteEvent(XEvent&event);
		
			void				GetCursorPos(POINT*target);
			void				SetCursorPos(int x, int y);
		#elif SYSTEM==WINDOWS
			void				UpdateNoClip();
		friend LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam,LPARAM lParam);
		#endif

		void		_Feed();

		void		LooseFocus();
		void		RestoreFocus();
		void		SetFocused(bool focused);
		bool		IsFocused()	const;
		void		ResetRegion();
		void		LoadCursors();

	public:
		void		HandleMouseWheel(short delta);

		#if SYSTEM==UNIX
			void				Assign(Display*display, Window window, int screen);
		#endif
		bool		SignalButtonDown(BYTE id, bool update_if_bound=false);				//!< Evokes a button down event for the specified mouse button. Invoked by the working context - should not be invoked by the client application. \param id Button id (0=left button, 1=middle button, 2=right button) \param update_if_bound Forces the mouse handler to update before evoking the bound event \return true if an event handler was executed
		bool		SignalButtonUp(BYTE id, bool update_if_bound=false);					//!< Evokes a button up event for the specified mouse button. Invoked by the working context - should not be invoked by the client application. \param id Button id (0=left button, 1=middle button, 2=right button) \param update_if_bound Forces the mouse handler to update before evoking the bound event \return true if an event handler was executed
		void		SetRegion(int width, int height);
		void		SetRegion(RECT region);
		void		RedefineWindow(RECT window, HWND);
		void		Update();							//< Evokes the per frame update. Invoked by the working context - should not be invoked by the client application.
		void		RegAnalogInputs();			//!< Registers analog input types to the input map

		void		RecordAbsoluteMouseMovement(long x, long y);
		void		RecordRelativeMouseMovement(long x, long y);


	public:
		M::TVec2<>		speed;				//!< The speed vector (x,y) scales the delta vector while the mouse is caught or locked (the unscaled delta vector is very small). \b speed is usually (100,100). 
			
		TMouseDelta		delta;				//!< Delta vector containing the mouse movement during the previous frame. Only updated while the mouse is caught or locked.
		TMousePosition	location,			//!< Current cursor location. Only updated while the mouse is not caught or locked.
						previous_location;	//!< Cursor location during the previous frame. Only updated while the mouse is not caught or locked.
		TMouseButtons	buttons;			//!< Current button state
		InputMap		&map;				//!< Input map reference
			
		struct CursorType		//! Cursor appearance
		{
			enum eCursor		//!< Cursor appearance
			{
				None,			//!< No cursor. Ignored (no changes)
				Hidden,			//!< Hidden (invisible) mouse cursor
				Default,		//!< Standard cursor
				EditText,		//!< I shaped cursor (usually used to indicate text editing)
				Waiting,		//!< Hourglass
				HalfWaiting,	//!< Cursor with hourglass
				CrossHair,		//!< Crosshair like cursor
				Hand,			//!< Pointer hand
				Help,			//!< Help cursor
				No,				//!< Crossed out circle
				SizeAll,			//!< Resize into all directions
				ResizeLeft,			//!< Left resize cursor
				ResizeRight,		//!< Right resize cursor
				ResizeUp,			//!< Up resize cursor
				ResizeDown,			//!< Down resize cursor
				ResizeDownRight,	//!< Resize cursor pointing bottomright
				ResizeUpLeft,	    //!< Resize cursor pointing upleft
				ResizeUpRight,		//!< Resize cursor pointing upright
				ResizeDownLeft,		//!< Resize cursor pointing bottomleft
				Count				//!< Number of different cursor styles
			};
		};
			
		typedef CursorType::eCursor	eCursor;
		typedef CursorType::eCursor	cursor_t;
			
					
		#if SYSTEM_VARIANCE==LINUX
			typedef	Cursor	HCURSOR;
		#elif SYSTEM!=WINDOWS
			#error stub
		#endif
			
	private:
		HCURSOR		cursor_reference[CursorType::Count],
					loaded_cursor;
		bool		custom_loaded;
	public:
	

		enum eButton	//! Button definition
		{
			LeftButton,		//!< Left mouse button identifier
			MiddleButton,	//!< Middle mouse button identifier
			RightButton,	//!< Right mouse button identifier
			MButton4,		//!< Mouse button 4 identifier
			MButton5		//!< Mouse button 5 identifier
		};

		/**/		Mouse(InputMap&map);
		BYTE		GetButton()							const;				//!< Queries the id of the button who's handler (down/up) is currently evoked.  \return id of the button being pressed or released (0=left button, 1=middle button, etc). The result is undefined if this method is not called as an immidiate result of a button event
		void		SetCustomCursor(HCURSOR cursor);
		void		SetCursor(eCursor cursor);								//!< Changes the mouse cursor appearance
		bool		CustomCursorIsBound()	const	{return custom_loaded;}
		void		BindWheel(FWheelLink wLink);								//!< Unbinds/binds the specified wheel event handler to the mouse wheel. \param WLink Mouse wheel event handler or NULL to unbind the mouse wheel event.
		bool		HideCursor(bool force=true);							//!< Hides the mouse cursor while above the active window. @param force Set true to disallow subsequent setCursor() invokations to alter show the cursor \return true if the mouse cursor could be hidden, false otherwise.
		void		ShowCursor(bool override=true);							//!< Shows the mouse cursor again. @param override Set true to override (and clear) the forced flag
		bool		CursorIsNotDefault()	const;

		/*!	\brief Locks the mouse cursor to the center of the screen.
								
		While caught Mouse will constantly reset the mouse position to the center of the screen and write the respective cursor movement 
		to the delta status variable. In combination with hideCursor() this can be used to set up a first person perspective, directly rotating
		a camera object, when the user moves the mouse.
		The lock is temporarily removed when the window loses its focus and reacquired when it regains focus.
		Use freeMouse() to free the mouse cursor again and restore its former position.
		*/
		void		Trap();
		void		Release();											//!< Frees the mouse cursor (if caught) and restores its position.
		/*!	\brief Locks the mouse cursor to its current position
								
		Similar to Trap(). Lock() will, however, not force the mouse to the center of the screen.
		Instead the mouse cursor will be locked to its current position.
		While locked Mouse will constantly reset the mouse position to its former position and write the respective cursor movement 
		to the delta status variable.
		The lock is temporarily removed when the window loses its focus and reacquired when it regains focus.
		Use Unlock() to unlock the mouse cursor again.
		*/
		void		Lock();
		void		Unlock();												//!< Unlocks the mouse cursor (if locked).
		bool		IsLocked()	const;												//!< Queries the current lock status. \return true if the mouse cursor is currently caught or locked, false otherwise.
		bool		IsIn(float left, float bottom, float right, float top)	const;	//!< Tests if the mouse cursor is currently in the specified region of the active window. \return true if the mouse cursor is currently within the specified rectangle, false otherwise.

		static const char*	CursorToString(cursor_t cursor);						//!< Converts the specified cursor value to a representative string

		const RECT&	GetActiveWindowRect()	const	{return window;}

		#if SYSTEM==UNIX
			bool	SetCursor(Cursor cursor);
			void	DestroyCursor(Cursor cursor);
			Cursor	CreateCursor(BYTE*data, BYTE*mask, int w, int h, int hot_x, int hot_y, int screen);
		#endif
	};

	extern Mouse mouse;

}


#endif
