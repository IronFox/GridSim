#ifndef cuda_fractalH
#define cuda_fractalH


/******************************************************************

CUDA based fractal landscape generation module.
For the initial phase this module will depend on the availability of CUDA.

This file is part of Delta-Works
Copyright (C) 2006-2009 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/





#include "../../math/vclasses.h"
#include "../../math/object.h"
#include "../../math/object_def.h"
#include "../../general/timer.h"
#include "../../general/system.h"
#include "../../general/thread_system.h"
#include "../../general/random.h"
#include "../../container/production_pipe.h"
#include "../../container/sorter.h"
#include "../../container/buffer.h"
#include "../../io/mutex_log.h"
#include "../../io/delta_crc32.h"
#include "../../general/auto.h"
#include "../../engine/renderer/opengl.h"
#include "../../geometry/coordinates.h"

#include "../../engine/aspect.h"

#include "fractal_kernel.h"

#define FRACTAL_NURBS_CONST		0.35

//#define HYBRID_KERNEL

//MAKE_SORT_CLASS(PrioritySort,priority)
MAKE_SORT_CLASS(DepthSort,depth)


class PrioritySort
{
public:
    template <class Type>
		static inline bool greaterObject(const Type*obj0, const Type*obj1)
		{
			return (((*obj0)->priority) > ((*obj1)->priority));
		}
    template <class Type, class Ident>
		static  inline bool greater(const Type*obj, const Ident&id)
		{
			return ((*obj)->priority) > (id);
		}
    template <class Type>
		static inline bool lesserObject(const Type*obj0, const Type*obj1)
		{
			return (((*obj0)->priority) < ((*obj1)->priority));
		}
    template <class Type, class Ident>
		static  inline bool less(const Type*obj, const Ident&id)\
		{
			return ((*obj)->priority) < (id);
		}
};

class DistanceSquareSort
{
public:
    template <class Type>
		static inline bool greaterObject(const Type*obj0, const Type*obj1)
		{
			return (((*obj0)->distance_square) > ((*obj1)->distance_square));
		}
    template <class Type, class Ident>
		static  inline bool greater(const Type*obj, const Ident&id)
		{
			return ((*obj)->distance_square) > (id);
		}
    template <class Type>
		static inline bool lesserObject(const Type*obj0, const Type*obj1)
		{
			return (((*obj0)->distance_square) < ((*obj1)->distance_square));
		}
    template <class Type, class Ident>
		static  inline bool less(const Type*obj, const Ident&id)\
		{
			return ((*obj)->distance_square) < (id);
		}
};

//#define VERTEX_DEFINITION



#define NUM_VERTICES(exponent) (((1LL<<(2*(exponent))) + 3LL*(1LL<<(exponent)) +2LL)/2LL)



/* warn-options and settings for fractal modules and derivatives */


//#define WARN_BUFFER_LIMIT_EXCEEDED	{}//{ErrMessage("limit reached!");}
//#define FRAC_NO_PIPES					//erases and inserts sectors immediately bypassing pipes
//#define FRAC_DISABLE_PATTERNS			//indicate that derivative modules should not use dynamic patterns
//#define FRAC_DISABLE_TTL				//do not check for timeouts
//#define FRAC_RANDOM_STATISTICS		//show trace random-balance (use resetStatistics() and getStatistics())
//#define FRAC_USE_THREADS				//enable thread-usage
//#define FRAC_MIN_FRAME_AGE		2	//2


/*#define USE_KERNEL			1
#define TRACE_LINEAR_PAST3	1		//not use kernel from trace3...trace5 (only effective if USE_KERNEL is not 0)
#define PROJECT_ON_KERNEL	0
#define PRESERVE_PARENT_NORMALS	0	//set 1 to not regenerate parent normals on child layers*/



/*#define BEGIN
#define END
#define LBEGIN*/

#ifndef BEGIN
	#define BEGIN	//logfile<<"entering: "+CString(__func__)+"\n";
#endif
#ifndef END
	#define END		//logfile<<"exiting: "+CString(__func__)+"\n";
#endif

#ifndef LBEGIN
	#define LBEGIN	//logfile<<"begin: "+CString(__func__)+"\n";
#endif

#define FRACTAL_BUILD_STATISTIC

/**
	@brief Cuda based fractal landscape generator
*/
namespace CudaFractal
{
	extern CMutexLog		logfile;
	
	extern unsigned			num_sectors;	//!< Total number of sectors created at this time
	extern unsigned			max_sectors,	//!< Absolute number of total faces allowed at any given time. 20000 roughly equals 1.6gb of ram consumption
							build_operations;	//!< Number of build or update operations registered. Available only if FRACTAL_BUILD_STATISTIC is defined
	extern CTimer::Time		build_time;			//!< Time spent performing build or update operations
	extern ::TContext		default_context;
	
	//static const float		default_sector_size=50000.0f;
	
	
	typedef Coordinates::CSpaceCoordinates::sector_t	sector_t;

	struct TMapVertex;
	
	class CSurfaceSegment;			//!< Fractal surface composition
	class CDataSurface;		//!< Surface containing local data. Used for root level only
	
	template <unsigned>
		class CTemplateSurface;	//!< CDataSurface derivative providing structure internal data storage rather than dynamically created
	
	class CBody;	//!< Body (landscape) structure
	template <unsigned, unsigned>
		class CTemplateBody;

	class CMap;
	template <unsigned>
		class CTemplateMap;
	
	class CGeometry;
	

	::THostSurfaceLink	link(CSurfaceSegment*face, BYTE orientation);		//!< Creates a new TLink object to the specified face using the specified orientation
	void				setRadialHeight(float v[3],double height, const CSurfaceSegment*context);	//!< Radial default height setter
	double				getRadialHeight(const float v[3],const CSurfaceSegment*context);	//!< Radial default height getter
	double				getRadialHeight(const float v[3],const CSpaceCoordinates::sector_t sector[3], float sector_size);		//!< Helper function
	void				getRadialUpVector(const float p[3], float out[3], const CSurfaceSegment*context);		//!< Retrieves the normalized up vector based on the current base location and context sector. \param p Base location \param out Normalized out vector \param context Position sector context
	void				setVerticalHeight(float v[3],double height, const CSurfaceSegment*context);	//!< Vertical default height setter
	double				getVerticalHeight(const float v[3],const CSurfaceSegment*context);	//!< Vertical default height setter
	double				getVerticalHeight(const float v[3],const CSpaceCoordinates::sector_t sector[3], float sector_size);		//!< Helper function
	void				getVerticalUpVector(const float p[3], float out[3], const CSurfaceSegment*context);		//!< Retrieves the normalized up vector based on the current base location and context sector. \param p Base location \param out Normalized out vector \param context Position sector context
	
	CString				randomWord();					/*!< Creates a totally random word of length (3-32) using the characters [a-z][A-Z][0-9]-_./\*/
	void				terminate();	//!< Implodes the used kernel. This should be called before the application quits
	void				makeSphereGeometry(CGeometry&target);	//!< Adjusts the given geometry to approximate a sphere
	CGeometry			makeSphereGeometry();					//!< Creates a new sphere approximating geometry
	void				makeHaloGeometry(CGeometry&target);		//!< Adjusts the given geometry to approximate a halo shape
	CGeometry			makeHaloGeometry();						//!< Creates a new halo approximating geometry
	CString				getConfiguration();
	
	

	
	void				reportFatalException(const TCodeLocation&location, const CString&message);
	
	#define FRAC_FATAL(message)	CudaFractal::reportFatalException(CLOCATION,message);
	
	struct NoAttachment	//! Empty attachment for attachmentless structures
	{};
	

	

	struct TMapVertex:public ::TVertexInfo	//!< Map information regarding one abstract vertex
	{
			unsigned				index,			//!< Index of this vertex in the vertex field
									sibling0,		//!< Index of the first sibling vertex (if existing, otherwise 0xFFFFFFFF)
									sibling1,		//!< Index of the second sibling vertex (if existing, otherwise 0xFFFFFFFF)
									sibling2,		//!< Index of the third sibling vertex (if existing, otherwise 0xFFFFFFFF)
									sibling3,		//!< Index of the fourth sibling vertex (if existing, otherwise 0xFFFFFFFF)
									border_index;	//!< Index along the border (if along a border, 0 otherwise)
			BYTE					dir;			//!< Direction of the closest border (0xFF = NA)
			//bool					grid_vertex;	//!< True if this vertex is directly derived from the respective parent surface
	};
	

	typedef	void		(*pInit)		(::TChannelConfig&vertex_channel, CRandom&random, const CBody*context);	//!< Initializes channel data of a new top level vertex \param vertex_channel Target channel data \param random Initialized random source
	
	
	
	/**
		\brief Fractal index map
		
		The fractal index map helps keep track of the relationships between vertices on one face. Only one map should exist per fractal face size.
		The array fields are expected to be filled in from static or dynamic arrays managed by a derivative map class (i.e. CTemplateMap).
		The template class CTemplateMap<> is mapped to CMap so that non-template code may use it without knowing the template parameters.
	*/
	class CMap
	{
	protected:
			Auto<bool>				uploaded;
	public:
			struct TParentInfo	//! Abstract information about one parent-derived vertex
			{
				unsigned			index,		//!< Index in a local face vertex field
									origin[4];	//!< Index in the respective parent face vertex field (given the respective parent orientation)
			};
			
			TMapVertex				*const	vertex_info;	//!< Vertex map
			const unsigned			vertex_range,			//!< Maximum number of vertices in one direction in a face
									vertex_maximum,			//!< Highest possible vertex index in one direction (vertex_range-1)
									vertex_count,			//!< Total number of vertices in one face
									child_vertex_count,		//!< Number of newly generated vertices
									inner_vertex_count,		//!< Number of child vertices that don't border to a neighbor
									center_vertex_count,	//!< Number of inner vertices that don't border to a border
									parent_vertex_count;			//!< Number of vertices copied from the respective parent face
			unsigned				*border_index[3],			//!< Border vertex index map
									*inner_border_index[3];	//!< Inner border vertex index map
			unsigned				*const child_vertex_index,		//!< Child vertex index map
									*const inner_vertex_index,		//!< Inner vertex index map
									*const center_vertex_index;		//!< Center vertex index map
			TParentInfo				*const parent_vertex_info;		//!< Parent vertex information map
			
	
			
	protected:
									CMap(TMapVertex*const,unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned*, unsigned*,unsigned*,  unsigned*,unsigned*,unsigned*, unsigned*const, unsigned*const, unsigned*const, TParentInfo*const);
	public:

	
	inline	unsigned				getIndex(unsigned col, unsigned row)
									{
										if (row >= vertex_range || col > row)
											FRAC_FATAL("Index-Retrival-Error for ("+IntToStr(col)+", "+IntToStr(row)+") range="+IntToStr(vertex_range));
										return row*(row+1)/2+col;
									}
			void					assembleDeviceFields();
	};
	
	/**
		\brief Derivative exponential fractal index map
		
		This derivative index map retrieves the required array sizes from the specified exponent parameter.
	*/
	template <unsigned Exponent>
		class CTemplateMap:public CMap
		{
		public:
		static	const unsigned			range		= (1<<Exponent)+1,					//!< Copied to \a CMap::vertex_range
										vertices	= NUM_VERTICES(Exponent),			//!< Copied to \a CMap::vertex_count
										parent_vertices	= NUM_VERTICES(Exponent-1),		//!< Copied to \a CMap::parent_vertex_count
										child_vertices	= vertices-parent_vertices,		//!< Copied to \a CMap::child_vertex_count
										inner_vertices = child_vertices - 3*(range/2),	//!< Copied to \a CMap::inner_vertex_count
										center_vertices = inner_vertices > 3*(range-2)/2?inner_vertices - 3*(range-2)/2:1;	//!< Copied to \a CMap::center_vertex_count
		
				
		
				TMapVertex				info[vertices];					//!< Mapped to \a CMap::vertex_info
		
				unsigned				border[3][range],				//!< Mapped to \a CMap::border_index
										inner_border[3][range-1],		//!< Mapped to \a CMap::inner_border_index
										child_vertex[child_vertices],	//!< Mapped to \a CMap::child_vertex_index
										inner[inner_vertices],
										center[center_vertices];		//!< Mapped to \a CMap::center_vertex_index
										
				TParentInfo				parent_vertex[parent_vertices];	//!< Mapped to \a CMap::parent_vertex_info

				
										CTemplateMap();
		};
	
	/**
		\brief Fractal geometry
	
		The fractal geometry is used to describe the rough geometry of a fractal shape. It may consist of only a singular triangle, or a sphere, or any shape as required.
	*/
	class	CGeometry
	{
	public:
			struct Vertex	//! Geometry vertex
			{
				float				position[3];	//!< Absolute position of this vertex
			};
	
			CArray<Vertex>			vertex_field;	//!< Geometrical vertices
			CArray<unsigned>		index_field;	//!< Triangular index path. Each three indices form one triangle using the vertices stored in \b vertex_field
	
									CGeometry(unsigned vertices=0, unsigned indices=0):vertex_field(vertices),index_field(indices)
									{}
	};

	/**
		\brief Fractal seed
		
		Rotatable, expandable
	*/
	class CSeed
	{
	public:
			CArray<BYTE>			array;	//!< Binary data content of this seed
			UINT32					as_int;	//!< Auto updated int representation of the seed
			
									CSeed();
									CSeed(const CString&string);
			CString					toString()	const;	//!< Converts the seed to a printable string
			CSeed&					operator=(const CString&string);	//!< Builds the local seed from a string
			void					expand(const CSeed&seed, BYTE child_id);
			void					rotate(unsigned depth, BYTE orientation);
			void					randomize();	//!< Randomizes the local seed content
			int						toInt()	const;	//!< Explicitly queries the int representation of the local seed. Under normal circumstances the result is identical to the local @a as_int variable
	};
	
	

	/**
		@brief Fractal view description
	*/
	class CView
	{
	public:
			Engine::CAspect<>	aspect;				//!< View aspect (projection, modelview matrix, etc)
			Engine::CFrustrum<>	frustrum;			//!< Frustrum that is spanned by the above aspect
			CSpaceCoordinates	coordinates;		//!< Central view position
			bool				require_reflection;	//!< True if patch reflection should be checked for visibility
			unsigned			min_layer;			//!< Minimum visible patch layer. Increasing this above 0 effectivly truncates the highest detail layer(s)
			float				lod;				//!< Level of detail. To determine the applicable level of detail start with 1.0 and work your way up or down
			
								CView():require_reflection(true),min_layer(0),lod(1)
								{}
	};
	
	/**
		@brief Helper structure to hold all information of an intersection between a ray and a fractal shape
	*/
	struct TRayIntersection:public ::TRayIntersection
	{
			CSurfaceSegment*			surface;	//!< Affected lowest detail surface
	};
	
	struct TGroundInfo: public ::TGroundInfo
	{
			CSurfaceSegment*			surface;
	};
	
	/**
		\brief Triangular fractal root construction
		
		A shape represents a collection of fractal faces as constructed from a fractal geometry (CGeometry).
		To ensure floating point precission even with larger compositions, the shape provides a fixed sector size.
		The sector is a integer vector that each face centers on. All of its vertices are relative to this vector.
	*/
	class CBody:public ::TContext
	{
	protected:
			//CRandom						random;			//!< Global random variable to retrieve random values from

			/*CThreadPool::Family			thread_family;	//!< Family of all threads belonging to this shape
			CProductionPipe<CSurfaceSegment>	waste_pipe,		//!< Pipe used to temporarily store unlinked faces
										focus_pipe;		//!< Pipe used to temporarily store faces that require children
			CProductionPipe<CSurfaceSegment> 	project_pipe;		//!< Pipe used to temporarily store faces that have been considered visible
			CArray<CThreadObject*>		operation_field;		//!< Thread reference field used to quickly start parallel processes (passed to the kernel)*/

	static	CSurfaceSegment				*production;	//!< Currently produced surface patch (if any)
	static	CTimer::Time				trace_time;		//!< Global time when tracing began
	static	CBuffer<CSurfaceSegment*>	need_children;
			

									
			friend class CSurfaceSegment;
			
									CBody(const CString&name,float sector_size, unsigned exponent, unsigned recursion_depth, CUDA::CDeviceArray< ::TVertexInfo>*, CUDA::CDeviceArray<unsigned>*,CUDA::CDeviceArray<unsigned>*,CUDA::CDeviceArray<unsigned>*, CMap*full_map,CMap*reduced_map);
									
	public:
			CArray<CDataSurface*>	faces;			//!< Field of face pointers (one for each face). The pointers are managed and automatically erased if the root shape is erased. Top level faces are always data faces
			CBuffer<CSurfaceSegment*>	face_buffer,		//!< Buffer containing all surfaces that have been traced during the last process() operation
									reflection_buffer,	//!< Buffer containing all surface reflection that have been traced during the last processReflection() operation
									water_buffer,		//!< Buffer containing all water segments that have been traced during the last processWater() operation
									attachment_buffer;	//!< Buffer containing all surfaces with attachments that have been traced during the last process() operation. Unlike face_buffer, surfaces with attachments are not ignored when focusing
			
			const unsigned			exponent;		//!< Central map exponent
			unsigned				min_river_layer,	//!< Minimum recursive depth for new rivers to be spawned
									max_river_layer;	//!< Maximum recursive depth for new rivers to be spawned


			CSpaceCoordinates		center;	
			
			CMap								*full_map,
												*reduced_map;
			CUDA::CDeviceArray< ::TVertexInfo>	*device_info_field;
			CUDA::CDeviceArray<unsigned>		*device_vertex_index_field,
												*device_border_field,
												*device_triangles;
			
			bool					fractal_clouds;

			unsigned				least_traced_level;	//!< Most detailed layer that has been chosen for rendering
			uint64_t				range;				//!< Number of vertices in one direction as derived from the detail exponent.
			CSeed					root_seed;			//!< Seed used to generate this shape. Using the same seed and root geometry will result in the same structure.

	virtual							~CBody()
									{
										clearFaces();
									};
	
			void					clearFaces();	//!< Deletes all faces and resizes the faces array to zero.
			void					purgeSegments();	//!< Deletes all sub faces
	
	protected:
			void					reset();						//!< Resets \b face_buffer and \b least_traced_level . Automatically called by trace0()
	public:
	
			//void					clearChildren(unsigned depth);	
			
			/**
				@brief Central processing entry point
				
				process() updates the local planet's provided level of detail to the specified view. Faces that exceeded their time to live are erased.
				Invoke the static subdivide() function once done
				
				
				@param time Current time
				@param top_level_ttl Time (in seconds) that top level surfaces remain in memory even if not referenced. Each sub level receives 90% of the next upper level's ttl time interval
				@param view View to adopt to
			*/
			void					process(const CTimer::Time&time, float top_level_ttl, const CView&view);
			
			/**
				@brief Subdivides pending surface segments
				
				@param time_frame Time (in seconds) that the finalizer may spend building new surface patches. Should not exceed 1/20th of a second
			*/
	static	void					subdivide(float time_frame);
	
			/**
				@brief Alternate version of subdivide()
				
				This version starts the finalization process as a background operation while the main frame returns
				and performs other tasks. Before calling backgroundSubdivide() the first time, make sure to invoke
				backgroundInit() once, with a non-NULL pointer to an initialized Engine::OpenGL object as parameter.
				Before calling any shape's process() method it is @b integral that backgroundNext() is <b>executed exactly once</b>.
				Failing to do so will result in undesired behavior up to crashes, hangups and deadlocks.
				
				@param min_time_frame Minimum required time (in seconds) to spend processing
				@param max_time_frame Maximum allowed time (in seconds) to spend processing
			*/
	static	void					backgroundSubdivide(float min_time_frame, float max_time_frame);
			/**
				@brief Finishes background finalization and cleans up for a new sequence of process() operations
				 
				This method is required only if backgroundSubdivide() is used instead of subdivide().
				The standard subdivide() method performs operations on the calling thread and does not require any waiting or cleaning.
				Before calling any shape's process() method it is @b integral that backgroundNext() is <b>executed exactly once</b>.
				Failing to do so will result in undesired behavior up to crashes, hangups and deadlocks.
			*/
	static	CTimer::Time			backgroundNext();
			/**
				@brief Processes a single finalization step and advances the step counter.

				Automatically retrieves new faces from the production pipe if necessary.
				@return true if more steps can be performed, false otherwise
			*/
	static	bool					processStep();
			
			/**
				@brief Determines the current level of overloading on the fractal processor of this surface
				
				The overload level is determined from the summed priorities of all remaining child requesting vertex patches (CSurfaceSegment's).
				The returned value is 0 if all faces have been processed and the processor is idle
				
				 @return Current overload level of this shape
			*/
	static	float					currentOverload();
			
			/**
				@brief Performs a recursive ray casting test on the currently available data
				@param b Ray base point. Intersections before this point are ignored
				@param d Normalized ray direction
				@param intersection [out] Reference to an intersection object that should hold all available information of the detected intersection
				@param is_planet_space Set true if the specified coordinates are already in planet relative space
				@return true if an intersection was detected, false otherwise
			*/
			bool					rayCast(const CSpaceCoordinates&b, const float d[3], TRayIntersection&intersection,bool is_planet_space=false);
			bool					groundQuery(const CSpaceCoordinates&b, unsigned min_layer, TGroundInfo&ground,bool is_planet_space=false);

			
			void					checkLinkage();	//!< Verifies the integrity of the interface linkage. An exception of type CException is thrown if this test fails. This test is slow and should not be called unless bugs are expected.
/*
			CSurfaceSegment*				detectVisualPathCollision(const double p0[3],const double p1[3],const int sector[3], float&scalar_out, float point_out[3],float normal_out[3], int sector_out[3]);	//!< Determines an intersection of the specified egde and the most detailed faces with a priority >= 0. Faces that have been rendered invisible are not checked. The method returns face, point and normal. Both point and normal remain undefined if the method returns NULL.
			CSurfaceSegment*				detectVisualCollision(const double p0[3],const double p1[3],const int sector[3], float&scalar_out, float point_out[3],float normal_out[3], int sector_out[3]);	//!< Determines an intersection of the specified egde and the most detailed visible faces. The method returns face, point and normal. Both point and normal remain undefined if the method returns NULL.
			CSurfaceSegment*				detectCollision(const double p0[3],const double p1[3],const int sector[3], float&scalar_out, float point_out[3],float normal_out[3], int sector_out[3]);	//!< Determines an intersection of the specified egde and the most detailed faces. The method returns face, point and normal. Both point and normal remain undefined if the method returns NULL.
*/
	
	template <typename T>
		inline	void				getTranslation(const sector_t from_sector[3], const sector_t to_sector[3],T out_vector[3])	//!< Retrieves the translation vector between two sectors
									{
										out_vector[0] = (T)sector_size * (T)(to_sector[0]-from_sector[0]);
										out_vector[1] = (T)sector_size * (T)(to_sector[1]-from_sector[1]);
										out_vector[2] = (T)sector_size * (T)(to_sector[2]-from_sector[2]);
									}
	template <typename T0, typename T1>
		inline void					convertToAbsolute(const sector_t sector[3], const T0 vertex[3], T1 absolute_vertex[3])	const	//!< Converts the specified local vertex to its counterpart in absolute space
									{
										absolute_vertex[0] = (T1)sector_size * (T1)sector[0] + (T1)vertex[0];
										absolute_vertex[1] = (T1)sector_size * (T1)sector[1] + (T1)vertex[1];
										absolute_vertex[2] = (T1)sector_size * (T1)sector[2] + (T1)vertex[2];
									}									

	static	void					defaultInit(::TChannelConfig&vertex, CRandom&random, const CBody*context);
			/**
				\brief Completely rebuilds this shape from scratch. The specified geometry is used as base shape
				\param geometry Fractal geometry to use as shape
			*/
			void					rebuildFromGeometry(const CGeometry&geometry, pInit init_function=defaultInit);
	virtual	CDataSurface*			makeFace()=0;	//!< Abstract top level face constructor
	};
	
	
	/**
		@brief  Fractal data patch
		Contains links to parent, up to three neighbors, and up to 4 children.
	*/
	class CSurfaceSegment:public ::THostSurface
	{
	protected:
									CSurfaceSegment(unsigned exponent, unsigned vertex_count);
									CSurfaceSegment(unsigned exponent_, unsigned vertex_count_, CBody*super_, CSurfaceSegment*parent_,BYTE child_index_,const CTimer::Time now);
			void					modifySphere(const CAbstractSphere<float>&child_sphere);	//!< Alter the local face sphere to include the given child sphere and pass the local sphere on to the respective parent recursivly.
			THostSurfaceLink		resolveChild(const THostSurfaceLink&link, BYTE index);					//!< Returns a link to the child of the given face in the given direction \param link Link to the respective parent neighbor \param index Orientation index of the requested child (mostly opposing the index of the respective neighbor)
			THostSurfaceLink		resolveChildRec(const THostSurfaceLink&link, BYTE index);
			void					include(CSurfaceSegment*child);	//!< Modifies the local CSurfaceSegment::global_sphere variable so that it includes the specified child's CSurfaceSegment::global_sphere
			int						setPriority(const CView&view, bool override_only,const CTimer::Time&);	//!< Invokes the project() callback function to determine the local priority @return local priority @param view View to apply @param override_only Indicates that this segment may have priority only if it features an override flag
			void					setReflectionPriority(const CView&view);
			void					setWaterPriority(const CView&view);

	public:
			class CAttachment	//! Custom surface attachment
			{
			public:
			virtual					~CAttachment()	{};
			};
			
			union TNeighborDepthConfig	//! Structure holding each one value for each neighbor describing the depth difference to the local segment
			{
				unsigned			delta[3];			//!< Detail difference (degrade only) to the currently rendered neighboring face.
				struct
				{
					unsigned		delta0,
									delta1,
									delta2;
				};
			};
	
	
			/*struct
			{
				CTimer::Time		*time;				//!< Project time
				float				ttl;				//!< Time to live
				const CView			*view;				//!< Used view
			}						project_parameters;	//!< Parameters passed to a child during projecting*/
	

			CSeed					inner_seed,			//!< Seed used for inner vertex content and passed to child #3
									outer_seed[3];		//!< Seed used for vertices bordering neighbor i and passed to child #i
			const unsigned			exponent,			//!< Size exponent
									vertex_range,		//!< Number of vertices in one direction
									vertex_count;		//!< Number of vertices (derivable from exponent)

			CAbstractSphere<float>	local_sphere,		//!< Sphere including all local vertices.
									global_sphere;		//!< Sphere including all local and child vertices.
			
			CVec3<float>			corner[3],			//!< Sector relative surface corner vector
									water_center;		//!< Water sphere center
	
			CSurfaceSegment*		parent;				//!< Link to the respective parent or NULL if this is a top level surface
			Auto<CBody*>			super;				//!< Governing Body
			
			float					translation[3],		//!< Temporal translation vector of this surface relative to the view coordinates used during the last project() operation. Valid only if this surface was inserted into the project buffer
									distance_square;	//!< Squared distance between view coordinates and the global sphere of this surface

			TNeighborDepthConfig	surface,
									reflection,
									water;
			
			
			unsigned				priority;			//!< Process priority. Initially this is set to 0. Later on it's set to the last occured project result.
			//CRandom					random;				//!< Random generator structure.
			
			union
			{
				CSurfaceSegment			*child[4];			//!< Children. Either all are set or none.
				struct
				{
					CSurfaceSegment		*c0,*c1,*c2,*c3;
				};
			};
			
			CTimer::Time			referenced;			//!< Time of last project call.
			//bool					has_data;			//!< True if the vertices of the local face have been defined.
			
			struct Visibility
			{
				enum visibility_t
				{
					Invisible	=	0x00,
					Range		=	0x01,
					Primary		=	0x02,
					Reflection	=	0x04,
					Water		=	0x08,
					Override	=	0x10	//consider visibility for subdivision on demand by a visibile segment that lacks neighbors to subdivide
				};
			};
			
			UINT32					visibility;
									
			CAttachment				*attachment;

	static	void	(*onUpdate)(CSurfaceSegment*surface);
	static	void	(*onSegmentAvailable)(CSurfaceSegment*surface);
	
	static	unsigned				max_load;	//!< Maximum number of faces that may be loaded at the same time. 0 (disabled) by default.

	virtual							~CSurfaceSegment();
		template <typename T0, typename T1>
			inline void	convertToAbsolute(const T0 vertex[3], T1 absolute_vertex[3])	const	//!< Converts the specified local vertex to its counterpart in absolute space
			{
				absolute_vertex[0] = (T1)super->sector_size * (T1)sector[0] + (T1)vertex[0];
				absolute_vertex[1] = (T1)super->sector_size * (T1)sector[1] + (T1)vertex[1];
				absolute_vertex[2] = (T1)super->sector_size * (T1)sector[2] + (T1)vertex[2];
			}
		
		template <typename T0, typename T1>
			inline void	convertToRelative(const T0 absolute_vertex[3], T1 relative_vertex[3])	const	//!< Converts the specified global vertex to its counterpart in local space
			{
				relative_vertex[0] = (T1)(absolute_vertex[0]-(T0)super->sector_size * (T0)sector[0]);
				relative_vertex[1] = (T1)(absolute_vertex[1]-(T0)super->sector_size * (T0)sector[1]);
				relative_vertex[2] = (T1)(absolute_vertex[2]-(T0)super->sector_size * (T0)sector[2]);
			}
			void					setNeighbors(const THostSurfaceLink&link0, const THostSurfaceLink&link1, const THostSurfaceLink&link2);
			void					resetLinkage();		//!< Clears both remote and local neighbor links
			void					unhook();			//!< Clears all remote neighbor links to the local surface. Does not change the local neighbor links.
			void					reinit();
			void					resolve(const THostSurfaceLink&l0, const THostSurfaceLink&l1, const THostSurfaceLink&l2);	//!< @deprecated
			void					resolve(unsigned d0, unsigned d1, unsigned d2);
			void					resolve();
			void					resolveReflection(const THostSurfaceLink&l0, const THostSurfaceLink&l1, const THostSurfaceLink&l2);	//!< @deprecated
			void					resolveReflection(unsigned d0, unsigned d1, unsigned d2);
			void					resolveReflection();
			void					resolveWater(unsigned d0, unsigned d1, unsigned d2);
			void					resolveWater();
			/**
				@brief Builds the local vertex data set from its parent's vertex data
			*/
			void					build();
			void					finishBuild();	//!< Retrieves the three corner vertices, updates the sphere, and links neighbors. Automatically called by build()
			bool					rayCast(const CSpaceCoordinates&b, const float d[3], TRayIntersection&intersection);
			bool					groundQuery(const CSpaceCoordinates&b, const float d[3], unsigned min_layer, TGroundInfo&ground);
			
//			void					synchronizeBorders();
			
			void					dropChildren();
			/**
				@brief Creates the next missing child (if any)
				@param time Current time
				@return Pointer to the newly created child or NULL if all children have been created
			*/
			CSurfaceSegment*			createChild(const CTimer::Time&time);
			/**
				@brief Creates all children (if non existing)
				@param time Current time
			*/
			void					createChildren(const CTimer::Time&time);
	TENTRY	void					project(const CView&view, float ttl, const CTimer::Time&time, bool override_only);
	TENTRY	void					projectReflection(const CView&view);
	TENTRY	void					projectWater(const CView&view);
			//void					projectClouds(bool has_translation);
			void					checkTimeouts(const CTimer::Time&time, float ttl);
			void					mergeEdges();
			void					update();
			void					updateClouds();
			bool 					orderOfEdge(BYTE edge)	const;
			
			/**
				@brief Recursively assigns the Visibility::Override flag to the next surface segment responsible for generating a needed surface segment
				
				This method is invoked if a visible or otherwise required surface segment wants to subdivide but has one or more non-synchronized edges.
				The nearest parent of the non existing required surface segment is Visibility::Override flagged. Also all of its parents are flagged that way.
				The flags are valid at most one frame, then reset.
				
				@param child_index Index of the calling child (0-3)
				@param neighbor_index Index of the needed neighbor (0-2)
				@param t Time to assign to the referenced member variable of triggered segments
				@param priority_to_set Priority value to assign to triggered segments
			*/
			void					triggerOverrideFlag(BYTE child_index, BYTE neighbor_index,const CTimer::Time&t, unsigned priority_to_set);
			/**
				@brief Inner method of triggerOverrideFlag(BYTE child_index, BYTE neighbor_index)
				
				Flags the local segment and all parents. NULL-pointer sensitive
				
				@param t Time to assign to the referenced member variable of triggered segments
				@param priority_to_set Priority value to assign to triggered segments
				@return true on success, false if the local segment is not existing or not ready
			*/
			bool					triggerOverrideFlag(const CTimer::Time&t, unsigned priority_to_set);
	
			
			/**
				@brief Default visual sphere definition method
				
				defaultDefineSphere() updates @b sphere so that it roughly includes all vertices of the local fractal face.
				The method is programmed for performance and thus does not check each vertex separatly.
				Under normal circumstances the resulting sphere will be 10% larger than required to minimize non-included vertices.
				
				@param sphere Sphere to modify
			*/
			void					defaultDefineSphere(CAbstractSphere<float>&sphere);
			void					updateSphere();
			void					checkLinkage();	
			void					getCoverVertices(CArray< ::TCoverVertex>&cover_vertices);	//!< Retrieves one cover vertex per rendered vertex of this surface
	};
	
	/**
		\brief Fractal root face
		
		The root face holds all basic information about a face. That is all its vertices, children, parent, and neighbor links. All vertices are seen relative to the central face sector coordinates (CSurfaceSegment::sector)
	*/
	class CDataSurface:public CSurfaceSegment//, public CThreadObject
	{
	protected:
			//CConstruct::TProjectThreadParameter	split_param[4];
			
									
			
									CDataSurface(::TVertex*vertices, unsigned exponent, unsigned vertex_count);

			static	bool			faceCollision(const ::TVertex&v0, const ::TVertex&v1, const ::TVertex&v2, const float coord[3], float point_out[3], float normal_out[3]);
			
			friend class CBody;


	public:
			::TVertex				*vertex_pointer;			//!< Non proprietary pointer to the first element of the appointed vertex array
			

			
	//virtual							~CDataSurface();
			
			inline ::TVertex&		getVertex(unsigned col, unsigned row)
			{
				return vertex_pointer[super->full_map->getIndex(col,row)];
			}			
			inline const ::TVertex&	getVertex(unsigned col, unsigned row)	const
			{
				return vertex_pointer[super->full_map->getIndex(col,row)];
			}
			
			inline ::TVertex&		getCorner(BYTE index)
			{
				return vertex_pointer[super->full_map->border_index[index][0]];
			}			
			inline const ::TVertex&	getCorner(BYTE index)	const
			{
				return vertex_pointer[super->full_map->border_index[index][0]];
			}
			

			void					copyEdge(BYTE k);
			void					fillAllNormals();
			void					fillAllUnlockedNormals(bool*lock_map);
			
			void					update();
	
			void					recSyncBorders();	//debugging only

			

			void					generateLayer(unsigned level);
			void					generateLayer(unsigned level, void(*updateHeight)(::TVertex&vertex));	//!< Version of generateLayer using a custom height update 
			void					syncGeneratedBorders(unsigned level);	//!< Synchronizes border vertices as generated by generateLayer(). 
			void					mergeBorderNormals(bool update=true);	//!< Used during shape top level generation only
		
	};



	template <unsigned Exponent>
		class CTemplateSurface:public CDataSurface
		{
		public:
				
				static const unsigned	vertices=NUM_VERTICES(Exponent),
										range=(1<<Exponent)+1,
										minor_count=vertices-NUM_VERTICES(Exponent/2);
				
				
				::TVertex				vertex[vertices];
		
										CTemplateSurface();
										CTemplateSurface(CBody*super_);
										CTemplateSurface(CBody*super_,CTemplateSurface<Exponent>*parent, BYTE child_index, const CTimer::Time&time);
											
										CTemplateSurface(CBody*super_,CTemplateSurface<Exponent>*parent, BYTE child_index, const THostSurfaceLink&n0, const THostSurfaceLink&n1, const THostSurfaceLink&n2, const CTimer::Time&time);
		//virtual	void					createChildren(const CTimer::Time&time);
		};
	
	
	template <unsigned Exponent, unsigned ExponentReduction>
		class CTemplateBody:public CBody
		{
		public:
				typedef CTemplateSurface<Exponent>	Face;
		static	CTemplateMap<Exponent>						full_map;
		static	CTemplateMap<Exponent-ExponentReduction>	reduced_map;
		static	CUDA::CDeviceArray< ::TVertexInfo>			device_info_field;
		static	CUDA::CDeviceArray<unsigned>				device_vertex_index_field,
															device_border_field[3],
															device_triangles;
				
										CTemplateBody(float sector_size=default_sector_size, unsigned max_recursion_level=32);
										CTemplateBody(const CString&name, float sector_size=default_sector_size, unsigned max_recursion_level=32);
		virtual	CDataSurface*			makeFace();
		static	void					assembleDeviceMaps(unsigned lookup_reduction);
		};
	
	
	/**
		@brief Triangle index pattern storage
		
		CAbstractIndexPatterns stores a number of triangular patterns for different surface-to-neighbor-constellations
	*/
	class CAbstractIndexPatterns
	{
	public:
			UINT32					*const pattern,			//!< Pointer to index patterns
									*const pattern_offset,	//!< Pattern offsets. A pattern @a i covers all entries of @a pattern from @a pattern_offset[i] to @a pattern_offset[i+1]
									*const core_pattern;	//!< Pointer to a pattern covern only the inner non-border triangles
			const UINT32 			num_patterns,			//!< Number of patterns stored in this structure. Up to @a num_patterns+1 entries of @a pattern_offset are valid
									exponent,				//!< Size exponent used to generate the local index pattern set
									step0,					//!< Factor applied to neighbor 0 detail difference
									step1;					//!< Factor applied to neighbor 1 detail difference 
									//step2 equals 1

									CAbstractIndexPatterns(UINT32*const pattern, UINT32*const pattern_offset, UINT32*const core_pattern, UINT32 num_patterns, UINT32 exponent);
			unsigned				getPatternIndexFor(const CSurfaceSegment*seg, const CSurfaceSegment::TNeighborDepthConfig&config)	const; 	//!< Retrieves the index of the pattern most suited for the current status of the specified surface. The resulting index can be used as array index of @a pattern_offset @param seg Parent surface segment @param config Depth configuration to retrieve the pattern for @return applicable pattern
			unsigned				getPatternIndexFor(const CSurfaceSegment::TNeighborDepthConfig&config,const CSurfaceSegment*seg)	const; 	//!< @overload
			unsigned				getReflectionPatternIndexFor(const CSurfaceSegment*face);	//!< Retrieves the index of the pattern most suited for the current status of the specified surface. The resulting index can be used as array index of @a pattern_offset
	};
	
	/**
		@brief Template derivative of CAbstractIndexPatterns
		
		CIndexPatterns must be constructed via create() before it can be used.
	*/
	template <unsigned Exponent>
		class CIndexPatterns:public CAbstractIndexPatterns
		{
		private:
				static CTemplateMap<Exponent>	map;
				
				static const unsigned	range = (1<<Exponent)+1;
				static UINT32 			core_pattern[(range-4)*(range-4)*3],
										*current,
										*ocurrent;
				static bool				created;
																					
				static void			 	createCorePattern();
				static void				makePattern(unsigned r0, unsigned r1, unsigned r2);
		public:
				static UINT32			pattern[(Exponent+1)*(Exponent+1)*(Exponent+1)*range*range*3],	//!< Maps to CAbstractIndexPatterns::pattern
										pattern_offset[(Exponent+1)*(Exponent+1)*(Exponent+1)+1];		//!< Maps to CAbstractIndexPatterns::pattern_offset
		 
										CIndexPatterns();
				void					create();			//!< Creates the local pattern. Must be called at least once before accessing the contents of @a pattern and @a pattern_offset . Subsequent calls to this method have no effect
		};

	
	/**
		\brief Texture space class to roll a planar texture around a sphere
		
		The CRollSpace class can be used to project a seamless planar texture onto the surface of a sphere in such a way that the observer believes
		thet the texture is actually fixed on the geometry. The object will produce a system that can be loaded into a graphics context to use the vertex
		space coordinates as texture coordinates.
		This illusion requires the following conditions to be met:<br />
		<ul>
		<li>The texture must be seamless. Large texture coordinates are used</li>
		<li>The texture should be roughly equal colored to not create ugly patterns on larger views</li>
		<li>The used texture range (the visual size of one repeated pattern) should be below 1% of the sphere's size or the illusion will become obvious</li>
		<li>The used clip-size must be a multiple of any used texture range or odd texture jumps will occur</li>
		</ul>
		
	*/
	class CRollSpace
	{
	private:
			void	mod(double&val)	const;
	public:
			double	invert[16],			//!< Final (unscaled) system to load to a graphics context. Use exportTo() to create a scaled version
					texture_root[3],	//!< Current texture zero space. This is the absolute location of the texture point of origin
					texture_up[3],		//!< Vertical texture space axis pointing along the (invisible) up axis of an applied texture
					texture_x[3],		//!< Horizontal texture space axis pointing along the x axis of an applied texture
					texture_y[3],		//!< Depth texture space axis pointing along the y axis of an applied texture
					modal_range;		//!< Coordinate range to capture \b texture_root in. Texture coordinates generated by CRollSpace can become very large and unclipped texture offsets would soon create undesired effects.
			
					CRollSpace(double modal_range=CSpaceCoordinates::default_sector_size);							//!< Creates a new roll space with texture_up pointing along the y-axis in space. \param modal_range Coordinate range to capture \b texture_root in. Texture coordinates generated by CRollSpace can become very large and unclipped texture offsets would soon create undesired effects.
			void	roll(const double up[3], const double translation[3]);		//!< Rolls the local system so that the new texture up coordinate matches \b up. \param up Vertical direction \param translation Observer movement
			void	roll(const float up[3], const float translation[3]);		//!< Rolls the local system so that the new texture up coordinate matches \b up. \param up Vertical direction \param translation Observer movement
			void	roll(const float up[3], const double translation[3]);		//!< Rolls the local system so that the new texture up coordinate matches \b up. \param up Vertical direction \param translation Observer movement
			void	exportTo(float system[16], double range, bool include_translation=true) const;			//!< Creates a scaled system \param system Out pointer to a system matrix to write to \param range Size of one texture pattern @param include_translation Set true to include system translation. False will set the translation column of the target matrix to 0
	};
	
	


	/**
		@brief Initializes background core for parallel landscape construction in the background.
		
		This function is required to be executed once if landscape construction should be performed in the background via
		backgroundSubdivide().
	
	*/
	void	backgroundInit(Engine::OpenGL*opengl);



	
/* module-composition *


b) Face

The sector-class/struct must provide the following definitions:

			typedef <type>			FloatType;
			static const unsigned	Size=<exponent>;
			
FloatType specifies the default float-type used for vertices and prameters. (float by default)
Size specifies the sector-resolution-exponent. The sector will have around 2^Size * 2^Size faces.
(5 by default)

In addition to these definitions it must also provide the following static method:
template <class Super, class Norm>
static void				update(Map&map,CFractalSector<Reflection,Norm>*face)

map is a reference to the respective map-object (i.e. CSphereMap<...>)
while norm classifies the map-norm (since the map-norm is no longer bound to the module)
In this method all necessary updates to the additional sector-data should be performed.
Whenever the underlying sector-data changes this method is called by the fractal-tree.
If the sector does not provide any additional data then this method may be empty.

c) Map

The map-class/struct must provide the following definitions:

			static const unsigned	MaxLevel = <maximum number of visible levels>,
									MaxSectors = <maximum faces per level>;
									
These definitions limit the tracable range of the fractal-tree.
The trace-command of any map-object will recursively scan the tree and execute project(...)
on every tree-node. Depending on the result of the call a face may be added to an internal
trace-buffer allowing to specifically pick the faces to actually render.
This buffer is limited by MaxLevel and MaxSectors to allow a maximum of MaxSectors rendered
faces per recursive depth-layer.

Methods:
Necessary if trace/project is supposed to be callable:

			template <class Norm>
				eProjectResult				project(CFractalSector<Reflection,Norm>*face)

'Reflection' is a typedef on the local module.
The method is required to return one of the following depending on the state of 'face':
CR_IGNORE: face is neither inserted nor further sub-tiled.
CR_INSERT: face is inserted into the buffer but not further sub-tiled.
CR_FOCUS: face is not inserted but instead its children are traced (if available).

Necessary if traceAlt/projectAlt is supposed to be callable:

			template <class MapType, class Norm, class Parameter>
				static eProjectResult		projectAlt(MapType&self, CFractalSector<Reflection,Norm>*face, const __int64&time, float ttl, const Parameter&parameter)

The method is required to return one of the above however it's also supposed to perform
the subtiling by itself. This is done by calling
		face->child[i]->projectAlt(self,time,ttl,parameter);
on all its children if required.
'parameter' may be any type of parameter passed through the tree.



*/



#if 0
 triangle order-specification:

vertices:

	col->
row 1
 |	23
 V	456
	7890
	12345
	678901
	...

children:

	0
  . 3 .
1	.	2

.
|\
| \
|0 \
|---\
|\ 3|\
|1\ |2\
|__\|__\

(the third child`s orientation is inverted)

neighbors:

	.
 0 / \ 2
  .___.
	1

orientation (note arrows):

0:
	  .
-> 0 / \ 2
	.___.
	  1

1:
	  .
-> 1 / \ 0
	.___.
	  2

2:
	  .
-> 2 / \ 1
	.___.
	  0

corner-vertices:

0	(0,1)



1		2	(1,0)
(0,0)



vertex hertiage

p=parent
s=sibling

		  . p0 .
	. s0 _	|  _  s3 .
* .	    	v		  . *
	 s1 .	|	. s2
			p1

		
#endif



	
	


}

#include "fractal.tpl.h"


#endif
