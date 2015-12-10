#include "../../global_root.h"
#include "mw_console.h"


//static ConsoleWindow*	focused_console(NULL);
//static Mutex			focus_mutex;


void Engine::ConsoleWindow::onChar(char c)
{
	MutexLock	lock(mutex);
	
	if (input.pressed[Key::Ctrl] && !input.pressed[Key::Alt])	//somehow Ctrl is issued when Alt is pressed. how odd...
		return;
	//ShowMessage("hello");
	if (!busy /*&& keyboard.GetInputLen()<254*/)
	{
		input_history_position = input_history.count();
		if (sel_start < cursor)
		{
			keyboard.ReplaceInputSection(sel_start,cursor-sel_start,c);
			cursor = sel_start+1;
		}
		else
		{
			keyboard.ReplaceInputSection(cursor,sel_start-cursor,c);
			cursor++;
		}
		sel_start = cursor;
		SignalWindowContentChange();
	}
}

void Engine::ConsoleWindow::onKeyDown(Key::Name key)
{
	if (busy)
		return;
	MutexLock	lock(mutex);
	switch (key)
	{
		case Key::LeftMouseButton:
			{
				int bar_upper = clientHeight()-spacing-20,
					bar_lower = spacing+20,
					bar_height = bar_upper-bar_lower;
				float	sector_lower = (float)retro_perspective/(float)history,
						sector_upper = clamped(sector_lower+(float)visible_lines/(float)history,0,1);
				int		ilower = bar_lower + (int)(sector_lower*(float)bar_height),
						iupper = bar_lower + (int)(sector_upper*(float)bar_height);

				int rx = mouse.location.absolute.x-originX()-clientOffsetX(),
					ry = clientHeight() - (mouse.location.absolute.y-originY()-clientOffsetY());
				if (rx > (int)clientWidth()-spacing-15
					&&
					rx < (int)clientWidth()-spacing
					&&
					ry > ilower-10
					&&
					ry < iupper+10)
				{
					slider_grabbed = true;
					grab_delta = ry-ilower;
				}
			}
		break;
		case Key::Backspace:
			if (cursor)
			{
				input_history_position = input_history.count();
				keyboard.ReplaceInputSection(--cursor,1,"");
				sel_start = cursor;
				SignalWindowContentChange();
			}
		break;
		case Key::Up:
			if (input_history_position>0)
			{
				input_history_position--;
				
				fillLine(input_history[input_history_position]);
				SignalWindowContentChange();
			}
		break;
		case Key::Down:
			if (input_history_position < input_history.count())
			{
				input_history_position++;
				fillLine(input_history[input_history_position]);
				SignalWindowContentChange();
			}
		break;
		case Key::Left:
			if (cursor)
			{
				cursor--;
				if (!input.pressed[Key::Shift])
					sel_start = cursor;
				SignalWindowContentChange();
			}
		break;
		case Key::Right:
			if (cursor < keyboard.GetInputLen())
			{
				cursor++;
				if (!input.pressed[Key::Shift])
					sel_start = cursor;
				SignalWindowContentChange();
			}
		break;
		case Key::Enter:
		case Key::Return:
		{
			bool command_line = show_command_line;
			show_command_line = false;

			const char*input = keyboard.GetInput();
			if (echo)
				history << context+"> "+input;

			last_check = timer.now();
			busy = true;

			if (input_history_position < input_history.count())
				input_history.erase(input_history_position);

			input_history << input;
			input_history_position = input_history.count();


			onInput(input);
			
			busy = false;

			keyboard.FlushInput();
			cursor = 0;
			sel_start = 0;
			show_command_line = command_line;
			show_progress_bar = show_progress_bar && !command_line;
			SignalWindowContentChange();
		}
		break;
		case Key::Del:
			input_history_position = input_history.count();

			if (sel_start == cursor)
				keyboard.ReplaceInputSection(cursor,1,"");
			else
			{
				if (sel_start < cursor)
				{
					keyboard.ReplaceInputSection(sel_start,cursor-sel_start,"");
					cursor = sel_start;
				}
				else
					keyboard.ReplaceInputSection(cursor,sel_start-cursor,"");
				sel_start = cursor;
			}
			SignalWindowContentChange();
		break;
		case Key::PageUp:
			retro_perspective+=visible_lines;
			SignalWindowContentChange();
		break;
		case Key::PageDown:
			if (retro_perspective >= visible_lines)
				retro_perspective-=visible_lines;
			else
				retro_perspective = 0;
			SignalWindowContentChange();
		break;
		case Key::Home:
			cursor = 0;
			if (!input.pressed[Key::Shift])
				sel_start = cursor;
			SignalWindowContentChange();
		break;
		case Key::End:
			cursor = keyboard.GetInputLen();
			if (!input.pressed[Key::Shift])
				sel_start = cursor;
			SignalWindowContentChange();
		break;
		case Key::C:
			if (input.pressed[Key::Ctrl])
			{
				copyToClipboard();
			}
		break;
		case Key::X:
			if (input.pressed[Key::Ctrl])
			{
				if (sel_start != cursor)
				{
					copyToClipboard();
					input_history_position = input_history.count();

					if (sel_start < cursor)
					{
						keyboard.ReplaceInputSection(sel_start,cursor-sel_start,"");
						cursor = sel_start;
					}
					else
						keyboard.ReplaceInputSection(cursor,sel_start-cursor,"");
					sel_start = cursor;
					SignalWindowContentChange();
				}
			}
		break;
		case Key::V:
			if (input.pressed[Key::Ctrl])
			{
				input_history_position = input_history.count();
				char buffer[0x100];
				if (System::getFromClipboardIfText(getWindow(),buffer,sizeof(buffer)))
				{
					if (sel_start < cursor)
					{
						keyboard.ReplaceInputSection(sel_start,cursor-sel_start,buffer);
						cursor = sel_start+strlen(buffer);
					}
					else
					{
						keyboard.ReplaceInputSection(cursor,sel_start-cursor,buffer);
						cursor+=strlen(buffer);
					}
					sel_start = cursor;
					SignalWindowContentChange();
				}
			}
		break;
	}

}

void	Engine::ConsoleWindow::copyToClipboard()
{
	index_t begin(sel_start),end(cursor);
	if (end < begin)
		swp(end,begin);
	String line = keyboard.GetInput()+begin;
	line.erase(end-begin);
	if (!System::copyToClipboard(getWindow(),line.c_str()))
		ErrMessage("copy failed :S");
}








void Engine::ConsoleWindow::onMouseMove()
{
	if (busy || !slider_grabbed)
		return;
	MutexLock	lock(mutex);

	int bar_upper = clientHeight()-spacing-20,
		bar_lower = spacing+20,
		bar_height = bar_upper-bar_lower;
	float	sector_lower = (float)retro_perspective/(float)history,
			sector_upper = clamped(sector_lower+(float)visible_lines/(float)history,0,1);
	int		ilower = bar_lower + (int)(sector_lower*(float)bar_height),
			iupper = bar_lower + (int)(sector_upper*(float)bar_height);

	int ry = clientHeight() - (mouse.location.absolute.y-originY()-clientOffsetY()),
		new_ilower = ry-grab_delta,
		delta = (int)((float)(new_ilower - ilower)*(float)history/(float)bar_height);
	if (delta > 0)
		retro_perspective += delta;
	else
		if (-delta > (int)retro_perspective)
			retro_perspective = 0;
		else
			retro_perspective += delta;

	SignalWindowContentChange();
}

void Engine::ConsoleWindow::onKeyUp(Key::Name key)
{
	if (key != Key::LeftMouseButton || busy || !slider_grabbed)
		return;
	slider_grabbed = false;
	SignalWindowContentChange();
}

void Engine::ConsoleWindow::onMouseWheel(short delta)
{
	if (busy || slider_grabbed)
		return;

	MutexLock	lock(mutex);

	if (delta > 0)
		retro_perspective+=delta/100;
	else
	{
		USHORT neg = -delta/100;
		if (neg <= retro_perspective)
			retro_perspective -= neg;
		else
			retro_perspective = 0;
	}
	SignalWindowContentChange();
}



void Engine::ConsoleWindow::onCreate()
{
	glClearColor(1,1,1,0.0);
}


void	Engine::ConsoleWindow::write(int x, int y, const String&text)
{
	textout.color(0.3,0.3,0.3);
	textout.locate(x,y);
	textout.print(text);
}

void	Engine::ConsoleWindow::onPaint()
{
	MutexLock	lock(mutex);
	
	double now = timer.toSecondsd(timer.now());
	unsigned	width = clientWidth(),
				height = clientHeight();
	aspect.UpdateProjection(	0,0,		//create the internal overlay (hud) projection matrix using 0,0 as lower left corner
				width,height,		//1,1 as upper right corner
				-1,			//-1 as near z plane
				1);			//1 as far z plane
	aspect.UpdateView();
	//self->pick(self->aspect);
	glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glLoadMatrixf(aspect.projection.v);
        //glOrtho(-1,1,-1,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLoadMatrixf(aspect.view.v);
//	ShowMessage(__toString(self->aspect.projection)+"\n/\n"+__toString(self->aspect.modelview)+"\n\n"+EveOpenGL::renderState());
		glDisable(GL_DEPTH_TEST);


		if (!background.IsEmpty())
		{
			display.useTexture(background);

			glWhite(0.15);
			glBegin(GL_QUADS);
				glTexCoord2f(1,0); glVertex2i(width,0);
				glTexCoord2f(1,1); glVertex2i(width,512);
				glTexCoord2f(0,1); glVertex2i((int)width-512,512);
				glTexCoord2f(0,0); glVertex2i((int)width-512,0);
			glEnd();
			display.useTexture(NULL);
		}


		glGrey(0.3);

		glPushMatrix();
			glTranslatef(-0.5f,-0.5f,0.0f);
			glBegin(GL_LINE_STRIP);
				glVertex2i(spacing,height-19);
				glVertex2i(spacing+54,height-19);
				glVertex2i(spacing+54,height-spacing);
				glVertex2i(width-spacing,height-spacing);
				glVertex2i(width-spacing,spacing);
				glVertex2i(spacing,spacing);
				glVertex2i(spacing,height-19);
			glEnd();
		glPopMatrix();

		write(spacing,height-ConsoleWindow::font_height+2,"history");





		int			lines = ((int)height-(28+spacing))/ConsoleWindow::font_height-1;

		visible_lines = lines;

		if (retro_perspective+lines > history)
			if (history >= (unsigned)lines)
				retro_perspective = history-lines;
			else
				retro_perspective = 0;

		int			start = int(history > (index_t)lines?history-lines-retro_perspective:0),
					end = int(start+ vmin(history-start,lines)),
					shown_lines = int(history-start),
					upper_border = start>0?(lines+3)*ConsoleWindow::font_height+spacing:height;
		size_t		input_len = keyboard.GetInputLen();

		bool		in_string = cursor < input_len;

		const char*	input = keyboard.GetInput();

		String		context = this->context+"> ";

		int			offset = (int)textout.unscaledLength(context),
					char_len = (int)textout.unscaledLength(in_string?input[cursor]:'X'),
					cursor_x = spacing+4+offset+(int)textout.unscaledLength(input,cursor)+2*!in_string,
					cursor_y = upper_border-19-ConsoleWindow::font_height-(int)(end-start)*ConsoleWindow::font_height,
					cursor_upper = cursor_y-1,
					cursor_lower = cursor_y-5,
					sel_start_x = spacing+4+offset+(int)textout.unscaledLength(input,sel_start);


		if (cursor != sel_start)
		{
			glGrey(0.6,0.5);
			glBegin(GL_QUADS);
				glVertex2i(sel_start_x,cursor_upper);
				glVertex2i(cursor_x,cursor_upper);
				glVertex2i(cursor_x,cursor_upper+ConsoleWindow::font_height);
				glVertex2i(sel_start_x,cursor_upper+ConsoleWindow::font_height);
			glEnd();
		}

		//scroll indicator
		if (history)
		{
			int bar_upper = height-spacing-20,
				bar_lower = spacing+20,
				bar_height = bar_upper-bar_lower;
			glGrey(0.7,vmin(1,(float)history/(float)lines));
			glBegin(GL_LINES);
				glVertex2f(width-spacing-8.5,bar_upper);
				glVertex2f(width-spacing-8.5,bar_lower);
			glEnd();
			if (slider_grabbed)
				glColor3f(0.65,0.75,1.0);
			//glGrey(0.8);
			float	sector_lower = (float)retro_perspective/(float)history,
					sector_upper = clamped(sector_lower+(float)lines/(float)history,0,1);
			int		ilower = bar_lower + (int)(sector_lower*(float)bar_height),
					iupper = bar_lower + (int)(sector_upper*(float)bar_height);
			glBegin(GL_QUADS);
				glVertex2f(width-spacing-9.5,ilower);
				glVertex2f(width-spacing-6.5,ilower);
				glVertex2f(width-spacing-6.5,iupper);
				glVertex2f(width-spacing-9.5,iupper);
			glEnd();
		}


		for (int i = start; i < end; i++)
			if (i >= 0)
				write(spacing+4,upper_border-19-ConsoleWindow::font_height-(int)(i-start)*ConsoleWindow::font_height,history[i]);

		if (lines >= 0 && history && show_progress_bar)
		{
			int	progress_start = spacing+4+(int)textout.unscaledLength(history.last())+10,
				progress_end = (int)width-spacing-20,
				progress_lower = upper_border-19-ConsoleWindow::font_height-(int)(history-start-1)*ConsoleWindow::font_height-2,
				progress_upper = progress_lower + ConsoleWindow::font_height-4;

			if (progress_start < progress_end)
			{
				glGrey(0.6,0.2);
				glBegin(GL_QUADS);
					glVertex2i(progress_start,progress_lower);
					glVertex2i(progress_end,progress_lower);
					glVertex2i(progress_end,progress_upper);
					glVertex2i(progress_start,progress_upper);
					glColor4f(0.55,0.6,0.7,0.45);
					glVertex2i(progress_start,progress_lower);
					glVertex2i(progress_start+(int)(progress*(progress_end-progress_start)),progress_lower);
					glVertex2i(progress_start+(int)(progress*(progress_end-progress_start)),progress_upper);
					glVertex2i(progress_start,progress_upper);
				glEnd();

				glGrey(0.6);
				glBegin(GL_LINE_LOOP);
					glVertex2i(progress_start,progress_lower);
					glVertex2i(progress_end,progress_lower);
					glVertex2i(progress_end,progress_upper);
					glVertex2i(progress_start,progress_upper);
				glEnd();

			}
		}


		if (lines >= 0 && show_command_line)
		{
			write(spacing+4,cursor_y,context);
			write(spacing+4+offset,cursor_y,input);
			if (isFocused())
			{
				glBegin(GL_QUADS);
					glGrey(0.5);
					glVertex2i(cursor_x+char_len,cursor_upper);
					glVertex2i(cursor_x,cursor_upper);
					glGrey(0.5,0.4);
					glVertex2i(cursor_x,cursor_lower);
					glVertex2i(cursor_x+char_len,cursor_lower);
				glEnd();
			}
		}


}



void	Engine::ConsoleWindow::init()
{
	echo = true;
	show_command_line = true;
	show_progress_bar = false;
	slider_grabbed = false;
	busy = false;
	input_handler = NULL;
	destroy_handler = NULL;
	progress = 0;
	cursor = 0;

	input_history_position=0;
	retro_perspective = 0;	//history up scrolling
	visible_lines = 10;

	input_handler = NULL;

	aspect.UpdateProjection(	0,0,		//create the internal overlay (hud) projection matrix using 0,0 as lower left corner
				Window::clientWidth(),Window::clientHeight(),		//1,1 as upper right corner
				-1,			//-1 as near z plane
				1);			//1 as far z plane
	aspect.UpdateView();


	context = "console";
}



void		Engine::ConsoleWindow::bindInputEvent(void (*input)(ConsoleWindow*, const String&))
{
	input_handler = input;
}

void		Engine::ConsoleWindow::bindDestroyEvent(void (*destroy)(ConsoleWindow*))
{
	destroy_handler = destroy;
}



void		Engine::ConsoleWindow::onDestroy()
{
	if (destroy_handler)
		destroy_handler(this);
}


static Engine::Window::Font*font;

static float lengthFunction(char c)
{
	return font->getWidth(&c,1);
}

void		Engine::ConsoleWindow::print(const String&line)
{
	MutexLock	lock(mutex);

	font = &textout.getFont();
	retro_perspective = 0;
	Array<String>	exploded,wrapped;
	explode('\n',line,exploded);
	for (index_t i = 0; i < exploded.count(); i++)
	{
		wrapf(exploded[i].trimRight(),clientWidth()-(spacing*2+14),lengthFunction,wrapped);
		for (index_t j = 0; j < wrapped.count(); j++)
			history << wrapped[j];
	}
	SignalWindowContentChange();
}

void		Engine::ConsoleWindow::printDirect(const String&line)
{
	MutexLock	lock(mutex);

	retro_perspective = 0;
	Timer::Time t = timer.now();
	//Window::ContextHandle h = Window::getCurrentContext();
	
	if (timer.toSecondsf(t-last_check)>0.1f)
	{
		application.InterruptCheckEvents();
		//ASSERT_EQUAL__(wglGetCurrentContext(),h.gl_context);
		last_check = t;
	}
	print(line);
		//ASSERT_EQUAL__(wglGetCurrentContext(),h.gl_context);
	update();
		//ASSERT_EQUAL__(wglGetCurrentContext(),h.gl_context);
}

void		Engine::ConsoleWindow::clearHistory()
{
	MutexLock	lock(mutex);

	retro_perspective = 0;
	history.clear();
	SignalWindowContentChange();
}

StringList*Engine::ConsoleWindow::getHistory()
{
	return &history;
}

void Engine::ConsoleWindow::setBackground(const OpenGL::Texture&texture)
{
	background = &texture;
}

void Engine::ConsoleWindow::setBackground(const OpenGL::Texture*texture)
{
	background = texture;
}

void Engine::ConsoleWindow::echoOff()
{
	echo = false;
}

void Engine::ConsoleWindow::echoOn()
{
	echo = true;
}

void	Engine::ConsoleWindow::showCommandLine()
{
	show_command_line = true;
}

void	Engine::ConsoleWindow::showCommandLine(bool b)
{
	show_command_line = b;
}

void	Engine::ConsoleWindow::hideCommandLine()
{
	show_command_line = false;
}

void	Engine::ConsoleWindow::setProgress(float progress_)
{
	retro_perspective = 0;
	progress = progress_;
	show_progress_bar = !show_command_line;

	Timer::Time t = timer.now();
	if (timer.toSecondsf(t-last_check)>0.1f)
	{
		application.InterruptCheckEvents();
		last_check = t;
	}

	if (show_progress_bar)
		update();
}

void	Engine::ConsoleWindow::showProgress()
{
	retro_perspective = 0;
	show_progress_bar = true;
	SignalWindowContentChange();
}

void	Engine::ConsoleWindow::showProgress(bool b)
{
	retro_perspective = 0;
	show_progress_bar = b;
	SignalWindowContentChange();
}

void	Engine::ConsoleWindow::hideProgress()
{
	retro_perspective = 0;
	show_progress_bar = false;
	SignalWindowContentChange();
}


void Engine::ConsoleWindow::setContext(const String&context_)
{
	MutexLock	lock(mutex);

	context = context_;
	SignalWindowContentChange();
}

void Engine::ConsoleWindow::fillLine(const String&line)
{
	MutexLock	lock(mutex);

	keyboard.FillInput(line);
	sel_start = cursor = line.length();
	SignalWindowContentChange();
}

