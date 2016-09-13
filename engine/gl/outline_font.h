#ifndef engine_gl_outline_fontH
#define engine_gl_outline_fontH

/******************************************************************

Windows OpenGL outline font.

******************************************************************/



#include "../../gl/gl.h"
#include "../textout.h"

namespace Engine
{

	#if SYSTEM==WINDOWS

	class GLOutlineFont
	{
	private:

	        GLYPHMETRICSFLOAT   gmf[256];	// Storage For Information About Our Outline Font Characters
	        GLuint              base;
	        POINTFLOAT          top_left;
			int					font_size;
	        float               internal_scale;
			float				size,jitter;
			

	public:
	                            GLOutlineFont();
	virtual                    ~GLOutlineFont();
	        void                make(HDC hDC, const char*font_name=NULL, int size=16, FontStyle style=Normal);
			void				unmake();
	        
	        float               GetWidth(const char*str);
	        float               GetWidth(const char*str, size_t len);
	        float               GetWidth(char);
			float				GetHeight()	{return size;}
	        void                begin(const TFontState&state);
	        void                alterColor(const TFontColor&color);
	        void                write(const char*str);
	        void                write(const char*str, size_t len);
	        void                end();
	};

	#endif
}

#endif

