#ifndef fractal_interfaceH
#define fractal_interfaceH

/*
	This module implements the general fractal interface: bodies, segments, index patterns, aspects, seeds, geometries, and hashing functions
*/



#include "../math/vclasses.h"
#include "../math/object.h"
#include "../math/object_def.h"
#include "../general/timer.h"
#include "../general/system.h"
#include "../general/thread_system.h"
#include "../general/random.h"
#include "../container/production_pipe.h"
#include "../container/sorter.h"
#include "../container/buffer.h"
#include "../io/mutex_log.h"
#include "../io/delta_crc32.h"
#include "../general/auto.h"
#include "../geometry/coordinates.h"
#include "../io/log_stream.h"



#include "fractal_kernel.h"

#define FRACTAL_NURBS_CONST		0.35

//#define HYBRID_KERNEL

//MAKE_SORT_CLASS(PrioritySort,priority)


#define DEBUG_POINT(_DOMAIN_)	//CONSOLE_DEBUG_POINT


//#define VERTEX_DEFINITION






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
	#define BEGIN	//logfile<<"entering: "+String(__func__)+"\n";
#endif
#ifndef END
	#define END		//logfile<<"exiting: "+String(__func__)+"\n";
#endif

#ifndef LBEGIN
	#define LBEGIN	//logfile<<"begin: "+String(__func__)+"\n";
#endif

#define FRACTAL_BUILD_STATISTIC

namespace DeltaWorks
{

	/**
	@brief Fractal landscape generator
	*/
	namespace Fractal
	{
		extern SynchronizedLogFile		logfile;
	
		extern unsigned			num_sectors;		//!< Total number of sectors created at this time
		extern unsigned			max_sectors,		//!< Absolute number of faces allowed at any given time.
								build_operations;	//!< Number of build or update operations registered. Available only if FRACTAL_BUILD_STATISTIC is defined
		extern Timer::Time		build_time;			//!< Time spent performing build or update operations. Available only if FRACTAL_BUILD_STATISTIC is defined
		extern TContext			default_context;	//!< Empty default context passed for certain kernel operations

		extern bool				mipmapping,				//!< Set true to enable automatic mipmap level generation of segment textures. True by default
								texture_compression,	//!< Set true to enable texture compression on segment textures. False by default. Leave this off. Slow, faulty.
								background_subdiv;		//!< Set true to process additional subdivision in the background. Background operations will be started after calling the subdivide() method and stopped before the first process...() call

	
		/**
			@brief Published background subdivision methods
		*/
		namespace BackgroundSubDiv
		{
			void										signalEnd();	//!< Signals that background subdivision should come to an end at the earliest possibility
		}



		//static const float		default_sector_size=50000.0f;
	
	
		typedef Composite::sector_t	sector_t;		//!< Sector coordinate type. Typically int32

		struct TMapVertex;
	

	
		class VertexMap;
		template <unsigned,unsigned =0>
			class TemplateMap;
	
		class Geometry;
	
	
		String				randomWord();	//!< Creates a random word usable as seed for new fractal bodies
		void				terminate();	//!< Implodes the used kernel. This should be called before the application quits
		void				makeSphereGeometry(Geometry&target);	//!< Adjusts the given geometry to approximate a sphere
		Geometry			makeSphereGeometry();					//!< Creates a new sphere approximating geometry
		void				makeHaloGeometry(Geometry&target);		//!< Adjusts the given geometry to approximate a halo shape
		Geometry			makeHaloGeometry();						//!< Creates a new halo approximating geometry
		String				getConfiguration();
	

	
		void				reportFatalException(const TCodeLocation&location, const String&message);
	
		#define FRAC_FATAL(message)	Fractal::reportFatalException(CLOCATION,message);
	
	

	

	

		typedef	void		(*pInit)		(TChannelConfig&vertex_channel, Random&random, const TContext*context);	//!< Initializes channel data of a new top level vertex \param vertex_channel Target channel data \param random Initialized random source
	
	
		/**
			\brief Fractal geometry
	
			The fractal geometry is used to describe the rough geometry of a fractal shape. It may consist of only a singular triangle, or a sphere, or any shape as required.
		*/
		class Geometry 
		{
		public:
				typedef TVec3<>	Vertex;

				Array<Vertex>			vertexField;	//!< Geometrical vertices
				Array<UINT32>			triangleField,	//!< Triangular index path. Each three indices form one triangle using the vertices stored in \b vertex_field
										edgeField;		//!< Edge index path. Each two indices form one edge, using the vertices stored in @b vertexField

		};

	
	
	
		/**
			@brief 64 bit helper hash function
		
			Not my concept, only my implementation. I know, it looks very very odd.<br>
			Found here:<br>
			http://www.java-forums.org/new-java/17026-i-need-32-64-bit-hash-function.html

		*/
		class Hash64
		{
		public:
				UINT64			byte_table[0x100];
		static const UINT64		start_value = 0xBB40E64DA205B064ULL,
								factor = 7664345821815920749ULL;
	
	
								Hash64()
								{
									UINT64 h = 0x544B2FBACAAF1684ULL;
									for (unsigned i = 0; i < 256; i++)
									{
										for (unsigned j = 0; j < 31; j++)
										{
											h = (h >> 7) ^ h;
											h = (h << 11) ^ h;
											h = (h >> 10) ^ h;
										}
										byte_table[i] = h;
									}
								}
			
				UINT64			operator()(const void*data, size_t len)	const
								{
									UINT64 hashed = start_value;
									const BYTE*field = (const BYTE*)data;
									for (size_t i = 0; i < len; i++)
									{
										hashed = (hashed * factor) ^ byte_table[field[i]];
									}
									return hashed;
								}
		};
  

	
	
		/**
			\brief Dynamic length fractal seed
		
			Rotatable, expandable<br>
			This seed type expands as recursion progresses. Due to the basically infinite nature
			of this seed, it may or may not cause significant additional memory consumption.
		*/
		class DynamicSeed
		{
		public:
				typedef Array<BYTE,Primitive>	ValueType;	//!< Seed data type
			
				ValueType				value;	//!< Binary data content of this seed
				UINT32					as_int;	//!< Auto-updated int32 representation of the seed
			
										DynamicSeed();
										DynamicSeed(const String&string);
				String					toHex()	const;	//!< Converts the seed to a printable string
				DynamicSeed&			hash(const String&string);	//!< Builds the local seed from a string
				DynamicSeed&			operator=(const ValueType&array);	//!< Builds the local seed from an existing binary seed
				void					implant(const DynamicSeed&seed, BYTE child_id);
				void					rotate(unsigned depth, BYTE orientation);
				void					randomize();	//!< Randomizes the local seed content
				UINT32					toInt()	const;	//!< Explicitly queries the int representation of the local seed. Under normal circumstances the result is identical to the local @a as_int variable
		};
	
	
		/**
			@brief Fixed length 64 bit fractal seed
		
			Rotatable, shiftable<br>
			This seed type stays 64 bit at all times. Increases processing and storage efficiency
			while preserving sufficient diversity for any feasible application scenario.
		*/
		class Seed64
		{
		public:
		static	Hash64					hashFunction;	//!< Hashing functor
	
				typedef UINT64			ValueType;	//!< Seed data type
			
				UINT64					value;	//!< Raw 64bit seed
				UINT32					as_int;	//!< Auto-updated int32 representation of the seed
			
										Seed64();
										Seed64(const String&string);
				String					toHex()	const;	//!< Converts the seed to a printable string
				Seed64&				hash(const String&string);	//!< Builds the local seed from a string
				Seed64&				operator=(const UINT64&seed);	//!< Builds the local seed from an existing binary seed
				void					implant(const Seed64&seed, BYTE child_id);
				void					rotate(unsigned depth, BYTE orientation);
				void					randomize();	//!< Randomizes the local seed content
				/**
					@brief Explicitly queries the int representation of the local seed. Under normal circumstances the result is identical to the local @a as_int variable
				
					Algorithm designed by Thomas Wang, Jan 1997<br />
					http://www.concentric.net/~Ttwang/tech/inthash.htm
				*/
				UINT32					toInt()	const;
		};
	
	

		typedef Seed64				Seed;	//!< Could use dynamic seeds here too
	
	


		
		/**
		\brief Texture space class to roll a planar texture around a sphere
		
		The RollSpace class can be used to project a seamless planar texture onto the surface of a sphere in such a way that the observer believes
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
		class RollSpace
		{
		private:
				void	mod(double&val)	const;
		public:
				TMatrix4<double>	invert;		//!< Final (unscaled) system to load to a graphics context. Use exportTo() to create a scaled version
				TVec3<double>		texture_root,	//!< Current texture zero space. This is the absolute location of the texture point of origin
									texture_up,		//!< Vertical texture space axis pointing along the (invisible) up axis of an applied texture
									texture_x,		//!< Horizontal texture space axis pointing along the x axis of an applied texture
									texture_y;		//!< Depth texture space axis pointing along the y axis of an applied texture
				double				modal_range;		//!< Coordinate range to capture \b texture_root in. Texture coordinates generated by RollSpace can become very large and unclipped texture offsets would soon create undesired effects.
			
						RollSpace(double modal_range=Composite::Coordinates::default_sector_size);							//!< Creates a new roll space with texture_up pointing along the y-axis in space. \param modal_range Coordinate range to capture \b texture_root in. Texture coordinates generated by RollSpace can become very large and unclipped texture offsets would soon create undesired effects.
				void	roll(const TVec3<double>&up, const TVec3<double>&translation);		//!< Rolls the local system so that the new texture up coordinate matches \b up. \param up Vertical direction \param translation Observer movement
				void	roll(const TVec3<float>&up, const TVec3<float>& translation);		//!< Rolls the local system so that the new texture up coordinate matches \b up. \param up Vertical direction \param translation Observer movement
				void	roll(const TVec3<float>&up, const TVec3<double>&translation);		//!< Rolls the local system so that the new texture up coordinate matches \b up. \param up Vertical direction \param translation Observer movement
				void	exportTo(TMatrix4<float>&system, double range, bool include_translation=true) const;			//!< Creates a scaled system \param system Out pointer to a system matrix to write to \param range Size of one texture pattern @param include_translation Set true to include system translation. False will set the translation column of the target matrix to 0
		};
	
	
		float			relativeSeamWidth(unsigned exponent);	//!< Calculates the relative safety seam width (in texels) between style and normal regions of a segment texture. The resulting value is auto-transformed to texture coordinate space (range [0,1])
		float			relativeTexelSize(unsigned exponent);	//!< Calculates the width/height of a single texture pixel at the specified exponent in texture coordinate space (range [0,1])








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
	s=GetSibling

			  . p0 .
		. s0 _	|  _  s3 .
	p3 .    _	v	_	  . p2
		 s1 .	|	. s2
				p1


	Extended border system:
	Children contain part of their parents vertex data to adapt to level of detail changes (necessary due to parent vertex degeneration).
	These vertices will be placed after the standard inner vertices in the following pattern:

	[core vertices][edge0,parent0, length=(2<<(exponent-1))-1][edge0,parent1,length=(2<<exponent-2))-1]...[edge1,parent0][edge1,parent1]...[edge2,parent0]...
	Total vertex volume along one edge: (exponent-1)^2




	Override system:
	The system may look odd but it works (though perhaps not perfectly)
	Override priority is added to normal priority by a setPriority() invocation that determines that not all its edges have been synchronized.
	As such it assigns additional priority (its own priority in fact) to the next available parents (and their parents, recursively) of the missing
	neighbor(s). Since the project() method is inherently invoked in a sub-optimal order in regards to override priorities, two scenarios commonly
	occur:

	a) (prior case) the segment receives additional priority ahead of its respective project() (and thus setPriority()) invocation.
	The Visibility::Override flag is set, thus preventing it from resetting its current priority value. It will, however, clear the flag.
	Should the respective segment be considered visible by itself, then the override priority will be overwritten with its local priority.

	b) (successive case) the segment receives additional priority after its respective project() invocation. Subdivision can no longer take place
	during this frame, however the increased priority will take effect during the next subdivision.
	Other than that, this case behaves just like case a), though delayed until the next frame.

	Case b) may put considerably more priority on a surface segment than case a) if the respective segment is both visible and in demand.
	Both effects do not last longer than two frames. The imbalance effect is thus considered minor.

	#endif



	
	


	}

	#include "interface.tpl.h"
}

#endif
