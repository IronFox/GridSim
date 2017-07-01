#ifndef engine_texture_resourceTplH
#define engine_texture_resourceTplH

/******************************************************************

Generalized texture-archive. The archive extracts
images from the archive and directly loads their content
to the renderer.

******************************************************************/

namespace Engine
{

	template <class GL>TextureResource<GL>::TextureResource():last_content_type(PixelType::Color),was_cube(false)
	{}

	template <class GL> bool TextureResource<GL>::isCube()
	{
	    return selected->faces == 6;
	}

	template <class GL>const typename GL::Texture* TextureResource<GL>::getTexture(const String&name)
	{
	    return TextureArchive::select(name)?getTexture():NULL;
	}

	template <class GL>const typename GL::Texture* TextureResource<GL>::getTexture64(const tName&name)
	{
	    was_cube = false;
	    return TextureArchive::select64(name)?getTexture():NULL;
	}

	template <class GL>
		const typename GL::Texture* TextureResource<GL>::getTexture()
		{
			STAentry*entry = TextureArchive::getSelected();
			was_cube = entry->faces == 6;
			last_content_type = entry->face[0].content_type;
			if (entry->custom_attachment)
				return (typename GL::Texture*)entry->custom_attachment;
			
			typename GL::Texture*target;
			entry->custom_attachment = target = SignalNew(new typename GL::Texture());
			
			if (was_cube)
			{
				const Image*images[6];
				if (!TextureArchive::getAll(images))   //gl equalizes if necessary
					return NULL;
				target->loadCube(*images[0],*images[1],*images[2],*images[3],*images[4],*images[5]);
			}
			else
			{
				const Image*image = TextureArchive::GetData();
				if (!image)
					return NULL;
				target->load(*image);
			}
			return target;
		}

	template <class GL>inline bool TextureResource<GL>::wasCube()	const
	{
	    return was_cube;
	}

	template <class GL>
		inline	PixelType		TextureResource<GL>::lastContentType() const
		{
			return last_content_type;
		}



}

#endif
