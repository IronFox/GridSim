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
		

		/*
		Size serialization convention:
		big ending (large -> small)
		bytes used as needed.
		The first 2-3 bits indicate number of bytes used:

		00: 6 bit value, 1 byte
		01: 14 bit value, 2 bytes
		10: 22 bit value, 3 bytes
		110: 29 bit value, 4 bytes
		111: 61 bit value, 8 bytes
		*/


		/**
		Writes the serialized content of the local object to the specified stream.
		The size of the written data must match the current result of GetSerialSize() as long as the object remains unchanged.
		@param outStream Stream to write to
		*/
		virtual	void					Serialize(IWriteStream&outStream) const=0;
		/**
		Reads the content of the local object from the specified stream
		@param inStream Stream to read from
		*/
		virtual	void					Deserialize(IReadStream&inStream)=0;
	};

	/**
		@brief Object implementing ISerializable including a virtual destructor
	*/
	class SerializableObject: public ISerializable
	{
	public:
		virtual						~SerializableObject()	{};
	};


	namespace Serialization
	{

		template <typename T>
			inline void	SerialSyncPOD(IReadStream&stream, T&val)
			{
				stream.ReadPrimitive(val);
			}
		template <typename T>
			inline void	SerialSyncPOD(IWriteStream&stream, const T&val)
			{
				stream.WritePrimitive(val);
			}

		#undef SERIAL_PRIM
		#define SERIAL_PRIM(TYPE)	inline void SerialSync(IReadStream&stream, TYPE&val){SerialSyncPOD(stream, val);} inline void SerialSync(IWriteStream&stream, TYPE val){SerialSyncPOD(stream, val);}

		SERIAL_PRIM(signed char);
		SERIAL_PRIM(unsigned char);
		SERIAL_PRIM(signed short);
		SERIAL_PRIM(unsigned short);
		SERIAL_PRIM(signed long);
		SERIAL_PRIM(unsigned long);
		SERIAL_PRIM(signed long long);
		SERIAL_PRIM(unsigned long long);
		SERIAL_PRIM(signed int);
		SERIAL_PRIM(unsigned int);
		SERIAL_PRIM(bool);
		SERIAL_PRIM(float);
		SERIAL_PRIM(double);
		SERIAL_PRIM(long double);

		inline void SerialSync(IReadStream&stream, ISerializable&serializable)
		{
			serializable.Deserialize(stream);
		}
		inline void SerialSync(IWriteStream&stream, const ISerializable&serializable)
		{
			serializable.Serialize(stream);
		}

		template <typename A, typename B>
			inline void SerialSync(IReadStream&stream, std::pair<A,B>&p)
			{
				SerialSync(stream,p.first);
				SerialSync(stream,p.second);
			}
		template <typename A, typename B>
			inline void SerialSync(IWriteStream&stream, const std::pair<A,B>&p)
			{
				SerialSync(stream,p.first);
				SerialSync(stream,p.second);
			}

	};


	/**
	Simplified interface to serialized to a pre-allocated memory section. 
	@param targetData Pointer to the memory section to write to (may be NULL causing the method to fail)
	@param availableSpace Space (in bytes) available in the appointed memory section for serialization
	@param[out] outUsedSize Optional resulting serialized size in memory
	*/
	template <typename Serializable>
	inline void			SerializeToMemory(const Serializable&serializable, void*targetData, serial_size_t availableSpace, serial_size_t*outUsedSize=nullptr)
	{
		using Serialization::SerialSync;
		MemWriteStream	writer(targetData,availableSpace);
		SerialSync(writer,serializable);
		if (outUsedSize)
			(*outUsedSize) = writer.GetCurrent()-(BYTE*)targetData;
	}

	
	/**
	Simplified interface to serialized to a pre-allocated memory section. 
	If the memory consumed during serialization does not exactly match the amount available, an exception is thrown
	@param targetData Pointer to the memory section to write to (may be NULL causing the method to fail)
	@param availableSpace Space (in bytes) available in the appointed memory section for serialization
	*/
	template <typename Serializable>
	inline void			SerializeToCompactMemory(const Serializable&serializable, void*targetData, serial_size_t availableSpace)
	{
		using Serialization::SerialSync;
		MemWriteStream	writer(targetData,availableSpace);
		SerialSync(writer,serializable);
		if (writer.GetRemainingSpace() != 0)
			throw Except::Memory::SerializationFault(CLOCATION, "Memory consumed during serialization does not match available memory");
	}


	/**
	Simplified interface to read from a loaded memory section.
	@param data Pointer to the memory section to read from
	@param dataSize Size (in bytes) of the memory section that should be deserialized
	*/
	template <typename Serializable>
	inline void			DeserializeFromMemory(Serializable&serializable, const void*data, serial_size_t dataSize)
	{
		using Serialization::SerialSync;
		MemReadStream	reader(data,dataSize);
		SerialSync(reader,serializable);

	}


	/**
	Simplified interface to read from a loaded memory section.
	If the data consumed during deserialization does not exactly match the amount available, an exception is thrown
	@param data Pointer to the memory section to read from
	@param dataSize Size (in bytes) of the memory section that should be deserialized
	*/
	template <typename Serializable>
	inline void			DeserializeFromCompactMemory(Serializable&serializable, const void*data, serial_size_t dataSize)
	{
		using Serialization::SerialSync;
		MemReadStream	reader(data,dataSize);
		SerialSync(reader,serializable);
		if (reader.GetRemainingBytes() != 0)
			throw Except::Memory::SerializationFault(CLOCATION, "Data consumed during deserialization does not match available data");
	}

}

#endif
