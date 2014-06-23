#include "../../global_root.h"
#include "texture_triangle_buffer.h"

#ifdef TT_DEBUG_MODE
    #include <iostream>
    //using namespace std;
#endif

namespace Engine
{
	/**
		@brief Texture triangle allocation helper
		
		This unit manages allocations of triangular halves of quadratic textures. Each buffer applies the same dimension to all of its textures and initializes them automatically.
		 
	*/
	namespace TextureTriangle
	{
		GLuint		Allocation::handle()	const
		{
			if (!parent || !texture_index)
				return 0;
			return parent->textures[texture_index-1];
		}
		
		void		Allocation::free()
		{
			if (!parent || !texture_index)
				return;
			parent->free(*this);
		}
		
		void		Buffer::setTextureDimension(unsigned texture_dimension)
		{
			if (!isEmpty())
				return;
			this->texture_dimension = texture_dimension;
		}
		
		void		Buffer::setType(GLenum type)
		{
			if (!isEmpty())
				return;
			this->texture_type = type;
		}
				
		void		Buffer::clear()
		{
			if (application_shutting_down)
				return;
			#ifndef TT_DEBUG_MODE
				for (unsigned i = 0; i < textures.count(); i++)
					glDeleteTextures(1,&textures[i]);
			#else
				cout << "Clear operation. Freeing all textures"<<endl;
			#endif
			textures.reset();
			allocated.reset();
			free_allocations.clear();
		}
		
		void		Buffer::create(GLuint&handle)
		{
			handle = 0;
			glGenTextures(1,&handle);
			if (!handle)
				FATAL__("OpenGL not properly initialized for texture triangle buffer allocation");
			glBindTexture( GL_TEXTURE_2D, handle);

			glTexImage2D( GL_TEXTURE_2D, 0, texture_type, 
						  texture_dimension, texture_dimension, 
						  0, texture_type, GL_UNSIGNED_BYTE, NULL );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glBindTexture( GL_TEXTURE_2D, 0);
		}
		
		TAllocation	Buffer::allocate()
		{
			TAllocation result;
			#ifdef TT_DEBUG_MODE
				cout << "Allocating texture half. Looking for free allocations"<<endl;
			#endif
			while (free_allocations.pop(result))
			{
				#ifdef TT_DEBUG_MODE
					cout << "Now looking at "<<result.texture_index<<"["<<(int)result.first_half<<"]..."<<endl;
				#endif
			
				index_t index = (result.texture_index-1);
				if (index < textures.count() && !allocated[index*2+result.first_half])
				{
					#ifdef TT_DEBUG_MODE
						cout << "Valid. Setting flag"<<endl;
					#endif
					allocated[index*2+result.first_half] = true;
					return result;
				}
				#ifdef TT_DEBUG_MODE
					else
						cout << "Index invalid or flag already set. Skipping to next"<<endl;
				#endif
			}
			#ifdef TT_DEBUG_MODE
				cout << "No valid allocation found. Creating new allocations"<<endl;
			#endif
			GLuint&handle = textures.append();
			result.texture_index = textures.count();
			allocated.append() = false;
			allocated.append() = true;
			#ifndef TT_DEBUG_MODE
				create(handle);
			#else
				cout << "Creating texture entry "<<result.texture_index<<endl;
			#endif
			result.first_half = true;
			
			TAllocation&other = free_allocations.push();
			other.texture_index = result.texture_index;
			other.first_half = false;
			return result;
		}
			
			
		void	Buffer::allocate(Allocation&allocation)
		{
			allocation.free();
			((TAllocation&)allocation) = allocate();
			allocation.parent = this;
		}
		
		void	Buffer::free(TAllocation&allocation)
		{
			#ifdef TT_DEBUG_MODE
				cout << "Attempting to free texture allocation "<<allocation.texture_index<<"["<<(int)allocation.first_half<<"]..."<<endl;
			#endif
			if (application_shutting_down)
			{
				#ifdef TT_DEBUG_MODE
					cout << "Unable to free texture allocation. Application shutting down"<<endl;
				#endif
				return;
			}
			index_t index = allocation.texture_index-1;
			#ifdef TT_DEBUG_MODE
				cout << "Effectively freeing index "<<index<<endl;
			#endif
			if (index >= textures.count() || !allocated[index*2+allocation.first_half])
			{
				#ifdef TT_DEBUG_MODE
					cout << "Index mismatch or half not allocated"<<endl;
				#endif
				allocation.texture_index = 0;
				return;
			}
			if (index+1==textures.count() && !allocated[index*2+!allocation.first_half])	//both halves of last texture now free
			{
				#ifdef TT_DEBUG_MODE
					cout << "Texture is final half of last texture. Freeing texture "<<endl;
					textures.pop();
				#else
					GLuint id = textures.pop();
					glDeleteTextures(1,&id);
				#endif
				allocated.pop();
				allocated.pop();
				while (textures.count() && !allocated.last() && !allocated[allocated.count()-2])
				{
					#ifdef TT_DEBUG_MODE
						cout << "Cascade: Freeing last texture"<<endl;
						textures.pop();
					#else
						id = textures.pop();
						glDeleteTextures(1,&id);
					#endif
					allocated.pop();
					allocated.pop();
				}
			}
			else
			{
				#ifdef TT_DEBUG_MODE
					cout << "Texture is not final half of last texture. Unmapping allocation"<<endl;
				#endif
				allocated[index*2+allocation.first_half] = false;
				free_allocations.push(allocation);
			}
			allocation.texture_index = 0;
			#ifdef TT_DEBUG_MODE
				cout << "Number of allocated textures is now "<<textures.count()<<". Number of free allocations is "<<free_allocations.count()<<endl;
			#endif
		}
	}
}
