#ifndef cuda_buffer_objectH
#define cuda_buffer_objectH

#include "root.h"
#include <cuda_gl_interop.h>
#include "../engine/gl/extensions.h"
#include "../engine/gl/buffer_object.h"
#include "cuda_array.h"

namespace CUDA
{
	extern CMetaChannel		buffer_channel;
	
	class CBufferObject:public Engine::CGLBufferObject, public CMetaObject
	{
	public:
			bool		registered;
			
			
						CBufferObject():CMetaObject(&buffer_channel),registered(false)
						{}
						~CBufferObject()
						{
							if (object_handle && registered)
							{
								CUDA_ASSERT(cudaGLUnregisterBufferObject( object_handle ));
								buffer_channel.delocate(1);
								unlink();
							}
						}
			
			
		template <typename T>
			void		registerAndMap(T*&pointer)
						{
							ASSERT__(object_handle != 0);
							//ASSERT__(glIsBuffer(object_handle));
							buffer_channel.allocate(1);
							if (!registered)
							{
								//cout << "attempting to register "<<object_handle<<endl;
								//unsigned step = 0;
								while (cudaGLRegisterBufferObject(object_handle) != cudaSuccess)
								{
									/*if (++step > 10)
										FATAL__("CUDA corruption detected while trying to free space for vertex buffer object registration");*/
									//bool cont = device_channel.total_allocated || buffer_channel.total_allocated;
									if (!buffer_channel.total_allocated &&
										!device_channel.total_allocated)
										FATAL__("Unable to free sufficient memory for buffer registration");
										
									if (buffer_channel.total_allocated)
									{
										//cout << "mapping failed. freeing buffer memory ("<<(step)<<")"<<endl;
										buffer_channel.freeMemory();
									}
									if (device_channel.total_allocated)
									{
										//cout << "mapping failed. freeing device memory "<<endl;
										device_channel.freeMemory();
									}
									cudaGetLastError();
								}
							}
							reregister(true);
							registered = true;
							CUDA_ASSERT(cudaGLMapBufferObject( (void**)&pointer, object_handle ));
	
							
						}
			
			void		release()
						{
							CUDA_ASSERT(cudaGLUnmapBufferObject( object_handle ));
							
							CUDA_ASSERT(cudaGLUnregisterBufferObject( object_handle ));
							registered = false;
							unlink();
						}
			
			void		suspend()
						{
							CUDA_ASSERT(cudaGLUnregisterBufferObject( object_handle ));
							registered = false;
						}
						
			void		restore()
						{
						
						
						}
			size_t		contentSize()	const
						{
							return 1;
						}
			
			
			void		resize(size_t size_)
						{
							if (!object_handle)
							{
								ASSERT_NOT_NULL__(glGenBuffers);
								glGenBuffers(1,&object_handle);
								ASSERT__(object_handle != 0);
							}
							if (object_size == size_)
								return;
							
							if (registered)
							{
								//cout << "resizing registered buffer object "<<object_handle<<" to "<<size_<<endl;
								suspend();
								unlink();
							}
							/*else
								cout << "resizing unregistered buffer object "<<object_handle<<" to "<<size_<<endl;*/
							
							glGetError();
							glBindBuffer(GL_ARRAY_BUFFER_ARB,object_handle);
							glBufferData(GL_ARRAY_BUFFER_ARB,size_,NULL,GL_STATIC_DRAW_ARB);
							glBindBuffer(GL_ARRAY_BUFFER_ARB,0);
							ASSERT__(glGetError()==GL_NO_ERROR);
							object_size = size_;
						}
			
		template <typename T>
			void		load(CDeviceArray<T>&device_data)
						{
							if (!object_handle)
							{
								ASSERT_NOT_NULL__(glGenBuffers);
								glGenBuffers(1,&object_handle);
								ASSERT__(object_handle != 0);
							}
							elif (registered)
							{
								CUDA_ASSERT(cudaGLUnregisterBufferObject( object_handle ));
								buffer_channel.delocate(1);
								unlink();
							}
							glGetError();
							if (object_size != device_data.contentSize())
							{
								glBindBuffer(GL_ARRAY_BUFFER_ARB,object_handle);
								glBufferData(GL_ARRAY_BUFFER_ARB,device_data.contentSize(),NULL,GL_STATIC_DRAW_ARB);
								glBindBuffer(GL_ARRAY_BUFFER_ARB,0);
								ASSERT__(glGetError()==GL_NO_ERROR);
								object_size = device_data.contentSize();
							}
							registered = false;
							
							void*mapping;
							const void*source = device_data.devicePointer();
							CUDA_ASSERT(cudaGLRegisterBufferObject(object_handle))
							CUDA_ASSERT(cudaGLMapBufferObject(&mapping, object_handle ));
							cudaMemcpy( mapping, source, device_data.contentSize(), cudaMemcpyDeviceToHost );
							CUDA_ASSERT(cudaGLUnmapBufferObject( object_handle ));
							CUDA_ASSERT(cudaGLUnregisterBufferObject( object_handle ));
						}
						
			void		load(const void*data, size_t size_)	//!< Loads data into the local buffer object @param data Pointer to the first byte of the data to buffer @param size_ Size (in bytes) of the data to buffer
						{
							if (!object_handle)
							{
								ASSERT_NOT_NULL__(glGenBuffers);
								glGenBuffers(1,&object_handle);
								ASSERT__(object_handle != 0);
							}
							elif (registered)
							{
								CUDA_ASSERT(cudaGLUnregisterBufferObject( object_handle ));
								buffer_channel.delocate(1);
								unlink();
							}
							glGetError();
							glBindBuffer(GL_ARRAY_BUFFER_ARB,object_handle);
							glBufferData(GL_ARRAY_BUFFER_ARB,size_,data,GL_STATIC_DRAW_ARB);
							glBindBuffer(GL_ARRAY_BUFFER_ARB,0);
							ASSERT__(glGetError()==GL_NO_ERROR);
							object_size = size_;
							registered = false;
						}
			void		load(const void*data)
						{
							if (registered)
							{
								CUDA_ASSERT(cudaGLUnregisterBufferObject( object_handle ));
								buffer_channel.delocate(1);
								unlink();
							}
							glGetError();
							glBindBuffer(GL_ARRAY_BUFFER_ARB,object_handle);
							glBufferData(GL_ARRAY_BUFFER_ARB,object_size,data,GL_STATIC_DRAW_ARB);
							glBindBuffer(GL_ARRAY_BUFFER_ARB,0);
							ASSERT__(glGetError()==GL_NO_ERROR);
							registered = false;
						}
	};




}



#endif
