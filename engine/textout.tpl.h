#ifndef engine_textoutTplH
#define engine_textoutTplH

/******************************************************************

Font-dependent textout-manager. The appearance of the
resulting render depends on the used font.

******************************************************************/


namespace Engine
{

	template <class Font>
		void		Textout<Font>::beginOutput()
		{
			active_font->begin(state);
		}
		
	template <class Font>
		void		Textout<Font>::writeSegment(const char*field, size_t len)
		{
			active_font->write(field,len);
		}
		
	template <class Font>
		void		Textout<Font>::endOutput()
		{
			active_font->end();
		}
		
	template <class Font>
		void		Textout<Font>::alterColor(const TFontColor&color)
		{
			active_font->alterColor(color);
		}
		

	template <class Font> Textout<Font>::Textout():active_font(&local_font)
	{}


	
	
	
	
	

	template <class Font> void Textout<Font>::setFont(Font*font_)
	{
	    if (font_)
	        active_font = font_;
	    else
	        active_font = &local_font;
	}
	
	
	

	template <class Font> Font& Textout<Font>::getFont()
	{
	    return local_font;
	}

	template <class Font> Font* Textout<Font>::getActiveFont()
	{
	    return active_font;
	}

	
	
	
	template <class Font> float Textout<Font>::unscaledLength(const char*line)
	{
	    return active_font->getWidth(line);
	}

	template <class Font> float Textout<Font>::unscaledLength(const char*line, size_t len)
	{
	    return active_font->getWidth(line,len);
	}

	template <class Font> float Textout<Font>::unscaledLength(const String&line)
	{
	    return active_font->getWidth(line.c_str(),line.length());
	}
	template <class Font> float Textout<Font>::unscaledLength(const StringRef&line)
	{
	    return active_font->getWidth(line.pointer(),line.length());
	}

	template <class Font> float Textout<Font>::scaledLength(const char*line)
	{
	    return active_font->getWidth(line)*state.x_scale;
	}

	template <class Font> float Textout<Font>::scaledLength(const char*line, size_t len)
	{
	    return active_font->getWidth(line,len)*state.x_scale;
	}

	template <class Font> float Textout<Font>::scaledLength(const String&line)
	{
	    return active_font->getWidth(line.c_str(),line.length())*state.x_scale;
	}
	template <class Font> float Textout<Font>::scaledLength(const StringRef&line)
	{
	    return active_font->getWidth(line.pointer(),line.length())*state.x_scale;
	}
	
	
	template <class Font> float Textout<Font>::getUnscaledWidth(const char*line)
	{
	    return active_font->getWidth(line);
	}

	template <class Font> float Textout<Font>::getUnscaledWidth(const char*line, size_t len)
	{
	    return active_font->getWidth(line,len);
	}
	
	
	template <class Font> float Textout<Font>::unscaledWidth(const char*line)
	{
	    return active_font->getWidth(line);
	}

	template <class Font> float Textout<Font>::unscaledWidth(const char*line, size_t len)
	{
	    return active_font->getWidth(line,len);
	}

	template <class Font> float Textout<Font>::getUnscaledHeight()
	{
		return active_font->getHeight();
	}


	template <class Font> float Textout<Font>::unscaledWidth(const String&line)
	{
	    return active_font->getWidth(line.c_str(),line.length());
	}
	template <class Font> float Textout<Font>::unscaledWidth(const StringRef&line)
	{
	    return active_font->getWidth(line.pointer(),line.length());
	}

	template <class Font> float Textout<Font>::scaledWidth(const char*line)
	{
	    return active_font->getWidth(line)*state.x_scale;
	}

	template <class Font> float Textout<Font>::scaledWidth(const char*line, size_t len)
	{
	    return active_font->getWidth(line,len)*state.x_scale;
	}

	template <class Font> float Textout<Font>::scaledWidth(const String&line)
	{
	    return active_font->getWidth(line.c_str(),line.length())*state.x_scale;
	}
	template <class Font> float Textout<Font>::scaledWidth(const StringRef&line)
	{
	    return active_font->getWidth(line.pointer(),line.length())*state.x_scale;
	}
	
	
	
	template <class Font>
		void	Textout<Font>::print(const char*str, size_t len)
		{
			if (!len)
				return;
		    active_font->begin(state);
		    const char*at = str,*end=str+len;
		    while (at<end)
		    {
		        size_t len(0);
		        while (at+len<end && at[len] != '\n')
		            len++;
		        active_font->write(at,len);
		        at+=len;
		        if (at<end)
		        {
		            active_font->end();
					newLine();
		            active_font->begin(state);
		            at++;
		        }
		    }
		    active_font->end();
		}

	template <class Font> void Textout<Font>::print(const char*str)
	{
	    if (!str)
	        return;
	    active_font->begin(state);
	    const char*at = str;
	    while (*at)
	    {
	        size_t len(0);
	        while (at[len] && at[len] != '\n')
	            len++;
	        active_font->write(at,len);
	        at+=len;
	        if (*at)
	        {
	            active_font->end();
				newLine();
	            active_font->begin(state);
	            at++;
	        }
	    }
	    active_font->end();
	}

	template <class Font>
		void	Textout<Font>::stream(const char*str, size_t len)
		{
			if (!len)
				return;
		    active_font->begin(state);
		    const char*at = str,*end=str+len;
		    while (at<end)
		    {
		        size_t len(0);
		        while (at+len<end && at[len] != '\n')
		            len++;
		        active_font->write(at,len);
				if (at+len >= end)
				{
					state.indent += scaledWidth(at,len);
				}
		        at+=len;
		        if (at<end)
		        {
		            active_font->end();
					newLine();
		            active_font->begin(state);
		            at++;
		        }
		    }
		    active_font->end();
		}

	template <class Font> void Textout<Font>::print(const String&str)
	{
	    print(str.c_str(),str.length());
	}
	template <class Font> void Textout<Font>::print(const StringRef&str)
	{
	    print(str.pointer(),str.length());
	}

	template <class Font> void Textout<Font>::printTagged(const char*str, char tag)
	{
	    if (!str)
	        return;
	    TFontColor 	color_state = state;
	    TFontColor	color_stack[0x100];
	    BYTE        color_depth(0);

	    active_font->begin(state);
	    const char*at = str;
	    while (*at)
	    {
	        size_t l(0);
	        while (at[l] && at[l] != '\n' && at[l] != tag)
	            l++;
	        active_font->write(at,l);
	        at+=l;
	        if ((*at) == '\n')
	        {
	            active_font->end();
				newLine();
	            active_font->begin(state);
	            active_font->alterColor(color_state);
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
	                        active_font->alterColor(color_state);
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
	                            active_font->alterColor(color_state);
	                        }
	            }
	    }
	    active_font->end();
	}

	template <class Font> void Textout<Font>::printTagged(const String&str, char tag)
	{
	    printTagged(str.c_str(),tag);
	}


	template <class Font> void Textout<Font>::println(const char*str)
	{
		print(str);
		newLine();
	}

	template <class Font> void Textout<Font>::println(const String&str)
	{
		print(str);
		newLine();
	}
	template <class Font> void Textout<Font>::println(const StringRef&str)
	{
		print(str);
		newLine();
	}

	template <class Font> void Textout<Font>::printTaggedLine(const char*str, char tag)
	{
		printTagged(str,tag);
		newLine();
	}

	template <class Font> void Textout<Font>::printTaggedLine(const String&str, char tag)
	{
		printTagged(str,tag);
		newLine();
	}



	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(const String&str)
		{
			stream(str.c_str(),str.length());
			return *this;
		}
	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(const StringRef&str)
		{
			stream(str.pointer(),str.length());
			return *this;
		}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(const char*str)
		{
			stream(str,strlen(str));
			return *this;
		}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(const TNewLine&)
		{
			newLine();
			return *this;
		}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(const TSpace&space)
		{
			if (!space.length)
				return *this;
			static const char blank = ' ';
			state.indent += scaledWidth(&blank,1)*space.length;
			return *this;
		}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(const TTabSpace&space)
		{
			if (!space.length)
				return *this;
			static const char blank = ' ';
			float tab_width = scaledWidth(&blank,1)*4;
			state.indent = (ceil(state.indent/tab_width)+space.length-1)*tab_width;
			return *this;
		}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(char c)
		{
			if (c == '\n')
			{
				newLine();
				return *this;
			}
		    active_font->begin(state);
		        active_font->write(&c,1);
		    active_font->end();
			state.indent += scaledWidth(&c,1);
			return *this;
		}
	

	template <class Font>
		template <typename T>	
			void Textout<Font>::streamUnsigned(T value)
			{
				char	char_buffer[256],
						*end = char_buffer+ARRAYSIZE(char_buffer),
						*c = end;
				while (value && c != char_buffer)
				{
					(*(--c)) = (char)('0'+(value%10));
					value/=10;
				}
				if (c==end)
					(*(--c)) = (char)'0';

				active_font->begin(state);
					active_font->write(c,end-c);
				active_font->end();
				state.indent += scaledWidth(c,end-c);
			}

	template <class Font>
		template <typename T, typename UT>
			void Textout<Font>::streamSigned(T value)
			{
				bool negative = false;
				if (value < 0)
				{
					value*=-1;
					negative = true;
				}

				char	char_buffer[256],
						*end = char_buffer+ARRAYSIZE(char_buffer),
						*c = end;
				UT uval = value;
				while (uval && c != char_buffer)
				{
					(*(--c)) = (char)('0'+(uval%10));
					uval/=10;
				}
				if (c==end)
					(*(--c)) = (char)'0';

				active_font->begin(state);
					if (negative)
						active_font->write("-",1);
					active_font->write(c,end-c);
				active_font->end();
				state.indent += scaledWidth(c,end-c);
				if (negative)
					state.indent += scaledWidth("-",1);
			}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(BYTE b)
		{
			streamUnsigned(b);
			return *this;
		}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(long long ll)
		{
			streamSigned<long long, unsigned long long>(ll);
			return *this;
		}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(unsigned long long ll)
		{
			streamUnsigned(ll);
			return *this;
		}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(int i)
		{
			streamSigned<int,unsigned>(i);
			return *this;
		}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(unsigned u)
		{
			streamUnsigned(u);
			return *this;
		}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(long l)
		{
			streamSigned<long, unsigned long>(l);
			return *this;
		}
		
	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(unsigned long l)
		{
			streamUnsigned(l);
			return *this;
		}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(short s)
		{
			streamSigned<short,unsigned short>(s);
			return *this;
		}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(unsigned short s)
		{
			streamUnsigned(s);
			return *this;
		}

	template <class Font>
		template <typename T>
			void Textout<Font>::streamFloat(T value, unsigned precision)
			{
				char	char_buffer[256],
						*str = String::floatToStr(value, precision, false, char_buffer+ARRAYSIZE(char_buffer), char_buffer);
				
				active_font->begin(state);
					active_font->write(str,char_buffer+ARRAYSIZE(char_buffer)-str);
				active_font->end();
				state.indent += scaledWidth(str,char_buffer+ARRAYSIZE(char_buffer)-str);
			}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(float f)
		{
			streamFloat(f,5);
			return *this;
		}

	template <class Font>
		Textout<Font>&	Textout<Font>::operator<<(double d)
		{
			streamFloat(d,8);
			return *this;
		}


}

#endif

