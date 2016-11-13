#ifndef string_converterTplH
#define string_converterTplH



/******************************************************************

E:\include\string\string_converter.tpl.h

******************************************************************/

template <>
	inline bool	convert<BYTE>(const char*string, BYTE&out)
	{
		return convertToByte(string,out);
	}

template <>
	inline bool	convert<char>(const char*string, char&out)
	{
		return convertToChar(string,out);
	}

template <>
	inline bool	convert<short>(const char*string, short&out)
	{
		return convertToShort(string,out);
	}

template <>
	inline bool	convert<long>(const char*string, long&out)
	{
		return convertToLong(string,out);
	}

template <>
	inline bool	convert<int>(const char*string, int&out)
	{
		return convertToInt(string,out);
	}

template <>
	inline bool	convert<long long>(const char*string, long long&out)
	{
		return convertToInt64(string,out);
	}

template <>
	inline bool	convert<unsigned long long>(const char*string, unsigned long long&out)
	{
		return convertToUnsignedInt64(string,out);
	}

template <>
	inline bool	convert<long double>(const char*string, long double&out)
	{
		return convertToLongDouble(string,out);
	}

template <>
	inline bool	convert<double>(const char*string, double&out)
	{
		return convertToDouble(string,out);
	}

template <>
	inline bool	convert<float>(const char*string, float&out)
	{
		return convertToFloat(string,out);
	}

template <>
	inline bool	convert<bool>(const char*string, bool&out)
	{
		return convertToBool(string,out);
	}

template <>
	inline bool	convert<unsigned>(const char*string, unsigned&out)
	{
		return convertToUnsigned(string,out);
	}

template <>
	inline bool	convert<unsigned short>(const char*string, unsigned short&out)
	{
		return convertToUnsignedShort(string,out);
	}

template <>
	inline bool	convert<Key::Name>(const char*string, Key::Name&out)
	{
		return convertToKey(string,out);
	}





template <>
	inline bool	convert<BYTE>(const char*string, size_t length, BYTE&out)
	{
		return convertToByte(string,length,out);
	}

template <>
	inline bool	convert<char>(const char*string, size_t length, char&out)
	{
		return convertToChar(string,length,out);
	}

template <>
	inline bool	convert<short>(const char*string, size_t length, short&out)
	{
		return convertToShort(string,length,out);
	}

template <>
	inline bool	convert<long>(const char*string, size_t length, long&out)
	{
		return convertToLong(string,length,out);
	}

template <>
	inline bool	convert<int>(const char*string, size_t length, int&out)
	{
		return convertToInt(string,length,out);
	}

template <>
	inline bool	convert<long long>(const char*string, size_t length, long long&out)
	{
		return convertToInt64(string,length,out);
	}


template <>
	inline bool	convert<unsigned long long>(const char*string, size_t length, unsigned long long&out)
	{
		return convertToUnsignedInt64(string,length,out);
	}

template <>
	inline bool	convert<long double>(const char*string, size_t length, long double&out)
	{
		return convertToLongDouble(string,length,out);
	}

template <>
	inline bool	convert<double>(const char*string, size_t length, double&out)
	{
		return convertToDouble(string,length,out);
	}

template <>
	inline bool	convert<float>(const char*string, size_t length, float&out)
	{
		return convertToFloat(string,length,out);
	}

template <>
	inline bool	convert<bool>(const char*string, size_t length, bool&out)
	{
		return convertToBool(string,length,out);
	}

template <>
	inline bool	convert<unsigned>(const char*string, size_t length, unsigned&out)
	{
		return convertToUnsigned(string,length,out);
	}

template <>
	inline bool	convert<unsigned short>(const char*string, size_t length, unsigned short&out)
	{
		return convertToUnsignedShort(string,length,out);
	}

template <>
	inline bool	convert<Key::Name>(const char*string, size_t length, Key::Name&out)
	{
		return convertToKey(string,length,out);
	}



#endif
