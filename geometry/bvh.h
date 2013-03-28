#ifndef bvhH
#define bvhH


#include "../math/vector.h"
#include "../container/buffer.h"

struct NoBVHAttachment	{};

template <typename Object, typename Float = float, typename Attachment = NoBVHAttachment>
    class BVH
    {
	public:
        /// <summary>
        /// Tuple that contains both an element and its current bounding box.
        /// Primarily used internally, but may be used to more efficiently pass new entries to the hierarchy.
        /// </summary>
        struct Entry
        {
            Object*			element;
            Box<Float>		boundingBox;
            
			/**/			Entry():element(NULL)	{}
			/**/			Entry(Object*element, const Box<Float>&box) : element(element),boundingBox(box)	{}
        };

	private:
        class Node
        {
		public:
            const bool		isLeaf;
            const Box<Float>boundingBox;
			Attachment		attachment;


			/**/			Node(bool isLeaf, const Box<Float>&boundingBox) : isLeaf(isLeaf),boundingBox(boundingBox)	{}
			virtual			~Node()	{}
        };

        class LeafNode : public Node
        {
		public:
            Array<Entry>	elements;

            /**/			LeafNode(const Box<Float>&boundingBox) : Node(true, boundingBox) { }
        };

        class InnerNode : public Node
        {
		public:
            Node			*child0,
							*child1;
            int				axis;

            /**/			InnerNode(const Box<Float>&boundingBox) : Node(false, boundingBox), child0(NULL),child1(NULL),axis(0) { }
			virtual			~InnerNode()	{if (child0) delete child0; if (child1) delete child1;}
        };

        Node				*_root;
        Buffer<Entry>		_allItems;
		bool				_locked;



        void				_RebuildIfNeeded()
        {
            //lock (this)
            {
                if (!_root && _allItems.isNotEmpty())
                    ForceReconstruction();
            }
        }

	public:
		class Accessor
		{
		private:
			Node*			_GetNode()	{return reinterpret_cast<Node*>(this);}
			const Node*		_GetNode()	const {return reinterpret_cast<const Node*>(this);}
		public:
			count_t			CountObjects()	const	{const Node*n = _GetNode(); return n->isLeaf ? ((LeafNode*)n)->elements.count() : 0;}
			void			GetObjects(Object**)	const;
			bool			IsLeaf() const	{return _GetNode()->isLeaf;}
			Accessor*		GetFirstChild() const {const Node*n = _GetNode(); return n->isLeaf ? NULL : reinterpret_cast<Accessor*>(((InnerNode*)n)->child0);}
			Accessor*		GetSecondChild() const {const Node*n = _GetNode(); return n->isLeaf ? NULL : reinterpret_cast<Accessor*>(((InnerNode*)n)->child1);}
			const Box<Float>&GetBoundingBox() const {return _GetNode()->boundingBox;}
			Attachment&		GetAttachment()	{return _GetNode()->attachment;}
		};

		typedef Accessor*	iterator;


		/**/				BVH():_root(NULL),_locked(false)	{}
		/**/				~BVH()	{if (_root) delete _root;}

		void				DisallowAutomaticReconstruction()	{_locked = true;}
		void				AllowAutomaticReconstruction()	{_locked = false;_RebuildIfNeeded();}

        void				Insert(Object*element, const Box<Float>&space)
        {
			Insert(Entry(element, space));
        }
        void				Insert(const Entry&entry)
        {
            //lock(_allItems)
            {
                _allItems << entry;
				if (!_locked)
					ForceReconstruction();
				else
				{
					if (_root)
						delete _root;
					_root = NULL;
				}
            }
        }

        void				Insert(const Entry*entries, count_t numEntries)
        {
            //lock (_allItems)
            {
				_allItems.append(entries,numEntries);
				if (!_locked)
                    ForceReconstruction();
				else
				{
					if (_root)
						delete _root;
					_root = NULL;
				}
            }
        }
		void				Insert(const ArrayData<Entry>&entries)	{Insert(entries.pointer(),entries.count());}
        void				Remove(Object*element)
        {
            //lock(_allItems)
                for (index_t i = 0; i < _allItems.count(); i++)
                    if (_allItems[i].element == element)
                    {
                        _allItems.erase(i);
						if (!_locked)
                            ForceReconstruction();
						else
						{
							if (_root)
								delete _root;
							_root = NULL;
						}
                        return;
                    }
        }

	private:
        class Bucket
        {
		public:
			count_t				numElements;
            Box<Float>			boundingBox;;
            float				volumeUntilThis,
								volumeFromThis;

			/**/				Bucket()	{Reset();}

            void				Reset()
            {
                boundingBox.setAll(MaxInvalidRange<Float>());
                volumeUntilThis = 0;
                volumeFromThis = 0;
				numElements = 0;
            }

        };


        static Node*		_BuildNode(Entry*const elements, const count_t numElements, ArrayData<Bucket>&buckets)
        {
            Box<Float> boundingBox;
			boundingBox.setAll(MaxInvalidRange<Float>());

			for (index_t i = 0; i < numElements; i++)
                boundingBox.include(elements[i].boundingBox);
            if (numElements <= 4)
            {
                LeafNode*leaf = new LeafNode(boundingBox);
                leaf->elements.resizeAndCopy(elements, numElements);
                return leaf;
            }


            Box<Float> centerVolume;
			centerVolume.setAll(MaxInvalidRange<Float>());
			for (index_t i = 0; i < numElements; i++)
                centerVolume.include(elements[i].boundingBox.center());


            float minVolume = std::numeric_limits<float>::max();
            int useAxis = -1;
            index_t splitAtBucket = 0;

            for (int axis = 0; axis < 3; axis++)
            {
                const TRange<Float>&axisRange = centerVolume.axis[axis];
                Float extend = axisRange.extend();
                if (extend == 0)
                    continue;
                    //extend = 1.0f;
				for (index_t i = 0; i < numElements; i++)
                {
					const Entry&e = elements[i];
					index_t bucketIndex = std::min(buckets.count() - 1, (index_t)(float(e.boundingBox.axis[axis].center() - axisRange.min) / extend * buckets.count()));
                    Bucket&bucket = buckets[bucketIndex];
                    bucket.numElements ++;
                    bucket.boundingBox.include(e.boundingBox);
                }
				if (buckets.first().numElements == 0 || buckets.last().numElements == 0)
				{
					foreach (buckets,b)
						b->Reset();
					continue;
				}
				
                Box<Float> bounds = buckets[0].boundingBox;
                float volume;
				bounds.getVolume(volume);
                buckets[0].volumeUntilThis = volume;
                for (index_t i = 1; i < buckets.count(); i++)
                {
                    if (buckets[i].numElements > 0)
                    {
                        bounds.include(buckets[i].boundingBox);
                        bounds.getVolume(volume);
                    }
                    buckets[i].volumeUntilThis = volume;
                   // Debug.WriteLine("volumeUntil(axis " + axis + ", bucket " + i + ") = " + volume);

                }
                bounds = buckets.last().boundingBox;
                bounds.getVolume(volume);
                buckets.last().volumeFromThis = volume;
                for (index_t i = buckets.count() - 2; i < buckets.count(); i--)
                {
                    if (buckets[i].numElements > 0)
                    {
                        bounds.include(buckets[i].boundingBox);
                        bounds.getVolume(volume);
                    }
                    buckets[i].volumeFromThis = volume;
                    //Debug.WriteLine("volumeFrom(axis " + axis + ", bucket " + i + ") = " + volume);
                }

                for (index_t b = 1; b < buckets.count(); b++)
                {
                    volume = buckets[b-1].volumeUntilThis + buckets[b].volumeFromThis;
                    if (volume < minVolume)
                    {
                        minVolume = volume;
                        useAxis = axis;
                        splitAtBucket = b;
                    }
                    //Debug.WriteLine("volume(axis " + axis + ", split " + b + ") = " + volume);
                }
                foreach (buckets,b)
                    b->Reset();
            }
            if (useAxis == -1)
            {
                LeafNode*leaf = new LeafNode(boundingBox);
                leaf->elements.resizeAndCopy(elements,numElements);
                return leaf;
            }

            TRange<Float> axisRange2 = centerVolume.axis[useAxis];
            Float extend2 = axisRange2.extend();
            DBG_ASSERT__(extend2 != 0);

			//ASSERT__(numElements < 300000);


			Entry	*i0 = elements,
					*i1 = elements + numElements -1;
			do
			{
				while (i0 < i1)
				{

	                index_t bucketIndex = std::min(buckets.count() - 1, (index_t)(float(i0->boundingBox.axis[useAxis].center() - axisRange2.min) / extend2 * buckets.count()));
						//std::min(buckets.count() - 1, (index_t)((i0->boundingBox.axis[useAxis].center() - axisRange2.min) / extend2 * buckets.count()));
					if (bucketIndex >= splitAtBucket)
						break;
					i0++;
				}
				while (i0 < i1)
				{
	                index_t bucketIndex = std::min(buckets.count() - 1, (index_t)(float(i1->boundingBox.axis[useAxis].center() - axisRange2.min) / extend2 * buckets.count()));
					if (bucketIndex < splitAtBucket)
						break;
					i1--;
				}

				if (i0 < i1)
					swp(*i0,*i1);
			}
			while (i0 < i1);

            InnerNode*innerNode = new InnerNode(boundingBox);
            innerNode->axis = useAxis;
            innerNode->child0 = _BuildNode(elements, i0-elements, buckets);
            innerNode->child1 = _BuildNode(i0, elements + numElements - i0, buckets);
            return innerNode;
        }


        static void _WalkNode(Node*node, const Box<Float>&space, BasicBuffer<Object*>&found)
        {
            if (node->isLeaf)
            {
                LeafNode*leaf = (LeafNode*)node;
                if (space.intersects(leaf->boundingBox))
                    foreach (leaf->elements,e)
                        if (space.intersects(e->boundingBox))
                            found << e->element;
                return;
            }
            InnerNode*n = (InnerNode*)node;
            if (space.intersects(n->boundingBox))
            {
                _WalkNode(n->child0, space, found);
                _WalkNode(n->child1, space, found);
            }
        }
        static count_t _WalkNode(Node*node, const Box<Float>&space)
        {
            if (node->isLeaf)
            {
				count_t result = 0;
                LeafNode*leaf = (LeafNode*)node;
                if (space.intersects(leaf->boundingBox))
                    foreach (leaf->elements,e)
                        if (space.intersects(e->boundingBox))
							result ++;
                return result;
            }
            InnerNode*n = (InnerNode*)node;
            if (space.intersects(n->boundingBox))
            {
                return _WalkNode(n->child0, space) + _WalkNode(n->child1, space);
            }
			return 0;
        }
        static void _RecursivelyWalkNode(Node*node, const TVec3<Float>&point, BasicBuffer<Object*>&found)
        {
            if (node->isLeaf)
            {
                LeafNode*leaf = (LeafNode*)node;
                if (leaf->boundingBox.contains(point))
                    foreach (leaf->elements,e)
                        if (e->boundingBox.contains(point))
                            found << e->element;
                return;
            }
            InnerNode*n = (InnerNode*)node;
            if (n->boundingBox.contains(point))
            {
                _RecursivelyWalkNode(n->child0, point, found);
                _RecursivelyWalkNode(n->child1, point, found);
            }
        }

        static void _RecursivelyFindClosest(Node*node, const TVec3<Float>&point, Object*&closest, Float&distance)
        {
            if (node->isLeaf)
            {
				TVec3<Float>	c;
                LeafNode*leaf = (LeafNode*)node;
                if (leaf->boundingBox.contains(point))
                    foreach (leaf->elements,e)
                        if (e->boundingBox.contains(point))
                        {
							e->boundingBox.getCenter(c);
                            Float d = Vec::quadraticDistance(c, point);
                            if (d < distance)
                            {
                                distance = d;
                                closest = e->element;
                            }
                        }
                return;
            }
            InnerNode*n = (InnerNode*)node;
            if (n->boundingBox.contains(point))
            {
                _RecursivelyFindClosest(n->child0, point, closest, distance);
                _RecursivelyFindClosest(n->child1, point, closest, distance);
            }
        }

	public:
        void ForceReconstruction(bool verbose = false)
        {
            //lock(_allItems)
            {
				if (_root)
					delete _root;
				_root = NULL;
                if (_allItems.isEmpty())
                {
                    //if (verbose)
                      //  Debug.WriteLine("Tree is empty");
                    //_root = null;
                    return;
                }
				Array<Bucket>	buckets(16);
                _root = _BuildNode(_allItems.pointer(),_allItems.count(),buckets);
                //if (verbose)
                  //  printNode(_root,0);
            }
        }

		bool	IsEmpty() const	{return _allItems.isEmpty();}
		bool	IsNotEmpty() const {return _allItems.isNotEmpty();}

        void Lookup(const Box<Float>&space, BasicBuffer<Object*>&found)	const
        {
            Node*root = _root;
            if (!root)
                return;
            _WalkNode(root, space, found);
        }
        count_t CountHits(const Box<Float>&space)	const
        {
            Node*root = _root;
            if (!root)
                return 0;
            return _WalkNode(root, space);
        }
        void Lookup(const TVec3<Float>&position, BasicBuffer<Object*>&found)	const
        {
            Node*root = _root;
            if (!root)
                return;
            _RecursivelyWalkNode(root, position, found);
        }
        Object* LookupClosest(const TVec3<Float>&position)	const
        {
            Node*root = _root;
            if (!root)
                return NULL;
            Float distance = std::numeric_limits<Float>::max();
			Object*out;
            _RecursivelyFindClosest(root, position, out, distance);
            return distance < std::numeric_limits<Float>::max() ? out : NULL;
        }
        void Clear()
        {
            //lock (this)
            {
                _allItems.clear();
				if (_root)
					delete _root;
                _root = NULL;
            }
        }


		iterator	GetRoot()	const
		{
			return reinterpret_cast<Accessor*>(_root);
		}
	};



template <typename Object, typename Float, typename Attachment>
	void			BVH<Object,Float,Attachment>::Accessor::GetObjects(Object**rs)	const
	{
		const Node*n = _GetNode();
		if (!n->isLeaf)
			return;
		const LeafNode*l = (LeafNode*)n;
		foreach (l->elements,e)
		{
			(*rs++) = e->element;
		}
	}



#endif
