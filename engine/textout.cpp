#include "../global_root.h"
#include "textout.h"

/******************************************************************

Font-dependent textout-manager. The appearance of the
resulting render depends on the used font.

******************************************************************/

namespace Engine
{

	VirtualTextout::VirtualTextout():stack_depth(0)
	{
		Vec::set(state,1);
	    state.left = 0;
	    state.top = 1;
		state.depth = 0;
	    state.x_scale = 1;
	    state.y_scale = 1;
	    state.line = 0;
		state.indent = 0;
	}

	void VirtualTextout::newLine()
	{
		state.indent = 0;
		state.line++;
	}

	void	VirtualTextout::color(const TVec3<>&color)
	{
		state.rgb = color;
		state.alpha = 1;
	}
	void	VirtualTextout::color(const TVec3<double>&color)
	{
		Vec::copy(color,state.rgb);
		state.alpha = 1;
	}
	void	VirtualTextout::color(const TVec3<>&color,float alpha)
	{
		state.rgb = color;
		state.alpha = alpha;
	}


	void	VirtualTextout::color(const TVec3<double>&color, double alpha)
	{
		Vec::copy(color,state.rgb);
		state.alpha = alpha;
	}

	void	VirtualTextout::color(const TVec4<>&color)
	{
		Vec::copy(color,state);
	}

	void	VirtualTextout::color(const TVec4<double>&color)
	{
		Vec::copy(color,state);
	}

	
	void VirtualTextout::color(float red, float green, float blue, float alpha)
	{
	    Vec::def(state,red,green,blue,alpha);
	}

	void VirtualTextout::color4(const float*color)
	{
		Vec::copy(Vec::ref4(color),state);
	}

	void VirtualTextout::color4(const double*color)
	{
		Vec::copy(Vec::ref4(color),state);
	}

	void VirtualTextout::color(float red, float green, float blue)
	{
	    Vec::def(state,red,green,blue,1);
	}

	void VirtualTextout::color3(const float*color)
	{
		Vec::copy(Vec::ref3(color),state.rgb);
	    state.alpha = 1;
	}

	void VirtualTextout::color3(const double*color)
	{
		Vec::copy(Vec::ref3(color),state.rgb);
	    state.alpha = 1;
	}

	void VirtualTextout::color3a(const float*color, float alpha)
	{
		Vec::copy(Vec::ref3(color),state.rgb);
	    state.alpha = alpha;
	}

	void VirtualTextout::color3a(const double*color, float alpha)
	{
		Vec::copy(Vec::ref3(color),state.rgb);
	    state.alpha = alpha;
	}
	
	
	
	

	void VirtualTextout::move(float x, float y, float z)
	{
	    state.left += x;
	    state.top += y;
		state.depth += z;
		state.line = 0;
		state.indent = 0;
	}


	void VirtualTextout::locate(float x, float y, float z)
	{
	    state.left = x;
	    state.top = y;
		state.depth = z;
		state.line = 0;
		state.indent = 0;
	}

	void VirtualTextout::locate(const TVec3<>&p)
	{
		locate(p.x,p.y,p.z);
	}

	void VirtualTextout::locate(const TVec2<>&p)
	{
		locate(p.x,p.y);
	}



	void VirtualTextout::locate(const float p[2])
	{
		locate(p[0],p[1]);
	}

	void VirtualTextout::locate3fv(const float p[3])
	{
		locate(p[0],p[1],p[2]);
	}

	void VirtualTextout::setScale(float x, float y)
	{
	    state.x_scale=x;
	    state.y_scale=y;
	}

	void VirtualTextout::scale(float by_x, float by_y)
	{
	    state.x_scale*=by_x;
	    state.y_scale*=by_y;
	}


	void VirtualTextout::pushState()
	{
	    stack[stack_depth++] = state;
	}

	void VirtualTextout::popState()
	{
	    state = stack[--stack_depth];
	}
	
	void VirtualTextout::line(unsigned line_)
	{
	    state.line = line_;
	}

	unsigned VirtualTextout::line()	const
	{
	    return state.line;
	}

	

	

	void	VirtualTextout::write(const char*str, size_t len)
	{
		if (!len)
			return;
		beginOutput();
		const char*at = str,*end=str+len;
		while (at<end)
		{
			size_t len(0);
			while (at+len<end && at[len] != '\n')
				len++;
			writeSegment(at,len);
			at+=len;
			if (at<end)
			{
				state.line++;
				endOutput();
				beginOutput();
				at++;
			}
		}
		endOutput();
	}

	void	VirtualTextout::write(const char*str)
	{
	    if (!str)
	        return;
	    beginOutput();
	    const char*at = str;
	    while (*at)
	    {
	        size_t len(0);
	        while (at[len] && at[len] != '\n')
	            len++;
	        writeSegment(at,len);
	        at+=len;
	        if (*at)
	        {
	            state.line++;
	            endOutput();
	            beginOutput();
	            at++;
	        }
	    }
	    endOutput();
	}

	void	VirtualTextout::write(const String&str)
	{
	    write(str.c_str(),str.length());
	}

	void	VirtualTextout::writeTagged(const char*str, char tag)
	{
	    if (!str)
	        return;
	    TFontColor 	color_state = state;
	    TFontColor	color_stack[0x100];
	    BYTE        color_depth(0);

	    beginOutput();
	    const char*at = str;
	    while (*at)
	    {
	        size_t l(0);
	        while (at[l] && at[l] != '\n' && at[l] != tag)
	            l++;
	        writeSegment(at,l);
	        at+=l;
	        if ((*at) == '\n')
	        {
	            state.line++;
	            endOutput();
	            beginOutput();
	            alterColor(color_state);
	            at++;
	        }
	        else
	            if ((*at) == tag)
	            {
	                at++;
	                size_t remaining = strlen(at);
	                if (remaining > 4 && !strncmpi(at,"push",4))
	                {
	                    color_stack[color_depth++] = color_state;
	                    at+=4;
	                }
	                else
	                    if (remaining > 3 && !strncmpi(at,"pop",3))
	                    {
	                        color_state = color_stack[--color_depth];
	                        alterColor(color_state);
	                        at+=3;
	                    }
	                    else
	                        if (remaining > 8)
	                        {
	                            color_state.red = (float)hexStr(at)/255;
	                            color_state.green = (float)hexStr(at+2)/255;
	                            color_state.blue = (float)hexStr(at+4)/255;
	                            color_state.alpha = (float)hexStr(at+6)/255;
	                            at+=8;
	                            alterColor(color_state);
	                        }
	            }
	    }
	    endOutput();
	}

	void VirtualTextout::writeTagged(const String&str, char tag)
	{
	    writeTagged(str.c_str(),tag);
	}


	void VirtualTextout::writeln(const char*str)
	{
		write(str);
		state.line++;
	}

	void VirtualTextout::writeln(const String&str)
	{
		write(str);
		state.line++;
	}

	void VirtualTextout::writeTaggedLine(const char*str, char tag)
	{
		writeTagged(str,tag);
		state.line++;
	}

	void VirtualTextout::writeTaggedLine(const String&str, char tag)
	{
		writeTagged(str,tag);
		state.line++;
	}
	
	float VirtualTextout::getUnscaledWidth(const String&line)
	{
		return getUnscaledWidth(line.c_str(),line.length());
	}
	
	float VirtualTextout::getScaledWidth(const char*line, size_t len)
	{
		return getUnscaledWidth(line,len)*state.x_scale;
	}
	
	float VirtualTextout::getScaledWidth(const char*line)
	{
		return getUnscaledWidth(line)*state.x_scale;
	}

	float VirtualTextout::getScaledWidth(const String&line)
	{
		return getUnscaledWidth(line.c_str(),line.length())*state.x_scale;
	}

	float VirtualTextout::getScaledHeight()
	{
		return getUnscaledHeight() * state.y_scale;
	}

	float			getScaledHeight();										//!< Determine the height of a string \return Scaled height of a single line



BYTE hexChar(char c)
{
    if (c>='0' && c <= '9')
        return c-0x30;
    if (c>='a' && c <= 'f')
        return 10+c-'a';
    if (c>='A' && c <= 'F')
        return 10+c-'A';
    return 0;
}


BYTE hexStr(const char*line)
{
    return hexChar(line[0])*0x10+hexChar(line[1]);
}

}
