#include "../../global_root.h"
#include "outline_font.h"

/******************************************************************

Windows OpenGL outline font.

******************************************************************/

namespace Engine
{

	#if SYSTEM==WINDOWS

	GLOutlineFont::GLOutlineFont():base(0)
	{}

	GLOutlineFont::~GLOutlineFont()
	{
		unmake();
	}

	void	GLOutlineFont::unmake()
	{
	    if (base)
	        glDeleteLists(base,0x100);
		base = 0;
	}


	void GLOutlineFont::make(HDC hDC, const char*font_name, int size_, FontStyle style)
	{
		font_size = size_;
		//size = size_;
	    const char*name = font_name?font_name:"Courier New";
	    if (!base)
	    	base = glGenLists(0x100);

		size = 0;

	        HFONT font = CreateFontA( -size_,                           // Height Of Font
	                        0,                                  // Width Of Font
	                        0,                                  // Angle Of Escapement
	                        0,                                  // Orientation Angle
							((style&Bold)?FW_BOLD:FW_NORMAL),            // Font Weight
							((style&Italic)?TRUE:FALSE),              // Italic
	                        FALSE,                              // Underline
	                        FALSE,                              // Strikeout
	                        ANSI_CHARSET,
	                        OUT_TT_PRECIS,
	                        CLIP_DEFAULT_PRECIS,
	                        ANTIALIASED_QUALITY,
	                        FF_DONTCARE|DEFAULT_PITCH,
	                        name);
	        if (!font)
	            return;

	        HFONT oldfont = (HFONT)SelectObject(hDC, font);           // Selects The Font We Want

	             wglUseFontOutlines(hDC,                             // Select The Current DC
	                                0,                              // Starting Character
	                                0xFF,                          // Number Of Display Lists To Build
	                                base,                           // Starting Display Lists
	                                0.f,                           // Deviation From The True Outlines
	                                0.2f,                      // Font Thickness In The Z Direction
	                                WGL_FONT_POLYGONS,              // Use Polygons, Not Lines
	                                gmf);                           // Address Of Buffer To Recieve Data
	    	SelectObject(hDC, oldfont);							// Selects The Font We Want
	    	DeleteObject(font);									// Delete The Font
	        for (USHORT k = 0; k < 0x100; k++)
	            if (gmf[k].gmfBlackBoxY > size)
	                size = gmf[k].gmfBlackBoxY;	

	    top_left = gmf[(BYTE)'X'].gmfptGlyphOrigin;


	    internal_scale = 1.0f/size;
	}


	float GLOutlineFont::GetWidth(const char*str)
	{
	    float len(0);
	    while (*str)
	        len+= gmf[(BYTE)*str++].gmfCellIncX;
	    return len*internal_scale * font_size;
	}

	float GLOutlineFont::GetWidth(const char*str, size_t len)
	{
	    float result(0);
	    for (index_t i = 0; i < len; i++)
	        result+= gmf[(BYTE)str[i]].gmfCellIncX;
	    return result*internal_scale * font_size;
	}

	float GLOutlineFont::GetWidth(char c)
	{
	    return gmf[(BYTE)c].gmfCellIncX;//*internal_scale;
	}



	void GLOutlineFont::begin(const TFontState&state)
	{
	    glDisable(GL_TEXTURE_2D);
	    glPushMatrix();
	    
	        glTranslatef(state.left+state.indent,
			state.top-
			state.lineOffset,
			//(float)(state.line*state.y_scale*internal_scale*font_size),
			state.depth);
			//glTranslatef(0.5,0.5,0);
	        //glTranslatef(state.left+state.indent,state.top,state.depth);
	        glScalef(state.x_scale * internal_scale* font_size,state.y_scale * internal_scale*font_size,1);
	        //glTranslatef(0,-(float)state.line-1,0);
	        glColor4f(state.red, state.green, state.blue, state.alpha/4.0f);

			jitter = 0.3f / font_size;

			
	        glPushAttrib(GL_LIST_BIT|GL_POLYGON_BIT);
	    	glListBase(base);
	}

	void GLOutlineFont::alterColor(const TFontColor&color)
	{
	        glColor4f(color.red, color.green, color.blue, color.alpha/4.0f);
	        //glColor4fv(color.v);
	}

	void GLOutlineFont::write(const char*str)
	{
		//glPushMatrix();
		//	glTranslatef(-jitter,-jitter*0.5,0);
	 //       glCallLists((GLsizei)strlen(str), GL_UNSIGNED_BYTE, str);
		//glPopMatrix();
		//glPushMatrix();
		//	glTranslatef(jitter*0.5,-jitter,0);
	 //       glCallLists((GLsizei)strlen(str), GL_UNSIGNED_BYTE, str);
		//glPopMatrix();
		//glPushMatrix();
		//	glTranslatef(-jitter*0.5,jitter,0);
	 //       glCallLists((GLsizei)strlen(str), GL_UNSIGNED_BYTE, str);
		//glPopMatrix();
		//glPushMatrix();
		//	glTranslatef(jitter,jitter*0.5,0);
	 //       glCallLists((GLsizei)strlen(str), GL_UNSIGNED_BYTE, str);
		//glPopMatrix();
		/*glPushMatrix();
	        glCallLists((GLsizei)strlen(str), GL_UNSIGNED_BYTE, str);
		glPopMatrix();*/
	        glCallLists((GLsizei)strlen(str), GL_UNSIGNED_BYTE, str);
	}

	void GLOutlineFont::write(const char*str, size_t len)
	{
		//glPushMatrix();
		//	glTranslatef(-jitter,-jitter*0.5,0);
	 //       glCallLists((GLsizei)len, GL_UNSIGNED_BYTE, str);
		//glPopMatrix();
		//glPushMatrix();
		//	glTranslatef(jitter*0.5,-jitter,0);
	 //       glCallLists((GLsizei)len, GL_UNSIGNED_BYTE, str);
		//glPopMatrix();
		//glPushMatrix();
		//	glTranslatef(-jitter*0.5,jitter,0);
	 //       glCallLists((GLsizei)len, GL_UNSIGNED_BYTE, str);
		//glPopMatrix();
		//glPushMatrix();
		//	glTranslatef(jitter,jitter*0.5,0);
	 //       glCallLists((GLsizei)len, GL_UNSIGNED_BYTE, str);
		//glPopMatrix();
		/*glPushMatrix();
	        glCallLists((GLsizei)len, GL_UNSIGNED_BYTE, str);
		glPopMatrix();*/
	        glCallLists((GLsizei)len, GL_UNSIGNED_BYTE, str);
	}

	void GLOutlineFont::end()
	{
	        glPopAttrib();
	    glPopMatrix();
	}

	#endif
}
