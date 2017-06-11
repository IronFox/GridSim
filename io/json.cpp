#include "json.h"
#include <string/string_buffer.h>

namespace JSON
{
	typedef StringEncoding::UTF8Char	UTF8Char;
	typedef StringEncoding::UTF16Char	UTF16Char;

	String Entity::CastToString() const
	{
		switch (type)
		{
			case Int64:
				return String(intBoolValue);
			case Double:
				return String(doubleValue);
			case StringValue:
				return stringValue;
			case Bool:
				return String(intBoolValue!=0);
			default:
				return "";	//no useful conversion
		}
	}
	INT64 Entity::CastToInt() const
	{
		switch (type)
		{
			case Int64:
			case Bool:
				return intBoolValue;
			case Double:
				return (INT64)doubleValue;
			case StringValue:
			{
				INT64 rs=0;
				convert(stringValue.c_str(),stringValue.length(),rs);
				return rs;
			}
			default:
				return 0;	//no useful conversion
		}	
	}

	double Entity::CastToDouble() const
	{
		switch (type)
		{
			case Int64:
			case Bool:
				return (double)intBoolValue;
			case Double:
				return doubleValue;
			case StringValue:
			{
				double rs=0;
				convert(stringValue.c_str(),stringValue.length(),rs);
				return rs;
			}
			default:
				return 0;	//no useful conversion
		}	
	}

	void Entity::Clear()
	{
		stringValue.Clear();
		arrayValue.Clear();
		objectValue.reset();
		intBoolValue = 0;
		doubleValue = 0;
		type = Null;
	}

	Object & Entity::SetObject()
	{
		Clear();
		type = ObjectValue;
		objectValue.reset(new Object());
		return *objectValue;
	}

	bool Matches(const UTF8Char&c0, char c1)
	{
		return c0.numCharsUsed == 1 && c0.encoded[0] == c1;
	}


	static void SearchForASCII(IReadStream & stream, char c, LineCounter&lineCounter)
	{
		UTF8Char ch;
		for(;;)
		{
			StringEncoding::Deserialize(stream,ch);
			lineCounter.Feed(ch);

			if (ch.numCharsUsed == 1 && ch.encoded[0] == c)
				return;
		}
	}

	static UTF8Char SearchFor(IReadStream & stream, const std::function<bool(const UTF8Char&)>&f, LineCounter&lineCounter)
	{
		UTF8Char ch;
		for(;;)
		{
			StringEncoding::Deserialize(stream,ch);
			lineCounter.Feed(ch);
			if (f(ch))
				return ch;
		}
	}

	static String		ReadString(IReadStream&stream, LineCounter&lineCounter)
	{
		UTF8Char ch;
		bool escaped = false;
		StringBuffer temp;
		for(;;)
		{
			StringEncoding::Deserialize(stream,ch);
			lineCounter.Feed(ch);
			if (ch.numCharsUsed != 1)
			{
				escaped = false;
				for (BYTE i = 0; i < ch.numCharsUsed; i++)
					temp << ch.encoded[i];
				continue;
			}
			switch (ch.encoded[0])
			{
				case '"':
					if (!escaped)
						return temp.CopyToString();
					break;
				case '\\':
					if (escaped)
						break;
					escaped = true;
					continue;
				case 'u':
					if (escaped)
					{
						escaped = false;
						UTF16Char ch16;
						char32_t unicode = 0;
						for (int c = 0; c < 2; c++)
						{
							if (c == 1)
							{
								StringEncoding::Deserialize(stream,ch);
								if (!Matches(ch,'\\'))
									throw Except::Program::DataConsistencyFault(CLOCATION,"\\uXXXX\\uYYYY sequence expected intermediate \\u in line "+String(lineCounter.inLine));
								StringEncoding::Deserialize(stream,ch);
								if (!Matches(ch,'u'))
									throw Except::Program::DataConsistencyFault(CLOCATION,"\\uXXXX\\uYYYY sequence expected intermediate \\u in line "+String(lineCounter.inLine));
							}
							for (int k = 0; k < 4; k++)
							{
								StringEncoding::Deserialize(stream,ch);
								if (ch.numCharsUsed > 1)
									throw Except::Program::DataConsistencyFault(CLOCATION,"\\uXXXX\\uYYYY sequence expected four hexadecimal number characters, but got non-ASCII character in sequence in line "+String(lineCounter.inLine));

								ch16.encoded[c] <<= 4;
								if (ch.encoded[0] >= '0' && ch.encoded[0] <= '9')
								{
									ch16.encoded[c] |= (ch.encoded[0] - '0');
									continue;
								}
								if (ch.encoded[0] >= 'a' && ch.encoded[0] <= 'f')
								{
									ch16.encoded[c] |= ((ch.encoded[0] - 'a')+10);
									continue;
								}
								if (ch.encoded[0] >= 'A' && ch.encoded[0] <= 'F')
								{
									ch16.encoded[c] |= ((ch.encoded[0] - 'a')+10);
									continue;
								}
								throw Except::Program::DataConsistencyFault(CLOCATION,"\\uXXXX\\uYYYY sequence expected four hexadecimal number characters, but got non-hexadecimal number in sequence in line "+String(lineCounter.inLine));
							}
							ch16.numCharsUsed ++;

							if (StringEncoding::IsValid(ch16))
								break;
						}
						StringEncoding::EncodeUnicode(StringEncoding::ToUnicode(ch16),ch);
						for (BYTE i = 0; i < ch.numCharsUsed; i++)
							temp << ch.encoded[i];

						continue;
					}
					break;
			}
			temp << ch.encoded[0];
			escaped = false;
		}
	}


	static void					DeserializePrimitive(Entity&e,char firstChar,IReadStream&stream,LineCounter&counter)
	{
		StringBuffer word;
		word << firstChar;
		for (;;)
		{
			UTF8Char ch;
			StringEncoding::Deserialize(stream,ch);
			counter.Feed(ch);
			if (ch.numCharsUsed == 1 && (isWhitespace(ch.encoded[0])) || ch.encoded[0] == ',')
				break;

			for (BYTE i = 0; i < ch.numCharsUsed; i++)
				word << ch.encoded[i];
		}

		StringRef myWord = word.ToStringRef();
		bool b;
		INT64 in;
		double d;
		if (myWord == "null")
			e.SetNull();
		elif (convert(myWord.pointer(),myWord.GetLength(),b))
			e.Set(b);
		elif (convert(myWord.pointer(),myWord.GetLength(),in))
			e.Set(in);
		elif (convert(myWord.pointer(),myWord.GetLength(),d))
			e.Set(d);
		else
			throw Except::Program::DataConsistencyFault(CLOCATION,"'"+String(myWord)+"' is no valid JSON value");
	}

	static	void				DeserializeArray(Array<Entity>&target, IReadStream&stream,LineCounter&lineCounter);

	static void DeserializeAny(Entity&e, const UTF8Char&firstChar, IReadStream&stream,LineCounter&counter)
	{
		if (firstChar.numCharsUsed != 1)	//requiring control char now
			throw Except::Program::DataConsistencyFault(CLOCATION,"Expecting control char in line "+String(counter.inLine));
		if (firstChar.encoded[0] == '"')
			e.Set(ReadString(stream,counter));
		elif (firstChar.encoded[0] == '{')
			e.SetObject().Deserialize(stream,counter);
		elif (firstChar.encoded[0] == '[')
			DeserializeArray(e.SetArray(),stream,counter);
		else
			DeserializePrimitive(e,firstChar.encoded[0],stream,counter);
	}


	static	void				DeserializeArray(Array<Entity>&target, IReadStream&stream,LineCounter&lineCounter)
	{
		Buffer0<Entity,Swap> tmp;
		UTF8Char ch;
		for (;;)
		{
			for (;;)
			{
				StringEncoding::Deserialize(stream,ch);
				lineCounter.Feed(ch);
				if (ch.numCharsUsed != 1 || !isWhitespace(ch.encoded[0]))	//search through whitespace
					break;
			}
			if (Matches(ch,']'))
			{
				tmp.MoveToArray(target);
				return;
			}

			Entity&e = tmp.Append();

			DeserializeAny(e,ch,stream,lineCounter);

			UTF8Char ch = SearchFor(stream,[](const UTF8Char&ch){return Matches(ch,',') || Matches(ch,']');},lineCounter);
			if (Matches(ch,']'))
			{
				tmp.MoveToArray(target);
				return;
			}
		}
	}


	void Object::Deserialize(IReadStream & stream)
	{
		LineCounter counter;
		SearchForASCII(stream,'{',counter);
		Deserialize(stream,counter);
	}

	void Object::Deserialize(IReadStream & stream,LineCounter&counter)
	{
		
		//SearchForASCII(stream,'{',counter);
		
		for (;;)
		{
			UTF8Char ch = SearchFor(stream,[](const UTF8Char&ch){return Matches(ch,'\"') || Matches(ch,'}');},counter);
			if (Matches(ch,'}'))
				return;
			String key = ReadString(stream,counter);
			SearchForASCII(stream,':',counter);
			for (;;)
			{
				StringEncoding::Deserialize(stream,ch);
				counter.Feed(ch);
				if (ch.numCharsUsed != 1 || !isWhitespace(ch.encoded[0]))	//search through whitespace
					break;
			}

			Entity&e = entities.Set(key);

			DeserializeAny(e,ch,stream,counter);

			ch = SearchFor(stream,[](const UTF8Char&ch){return Matches(ch,',') || Matches(ch,'}');},counter);
			if (Matches(ch,'}'))
				return;
		}
	}
}
