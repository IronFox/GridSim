#ifndef iserializableH
#define iserializableH

#include "read_stream.h"
#include "write_stream.h"

namespace DeltaWorks
{

	template <typename T>
		struct SerialType
		{
			static const bool HasFixedSize = T::HasFixedSize;
			static const serial_size_t FixedSize = T::FixedSize;
			static inline void	SerializeTo(IWriteStream&stream, const T&instance, bool integrate_size)
			{
				instance.Serialize(stream,integrate_size);
			}
			static inline void Deserialize(IReadStream&stream, T&instance, bool derive_size, serial_size_t remaining_size)
			{
				instance.Deserialize(stream,derive_size, remaining_size);
			}
		};

	#undef SERIAL_PRIMITIVE
	#define SERIAL_PRIMITIVE(PRIM)\
		template <>\
		struct SerialType<PRIM>\
		{\
			static const bool HasFixedSize = true;\
			static const serial_size_t FixedSize = sizeof(PRIM);\
			static inline void	SerializeTo(IWriteStream&stream, PRIM prim, bool integrate_size)\
			{\
				stream.WritePrimitive(prim);\
			}\
			static inline void Deserialize(IReadStream&stream, PRIM&prim, bool derive_size, serial_size_t remaining_size)\
			{\
				stream.ReadPrimitive(prim);\
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


		/**
		Queries the byte size of the serialized data of this object
		@param exportSize True if the result should include additional space required for any size variable such as a string or array length.
			If false, then an external variable will hold the serialized data size, which will be passed into Deserialize() as fixedSize.
			If true, then no size data will be stored externally, and the local structure should serialize any variable size data manually. Deserialize() will receive fixedSize=EmbeddedSize in this case.
		@return Serial size (in bytes) of this object
		*/
		virtual	serial_size_t			GetSerialSize(bool exportSize) const=0;
		/**
		Writes the serialized content of the local object to the specified stream.
		The size of the written data must match the current result of GetSerialSize() as long as the object remains unchanged.
		@param outStream Stream to write to @param exportSize True if the object is expected to export any dynamic size such as string or array length @return true if the object could be fully written to the out stream, false otherwise
		*/
		virtual	void					Serialize(IWriteStream&outStream, bool exportSize) const=0;
		virtual	void					Deserialize(IReadStream&inStream, serial_size_t fixedSize)=0;			//!< Reads the content of the local object from the specified stream @param inStream Stream to read from @param fixedSize ISerializable::EmbeddedSize if any dynamic object size should be read from the stream or the size in bytes that the local object should adopt to otherwise. If this parameter is ISerializable::EmbeddedSize then the object data was serialized with @b export_size set to true @return true if the object data could be fully read from the out stream, false otherwise
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


	/**
	Simplified interface to serialized to a pre-allocated memory section. 
	@param targetData Pointer to the memory section to write to (may be NULL causing the method to fail)
	@param availableSpace Space (in bytes) available in the appointed memory section for serialization
	@param exportSize True if the object is expected to export any dynamic size such as string or array length
	@param[out] outUsedSize Optional resulting serialized size in memory
	*/
	void			SerializeToMemory(const ISerializable&serializable, void*targetData, serial_size_t availableSpace, bool exportSize, serial_size_t*outUsedSize=nullptr);

	/**
	Simplified interface to serialized to a pre-allocated memory section. 
	If the memory consumed during serialization does not exactly match the amount available, an exception is thrown
	@param targetData Pointer to the memory section to write to (may be NULL causing the method to fail)
	@param availableSpace Space (in bytes) available in the appointed memory section for serialization
	@param exportSize True if the object is expected to export any dynamic size such as string or array length
	*/
	void			SerializeToCompactMemory(const ISerializable&serializable, void*targetData, serial_size_t availableSpace, bool exportSize);

	/**
	Simplified interface to read from a loaded memory section.
	@param data Pointer to the memory section to read from
	@param dataSize Size (in bytes) of the memory section that should be deserialized
	@param embeddedSize True if the object was serialized with @b export_size set true
	@return true if Deserialization succeeded, false otherwise.
	*/
	void			DeserializeFromMemory(ISerializable&serializable, const void*data, serial_size_t dataSize, bool embeddedSize);


	/**
	Simplified interface to read from a loaded memory section.
	If the data consumed during deserialization does not exactly match the amount available, an exception is thrown
	@param data Pointer to the memory section to read from
	@param dataSize Size (in bytes) of the memory section that should be deserialized
	@param embeddedSize True if the object was serialized with @b export_size set true
	@return true if Deserialization succeeded, false otherwise.
	*/
	void			DeserializeFromCompactMemory(ISerializable&serializable, const void*data, serial_size_t dataSize, bool embeddedSize);
}

#endif
