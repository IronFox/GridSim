#ifndef engine_gl_texture_fontH
#define engine_gl_texture_fontH

/******************************************************************

OpenGL texture-font.

******************************************************************/


#include "../textout.h"
#include "../../gl/gl.h"

namespace Engine
{


	class GLTextureFont	//! OpenGL texture font object usable in combination with the Textout class
	{
	protected:
	        GLuint              base,		//!< Display list base used to store the final characters
								texture;	//!< OpenGL texture reference
	public:
	                            GLTextureFont();
	virtual                    ~GLTextureFont();
	        void                make(GLuint texture);	//!< Initializes the texture object using the specified texture. The object may be reinitialized again using different textures. \param texture Reference to an OpenGL texture to use as font.

	        float               GetWidth(const char*str);				//!< Calculates the unscaled width of the specified string. For EveGLTextureFont this is identical to the length of the string. \param str String to determine the length of \return Unscaled length of the string
	        float               GetWidth(const StringRef&str);	//!< Calculates the unscaled width of the specified string. For EveGLTextureFont this is identical to the length of the string. \param str String to determine the length of \param len Number of characters to stop after \return Unscaled length of the string
	        float               GetWidth(char c);						//!< Calculates the unscaled width of a single character. For EveGLTextureFont this is always 1. \param c Character to determine the width of \return Unscaled length of the character
			float				GetHeight()	{return 1;}
	        void                Begin(const TFontState&state);		//!< Begins text output starting with the specified state \param state Status to begin with
	        void                AlterColor(const TFontColor&color);	//!< Changes the active color while between begin() and end(). \param color Color to use from here on
	        void                Write(const StringRef&str);	//!< Print the specified string. Newline characters are overridden  \param str String to print \param len Number of characters to print of \b str
	        void                End();									//!< Ends text output
	};
}

#endif

