#ifndef engine_displayH
#define engine_displayH

/******************************************************************

Engine Display

******************************************************************/


#include "renderer/renderer.h"
#include "interface/keyboard.h"
#include "interface/mouse.h"
#include "interface/joystick.h"
#include "interface/screen.h"
#include "timing.h"
#include "aspect.h"
#include "texture_resource.h"
#include "textout.h"
#include "../math/resolution.h"


#if SYSTEM==WINDOWS

#elif SYSTEM==UNIX

	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/Xos.h>
	typedef int DEVMODE;
#endif

#include "../io/log.h"

/*!
	\brief Game engine modules

	The Engine namespace consists of modules directly relating to graphical realtime applications.
*/
namespace Engine
{

#if SYSTEM==WINDOWS

#elif SYSTEM==UNIX
	void				ExecuteEvent(XEvent&event);
#endif


	void logOperations(LogFile*);

	#define ENGINE_LOG(msg)	{if (Engine::log_file) (*Engine::log_file)<<__FILE__<<"("<<(int)__LINE__<<") <"<<__func__<<">: "<<String(msg)<<nl; }


	typedef bool (*pEngineExec)();


	template <class> class  Display;
	class				   Context;


	#define ERR_NO_ERROR					0
	#define ERR_CLASS_REGISTRATION_FAILED   1
	#define ERR_WINDOW_CREATION_FAILED	 	2
	#define ERR_X_CONNECTION_FAILED			3

	class DisplayConfig
	{
	public:
		typedef function<void(const Resolution&, bool, bool)>	FOnResize;
		typedef void (*f_on_resize)(const Resolution&newRes, bool is_final, bool is_full_screen);

		enum border_style_t
		{
			NoBorder,
			FixedBorder,
			ResizableBorder
		};


		String			window_name,
						icon_filename;
		border_style_t	border_style;
		FOnResize		onResize;



		DisplayConfig():border_style(FixedBorder)
		{}
		DisplayConfig(border_style_t border_style_):border_style(border_style_)
		{}
		DisplayConfig(const char*window_name_):window_name(window_name_),border_style(FixedBorder)
		{}
		DisplayConfig(const String&window_name_):window_name(window_name_),border_style(FixedBorder)
		{}
		DisplayConfig(const String&window_name_, bool hide_border_):window_name(window_name_),border_style(FixedBorder)
		{}
		DisplayConfig(border_style_t style, const FOnResize&resize):border_style(style),onResize(resize)
		{}
		DisplayConfig(const char*window_name_, const FOnResize&resize):window_name(window_name_),border_style(ResizableBorder),onResize(resize)
		{}
		DisplayConfig(const String&window_name_, const FOnResize&resize):window_name(window_name_),border_style(ResizableBorder),onResize(resize)
		{}
		DisplayConfig(const String&window_name_, border_style_t style, const FOnResize&resize):window_name(window_name_),border_style(style),onResize(resize)
		{}
		DisplayConfig(border_style_t style, f_on_resize resize):border_style(style),onResize(resize)
		{}
		DisplayConfig(const char*window_name_, f_on_resize resize):window_name(window_name_),border_style(ResizableBorder),onResize(resize)
		{}
		DisplayConfig(const String&window_name_, f_on_resize resize):window_name(window_name_),border_style(ResizableBorder),onResize(resize)
		{}
		DisplayConfig(const String&window_name_, border_style_t style, f_on_resize resize):window_name(window_name_),border_style(style),onResize(resize)
		{}

	};

	typedef function<bool(const Array<String>&)>	DragEventHandler;
	typedef function<void(const Array<String>&)>	DropEventHandler;

	/*!
		\brief	Display root interface

		The class requires a visual interface (i.e. OpenGL) as template parameter

	*/

	template <class GL> class Display:public GL
	{
	private:
			pEngineExec					exec_target;
			bool						context_error,
										region_locked,
										depth_test_locked,
										framebuffer_bound,
										resolution_overridden;
			Resolution					target_buffer_resolution,
										overridden_client_resolution;
			bool						framebuffer_alpha;
			TVec3<float>				camera_location;
	static  SimpleGeometry				pivot,omni,spot,direct;
	static  bool						pivot_defined,
										lights_defined;

	#if SYSTEM==WINDOWS
	inline  void						process(const MSG&msg);
	#elif SYSTEM==UNIX
	inline  void						process(XEvent&event);
	#endif
			RECT						transform(const TFloatRect&rect);
			RECT						transform(const TFloatRect&rect, const Resolution&res);

	public:

	#undef FORWARD
	/*!
	  \def FORWARD
	  Methods that use this simply foward to the underlieing context class
	*/
	#define FORWARD static inline

			TextureResource<GL>			archive;			//!< gl specific archive
			TVisualConfig				config;			 	//!< startup configuration

										Display();
										/*!
											\brief Creates the eve interface
											\return true on success

											create() creates a window and binds the GL rendering context to the window.
											Successive calls to create() will have no effect until the Display context
											has been destroyed via close(). The window and rendering context will
											be destroyed automatically if the Display object instance is deleted.
										*/
	inline	bool						create(const DisplayConfig&config = DisplayConfig());	//!< @overload
	FORWARD bool						hideCursor();   				//!< Hides the mouse cursor while above the active window. A created context is required. \return true if cursor is hidden
	FORWARD void						showCursor();   				//!< Shows cursor again if it has previously been hidden.
										/*!
											\brief Changes the location and size of the associated window
											\param left New left coordinate of the window in pixels (relative to the left screen border)
											\param top New top coordinate of the window in pixels (relative to the top screen border - facing downwards)
											\param width New window width in pixels
											\param height New window height in pixels

											localWindow() redefines the location and size of the active window. If no window is active then
											the specified window location will be applied during the next create() call.
											Depending on the underlying system, the window size either specifies the size
											of the inner client area or the windoe including its border (if any).
										*/
	inline  void						locateWindow(unsigned left, unsigned top, unsigned width, unsigned height);
	inline  void						locateWindow(const RECT&rect);	//!< Identical to the above localeWindow() using a RECT struct.
										/*!
											\brief Changes the size of the associated window
											\param width New window width in pixels
											\param height New window height in pixels

											resizeWindow() redefines the size of the active window. If no window is active then
											the specified window dimension will be applied during the next create() call.
										*/
	inline  void						resizeWindow(unsigned width, unsigned height, DisplayConfig::border_style_t style);
	inline	void						setDimension(unsigned width, unsigned height, DisplayConfig::border_style_t style);	//!< @copydoc resizeWindow()
	inline	void						setDimensions(unsigned width, unsigned height, DisplayConfig::border_style_t style);	//!< @copydoc resizeWindow()
	inline	void						setSize(unsigned width, unsigned height, DisplayConfig::border_style_t style);	//!< @copydoc resizeWindow()

	void								SignalWindowResize(bool final);

	FORWARD const RECT&					windowLocation();	//!< Retrieves the current window location and size \return RECT Struct containing the current window location.
	FORWARD	unsigned					width();			//!< Queries the current window width
	FORWARD	unsigned					height();				//!< Queries the current window height
	inline	UINT						clientWidth()			const;	//!< Queries the current window's inner width
	inline	UINT						clientHeight()			const;	//!< Queries the current window's inner height
	inline	float						pixelAspect()	const;	   //!< window pixel-aspect
	FORWARD	Resolution					size();							//!< Queries the current window size
	inline	Resolution					clientSize()			const;	//!< Queries the current window's inner size
	inline	Resolution					currentTargetResolution()				const;
	FORWARD	Resolution					dimension();					//!< Identical to size()
	FORWARD	Resolution					dimensions();					//!< Identical to size()
	FORWARD void						queryScreen(ResolutionList*r_list, FrequencyList*f_list=NULL, DWORD w_min=0, DWORD h_min=0, DWORD f_min=0);
	FORWARD	Resolution					getScreenSize();
			RECT						transformViewport(const TFloatRect&rect, const Resolution&clientSize);
	inline  void						setScreen(const TDisplayMode&mode);
	FORWARD short						getRefreshRate();
	#if SYSTEM==WINDOWS
	FORWARD bool						getScreen(DEVMODE&mode);
	FORWARD bool						getScreen(DEVMODE*mode);
	FORWARD bool						isCurrent(const DEVMODE&screen);
	FORWARD void						AcceptFileDrop();
	FORWARD void						AcceptFileDrop(const DragEventHandler&, const DropEventHandler&);
	FORWARD void						BlockFileDrop();

	#elif SYSTEM==UNIX
	FORWARD int							findScreen(DWORD width, DWORD height, DWORD&refresh_rate);
	/*FORWARD const XRRScreenSize&		getScreen(unsigned index);
	FORWARD bool						getScreen(XRRScreenSize*size);
	FORWARD bool						getScreen(XRRScreenSize&size);
	FORWARD bool						isCurrent(const XRRScreenSize&size);*/
	#endif

			void						renderSomething();
	inline	void						renderPivot();		//!< Identical to renderSomething()
	inline	void						renderToken();		//!< Identical to renderSomething()
			void						renderLights();		//!< Renders the active light scenario in line geometries

	inline  bool						applyScreen();
	inline  bool						applyWindowScreen(DWORD refresh_rate=0);
	inline  void						resetScreen();
	inline	void						capture(FloatImage&target);
	inline	void						capture(Image&target);
	inline	void						capture(typename GL::Texture&target);
	inline	void						captureDepth(typename GL::Texture&target);

			void						interruptCheckEvents();
			void						assign(pEngineExec target);		//!< Assigns a new loop-function \param target Callback function of the form bool func(). The function may return false causing execute() to return
			void						execute();					  	//!< Executes the assigned loop-function
			void						executeNoClr();				 //!< Executes the assigned loop-function without clearing the screen per loop
			void						destroy();					//!< Terminates engine execution
			void						terminate();				//!< \deprecated {use close() instead}
			String						errorStr();					 //!< Returns a string representation of the last occured error. \return String representation of the last occured error

			void						lockRegion();					   //!< Prevent subsequent pick(...) or pickCentered(...) calls from affecting the render region
			void						unlockRegion();					 //!< Allow subsequent pick(...) or pickCentered(...) calls to affect the render region
			void						pickRegion(const TFloatRect&rect);   //!< Pick rendering region \param rect New render region
	template <class C>
			void						pick(const Aspect<C>&aspect);	//!< Picks aspect (i.e. a camera) for rendering \param aspect Aspect that should be used from now on
	template <class C>
			void						pickCentered(const Aspect<C>&aspect);				//!< Picks aspect (i.e. a camera) as if it were located in the point of origin \param aspect Aspect that should be used from now on
			bool						bindFrameBuffer(const typename GL::FBO&pobj);	 //!< Binds a pixel buffer object for rendering \param pobj Pixel buffer object that should be rendered to
			void						TargetBackbuffer();										//!< Unbinds bound pixel buffer object

			void						overrideSetClientResolution(const Resolution&res);

	};


	class Context
	{
	public:
		typedef LRESULT (*EventHook)(HWND window, UINT Msg, WPARAM wParam,LPARAM lParam);
	private:
			pEngineExec					exec_target;
	#if SYSTEM==WINDOWS

			EventHook					eventHook;
			HINSTANCE					hInstance;
			HWND						hWnd;
	static	HHOOK						hHook;
	static	unsigned					hook_counter;
			bool						class_created;
	static  LRESULT CALLBACK			WndProc(HWND hWnd, UINT Msg, WPARAM wParam,LPARAM lParam);
	static 	LRESULT CALLBACK 			mouseHook(int nCode, WPARAM wParam,   LPARAM lParam);
	static	void						installHook();
	static	void						uninstallHook(bool force=false);
	#elif SYSTEM==UNIX
			Display						*display;
			Window						window; //is long unsigned int
			int							screen;
			Colormap					colormap;   //is long unsigned int
			TWindowAttributes			window_attributes;
			bool						function_key_mask[0x100];

	friend  void						ExecuteEvent(XEvent&event);

	#endif

			void						looseFocus();
			void						restoreFocus();
			void						locateWindow();
			void						eventClose();


	protected:
			int								_error;
			DWORD							_trate;
			RECT					 		_location,
											client_area;
			DisplayConfig::border_style_t	border_style;
			DisplayConfig::FOnResize		onResize;
	public:
			bool						shutting_down;

										Context();

		template <typename T>
			void						GetAbsoluteClientRegion(Rect<T>&rect)		const
			{
				rect.x.min = client_area.left;
				rect.y.min = client_area.top;
				rect.x.max = client_area.right;
				rect.y.max = client_area.bottom;
			}

			

	#if SYSTEM==WINDOWS
			void						RegisterEventHook(EventHook hook)	{eventHook = hook;}
			HWND						window()	const;
			HWND						getWindow()	const
										{
											return window();
										}
			HWND						createWindow(const String&window_name, DisplayConfig::border_style_t border_style, const DisplayConfig::FOnResize&onResize, const String&icon_filename);
			HWND 						createChildWindow(HWND parent, bool enabled);
			void						setWindow(HWND);
			WString						createClass();
			void						destroyClass();
	#elif SYSTEM==UNIX
			Display*					connect();
			Display*					connection();
			void						disconnect();
			Window						createWindow(const String&window_name, const TWindowAttributes&attributes, DisplayConfig::border_style_t border_style, const DisplayConfig::FOnResize&onResize, const String&icon_filename);
			void						setWindow(Window);
	#endif

			bool						hideCursor();
			void						showCursor();
			void						resizeWindow(unsigned width, unsigned height, DisplayConfig::border_style_t border_style);
			void						locateWindow(unsigned left, unsigned top, unsigned width, unsigned height);
			void						locateWindow(const RECT&dimensions);
			const RECT&					windowLocation()						const;
			const RECT&					windowClientArea()						const;
			Resolution					windowSize()							const;
			Resolution					clientSize()							const;
			UINT						clientWidth()							const;
			UINT						clientHeight()							const;
			RECT						transform(const TFloatRect&field)		const;
			static RECT					transform(const TFloatRect&field, const Resolution&);
			void						destroyWindow();

			void						signalResize(bool is_final, bool is_maximized);

			bool						isTopWindow()							const;
			void						focus();
			bool						isFocused()								const;
			bool						isMinimized()							const;

			void						checkFocus();	//sends isFocused() to eveMouse.setFocused()

			float						pixelAspectf()							const;
			double						pixelAspectd()							const;
			float						windowAspectf()							const;
			double						windowAspectd()							const;

			unsigned					countScreenModes();
			Resolution					getScreenSize();
			short						getRefreshRate();
	#if SYSTEM==WINDOWS
			void						AcceptFileDrop();
			void						AcceptFileDrop(const DragEventHandler&, const DropEventHandler&);
			void						BlockFileDrop();


			DEVMODE*					getScreen(unsigned index);
			bool						isCurrent(const DEVMODE&screen);
			bool						getScreen(DEVMODE*mode);
			bool						getScreen(DEVMODE&mode);
	#elif SYSTEM==UNIX

			int							findScreen(DWORD width, DWORD height, DWORD&refresh_rate);
	#endif


			void						setScreen(DEVMODE&screen);
			void						setScreen(const TDisplayMode&mode);
			void						setScreen(DEVMODE*screen);
			bool						applyScreen();
			bool						applyWindowScreen(DWORD refresh_rate=0);
			void						resetScreen();

			void						queryScreen(ResolutionList*r_list, FrequencyList*f_list=NULL, DWORD w_min=0, DWORD h_min=0, DWORD f_min=0);

			void						assign(pEngineExec target);
			void						execute();
			void						handleEvents();
			void						close();

			

			unsigned					errorCode();
			const char*					errorStr();
	};


	/*!


		\fn void Display::queryScreen(ResolutionList*r_list, FrequencyList*f_list=NULL, DWORD w_min=0, DWORD h_min=0, DWORD f_min=0)
		\brief Queries possible screen resolutions
		\param r_list Pointer to output resolution list
		\param f_list Pointer to output frequency list or NULL
		\param w_min  Minimum screen width (for resolution to be listed)
		\param h_min  Minimum screen height (for resolution to be listed)
		\param f_min  Minimum refresh rate (for resolution to be listed)

		queryScreen() queries the main screen for all possible resolutions and writes them to \a r_list. If \a f_list is not \a NULL then all possible
		refresh rates are listed separatly to \a f_list. \a w_min, \a h_min and \a f_min can be used to exclude smaller screen resolutions.


		\fn void Display::setScreen(const DisplayMode&mode)
		\brief Sets the active screen resolution target
		\param mode Const reference to the target screen mode

		The method merely sets the resolution target. To change the resolution to the specified target call \a applyScreen()


		\fn short Display::getRefreshRate()
		\brief Returns the current screen refresh rate
		\return Current screen refresh rate


		\fn void Display::renderSomething()
		\brief Renders a 1x1x1 colored non-textured token to the point of origin

		renderSomething() (renderPivot()/renderToken()) can be very useful when setting up a new application where cameras may be misplaced.
		The rendering process of this token is fairly rubust and can cope with most aversive rendering setups.

		\fn bool Display::applyScreen()
		\brief Applies the target screen resolution to the screen
		\return true on success

		This method applies the target screen resolution (set by \a setScreen(...) ) to the main screen.

		\fn bool  Display::applyWindowScreen(DWORD refresh_rate=0)
		\brief Uses the active window dimensions as screen resolution
		\param refresh_rate Target refresh rate. 0 implies highest possible.
		\return true on success

		\fn void  Display::resetScreen()
		\brief Restores the native screen resolution


	*/



	extern Context				  	context;
	extern LogFile					*log_file;

}


#include "display.tpl.h"
#endif
