#include "../../global_root.h"
#include "keyboard.h"


/******************************************************************

engine keyboard-interface.

******************************************************************/

namespace Engine
{


	Keyboard keyboard(input);

	
	void Keyboard::keyDown(Key::Name index)
	{
		map.keyDown(index);
	}

	void Keyboard::keyUp(Key::Name index)
	{
		map.keyUp(index);
	}
	
	char Keyboard::convertInput(char c)
	{
		c = translation[(BYTE)c];
		if (map.pressed[Key::Shift] && c>='a' && c <='z')
			c+='A'-'a';
		return c;
	}


	void Keyboard::input(char c)
	{
		if (read)
		{
			if (!block || timer.toSecondsf(timer.now() - block_start) > block_len)
			{
				block = false;
				
				c = translation[(BYTE)c];
				if (map.pressed[Key::Shift] && c>='a' && c <='z')
					c+='A'-'a';

				if (c)
				{
					if (reader)
						reader(c);
					else
					{
						buffer[buffer_len++] = c;
						if (onCharHandled)
							onCharHandled(c);
					}
				}
			}
		}
	}


	const char* Keyboard::getInput()
	{
		buffer[buffer_len] = 0;
		return buffer;
	}

	BYTE Keyboard::getInputLen()
	{
		return buffer_len;
	}


	void Keyboard::dropLastCharacter()
	{
		if (buffer_len)
			buffer_len--;
	}

	void Keyboard::truncateIfLonger(BYTE len)
	{
		if (buffer_len > len)
			buffer_len = len;
	}

	void Keyboard::fillInput(const String&strn)
	{
		size_t len = strn.length();
		if (len > 255)
			len = 255;
		buffer_len = (BYTE)len;
		memcpy(buffer,strn.c_str(),buffer_len);
	}

	void Keyboard::fillInput(const char*strn)
	{
		size_t len = strlen(strn);
		if (len > 255)
			len = 255;
		buffer_len = (BYTE)len;
		memcpy(buffer,strn,buffer_len);
	}

	void Keyboard::replaceInputSection(BYTE offset, BYTE len, const String&strn)
	{
		size_t insert_len = strn.length();
		if (offset > buffer_len)
			offset = buffer_len;
		char temp[0x100];
		unsigned end = (unsigned)offset+(unsigned)len,
				 new_end = (unsigned)offset+(unsigned)insert_len;
		if (end > buffer_len)
			end = buffer_len;
		if (new_end > 255)
		{
			insert_len = 255-offset;
			new_end = 255;
		}
		unsigned rest;
		if (end<buffer_len)
		{
			rest = (unsigned)buffer_len-end;
			memcpy(temp,buffer+end,rest);
		}
		else
			rest = 0;
		memcpy(buffer+offset,strn.c_str(),insert_len);
		if (new_end+rest > 255)
			rest = 255-new_end;
		if (rest)
			memcpy(buffer+new_end,temp,rest);
		buffer_len = new_end+rest;
	}

	void Keyboard::replaceInputSection(BYTE offset, BYTE len, const char*strn)
	{
		unsigned insert_len = (unsigned)strlen(strn);
		if (offset > buffer_len)
			offset = buffer_len;
		char temp[0x100];
		unsigned end = (unsigned)offset+(unsigned)len,
				 new_end = (unsigned)offset+insert_len;
		if (end > buffer_len)
			end = buffer_len;
		if (new_end > 255)
		{
			insert_len = 255-offset;
			new_end = 255;
		}
		unsigned rest;
		if (end<buffer_len)
		{
			rest = (unsigned)buffer_len-end;
			memcpy(temp,buffer+end,rest);
		}
		else
			rest = 0;
		memcpy(buffer+offset,strn,insert_len);
		if (new_end+rest > 255)
			rest = 255-new_end;
		if (rest)
			memcpy(buffer+new_end,temp,rest);
		buffer_len = new_end+rest;
	}



	void Keyboard::flushInput()
	{
		buffer_len = 0;
	}








	Keyboard::Keyboard(InputMap&map_):block(false),buffer_len(0),map(map_),read(false),reader(NULL),onCharHandled(NULL)
	{
		buffer[256] = 0;
		resetTranslation();
	}

	Keyboard::~Keyboard()
	{}

	void Keyboard::delayRead(float len)
	{
		block = true;
		block_len = len;
		block_start = timer.now();
	}




	void Keyboard::resetTranslation()
	{
		filter("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789°!\"§$%&/()=?´`{[]}\\#+'*~-_.:,;|<> ");

		translation[(BYTE)'â'] = 'a';
		translation[(BYTE)'ô'] = 'o';
		translation[(BYTE)'û'] = 'u';
		translation[(BYTE)'î'] = 'i';
		translation[(BYTE)'ê'] = 'e';
		translation[(BYTE)'Â'] = 'A';
		translation[(BYTE)'Ô'] = 'O';
		translation[(BYTE)'Û'] = 'U';
		translation[(BYTE)'Î'] = 'I';
		translation[(BYTE)'Ê'] = 'E';

		translation[(BYTE)'à'] = 'a';
		translation[(BYTE)'ò'] = 'o';
		translation[(BYTE)'ù'] = 'u';
		translation[(BYTE)'ì'] = 'i';
		translation[(BYTE)'è'] = 'e';
		translation[(BYTE)'À'] = 'A';
		translation[(BYTE)'Ò'] = 'O';
		translation[(BYTE)'Ù'] = 'U';
		translation[(BYTE)'Ì'] = 'I';
		translation[(BYTE)'È'] = 'E';
	}


	void Keyboard::filter(const char*characters)
	{
		memset(translation,0,sizeof(translation));
		unsigned index = 0;
		while (characters[index])
		{
			translation[(BYTE)characters[index]] = characters[index];
			index++;
		}
	}

	void Keyboard::allow(const char*characters)
	{
		unsigned index = 0;
		while (characters[index])
		{
			translation[(BYTE)characters[index]] = characters[index];
			index++;
		}
	}

	void Keyboard::disallow(const char*characters)
	{
		unsigned index = 0;
		while (characters[index])
			translation[(BYTE)characters[index++]] = 0;
	}

	void Keyboard::translate(const char*from, const char*to)
	{
		while (*from && *to)
			translation[(BYTE)*from++] = *to++;
	}


}
