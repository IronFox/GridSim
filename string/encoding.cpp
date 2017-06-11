#include "encoding.h"

namespace StringEncoding
{

	bool	IsValidUnicode(char32_t c)
	{
		const UINT32 u = (UINT32)c;

		return u < 0xD800 || (u>=0xE000 && u <= 0x10FFFF);
	}

	static bool	CompleteUtf16(char16_t c, char32_t&out)
	{
		UINT16 header = ((UINT16)c) & 0xFC00;
		if (header != 0xD800)
		{
			out = c;
			return true;
		}
		return false;
	}

	static char32_t	Utf16ToUnicodeChar(char16_t c0, char16_t c1)
	{
		char32_t rs;
		UINT16 header0 = ((UINT16)c0) & 0xFC00;
		UINT16 header1 = ((UINT16)c1) & 0xFC00;
		if (header0 != 0xD800)
			throw Except::Program::DataConsistencyFault(CLOCATION,"Invalid UTF16 header #"+String(header0));
		if (header1 != 0xDC00)
			throw Except::Program::DataConsistencyFault(CLOCATION,"Invalid UTF16 header #"+String(header0));
		UINT16 high = (UINT16)c0 - 0xD800;
		UINT16 low = (UINT16)c1 - 0xDC00;

		UINT32 combined = (((UINT32)high) << 10) | low;
		return (char32_t)(combined + 0x10000);
	}

	
	template <typename Base>
		struct UTF16Code
		{
			typedef Base		CharType;
			typedef UTF16Char	EncodedType;


			static String	GetName() {return "UTF16";}

			static count_t	UnicodeToEncodedLength(char32_t unicode)
			{
				if (!IsValidUnicode(unicode))
					throw Except::Program::DataConsistencyFault(CLOCATION, GetName()+" encoding error. Given Unicode character #"+String(unicode)+" is not valid");

				if ((UINT32)unicode <= 0xFFFF)
					return 1;
				return 2;
			}


			static void		EncodeUnicode(char32_t unicode, EncodedType&rs)
			{
				UINT32 uni = unicode;
				if (!IsValidUnicode(unicode))
					throw Except::Program::DataConsistencyFault(CLOCATION, GetName()+" encoding error. Given Unicode character #"+String(unicode)+" is not valid");
				if (uni <= 0xFFFF)
				{
					rs.numCharsUsed = 1;
					rs.encoded[0] = (Base)uni;
				}
				else
				{
					uni -= 0x10000;
					rs.numCharsUsed = 2;
					rs.encoded[0] = (Base)((uni >> 10) + 0xD800);
					rs.encoded[1] = (Base)((uni & 0x3FF) + 0xDC00);
				}
				ASSERT_EQUAL__(DecipherVariableLength(rs.encoded[0]),rs.numCharsUsed);
			}

			static BYTE		DecipherVariableLength(CharType c)
			{
				UINT16 header = ((UINT16)c) & 0xFC00;
				if (header != 0xD800)
					return 1;
				return 2;
			}

			static char32_t		DeserializeUnicode(const CharType*&source)
			{
				UTF16Char ch;
				ch.numCharsUsed = DecipherVariableLength(*source);
				memcpy(ch.encoded,source,ch.numCharsUsed*sizeof(Base));
				source += ch.numCharsUsed;
				return ToUnicode(ch);
			}

		};


	struct UTF8Code
	{
		typedef char		CharType;
		typedef UTF8Char	EncodedType;

		static String	GetName() {return "UTF8";}


		static count_t	UnicodeToEncodedLength(char32_t unicode)
		{
			return EncodedUTF8Length(unicode);
		}

		static void		EncodeUnicode(char32_t unicode, EncodedType&rs)
		{
			::StringEncoding::EncodeUnicode(unicode,rs);
		}

		static BYTE		DecipherVariableLength(CharType c)
		{
			return UTF8Length(c);
		}

		static char32_t		DeserializeUnicode(const CharType*&source)
		{
			UTF8Char ch;
			ch.numCharsUsed = DecipherVariableLength(*source);
			memcpy(ch.encoded,source,ch.numCharsUsed);
			source += ch.numCharsUsed;
			return ToUnicode(ch);
		}

	};



	struct UnicodeFixedCode
	{
		typedef char32_t	CharType;

		static char32_t	ConvertUnicode(char32_t c) {return c;}
		static char32_t	ConvertToUnicode(char32_t c) {return c;}
	};

	struct CP1252FixedCode
	{
		typedef char	CharType;

		static char		ConvertUnicode(char32_t c) {return EncodeUnicodeToCP1252(c);}
		static char32_t	ConvertToUnicode(char c) {return ToUnicodeFromCP1252(c);}
	};


	template <typename FixedCode, typename Code>
		static void	FixedLengthToVariableLength(const StringType::ReferenceExpression<typename FixedCode::CharType>&fixedSource, StringType::Template<typename Code::CharType>&variableDest)
		{
			count_t len = 0;

			auto*s = fixedSource.pointer(),
				*const end = fixedSource.end();
			for (; s != end; ++s)
				len += Code::UnicodeToEncodedLength( FixedCode::ConvertToUnicode(*s) );

			variableDest.setLength(len);
			s = fixedSource.pointer();
			auto*out = variableDest.mutablePointer();
			for (; s != end; ++s)
			{
				typename Code::EncodedType encoded;
				Code::EncodeUnicode( FixedCode::ConvertToUnicode(*s),encoded);
				for (BYTE k = 0; k < encoded.numCharsUsed; k++)
				{
					*out = encoded.encoded[k];
					out++;
				}
			}
			ASSERT_EQUAL__(out,variableDest.end());
		}


	template <typename Code, typename FixedCode>
		static void	VariableLengthToFixedLength(const StringType::ReferenceExpression<typename Code::CharType>&variableSource, StringType::Template<typename FixedCode::CharType>&fixedDest)
		{
			count_t length = 0;
			{
				const auto*read = variableSource.pointer();
				const auto*const end = variableSource.end();
				while (read < end)
				{
					count_t len = Code::DecipherVariableLength(*read);
					read += len;
					if (read > end)
						throw Except::Program::DataConsistencyFault(CLOCATION, Code::GetName()+" decoding error. String incomplete");
					length++;
				}
			}

			fixedDest.setLength(length);

			auto*write = fixedDest.mutablePointer();
			{
				const auto*read = variableSource.pointer();
				const auto*const end = variableSource.end();
				while (read < end)
				{
					*write = FixedCode::ConvertUnicode( Code::DeserializeUnicode(read) );
					write++;
				}
				ASSERT_EQUAL__(read,end);
			}
			ASSERT_EQUAL__(write,fixedDest.end());
		}



	bool	Is1252MultiByte(char csource)
	{
		return ((signed char)csource) < 0;
	}


	BYTE	UTF8Length(char firstChar)
	{
		unsigned char usource = firstChar;
		BYTE numOnes = 0;
		while (usource & 0x80)	//look for first bit that is zero (length-terminator). may be first bit
		{
			usource <<= 1;	//otherwise shift up (replace test-bit with next in line)
			numOnes ++;	//increment length
		}
		if (numOnes == 1)	//not defined. encodings always either start with 0* or 11*, never 10* for the first char
			throw Except::Program::DataConsistencyFault(CLOCATION, "Invalid leading UTF-8 character "+DataToBinary(&firstChar,1)); //make sure nothing went wrong. In theory the length _could_ be 1+8, if the byte is 255, but that is not utf8
		
		const BYTE length = std::max<BYTE>(numOnes,1);
		if ((count_t)length > ARRAYSIZE(UTF8Char::encoded))
			throw Except::Program::DataConsistencyFault(CLOCATION, "Invalid leading UTF-8 character "+DataToBinary(&firstChar,1)); //make sure nothing went wrong. In theory the length _could_ be 1+8, if the byte is 255, but that is not utf8
		return length;
	}


	void	Deserialize(IReadStream&source, UTF8Char&utf8Dest)
	{
		count_t len = 0;
		unsigned char first;
		source.Read(&first,1);

		BYTE length = UTF8Length(first);
		utf8Dest.numCharsUsed = length;
		utf8Dest.encoded[0] = first;
		if (length > 1)
			source.Read(utf8Dest.encoded+1,length-1);
	}



	char32_t	ToUnicodeFromCP1252(char c)
	{
		if ((BYTE)c < 128 || (BYTE)c >= 160)
			return (char32_t)(BYTE)c;
		switch ((BYTE)c)
		{
			case 128:
				return 0x20AC;
			case 130:
				return 0x201A;
			case 131:
				return 0x0192;
			case 132:
				return 0x201E;
			case 133:
				return 0x2026;
			case 134:
				return 0x2020;
			case 135:
				return 0x2021;
			case 136:
				return 0x02C6;
			case 137:
				return 0x2030;
			case 138:
				return 0x0160;
			case 139:
				return 0x2039;
			case 140:
				return 0x0152;
			case 142:
				return 0x017D;
			case 145:
				return 0x2018;
			case 146:
				return 0x2019;
			case 147:
				return 0x201C;
			case 148:
				return 0x201D;
			case 149:
				return 0x2022;
			case 150:
				return 0x2013;
			case 151:
				return 0x2014;
			case 152:
				return 0x02DC;
			case 153:
				return 0x2122;
			case 154:
				return 0x0161;
			case 155:
				return 0x203A;
			case 156:
				return 0x0153;
			case 158:
				return 0x017E;
			case 159:
				return 0x0178;
			default:
				return (char32_t)(BYTE)c;
		};
	}

	char	EncodeUnicodeToCP1252(char32_t c)
	{
		if ((UINT32)c < 0x100)
		{
			return (char)(BYTE)(UINT32)c;
		}
		switch ((UINT32)c)
		{
			case 0x20AC:
				return (char)(BYTE)128;
			case 0x201A:
				return (char)(BYTE)130;
			case 0x0192:
				return (char)(BYTE)131;
			case 0x201E:
				return (char)(BYTE)132;
			case 0x2026:
				return (char)(BYTE)133;
			case 0x2020:
				return (char)(BYTE)134;
			case 0x2021:
				return (char)(BYTE)135;
			case 0x02C6:
				return (char)(BYTE)136;
			case 0x2030:
				return (char)(BYTE)137;
			case 0x0160:
				return (char)(BYTE)138;
			case 0x2039:
				return (char)(BYTE)139;
			case 0x0152:
				return (char)(BYTE)140;
			case 0x017D:
				return (char)(BYTE)142;
			case 0x2018:
				return (char)(BYTE)145;
			case 0x2019:
				return (char)(BYTE)146;
			case 0x201C:
				return (char)(BYTE)147;
			case 0x201D:
				return (char)(BYTE)148;
			case 0x2022:
				return (char)(BYTE)149;
			case 0x2013:
				return (char)(BYTE)150;
			case 0x2014:
				return (char)(BYTE)151;
			case 0x02DC:
				return (char)(BYTE)152;
			case 0x2122:
				return (char)(BYTE)153;
			case 0x0161:
				return (char)(BYTE)154;
			case 0x203A:
				return (char)(BYTE)155;
			case 0x0153:
				return (char)(BYTE)156;
			case 0x017E:
				return (char)(BYTE)158;
			case 0x0178:
				return (char)(BYTE)159;
		};

		if ((UINT32)c > 255)
			throw Except::Program::DataConsistencyFault(CLOCATION, "Unicode character is out of range for CP-1252");
		return (char)(BYTE)(UINT32)c;
	}

	char ToCP1252(const UTF8Char&ch)
	{
		if (!ch.numCharsUsed)
			return 0;
		return EncodeUnicodeToCP1252(
				ToUnicode(ch));
	}

	void	EncodeCP1252(char c, UTF8Char&ch)
	{
		EncodeUnicode(
			ToUnicodeFromCP1252(c),ch);
	}


	void	UTF8ToUnicode(const String&utf8Source, UnicodeString&unicodeDest)
	{
		UTF8ToUnicode(utf8Source.ref(),unicodeDest);
	}
	void	UnicodeToUTF8(const StringType::ReferenceExpression<char32_t>&unicodeSource, String&utf8Dest)
	{
		FixedLengthToVariableLength<UnicodeFixedCode,UTF8Code>(unicodeSource,utf8Dest);
	}

	void	UnicodeToUTF8(const UnicodeString&unicodeSource, String&utf8Dest)
	{
		UnicodeToUTF8(unicodeSource.ref(),utf8Dest);
	}



	void	UTF8ToUnicode(const StringRef&utf8Source, UnicodeString&unicodeDest)
	{
		VariableLengthToFixedLength<UTF8Code,UnicodeFixedCode>(utf8Source,unicodeDest);
	}





	char32_t	ToUnicode(const UTF16Char&c)
	{
		if (c.numCharsUsed == 0)
			return 0;

		if (c.numCharsUsed == 1)
		{
			char32_t rs;
			if (!CompleteUtf16(c.encoded[0],rs))
				throw Except::Program::DataConsistencyFault(CLOCATION,"Invalid UTF16 character "+BinaryToHex(&c.encoded[0],2));
			return rs;
		}

		return Utf16ToUnicodeChar(c.encoded[0],c.encoded[1]);
	}

	bool	IsValid(const UTF16Char&c)
	{
		if (c.numCharsUsed == 0)
			return true;

		if (c.numCharsUsed == 1)
		{
			char32_t rs;
			return CompleteUtf16(c.encoded[0],rs);
		}

		UINT16 header0 = ((UINT16)c.encoded[0]) & 0xFC00;
		UINT16 header1 = ((UINT16)c.encoded[1]) & 0xFC00;

		return header0 == 0xD800 && header1 == 0xDC00;
	}

	void	AssertValidity(const UTF16Char&c)
	{
		if (c.numCharsUsed == 0)
			return;

		if (c.numCharsUsed == 1)
		{
			UINT16 header = ((UINT16)c.encoded[0]) & 0xFC00;
			if (header != 0xD800)
				return;
			throw Except::Program::DataConsistencyFault(CLOCATION,"Only encoded UTF16 character contains reserved header");
		}

		UINT16 header0 = ((UINT16)c.encoded[0]) & 0xFC00;
		UINT16 header1 = ((UINT16)c.encoded[1]) & 0xFC00;
		if (header0 != 0xD800)
			throw Except::Program::DataConsistencyFault(CLOCATION,"First encoded UTF16 character contains invalid header");
		if (header1 != 0xDC00)
			throw Except::Program::DataConsistencyFault(CLOCATION,"Second encoded UTF16 character contains invalid header");
	}

	bool	IsValid(const UTF8Char&c)
	{
		if (c.numCharsUsed == 0)
			return true;

		const BYTE length = UTF8Length(c.encoded[0]);
		if (length != c.numCharsUsed)
			return false;
		for (BYTE k = 1; k < c.numCharsUsed; k++)
			if ((c.encoded[k] & 0xC0) != 0x80)
				return false;
		return true;
	}
	
	void	AssertValidity(const UTF8Char&c)
	{
		if (c.numCharsUsed == 0)
			return;

		const BYTE length = UTF8Length(c.encoded[0]);
		if (length != c.numCharsUsed)
			throw Except::Program::DataConsistencyFault(CLOCATION, "UTF8 Encoding error. Expected "+String(length)+" bytes in encoded string, but got "+String(c.numCharsUsed));
		for (BYTE k = 1; k < c.numCharsUsed; k++)
			if ((c.encoded[k] & 0xC0) != 0x80)
				throw Except::Program::DataConsistencyFault(CLOCATION, "UTF8 Encoding error. Expected following byte "+String(k)+" to contain 10xxxxxx header");
	}


	char32_t	ToUnicode(const UTF8Char&c)
	{
		if (c.numCharsUsed == 0)
			return 0;
		count_t len = 0;
		unsigned char first = (BYTE) c.encoded[0];
		

		const BYTE length = UTF8Length(first);
		if (length != c.numCharsUsed)
			throw Except::Program::DataConsistencyFault(CLOCATION, "UTF8 Encoding error. Expected "+String(length)+" bytes in encoded string, but got "+String(c.numCharsUsed));


		UINT32 rs = first & ((0xFF>>(length)));

		for (index_t at = 1; at < length; at++)
		{
			if ((c.encoded[at] & 0xC0) != 0x80)
				throw Except::Program::DataConsistencyFault(CLOCATION, "UTF8 Encoding error. Expected following byte "+String(at)+" to contain 10xxxxxx header (encountered "+DataToBinary(&c.encoded[at],1)+")");
			rs <<= 6;
			rs |= (c.encoded[at] & 0x3F);
		}
		
		return (char32_t)rs;
	}



	BYTE	EncodedUTF8Length(char32_t c)
	{
		UINT32 u = (UINT32)c;
		if (u <= 0x007F)
			return 1;
		if (u <= 0x07FF)
			return 2;
		if (u <= 0xFFFF)
			return 3;
		if (u <= 0x10FFFF)
			return 4;
		throw Except::Program::DataConsistencyFault(CLOCATION,"Invalid Unicode character "+BinaryToHex(&c,4));
	}

	void	EncodeUnicode(char32_t c, UTF8Char&rs)
	{
		UINT32 u = (UINT32)c;
		if (u <= 0x007F)
		{
			rs.numCharsUsed = 1;
			rs.encoded[0] = (char)(BYTE)u;
			return;
		}

		if (u <= 0x07FF)
		{
			rs.numCharsUsed = 2;
			rs.encoded[1] = 0x80 | (u& 0x3F);	//smallest 6 bits
			rs.encoded[0] = 0xC0 | (u>>6);
			return;
		}

		if (u <= 0xFFFF)
		{
			rs.numCharsUsed = 3;
			rs.encoded[2] = 0x80 | (u& 0x3F);	//smallest 6 bits
			rs.encoded[1] = 0x80 | ((u>>6)& 0x3F);	//next smallest 6 bits
			rs.encoded[0] = 0xE0 | (u>>12);
			return;
		}
		if (u <= 0x10FFFF)
		{
			rs.numCharsUsed = 4;
			rs.encoded[3] = 0x80 | (u& 0x3F);	//smallest 6 bits
			rs.encoded[2] = 0x80 | ((u>>6)& 0x3F);	//next smallest 6 bits
			rs.encoded[1] = 0x80 | ((u>>12)& 0x3F);	//next smallest 6 bits
			rs.encoded[0] = 0xF0 | (u>>18);
			return;
		}

		throw Except::Program::DataConsistencyFault(CLOCATION,"Invalid Unicode character "+BinaryToHex(&c,4));
	}

	void	EncodeUnicode(char32_t c, UTF16Char&rs)
	{
		UINT32 uni = (UINT32)c;

		//technically we're supposed to exclude [U+D800 to U+DFFF] here, but for testing purposes this is counter-productive
		//if (uni <= 0xD7FF || (uni >= 0xE000 && uni <= 0xFFFF))
		if (uni <= 0xFFFF)
		{
			rs.numCharsUsed = 1;
			rs.encoded[0] = (char16_t)uni;

			if ((uni & 0xFC00) == 0xD800)
				throw Except::Program::DataConsistencyFault(CLOCATION, "UTF-16 encoding error. Given Unicode character #"+String(uni)+" is not valid");
		}
		else
		{
			rs.numCharsUsed = 2;
			uni -= 0x10000;
			rs.encoded[0] = (char16_t)((uni >> 10) + 0xD800);
			rs.encoded[1] = (char16_t)((uni & 0x3FF) + 0xDC00);
		}

	}

	



	template <typename T>
	static void	Utf8ToUtf16T(const StringRef&utf8Source, StringType::Template<T>&utf16Dest)
	{
		count_t len = 0;
		const char*utf8 = utf8Source.pointer();
		char const*const end = utf8 + utf8Source.length();

		StringType::Template<char32_t> unicode;
		UTF8ToUnicode(utf8Source,unicode);
		FixedLengthToVariableLength<UnicodeFixedCode, UTF16Code<T> >(unicode.ref(),utf16Dest);
	}



	void	UTF16ToUnicode(const StringType::ReferenceExpression<char16_t>&utf16Source, UnicodeString&unicodeDest)
	{
		VariableLengthToFixedLength<UTF16Code<char16_t>,UnicodeFixedCode>(utf16Source,unicodeDest);
	}

	void	UTF16ToUnicode(const StringType::Template<char16_t>&utf16Source, UnicodeString&unicodeDest)
	{
		UTF16ToUnicode(utf16Source.ref(),unicodeDest);
	}



	#ifdef WIN32
		static_assert(sizeof(wchar_t)==sizeof(char16_t),"Expected wchar_t to be 16 bit on windows");
		void	UnicodeToUTF16(const StringType::ReferenceExpression<char32_t>&unicodeSource, StringType::Template<wchar_t>&utf16Dest)
		{
			FixedLengthToVariableLength<UnicodeFixedCode, UTF16Code<wchar_t> >(unicodeSource,utf16Dest);
		}
		void	UnicodeToUTF16(const UnicodeString&unicodeSource, StringType::Template<wchar_t>&utf16Dest)
		{
			UnicodeToUTF16(unicodeSource.ref(),utf16Dest);
		}

		void	UTF8ToUTF16(const StringRef&utf8Source, StringType::Template<wchar_t>&utf16Dest)
		{
			Utf8ToUtf16T(utf8Source,utf16Dest);
		}

		void	UTF8ToUTF16(const String&utf8Source, StringType::Template<wchar_t>&utf16Dest)
		{
			Utf8ToUtf16T(utf8Source.ref(),utf16Dest);
		}

	#endif

	void	UnicodeToUTF16(const UnicodeString&unicodeSource, StringType::Template<char16_t>&utf16Dest)
	{
		UnicodeToUTF16(unicodeSource.ref(),utf16Dest);
	}

	void	UnicodeToUTF16(const StringType::ReferenceExpression<char32_t>&unicodeSource, StringType::Template<char16_t>&utf16Dest)
	{
		FixedLengthToVariableLength<UnicodeFixedCode, UTF16Code<char16_t> >(unicodeSource,utf16Dest);
	}

	void	UTF8ToUTF16(const StringRef&utf8Source, StringType::Template<char16_t>&utf16Dest)
	{
		Utf8ToUtf16T(utf8Source,utf16Dest);
	}



	void	UTF8ToCP1252(const StringRef&utf8Source, String&cp1252Dest)
	{
		VariableLengthToFixedLength<UTF8Code,CP1252FixedCode>(utf8Source,cp1252Dest);
	}


	void	UTF8ToCP1252(const String&utf8Source, String&cp1252Dest)
	{
		UTF8ToCP1252(utf8Source.ref(),cp1252Dest);
	}

	void	CP1252ToUTF8(const StringRef&cp1252Source, String&utf8Dest)
	{
		FixedLengthToVariableLength<CP1252FixedCode, UTF8Code>(cp1252Source,utf8Dest);
	}

	void	CP1252ToUTF8(const String&cp1252Source, String&utf8Dest)
	{
		CP1252ToUTF8(cp1252Source.ref(),utf8Dest);
	}

	void	CP1252ToUnicode(const StringRef&cp1252Source, UnicodeString&unicodeDest)
	{
		unicodeDest.SetLength(cp1252Source.GetLength());
		auto*out = unicodeDest.mutablePointer();
		const auto*in = cp1252Source.pointer();
		const auto*const end = cp1252Source.end();
		for (; in != end; ++in, ++out)
		{
			*out = ToUnicodeFromCP1252(*in);
		}
		ASSERT_EQUAL__(out,unicodeDest.end());
	}

	void	CP1252ToUnicode(const String&cp1252Source, UnicodeString&unicodeDest)
	{
		CP1252ToUnicode(cp1252Source.ref(),unicodeDest);
	}

	void	UnicodeToCP1252(const StringType::ReferenceExpression<char32_t>&unicodeSource, String&cp1525Dest)
	{
		cp1525Dest.SetLength(unicodeSource.GetLength());
		auto*out = cp1525Dest.mutablePointer();
		const auto*in = unicodeSource.pointer();
		const auto*const end = unicodeSource.end();
		for (; in != end; ++in, ++out)
		{
			*out = EncodeUnicodeToCP1252(*in);
		}
		ASSERT_EQUAL__(out,cp1525Dest.end());
	}

	void	UnicodeToCP1252(const UnicodeString&unicodeSource, String&cp1525Dest)
	{
		UnicodeToCP1252(unicodeSource.ref(),cp1525Dest);
	}


}






