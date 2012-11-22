#ifndef graph_meshH
#define graph_meshH

#include "../container/buffer.h"
#include "../math/vector.h"
#include "../io/xml.h"
#include "../io/texture_compression.h"
#include "cgs.h"
#include "cgs_track_segment.h"
#include "converter/obj.h"
#include "../gl/gl.h"
#include "vertex_table.h"


#define CIRCLE_CONTROL_FACTOR		0.39052429175126996746554085052687			//(4*(sqrt(2)-1)/3) / sqrt(2)
#define COMMON_CONTROL_FACTOR		0.3


inline	void	decValidIndex(index_t&index, index_t threshold)
{
	if (index >= threshold && index != InvalidIndex)
		index--;
}



class SurfaceDescription
{
public:
		struct TVertex
		{
			float3			position,
							normal,
							tangent;	//!< Vertex tangent, perpendicular to the track's direction, and the normal (pointing to the right, when looking along track)
			float2			tcoord;
			float			tx;			//!< In the range [0,1], describes the t-position from the left to the right edge of the track
		};

		struct TControl
		{
			static const count_t	NumFloats = 15;
			union
			{
				struct
				{
					TVec3<>	position,
							direction,
							up;				//!< Up axis. Must be normalized and orthogonal to @a direction
					TVec2<>	scale;
					float	angle0,
							angle1,
							texcoord0,
							texcoord1;
				};
				float		field[NumFloats];
			};
		};

		struct InterpolatedSlice : public TControl
		{
			float3			right;
			float			t,
							texcoord_y;

							InterpolatedSlice()
							{}
							InterpolatedSlice(const TControl&slice, float t_):TControl(slice),t(t_)
							{
								Vec::cross(direction,up,right);
							}

			void			makeVertex(float x, TVertex&vtx)	const;
			void			makePoint(float x, TVec3<>&pt)	const;
			void			makePoint(float x, TVec3<>&pt, TVec3<>&normal)	const;
			void			makePoint(float x, TVec3<>&pt, TVec3<>&normal, TVec3<>&tangent)	const;
			count_t			calculateSteps(float tolerance0,float tolerance1)	const;
			count_t			calculateSteps(float tolerance)	const;
			float			estimateLength()	const;
			bool			isClosedLoop()		const;
			float			texExtend()							const;

		};

		struct TConnector
		{
			TControl		state,
							slope;
		};

		typedef Buffer<UINT32,0>				IndexContainer;
		typedef Buffer<TVertex,0,Primitive>		VertexContainer;
		typedef BasicBuffer<TVertex,Primitive>	OutVertexContainer;

		VertexContainer							vertices;
		IndexContainer							triangle_indices,
												quad_indices;

		struct Edge : public IndexContainer
		{
			bool left_edge;
		};

		Buffer<Edge,0>							edges;
		Buffer<TConnector,0,Primitive>			sub_connectors;

		unsigned								texture_repetitions;


		void									swap(SurfaceDescription&other)
												{
													vertices.swap(other.vertices);
													triangle_indices.swap(other.triangle_indices);
													quad_indices.swap(other.quad_indices);
													edges.swap(other.edges);
													sub_connectors.swap(other.sub_connectors);
													swp(texture_repetitions,other.texture_repetitions);
												}
		void									compact()
												{
													vertices.compact();
													triangle_indices.compact();
													quad_indices.compact();
													edges.compact();
													sub_connectors.compact();
												}
		void									reset()
												{
													vertices.reset();
													triangle_indices.reset();
													quad_indices.reset();
													edges.reset();
													sub_connectors.reset();
													texture_repetitions = 1;
												}
		void									clear()
												{
													vertices.clear(0);
													triangle_indices.clear(0);
													quad_indices.clear(0);
													edges.clear(0);
													sub_connectors.clear(0);
													texture_repetitions = 1;
												}

		void				generateNormals();

		void				buildSegment(const SurfaceDescription::TConnector&begin, const SurfaceDescription::TConnector&end,const SurfaceDescription::TControl control_points[2], float tolerance0, float tolerance1);
		void				buildArc(const OutVertexContainer&arc_vertices, float near_distance, float far_distance, float extend_along_track);
		void				buildRails(const SurfaceDescription&source, float innerExtend, float outerExtend, float upperExtend, float lowerExtend);
		void				buildBarriers(const SurfaceDescription&source, float barrierPosition, float barrierHeight0, float barrierHeight1);

static	void				interpolate(const SurfaceDescription::TConnector&begin, const SurfaceDescription::TConnector&end,const SurfaceDescription::TControl control_points[2], float t, InterpolatedSlice&out);
static	void				buildControlPoints(const SurfaceDescription::TConnector&begin, const SurfaceDescription::TConnector&end,SurfaceDescription::TControl control_points[2], float control_factor0=COMMON_CONTROL_FACTOR, float control_factor1=COMMON_CONTROL_FACTOR);
static	void				updateSlope(const TControl&predecessor, const TControl&successor, bool smooth_direction, TConnector&slice);
static	void				updateRightSlope(const TControl&predecessor, bool smooth_direction, TConnector&slice);
static	void				updateLeftSlope(const TControl&successor, bool smooth_direction, TConnector&slice);
							/**
								@brief Generates vertices along the track arc surrounding the specified control. The number of generated vertices depends on the specified tolerance and the curvature of the local node
							*/
static	void				buildArc(OutVertexContainer&arc_out, const TControl&control, float tolerance, bool flipped);
private:
static	void				subdivide(const SurfaceDescription::TConnector&begin, const SurfaceDescription::TConnector&end,const SurfaceDescription::TControl control_points[2], float tolerance, const InterpolatedSlice&ref0, const InterpolatedSlice&ref1, float t0, float t1, Buffer<InterpolatedSlice,256,POD>&outIntersections, int force);
		
};
DECLARE__(SurfaceDescription,Swappable);
DECLARE__(SurfaceDescription::TVertex,POD);


class SurfaceNetwork
{
public:
		template <typename Adoptable>
			class Container : public IndexTable<Adoptable,AdoptStrategy>
			{
			private:
					index_t		counter;
			public:
					typedef IndexTable<Adoptable,AdoptStrategy>	Super;

					typedef typename Super::DataType	DataType;

								Container():counter(0)
								{}

					DataType&	create(index_t&index)
								{
									DataType*rs;
									while (counter == InvalidIndex || (rs = setNew(counter))==NULL)
										counter++;
									index = counter++;
									DBG_ASSERT_EQUAL__(rs,queryPointer(index));
									return *rs;
								}
			};

		struct Attachment
		{
			virtual	~Attachment()	{};
		};


		struct Node : public SurfaceDescription::TControl
		{
			Array<float>	subdivision[2];	//!< Describes the subdivision steps for outbound segments. Only contains borders between each two steps. All must be in the range (0,1), pairwise different, and ascending in order.
			Array<index_t>	segments[2];		//!< Index of the segment bound to each subdivision. This field must be one element larger than subdivision
			SurfaceDescription::TControl	slope;
			
			UINT32			lod_index;			//!< Strictly client variable that is merely provided but not used by the local structures
			index_t			node_id;			//!< Temporary variable used during compactification. Not used otherwise


			Attachment		*attachment;		//!< Custom attachment. Initialized with NULL, and moved if the container is resized, but not deleted automatically

							Node():attachment(NULL),lod_index(0),node_id(InvalidIndex)
							{
								segments[0].setSize(1);
								segments[1].setSize(1);

								segments[0].first() = InvalidIndex;
								segments[1].first() = InvalidIndex;

								angle0 = -0.25f;
								angle1 = 0.25f;
								Vec::def(direction,1,0,0);
								Vec::clear(position);
								Vec::set(scale,1);
								texcoord0 = 0;
								texcoord1 = 1;
								Vec::def(up,0,1,0);
							}

			void			adoptData(Node&other)
							{
								((SurfaceDescription::TControl&)*this) = other;
								subdivision[0].adoptData(other.subdivision[0]);
								subdivision[1].adoptData(other.subdivision[1]);
								segments[0].adoptData(other.segments[0]);
								segments[1].adoptData(other.segments[1]);

								slope = other.slope;
								lod_index = other.lod_index;
								node_id = other.node_id;

								attachment = other.attachment;
							}

			bool			available(bool outbound, index_t subdiv)	const
							{
								return subdiv < segments[outbound].count() && segments[outbound][subdiv] == InvalidIndex;
							}
			index_t			firstAvailable(bool outbound) const
							{
								for (index_t i = 0; i < segments[outbound].count(); i++)
									if (segments[outbound][i] == InvalidIndex)
										return i;
								return InvalidIndex;
							}
			bool			anyAvailable(bool outbound)	const	{return firstAvailable(outbound) != InvalidIndex;}

			bool			buildConnector(bool outbound, index_t subdiv, bool second_node, SurfaceDescription::TConnector&out_connector)	const;
			bool			buildState(bool outbound, index_t subdiv, bool second_node, SurfaceDescription::TControl&out_state)	const;

			void			buildArc(BasicBuffer<SurfaceDescription::TVertex>&arc_out, index_t lod, bool flip)	const;
			Node&			setPosition(float x,float y,float z)	{Vec::def(position,x,y,z); return *this;}
			Node&			setDirection(float x,float y,float z)	{Vec::def(direction,x,y,z); Vec::normalize(direction); return *this;}
			Node&			setUpAxis(float x, float y, float z)	{Vec::def(up,x,y,z); Vec::normalize(up); return *this;}
			Node&			setTexCoords(float x0, float x1)		{texcoord0 = x0; texcoord1 = x1; return *this;}
			Node&			setAngles(float a0, float a1)			{angle0 = a0; angle1 = a1; return *this;}
			Node&			setScale(float scale_x, float scale_y)	{Vec::def(scale,scale_x,scale_y); return *this;}
			Node&			splitEvenly(bool outbound, count_t num);
		};



		struct Segment
		{
			struct Connector
			{
				index_t						node,
											subdivision_step;	//!< Index of the specified node's subdivision
				float						control_factor;

				bool						outbound;		//!< True if this connector is mounted to the outbound side of the targeted node, false otherwise

											Connector():node(InvalidIndex),subdivision_step(0),control_factor(COMMON_CONTROL_FACTOR),outbound(false)
											{}
											Connector(index_t node_, index_t subdivision_step_, bool outbound_):node(node_),subdivision_step(subdivision_step_),control_factor(COMMON_CONTROL_FACTOR),outbound(outbound_)
											{}
			};

			SurfaceDescription::TControl	controls[2];
			Connector						connector[2];
			Array<SurfaceDescription>		compiled_surfaces;	//three per lod: 0: stitched to fit lower resolution node[0], 1: stitched to fit lower resolution node[1], 2: not stitched (in case both nodes have reduced lod, use less detailed surface)

			Attachment						*attachment;		//!< Custom attachment. Initialized with NULL, and moved if the container is resized, but not deleted automatically
			index_t							seg_id;				//!< Temporary variable used during compactification. Not used otherwise, but valid afterwards

											Segment():attachment(NULL),seg_id(InvalidIndex)
											{}

			void							adoptData(Segment&other)
											{
												controls[0] = other.controls[0];
												controls[1] = other.controls[1];
												connector[0] = other.connector[0];
												connector[1] = other.connector[1];
												compiled_surfaces.adoptData(other.compiled_surfaces);
												seg_id = other.seg_id;

												attachment = other.attachment;
											}
			BYTE							getEndIndex(index_t node_index)	const
											{
												if (connector[0].node == node_index)
													return 0;
												if (connector[1].node == node_index)
													return 1;
												FATAL__("Integrity violated");
												return 0;
											}
			float							getMinimalDistanceTo(const TVec3<>&referencePointer, count_t scan_resolution=10)	const;
		};


		struct Compacted
		{
			Array<Node>						nodes;
			Array<Segment>					segments;
			Array<bool>						node_is_flipped;

			void							adoptData(Compacted&other)
											{
												nodes.adoptData(other.nodes);
												segments.adoptData(other.segments);
												node_is_flipped.adoptData(other.node_is_flipped);
											}
		};
		typedef Compacted Compact;

		Container<Node>						nodes;
		Container<Segment>					segments;

static	count_t								num_lods;
static	float								min_tolerance;


		void								swap(SurfaceNetwork&other)
											{						
												nodes.swap(other.nodes);
												segments.swap(other.segments);
											}
		void								adoptData(SurfaceNetwork&other)
											{
												nodes.adoptData(other.nodes);
												segments.adoptData(other.segments);
											}
		
		index_t								link(const Segment::Connector&slot0, const Segment::Connector&slot1, bool update_controls, bool build);
		/**
			@brief Attempts to create an optimal connection between the two specified nodes

			@param node0 Node index to connect from
			@param node1 Node index to connect to
			@param manage_slots Set true to automatically adjust node slot count and occupation as needed. Compares distances.
			@return Index of the new connection, or InvalidIndex, if creation was impossible
		*/
		index_t								smartLink(index_t node0, index_t node1, bool manage_slots,bool update_controls, bool build);
		index_t								findSegment(const Segment::Connector&slot0, const Segment::Connector&slot1)	const;
		void								unlink(index_t segment_index);
		void								managedUnlink(index_t segment_index,bool update_controls, bool build);

		/**
			@brief Copies all local data into a more compact form with no unused memory inbetwen
		*/
		void								compact(Compacted&target);
		/**
			@brief Similar to the above but uses adoptData() were appropriate
		*/
		void								moveCompact(Compacted&target);

		void								updateSlopes(index_t node, bool update_connected_segments, bool rebuild_segments);
		void								updateSlopes(Node&node, bool update_connected_segments, bool rebuild_segments);
		void								updateControls(index_t segment, bool rebuild);
		void								updateControls(Segment&segment, bool rebuild);
		void								rebuildSegment(index_t segment);
		void								rebuildSegment(Segment&segment);
		void								updateAllSlopes(bool update_all_controls, bool rebuild_segments);

		void								verifyIntegrity()	const;

		bool								nodeIsFlipped(index_t node)	const;

static	void								even(Array<float>&subdivison_field);

static	void								compileBarrierGeometry(CGS::Geometry<>&target,const Segment&segment, float position,float height0,float height1,name64_t texture, CGS::TextureResource*resource=NULL);
static	void								compileArcGeometry(CGS::Geometry<>&target, const Node&node, float near_distance, float far_distance, float extend_along_track, name64_t texture, name64_t normal_texture, bool node_is_flipped, CGS::TextureResource*resource=NULL);
static	void								compileRailGeometry(CGS::Geometry<>&target, const Segment&segment, float innerExtend, float outerExtend, float upperExtend, float lowerExtend, name64_t texture, name64_t normal_texture, CGS::TextureResource*resource=NULL);
static	void								compileFromDescriptions(CGS::Geometry<>&target, const Array<SurfaceDescription>&lods, float shortest_edge, name64_t texture, name64_t normal_texture, CGS::TextureResource*resource =NULL);
static	void								compileFromDescriptions(CGS::Geometry<>&target, const Array<SurfaceDescription>&lods, const SurfaceDescription&ph_hull, float shortest_edge, name64_t texture, name64_t normal_texture, CGS::TextureResource*resource =NULL);

private:
		index_t								smartMakeRoom(Node&node,index_t node_index,bool outbound,const Node&opposing_node);

};




class GraphNode
{
public:
		TVec2<>					position,
								direction;
		bool					processed,
								selected;
};


class Shape
{
public:
		class Node
		{
		public:
				TVec2<>			position,
								normal;
		};
		
		Buffer<Node>			nodes;
		
		
		void					adoptData(Shape&other)
								{
									nodes.adoptData(other.nodes);
								}
};

DECLARE__(Shape,Adoptable);






class Outline
{
public:
		Buffer<Shape>			shapes;
		
		
		count_t					countNodes()	const
								{
									count_t rs = 0;
									for (index_t i = 0; i < shapes.count(); i++)
										rs += shapes[i].nodes.count();
									return rs;
								}
};



class Profile
{
public:
		class Node:public GraphNode
		{
		public:
				float		control_dist[2];
				TVec2<>		control[2];
				
				
				Node&		reset()
							{
								Vec::clear(position);
								Vec::def(direction,1,0);
								control_dist[0] = control_dist[1] = 0;
								processed = false;
								selected = false;
								return *this;
							}
				Node&		moveTo(float x, float y)
							{
								Vec::def(position,x,y);
								return *this;
							}
		};



		Buffer<Node>		nodes;

		void				swap(Profile&other)
							{
								nodes.swap(other.nodes);
							}
		void				adoptData(Profile&other)
							{
								nodes.adoptData(other.nodes);
							}
		void				clearSelection()
							{
								for (auto it = nodes.begin(); it != nodes.end(); ++it)
									it->selected = false;
								//edges cannot be selected (yet)
							}
		void				clear(bool free_memory)
							{
								if (free_memory)
								{
									nodes.clear(0);
								}
								else
								{
									nodes.reset();
								}
							}

		void				readFrom(const XML::Node*xprofile);
};

class GraphMesh;

class Graph
{
public:
		enum layout_t
		{
			Plain,
			Split,
			Ramp,

			LayoutTypes

		};

		
static	const char*			toString(layout_t);
static	bool				decode(const String&string, layout_t&);
protected:
static	void				splitHull(SurfaceDescription&desc_out);
static	float				splitIntersectionFactor(const SurfaceDescription::TVertex&v0,const SurfaceDescription::TVertex&v1);
static	UINT32				insertInterpolatedVertex(Buffer<SurfaceDescription::TVertex,0,POD>&vout,UINT32 v0,UINT32 v1,float factor);
protected:
		
		bool				defineSurface(const Profile::Node&p0,const Profile::Node&p1,index_t edge_index,float x_step, float z_step, float texture_size_x, float texture_size_y,SurfaceDescription&desc_out, layout_t layout, float simplify_x)	const;
		//bool				defineProfileLessSurface(index_t edge_index,float step,float texture_size,Buffer<TVertex>&vout,Buffer<UINT32>&iout, layout_t layout)	const;
		
		friend class GraphMesh;
public:
static	void				splitBend(SurfaceDescription::TVertex&vtx);
static	void				ramp(SurfaceDescription::TVertex&vtx);

		class Node:public GraphNode
		{
		public:
				index_t		in_edge,
							out_edge;
				float		x_coord;
			
				void		reset()
							{
								in_edge = InvalidIndex;
								out_edge = InvalidIndex;
								processed = false;
								selected = false;
								x_coord = 0;
								Vec::def(direction,1,0);
							}
		};


		class Edge
		{
		public:
				index_t		node[2];
				float		control_dist[2];
				float		x_coord[2];


				//#ifdef GRAPH_EDIT
					TVec2<>	control[2];	//needed by graph editor
				//#endif
				
				
				void 		reset()
							{
								node[0] = node[1] = InvalidIndex;
								x_coord[0] = x_coord[1] = 0;
								control_dist[0] = control_dist[1] = 1;
							}
		inline	BYTE		indexOf(index_t node_)	const
							{
								if (node_ == node[0])
									return 0;
								if (node_ == node[1])
									return 1;
								return 0xFF;
							}
		};
		



		Buffer<Node>		nodes;
		Buffer<Edge>		edges;
		

		void				swap(Graph&other)
							{
								nodes.swap(other.nodes);
								edges.swap(other.edges);
							}
		void				adoptData(Graph&other)
							{
								nodes.adoptData(other.nodes);
								edges.adoptData(other.edges);
							}

		void				clearSelection()
							{
								for (auto it = nodes.begin(); it != nodes.end(); ++it)
									it->selected = false;
								//edges cannot be selected (yet)
							}
		void				clear(bool free_memory)
							{
								if (free_memory)
								{
									nodes.clear(0);
									edges.clear(0);
								}
								else
								{
									nodes.reset();
									edges.reset();
								}
							}

		/**
			@brief Updates the edge controls of all edges

			Calling this method is only needed when using the local graph instance in some sort of editor, that needs those controls.
			Loading and building does neither write nor read to these controls.
		*/
		void				updateEdgeControls(float control_factor0=COMMON_CONTROL_FACTOR, float control_factor1=COMMON_CONTROL_FACTOR)
							{
								for (index_t i = 0; i < edges.count(); i++)
								{
									Edge&edge = edges[i];
									const Graph::Node	&n0 = nodes[edge.node[0]],
														&n1 = nodes[edge.node[1]];
									float len = Vec::distance(n0.position,n1.position);

									Vec::mad(n0.position,n0.direction,len*control_factor0*edge.control_dist[0],edge.control[0]);
									Vec::mad(n1.position,n1.direction,-len*control_factor1*edge.control_dist[1],edge.control[1]);
								}
							}

		void				loadFromFile(const String&filename,bool compactify=true);
		void				saveToFile(const String&filename)	const;

		void				writeTo(XML::Node&node)	const;
		void				readFrom(const XML::Node*xnodes, const XML::Node*xedges);
		
		
		void				verifyIntegrity(const TCodeLocation&location)	const;
		void				eraseEdge(index_t index);
		void				updateDirectionsAndXCoords();
		

};

class GraphTextureResource : public CGS::TextureResource
{
private:
		GenericHashContainer<name64_t,CGS::TextureA>	container;

public:
		void					insert(const String&filename, name64_t as_name, bool bump_map, float bump_strength = 0.01f);

virtual count_t					countEntries()
								{
									throw Program::FunctionalityNotImplemented();
								}
virtual CGS::TextureA*			entry(index_t index)
								{
									throw Program::FunctionalityNotImplemented();
								}
								
virtual CGS::TextureA*			retrieve(const name64_t&name)
								{
									return container.lookup(name);
								}
};

class GraphMesh
{
private:

public:
		Profile					visual_profile,
								physical_profile;
		Graph					visual_graph,
								physical_graph;
		bool					has_caps;

		
static	GraphTextureResource	texture_resource;


								GraphMesh():has_caps(false)
								{}

		void					adoptData(GraphMesh&other)
								{
									visual_profile.adoptData(other.visual_profile);
									visual_graph.adoptData(other.visual_graph);
									physical_profile.adoptData(other.physical_profile);
									physical_graph.adoptData(other.physical_graph);
									has_caps = other.has_caps;
								}
		void					swap(GraphMesh&other)
								{
									visual_profile.swap(other.visual_profile);
									visual_graph.swap(other.visual_graph);
									physical_profile.swap(other.physical_profile);
									physical_graph.swap(other.physical_graph);
									swp(has_caps,other.has_caps);
								}

		void					clearSelection()
								{
									visual_profile.clearSelection();
									physical_profile.clearSelection();
									visual_graph.clearSelection();
									physical_graph.clearSelection();
								}
		void					clear(bool free_memory)
								{
									physical_profile.clear(free_memory);
									visual_profile.clear(free_memory);
									physical_graph.clear(free_memory);
									visual_graph.clear(free_memory);
								}
		/**
			@brief Exports the local graph mesh definition to a GS geometry
			@param target GS geometry to write to
			@param step Sub division steps along bended edges. Straight edges use reduced sub-division
			@param texture_size Texture tile size
			@param split Set true to generate a split symmetric geometry, useful for splitting the track. Not made to be bent
			@param texture Texture name to use. Set 0 to not use any texture
			@param normal_texture Texture name to use as normal map. Set to 0 to not use any normal map
			@param glow_texture Texture to use as a glowing decal texture, that is not affected by lighting
			@param export_visual_sub_hulls Set true to export all visual hulls into to the CGS-geometry. Set false to only export physical and highest detail hull
			@return Number of detail levels created
		*/
		count_t				createGeometry(CGS::Geometry<>&target, float step, float texture_size_x, float texture_size_y, Graph::layout_t layout, name64_t texture, name64_t normal_texture, name64_t glow_texture, bool export_visual_sub_hulls)	const;


		void				loadFromFile(const String&filename,bool compactify=true);
		void				saveToFile(const String&filename)	const;


static	void				buildStub(Graph::layout_t layout, float step, SurfaceDescription&desc);

};



#endif
