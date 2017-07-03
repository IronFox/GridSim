#ifndef cgs_track_segmentH
#define cgs_track_segmentH

#include "cgs.h"
#include "../io/log_stream.h"
#include "coordinates.h"

namespace DeltaWorks
{

	namespace CGS
	{
		class Tile
		{
		public:
				CGS::Geometry<>			geometry;	//!< Loaded geometry (may be empty, but usually isn't)
				M::Box<>					dim;		//!< Dimension of the loaded geometry
				PathString				filename;	//!< Filename that this geometry was originally loaded from
				index_t					index;		//!< Linearized index of this tile.
			
	
				bool					loadFromFile(const PathString&filename);
		};



		/**
			@brief Base track node
		*/
		struct TTrackNode
		{
				Composite::Coordinates	coordinates;	//!< Node coordinates
				M::TVec2<>					scale,			//!< Node scale: first component is width scale, second component height scale
										scale_direction;	//!< Scale direction. May be any length
				M::TVec3<>					up,				//!< Normalized node up axis
										direction;		//!< Normalized directional axis
		};
	
		/**
			@brief Node segment slot
		*/
		struct TNodeSegmentSlot
		{
				bool					is_outbound,	//!< True if the specified slot is part of the outbound connector of the node, false otherwise
										flipped;		//!< True if the specified slot axis should be flipped
				index_t					slot_index;		//!< Index of the used stub slot, 0 if the specified connector does not feature a stub, InvalidIndex, if this slot is invalid
		};
	
		class TrackNode;
		class TrackSegment;
	
	
		/**
			@brief Basic node connector instance
		*/
		class BaseTrackConnector
		{
		public:
				TrackSegment				*segment;		//!< Pointer to the linked segment
				bool						flipped;
	
											BaseTrackConnector():segment(NULL),flipped(false)
											{}
		};
	
		/**
			@brief Node connector instance
		
			Each track node provides two connectors, an inbound and an outbound connector.
		*/
		class TrackConnector:public BaseTrackConnector
		{
		public:
				Ctr::Array<TTrackNode>			nodes;			//!< Override connector nodes retrieved from the local stub. If non empty then segment must be NULL and stub non NULL.
				Ctr::Array<BaseTrackConnector>	connectors;		//!< Must be of the same size as nodes. Lists one base track connector per node.
				Tile						*stub;			//!< A stub is a geometry that is placed before or after the governing track node. The geometry is not altered and treated as an instance. Attaching a stub enforces the governing node's scale to be {1,1} and segment to be NULL.
				std::shared_ptr<CGS::StaticInstance<> >	instance;		//!< Stub instance. Valid only if @a stub is not NULL
			
				const bool					is_outbound;	//!< True if this is an outbound connector
				TrackNode					*const	parent;
						
											TrackConnector(TrackNode*parent_,bool outbound):stub(NULL),is_outbound(outbound),parent(parent_)
											{}
		virtual								~TrackConnector()
											{}
			
				void						update(const M::TMatrix4<>&parent_system, bool final);	//!< Repositions connectors and instance if appropriate
				bool						isValid(String*error_out=NULL)	const;	//!< Checks if the local connector is valid

				void						swap(TrackConnector&other)
											{
												nodes.swap(other.nodes);
												connectors.swap(other.connectors);
												swp(stub,other.stub);
												instance.swap(other.instance);
												swp(BaseTrackConnector::segment,other.BaseTrackConnector::segment);
												swp(BaseTrackConnector::flipped,other.BaseTrackConnector::flipped);
											}
			
				/**
					@brief Configures the local connector to function as a stub

					If one or more segments are currently connected then their respective connector link is unset and the onDisconnectFrom() method invoked for each.
					The local connector array is resized and flushed in accordance to the linked stub tile (if any). The @a connected array is resized and filled with NULL pointers.
					@a segment is set to NULL.
					The tile is rotated by 180 degrees if this is an outbound connector.
				
					@param stub Tile to set as stub. May be NULL, which then behaves identical to unset()
					@param preserve_where_possible Causes the method to preserve the current connector configuration as far as possible
				*/
				void						setStub(Tile*stub, bool preserve_where_possible=false);
			
				/**
					@brief Configures the local connection to no longer function as a stub
				*/
				void						unsetStub(bool preserve_where_possible=false);
			
				/**
					@brief Configures the local connector to function as a segment connector
				
					If one or more segments are currently connected then their respective connector link is unset and the onDisconnectFrom() method invoked for each.
					@a connectors and @a connected are cleared.
					@a segment is set to the specified segment, the respective segment's connector link is set to the this, and its onConnectTo() method executed once.
					The method fails if the specified segment is already connected on the required slot.
				
					@param segment Segment to set. May be NULL, which then behaves identical to unset()
					@param flip Set true if the local connector should be flipped
				*/
				bool						setSegment(TrackSegment*segment, bool flip);
				bool						setSegment(TrackSegment*segment, bool end, bool flip);
				bool						setStubSegment(index_t index, TrackSegment*segment, bool flip);
			
				/**
					@brief Dynamically chooses between setSegment(TrackSegment*segment, bool flip) and setStubSegment(index_t index, TrackSegment*segment, bool flip)
				
					@param slot_index Index of the stub slot to use. Only used if a stub exists, ignored otherwise
					@param segment Segment to link
					@param flip Set true to flip the connector direction
				*/
				bool						setSegment(index_t slot_index, TrackSegment*segment, bool flip);

				/**
					@brief Identifies the slot, that the specified segment is currently connected to

					If a stub exists, then the result may be in the range [0,connectors.count()-1], or InvalidIndex, if the specified segment could not be found.
					If no stub exists, then the result can only be 0 or InvalidIndex, depending on whether the local connector is connected to the specified stub

				*/
				bool						getIndexOfSegment(const TrackSegment*segment, TNodeSegmentSlot&slot_out)	const;
				void						requireIndexOfSegment(const TrackSegment*segment, TNodeSegmentSlot&slot_out)	const;
			
				void						unset(bool preserve_connections=false);	//!< Clears the local connector @param preserve_connections Causes the method to preserve connectors and connection links
				void						disconnect(TrackSegment*, bool cascade_event);	//!< Clears any connection to the specified track segment			
		};
	


		class TrackNode:public TTrackNode
		{
		public:
				TrackConnector				inbound,
											outbound;
									
											TrackNode():inbound(this,false),outbound(this,true)
											{
												M::Vec::set(scale,1);
												M::Vec::def(up,0,1,0);
												M::Vec::def(direction,1,0,0);
												M::Vec::clear(scale_direction);
											}
		virtual								~TrackNode();
				void						swap(TrackNode&other)
											{
												swp((TTrackNode&)*this,(TTrackNode&)other);
												inbound.swap(other.inbound);
												outbound.swap(other.outbound);
											}
				void						makeSystem(M::TMatrix4<>& system_out)	const;
				void						update(bool final);
				void						update(const M::TMatrix4<>&precompiled_matrix, bool final);
				/**
					@brief Attempts to link the specified segment to the best free slot
				
					The method fails if all slots are taken
				
					@param reference Opposite node position (for direction reference)
					@param seg Track segment to link
					@param end Set true if the end point of the segment should be linked, false if the start point should be linked
					@param sector_size Sector size to apply
					@return true on success, false otherwise
				*/
				bool						setClosestSegment(const Composite::Coordinates&reference, TrackSegment*seg, bool end, float sector_size);
				/**
					@brief Attempts to locate the best matching free segment slot
				
					@param reference Opposite node position (for direction reference)
					@param sector_size Sector size to apply
					@param end_node True, if an end-point connector for a new segment is sought, false otherwise.
					@param slot [out] Resulting slot. Undefined if the method returns false
					@return true on success, false otherwise
				*/
				bool						getClosestFreeSlot(const Composite::Coordinates&reference, float sector_size, TNodeSegmentSlot&slot);
				void						disconnect(TrackSegment*, bool cascade_event);
				bool						setSegment(bool outbound, index_t slot_index, TrackSegment*segment, bool flip);
				/**
					@brief Variant of setSegment()
				*/
				bool						setSegment(const TNodeSegmentSlot&slot, TrackSegment*segment);
		};

		class TrackSegment
		{
		public:
				struct TEndPoint:public TTrackNode
				{
						M::TVec3<>					position_control;	//!< NURBS control point
						M::TVec2<>					scale_control;		//!< NURBS control point
				};
	
				/**
					@brief Support structure for bending operations
				
					TFrame defines the position, scale and orientation as resolved by interpolate
				*/
				struct TFrame
				{
						M::TVec3<>					position;
						M::TVec2<>					scale;		//first component is width scale, second component height scale
						M::TMatrix3<>				system;
				};
			
			
			
				TEndPoint						end_point[2];		//!< Spline end point configuration as retrieved from @b connector
				TrackConnector					*connector[2];		//!< End point connector pointer (may be NULL if not connected)
				bool							repetitive;			//!< If true then the geometry will be repeated to approximately preserve its proportions. This works better the longer the track segment in relation to the length of the geometry
				Tile							*tile;				//!< Custom link to an attached geometry
			
				bool							changed;			//!< Set true if one or both of the end points have been altered
			
												TrackSegment():repetitive(true),tile(NULL),changed(false)
												{
													M::Vec::clear(M::Vec::ref2(connector));
												}
		virtual									~TrackSegment();
	
												/**
													@brief Resolves a frame along the local spline
												
													@a begin and @b end must be in the same sector space for this method to operate properly.
												
													@param fc Curve position factor in the range 0 (=beginning) to 1 (=end)
													@param out_frame [out] TFrame structure to hold the resolved spline point, scale, and orientation
												*/
				void							interpolate(float fc, TFrame&out_frame)	const;
			
				/**
					@brief Positions the begin and end NURBS control vectors based on the end point positions and directions.
				
					updateControls() neither checks nor sets/unsets the @b changed flag.
					end is moved so that it is located within the same sector space as begin for flawless spline bending.

					@param control_factor0 Relative control distance for the beginning control point in multiples of the absolute distance between the two end points. Should be in the range [0, 0.5)
					@param control_factor1 Like control_factor0 for the end control points
				*/
				void							updateControls(float sector_size, float control_factor0=0.39, float control_factor1=0.39);
				void							bend(const Tile&source, CGS::Geometry<>&target)	const;	//!< Bends a CGS geometry according to the local segment configuration


			
				void							smartConnect(TrackNode&node0, TrackNode&node1, float sector_size);	//!< Attempts to find the ideal connection between the two specified track nodes including stub connectors
				void							connect(TrackConnector&from, TrackConnector&to,float sector_size);	//!< Attempts to set the local connection to the specified connectors. Any already connected nodes are disconnected. Connectors are flipped as appropriate
				void							connectStub(TrackConnector&from, index_t stub_connector, TrackConnector&to,float sector_size);	//!< Similar to the above but attempts to connect to the specified @a from stub connector instead
				void							connectStub(TrackConnector&from, TrackConnector&to, index_t stub_connector,float sector_size);	//!< Similar to the above but attempts to connect to the specified @a to stub connector instead
				void							connectStubs(TrackConnector&from, index_t from_stub_connector, TrackConnector&to, index_t to_stub_connector,float sector_size);	//!< Similar to the above but attempts to connect to the specified @a from and @a to stub connector instead
			
				void							disconnectFrom(TrackConnector*);
				void							disconnect();
	
		inline	BYTE							GetIndexOf(TrackConnector*connector_)	const
												{
													if (connector[0] == connector_)
														return 0;
													if (connector[1] == connector_)
														return 1;
													FATAL__("Requesting index of connector "+String(connector_)+" wich is not linked to local segment");
													return 0;
												}
			
		virtual	void							onDisconnectFrom(TrackConnector*);
		virtual	void							onConnectTo(TrackConnector*)		{};
		virtual	void							OnChanged(bool final)				{};

		private:
			template <typename Def>
				void							bendHull(const Mesh<Def>&source_hull, Mesh<Def>&target_hull, count_t repeat, float lower_z_boundary, float zrange, float segment_length, float stretch,const M::TMatrix4<CGS::StdDef::SystemType>&path, const M::TVec3<>&center) const
												{
													count_t	vframe_length = source_hull.vertex_field.length();
													ASSERT1__(source_hull.valid(),source_hull.errorStr());	//TRANSITORY
				
													target_hull.vertex_field.SetSize(vframe_length*repeat);
													//const Mesh<Def>::Vertex	*vfrom = source.vertex_field;


													Concurrency::parallel_for(index_t(0),vframe_length,[this,center,&path,&source_hull,repeat,vframe_length,&target_hull,segment_length,zrange,stretch,lower_z_boundary](index_t k)
													{
													//for (index_t k = 0; k < vframe_length; k++)
													//{
														const typename Mesh<Def>::Vertex	*vfrom = source_hull.vertex_field + k;
														M::TVec3<> p,n;
														M::Mat::transform(path,vfrom->position,p);
														M::Mat::rotate(path,vfrom->normal,n);
					
														for (index_t l = 0; l < repeat; l++)
														{
															typename Mesh<Def>::Vertex	&vto = target_hull.vertex_field[l*vframe_length+k];
															float z = p.z;
															float rel = 1.0f-((z-lower_z_boundary)/zrange+segment_length*l);
															TFrame vec;
															interpolate(rel,vec);
															M::Vec::mad(vec.position,vec.system.x,p.x*vec.scale.x, vto.position);
															M::Vec::mad(vto.position,vec.system.y,p.y*vec.scale.y);
															M::Vec::sub(vto.position,center);
						
															M::TVec3<> n2,n3;
															M::Vec::def(n2,n.x/vec.scale.x,n.y/vec.scale.y,n.z/stretch);
															M::Mat::Mult(vec.system,n2,vto.normal);
															M::Vec::normalize0(vto.normal);
														}
														//vfrom++;
													});
													//ASSERT_CONCLUSION(obj.vertex_field,vfrom);

													target_hull.triangle_field.SetSize(source_hull.triangle_field.length()*repeat);
													const typename Mesh<Def>::Triangle	*tfrom = source_hull.triangle_field.pointer();
													for (index_t k = 0; k < source_hull.triangle_field.length(); k++)
													{
														for (index_t l = 0; l < repeat; l++)
														{
															typename Mesh<Def>::Triangle	&tto = target_hull.triangle_field[l*source_hull.triangle_field.length()+k];
						
															tto.v0 = target_hull.vertex_field+(index_t)(tfrom->v0-source_hull.vertex_field)+l*vframe_length;
															tto.v1 = target_hull.vertex_field+(index_t)(tfrom->v1-source_hull.vertex_field)+l*vframe_length;
															tto.v2 = target_hull.vertex_field+(index_t)(tfrom->v2-source_hull.vertex_field)+l*vframe_length;
														}
														tfrom++;
													}
													ASSERT_CONCLUSION(source_hull.triangle_field,tfrom);

													target_hull.quad_field.SetSize(source_hull.quad_field.length()*repeat);
													const typename Mesh<Def>::Quad	*qfrom = source_hull.quad_field.pointer();
													for (index_t k = 0; k < source_hull.quad_field.length(); k++)
													{
														for (index_t l = 0; l < repeat; l++)
														{
															typename Mesh<Def>::Quad	&qto = target_hull.quad_field[l*source_hull.quad_field.length()+k];
						
															qto.v0 = target_hull.vertex_field+(index_t)(qfrom->v0-source_hull.vertex_field)+l*vframe_length;
															qto.v1 = target_hull.vertex_field+(index_t)(qfrom->v1-source_hull.vertex_field)+l*vframe_length;
															qto.v2 = target_hull.vertex_field+(index_t)(qfrom->v2-source_hull.vertex_field)+l*vframe_length;
															qto.v3 = target_hull.vertex_field+(index_t)(qfrom->v3-source_hull.vertex_field)+l*vframe_length;
														}
														qfrom++;
													}
													ASSERT_CONCLUSION(source_hull.quad_field,qfrom);
					
													target_hull.edge_field.SetSize(source_hull.edge_field.length()*repeat);
													const typename Mesh<Def>::Edge	*efrom = source_hull.edge_field.pointer();
													for (index_t k = 0; k < source_hull.edge_field.length(); k++)
													{
														for (index_t l = 0; l < repeat; l++)
														{
															typename Mesh<Def>::Edge	&eto = target_hull.edge_field[l*source_hull.edge_field.length()+k];
						
															eto.v0 = target_hull.vertex_field+(index_t)(efrom->v0-source_hull.vertex_field)+l*vframe_length;
															eto.v1 = target_hull.vertex_field+(index_t)(efrom->v1-source_hull.vertex_field)+l*vframe_length;
						
															for (BYTE m = 0; m < 2; m++)
																if (efrom->n[m])
																	if (efrom->n[m].is_quad)
																		eto.n[m].set(target_hull.quad_field+(index_t)(efrom->n[m].quad-source_hull.quad_field)+l*source_hull.quad_field.length());
																	else
																		eto.n[m].set(target_hull.triangle_field+(index_t)(efrom->n[m].triangle-source_hull.triangle_field)+l*source_hull.triangle_field.length());
														}
														efrom++;
													}
													ASSERT_CONCLUSION(source_hull.edge_field,efrom);
				
													ASSERT1__(target_hull.valid(),target_hull.errorStr());
				
												}
		};


	}
}



#endif
