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
	BYTE	EncodedUTF8Length(char32_t utf32Char);




	namespace FromUTF8
	{
		char32_t	ToUTF32(const UTF8Char&c);
		void		ToUTF32(const StringRef&utf8Source, UTF32String&utf32Dest);
		void		ToUTF32(const String&utf8Source, UTF32String&utf32Dest);
		char		ToCP1252(const UTF8Char&ch);
		void		ToCP1252(const StringRef&utf8Source, String&cp1252Dest);
		void		ToCP1252(const String&utf8Source, String&cp1252Dest);
		void		ToUTF16(const StringRef&utf8Source, StringType::Template<char16_t>&utf16Dest);
		#ifdef WIN32
			void	ToUTF16(const StringRef&utf8Source, StringType::Template<wchar_t>&utf16Dest);
			void	ToUTF16(const String&utf8Source, StringType::Template<wchar_t>&utf16Dest);
		#endif
	};

	namespace FromUTF32
	{
		void		ToUTF8(char32_t c, UTF8Char&rs);
		void		ToUTF8(const StringType::ReferenceExpression<char32_t>&utf32Source, String&utf8Dest);
		void		ToUTF8(const UTF32String&utf32Source, String&utf8Dest);
		char		ToCP1252(char32_t c);
		void		ToCP1252(const StringType::ReferenceExpression<char32_t>&utf32Source, String&cp1525Dest);
		void		ToCP1252(const UTF32String&utf32Source, String&cp1525Dest);
		void		ToUTF16(char32_t c, UTF16Char&rs);
		void		ToUTF16(const StringType::ReferenceExpression<char32_t>&utf32Source, StringType::Template<char16_t>&utf16Dest);
		void		ToUTF16(const UTF32String&utf32Source, StringType::Template<char16_t>&utf16Dest);
		#ifdef WIN32
			void	ToUTF16(const UTF32String&utf32Source, StringType::Template<wchar_t>&utf16Dest);
			void	ToUTF16(const StringType::ReferenceExpression<char32_t>&utf32Source, StringType::Template<wchar_t>&utf16Dest);
		#endif
	}

	namespace FromUTF16
	{
		char32_t	ToUTF32(const UTF16Char&c);
		void		ToUTF32(const StringType::ReferenceExpression<char16_t>&utf16Source, UTF32String&utf32Dest);
		void		ToUTF32(const StringType::Template<char16_t>&utf16Source, UTF32String&utf32Dest);
		#ifdef WIN32
			static_assert(sizeof(wchar_t)==sizeof(char16_t),"Expected wchar_t to be 16 bit on windows");
			void	ToUTF32(const StringType::ReferenceExpression<wchar_t>&utf16Source, UTF32String&utf32Dest);
			void	ToUTF32(const StringType::Template<wchar_t>&utf16Source, UTF32String&utf32Dest);
		#endif
	}

	namespace FromCP1252
	{
		void		ToUTF8(char, UTF8Char&ch);
		void		ToUTF8(const StringRef&cp1252Source, String&utf8Dest);
		void		ToUTF8(const String&cp1252Source, String&utf8Dest);
		char32_t	ToUTF32(char);
		void		ToUTF32(const StringRef&cp1252Source, UTF32String&utf32Dest);
		void		ToUTF32(const String&cp1252Source, UTF32String&utf32Dest);
	}

	void	Deserialize(IReadStream&source, UTF8Char&utf8Dest);

	
	bool	IsValid(const UTF16Char&);
	bool	IsValid(const UTF8Char&);
	bool	IsValidUnicode(char32_t c);
	void	AssertValidity(const UTF16Char&);
	void	AssertValidity(const UTF8Char&);

	
}
