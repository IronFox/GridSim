#ifndef cuda_host_arrayH
#define cuda_host_arrayH

#include "meta_object.h"
#include "../io/file_system.h"
#include "../container/hashtable.h"
#include "../container/buffer.h"


namespace CUDA
{
	extern CMetaChannel	host_channel;


	class CHostObject:public CMetaObject
	{
	public:
						CHostObject():CMetaObject(&host_channel)
						{}
			
			void*		allocate(size_t size)
						{
							if (!size)
								return NULL;
							//cout << "allocating "<<size<<" bytes. current load is "<<host_channel.total_allocated<<endl;
							host_channel.allocate(size);
							reregister(false);
							void*result = malloc(size);
							while (!result)
							{
								host_channel.freeMemory();
								result = malloc(size);
							}
							//cout << "allocated "<<size<<" bytes. current load is "<<host_channel.total_allocated<<endl;
							return result;
						}
			
			void		free(void*pointer, size_t size)
						{
							if (!pointer)
								return;
							host_channel.delocate(size);
							::free(pointer);
						}
	};

	class CSwapSpace
	{
	protected:
			FILE				*file;
			FileSystem::CTempFile			filename;
			unsigned			total_blocks;
			CBuffer<unsigned>	free_blocks;
			
	static	CIndexContainer<CSwapSpace>	map;
	public:
			UINT32				block_size;
			
								CSwapSpace():file(NULL),total_blocks(0),block_size(0)
								{}
			virtual				~CSwapSpace()
								{
									if (file)
									{
										//cout << "closing down swap space ("<<file<<") of block size "<<block_size<<". bye"<<endl;
										//rewind(file);
										fclose(file);
										file = NULL;
										//cout << "closed"<<endl;
									}
								}
	static	CSwapSpace*			find(UINT32 block_size)
								{
									//cout << "attempting to find swap space with block size "<<block_size<<endl;
									CSwapSpace*result = map.define(block_size);
									if (!result->file)
									{
										//cout << " swap space created"<<endl;
										result->block_size = block_size;
										CString filename = result->filename.getLocation();
										//CString filename = "swap"+CString(block_size)+".tmp";
										result->file = fopen(filename.c_str(),"w+b");
										ASSERT_NOT_NULL__(result->file);
										//cout << " file '"<<filename<<"' ready"<<endl;
									}
									//else
										//cout << " reusing existing swap space"<<endl;
									return result;
								}
			long				allocate()
								{
									if (free_blocks.count())
									{
										unsigned index = free_blocks.pop();
										//cout << "reusing previously allocated block "<<index<<endl;
										return (long)block_size*index;
									}
									unsigned index = total_blocks++;
									//cout << "appended new block "<<index<<endl;
									return (long)block_size*index;
								}
			void				free(long address)
								{
									if (!file)
										return;
									unsigned index = (unsigned)(address/block_size);
									free_blocks << index;
									//cout << "block "<<index<<" freed"<<endl;
								}
			void				write(long address, const void*data)
								{
									//cout << "seeking file address "<<address<<" for write operation"<<endl;
									ASSERT2__(!fseek(file,address,SEEK_SET),file,address);
									//cout << "writing "<<block_size<<" bytes to disk ("<<file<<")"<<endl;
									//cout << "file pointer is "<<ftell(file)<<endl;
									size_t written = fwrite(data,1,block_size,file);
									ASSERT_EQUAL__((unsigned)written,block_size);
									//cout << "file pointer is "<<ftell(file)<<endl;
									//cout << "transaction completed"<<endl;
								}
			void				read(long address, void*data)
								{
									//cout << "seeking file address "<<address<<" for read operation"<<endl;
									ASSERT2__(!fseek(file,address,SEEK_SET),file,address);
									//cout << "ftell="<<ftell(file)<<endl;
									//BYTE*target = (BYTE*)data,
									//	*end = target+block_size;
									
									//cout << "reading "<<block_size<<" bytes from disk ("<<file<<")"<<endl;
									size_t read = fread(data,1,block_size,file);
									ASSERT_EQUAL__((unsigned)read,block_size);
									//cout << "ftell="<<ftell(file)<<endl;
									//cout << "read="<<read<<endl;
									//ASSERT3__(read==block_size,read,block_size,ferror(file));
									//cout << "transaction completed"<<endl;
								}
	
	};

	class CSwapAddress
	{
	protected:
			CSwapSpace			*swap_space;
			long				file_address;
			
								CSwapAddress(const CSwapAddress&other)
								{}
			CSwapAddress&		operator=(const CSwapAddress&other)
								{
									return *this;
								}
	public:
								CSwapAddress():swap_space(NULL),file_address(0)
								{}
	virtual						~CSwapAddress()
								{
									if (swap_space)
										swap_space->free(file_address);
								}
			void				adoptContent(CSwapAddress&other)
								{
									free();
									swap_space = other.swap_space;
									file_address = other.file_address;
									other.swap_space = NULL;
									other.file_address = 0;
								}
			void				free()
								{
									if (swap_space)
										swap_space->free(file_address);
									swap_space = NULL;
								}
			void				load(const void*data, size_t size)
								{
									if (swap_space && swap_space->block_size != size)
									{
										//cout << "swap space is inappropriate for data of size "<<size<<endl;
										swap_space->free(file_address);
										swap_space = NULL;
									}
									if (!swap_space)
									{
										//cout << "locating new swap space for block size "<<size<<endl;
										swap_space = CSwapSpace::find((unsigned)size);
										file_address = swap_space->allocate();
									}
									swap_space->write(file_address,data);
								}
			void				restore(void*data, size_t size)	const
								{
									if (!swap_space)
										return;
									ASSERT_EQUAL__(swap_space->block_size,(unsigned)size);
									swap_space->read(file_address,data);
								}
			operator bool()	const
			{
				return swap_space != NULL;
			}
	
	};

	template <typename T>
		class CHostArray:public CHostObject
		{
		protected:
				CSwapAddress	swapped;
				T*				data;
				UINT32			elements;
				
								CHostArray(const CHostArray<T>&other)
								{}
				CHostArray<T>&	operator=(const CHostArray<T>&other)
								{
									return *this;
								}
		public:
								CHostArray():data(NULL),elements(0)
								{}
		virtual					~CHostArray()
								{
									CHostObject::free(data,elements*sizeof(T));
									swapped.free();
								}
				void			adoptContent(CHostArray<T>&other)
								{
									free();
									swapped.adoptContent(other.swapped);
									data = other.data;
									elements = other.elements;
									other.data = NULL;
									other.elements = 0;
								}
				void			resize(UINT32 new_size)
								{
									if (new_size == elements)
										return;
									swapped.free();
									CHostObject::free(data,elements*sizeof(T));
									data = (T*)allocate(sizeof(T)*new_size);
									elements = new_size;
								}
				void			resizeCopy(UINT32 new_size)
								{
									if (new_size == elements)
										return;
									restore();
									swapped.free();
									T*new_field = (T*)allocate(new_size*sizeof(T));
									for (unsigned i = 0; i < elements && i < new_size; i++)
										new_field[i] = data[i];
									CHostObject::free(data,elements*sizeof(T));
									data = new_field;
									elements = new_size;
								}
			template <typename T0>
				void			fill(const T0&element,UINT32 offset,UINT32 max)
								{
									if (offset >= elements)
										offset = elements-1;
									if (max > elements)
										max = elements;
									restore();
									swapped.free();
									for (unsigned i = offset; i < max; i++)
										data[i] = element;
								}
				void			free()
								{
									swapped.free();
									CHostObject::free(data,elements*sizeof(T));
									data = NULL;
									elements = 0;
								}
				T*				pointer()
								{
									if (data || !elements)
										return data;
									restore();
									reregister(true);
									return data;
								}
				size_t			contentSize()	const
								{
									return elements*sizeof(T);
								}
				UINT32			length()	const
								{
									return elements;
								}
				UINT32			count()	const
								{
									return elements;
								}
				void			signalChanged()
								{
									swapped.free();
								}
				void			suspend()
								{
									if (!data || swapped)
										return;
									//cout << "suspending data to disk"<<endl;
									swapped.load(data,elements*sizeof(T));
									CHostObject::free(data,elements*sizeof(T));
									data = NULL;
								}
				void			restore()
								{
									if (data || !elements)
										return;
									//cout << "restoring data from disk"<<endl;
									data = (T*)allocate(elements*sizeof(T));
									swapped.restore(data,elements*sizeof(T));
									host_channel.restored++;
									host_channel.shifted_bytes += sizeof(T)*elements;
								}
				void			copyFrom(const T*other, unsigned count)
								{
									if (count > elements)
										count = elements;
									restore();
									swapped.free();
									memcpy(data,other,count*sizeof(T));
								}
				void			readFrom(const CHostArray<T>&other, unsigned count)
								{
									if (count > elements)
										count = elements;
									if (!other.elements)
										return;
									restore();
									swapped.free();
									if (other.data)
										memcpy(data,other.data,count*sizeof(T));
									else
										other.swapped.restore(data,count*sizeof(T));
								}
				T&				operator[](unsigned index)
								{
									restore();
									return data[index];
								}
		};




}


#endif
