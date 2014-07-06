#include "../../global_root.h"
#include "multi_window.h"

namespace Engine
{
	#if SYSTEM==UNIX
		void	emptyHook(Window*,XEvent&event)	{}
	#elif SYSTEM==WINDOWS
		LRESULT emptyHook(Window*,HWND window, UINT Msg, WPARAM wParam,LPARAM lParam)
		{
			return DefWindowProcW(window, Msg, wParam, lParam);
		}
	#endif

	Application		application;

	#ifndef _DEBUG
		#define USE_HOOKS
	#endif

	void				MenuEntry::execute(Window*window)
	{
		if (entry_callback)
			(window->*entry_callback)(this);
		elif (callback)
			(window->*callback)();
		elif (simple_entry_callback)
			simple_entry_callback(this);
		elif (simple_callback)
			simple_callback();
	}
	
	
	
	
	
	//static void	emptyFunc(WindowLink*)
	//{}


//static const char*  eventToString(int event);

	void	Application::SetFontName(const String&name)
	{
		x_menu_font = name;
	}

	void 	Application::Execute()
	{
		Execute(NULL);
	}

	void	Application::Execute(bool(*callback)())
	{
		fatal_exception_caught = false;
		shutting_down = false;
		bool doloop = true;
		while (doloop)
		{
	        Engine::timing.update();

			foreach (windows,window)
				if ((*window)->is_new)
				{
					(*window)->is_new = false;
					(*window)->update();
				}



	        #if SYSTEM==WINDOWS
	            MSG msg;
				if (!realtime)
				{
		            if (!GetMessage(&msg, NULL,0,0))
		                FATAL__("event-handling error");
		            TranslateMessage(&msg);
		            DispatchMessage(&msg);
				}
	            while (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
				{
		            if (!GetMessage(&msg, NULL,0,0))
		                FATAL__("event-handling error");
		            TranslateMessage(&msg);
		            DispatchMessage(&msg);
				}
				if (fatal_exception_caught)
					shutting_down = true;
	        #elif SYSTEM==UNIX
				if (!realtime)
	            {
					XEvent event;
	                if (XNextEvent(display,&event))
	                    FATAL__("event-handling error");
	                wExecuteEvent(event);
	            }
	            while (XPending(display))
	            {
					XEvent event;
	                if (XNextEvent(display,&event))
	                    FATAL__("event-handling error");
	                wExecuteEvent(event);
	            }
	        #endif


			doloop = (!callback || callback()) && !shutting_down && windows.count();

			if (doloop)
				foreach (windows,window)
					if ((*window)->requires_update)
					{
						(*window)->forceUpdate();
					}
		}
		if (application.fatal_exception_caught)
		{
			application.fatal_exception_caught = false;
			throw std::exception(application.fatal_exception.c_str(),1);
		}
	}

	void Application::InterruptCheckEvents()
	{
		#if SYSTEM==WINDOWS
			MSG msg;
			while (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
			{
				if (!GetMessage(&msg, NULL,0,0))
					FATAL__("event-handling error");
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		#elif SYSTEM==UNIX
			while (XPending(application.display))
			{
				XEvent event;
				if (XNextEvent(application.display,&event))
					FATAL__("event-handling error");
				wExecuteEvent(event);
			}
		#endif

		foreach (windows,window)
			if ((*window)->requires_update)
				(*window)->forceUpdate();
	}

	void Application::Terminate()
	{
		shutting_down = true;
	}


	void Application::EnterRealtimeMode()
	{
		realtime = true;
	}

	void Application::EnterLazyMode()
	{
		realtime = false;
	}


	Window*	Application::FindWindow(HWND handle)
	{
		return windows.lookup(handle);
	}

	Window*	Application::GetFocusedWindow()
	{
		#if SYSTEM==WINDOWS
			return windows.lookup(GetFocus());
		#elif SYSTEM==UNIX
			Window focus;
			int revert_to;
			XGetInputFocus(display, &focus, &revert_to);
			return windows.lookup(focus);
		#endif
		return NULL;
	}

	MenuEntry::MenuEntry():parent(NULL),enabled(true),checked(false),radio(false),item_id(application.menu_id++),open(false),mouse_over(false),caption_offset_x(0),caption_offset_y(0),callback(NULL),entry_callback(NULL),simple_entry_callback(NULL),simple_callback(NULL)
	{
		callback = NULL;
	    #if SYSTEM==WINDOWS
	        menu_handle = NULL;
	    #endif
		application.menu_items.insert(this);
	}

	MenuEntry::MenuEntry(const String&caption_):parent(NULL),enabled(true),checked(false),radio(false),caption(caption_),item_id(application.menu_id++),open(false),mouse_over(false),caption_offset_x(0),caption_offset_y(0),callback(NULL),entry_callback(NULL),simple_entry_callback(NULL),simple_callback(NULL)
	{
		callback = NULL;
	    #if SYSTEM==WINDOWS
	        menu_handle = NULL;
	    #endif
		application.menu_items.insert(this);
	}
	


	void	MenuEntry::setCallback(pEntryCallback callback_)
	{
		callback = NULL;
		entry_callback = callback_;
		simple_callback = NULL;
		simple_entry_callback = NULL;
	}
		

	void	MenuEntry::setCallback(pCallback callback_)
	{
		callback = callback_;
		entry_callback = NULL;
		simple_callback = NULL;
		simple_entry_callback = NULL;
	}
		

	void	MenuEntry::setCallback(pSimpleCallback callback_)
	{
		callback = NULL;
		entry_callback = NULL;
		simple_callback = callback_;
		simple_entry_callback = NULL;
	}
	void	MenuEntry::setCallback(pSimpleEntryCallback callback_)
	{
		callback = NULL;
		entry_callback = NULL;
		simple_callback = NULL;
		simple_entry_callback = callback_;
	}
		


	MenuEntry::~MenuEntry()
	{
		#if SYSTEM==WINDOWS
			if (menu_handle)
				DestroyMenu(menu_handle);
		#endif
		application.menu_items.drop(this);
	}

	void MenuEntry::updateInfo()
	{
		#if SYSTEM ==WINDOWS
			if (!parent)
				return;

			MENUITEMINFOA info;
			info.cbSize = sizeof(info);
			info.fMask = MIIM_SUBMENU|MIIM_STATE|MIIM_STRING|MIIM_FTYPE; // MIIM_TYPE|MIIM_SUBMENU|MIIM_STRING|MIIM_STATE|MIIM_FTYPE; //|MIIM_DATA;
			info.fType = caption=="-"?MFT_SEPARATOR:MFT_STRING;
			if (radio)
				info.fType |= MFT_RADIOCHECK;
			info.fState = enabled?MFS_ENABLED:MFS_DISABLED;
			if (checked)
				info.fState |= MFS_CHECKED;
			info.wID = item_id;
			info.hSubMenu = menu_handle;
			info.hbmpChecked = NULL;
			info.hbmpUnchecked = NULL;
			info.dwItemData = 0;
			info.dwTypeData = caption.mutablePointer();	//ok, this is PLAIN LAME
			info.cch = UINT(caption.length()+1);
			info.hbmpItem = NULL;

			if (!SetMenuItemInfoA(parent->menu_handle,item_id,FALSE,&info))
			{
				ErrMessage("set menu info of "+caption+" failed :S");
	            LPVOID lpMsgBuf;
	            FormatMessage(
	                FORMAT_MESSAGE_ALLOCATE_BUFFER |
	                FORMAT_MESSAGE_FROM_SYSTEM |
	                FORMAT_MESSAGE_IGNORE_INSERTS,
	                NULL,
	                GetLastError(),
	                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	                (LPTSTR) &lpMsgBuf,
	                0,
	                NULL
	                );
	            //unsigned len = strlen((char*)lpMsgBuf);
				ErrMessage((const char*)lpMsgBuf);

	            LocalFree( lpMsgBuf );

			}
		#endif
	}


	void						MenuEntry::attach(MenuEntry*sub)
	{
		if (!this)
			return;
		sub->parent = this;
		children.append(sub);
		#if SYSTEM==WINDOWS
			if (!menu_handle)
			{
//			ShowMessage("menu handle did not exist - creating");
				menu_handle = CreateMenu();
				if (!menu_handle)
					ErrMessage("menu handle missing :S");
				updateInfo();
			}
			AppendMenuA(menu_handle,MF_STRING,sub->item_id,sub->caption.c_str());
			sub->updateInfo();
		#endif
	}

	bool						MenuEntry::isAttached(MenuEntry*sub)
	{
		return this && children(sub)>0;
	}

	bool						MenuEntry::detach(MenuEntry*entry)
	{
		if (!this)
			return false;
		if (children.drop(entry))
		{
			#if SYSTEM==WINDOWS
				DeleteMenu(menu_handle,entry->item_id,MF_BYCOMMAND);
			#endif
			DISCARD(entry);
			return true;
		}
		return false;
	}

	MenuEntry*					MenuEntry::setRadio(bool is_radio)
	{
		if (!this)
			return this;
		radio = is_radio;
		updateInfo();
		return this;
	}

	MenuEntry*					MenuEntry::unsetRadio()
	{
		if (!this)
			return this;
		radio = false;
		updateInfo();
		return this;
	}

	bool						MenuEntry::isEnabled()	const
	{
		return this && enabled;
	}

	MenuEntry*					MenuEntry::disable()
	{
		if (!this)
			return this;
		enabled = false;
		#if SYSTEM==WINDOWS
			if (parent)
				EnableMenuItem(parent->menu_handle,item_id,MF_BYCOMMAND|MF_GRAYED);
		#endif
		return this;
	}

	MenuEntry*					MenuEntry::enable()
	{
		if (!this)
			return this;
		enabled = true;
		#if SYSTEM==WINDOWS
			if (parent)
				EnableMenuItem(parent->menu_handle,item_id,MF_BYCOMMAND|MF_ENABLED);
		#endif
		return this;
	}
	
	MenuEntry*					MenuEntry::setEnabled(bool enabled)
	{
		return enabled?enable():disable();
	}
	





	const String&				MenuEntry::getCaption()	const
	{
		return caption;

	}

	MenuEntry*						MenuEntry::setCaption(const String&new_name)
	{
		if (!this)
			return this;
		caption = new_name;
		#if SYSTEM==WINDOWS
			if (parent)
				ModifyMenuA(parent->menu_handle,item_id,MF_BYCOMMAND|MF_STRING,item_id,caption.c_str());
		#endif
		return this;
	}
	
	MenuEntry*						MenuEntry::check()
	{
		if (!this)
			return this;
		checked = true;
		updateInfo();
		return this;
	}
	
	MenuEntry*						MenuEntry::uncheck()
	{
		if (!this)
			return this;
		checked = false;
		updateInfo();
		return this;
	}
	
	MenuEntry*						MenuEntry::setChecked(bool checked_)
	{
		if (!this)
			return this;
		checked = checked_;
		updateInfo();
		return this;
	}



	MenuEntry*					MenuEntry::find(char*buffer,FindType type)
	{
		//ShowMessage("finding "+String(buffer));
		MenuEntry*result;
		char*c = buffer;
		while ((*c) && (*c) != '/')
			c++;
		bool recursive = (*c)!=0;
		(*c++) = 0;
		//ShowMessage("section is "+String(buffer));
		for (unsigned i = 0; i < children; i++)
		{
			MenuEntry*entry = children[i];
			if (!strcmp(entry->caption.c_str(),buffer))
			{
				//ShowMessage("entry '"+String(buffer)+"' found");
				if (recursive)
				{
					//ShowMessage("is recursive. passing on '"+String(c)+"'");
					if(result = entry->find(c,type))
						return result;
					if (type == Closest)
						return entry;
					return NULL;
				}
				else
					if (strcmp(buffer,"-") || type != Create)
						return entry;
			}
		}

		//ShowMessage("entry '"+String(buffer)+"' not found");

		if (type == Closest)
			return this;
		if (type == Exact)
			return NULL;
		//ShowMessage("creating child named '"+String(buffer)+"' in "+caption);
		MenuEntry*sub = SHIELDED(new MenuEntry(buffer));
		attach(sub);
		if (recursive)
			return sub->find(c,type);
		return sub;
	}


	Menu::Menu():last_focus(NULL),down_focus(NULL)
	{
		#if SYSTEM==WINDOWS
			menu_handle = CreateMenu();
			if (!menu_handle)
				ErrMessage("bad menu handle");
		#endif
		MenuEntry::open = true;

	}


	bool MenuEntry::in(int x, int y, const RECT&r)
	{
		return x >= r.left && x <= r.right && y >= r.bottom && y <= r.top;
	}

	bool MenuEntry::hasCallback()	const
	{
		return callback || entry_callback || simple_callback || simple_entry_callback;
	
	}

	void MenuEntry::close()
	{
		open = false;
		for (unsigned i = 0; i < children; i++)
			children[i]->close();
	}

	MenuEntry* MenuEntry::mouseMove(int x, int y)
	{
		if (!enabled || caption=='-' || !in(x,y,full_region))
		{
			return NULL;
		}
		mouse_over = in(x,y,core_region);
		if (mouse_over)
			return this;
		if (open)
			for (index_t i = children-1; i < children; i--)
				if (MenuEntry*rs = children[i]->mouseMove(x,y))
					return rs;
		return NULL;
	}

	bool	Menu::mouseMove(int x, int y, bool&redraw)
	{
		if (last_focus)
			last_focus->mouse_over = false;
		MenuEntry*entry = MenuEntry::mouseMove(x,y);
		redraw = entry != last_focus;
		last_focus = entry;
		return last_focus != NULL;
	}

	bool Menu::close()
	{
		bool was_open = false;
		for (unsigned i = 0; i < children; i++)
		{
			was_open |= children[i]->open;
			children[i]->close();
		}
		return was_open;
	}


	bool Menu::mouseDown(bool&redraw)
	{
		down_focus = last_focus;
		bool do_open = last_focus && !last_focus->open && (last_focus->children || last_focus->hasCallback()),
			 was_open = false;
		for (unsigned i = 0; i < children; i++)
		{
			was_open |= children[i]->open;
			children[i]->close();
		}
		if (do_open)
		{
			MenuEntry*entry = last_focus;
			while (entry != this)
			{
				entry->open = entry->children!=0;
				entry = entry->parent;
			}
		}
		redraw = last_focus != NULL || was_open;
		return last_focus != NULL;
	}


	bool	Menu::mouseUp(Window*window)
	{
		if (last_focus && last_focus == down_focus && last_focus->hasCallback())
		{
			String path = last_focus->caption;
			MenuEntry*entry = last_focus->parent;
			while (entry != this)
			{
				path = entry->caption+"/"+path;
				entry = entry->parent;
			}

			last_focus->execute(window);
			for (unsigned i = 0; i < children; i++)
				children[i]->close();
		}
		return last_focus != NULL;
	}

	void MenuEntry::render(Window*window)
	{
		int x_menu_font_size = application.x_menu_font_size;
		if (!children)
			open = false;
		if (caption.length() && (mouse_over || open))
		{
			int left = core_region.left-2,
				right = core_region.right+2,
				bottom = core_region.bottom,
				top = core_region.top;
			glBegin(GL_POLYGON);
				if (open)
					glGrey(0.65);
				else
					glGrey(0.95);
				glVertex2f(left+2.5,top-0.5);
				glVertex2f(left+0.5,top-2.5);	//i have no idea why i need to use 0.5 here. must be something of projection inaccuracies in combination with fsaa
				if (open)
					glGrey(0.7);
				else
					glGrey(0.85);
				glVertex2f(left+0.5,bottom+2.5);
				glVertex2f(left+2.5,bottom+0.5);
				glGrey(0.75);
				glVertex2f(right-2.5,bottom+0.5);
				glVertex2f(right-0.5,bottom+2.5);
				if (open)
					glGrey(0.7);
				else
					glGrey(0.85);
				glVertex2f(right-0.5,top-2.5);
				glVertex2f(right-2.5,top-0.5);
			glEnd();
			glBegin(GL_LINE_LOOP);
				glGrey(0.5);
				glVertex2f(left+0.5,top-2.5);	//i have no idea why i need to use 0.5 here. must be something of projection inaccuracies in combination with fsaa
				glVertex2f(left+0.5,bottom+2.5);
				glVertex2f(left+2.5,bottom+0.5);
				glVertex2f(right-2.5,bottom+0.5);
				glVertex2f(right-0.5,bottom+2.5);
				glVertex2f(right-0.5,top-2.5);
				glVertex2f(right-2.5,top-0.5);
				glVertex2f(left+2.5,top-0.5);
			glEnd();
		}
		if (caption != '-')
		{
			window->textout.locate(core_region.left+caption_offset_x, core_region.bottom+caption_offset_y+x_menu_font_size/8);
			if (enabled)
				window->textout.color(0,0,0);
			else
				window->textout.color(0.4,0.4,0.4);
			window->textout.print(caption);

			if (children && child_region.top == core_region.top)	//opening to the right
			{
				int left = core_region.right-x_menu_font_size*2/3,
					right = core_region.right-x_menu_font_size/2,
					middle = core_region.bottom+caption_offset_y+x_menu_font_size/2,
					top = middle + x_menu_font_size*1/5,
					bottom = middle - x_menu_font_size*1/5;

				glBegin(GL_QUADS);	//outer arrow borders (anti-aliasing)
					glBlack();
					glVertex2i(left,top);
					glVertex2i(right,middle);
					glBlack(0);
					glVertex2i(right+5,middle);
					glVertex2i(left,top+3);

					glVertex2i(left,bottom-3);
					glVertex2i(right+5,middle);
					glBlack();
					glVertex2i(right,middle);
					glVertex2i(left,bottom);
				glEnd();
				glBegin(GL_TRIANGLES);	//inner arrow

					glVertex2i(right,middle);
					glVertex2i(left,top);
					glVertex2i(left,bottom);
				glEnd();
			}
			if (checked)
			{
				if (radio)
				{
					glEnable(GL_POINT_SMOOTH);
					glPointSize(x_menu_font_size*0.65);
					glBlack();
					glBegin(GL_POINTS);
						glVertex2i(core_region.left+x_menu_font_size/2,core_region.bottom+caption_offset_y+x_menu_font_size/2);
					glEnd();
				}
				else
				{
					int left = core_region.left+x_menu_font_size/6,
						right = core_region.left+x_menu_font_size*5/6,
						middle = core_region.bottom+caption_offset_y+x_menu_font_size/3,
						top = middle + x_menu_font_size*2/5,
						left_top = middle + x_menu_font_size/3,
						bottom = middle - x_menu_font_size*2/3,
						corner = left+(right-left)/3,
						corner_width = x_menu_font_size/9,
						outer_width = x_menu_font_size/8;

					glBegin(GL_QUADS);
						glBlack();
						glVertex2i(left,left_top);
						glVertex2i(corner,middle);
						glBlack(0);
						glVertex2i(corner,middle+3);
						glVertex2i(left,left_top+3);

						glVertex2i(right,top+3);
						glVertex2i(corner,middle+3);
						glBlack();
						glVertex2i(corner,middle);
						glVertex2i(right,top);

						glVertex2i(right,top);
						glVertex2i(corner,middle);
						glVertex2i(corner,middle-corner_width);
						glVertex2i(right,top-outer_width);

						glVertex2i(corner,middle);
						glVertex2i(left,left_top);
						glVertex2i(left,left_top-outer_width);
						glVertex2i(corner,middle-corner_width);

						glVertex2i(right,top-corner_width);
						glVertex2i(corner,middle-corner_width);
						glBlack(0);
						glVertex2i(corner,middle-corner_width-3);
						glVertex2i(right,top-corner_width-3);

						glVertex2i(left,left_top-corner_width-3);
						glVertex2i(corner,middle-corner_width-3);
						glBlack();
						glVertex2i(corner,middle-corner_width);
						glVertex2i(left,left_top-corner_width);


					glEnd();
				}
			}
		}
		else
		{
			glBegin(GL_LINES);
				float y = core_region.bottom+caption_offset_y+x_menu_font_size/2+0.5;
				glGrey(0.6);
				glVertex2f(core_region.left+5, y);
				glVertex2f(core_region.right-5, y);
				glWhite();
				glVertex2f(core_region.left+5, y-1);
				glVertex2f(core_region.right-5, y-1);
			glEnd();
		}
		if (open)
		{
			if (caption.length())
			{
				int left = child_region.left-4,
					right = child_region.right+4,
					bottom = child_region.bottom-2,
					top = child_region.top-2,
					shadow_left = left+10,
					shadow_left_fade = shadow_left-6,
					shadow_right = right+4,
					shadow_top = top-10,
					shadow_top_fade = shadow_top+6,
					shadow_bottom = bottom-4;

				glBegin(GL_QUADS);
					glGrey(0.95);
					glVertex2i(left,bottom);
					glVertex2i(right,bottom);
					glVertex2i(right,top);
					glVertex2i(left,top);

					glGrey(0.1,0.8);
					glVertex2i(right,bottom);
					glVertex2i(shadow_left,bottom);
					glGrey(0.1,0);
					glVertex2i(shadow_left,shadow_bottom);
					glVertex2i(shadow_right,shadow_bottom);

					glVertex2i(shadow_right,shadow_bottom);
					glVertex2i(shadow_right,shadow_top);
					glGrey(0.1,0.8);
					glVertex2i(right,shadow_top);
					glVertex2i(right,bottom);

					glVertex2i(right,shadow_top);
					glGrey(0.1,0);
					glVertex2i(shadow_right,shadow_top);
					glVertex2i(shadow_right,shadow_top_fade);
					glVertex2i(right,shadow_top_fade);

					glVertex2i(shadow_left_fade,bottom);
					glVertex2i(shadow_left_fade,shadow_bottom);
					glVertex2i(shadow_left,shadow_bottom);
					glGrey(0.1,0.8);
					glVertex2i(shadow_left,bottom);
				glEnd();
				//XDrawLine(application.display,window->window,left,bottom,right,bottom);
				glBegin(GL_LINE_LOOP);
					glGrey(0.5);
					glVertex2f(left+0.5,bottom+0.5);	//i have no idea why i need to use 0.5 here. must be something of projection inaccuracies in combination with fsaa
					glVertex2f(right-0.5,bottom+0.5);
					glVertex2f(right-0.5,top-0.5);
					glVertex2f(left+0.5,top-0.5);
				glEnd();
			}
			for (index_t i = children-1; i < children; i--)
				children[i]->render(window);
		}
	}

	void	MenuEntry::include(const RECT&r, RECT&in)
	{
		if (r.left < in.left)
			in.left = r.left;
		if (r.right > in.right)
			in.right = r.right;
		if (r.top > in.top)
			in.top = r.top;
		if (r.bottom < in.bottom)
			in.bottom = r.bottom;
	}

	void 	MenuEntry::updateRegion(Window*window, bool spawn_right)
	{
		int x_menu_font_size = application.x_menu_font_size;
		/*child_region = */full_region = core_region;
		if (spawn_right)
		{
			//child_region.left = core_region.right+5;	//specified by parent call
			child_region.top = core_region.top;
		}
		else
		{
			child_region.top = core_region.bottom;
			child_region.left = core_region.left;
		}
		child_region.right = child_region.left;
		child_region.bottom = LONG(child_region.top - x_menu_font_size*children*3/2-x_menu_font_size*1/3);
		full_region.top = core_region.top;
		full_region.bottom = child_region.bottom;

		if (open)
		{
			int right_border = core_region.right;
			for (unsigned i = 0; i < children; i++)
			{
				MenuEntry*entry = children[i];
				int right = child_region.left + (int)window->textout.unscaledLength(entry->caption) + 2*x_menu_font_size;
				if (right > right_border)
					right_border = right;
			}
			for (unsigned i = 0; i < children; i++)
			{
				MenuEntry*entry = children[i];
				entry->core_region.left = child_region.left;
				entry->core_region.top = child_region.top-i*x_menu_font_size*3/2-x_menu_font_size*1/3;
				entry->child_region.left = right_border;
				entry->caption_offset_y = x_menu_font_size*1/3-1;
				entry->core_region.bottom = entry->core_region.top-x_menu_font_size*3/2+1;
				entry->core_region.right = entry->core_region.left + (unsigned)window->textout.unscaledLength(entry->caption);
				entry->core_region.right += x_menu_font_size;	//check or radio
				entry->caption_offset_x = x_menu_font_size;
				//if (entry->children)
					entry->core_region.right += x_menu_font_size;	//arrow

				entry->updateRegion(window,true);
				include(entry->core_region,child_region);
				include(entry->full_region,full_region);
			}
			for (unsigned i = 0; i < children; i++)
			{
				children[i]->core_region.right = child_region.right;	//strech to right border
				include(children[i]->core_region,children[i]->full_region);
			}
		}
		include(child_region,full_region);
	}

	void	Menu::update(int x, int y, Window*window)
	{
		int x_menu_font_size = application.x_menu_font_size;
		full_region.left = x;
		full_region.top = y+x_menu_font_size;
		full_region.bottom = y-x_menu_font_size/3;

		core_region.left = 0;
		core_region.right = 0;
		core_region.top = 0;
		core_region.bottom = 0;


	    unsigned at = x;
	    children.reset();
	    while (MenuEntry*entry = children.each())
	    {
			entry->core_region.left = at;
			entry->core_region.top = full_region.top;
			entry->core_region.bottom = y-x_menu_font_size/3;
			entry->caption_offset_y = x_menu_font_size/6;
	        at += (unsigned)window->textout.unscaledLength(entry->caption)+10;
	        entry->core_region.right = at-10;
			entry->updateRegion(window,false);
			include(entry->full_region,full_region);
	    }
	    if (full_region.right < (int)at)
	    	full_region.right = (int)at;
	    child_region = full_region;
	}



	void			Application::RegisterEventHook(EventHook hook)
	{
		if (hook)
			event_hook = hook;
		else
			event_hook = emptyHook;
	}


	Application::Application():event_hook(emptyHook)
	{
		built = false;
		

		hInstance=NULL;
		fatal_exception_caught=false;
		shutting_down = false;
		realtime=false;
		active_window=NULL;
		creating_window=NULL;
		active_screen=0;
		menu_id=0;
		
		#if SYSTEM==UNIX
			x_menu_height=26;
		#endif
		x_menu_font_size=12;
		x_menu_font = "verdana";
		
		

		#if SYSTEM==UNIX
			memset(function_key_mask,false,sizeof(function_key_mask));
			function_key_mask[Key::Left] = true;
			function_key_mask[Key::Right] = true;
			function_key_mask[Key::Up] = true;
			function_key_mask[Key::Down] = true;
			function_key_mask[Key::PageUp] = true;
			function_key_mask[Key::PageDown] = true;
			function_key_mask[Key::End] = true;
			function_key_mask[Key::Home] = true;
			function_key_mask[Key::Insert] = true;
			function_key_mask[Key::Delete] = true;
			function_key_mask[Key::Enter] = true;
			function_key_mask[Key::Return] = true;
			function_key_mask[Key::Backspace] = true;
			function_key_mask[Key::Shift] = true;
			function_key_mask[Key::NumLock] = true;
			function_key_mask[Key::Tab] = true;
			function_key_mask[Key::Pause] = true;
			function_key_mask[Key::Escape] = true;
			function_key_mask[Key::Down] = true;

			function_key_mask[Key::F1] = true;
			function_key_mask[Key::F2] = true;
			function_key_mask[Key::F3] = true;
			function_key_mask[Key::F4] = true;
			function_key_mask[Key::F5] = true;
			function_key_mask[Key::F6] = true;
			function_key_mask[Key::F7] = true;
			function_key_mask[Key::F8] = true;
			function_key_mask[Key::F9] = true;
			function_key_mask[Key::F10] = true;
			function_key_mask[Key::F11] = true;
			function_key_mask[Key::F12] = true;




		#endif
	}

	Application::~Application()
	{
		Terminate();
		Destroy();
	}


	bool    Application::Create()
	{
	    error = NoError;
	    if (built)
	        return true;
	    #if SYSTEM==WINDOWS
			hInstance = getInstance();
	        WNDCLASSW wc;
	        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	        wc.lpfnWndProc = (WNDPROC) wExecuteEvent;
	        wc.cbClsExtra = 0;
	        wc.cbWndExtra = 0;
	        wc.hInstance = hInstance;
	        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);;
	        wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	        wc.hbrBackground = NULL;
	        wc.lpszMenuName = NULL;
	        wc.lpszClassName = WINDOW_CONTEXT_CLASS_NAME;
	        if (!RegisterClassW(&wc))
	        {
	            error = ClassRegistrationFailed;
	            return false;
	        }
	    #elif SYSTEM==UNIX
	        display = getDisplay();
	        if (!display)
	        {
	            error = XConnectionFailed;
	            return false;
	        }

	    #endif

	    built = true;
	    return true;
	}

	bool    Application::Create(const String&icon_name)
	{
	    error = NoError;
	    if (built)
	        return true;
	    #if SYSTEM==WINDOWS
			hInstance = getInstance();
	        WNDCLASSW wc;
	        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	        wc.lpfnWndProc = (WNDPROC) wExecuteEvent;
	        wc.cbClsExtra = 0;
	        wc.cbWndExtra = 0;
	        wc.hInstance = hInstance;
			if (icon_name.length())
			{
				wc.hIcon = (HICON)LoadImageA(hInstance,icon_name.c_str(),IMAGE_ICON,0,0,LR_LOADFROMFILE);
			}
			else
				wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	        wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	        wc.hbrBackground = NULL;
	        wc.lpszMenuName = NULL;
	        wc.lpszClassName = WINDOW_CONTEXT_CLASS_NAME;
	        if (!RegisterClassW(&wc))
	        {
	            error = ClassRegistrationFailed;
	            return false;
	        }
	    #elif SYSTEM==UNIX
	        display = getDisplay();
	        if (!display)
	        {
	            error = XConnectionFailed;
	            return false;
	        }

	    #endif

	    built = true;
	    return true;
	}


	void Application::Destroy()
	{
	    if (!built)
	        return;
	    while (windows)
	        windows.last()->destroy();
	    #if SYSTEM==WINDOWS
	        UnregisterClassW(WINDOW_CONTEXT_CLASS_NAME,hInstance);
	    #elif SYSTEM==UNIX

	        glXMakeCurrent(display,None,NULL);


	        closeDisplay();
	    #endif
	    built = false;
	}

#if SYSTEM==UNIX
	bool Application::isFunctionKey(unsigned key)
	{
		if (key >= 0x100)
			return false;
		return function_key_mask[key];
	}
#endif

	String					Window::getTitle()				const
	{
		char buffer[0x500];
		buffer[0] = 0;
		GetWindowText(getWindow(),buffer,sizeof(buffer)-1);
		return buffer;
	}
	void					Window::setTitle(const String&title)
	{
		SetWindowText(getWindow(),title.c_str());
	}


	const Image*	Window::getDesktop()
	{
		if (!created)
			return NULL;
		#if SYSTEM==UNIX
			Window root,w;
			int i;
			unsigned width,height,u;
			XGetGeometry(application.display, window, &root, &i, &i, &u,  &u,&u,&u);
			XGetGeometry(application.display, root, &w, &i, &i, &width,  &height,&u,&u);
			XImage*image = XGetImage(application.display,root,0,0,width,height,AllPlanes,XYPixmap);
			if (!image)
				return NULL;
			desktop_background.setDimensions(image->width,image->height,3);
			for (int y = 0; y < image->height; y++)
				memcpy(desktop_background.getData()+(image->height-y-1)*image->width*3,image->data+y*image->bytes_per_line,image->width*3);
			XDestroyImage(image);

		#elif SYSTEM==WINDOWS

			HWND root = GetAncestor(window,GA_ROOT);
			if (!root || root == window)
				root = GetShellWindow();

			if (!root || root == window)
				root = GetDesktopWindow();
			if (!root)
				return NULL;
			// Create a normal DC and a memory DC for the entire screen. The
			// normal DC provides a "snapshot" of the screen contents. The
			// memory DC keeps a copy of this "snapshot" in the associated
			// bitmap.
	        ShowWindow(window, SW_HIDE);
			Sleep(10);
			HDC hdcScreen = 	::GetDC(root),								//CreateDC("DISPLAY", NULL, NULL, NULL),
				hdcCompatible = CreateCompatibleDC(hdcScreen);

			// Create a compatible bitmap for hdcScreen.
/*		unsigned width = GetDeviceCaps(hdcScreen, HORZRES),
						height = GetDeviceCaps(hdcScreen, VERTRES);*/
			RECT rootrect;
			GetWindowRect(root,&rootrect);
			unsigned width = rootrect.right-rootrect.left,
						height = rootrect.bottom - rootrect.top;

			HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen,
								width,
								height);
//		ShowMessage("got screen bitmap - now copying ("+IntToStr(GetDeviceCaps(hdcScreen, HORZRES))+"x"+IntToStr(GetDeviceCaps(hdcScreen, VERTRES))+")");
			if (!hbmScreen
				||
				!SelectObject(hdcCompatible, hbmScreen)
				||
				!BitBlt(hdcCompatible,
					0,0,
	               width, height,
	               hdcScreen,
	               rootrect.left,rootrect.top,
	               SRCCOPY))
			{
				ErrMessage("failed to copy");
				//DeleteDC(hdcScreen);
				ReleaseDC(root,hdcScreen);
				DeleteDC(hdcCompatible);
				if (hbmScreen)
					DeleteObject(hbmScreen);
				return NULL;
			}
			if (!hidden)
				ShowWindow(window, SW_SHOW);

			desktop_background.setDimensions(width,height,3);

			BITMAPINFO info;
			info.bmiHeader.biSize = sizeof(info.bmiHeader);
			info.bmiHeader.biWidth = width;
			info.bmiHeader.biHeight = height;
			info.bmiHeader.biPlanes = 1;
			info.bmiHeader.biBitCount = 24;
			info.bmiHeader.biCompression = BI_RGB;
			info.bmiHeader.biSizeImage = 0;
			info.bmiHeader.biXPelsPerMeter = 100;
			info.bmiHeader.biYPelsPerMeter = 100;
			info.bmiHeader.biClrUsed = 0;
			info.bmiHeader.biClrImportant = 0;



			GetDIBits(hdcCompatible,hbmScreen,0,height,desktop_background.getData(),&info,DIB_RGB_COLORS);
/*		char*source_data = (char*)bitmap.bmBits;

			for (int y = 0; y < height; y++)
				memcpy(desktop_background.Data+(height-y-1)*width*3,source_data+y*bitmap.bmWidthBytes,width*3);*/

/*		for (int x = 0; x < width; x++)
				for (int y = 0; y < height; y++)
				{
					COLORREF pixel = GetPixel(hdcCompatible,x,y);
					desktop_background.set(x,height-1-y,pixel&0xFF,(pixel>>8)&0xFF,(pixel>>16)&0xFF);
				}*/
			desktop_background.swapChannels(0,2);
			ReleaseDC(root,hdcScreen);
//		DeleteDC(hdcScreen);
			DeleteDC(hdcCompatible);
			DeleteObject(hbmScreen);

		#endif

		return &desktop_background;
	}



	void Window::assembleClientRect(RECT&out)	const
	{
		out.left = location.left+client_offset_x;
		out.top = location.top+client_offset_y;
		out.right = out.left+client_width;
		out.bottom = out.top+client_height;
	}

	bool	Window::isUpdating()	const
	{
		return in_mutex != 0;
	}

	bool Window::beginUpdate(ContextHandle&previous_handle)
	{
		if (in_mutex)
			return false;
		update_mutex.lock();
		if (in_mutex)
		{
			update_mutex.release();
			return false;
		}
		in_mutex = __LINE__;
		detailBeginUpdate(previous_handle);
		return true;
	}

	void Window::forceBeginUpdate(ContextHandle&previous_handle)
	{
		update_mutex.lock();

		ASSERT__(!in_mutex);
		//while (in_mutex);

		in_mutex = __LINE__;

		detailBeginUpdate(previous_handle);
	}

	void	Window::detailBeginUpdate(ContextHandle&previous_handle)
	{
		RECT region;
		assembleClientRect(region);
		previous_handle = Window::getCurrentContext();
		#if SYSTEM==UNIX
			glXMakeCurrent(application.display,window,gl_context);
			if (font_remake_counter)
			{
				font_remake_counter--;
				textout.getFont().make(0,application.x_menu_font.c_str(),application.x_menu_font_size);
			}
		#elif SYSTEM==WINDOWS
			wglMakeCurrent(local_context.device_context,local_context.gl_context);
			if (font_remake_counter)
			{
				font_remake_counter--;
				textout.getFont().make(local_context.device_context,application.x_menu_font.c_str(),application.x_menu_font_size);
			}
		#endif

	    bind();
	    glViewport(0,0,client_width,client_height);

	    Engine::timing.update();
		mouse.redefineWindow(region,window);
		mouse.update();
		
		display.overrideSetClientResolution(Resolution(client_width,client_height));
//		display.lockRegion();

		glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	}

	void Window::endUpdate(ContextHandle previous_handle)
	{
		//display.unlockRegion();
		#if SYSTEM==WINDOWS
	        SwapBuffers(local_context.device_context);
			wglMakeCurrent(previous_handle.device_context,previous_handle.gl_context);
			//ASSERT2__(wglMakeCurrent(previous_handle.device_context,previous_handle.gl_context),previous_handle.device_context,previous_handle.gl_context);
			//ASSERT_EQUAL__(wglGetCurrentContext(),previous_handle.gl_context);
	    #elif SYSTEM==UNIX
			if (menu.children)
			{
				glPushAttrib(GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_ENABLE_BIT|GL_POINT_BIT);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
				glDisable(GL_FOG);
				glDisable(GL_TEXTURE_1D);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_3D);
				glDisable(GL_TEXTURE_CUBE_MAP);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_LIGHTING);
				glDisable(GL_LINE_SMOOTH);
				glDisable(GL_CULL_FACE);

				glViewport(0,0,client_width,client_height+application.x_menu_height);
				glMatrixMode(GL_PROJECTION);
					glPushMatrix();
						glLoadIdentity();
						glOrtho(0,client_width,0,client_height+application.x_menu_height,-1,1);
						glMatrixMode(GL_MODELVIEW);
							glPushMatrix();
								glLoadIdentity();

								glDisable(GL_TEXTURE_2D);
								glColor3f(X_MENU_BACKGROUND);
								glBegin(GL_QUADS);
									glVertex2i(0,client_height);
									glVertex2i(client_width,client_height);
									glVertex2i(client_width,client_height+application.x_menu_height);
									glVertex2i(0,client_height+application.x_menu_height);
								glEnd();
								textout.color(0,0,0);
								menu.update(5,client_height+application.x_menu_height/3,this);

								menu.render(this);

		/*                        textout.SetPosition(5,client_height+x_menu_height/3);
								textout.print("File   Edit");*/

							glPopMatrix();
						glMatrixMode(GL_PROJECTION);
					glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopAttrib();
			}



	        glXSwapBuffers(application.display,window);
			//glXMakeCurrent(application.display,window,None);
	    #endif
		requires_update = false;
		in_mutex = false;
		update_mutex.release();
	}

	void Window::update()
	{
		if (!created || is_new)
			return;
		//Window::ContextHandle h = Window::getCurrentContext();
		ContextHandle previous_handle;
		if (beginUpdate(previous_handle))
		{
			onPaint();

			endUpdate(previous_handle);
			//ASSERT_EQUAL__(wglGetCurrentContext(),h.gl_context);
			//ASSERT_EQUAL__(previous_handle.gl_context,h.gl_context);
		}
	}

	void Window::forceUpdate()
	{
		if (!created || is_new)
			return;
		//Window::ContextHandle h = Window::getCurrentContext();
		ContextHandle previous_handle;
		forceBeginUpdate(previous_handle);
			onPaint();
		endUpdate(previous_handle);
	}


	void Window::updateIfRequired()
	{
		if (requires_update)
			update();
	}

	bool	Window::bindContext(ContextHandle&previous_handle)
	{
		if (!created || in_mutex)
			return false;
		update_mutex.lock();
		if (in_mutex)
		{
			update_mutex.release();
			return false;
		}
		in_mutex = __LINE__;
		previous_handle = Window::getCurrentContext();
		#if SYSTEM==UNIX
			glXGetCurrentContext();
			glXMakeCurrent(application.display,window,gl_context);
		#elif SYSTEM==WINDOWS
			wglGetCurrentContext();
			ASSERT__(wglMakeCurrent(local_context.device_context,local_context.gl_context));
		#endif

		return true;
	}

	void	Window::releaseContext(ContextHandle previous_handle)
	{
		#if SYSTEM==WINDOWS
			wglMakeCurrent(previous_handle.device_context,previous_handle.gl_context);
		#elif SYSTEM==UNIX

			glXMakeCurrent(application.display,None,None);
		#endif
		in_mutex = false;
		update_mutex.release();

	}
	
	void	Window::callEmbedded(void (Window::*embedded)())
	{
		ContextHandle previous_handle;
		if (bindContext(previous_handle))
		{
			(this->*embedded)();
			releaseContext(previous_handle);
		}
	}
	void	Window::callEmbedded(void (Window::*embedded)(void*),void*parameter)
	{
		ContextHandle previous_handle;
		if (bindContext(previous_handle))
		{
			(this->*embedded)(parameter);
			releaseContext(previous_handle);
		}
	}


	void	Window::callEmbedded(void (*embedded)())
	{
		ContextHandle previous_handle;
		if (bindContext(previous_handle))
		{
			embedded();
			releaseContext(previous_handle);
		}
	}
	void	Window::callEmbedded(void (*embedded)(void*),void*parameter)
	{
		ContextHandle previous_handle;
		if (bindContext(previous_handle))
		{
			embedded(parameter);
			releaseContext(previous_handle);
		}
	}




	bool	Window::require(unsigned extensions)
	{
		ContextHandle previous_handle;
		if (bindContext(previous_handle))
		{
			bool result = Engine::glExtensions.Init(extensions);
			releaseContext(previous_handle);
			return result;
		}
		return false;
	}


	bool Window::requiresUpdate()	const
	{
		return requires_update;
	}


	void Window::SignalWindowContentChange(bool ru)
	{
		if (ru)
			SignalWindowContentChange();
		else
			requires_update = ru;
	}

	void Window::SignalWindowContentChange()
	{
		requires_update = true;
	}



	bool Window::isFocused()	const
	{
		#if SYSTEM==WINDOWS
			return GetFocus() == window;
		#elif SYSTEM==UNIX
			Window focus;
			int revert_to;
			XGetInputFocus(application.display, &focus, &revert_to);
			return focus == window;
		#endif
	}


	void Window::acceptFileDrop()
	{
		accept_dropped_files = true;
		#if SYSTEM==WINDOWS
			if (created)
				DragAcceptFiles(window,TRUE);
		#endif
	}
	
	void Window::onFileDrop(const Array<String>&files)
	{
		if (file_drop_handler)
			file_drop_handler(files);
	}
	
	void Window::acceptFileDrop(void (*handler)(const Array<String>&files))
	{
		file_drop_handler = handler;
		accept_dropped_files = handler != NULL;
		#if SYSTEM==WINDOWS
			if (created)
				DragAcceptFiles(window,accept_dropped_files);
		#endif
	}

	void Window::blockFileDrop()
	{
		accept_dropped_files = false;
		#if SYSTEM==WINDOWS
			if (created)
				DragAcceptFiles(window,FALSE);
		#endif
	}



	POINT		Window::getMouse(bool relative)	const
	{
		POINT rs;
		#if SYSTEM==UNIX
			Window dummy,last_root;
			int root_x,root_y,win_x,win_y;
			unsigned idummy;
			XQueryPointer(::getDisplay(),window,&last_root,&dummy,&root_x,&root_y,&win_x,&win_y,&idummy);
			if (relative)
			{
				rs.x = win_x;
				rs.y = win_y;
			}
			else
			{
				rs.x = root_x;
				rs.y = root_y;
			}
		#elif SYSTEM==WINDOWS
			GetCursorPos(&rs);
			if (relative)
			{
				rs.x -= location.left;
				rs.y -= location.top;
			}
		#endif

		return rs;
	}
	/*
	bool			Window::isInClientArea(const POINT&p)	const
	{
		return p.x > location.left + client_offset_x && p.y > location.top + client_offset_y && p.x < location.left + client_offset_x + (int)client_width && p.x < location.top + client_offset_y + (int)client_height;
	}
	*/
	void			Window::relativate(const POINT&p, float&x, float &y)	const
	{
		x = (float)(p.x-location.left-client_offset_x)/(float)(client_width);
		y = 1.0f-(float)(p.y-location.top-client_offset_y)/(float)(client_height);
	}

	void			Window::getRelativeMouse(float&x, float&y)	const
	{
		POINT p = getMouse(false);
		relativate(p,x,y);
	}

	bool			Window::hasMouse()	const
	{
		POINT p = getMouse(false);
		return p.x > location.left && p.y > location.top && p.x < location.right && p.y < location.bottom;
	}


	void			Window::pick(const Engine::Aspect<float>&aspect)
	{
		Resolution res = clientSize();
		display.SetViewport(display.transformViewport(aspect.region,res),res);	//to allow FBO region
		//glViewport((GLint)(aspect.region.left*(float)client_width),(GLint)(aspect.region.bottom*(float)client_height),(GLint)(aspect.region.width()*(float)client_width),(GLint)(aspect.region.height()*(float)client_height));
		display.SetCameraMatrices(aspect.view,aspect.projection,aspect.viewInvert);
		display.setDepthTest(aspect.depthTest);
		Engine::environment_matrix.x.xyz = aspect.viewInvert.x.xyz;
		Engine::environment_matrix.y.xyz = aspect.viewInvert.y.xyz;
		Engine::environment_matrix.z.xyz = aspect.viewInvert.z.xyz;
	}

	void Window::clearMenu()
	{
		menu.children.clear();
		menu.last_focus = NULL;
		menu.down_focus = NULL;
		#if SYSTEM==WINDOWS
			if (created)
			{
				SetMenu(window,NULL);
				DrawMenuBar(window);
			}
		#endif
	}


	
	
	MenuEntry* Window::createMenuEntry(const String&path,Menu::pSimpleCallback callback)
	{
		return createMenuEntryT(path,callback);
	}
	
	MenuEntry* Window::createMenuEntry(const String&path,Menu::pSimpleEntryCallback callback)
	{
		return createMenuEntryT(path,callback);
	}
	
	MenuEntry* Window::createMenuEntry(const String&path,Menu::pCallback callback)
	{
		return createMenuEntryT(path,callback);
	}
	
	MenuEntry* Window::createMenuEntry(const String&path,Menu::pEntryCallback callback)
	{
		return createMenuEntryT(path,callback);
	}


	MenuEntry* Window::findMenuEntry(const String&path)
	{
		CAString buffer(path.c_str(), path.length()+1);
		return menu.find(buffer.pointer(),MenuEntry::Exact);
	}

	void Window::destroyMenuEntry(MenuEntry*entry)
	{
		if (!entry)
			return;
		entry->parent->detach(entry);
		DISCARD(entry);
		#if SYSTEM==WINDOWS
			if (created)
			{
				SetMenu(window,menu.menu_handle);
				DrawMenuBar(window);
			}
		#endif
	}

	bool Window::renameMenuEntry(MenuEntry*origin, const String&new_path)
	{
		if (!origin || !origin->parent)
			return false;
		CAString buffer(new_path.c_str(),new_path.length()+1);
		
		origin->parent->detach(origin);
		MenuEntry*destination = menu.find(buffer.pointer(),MenuEntry::Closest);

		const char*name_begin = buffer+new_path.length()-1;
		while (name_begin > buffer.pointer() && *name_begin != '/')
			name_begin--;
		if (*name_begin == '/')
			name_begin++;
		origin->caption = name_begin;

		destination->attach(origin);

		#if SYSTEM==WINDOWS
			if (created)
			{
				SetMenu(window,menu.menu_handle);
				DrawMenuBar(window);
			}
		#endif
		return true;
	}



	Resolution		Window::clientSize()	const
	{
		return Resolution(client_width,client_height);
	}

	unsigned		Window::clientWidth()	const
	{
		return client_width;
	}

	unsigned		Window::clientHeight()	const
	{
		return client_height;
	}

	float			Window::clientAspect()	const
	{
		return (float)client_width/(float)client_height;
	}


	unsigned		Window::clientOffsetX()	const
	{
		return client_offset_x;
	}

	unsigned		Window::clientOffsetY()	const
	{
		return client_offset_y;
	}

	unsigned		Window::originX()	const
	{
		return location.left;
	}

	unsigned		Window::originY()	const
	{
		return location.top;
	}

	unsigned		Window::height()	const
	{
		return location.bottom-location.top;
	}

	unsigned		Window::width()	const
	{
		return location.right-location.left;
	}


	bool Window::isVisible()	const
	{
		if (!created)
			return false;
		#if SYSTEM==WINDOWS
			return IsWindowVisible(window)!=0;
		#else
			return !hidden;
		#endif
	}


	void Window::bind()
	{
		if (!this || !created)
			return;
		#if SYSTEM==UNIX
			Window root_return,outer_window,*children;
			unsigned dummy;


			XGetGeometry(application.display,window, &root_return, &client_offset_x, &client_offset_y, &client_width, &client_height, &dummy, &dummy);

			XQueryTree(application.display, window, &root_return, &outer_window,&children,&dummy);
			if (children)
				XFree(children);
			if (root_return == outer_window)
			{
				client_offset_x = 0;
				client_offset_y = 0;
				outer_window = window;
			}
			XWindowAttributes	attributes;
			XGetWindowAttributes(application.display, outer_window, &attributes);
			location.left = attributes.x;
			location.top = attributes.y;
			location.right = location.left+attributes.width;
			location.bottom = location.top+attributes.height;

			if (outer_window == window)
			{
				client_width = attributes.width;
				client_height = attributes.height;
			}

			if (menu.children.count())
			{
				client_height -= application.x_menu_height;
				client_offset_y += application.x_menu_height;
			}

			screen = attributes.screen;
			mouse.assign(application.display,window,XScreenNumberOfScreen(screen));
		#elif SYSTEM==WINDOWS
			WINDOWINFO	info;
			GetWindowInfo(window,&info);
			location = info.rcWindow;
			RECT client_rect = info.rcClient;

			client_width = client_rect.right-client_rect.left;
			client_height = client_rect.bottom-client_rect.top;

			client_offset_x = client_rect.left-location.left;
			client_offset_y = client_rect.top-location.top;
			screen = MonitorFromWindow(window,MONITOR_DEFAULTTONEAREST);
		#endif
	    if (application.active_window == this)
	        return;
	    application.active_window = this;
	    //input.chooseProfile(profile);

		RECT region;
		assembleClientRect(region);
		mouse.redefineWindow(region,window);


		if (application.active_screen != screen)
		{
			application.active_screen = screen;
			RECT	screen_location;

			#if SYSTEM==UNIX
	 			Window dummy;
	 			int x, y;
	 			unsigned udummy;
	 			XGetGeometry(application.display,root_return,&dummy,&x,&y,&udummy,&udummy,&udummy,&udummy);

	 			screen_location.left = x;
	 			screen_location.top = y;
	 			screen_location.bottom = screen_location.top+XHeightOfScreen(attributes.screen);
	 			screen_location.right = screen_location.left+XWidthOfScreen(attributes.screen);

			#elif SYSTEM==WINDOWS
				MONITORINFO	info;
				info.cbSize = sizeof(info);
				GetMonitorInfo(screen,&info);
				screen_location = info.rcMonitor;
			#endif

			mouse.setRegion(screen_location);
		}
	}

	HWND			Window::getWindow()	const
	{
		return window;
	}

	bool Window::isCreated()	const
	{
		return created;
	}
	
	bool	Window::make(const String&name)
	{
		if (created)
			destroy();
		return create(location.left,location.top,location.right-location.left,location.bottom-location.top,name,config);
	}
	
	bool	Window::make(int x, int y, int width, int height, const String&name)
	{
		if (created)
			destroy();
		return create(x,y,width,height,name,config);
	}
	
	bool	Window::make(int x, int y, int width, int height, const String&name, const Engine::TVisualConfig&config)
	{
		if (created)
			destroy();
		return create(x,y,width,height,name,config);
	}



	bool	Window::makeAsChild(const Window&parent, const String&name)
	{
		if (created)
			destroy();
		return create(location.left,location.top,location.right-location.left,location.bottom-location.top,name,config,parent.getWindow());
	}
	
	bool	Window::makeAsChild(const Window&parent, int x, int y, int width, int height, const String&name)
	{
		if (created)
			destroy();
		return create(x, y, width, height, name, config, parent.getWindow());
	}
	
	bool	Window::makeAsChild(const Window&parent, int x, int y, int width, int height, const String&name, const Engine::TVisualConfig&config)
	{
		if (created)
			destroy();
		return create(x, y, width, height, name, config, parent.getWindow());
	}



	bool	Window::makeAsChild(HWND parent, const String&name)
	{
		if (created)
			destroy();
		return create(location.left, location.top, location.right - location.left, location.bottom - location.top, name, config, parent);
	}

	bool	Window::makeAsChild(HWND parent, int x, int y, int width, int height, const String&name)
	{
		if (created)
			destroy();
		return create(x, y, width, height, name, config, parent);
	}

	bool	Window::makeAsChild(HWND parent, int x, int y, int width, int height, const String&name, const Engine::TVisualConfig&config)
	{
		if (created)
			destroy();
		return create(x, y, width, height, name, config, parent);
	}



	bool Window::remake()
	{
		if (created)
		{
			return show();
		}
		return create(location.left,location.top,location.right-location.left,location.bottom-location.top,name,config);
	}


	void Window::moveTo(int x, int y, int width, int height)
	{
		location.left = x;
		location.top = y;
		location.right = x+width;
		location.bottom = y+height;

		if (created)
		{
			#if SYSTEM==UNIX
	            XMoveResizeWindow(application.display, window, x,y,width,height);
			#elif SYSTEM==WINDOWS
				SetWindowPos(window,NULL,x,y,width,height,0);
			#endif
			update();
		}
	}

	void	Window::stayOnTop(bool b)
	{
		if (!created)
			return;
		#if SYSTEM==WINDOWS
			SetWindowPos(window,b?HWND_TOPMOST:HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
/*		if (!b)
				SetWindowPos(window,GetNextWindow(window,GW_HWNDNEXT),0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);*/
		#endif
	}





	void Window::hide()
	{
		if (hidden || !created)
			return;
		#if SYSTEM==WINDOWS
			ShowWindow(window,SW_HIDE);
		#elif SYSTEM==UNIX
			XUnmapWindow(application.display,window);
		#endif
		hidden = true;
	}

	bool Window::show()
	{
		if (!created)
		{
			return remake();
		}
		#if SYSTEM==WINDOWS
			ShowWindow(window,SW_SHOW);
		#elif SYSTEM==UNIX
			if (hidden)
				XMapWindow(application.display,window);
			XRaiseWindow(application.display,window);
		#endif
		hidden = false;
		return true;
	}

	bool Window::create(int x, int y, int width, int height, const String&name_, const Engine::TVisualConfig&config_, HWND  parent/* = NULL*/)
	{
		font_remake_counter = 2;
		config = config_;

		display.config = config;
		//clearMenu();
		application.creating_window = this;
		hidden = false;
		name = name_;
		location.left = x;
		location.top = y;
		location.right = x+width;
		location.bottom = y+height;
		created = false;
		is_new = true;
		requires_update = true;
	    error = NoError;
	    if (!application.Create())
	    {
	        error = ContextError;
	        return false;
	    }
	    while (true)
	    {
	        bool retry = false;

	        #if SYSTEM==WINDOWS
	            DWORD style = WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
				if (parent != NULL)
					//style |= WS_CHILD | WS_BORDER | WS_CAPTION;
					style |= WS_CHILD;
				else
					style |= WS_POPUP | WS_OVERLAPPEDWINDOW | WS_SIZEBOX;

				
				Array<wchar_t>	w_name;
				w_name.setSize(name.length()+1);
				for (unsigned i = 0; i < name.length(); i++)
					w_name[i] = (USHORT)(BYTE)name.get(i);
				w_name.last() = 0;

				window = CreateWindowW(
					WINDOW_CONTEXT_CLASS_NAME,w_name.pointer(),
					style,
					x,y, //Position
					width,height, //Dimensionen
					parent?parent:HWND_DESKTOP,
					NULL,
					application.hInstance, //Application
					NULL);						

	            if (!window)
	            {
	                error = WindowCreationFailed;
	                return false;
	            }
	            ShowWindow(window, SW_SHOW);
	            UpdateWindow(window);
				#if 0
					ShowWindow(window, SW_HIDE);
					AnimateWindow(window,200,0x00080000);
				#endif
	            //locateWindow();
	            SetForegroundWindow(window);
	            SetFocus(window);

				#ifndef PFD_SUPPORT_COMPOSITION
					#define PFD_SUPPORT_COMPOSITION	0x00008000
				#endif

	            RECT Screen;
	            static PIXELFORMATDESCRIPTOR pfd=
	            {
	                sizeof(PIXELFORMATDESCRIPTOR), //own size for internal copy-functions i presume
	                1,  //version
	                PFD_DRAW_TO_WINDOW |        //draw to window rather than to bitmap (have to look up later)
	                PFD_SUPPORT_OPENGL |        //use openGL (could use "PFD_SUPPORT_DIRECTDRAW" additional maybe?)
					PFD_SUPPORT_COMPOSITION |	//aero
	                PFD_DOUBLEBUFFER,           //swap-buffer-system
	                PFD_TYPE_RGBA,              //color-type supporting alpha-channel
	                config.color_buffer_bits,   //by default use 32 bits here
	                0,0,0,0,0,0,
	                config.alpha_buffer_bits,   //by default use 8 bits here
	                0,
	                config.accumulation_buffer_bits,    //should be zero by default
	                0,0,0,0,                            //ignored bits of accumulation_buffer
	                config.depth_buffer_bits,           //depth-buffer-bits 16 by default
	                config.stencil_buffer_bits,         //bits for stencil-buffer to use
	                config.auxiliary_buffer_bits,       //auxiliary_buffer_bits (whatever that buffer can be used for)
	                PFD_MAIN_PLANE,                     //draw to first layer only (would need to enable PFD_SWAP_LAYER_BUFFERS to use multible layers)
	                0,                                  //reserved (for whatever)
	                0,0,0                               //"Layer-Masks ignored" - maybe this is to reduce the ammount of colors displayed... would UpdateProjection sense somehow - each bit ignored would half the number
	            };
	            GLint PixelFormat;
	            local_context.device_context = ::GetDC(window);
	            if (!local_context.device_context)
	            {
	                DestroyWindow(window);
	                error = UnableToRetrieveDC;
	                return false;
	            }

	            if (!wglChoosePixelFormat)
	            {
	                PixelFormat = ChoosePixelFormat(local_context.device_context, &pfd);
	                if (!PixelFormat)
	                {
	                    ReleaseDC(window,local_context.device_context);
	                    DestroyWindow(window);
	                    error = ConfigUnsupported;
	                    return false;
	                }
	                if (config.fsaa_samples)
	                    retry = true;
	            }
	            else
	            {
	                unsigned nof;
	                int attribs[] =
	                {
	                    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
	                    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
	                    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
	                    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
	                    WGL_STEREO_ARB, GL_FALSE,
	                    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
	                    WGL_COLOR_BITS_ARB, config.color_buffer_bits,
	                    WGL_ALPHA_BITS_ARB, config.alpha_buffer_bits,
	                    WGL_DEPTH_BITS_ARB, config.depth_buffer_bits,
	                    WGL_STENCIL_BITS_ARB, config.stencil_buffer_bits,
	                    config.fsaa_samples?WGL_SAMPLES_ARB:0, config.fsaa_samples, //zero terminate here if no samples set
	                    WGL_SAMPLE_BUFFERS_ARB,config.fsaa_samples?GL_TRUE:GL_FALSE,
	                    0,0
	                };

	                /*
	                it seems some implementations of opengl do not support the WGL_SAMPLES_ARB / WGL_SAMPLE_BUFFERS_ARB - indices
	                and fail when assigned.
	                strange.
	                */
	                if (!wglChoosePixelFormat(local_context.device_context,attribs,NULL,1,&PixelFormat,&nof) || !nof)
	                {
	                    attribs[13] = 24;
	                    attribs[17] = 16;
	                    attribs[19] = 8;
	                    attribs[20] = 0; //zero terminate here
	                    attribs[21] = 0;
//                    config.fsaa_samples = 0;
	                    if (!wglChoosePixelFormat(local_context.device_context,attribs,NULL,1,&PixelFormat,&nof) || !nof)
	                    {
	                        ReleaseDC(window,local_context.device_context);
	                        DestroyWindow(window);
	                        error = ConfigRejected;
	                        return false;
	                    }
	                }
	                DescribePixelFormat(local_context.device_context,PixelFormat,sizeof(pfd),&pfd);
	            }

	            if (!SetPixelFormat(local_context.device_context,PixelFormat,&pfd))
	            {
	                ReleaseDC(window,local_context.device_context);
	                DestroyWindow(window);
	                error = PixelFormatSetFailed;
	                return false;
	            }

	            local_context.gl_context = wglCreateContext(local_context.device_context);
	            if (!local_context.gl_context)
	            {
	                ReleaseDC(window,local_context.device_context);
	                DestroyWindow(window);
	                error = ContextCreationFailed;
	                return false;
	            }

	            Window*root = application.windows.first();
	            if (root)
	                wglShareLists(root->local_context.gl_context,local_context.gl_context);


	            if (!wglMakeCurrent(local_context.device_context,local_context.gl_context))
	            {
	                wglDeleteContext(local_context.gl_context);
	                ReleaseDC(window,local_context.device_context);
	                DestroyWindow(window);
	                error = ContextNotBindable;
	                return false;
	            }
	            GetClientRect(window, &Screen);
	            Engine::glExtensions.Initialize(local_context.device_context);
	            Engine::glExtensions.Init(EXT_WIN_CONTROL_BIT|EXT_MULTITEXTURE_BIT|EXT_OCCLUSION_QUERY_BIT|EXT_VERTEX_BUFFER_OBJECT_BIT);

	            if (retry && wglChoosePixelFormat != NULL)
				{
	                wglDeleteContext(local_context.gl_context);
	                ReleaseDC(window,local_context.device_context);
	                DestroyWindow(window);
	                continue;
				}

	            if (wglSwapInterval)
	                wglSwapInterval(config.vertical_sync);

				screen = MonitorFromWindow(window,MONITOR_DEFAULTTONEAREST);

				{
					SetMenu(window,menu.menu_handle);
					DrawMenuBar(window);
					DragAcceptFiles(window,accept_dropped_files);
				}


	        #elif SYSTEM==UNIX


	            int attribs[] = {
	                GLX_RGBA,
	                GLX_USE_GL,
	                GLX_DOUBLEBUFFER,
	                GLX_RED_SIZE,   config.color_buffer_bits/3,
	                GLX_GREEN_SIZE, config.color_buffer_bits/3,
	                GLX_BLUE_SIZE,  config.color_buffer_bits/3,
	                GLX_ALPHA_SIZE, config.alpha_buffer_bits,
	                GLX_DEPTH_SIZE, config.depth_buffer_bits,
	                GLX_STENCIL_SIZE, config.stencil_buffer_bits,
	                config.fsaa_samples?GLX_SAMPLES_ARB:None, config.fsaa_samples, //zero terminate here if no samples set
	                GLX_SAMPLE_BUFFERS_ARB,config.fsaa_samples?GL_TRUE:GL_FALSE,
	                None
	            };

	            visual = glXChooseVisual(application.display,DefaultScreen(application.display),attribs);
	            if (!visual)
	            {
	                int backup[] =
	                {
	                    GLX_RGBA,
	                    GLX_DOUBLEBUFFER,
	                    GLX_RED_SIZE,   8,
	                    GLX_GREEN_SIZE, 8,
	                    GLX_BLUE_SIZE,  8,
	                    GLX_ALPHA_SIZE, 8,
	                    GLX_DEPTH_SIZE, 24,
	                    GLX_STENCIL_SIZE, 4,
	                    None
	                };


	                visual = glXChooseVisual(application.display,DefaultScreen(application.display),backup);

	                if (!visual)
	                {
	                    error = ConfigRejected;
	                    return false;
	                }
	            }
//			screen = visual->screen;

	            Window*root = application.windows.first();
				gl_context = glXCreateContext(application.display,visual,root?root->gl_context:NULL,True);
	            if (!gl_context)
	            {
	                error = ContextCreationFailed;
	                XFree(visual);
	                visual = NULL;
	                return false;
	            }

	            if (!glXIsDirect(application.display,gl_context))
	                ErrMessage("warning: window "+name+" rendering indirect!");

	            glXSwapIntervalSGI(config.vertical_sync);


	            /*out_attributes.visual = visual->visual;
	            out_attributes.depth = visual->depth;
	            out_attributes.screen_number = visual->screen;*/


	            colormap=XCreateColormap(application.display,RootWindow(application.display,visual->screen),visual->visual,AllocNone);
	            if (!colormap)
	            {
	                XFree(visual);
	                glXDestroyContext(application.display,gl_context);
	                error = ColorMapCreationFailed;
	                return false;
	            }

	            XSetWindowAttributes attrib;
	            attrib.colormap = colormap;
	            attrib.border_pixel = CopyFromParent; //hide_border?None:
	            attrib.event_mask = ExposureMask|ButtonPressMask|StructureNotifyMask|KeyPressMask|SubstructureRedirectMask;
	            attrib.override_redirect = False;

	            window = XCreateWindow(application.display,RootWindow(application.display,visual->screen),x,y,width,height,True,visual->depth,InputOutput,visual->visual,CWBorderPixel|CWColormap|CWEventMask|CWOverrideRedirect,&attrib);
	            if (!window)
	            {
	                XFree(visual);
	                glXDestroyContext(application.display,gl_context);
	                XFreeColormap(application.display,colormap);
	                error = WindowCreationFailed;
	                return false;
	            }
	            Atom atom = XInternAtom (application.display, "WM_DELETE_WINDOW", False);
	            XSetWMProtocols(application.display, window, &atom , 1);

//            config = XRRGetScreenInfo(application.display,window);

	            XSetStandardProperties(application.display,window,name.c_str(),"",None,0,0,0);
	            XSelectInput(application.display, window,          /*EnterWindowMask | LeaveWindowMask | VisibilityChangeMask | ResizeRedirectMask | SubstructureRedirectMask | PropertyChangeMask |  */             ButtonPressMask | ButtonReleaseMask | FocusChangeMask | KeyPressMask | KeyReleaseMask | SubstructureNotifyMask |StructureNotifyMask | ExposureMask|PointerMotionMask);   //

	            //XGrabButton(application.display,AnyButton,AnyModifier,window,true,ButtonPressMask,GrabModeAsync,GrabModeAsync,None,None);


	            XMapWindow(application.display,window);
	            //XGrabKeyboard(display_, window, True, GrabModeAsync, GrabModeAsync, CurrentTime);
	            //XGrabPointer(display_, window, True, ButtonPressMask|ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None /*window*/, None, CurrentTime);
	            XMoveResizeWindow(application.display, window, x,y,width,height);

	            if (!glXMakeCurrent(application.display,window,gl_context))
	            {
	                XFree(visual);
	                glXDestroyContext(application.display,gl_context);
	                XDestroyWindow(application.display,window);
	                XFreeColormap(application.display,colormap);
	                error = ContextNotBindable;
	                return false;
	            }
				//XSetInputFocus(application.display, window, RevertToNone,CurrentTime);
	            Engine::gl_extensions.initialize(application.display,visual->screen);
	            Engine::gl_extensions.init(EXT_WIN_CONTROL_BIT|EXT_MULTITEXTURE_BIT);


	        #endif

	        break;
	    }
	    glViewport(0,0,width,height);
	    Engine::timing.initialize();

	   // pixelAspect = application.pixelAspectf(width,height);
		display.initDefaultExtensions();
	    //initGL();
		focused = true;
	    created = true;
	    application.windows.insert(this);
		application.creating_window = NULL;

		
	    glEnable(GL_BLEND);
	    glDepthFunc(GL_LEQUAL);
	    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	    glShadeModel(GL_SMOOTH);
	    glClearStencil(0);
	    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	    glClearDepth(1.0f);
	    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	    glMatrixMode(GL_MODELVIEW);

		onCreate();

		Window::update();
		return true;
	}


	void	Window::ignoreEvents(bool do_ignore)
	{
		ignoring_events = do_ignore;
	}


	void							Window::ShowBorder(bool newShow)
	{
		if (newShow == showingBorder)
			return;
		showingBorder = newShow;

		{
			LONG lStyle = GetWindowLong(window, GWL_STYLE);
			if (newShow)
				lStyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
			else
				lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
			SetWindowLong(window, GWL_STYLE, lStyle);
			SetWindowPos(window, NULL, 0,0,0,0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
		}


	}


	void Window::init()
	{
		showingBorder = true;
		accept_dropped_files = false;
		file_drop_handler = NULL;
		mouse.RegAnalogInputs();
		//joystick.regAnalogInputs();
		created = false;
		in_mutex = false;
		ignoring_events = false;

		font_remake_counter = 0;
	}

	Window::Window():keyboard(input),mouse(input),config(Engine::default_buffer_config)
	{
		init();
		//create(0,0,800,600,"MyWindow",Engine::default_buffer_config);
	}

	Window::Window(const String&name):keyboard(input),mouse(input)
	{
		init();
		int		x = 0,
				y = 0;
		#if SYSTEM==WINDOWS
			x = y = CW_USEDEFAULT;
		#endif
		create(x,y,800,600,name,Engine::default_buffer_config);
	}

	Window::Window(int x, int y, int width, int height, const String&name):keyboard(input),mouse(input)
	{
		init();
		create(x,y,width,height,name,Engine::default_buffer_config);
	}

	Window::Window(int x, int y, int width, int height, const String&name, const Engine::TVisualConfig&config):keyboard(input),mouse(input)
	{
		init();
		create(x,y,width,height,name,config);
	}



	Window::~Window()
	{
	    destroy();
	}


	void Window::destroy()
	{
		//clearMenu();
	    if (!created)
	        return;
		ContextHandle previous_handle;
	    if (bindContext(previous_handle))
	    {
	    	display.resetLighting();
			textout.getFont().unmake();
			if (previous_handle.gl_context == local_context.gl_context)
				previous_handle.gl_context = NULL;
	    	releaseContext(previous_handle);
	    }
		application.windows.drop(this);
	    #if SYSTEM==UNIX
//        glXMakeCurrent(application.display,window,NULL);   //<- this causes the application to crash
	        XFree(visual);
	        glXDestroyContext(application.display,gl_context);
	        XDestroyWindow(application.display,window);
	        XFreeColormap(application.display,colormap);
	    #elif SYSTEM==WINDOWS
			SetMenu(window,NULL);
	       // wglMakeCurrent(device_context,NULL);
	        wglDeleteContext(local_context.gl_context);
	        ReleaseDC(window,local_context.device_context);
	        DestroyWindow(window);
	    #endif
	    created = false;
	}


#if SYSTEM==WINDOWS

	#if SYSTEM==WINDOWS
		#ifndef WM_XBUTTONDOWN
			#define WM_XBUTTONDOWN 523
		#endif
		#ifndef WM_XBUTTONUP
			#define WM_XBUTTONUP 524
		#endif

		#ifndef VK_XBUTTON1
			#define VK_XBUTTON1 5
		#endif
		#ifndef VK_XBUTTON2
			#define VK_XBUTTON2 6
		#endif

		namespace Hook
		{
			HHOOK		hHook(NULL);
			Window		*installed_on[5]={NULL,NULL,NULL,NULL,NULL};
			bool		primary[5] = {false,false,false,false,false};
			
			LRESULT CALLBACK callback(int nCode,WPARAM wParam,LPARAM lParam);
		

			void	installIfNULL()
			{
				if (!hHook)
				{
					hHook = SetWindowsHookEx(WH_MOUSE_LL,callback,getInstance(),0);
				}
			}


			void	install(index_t button_index, Window*on)
			{
				#ifdef USE_HOOKS
					ASSERT_NOT_NULL__(on);
					ASSERT_LESS__(button_index,ARRAYSIZE(installed_on));
					//ASSERT_IS_NULL__(installed_on[button_index]);
					installed_on[button_index] = NULL;


					installIfNULL();
					bool matched = false;
					for (index_t k = 0; k < ARRAYSIZE(installed_on); k++)
						matched = matched || installed_on[k] == on;
					installed_on[button_index] = on;
					primary[button_index] = !matched;
				#endif
			}
		
			void	uninstallIfUnnecessary()
			{
				if (!installed_on[0] && !installed_on[1] && !installed_on[2] && !installed_on[3] && !installed_on[4] && hHook)
				{
					UnhookWindowsHookEx(hHook);
					hHook = NULL;
				}
			}

			void	triggerMoveEvent(bool update)
			{
				for (index_t k = 0; k < ARRAYSIZE(installed_on); k++)
				{
					if (primary[k] && installed_on[k])
					{
						POINT p;
						GetCursorPos(&p);
						const RECT&w = installed_on[k]->location;
						if (p.x < w.left || p.x > w.right || p.y < w.top || p.y > w.bottom)
						{
							if (update)
								installed_on[k]->mouse.update();
							installed_on[k]->onMouseMove();
							installed_on[k]->updateIfRequired();
						}
					}
				}
			}


			LRESULT CALLBACK callback(int nCode,WPARAM wParam,LPARAM lParam)
			{
				if (nCode < 0)
					return CallNextHookEx(hHook,nCode,wParam,lParam);
				if (nCode == HC_ACTION)
				{
					//Window*window = application.windows.lookup(hWnd);
		
					index_t button = 0xFF;
					switch (wParam)
					{
						case WM_LBUTTONUP:	button = 0; break;
						case WM_MBUTTONUP:  button = 1; break;
						case WM_RBUTTONUP:  button = 2; break;
						case WM_XBUTTONUP:
						{
							MSLLHOOKSTRUCT* MouseInfo = (MSLLHOOKSTRUCT*)lParam;
							button = 2+HIWORD(MouseInfo->mouseData);
						}
						break;
						case WM_MOVING:			/*triggerMoveEvent(false);*/						break;
						case WM_MOUSEMOVE:
							//if (window->mouseMovePntr)
							{
								triggerMoveEvent(true);
							}
						break;

					}
					if (button < ARRAYSIZE(installed_on))
					{
						if (installed_on[button] != NULL)
						{
							installed_on[button]->mouse.buttonUp((BYTE)button,true);
							installed_on[button]->onKeyUp((Key::Name)(Key::MouseButton0+button));
							installed_on[button] = NULL;
							
							for (index_t k = 0; k < ARRAYSIZE(installed_on); k++)
							{
								if (!installed_on[k])
									continue;
								bool matched = false;
								for (index_t j = 0; j < k; j++)
									if (installed_on[j] == installed_on[k])
									{
										matched = true;
										break;
									}
								primary[k] = !matched;
							}
						}

						uninstallIfUnnecessary();
					}
					return CallNextHookEx(hHook,-1,wParam,lParam);
				}

				return CallNextHookEx(hHook,nCode,wParam,lParam);
			}

		}

	#endif



	LRESULT CALLBACK wExecuteEvent(HWND hWnd, UINT Msg, WPARAM wParam,LPARAM lParam)
	{



		try
		{

			MenuEntry*entry;
			Window*window = application.windows.lookup(hWnd);
			if (window)
			{
				if (window->ignoring_events)
					return DefWindowProcW(hWnd, Msg, wParam, lParam);
				window->bind();
				switch (Msg)
				{
					case WM_SETCURSOR:
						return (LOWORD(lParam) == HTCLIENT) && window->mouse.cursorIsNotDefault() ? 1 : DefWindowProcW(hWnd, Msg, wParam, lParam);

					case MW_CREATE_WINDOW_REQUEST:
					{
						TWindowCreationRequest*req = reinterpret_cast<TWindowCreationRequest*>(lParam);
						if (req->parent)
							req->window->makeAsChild(*req->parent,req->x,req->y,req->width,req->height,req->name,req->config);
						else
							req->window->make(req->x,req->y,req->width,req->height,req->name,req->config);
					}
					break;
					case WM_ERASEBKGND: return 1;													break;
					case WM_COMMAND:
						if (!HIWORD(wParam) && (entry = application.menu_items.lookup(LOWORD(wParam))))
							entry->execute(window);
					return 0;
					case WM_SYSKEYDOWN:
			        case WM_KEYDOWN:
						window->input.keyDown((Key::Name)wParam);
						window->onKeyDown((Key::Name)wParam);
					break;
			        case WM_SYSKEYUP:
			        case WM_KEYUP:
						window->input.keyUp((Key::Name)wParam);
						window->onKeyUp((Key::Name)wParam);
					break;
			        case WM_CHAR:
						if (char c = window->keyboard.convertInput(wParam))
							window->onChar(c);
					break;
					case WM_LBUTTONDOWN:	window->mouse.buttonDown(0,true);window->onKeyDown(Key::MouseButton0); Hook::install(0,window);	break;
			        case WM_LBUTTONUP:  	window->mouse.buttonUp(0,true);window->onKeyUp(Key::MouseButton0);								break;
					case WM_MBUTTONDOWN:	window->mouse.buttonDown(1,true);window->onKeyDown(Key::MouseButton1); Hook::install(1,window);	break;
			        case WM_MBUTTONUP:  	window->mouse.buttonUp(1,true);window->onKeyUp(Key::MouseButton1);								break;
					case WM_RBUTTONDOWN:	window->mouse.buttonDown(2,true);window->onKeyDown(Key::MouseButton2); Hook::install(2,window);	break;
			        case WM_RBUTTONUP:  	window->mouse.buttonUp(2,true);window->onKeyUp(Key::MouseButton2);								break;
					case WM_XBUTTONDOWN:
					{
						WORD button = HIWORD (wParam)==1?3:4;
						window->mouse.buttonDown(button,true);
						window->onKeyDown((Key::Name)(Key::MouseButton0 + button));
						Hook::install(button,window);
					}
					break;
					case WM_XBUTTONUP:
					{
						WORD button = HIWORD (wParam)==1?3:4;
						window->mouse.buttonUp(button,true);
						window->onKeyUp((Key::Name)(Key::MouseButton0 + button));
						
						//mouse.buttonUp(HIWORD (wParam)==1?3:4);
					}
					return 0;


					case WM_MOUSELEAVE:	//does this event even work...???
						window->mouse.buttonUp(0,true);
						window->mouse.buttonUp(1,true);
						window->mouse.buttonUp(2,true);
						window->mouse.buttonUp(3,true);
						window->mouse.buttonUp(4,true);
				        window->onKeyUp(Key::MouseButton0);
				        window->onKeyUp(Key::MouseButton1);
				        window->onKeyUp(Key::MouseButton2);
				        window->onKeyUp(Key::MouseButton3);
				        window->onKeyUp(Key::MouseButton4);
					break;
			        case WM_MOUSEWHEEL:
						window->onMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
					break;
					case WM_MOVING:			window->onMove();						break;
					case WM_MOUSEMOVE:
						//if (window->mouseMovePntr)
						{
							window->mouse.update();
							window->onMouseMove();
						}
					break;
					case WM_QUIT:			application.shutting_down = true;								break;
			        case WM_DESTROY:
			        case WM_CLOSE:
						window->onDestroy();
						window->destroy();
					break;
					case WM_DISPLAYCHANGE:	window->mouse.setRegion(LOWORD(wParam),HIWORD(wParam));		break;
//		case WM_SHOWWINDOW:
			        case WM_SETFOCUS:
						if (!window->focused)
						{
							window->onFocus();
							window->focused = true;
						}
					break;
			        case WM_KILLFOCUS:
						if (window->focused)
						{
							window->onUnFocus();
							window->focused = false;
						}
					break;

					case WM_DROPFILES:
						if (window->accept_dropped_files)
						{
							HDROP hDrop = (HDROP)wParam;
							UINT cnt = DragQueryFile(hDrop,0xFFFFFFFF,NULL,0);
							if (cnt)
							{
								Array<String>	files(cnt);
								for (unsigned i = 0; i < cnt; i++)
								{
									unsigned len = DragQueryFile(hDrop,i,NULL,0);
									files[i].resize(len);
									DragQueryFileA(hDrop,i,files[i].mutablePointer(),len+1);
									if (strlen(files[i].c_str()) != files[i].length())
										FATAL__("File drop read error");
								}
								window->onFileDrop(files);
							}
							DragFinish(hDrop);
						}
					break;
					case WM_PRINT:
					case WM_PAINT:
						window->requires_update = true;
					break;
			        default:
						return application.event_hook(window,hWnd,Msg,wParam,lParam);
			    }
			}
			else
				if (Msg == WM_NCLBUTTONUP)
					ShowMessage("mouse_ up");
		}
		catch (const std::exception&exception)
		{
			application.fatal_exception = exception.what();
			application.fatal_exception += " ("+String(typeid(exception).name())+")";

			application.fatal_exception_caught = true;
		}
		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	    //return 0;
	}

#elif SYSTEM==UNIX

	static const char*  eventToString(int event)
	{
	    #define ecase(c)    case c: return #c;
	    switch (event)
	    {
	        ecase(Expose)
	        ecase(EnterNotify)
	        ecase(LeaveNotify)
	        ecase(MotionNotify)
	        ecase(KeymapNotify)
	        ecase(VisibilityNotify)
	        ecase(CirculateNotify)
	        ecase(ConfigureNotify)
	        ecase(GravityNotify)
	        ecase(MapNotify)
	        ecase(ReparentNotify)
	        ecase(UnmapNotify)
	        ecase(ResizeRequest)
	        ecase(CreateNotify)
	        ecase(PropertyNotify)
	        ecase(ColormapNotify)
	        ecase(KeyPress)
	        ecase(KeyRelease)
	        ecase(ButtonPress)
	        ecase(ButtonRelease)
	        ecase(FocusIn)
	        ecase(FocusOut)
	        ecase(DestroyNotify)
	    }
	    return "unknown";
	}


	void wExecuteEvent(XEvent&event)
	{
		Window*window = application.windows.lookup(event.xany.window);
		if (!window)
		{
			if (event.type == ButtonPress)
				cout << "button press on unknown window\n";
			return;
		}
		if (window->ignoring_events)
			return;
		window->bind();

//    ShowMessage("event 0x"+IntToHex((int)event.type,8)+" ("+eventToString(event.type)+")");
		static	Timer::Time last_motion = 0;

	    switch (event.type)
	    {
			default:
			{
				application.event_hook(window,event);
			}
			break;
	        case KeyPress:
	        {
				window->keyboard.keyDown((Key::Name)event.xkey.keycode);
	            window->onKeyDown(event.xkey.keycode);
	            if (!application.isFunctionKey(event.xkey.keycode))	//per definition of XLookupKeysym i should not have to check this
	            {
					KeySym symbol = XLookupKeysym(&event.xkey,window->input.pressed[Key::Shift]);
					if (symbol != NoSymbol)
					{
						if (char c = window->keyboard.convertInput(symbol))
							window->onChar(symbol);
					}
				}
	        }
	        break;
			case Expose:		if (!event.xexpose.count) window->update();	break;
	        case KeyRelease:
				window->keyboard.keyUp((Key::Name)event.xkey.keycode);
				window->onKeyUp(event.xkey.keycode);
			break;
			case ButtonPress:
			{
				application.windows.reset();
				while (Window*w = application.windows.each())
					if (w != window && w->menu.close())
						w->update();
				unsigned id = event.xbutton.button-Button1;
				bool redraw;
				if (!id)
				{
					POINT p = window->getMouse(true);
					p.y = window->client_height+application.x_menu_height-p.y;
					bool redraw;
					window->menu.mouseMove(p.x,p.y,redraw);
				}
				if (id != 0 || !window->menu.mouseDown(redraw))
				{
					if (id < 3)
					{
						window->mouse.buttonDown(id,true);
						window->onKeyDown((Key::Name)(Key::MouseButton0+id));
					}
					elif (id >= 5)
					{
						window->mouse.buttonDown(((id-5)%2)+3,true);
						window->onKeyDown((Key::Name)(Key::MouseButton0+((id-5)%2)+3));
					}
					else
					{
						short delta = (1-(id-3)*2)*30;
						window->onMouseWheel(delta);

					}
				}
				if (redraw)
					window->SignalWindowContentChange();
			}
			break;
			case ButtonRelease:
			{
				unsigned id = event.xbutton.button-Button1;
				if (!id)
				{
					POINT p = window->getMouse(true);
					p.y = window->client_height+application.x_menu_height-p.y;
					bool redraw;
					window->menu.mouseMove(p.x,p.y,redraw);
				}
				if (id != 0 || !window->menu.mouseUp(window))
				{
					if (id < 3)
					{
						window->mouse.buttonUp(id,true);
						window->onKeyUp((Key::Name)(Key::MouseButton0+id));
					}
					elif (id >= 5)
					{
						window->mouse.buttonUp(((id-5)%2)+3,true);
						window->onKeyUp((Key::Name)(Key::MouseButton0+((id-5)%2)+3));
					}
				}
				else
					window->SignalWindowContentChange();
			}
			break;
			case ConfigureRequest:
			case MotionNotify:

				if (!last_motion || timer.toSecondsf(timer.now()-last_motion)>0.05)	//prevent motion event stack overfilling
				{
					bool caught = false;
					POINT p = window->getMouse(true);
					p.y = window->client_height+application.x_menu_height-p.y;
					bool redraw;
					if (!window->menu.mouseMove(p.x,p.y,redraw))
					{
						//if (window->mouseMovePntr)
						{
							window->mouse.update();
							window->onMouseMove();
						}
					}
					if (redraw)
					{
						window->SignalWindowContentChange();
					}
					last_motion = timer.now();
				}


			break;
	        case FocusIn:
	        	if (!window->focused)
	        	{
					window->onFocus();
					window->focused = true;
				}
			break;
	        case FocusOut:
	        	if (window->focused)
	        	{
					window->onUnFocus();
					window->focused = false;
				}
			break;

			case 0x00000021:	//x button pressed
	        case DestroyNotify:
				window->onDestroy();
				window->destroy();
			break;
	    }
		if (window->requires_update)
		{
			window->update();
			if (event.type == MotionNotify || event.type == ConfigureRequest)
				last_motion = timer.now();
		}

	}

#endif


}
