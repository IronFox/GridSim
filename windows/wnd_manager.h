#ifndef wnd_managerH
#define wnd_managerH


/******************************************************************

E:\include\windows\wnd_manager.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

#if SYSTEM==WINDOWS

#include <windows.h>
#include <windowsx.h>
#include "../global_string.h"
#include "../container/binarytree.h"


/******************************************************************************************

wnd_manager.h

This file is part of the DeltaWorks-Foundation or an immediate derivative core application.
Original Author: Stefan Elsen
Copyright (C) 2006-2007 University of Trier, Computer Science Department, Germany.


******************************************************************************************/

#define WINDOW_NONAME	""

typedef void (*WndPainter)(HDC);

class Window
{
		HINSTANCE	instance;
		WndPainter	update;
		RECT		position;
		Window		*parent;
		String		application_name,
					caption;
friend	class	WindowPool;

public:
		HWND		ident;
		String		name;


					Window(const String&ApplicationName, const String&Caption, const String&Name, HINSTANCE Instance, Window*Parent, WndPainter Update, unsigned left, unsigned top, unsigned width, unsigned height);
					~Window();
		void		redraw();
		void		redraw(const RECT&rect);
		RECT		getRect();
		void		setVisible();
		void		setInvisible();
};

class WindowPool:public List::BinaryTree<Window,IdentSort>
{
private:
		String		application_name;
		List::ReferenceBinaryTree<Window,NameSort>	finder;
		List::BinaryTree<Window,NameSort>		destroyed;
		HINSTANCE	instance;
public:
					WindowPool();
		Window*	create(const String&Caption, const String&Name,Window*Parent, WndPainter Update, unsigned left, unsigned top, unsigned width, unsigned height);
		Window*	create(const String&Caption, const String&Name,WndPainter Update, unsigned left, unsigned top, unsigned width, unsigned height);
		bool		destroy(HWND window);
		void		paint(HWND window);
		void		setApplicationName(const String&name);
		void		setInstance(HINSTANCE hInstance);
		bool		setVisible(const String&window_name);
		bool		setInvisible(const String&window_name);
		bool		redraw(const String&window_name);
		void		update(HWND window);
};

#endif
#endif
