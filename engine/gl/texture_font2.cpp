#include "../../global_root.h"
#include "texture_font2.h"

/******************************************************************

OpenGL texture-font.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

namespace Engine
{

	GLTextureFont2::GLTextureFont2():base(0),texture(0),scale(1.0f)
	{
		clear();
	}

	GLTextureFont2::~GLTextureFont2()
	{
		clear();
	}
	
	void				GLTextureFont2::clear()
	{
		if (base)
	        glDeleteLists(base,0x100);
		if (texture)
			glDeleteTextures(1,&texture);
		for (unsigned i = 0; i < ARRAYSIZE(chars); i++)
			chars[i].isset = false;
	}

	template <typename T>
		static bool			get(const XML::Node&node, const String&name, T&out)
		{
			String val;
			return node.query(name,val) && convert(val.c_str(),out);
		}

	
	
	void				GLTextureFont2::loadFromFile(const String&filename, float scale_)
	{
		XML::Container	container;
		container.loadFromFile(filename);

		const XML::Node*xfont = container.find("font");
		if (!xfont)
			throw IO::DriveAccess::FileFormatFault(globalString("XML font file lacks <font> tag"));

		if (!get(*xfont,"scale",scale))
			throw IO::DriveAccess::FileFormatFault(globalString("XML <font> attribute lacks 'scale' parameter"));
		scale*=scale_;
		String imagefile;
		if (!xfont->query("image",imagefile))
			throw IO::DriveAccess::FileFormatFault(globalString("XML <font> attribute lacks 'image' parameter"));
			
		Image image;
		Magic::loadFromFile(image,imagefile);


		if (image.channels() != 4)
			throw IO::DriveAccess::FileFormatFault("XML font file specifies unusable font image. Image must have 4 channels (RGBA) but has "+String(image.channels()));

		clear();
	
		height = 0;
		unsigned lowest = UNSIGNED_UNDEF;
		
		for (index_t i = 0; i < xfont->children.count(); i++)
		{
			const XML::Node&xchar = xfont->children[i];
			if (xchar.name != "char")
				continue;
			
			BYTE id;
			if (!get(xchar,"id",id))
				continue;
			unsigned left, right,bottom,top;
			if (!get(xchar,"left",left))
				continue;
			if (!get(xchar,"right",right))
				continue;
			if (!get(xchar,"bottom",bottom))
				continue;
			if (!get(xchar,"top",top))
				continue;
			chars[id].isset = true;
			chars[id].width = right-left+1;
			chars[id].height = top-bottom+1;
			chars[id].yoffset = bottom;
			if (bottom < lowest)
				lowest = bottom;
			if (chars[id].height > height)
				height = chars[id].height;
			chars[id].texcoords.Set(left,bottom,right,top);
			chars[id].texcoords.x /= (float)(image.width()-1);
			chars[id].texcoords.y /= (float)(image.height()-1);
		}
		for (unsigned i = 0; i < ARRAYSIZE(chars); i++)
			chars[i].yoffset -= lowest;
			
		//ShowMessage(String(lower_edge)+" . "+String(upper_edge));
	
		glGenTextures(1,&texture);

		if (!texture)
			throw Renderer::TextureTransfer::GeneralFault("OpenGL refuses to create texture handle (glGetError()='"+String(glGetError())+")");

		glBindTexture(GL_TEXTURE_2D,texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		gluBuild2DMipmaps(GL_TEXTURE_2D,4,image.getWidth(),image.getHeight(),GL_RGBA,GL_UNSIGNED_BYTE,image.getData());
		glBindTexture(GL_TEXTURE_2D,0);
		
		base = glGenLists(0x100);

		if (!base)
			throw Renderer::GeometryTransfer::GeneralFault("OpenGL refuses to create display list (glGetError()='"+String(glGetError())+")");


		ASSERT__(base > 0);
		if (chars[(BYTE)'i'].isset && !chars[(BYTE)' '].isset)
		{
			TCharacter&space = chars[(BYTE)' '];
			space.isset = true;
			space.texcoords.SetMinAndMax(0);
			space.width = chars[(BYTE)'i'].width;
			space.height = 1;
			space.yoffset = 0;
		}
		
		for (index_t i = 0; i < ARRAYSIZE(chars); i++)
			if (chars[i].isset)
			{
				GLuint local = base+(BYTE)i;

	            glNewList(local,GL_COMPILE);
	            
	                glBegin(GL_QUADS);
	                    glTexCoord2f(chars[i].texcoords.x.min,chars[i].texcoords.y.min);   	glVertex2f(0,scale*chars[i].yoffset);
	                    glTexCoord2f(chars[i].texcoords.x.max,chars[i].texcoords.y.min);	glVertex2f(scale*chars[i].width,scale*chars[i].yoffset);
	                    glTexCoord2f(chars[i].texcoords.x.max,chars[i].texcoords.y.max);   	glVertex2f(scale*chars[i].width,scale*(chars[i].yoffset+chars[i].height));
	                    glTexCoord2f(chars[i].texcoords.x.min,chars[i].texcoords.y.max);   	glVertex2f(0,scale*(chars[i].yoffset+chars[i].height));
	                glEnd();
	                glTranslatef(scale*chars[i].width,0,0);
	            glEndList();
	        }
	}

	float GLTextureFont2::getWidth(const char*str)
	{
		unsigned rs = 0;
		while (*str)
		{
			if (chars[(BYTE)*str].isset)
				rs += chars[(BYTE)*str].width;
			str++;
		}
	    return rs*scale;
	}

	float GLTextureFont2::getWidth(const char*str, size_t len)
	{
		float rs = 0;
		for (index_t i = 0; i < len; i++)
		{
			BYTE index = (BYTE)str[i];
			if (chars[index].isset)
				rs += chars[index].width;
		}
	    return rs*scale;
	}

	float GLTextureFont2::getWidth(char c)
	{
	    return chars[(BYTE)c].isset?scale*chars[(BYTE)c].width:0;
	}

	void GLTextureFont2::begin(const TFontState&state)
	{
	    glPushMatrix();
	        glTranslatef(state.left+state.indent,state.top,state.depth);
	        glTranslatef(0,-state.lineOffset,0);
	        glScalef(state.x_scale,state.y_scale,1);
	        glTranslatef(0,-1,0);
	        glColor4fv(state.v);
	        glPushAttrib(GL_LIST_BIT);
	    	glListBase(base);
	    	glEnable(GL_TEXTURE_2D);
	    	glBindTexture(GL_TEXTURE_2D,texture);
	}

	void GLTextureFont2::alterColor(const TFontColor&color)
	{
	        glColor4fv(color.v);
	}

	void GLTextureFont2::write(const char*str)
	{
			write(str,strlen(str));
	}

	void GLTextureFont2::write(const char*str, size_t len)
	{
		for (index_t i = 0; i < len; i++)
		{
			BYTE at= str[i];
			if (chars[at].isset)
				glCallList(base + at);
			else
				glCallList(base + (BYTE)'?');
		}
	        //glCallLists(GLuint(len),GL_UNSIGNED_BYTE,str);
	}

	void GLTextureFont2::end()
	{
	        glPopAttrib();
	    glPopMatrix();
	}
}
