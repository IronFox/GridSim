#include "serializable.h"

namespace DeltaWorks
{
	void			SerializeToMemory(const ISerializable&serializable, void*targetData, serial_size_t availableSpace, bool exportSize, serial_size_t*outUsedSize/*=nullptr*/)
	{
		if (!availableSpace)
		{
			if (exportSize)
				throw Except::Memory::SerializationFault(CLOCATION, "Insufficient space to write dynamic size");
			MemWriteStream	writer;
			if (outUsedSize)
				(*outUsedSize) = 0;
			serializable.Serialize(writer, exportSize);
			return;
		}

		if (!targetData)
			throw Except::Memory::SerializationFault(CLOCATION, "Target data is NULL");
		MemWriteStream	writer(targetData,availableSpace);
		serializable.Serialize(writer, exportSize);
		if (outUsedSize)
			(*outUsedSize) = writer.GetCurrent()-(BYTE*)targetData;
	}


	void			SerializeToCompactMemory(const ISerializable&serializable, void*targetData, serial_size_t availableSpace, bool exportSize)
	{
		if (!availableSpace)
		{
			if (exportSize)
				throw Except::Memory::SerializationFault(CLOCATION, "Insufficient space to write dynamic size");
			MemWriteStream	writer;
			serializable.Serialize(writer, exportSize);
			return;
		}

		if (!targetData)
			throw Except::Memory::SerializationFault(CLOCATION, "Target data is NULL");
		MemWriteStream	writer(targetData,availableSpace);
		serializable.Serialize(writer, exportSize);
		if (writer.GetRemainingSpace() != 0)
			throw Except::Memory::SerializationFault(CLOCATION, "Memory consumed during serialization does not match available memory");
	}

	void			DeserializeFromCompactMemory(ISerializable&serializable, const void*data, serial_size_t dataSize, bool embeddedSize)
	{
		if (!dataSize)
		{
			if (embeddedSize)
			{
				//impossible
				throw Except::Memory::SerializationFault(CLOCATION, "Insufficient data to read dynamic size");
			}
			MemReadStream	reader;
			serializable.Deserialize(reader,0);
			return;
		}
		ASSERT_NOT_NULL__(data);
		MemReadStream	reader(data,dataSize);
		serializable.Deserialize(reader,embeddedSize?ISerializable::EmbeddedSize:dataSize);
		if (reader.GetRemainingBytes() != 0)
			throw Except::Memory::SerializationFault(CLOCATION, "Data consumed during deserialization does not match available data");

	}

	void			DeserializeFromMemory(ISerializable&serializable, const void*data, serial_size_t dataSize, bool embeddedSize)
	{
		if (!dataSize)
		{
			if (embeddedSize)
			{
				//impossible
				throw Except::Memory::SerializationFault(CLOCATION, "Insufficient data to read dynamic size");
			}
			MemReadStream	reader;
			serializable.Deserialize(reader,0);
			return;
		}
		ASSERT_NOT_NULL__(data);
		MemReadStream	reader(data,dataSize);
		serializable.Deserialize(reader,embeddedSize?ISerializable::EmbeddedSize:dataSize);
	}

}

