#ifndef attrib_tableTplH
#define attrib_tableTplH

template <>
	inline bool						convert<String>(const char*from, String&to)
	{
		to = from;
		return true;
	}

namespace Attrib
{

	template <typename T>
		RootConfiguration<T>				StaticTable<T>::configuration[Count];
	
	


	template <typename T>
		inline enAttributeType				Table::typeOf()
		{
			return UnknownType;
		}
		
	template <>
		inline enAttributeType					Table::typeOf<int>()
		{
			return Int;
		}
	
	template <>
		inline enAttributeType					Table::typeOf<bool>()
		{
			return Boolean;
		}
	
	template <>
		inline enAttributeType					Table::typeOf<float>()
		{
			return Float;
		}
	
	template <>
		inline enAttributeType					Table::typeOf<double>()
		{
			return Double;
		}
		
	template <>
		inline enAttributeType					Table::typeOf<const char*>()
		{
			return StringType;
		}
	
	template <>
		inline enAttributeType					Table::typeOf<String>()
		{
			return StringType;
		}
		
		
			
	template <typename T>
		const TAttribute&				Table::reg(const String&name, unsigned flags, const T& initial_value)
		{
			finalized = false;
			bool copyable = (flags&Copyable)!=0;
			StaticTable<T>::configuration[copyable].attributes.append().length = 1;
			StaticTable<T>::configuration[copyable].attribute_names.append().set(name,flags);
			StaticTable<T>::configuration[copyable].values.append()= initial_value;
			TAttribute	&attrib = index_map.append();
			attrib.type = typeOf<T>();
			attrib.field_index = StaticTable<T>::configuration[copyable].attributes.fillLevel()-1;
			attrib.global_id = index_map.fillLevel()-1;
			attrib.exists = true;
			attrib.copyable = copyable;
			
			name_map.set(name,attrib);
			return attrib;
		}
		
	template <typename T>
		const TAttribute&					Table::reg(const String&name, unsigned flags, const T& initial_value0, const T& initial_value1)
		{
			finalized = false;
			bool copyable = (flags&Copyable)!=0;
			StaticTable<T>::configuration[copyable].attributes.append().length = 2;
			StaticTable<T>::configuration[copyable].attribute_names.append().set(name,flags);
			StaticTable<T>::configuration[copyable].values.append()= initial_value0;
			StaticTable<T>::configuration[copyable].values.append()= initial_value1;
			TAttribute	&attrib = index_map.append();
			attrib.type = typeOf<T>();
			attrib.field_index = StaticTable<T>::configuration[copyable].attributes.fillLevel()-1;
			attrib.global_id = index_map.fillLevel()-1;
			attrib.exists = true;
			attrib.copyable = copyable;
			
			name_map.set(name,attrib);
			return attrib;
		}
		
	template <typename T>
		const TAttribute&					Table::reg(const String&name, unsigned flags, const T& initial_value0, const T& initial_value1, const T& initial_value2)
		{
			finalized = false;
			bool copyable = (flags&Copyable)!=0;
			StaticTable<T>::configuration[copyable].attributes.append().length = 3;
			StaticTable<T>::configuration[copyable].attribute_names.append().set(name,flags);
			StaticTable<T>::configuration[copyable].values.append()= initial_value0;
			StaticTable<T>::configuration[copyable].values.append()= initial_value1;
			StaticTable<T>::configuration[copyable].values.append()= initial_value2;
			TAttribute	&attrib = index_map.append();
			attrib.type = typeOf<T>();
			attrib.field_index = StaticTable<T>::configuration[copyable].attributes.fillLevel()-1;
			attrib.global_id = index_map.fillLevel()-1;
			attrib.exists = true;
			attrib.copyable = copyable;
			
			name_map.set(name,attrib);
			return attrib;
		}
	
	template <typename T>
		const TAttribute&					Table::reg(const String&name, unsigned flags, const T initial_values[], unsigned length)
		{
			finalized = false;
			bool copyable = (flags&Copyable)!=0;
			StaticTable<T>::configuration[copyable].attributes.append().length = length;
			StaticTable<T>::configuration[copyable].attribute_names.append().set(name,flags);
			for (unsigned i = 0; i < length; i++)
				StaticTable<T>::configuration[copyable].values.append()= initial_values[i];
			TAttribute	&attrib = index_map.append();
			attrib.type = typeOf<T>();
			attrib.field_index = StaticTable<T>::configuration[copyable].attributes.fillLevel()-1;
			attrib.global_id = index_map.fillLevel()-1;
			attrib.exists = true;
			attrib.copyable = copyable;
			
			name_map.set(name,attrib);
			return attrib;//.global_id;
		}

	
	
	template <typename T>
		bool						Table::get(const TAttribute&attrib, T*target, count_t len)
		{
			if (!finalized)
				finalize();
			if (!exists(attrib))
				return false;
			size_t alength = attributeLength(attrib);
			if (len > alength)
				len = alength;
			unsigned config = attrib.copyable;
			
			switch (attrib.type)
			{
				case StringType:
					for (index_t i = 0; i < len; i++)
						if (!convert(StaticTable<String>::configuration[config].attributes[attrib.field_index].field[i],target[i]))
							return false;
				return true;
				case BoolType:
					for (index_t i = 0; i < len; i++)
						target[i] = (T)(int)StaticTable<bool>::configuration[config].attributes[attrib.field_index].field[i];
				return true;
				case IntType:
					for (index_t i = 0; i < len; i++)
						target[i] = (T)StaticTable<int>::configuration[config].attributes[attrib.field_index].field[i];
				return true;
				case FloatType:
					for (index_t i = 0; i < len; i++)
						target[i] = (T)StaticTable<float>::configuration[config].attributes[attrib.field_index].field[i];
				return true;
				case DoubleType:
					for (index_t i = 0; i < len; i++)
						target[i] = (T)StaticTable<double>::configuration[config].attributes[attrib.field_index].field[i];
				return true;
			}
			return false;
		}


	
	template <typename T>
		bool						Table::get(const TAttribute&attrib, T*target)
		{
			if (!finalized)
				finalize();
			if (!exists(attrib))
				return false;
			unsigned len = attributeLength(attrib);
			unsigned config = attrib.copyable;
			
			switch (attrib.type)
			{
				case StringType:
					for (unsigned i = 0; i < len; i++)
						if (!convert(StaticTable<String>::configuration[config].attributes[attrib.field_index].field[i],target[i]))
							return false;
				return true;
				case BoolType:
					for (unsigned i = 0; i < len; i++)
						target[i] = (T)(int)StaticTable<bool>::configuration[config].attributes[attrib.field_index].field[i];
				return true;
				case IntType:
					for (unsigned i = 0; i < len; i++)
						target[i] = (T)StaticTable<int>::configuration[config].attributes[attrib.field_index].field[i];
				return true;
				case FloatType:
					for (unsigned i = 0; i < len; i++)
						target[i] = (T)StaticTable<float>::configuration[config].attributes[attrib.field_index].field[i];
				return true;
				case DoubleType:
					for (unsigned i = 0; i < len; i++)
						target[i] = (T)StaticTable<double>::configuration[config].attributes[attrib.field_index].field[i];
				return true;
			}
			return false;
		}

	template <typename T>
		T						Table::get(const TAttribute&attrib)
		{
			if (!finalized)
				finalize();
			unsigned config = attrib.copyable;
			T result = (T)0.0f;
			if (!exists(attrib))
				return result;
			
			switch (attrib.type)
			{
				case StringType:
					convert(StaticTable<String>::configuration[config].attributes[attrib.field_index].field[0],result);
				break;
				case BoolType:
					result = (T)(int)StaticTable<bool>::configuration[config].attributes[attrib.field_index].field[0];
				break;
				case IntType:
					result = (T)StaticTable<int>::configuration[config].attributes[attrib.field_index].field[0];
				break;
				case FloatType:
					result = (T)StaticTable<float>::configuration[config].attributes[attrib.field_index].field[0];
				break;
				case DoubleType:
					result = (T)StaticTable<double>::configuration[config].attributes[attrib.field_index].field[0];
				break;
			}
			return result;
		}
	
	
	template <typename T>
		bool						Instance::get(const TAttribute&attrib, T*target, unsigned len)	const
		{
			if (!Table::exists(attrib) || !attrib.copyable)
				return false;
			unsigned alength = Table::attributeLength(attrib);
			if (len > alength)
				len = alength;
			
			switch (attrib.type)
			{
				case StringType:
					for (unsigned i = 0; i < len; i++)
						if (!convert(string_storage.attributes[attrib.field_index].field[i],target[i]))
							return false;
				return true;
				case BoolType:
					for (unsigned i = 0; i < len; i++)
						target[i] = (T)(int)bool_storage.attributes[attrib.field_index].field[i];
				return true;
				case IntType:
					for (unsigned i = 0; i < len; i++)
						target[i] = (T)int_storage.attributes[attrib.field_index].field[i];
				return true;
				case FloatType:
					for (unsigned i = 0; i < len; i++)
						target[i] = (T)float_storage.attributes[attrib.field_index].field[i];
				return true;
				case DoubleType:
					for (unsigned i = 0; i < len; i++)
						target[i] = (T)double_storage.attributes[attrib.field_index].field[i];
				return true;
			}
			return false;
		}


	
	template <typename T>
		bool						Instance::get(const TAttribute&attrib, T*target)	const
		{
			if (!Table::exists(attrib) || !attrib.copyable)
				return false;
			unsigned len = Table::attributeLength(attrib);
			
			switch (attrib.type)
			{
				case StringType:
					for (unsigned i = 0; i < len; i++)
						if (!convert(string_storage.attributes[attrib.field_index].field[i],target[i]))
							return false;
				return true;
				case BoolType:
					for (unsigned i = 0; i < len; i++)
						target[i] = (T)(int)bool_storage.attributes[attrib.field_index].field[i];
				return true;
				case IntType:
					for (unsigned i = 0; i < len; i++)
						target[i] = (T)int_storage.attributes[attrib.field_index].field[i];
				return true;
				case FloatType:
					for (unsigned i = 0; i < len; i++)
						target[i] = (T)float_storage.attributes[attrib.field_index].field[i];
				return true;
				case DoubleType:
					for (unsigned i = 0; i < len; i++)
						target[i] = (T)double_storage.attributes[attrib.field_index].field[i];
				return true;
			}
			return false;
		}

	template <typename T>
		T						Instance::get(const TAttribute&attrib)	const
		{
			unsigned config = attrib.copyable;
			T result = (T)0.0f;
			if (!Table::exists(attrib)|| !attrib.copyable)
				return result;
			
			switch (attrib.type)
			{
				case StringType:
					convert(string_storage.attributes[attrib.field_index].field[0],result);
				break;
				case BoolType:
					result = (T)(int)bool_storage.attributes[attrib.field_index].field[0];
				break;
				case IntType:
					result = (T)int_storage.attributes[attrib.field_index].field[0];
				break;
				case FloatType:
					result = (T)float_storage.attributes[attrib.field_index].field[0];
				break;
				case DoubleType:
					result = (T)double_storage.attributes[attrib.field_index].field[0];
				break;
			}
			return result;
		}

}



#endif
