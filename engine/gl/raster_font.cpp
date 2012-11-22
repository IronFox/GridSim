#include "../../global_root.h"
#include "raster_font.h"

/******************************************************************

E:\include\eve\eve.gl_raster_font.cpp


******************************************************************/

namespace Engine
{

	static const unsigned Characters = 256-32;

	GLRasterFont::GLRasterFont():base(0)
	{}

	GLRasterFont::~GLRasterFont()
	{
		unmake();
	}

	void	GLRasterFont::unmake()
	{
	    if (base)
	        glDeleteLists(base,Characters);
		base = 0;
	}

	bool GLRasterFont::isMade()
	{
	    return base!=0;
	}

	bool GLRasterFont::remake(const char*font_name, int size, FontStyle style)
	{
	    if (!base)
	        base = glGenLists(Characters);                  // Storage For [Characters] Characters
	    if (!font_name)
	        font_name = "helvetica";
		font_size = size;
	    #if SYSTEM==WINDOWS

	        HFONT   font;                       // Windows Font ID
	        HFONT   oldfont;                    // Used For Good House Keeping

	        font = CreateFontA(  -size,                // Height Of Font ( NEW )
	                            0,              // Width Of Font
	                            0,              // Angle Of Escapement
	                            0,              // Orientation Angle        
	                            ((style&Bold)?FW_BOLD:FW_NORMAL),            // Font Weight
	                            ((style&Italic)?TRUE:FALSE),              // Italic
	                            FALSE,              // Underline
	                            FALSE,              // Strikeout        
	                            ANSI_CHARSET,           // Character Set Identifier
	                            OUT_TT_PRECIS,          // Output Precision
	                            CLIP_DEFAULT_PRECIS,        // Clipping Precision
	                            ANTIALIASED_QUALITY,        // Output Quality
	                            FF_DONTCARE|DEFAULT_PITCH,  // Family And Pitch
	                            font_name);         // Font Name
	        if (!font)
	            return false;
	        oldfont = (HFONT)SelectObject(hDC, font);       // Selects The Font We Want
	        if (!GetCharWidth32(hDC,0,255,width))
	            ErrMessage("unable to gather font width attribute");
	        wglUseFontBitmaps(hDC, 32, Characters, base);           // Builds [Characters] Characters Starting At Character 32
	        SelectObject(hDC, oldfont);             // Selects The Font We Want
	        DeleteObject(font);                 // Delete The Font
			
			//and here we got for a second round: (without the font won't render. dunno why)
	        font = CreateFontA(  -size,                // Height Of Font ( NEW )
	                            0,              // Width Of Font
	                            0,              // Angle Of Escapement
	                            0,              // Orientation Angle        
	                            ((style&Bold)?FW_BOLD:FW_NORMAL),            // Font Weight
	                            ((style&Italic)?TRUE:FALSE),              // Italic
	                            FALSE,              // Underline
	                            FALSE,              // Strikeout        
	                            ANSI_CHARSET,           // Character Set Identifier
	                            OUT_TT_PRECIS,          // Output Precision
	                            CLIP_DEFAULT_PRECIS,        // Clipping Precision
	                            ANTIALIASED_QUALITY,        // Output Quality
	                            FF_DONTCARE|DEFAULT_PITCH,  // Family And Pitch
	                            font_name);         // Font Name
	        if (!font)
	            return false;
	        oldfont = (HFONT)SelectObject(hDC, font);       // Selects The Font We Want
	        if (!GetCharWidth32(hDC,0,255,width))
	            ErrMessage("unable to gather font width attribute");
	        wglUseFontBitmaps(hDC, 32, Characters, base);           // Builds [Characters] Characters Starting At Character 32
	        SelectObject(hDC, oldfont);             // Selects The Font We Want
	        DeleteObject(font);                 // Delete The Font
			height = abs(size);

	    #elif SYSTEM==UNIX

	        String full_name = "-*-"+String(font_name)+"-"+(style&Bold?"bold":"medium")+"-r-"+(style&Italic?"italic":"normal")+"-*-"+IntToStr(size)+"-*-*-*-p-*-*-*";
	        /*
	        int fonts(0);
	        char**font_list = XListFonts(getDisplay(),"*",10240,&fonts);
	        ShowMessage("fonts matching: "+IntToStr(fonts));
	        for (int i = 0; i < fonts; i++)
	            ShowMessage(font_list[i]);
	        XFreeFontNames(font_list);*/
	      
	        //String full_name = "-*-"+String(font_name)+"-24-*";
	        //ShowMessage(full_name);
	        XFontStruct*font = XLoadQueryFont(getDisplay(), full_name.c_str());
	        if (!font)
	        {
				ShowMessage("Unable to retrieve requested font ('"+full_name+"'). Defaulting to 'fixed'...");
	            font = XLoadQueryFont(getDisplay(), "fixed");
	            if (!font)                    
	                return false;
	        }
	        if (font->per_char)
	        {
				short	ascent = 0,
						descent = 0;
	            for (unsigned i = 0; i < 255; i++)
				{
	                width[i] = font->per_char[i].width;
					if (abs(font->per_char[i].ascent)>ascent)
						ascent = abs(font->per_char[i].ascent);
					if (abs(font->per_char[i].descent)>descent)
						descent = abs(font->per_char[i].descent);
				}
				height = ascent;//+descent;
	        }
	        else
	            for (unsigned i = 0; i < 255; i++)
				{
	                width[i] = font->min_bounds.width;
					height = abs(font->ascent)+abs(font->descent);
				}
	        glXUseXFont(font->fid, 32, Characters, base);
	        XFreeFont(getDisplay(), font);
	    
	    #endif
	    return true;
	}


	bool GLRasterFont::make(HDC hDC,const char*font_name,int size, FontStyle style)
	{
		this->hDC = hDC;
		return remake(font_name,size,style);

	}    

	float GLRasterFont::getHeight()
	{
		return height;
	}
	
	float GLRasterFont::getWidth(const char*str)
	{
	    float len(0);
	    while (*str)
	        len+= width[(BYTE)*str++];
	    return len;
	}

	float GLRasterFont::getWidth(const char*str, size_t len)
	{
	    float result(0);
	    for (index_t i = 0; i < len; i++)
	        result+= width[(unsigned)*str++];
	    return result;
	}

	float GLRasterFont::getWidth(char c)
	{
	    return width[(BYTE)c];
	}

	void GLRasterFont::begin(const TFontState&state)
	{
		if (base == 0)
			FATAL__("Trying to render GLRasterFont text, but the local font has not been created.");
		ASSERT__(base != 0);
	    glDisable(GL_TEXTURE_2D);
	    glPushMatrix();
	        // glTranslatef(state.left,state.top-(float)(state.line*font_size),state.depth);
	        glTranslatef(state.left+state.indent,state.top-(float)(state.line*state.y_scale*height),state.depth);
	        glColor4fv(state.v);
	        glRasterPos2f(0,0);
	        glPushAttrib(GL_LIST_BIT|GL_POLYGON_BIT);
	        glListBase(base-32);
	}

	void GLRasterFont::alterColor(const TFontColor&color)
	{
	        glColor4fv(color.v);
	}

	void GLRasterFont::write(const char*str)
	{
	        glCallLists((GLsizei)strlen(str), GL_UNSIGNED_BYTE, str);
	}

	void GLRasterFont::write(const char*str, size_t len)
	{
	        glCallLists((GLsizei)len, GL_UNSIGNED_BYTE, str);
	}

	void GLRasterFont::end()
	{
	        glPopAttrib();
	    glPopMatrix();
	}

}
