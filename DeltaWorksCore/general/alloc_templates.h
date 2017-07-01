#ifndef useful_templatesH
#define useful_templatesH

/******************************************************************

Collection of useful memory-allocation/-delocation functions.

******************************************************************/

namespace DeltaWorks
{

	namespace AllocTemplates //! Array helper functions
	{

	template <class C>  inline C*   alloc(size_t elements);
	template <class C>  inline void alloc(C*&target, size_t elements);
	template <class C>  inline void dealloc(C*array);
	template <class C>  inline void re_alloc(C*&array, size_t elements);
	template <class FieldType, class IndexType0, class IndexType1> inline void   reloc(FieldType*&array, IndexType0&length, IndexType1 new_length);



	/*!
		\brief Allocates a new array of the specified size and type
		\param elements Number of elements the array should be in size
		\return New array of the specified size or NULL if elements was 0.

		This function allocates a new array containing \a elements elements and returns the pointer.
		The client application is responsible for discarding the returned array.<br />
		Example:<br />
			int*field = alloc<int>(100);
	*/

	template <class C> inline C*   alloc(size_t elements)
	{
		return elements?new C[elements]:nullptr;
	}


	/*!
		\brief Allocates a new array of the specified size and stores the result in \b target
		\param elements Number of elements the array should be in size
		\param target Output pointer reference. Any previous pointer in target will be overwritten.

		This function allocates a new array containing \a elements elements and writes the pointer to \a target.
		The client application is responsible for discarding the array in \a target.<br />
		Example:<br />
			int*field;<br />
			alloc(field,100);
	*/
	template <class C> inline void alloc(C*&target, size_t elements)
	{
		target = elements?new C[elements]:nullptr;
	}

	/*!
		\brief Discards a given array
		\param array Pointer to the array that should be discarded or NULL.

		This function frees memory of the given array. \a array may also be NULL.<br />
		Example:<br />
			int*field;<br />
			alloc(field,x); //x may be 0<br />
			deloc(field);
	*/
	template <class C> inline void dealloc(C*array)
	{
		//if (array)	//as it turns out, deleting null is perfectly safe
		delete[] array;
	}

	/*!
		\brief Reallocates the specified array
		\param array Pointer to the array that should be reallocated
		\param elements Number of elements the new array should be in size

		This function frees memory of the given array and the allocates a new
		array in target.<br />
		Example:<br />
			int*field = alloc<int>(10);<br />
			re_alloc(field,20); //field now 20 elements long
	*/
	template <class C> inline void re_alloc(C*&array, size_t elements)
	{
		//if (array)	//as it turns out, deleting null is perfectly safe
		delete[] array;
		array = elements? new C[elements]:nullptr;	//this test, however, is not redundant
	}

	/*!
		\brief Potentially resizes the specified array
		\param array Pointer to the array that should be resized
		\param length Reference to a variable containing the previous size of the array
		\param new_length New length of the array

		The procedure has no effect if \a length equals \a new_length or \a new_length is \a size_t_UNDEF (size_t -1).
		If \a new_length and \a length are not identical and new_length is not \a size_t_UNDEF then
		the old array (if any) will be discarded and a new one of the specified size be created (if any).
		\a new_length will then be copied to \a length.
	*/
	template <class FieldType, class IndexType0, class IndexType1>
	inline void   reloc(FieldType*&array, IndexType0&length, IndexType1 new_length)
	{
		if (length == new_length || new_length == (IndexType1)-1)
			return;
		length = new_length;
		re_alloc(array,length);
	}

	}

	using namespace AllocTemplates;
}


#endif
