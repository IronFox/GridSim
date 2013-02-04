#ifndef gl_gui_componentsH
#define gl_gui_componentsH

#include "gui.h"

namespace Engine
{
	namespace GUI
	{

		/**
			@brief Simple label component
			
			A label simply displays a string and provides no further functionality. The label may be multi-lined
		*/
		class Label:public Component, public IToString
		{
		protected:
			String						caption;	//!< Single line (complete) caption of the label
			Array<String>				lines;			//!< Split (wrapped) caption
			bool						text_changed;	//!< Indicates that the text changed and the text should be re-wrapped before printing it the next time
			TVec4<GLfloat>				text_color;			//!< Label text color (solid white by default)
			float						last_width;
				
			void						setup();
			static	float				charLen(char c);

										Label(const String&type):Component("Label/"+type)	//!< Derivative label constructor
										{
											setup();
										}		
		public:
			bool						wrap_text,			//!< Indicates that the caption should be wrapped at t
										fill_background;	//!< Fills the label background before writing the label caption (false by default)
			TVec3<GLfloat>				background_color;	//!< Label background used if @b fill_background is true (light blueish by default)
			TIcon						icon;				//!< Label icon (empty by default)
				
										Label():Component("Label")	//!< Simple label constructor
										{
											setup();
										}

			virtual	float				clientMinWidth()	const;	
			virtual	float				clientMinHeight()	const;
			virtual	void				onColorPaint();
			const String&				text() const	{return caption;}	//!< Retrieves the current caption
			Label*						setText(const String&text);		//!< Updates label caption
			Label*						setColor(const TVec4<>&color);	//!< Updates label text color
			String						toString()	const	{return caption;}	//!< Simple CSObject toString() override
			virtual	void				updateLayout(const Rect<float>&parent_region);
		};
		
		typedef shared_ptr<Label>		PLabel;

		class SliderLayout
		{
		public:
				TTexture				bar_left,
										bar_center,
										slider;
				
				float					min_width,
										min_height;
		static 	SliderLayout			global;	//!< Global slider layout, applied by default to the appearance of a new slider
		
				void					loadFromFile(const String&filename, float scale=1.0f);
		};
		
		/**
			@brief Scrollbar layout
			
			A scrollbar layout defines the shapes and textures used by a vertical scrollbar. Horizontal scrollbars are supposed to simply rotate the textures/shapes.
			The visual scrollbar is defined by each the center (stretching) part and the bottom end of both the background (back) and the cursor. The top end is supposed to be
			simply the mirrored bottom end of those two.
			Of the two scrollbuttons only the bottom one is defined as one solid texture/shape. Again the top button is expected to be the reflected bottom button.
		*/
		class ScrollBarLayout
		{
		public:
				TTexture				back_center,	//!< Center fragment of the background
										back_bottom,	//!< Bottom fragment of the background
										cursor_center,	//!< Center fragment of the cursor
										cursor_bottom,	//!< Bottom fragment of the cursor
										bottom_button;	//!< Down button
				float					button_indent,	//!< Upper space of the button that is both used by the cursor and the button. Sort of like an overlapping if the shape between button and cursor is not straight
										min_width,		//!< Minimum width of the scrollbar. This is in fact the same as the maximum width since the width of a vertical scrollbar is usually static.
										min_height;		//!< Minimum height of the scrollbar
				
				/**
					@brief Loads the local scrollbar layout from an xml file
					@param filename Path to the xml file
					@param scale Scale to apply to the loaded layout
					@param error_out String to store an error description in (in case of an error) or NULL if the error output should be ignored.
					@return true on success
				*/
				void					loadFromFile(const String&filename, float scale=1.0f);
				
				
				
		static 	ScrollBarLayout		global;	//!< Global scrollbar layout, applied by default to the appearance of a new scrollbar
		};
		
		
		/**
			@brief Data structure used by scrollbar to specify the current scrollbar status
		*/
		struct TScrollData
		{
				float					min,		//!< Smallest scrollbar value
										max,		//!< Greatest scrollbar value
										window,		//!< Window covered by the scrollbar view (effectivly defines the length of the bar)
										current;	//!< Scroll window position ranging 0 to 1
										
										TScrollData():min(0),max(0),window(100),current(0)
										{}
		};

		/**
			@brief Abstract scrollable object
			
			Scrollable defines an abstract object that can be scrolled on up to two directions
		*/
		class Scrollable
		{
		public:
				TScrollData				horizontal,	//!< Horizontal scrolldata
										vertical;	//!< Vertical scrolldata
				
		virtual							~Scrollable()	{};
		virtual	void					onScroll()				{}
		virtual	void					onHorizontalScroll()	{onScroll();}	//!< Triggered if a horizontal scrollbar - that this scrollable component has been attached to - was scrolled
		virtual	void					onVerticalScroll()		{onScroll();}	//!< Triggered if a vertical scrollbar - that this scrollable component has been attached to - was scrolled
		};
		
		
		
		/**
			@brief Scrollbar component
			
			The scrollbar is vertical by default but can be horizontal too
		*/
		class ScrollBar: public Component
		{
		protected:
				float						cursor_hook[2],
											cursor_range;
				bool						up_pressed,
											down_pressed,
											cursor_grabbed;
				TFreeCell					background_top,
											background_center,
											background_bottom,
											cursor_top,
											cursor_center,
											cursor_bottom,
											up_button,
											down_button;
				Rect<float>				cursor_region;
				
				void						setup();
		static	void						paintColor(const TCell&);
		static	void						paintNormal(const TCell&, bool);
		public:
				weak_ptr<Scrollable>		scrollable;
				bool						horizontal,		//!< Indicates that the scrollbar is horizontal rather than vertical
											auto_visibility;	//!< Indicates that the governing structure (ie a scrollbox) should attempt to determine and update the visibility of this scrollbar automatically
				FunctionalEvent				on_scroll;		//!< Event that is fired if the scrollbar changes
				TScrollData					scroll_data;		//!< Current scrollbar state
				ScrollBarLayout*			scroll_layout;		//!< Inner scrollbar layout. Functionality is undetermined if the scrollbar has no inner layout
										
										
				
										ScrollBar():Component("ScrollBar")
										{
											setup();
											anchored.set(false,true,true,true);
											width = minWidth(false);
											height = minHeight(false);
											scroll_data.window = height;
										}
										ScrollBar(bool horizontal_):Component("ScrollBar")
										{
											setup();
											horizontal = horizontal_;
											width = minWidth(false);
											height = minHeight(false);
											if (horizontal)
											{
												anchored.set(true,true,true,false);
												scroll_data.window = width;
											}
											else
											{
												anchored.set(false,true,true,true);
												scroll_data.window = height;
											}
										}
			void						scrollTo(float v);
		virtual	void					onScroll();	//!< Triggered if the scrollbar changes. Invokes the local event container and notifies the linked scrollable by default
		virtual	void					updateLayout(const Rect<float>&parent_region);
		virtual	float					clientMinWidth()	const;
		virtual	float					clientMinHeight()	const;
		virtual	void					onColorPaint();
		virtual	void					onNormalPaint();
		virtual	eEventResult			onMouseDrag(float x, float y);
		virtual	eEventResult			onMouseDown(float x, float y, TExtEventResult&);
		virtual	eEventResult			onMouseUp(float x, float y);
		virtual	eEventResult			onMouseWheel(float x, float y, short delta);
		};
		typedef shared_ptr<ScrollBar>		PScrollBar;
		


		/**
			@brief Horizontal slider component
			
			Strictly horizontal slider component
		*/
		class Slider: public Component
		{
		protected:
			float						cursor_hook[2];
			bool						cursor_grabbed;
			TFreeCell					bar_left,
										bar_center,
										bar_right,
										slider;
			float						slide_from,
										slide_range;
			float						current,
										max;
			Rect<float>					cursor_region;
				
			void						setup();
			void						updateCursorRegion();
				
		public:

			FunctionalEvent				on_slide;		//!< Event that is fired if the slider changes
			SliderLayout*				slider_layout;		//!< Inner slider layout. Functionality is undetermined if the slider has no inner layout
										
										
				
										Slider():Component("Slider")
										{
											setup();
											anchored.set(true,false,true,true);
											width = minWidth(false);
											height = minHeight(false);
										}
			virtual	void				onSlide();	//!< Triggered if the slider changes. Invokes the local event container by default
			virtual	void				updateLayout(const Rect<float>&parent_region);
			virtual	float				clientMinWidth()	const;
			virtual	float				clientMinHeight()	const;
			virtual	void				onColorPaint();
			virtual	void				onNormalPaint();
			virtual	eEventResult		onMouseDrag(float x, float y);
			virtual	eEventResult		onMouseDown(float x, float y, TExtEventResult&);
			virtual	eEventResult		onMouseUp(float x, float y);
			virtual	eEventResult		onMouseWheel(float x, float y, short delta);
		
			inline float				getMax()const {return max;}
			inline float				getCurrent() const {return current;}
			void						setMax(float max_)	{if (max_ != max) {max = max_; signalLayoutChange();}}
			void						setCurrent(float current_)	{if (current_ != current) {current = current_; signalLayoutChange();}}
		};
		typedef shared_ptr<Slider>		PSlider;
	


		/**
			@brief Panel component
			
			Panels store a collection of child GUI
		*/
		class Panel:public Component
		{
		protected:
				Buffer<shared_ptr<Component>,4 >		children;

										Panel(const String&type_name):Component("Panel/"+type_name)	//!< Creates a derivative panel
										{
											layout = global_layout.reference();
											anchored.setAll(true);
										}
		public:
		
		static	Layout					global_layout;	//!< Default panel layout
				
										Panel():Component("Panel")	//!< Creates a plain panel
										{
											layout = global_layout.reference();
											anchored.setAll(true);
										}
		virtual	void					updateLayout(const Rect<float>&parent_region);
		virtual	float					clientMinWidth()	const;
		virtual	float					clientMinHeight()	const;
		virtual	void					onColorPaint();
		virtual	void					onNormalPaint();
		virtual	eEventResult			onMouseHover(float x, float y, TExtEventResult&);
		virtual	eEventResult			onMouseDown(float x, float y, TExtEventResult&);
		virtual	eEventResult			onMouseWheel(float x, float y, short delta);
				bool					getChildSpace(Rect<float>&out_rect)	const;	//!< Retrieves the space currently occupied by the children of this panel @param out_rect Rectangle container to store the child space in @return true if the local panel has at least one child, false otherwise
		
				void					append(const shared_ptr<Component>&component);	//!< Appends  the specified component beneath the last member component of the local panel
				void					appendRight(const shared_ptr<Component>&component);	//!< Appends the specified component to the right of the last member component of the local panel
				/**
					@brief Adds a row in the form "[caption][tab][component]" to the end of the panel
					
					addRow() adds a new label with the specified caption as well as the specified GUI to the bottom end of the local panel's elements
					
					@param caption Caption to assign to the created label object. The label object is located at the left edge underneath the last child component
					@param caption_width Minimal width to allocate for the caption label
					@param component Component object to add. The component is located to the right of the newly created caption, at least @a caption_width to right of the left edge of the label.
					@return @a component
				*/
			template <class Component>
				shared_ptr<Component>	addRow(const String&caption, float caption_width, const shared_ptr<Component>&component)
				{
					if (!component)
						return shared_ptr<Component>();
					shared_ptr<GUI::Component> last = children.isNotEmpty()?children.last():shared_ptr<GUI::Component>();
					float	y_offset = (last?last->offset.top-last->height:0),
							x_offset = 0;

					shared_ptr<GUI::Label> label(new GUI::Label());
					label->setText(caption);
					
					float h = vmax(label->height,component->height);

					
					label->anchored.set(true,false,false,true);
					label->offset.left = x_offset;
					label->offset.top = y_offset-h/2+label->height/2;
					this->add(label);
					x_offset = vmax(caption_width,label->width);
					
					component->anchored.set(true,false,true,true);
					component->offset.left = x_offset;
					component->offset.top = y_offset-h/2+component->height/2;
					this->add(component);
					return component;
				}
				
				
		virtual	bool					add(const shared_ptr<Component>&component);		//!< Adds a component to the panel. The added panel is drawn last 	@return true on success
		virtual	bool					erase(const shared_ptr<Component>&component);	//!< Erases a component from the panel.		@return true on success
		virtual	bool					erase(index_t index);			//!< Erases a component by index from the panel		@return true on success
		virtual	bool					moveUp(const shared_ptr<Component>&component);	//!< Moves a component further up in the order of child GUI			@return true on success
		virtual	bool					moveUp(index_t index);			//!< Moves a component by index further up in the order of child GUI	@return true on success
		virtual	bool					moveDown(const shared_ptr<Component>&component);	//!< Moves a component further down in the order of child GUI		@return true on success
		virtual	bool					moveDown(index_t index);		//!< Moves a component by index further down in the order of child GUI	@return true on success
		virtual	bool					moveTop(const shared_ptr<Component>&component);	//!< Moves a component to the top of the order of child GUI			@return true on success
		virtual	bool					moveTop(index_t index);		//!< Moves a component by index to the top of the order of child GUI	@return true on success
		virtual	bool					moveBottom(const shared_ptr<Component>&component);	//!< Moves a component to the bottom of the order of child GUI	@return true on success
		virtual	bool					moveBottom(index_t index);			//!< Moves a component by index to the bottom of the order of child GUI	@return true on success
		virtual	shared_ptr<const Component>		child(index_t index) const	{return children[index];}
		virtual	shared_ptr<Component>		child(index_t index)	{return children[index];}
		virtual	count_t					countChildren() const {return children.count();}
		virtual	index_t					indexOf(const shared_ptr<Component>&child)const	{return children.indexOf(child)+1;}	//!< Queries the index of a child +1. @return Index+1 or 0 if the child could not be found
		virtual	void					clear()	{children.reset();}
		};
		typedef shared_ptr<Panel>		PPanel;
		
		/**
			@brief Scrollable panel
			
			A scrollbox automatically provides a horizontal and vertical scrollbar. Both can be set invisible individually.
		*/
		class ScrollBox: public Panel, public Scrollable
		{
		protected:
				Rect<float>						effective_client_region;
				Buffer<shared_ptr<Component>,4>	visible_children;
				

				void								createBars()
				{
					horizontal_bar.reset(new ScrollBar(true));
					vertical_bar.reset(new ScrollBar(false));
				}
										ScrollBox(const String&sub_type_name):Panel("ScrollBox/"+sub_type_name)
										{
											createBars();
										}
		public:
				shared_ptr<ScrollBar>	horizontal_bar,	//!< Horizontal scrollbar
										vertical_bar;	//!< Vertical scrollbar
		
										ScrollBox():Panel("ScrollBox")
										{
											createBars();
										}
				shared_ptr<Scrollable>	toScrollable()
										{
											return static_pointer_cast<Scrollable, ScrollBox>(static_pointer_cast<ScrollBox, Component>(shared_from_this()));
										}
		//virtual	void					onScroll();
		virtual	void					updateLayout(const Rect<float>&parent_region);
		virtual	float					clientMinWidth()	const;
		virtual	float					clientMinHeight()	const;
		virtual	void					onColorPaint();
		virtual	void					onNormalPaint();
		virtual	eEventResult			onMouseHover(float x, float y, TExtEventResult&);
		virtual	eEventResult			onMouseDown(float x, float y, TExtEventResult&);
		virtual	eEventResult			onMouseWheel(float x, float y, short delta);
				void					append(const shared_ptr<Component>&component);
		virtual	shared_ptr<const Component>	child(index_t index) const;
		virtual	shared_ptr<Component>			child(index_t index);
		virtual	size_t					countChildren() const;
		virtual	bool					erase(const shared_ptr<Component>&component);
		virtual	bool					erase(index_t index);
		virtual	void					clear()	{children.reset();visible_children.reset();}

			void						scrollTo(float x, float y);
			void						scrollToX(float x);
			void						scrollToY(float y);

		};
		typedef shared_ptr<ScrollBox>		PScrollBox;
		
		/**
			@brief Button component
		*/
		class Button:public Component
		{
		protected:
			bool						down, pressed;
			String						caption;		//!< Button caption
		public:
			static	Layout				global_layout;	//!< Global default button layout
			FunctionalEvent				on_execute;		//!< Event that is fired if the button is pressed

			/**/						Button():Component("Button"),down(false),pressed(false),caption("Button")
										{
											layout = global_layout.reference();
											width = minWidth(false);
											height = minHeight(false);
										}
			/**/						Button(const String&caption_):Component("Button"),down(false),pressed(false),caption(caption_)
										{
											layout = global_layout.reference();
											width = minWidth(false);
											height = minHeight(false);
										}
			virtual	void				onColorPaint()	override;
			virtual	void				onNormalPaint()	override;
			virtual	float				clientMinWidth()	const	override;
			virtual	float				clientMinHeight()	const	override;
			virtual	eEventResult		onMouseDrag(float x, float y)	override;
			virtual	eEventResult		onMouseDown(float x, float y, TExtEventResult&)	override;
			virtual	eEventResult		onMouseUp(float x, float y)	override;
			virtual	eEventResult		onKeyDown(Key::Name key)	override;
			virtual	eEventResult		onKeyUp(Key::Name key)	override;
		
			void						setCaption(const String&caption_)	//!< Updates the caption of the local string and automatically adjusts the local button width to match the minimum required size
										{
											caption = caption_;
											width = minWidth(false);
											shared_ptr<Window>wnd = window();
											if (wnd)
												wnd->apply(RequestingReshape);
										}
			virtual	void				onExecute()
										{
											on_execute();
										}
		};
		typedef shared_ptr<Button>		PButton;
		
		
		/**
			@brief Input component
			
			Alles editing of a string including the most common operations. Also supports copy&paste
		*/
		class Edit:public Component
		{
		protected:
				bool					go_left,	//!< Indicates that during the next onTick() invocation the view should move to the left
										go_right,	//!< Indicates that during the next onTick() invocation the view should move to the right
										view_right_most;	//!< Indicates that the view cannot move any further to the right
				count_t					cursor_ticks;		//!< Tick counter for cursor visibility
				float					cursor_offset,
										cursor_length;
										

				void					setup();
				void					updateView();

										Edit(const String&type):Component("Edit/"+type),text(8)	//!< Creates an edit derivative
										{
											setup();
										}
				inline float			charWidth(char c)
										{
											if (mask_input)
												return textout.getFont().getHeight()*0.75f;
											return textout.getFont().getWidth(c);
										}
										
				
				inline float			textWidth(const char*str, size_t num_chars)
										{
											if (mask_input)
												return textout.getFont().getHeight()*0.75*num_chars;
											return textout.unscaledLength(str,num_chars);
										}
				StringBuffer			text;			//!< Edit buffer
		public:
			union
			{
				bool					readonly;		//!< Indicates that this edit can be read and copied but not modified
				bool					read_only;		//!< Mapping to readonly
			};
			union
			{
				bool					mask_input;		//!< Render dots instead of characters. Prevents copying from this edit (but still allows pasting into it)
				bool					masked_input;	//!< Mapping to mask_input
				bool					masked_view;	//!< Mapping to mask_input
			};
				size_t					cursor,			//!< Current cursor location
										sel_start,		//!< Selection start. This can be before or after the cursor location
										view_begin,		//!< First visible character
										view_end;		//!< One past the last visible character
				FunctionalEvent			on_change,	//!< Event that is fired if the local input is changed
										on_enter;	//!< Event that is fired if the user pressed enter or return on this edit field
				bool					(*acceptChar)(char);	//!< Pointer to a char filter. Only if the function returns true then the character is inserted
				

		static	Layout					global_layout;	//!< Global default edit layout
				
										Edit():Component("Edit"),readonly(false),mask_input(false),text(8),acceptChar(NULL)	//!< Creates a standard edit
										{
											setup();
										}
			void						setText(const char*new_text);
			void						setText(const String&new_text);
			void						setText(const ReferenceExpression<char>&new_text);
			StringBuffer&				getBuffer()			{signalVisualChange(); return text;}
			const StringBuffer&			getBuffer() const	{return text;}
			String						getText()		const{return text.toString();}
			ReferenceExpression<char>	getTextRef()	const {return text.toStringRef();}
			virtual	eEventResult		onFocusGained()	override				{cursor_ticks = 0; return RequestingRepaint;}
			virtual	eEventResult		onFocusLost()	override					{return RequestingRepaint;}
			virtual	float				clientMinWidth()	const	override		{return 50;}
			virtual	float				clientMinHeight()	const	override		{return textout.getFont().getHeight();}
			virtual	void				onColorPaint()	override;
			virtual	eEventResult		onKeyDown(Key::Name key)	override;
			virtual	eEventResult		onKeyUp(Key::Name key)	override;
			virtual	eEventResult		onChar(char c)	override;
			virtual	eEventResult		onMouseDrag(float x, float y)	override;
			virtual	eEventResult		onMouseDown(float x, float y, TExtEventResult&)	override;
			virtual	eEventResult		onMouseHover(float x, float y, TExtEventResult&)	override;
			virtual	eEventResult		onMouseUp(float x, float y)	override;
			virtual	eEventResult		onTick()	override;
			virtual	bool				tabFocusable()	const	override				{return true;}
			virtual	void				updateLayout(const Rect<float>&parent_region)	override;
		};
		typedef shared_ptr<Edit>		PEdit;
		
		typedef Edit	Input;
		
		class Menu;
		
		/**
			@brief Standardized menu entry component
		*/
		class MenuEntry:public Label
		{
		protected:
				weak_ptr<Menu>		parent;		//!< Pointer to the menu that contains this entry. NULL by default, indicating no parent.
				shared_ptr<Window>	menu_window;	//!< Pointer to the window containing a sub menu (if any). NULL if no sub menu has been created, automatically deleted on destruction
				//unsigned				level;
				
				friend class Menu;
				
				void					setup();
				void					correctMenuWindowSize()	const;
				
										MenuEntry(const String&sub_type_name):Label("MenuEntry/"+sub_type_name)	//!< Creates a menu entry derivative
										{
											setup();
										}
		public:
				shared_ptr<IToString>	object;	//!< Link to the object used to dynamically retrieve the local entry caption. NULL by default @b Not automatically deleted.
				FunctionalEvent			on_execute;		//!< Event fired if this menu entry is executed
				bool					open_down;	//!< Indicates that the sub menu of this item (if any) opens down, rather than to the right.
				
										MenuEntry():Label("MenuEntry")	//!< Creates a standard menu entry
										{
											setup();
										}
		virtual									~MenuEntry();
				const shared_ptr<Window>&		menuWindow();	//!< Retrieves (and possibly creates) the window containing the sub menu entries
				shared_ptr<Menu>		menu();			//!< Retrieves (and possibly creates) the sub menu of this entry
				shared_ptr<const Menu>	menu()	const;	//!< @overload
				void							discardMenu();	//!< Discards that local sub menu
		virtual	void							onColorPaint();	
		virtual	eEventResult					onMouseDown(float x, float y, TExtEventResult&);
		virtual	eEventResult					onMouseHover(float x, float y, TExtEventResult&);
		virtual	eEventResult					onFocusGained();
		virtual	eEventResult					onFocusLost();
		virtual	eEventResult					onMouseExit();
		virtual	eEventResult					onKeyDown(Key::Name key);
		virtual	void							onMenuClose(const shared_ptr<MenuEntry>&child);
		virtual	void							updateLayout(const Rect<float>&parent_region);
		};
		typedef shared_ptr<MenuEntry>		PMenuEntry;
		
		
		/**
			@brief Standard menu component containing menu entries
			
			The Menu container enforces that all of its children are of type MenuEntry
		*/
		class Menu:public ScrollBox
		{
		protected:
				weak_ptr<MenuEntry>			parent;	//!< Parent menu entry. NULL by default
				index_t								selected_entry;	//!< Currently selected menu entry
				
				friend class MenuEntry;
				
				ScrollBox::append;
				
				void					setup();
				void					arrangeItems();
				
										Menu(const String&sub_type_name):ScrollBox("Menu/"+sub_type_name)	//!< Creates a menu derivative
										{
											setup();
										}
		public:
		static	Layout					global_layout;	//!< Global standard menu layout
		
										Menu():ScrollBox("Menu")	//!< Creates a simple menu
										{
											setup();
										}
		virtual	shared_ptr<MenuEntry>	add(const String&caption);	//!< Adds a new simple menu entry to this menu @param caption Caption of the menu entry @return Pointer to the newly created menu entry
		virtual	bool					add(const shared_ptr<Component>&component);	//!< Adds a component to this menu. Must of a MenuEntry of derivative class. @param component Menu entry to add. The method fails if the passed pointer is NULL or not of type MenuEntry @return true on success
		virtual	bool					erase(const shared_ptr<Component>&component);
		virtual	bool					erase(index_t index);
		virtual	bool					moveUp(const shared_ptr<Component>&component);
		virtual	bool					moveUp(index_t index);
		virtual	bool					moveDown(const shared_ptr<Component>&component);
		virtual	bool					moveDown(index_t index);
		virtual	bool					moveTop(const shared_ptr<Component>&component);
		virtual	bool					moveTop(index_t index);
		virtual	bool					moveBottom(const shared_ptr<Component>&component);
		virtual	bool					moveBottom(index_t index);
		
		virtual	float					idealHeight()	const;		//!< Retrieves the ideal height of this menu

		virtual	eEventResult			onKeyDown(Key::Name key);
		virtual	eEventResult			onMouseHover(float x, float y, TExtEventResult&);
		};
		typedef shared_ptr<Menu>		PMenu;
		
		/**
			@brief Combobox component
			
			The combobox expands a menu if clicked allowing the user to choose a new element
		*/
		class ComboBox:public MenuEntry
		{
		protected:
				index_t					selected_entry;
				
				void					setup();
		public:
				shared_ptr<IToString>	selected_object;	//!< Currently selected object (if any)
				FunctionalEvent			on_change;		//!< Event that is fired if the selected item changed
				
		static	Layout					global_layout;
				
										ComboBox():MenuEntry("ComboBox")
										{
											setup();
										}
										ComboBox(const String&sub_type_name):MenuEntry("ComboBox/"+sub_type_name)
										{
											setup();
										}
		virtual	float					minWidth(bool include_offsets)	const;
		virtual	void					updateLayout(const Rect<float>&parent_space);
				void					select(index_t index);
				index_t					selected()	const	{return selected_entry;}
		
		virtual	void					onMenuClose(const shared_ptr<MenuEntry>&child);			
		virtual	eEventResult			onMouseHover(float x, float y, TExtEventResult&ext);
		
		};
		typedef shared_ptr<ComboBox>		PComboBox;
		
		/**
			@brief Checkbox component
		*/
		class CheckBox:public Component
		{
		protected:
			bool						down, pressed;
			bool						checked;	//!< True if the checkbox is currently checked
			String						caption;	//!< Checkbox caption
		public:
			struct TStyle
			{
				OpenGL::Texture			box_color,
										box_normal,
										check_mark,
										highlight_mark;
			};

		
		
				
			TStyle						*style;			//!< Inner checkbox style
			FunctionalEvent				on_change;
				
			static	TStyle				global_style;	//!< Global default checkbox style
				
										CheckBox():Component("Checkbox"),down(false),pressed(false),checked(false),caption("Checkbox"),style(&global_style)
										{
											width = minWidth(false);
											height = minHeight(false);
										}
			virtual	void				onColorPaint();
			virtual	void				onNormalPaint();
			virtual	float				minWidth(bool include_offsets)	const;
			virtual	float				minHeight(bool include_offsets)	const;
			virtual	eEventResult		onMouseDrag(float x, float y);
			virtual	eEventResult		onMouseDown(float x, float y, TExtEventResult&);
			virtual	eEventResult		onMouseUp(float x, float y);

			inline bool					isChecked()	const	{return checked;}
			void						setChecked(bool b)	{if (checked == b) return; checked = b; signalVisualChange();}

			inline const String&		getCaption()	const {return caption;}
			CheckBox*					setCaption(const String&caption_)
										{
											caption = caption_;
											width = minWidth(false);
											signalLayoutChange();
											return this;
										}
			inline float				boxSize()	const	{return 16.f;}
		};
		typedef shared_ptr<CheckBox>		PCheckBox;
		
		
		void			loadTheme(const String&theme_file, float scale=1.0f);
		
		void			showMessage(Operator&op, const String&title, const String&message);
		void			showMessage(Operator&op, const String&message);
		inline	void	showMessage(const shared_ptr<Operator>&op, const String&title, const String&message)	{if (!op)return; showMessage(*op,title,message);}
		inline	void	showMessage(const shared_ptr<Operator>&op, const String&message)						{if (!op)return; showMessage(*op,message);}
		bool			showingMessage();
		void			hideMessage();
		
		void			showChoice(Operator&op, const String&title, const String&query, const Array<String>&choices, const function<void(index_t)>&onSelect);
		inline void		showChoice(Operator&op, const String&query, const Array<String>&choices, const function<void(index_t)>&onSelect)	{showChoice(op,"Inquiry",query,choices,onSelect);}
		inline void		showChoice(const shared_ptr<Operator>&op, const String&title, const String&query, const Array<String>&choices, const function<void(index_t)>&onSelect)	{showChoice(*op,title,query,choices,onSelect);}
		inline void		showChoice(const shared_ptr<Operator>&op, const String&query, const Array<String>&choices, const function<void(index_t)>&onSelect)	{showChoice(*op,"Inquiry",query,choices,onSelect);}
		
	}
}


#endif
