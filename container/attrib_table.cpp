#include "../global_root.h"
#include "attrib_table.h"

namespace Attrib
{
	bool		Table::finalized = false;


	void		Table::clear()
	{
		StaticTable<bool>::clear();
		StaticTable<int>::clear();
		StaticTable<float>::clear();
		StaticTable<double>::clear();
		StaticTable<String>::clear();
	}
	
	void							Table::buildInstance(Instance&target)
	{
	
		static const unsigned index = StaticTable<bool>::Instantiable;
		target.bool_storage.read(StaticTable<bool>::configuration[index]);
		target.int_storage.read(StaticTable<int>::configuration[index]);
		target.float_storage.read(StaticTable<float>::configuration[index]);
		target.double_storage.read(StaticTable<double>::configuration[index]);
		target.string_storage.read(StaticTable<String>::configuration[index]);
	}
	
	void							Table::finalize()
	{
		if (finalized)
			return;
		StaticTable<bool>::finalize();
		StaticTable<int>::finalize();
		StaticTable<float>::finalize();
		StaticTable<double>::finalize();
		StaticTable<String>::finalize();
		
		
		finalized = true;
	
	
	}
	
	TAttribute					Table::findAttribute(const String&name)	const
	{
		TAttribute	a;
		if (name_map.query(name,a))
			return a;
		a.exists = false;
		return a;
	}
	
	size_t					Table::attributeLength(const TAttribute&attrib)
	{
		if (!attrib.exists)
			return 0;
		index_t index = attrib.copyable;
		switch (attrib.type)
		{
			case StringType:
				return StaticTable<String>::configuration[index].attributes[attrib.field_index].length;
			case BoolType:
				return StaticTable<bool>::configuration[index].attributes[attrib.field_index].length;
			case IntType:
				return StaticTable<int>::configuration[index].attributes[attrib.field_index].length;
			case FloatType:
				return StaticTable<float>::configuration[index].attributes[attrib.field_index].length;
			case DoubleType:
				return StaticTable<double>::configuration[index].attributes[attrib.field_index].length;
		}
		return 0;
	}
	
	size_t					Table::fieldLength(enAttributeType type, bool copyable)
	{
		index_t index = copyable;
		switch (type)
		{
			case StringType:
				return StaticTable<String>::configuration[index].attributes.count();
			case BoolType:
				return StaticTable<bool>::configuration[index].attributes.count();
			case IntType:
				return StaticTable<int>::configuration[index].attributes.count();
			case FloatType:
				return StaticTable<float>::configuration[index].attributes.count();
			case DoubleType:
				return StaticTable<double>::configuration[index].attributes.count();
		}
		return 0;
	}
	
	static inline bool validBool(unsigned val)
	{
		return val <= 1;
	}
	
	bool					Table::exists(const TAttribute&attrib)
	{
		return attrib.exists && attrib.field_index < fieldLength(attrib.type,attrib.copyable) && validBool(attrib.copyable);
	}
	
	const TAttributeConfig&		Table::attributeConfig(const TAttribute&attrib)
	{
		static TAttributeConfig	general_fault = {"",0};
		if (!exists(attrib))
			return general_fault;
		unsigned config = attrib.copyable,
				index = attrib.field_index;
		
		switch (attrib.type)
		{
			case StringType:
				return StaticTable<String>::configuration[config].attribute_names[index];
			case BoolType:
				return StaticTable<bool>::configuration[config].attribute_names[index];
			case IntType:
				return StaticTable<int>::configuration[config].attribute_names[index];
			case FloatType:
				return StaticTable<float>::configuration[config].attribute_names[index];
			case DoubleType:
				return StaticTable<double>::configuration[config].attribute_names[index];
		}
		return general_fault;
	
	
	}
	
	
	bool						Table::set(const TAttribute&attrib, const int*values, count_t len)
	{
		if (!finalized)
			finalize();
		if (!exists(attrib) || !len)
			return false;
		size_t alength = attributeLength(attrib);
		if (len > alength)
			len = alength;
		unsigned config = attrib.copyable;
		switch (attrib.type)
		{
			case StringType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<String>::configuration[config].attributes[attrib.field_index].field[i] = String(values[i]);
			return true;
			case BoolType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<bool>::configuration[config].attributes[attrib.field_index].field[i] = values[i]!=0;
			return true;
			case IntType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<int>::configuration[config].attributes[attrib.field_index].field[i] = values[i];
			return true;
			case FloatType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<float>::configuration[config].attributes[attrib.field_index].field[i] = values[i];
			return true;
			case DoubleType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<double>::configuration[config].attributes[attrib.field_index].field[i] = values[i];
			return true;
		}
		return false;
	}
	
	bool						Table::set(const TAttribute&attrib, const bool*values, count_t len)
	{
		if (!finalized)
			finalize();
		if (!exists(attrib) || !len)
			return false;
		size_t alength = attributeLength(attrib);
		if (len > alength)
			len = alength;
		unsigned config = attrib.copyable;
		switch (attrib.type)
		{
			case StringType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<String>::configuration[config].attributes[attrib.field_index].field[i] = values[i]?"true":"false";
			return true;
			case BoolType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<bool>::configuration[config].attributes[attrib.field_index].field[i] = values[i];
			return true;
			case IntType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<int>::configuration[config].attributes[attrib.field_index].field[i] = values[i];
			return true;
			case FloatType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<float>::configuration[config].attributes[attrib.field_index].field[i] = values[i];
			return true;
			case DoubleType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<double>::configuration[config].attributes[attrib.field_index].field[i] = values[i];
			return true;
		}
		return false;
	}
	
	bool						Table::set(const TAttribute&attrib, const float*values, count_t len)
	{
		if (!finalized)
			finalize();
		if (!exists(attrib) || !len)
			return false;
		
		size_t alength = attributeLength(attrib);
		if (len > alength)
			len = alength;
		unsigned config = attrib.copyable;

		switch (attrib.type)
		{
			case StringType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<String>::configuration[config].attributes[attrib.field_index].field[i] = String(values[i]);
			return true;
			case BoolType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<bool>::configuration[config].attributes[attrib.field_index].field[i] = values[i]!=0;
			return true;
			case IntType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<int>::configuration[config].attributes[attrib.field_index].field[i] = (int)values[i];
			return true;
			case FloatType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<float>::configuration[config].attributes[attrib.field_index].field[i] = values[i];
			return true;
			case DoubleType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<double>::configuration[config].attributes[attrib.field_index].field[i] = values[i];
			return true;
		}
		return false;
	}
	
	bool						Table::set(const TAttribute&attrib, const double*values, count_t len)
	{
		if (!finalized)
			finalize();
		if (!exists(attrib) || !len)
			return false;
		
		size_t alength = attributeLength(attrib);
		if (len > alength)
			len = alength;
		unsigned config = attrib.copyable;

		switch (attrib.type)
		{
			case StringType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<String>::configuration[config].attributes[attrib.field_index].field[i] = String(values[i]);
			return true;
			case BoolType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<bool>::configuration[config].attributes[attrib.field_index].field[i] = values[i]!=0;
			return true;
			case IntType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<int>::configuration[config].attributes[attrib.field_index].field[i] = (int)values[i];
			return true;
			case FloatType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<float>::configuration[config].attributes[attrib.field_index].field[i] = (float)values[i];
			return true;
			case DoubleType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<double>::configuration[config].attributes[attrib.field_index].field[i] = values[i];
			return true;
		}
		return false;
	}
	
	bool						Table::set(const TAttribute&attrib, const String*values, count_t len)
	{
		if (!finalized)
			finalize();
		if (!exists(attrib) || !len)
			return false;
		
		size_t alength = attributeLength(attrib);
		if (len > alength)
			len = alength;
		unsigned config = attrib.copyable;

		switch (attrib.type)
		{
			case StringType:
				for (unsigned i = 0; i < len; i++)
					StaticTable<String>::configuration[config].attributes[attrib.field_index].field[i] = values[i];
			return true;
			case BoolType:
				for (unsigned i = 0; i < len; i++)
					if (!convert(values[i].c_str(),StaticTable<bool>::configuration[config].attributes[attrib.field_index].field[i]))
						return false;
			return true;
			case IntType:
				for (unsigned i = 0; i < len; i++)
					if (!convert(values[i].c_str(),StaticTable<int>::configuration[config].attributes[attrib.field_index].field[i]))
						return false;
			return true;
			case FloatType:
				for (unsigned i = 0; i < len; i++)
					if (!convert(values[i].c_str(),StaticTable<float>::configuration[config].attributes[attrib.field_index].field[i]))
						return false;
			return true;
			case DoubleType:
				for (unsigned i = 0; i < len; i++)
					if (!convert(values[i].c_str(),StaticTable<double>::configuration[config].attributes[attrib.field_index].field[i]))
						return false;
			return true;
		}
		return false;
	}
	
	bool						Table::set(const TAttribute&attrib, const char*string)
	{
		if (!finalized)
			finalize();
		if (!exists(attrib) || !string)
			return false;
		
	
		switch (attrib.type)
		{
			case StringType:
				StaticTable<String>::configuration[attrib.copyable].attributes[attrib.field_index].field[0] = string;
			return true;
			case BoolType:
				return convert(string,StaticTable<bool>::configuration[attrib.copyable].attributes[attrib.field_index].field[0]);
			case IntType:
				return convert(string,StaticTable<int>::configuration[attrib.copyable].attributes[attrib.field_index].field[0]);
			case FloatType:
				return convert(string,StaticTable<float>::configuration[attrib.copyable].attributes[attrib.field_index].field[0]);
			case DoubleType:
				return convert(string,StaticTable<double>::configuration[attrib.copyable].attributes[attrib.field_index].field[0]);
		}
		return false;
	}
	

}

