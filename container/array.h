#ifndef arrayH
#define arrayH


#include <wchar.h>
#include "../interface/serializable.h"
#include "strategy.h"
#include <cstdarg>

#include "../general/typedefs.h"

#define __ARRAY_RVALUE_REFERENCES__	1		//!< Allows && references in constructors and assignment operators


#ifdef _DEBUG
#define __ARRAY_DBG_RANGE_CHECK__		//!< Checks index ranges in debug mode
#endif

#if SYSTEM==WINDOWS
	#define wcswcs	wcsstr
#endif

#undef ASSERT_CONCLUSION
#ifdef _DEBUG
	#define ASSERT_CONCLUSION(array,pointer)	{if (!array.IsConcludedBy(pointer)) FATAL__("Conclusion exception for array '" #array "', element '" #pointer "' ("+String(array.GetIndexOf(pointer))+"/"+String(array.GetLength())+")"); }
#else
	#define ASSERT_CONCLUSION(array,pointer)
#endif


/**
	@brief Experimental property helper macro

	This macro should allow to place property like single instance classes
	into a parent class. Overhead warning. Deprecated style.
*/
#define PROPERTY(TYPE,NAME,PARENT_CLASS,GET,SET)\
			class {\
				public:\
				inline PARENT_CLASS*	This()\
				{\
					return (PARENT_CLASS*)((BYTE*)this-((BYTE*)&((PARENT_CLASS*)1)->NAME-1));\
				}\
				inline const PARENT_CLASS*	This()	const\
				{\
					return (const PARENT_CLASS*)((const BYTE*)this-((const BYTE*)&((const PARENT_CLASS*)1)->NAME-1));\
				}\
				inline operator TYPE() const\
				{\
					return This()->GET();\
				}\
				inline void operator=(TYPE value)\
				{\
					This()->SET(value);\
				}\
			} NAME;



/**
	@brief Overload serilizable check (void*)

	The compiler defaults to this if the checked class is not a derivative of ISerializable.
	@return Always false
*/
inline	bool		IsISerializable(const void*)
					{
						return false;
					}
inline	bool		IsISerializable(const volatile void*)
					{
						return false;
					}
inline	size_t		GetFixedSize(const void*)
					{
						return 0;
					}

/**
	@brief Overload serilizable check (ISerializable*)

	@return Always true
*/
inline	bool		IsISerializable(const ISerializable*)
					{
						return true;
					}
inline	bool		IsISerializable(const volatile ISerializable*)
					{
						return true;
					}
inline	size_t		GetFixedSize(const ISerializable*serial)
					{
						return serial->HasFixedSize() ? serial->GetSerialSize(true) : 0;
					}

/**
	@brief Overload serilizable size query (void*)

	The compiler defaults to this if the checked class is not a derivative of ISerializable.

	@param	native_size Size of the passed object. This matches the effective return
			value of this function overload
	@param	export_size Unused. Defines whether or not any dynamic object size should be
			exported along with the object's payload. Not applicable for non-serializable
			objects
	@return The provided (native) data size
*/
inline	ISerializable::serial_size_t		GetSerialSizeOf(const void*, ISerializable::serial_size_t native_size, bool export_size)
											{
												//throw ERROR_MAKE__("bad");
												return native_size;
											}
inline	ISerializable::serial_size_t		GetSerialSizeOf(volatile const void*, ISerializable::serial_size_t native_size, bool export_size)
											{
												//throw ERROR_MAKE__("bad");
												return native_size;
											}

/**
	@brief Overload serilizable size query (ISerializable*)

	@param	object Serializable object to query the size of
	@param	native_size Size of the passed object. Unused by this function overload
	@param	export_size Defines whether or not any dynamic object size should be
			exported along with the object's payload. The object may or may not
			apply this parameter.
	@return The passed object's serial size
*/
inline	ISerializable::serial_size_t		GetSerialSizeOf(const ISerializable*object, ISerializable::serial_size_t native_size, bool export_size)
											{
												//throw ERROR_MAKE__("good");
												return object->GetSerialSize(export_size);
											}
inline	ISerializable::serial_size_t		GetSerialSizeOf(volatile const ISerializable*object, ISerializable::serial_size_t native_size, bool export_size)
											{
												return const_cast<const ISerializable*>(object)->GetSerialSize(export_size);
											}


inline	bool		SerializeObject(const void*object, ISerializable::serial_size_t native_object_size, IWriteStream&out_stream, bool export_size)
					{
						return out_stream.Write(object,native_object_size);
					}
inline	bool		SerializeObject(const volatile void*object, ISerializable::serial_size_t native_object_size, IWriteStream&out_stream, bool export_size)
					{
						return out_stream.Write(object,native_object_size);
					}

inline	bool		SerializeObject(const ISerializable*object, ISerializable::serial_size_t native_object_size, IWriteStream&out_stream, bool export_size)
					{
						return object->Serialize(out_stream,export_size);
					}
inline	bool		SerializeObject(const volatile ISerializable*object, ISerializable::serial_size_t native_object_size, IWriteStream&out_stream, bool export_size)
					{
						return const_cast<const ISerializable*>(object)->Serialize(out_stream,export_size);
					}


inline	bool		DeserializeObject(void*object, ISerializable::serial_size_t native_object_size, IReadStream&in_stream,ISerializable::serial_size_t fixed_size)
					{
						return in_stream.Read(object,native_object_size);
					}
inline	bool		DeserializeObject(volatile void*object, ISerializable::serial_size_t native_object_size, IReadStream&in_stream,ISerializable::serial_size_t fixed_size)
					{
						return in_stream.Read(object,native_object_size);
					}

inline	bool		DeserializeObject(ISerializable*object, ISerializable::serial_size_t native_object_size, IReadStream&in_stream,ISerializable::serial_size_t fixed_size)
					{
						return object->Deserialize(in_stream,fixed_size);
					}
inline	bool		DeserializeObject(volatile ISerializable*object, ISerializable::serial_size_t native_object_size, IReadStream&in_stream,ISerializable::serial_size_t fixed_size)
					{
						return const_cast<ISerializable*>(object)->Deserialize(in_stream,fixed_size);
					}

/**
	@brief Array with a fixed number of elements

	FixedArray 
*/

template <typename T, size_t Length>
	class FixedArray:public SerializableObject
	{
	public:	
		T						value[Length];
		typedef T*				iterator;
		typedef const T*		const_iterator;
			
		/**/					FixedArray()	{}
		/**/					FixedArray(std::initializer_list<T> l)
		{
			index_t at = 0;
			for (auto it = l.begin(); it != l.end() && at < Length; ++it)
				value[at++] = *it;
		}


		iterator				begin()
		{
			return value;
		}

		const_iterator			begin() const
		{
			return value;
		}

		iterator				end()
		{
			return value+Length;
		}

		const_iterator			end() const
		{
			return value+Length;
		}

		size_t					size() const {return Length;}
		count_t					Count() const {return Length;}

		/**
		Executes swap() on all elements
		*/
		void					swap(FixedArray<T,Length>&other)
		{
			for (index_t i = 0; i < Length; i++)
				value[i].swap(other.value[i]);
		}
		/**
		Executes adoptData() on all elements
		*/
		void					adoptData(FixedArray<T,Length>&other)
		{
			for (index_t i = 0; i < Length; i++)
				value[i].adoptData(other.value[i]);
		}

		void					Fill(const T&pattern)
		{
			for (index_t i = 0; i < Length; i++)
				value[i] = pattern;
		}

		virtual	serial_size_t	GetSerialSize(bool export_size) const	override
		{
			serial_size_t result = 0;
			for (index_t i = 0; i < Length; i++)
				result += GetSerialSizeOf(value+i,sizeof(T),true);//must pass true here because the individual object size cannot be restored from the global data size
			return result;
		}
			
		virtual	bool			Serialize(IWriteStream&out_stream, bool export_size) const	override
		{
			if (!IsISerializable((const T*)value))
				return out_stream.Write(value,sizeof(value));
	
			for (index_t i = 0; i < Length; i++)
				if (!SerializeObject(value+i,sizeof(T),out_stream,true))
					return false;
			return true;
		}
			
		virtual	bool			Deserialize(IReadStream&in_stream, serial_size_t)	override
		{
			if (!IsISerializable((const T*)value))
				return in_stream.Read(value,sizeof(value));
	
			for (index_t i = 0; i < Length; i++)
				if (!DeserializeObject(value+i,sizeof(T),in_stream,EmbeddedSize))
					return false;
			return true;
		}	
		
		virtual bool			HasFixedSize() const override
		{
			return GetFixedSize((const T*)value) != 0;
		}


		T&						operator[](index_t index)
		{
			#ifdef __ARRAY_DBG_RANGE_CHECK__
				if (index >= Length)
					FATAL__("Index out of bounds");
			#endif
			return value[index];
		}

		const T&						operator[](index_t index) const
		{
			#ifdef __ARRAY_DBG_RANGE_CHECK__
				if (index >= Length)
					FATAL__("Index out of bounds");
			#endif
			return value[index];
		}
	};


namespace Compare
{
	template <typename T0, typename T1=T0>
		inline int		Primitives(const T0&a, const T1&b)
		{
			if (a < b)
				return -1;
			if (a > b)
				return 1;
			return 0;
		}
}


class Arrays	//! Defines a number of array specific constants and types
{
public:
			typedef size_t			count_t,index_t;				//!< Array element count type
			static const count_t	Undefined=count_t(-1);	//!< Undefined length value
};


template <typename T>
	class FixedSizeSerializable
	{
	public:
		static size_t		GetSize()
		{
			static T		testElement;	//!< Used to test serializability
			return GetFixedSize(&testElement);
		}
	};


template <typename T>
	class ArrayRef
	{
	protected:
		T*				data;
		count_t			elements;

	public:
		typedef T*			iterator;
		typedef const T*	const_iterator;

		ArrayRef():data(nullptr),elements(0)	{}
		ArrayRef(T*data, count_t elements):data(data),elements(elements)	{}
		ArrayRef(T&element):data(&element),elements(1)	{}

		virtual ~ArrayRef()	{}

		template <typename I>
			inline	T*		operator+(I rel)	//! Retrieves a pointer to the nth element @param rel Relative index. 0 points to the first element in the array. 	@return Pointer to the requested element for sub array access
							{
								//#if defined(_DEBUG) && __ARRAY_DBG_RANGE_CHECK__
								//	if ((count_t)rel >= elements || rel < 0)
								//		FATAL__("Index out of bounds");
								//#endif
								return data+rel;
							}
				
		template <typename I>
			inline const T*	operator+(I rel) const	//! @copydoc operator+()
							{
								//#if defined(_DEBUG) && __ARRAY_DBG_RANGE_CHECK__
								//	if ((count_t)rel >= elements || rel < 0)
								//		FATAL__("Index out of bounds");
								//#endif
								return data+rel;
							}

		template <typename I>
			inline	T*		operator-(I rel)	//! Retrieves a pointer to the nth element @param rel Relative index. 0 points to the first element in the array. 	@return Pointer to the requested element for sub array access
							{
								//#if defined(_DEBUG) && __ARRAY_DBG_RANGE_CHECK__
								//	if ((count_t)-rel >= elements || rel > 0)
								//		FATAL__("Index out of bounds");
								//#endif
								return data-rel;
							}
				
		template <typename I>
			inline const T*	operator-(I rel) const	//! @copydoc operator-()
							{
								//#if defined(_DEBUG) && __ARRAY_DBG_RANGE_CHECK__
								//	if ((count_t)-rel >= elements || rel > 0)
								//		FATAL__("Index out of bounds");
								//#endif
								return data-rel;
							}
		inline	T*			pointer()			//! Explicit type conversion to a native array of the contained type \return Pointer to the first contained element or NULL if the local array is empty.
							{
								return data;
							}
		inline const T*		pointer() const		//! @copydoc pointer()
							{
								return data;
							}
		inline	T*			GetPointer()			//! Explicit type conversion to a native array of the contained type \return Pointer to the first contained element or NULL if the local array is empty.
							{
								return data;
							}
		inline const T*		GetPointer() const		//! @copydoc pointer()
							{
								return data;
							}

		inline	T&			operator[](index_t index)		//! Sub-element access \param index Index of the requested element (0 = first element) \return Reference to the requested element
							{
								#ifdef __ARRAY_DBG_RANGE_CHECK__
									if (index >= elements)
										FATAL__("Index out of bounds");
								#endif
								return data[index];
							}
		inline	const T&	operator[](index_t index) const	//! @copydoc operator[]()
							{
								#ifdef __ARRAY_DBG_RANGE_CHECK__
									if (index >= elements)
										FATAL__("Index out of bounds");
								#endif
								return data[index];
							}
		inline	T&			at(index_t index)		//! @copydoc operator[]()
							{
								#ifdef __ARRAY_DBG_RANGE_CHECK__
									if (index >= elements)
										FATAL__("Index out of bounds");
								#endif
								return data[index];
							}
		inline	const T&	at(index_t index) const	//! @copydoc at()
							{
								#ifdef __ARRAY_DBG_RANGE_CHECK__
									if (index >= elements)
										FATAL__("Index out of bounds");
								#endif
								return data[index];
							}
		inline	T&			GetFromEnd(index_t index)					//! Retrieves the nth element from the end of the array. fromEnd(0) is identical to last()
							{
								index = elements - index - 1;
								#ifdef __ARRAY_DBG_RANGE_CHECK__
									if (index >= elements)
										FATAL__("Index out of bounds");
								#endif
								return data[index];
							}
		inline	const T&	GetFromEnd(index_t index)			const	//! @copydoc fromEnd()
							{
								index = elements - index - 1;
								#ifdef __ARRAY_DBG_RANGE_CHECK__
									if (index >= elements)
										FATAL__("Index out of bounds");
								#endif
								return data[index];
							}

		template <typename T1>
			inline int		CompareTo(const ArrayRef<T1>&other, int comparer(const T&, const T1&) ) const	//! Compares the local array with the remote array. The objects of the local array must implement a compareTo method that accepts objects of the remote array
							{
								count_t len = elements < other.length()?elements:other.length();
								for (count_t i = 0; i < len; i++)
								{
									char val = comparer(data[i],other[i]);
									if (val != 0)
										return val;
								}

								if (elements > other.length())
									return 1;
								if (elements < other.length())
									return -1;
								return 0;
							}

		template <typename T1>
			inline int		compareTo(const ArrayRef<T1>&other) const	//! Compares the local array with the remote array. The objects of the local array must implement a compareTo method that accepts objects of the remote array
							{
								count_t len = elements < other.length()?elements:other.length();
								for (count_t i = 0; i < len; i++)
								{
									char val = data[i].compareTo(other[i]);
									if (val != 0)
										return val;
								}

								if (elements > other.length())
									return 1;
								if (elements < other.length())
									return -1;
								return 0;
							}

		template <typename T1>
			inline int		CompareTo(const ArrayRef<T1>&other) const	/**@copydoc compareTo()*/ {return compareTo(other);}

		template <typename T1>
			inline bool		operator==(const ArrayRef<T1>&other) const //! Equality query \return true if all elements of the local array are identical to their respective counter parts in \b other. Equality is queried via the = operator.
							{
								if (elements != other.length())
									return false;
								for (count_t i = 0; i < elements; i++)
									if (data[i] != other[i])
										return false;
								return true;
							}
				
		template <typename T1>
			inline bool		operator!=(const ArrayRef<T1>&other) const //! Equality query
							{
								if (elements != other.length())
									return true;
								for (count_t i = 0; i < elements; i++)
									if (data[i] != other[i])
										return true;
								return false;
							}

		template <typename T1>
			inline bool		operator>(const ArrayRef<T1>&other) const //! Dictionary comparison
							{
								count_t len = elements < other.length()?elements:other.length();
								for (count_t i = 0; i < len; i++)
									if (data[i] > other[i])
										return true;
									else
										if (data[i] < other[i])
											return false;
								return elements > other.length();
							}

		template <typename T1>
			inline bool		operator<(const ArrayRef<T1>&other) const //! Dictionary comparison
							{
								count_t len = elements < other.length()?elements:other.length();
								for (count_t i = 0; i < len; i++)
									if (data[i] > other[i])
										return false;
									else
										if (data[i] < other[i])
											return true;
								return elements < other.length();
							}

		template <typename T1>
			inline bool		operator>=(const ArrayRef<T1>&other) const //! Dictionary comparison
							{
								count_t len = elements < other.length()?elements:other.length();
								for (count_t i = 0; i < len; i++)
									if (data[i] > other[i])
										return true;
									else
										if (data[i] < other[i])
											return false;
								return true;
							}

		template <typename T1>
			inline bool		operator<=(const ArrayRef<T1>&other) const //! Dictionary comparison
							{
								count_t len = elements < other.length()?elements:other.length();
								for (count_t i = 0; i < len; i++)
									if (data[i] > other[i])
										return false;
									else
										if (data[i] < other[i])
											return true;
								return true;
							}

		template <typename T1>
			inline	void	Fill(const T1&element, count_t offset=0, count_t max=InvalidIndex)	//! Sets up to \b max elements starting from @b offset of the local array to \b element \param element Element to repeat @param offset First index \param max If specified: Maximum number of elements to set to \b element
							{
								if (!elements)
									return;
								if (offset >= elements)
									offset = elements-1;
								if (max > elements)
									max = elements;
								for (count_t i = offset; i < max; i++)
									data[i] = (T)element;
							}

		inline	bool		IsEmpty()	const	//! Checks if the local array is empty
							{
								return elements == 0;
							}
		inline	bool		IsNotEmpty()	const	//! Checks if the local array is not empty (contains at least one element)
							{
								return elements != 0;
							}

		inline bool			IsConcludedBy(const T*element)	const	//! Queries if the specified entry pointer points exactly one past the last element.
							{
								return element == data+elements;
							}

		inline bool			Owns(const T*element)	const	//! Queries if the specified entry pointer was taken from the local array. Actual pointer address is checked, not what it points to.
							{
								return element >= data && element < data+elements;
							}
		inline index_t		GetIndexOf(const T*element)	const	//! Queries the index of the specifed element with 0 being the first element. To determine whether ot not the specified element is member of this array use the Owns() method.
							{
								return element-data;
							}				
		inline count_t		findFirst(const T&entry) const	//!< Finds the index of the first occurance of the specified entry. Entries are compared via the == operator. @param entry Entry to look for @return Index of the found match plus one or 0 if no match was found
							{
								for (count_t i = 0; i < elements; i++)
									if (data[i] == entry)
										return i+1;
								return 0;
							}

		inline bool			Contains(const T&entry) const	{	return FindFirst(entry) != 0;	}

		template <class T1>
			inline index_t	FindFirst(const T1*field, count_t length) const
							{
								if (length > elements)
									return 0;
								for (count_t i = 0; i+length <= elements; i++)
								{
									bool match(true);
									for (count_t j = 0; j < length; j++)
										if (data[i+j] != field[j])
										{
											match = false;
											break;
										}
										if (match)
											return i+1;
								}
								return 0;
							}

		inline index_t		FindLast(const T&entry) const
							{
								for (count_t i = elements-1; i < elements; i--)
									if (data[i] == entry)
										return i+1;
								return 0;
							}

		template <class T1>
			inline index_t	FindLast(const T1*field, count_t length) const
							{
								if (length > elements)
									return 0;
								for (count_t i = elements-length; i<elements; i--)
								{
									bool match(true);
									for (count_t j = 0; j < length; j++)
										if (data[i+j] != field[j])
										{
											match = false;
											break;
										}
										if (match)
											return i+1;
								}
								return 0;
							}

		template <class T1>
			inline index_t	FindFirst(const T1*string) const
							{
								const T1*terminator(string);
								while (*terminator++);
								return FindFirst(string,terminator-string-1);
							}

		template <class T1>
			inline index_t	FindLast(const T1*string) const
							{
								const T1*terminator(string);
								while (*terminator++);
								return FindLast(string,terminator-string-1);
							}

		template <class T1>
			inline index_t	FindFirst(const ArrayRef<T1>&other) const
							{
								return FindFirst(other.data,other.elements);
							}

		template <class T1>
			inline index_t	FindLast(const ArrayRef<T1>&other) const
							{
								return FindLast(other.data,other.elements);
							}

		inline iterator		begin()			{return data;}
		inline const_iterator begin() const	{return data;}
		inline iterator		end()			{return data+elements;}
		inline const_iterator end() const	{return data+elements;}

		inline T&			first()	//!< Retrieves a reference to the first element in the field. The method will return an undefined result if the local array is empty
							{
								#ifdef __ARRAY_DBG_RANGE_CHECK__
									if (!elements)
										FATAL__("Array is empty - cannot retrieve first()");
								#endif
								return data[0];
							}

		inline const T&		first()	const
							{
								#ifdef __ARRAY_DBG_RANGE_CHECK__
									if (!elements)
										FATAL__("Array is empty - cannot retrieve first()");
								#endif

								return data[0];
							}

		inline T&			last()
							{
								#ifdef __ARRAY_DBG_RANGE_CHECK__
									if (!elements)
										FATAL__("Array is empty - cannot retrieve last()");
								#endif

								return data[elements-1];
							}

		inline const T&		last()	const
							{
								#ifdef __ARRAY_DBG_RANGE_CHECK__
									if (!elements)
										FATAL__("Array is empty - cannot retrieve last()");
								#endif
								return data[elements-1];
							}

		inline T&			First()	//!< Retrieves a reference to the first element in the field. The method will return an undefined result if the local array is empty
							{
								return first();
							}

		inline const T&		First()	const
							{
								return first();
							}

		inline T&			Last()
							{
								return last();
							}

		inline const T&		Last()	const
							{
								return last();
							}

		inline count_t		length()	const		//! Queries the current array size in elements \return Number of elements 
							{
								return elements;
							}
		inline count_t		GetLength()	const		/**@copydoc length()*/ {return length();}
		inline count_t		count()	const		//! Queries the current array size in elements \return Number of elements 
							{
								return elements;
							}
		inline count_t		Count()	const		/**@copydoc length()*/ {return length();}
		inline count_t		size()	const		//! Queries the current array size in elements \return Number of elements 
							{
								return elements;
							}
		inline size_t		GetContentSize()	const		//! Returns the summarized size of all contained elements in bytes \return Size of all elements
							{
								return elements*sizeof(T);
							}

		inline bool			IsTerminatedBy(const T&element)	const
							{
								return elements && data[elements-1] == element;
							}
	};


/**
@brief Array data containment

ArrayData provides all non-strategy dependent functionality that array usage requires. Methods or functions that do not require strategy-dependent functionality may accept ArrayData instead.
*/
template <class C>
	class ArrayData: public ArrayRef<C>, public SerializableObject, public Arrays
	{
		typedef ArrayRef<C>	Super;
	protected:
		using Super::data;
		using Super::elements;

		/**/				ArrayData(count_t length=0):Super(alloc<C>(length),length)	{}
		/**/				ArrayData(const C&e0, const C&e1):Super(alloc<C>(2),2)
							{
								data[0] = e0;
								data[1] = e1;
							}
		/**/				ArrayData(const C&e0, const C&e1, const C&e2):Super(alloc<C>(3),3)
							{
								data[0] = e0;
								data[1] = e1;
								data[2] = e2;
							}
		/**/				ArrayData(const C&e0, const C&e1, const C&e2, const C&e3):Super(alloc<C>(4),4)
							{
								data[0] = e0;
								data[1] = e1;
								data[2] = e2;
								data[3] = e3;
							}

	public:
		static const count_t	Undefined=Arrays::Undefined;
	private:

		/**/				ArrayData(const ArrayData<C>&){}
		/**/				ArrayData(const ArrayRef<C>&){}
		void				operator=(const ArrayData<C>&){}
		void				operator=(const ArrayRef<C>&){}
	public:
		#if __ARRAY_RVALUE_REFERENCES__
			/**/			ArrayData(ArrayData<C>&&rvalue):Super(rvalue.data,rvalue.elements)
							{
								rvalue.data = NULL;
								rvalue.elements = 0;
							}
			ArrayData<C>&	operator=(ArrayData<C>&&rvalue)
							{
								adoptData(rvalue);
								return *this;
							}
		#endif
		virtual				~ArrayData() override
							{
								if (!application_shutting_down)
									dealloc(data);
							}



				
		inline	void		free()		//! Frees the contained data and resets the local array length to 0
							{
								dealloc(data);
								elements = 0;
								data = NULL;
							}
		inline	void		clear()		/** @copydoc free() */{free();}
		inline	void		Clear()		/** @copydoc free() */{free();}
		inline	void		Free()		/** @copydoc free() */{free();}
			





		inline	void		set(count_t num_values, ...)	//! Resizes the local array and fills it with the specified elements. The elements must be specified in the type of the local array's data
							{
								setSize(num_values);
								va_list vl;
								va_start( vl, num_values );
								for (index_t i = 0; i < num_values; i++)
									data[i] = va_arg( vl, C );
							}

		template <typename T>
			inline	void	set2(const T&x, const T&y)	//! Resizes the local array and fills it with the specified elements. The elements must be specified in the type of the local array's data
							{
								setSize(2);
								data[0] = C(x);
								data[1] = C(y);
							}

		template <typename T>
			inline	void	set3(const T&x, const T&y, const T&z)	//! Resizes the local array and fills it with the specified elements. The elements must be specified in the type of the local array's data
							{
								setSize(3);
								data[0] = C(x);
								data[1] = C(y);
								data[2] = C(z);
							}

		template <typename T>
			inline	void	set4(const T&x, const T&y, const T&z, const T&w)	//! Resizes the local array and fills it with the specified elements. The elements must be specified in the type of the local array's data
							{
								setSize(4);
								data[0] = C(x);
								data[1] = C(y);
								data[2] = C(z);
								data[3] = C(w);
							}


		inline	void		adoptData(ArrayData<C>&other)	//! Clears any existing local data, adopts pointer and size and sets both NULL of the specified origin array.
							{
								if (this == &other)
									return;
								free();
								data = other.data;
								elements = other.elements;
								other.data = NULL;
								other.elements = 0;
							}

		inline	void		swap(ArrayData<C>&other)
							{
								swp(data,other.data);
								swp(elements,other.elements);
							}



		inline	void		setSize(count_t new_size)	/** @brief Resizes the array. The new array's content is constructed but uninitialized. \param new_size New array size in elements (may be 0) */
							{
								reloc(data,elements,new_size);
							}
		inline	void		SetSize(count_t new_size)	/** @copydoc setSize **/
							{
								reloc(data,elements,new_size);
							}
			

		//template <class T>


		virtual	serial_size_t	GetSerialSize(bool export_size) const	override
							{
								serial_size_t result = 0;
								if (export_size || (IsISerializable(data) && !FixedSizeSerializable<C>::GetSize()))
									result = GetSerialSizeOfSize((serial_size_t)elements);
								for (index_t i = 0; i < elements; i++)
									result += GetSerialSizeOf((const C*)data+i,sizeof(C),true);//must pass true here because the individual object size cannot be restored from the global data size
								return result;
							}

		virtual	bool		Serialize(IWriteStream&out_stream, bool export_size) const	override
							{
								if (export_size || (IsISerializable(data) && !FixedSizeSerializable<C>::GetSize()))
									if (!out_stream.WriteSize(elements))
										return false;
								if (!IsISerializable(data))
								{
									return out_stream.Write(data,(serial_size_t)Super::GetContentSize());
								}

								for (index_t i = 0; i < elements; i++)
								{
									if (!SerializeObject(data+i,sizeof(C),out_stream,true))
										return false;
								}
								return true;
							}

		virtual	bool		Deserialize(IReadStream&in_stream, serial_size_t fixed_size)	override
							{
								count_t size;
								if (fixed_size == EmbeddedSize)
								{
									//cout << "reading embedded size "<<endl;
									if (!in_stream.ReadSize(size))
										return false;
									//cout << "deciphered "<<size<<" elements"<<endl;
								}
								else
								{
									//cout << "using fixed size "<<endl;
									if (!IsISerializable(data))
										size = (count_t)(fixed_size/sizeof(C));
									else
									{
										size_t fixedElementSize = FixedSizeSerializable<C>::GetSize();
										if (fixedElementSize != 0)
										{
											size = (count_t)(fixed_size/fixedElementSize);
										}
										else
											if (!in_stream.ReadSize(size))
												return false;
									}
						
										//FATAL__("trying to Deserialize an array containing serializable objects from a fixed size stream data section not including any element count");
								}

								setSize(size);
								if (!IsISerializable(data))
								{
									//cout << "data is not of i-serializable type. reading plain"<<endl;
									return in_stream.Read(data,(serial_size_t)Super::GetContentSize());
								}

								//cout << "data is of i-serializable type. invoking deserializers"<<endl;
								for (index_t i = 0; i < elements; i++)
								{
									if (!DeserializeObject(data+i,sizeof(C),in_stream,EmbeddedSize))
									{
										//cout << "deserialization of element "<<i<<" with embedded size failed"<<endl;
										return false;
									}
								}

								return true;
							}
	};


DECLARE_T__(ArrayData,SwapStrategy);


	/*
		Verification array<T> (Array<T,Strategy>)

		Notes:
			for (a,b:T): a = b is defined as true if both a and b are undefined/uninitialized

		Abstract array: Field = Sequence 0...N of elements of type T
		Concrete array: state St = (count_t L, T*A)
		Invariants:
			* if L > 0: A is not NULL, and field of type T and length L (in elements)
			* if L = 0: A is NULL
			conversely:
				*if A is NULL: L is 0
				*if A is not NULL: L > 0

		
		for all states St whoes invariants are satisfied
			F: state St -> Sequence of elements of type T
				St.L > 0: F(St) = Sequence St.A[0]...St.A[St.L-1] of type T
				St.L = 0: F(St) = Empty sequence


		create: count_t -> array<T>
			abstract:
				create<array<T>>(count_t parameter) = 
					parameter > 0: Sequence A[0]...A[parameter-1] of type T; A[i] is of value create<T> (parameterless, undefined value for native types)
					parameter = 0: Empty sequence
			concrete:
				f_create<array<T>>(count_t parameter) -> state S:
					St.L <- parameter
					St.A <- alloc<T>(parameter)	(alloc<T> is verified to generate an array of length 'parameter' elements, and NULL if 'parameter' is 0. Elements are initialized without parameter)

					F(St) = 
						St.L > 0: Sequence St.A[0]...St.A[elements-1] of type T; St.A[i] is of value create<T> (parameterless, undefined value for native types)
						St.L = 0: Empty sequence
			
			-> correct

		append: state x const T* x count_t -> state
			abstract:
				append(Sequence<T> A = F(state St), Sequence<T> B = F(state(count_t L,const T*A))) =
					|A| = 0 and |B| = 0: Empty sequence
					|A| = 0 and |B| > 0: B
					|A| > 0 and |B| = 0: A
					otherwise: Sequence A[0]...A[|A|-1], B[0]...B[|B|-1] of type T
			concrete:
				f_append(state St, const T*A, count_t L) -> state St':
					St'.A <- alloc<T>(St.L+L);
					St'.L <- St.L+L;
					Strategy::moveElements(St.A,St'.A,St.L);	//move elements from old array over to new array (simply copy in most basic cases)
					dealloc<T>(St.A);							//deallocates a dynamic array. Doesn't do anything if St.A is NULL
					Strategy::copyElements(A,St'.A+St.L,L);		//explicitly copy elements from import array over to appropriate location in new field

				F(St') =
					St.L = 0 and L = 0:
						{
							implied by invariants:
								St.A = NULL
							St'.A <- alloc<T>(0+0); (=NULL)
							St'.L <- 0+0;							//invariant satisfaction (St'.L is 0 as required by St'.A = NULL)
							Strategy::moveElements(NULL,NULL,0);	//no effect
							dealloc<T>(NULL);						//no effect
							Strategy::copyElements(A,NULL+0,0);		//no effect
						}
						= F(state(0,NULL)) = Empty sequence

					St.L = 0 and L > 0:
						{
							implied by invariants:
								St.A = NULL
							St'.A <- alloc<T>(0+L); (=field of type T and length L)
							St'.L <- 0+L; (=L)						//invariant satisfaction (St'.A is field of type T and length St'.L)
							Strategy::moveElements(NULL,St'.A,0);	//no effect
							dealloc<T>(NULL);						//no effect
							Strategy::copyElements(A,St'.A+0,L);	//=Strategy::copyElements(A,St'.A,L) => copy L elements from A to St'.A. St'.L is L, so all elements of St' are defined by (L,A)
						}
						= Sequence A[0]...A[L-1] of type T (= Sequence<T> B where B = F(state(L,A)))

					St.L > 0 and L = 0:
						{
							implied by invariants:
								St.A != NULL
								St.A is field of type T and length St.L
							St'.A <- alloc<T>(St.L+0); (=field of type T and length St.L)
							St'.L <- St.L+0; (=St.L)					//invariant satisfaction (St'.A is field of type T and length St'.L)
							Strategy::moveElements(St.A,St'.A,St.L);	//Move L elements from St.A to St'.A. Both fields are of length St.L (=St'.L). St' is now completely defined by St
							dealloc<T>(St.A);							//discard old array
							Strategy::copyElements(A,St'.A+St.L,0);		//no effect
						}
						= Sequence St.A[0]...St.A[St.L-1] of type T (= Sequence<T> A where A = F(St))

					St.L > 0 and L > 0:
						{
							implied by invariants:
								St.A != NULL
								St.A is field of type T and length St.L
							St'.A <- alloc<T>(St.L+L);					//St'.A is now a field exactly long enough to contain all elements of both the previous state array St.A and append array A
							St'.L <- St.L+L;							//invariant satisfaction (St'.A is field of type T and length St'.L)
							Strategy::moveElements(St.A,St'.A,St.L);	//Move elements St.A[0]...St.A[St.L-1] of the previous state array St.A over to the new array St'.A. Side-note: At least one element is moved (St.L >= 1), and at least one element of St'.A is still uninitialized (L >= 1)
							dealloc<T>(St.A);							//deallocate old array
							Strategy::copyElements(A,St'.A+St.L,L);		//Copy A[0]...A[L-1] to the memory section immediately succeeding the section required for the elements of the old state St
						}
						= Sequence St.A[0]...St.A[St.L-1], A[0]...A[L-1] (= Sequence A[0]...A[|A|-1], B[0]...B[|B|-1] where A = F(St) and B = F(state(L,A)))
			-> correct


		The verification assumes that invoked functions are correct as well, which cannot be predicted in all cases
	*/

/*!

	\brief	Generalized array container
	
	Array is a generalized array management structure, implicitly performing many operations that manual array handling
	requires. The contained array content is automatically copied, allocated or deallocated where needed and shielded
	from inappropriate (and usually undesired) operations.
*/
template <class C, class Strategy=typename StrategySelector<C>::Default>
	class Array: public ArrayData<C>
	{
		typedef ArrayData<C>	Super;
		protected:
			#ifdef __GNUC__
				using ArrayData<C>::data;
				using ArrayData<C>::elements;
			#endif
		public:
			typedef typename Super::iterator iterator;
			typedef typename Super::const_iterator const_iterator;
			static const count_t	Undefined=Arrays::Undefined;

			typedef	Strategy	AppliedStrategy;

			typedef ArrayData<C>	Data;

			explicit	Array(count_t length=0):Data(length)	/** Creates a new array \param length Length of the new array object */ {}
			explicit	Array(const C&e0, const C&e1):Data(e0,e1)	/** Creates a new 2-element array @param e0 First element to Fill into the array @param e1 Second element to Fill into the array*/ {}
			explicit	Array(const C&e0, const C&e1, const C&e2):Data(e0,e1,e2)	/** Creates a new 3-element array @param e0 First element to Fill into the array @param e1 Second element to Fill into the array @param e2 Third element to Fill into the array*/ {}
			explicit	Array(const C&e0, const C&e1, const C&e2, const C&e3):Data(e0,e1,e2,e3)	/** Creates a new 4-element array @param e0 First element to Fill into the array @param e1 Second element to Fill into the array @param e2 Third element to Fill into the array @param e3 Fourth element to Fill into the array*/ {}
				
			inline		Array(const C*string)
			{
				if (string)
				{
					const C*terminator(string);
					while (*terminator++);
					elements = terminator-string;
				}
				else
					elements = 0;
				alloc(data,elements);
				Strategy::copyElements(string,data,elements);
			}
			
			template <class T>
				inline	Array(const T*field, count_t length):Data(length)	//!< Array copy constructor \param field Array that should be copies \param length Number of elements, field contains
				{
					Strategy::copyElements(field,data,elements);
				}
				
			template <class T,class OtherStrategy>
				inline	Array(const Array<T,OtherStrategy>&other):Data(other.count())
				{
					HybridStrategy<Strategy,OtherStrategy>::copyElements(other.GetPointer(),data,elements);
				}

				inline	Array(const std::initializer_list<C>&other):Data(other.size())
				{
					std::initializer_list<C>::const_iterator it = other.begin();
					index_t at = 0;
					for (;it != other.end(); ++it)
					{
						Super::data[at++] = *it;
					}
				}
				
				inline	Array(const Array<C,Strategy>&other):Data(other.count())
				{
					Strategy::copyElements(other.data,data,elements);
				}
				
			template <class T>
				inline	Array(const ArrayRef<T>&other):Data(other.count())
				{
					Strategy::copyElements(other.GetPointer(),data,elements);
				}
				
				inline	Array(const ArrayRef<C>&other):Data(other.Count())
				{
					Strategy::copyElements(other.GetPointer(),data,elements);
				}

		#if __ARRAY_RVALUE_REFERENCES__
				template <class OtherStrategy>
					Array(Array<C,OtherStrategy>&&rvalue):Data(0)
					{
						this->adoptData(rvalue);
					}


				template <class OtherStrategy>
					Array<C,Strategy>&	operator=(Array<C,OtherStrategy>&&rvalue)
					{
						this->adoptData(rvalue);
						return *this;
					}
		#endif

				
					inline Array<C,Strategy>& operator=(const Array<C,Strategy>&other) //! Assignment operator
					{
						if (&other == this)
							return *this;
						reloc(data,elements,other.elements);
						Strategy::copyElements(other.data,data,elements);
						return *this;
					}

					inline Array<C,Strategy>& operator=(const ArrayRef<C>&other) //! Assignment operator
					{
						if (&other == this)
							return *this;
						reloc(data,elements,other.Count());
						Strategy::copyElements(other.GetPointer(),data,elements);
						return *this;
					}
				
				template <class T, class OtherStrategy>
					inline Array<C,Strategy>& operator=(const Array<T,OtherStrategy>&other) //! Assignment operator. Copies each element via the = operator
					{
						if ((const Array<C,Strategy>*)&other == this)	//this should not happen but anyway
							return *this;
						reloc(data,elements,other.count());
						HybridStrategy<Strategy,OtherStrategy>::copyElements(other.pointer(),data,elements);
						return *this;
					}
				
				
				template <class T>
					inline Array<C,Strategy>& operator=(const ArrayRef<T>&other) //! Assignment operator. Copies each element via the = operator
					{
						if ((const ArrayRef<C>*)&other == this)	//this should not happen but anyway
							return *this;
						reloc(data,elements,other.count());
						Strategy::copyElements(other.GetPointer(),data,elements);
						return *this;
					}
				
							
					inline	C		shiftDown()	//!< Moves all array elements down, starting with the first, finishing with one before the last element. Elements are copied via the = operator. The original first element of the array will be removed from the array, the last one duplicated. @return Original (dropped) first element
					{
						if (!elements)
							return C();
						C first = data[0];
						for (index_t i = 0; i < elements-1; i++)
							Strategy::move(data[i+1],data[i]);
						return first;
					}
				
					inline	C		shiftUp()	//!< Moves all array elements up, starting with the last, finishing with one past the first element. Elements are copied via the = operator. The original last element of the array will be removed from the array, the first one duplicated. @return Original (dropped) last element
					{
						if (!elements)
							return C();
						C last = data[elements-1];
						for (index_t i = elements-1; i >0; i--)
							Strategy::move(data[i-1],data[i]);
						return last;
					}
				
			
				template <class T>
					inline void	copyFrom(const T*origin, count_t max=Undefined) //! Copies all elements from \b origin via the = operator \param origin Array to copy from (may be of a different entry type) \param max Maximum number of elements to read
					{
						if (max > elements)
							max = elements;

						Strategy::copyElements(origin,data,max);
					}
				template <class T>
					inline void	copyFrom(index_t target_offset, const T*origin, count_t max=Undefined) //! Copies all elements from \b origin via the = operator \param origin Array to copy from (may be of a different entry type) \param max Maximum number of elements to read
					{
						if (target_offset >= elements)
							return;
						if (max > elements - target_offset)
							max = elements - target_offset;

						Strategy::copyElements(origin,data+target_offset,max);
					}

					inline	void	importFrom(C*origin, count_t max=Undefined)	//! Moves up to @b max elements from @b origin to the local array. The type of movement depends on the used strategy
					{
						if (max > elements)
							max = elements;
						Strategy::moveElements(origin,data,max);
					}
				

			

				
					inline	void	resizeAndImport(C*origin, count_t length)	//!< Resizes the local field and imports the specified number of elements (see import())
					{
						Super::setSize(length);
						Strategy::moveElements(origin,data,length);
					}
				
				template <class T>
					inline void	resizeAndCopy(const T*origin, count_t length) //! Copies all elements from \b origin via the = operator overwriting any existing local elements \param origin Array to copy from (may be of a different entry type) \param count Number of elements to copy
					{
						Super::setSize(length);
						Strategy::copyElements(origin,data,length);
					}

				
				template <class T, class OtherStrategy>
					inline void	resizeAndCopy(const Array<T,OtherStrategy>&origin, count_t max=Undefined) //! Copies all elements from \b origin via the = operator overwriting any existing local elements \param origin Array to copy from (may be of a different entry type) \param max Maximum number of elements to read
					{
						if (max > origin.count())
							max = origin.count();
						Super::setSize(max);
						HybridStrategy<Strategy,OtherStrategy>::copyElements(origin.pointer(),data,max);
					}
			
					inline C&	append()	//!< Appends a singular element to the end of the array and returns a reference to it. append() has to allocate a new array, copy the contents of the old array and delete the old array. Copying is performed via the = operator @return Reference to the appended element
					{
						resizePreserveContent(elements+1);
						return data[elements-1];
					}

					inline C&	insert(index_t offset)
					{
						if (offset >= elements)
							return append();

						C	*new_field = alloc<C>(elements+1),
							*at = new_field;

						Strategy::moveElements(data,		new_field,			offset);
						Strategy::moveRange(data+offset,	data+elements,	new_field+offset+1);

						/*
						for (count_t i = 0; i < offset; i++)
							Strategy::move(data[i],*at++);
						for (count_t i = 0; i < max; i++)
							Strategy::move(origin.data[i],*at++);
						for (count_t i = offset; i < elements; i++)
							Strategy::move(data[i],*at++);*/
						elements ++;
						//ASSERT_EQUAL__(at,new_field+elements);
						dealloc(data);
						data = new_field;
						return data[offset];
					}

					inline void	insert(index_t offset, const C&element)
					{
						insert(offset) = element;
					}
			
					inline void appendImport(Array<C,Strategy>&origin, count_t max=Undefined)	//! Appends some or all elements of \b origin to the end of the local array. Movement behavior is defined by the used strategy \param origin Array to copy from \param max Maximum number of elements to append
					{
						if (max > origin.elements)
							max = origin.elements;
						count_t old_elements = elements;
						resizePreserveContent(elements+max);
						Strategy::moveElements(origin.data, data+old_elements, max);
					}

					inline void appendImport(C*import_data, count_t import_length)
					{
						count_t old_elements = elements;
						resizePreserveContent(elements+import_length);
						Strategy::moveElements(import_data, data+old_elements, import_length);
					}
			
				template <class T, class OtherStrategy>
					inline void appendCopy(const Array<T,OtherStrategy>&origin, count_t max=Undefined)	//! Appends some or all elements of \b origin to the end of the local array. Elements are duplicated explicitly using the = operator (strategy is ignored) \param origin Array to copy from \param max Maximum number of elements to append
					{
						if (max > origin.count())
							max = origin.count();
						count_t old_elements = elements;
						resizePreserveContent(elements+max);
						HybridStrategy<Strategy,OtherStrategy>::copyElements(origin.pointer(),data+old_elements,max);
					}

					inline void appendCopy(const ArrayData<C>&origin, count_t max=Undefined)	//! Appends some or all elements of \b origin to the end of the local array. Elements are duplicated explicitly using the = operator (strategy is ignored) \param origin Array to copy from \param max Maximum number of elements to append
					{
						if (max > origin.count())
							max = origin.count();
						count_t old_elements = elements;
						resizePreserveContent(elements+max);
						Strategy::copyElements(origin.pointer(),data+old_elements,max);
					}

				template <class T>
					inline void appendCopy(const T*copy_data, count_t copy_length)
					{
						count_t old_elements = elements;
						resizePreserveContent(elements+copy_length);
						Strategy::copyElements(copy_data,data+old_elements,copy_length);
					}
			

					inline void insertImport(index_t offset, Array<C,Strategy>&origin, count_t max=Undefined)	//! Inserts some or all elements of \b origin before the specified offset (0=first element). Elements are moved depending on the applied strategy @param offset Offset to insert in front of (0= first element) \param origin Array to copy from \param max Maximum number of elements to append
					{
						if (offset >= elements)
							appendImport(origin,max);
						else
						{
							if (max> origin.elements)
								max = origin.elements;
							if (!max)
								return;
							C	*new_field = alloc<C>(elements+max),
								*at = new_field;

							Strategy::moveElements(data,		new_field,			offset);
							Strategy::moveElements(origin.data,	new_field+offset,	max);

							Strategy::moveRange(data+offset,	data+elements,	new_field+offset+max);

							/*
							for (count_t i = 0; i < offset; i++)
								Strategy::move(data[i],*at++);
							for (count_t i = 0; i < max; i++)
								Strategy::move(origin.data[i],*at++);
							for (count_t i = offset; i < elements; i++)
								Strategy::move(data[i],*at++);*/
							elements += max;
							//ASSERT_EQUAL__(at,new_field+elements);
							dealloc(data);
							data = new_field;
						}
					}

				template <class T, class OtherStrategy>
					inline void insertCopy(index_t offset, const Array<T,OtherStrategy>&origin, count_t max=Undefined)	//! Inserts some or all elements of \b origin before the specified offset (0=first element). Elements are duplicated using the = operator @param offset Offset to insert in front of (0= first element) \param origin Array to copy from \param max Maximum number of elements to append
					{
						if (offset >= elements)
							appendCopy(origin,max);
						else
						{
							if (max > origin.count())
								max = origin.count();
							if (!max)
								return;
							C	*new_field = alloc<C>(elements+max);

							HybridStrategy<Strategy,OtherStrategy>::moveElements(data,		new_field,		offset);
							HybridStrategy<Strategy,OtherStrategy>::copyElements(origin.pointer(), new_field+offset,	max);
							HybridStrategy<Strategy,OtherStrategy>::moveRange(data+offset,	data+elements,	new_field+offset+max);

							elements += max;
							//ASSERT_EQUAL__(at,new_field+elements);
							dealloc(data);
							data = new_field;
						}
					}
			
					inline void	erase(index_t index, count_t count=1)	//! Erases a number of elements from the array. The remaining elements (if any) are moved depending on the used strategy. \param index Index of the first element to erase with 0 being the first entry \param count Number of elements to erase
					{
						if (index >= elements)
							return;
						if (count > elements-index)
							count = elements-index;
						if (!count)
							return;

						C*end = data+elements;

						elements -= count;
						C*new_field = alloc<C>(elements);
						Strategy::moveElements(data, new_field, index);
						Strategy::moveRange(data+index+count, end, new_field+index);

						dealloc(data);
						data = new_field;
					}
					inline iterator	erase(iterator position)
					{
						index_t index = position - Super::begin();
						erase(index);
						return Super::begin() + index;
					}
					inline iterator	erase(iterator first, iterator end)
					{
						index_t index = first - Super::begin();
						erase(index, end - first);
						return Super::begin() + index;
					}
				template <class T>
					inline bool	 FindAndErase (const T &element)
					{
						index_t index = Super::findFirst (element);
						if (index)
						{
							erase(index-1);
							return true;
						}
						return false;		
					}
				
					inline void revert()	//!< Reverses the order of elements in the local field
					{
						Strategy::revert(data,data+elements);
					}
			
		
				
					inline	void	resizePreserveContent(count_t new_size) //! Resizes the array. The new array's content is filled with the elements of the old array as far as possible. Element movement is defined by the used strategy \param new_size New array size in elements (may be 0)
					{
						if (new_size == elements || new_size == Undefined)
							return;
						C*	new_field = alloc<C>(new_size);
						count_t copy = new_size<elements?new_size:elements;
						Strategy::moveElements(data,new_field,copy);
						dealloc(data);
						data = new_field;
						elements = new_size;
					}
				

	};


template <>
	inline Array<char>::Array(const char*field):Data((count_t)strlen(field)+1)
	{
		alloc(data,elements);
		strncpy(data,field,elements);
	}

template <>
	inline Array<wchar_t>::Array(const wchar_t*field):Data((count_t)wcslen(field)+1)
	{
		alloc(data,elements);
		wcsncpy(data,field,elements);
	}
			
			
DECLARE_T2__(Array,class,class,SwapStrategy);


template <typename T>
	inline index_t	operator-(const T*ptr, const ArrayData<T>&field)
	{
		return static_cast<index_t>(ptr - field.pointer());
	}

/**
	\brief two dimensional array
	
	The Array2D class maps 2d access to a single-dimensional field. Internally it behaves like a normal Array instance, but it provides some 2d access helper method.
*/
template <class C, class Strategy=typename Strategy::StrategySelector<C>::Default>
	class Array2D:public Array<C,Strategy>
	{
	public:
		typedef Array2D<C,Strategy>	Self;
		typedef Array<C,Strategy>	Super;
	protected:
			
		Arrays::count_t	w;
			
		using Super::setSize;
		using Super::resizePreserveContent;
		using Super::erase;
		using Super::append;
	public:
		Array2D():w(0)
		{}
		Array2D(Arrays::count_t width, Arrays::count_t height):Super(width*height),w(width)
		{}

		template <class T, class OtherStrategy>
			Array2D(const Array2D<T,OtherStrategy>&other):Super(other),w(other.w)
			{}
				
		//inline Array2D(const Self&other):Super(other),w(other.w)
		//{};

		//#if __ARRAY_RVALUE_REFERENCES__
		//	Array2D(Self&&other):Super(other),w(other.w)
		//	{}
		//#endif
				
				
			
		inline	Arrays::count_t	width()	const	//! Retrieves this array's width \return width
		{
			return w;
		}
		inline	Arrays::count_t	height() const	//! Retrieves this array's height \return height
		{
			return w?Array<C,Strategy>::elements/w:0;
		}
		inline	Arrays::count_t	GetWidth()	const	//! Retrieves this array's width \return width
		{
			return w;
		}
		inline	Arrays::count_t	GetHeight() const	//! Retrieves this array's height \return height
		{
			return w?Array<C,Strategy>::elements/w:0;
		}

		void		CopyRowTo(index_t row, ArrayData<C>&target) const
		{
			#ifdef __ARRAY_DBG_RANGE_CHECK__
				if (row >= GetHeight())
					FATAL__("Index out of bounds");
			#endif
			target.SetSize(w);
			Strategy::copyElements(Super::pointer()+row*w,target.pointer(),w);
		}

		void		CopyColumnTo(index_t col, ArrayData<C>&target) const
		{
			#ifdef __ARRAY_DBG_RANGE_CHECK__
				if (col >= w)
					FATAL__("Index out of bounds");
			#endif
			count_t h = GetHeight();
			target.SetSize(h);
			for (index_t i= 0; i < h; i++)
			{
				target[i] = Super::data[i*w+col];
			}
		}

		void		CopyRowFrom(index_t row, const ArrayData<C>&source)
		{
			#ifdef __ARRAY_DBG_RANGE_CHECK__
				if (row >= GetHeight())
					FATAL__("Index out of bounds");
				ASSERT__(source.GetLength() == w);
			#endif
			Strategy::copyElements(source.pointer(),Super::pointer()+row*w,w);
		}

		void		CopyColumnFrom(index_t col, const ArrayData<C>&source)
		{
			count_t h = GetHeight();
			#ifdef __ARRAY_DBG_RANGE_CHECK__
				if (col >= w)
					FATAL__("Index out of bounds");
				ASSERT__(source.GetLength() == h);
			#endif
			for (index_t i= 0; i < h; i++)
			{
				Super::data[i*w+col] = source[i];
			}
		}

		void		SetSize(Arrays::count_t width, Arrays::count_t height)	//! Resizes the local 2d array to match the specified dimensions. The local array content is lost if the array's total size is changed
		{
			Super::SetSize(width*height);
			w = width;
		}
		void		Set(Arrays::count_t x, Arrays::count_t y, const C&value)	//! Updates a singular element at the specified position	\param x X coordinate. Must be less than width() \param y Y coordinate. Must be less than height() @param value Value to set \return Reference to the requested element
		{
			Super::data[y*w+x] = value;
		}
		
		C&			Get(Arrays::count_t x, Arrays::count_t y)	//! Retrieves a singular element at the specified position	\param x X coordinate. Must be less than width() \param y Y coordinate. Must be less than height() \return Reference to the requested element
		{
			#ifdef __ARRAY_DBG_RANGE_CHECK__
				if (x >= w || y >= height())
					FATAL__("Index out of bounds");
			#endif
			return Super::data[y*w+x];
		}
	
		const C&	Get(Arrays::count_t x, Arrays::count_t y)	const	//! Retrieves a singular element at the specified position \param x X coordinate. Must be less than width() \param y Y coordinate. Must be less than height() \return Reference to the requested element
		{
			#ifdef __ARRAY_DBG_RANGE_CHECK__
				if (x >= w || y >= height())
					FATAL__("Index out of bounds");
			#endif
			return Super::data[y*w+x];
		}
		void	adoptData(Array2D<C,Strategy>&other)	//! Adopts pointer and size and sets both NULL of the specified origin array.
		{
			w = other.w;
			Super::adoptData(other);
			other.w = 0;
		}
		void	swap(Array2D<C,Strategy>&other)
		{
			Super::swap(other);
			swp(w,other.w);
		}

	};


DECLARE_T2__(Array2D,class,class,SwapStrategy);





/*!
	\brief Array dealloc function to prevent accidental access violations.
*/
template <class C>
	inline void dealloc(ArrayData<C>&array)
	{
		array.free();
	}

/*!
	\brief Array re_alloc function to prevent accidental access violations.
*/
template <class C>
	inline void re_alloc(ArrayData<C>&array, Arrays::count_t elements)
	{
		array.setSize(elements);
	}

/*!
	\brief Array reloc function to prevent accidental access violations.
*/
template <class FieldType, class IndexType0, class IndexType1>
	inline void   reloc(ArrayData<FieldType>&array, IndexType0&length, IndexType1 new_length)
	{
		array.setSize(new_length);
		length = array.length();
	}

/*!
	\brief Array alloc function to prevent accidental access violations.
*/
template <class C>
	inline void alloc(ArrayData<C>&target, Arrays::count_t elements)
	{
		target.setSize(elements);
	}

			
typedef Array<char>	CAString;	//!< @deprecated 







#endif
