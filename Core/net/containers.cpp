#include "containers.h"

namespace DeltaWorks
{



	namespace Package
	{
		template class NetString<256>;

		bool			IsValidChar(char c)
		{
			switch (c)
			{
				case 'ä':
				case 'ö':
				case 'ü':
				case 'Ä':
				case 'Ö':
				case 'Ü':
				case 'ß':

				case ' ':
				case '\t':
				case '\n':
				//case '\r':	<- should not be in a network-string. Standard uses \n as newline
					return true;
			}
			bool result = isgraph((BYTE)c);
			return result;
		}

		bool			IsValid(const StringRef&str, const count_t maxLength)
		{
			if (str.GetLength() > maxLength)
				return false;
			for (index_t i = 0; i < str.length(); i++)
				if (!IsValidChar(str[i]))
				{
					//cout << "invalid char '"<<
					return false;
				}
			return true;
		}

		void			AssertValidity(const StringRef&str, const count_t maxLength)
		{
			try
			{
				CheckValidity(str,maxLength);
			}
			catch (const std::exception&ex)
			{
				Except::TriggerFatal(CLOCATION,ex.what());
			}
			catch (...)
			{
				Except::TriggerFatal(CLOCATION,"Unknown exception");
			}
		}

		void			CheckValidity(const StringRef&str, const count_t maxLength)
		{
			if (str.GetLength() > maxLength)
				throw Except::Program::ParameterFault(CLOCATION,String("NetString: '"+str+"' (length "+String(str.GetLength())+") exceeds maximum allowed length "+String(maxLength)));

			for (index_t i = 0; i < str.length(); i++)
			{
				const char c = str[i];
				if (!IsValidChar(c))
					throw Except::Program::ParameterFault(CLOCATION,"NetString: Character '"+String(c)+"' at position "+String(i)+" of '"+str+"' is not allowed");
			}
		}
	}
}
