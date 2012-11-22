#ifndef engine_texture_resourceH
#define engine_texture_resourceH

/******************************************************************

Generalized texture-archive. The archive extracts
images from the archive and directly loads their content
to the renderer. Based on archive.h.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include "../io/texture_archive.h"
#include "renderer/renderer.h"


namespace Engine
{

	/*!
		\brief Texture archive interface
		
		EveArchive requires a visual interface as template parameter (i.e. EveOpenGL).
		Provided methods allow to directly load textures from an external archive to the rendering context.
	*/
	template <class GL>
		class TextureResource:public ::TextureArchive
		{
		private:
		typedef typename GL::Texture		Texture;
				bool						was_cube;
				PixelType			last_content_type;

		public:

											TextureResource();
											/*!
												\brief Extracts the specified texture from the active folder of the currently loaded archive.
												\param name Name of the texture to retrieve from the current archive folder.
												\return Reference to respective texture object or an empty reference if the texture could not be found.
												
												getTexture() converts the specified name to a 64 bit integer and performs an exact lookup
												in the current working directory of the underlying archive. The loaded texture may be linear, planar or a cube.
												Requesting the same texture object multiple times will not result in multiple uploaded instances. Instead
												the underlying visual interface will increment its reference counter and return its hook to the previously
												loaded texture.
												The returned texture reference will be empty if lookup or upload (to the rendering context) failed.
												Use texture.isEmpty() to query the returned texture's status.
											*/
				const Texture*				getTexture(const String&name);
				const Texture*				getTexture64(const int64_t&name);	//!< Identical to the above getTexture() for 64bit integer names
				const Texture*				getTexture();						//!< Similar to getTexture() but will load the currently selected texture (via TextureArchive::select())
				bool						isCube();							//!< Queries whether or not the currently selected texture is a cube texture \return true if the currently selected texture is a cube texture, false otherwise.
		inline	bool						wasCube()			const;			//!< Queries whether or not the last loaded texture was a cube texture \return true if the previously loaded texture was a cube texture, false otherwise.
		inline	PixelType			lastContentType()	const;			//!< Queries the image type of the last loaded texture.
											/*!
												\brief Frees a texture object
												\param texture Texture reference to free
												
												Decrements the internal reference counter of the specified texture. If the reference counter hits 0, then the texture object may be unloaded.
												Whether or not non-referenced textures are actually unloaded depends on the value of the global boolean variable \b Engine::erase_unused_textures.
												By default it is set to true causing unreferenced textures to be unloaded but an application may choose to rather keep loaded textures for future
												reference.
											*/
		};
}

#include "texture_resource.tpl.h"

#endif
