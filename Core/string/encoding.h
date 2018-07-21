#pragma once

#include "../global_string.h"

namespace DeltaWorks
{
	/**
	Support for various string encodings and the conversion between them.
	*/
	namespace StringEncoding
	{


		/**
		Attributes of and conversion from UTF-8
		*/
		namespace UTF8
		{

			/**
			Character container that can hold all valid UTF-8 characters
			*/
			struct TChar
			{
				char		encoded[4];
				BYTE		numCharsUsed=0;	//0-4

				StringRef	ToRef() const {return StringRef(encoded,numCharsUsed);};
			};

			/**
			Determines the total length (in 8bit characters) of a variable-length UTF-8 encoded character, by looking at the first character.
			Throws an exception if the detected length exceeds 4.
			@return Character length in [1,4], including this first character
			*/
			BYTE			GetLength(char firstChar);

			char32_t		ToUTF32(const TChar&c);
			void			ToUTF32(const StringRef&utf8Source, UTF32String&utf32Dest);
			void			ToUTF32(const String&utf8Source, UTF32String&utf32Dest);
			char			ToCP1252(const TChar&ch);
			void			ToCP1252(const StringRef&utf8Source, String&cp1252Dest);
			void			ToCP1252(const String&utf8Source, String&cp1252Dest);
			void			ToUTF16(const String&utf8Source, StringType::Template<char16_t>&utf16Dest);
			void			ToUTF16(const StringRef&utf8Source, StringType::Template<char16_t>&utf16Dest);
			#ifdef WIN32
				void		ToUTF16(const StringRef&utf8Source, StringType::Template<wchar_t>&utf16Dest);
				void		ToUTF16(const String&utf8Source, StringType::Template<wchar_t>&utf16Dest);
			#endif
		};

		/**
		Attributes of and conversion from UTF-16
		*/
		namespace UTF16
		{
			/**
			Character container that can hold all valid UTF-16 characters
			*/
			struct TChar
			{
				char16_t	encoded[2];
				BYTE		numCharsUsed=0;	//0, 1 or 2
			};


			char32_t		ToUTF32(const TChar&c);
			void			ToUTF32(const StringType::ReferenceExpression<char16_t>&utf16Source, UTF32String&utf32Dest);
			void			ToUTF32(const UTF16String&utf16Source, UTF32String&utf32Dest);
			void			ToUTF8(const StringType::ReferenceExpression<char16_t>&utf16Source, String&utf8Dest);
			void			ToUTF8(const UTF16String&utf16Source, String&utf8Dest);
			UTF32String		ToUTF32(const StringType::ReferenceExpression<char16_t>&utf16Source);
			UTF32String		ToUTF32(const UTF16String&utf16Source);
			String			ToUTF8(const StringType::ReferenceExpression<char16_t>&utf16Source);
			String			ToUTF8(const UTF16String&utf16Source);
			#ifdef WIN32
				static_assert(sizeof(wchar_t)==sizeof(char16_t),"Expected wchar_t to be 16 bit on windows");
				void		ToUTF32(const StringType::ReferenceExpression<wchar_t>&utf16Source, UTF32String&utf32Dest);
				void		ToUTF32(const StringW&utf16Source, UTF32String&utf32Dest);
				void		ToUTF8(const StringType::ReferenceExpression<wchar_t>&utf16Source, String&utf8Dest);
				void		ToUTF8(const StringW&utf16Source, String&utf8Dest);
				UTF32String	ToUTF32(const StringType::ReferenceExpression<wchar_t>&utf16Source);
				UTF32String	ToUTF32(const StringW&utf16Source);
				String		ToUTF8(const StringType::ReferenceExpression<wchar_t>&utf16Source);
				String		ToUTF8(const StringW&utf16Source);
			#endif
		}

		/**
		Conversion functions from UTF-32
		*/
		namespace UTF32
		{
			BYTE			ToUTF8Length(char32_t utf32Char);
			void			ToUTF8(char32_t c, UTF8::TChar&rs);
			void			ToUTF8(const StringType::ReferenceExpression<char32_t>&utf32Source, String&utf8Dest);
			void			ToUTF8(const UTF32String&utf32Source, String&utf8Dest);
			char			ToCP1252(char32_t c);
			void			ToCP1252(const StringType::ReferenceExpression<char32_t>&utf32Source, String&cp1525Dest);
			void			ToCP1252(const UTF32String&utf32Source, String&cp1525Dest);
			void			ToUTF16(char32_t c, UTF16::TChar&rs);
			void			ToUTF16(const StringType::ReferenceExpression<char32_t>&utf32Source, UTF16String&utf16Dest);
			void			ToUTF16(const UTF32String&utf32Source, UTF16String&utf16Dest);
			#ifdef WIN32
				void		ToUTF16(const UTF32String&utf32Source, StringType::Template<wchar_t>&utf16Dest);
				void		ToUTF16(const StringType::ReferenceExpression<char32_t>&utf32Source, StringType::Template<wchar_t>&utf16Dest);
			#endif
		}

		/**
		Conversion functions from Windows Code Page 1252 (sometimes mislabeled ANSI, or ISO-8859-1)
		*/
		namespace CP1252
		{
			void			ToUTF8(char, UTF8::TChar&ch);
			void			ToUTF8(const StringRef&cp1252Source, String&utf8Dest);
			void			ToUTF8(const String&cp1252Source, String&utf8Dest);
			char32_t		ToUTF32(char);
			void			ToUTF32(const StringRef&cp1252Source, UTF32String&utf32Dest);
			void			ToUTF32(const String&cp1252Source, UTF32String&utf32Dest);
		}

		/**
		Deserializes a UTF-8 character from an arbitrary binary stream.
		The stream is advanced by the exact length of the extracted character
		*/
		void	Deserialize(IReadStream&source, UTF8::TChar&utf8Dest);

	
		bool	IsValid(const UTF16::TChar&);
		bool	IsValid(const UTF8::TChar&);
		bool	IsValidUnicode(char32_t c);
		void	AssertValidity(const UTF16::TChar&);
		void	AssertValidity(const UTF8::TChar&);

	
	}
}
