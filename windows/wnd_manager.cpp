#include "../global_root.h"
#include "wnd_manager.h"

/*************************************************************************************

wnd_manager.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

*************************************************************************************/

#if SYSTEM==WINDOWS

Window::Window(const String&ApplicationName, const String&Caption, const String&Name, HINSTANCE Instance, Window*Parent, WndPainter Update, unsigned left, unsigned top, unsigned width, unsigned height):
          instance(Instance),update(Update),parent(Parent),application_name(ApplicationName),caption(Caption),name(Name)
{
    position.left = left;
    position.top = top;
    position.right = left+width;
    position.bottom = top+height;
    ident = CreateWindowA(application_name.c_str(),
                          caption.c_str(),
                          WS_OVERLAPPEDWINDOW,
                          left,
                          top,
                          width,
                          height,
                          parent?parent->ident:NULL,
                          NULL,
                          instance,
                          NULL);
    if (!parent)
    	ShowWindow(ident, SW_NORMAL);
	UpdateWindow(ident);
}

Window::~Window()
{
    if (ident)
        CloseWindow(ident);
}

void Window::redraw()
{
    RedrawWindow(ident,NULL,NULL,RDW_INVALIDATE);
}

void Window::redraw(const RECT&rect)
{
    RedrawWindow(ident,&rect,NULL,RDW_INVALIDATE);
}

RECT Window::getRect()
{
    GetClientRect(ident,&position);
    return position;
}


void Window::setVisible()
{
    if (!ident) //window destroyed?
        ident = CreateWindowA(application_name.c_str(),
                              caption.c_str(),
                              WS_OVERLAPPEDWINDOW,
                              position.left,
                              position.top,
                              position.right-position.left,
                              position.bottom-position.top,
                              parent?parent->ident:NULL,
                              NULL,
                              instance,
                              NULL); //recreate
    ShowWindow(ident,SW_NORMAL);
}

void Window::setInvisible()
{
    if (ident)
        ShowWindow(ident,SW_HIDE);
}


WindowPool::WindowPool():application_name("unnamend application")
{}

Window*WindowPool::create(const String&Caption, const String&Name,Window*Parent, WndPainter Update, unsigned left, unsigned top, unsigned width, unsigned height)
{
    Window*window = new Window(application_name,Caption,Name,instance,Parent,Update,left,top,width,height);
    insert(window);
    finder.insert(window);
    return window;
}

Window*WindowPool::create(const String&Caption, const String&Name,WndPainter Update, unsigned left, unsigned top, unsigned width, unsigned height)
{
    Window*window = new Window(application_name,Caption,Name,instance,NULL,Update,left,top,width,height);
    insert(window);
    finder.insert(window);
    return window;
}

bool WindowPool::destroy(HWND window)
{
    Window*wnd = dropByIdent(window);
    if (!wnd)
        return true;
    finder.drop(wnd);
    wnd->ident = NULL;
    destroyed.insert(wnd);
    return wnd->parent!=NULL;
}

void WindowPool::paint(HWND window)
{
    PAINTSTRUCT pstruct;
    HDC hdc;
    Window*wnd = lookup(window);
    if (wnd)
    {
        hdc = BeginPaint(window,&pstruct ); /* prepare window for painting*/
        wnd->update(hdc);
        EndPaint(window,&pstruct);
    }
}

void WindowPool::setApplicationName(const String&name)
{
    application_name = name;
}

void WindowPool::setInstance(HINSTANCE hInstance)
{
    instance = hInstance;
}

bool WindowPool::redraw(const String&window_name)
{
    Window*window = finder.lookup(window_name);
    if (!window)
        return false;

    window->redraw();
    return true;
}


bool WindowPool::setVisible(const String&window_name)
{
    Window*window = finder.lookup(window_name);
    if (window)
    {
        window->setVisible();
        return true;
    }
    window = destroyed.dropByIdent(window_name);
    if (!window)
        return false;
    window->setVisible();
    insert(window);
    finder.insert(window);
    return true;
}

bool WindowPool::setInvisible(const String&window_name)
{
    Window*window = finder.lookup(window_name);
    if (!window)
        return false;

    window->setInvisible();
    return true;
}
#endif
