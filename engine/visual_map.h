#ifndef engine_visual_mapH
#define engine_visual_mapH

#include "renderer/renderer.h"
#include "scenery.h"
#include "../structure/map.h"
#include "../image/img_generators.h"

/******************************************************************

E:\include\engine\visual_map.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

namespace Engine
{


	template <class GL, class Def>
		class VisualMap;

	/*!
		\brief Eve map composition light

		MapLight enhances 
	*/
	template <class GL>
		class MapLight: public Map::Light
		{
		public:
				typename GL::QueryObject			query_object;	//!< Object used to query light-visibility
				GL									*renderer;		//!< Link to the used rendering interface (for light retrieval)
				Engine::Light						*light;			//!< Link to renderer light
				float								visibility,		//!< Flare visibility (0-1) animated depending on actual visibility in the scenery
													projected[2],	//!< Projected screen position
													distance;		//!< Distance between the light and the point of view
				bool								visible;		//!< True if the light is inside the view frustum


													MapLight(Map::Composition*composition, GL*renderer_):Map::Light(composition),renderer(renderer_),light(renderer_->createLight()),visibility(0),visible(false)
													{}
		virtual										~MapLight()
													{
														if (light)
															light->destroy();
														if (!query_object.isEmpty())
															renderer->discardObject(query_object);
													}
		virtual void								parse(XML::Node*node);
				void								update();	//!< Updates the local light configuration to the base Map::Light configuration
		};

	/*!
		\brief Eve map object instance (geometry or again a map)
		
		MapObject enhances the basic Map::ObjectInstance class to allow embedding of geometries into the map scenery.
	*/
	template <class GL, class Def=CGS::StdDef>
		class MapObject: public Map::ObjectInstance, public StructureEntity<Def>::Attachment
		{
		public:
				StructureEntity<>				*entity;
				GL								*renderer;
		
												MapObject(VisualMap<GL, Def>*composition);
		virtual void							parse(XML::Node*node);
				void							update();
		};

	/*!
		\brief Eve map start position (for races)

		MapStartPosition enhances Map::StartPosition to allow graphical rendering of the start position

	*/
	template <class GL, class Def=CGS::StdDef>
		class MapStartPosition: public Map::StartPosition
		{
		public:
				GL								*renderer;
				VisualMap<GL, Def>				*composition;
												MapStartPosition(VisualMap<GL, Def>*composition);
		};

	/*!
		\brief Eve map shape (for race checkpoints)

		MapShape enhances Map::Shape to allow graphical rendering of the shape

	*/
	template <class GL, class Def=CGS::StdDef>
		class MapShape: public Map::Shape
		{
		public:
				GL								*renderer;
												MapShape(VisualMap<GL, Def>*composition);
		};

	template <class GL, class Def=CGS::StdDef>
		class MapTrack: public Map::Track
		{
		public:
				GL								*renderer;
				VisualMap<GL, Def>				*composition;
		
												MapTrack(VisualMap<GL, Def>*composition);
				
				
		};
	/*!
		\brief Eve visual map composition

	*/
	template <class GL, class Def=CGS::StdDef>
		class VisualMap:public Map::Composition
		{
		protected:
				bool							flares_rendered;	//!< Set true if flares have been requested by the client application since the last frame.
				GL								*renderer;			//!< Pointer to the used renderer
				
		virtual Map::Light*			newLight();				//!< Creates a new composition light
		virtual Map::ObjectInstance*	newObject();			//!< Creates a new composition object instance
		virtual Map::StartPosition*	newStartPosition();		//!< Creates a new composition start position
		virtual Map::Shape*			newShape();				//!< Creates a new composition shape
		virtual Map::Track*			newTrack();				//!< Creates a new composition track
		//virtual Map::Volume*				newVolume();				//!< Creates a new composition volume
		virtual Map::Composition*		newComposition();		//!< Creates a new sub composition (should not invoke 'SHIELDED')
		
												VisualMap(VisualMap<GL,Def>*parent);
		public:
				struct FlareSprite	//! Flare sprite along the flare sprite line
				{
					typename GL::TextureObject	texture;	//!< Texture to use for this flare sprite. Empty sprites are not rendered
					typename GL::FloatType		width,		//!< Width of this sprite on the screen (measured in height units: 1=full screen height)
												height,		//!< Height of this sprite on the screen (1= full screen height)
												intensity,	//!< Sprite intensity (0-1) multiplied by the respective flare intensity
												position;	//!< Relative position along the flare line (0=light source, 1=screen center). Values may lie outside the [0,1] range
					bool						primary;	//!< Attribute set for sprites directly relating to the light source (i.e. at position 0)
				};
				
				MappedScenery<GL,Def>			stationary_scenery,		//!< Scenery for stationary entities
												holographic_scenery,	//!< Scenery for holographic entities
												dynamic_scenery;		//!< Scenery for moving entities

				static Signature				signature;			//!< Signature for object attachments
												
				Array<FlareSprite>				sprites;	//!< Sprite line to use for flares

												VisualMap(GL*renderer=NULL);	//!< Default constructor. \param renderer Renderer to use. Pass NULL to use the default instance of the template renderer type
		virtual									~VisualMap();
		
		virtual	bool							loadFromFile(const CFSFile*file, Map::LoadContext*context=NULL);
		virtual	void							clear();
				void							setRenderer(GL*renderer);	//!< Updates the used renderer. Do not use when the local composition is not empty
				void							setRenderer(GL&renderer);	//!< Updates the used renderer. Do not use when the local composition is not empty
				GL*								getRenderer()	const;		//!< Retrieves the current renderer
				
				void							generateCentralFlareSprite(FlareSprite&target);	//!< Generates a standard central flare sprite
				void							generateInnerGlow(FlareSprite&target, float r, float g, float b, float intensity, float size, float position);	//!< Generates an inner glow flare sprite
				void							generateOuterRing(FlareSprite&target, float r, float g, float b, float intensity, float size, float position);	//!< Generates an outer ring flare sprite
				
				void							remove(MapObject<GL,Def>*object);	//!< Removes the specified map object from the respective scenery (if embedded) \param object Object to remove. May be NULL causing the method to abort
				void							embed(MapObject<GL,Def>*object);		//!< Embeds the specified map object into the respective scenery (if not embedded) \param object Object to embed. May be NULL causing the method to abort
				
				void							respawnItems(float time_delta);		//!< Increases the Map::ObjectInstance::current_delay value of non-spawned items and spawns items exceeding their spawn or respawn delay.
		
		template <typename T>
				void							getGravity(const T position[3], const T&mass, T out_gravity[3]);	//!< Calculates the gravity affecting an object of the specified mass at the specified location \param position Central position of the object \param mass Object mass \param out_gravity Resulting gravitational force
			
				/*!
					\brief Recursivly renders the map composition using the specified camera
					
					\param camera This camera object is required to be already loaded and is only used to determine the visibility of objects
					\param lighted Set true to enable lighting where appropriate		
					\param use_opacity_query If set true render() will attempt to use pixel opacity queries for light source flare sprites (effective only if flare sprites are being rendered)
				*/
		template <typename T>
				void							render(const Aspect<T>&camera, bool lighted=true, bool use_opacity_query=true);
				
				/*!
					\brief Recursivly renders flares for loaded lights
					
					renderFlares() generates flare sprites if they don't already exist and renders a line of all sprites as specified by the current \b sprites array.
					The flare sprites will be rendered additive, meaning they will sum up with underlieing sprites or light geometry.
					This method should be called with depth test disabled.
					
					\param pixel_aspect Pixel aspect of the current rendering region
				*/
				void							renderFlares(float pixel_aspect);
				
		};
		

	/*!
		\brief Drive flame
	*/
	template <class GL, class Def=CGS::StdDef>
		class VisualDrive
		{
		protected:
		static	typename GL::TextureObject	flame_texture;		//!< Flame sprite texture	(loaded once)
		public:
				typename Def::SystemType	inner_system[16],	//!< system constructed from the accelerator's position and direction
											system[16],	//!< drive system with the flame pointing in positive Z axis (updated once per frame)
											invert[16];	//!< in-system (updated once per frame)
				typename GL::FloatType		smoke_end[3],	//!< Smoke end point. Recalculated based on its previous location and the flame's new location when calling render(). The actual smoke particles are placed along a nurbs curve beginning at the flame's hotspot.
											intensity;		//!< Flame intensity (0-1) affecting the visual width, length, and opacity. Also affects the smoke particle intensity.
				const CGS::TAccelerator<Def>&target;		//!< Original accelerator data. Color, width, and length of the flame is read fresh each frame and may be changed independently.
				CGS::SubGeometryA<Def>		&parent;		//!< Parent sub geometry that the target accelerator belongs to
		
											VisualDrive(CGS::SubGeometryA<Def>&parent, const CGS::TAccelerator<Def>&accel);	//!< Default constructor. The respective parent instance is required to tbe linked before calling the constructor
			template <typename T0, typename T1>
				void						render(const T0 up_vector[3], const T1&time_delta);	//!< This method renders the local flame. The active modelview matrix is assumed to be the identity matrix, lighting is expected to be disabled. The method renders both flame and smoke sprites. \param up_vector Vertical vector to calculate the new smoke end point (may be NULL) \param time_delta Time passed since last call in seconds
		};
		
	/*!
		\brief Simple spherical physical vehicle
		
		SimpleVehicle approximates the vehicle's physical hull with a sphere.
	*/
	template <class GL, class Def=CGS::StdDef>
		class SimpleVehicle:public ObjectEntity<Def>::Attachment
		{
		public:
				typedef typename Def::PhFloatType	Float;	//usually double
		
		private:
				StructureEntity<Def>				*structure_entity;		//!< Link to the respective uploaded geometry entity or NULL
				Vector<VisualDrive<GL,Def> >		drives;					//!< Visual drives. This list is created once when an entity is assigned. The respective coordinates are updated per frame (systems only)
				
				void								processFriction(Float vdelta[3], const Float&time_delta);
		public:
				
				struct Configuration	//! Simulation configuration of a vehicle
				{
					enum eFrictionModel	//! System used to calculate friction
					{
						NoFriction,				//!< Friction is not calculated
						ExponentialFriction,	//!< Friction is calculated using the (1-f)^t formula
						RealisticFriction		//!< Reality approximation
					};
					
					eFrictionModel					friction_model;		//!< Used friction model (RealisticFriction) by default
				
					Float							repel_threshold,	//!< Maximum terminal velocity that is consumed by a collision rather than repelled. 0 by default
													repel_intensity,	//!< Repel strength (0-1). 0.5 by default
													gravity_strength,	//!< Gravity scalar (0-1). 1 by default
													acceleration_energy_drain;	//!< Strength of energy drain by acceleration (energy_level -= effective_acceleration^2 * \b acceleration_energy_drain * time_delta). Processed during animate() or animateViaMouse(). 0 by default
													
					bool							scale_speed_by_energy,			//!< Multiply effective acceleration by current energy level (false by default). Processed during animate() or animateViaMouse()
													process_effects,				//!< Process collision effects on collision (true by default). Processed during handleStationaryCollision()
													hide_items_on_collision,		//!< Remove object from the scenery and set Map::ObjectInstance::current_delay to 0 on collision with an item (true by default). Processed during handleStationaryCollision()
													two_sided_collision_detection;	//!< Detect collisions with surfaces in both directions.
				};
				
				struct Collision	//! Physical collision report
				{
					bool							detected;		//!< True if this collision point was detected
					Float							normal[3],		//!< Absolute normal of collision point. Usually pointing towards the local vehicle
													position[3],	//!< Absolute remote point of collision.
													velocity;		//!< Terminal velocity (positive)
					SimpleVehicle<GL,Def>			*remote_vehicle;	//!< Pointer to the remote collision vehicle. Defined only if detectDynamicCollision() returned true.
					MapObject<GL,Def>				*remote_object;		//!< Pointer to the remote collision object. Defined only if detectStationaryCollision() returned true.
				};
				
				Configuration						config;			//!< Local physical configuration
				Collision							last_stationary_collision,	//!< Last encountered collision with the stationary environment. Check Collision::detected to check if a collision occured and set it to false.
													last_dynamic_collision;		//!< Last encountered collision with other vehicles. Check Collision::detected to check if a collision occured and set it to false.
				
				
				static Signature					signature;
				
				VisualMap<GL,Def>					*map;					//!< Link to the respective parent map or NULL

				Float								velocity[3],				//!< Current velocity vector (movement per second)
													rotation_velocity_x,		//!< Horizontal rotation velocity (degrees per second)
													rotation_velocity_y,		//!< Vertical rotation velocity (degrees per second)
													rotation_velocity_z,		//!< Z-axis rotation velocity (degrees per second)
													mass,					//!< Physical mass of this vehicle
													horizontal_friction,	//!< Friction along the relative x axis
													vertical_friction,		//!< Friction along the relative y axis
													directional_friction,	//!< Friction along the relative z axis
													rotation_friction,		//!< Rotation friction level (0-1). 0.95 by default
													updrift,				//!< Updrift strength
													radius,					//!< Vehicle radius
													closest[3],				//!< Position of the last detected closest point of the external scenery to the local vehicle
													closest_normal[3],		//!< Normal of the last detected closest point
													energy_level,			//!< Current vehicle energy level (if simulated)
													health_level,			//!< Current vehicle health level (if simulated)
													score;					//!< Current player/vehicle score
				VectorCamera<Float>				system;					//!< Vehicle system using the predefined functionality of a vector camera
				Float								mouse_acceleration,		//!< Mouse movement to angular degree factor. 1 by default
													mouse_smooth_time;		//!< Time interval in seconds over which mouse movement should be smoothed. 0.12 by default
				P<CGS::GeometryInstance<Def> >		instance;				//!< Pointer to a geometry instance (if any)
				unsigned							order;					//!< Unique vehicle order value. A vehicle may only collide with other vehicles of greater order thus preventing one collision from being handled twice
				static unsigned						global_order;			//!< Global order value used to initialize the local order.
			
													SimpleVehicle(const Float&x=0, const Float&y=0, const Float&z=0);
													SimpleVehicle(VisualMap<GL,Def>*map, const Float&x=0, const Float&y=0, const Float&z=0);
		
													/*!
														\brief Animates the local vehicle
													
														animateByMouse() alters the local velocity and orientation attributes and updates the instance system (if any)
														
														\param time_delta Time in seconds that passed since the last move() execution
														\param mouse_x_movement Horizontal mouse movement
														\param mouse_y_movement Vertical mouse movement
														\param z_rotation Rotation along the z-axis in degrees per second
														\param z_acceleration Acceleration along the z-axis in units per second
													*/
				void								animateByMouse(const Float&time_delta, const Float&mouse_x_movement, const Float&mouse_y_movement, const Float&z_rotation, const Float&z_acceleration);
			template <typename T>
				void								animateByMouse(const Float&time_delta, const Float&mouse_x_movement, const Float&mouse_y_movement, const Float&z_rotation, const T acceleration[3]);	//!< \overload

													/*!
														\brief Animates the local vehicle
													
														animate() alters the local velocity and orientation attributes and updates the instance system (if any)
														
														\param time_delta Time in seconds that passed since the last move() execution
														\param horizontal_rotation Horizontal rotation in degrees per second
														\param vertical_rotation Vertical rotation in degrees per second
														\param z_rotation Rotation along the z-axis in degrees per second
														\param z_acceleration Acceleration along the z-axis in units per second
													*/
				void								animate(const Float&time_delta, const Float&horizontal_rotation, const Float&vertical_rotation, const Float&z_rotation, const Float&z_acceleration);
			template <typename T>
				void								animate(const Float&time_delta, const Float&horizontal_rotation, const Float&vertical_rotation, const Float&z_rotation, const T acceleration[3]);	//!< \overload
				
				bool								detectStationaryCollision();	//!< Detects collisions with the stationary environment. Updates last_stationary_collision if a collision was detected \return true if a collision occured
				bool								detectDynamicCollision();		//!< Detects collisions with other vehicles (using the dynamic map scenery). Updates last_dynamic_collision if a collision was detected \return true if a collision occured
				
				void								handleStationaryCollision();	//!< Handles any occured stationary collisions. Use if(detectStationaryCollision()) handleStationaryCollision() or while(detectStationaryCollision()) handleStationaryCollision()
				void								handleDynamicCollision();		//!< Handles any occured dynamic collisions. Use if(detectDynamicCollision()) handleDynamicCollision() or while(detectDynamicCollision()) handleDynamicCollision()
				unsigned							adviceIterations(const Float&time_delta)	const;		//!< Estimates how many movement steps should be calculated given the present speed and time interval. The result is twice as high if double collision detection is enabled \return Adviced number of animation steps (greater or equal 1)
													/*!
														\brief Resets the local vehicle
														
														Resets all velocity attributes and positions the vehicle at the specified location.
														
														\param x Vehicle x-location
														\param y Vehicle y-location
														\param z Vehicle z-location
													*/
				void								reset(const Float&x=0, const Float&y=0, const Float&z=0);
				
													/*!
														\brief Resets the local vehicle to a start position
														
														Resets all velocity attributes and positions the vehicle at the passed start position. Also retrieves the local map pointer from the start position's composition pointer.
														The method behaves identical to the above reset() if \a position is NULL
														
														\param position Start position to position the vehicle at.
													
													*/
				void								reset(MapStartPosition<GL,Def>*position);
				void								setEntity(StructureEntity<Def>*entity);
				void								unsetEntity();
		};


}

#include "visual_map.tpl.h"

#endif
