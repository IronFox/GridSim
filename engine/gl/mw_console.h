#ifndef engine_mw_consoleH
#define engine_mw_consoleH

#include "multi_window.h"

namespace Engine
{


	class ConsoleWindow:public Window
	{
	protected:
	static	const int					font_height = 16;
	static	const int					spacing = 4;		//window border spacing

			Mutex						mutex;
			
			int							grab_delta;
			StringList					history,
										input_history;
			index_t						input_history_position,
										retro_perspective,
										visible_lines;
			OrthographicAspect<>		aspect;
			bool						busy,	//executing command
										echo,	//echo inputs to the history
										show_command_line,
										show_progress_bar,
										slider_grabbed;	//mouse pointer grabbed history slider
			float						progress;
			index_t						cursor,
										sel_start;
			String						context;
			OpenGL::Texture::Reference	background;
			Timer::Time					last_check;

			void						(*destroy_handler)(ConsoleWindow*);
			void						(*input_handler)(ConsoleWindow*,const String&);

			void						init();
			void						write(int x, int y, const String&text);
			void						copyToClipboard();
			
	public:

	virtual	void			onInput(const String&line)	
							{
								if (input_handler)
									input_handler(this,line);
							};
	

	virtual	void			onChar(char c);
	virtual	void			onKeyDown(Key::Name key);
	virtual	void			onKeyUp(Key::Name key);
	
	virtual	void			onMouseWheel(short);
	virtual	void			onMouseMove();
	virtual	void			onDestroy();

	virtual	void			onCreate();
	virtual	void			onPaint();

							ConsoleWindow()	{init();}
	virtual					~ConsoleWindow()	{}

			void			setBackground(const OpenGL::Texture&texture);
			void			setBackground(const OpenGL::Texture*texture);
			void			print(const String&line);
			void			printDirect(const String&line);
			void			echoOff();
			void			echoOn();
			void			showCommandLine();
			void			showCommandLine(bool);
			void			hideCommandLine();
			void			setContext(const String&context);
			void			setProgress(float progress);
			void			showProgress();
			void			showProgress(bool);
			void			hideProgress();
			void			clearHistory();
			void			fillLine(const String&line);
			StringList*		getHistory();
			void			bindInputEvent(void (*input)(ConsoleWindow*, const String&));
			void			bindDestroyEvent(void (*destroy)(ConsoleWindow*));
	};
}

#endif

