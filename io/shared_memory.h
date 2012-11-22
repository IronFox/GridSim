#ifndef shared_memoryH
#define shared_memoryH
#include "../global_root.h"

/**
	@brief Unified shared memory wrapper
*/
namespace SharedMemory
{
	/**
		@brief Information structure used to share simple debugging information
		
		Use sizeof(TDebugInfo) to create an appropriate shared memory section.
	*/
	struct TDebugInfo
	{
		char		domain[4];		//!< Domain character(s). Use some abbreviation of the respective file (maybe the first character(s) of its name). Fill unused with spaces.
		UINT32		line,			//!< Last logged line in the specified domain. Use macro __LINE__ to fill this value
					array_index0,	//!< Last referenced element index of an array (1). The specific array should be determinable by looking at the last logged line
					array_index1;	//!< Last referenced element index of an array (2). The specific array should be determinable by looking at the last logged line
	};


	/**
		@brief Mapping definition structure
	*/
	struct TMapping
	{
		void		*data;		//!< Pointer to the mapped data
		#if SYSTEM==WINDOWS
			HANDLE	handle;		//!< File handle (windows only)
		#endif
		
					TMapping();
	};

	/**
		@brief Creates a new or connects to an existing mapping
		
		@param name Global name of the mapping
		@param size Size of the shared section. This value is ignored if the specified mapping already exists
		@return New mapping definition. The @b data pointer of the result is NULL if the function failed
	*/
	TMapping		map(const char*name, size_t size);
	void			unmap(const TMapping&mapping);	//!< Unmaps an existing mapping
}





#endif
