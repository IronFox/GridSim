#ifndef iserializableH
#define iserializableH

#include "read_stream.h"
#include "write_stream.h"



template <typename T>
	struct SerialType
	{
		static const bool HasFixedSize = T::HasFixedSize;
		static const serial_size_t FixedSize = T::FixedSize;
		static inline bool	SerializeTo(IWriteStream&stream, const T&instance, bool integrate_size)
		{
			return instance.Serialize(stream,integrate_size);
		}
		static inline bool Deserialize(IReadStream&stream, T&instance, bool derive_size, serial_size_t remaining_size)
		{
			return instance.Deserialize(stream,derive_size, remaining_size);
		}
	};

#undef SERIAL_PRIMITIVE
#define SERIAL_PRIMITIVE(PRIM)\
	template <>\
	struct SerialType<PRIM>\
	{\
		static const bool HasFixedSize = true;\
		static const serial_size_t FixedSize = sizeof(PRIM);\
		static inline bool	SerializeTo(IWriteStream&stream, PRIM prim, bool integrate_size)\
		{\
			return stream.WritePrimitive(prim);\
		}\
		static inline bool Deserialize(IReadStream&stream, PRIM&prim, bool derive_size, serial_size_t remaining_size)\
		{\
			return stream.ReadPrimitive(prim);\
		}\
	};

SERIAL_PRIMITIVE(char);
SERIAL_PRIMITIVE(unsigned char);
SERIAL_PRIMITIVE(short);
SERIAL_PRIMITIVE(unsigned short);
SERIAL_PRIMITIVE(int);
SERIAL_PRIMITIVE(unsigned int);
SERIAL_PRIMITIVE(long);
SERIAL_PRIMITIVE(unsigned long);
SERIAL_PRIMITIVE(long long);
SERIAL_PRIMITIVE(unsigned long long);
SERIAL_PRIMITIVE(float);
SERIAL_PRIMITIVE(double);






/*!
	@brief	Serializable interface
	
	Derivatives of ISerializable provide an interface to read from and write to generic data streams.
	Serializable classes must define both read and write methods.<br>
	Sizes are 64bit little endian unsigned integers.

*/
interface ISerializable
{
public:
	typedef ::serial_size_t			serial_size_t;	//!< Size type used for serial data type

	static const serial_size_t		EmbeddedSize=(serial_size_t)-1;	//!< Constant that may be passed as the @b fixed_size parameter of the Deserialize() operation. It indicates that no fixed size is available and any dynamic size of the local object should be extracted from the stream.


	static	serial_size_t			GetSerialSizeOfSize(serial_size_t size)
									{
										if (size <= 0x3F)
											return 1;
										if (size <= 0x3FFF)
											return 2;
										if (size <= 0x3FFFFF)
											return 3;
										return 4;
									}


	virtual	serial_size_t			GetSerialSize(bool export_size) const=0;							//!< Queries the serial size of this object @param export_size True if the result should include additional space required for any size variable such as a string or array length @return Serial size (in bytes) of this object
	virtual	bool					Serialize(IWriteStream&out_stream, bool export_size) const=0;		//!< Writes the serialized content of the local object to the specified stream. The size of the written data must match the result of GetSerialSize() as long as the object remains unchanged. @param out_stream Stream to write to @param export_size True if the object is expected to export any dynamic size such as string or array length @return true if the object could be fully written to the out stream, false otherwise
	virtual	bool					Deserialize(IReadStream&in_stream, serial_size_t fixed_size)=0;			//!< Reads the content of the local object from the specified stream @param in_stream Stream to read from @param fixed_size ISerializable::EmbeddedSize if any dynamic object size should be read from the stream or the size in bytes that the local object should adopt to otherwise. If this parameter is ISerializable::EmbeddedSize then the object data was serialized with @b export_size set to true @return true if the object data could be fully read from the out stream, false otherwise
	virtual	bool					HasFixedSize() const {return false;}									//!< Queries whether this object is of fixed size (always the same no matter what content)
};

/**
	@brief Object implementing ISerializable including a virtual destructor
*/
class SerializableObject: public ISerializable
{
public:
	virtual						~SerializableObject()	{};
};


	//!< Simplified interface to write to a pre-allocated memory section.  @param target_data Pointer to the memory section to write to (may be NULL causing the method to fail) @param available_space Space (in bytes) available in the appointed memory section for serialization @param export_size True if the object is expected to export any dynamic size such as string or array length @return Size (in bytes) written to the specified memory section or 0 if serialization failed
inline	bool			SerializeToMemory(const ISerializable&serializable, void*targetData, ISerializable::serial_size_t availableSpace, bool exportSize, serial_size_t*outUsedSize=nullptr)
{
	if (!availableSpace)
	{
		if (exportSize)
			return false;
		MemWriteStream	writer;
		if (outUsedSize)
			(*outUsedSize) = 0;
		return serializable.Serialize(writer, exportSize);
	}

	if (!targetData)
		return false;
	MemWriteStream	writer(targetData,availableSpace);
	if (!serializable.Serialize(writer, exportSize))
		return false;
	if (outUsedSize)
		(*outUsedSize) = writer.GetCurrent()-(BYTE*)targetData;
	return true;
}

/**
	Simplified interface to read from a loaded memory section.
	@param data Pointer to the memory section to read from
	@param dataSize Size (in bytes) of the memory section that should be deserialized
	@param embeddedSize True if the object was serialized with @b export_size set true
	@return true if Deserialization succeeded, false otherwise.
*/
inline bool			DeserializeFromMemory(ISerializable&serializable, const void*data, ISerializable::serial_size_t dataSize, bool embeddedSize)
{
	if (!dataSize)
	{
		if (embeddedSize)
		{
			//impossible
			return false;
		}
		MemReadStream	reader;
		return serializable.Deserialize(reader,0);
	}
	ASSERT_NOT_NULL__(data);
	MemReadStream	reader(data,dataSize);
	return serializable.Deserialize(reader,embeddedSize?ISerializable::EmbeddedSize:dataSize);
}



#endif
