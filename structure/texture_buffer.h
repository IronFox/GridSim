#ifndef texture_bufferH
#define texture_bufferH

/******************************************************************

Texture-buffer allowing to buffer/write simple texture-collections.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/



#include "../container/lvector.h"
#include "../container/binarytree.h"
#include "../container/sortedlist.h"
#include "../io/riff_handler.h"
#include "../geometry/cgs.h"
#include "../global_string.h"


CLASS TextureSort
{
static	char	last;

static	inline	char	compareTextures(const CGS::TextureA*t0, const CGS::TextureA*t1)
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

static inline bool	greaterObject(CGS::TextureA*t0, CGS::TextureA*t1)
				{
					last = compareTextures(t0,t1);
					return last > 0;
				}
static inline bool	lesserObject(CGS::TextureA*t0, CGS::TextureA*t1)
				{
					return last < 0;
				}
static inline bool	greater(CGS::TextureA*t0, CGS::TextureA*const &t1)
				{
					last = compareTextures(t0,t1);
					return last > 0;
				}
static inline bool	less(CGS::TextureA*t0, CGS::TextureA*const &t1)
				{
					return last < 0;
				}
};


class TextureBuffer:public Named<List::Vector<CGS::TextureA> >, public Signature
{
private:
		::List::ReferenceBinaryTree<CGS::TextureA, TextureSort>	data_tree;
public:
		String			comment;

		CGS::TextureA*	lookupData(CGS::TextureA*texture);
		void			add(CGS::TextureA*texture);
		CGS::TextureA*	addDuplicate(CGS::TextureA*texture);
		bool			write(const String&filename);
		String			state();
		void			flush();
		void			clear();
};


#endif
