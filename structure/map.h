#ifndef mapH
#define mapH

#include "../geometry/cgs.h"
#include "../io/xml.h"
#include "../io/file_system.h"
#include "../math/vclasses.h"
#include "../container/string_list.h"
#include "../geometry/cgs_track_segment.h"
#include "../geometry/graph_mesh.h"
#include "../image/converter/magic.h"

namespace Map	//! Geometrical scenario composition
{

	/*!
		\brief Load context pointer as passed to the Composition::loadFromFile() method
		
		The printMessage() callback method will be invoked if a certain loading step is finishing or has been completed.
	*/
	class LoadContext
	{
	public:
	virtual void					printMessage(const String&message)	{};	//!< Message out callback method
	};


	class Composition;
	class ObjectInstance;



	class TextureResource : public CGS::TextureResource
	{
	public:
			bool						pool_textures;
	private:
			GenericHashContainer<name64_t,CGS::TextureA>	container;
			GenericHashTable<name64_t,CGS::TextureA*>		pool;
			CGS::Geometry<>				*currently_loading;	

	public:
										TextureResource():pool_textures(false)
										{}
			void						signalNowLoading(CGS::Geometry<>&geometry)
										{
											currently_loading = &geometry;
										}

			void						insert(const String&filename, name64_t as_name, bool bump_map,float bump_strength =0.01f, float blur_radius = 0.0f);
			void						insert(Image&image, name64_t as_name, bool bump_map,float bump_strength =0.01f, float blur_radius = 0.0f);
			void						insert(const Image&image, name64_t as_name);

	virtual count_t						countEntries()
										{
											throw Program::FunctionalityNotImplemented();
										}
	virtual CGS::TextureA*				entry(index_t index)
										{
											throw Program::FunctionalityNotImplemented();
										}
								
	virtual CGS::TextureA*				retrieve(const name64_t&name)
										{
											
											CGS::TextureA*rs=NULL;
											if (pool_textures && pool.query(name,rs))
												return rs;
											if (currently_loading != NULL)
												rs = currently_loading->local_textures.retrieve(name);
											if (rs && pool_textures)
												pool.set(name,rs);
											return rs != NULL? rs : container.lookup(name);
										}
	};
	
	
	/*!
		\brief Composition info structure
	*/
	struct TInfo
	{
			String					identity,	//!< Name of the composition
									artist,		//!< Original artist of the composition
									abstract;	//!< Additional abstract information about this composition (if any)
	};


	/*!
		\brief Abstract super class for all composition entities.
		
		Instances of the Entity class natively provide location and orientation information although some derivative entities do not require them.
	*/
	class Entity
	{
	public:
			enum Flags				//! Entity class flag collection
			{
				Directional		=0x1,	//!< Entity is directional, meaning it uses the \b direction[3] variable instead of the \b x_align[3] variable. Directional entities always point their z-axis along the \b direction vector while non-directional entities align their x-axis along the \b x_align vector (as far as possible)
				NoPosition		=0x2,	//!< Entity is abstract, meaning has no geometrical position in the scenery.
				NoOrientation	=0x4,	//!< Entity is a point rather than a system.
				NoOrder			=0x8,	//!< Entity has no order attribute
				
				MetaEntity		=NoPosition|NoOrientation|NoOrder,	//!< Entity is pure abstract
				Default			=0									//!< Entity is located, rotatable, and has a fixed order value
			};
			
			String					sub_line;						//!< Secondary description string
		
			TVec3<>					position,						//!< Position of this entity in R3. This vector is undefined if \b flags contains \b NoPosition
									y_align;						//!< Y-Align of the local entity. The y alignment directly defines the y-axis if the local entity is non-directional, otherwise the effective y-axis approximates this vector. This vector is undefined if \b flags contains \b NoOrientation
			Box<>					constraints;					//!< Dimensional constraints. The first three components define the lower box corner, the last three the upper box corner. Constraints are local (relative to the local system)
			TMatrix4<>				system,							//!< Effective entity system (4x4 out matrix). This matrix is undefined if \b flags contains \b NoPosition. This system may directly be passed to rendering contexts such as OpenGL
									inverse;						//!< Inverse of the entity system (4x4 in matrix). This matrix is automatically updated when the system matrix is recompiled.
			union
			{
				TVec3<>				x_align,						//!< X-Align of the local entity. The x alignment loosly defines the x-axis of the final entity system if \b flags does not contain \b Directional
									direction;						//!< Directional (Z-)align of the local entity. The z align is directly defined by this vector if \b flags contains \b Directional
			};
			const Flags				flags;							//!< Constant entity class configuration. \b flags may contain any combination of the above defined flags
			int						order;							//!< Effective order value of this entity. The functionality of the order attribute depends on the respective derivative class. If this entity is for instance a checkpoint shape then the order value effectively defines the checkpoint index.
			String					name;							//!< Name of this entity (mostly unique)
			const char				*const	entity_class;					//!< Name of the derivative class
			Composition				* composition;				//!< Pointer to the respective parent composition


									Entity(const char*entity_class_,int flags_):flags((Flags)flags_),order(0),entity_class(entity_class_),composition(NULL)
									{
										position = Vector<>::zero;
										y_align = Vector<>::y_axis;
										x_align = Vector<>::x_axis;
										constraints.setAllMin(-1);
										constraints.setAllMax(1);
									}
									/*!
										\brief Entity class constructor
										
										Any derivative class must provide a constant class string and flag combination
										
										\param entity_class_ Name of the derivative class
										\param flags_ Combination of Entity::Flags
									*/
									Entity(const char*entity_class_,int flags_,Composition*composition_):flags((Flags)flags_),order(0),entity_class(entity_class_),composition(composition_)
									{
										position = Vector<>::zero;
										y_align = Vector<>::y_axis;
										x_align = Vector<>::x_axis;
										constraints.setAllMin(-1);
										constraints.setAllMax(1);
									}
									Entity(Entity&&other):sub_line(other.sub_line),position(other.position),y_align(other.y_align),constraints(other.constraints),system(other.system),inverse(other.inverse),
															x_align(other.x_align),flags(other.flags),order(other.order),name(other.name),entity_class(other.entity_class),composition(other.composition)
									{
										ASSERT__(other.name.isEmpty());
									}
			Entity&					setComposition(Composition*composition_)
									{
										composition = composition_;
										return *this;
									}
	virtual							~Entity(){};
			void					updateSystem();	//!< Updates the local system from changed position or align vectors
	virtual void					parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context);	//!< Parses the content of the specified XML node and automatically updates the system
	virtual	bool					cast(const TVec3<>&from, const TVec3<>&direction, float&distance)	const;	//!< Virtual vector cast. Determines the closest ray intersection. \param from 3 component base vector \param direction Normalized ray direction vector \param distance in/out distance value along the specified ray axis of the closest intersection \return true if a closer intersection was detected, false otherwise

			void					adoptData(Entity&other)
									{
										sub_line.adoptData(other.sub_line);
										position = other.position;
										y_align = other.y_align;
										constraints = other.constraints;
										system = other.system;
										inverse = other.inverse;
										x_align = other.x_align;
										order = other.order;
										name.adoptData(other.name);
										composition = other.composition;
									}

	};
	
	/*!
		\brief Race start position entity class
		
		The respective moving entity is supposed to be placed pointing towards the negative z axis (3rd system column), with y (2nd system column) pointing up and x (1st system column) pointing to the right of a starting moving entity.
	*/
	class StartPosition:public Entity
	{
	public:
									StartPosition():Entity("StartPosition",Entity::Directional)
									{}
									StartPosition(Composition*composition):Entity("StartPosition",Entity::Directional,composition)
									{}
	};

	/*!
		\brief Composition light entity class
	*/
	class Light:public Entity
	{
	public:
			enum Type	//! Light type
			{
				Omni,		//!< Omni directional light source
				Direct,		//!< Directional (parallel) light source
				Spot,		//!< Directional spot light source
				Types		//!< Number of light types
			};
			Type					type;			//!< Light type
			float					spot_cutoff;	//!< Light spot cutoff
			TVec3<>					color;			//!< General light color
			union
			{
				struct
				{
					float			constant_attenuation,	//!< Light constant attenuation (1 by default)
									linear_attenuation,		//!< Light linear attenuation (0 by default)
									quadratic_attenuation;	//!< Light quadratic attenuation (0 by default)
				};
				TVec3<>				attenuation;
			};
			BYTE					spot_exponent;			//!< Light spot exponent (if type is Spot, 0...128)
			
									Light();
									Light(Composition*composition);
			void					parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context);	//!< Parses the local light configuration from the specifed XML node
	};
	
	/*!
		\brief Composition gravity center entity class
	
	*/
	class GravityCenter:public Entity
	{
	public:
			float					mass;	//!< Mass of the local gravity center
									
									GravityCenter():Entity("GravityCenter",Entity::NoOrientation|Entity::NoOrder),mass(1.0)
									{}
									GravityCenter(Composition*composition):Entity("GravityCenter",Entity::NoOrientation|Entity::NoOrder,composition),mass(1.0)
									{}
			void					parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context);	//!< Parses the local gravity center configuration from the specified XML node
	};
	
	/*!
		\brief Composition environment configuration
	*/
	struct TEnvironment
	{
			String					cube_archive,		//!< Archive file name to derive the cube texture from
									cube_path;			//!< Texture name of the cube texture in the specified cube archive
			TVec3<float>			color,				//!< Background color (black by default)
									global_gravity;		//!< Global (omni present) parallel gravity vector (acceleration per second, 0 vector by default)
			float					atmosphere_density;	//!< Atmospheric density applied to vehicles for friction and updrift. This value should relate to some sort of air density scalar (1.0 by default)
			Buffer<Light,0>			lights;				//!< Lights in the local environment
			Buffer<GravityCenter,0>	gravity_centers;	//!< Gravity centers in the local environment
			
									TEnvironment():atmosphere_density(1.0)
									{
										color = Vector<>::zero;
										global_gravity = Vector<>::zero;
									}
	};
	

	
	/*!
		\brief Effect entity (i.e. a collision or area effect)
	
	*/
	class Effect:public Entity
	{
	public:
			enum Type	//! Effect type enumeration
			{
				Score,		//!< Effect alters player score value
				Wind,		//!< Effect causes wind force on moving entities
				Gravity,	//!< Effect causes additional gravity force on moving entities
				Refuel,		//!< Effect refuels (energizes) a moving entity
				Repair,		//!< Effect repairs a moving entity
				Damage,		//!< Effect causes damage to a moving entity
				Kill,		//!< Effect instantly kills a moving entity
				Types		//!< Number of effect types
			};
			Type						type;	//!< Effect type
			float						strength;	//!< Strength of the specified type. This may be a punctual intensity if reacting on a collision or per second if the local effect affects an area.
		
										Effect():Entity("Effect",Entity::MetaEntity), type(Repair),strength(1)
										{}
										Effect(Composition*composition):Entity("Effect",Entity::MetaEntity,composition), type(Repair),strength(1)
										{}
			void						parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context);	//!< Parses the local effect configuration from the specified XML node
	};
	
	typedef shared_ptr<CGS::StaticInstance<> >	Instance;

	/*!
		\brief Object instantiation entity.
		
		A ObjectInstance entity instantiates a geometry or sub composition as provided by a Import instance. One Import instance may be referenced my any number of ObjectInstance instances.
	
	*/
	class ObjectInstance:public Entity
	{
	public:
			
			enum Type	//! Instance type enumeration
			{
				Item,			//!< Instance is a collectibale geometry that will disappear on collision
				Physical,		//!< Instance is physical and may block the path of a moving entity
				Holographic,	//!< Instance is holographic. Moving entities may fly through it.
				Types			//!< Number of instance types
			};
			Type					type;			//!< Instance type
			String					source_name;	//!< Data source name (pre load phase)
			count_t					spawn_count,	//!< Number of times this instance may spawn in total (1 by default, 0 deactivates this instance). Numbers greater than 1 allow this entity to respawn if collected (if type is Item)
									spawned;		//!< Client application value. Number of times this instance spawned so far
			float					appear_delay,	//!< Number of seconds to pass before this instance may appear the first time
									respawn_delay,	//!< Number of seconds to pass before this instance may respawn after being collected
									current_delay;	//!< Client application value. Time since item creation of last collection (to check if the item should spawn/respawn)

			Instance				instance;	//!< Pointer to a geometry instance (if the respective source is a geometry)

			Buffer<Effect>			collision_effects;	//!< Effects to occur on collision (if Physical or Item) or if inside geometrical radius (if Holographic)
			
									ObjectInstance():Entity("Object",Entity::NoOrder),type(Physical),spawn_count(0),spawned(0),appear_delay(0),respawn_delay(1),current_delay(0)
									{}
									ObjectInstance(Composition*composition):Entity("Object",Entity::NoOrder,composition),type(Physical),spawn_count(0),spawned(0),appear_delay(0),respawn_delay(1),current_delay(0)
									{}
									ObjectInstance(ObjectInstance&&other):Entity(other),type(other.type),source_name(other.source_name),spawn_count(other.spawn_count),spawned(other.spawned),appear_delay(other.appear_delay),respawn_delay(other.respawn_delay),current_delay(other.current_delay),instance(other.instance),collision_effects(other.collision_effects)
									{
										ASSERT__(other.collision_effects.isEmpty());
									}
	virtual	bool					cast(const TVec3<>&from, const TVec3<>&direction, float&distance)	const; 	//!< Virtual vector cast. Determines the closest ray intersection. \param from 3 component base vector \param direction Normalized ray direction vector \param distance in/out distance value along the specified ray axis of the closest intersection \return true if a closer intersection was detected, false otherwise
			void					parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context);	//!< Parses the local instance configuration from the specified XML node

			void					adoptData(ObjectInstance&other)
									{
										Entity::adoptData(other);
										type = other.type;
										source_name.swap(other.source_name);
										spawn_count = other.spawn_count;
										spawned = other.spawned; 
										appear_delay = other.appear_delay;
										respawn_delay = other.respawn_delay;
										current_delay = other.current_delay;
										instance.swap(other.instance);
										collision_effects.swap(other.collision_effects);
									}
	};
	
	
	/*!
		\brief Geometrical shape entity
		
		A Shape instance describes a 2 dimensional rectangle, ellipse, or polygon (i.e. as a checkpoint shape, a moving entity has to pass)
	
	*/
	class Shape:public Entity
	{
	public:
			enum Type	//! Base form type enumeration
			{
				Ellipse,	//!< Elliptical shape type. The actual form is calculated from both primary (horizontal) and secondary (vertical) diameters.
				Circle,		//!< Circular shape type. The actual form is calculated from just the primary diameter
				Rectangle,	//!< Rectangular shape type. The actual form is calculated from both primary (width) and secondary (height) diameters.
				Square,		//!< Square shape type. The actual form is calculated from just the primary diameter
				Polygon,	//!< Custom polygon shape type. The actual form is calculated from the specified polygon_points
				Types		//!< Number of shape types
			};
			Type					type;				//!< Base form type
			float					primary_diameter,	//!< Form width/double radius/horizontal stretch
									secondary_diameter;	//!< Form height/vertical stretch
			Buffer<Vec2<> >			polygon_points;		//!< Polygon border points. These points will be auto generated to some approximation if type is not Polygon.
			
			Mesh<TDef<float> >		triangulation;		//!< Triangulation of the local polygon
			
									Shape():Entity("Shape",Entity::Default),type(Circle),primary_diameter(2),secondary_diameter(2)
									{}
									Shape(Composition*composition):Entity("Shape",Entity::Default,composition),type(Circle),primary_diameter(2),secondary_diameter(2)
									{}
									Shape(Shape&&other):Entity(other),type(other.type),primary_diameter(other.primary_diameter),secondary_diameter(other.secondary_diameter),polygon_points(other.polygon_points),triangulation(other.triangulation)
									{
										ASSERT__(other.polygon_points.isEmpty());	//must have been moved
									}
	virtual							~Shape();
	virtual	void					parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context) override;	//!< Parses the local shape configuration from the specified XML node
			void					updateShape();			//!< Updates \b polygon_points (if type is not Polygon) and \b triangulation
			void					updateTriangulation();	//!< Updates just \b triangulation
	virtual	bool					cast(const TVec3<>&from, const TVec3<>&direction, float&distance)	const	override; 	//!< Virtual vector cast. Determines the closest ray intersection. \param from 3 component base vector \param direction Normalized ray direction vector \param distance in/out distance value along the specified ray axis of the closest intersection \return true if a closer intersection was detected, false otherwise

			void					adoptData(Shape&other)
									{
										Entity::adoptData(other);
										type = other.type;
										primary_diameter = other.primary_diameter;
										secondary_diameter = other.secondary_diameter;
										polygon_points.swap(other.polygon_points);
										triangulation.swap(other.triangulation);
									}
	};


	class HexGrid:public Entity
	{
	public:
			struct Cell
			{
				bool				has_floor,
									geometry_orthogonal;
				Instance			geometry_instance;
				float				scale;
				Box<>				object_bounds;
				//TreeLib::Tree		tree;


									Cell():has_floor(false),/*tree(NULL),*/geometry_orthogonal(true),scale(1.f)
									{}
			};
			Array2D<Cell>			grid;
			int2					grid_offset;
			float					cell_height;
			CGS::Geometry<>			hull;		//!< Geometry that describes the hull of all floor cells

									HexGrid():Entity("HexGrid",Entity::Default),grid(8,8),grid_offset(-4,-4),cell_height(1.f)
									{}
									HexGrid(Composition*composition):Entity("HexGrid",Entity::Default),grid(8,8),grid_offset(-4,-4),cell_height(1.f)
									{}
									HexGrid(HexGrid&&other):Entity(other),grid(other.grid),grid_offset(other.grid_offset),cell_height(other.cell_height),hull(other.hull)
									{}

	virtual	void					parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context) override;	//!< Parses the local shape configuration from the specified XML node


			void					adoptData(HexGrid&other)
									{
										Entity::adoptData(other);
										grid.adoptData(other.grid);
										grid_offset = other.grid_offset;
										cell_height = other.cell_height;
										hull.adoptData(other.hull);
									}

		template <typename T>
			void					getMatrixOfCell(TMatrix4<T>&target, index_t cell_x, index_t cell_y)	const
									{
										const float cell_width = cell_height / sin(M_PI/3.f),
													cell_indent = cell_width * 3.f/4.f;

										float2	coords(cell_indent * cell_x + cell_width*0.5f,cell_height*cell_y + (cell_x%2)*cell_height*0.5f + cell_height*1.5f);

										coords.x += cell_indent * grid_offset.x;
										coords.y += cell_height*(grid_offset.y-1);

										coords = -coords;

										float3	coords3(coords.x,0,coords.y);
										Mat::transform(system,coords3,target.w.xyz);
										Mat::copyOrientation(system,target);
									}
			void					getCornersOfCell(TVec2<> out_corners[6], index_t cell_x, index_t cell_y)	const;

			void					updateConstraints();
			void					updateHull();

			//void					putTrees();
	};


	
	/*!
		\brief Race track abstract entity
		
		A race track combined a number of start positions and checkpoints.
		The track is formed by layers of checkpoint shapes, each containing all shapes of the same order. The track must provide at least one shape of each order to the found maximum order.
		A moving entitiy is supposed to pass at least one checkpoint shape of each order to finish a lap.
	*/
	class Track:public Entity
	{
	public:
			bool					loop;		//!< Set true if the track allows multiple laps
			Buffer<StartPosition,0>	start_positions;	//!< Start positions. Each start position does provide a order value with 0 being pool-position
			Buffer<Shape,0,Adopt>	checkpoints;		//!< Checkpoint shapes
			
									Track():Entity("Track",Entity::MetaEntity),loop(false)
									{}
									Track(Composition*composition):Entity("Track",Entity::MetaEntity,composition),loop(false)
									{}
									Track(Track&&other):Entity(other),loop(other.loop),start_positions(other.start_positions),checkpoints(other.checkpoints)
									{
										ASSERT__(other.checkpoints.isEmpty());
									}
			void					parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context);	//!< Parses the local track configuration from the specified XML node
			void					adoptData(Track&other)
									{
										Entity::adoptData(other);
										loop = other.loop;
										start_positions.swap(other.start_positions);
										checkpoints.swap(other.checkpoints);
									}
	
	};

	class XTrack;

	class XNode:public Entity, public CGS::TrackNode
	{
	public:
			index_t					id;
			float					width,
									height;
			enum function_t
			{
				NoFunction,
				CheckPoint,
				Goal
			};
			function_t				function;


									XNode():Entity("XNode",Entity::Directional),id(InvalidIndex),width(1),height(1),function(NoFunction)
									{}
									XNode(Composition*composition):Entity("XNode",Entity::Directional,composition),id(InvalidIndex),width(1),height(1),function(NoFunction)
									{}
			void					adoptData(XNode&other)
									{
										Entity::adoptData(other);
										CGS::TrackNode::swap(other);
										id = other.id;
										width = other.width;
										height = other.height;
										function = other.function;
									}
			void					parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context);	//!< Parses the local track configuration from the specified XML node
	};


	class XSegment:public Entity, public CGS::TrackSegment
	{
	private:
			void					parseConnector(const XML::Node*node, BYTE index);
	public:
			index_t					node_id[2];
			CGS::TNodeSegmentSlot	node_slot[2];
			CGS::Geometry<>			bent_geometry;

			enum function_t
			{
				NoFunction,
				StartPosition,
				RevertedStartPosition
			};
			function_t				function;


									XSegment():Entity("XSegment",Entity::MetaEntity),function(NoFunction)
									{
										node_id[0] = InvalidIndex;
										node_id[1] = InvalidIndex;
									}
									XSegment(Composition*composition):Entity("XSegment",Entity::MetaEntity,composition),function(NoFunction)
									{
										node_id[0] = InvalidIndex;
										node_id[1] = InvalidIndex;
									}
			void					adoptData(XSegment&other)
									{
										((CGS::TrackSegment&)*this) = (CGS::TrackSegment&)other;
										Entity::adoptData(other);
										node_id[0] = other.node_id[0];
										node_id[1] = other.node_id[1];
										node_slot[0] = other.node_slot[0];
										node_slot[1] = other.node_slot[1];
										function = other.function;
										bent_geometry.adoptData(other.bent_geometry);
									}
			void					parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context);	//!< Parses the local track configuration from the specified XML node
			void					link(XTrack&parent);
	};

	class XTrack:public Entity
	{
	public:
			Buffer<XNode,0,Adopt>	nodes;
			Buffer<XSegment,0,Adopt>segments;

			IndexTable<index_t>		node_table;
			Buffer<index_t,0>		start_nodes;
			index_t					goal_node;


									XTrack():Entity("XTrack",Entity::MetaEntity),goal_node(InvalidIndex)
									{}
									XTrack(Composition*composition):Entity("XTrack",Entity::MetaEntity,composition),goal_node(InvalidIndex)
									{}
			void					adoptData(XTrack&other)
									{
										nodes.swap(other.nodes);
										segments.swap(other.segments);
										node_table.swap(other.node_table);
										start_nodes.swap(other.start_nodes);
										goal_node = other.goal_node;
										Entity::adoptData(other);
										for (index_t i = 0; i < segments.count(); i++)
											segments[i].link(*this);
									}
			void					parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context);	//!< Parses the local track configuration from the specified XML node
	};

	
	class SplineTrack;

	

	class SplineTrack:public Entity
	{
	public:
			SurfaceNetwork::Compact	network;

			struct StartSegment
			{
				index_t				segment_id;
				bool				forward;

									StartSegment(index_t segment_id_, bool forward_):segment_id(segment_id_),forward(forward_)
									{}
			};
			struct SegmentLOD
			{
				index_t					triangle_index_offset,
										quad_index_offset,
										quad_index_count;
			};
			ALIGN4
				struct Vertex
				{
					ALIGN4	float3		position,
										normal,
										tangent;
					ALIGN4	float2		texcoords;
				};


			Buffer<StartSegment,0>	start_segments;
			index_t					goal_node;
			Buffer<Vertex,0>		vertex_buffer;	//!< Vertex data needed by all segments of the local structure
			Buffer<UINT32,0>		index_buffer;	//!< Index data needed by all segments of the local structure. Each all triangles of one LOD are grouped.

			Array<Array<SegmentLOD> >	segment_lods;	//!< Primary order in this list is segments, secondary is LODs. Each three LODs are used per actual LOD, to adapt to neighboring lower level of detail
			Array<float4>			segment_bounding_sphere;

			ObjectMath::Mesh<CGS::SubGeometryA<>::PhDef>	ph_hull;

									SplineTrack():Entity("SplineTrack",Entity::MetaEntity),goal_node(InvalidIndex)
									{}
									SplineTrack(Composition*composition):Entity("SplineTrack",Entity::MetaEntity,composition),goal_node(InvalidIndex)
									{}
			void					adoptData(SplineTrack&other)
									{
										network.adoptData(other.network);
										start_segments.adoptData(other.start_segments);
										goal_node = other.goal_node;
										Entity::adoptData(other);
									}

			bool					findClosestSegment(const TVec3<>&reference_position, index_t&out_index, TVec2<>&out_factor, float&out_distance, SurfaceDescription::InterpolatedSlice&out_slice)	const;

			void					parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context);	//!< Parses the local track configuration from the specified XML node


	//semi-public:
			void					recompileGeometry(float scale);

	private:
	static	void					parseConnector(const XML::Node&xnode, SurfaceNetwork::Segment::Connector&connector);

	};


	
	/*!
		\brief Volume entity
		
		A volume may be either a sphere or a cylinder with effects occuring while a moving entity resides within.
	*/
	class Volume:public Entity
	{
	public:
			enum Type	//! Volume type enumeration
			{
				Sphere,		//!< Sphere type
				Cylinder,	//!< Cylinder type
				Types		//!< Number of types
			};
			Type					type;	//!< Volume type
			float					height,	//!< Geometrical height (cylinder only)
									radius;	//!< Geometrical radius
			
			Buffer<Effect>			effects;	//!< Effects that occur while a moving entity is within the volume
			
									Volume():Entity("Volume",Entity::Directional),type(Sphere),height(1),radius(1)
									{}
									Volume(Composition*composition):Entity("Volume",Entity::Directional,composition),type(Sphere),height(1),radius(1)
									{}
									Volume(Volume&&other):Entity(other),type(other.type),height(other.height),radius(other.radius),effects(other.effects)
									{
										ASSERT__(other.effects.isEmpty());
									}
			void					parse(const XML::Node&node,float scale, const FileSystem::Folder&loading_context);	//!< Parses the local volume configuration from the specified XML node
			
	virtual	bool					cast(const TVec3<>&from, const TVec3<>&direction, float&distance)	const; 	//!< Virtual vector cast. Determines the closest ray intersection. \param from 3 component base vector \param direction Normalized ray direction vector \param distance in/out distance value along the specified ray axis of the closest intersection \return true if a closer intersection was detected, false otherwise

			void					adoptData(Volume&other)
									{
										Entity::adoptData(other);
										type = other.type;
										height = other.height;
										radius = other.radius;
										effects.swap(other.effects);
									}
	};
	

	/*!
		\brief Central compositon container
	
		Create a derivative composition class to overload the entity constructors.
	*/
	class Composition
	{
	private:
			CFSFile						load_file;
			XML::Container 				container;	//used to load files
			
			LoadContext					void_context,
										*context;
			
	public:
			Instance					loadGeometry(const String&filename, float scale, const FileSystem::Folder&load_context, Box<>&constraints);
			CGS::Tile*					loadGeometry(const String&filename, float scale, const FileSystem::Folder&load_context);
	
			Composition					*parent;	//!< Pointer to a respective parent Composition instance or NULL
	
			TInfo						info;			//!< Abstract information structure
			TEnvironment				environment;	//!< Environment information (lights, gravity, air density)
			Buffer<Track,0,Adopt>		tracks;			//!< Implemented tracks (if any)
			Buffer<XTrack,0,Adopt>		xtracks;		//!< Implemented road tracks (if any)
			Buffer<SplineTrack,0,Adopt>	spline_tracks;	//!< Implemented pure spline road tracks (if any)
			Buffer<HexGrid,0,Adopt>		hex_grids;
			HashContainer<CGS::Tile>	geometries;			//!< Imported resources (if any)
			Buffer<ObjectInstance,0,Adopt>	objects;		//!< Instantiated resource objects (if any)
			Buffer<Volume,0,Adopt>		effects;		//!< Volumetric effects (if any)

			TextureResource				texture_resource;		//!< Texture resource for geometry loading
			
			
			StringList					warnings;		//!< Warnings generated during the last loadFromFile() call
			bool						has_data;		//!< True if the local composition features any kind of data
			
			
										Composition(Composition*parent=NULL);
	virtual								~Composition()	{}
	virtual	void						clear();			//!< Erases all local data
			bool						loadFromFile(const String&filename, float default_scale = 1.0f, LoadContext*context=NULL);	//!< Loads the local map content from the specified file \param filename File to load from \param context Loading context to report to
	virtual	bool						loadFromFile(const FileSystem::File*file, float default_scale = 1.0f, LoadContext*context=NULL);		//!< Virtual loading function
	};
}



#endif
