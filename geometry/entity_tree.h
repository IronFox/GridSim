#ifndef entity_treeH
#define entity_treeH

#include "coordinates.h"
#include "../container/buffer.h"
#include "../container/lvector.h"
#include "../math/object.h"


/*!
	\brief Recursive octree/octree-node for TreeEntity instances
	
	The EntityTree recursivly maps tree entities into the tree. Lookup is done by passing a volume (box) to the tree.
	Each EntityTree instance has one parent (or NULL) and can have up to 8 children.
*/
class EntityTree
{
public:
		typedef TBox<>	Volume;
		
		class Entity
		{
		public:
				Volume						volume;		//!< Entity volume
				const char*					type;
				//bool							added;
				
				
											Entity(const char*type_):type(type_)//,added(false)
											{}
		virtual								~Entity()
											{}
		};
		

	
protected:
		Buffer<Entity*>					entities;		//!< Entities mapped to the local tree node
		Volume								volume;			//!< Effective volume of the local tree node
		TVec3<>								split;			//!< Composite used to split the local volume into up to 8 child volumes.
		EntityTree							*child[8];		//!< Pointer to the respective children (up to 8). Any of these may be NULL.
		unsigned							level;			//!< Recursive level with 0 being the bottom-most level.
		bool								has_children;	//!< True if at least one SceneryTree::child pointer is not NULL

		void								recursiveRemap(const Buffer<Entity*>&source);		//!< Recursivly maps the elements of the specified list to the local tree node overwriting any existing mapped object entities.
		count_t								recursiveLookup(const Volume&space, Buffer<Entity*>&buffer);	//!< Volume based recursive lookup call \param space Volume to lookup \param exclude Structure entity to exclude the objects of during lookup or NULL to not exclude any object entities \param buffer Outbuffer for found object entities. The same object entity may be listed multiple times
		count_t								recursiveLookup(const TVec3<>&edge_point0, const TVec3<>&edge_point1, Buffer<Entity*>&buffer);
		count_t								recursionEnd(const TVec3<>&edge_point0, const TVec3<>&edge_point1, Buffer<Entity*>&buffer);
public:
static	bool								verbose;		//!< Print events to the console

											EntityTree(const Volume&space, unsigned level);	//!< Child node constructor \param space Volume this node should occupy. \param level Recursion level. A node on level 0 may not have child nodes.
											EntityTree(const EntityTree&other);				//!< Copy constructor
											EntityTree();										//!< Empty constructor
virtual										~EntityTree();
		void								remap(const Buffer<Entity*>&source, unsigned depth=0);				//!< Clears the local map and maps the object entities of the specified source \param source List of object entities to map @param depth Maximum tree depth. Set 0 to determine depth automatically
		void								remap(List::Vector<Entity>&source, unsigned depth=0);				//!< Clears the local map and maps the object entities of the specified source \param source List of object entities to map @param depth Maximum tree depth. Set 0 to determine depth automatically
		void								clear();											//!< Clears the local map
		
		/**
			@brief Volume based lookup call
			
			Determines all object entities that lie inside or on the border of the specified volume 
			
			\param space Volume to lookup
			\param out Outbuffer for found object entities. The out buffer may contain the same entry multiple times
		*/
		void 								lookup(const Volume&space, Buffer<Entity*>&out);
		void								lookup(const TVec3<>&edge_point0, const TVec3<>&edge_point1, Buffer<Entity*>&out);
		EntityTree&						operator=(const EntityTree&other);				//!< Copy operator
		
		EntityTree*						getChild(BYTE index);			//!< Retrieves the tree child associated with the respective index, \param index Index of the child to retrieve in the range [0,7]. \return Pointer to the respective child or NULL if the child does not exist or the specified index is greater than 7
		const EntityTree*					getChild(BYTE index)const;			//!< Retrieves the tree child associated with the respective index, \param index Index of the child to retrieve in the range [0,7]. \return Pointer to the respective child or NULL if the child does not exist or the specified index is greater than 7
		bool								hasChildren()		const;		//!< Returns true if the node has at least one child, false otherwise
		unsigned							getLevel()			const;		//!< Retrieves the recursive level of the local tree node (0 = bottom most node/leaf).
		const TVec3<>&						getSplitVector()	const;		//!< Retrieves the split point (3 components) used to determine the volumes of the local children. The returned vector is undefined if the local node has no children.
		const Volume&						getVolume()			const;		//!< Retrieves the actual volume of the local tree node
		count_t								countElements()		const;		//!< Retrieves the number of elements mapped in this node
		void								getElements(Buffer<Entity*>&out)	const;//!< Copies the pointers of all locally mapped object entities to the target list.
		const Buffer<Entity*>&				getElementList()	const;		//!< Retrieves a link to the local element list containing all mapped entities
};



/*!
	\brief Recursive composite octree/octree-node for TreeEntity instances
	
	The CompositeEntityTree recursivly maps tree entities into the tree. Lookup is done by passing a volume (box) to the tree.
	Each CompositeEntityTree instance has one parent (or NULL) and can have up to 8 children.
*/
class CompositeEntityTree
{
public:
		
		struct Volume	//! Lookup volume (box)
		{
			Composite::Coordinates			lower,
											upper;

			const Composite::Coordinates&	coordinates(BYTE index)	const {return index?upper:lower;}
			Composite::Coordinates&		coordinates(BYTE index)	{return index?upper:lower;};

			bool							containsAlongAxis(BYTE axis, double v, float sector_size)	const
			{
				return v >= lower.axisToDouble(axis,sector_size) && v <= upper.axisToDouble(axis,sector_size);
			}
		};
		
		class Entity
		{
		public:
				Volume						volume;		//!< Entity volume
				const char*					type;
				//bool							added;
				
				
											Entity(const char*type_):type(type_)//,added(false)
											{}
		virtual								~Entity()
											{}
		};
		

	
protected:
		Buffer<Entity*>					entities;		//!< Entities mapped to the local tree node
		Volume								volume;			//!< Effective volume of the local tree node
		Composite::Coordinates				split;			//!< Composite used to split the local volume into up to 8 child volumes.
		CompositeEntityTree				*child[8];		//!< Pointer to the respective children (up to 8). Any of these may be NULL.
		unsigned							level;			//!< Recursive level with 0 being the bottom-most level.
		bool								has_children;	//!< True if at least one SceneryTree::child pointer is not NULL
		float								sector_size;	//!< Sector size used during mapping

		void								recursiveRemap(const Buffer<Entity*>&source, float sector_size);		//!< Recursivly maps the elements of the specified list to the local tree node overwriting any existing mapped object entities.
		count_t								recursiveLookup(const Volume&space, Buffer<Entity*>&buffer, float sector_size);	//!< Volume based recursive lookup call \param space Volume to lookup \param exclude Structure entity to exclude the objects of during lookup or NULL to not exclude any object entities \param buffer Outbuffer for found object entities. The same object entity may be listed multiple times
		count_t								recursiveLookup(const Composite::Coordinates&edge_point0, const Composite::Coordinates&edge_point1, Buffer<Entity*>&buffer, float sector_size);

public:
static	bool								verbose;		//!< Print events to the console

											CompositeEntityTree(const Volume&space, unsigned level);	//!< Child node constructor \param space Volume this node should occupy. \param level Recursion level. A node on level 0 may not have child nodes.
											CompositeEntityTree(const CompositeEntityTree&other);				//!< Copy constructor
											CompositeEntityTree();										//!< Empty constructor
virtual										~CompositeEntityTree();
		void								remap(const Buffer<Entity*>&source, float sector_size, unsigned depth=0);				//!< Clears the local map and maps the object entities of the specified source \param source List of object entities to map @param depth Maximum tree depth. Set 0 to determine depth automatically
		void								remap(List::Vector<Entity>&source, float sector_size, unsigned depth=0);				//!< Clears the local map and maps the object entities of the specified source \param source List of object entities to map @param depth Maximum tree depth. Set 0 to determine depth automatically
		void								clear();											//!< Clears the local map
		
		/**
			@brief Volume based lookup call
			
			Determines all object entities that lie inside or on the border of the specified volume 
			
			\param space Volume to lookup
			\param out Outbuffer for found object entities. The out buffer may contain the same entry multiple times
		*/
		void 								lookup(const Volume&space, Buffer<Entity*>&out);
		void								lookup(const Composite::Coordinates&edge_point0, const Composite::Coordinates&edge_point1, Buffer<Entity*>&out);
		CompositeEntityTree&				operator=(const CompositeEntityTree&other);				//!< Copy operator
		
		CompositeEntityTree*				getChild(BYTE index);			//!< Retrieves the tree child associated with the respective index, \param index Index of the child to retrieve in the range [0,7]. \return Pointer to the respective child or NULL if the child does not exist or the specified index is greater than 7
		bool								hasChildren()		const;		//!< Returns true if the node has at least one child, false otherwise
		unsigned							getLevel()			const;		//!< Retrieves the recursive level of the local tree node (0 = bottom most node/leaf).
		const Composite::Coordinates&		getSplitVector()	const;		//!< Retrieves the split point (3 components) used to determine the volumes of the local children. The returned vector is undefined if the local node has no children.
		const Volume&						getVolume()			const;		//!< Retrieves the actual volume of the local tree node
		count_t								countElements()		const;		//!< Retrieves the number of elements mapped in this node
		void								getElements(Buffer<Entity*>&out)	const;//!< Copies the pointers of all locally mapped object entities to the target list.
		const Buffer<Entity*>&				getElementList()	const;		//!< Retrieves a link to the local element list containing all mapped entities
};




#endif
