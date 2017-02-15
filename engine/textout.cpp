#include "../global_root.h"
#include "textout.h"

/******************************************************************

Font-dependent textout-manager. The appearance of the
resulting render depends on the used font.

******************************************************************/

namespace Engine
{

	VirtualTextout::VirtualTextout():stackDepth(0),colorStackDepth(0)
	{
		Vec::set(state,1);
	    state.left = 0;
	    state.top = 1;
		state.depth = 0;
	    state.x_scale = 1;
	    state.y_scale = 1;
	    state.lineOffset = 0;
		state.indent = 0;
	}

	void VirtualTextout::NewLine()
	{
		state.indent = 0;
		state.lineOffset += QueryScaledHeight();
	}

	void	VirtualTextout::Tint(float red, float green, float blue)
	{
		state.red *= red;
		state.green *= green;
		state.blue *= blue;
	}


	void	VirtualTextout::SetColor(const TVec3<>&color)
	{
		state.rgb = color;
		state.alpha = 1;
	}
	void	VirtualTextout::SetColor(const TVec3<double>&color)
	{
		Vec::copy(color,state.rgb);
		state.alpha = 1;
	}
	void	VirtualTextout::SetColor(const TVec3<>&color,float alpha)
	{
		state.rgb = color;
		state.alpha = alpha;
	}


	void	VirtualTextout::SetColor(const TVec3<double>&color, double alpha)
	{
		Vec::copy(color,state.rgb);
		state.alpha = alpha;
	}

	void	VirtualTextout::SetColor(const TVec4<>&color)
	{
		Vec::copy(color,state);
	}

	void	VirtualTextout::SetColor(const TVec4<double>&color)
	{
		Vec::copy(color,state);
	}

	
	void VirtualTextout::SetColor(float red, float green, float blue, float alpha)
	{
	    Vec::def(state,red,green,blue,alpha);
	}

	void VirtualTextout::SetColor4fv(const float*color)
	{
		Vec::copy(Vec::ref4(color),state);
	}

	void VirtualTextout::SetColor4dv(const double*color)
	{
		Vec::copy(Vec::ref4(color),state);
	}

	void VirtualTextout::SetColor(float red, float green, float blue)
	{
	    Vec::def(state,red,green,blue,1);
	}

	void VirtualTextout::SetColor3fv(const float*color)
	{
		Vec::copy(Vec::ref3(color),state.rgb);
	    state.alpha = 1;
	}

	void VirtualTextout::SetColor3dv(const double*color)
	{
		Vec::copy(Vec::ref3(color),state.rgb);
	    state.alpha = 1;
	}

	void VirtualTextout::SetColor3fv(const float*color, float alpha)
	{
		Vec::copy(Vec::ref3(color),state.rgb);
	    state.alpha = alpha;
	}

	void VirtualTextout::SetColor3dv(const double*color, float alpha)
	{
		Vec::copy(Vec::ref3(color),state.rgb);
	    state.alpha = alpha;
	}
	
	
	
	

	void VirtualTextout::MoveBy(float x, float y, float z)
	{
	    state.left += x;
	    state.top += y;
		state.depth += z;
		state.lineOffset = 0;
		state.indent = 0;
	}


	void VirtualTextout::MoveTo(float x, float y, float z)
	{
	    state.left = x;
	    state.top = y;
		state.depth = z;
		state.lineOffset = 0;
		state.indent = 0;
	}

	void VirtualTextout::MoveTo(const TVec3<>&p)
	{
		MoveTo(p.x,p.y,p.z);
	}

	void VirtualTextout::MoveTo(const TVec2<>&p)
	{
		MoveTo(p.x,p.y);
	}



	void VirtualTextout::MoveTo(const float p[2])
	{
		MoveTo(p[0],p[1]);
	}

	void VirtualTextout::MoveTo3fv(const float p[3])
	{
		MoveTo(p[0],p[1],p[2]);
	}

	void VirtualTextout::SetScale(float x, float y)
	{
	    state.x_scale=x;
	    state.y_scale=y;
	}

	void VirtualTextout::Scale(float by_x, float by_y)
	{
	    state.x_scale*=by_x;
	    state.y_scale*=by_y;
	}


	void VirtualTextout::PushState()
	{
		DBG_ASSERT__(stackDepth < 0xFF);
	    stack[stackDepth++] = state;
	}

	void VirtualTextout::PopState()
	{
		DBG_ASSERT__(stackDepth > 0);
	    state = stack[--stackDepth];
	}


	void VirtualTextout::PushColorState()
	{
	    colorStack[colorStackDepth++] = state;
	}

	void VirtualTextout::PopColorState()
	{
	    ((TFontColor&) state) = colorStack[--colorStackDepth];
	}
	
	void VirtualTextout::SetLine(int line_)
	{
	    state.lineOffset = float(line_) * QueryScaledHeight();
	}

	void VirtualTextout::SetLineOffset(float offset)
	{
	    state.lineOffset = offset;
	}

	float VirtualTextout::GetLineOffset()	const
	{
	    return state.lineOffset;
	}

	

	

	void	VirtualTextout::Write(const StringRef&str)
	{
		if (str.GetLength()==0)
			return;
		BeginOutput();
		const char*at = str.pointer(),
				*const end=at+str.GetLength();
		while (at<end)
		{
			size_t len(0);
			while (at+len<end && at[len] != '\n')
				len++;
			WriteSegment(at,len);
			at+=len;
			if (at<end)
			{
				state.lineOffset += QueryScaledHeight();
				EndOutput();
				BeginOutput();
				at++;
			}
		}
		EndOutput();
	}

	void	VirtualTextout::Write(const char*str)
	{
	    if (!str)
	        return;
	    BeginOutput();
	    const char*at = str;
	    while (*at)
	    {
	        size_t len(0);
	        while (at[len] && at[len] != '\n')
	            len++;
	        WriteSegment(at,len);
	        at+=len;
	        if (*at)
	        {
	            state.lineOffset+=QueryScaledHeight();
	            EndOutput();
	            BeginOutput();
	            at++;
	        }
	    }
	    EndOutput();
	}

	void	VirtualTextout::Write(const String&str)
	{
	    Write(str.ref());
	}

	void	VirtualTextout::WriteTagged(const char*str, char tag)
	{
	    if (!str)
	        return;
	    TFontColor 	color_state = state;
	    TFontColor	color_stack[0x100];
	    BYTE        color_depth(0);

	    BeginOutput();
	    const char*at = str;
	    while (*at)
	    {
	        size_t l(0);
	        while (at[l] && at[l] != '\n' && at[l] != tag)
	            l++;
	        WriteSegment(at,l);
	        at+=l;
	        if ((*at) == '\n')
	        {
	            state.lineOffset += QueryScaledHeight();
	            EndOutput();
	            BeginOutput();
	            AlterColor(color_state);
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
	                        AlterColor(color_state);
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
	                            AlterColor(color_state);
	                        }
	            }
	    }
	    EndOutput();
	}

	void VirtualTextout::WriteTagged(const String&str, char tag)
	{
	    WriteTagged(str.c_str(),tag);
	}


	void VirtualTextout::WriteLn(const char*str)
	{
		Write(str);
		state.lineOffset += QueryScaledHeight();
	}

	void VirtualTextout::WriteLn(const String&str)
	{
		Write(str);
		state.lineOffset += QueryScaledHeight();
	}

	void VirtualTextout::WriteTaggedLine(const char*str, char tag)
	{
		WriteTagged(str,tag);
		state.lineOffset += QueryScaledHeight();
	}

	void VirtualTextout::WriteTaggedLine(const String&str, char tag)
	{
		WriteTagged(str,tag);
		state.lineOffset += QueryScaledHeight();
	}
	
	float VirtualTextout::QueryScaledWidth(const char*line) const
	{
		return QueryUnscaledWidth(StringRef(line))*state.x_scale;
	}

	float VirtualTextout::QueryScaledWidth(const String&line) const
	{
		return QueryUnscaledWidth(line.ref())*state.x_scale;
	}
	float VirtualTextout::QueryScaledWidth(const StringRef&line) const
	{
		return QueryUnscaledWidth(line)*state.x_scale;
	}

	float VirtualTextout::QueryScaledHeight() const
	{
		return QueryUnscaledHeight() * state.y_scale;
	}

	float			QueryScaledHeight();										//!< Determine the height of a string \return Scaled height of a single line



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
