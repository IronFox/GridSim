#include "../../global_root.h"
#include "texture_font.h"

/******************************************************************

OpenGL texture-font.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

namespace Engine
{
	static char global_alpha[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\"$!?();,.:_- öäü#+*'/&%=[]{}\\ß<>|";

	GLTextureFont::GLTextureFont():base(0)
	{}

	GLTextureFont::~GLTextureFont()
	{
	    if (base)
	        glDeleteLists(base,0x100);
	}

	void GLTextureFont::make(GLuint texture_)
	{
	    const float   W = 15.8,
	                  H = 7.85;

	    texture = texture_;
	    if (!base)
	    {
	        base = glGenLists(0x100);
	        
	        for (unsigned i = 0; i < sizeof(global_alpha)-1; i++)
	        {
	            unsigned w_base = (unsigned)floor(W),
	                     line = i/w_base,
	                     l_pos = i%w_base;

	            float t_left = (float)l_pos/W,
	                  t_top = 1.0-((float)line+0.4)/H,
	                  t_right = ((float)l_pos+1)/W,
	                  t_bottom=1.0-((float)line+1.3)/H;

	            GLuint local = base+(BYTE)global_alpha[i];
	            if (local < base || local >= base+0x100)
	                FATAL__("font creation error for character "+String(global_alpha[i])+" ("+IntToStr((unsigned)(BYTE)global_alpha[i])+")");

	            glNewList(local,GL_COMPILE);
	            
	                glBegin(GL_QUADS);
	                    glTexCoord2f(t_left,t_bottom);   glVertex2f(0,0);
	                    glTexCoord2f(t_right,t_bottom);  glVertex2f(0.5,0);
	                    glTexCoord2f(t_right,t_top);     glVertex2f(0.5,1);
	                    glTexCoord2f(t_left,t_top);      glVertex2f(0,1);
	                glEnd();
	                glTranslatef(0.5,0,0);
	            glEndList();
	        }
	    }
	}

	float GLTextureFont::getWidth(const char*str)
	{
	    return 0.5f*strlen(str);
	}

	float GLTextureFont::getWidth(const char*str, size_t len)
	{
	    return 0.5f*len;
	}

	float GLTextureFont::getWidth(char)
	{
	    return 0.5f;
	}

	void GLTextureFont::begin(const TFontState&state)
	{
	    glPushMatrix();
	        glTranslatef(state.left+state.indent,state.top,state.depth);
	        glTranslatef(0,-state.lineOffset,0);
	        glScalef(state.x_scale,state.y_scale,1);
	        glTranslatef(0,-1,0);
//	        glTranslatef(0,-(float)state.line-1,0);
	        glColor4fv(state.v);
	        glPushAttrib(GL_LIST_BIT);
	    	glListBase(base);
	    	glEnable(GL_TEXTURE_2D);
	    	glBindTexture(GL_TEXTURE_2D,texture);
	}

	void GLTextureFont::alterColor(const TFontColor&color)
	{
	        glColor4fv(color.v);
	}

	void GLTextureFont::write(const char*str)
	{
	        glCallLists((GLsizei)strlen(str), GL_UNSIGNED_BYTE, str);
	}

	void GLTextureFont::write(const char*str, size_t len)
	{
	        glCallLists(GLuint(len),GL_UNSIGNED_BYTE,str);
	}

	void GLTextureFont::end()
	{
	        glPopAttrib();
	    glPopMatrix();
	}
}
