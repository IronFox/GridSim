#ifndef engine_textoutH
#define engine_textoutH

/******************************************************************

Font-dependent textout-manager. The appearance of the
resulting render depends on the used font.

******************************************************************/



#include "../global_string.h"
#include "../math/vector.h"
#include "../io/str_stream.h"

namespace Engine
{

	BYTE hexStr(const char*line);
	BYTE hexChar(char c);

	enum    FontStyle
	{
		Normal,
		Bold,
		Italic,
		NormalStyle = Normal,
		BoldStyle=Bold,
		ItalicStyle=Italic
	};


	typedef TVec4<>	TFontColor;
			

	struct TFontState:public TFontColor		//! Font status fragment
	{
			float		left,				//!< Current root position (x)
						top,				//!< Current root position (y)
						depth,				//!< Current root position (z)
						indent;				//!< Current indentation offset (x)
		union
		{
			struct
			{
				float	x_scale,			//!< Current font scale (width)
						y_scale;			//!< Current font scale (height)
			};
			struct
			{
				float	scale_x,			//!< Current font scale (width)
						scale_y;			//!< Current font scale (height)
			};
		};
			float		lineOffset;				//!< Current line
	};

	/*
	template class EveFont	//the height of a character must always be 1
	{
		float	getWidth(const char*str);
		float	getWidth(const char*str, size_t len);
		float	getWidth(char);
		void	begin(const TFontState&state);
		void	alterColor(const TFontColor&color);
		void	write(const char*str);
		void	write(const char*str, size_t len);
		void	end();
	}
	*/

	/**
		@brief Virtual textout control class
		
		Text output is passed to virtual methods which may be implemented in various ways
	*/
	class VirtualTextout
	{
	protected:
			TFontState		stack[0x100];
			TFontColor		colorStack[0x100];
			BYTE			stack_depth,colorStackDepth;
			
			
	virtual	void			beginOutput()=0;
	virtual	void			writeSegment(const char*field, size_t len)=0;
	virtual	void			endOutput()=0;
	virtual	void			alterColor(const TFontColor&color)=0;
	public:
			TFontState		state;	//!< Current output status


			
			
							VirtualTextout();
	virtual					~VirtualTextout()	{};
			
	
			void			color(float red, float green, float blue, float alpha);	//!< Respecify the current textout color \param red Red color segment (0 ... 1) \param green Green color segment (0 ... 1)	\param blue Blue color segment (0 ... 1)	\param alpha Opacity (0 = invisible ... 1 = opaque)
			void			color4(const float*color);								//!< Respecify the current textout color via a 4 element array \param color 4 segment float array in rgba order.
			void			color4(const double*color);								//!< Respecify the current textout color via a 4 element array \param color 4 segment double array in rgba order.
			void			color(float red, float green, float blue);				//!< Respecify the current textout color (opaque) \param red Red color segment (0 ... 1) \param green Green color segment (0 ... 1)	\param blue Blue color segment (0 ... 1) 
			void			color(const TVec3<>&color);
			void			color(const TVec3<double>&color);
			void			color(const TVec3<>&color,float alpha);
			void			color(const TVec3<double>&color, double alpha);
			void			color(const TVec4<>&color);
			void			color(const TVec4<double>&color);
			void			color3(const float*color);								//!< Respecify the current textout color via a 3 element array (opaque) \param color 3 segment float array in rgb order.
			void			color3(const double*color);								//!< Respecify the current textout color via a 3 element array (opaque) \param color 3 segment double array in rgb order.
			void			color3a(const float*color, float alpha);				//!< Respecify the current textout color via a 3 element array and an alpha value \param color 3 segment float array in rgb order. @param alpha Opacity (0 = invisible ... 1 = opaque)
			void			color3a(const double*color, float alpha);				//!< Respecify the current textout color via a 3 element array and an alpha value \param color 3 segment double array in rgb order. @param alpha Opacity (0 = invisible ... 1 = opaque)
				
	virtual	float			getUnscaledWidth(const char*line, size_t len)=0;		//!< Determine the length of a string in its native (unscaled) size \param line Pointer to a field of characters containing the string to analyse \param len Number of characters to analyse out of line \return Unscaled length of the specified string
	virtual	float			getUnscaledWidth(const char*line)=0;					//!< Determine the length of a string in its native (unscaled) size \param line Pointer to a zero terminated character array containing the string to analyse \return Unscaled length of the specified string
	virtual	float			getUnscaledHeight() = 0;								//!< Determine the height of a string in its native (unscaled) size
			float			getUnscaledWidth(const String&line);					//!< Determine the length of a string in its native (unscaled) size \param line String to analyse \return Unscaled length of the specified string
			float			getScaledWidth(const char*line, size_t len);			//!< Determine the length of a string \param line Pointer to a field of characters containing the string to analyse \param len Number of characters to analyse out of line \return Scaled length of the specified string
			float			getScaledWidth(const char*line);						//!< Determine the length of a string \param line Pointer to a zero terminated character array containing the string to analyse \return Scaled length of the specified string
			float			getScaledWidth(const String&line);						//!< Determine the length of a string \param line String to analyse \return Scaled length of the specified string
			float			getScaledHeight();										//!< Determine the height of a string \return Scaled height of a single line

			void			newLine();												//!< Increases line count and resets current line indentation to 0
			void			move(float by_x, float by_y, float by_z=0);				//!< Move the out cursor. Also resets the active line to 0. \param by_x X-delta \param by_y Y-delta \param by_z Optional Z-delta
			void			locate(float x, float y, float z=0);					//!< Position the out cursor at a new location. Also resets the active line to 0. \param x X-coordinate \param y Y-coordinate \param z Optional z coordinate
			void			locate(const TVec2<>&p);								//!< Position the out cursor at a new location. Also resets the active line to 0.
			void			locate(const TVec3<>&p);								//!< Position the out cursor at a new location. Also resets the active line to 0.
			void			locate(const float p[2]);								//!< Position the out cursor at a new location. Also resets the active line to 0. \param p Two component position vector
			void			locate3fv(const float p[3]);							//!< Position the out cursor at a new location. Also resets the active line to 0. \param p Two component position vector
			void			locate3(const float p[3])	{locate3fv(p);}				//!< @overload
			void			setScale(float x, float y);								//!< Redefine character scale \param x Character width \param y Character height
			void			scale(float by_x, float by_y);							//!< Alter the character scale \param by_x Width-factor \param by_y Height-factor
			void			pushState();											//!< Pushes the current output status to the local stack
			void			PushColorState();
			void			popState();												//!< Restores the current output status from the local stack
			void			PopColorState();
			void			SetLineOffset(float offset);
			void			SetLine(int line_index);								//!< Changes the active line. \param line_index Line to change to (0=first line)
			float			GetLineOffset()	const;									//!< Queries the current line offset (increased by newlines)

			void			write(const char*str, size_t len);					//!< Prints a string segment with a predefined length @param str Pointer to an array of characters containing at least @b len characters @param len Number of characters to print
			void			write(const char*str);
			void			write(const String&str);
			void			writeTagged(const char*str, char tag);
			void			writeTagged(const String&str, char tag);
			void			writeln(const char*str);
			void			writeln(const String&str);
			void			writeTaggedLine(const char*str, char tag);
			void			writeTaggedLine(const String&str, char tag);
	};
	
	
	/*!
		\brief Textout control class

		The template Textout class acts as a wrapper class for the underlying font object. It controls the current cursor position, color, scale and line of a compatible font object.
		Currently working with this module are the classes EveGLOutlineFont (unfortunately windows only), EveGLTextureFont and (with certain limitations) EveGLRasterFont.
		Each Textout instance provides its own font object but may use an external font object (of the same type) as well.
	*/

	template <class Font>
		class Textout:public VirtualTextout
		{
		protected:
				Font			local_font,*active_font;
				
		virtual	void			beginOutput();
		virtual	void			writeSegment(const char*field, size_t len);
		virtual	void			endOutput();
		virtual	void			alterColor(const TFontColor&color);
				void			stream(const char*str, size_t len);
			template <typename T>	
				void			streamUnsigned(T value);
			template <typename T, typename UT>
				void			streamSigned(T value);
			template <typename T>
				void			streamFloat(T value, unsigned precision);


		public:
				
								Textout();


				void			setFont(Font*font);										//!< Adopt an external font object of template type Font \param font Pointer to an external font object. Passing NULL will cause Textout to switch back to its internal font object.
				Font&			getFont();												//!< Get a reference to the internal font object \return Reference to the internal font object
				Font*			getActiveFont();										//!< Get a pointer to the currently active font object. This may be the internal object or an external font object assigned via setFont(...). \return Pointer to the currently used font object.
				float			unscaledLength(const char*line, size_t len);			//!< Determine the length of a string in its native (unscaled) size \param line Pointer to a field of characters containing the string to analyse \param len Number of characters to analyse out of line \return Unscaled length of the specified string
				float			unscaledLength(const char*line);						//!< Determine the length of a string in its native (unscaled) size \param line Pointer to a zero terminated character array containing the string to analyse \return Unscaled length of the specified string
				float			unscaledLength(const String&line);						//!< Determine the length of a string in its native (unscaled) size \param line String to analyse \return Unscaled length of the specified string
				float			scaledLength(const char*line, size_t len);			//!< Determine the length of a string \param line Pointer to a field of characters containing the string to analyse \param len Number of characters to analyse out of line \return Scaled length of the specified string
				float			scaledLength(const char*line);							//!< Determine the length of a string \param line Pointer to a zero terminated character array containing the string to analyse \return Scaled length of the specified string
				float			scaledLength(const String&line);						//!< Determine the length of a string \param line String to analyse \return Scaled length of the specified string
				
				float			unscaledWidth(const char*line, size_t len);			//!< Determine the length of a string in its native (unscaled) size \param line Pointer to a field of characters containing the string to analyse \param len Number of characters to analyse out of line \return Unscaled length of the specified string
				float			unscaledWidth(const char*line);							//!< Determine the length of a string in its native (unscaled) size \param line Pointer to a zero terminated character array containing the string to analyse \return Unscaled length of the specified string
				float			unscaledWidth(const String&line);						//!< Determine the length of a string in its native (unscaled) size \param line String to analyse \return Unscaled length of the specified string
				float			scaledWidth(const char*line, size_t len);				//!< Determine the length of a string \param line Pointer to a field of characters containing the string to analyse \param len Number of characters to analyse out of line \return Scaled length of the specified string
				float			scaledWidth(const char*line);							//!< Determine the length of a string \param line Pointer to a zero terminated character array containing the string to analyse \return Scaled length of the specified string
				float			scaledWidth(const String&line);						//!< Determine the length of a string \param line String to analyse \return Scaled length of the specified string

		virtual	float			getUnscaledWidth(const char*line, size_t len);		//!< Determine the length of a string in its native (unscaled) size \param line Pointer to a field of characters containing the string to analyse \param len Number of characters to analyse out of line \return Unscaled length of the specified string
		virtual	float			getUnscaledWidth(const char*line);						//!< Determine the length of a string in its native (unscaled) size \param line Pointer to a zero terminated character array containing the string to analyse \return Unscaled length of the specified string
		virtual	float			getUnscaledHeight();									//!< Determine the height of a string in its native (unscaled) size

				Textout<Font>&	operator<<(const String&);
				Textout<Font>&	operator<<(const char*);
				Textout<Font>&	operator<<(const TNewLine&);
				Textout<Font>&	operator<<(const TSpace&);
				Textout<Font>&	operator<<(const TTabSpace&);
				Textout<Font>&	operator<<(char);
				Textout<Font>&	operator<<(BYTE);
				Textout<Font>&	operator<<(long long);
				Textout<Font>&	operator<<(unsigned long long);
				Textout<Font>&	operator<<(int);
				Textout<Font>&	operator<<(unsigned);
				Textout<Font>&	operator<<(long);
				Textout<Font>&	operator<<(unsigned long);
				Textout<Font>&	operator<<(short);
				Textout<Font>&	operator<<(unsigned short);
				Textout<Font>&	operator<<(float);
				Textout<Font>&	operator<<(double);
			template <typename T>
				Textout<Font>&	operator<<(const TVec2<T>&v)	{return *this << '('<<v.x << ", "<<v.y<<')';}
			template <typename T>
				Textout<Font>&	operator<<(const TVec3<T>&v)	{return *this << '('<<v.x << ", "<<v.y<< ", "<<v.z<<')';}
				
				void			print(const char*str, size_t len);					//!< Prints a string segment with a predefined length @param str Pointer to an array of characters containing at least @b len characters @param len Number of characters to print
				void			print(const char*str);
				void			print(const String&str);
				void			printTagged(const char*str, char tag);
				void			printTagged(const String&str, char tag);
				void			println(const char*str);
				void			println(const String&str);
				void			printTaggedLine(const char*str, char tag);
				void			printTaggedLine(const String&str, char tag);
		};

	/*!

		*	\fn	void Textout::print(const char*str)
		*	\brief Prints the specified string
		*	\param str String to print

	Prints the specified string at the current cursor position. The string may contain newline characters causing the print(...)-method to return to the beginning again and increment the state.line variable by one.
	Succeeding print(...) calls will begin at the line, that the previous call ended at.

		*	\fn	void Textout::print(const String&str)
		*	\brief Prints the specified string
		*	\param str String to print

	Prints the specified string at the current cursor position. The string may contain newline characters causing the print(...)-method to return to the beginning again and increment the state.line variable by one.
	Succeeding print(...) calls will begin at the line, that the previous call ended at.


		*	\fn	void Textout::print(const char*str, char tag)
		*	\brief Prints the specified string using tags
		*	\param str String to print
		*	\param tag Tag-initiator
		
		This method prints the string tag-sensitive. Assuming the tag-variable is '%', then the following tags are possible:
		'%push':		Pushes the current output status to the local stack
		'%pop':		Restores the current output status from the local stack
		'%rrggbbaa':	Hex representation of the color to use from this point on
		-
		A string "%0000ffffsomething %push%ff0000ffvery %popstupid" would color 'something' and 'stupid' blue but the 'very' in the middle red.
		
		*	\fn	void Textout::print(const String&str, char tag)
		*	\brief Prints the specified string using tags
		*	\param str String to print
		*	\param tag Tag-initiator
		
		This method prints the string tag-sensitive. Assuming the tag-variable is '%', then the following tags are possible:
		'%push':		Pushes the current output status to the local stack
		'%pop':		Restores the current output status from the local stack
		'%rrggbbaa':	Hex representation of the color to use from this point on
		-
		A string "%0000ffffsomething %push%ff0000ffvery %popstupid" would color 'something' and 'stupid' blue but the 'very' in the middle red.
		
		*/
}
 
#include "textout.tpl.h"
#endif
