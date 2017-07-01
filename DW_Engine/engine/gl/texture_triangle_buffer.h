#ifndef gl_texture_triangle_bufferH
#define gl_texture_triangle_bufferH

#include "../../container/buffer.h"
#include "../../container/queue.h"
#include "../../gl/gl.h"
//#include "../renderer/opengl.h"

namespace Engine
{
	/**
		@brief Texture triangle allocation helper
		
		This unit manages allocations of triangular halves of quadratic textures. Each buffer applies the same dimension to all of its textures and initializes them automatically.
		 
	*/
	namespace TextureTriangle
	{
		class Buffer;
		
		//#define TT_DEBUG_MODE
		
		/**
			@brief Singular texture triangle allocation
			
			The content of the struct points to one half of an allocated triangle. Each two allocations may point to the same texture, yet different halves.
		*/
		struct TAllocation
		{
				index_t		texture_index;	//!< Index of the allocated texture or 0 if the allocation is empty
				bool		first_half;		//!< True if this allocation points to the first half of the specified texture, false otherwise. Invalid if @b texture_index is 0.
		};
		
		/**
			@brief Self managing allocation
			
			Instances of Allocation initialize to empty and automatically free the allocated texture half (if any). These instances cannot be copied and protect their data from invalid access.
		*/
		class Allocation:protected TAllocation
		{
		protected:
				friend class Buffer;
				Buffer*	parent;			//!< Link to the buffer that filled this allocation or NULL if no allocation has been performed
				
							Allocation(const Allocation&other)
							{}
		public:
							Allocation():parent(NULL)	{texture_index = 0;}
		virtual				~Allocation()		{free();}
				GLuint		handle()	const;									//!< Retrieves the texture handle of this allocation from the parent buffer. @return Texture handle or 0 if the local allocation is empty
		inline	bool		firstHalf() const	{return first_half;}			//! Queries whether this allocation is the first half of the allocated texture @return True if this is the first half of the allocated texture, false otherwise.
				void		free();												//!< Frees this texture allocation. The method has no effect if no texture half has been allocated
		inline	bool		IsEmpty()	const	{return !texture_index;}		//! Queries whether or not a texture half has been allocated @return True if @b no texture half has been allocated, false otherwise
		};

	
		/**
			@brief Texture triangle container
		*/
		class Buffer
		{
		protected:
				friend class Allocation;
				::Buffer<GLuint>	textures;			//!< Allocated textures
				::Buffer<bool>		allocated;			//!< Map to indicate whether or not a texture half has been allocated. This field contains two boolean values per texture
				Queue<TAllocation>	free_allocations;	//!< Queue of available allocations
				unsigned			texture_dimension;	//!< Dimension of textures applied to both width and height
				GLenum				texture_type;				//!< OpenGL texture type applied to all textures
				
				
				void				create(GLuint&handle);	//!< Initializes the specified texture handle to a new texture
				
		public:
									Buffer(unsigned texture_dimension_=256, GLenum type_=GL_RGB):texture_dimension(texture_dimension_),texture_type(type_)
									{}
		virtual						~Buffer()												{clear();}
				void				setTextureDimension(unsigned texture_dimension);	//!< Sets the texture dimension. The method fails if textures are currently allocated
				void				setType(GLenum type);								//!< Sets the texture type. The method fails if texture are currently allocated
		inline	unsigned			textureDimension() const								{return texture_dimension;}	//! Queries the currently used texture dimension
		inline	GLenum				textureType() const										{return texture_type;}				//! Queries the currently used texture type
				void				clear();											//!< Frees all currently allocated textures
		inline	bool				IsEmpty() const											{return textures.count()!=0;}//! Queries whether or not textures are currently allocated
				TAllocation			allocate();											//!< Allocates a new texture half
				void				allocate(Allocation&allocation);					//!< @overload
				void				free(TAllocation&allocation);						//!< Frees a texture half. The free a Allocation instance call Allocation::free()
		inline	GLuint				getTextureHandle(const TAllocation&allocation) const 	{return allocation.texture_index?textures[allocation.texture_index-1]:0;}	//! Retrieves a texture handle
		};
	}
}






#endif
