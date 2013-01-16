#ifndef iserializableH
#define iserializableH

#include "read_stream.h"
#include "write_stream.h"



template <typename T>
	struct SerialType
	{
		static const bool IsFixedSize = T::IsFixedSize;
		static const serial_size_t FixedSize = T::FixedSize;
		static inline bool	serializeTo(IWriteStream&stream, const T&instance, bool integrate_size)
		{
			return instance.serialize(stream,integrate_size);
		}
		static inline bool deserialize(IReadStream&stream, T&instance, bool derive_size, serial_size_t remaining_size)
		{
			return instance.deserialize(stream,derive_size, remaining_size);
		}
	};

#undef SERIAL_PRIMITIVE
#define SERIAL_PRIMITIVE(PRIM)\
	template <>\
	struct SerialType<PRIM>\
	{\
		static const bool IsFixedSize = true;\
		static const serial_size_t FixedSize = sizeof(PRIM);\
		static inline bool	serializeTo(IWriteStream&stream, PRIM prim, bool integrate_size)\
		{\
			return stream.writePrimitive(prim);\
		}\
		static inline bool deserialize(IReadStream&stream, PRIM&prim, bool derive_size, serial_size_t remaining_size)\
		{\
			return stream.readPrimitive(prim);\
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
		typedef ::serial_size_t		serial_size_t;	//!< Size type used for serial data type

		static const serial_size_t	EmbeddedSize=(serial_size_t)-1;	//!< Constant that may be passed as the @b fixed_size parameter of the deserialize() operation. It indicates that no fixed size is available and any dynamic size of the local object should be extracted from the stream.


static	serial_size_t				serialSizeOfSize(serial_size_t size)
									{
										if (size <= 0x3F)
											return 1;
										if (size <= 0x3FFF)
											return 2;
										if (size <= 0x3FFFFF)
											return 3;
										return 4;
									}


virtual	serial_size_t				serialSize(bool export_size) const=0;							//!< Queries the serial size of this object @param export_size True if the result should include additional space required for any size variable such as a string or array length @return Serial size (in bytes) of this object
virtual	bool						serialize(IWriteStream&out_stream, bool export_size) const=0;		//!< Writes the serialized content of the local object to the specified stream. The size of the written data must match the result of serialSize() as long as the object remains unchanged. @param out_stream Stream to write to @param export_size True if the object is expected to export any dynamic size such as string or array length @return true if the object could be fully written to the out stream, false otherwise
virtual	bool						deserialize(IReadStream&in_stream, serial_size_t fixed_size)=0;			//!< Reads the content of the local object from the specified stream @param in_stream Stream to read from @param fixed_size ISerializable::EmbeddedSize if any dynamic object size should be read from the stream or the size in bytes that the local object should adopt to otherwise. If this parameter is ISerializable::EmbeddedSize then the object data was serialized with @b export_size set to true @return true if the object data could be fully read from the out stream, false otherwise
virtual	bool						fixedSize() const {return false;}									//!< Queries whether this object is of fixed size (always the same no matter what content)
};

/**
	@brief Object implementing ISerializable including a virtual destructor
*/
class SerializableObject: public ISerializable
{
public:
virtual						~SerializableObject()	{};

};


inline	ISerializable::serial_size_t			serializeToMemory(const ISerializable&serializable, void*target_data, ISerializable::serial_size_t available_space, bool export_size)	//!< Simplified interface to write to a pre-allocated memory section.  @param target_data Pointer to the memory section to write to (may be NULL causing the method to fail) @param available_space Space (in bytes) available in the appointed memory section for serialization @param export_size True if the object is expected to export any dynamic size such as string or array length @return Size (in bytes) written to the specified memory section or 0 if serialization failed
{
	if (!target_data)
		return 0;
	MemWriteStream	writer(target_data,available_space);
	if (!serializable.serialize(writer, export_size))
		return 0;
	return writer.current()-(BYTE*)target_data;
}

inline ISerializable::serial_size_t			deserializeFromMemory(ISerializable&serializable, const void*data, ISerializable::serial_size_t available_data, bool dynamic_size)	//!< Simplified interface to read from a loaded memory section. @param data Pointer to the memory section to read from @param available_data Size (in bytes) of the memory section that should be deserialized @param dynamic_size True if the object was serialized with @b export_size  set true @return Size (in bytes) read from the specified memory section or 0 if deserialization failed
{
	if (!data)
		return 0;
	MemReadStream	reader(data,available_data);
	if (!serializable.deserialize(reader,dynamic_size?ISerializable::EmbeddedSize:available_data))
		return 0;
	return reader.current()-(const BYTE*)data;
}
		

#endif
