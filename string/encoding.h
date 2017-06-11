#pragma once

#include "../global_string.h"

namespace StringEncoding
{
	struct UTF8Char
	{
		char		encoded[4];
		BYTE		numCharsUsed=0;	//0-4

		StringRef	ToRef() const {return StringRef(encoded,numCharsUsed);};
	};
	struct UTF16Char
	{
		char16_t	encoded[2];
		BYTE		numCharsUsed=0;	//0, 1 or 2
	};


	/**
	Determines the total length (in 8bit characters) of a variable-length UTF-8 encoded character, by looking at the first character.
	Throws an exception if the detected length exceeds 4.
	@return Character length in [1,4], including this first character
	*/
	BYTE	UTF8Length(char firstChar);
	BYTE	EncodedUTF8Length(char32_t unicodeChar);


	void	Deserialize(IReadStream&source, UTF8Char&utf8Dest);

	void	EncodeUnicode(char32_t c, UTF16Char&rs);
	void	EncodeUnicode(char32_t c, UTF8Char&rs);
	char	EncodeUnicodeToCP1252(char32_t c);
	
	char32_t	ToUnicodeFromCP1252(char);
	char32_t	ToUnicode(const UTF16Char&c);
	char32_t	ToUnicode(const UTF8Char&c);
	bool	IsValid(const UTF16Char&);
	bool	IsValid(const UTF8Char&);
	bool	IsValidUnicode(char32_t c);
	void	AssertValidity(const UTF16Char&);
	void	AssertValidity(const UTF8Char&);

	char	ToCP1252(const UTF8Char&ch);
	void	EncodeCP1252(char, UTF8Char&ch);

	void	UTF8ToCP1252(const StringRef&utf8Source, String&cp1252Dest);
	void	UTF8ToCP1252(const String&utf8Source, String&cp1252Dest);
	void	CP1252ToUTF8(const StringRef&cp1252Source, String&utf8Dest);
	void	CP1252ToUTF8(const String&cp1252Source, String&utf8Dest);
	void	CP1252ToUnicode(const StringRef&cp1252Source, UnicodeString&unicodeDest);
	void	CP1252ToUnicode(const String&cp1252Source, UnicodeString&unicodeDest);
	void	UTF8ToUnicode(const StringRef&utf8Source, UnicodeString&unicodeDest);
	void	UTF8ToUnicode(const String&utf8Source, UnicodeString&unicodeDest);
	void	UnicodeToUTF8(const StringType::ReferenceExpression<char32_t>&unicodeSource, String&utf8Dest);
	void	UnicodeToUTF8(const UnicodeString&unicodeSource, String&utf8Dest);
	void	UnicodeToCP1252(const StringType::ReferenceExpression<char32_t>&unicodeSource, String&cp1525Dest);
	void	UnicodeToCP1252(const UnicodeString&unicodeSource, String&cp1525Dest);
	
	void	UTF8ToUTF16(const StringRef&utf8Source, StringType::Template<char16_t>&utf16Dest);
	void	UnicodeToUTF16(const StringType::ReferenceExpression<char32_t>&unicodeSource, StringType::Template<char16_t>&utf16Dest);
	void	UnicodeToUTF16(const UnicodeString&unicodeSource, StringType::Template<char16_t>&utf16Dest);

	void	UTF16ToUnicode(const StringType::ReferenceExpression<char16_t>&utf16Source, UnicodeString&unicodeDest);
	void	UTF16ToUnicode(const StringType::Template<char16_t>&utf16Source, UnicodeString&unicodeDest);

	#ifdef WIN32
		static_assert(sizeof(wchar_t)==sizeof(char16_t),"Expected wchar_t to be 16 bit on windows");
		void	UnicodeToUTF16(const UnicodeString&unicodeSource, StringType::Template<wchar_t>&utf16Dest);
		void	UnicodeToUTF16(const StringType::ReferenceExpression<char32_t>&unicodeSource, StringType::Template<wchar_t>&utf16Dest);
		void	UTF8ToUTF16(const StringRef&utf8Source, StringType::Template<wchar_t>&utf16Dest);
		void	UTF8ToUTF16(const String&utf8Source, StringType::Template<wchar_t>&utf16Dest);

		void	UTF16ToUnicode(const StringType::ReferenceExpression<wchar_t>&utf16Source, UnicodeString&unicodeDest);
		void	UTF16ToUnicode(const StringType::Template<wchar_t>&utf16Source, UnicodeString&unicodeDest);

	#endif
}
