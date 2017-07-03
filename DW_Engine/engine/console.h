#ifndef engine_consoleH
#define engine_consoleH

/******************************************************************

Language/Font-independed console.

******************************************************************/


#include "interface/keyboard.h"
#include "renderer/renderer.h"
#include "textout.h"
#include "timing.h"
#include "../geometry/simple_geometry.h"


#define EVE_PANEL_SIZE		0.05f
#define EVE_INNER_SIZE		(EVE_PANEL_SIZE*0.8f)
#define EVE_OUTER_SIZE		(EVE_PANEL_SIZE-EVE_INNER_SIZE)


//enum eConsoleAction{CA_NONE,CA_OPEN,CA_CLOSE,CA_TOGGLE,CA_FOCUS};
namespace Engine
{

	/*!
		\brief Non visual console root class
		
		Console logs console outputs to a history, manages keyboard events and keeps track of all other non-visual aspects of a console.

	*/
	class Console
	{
	public:
		enum Action		//! Type of action directly resulting from a user mouse click
		{
			NoAction,	//!< No action necessary
			Open,		//!< Open/show console
			Close,		//!< Close/hide console
			Toggle,		//!< Toggle console visibility
			Focus,		//!< Focus console
			Unfocus		//!< Unfocus console
		};
		typedef void (*pInputParser)(const char*input);

	protected:
		Ctr::StringList		lines,	//!< Console history
							inputs;	//!< User input history
		InputProfile		profile;			//!< Keyboard profile which is active while the console is focused
		unsigned			selected_input;		//!< Currently selected input history entry
		pInputParser		parser;				//!< Input parser
		float				status;				//!< Open/close status (0=fully closed, 1=fully open)
		bool				is_focused,			//!< True if the console is currently focused
							target_open;		//!< True if the console is animating towards an open state, false if towards a closed state

			
//	static	Console*			target;				//!< Currently focused console (global)
		void				fetchInput();	//!< Invoked when the user presses the return key, executing the current input (if any)
		void				prevInput();	//!< Invoked when the user presses the up key to query the previous entry in the input history
		void				nextInput();	//!< Invoked when the user presses the down key to query the next entry in the input history
		void				back();			//!< Invoked when the user presses the backspace key to erase the last character from the input
		void 				closeEvent();	//!< Invoked when the user pressed the console close key


	public:
		enum				eConsoleSection		//! Graphical console section flags
		{
			Head=1,				//!< Target is console head
			Body=2,				//!< Target is console body
			Foot=4				//!< Target is console foot
		};

							Console();
		virtual				~Console();
		void				print(const String&line);	//!< Appends a new line to the console history
		void				emptyLine();				//!< Appends an empty line to the console history
		void				focus();					//!< Focuses the console
		void				killFocus();				//!< Unfocuses the console
		void				unfocus();					//!< Identical to killFocus()
		void				forgetLastInput();			//!< Drops the last element of the console history
		void				clearHistory();				//!< Drops all elements from the console history
		virtual	void		setState(bool open);		//!< Sets the current console open status. The console is directly displayed in the specified status with no animation taking place
		void				setTarget(bool open);		//!< Sets the animation target of the console.
		void				open();								//!< Identical to setTarget(true)
		void				close();							//!< Identical to setTarget(false)
		bool				isOpen()					const;	//!< True if the console is currently open or opening
		void				toggle();							//!< Toggles the console animation target (opening -> closing and vice versa)
		bool				focused()					const;	//!< True if the console is currently open or opening and focused
			
			
		void				bindCloseKey(Key::Name key);		//!< Binds the specified key to the console close event. Whenever the user presses the specified key while the console is focused, the console closes automatically.

		void				setInterpretor(pInputParser);		//!< Redefines the console input interpretor
	};


	template <class GL, class Font>
		class VisualConsole:public Console		//! Visual enhancement of Console
		{
		protected:
			typedef typename GL::TextureObject	Texture;
			GL									*renderer;	//!< Active renderer
				
			/*!
				\brief Rectangular visual section of the console
					
				VisualConsole consists of three such: one for the head, one for the body and one for the foot.
				Each of these may have its own background texture and be animated in a different way.
				Also the console history may be attached to any one of these sectors.
			*/
			struct	Section
			{
				TFloatRect		relative,			//!< Relative console base region (relative to the parent console location and size)
								absolute,			//!< Absolute base screen region of this section
								current;			//!< Current (animated) absolute screen region of this section
				M::TVec2<>			trans0,				//!< Primary translation of this section (during the first half of the animation)
								trans1,				//!< Secondary translation of this section (during the second half of the animation) in addition to the primary translation
								translation;		//!< Effective current (animated) translation
				Texture			primary,			//!< Background texture of this section (if any)
								shiny;				//!< Animated shiny texture of this section (if any)
					
								Section();
				void			setPrimaryTranslation(float x, float y);	//!< Updates the (negative) primary translation of this section
				void			setSecondaryTranslation(float x, float y);	//!< Updates the (negative) secondary translation of this section
				void			updateState(float state);					//!< Updates \b current and \b translation based on the specified state scalar (0-1)
				void			updateAbsolutePosition(const TFloatRect&parentLocation);	//!< Updates absolute base position relative the specified parent console location
				void			render(GL*renderer);						//!< Renders this section using the specified renderer
			};

			Textout<Font>		textout;							//!< Console textout. Must be initialized externally to work
			TFloatRect			location;
			float				//x,									//!< Left border of the console
								//y,									//!< Bottom border of the console
								//width,								//!< Width of the console
								//height,								//!< Height of the console
								font_x,								//!< Font width
								font_y;								//!< Font height
			Section				head,								//!< Header section
								body,								//!< Body section
								foot;								//!< Footer section
			unsigned			history_state;						//!< History scroll position
			TFloatRect			relative_text,						//!< History render position relative to the text target section
								absolute_text;						//!< Current absolute history frame
			Section				*text_target;						//!< Target section to adjust the visual history to (usually the body)
				
			void				updateSectionPositions();			//!< Updates both the absolute positions and states of all sections as well as the absolute text position
			String				Truncate(const String&origin);		//!< Truncates the given string so that it fits into the absolute history frame
			void				printLine(const String&line);		//!< Adds the specified line (wrapped) to the console history
			Section&			getSec(eConsoleSection ident)		//!< Retrieves a section via a section identifier
								{
									switch (ident)
									{
										case Head:
											return head;
										case Body:
											return body;
										case Foot:
											return foot;
									}
									FATAL__("illegal section-ident");
								}
				


		public:

								VisualConsole();
								VisualConsole(GL*renderer);
								VisualConsole(GL&renderer);
									
			void				setRenderer(GL*renderer);			//!< Specifies the renderer to use (optional)
			void				setRenderer(GL&renderer);			//!< Specifies the renderer to use (optional)
			GL*					getRenderer()	const;				//!< Retrieves the currently used renderer
									
			Font&				getFont();													//!< Retrieves the local font object
			void				setFinalRegion(float x, float y, float width, float height);	//!< Defines the final (visible) console location
			void				setFontSize(float font_x, float font_y);						//!< Defines history (and input) font size
			void				setTexture(eConsoleSection section, const Texture&texture);							//!< Defines the background texture of a section \param section Target section \param texture Target texture object or an empty texture object to disable background rendering of the specified sections
			void				setTextures(eConsoleSection section, const Texture&primary, const Texture&shiny);	//!< Defines both the background and shining texture a section \param section Target section \param primary Target texture object or an empty texture object to disable background rendering of the specified sections \param shiny Target texture object or an empty texture object to disable shiny-rendering of the specified sections
			void				setPrimaryTranslation(float x, float y, unsigned section_mask);						//!< Sets the primary animation movement of one or more sections. The primary movement occurs during the first half of the open/close animation \param x Primary x translation \param y Primary y translation \param section_mask Combination of one or more eConsoleSection enumerations (i.e. Head|Foot)
			void				setSecondaryTranslation(float x, float y, unsigned section_mask);						//!< Sets the secondary animation movement of one or more sections. The secondary movement occurs during the second half of the open/close animation in addition to the full primary movement vector \param x Primary x translation \param y Primary y translation \param section_mask Combination of one or more eConsoleSection enumerations (i.e. Head|Foot)
			void				setRelativeTextureArea(eConsoleSection section, float left, float bottom, float right, float top);	//!< Defines the final (visible) position of the specified section \param section Target section \param left Left border (relative to the console location) \param bottom Bottom border (relative to the console location) \param right Right border (relative to the console location) \param top Top border (relative to the console location)
			void				setRelativeTextArea(eConsoleSection target, float left, float bottom, float right, float top);		//!< Links history output to the target section and defines the text position relative to the target section \param section Target section \param left Left border (relative to the console and section location) \param bottom Bottom border (relative to the console and section location) \param right Right border (relative to the console and section location) \param top Top border (relative to the console and section location)

				
			void				render();		//!< Renders the console
				
			void				print(const String&line, bool word_wrap=true);	//!< Appends the specified line (wrapped) to the end of the console history \param line Line to print \param word_wrap Set true to automatically split the specified line into multiple lines where needed

			void				setState(bool);

			Action				resolve(const float pointer[2]);			//!< Resolves a mouse click at the specified position. The console checks where the specified position is in its current animation state and suggest an action \param pointer Current mouse pointer position \return Suggested action (if any)
			void				execute(Action);							//!< Executes an action suggested by resolve()
			Action				resolveAndExecute(const float pointer[2]);	//!< Resolves and automatically executes an action. Identical to execute(resolve(pointer)) \param pointer Current mouse pointer position \return Resolved and executed action (if any)

//			void				loadDefaults(TextureResource<GL>&archive, int window_width, int window_height, Key::Name close_key=Key::Caret, const String&head_name="head", const String&head_light="head_li",const String&body="body",const String&foot="foot", const String&foot_light="foot_li");	//!< Generates the default console
		};

}

#include "console.tpl.h"

#endif
