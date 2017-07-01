#ifndef multi_windowH
#define multi_windowH

#include "../renderer/opengl.h"
#include "../display.h"
#include "../../image/image.h"
#include "raster_font.h"
#include "outline_font.h"
#include "../../general/ref.h"
#include "../../container/sortedlist.h"
#include "../../container/binarytree.h"
//#include <io/log.h>
#include "../../general/system.h"



//Log out("log.txt",true);


#if SYSTEM==WINDOWS
    #include <windows.h>
#endif


namespace Engine
{
	#define	MW_CREATE_WINDOW_REQUEST	0x1001


	#define WINDOW_CONTEXT_CLASS_NAME   L"Multi Window Class"
	
	#define X_MENU_BACKGROUND  0.92,0.92,0.92

	MAKE_SORT_CLASS(WindowSort,window)
	MAKE_SORT_CLASS(ItemIDSort,item_id)


	#if SYSTEM==WINDOWS
	LRESULT CALLBACK            wExecuteEvent(HWND window, UINT Msg, WPARAM wParam,LPARAM lParam);
	#elif SYSTEM==UNIX
	void 						wExecuteEvent(XEvent&event);

	typedef Screen*	HMONITOR;

	#endif


	class Window;
	class Application;

	extern Application		application;



	struct TWindowCreationRequest
	{
		Window				*window;
		String				name;
		int					x,
							y;
		UINT				width,
							height;

		Engine::TVisualConfig	config;
		const Window		*parent;

							TWindowCreationRequest(Window*window_)
							{
								window = window_;
								x = 0;
								y = 0;
								width = 600;
								height = 600;
								config = Engine::default_buffer_config;

								parent = NULL;
							}
	};


	/**
	\brief Single menu entry

	MenuEntry describes one menu entry. A menu entry may be a group opening a sub menu on select or a simple entry which executes a handler on select.
	*/
	class MenuEntry
	{
	protected:
		List::Vector<MenuEntry>		children;		//!< Children of this menu enry (if any)
		MenuEntry					*parent;		//!< Pointer to the respective parent menu entry or NULL if no such exists
		bool						enabled,		//!< True if this element is enabled/can be executed
									checked,		//!< True if this element is checked (check symbol or dot)
									radio,			//!< True if this element should appear as a radio group element
									open,			//!< True if this element's sub menu is currently open (linux only)
									mouse_over;		//!< True if the mouse currently hovers over this menu entry (linux only)
		String						caption;		//!< Visible caption of this menu entry
		UINT						item_id,			//!< Entry identifier (windows only)
									caption_offset_x,	//!< Horizontal relative caption offset (linux only)
									caption_offset_y;	//!< Vertical relative caption offset (linux only)
		RECT						core_region,
									full_region,
									child_region;
		friend class Menu;
		friend class Window;
		friend struct ItemIDSort;
	public:
		#define MENUCALLBACK	//!< Menu callback annotation
			
		enum	FindType		//! Lookup configuration
		{
			Create,				//!< Create missing menu entries
			Exact,				//!< Return existing matching entry or NULL
			Closest				//!< Return closest existing menu entry
		};
			
		class	Attachment		//!< Menu entry attachment
		{
		public:
			virtual			~Attachment()	{}
		};
			
		typedef 	void	(Window::*pEntryCallback)(MenuEntry*);
		typedef 	void	(Window::*pCallback)();
		typedef 	void	(*pSimpleEntryCallback)(MenuEntry*);
		typedef 	void	(*pSimpleCallback)();
			
		pCallback					callback;		//!< Event handler method that requires no menu entry as parameter
		pEntryCallback				entry_callback;	//!< Event handler method that requires the menu entry as parameter
		pSimpleCallback				simple_callback;	//!< Plain handler function that requires no menu entry as parameter
		pSimpleEntryCallback		simple_entry_callback;	//!< Plain handler function that requires the menu entry as parameter
			
		std::shared_ptr<Attachment>	attachment;		//!< Custom menu entry attachment
			
		#if SYSTEM==WINDOWS
			HMENU					menu_handle;	//!< Handle to windows menu (if any)
		#endif

		void						updateInfo();	//!< Updates the information attached to the windows menu handle

		MenuEntry();
		MenuEntry(const String&caption);
		virtual ~MenuEntry();
		void						execute(Window*window);	//!< Executes the attached callback pointers. Priorizes \b entry_callback
		void						attach(MenuEntry*entry);		//!< Attaches an entry as child to this menu entry
		bool						isAttached(MenuEntry*entry);	//!< Checks if the specified entry is a child of this menu entry
		bool						detach(MenuEntry*);			//!< Removes the specified entry and deletes it if it is a child of this menu entry
		bool						isEnabled()	const;				//!< Checks if this menu entry is enabled
		MenuEntry*					enable();						//!< Enables the local menu entry. The local object may be NULL \return this
		MenuEntry*					disable();						//!< Disables the local menu entry. The local object may be NULL \return this
		MenuEntry*					setEnabled(bool enabled);		//!< Updates the local menu entry enabled state. The local object may be NULL \return this
		bool						isChecked()	const {return this&&checked;};	//!< Checks if this menu entry is currently checked
		MenuEntry*					check();						//!< Updates the checked-state of the local menu entry to true. The local object may be NULL \return this
		MenuEntry*					uncheck();						//!< Updates the checked-state of the local menu entry to false. The local object may be NULL \return this
		MenuEntry*					setChecked(bool checked);		//!< Updates the checked-state of the local menu entry. The local object may be NULL \return this
		MenuEntry*					setRadio(bool is_radio=true);	//!< Changes the appearance of the local menu entry. The local object may be NULL \return this
		MenuEntry*					unsetRadio();					//!< Changes the appearance of the local menu entry. The local object may be NULL \return this
		bool						hasCallback()	const;			//!< Queries whether or not this menu entry has a callback method/function pointer attached
		const String&				getCaption()	const;			//!< Retrieves the local menu entry caption
		MenuEntry*					getParent()		const	{return parent;}
		MenuEntry*					setCaption(const String&);		//!< Updates the local menu entry caption \return this
		MenuEntry*					find(char*buffer,FindType type);	//!< Attempts to find a menu entry via its path
		void                        render(Window*window);				//!< Renders the local menu entry using the specified window
		static	void				include(const RECT&,RECT&);
		static	bool				in(int x, int y, const RECT&rect);	//!< Checks if the specified coordinates are within the specified rectangle
		void 						updateRegion(Window*window, bool spawn_right);
		MenuEntry*					mouseMove(int, int);
		void						close();								//!< Closes this menu entry
		void						setCallback(pEntryCallback callback_);
		void						setCallback(pCallback callback_);
		void						setCallback(pSimpleCallback callback_);
		void						setCallback(pSimpleEntryCallback callback_);
	};

	/**
	\brief Window menu

	Menu describes a menu bar attached to one window
	*/
	class Menu:public MenuEntry
	{
	public:
		MenuEntry					*last_focus,	//!< Last detected focused menu entry
									*down_focus;	//!< Menu entry that was underneath the mouse when the left button was pressed
	public:
		Menu();
		void						update(int x, int y, Window*window);

		bool						mouseMove(int, int, bool&);
		bool						mouseDown(bool&);
		bool						mouseUp(Window*);
		bool						close();
	};

	class Window
	{
	public:
		typedef GLRasterFont	Font;
		enum Error
		{
			NoError,
			ContextError,
			ConfigUnsupported,
			PixelFormatSetFailed,
			WindowCreationFailed,
			UnableToRetrieveDC,
			ContextCreationFailed,
			ConfigRejected,
			ColorMapCreationFailed,
			ContextNotBindable
		};
			
		typedef void (Window::*pMethod)();
		#if SYSTEM==WINDOWS
			struct				ContextHandle
			{
				HGLRC					gl_context;
				HDC						device_context;     //!< Handle of the device window_context
			};
		#elif SYSTEM==UNIX
			typedef GLXContext			ContextHandle;
		#else
			#error Whatever system you're using, it's not supported yet. Sorry
		#endif

	private:
		#if SYSTEM==WINDOWS
			friend  LRESULT CALLBACK    wExecuteEvent(HWND window, UINT Msg, WPARAM wParam,LPARAM lParam);

		#elif SYSTEM==UNIX
			Colormap                    colormap;   //is long unsigned int
			XVisualInfo                *visual;             //!< Handle of the chosen visual
			friend  void                wExecuteEvent(XEvent&event);

		#else
			#error Whatever system you're using, it's not supported yet. Sorry
		#endif
		ContextHandle					local_context;         //!< Handle of the OpenGL window_context
		HWND                        	window;

		volatile bool					requires_update;

		bool                            created,
										is_new,
										hidden,
										focused,
										accept_dropped_files;

		Error                           error;
		int								client_offset_x,
										client_offset_y;

		//InputProfile					profile;
		unsigned		                client_width,
										client_height,
										font_remake_counter;
		void (*file_drop_handler)(const Array<String>&files);

		HMONITOR		                screen;
			
	protected:
		Menu							menu;		//!< Menu container
		String			                name;		//!< Window name. Changing this attribute will \b not change the visual caption of the window
	public:
		RECT			                location;	//!< Absolute location of this window with 0,0 being the topleft screen corner
		Display<OpenGL>					display;	//!< Display structure associated with this window
		Textout<Font>					textout;	//!< Textout handler associated with this window
		InputMap						input;		//!< General input interface
		Keyboard						keyboard;	//!< Keyboard handler associated with this window
		Mouse							mouse;		//!< Mouse handler associated with this window
		Engine::TVisualConfig			config;
	private:
		Image							desktop_background;
		Mutex							update_mutex;
		bool							showingBorder;
		volatile int					in_mutex;
		volatile bool					ignoring_events;
			
		friend class Application;
		friend class MenuEntry;
		friend class Menu;
		friend struct WindowSort;

		void							init();
		//bool 							create(int x, int y, int width, int height, const String&name, const Engine::TVisualConfig&config,const Window*parent=NULL);
		bool 							create(int x, int y, int width, int height, const String&name, const Engine::TVisualConfig&config, HWND parent = NULL);

		bool							bindContext(ContextHandle&previous_handle);
		void							releaseContext(ContextHandle previous_handle);

		void							bind();
			
		void							forceBeginUpdate(ContextHandle&previous_handle);
		bool            				beginUpdate(ContextHandle&previous_handle);
		void            				endUpdate(ContextHandle previous_handle);
		void							forceUpdate();
		void							detailBeginUpdate(ContextHandle&previous_handle);



		template <typename T>
			MenuEntry*					createMenuEntryT(const String&path, T callback)
		{
			CAString buffer(path.c_str());
			//ShowMessage("'"+path+"' => '"+buffer+"'");
			if (MenuEntry*entry = menu.find(buffer.pointer(),MenuEntry::Create))
			{
				entry->setCallback(callback);
				#if SYSTEM==WINDOWS
					if (created)
					{
						SetMenu(window,menu.menu_handle);
						DrawMenuBar(window);
					}
				#endif
				return entry;
			}
			return NULL;
		}


	public:
		Window();
		Window(const String&name);
		Window(int x, int y, int width, int height, const String&name);
		Window(int x, int y, int width, int height, const String&name, const Engine::TVisualConfig&config);
		virtual	~Window();

		static ContextHandle	getCurrentContext()
								{
									#if SYSTEM==WINDOWS
										ContextHandle rs;
										rs.gl_context = wglGetCurrentContext();
										rs.device_context = wglGetCurrentDC();
										return rs;
									#else
										return glXGetCurrentContext();
									#endif
								}
		virtual	void			onPaint()		{};
		virtual	void			onFocus()		{};
		virtual	void			onUnFocus()		{};
		virtual	void			onMove()		{};
		virtual	void			onClick()		{};
		virtual	void			onCreate()		{};		//!< This callback is invoked after the window and OpenGL context have been created but before it is painted the first time. During invokation a valid OpenGL context exists
		virtual	void			onDestroy()		{};
		virtual	void			onMouseMove()	{};
		virtual	void			onMouseWheel(short delta)
								{
									mouse.HandleMouseWheel(delta);
								}
		virtual	void			onFileDrop(const Array<String>&files);
		virtual	void			onChar(char c)
								{
									keyboard.input(c);
								}
		virtual	void			onKeyDown(Key::Name key)						{};
		virtual	void			onKeyUp(Key::Name key)						{};

		#if SYSTEM==WINDOWS
			HDC					GetDC()	const	{return local_context.device_context;}
		#endif

		/**
			@brief Set whether or not system-events (such as redraw or button events) should be ignored
		*/
		void					ignoreEvents(bool);
		/**
			@brief Queries, whether or not the local window is currently in an update-procedure. Calls to update() will be ignored if it is
		*/
		bool					isUpdating()	const;
		void					update();
		void					updateIfRequired();
		void					destroy();
		bool					remake();
		bool					make(const String&name);
		bool					make(int x, int y, int width, int height, const String&name);
		bool					make(int x, int y, int width, int height, const String&name, const Engine::TVisualConfig&config);
		bool					makeAsChild(const Window&parent, const String&name);
		bool					makeAsChild(const Window&parent, int x, int y, int width, int height, const String&name);
		bool					makeAsChild(const Window&parent, int x, int y, int width, int height, const String&name, const Engine::TVisualConfig&config);
		bool					makeAsChild(HWND parent, const String&name);
		bool					makeAsChild(HWND parent, int x, int y, int width, int height, const String&name);
		bool					makeAsChild(HWND parent, int x, int y, int width, int height, const String&name, const Engine::TVisualConfig&config);

		void					ShowBorder(bool);
		bool					ShowingBorder()	const	{return showingBorder;}


		HWND					getWindow()			const;
		String					getTitle()				const;
		void					setTitle(const String&);
		bool					show();
		void					hide();
		void					stayOnTop(bool);
		void					moveTo(int x, int y, int width, int height);
		bool					isCreated()			const;
		bool					requiresUpdate()	const;
		bool					isVisible()			const;
		void					callEmbedded(void (Window::*embedded)());
		void					callEmbedded(void (Window::*embedded)(void*), void*);
		void					callEmbedded(void (*embedded)());
		void					callEmbedded(void (*embedded)(void*), void*);
		template <typename F>
		inline void				embed(F function)
								{
									ContextHandle prev;
									if (bindContext(prev))
									{
										function();
										releaseContext(prev);
										return;
									}
									FATAL__("Unable to embed");
								}

		void					SignalWindowContentChange(bool);
		void					SignalWindowContentChange();

		bool					require(unsigned extensions);
		const Image*			getDesktop();
		unsigned				clientWidth()		const;
		unsigned				clientHeight()		const;
		float					clientAspect()		const;
		Resolution				clientSize()		const;
		unsigned				clientOffsetX()		const;
		unsigned				clientOffsetY()		const;
		unsigned				originX()			const;
		unsigned				originY()			const;
		unsigned				height()			const;
		unsigned				width()				const;
		void					SetSize(UINT w, UINT h);
		void					assembleClientRect(RECT&)	const;	//!< Fills the specified rect with the absolute location of the client region of this window
		bool 					isFocused()			const;
		POINT					getMouse(bool relative)	const;
		void					getRelativeMouse(float&x, float&y)	const;
		void					relativate(const POINT&p, float&x, float &y)	const;
		template <typename T>
			void				GetAbsoluteClientRegion(Rect<T>&rect)		const
			{
				rect.x.min = location.left+client_offset_x;
				rect.y.min = location.top+client_offset_y;
				rect.x.max = rect.x.min + client_width;
				rect.y.max = rect.y.min + client_height;
			}
		bool					hasMouse()			const;

		void					acceptFileDrop();
		void					acceptFileDrop(void (*handler)(const Array<String>&files));
		void					blockFileDrop();

		void					pick(const Engine::Aspect<float>&aspect);



		void					clearMenu();
		MenuEntry*				createMenuEntry(const String&path,Menu::pCallback callback);
		MenuEntry*				createMenuEntry(const String&path,Menu::pEntryCallback callback=NULL);
		MenuEntry*				createMenuEntry(const String&path,Menu::pSimpleCallback callback);
		MenuEntry*				createMenuEntry(const String&path,Menu::pSimpleEntryCallback callback);
		MenuEntry*				findMenuEntry(const String&path);
		void					destroyMenuEntry(MenuEntry*entry);
		bool					renameMenuEntry(MenuEntry*entry, const String&new_path);
	};

	class Application
	{
	public:
		enum Error
		{
			NoError,
			ClassRegistrationFailed,
			XConnectionFailed
		};

		#if SYSTEM==UNIX
			typedef void	(*EventHook)(Window*,XEvent&event);
		#elif SYSTEM==WINDOWS
			typedef	LRESULT (*EventHook)(Window*,HWND window, UINT Msg, WPARAM wParam,LPARAM lParam);
		#endif


	private:

		#if SYSTEM==UNIX
			::Display    *display;
			bool		function_key_mask[0x100];
			friend  void                        wExecuteEvent(XEvent&event);
		#elif SYSTEM==WINDOWS
			friend  LRESULT CALLBACK            wExecuteEvent(HWND window, UINT Msg, WPARAM wParam,LPARAM lParam);
			//friend	LRESULT CALLBACK			Hook::callback(int nCode,WPARAM wParam,LPARAM lParam);

		#endif

		bool			built;
		Error			error;

		HINSTANCE		hInstance;
		Sorted<List::ReferenceVector<Window>,WindowSort>   windows;
		EventHook		event_hook;


		String			fatal_exception;
		bool			fatal_exception_caught;

		bool			shutting_down,
						realtime;
		Window			*active_window,
						*creating_window;
		HMONITOR		active_screen;
		Thread			callback_thread;
	
		UINT16			menu_id;
			
		List::ReferenceBinaryTree<MenuEntry,ItemIDSort>	menu_items;

		int
		#if SYSTEM==UNIX
						x_menu_height,
		#endif
						x_menu_font_size;
		String			x_menu_font;

		friend class	MenuEntry;
		friend class	Menu;
		friend class	Window;
	public:
		/**/			Application();
		virtual			~Application();

		bool			Create();
		bool			Create(const String&icon_name);
		void			Destroy();
		#if SYSTEM==UNIX
			#undef IsFunctionKey
			bool		IsFunctionKey(unsigned key);
		#endif

		void			SetFontName(const String&name);
		void			InterruptCheckEvents();
		void			Execute(bool(*callback)());
		void			Execute();
		void			EnterRealtimeMode();
		void			EnterLazyMode();
		void			Terminate();
		Window*			FindWindow(HWND handle);
		Window*			GetFocusedWindow();
		void			RegisterEventHook(EventHook hook);
		void			ClearEventHook()	{RegisterEventHook(NULL);}
	};


}

#endif
