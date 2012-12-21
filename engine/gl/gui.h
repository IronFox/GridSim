#ifndef gl_guiH
#define gl_guiH



#include "../renderer/opengl.h"
#include "../display.h"
#include "../../math/vector.h"
#include "../../io/file_system.h"
#include "../../io/xml.h"
#include "../../image/converter/magic.h"
#include "../../container/sorter.h"

#include "texture_font2.h"
#include "../../structure/event.h"
#include <memory> 

//#include "../../boost_integration/pointer.h"


//#define DEEP_GUI



namespace Engine
{
	
	/**
		@brief Advanced embedded window system
	*/
	namespace GUI
	{
		class Operator;

		struct TCellLayout;

		
		static const float BorderWidth = 7;	//!< Resize border of windows in pixels
		
		static const float font_offset = -2;	//!< Vertical font offset
		
		static const float global_anisotropy = 20;
		
		/**
			@brief Simple hybrid texture containing both color and normal maps
		*/
		struct TTexture
		{
		public:
				float					width,	//!< Width of the texture (in scaled pixels)
										height;	//!< Height of the texture (in scaled pixels)
				OpenGL::Texture			color,	//!< Color map of the texture
										normal;	//!< Normal map of the texture
		static	void					load(XML::Node*node, const char*aname, FileSystem::Folder&folder, TTexture&out, float scale);
				void					adoptData(TTexture&other)
										{
											width = other.width;
											height = other.height;
											color.adoptData(other.color);
											normal.adoptData(other.normal);
											other.width = 0;
											other.height = 0;
										}
		};
		
				
		struct TFreeCell	//! Free component layout cell
		{
				Rect<float>			region;			//!< Effective cell region
				BYTE					orientation;	//!< Cell orientation. Rotation is 90 degress times orientation
				const OpenGL::Texture	*color,			//!< Cell color texture
										*normal;			//!< Cell normal texture

										TFreeCell():color(NULL),normal(NULL)
										{}
										
		static	void					paintColor(Display<OpenGL>&display, const TFreeCell&);
		static	void					paintNormal(Display<OpenGL>&display, const TFreeCell&, bool);
									
		};
		
		
		/**
			@brief Window layout cell
			
			Each window layout cell contains a relative location and texture used to render the cell
		*/
		struct TCell
		{
		private:
				TCell&					operator=(const TCell&other);
		public:
				float 					width;			//!< Cell width. Constant if @b variable_width is false. Otherwise updated by WindowLoayout::locateCells()
				bool					variable_width;	//!< Indicates whether or not this cell has a fixed or variable width
				OpenGL::Texture			color_texture,	//!< Texture of this cell affecting plain color
										normal_texture;	//!< Texture of this cell affecting normals
				
				void					adoptData(TCell&other)
										{
											width = other.width;
											variable_width = other.variable_width;
											color_texture.adoptData(other.color_texture);
											normal_texture.adoptData(other.normal_texture);
											other.width = 0;
											other.variable_width = false;
										}
		};
		
		struct TIOCell	//! Simplified io layout cell used during layout loading
		{
				Image					normal,	//!< Raw normal image
										color;	//!< Raw color image
				bool					variable_width;	//!< Cell is of variable width
		};
		
		struct TIORow	//!< Simplified io layout row used during layout loading
		{
				List::Vector<TIOCell>	cells;	//!< Cell container
				bool					variable_height;	//!< Row is of variable height
		};


		/**
			@brief Final layout cell row.
			
			Each row can hold any number of cells, both of fixed and variable width. The row itself may be of fixed or variable height.
		*/
		struct TRow
		{
		protected:
				TRow&					operator=(const TRow&other) {return *this;}
		public:
				Array<TCell>			cells;	//!< Layout cell container
				float					height;	//!< Row height
				bool					variable_height;	//!< True if this row is of variable height
				count_t					variable_cells;		//!< Number of cells with non-fixed widths. Always at least 1
				float					fixed_width;		//!< Summed width of all cells with fixed widths in this row
		};
		
		/**
			@brief General window/component layout
			
			A layout is a collection of cells arranged in rows. Layouts may be applied to both components and windows.
		*/
		class Layout
		{
		protected:
		static	void					applyArea(Rect<float>&target, const Rect<float>&window, const Rect<float>&relative);
				Layout&					operator=(const Layout&other) {return *this;}
		public:
				Array<TRow>				rows;				//!< Collection of rows
				Rect<float>				title_position,		//!< Title position.	Negative values are interpreted relative to the right/top edges, positive ones to the left, bottom edges
										border_edge,		//!< Distance from the window/component edge to the effective (visual) edge of the layout. All values are >= 0
										client_edge;		//!< Distance from the window/component edge to the client edge of the layout. All values are >= 0
				float					min_width,			//!< Minimum width of this layout
										min_height;			//!< Minimum height of this layout
				count_t					variable_rows,		//!< Number of rows with non-fixed heights. Always at least 1
										cell_count;			//!< Total number of cells in this layout
				Layout					*override;			//!< Layout override

										Layout();

				/**
					@brief Loads a layout from the specified xml file.
					
					The method requires a valid OpenGL context
					
					@param filename Filename  of the xml file to load
					@param scale Relative scale that should be applied to the loaded layout
				*/
				void					loadFromFile(const String&filename, float scale=1.0f);	
				
				void					updateCells(const Rect<float>&window_location, TCellLayout&layout)	const;		//!< Updates the final layout of a window or component depending on the window's location
				
				Layout*				reference()	{return override!=NULL?override:this;}
		};
		
		

		/**
			@brief Instance of a layout cell applied to a window or component.
			
			Each window/component has its own collection of cell instances even if they use the same layout.
		*/
		struct TCellInstance
		{
				Rect<float>				region;			//!< Effective cell region (in pixels)
				const OpenGL::Texture	*color_texture,	//!< Color texture to fill the cell with
										*normal_texture;	//!< Normal texture to fill the cell with
				//float					width;			//!< Effective width of the cell.
				
										TCellInstance():color_texture(NULL),normal_texture(NULL)
										{}
		};

		/**
			@brief Effective (applied) cell layout of a window or component
			
			This layout needs to be updated each time the window/component is resized or its layout is changed.
		*/
		struct TCellLayout
		{
				Array<TCellInstance>	cells;				//!< Container for all cell instances (generally unsorted)
				Rect<float>				title,				//!< Effective (absolute) title region
										border,				//!< Effective (absolute) region of the visual border. Mouse clicks outside this border are generally applied to the next lower window
										client;				//!< Effective (absolute) region of the client area of the layout.
		};
		
		
		
		class Window;
		
		/**
			@brief Root component
			
			Visual components are the abstract foundation of the GUI system.
		*/
		class Component: public enable_shared_from_this<Component>
		{
		private:
		static	void 						charRead(char c);
		protected:
				weak_ptr<Window>			window_link;	//!< Link to the owning window. Not NULL if this component is part of any window.
				friend class Window;
				
		public:
		static	void						keyDown(int key);	//!< Key down event linked to the keyboard interface
		static	void						keyUp(int key);		//!< Key up event linked to the keyboard interface
		
		
				enum eEventResult		//! Generic event result
				{
					Unsupported,		//!< The invoked event type is not supported by the component in its current state or at all
					Handled,			//!< The invoked event has been handled but no visual changes occured
					RequestingRepaint,	//!< The invoked event has been handled and the component requests a window repaint
					RequestingReshape	//!< The invoked event has been handled and the component requests a complete window layout update and repaint
				};

				struct TExtEventResult	//! Struct to hold additional event results of certain event methods
				{
					Mouse::eCursor			custom_cursor;	//!< Cursor to replace the current one with
					shared_ptr<Component>	caught_by;		//!< Component that actually caught the event
				};

				Rect<float>				current_region,	//!< Current component region. This rectangle completely surrounds the component including all cells of its layout (if any)
											offset;			//!< Signed offset from the parent region. Effective only if @b anchored.coord[x] is true. should be negative for right/top offset
				Rect<bool>					anchored;		//!< Indicates that the respective coordinates of the final component region is calculated relative to the respective parent edge.
				float						width,			//!< Fixed component width if either anchored.left or anchored.right is false. Has no effect if both anchored.left and anchored.right are true
											height;			//!< Fixed component height if either anchored.bottom or anchored.top is false. Has no effect if both anchored.bottom and anchored.top are true
		const	String						type_name;		//!< Constant type name of this component. Assigned during construction (usually the class name without the leading 'C')
				TCellLayout					cell_layout;	//!< Effective applied cell layout. This variable is updated even if this component has no layout
				Layout*						layout;		//!< Layout attached to this component or NULL if this component has no layout
		static	Textout<GLTextureFont2>		textout;		//!< Global textout used to render text
				float						tick_interval;	//!< Interval (in seconds) between executions of the onTick() method
				bool						enabled,		//!< Indicates that this item may receive events
											visible;		//!< Indicates that this item is visible. Invisible items are automatically treated as disabled.

		
				
				
												Component(const String&type_name);	//!< Constructs a new component. Requires a type_name
		virtual									~Component();
		virtual	void							updateLayout(const Rect<float>&parent_region);	//!< Updates the applied layout and possibly existing other components to the changed parent region rectangle. Also updates @b current_region @param parent_region Absolute location of the respective parent component or window
		virtual	float							clientMinWidth()	const	{return 0;}			//!< Queries the minimum width of the inner content of this component (excluding the minimum size of the layout)
		virtual	float							clientMinHeight()	const	{return 0;}			//!< Queries the minimum height of the inner content of this component (excluding the minimum size of the layout)
		virtual	float							minWidth(bool include_offsets)	const;			//!< Queries the effective minimum width of this component	@param include_offsets Set true to also include anchor offsets @return Minimum width of this component
		virtual	float							minHeight(bool include_offsets)	const;			//!< Queries the effective minimum height of this component 	@param include_offsets Set true to also include anchor offsets @return Minimum height of this component
		virtual	void							onColorPaint();						//!< Causes this component to repaint its color components
		virtual	void							onNormalPaint();						//!< Causes this component to repaint its normal components
		virtual	shared_ptr<Component>			getFocused()					{return shared_from_this();};	//!< Retrieves the actually focused element from the component returned by onMouseDown(). Returns this by default
		virtual	eEventResult					onMouseDown(float x, float y, TExtEventResult&)		{return Unsupported;};	//!< Triggered if the primary mouse button was pressed over this component @param x Window space x coordinate of the mouse cursor @param y Window space y coordinate of the mouse cursor @return Event result
		virtual	eEventResult					onMouseHover(float x, float y, TExtEventResult&)	{return Unsupported;};	//!< Triggered if the mouse was moved while above this component @param x Window space x coordinate of the mouse cursor @param y Window space y coordinate of the mouse cursor @return Event result
		virtual	eEventResult					onMouseExit()					{return Unsupported;};						//!< Triggered if the mouse has left the area of this component @return Event result
		virtual	eEventResult					onMouseDrag(float x, float y)	{return Unsupported;};						//!< Triggered if the mouse was moved while the primary mouse button is down and this component is the currently clicked component @param x Window space x coordinate of the mouse cursor @param y Window space y coordinate of the mouse cursor @return Event result
		virtual	eEventResult					onMouseUp(float x, float y)		{return Unsupported;};						//!< Triggered if the primary mouse button was released and this component was the clicked component
		virtual	eEventResult					onMouseWheel(float x, float y, short delta)		{return Unsupported;};		//!< Triggered if the mouse wheel was used while the mouse cursor is above this component
		virtual	eEventResult					onFocusGained()					{return Unsupported;}						//!< Triggered if this component has gained the input focus
		virtual	eEventResult					onFocusLost()					{return Unsupported;}						//!< Triggered if this component has lost the input focus
		virtual	eEventResult					onKeyDown(Key::Name key)		{return Unsupported;}						//!< Triggered if the user has pressed a supported key while this component had the focus. This event may fire multiple times before the user releases the key and the onKeyUp() event method is triggered.
		virtual	eEventResult					onKeyUp(Key::Name key)			{return Unsupported;}						//!< Triggered if the user has released a supported key while this component had the focus.
		virtual	eEventResult					onChar(char c)					{return Unsupported;}						//!< Triggered if the user has pressed a character key while this component had the focus.
		virtual	eEventResult					onTick()						{return Unsupported;}						//!< Triggered each time the counter hit @b tick_interval seconds while this component has the focus
		virtual	bool							tabFocusable() const			{return false;}								//!< Queries whether or not this entry is focusable via the tab key
				void							setWindow(const weak_ptr<Window>&wnd);						//!< Updates the local window link variable as well as that of all children (recursively)
				bool							isFocused()	const;															//!< Queries whether or not this component currently has the input focus
				shared_ptr<Window>				window()	const				{return window_link.lock();}						//!< Retrieves the super window of this component (if any)
		virtual	shared_ptr<const Component>		child(index_t) const			{return shared_ptr<const Component>();}							//!< Queries the nth child of this component (if any)
		virtual	shared_ptr<Component>			child(index_t)	{return shared_ptr<Component>();}									//!< @overload
		virtual	count_t							countChildren()	const {return 0;}								//!< Queries the number of children of this component (if any) @return Number of children
		virtual	index_t							indexOfChild(const shared_ptr<Component>&child) const;						//!< Determines the index of the specified child or 0xFFFFFFFF if the specified component is no child of this component.
				shared_ptr<Component>			successorOf(const shared_ptr<Component>&child);									//!< Queries the successor element of the specified one @return successor or NULL if no successor could be found
				void							locate(const Rect<float>&parent_region,Rect<float>&region)	const;	//!< Resolves the absolute location of the local item based on the specified parent region.
		
		static	void 							setFocused(const shared_ptr<Component>&component);								//!< Changes the currently focused component

				shared_ptr<Operator>			getOperator()	const;
				shared_ptr<Operator>			requireOperator()	const;
		};
		
		/**
			@brief Icon container
			
			The icon is considered invisible if the contained texture is empty.
			The values of the other variables have no effect in that case.
		*/
		struct TIcon
		{
			float								aspect;	//!< Pixel aspect (width/height) of the icon
			Rect<float>						texcoords;	//!< Texture coordinates of the icon in the specified texture
			OpenGL::Texture						texture;	//!< Effective (color) texture
		};

		
		
		
		/**
			@brief Window coordinates
			
			The specified coordinates describe the center of the window on the screen
		*/
		struct TScreenPosition
		{
			union
			{
				struct
				{
					float						x,		//!< X coordinates of the window in pixels. This value is multiplied by @b shell_radius to retrieve the absolute coordinates
												y,		//!< Y coordinates of the window in pixels. This value is multiplied by @b shell_radius to retrieve the absolute coordinates
												shell_radius;	//!< Effective shell radius (distance of the window from the screen). A higher shell radius will result in a smaller window
				};
				float							coord[3];	//!< Vector mapping of the above attributes
			};
		};


		/**
			@brief GUI Window
		*/
		class Window: public enable_shared_from_this<Window>
		{
		protected:
				friend class Component;
				friend void	 render();
				//friend bool	 mouseWheel(short delta);
		public:
			
				shared_ptr<Component>		component_link;
				weak_ptr<Operator>			operator_link;
				
				struct ClickResult
				{
					enum value_t	//! Click result returned by the resolve() method
					{
						Missed,			//!< The mouse cursor is not above the window
						Ignored,		//!< The mouse cursor is above the window but a click would have no effect
						Component,		//!< The mouse cursor is above the component
						DragWindow,			//!< The mouse cursor is above the window but not above the edges or the component causing the window to be dragged if the primary mouse button is pressed.
						ResizeTopRight,		//!< The mouse cursor is above the top right resize corner of the window
						ResizeRight,		//!< The mouse cursor is above the right window edge
						ResizeBottomRight,	//!< The mouse cursor is above the bottom right resize corner of the window
						ResizeBottom,		//!< The mouse cursor is above the bottom window edge
						ResizeBottomLeft,	//!< The mouse cursor is above the bottom left resize corner of the window
						ResizeLeft,			//!< The mouse cursor is above the left window edge
						ResizeTopLeft,		//!< The mouse cursor is above the top left resize corner of the window
						ResizeTop			//!< The mouse cursor is above the top window edge
					};
				};
				
		
				#ifdef DEEP_GUI
					TScreenPosition		current_center,	//!< Current shell location of this window (x,y,radius)
										origin,			//!< Origin shell location
										destination;	//!< Target shell location
				#else
					float				x,				//!< Central window X position (center point) in the range [-display.clientWidth()/2, display.clientWidth()/2]
										y;				//!< Central window Y position (center point) in the range [-display.clientHeight()/2, display.clientHeight()/2]
				#endif
				size_t					exp_x,			//!< Current texture exponent along the x axis
										exp_y,			//!< Current texture exponent along the y axis
										iwidth,			//!< Effective (applied) integer width
										iheight;		//!< Effective (applied) integer height
				float					progress,		//!< Animation progress from @b origin to @b destination (0-1)
										fwidth,			//!< Float window width
										fheight,		//!< Float window height
										usage_x,		//!< Usage of the texture buffers in x direction (0-1)
										usage_y;		//!< Usage of the texture buffers in y direction (0-1)
				
				Layout					*layout;		//!< Used layout (if any) or NULL. Unmanaged at this point.
		static	Layout					common_style,	//!< Common window style
										menu_style,		//!< Menu window
										hint_style;		//!< Hint display window
				TCellLayout				cell_layout;	//!< Active layout as applied by the active layout.
				String					title;			//!< Window title. Empty by default
				/*EClickResult			click_result,	//!< Result of the last click event that was caught (or not caught) by this window
										hover_result;	//!< Result of the last mouse hover event that was processed by this window*/
				TFrameBuffer			color_buffer,	//!< Texture buffer to store the color components of the window in
										normal_buffer;	//!< Texture buffer to store the normal components of the window in
				bool					layout_changed,	//!< Indicates that the general window content has changed and requires a layout and render update into the respective color and normal buffers
										visual_changed,	//!< Indicates that the window should be repainted
										is_modal,		//!< True if this window does not allow events to pass further down.
										fixed_position,	//!< Window is fixed to its current location and may neither be resized nor moved
										fixed_size;		//!< Windows has fixed size and may be moved but not resized
				Timer::Time			hidden;			//!< Time stamp when this window was hidden
				
										Window(Layout*style=&common_style);	//!< Creates a new window using the specified style
		virtual							~Window()
										{}
				
				void					setSize(float width, float height);				//!< Updates window dimensions
				void					setWidth(float width);							//!< Updates window width
				void					setHeight(float height);						//!< Updates window height
				void					drag(const TVec2<float>&d);								//!< Causes the window to move by the specified delta vector. The specified vector is modifable to allow the method to reduce it if dragging is not possible
				void					dragResize(TVec2<float>&d,ClickResult::value_t resolved);	//!< Causes the window to be resized by the specified delta vector.  The specified vector is modifable to allow the method to reduce it if resizing is not possible
				void					mouseUp(float x, float y);						//!< Signals that the mouse has been released
				ClickResult::value_t	resolve(float x_, float y_, float&inner_x, float&inner_y);	//!< Resolves how a click would be handled given the current mouse position @param x_ Mouse x coordinate @param y_ Mouse y coordinate @param inner_x X coordinate of the resulting window relative mouse position @param inner_y Y coordinate of the resulting window relative mouse position @return Resolve result
				void					renderBuffers(Display<OpenGL>&display);							//!< Rerenders the window content into the buffers
				#ifdef DEEP_GUI
					void					setShellDestination(float new_shell_radius);				//!< Changes the window animation target
				#endif
		
				float					minHeight()	const;									//!< Resolves the minimum height of this window in pixels
				float					minWidth()	const;									//!< Resolves the minimum width of this window in pixels
				void					updateLayout();										//!< Updates the layout and component layout if existing
				void					setComponent(const shared_ptr<Component>&component);					//!< Changes the primary component of this window @param discardable Set true if the assigned component may be erased when the window is deleted or another component assigned
				void					apply(Component::eEventResult rs);	//!< Applies the result of a component event to the local state variables
		};
		
		
		enum mode_t
		{
			Planar,
			Cylindrical
		};

		class Operator: public enable_shared_from_this<Operator>	//! GUI Instance
		{
		protected:
				bool									owns_mouse_down;		//!< True if the currently pressed mouse button is handled by the window system
				Display<OpenGL>							*display;
				const Mouse								*mouse;
				InputMap								*input;
				mode_t									mode;
				bool									created,
														stack_changed;
				Buffer<shared_ptr<Window> >				window_stack;
				Buffer<weak_ptr<Window> >				menu_stack;
				OrthographicAspect<>					window_space,
														texture_space;
				Camera<>								projected_space;
				Buffer<Rect<int> >						focus_stack;
				GLuint									layer_texture;


				void									cylindricVertex(float px, float py, float tx, float ty, float r=1)	const;
				void									cylindricVertex(Window*window, float x, float y)	const;
				void									planarVertex(float px, float py, float tx, float ty) const;
				void									planarVertex(Window*window, float x, float y)	const;


				/**
					@brief Projects a point of a window to the screen
					@param window Window to use as reference
					@param x Window relative x coordinate (-1 to 1)
					@param y Window relative y coordinate (-1 to 1)
				*/
				void									project(Window*window, float x, float y, TVec2<float>&p)	const;
				void									unproject(const TVec3<float>&f, TVec2<float>&p) const;	//f required to be normalized

				void									unprojectMouse(TVec2<float>&p)	const;
				void									render(const shared_ptr<Window>&window, float w, float h, bool is_menu);//!< Renders this window given the current display dimensions @param w Display width @param h Display Height @param is_menu True if this window is rendered as a menu, false otherwise
				void									renderBox(const shared_ptr<Window>&window, float w, float h, bool is_menu);
				float									radiusOf(index_t stack_layer)	const;
				void									apply(const Rect<int>&port);

				void									bind(Key::Name key);

														Operator(Display<OpenGL>&display, const Mouse&mouse, InputMap&input, mode_t mode);

		public:
		virtual											~Operator()
														{
														}
		
		static	shared_ptr<Operator>					create(Display<OpenGL>&display, const Mouse&mouse, InputMap&input, mode_t mode=Cylindrical);
				Display<OpenGL>&						getDisplay()	{return *display;}
				void									render();					//!< Animates and renders all windows. Also triggers certain mouse movement related component events

				bool									ownsMouseDown()	const	{return owns_mouse_down;}
				void									updateDisplaySize();	//!< Updates local texture sizes based on display size

				void									insertWindow(const shared_ptr<Window>&window);	//!< Appends a window to the local window stack as new top level window. If the window is already inserted then it will simply be moved to the top position @param window Window to append @param managed Set true to also add the window to the local container, automatically deleting it if no longer necessary
				void									removeWindow(const shared_ptr<Window>&window);				//!< Removes a window from the window stack (does not delete the window)
				bool									windowIsVisible(const shared_ptr<Window>&window)	const;
				shared_ptr<Window>						createWindow(const Rect<float>&region, const String&name,const shared_ptr<Component>&component=shared_ptr<Component>());		//!< Creates a new window @param region Window region in pixels. Allowed region is (0,0) [lower left corner] to (display.width(),display.height()) [upper right corner] @param window name (and title)  @param component Component to put on the new window @return new window
				shared_ptr<Window>						createWindow(const Rect<float>&region, const String&name,Layout*layout,const shared_ptr<Component>&component);//!< Creates a new window @param region Window region in pixels. Allowed region is (0,0) [lower left corner] to (display.width(),display.height()) [upper right corner] @param window name (and title)  @param layout Layout to apply to the new window @param component Component to put on the new window @return new window
				bool									mouseDown();			//!< Signals that the main mouse button has been pressed.
				void									mouseUp();				//!< Signals that the main mouse button has been released
				bool									mouseWheel(short delta);	//!< Signals that the mouse wheel has been used

				void									showMenu(const shared_ptr<Window>&menu_window);
				void									hideMenus();


				void									focus(const Rect<float>&region);	//!< Focuses on an area by applying the current viewport and translation to the specified region and further limiting the viewport. The existing translation will be modified by dx and dy
				void									unfocus();	//!< Reverts the focus process by jumping back to the next upper focus
				void									resetFocus()	{focus_stack.reset();}

		};

		void							loadBump(const String&filename, OpenGL::Texture&target);	//!< Loads a bump texture
		void							loadBump(const String&filename, Image&target);			//!< Loads a bump texture as normals into an image
		void							loadColor(const String&filename, OpenGL::Texture&target);	//!< Loads a color texture
		
		void							setBlurEffect(bool setting);	//!< Updates the blur effect setting. If both blur and refract are disabled then texture copying is disabled altogether
		void							setRefractEffect(bool setting);	//!< Updates the refraction effect setting. If both blur and refract are disabled then texture copying is disabled altogether
		void							setBumpEffect(bool setting);	//!< Disables bump maps. This also disables refraction
		
	}



}



#endif

