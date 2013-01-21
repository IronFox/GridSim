#include "../../global_root.h"
#include "mouse.h"

    #include <iostream>
    using namespace std;


/******************************************************************

engine mouse-interface.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


namespace Engine
{

	const char*			Mouse::cursorToString(cursor_t cursor)
	{
		#undef CASE
		#define CASE(_CURSOR_)	case _CURSOR_: return #_CURSOR_;
		
		switch (cursor)
		{
			CASE(CursorType::None)
			CASE(CursorType::Hidden)
			CASE(CursorType::Default)
			CASE(CursorType::EditText)
			CASE(CursorType::Waiting)
			CASE(CursorType::HalfWaiting)
			CASE(CursorType::CrossHair)
			CASE(CursorType::Hand)
			CASE(CursorType::Help)
			CASE(CursorType::No)
			CASE(CursorType::SizeAll)
			CASE(CursorType::ResizeLeft)
			CASE(CursorType::ResizeRight)
			CASE(CursorType::ResizeUp)
			CASE(CursorType::ResizeDown)
			CASE(CursorType::ResizeDownRight)
			CASE(CursorType::ResizeUpLeft)
			CASE(CursorType::ResizeUpRight)
			CASE(CursorType::ResizeDownLeft)
		}
		return "Unknown cursor type";
		#undef CASE
	}


	bool Mouse::buttonDown(BYTE id, bool update_if_bound)
	{
		if (id >= 5 || buttons.down[id])
			return false;
		button_id = id;
		buttons.down[id] = true;
		buttons.pressed = true;
		if (update_if_bound)
			update();
		return map.keyDown(Key::MouseButton0+(int)id);
	}

	bool Mouse::buttonUp(BYTE id, bool update_if_bound)
	{
		if (id >= 5 || !buttons.down[id])
			return false;
		button_id = id;
		buttons.down[id] = false;
		buttons.pressed = buttons.down[0] || buttons.down[1] || buttons.down[2] || buttons.down[3] || buttons.down[4];
		if (update_if_bound)
			update();
		return map.keyUp(Key::MouseButton0+(int)id);
	}

	BYTE	Mouse::getButton()							const
	{
		return button_id;
	}




	void Mouse::bindWheel(wheelLink Link)
	{
	    wheel_link = Link;
	}

	#if SYSTEM==WINDOWS
	void Mouse::updateNoClip()
	{
	    no_clip.left = screen_clip.left-(screen_clip.right-screen_clip.left)*10;
	    no_clip.top = screen_clip.top-(screen_clip.bottom-screen_clip.top)*10;
	    no_clip.right = screen_clip.right+(screen_clip.right-screen_clip.left)*10;
	    no_clip.bottom = screen_clip.bottom+(screen_clip.bottom-screen_clip.top)*10;
	}

	#elif SYSTEM==UNIX

	    void Mouse::assign(Display*display_, Window window_, int screen_)
	    {
	        display = display_;
	        window_handle = window_;
	        screen = screen_;
			cursor_visible = true;
	    }
	    
	    void  Mouse::GetCursorPos(POINT*target)
	    {
	        Window dummy;
	        int root_x,root_y,win_x,win_y;
	        unsigned idummy;
	        XQueryPointer(display,window_handle,&last_root,&dummy,&root_x,&root_y,&win_x,&win_y,&idummy);
	        target->x = root_x;
	        target->y = root_y;
	        //cout << "query: "<<root_x<<", "<<root_y<<endl;
	    }
	    
	    void  Mouse::SetCursorPos(int x, int y)
	    {
	    	//cout << "set: "<<x<<", "<<y<<endl;
	        XWarpPointer(display,last_root,/*window_handle*/last_root,0,0,0,0,x,y);
	    }
	    

	#endif

	
	void	Mouse::loadCursors()
	{
		#if SYSTEM==WINDOWS
			cursor_reference[CursorType::Default] = LoadCursor(NULL,IDC_ARROW);
			cursor_reference[CursorType::EditText] = LoadCursor(NULL,IDC_IBEAM);
			cursor_reference[CursorType::Waiting] = LoadCursor(NULL,IDC_WAIT);
			cursor_reference[CursorType::HalfWaiting] = LoadCursor(NULL,IDC_APPSTARTING);
			cursor_reference[CursorType::CrossHair] = LoadCursor(NULL,IDC_CROSS);
			cursor_reference[CursorType::CrossHair] = LoadCursor(NULL,IDC_HAND);
			cursor_reference[CursorType::Help] = LoadCursor(NULL,IDC_HELP);
			cursor_reference[CursorType::No] = LoadCursor(NULL,IDC_NO);
			cursor_reference[CursorType::SizeAll] = LoadCursor(NULL,IDC_SIZEALL);
			cursor_reference[CursorType::ResizeRight] =
			cursor_reference[CursorType::ResizeLeft] = LoadCursor(NULL,IDC_SIZEWE);
			cursor_reference[CursorType::ResizeUp] =
			cursor_reference[CursorType::ResizeDown] = LoadCursor(NULL,IDC_SIZENS);
			cursor_reference[CursorType::ResizeUpLeft] =
			cursor_reference[CursorType::ResizeDownRight] = LoadCursor(NULL,IDC_SIZENWSE);
			cursor_reference[CursorType::ResizeUpRight] =
			cursor_reference[CursorType::ResizeDownLeft] = LoadCursor(NULL,IDC_SIZENESW);
		#elif SYSTEM_VARIANCE==LINUX
			
			
			cursor_reference[CursorType::Default] = XCreateFontCursor(display, XC_arrow);	//None does NOT work here... stupid documentation
			cursor_reference[CursorType::EditText] = XCreateFontCursor(display, XC_xterm);
			cursor_reference[CursorType::Waiting] = XCreateFontCursor(display, XC_watch);
			cursor_reference[CursorType::HalfWaiting] = XCreateFontCursor(display, XC_clock);
			cursor_reference[CursorType::CrossHair] = XCreateFontCursor(display, XC_crosshair);
			cursor_reference[CursorType::Hand] = XCreateFontCursor(display, XC_hand2);
			cursor_reference[CursorType::Help] = XCreateFontCursor(display, XC_question_arrow);
			cursor_reference[CursorType::No] = XCreateFontCursor(display, XC_cross);
			cursor_reference[CursorType::SizeAll] = XCreateFontCursor(display, XC_sizing);
			cursor_reference[CursorType::ResizeRight] = XCreateFontCursor(display, XC_right_side);
			cursor_reference[CursorType::ResizeLeft] = XCreateFontCursor(display, XC_left_side);
			cursor_reference[CursorType::ResizeUp] = XCreateFontCursor(display, XC_top_side);
			cursor_reference[CursorType::ResizeDown] = XCreateFontCursor(display, XC_bottom_side);
			cursor_reference[CursorType::ResizeDownRight] = XCreateFontCursor(display, XC_bottom_right_corner);
			cursor_reference[CursorType::ResizeUpRight] = XCreateFontCursor(display, XC_top_right_corner);
			cursor_reference[CursorType::ResizeUpLeft] = XCreateFontCursor(display, XC_top_left_corner);
			cursor_reference[CursorType::ResizeDownLeft] = XCreateFontCursor(display, XC_bottom_left_corner);
			
			/*for (unsigned i = 0; i < ARRAYSIZE(cursor_reference); i++)
				cout << cursorToString((cursor_t)i)<<" set to "<<cursor_reference[i]<<endl;*/
			
		#else
			#error stub
		#endif
	}
	
	Mouse::Mouse(InputMap&map_):map(map_),locked(false),focus(true),cursor_visible(true),force_invisible(false),wheel_link(NULL)
	#if SYSTEM==UNIX
	    ,blank_cursor((Cursor)0)
	#endif
	{
	    buttons.left_down   = false;
	    buttons.middle_down = false;
	    buttons.right_down  = false;
	    buttons.pressed     = false;
	    speed[0] = 100;
	    speed[1] = 100;
		memset(cursor_reference,0,sizeof(cursor_reference));
		loaded_cursor = NULL;

	    #if SYSTEM==WINDOWS
	        GetClipCursor(&initial_clip);
	        window = screen_clip = initial_clip;
		    mx = (screen_clip.left+screen_clip.right)/2;
		    my = (screen_clip.top+screen_clip.bottom)/2;		
			
	        updateNoClip();
			
		
	    #else
	        window.left = window.top = window.right = window.bottom = 0;
			
			
		
	    #endif
		
	}


	void Mouse::redefineWindow(RECT window_)
	{
	    window = window_;
		mx = (window.left+window.right)/2;
		my = (window.top+window.bottom)/2;		
	    //ShowMessage("window set to "+IntToStr(window.left)+", "+IntToStr(window.top)+" - "+IntToStr(window.right)+", "+IntToStr(window.bottom));
	}

	void Mouse::setRegion(int width, int height)
	{
	    mx = width/2;
	    my = height/2;
	    screen_clip.left = 0;
	    screen_clip.top = 0;
	    screen_clip.right = width;
	    screen_clip.bottom = height;
	//    ShowMessage("region updated to "+IntToStr(width)+", "+IntToStr(height));
	    #if SYSTEM==WINDOWS
	        updateNoClip();
	        if (locked)
	            ClipCursor(&no_clip);
	        else
	            ClipCursor(NULL);
	    #endif
	}

	void Mouse::resetRegion()
	{
	    #if SYSTEM==WINDOWS
	        setRegion(initial_clip);
	    #endif
	}

	void Mouse::setRegion(RECT region)
	{
	    mx = (region.left+region.right)/2;
	    my = (region.top+region.bottom)/2;
	    screen_clip = region;
	    #if SYSTEM==WINDOWS
	        updateNoClip();
	        if (locked)
	            ClipCursor(&no_clip);
	        else
	            ClipCursor(NULL);
	    #endif
	}
	
	void Mouse::regAnalogInputs()
	{
		map.regAnalog("MouseX",location.fx,0,1);
		map.regAnalog("MouseY",location.fy,0,1);
	}


	void Mouse::update()
	{
	    previous_location = location;
	    delta.x = 0;
	    delta.y = 0;
	    if (locked&&focus)
	    {
	        POINT mp;
	        GetCursorPos(&mp);
	        short delta_x = mp.x-mx,
	              delta_y = my-mp.y;
	        SetCursorPos(mx,my);
	        delta.x = speed[0]*delta_x/(screen_clip.right-screen_clip.left);
	        delta.y = speed[1]*delta_y/(screen_clip.bottom-screen_clip.top);
	        return;
	    }
	    GetCursorPos(&location.p);
		//ShowMessage(String(location.x)+", "+String(location.y)+" / "+String(window.left)+", "+String(window.top)+", "+String(window.right)+", "+String(window.bottom));
	    location.fx = ((float)location.x-window.left)/(window.right-window.left);
	    location.fy = (1-((float)location.y-window.top)/(window.bottom-window.top));
		
#if 0
		#if SYSTEM==WINDOWS
			if (loaded_cursor)
			{
				#if SYSTEM==WINDOWS
					SetCursor(loaded_cursor);
				#elif SYSTEM_VARIANCE==LINUX
					setCursor(loaded_cursor);
				#else
					#error stub
				#endif
			}
		#endif
#endif /*0*/	
	}

	void Mouse::freeMouse()
	{
	    if (!locked)
	        return;
	    
	    SetCursorPos(location.x,location.y);
	    
	    locked = false;
	    #if SYSTEM==WINDOWS
	        ClipCursor(NULL);
	    #endif
	}

	void Mouse::catchMouse()
	{
	    if (locked)
	        return;
	    GetCursorPos(&location.p);
	    location.fx = ((float)location.x-window.left)/(window.right-window.left);
	    location.fy = 1-((float)location.y-window.top)/(window.bottom-window.top);
	    mx = (window.right+window.left)/2;
	    my = (window.bottom+window.top)/2;
	    SetCursorPos(mx,my);
	 
	    locked = true;
	    #if SYSTEM==WINDOWS
	        ClipCursor(&no_clip);
	    #endif
	}

	void	Mouse::setFocused(bool focused)
	{
		if (focused)
			restoreFocus();
		else
			looseFocus();
	}

	bool	Mouse::isFocused()
	{
		return focus;
	}


	void Mouse::looseFocus()
	{
	    if (!focus)
	        return;
	    if (locked)
	        SetCursorPos(location.x,location.y);
		if (!cursor_visible)
		{
		    #if SYSTEM==WINDOWS
		        ShowCursor(TRUE);
		    #elif SYSTEM==UNIX
		        setCursor(None);    //None means no custom cursor thus default to standard cursor
		    #endif
		}
			
	       
	    focus = false;
	}

	void Mouse::restoreFocus()
	{
	    if (focus)
	        return;
	    GetCursorPos(&location.p);
	    location.fx = ((float)location.x-window.left)/(window.right-window.left);
	    location.fy = 1-((float)location.y-window.top)/(window.bottom-window.top);
	    if (locked)
	    {
	        #if SYSTEM==WINDOWS
	            ClipCursor(&no_clip);
	        #endif
	        SetCursorPos(mx,my);
	    }
		if (!cursor_visible)
		{
		    #if SYSTEM==WINDOWS
		        ShowCursor(FALSE);
		    #elif SYSTEM==UNIX
		        BYTE blank_cdata[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		        BYTE blank_cmask[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		        if (!blank_cursor)
		            blank_cursor = createCursor(blank_cdata,blank_cmask,8,8,0,0,screen);
		        setCursor(blank_cursor);
		    #endif
		}
	    focus = true;
	}

	bool Mouse::in(float left, float bottom, float right, float top)
	{
	    return location.fx >= left && location.fy >= bottom && location.fx <= right && location.fy <= top;
	}

	void Mouse::lock()
	{
	    if (locked)
	        return;
	    GetCursorPos(&location.p);
	    location.fx = ((float)location.x-window.left)/(window.right-window.left);
	    location.fy = 1-((float)location.y-window.top)/(window.bottom-window.top);
	    mx = location.x;
	    my = location.y;
	    locked = true;
	    #if SYSTEM==WINDOWS
	        ClipCursor(&no_clip);
	    #endif
	}

	void Mouse::unlock()
	{
	    locked = false;
	    #if SYSTEM==WINDOWS
	        ClipCursor(NULL);
	    #endif
	}

	bool Mouse::isLocked()
	{
	    return locked;
	}

	#if SYSTEM==UNIX

	bool Mouse::setCursor(Cursor cursor)
	{
	    if (!display || !cursor || !window_handle)
	    {
	    	/*if (!display)
	    		cout << "display not set"<<endl;
	    	elif (!cursor)
	    		cout << "cursor not set"<<endl;
	    	else
	    		cout << "window not set"<<endl;
	    	flush(cout);*/
	        return false;
	    }
	    //cout << "attempting to set linux cursor "<<cursor<<" on display "<<display<<" and window "<<window_handle<<endl;
	    return !XDefineCursor(display, window_handle, cursor);
	}

	void Mouse::destroyCursor(Cursor cursor)
	{
	    if (!cursor || !display)
	        return;
	    XFreeCursor(display, cursor);
	    XSync(display, False);
	}


	Cursor Mouse::createCursor(BYTE*data, BYTE*mask, int w, int h, int hot_x, int hot_y, int screen)
	{
	    if (!display)
	        return (Cursor)0;

		XGCValues GCvalues;
		GC        GCcursor;
		XImage     *data_image, *mask_image;
		Pixmap     data_pixmap, mask_pixmap;
		char       *x_data, *x_mask;
		static     XColor black = {  0,  0,  0,  0 };
		static     XColor white = { 0xffff, 0xffff, 0xffff, 0xffff };


		/* Mix the mask and the data */
		unsigned clen = (w/8)*h;
		//Array<char>	x_data(clen),	x_mask(clen);
		alloc(x_data, clen);
		alloc(x_mask, clen);
		
		for (unsigned i = 0; i < clen; i++)
	    {
			/* The mask is OR'd with the data to turn inverted color
			   pixels black since inverted color cursors aren't supported
			   under X11.
			 */
			x_mask[i] = data[i] | mask[i];
			x_data[i] = data[i];
		}

		/* Create the data image */
		data_image = XCreateImage(display,DefaultVisual(display, screen), 1, XYBitmap, 0, x_data, w, h, 8, w/8);
		data_image->byte_order = MSBFirst;
		data_image->bitmap_bit_order = MSBFirst;
		data_pixmap = XCreatePixmap(display, window_handle, w, h, 1);

		/* Create the data mask */
		mask_image = XCreateImage(display,DefaultVisual(display, screen),1, XYBitmap, 0, x_mask, w, h, 8, w/8);
		mask_image->byte_order = MSBFirst;
		mask_image->bitmap_bit_order = MSBFirst;
		mask_pixmap = XCreatePixmap(display, window_handle, w, h, 1);

		/* Create the graphics context */
		GCvalues.function = GXcopy;
		GCvalues.foreground = ~0;
		GCvalues.background =  0;
		GCvalues.plane_mask = AllPlanes;
		GCcursor = XCreateGC(display, data_pixmap,
				(GCFunction|GCForeground|GCBackground|GCPlaneMask),
									&GCvalues);

		/* Blit the images to the pixmaps */
		XPutImage(display, data_pixmap, GCcursor, data_image,
								0, 0, 0, 0, w, h);
		XPutImage(display, mask_pixmap, GCcursor, mask_image,
								0, 0, 0, 0, w, h);
		XFreeGC(display, GCcursor);

		/* These free the x_data and x_mask memory pointers */
		XDestroyImage(data_image);
		XDestroyImage(mask_image);

		/* Create the cursor */
		Cursor result = XCreatePixmapCursor(display, data_pixmap,
					mask_pixmap, &black, &white, hot_x, hot_y);
		XFreePixmap(display, data_pixmap);
		XFreePixmap(display, mask_pixmap);
	    XSync(display, False);
	    
	    //deloc(x_data);	//delocating this memory section causes a segmentation fault. i believe it is automatically delocated in the above functions.
	    //deloc(x_mask);

		return result;
	}

	//inv: 81 63 55 70
	//bas: 75 51 41 60
	#endif
	
	bool	Mouse::cursorIsNotDefault()	const
	{
		return (loaded_cursor != cursor_reference[CursorType::Default]);
	}

	void	Mouse::setCursor(eCursor cursor)
	{
		if (cursor == CursorType::None || cursor >= CursorType::Count)
			return;
		if (cursor != CursorType::Hidden && !cursor_visible)
			showCursor(false);
		if (cursor == CursorType::Hidden)
		{
			hideCursor(false);
			return;
		}
		if (!cursor_reference[CursorType::EditText])
			loadCursors();
		
		if (loaded_cursor == cursor_reference[cursor])
			return;
		
		if (cursor_visible)
		{
			#if SYSTEM==WINDOWS
				SetCursor(cursor_reference[cursor]);
			#elif SYSTEM_VARIANCE==LINUX
				//cout << "setting cursor "<<cursorToString(cursor)<<endl;
				setCursor(cursor_reference[cursor]);	//for some reason this may fail occasionally, during startup at least. it does work most of the time though
				//ASSERT2__(setCursor(cursor_reference[cursor]),cursorToString(cursor),cursor_reference[cursor]); //so, no assertion here
			#else
				#error stub
			#endif
			loaded_cursor = cursor_reference[cursor];
		}
	}
	

	bool Mouse::hideCursor(bool force)
	{
		force_invisible	= force;
		if (!cursor_visible)
			return true;
		cursor_visible = false;
		if (!focus)
			return true;
	    #if SYSTEM==WINDOWS
	        ShowCursor(FALSE);
	        return true;
	    #elif SYSTEM==UNIX
	        BYTE blank_cdata[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	        BYTE blank_cmask[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	        if (!blank_cursor)
	            blank_cursor = createCursor(blank_cdata,blank_cmask,8,8,0,0,screen);
	        return setCursor(blank_cursor);
	    #endif
	}

	void Mouse::showCursor(bool override)
	{
		if (force_invisible && !override)
			return;
		force_invisible = false;
		if (cursor_visible)
			return;
		cursor_visible = true;
		if (!focus)
			return;

	    #if SYSTEM==WINDOWS
	        ShowCursor(TRUE);
	    #elif SYSTEM==UNIX
	        setCursor(None);    //None means no custom cursor thus default to standard cursor
	    #endif
	}



	Mouse mouse(input);
}
