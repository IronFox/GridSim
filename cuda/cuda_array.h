#ifndef cuda_arrayH
#define cuda_arrayH

#include "root.h"

#include "../global_string.h"
#include "meta_object.h"
#include "host_array.h"

namespace CUDA
{

	extern CMetaChannel	device_channel;


	/**
		@brief Persistent object that can be moved between host and device memory as needed
	*/
	class CDeviceObject:public CMetaObject
	{

	protected:
						CDeviceObject():CMetaObject(&device_channel)
						{}
	
	public:
	
	static	bool		accessible(const void*device_pointer, size_t length)
						{
							static CArray<BYTE>	buffer;
							if (buffer.contentSize() < length)
								buffer.resize(length);
							return cudaMemcpy(buffer,device_pointer,length,cudaMemcpyDeviceToHost)==cudaSuccess;
						}
	
	static	void*		allocate(size_t len)
						{
							if (!len)
								return NULL;
							device_channel.allocate(len);
							
							void*	device_pointer;								
							cudaError_t error = cudaMalloc((void**)&device_pointer,len);
							if (error != cudaSuccess)
							{
								//cout << " memory error. delocating object(s)"<<endl;
								//CMetaObject*current = first;
								do
								{
									if (error != cudaErrorMemoryAllocation)
										FATAL__("CUDA memory allocation failed with error code #"+CString(error)+" '"+CString(cudaGetErrorString(error)));
									//cout << "insufficient memory space detected. shifting..."<<endl;
									
									device_channel.freeMemory();
									//cout << "done. retrying..."<<endl;
									error = cudaMalloc((void**)&device_pointer,len);
									if (error == cudaSuccess)
									{
										//cout << "success. reseting error flag"<<endl;
										cudaGetLastError();
									}
								}
								while (error != cudaSuccess);
							}
							
							/*
							{
								if (!accessible(device_pointer,len))
								{
									cout << "memory pointer "<<device_pointer<<" not accessible. trying to reallocate ..."<<endl;
									void*rs = allocate(len);
									cudaFree(device_pointer);
									cudaGetLastError();
									return rs;
								}
							}*/
							//cout << "allocated new device field of size "<<len<<" at "<<device_pointer<<endl;
							return device_pointer;
						}
	static	void		delocate(void*pntr, size_t len)
						{
							if (!pntr)
								return;
							device_channel.delocate(len);
							CUDA_ASSERT(cudaFree(pntr));
							//cout << "delocated device field of size "<<len<<" at "<<pntr<<endl;
						}

	};



	/*!
		\brief	CUDA Device space array container
		
		The object is automatically moved to and from device memory as needed. An array container suspended to host RAM will automatically be restored when its pointer is queried for CUDA operations.
		Other operations access device or host memory as appropriate. Interactions with host memory are naturally faster as long as the object is suspended.
	*/
	template <class C>
		class CDeviceArray:public CDeviceObject
		{
			public:
						typedef uint32_t	UINT32;
			protected:
						C*				data;
						UINT32			elements;
						
						CHostArray<C>	host_storage;
						//bool			on_device;
						
						
						/**
							@brief Allocates a memory section in device memory
							
							alloc() will automatically suspend objects to host memory (starting with the least recently used) until the allocation operation succeeds
						*/
						inline static C*				alloc(UINT32 length)
						{
							return (C*)allocate(sizeof(C)*length);

						}
						
						inline static void			deloc(C*field, unsigned length)
						{
							delocate(field,sizeof(C)*length);
						}
						
						inline static void re_alloc(C*&array, unsigned prev_elements, unsigned elements)
						{
							deloc(array, prev_elements);
							array = alloc(elements);
						}
						
						
						inline static bool   reloc(C*&array, UINT32&length, UINT32 new_length)
						{
						    if (length == new_length || new_length == UNSIGNED_UNDEF)
						        return false;
							//cout << "reloc("<<array<<", "<<length<<", "<<new_length<<")\n";
						    re_alloc(array,length,new_length);
						    length = new_length;
							return true;
						}
						
			public:
						CDeviceArray():data(NULL),elements(0)	//!< Creates a new array \param length Length of the new array object
						{}
						
						CDeviceArray(const CDeviceArray<C>&other):elements(other.count())
						{
							host_storage = other.host_storage;
							if (other.data)
							{
								data = alloc(elements);
								CUDA_ASSERT(cudaMemcpy(data,other.data,sizeof(C)*elements,cudaMemcpyDeviceToDevice))
							}
							else
								data = NULL;
						}
						
			virtual		~CDeviceArray()
						{
							deloc(data,elements);
						}
						
						inline CDeviceArray<C>& operator=(const CDeviceArray<C>&other) //! Assignent operator
						{
							if (&other == this)
								return *this;
							readFrom(other);
							return *this;
						}
						
						inline CDeviceArray<C>& operator=(const CArray<C>&other) //! Assignent operator
						{
							if (Device::emulation)
							{
								host_storage.resize(other.length());
								host_storage.copyFrom(other.pointer(),other.length());
								elements = other.length();
							}
							else
							{
								if (reloc(data,elements,other.length()))
								{
									reregister(false);
									host_storage.free();
								}
								if (elements)
									if (onDevice())
										CUDA_ASSERT(cudaMemcpy(data,other.pointer(),sizeof(C)*elements,cudaMemcpyHostToDevice))
									else
									{
										host_storage.resize(other.length());
										host_storage.copyFrom(other.pointer(),other.length());
									}
							}
							return *this;
						}
						
						
						
						inline	void	free()		//! Frees the contained data and resets the local array length to 0
						{
							deloc(data,elements);
							host_storage.free();
							elements = 0;
							data = NULL;
						}
						
						inline	bool	onDevice()	const
						{
							return data != NULL;
						}
						
						inline bool		onHost()	const
						{
							return host_storage.length()>0;
						}
						
						inline bool		isHybrid()	const
						{
							return onDevice() && onHost();
						}
						
						inline bool		isEmpty()	const
						{
							return !onDevice() && !onHost();
						}

						/**
							@brief Retrieves a pointer to the contained data in device memory
							The data is shifted to device memory if needed
						*/
						inline	C*		devicePointer()
						{
							if (!onDevice())
								duplicateToDevice();
							//reregister(true);
							return data;
						}

						/**
							@brief Retrieves a pointer to the contained data in host memory
							The data is shifted to host memory if needed
						*/
						inline	C*		hostPointer()
						{
							if (!onHost())
								duplicateToHost();
							return host_storage.pointer();
						}

						inline	void	set(unsigned index, const C&element)
						{
							if (onHost())
							{
								host_storage[index] = element;
								signalHostMemoryAltered();
							}
							else
							{
								cudaError_t error = cudaMemcpy(data+index,&element,sizeof(C),cudaMemcpyHostToDevice);
								if (error != cudaSuccess)
									FATAL__("Failed to write element "+CString(index)+"/"+CString(elements)+" ("+CString(sizeof(C))+" bytes) of array 0x"+PointerToHex(data)+" with error code #"+CString(error)+" ('"+cudaGetErrorString(error)+"')");
								signalDeviceMemoryAltered();
							}
						}
						

						inline	C		operator[](unsigned index)		//!< @overload
						{
							C	element;
							if (onHost())
								return host_storage[index];
							else
							{
								cudaError_t error = cudaMemcpy(&element,data+index,sizeof(C),cudaMemcpyDeviceToHost);
								if (error != cudaSuccess)
									FATAL__("Failed to access element "+CString(index)+"/"+CString(elements)+" ("+CString(sizeof(C))+" bytes) of array 0x"+PointerToHex(data)+" with error code #"+CString(error)+" ('"+cudaGetErrorString(error)+"')");
							}
							return element;
						}
						inline	C		operator[](unsigned index) const	//!< @overload
						{
							C	element;
							if (onHost())
								return host_storage[index];
							else
							{
								cudaGetLastError();
								cudaError_t error = cudaMemcpy(&element,data+index,sizeof(C),cudaMemcpyDeviceToHost);
								if (error != cudaSuccess)
									FATAL__("Failed to access element "+CString(index)+"/"+CString(elements)+" ("+CString(sizeof(C))+" bytes) of array 0x"+PointerToHex(data)+" with error code #"+CString(error)+" ('"+cudaGetErrorString(error)+"')");
							}
							return element;
						}
					
					
						
					template <typename T>
						inline	void	fill(const T&element, UINT32 offset=0, UINT32 max=UNSIGNED_UNDEF)	//! Sets up to \b max elements starting from @b offset of the local array to \b element \param element Element to repeat @param offset First index \param max If specified: Maximum number of elements to set to \b element
						{
							if (!elements)
								return;
							if (offset >= elements)
								offset = elements-1;
							if (max > elements)
								max = elements;
							if (onDevice())
								for (register UINT32 i = offset; i < max; i++)
									CUDA_ASSERT(cudaMemcpy(data+i,&element,sizeof(C),cudaMemcpyHostToDevice))
							if (onHost())
								host_storage.fill(element,offset,max);
						}
					
						inline void	copyFrom(const C*origin, UINT32 max=UNSIGNED_UNDEF) //! Copies all elements from \b origin via the = operator \param origin Array to copy from (may be of a different entry type) \param max Maximum number of elements to read
						{
							if (max > elements)
								max = elements;
							if (onDevice())
								CUDA_ASSERT(cudaMemcpy(data,origin,sizeof(C)*max,cudaMemcpyHostToDevice))
							if (onHost())
								host_storage.copyFrom(origin,max);
						}
						
						inline	void	adoptContent(CDeviceArray<C>&other)	//! Clears any existing local data, adopts pointer and size and sets both NULL of the specified origin array.
						{
							if (this == &other)
								return;
							free();
							data = other.data;
							elements = other.elements;
							host_storage.adoptContent(other.host_storage);
							if (onDevice())
								reregister(false);
							other.data = NULL;
							other.elements = 0;
						}
					


						inline void	readFrom(const CDeviceArray<C>&origin, UINT32 max=UNSIGNED_UNDEF) //! Copies all elements from \b origin via the = operator overwriting any existing local elements \param origin Array to copy from (may be of a different entry type) \param max Maximum number of elements to read
						{
							if (max > origin.elements)
								max = origin.elements;
							resize(max,origin.onDevice());
							if (onDevice())
								if (origin.onDevice())
									CUDA_ASSERT(cudaMemcpy(data,origin.data,sizeof(C)*max,cudaMemcpyDeviceToDevice))
								else
									CUDA_ASSERT(cudaMemcpy(data,origin.host_storage.pointer(),sizeof(C)*max,cudaMemcpyHostToDevice));
							
							if (onHost())
								if (origin.onHost())
									host_storage.readFrom(origin.host_storage,max);
								else
									CUDA_ASSERT(cudaMemcpy(host_storage.pointer(),origin.data,sizeof(C)*max,cudaMemcpyDeviceToHost));
						}
						
						
						/**
							@brief Resizes the local data storage if necessary
							@param new_size New array size in elements
							@param to_device Set true if the new field should be allocated in device space rather than host space
						
						*/
						inline	void	resize(UINT32 new_size, bool to_device)
						{
							if (elements == new_size || new_size == 0xFFFFFFFF)
								return;
							if (to_device)
							{
								if (reloc(data,elements,new_size))
								{
									host_storage.free();
									reregister(false);
								}
							}
							else
							{
								deloc(data,elements);
								data = NULL;
								unlink();
								elements = new_size;
								host_storage.resize(new_size);
							}
						}
						
						/**
							@brief Resizes the local data storage and copies all of the available content
							The location (device/host) is not affected by this operation.
							If the local array is hybrid then both device and host memory sections are resized and copied.
						*/
						inline	void	resizeCopy(UINT32 new_size)
						{
							if (new_size == elements || new_size == UNSIGNED_UNDEF)
								return;
							if (onHost())
								host_storage.resizeCopy(new_size);
							if (onDevice())
							{
								C*	new_field = alloc(new_size);
								UINT32 copy = new_size<elements?new_size:elements;
								CUDA_ASSERT(cudaMemcpy(new_field,data,sizeof(C)*copy,cudaMemcpyDeviceToDevice))
								deloc(data,elements);
								data = new_field;
								elements = new_size;
								reregister(false);
							}
							
						}
						
						inline UINT32 length()	const		//! Queries the current array size in elements \return Number of elements 
						{
							return elements;
						}
						inline UINT32 count()	const		//! Queries the current array size in elements \return Number of elements 
						{
							return elements;
						}
						inline UINT32 size()	const		//! Queries the current array size in elements \return Number of elements 
						{
							return elements;
						}
						size_t contentSize()	const		//! Returns the summarized size of all contained elements in bytes \return Size of all elements
						{
							return (size_t)elements*sizeof(C);
						}
						
						/**
							@brief Downloads the local content to the specified target array
							downloadTo() priorizes the local storage if available so no actual bus transfer is performed if onHost() or isHybrid() return true.
						*/
						inline void	downloadTo(CArray<C>&target, UINT32 begin=0, UINT32 count=UNSIGNED_UNDEF)
						{
							if (begin>=elements)
								count = 0;
							elif (count > elements-begin)
								count = elements-begin;
							if (onHost())
							{
								target.resize(count);
								target.copyFrom(host_storage.pointer()+begin,count);
							}
							elif (onDevice())
							{
								target.resize(count);
								CUDA_ASSERT(cudaMemcpy(target.pointer()+begin,data,sizeof(C)*count,cudaMemcpyDeviceToHost))
							}
							else
								target.free();
						}
		
				void	duplicateToHost()
						{
							if (!onDevice() || onHost())
								return;
							host_storage.resize(elements);
							CUDA_ASSERT(cudaMemcpy(host_storage.pointer(),data,sizeof(C)*elements,cudaMemcpyDeviceToHost));
							device_channel.shifted_bytes += sizeof(C)*elements;
						}
				
				void	duplicateToDevice()
						{
							if (!onHost() || onDevice())
								return;
							data = alloc(host_storage.length());
							CUDA_ASSERT(cudaMemcpy(data,host_storage.pointer(),sizeof(C)*elements,cudaMemcpyHostToDevice));
							device_channel.shifted_bytes += sizeof(C)*elements;
							if (elements)
								reregister(false);
						}
				
				void	signalHostMemoryAltered()
						{
							if (onDevice())
							{
								deloc(data,elements);
								data = NULL;
							}
							host_storage.signalChanged();
						}
						
				void	signalDeviceMemoryAltered()
						{
							if (onHost())
								host_storage.free();
						}
						
		virtual	void	suspend()
						{
							if (!onDevice())
								return;
							//cout << "shifting "<<contentSize()<<" byte(s) to host memory"<<endl;
							if (!onHost())
							{
								host_storage.resize(elements);
								CUDA_ASSERT(cudaMemcpy(host_storage.pointer(),data,sizeof(C)*elements,cudaMemcpyDeviceToHost));
							}
							ASSERT_EQUAL__(elements,host_storage.length());
							deloc(data,elements);
							data = NULL;
						}
						
		virtual	void	restore()
						{
							if (!onHost() || onDevice())
								return;
							//cout << "shifting "<<contentSize()<<" byte(s) to device memory"<<endl;
							ASSERT_EQUAL__(elements,host_storage.length());
							data = alloc(host_storage.length());
							CUDA_ASSERT(cudaMemcpy(data,host_storage.pointer(),sizeof(C)*elements,cudaMemcpyHostToDevice));
							
							//host_storage.free();
							device_channel.restored++;
							device_channel.shifted_bytes += sizeof(C)*elements;
							if (elements)
								reregister(false);
						}
		};


}


#endif
