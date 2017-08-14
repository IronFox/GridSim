#ifndef texture_bufferH
#define texture_bufferH

/******************************************************************

Texture-buffer allowing to buffer/write simple texture-collections.

******************************************************************/

#if 0


#include "../io/riff_handler.h"
#include "../geometry/cgs.h"
#include "../global_string.h"
#include "../container/hashtable.h"

namespace DeltaWorks
{


	/*inline	int	CompareTextures(const CGS::TextureA*t0, const CGS::TextureA*t1)
	{
		if (t0->face_field.length() < t1->face_field.length())
			return -1;
		if (t0->face_field.length() > t1->face_field.length())
			return 1;
		if (t0->data_hash < t1->data_hash)
			return -1;
		if (t0->data_hash > t1->data_hash)
			return 1;

		for (BYTE k = 0; k < t0->face_field.length(); k++)
		{
			if (t0->face_field[k].size() < t1->face_field[k].size())
				return -1;
			if (t0->face_field[k].size() > t1->face_field[k].size())
				return 1;
			char rs = fastCompare((char*)t0->face_field[k].pointer(),(char*)t1->face_field[k].pointer(), t0->face_field[k].size());
			if (rs)
				return rs;
		}
		return 0;
	}
*/


	class TextureBuffer:public Ctr::GenericHashSet<CGS::TextureA,Ctr::StdHash,Adopt>
	{
	public:
		typedef Ctr::GenericHashSet<CGS::TextureA,Ctr::StdHash,Adopt> Super;

		String			comment;

		void			Add(const CGS::TextureA&texture);
		void			Write(const PathString&filename);
		String			GetState();
	};
}

#endif /*0*/


#endif
