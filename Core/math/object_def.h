#ifndef object_defH
#define object_defH


/******************************************************************

Collection of standard object-definition-classes
applicable to the Mesh-class off math/object.h

******************************************************************/

namespace DeltaWorks
{

	template <class C> struct TDef
	{
		typedef C                       Type;

		struct Edge
		{};
		struct Vertex
		{};
		struct Triangle
		{};
		struct Quad
		{};
		struct Mesh
		{};
	};

	template <class C> struct TPhDef
	{
		typedef C                       Type;

		struct Edge
		{
				M::TVec3<C>				normal;
				C						grip;
		};

		struct Vertex
		{
				M::TVec3<C>				normal,p0,p1;
				C						grip;
		};

		struct Triangle
		{
				M::TVec3<C>				normal,center,gcenter;
				C						grip,updrift;
				__int64                 name;
				UINT32                  group;
		};
	
		typedef Triangle	Quad;
    
		struct Mesh
		{};
	};

	/*
	template <class C> struct TPhFileDef
	{
		typedef C                       Type;
		typedef C                       FTYPE; // ALIGN1

		struct Edge
		{} ALIGN1;

		struct Vertex
		{} ALIGN1;

		struct Triangle
		{
				FTYPE                   grip,updrift;       //8 bytes
				FINT64                  name;               //8 bytes
				FUINT32                 group,reserved;     //8 bytes
		} ALIGN1;
	
		typedef Triangle	Quad;
    
		struct Mesh
		{} ALIGN1;
	} ALIGN1;
	*/


	template <class C> struct TVsDef
	{
		typedef C                       Type;

		struct Edge
		{
				M::TVec3<C>				normal;
				M::TVec3<C>				orientation;
		};

		struct Vertex
		{
				M::TVec3<C>				normal;
		};

		struct Triangle
		{
				M::TVec3<C>				normal;
		};
	
		struct Quad
		{
				M::TVec3<C>				normal;	//!< normal[0] covers v0,v1, and v2; normal[1] covers v0,v2, and v3
		};
    
		struct Mesh
		{};
	};


}

#endif
