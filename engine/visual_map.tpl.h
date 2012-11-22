#ifndef engine_visual_mapTplH
#define engine_visual_mapTplH


/******************************************************************

E:\include\engine\visual_map.tpl.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

namespace Engine
{

	template <class GL>
		void	MapLight<GL>::parse(XML::Node*node)
		{
			Map::Light::parse(node);
			update();
		}

	template <class GL>
		void	MapLight<GL>::update()
		{
			if (!light)
				light = renderer->createLight();
			
			switch (type)
			{
				case Omni:
					light->setType(Engine::Light::Omni);
				break;
				case Direct:
					light->setType(Engine::Light::Direct);
				break;
				case Spot:
					light->setType(Engine::Light::Spot);
				break;
			}
			
			light->setPosition(Entity::position);
			light->setAttenuation(constant_attenuation,linear_attenuation,quadratic_attenuation);
			light->setSpotExponent(spot_exponent);

			if (light->getType() != Engine::Light::Omni)
			{
				light->setDirection(direction);
			}
			light->setDiffuse(color);
			light->setAmbient(color);
			light->setSpecular(color);
			light->setSpotCutoff(spot_cutoff);
		}



		
	template <class GL, class Def>
		MapObject<GL, Def>::MapObject(VisualMap<GL, Def>*composition):Map::ObjectInstance(composition),entity(NULL),renderer(composition->getRenderer())
		{}

	template <class GL, class Def>	
		void			MapObject<GL,Def>::parse(XML::Node*node)
		{
			Map::ObjectInstance::parse(node);
			update();
		}

	template <class GL, class Def>
		void			MapObject<GL,Def>::update()
		{
			/*...*/
		}

	template <class GL, class Def>
		MapStartPosition<GL,Def>::MapStartPosition(VisualMap<GL,Def>*composition_):Map::StartPosition(composition_),renderer(composition_->getRenderer()),composition(composition_)
		{}

	template <class GL, class Def>
		MapShape<GL,Def>::MapShape(VisualMap<GL,Def>*composition):Map::Shape(composition),renderer(composition->getRenderer())
		{}
		
	template <class GL, class Def>
		MapTrack<GL,Def>::MapTrack(VisualMap<GL,Def>*composition_):Map::Track(composition_),composition(composition_),renderer(composition_->getRenderer())
		{}
		






	template <class GL, class Def>
		Map::Light*			VisualMap<GL,Def>::newLight()
		{
			if (!renderer)
			{
				setRenderer(GL::global_instance);
				if (!renderer)
					FATAL__("trying to create light without renderer");
			}
			return SHIELDED(new MapLight<GL>(this,renderer));
		}

	template <class GL, class Def>	
		Map::ObjectInstance*	VisualMap<GL,Def>::newObject()
		{
			if (!renderer)
			{
				setRenderer(GL::global_instance);
				if (!renderer)
					FATAL__("trying to create object without renderer");
			}
			return SHIELDED(new MapObject<GL,Def>(this));
		}
		
	template <class GL, class Def>	
		Map::StartPosition*		VisualMap<GL,Def>::newStartPosition()
		{
			if (!renderer)
			{
				setRenderer(GL::global_instance);
				if (!renderer)
					FATAL__("trying to create start position without renderer");
			}
			return SHIELDED(new MapStartPosition<GL,Def>(this));
		}

	template <class GL, class Def>	
		Map::Track*		VisualMap<GL,Def>::newTrack()
		{
			if (!renderer)
			{
				setRenderer(GL::global_instance);
				if (!renderer)
					FATAL__("trying to create start position without renderer");
			}
			return SHIELDED(new MapTrack<GL,Def>(this));
		}

	template <class GL, class Def>	
		Map::Shape*			VisualMap<GL,Def>::newShape()
		{
			if (!renderer)
			{
				setRenderer(GL::global_instance);
				if (!renderer)
					FATAL__("trying to create start position without renderer");
			}
			return SHIELDED(new MapShape<GL,Def>(this));
		}
		

	template <class GL, class Def>
		Map::Composition*	VisualMap<GL,Def>::newComposition()
		{
			return new VisualMap<GL,Def>(this);
		}
		

	template <class GL, class Def>	
		VisualMap<GL,Def>::VisualMap(VisualMap<GL,Def>*parent):Map::Composition(parent),flares_rendered(false),renderer(parent->renderer),stationary_scenery(4,parent->renderer),dynamic_scenery(4,parent->renderer)
		{}
		

	template <class GL, class Def>	
		VisualMap<GL,Def>::VisualMap(GL*renderer_):flares_rendered(false),renderer(renderer_),stationary_scenery(4,renderer_),dynamic_scenery(4,renderer_)
		{}
		

	template <class GL, class Def>	
		bool		VisualMap<GL,Def>::loadFromFile(const CFSFile*file, Map::LoadContext*context)
		{
			if (!Map::Composition::loadFromFile(file,context))
				return false;
			if (!renderer)
			{
				setRenderer(GL::global_instance);
				if (!renderer)
					FATAL__("trying to load map without renderer");
			}
			
			stationary_scenery.lock();
			holographic_scenery.lock();
			
			objects.reset();
			while (MapObject<GL,Def>*object = (MapObject<GL,Def>*)objects.each())
			{
				if (object->instance)
				{
					object->update();
					switch (object->type)
					{
						case Map::ObjectInstance::Holographic:
							object->entity = holographic_scenery.embed(object->instance.pointer());
						break;
						case Map::ObjectInstance::Physical:
							object->entity = stationary_scenery.embed(object->instance.pointer());
						break;
						case Map::ObjectInstance::Item:
							object->entity = stationary_scenery.embed(object->instance.pointer());
							if (object->entity && (!object->spawn_count || object->appear_delay))
								object->entity->config = StructureConfig::Invisible|StructureConfig::NonDetectable;
							else
								object->spawned++;
						break;
					}
					if (object->entity)
						object->entity->attachment.set(object,signature);

				}
			}
			stationary_scenery.unlock();
			holographic_scenery.unlock();
			stationary_scenery.rebuild();
			holographic_scenery.rebuild();
			return true;
		}

	template <class GL, class Def>
		void		VisualMap<GL,Def>::clear()
		{
			stationary_scenery.clear();
			dynamic_scenery.clear();
			Map::Composition::clear();
		}

	template <class GL, class Def>
		void		VisualMap<GL,Def>::setRenderer(GL*renderer_)
		{
			if (renderer == renderer_)
				return;
			if (environment.lights)
				FATAL__("Attempting to set new renderer with existing lights");
			renderer = renderer_;
			stationary_scenery.setRenderer(renderer_);
			dynamic_scenery.setRenderer(renderer_);
			Array<Map::Import*>	resources;
			data.exportTo(resources);
			for (unsigned i = 0; i < resources.count(); i++)
				if (resources[i]->composition)
					((VisualMap<GL,Def>*)resources[i]->composition.pointer())->setRenderer(renderer_);
		}
		
	template <class GL, class Def>
		void		VisualMap<GL,Def>::setRenderer(GL&renderer)
		{
			setRenderer(&renderer);
		}
		
	template <class GL, class Def>
		GL*			VisualMap<GL,Def>::getRenderer()	const
		{
			return renderer;
		}
		
	template <class GL, class Def> template <typename T>
		void		VisualMap<GL,Def>::render(const Aspect<T>&camera, bool lighted, bool use_opacity_query)
		{
			SCENERY_BEGIN
			
			if (!renderer)
			{
				setRenderer(GL::global_instance);
				if (!renderer)
					FATAL__("trying to render without renderer");
			}
			
			{
				if (lighted)
					renderer->enableLighting();
					
					stationary_scenery.resolve(camera);
					holographic_scenery.resolve(camera);
					dynamic_scenery.resolve(camera);
					stationary_scenery.renderOpaqueVisuals();
					holographic_scenery.renderOpaqueVisuals();
					dynamic_scenery.renderOpaqueVisuals();
					stationary_scenery.renderTransparentVisuals();
					holographic_scenery.renderTransparentVisuals();
					dynamic_scenery.renderTransparentVisuals();
					stationary_scenery.postRenderCleanup();
					holographic_scenery.postRenderCleanup();
					dynamic_scenery.postRenderCleanup();

				if (lighted)
					renderer->disableLighting();
				

				objects.reset();
				while (MapObject<GL,Def>*object = (MapObject<GL,Def>*)objects.each())
					if (object->source && object->source->composition)
					{
						renderer->enterSubSystem(object->system);
							Aspect<T>	sub=camera;
							__transformSystem(object->inverse,camera.modelview,sub.modelview);
							__invertSystem(sub.modelview,sub.modelview_invert);
							((VisualMap<GL,Def>*)object->source->composition.pointer())->render(sub,lighted);
						renderer->exitSubSystem();
					}
			
			
				if (!flares_rendered)
					return;
				bool casting = use_opacity_query && renderer->queryBegin();
				
				static Timer::Time t;
				float tdelta = clamped(timer.toSecondsf(timer.now()-t),0,1);
				t = timer.now();
				
				bool query_cast = casting && tdelta < 1 && tdelta > 0;
				
				environment.lights.reset();
				while (MapLight<GL>*light = (MapLight<GL>*)environment.lights.each())
					if (light->light && light->light->isEnabled())
					{
						//renderer->castPointQuery(flare_query,light->position);
						//bool visible = renderer->resolveObjectSize(flare_query)>0;
						light->visible = camera.pointToScreen(light->position,light->projected);
						if (light->query_object.isEmpty())
							light->query_object = renderer->queryObject();
						
						bool visible = light->visible && light->projected[0]>=-1 && light->projected[0]<=1 && light->projected[1]>=-1 && light->projected[1]<=1;
						if (query_cast)
							visible = renderer->resolveObjectSize(light->query_object)&&visible;
						if (casting)
							renderer->castPointQuery(light->query_object,light->position);
						if (visible)
						{
							light->visibility = vmin(light->visibility+2*tdelta,1);
						}
						else
							light->visibility = vmax(light->visibility-2*tdelta,0);
						light->distance = _distance(light->position,camera.absoluteLocation());
						/*if (visible)
							ShowMessage(_toString(light->projected,2));*/
					}
					else
						light->visible = false;
				
				if (casting)
					renderer->queryEnd();
				
				flares_rendered = false;
						
			}
			SCENERY_END
		}
		
	template <class GL, class Def> template <typename T>
		void		VisualMap<GL,Def>::getGravity(const T position[3], const T&mass, T gravity_vector[3])
		{
			static const T g_const = 0.00000000006673;	
			
			_c3(environment.global_gravity,gravity_vector);
			environment.gravity_centers.reset();
			while (Map::GravityCenter*node = environment.gravity_centers.each())
			{
				T distance[3],ndistance[3];
				_sub(node->position,position,distance);
				_c3(distance,ndistance);
				_normalize0(ndistance);
				T d = _dot(distance);
				if (d > getError<T>())
					_mad(gravity_vector,ndistance,g_const*mass*node->mass/d);
			}
		}

	template <class GL, class Def>
		VisualMap<GL,Def>::~VisualMap()
		{
			if (renderer)
			{
				for (unsigned i = 0; i < sprites.count(); i++)
					renderer->discardObject(sprites[i].texture);
			}
		}

	template <class GL, class Def>
		void	VisualMap<GL,Def>::remove(MapObject<GL,Def>*object)
		{
			if (!object || !object->entity)
				return;
			object->entity->config = StructureConfig::Invisible|StructureConfig::NonDetectable;
		}
		
	template <class GL, class Def>
		void	VisualMap<GL,Def>::embed(MapObject<GL,Def>*object)
		{
			if (!object || !object->entity)
				return;
			object->entity->config = StructureConfig::Default;
		}

		
	template <class GL, class Def>
		void	VisualMap<GL,Def>::respawnItems(float time_delta)
		{
			objects.reset();
			while (MapObject<GL,Def>*object = (MapObject<GL,Def>*)objects.each())
			{
				if (object->entity && (object->entity->config & StructureConfig::Invisible) && object->spawned < object->spawn_count && object->type == Map::ObjectInstance::Item)
				{
					object->current_delay += time_delta;
					bool do_spawn = (!object->spawn_count && object->current_delay >= object->appear_delay) || (object->spawn_count && object->current_delay >= object->respawn_delay);
					if (do_spawn)
					{
						object->spawned++;
						embed(object);
					}
				}
			}
		}

		
	template <class GL, class Def>
		void	VisualMap<GL,Def>::generateCentralFlareSprite(FlareSprite&target)
		{
			static Image	upper_blend(256,256,4),
							lower_blend(512,16,4),
							image(512,256,4);
			image.setChannel(0,255);
			image.setChannel(1,255);
			image.setChannel(2,255);
			image.setChannel(3,0);
			igRadial(lower_blend, CGColor(1,1,1,1), CGColor(1,1,1,0));
			igExtRadial(upper_blend,CGColor(1,1,1,1), CGColor(1,1,1,0));
			image.paint(&lower_blend,0,120);
			image.paint(&upper_blend,128,0);
			
			if (!target.texture.isEmpty())
				renderer->discardObject(target.texture);
			target.texture = renderer->textureObject(&image);
			target.width = 2;
			target.height = 1;
			target.position = 0;
			target.intensity = 1;
			target.primary = true;
		}

	template <class GL, class Def>
		void	VisualMap<GL,Def>::generateInnerGlow(FlareSprite&target, float r, float g, float b, float intensity, float size, float position)
		{
			static Image	image(128,128,4);
			
			igRadial(image,CGColor(r,g,b),CGColor(r,g,b,0));
			if (!target.texture.isEmpty())
				renderer->discardObject(target.texture);
			target.texture = renderer->textureObject(&image);
			target.width = target.height = size;
			target.position = position;
			target.intensity = intensity;
			target.primary = false;
		}

	template <class GL, class Def>
		void	VisualMap<GL,Def>::generateOuterRing(FlareSprite&target, float r, float g, float b, float intensity, float size, float position)
		{
			static Image	image(128,128,4);
			
			igRing(image,0.3,CGColor(r,g,b,0),CGColor(r,g,b),CGColor(r,g,b,0),CGColor(r,g,b,0));
			if (!target.texture.isEmpty())
				renderer->discardObject(target.texture);
			target.texture = renderer->textureObject(&image);
			target.width = target.height = size;
			target.position = position;
			target.intensity = intensity;
			target.primary = false;
		}

		
	template <class GL, class Def>
		void		VisualMap<GL,Def>::renderFlares(float pixel_aspect)
		{
			if (!renderer)
			{
				setRenderer(GL::global_instance);
				if (!renderer)
					FATAL__("trying to render flares without renderer");
			}
			if (!sprites.count())
			{
				sprites.setSize(5);
				Image	image;
				
				generateCentralFlareSprite(sprites[0]);
				//							red,		green,	blue,	alpha,	size,		position
				generateOuterRing(sprites[1],1,		1,		1,	0.2,	0.4,	1.5);
				generateInnerGlow(sprites[2],0.7,	0.7,	1,	0.2,	0.3,	0.66);
				generateOuterRing(sprites[3],1,		0.9,	0.8,0.3,	0.2,	0.33);
				generateInnerGlow(sprites[4],1,		0.7,	0.7,0.1,	0.4,	-0.33);
			}

			renderer->unbindAll();
			
		    tCTVertex c0,c1,c2,c3;
		    c0.position[3] = 1;
			c0.position[2] = 0;
			_set4(c0.color,1);
		    c1 = c2 = c3 = c0;
		    _v2(c0.coord,0,0);
		    _v2(c1.coord,1,0);
		    _v2(c2.coord,1,1);
		    _v2(c3.coord,0,1);

			renderer->disableLighting();
			renderer->setFlareBlendFunc();
			float identity[16];
			__eye4(identity);
			renderer->replaceModelview(identity);
			renderer->replaceProjection(identity);
			environment.lights.reset();
			while (MapLight<GL>*light = (MapLight<GL>*)environment.lights.each())
			{
				if (!light->visible || light->visibility <= getError<float>())
					continue;
				float intensity = vmax(light->visibility*(1.5f-_length2(light->projected)),0)/2;
				_c3(light->color,c0.color);
				c0.color[0] = 1.0-((1.0-c0.color[0])*0.7);
				c0.color[1] = 1.0-((1.0-c0.color[1])*0.7);
				c0.color[2] = 1.0-((1.0-c0.color[2])*0.7);
				//_mult(c0.color,1.2);
				//_clamp(c0.color,0,1);
				_c3(c0.color,c1.color);
				_c3(c0.color,c2.color);
				_c3(c0.color,c3.color);
				float base_size = _dot(c0.color);
				for (unsigned i = 0; i < sprites.count(); i++)
				{
					if (sprites[i].texture.isEmpty())
						continue;
					typename GL::FloatType at[2];
					_mult2(light->projected,1.0-sprites[i].position,at);
					float w = sprites[i].width/2/pixel_aspect,
							h = sprites[i].height/2;
					if (true) //sprites[i].distance_dependent)
					{
						float size = base_size/(light->constant_attenuation+light->linear_attenuation*light->distance+light->quadratic_attenuation*sqr(light->distance));
						w*=size;
						h*=size;
					}
					
					
					_v2(c0.position,at[0]-w,at[1]-h);
					_v2(c1.position,at[0]+w,at[1]-h);
					_v2(c2.position,at[0]+w,at[1]+h);
					_v2(c3.position,at[0]-w,at[1]+h);
					c3.color[3] = c2.color[3] = c1.color[3] = c0.color[3] = light->visibility*vmax(1.2f-_length2(light->projected)*(float)!sprites[i].primary,0)/1.2*sprites[i].intensity;

					renderer->useTexture(sprites[i].texture);
		            renderer->face(c0,c1,c2,c3);
				}
			}
			renderer->setDefaultBlendFunc();
			renderer->useTexture(NULL);
			flares_rendered = true;
			renderer->restoreModelview();
			renderer->restoreProjection();
		}

		
		
		
		
		
		
		

	template <class GL, class Def>
		SimpleVehicle<GL,Def>::SimpleVehicle(const Float&x, const Float&y, const Float&z):map(NULL),structure_entity(NULL),
			mass(1.0),horizontal_friction(0.7),vertical_friction(0.9),directional_friction(0.3),rotation_friction(0.95),updrift(0.01),radius(1.0),mouse_acceleration(1),mouse_smooth_time(0.12),order(global_order++)
		{
			config.repel_threshold = 0;
			config.repel_intensity = 0.5;
			config.gravity_strength = 1;
			config.acceleration_energy_drain = 0;
			config.scale_speed_by_energy = false;
			config.process_effects = true;
			config.hide_items_on_collision = true;
			config.friction_model = Configuration::ExponentialFriction;
		
			system.make(1,0.1,600,45);
			reset(x,y,z);
		}
		
	template <class GL, class Def>
		SimpleVehicle<GL,Def>::SimpleVehicle(VisualMap<GL,Def>*map_, const Float&x, const Float&y, const Float&z):map(map_),structure_entity(NULL),
			mass(1.0),horizontal_friction(0.7),vertical_friction(0.9),directional_friction(0.3),rotation_friction(0.95),updrift(0.01),radius(1.0),mouse_acceleration(1),mouse_smooth_time(0.12),order(global_order++)
		{
			config.repel_threshold = 0;
			config.repel_intensity = 0.5;
			config.gravity_strength = 1;
			config.acceleration_energy_drain = 0;
			
			config.scale_speed_by_energy = false;
			config.process_effects = true;
			config.hide_items_on_collision = true;
			config.friction_model = Configuration::ExponentialFriction;

			system.make(1,0.1,600,45);
			reset(x,y,z);
		}

	template <class GL, class Def>
		void			SimpleVehicle<GL,Def>::reset(const Float&x, const Float&y, const Float&z)
		{
			rotation_velocity_x = 0;
			rotation_velocity_x = 0;
			rotation_velocity_z = 0;
			energy_level = 0.1;
			health_level = 1;
			score = 0;
			_clear(closest);
			_clear(closest_normal);
			_clear(velocity);
			_v3(system.location,x,y,z);
			_v3(system.direction,0,0,1);
			_v3(system.vertical,0,1,0);
			system.build();
			
			last_stationary_collision.detected = false;
			last_stationary_collision.remote_vehicle = NULL;
			last_stationary_collision.remote_object = NULL;
			
			last_dynamic_collision.detected = false;
			last_dynamic_collision.remote_vehicle = NULL;
			last_dynamic_collision.remote_object = NULL;
		}
		
	template <class GL, class Def>
		void			SimpleVehicle<GL,Def>::reset(MapStartPosition<GL,Def>*pos)
		{
			if (!pos)
			{
				reset();
				return;
			}
			map = pos->composition;

			rotation_velocity_x = 0;
			rotation_velocity_x = 0;
			rotation_velocity_z = 0;
			energy_level = 0.1;
			health_level = 1;
			score = 0;
			_clear(closest);
			_clear(closest_normal);
			_clear(velocity);
			
			last_stationary_collision.detected = false;
			last_stationary_collision.remote_vehicle = NULL;
			last_stationary_collision.remote_object = NULL;
			
			last_dynamic_collision.detected = false;
			last_dynamic_collision.remote_vehicle = NULL;
			last_dynamic_collision.remote_object = NULL;

			_c3(pos->y_align,system.vertical);
			_mult(pos->direction,-1,system.direction);
			_c3(pos->position,system.location);
			system.build();
		}
		
		
		
	template <class GL, class Def>
		void	SimpleVehicle<GL,Def>::setEntity(StructureEntity<Def>*entity)
		{
			structure_entity = entity;
			for (unsigned i = 0; i < entity->object_entities; i++)
				entity->object_entities[i]->attachment.set(this,signature);
		}

	template <class GL, class Def>
		void	SimpleVehicle<GL,Def>::unsetEntity()
		{
			if (!structure_entity)
				return;
			for (unsigned i = 0; i < structure_entity->object_entities; i++)
				structure_entity->object_entities[i]->attachment.set(NULL,signature);
		}
	
	template <class GL, class Def>
		unsigned	SimpleVehicle<GL,Def>::adviceIterations(const Float&time_delta)	const
		{
			Float speed = _length(velocity),
					distance = speed * time_delta;
			if (!config.two_sided_collision_detection)
				return vmax((unsigned)(distance/radius*2),1);
			return vmax((unsigned)(distance/radius*4),1);
		}

	template <class GL, class Def>
		void		SimpleVehicle<GL,Def>::processFriction(Float vdelta[3], const Float&time_delta)
		{
			if (map)
			{
				Float local_velocity[3],gravity[3],local_gravity[3],
						size = (2*M_PI*radius*radius),
						density = map->environment.atmosphere_density,
						f_slow = 6*M_PI*density*radius*time_delta/mass,
						f_fast = 0.5*density*size*time_delta/mass;
						
				#undef FRICTION
				#define FRICTION(velocity,friction)  (vmin(vabs(velocity),1)*(f_fast*sqr(velocity)*(friction)*sign(velocity))+vmax(1-vabs(velocity),0)*(velocity*f_slow*(friction)))

				__rotate(system.modelview,velocity,local_velocity);
				
				map->getGravity(system.location,mass,gravity);
				
				_mult(gravity,config.gravity_strength);
				
				__rotate(system.modelview,gravity,local_gravity);
				Float effective_updrift = vmin(_length(velocity)*updrift*density,1)*_length(gravity);
				
				if (effective_updrift < 0)
					effective_updrift = 0;
				vdelta[1] += effective_updrift;
				_add(vdelta,local_gravity);

				density /= mass;
				Float friction[3],global_friction[3];
				
				switch (config.friction_model)
				{
					case Configuration::ExponentialFriction:
						friction[0] = local_velocity[0] * (1.0-vpow(1.0-density*horizontal_friction,time_delta));
						friction[1] = local_velocity[1] * (1.0-vpow(1.0-density*vertical_friction,time_delta));
						friction[2] = local_velocity[2] * (1.0-vpow(1.0-density*directional_friction,time_delta));
					break;
					case Configuration::RealisticFriction:
					{
						float v[3],
								l = _length(local_velocity);
						_div(local_velocity,l,v);
						friction[0] = v[0]*sqr(local_velocity[0])*horizontal_friction*f_fast;
						friction[1] = v[1]*sqr(local_velocity[1])*vertical_friction*f_fast;
						friction[2] = v[2]*sqr(local_velocity[2])*directional_friction*f_fast;
						
//#define FRICTION(velocity,friction)  (vmin(vabs(velocity),1)*(f_fast*sqr(velocity)*(friction)*sign(velocity))+vmax(1-vabs(velocity),0)*(velocity*f_slow*(friction)))					
						/*friction[0] = FRICTION(local_velocity[0],horizontal_friction);
						friction[1] = FRICTION(local_velocity[1],vertical_friction);
						friction[2] = FRICTION(local_velocity[2],directional_friction);*/
						
						if (sign(local_velocity[0]-friction[0]) != sign(local_velocity[0]))
							friction[0] = local_velocity[0];
						if (sign(local_velocity[1]-friction[1]) != sign(local_velocity[1]))
							friction[1] = local_velocity[1];
						if (sign(local_velocity[2]-friction[2]) != sign(local_velocity[2]))
							friction[2] = local_velocity[2];
						
					}
					break;
				}
				if (config.friction_model != Configuration::NoFriction)
				{
					__rotate(system.modelview_invert,friction,global_friction);
					_sub(velocity,global_friction);
				}
			}
		}


	template <class GL, class Def>
		void			SimpleVehicle<GL,Def>::animateByMouse(const Float&time_delta, const Float&mouse_x_movement, const Float&mouse_y_movement, const Float&z_rotation, const Float&z_acceleration)
		{
			Float vec[3] = {0,0,-z_acceleration};
			animateByMouse(time_delta,mouse_x_movement,mouse_y_movement,z_rotation,vec);
		}
		
	template <class GL, class Def> template <typename T>
		void			SimpleVehicle<GL,Def>::animateByMouse(const Float&time_delta, const Float&mouse_x_movement, const Float&mouse_y_movement, const Float&z_rotation, const T acceleration[3])
		{
			Float	mhistory_weight = sqr(fmax(mouse_smooth_time-time_delta,0)/mouse_smooth_time),
					vdelta[3],	// Local speed change
					global_delta[3],	// Global speed change
					gravity[3]={0,0,0},local_gravity[3],local_velocity[3],
					mx = (mouse_acceleration*mouse_x_movement*((Float)1-mhistory_weight)+rotation_velocity_x*mhistory_weight),
					my = (mouse_acceleration*mouse_y_movement*((Float)1-mhistory_weight)+rotation_velocity_y*mhistory_weight);
			rotation_velocity_z *= vpow(1-rotation_friction,time_delta);
			rotation_velocity_x = mx;
			rotation_velocity_y = my;
			if (!config.scale_speed_by_energy)
				_c3(acceleration,vdelta);
			else
				_mult(acceleration,energy_level,vdelta);
				
			if (config.acceleration_energy_drain > 0)
				energy_level -= config.acceleration_energy_drain * _dot(vdelta) * time_delta;
			rotation_velocity_z += z_rotation*time_delta;
			
			processFriction(vdelta,time_delta);
			__rotate(system.modelview_invert,vdelta,global_delta);
			_mad(velocity,global_delta,time_delta);
			//__rotate(system.modelview_invert,local_velocity,velocity);
			
			_mad(system.location,velocity,time_delta);
			system.rotateDirectional(rotation_velocity_z*time_delta,false);
			system.rotatePlanar(rotation_velocity_y,rotation_velocity_x);
			_normalize(system.direction);
			_normalize(system.vertical);
			_c3(system.system+3,system.vertical);
			if (instance)
			{
				_c16(system.modelview_invert,instance->matrix);
				instance->update();
			}
		}

	template <class GL, class Def>
		void			SimpleVehicle<GL,Def>::animate(const Float&time_delta, const Float&horizontal_rotation, const Float&vertical_rotation, const Float&z_rotation, const Float&z_acceleration)
		{
			Float vec[3] = {0,0,-z_acceleration};
			animate(time_delta,horizontal_rotation,vertical_rotation,z_rotation,vec);
		}
		
	template <class GL, class Def> template <typename T>
		void			SimpleVehicle<GL,Def>::animate(const Float&time_delta, const Float&horizontal_rotation, const Float&vertical_rotation, const Float&z_rotation, const T acceleration[3])
		{
			Float	mhistory_weight = sqr(fmax(0.12-time_delta,0)/0.12),
					vdelta[3],	// Local speed change
					global_delta[3],	// Global speed change
					gravity[3]={0,0,0},local_gravity[3],local_velocity[3],
					glide = vpow(1-rotation_friction,time_delta);
			
			rotation_velocity_x *= glide;
			rotation_velocity_z *= glide;
			rotation_velocity_y *= glide;
			
			rotation_velocity_x += horizontal_rotation*time_delta;
			rotation_velocity_y += vertical_rotation*time_delta;
			rotation_velocity_z += z_rotation*time_delta;
			
			if (!config.scale_speed_by_energy)
				_c3(acceleration,vdelta);
			else
				_mult(acceleration,energy_level,vdelta);
			if (config.acceleration_energy_drain > 0)
				energy_level -= config.acceleration_energy_drain * _dot(vdelta) * time_delta;
				
			processFriction(vdelta,time_delta);
			__rotate(system.modelview_invert,vdelta,global_delta);
			_mad(velocity,global_delta,time_delta);
			//__rotate(system.modelview_invert,local_velocity,velocity);
			
			_mad(system.location,velocity,time_delta);
			
			system.rotateDirectional(rotation_velocity_z*time_delta,false);
			system.rotatePlanar(rotation_velocity_y*time_delta,rotation_velocity_x*time_delta);
			_normalize(system.direction);
			_normalize(system.vertical);
			_c3(system.system+3,system.vertical);
			if (instance)
			{
				_c16(system.modelview_invert,instance->matrix);
				instance->update();
			}
		}
		
		
	template <class GL, class Def>
		bool SimpleVehicle<GL,Def>::detectStationaryCollision()
		{
			Float zero[3]={0,0,0};
			if (map)
			{
				if (ObjectEntity<Def>*entity = map->stationary_scenery.lookupClosest(system.location,radius,closest,closest_normal))
				{
					Float delta[3];
					_sub(closest,system.location,delta);
					Float distance = _length(delta);
					if (distance < radius)
					{
						
						if (config.two_sided_collision_detection)
						{
							if (_dot(delta,closest_normal)>0)
							{
								_mult(closest_normal,-1);
								//ShowMessage("Inverted");
							}
							/*else
								ShowMessage("Not Inverted");*/

						}
						
						Float convergence = -_dot(closest_normal,velocity);
						if (convergence>0)
						{
							last_stationary_collision.detected = true;
							last_stationary_collision.velocity = convergence;
							_c3(closest,last_stationary_collision.position);
							_c3(closest_normal,last_stationary_collision.normal);
							last_stationary_collision.remote_object = (MapObject<GL,Def>*)entity->structure->attachment.get(VisualMap<GL,Def>::signature);
							return true;
						}
					}
				}
			}
			return false;
		}
		
	template <class GL, class Def>
		bool SimpleVehicle<GL,Def>::detectDynamicCollision()
		{
			if (map && structure_entity)
			{
				typename SceneryTree<Def>::Volume 	vol;
				_sub(system.location,radius,vol.lower);
				_add(system.location,radius,vol.upper);
				Buffer<ObjectEntity<Def>*>	buffer;
				map->dynamic_scenery.tree.lookup(vol,buffer,structure_entity);
				for (unsigned i = 0; i < buffer.fillLevel(); i++)
				{
					SimpleVehicle<GL,Def>*other = (SimpleVehicle<GL,Def>*)buffer[i]->attachment.get(signature);
					if (!other || order >= other->order)
						continue;
					Float delta[3],len;
					_sub(system.location,other->system.location,delta);
					len = _length(delta);
					if (len < radius*2)
					{

						_div(delta,len);
						_mad(system.location,delta,(radius*2-len)*0.55);
						_mad(other->system.location,delta,-(2*radius-len)*0.55);
						
						Float convergence = -_dot(delta,velocity);
						
						
						//if (convergence > 0)
						{
							last_dynamic_collision.detected = true;
							last_dynamic_collision.velocity = convergence;
							last_dynamic_collision.remote_vehicle = other;
							_center(system.location,other->system.location,last_dynamic_collision.position);
							_c3(delta,last_dynamic_collision.normal);
							return true;
							

							/*_mad(other->velocity,delta,-convergence);
							_mad(velocity,delta,convergence);*/
						}
					}
				}
			}
			return false;
		}

	template <class GL, class Def>
		void	SimpleVehicle<GL,Def>::handleStationaryCollision()
		{
			Collision&c = last_stationary_collision;
			if (!c.detected)
				return;
			if (c.remote_object)
			{
				if (config.process_effects)
				{
					c.remote_object->collision_effects.reset();
					while (Map::Effect*effect = c.remote_object->collision_effects.each())
					{
						switch (effect->type)
						{
							case Map::Effect::Score:
								score += effect->strength;
							break;
							case Map::Effect::Refuel:
								energy_level += effect->strength;
							break;
							case Map::Effect::Repair:
								health_level += effect->strength;
							break;
							case Map::Effect::Damage:
								health_level -= effect->strength;
							break;
							case Map::Effect::Kill:
								health_level = 0;
							break;
						}
					}
				}
				
				if (config.hide_items_on_collision && c.remote_object->type == Map::ObjectInstance::Item && map)
				{
					map->remove(c.remote_object);
					c.remote_object->current_delay = 0;
					return;
				}
			}
			
			Float delta[3];
			_sub(c.position,system.location,delta);
			Float distance = _length(delta);
			_mad(system.location,c.normal,radius-distance);

			Float repel_strength = 1;
			if (last_stationary_collision.velocity > config.repel_threshold)
				repel_strength += config.repel_intensity;
			_mad(velocity,last_stationary_collision.normal,repel_strength*last_stationary_collision.velocity);
		}

	template <class GL, class Def>
		void	SimpleVehicle<GL,Def>::handleDynamicCollision()
		{
			if (!last_dynamic_collision.detected)
				return;
			if (last_dynamic_collision.velocity > 0)
			{
				_mad(last_dynamic_collision.remote_vehicle->velocity,last_dynamic_collision.normal,-last_dynamic_collision.velocity);
				_mad(velocity,last_dynamic_collision.normal,last_dynamic_collision.velocity);
			}
			Float convergence = -_dot(last_dynamic_collision.normal,last_dynamic_collision.remote_vehicle->velocity);
			if (convergence > 0)
			{
				_mad(last_dynamic_collision.remote_vehicle->velocity,last_dynamic_collision.normal,convergence);
				_mad(velocity,last_dynamic_collision.normal,-convergence);
			}
		}


	template <class GL, class Def>
		Signature	SimpleVehicle<GL,Def>::signature = Signature();
	template <class GL, class Def>
		Signature	VisualMap<GL,Def>::signature = Signature();
	template <class GL, class Def>
		unsigned	SimpleVehicle<GL,Def>::global_order=0;


}	

#endif

