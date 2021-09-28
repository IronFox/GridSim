#ifndef engine_gl_raster_fontH
#define engine_gl_raster_fontH

/******************************************************************

Rastered OpenGL font.

******************************************************************/

#include <global_root.h>
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
	        
	        float               GetWidth(const char*str);
	        float               GetWidth(const StringRef&str);
	        float               GetWidth(char);
			float				GetHeight();
	        void                Begin(const TFontState&state);
	        void                AlterColor(const TFontColor&color);
	        void                Write(const char*str);
	        void                Write(const StringRef&str);
	        void                End();
	};
}

#endif