#ifndef cuda_fractal_baseH
#define cuda_fractal_baseH

#include "../cuda_array.h"
#include "../buffer_object.h"
#include "../../engine/gl/texture.h"

#include "kernel/config.h"



	/**
		@brief Fractal vertex channels
	*/
	namespace Channel
	{
		enum eChannels
		{
			C0,
			C1,
			C2,
			#if FRACTAL_KERNEL!=MINIMAL_KERNEL
				C3,
			#endif
			#if FRACTAL_KERNEL==FULL_KERNEL
				C4,
			#endif
			Oceanic,
			Age,
			Water,
			
			Count
		};
	}


	
	struct TChannelConfig
	{
		float	c0,
				c1,
				c2,
				#if FRACTAL_KERNEL!=MINIMAL_KERNEL
					c3,
				#endif
				#if FRACTAL_KERNEL==FULL_KERNEL
					c4,
				#endif
				oceanic,
				age,
				water;
	};
	
	/**
		@brief Fractal vertex structure
	*/
	struct TVertex
	{
		TChannelConfig	channel;
		float			position[3],				//!< Sector relative vertex position. Positions are relative to the same sector across one surface.
						#if DOUBLE_NORMAL_VECTORS
							base_normal[3],				//!< Normal calculated from local vertices. Vertices of neighboring surfaces are not used
						#endif
						normal[3],					//!< Final vertex normal, merged across neighbor edges
						#ifdef RIVERS
							fractal_height,			//!< Pre-river fractal height
							#define FRACTAL_HEIGHT	fractal_height
						#else
							#define FRACTAL_HEIGHT	height
						#endif
						height;						//!< Height value relative to the planetary base height
	};
	
	
	/**
		@brief River node used to describe a singular river vertex
	*/
	struct TRiverNode
	{
		float			x,		//!< Grid x coordinate
						y,		//!< Grid y coordinate
						width,	//!< Node radius
						depth;	//!< Central node height
	};
	
	/**
		@brief Kernel ray intersection helper structure
	*/
	struct TRayIntersection
	{
		int				isset;		//!< 1 if an intersection was detected, 0 otherwise
		::TVertex		vertex[3];	//!< Corner vertices of the intersected triangle
		float			fc[3];		//!< Intersection factors (fc[0] <> vertex[0]->vertex[1]; fc[1] <> vertex[0]->vertex[2]; fc[2] <> ray)
	};
	
	/**
		@brief Kernel ground intersection helper structure
	*/
	struct TGroundInfo
	{
		int				isset;						//!< 1 if the ground was detected, 0 otherwise
		float			height_above_ground,		//!< Height of the querying point above the detected ground.
						ground_height,				//!< Height of the detected ground above the contextual base height as a factor to the contextual variance. ranges [-1,+1]
						position[3],				//!< Sector relative (interpolated) ground position
						normal[3];					//!< (interpolated) ground normal
	};
	
	
	/**
		@brief Surface cover vertex
		
		Can be used to put trees or buildings on vertices if appropriate
	*/
	struct TCoverVertex
	{
		TChannelConfig	channel;		//!< Base channel configuration of this vertex
		float			position[3],	//!< Sector relative vertex position
						up[3],			//!< Normalized up axis from this vertex
						normal[3],		//!< Normalized ground normal
						height,			//!< Height of this vertex along the context variance range [-1,+1]
						snow_line,		//!< Snow coverage height barrier
						snow,			//!< Snow density
						sand,			//!< Sand coverage
						fertility,		//!< General fertility of this vertex [0,1]. This includes all below listed values
						temperature,	//!< General temperature of this vertex based on  sphere location (cold near poles, warm near equator) [0,1].
						planarity;		//!< Planarity of this vertex [0,1]. This value is included in fertility
	};
	


	struct TBaseSurface
	{
		#undef None
		enum Flags	//! Surface segment flags
		{
			None 					= 0x0000,	//!< Clear flags
			HasChildren				= 0x0001,	//!< surface patch has children
			EdgeMergedOffset		= 0x0002,	//!< Surface edge base flag
			Edge0Merged				= 0x0002,	//!< Edge to neighbor 0 has been merged in the past and need not be merged again (simply copied). This is identical to @b EdgeMergedOffset<<0
			Edge1Merged				= 0x0004,	//!< Edge to neighbor 1 has been merged in the past and need not be merged again (simply copied). This is identical to @b EdgeMergedOffset<<1
			Edge2Merged				= 0x0008,	//!< Edge to neighbor 2 has been merged in the past and need not be merged again (simply copied). This is identical to @b EdgeMergedOffset<<2
			HasData					= 0x0010,	//!< Fractal data has been generated
			RequiresUpdate			= 0x0020,	//!< Significant changes to the structure indicate that this surface should be updated
			RequiresTextureUpdate	= 0x0040,	//!< Only textures require an update
			SubdivReflection		= 0x0080,	//!< The castReflection() method should focus
			SubdivWater				= 0x0100,	//!< The castReflection() method should focus
			Edge0Open				= 0x0200,	//!< Edge to neighbor 0 is open and can never be merged. Identical to @b Edge0Open<<0
			Edge1Open				= 0x0400,	//!< Edge to neighbor 1 is open and can never be merged. Identical to @b Edge0Open<<1
			Edge2Open				= 0x0800,	//!< Edge to neighbor 2 is open and can never be merged. Identical to @b Edge0Open<<2
			AvailableEventTriggered	= 0x1000	//!< Specifies that the onSegmentAvailable event has been triggered for this segment.
		};
	
	
		int				sector[3];		//!< Sector coordinates of this surface
		//TComposite		central_height;	//!< Absolute height of the center of this surface
		unsigned		depth;			//!< Face depth with 0 being the deepest possible face
		unsigned char	child_index;	//!< Index of this child as seen from its parent
		int				inner_seed,		//!< Seed used to generate inner vertex content
						outer_seed[3];	//!< Seed used to generate edge vertex content
		unsigned		flags;			//!< Various surface flags
	};
	
	/**
		@brief Triangular fractal surface
	*/
	struct TDeviceSurface:public TBaseSurface
	{
		::TVertex		*vertex;		//!< Device memory section pointing to the first vertex to process
		#ifdef RIVERS
			TRiverNode	*river_nodes;
			unsigned	*river_lengths,
						rivers;
		#endif
	};
	
	struct THostSurface;
	
	
	struct THostSurfaceLink
	{
		THostSurface	*surface;
		BYTE			orientation;
		bool			primary;
	
		THostSurfaceLink():surface(NULL),orientation(0),primary(true)
		{}
	};
	
	struct THostSurface:public TBaseSurface
	{
		CUDA::CDeviceArray< ::TVertex>	vertex_field;	//!< Raw device space vertex data
		CUDA::CBufferObject			vbo;				//!< Vertex buffer object containing renderable vertex floats. Each vertex provides 9 floats, the first 3 for the sector relative position, 1 for the height, 3 for the normal, the final 2 for texture coordinates
									//cloud_vbo;			//!< Vertex buffer object containing renderable cloud vertices. Each vertex provides 9 floats, the first 3 for the base position, the next 3 for the top position, the final 3 for the normal for lighting. Density can be calculated from the distance of the first two vectors, the normal (if any) is the normalized difference vector
		Engine::CGLTexture			texture;			//!< Color/normal texture assigned to this surface
		THostSurfaceLink			neighbor_link[3];	//!< Links to adjacent host surfaces
		
		#ifdef RIVERS
			CUDA::CDeviceArray<TRiverNode>	river_nodes;	//!< Device space river nodes (ordered by river)
			CUDA::CDeviceArray<unsigned>	river_lengths;	//!< Number of nodes in @a river_nodes of each river. The nodes of each river are stored in path order
		#endif		
	};
	
	
	

	/**
		@brief Calculation context
	*/
	struct TContext
	{
		//TComposite		base_height;	//!< Base radius of the sphere context
		float			base_heightf,	//!< Base radius of the sphere context as one absolute single precision float
						variance,		//!< Height factor
						sector_size,	//!< Base sector size of the environment
						noise_level,	//!< Fractal noise level
						fertility,		//!< Planet fertility level
						temperature;	//!< Planetary temperature
		unsigned		recursive_depth,//!< Total number of recursive layers
						edge_length;	//!< Number of vertices along each edge (range)
		bool			has_ocean;		//!< True if the fractal body features a oceanic water surface
	};


	/**
		@brief Global vertex definition structure
		
		This structure helps identifying the properties of a vertex. It will be expanded as needed
	*/
	struct TVertexInfo
	{
		bool			grid_vertex;	//!< True if this vertex exactly matches a parent vertex, false if it is required to be newly generated
		struct
		{
			unsigned	match,		//!< Exact matching index of this vertex in parent space. Only valid if @a grid_vertex is true
						parent0,	//!< Index of the first primary parent vertex in parent space. Only valid if @a grid_vertex is false
						parent1,	//!< Index of the second primary parent vertex in parent space. Only valid if @a grid_vertex is false
						parent2,	//!< Index of the first secondary parent vertex in parent space. Only valid if @a grid_vertex is false
						parent3;	//!< Index of the second secondary parent vertex in parent space. Only valid if @a grid_vertex is false and @a edge_index -1
		}				parent_space[4];	//!< Parent space field. Give surface orientation as parameter to determine the exact location parent space
		char			edge_index;	//!< [0-2] if this vertex borders a surface edge, -1 otherwise.
		
		unsigned		x,	//!< Child space x index (see fractal diagram for details)
						y,	//!< Child space y index (see fractal diagram for details)
						parent0,	//!< Index of the first primary parent vertex in child space. Only valid if @a grid_vertex is false
						parent1,	//!< Index of the seconds primary parent vertex in child space. Only valid if @a grid_vertex is false
						parent2,	//!< Index of the first secondary parent vertex in child space. Only valid if @a grid_vertex is false
						parent3		//!< Index of the second secondary parent vertex in child space. Only valid if @a grid_vertex is false and @a edge_index -1
						;
	};
	
#endif
