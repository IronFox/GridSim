#ifndef engine_gl_raster_fontH
#define engine_gl_raster_fontH

/******************************************************************

Rastered OpenGL font.

******************************************************************/

#include "../../gl/gl.h"
#include "../textout.h"
#if SYSTEM==UNIX
	#include "../../gl/glx.h"
#endif

namespace Engine
{

	class GLRasterFont
	{
	private:
	        GLuint              base;
	        int                 width[0x100],font_size,height;
			HDC					hDC;

	public:

	                            GLRasterFont();
	virtual                    ~GLRasterFont();
	        bool                make(HDC hDC,const char*font_name=NULL, int size=16, FontStyle style=Normal);
			bool				remake(const char*font_name=NULL, int size=16, FontStyle style=Normal);
	        bool                isMade();
			void				unmake();
	        
	        float               getWidth(const char*str);
	        float               getWidth(const char*str, size_t len);
	        float               getWidth(char);
			float				getHeight();
	        void                begin(const TFontState&state);
	        void                alterColor(const TFontColor&color);
	        void                write(const char*str);
	        void                write(const char*str, size_t len);
	        void                end();
	};
}

#endif
