#include "physics.h"



namespace Physics
{
	static ErrorPrint					error_out;

	float								Wheel::trail_max_age=10;
	void 								(*Wheel::onTrailExpand)(Wheel*wheel, TTrail&trail) = NULL;
	void								(*Wheel::onTrailFinish)(Wheel*wheel, TTrail&trail) = NULL;
	void								(*Wheel::onTrailDelete)(Wheel*wheel, TTrail&trail) = NULL;
	OpenAL::Wave						Wheel::tire_scream;	//!< Tire scream sound	
	Buffer<float4>						explosions;
	
	static TVec3<Composite::sector_t>	last_observer_sector = {0,0,0};
	static float						last_scale=1;
	
	static NxPhysicsSDK* 				gPhysicsSDK = NULL;	
	
	namespace Const
	{
		static const double g_const = 9.81;
		static const double brake = 0.8;
		static const double max_brake = 6;			//!< max braking force
		static const double center_height = 0.15;			//percentual height of the center of gravity along the vertical axis of the vehicle
		static const double contraction_speed = 1;
		static const double retraction_speed = 2;		//higher = softer wheel ground contact, lower = less oscillation
		static const double transversal_grip = 0.07,	//0.1 at 60hz
							drift_slide_factor = 3.0,
							drive_slide_factor = 0.06,
							brake_torque = 70;
		static const float	base_contraction_travel = 0.4f,	//base position of the physx wheel along the contraction vector of the CGS wheel, where 0 = base CGS wheel position and 1 = most contracted CGS wheel position
							contraction_extend_factor = 1.0f;	//factor applied to physX wheel suspension travel as opposed to CGS wheel suspension travel
		
		static const double StaticRoleFriction = 0.01,
							LinearRoleFriction = 0.2,
							QuadraticRoleFriction = 0.15;


		
	}


	Queue<TContact>		contacts;
	Mutex					contact_mutex;
	void								(*onVehicleErase)(Vehicle*) = NULL;
	
	
	
	NxMaterial*	VehiclePrototype::hull_material(NULL);
	NxMaterial*	StructurePrototype::hull_material[2] = {NULL,NULL};
	
	
	OpenAL::Wave						SoundInstance::impact_sound,
										SoundInstance::splash_sound;
	static Buffer<SoundInstance,0,Swap>	active_sounds;
	//Vec3<float>						Particles::particle_delta;

	static Timer::Time	last_impact;
	
	/*
	Buffer<TTriangle>					Wheel::triangles;
	Buffer<TQuad>						Wheel::quads;
	Buffer<TVertex>					Wheel::collidable_vertices,	//!< Vertices considered for collisions
										Wheel::all_vertices;
	Buffer<TEdge>						Wheel::edges;
	Buffer<TBody>						Wheel::bodies;
	Buffer<AbstractGround*>			Wheel::grounds;
	*/
	//static Log	logfile("physics.log",true);
	static bool was_relativated = true;

	LogFile								logfile("physics.log",true);
	
	

	SoundInstance::SoundInstance(const Composite::Coordinates&coordinates, float system_unit_size, float intensity, const OpenAL::Wave&wave, bool alter_pitch):started(false)
	{
		this->coordinates = coordinates;
		create();
		setWaveBuffer(wave);
		setVolume(intensity);
		if (alter_pitch)
			setPitch(2-intensity);
		setReferenceDistance(10*system_unit_size);
	}
	
	static void sound(const Composite::Coordinates&coordinates, float system_unit_size, float intensity, const OpenAL::Wave&wave, bool alter_pitch=false)
	{
		Timer::Time t = timer.now();
		float delta = timer.toSecondsf(t-last_impact);
		if (delta > 0 && delta < 0.2)
			return;
		float i = intensity;//system_unit_size/50;
		last_impact = t;
		if (i > 1)
		{
			active_sounds << SoundInstance(coordinates,system_unit_size,clamped(i/2,0,1),wave,alter_pitch);
		}
		else
			active_sounds << SoundInstance(coordinates,system_unit_size,clamped(i,0,1),wave,alter_pitch);
	}
	
	static void	impact(const Composite::Coordinates&coordinates, float system_unit_size,float intensity)
	{
		sound(coordinates, system_unit_size,intensity,SoundInstance::impact_sound,false);
	}
	


	
	
	class Report : public NxUserContactReport
	{
	public:
		virtual void  onContactNotify(NxContactPair& pair, NxU32 events)
		{
			NxContactStreamIterator i(pair.stream);
			//user can call getNumPairs() here
			while(i.goNextPair())
			{
				//user can also call getShape() and getNumPatches() here
				NxShape *s0 = i.getShape(0),
						*s1 = i.getShape(1);
				if (s0->getType()==NX_SHAPE_WHEEL || s1->getType()==NX_SHAPE_WHEEL)
				{
					while(i.goNextPatch())
						while(i.goNextPoint());
					continue;
				}
				while(i.goNextPatch())
				{
					//user can also call getPatchNormal() and getNumPoints() here
					const NxVec3& normal = i.getPatchNormal();
					while(i.goNextPoint())
					{
						float intensity = i.getPointNormalForce()/1000000;
						if (intensity < 0.1)
							continue;
						contact_mutex.lock();
							TContact&c = contacts.push();
							c.coordinates.sector = last_observer_sector;
							NxVec::mult(i.getPoint(),last_scale,c.coordinates.remainder);
							c.intensity = intensity;
							c.age = 0;
						contact_mutex.release();
						impact(c.coordinates,last_scale,vabs(intensity)*4);						
					}		
				}
			}

		}

	};

	static Report	global_report;
	
	
		

	inline static float intensityOf(const TSmokeParticle&particle)
	{
		return sin(M_PI/(particle.age/0.01+1));
		//(particle.age<0.1?particle.age*10:1.0-(particle.age-0.1)/0.9);
	}
	
	
	
	bool Entity::detectGround(const TEnvironment&env, float delta)
	{
		if (ground_detected)
		{
			ground_age += delta;
			if (ground_age > height_above_ground/(1+velocity)/100)
			{
				ground_age = 0;
				ground_detected = false;
			}
		
		}
		
		/* ...*/
		
		return ground_detected;
	}
	
	
	void FirstPerson::process(const TEnvironment&env,float delta)
	{
		atmosphere_density = env.getAtmosphereDensity(coordinates);
		float	density = clamp(atmosphere_density,0,1);

		bool above_water = height_above_water > 0;
		
		if (velocity > getError<float>())
		{
			float	d = atmosphere_density;
			if (d > 1)
				d = 10;//water
		
			float	size = (2*M_PI*height*height/(env.system_unit_size*env.system_unit_size)),
					f_slow = 6*M_PI*d*height*velocity,
					f_fast = 0.5*d*size*env.system_unit_size*sqr(velocity/env.system_unit_size),
					f_final = (f_slow + f_fast)*delta/80;
					
			static const float friction_strength = 0.001;
			float3			friction,
							normalized = inertia/velocity;
			friction = normalized*(f_final*friction_strength);
						
			if (sign(inertia.x-friction.x) != sign(inertia.x))
				friction.x = inertia.x;
			if (sign(inertia.y-friction.y) != sign(inertia.y))
				friction.y = inertia.y;
			if (sign(inertia.z-friction.z) != sign(inertia.z))
				friction.z = inertia.z;
			inertia -= friction;
		}
		
		bool changed = false;
		TMatrix3<> matrix;
		if (fabs(rotate_horizonal) > TypeInfo<float>::error)
		{
			Mat::rotationMatrix(rotate_horizonal,y_axis,matrix);
			Mat::mult(matrix,z_axis);
			changed = true;
		}
		if (fabs(rotate_vertical) > TypeInfo<float>::error)
		{
			float a = acos(Vec::dot(z_axis,y_axis))*180/M_PI;
			if (a-rotate_vertical > 179)
				rotate_vertical = -(179-a);
			if (a-rotate_vertical < 1)
				rotate_vertical = -(1-a);
			float3 axis;
			Vec::cross(z_axis,y_axis,axis);
			Vec::normalize0(axis);
			Mat::rotationMatrix(-rotate_vertical,axis,matrix);
			Mat::mult(matrix,z_axis);
			//__mult331(matrix,y_axis);
			changed = true;
		}
		float3	accel;
		Mat::rotate(system,speed,accel);
		inertia += accel*delta;
		
		float3	gravity;
		env.getGravityAcceleration(coordinates,gravity);
		if (atmosphere_density > 1)
			gravity /= atmosphere_density;
		inertia += gravity*delta*10;
		
		if (height_above_ground < height*1.2)
		{
			y_axis = up;
			float3	rotated_speed;
			TMatrix3<> matrix;
			matrix.y = y_axis;
			Vec::cross(y_axis,z_axis,matrix.x);
			Vec::normalize0(matrix.x);
			Vec::cross(matrix.x,matrix.y,matrix.z);
			Vec::normalize0(matrix.z);
			Mat::mult(matrix,speed,rotated_speed);
			float fc = vpow(0.2,delta);
			Vec::interpolate(inertia,rotated_speed,fc,inertia);
			changed = true;
			inertia -= up*fabs(height_above_ground-height)*delta*10;
		}		
		coordinates.mad(inertia,delta,env.sector_size);
		
		if (!detectGround(env,delta))
			height_above_ground = height_above_water-ground_height_above_water;
			
		{
			if (height_above_ground < height)
			{
				float indent = -ground_normal*inertia;
				//if (indent > 0)
					//inertia += ground_normal*indent;
				coordinates.mad(up,height-height_above_ground,env.sector_size);
				height_above_ground = height;
			}

		}
		if (changed)
			updateSystem();
		
		


		
		
		velocity = inertia.length();
		if (above_water != height_above_water > 0)
		{
			sound(coordinates,env.system_unit_size, velocity/10,SoundInstance::splash_sound);
		}
		
	}
		
	
	//bool					Particles::in_sequence=false;

	/*
	void					Particles::begin()
	{
		if (in_sequence)
			FATAL__("sequence was not terminated last time");
		static OpenGL::TextureObject	flare_texture;
		if (flare_texture.isEmpty())
		{
			Image image(64,64,4);
			igRadial(image,CGColor(1,1,1),CGColor(0,0,0,0),false);
			flare_texture = display.textureObject(&image);
		}
		float identity[4*4];
		__eye4(identity);
		display.replaceModelview(identity);
		//glPushMatrix();
			glColor3f(1,0,0);
			//glLoadIdentity();
			display.useTexture(flare_texture);
			display.depthMask(false);
			display.setFlareBlendFunc();
			in_sequence = true;
			
	}
	
	void 					Particles::end()
	{
		if (!in_sequence)
			FATAL__("trying to terminate non existing sequence");
			display.depthMask(true);
			display.setDefaultBlendFunc();
			display.useTexture(NULL);
		//glPopMatrix();
		display.restoreModelview();
		in_sequence = false;
	}




		
	void 					Flame::render(const Camera<>&camera,const float color[3], float intensity, Vehicle*ship)
	{
		if (!accel)
			return;
		if (!in_sequence)
			FATAL__("trying to render flame out of sequence");
	
		float begin[4],dir[3],end[4],projected_begin[4],projected_end[4];
		__transform(system,accel->position,begin);
		__rotate(system,accel->direction,dir);
		float l = accel->flame_length*intensity*2;
		Vec::mult(dir,-l,end);
		Vec::add(end,begin);
		
		{
			float tdelta = timing.delta/trail_length;
			for (DQueue::iterator it = trail.begin(); it != trail.end(); ++it)
			{
				Vec::sub(it->position,particle_delta);
				//Vec::mult(it->velocity,pow(0.9,timing.delta*SolarRenderer::atmosphere_density));
				Vec::mad(it->position,it->velocity,timing.delta);
				//Vec::mad(it->position,dir,-(1.0-it->age)*Vec::length(last_frame_movement)*2);
				it->age+=tdelta;
			}
			
			while (!trail.isEmpty() && trail.oldest().age > 1)
				trail.pop();
			//if (!uber_drive_active)
			{
				TSmokeParticle p;
				p.intensity = intensity*vmin(ship->atmosphere_density,1.0);
				Vec::c3(end,p.position);
				Vec::cross(system,dir,p.up);
				Vec::mult(dir,-1*intensity*100,p.velocity);
				Vec::mad(p.velocity,particle_delta,(1.0-clamp(ship->atmosphere_density,0,1))/timing.delta);
				Vec::normalize0(p.up);
				Vec::cross(p.up,dir,p.right);
				p.age = 0;
				trail << p;
			}
		}
		
		if (ship->atmosphere_density > 1)
			return;
		
		__transform(camera.modelview,begin);
		__transform(camera.modelview,end);
		begin[3] = 1;
		end[3] = 1;
		__mult441(camera.projection,begin,projected_begin);
		__mult441(camera.projection,end,projected_end);
		Vec::div(projected_begin,projected_begin[3]);
		Vec::div(projected_end,projected_end[3]);
		float len = Vec::distance2(projected_begin,projected_end);
		unsigned resolution = clamped(10+(unsigned)(len*60.0f),1,100);
		
		float effective_color[3];
		//if (!uber_drive_active)
		{
			Vec::mult(color,0.8,effective_color);
			Vec::addValue(effective_color,0.2);
		}
		//else
			//Vec::v3(effective_color,0.3,0.4,1);
		glBegin(GL_QUADS);
			for (unsigned j = 0; j <= resolution; j++)
			{
				float fc = (float)j/(float)(resolution),
						p[3],
						at = 1.0f-fc;
				Vec::interpolate(begin,end,fc,p);
				float size = sin(pow(1.4,-((7.0f-at*7.0f)-2.9)))*accel->flame_width/1.3;
				//if (uber_drive_active)
					//size*=3;
							//sin(pow(1.4,-((7 - (at*3-1)/2*7)-2.9)))*a.flame_width/1.3;
				float alpha = at*40.0f/(float)resolution;
				//if (!uber_drive_active)
					alpha*=intensity;
				glColor4f(effective_color[0],effective_color[1],effective_color[2],alpha);
				glTexCoord2f(0,0);glVertex3f(p[0]-size,p[1]-size,p[2]);
				glTexCoord2f(1,0);glVertex3f(p[0]+size,p[1]-size,p[2]);
				glTexCoord2f(1,1);glVertex3f(p[0]+size,p[1]+size,p[2]);
				glTexCoord2f(0,1);glVertex3f(p[0]-size,p[1]+size,p[2]);
			}
		glEnd();
		
	}
	
	void			Flame::renderTrail(const Camera<>&camera)
	{
		if (!accel)
			return;
		
		if (!in_sequence)
			FATAL__("trying to render flame trail out of sequence");
	
		if (!trail.length())
			return;
		float begin[4],dir[3],end[4],projected_begin[4],projected_end[4];
		begin[3] = 1;
		end[3] = 1;
		unsigned resolution=0;
		glBegin(GL_QUADS);
			DQueue::iterator prev = trail.begin();
			for (DQueue::iterator next = prev+1; next != trail.end(); ++next)
			{
				float	size0 = intensityOf(*prev),
						size1 = intensityOf(*next);

				__transform(camera.modelview,prev->position,begin);
				__transform(camera.modelview,next->position,end);
				begin[3] = 1;
				end[3] = 1;
				__mult441(camera.projection,begin,projected_begin);
				__mult441(camera.projection,end,projected_end);
				Vec::div(projected_begin,projected_begin[3]);
				Vec::div(projected_end,projected_end[3]);
				float len = Vec::distance2(projected_begin,projected_end);
				resolution = clamped(10+(unsigned)(len*100.0f),1,200);
				
				
				for (unsigned j = 1; j <= resolution; j++)
				{
					float fc = (float)j/(float)(resolution),
							p[3],
							at = 1.0f-fc;
					Vec::interpolate(begin,end,fc,p);
					float	intensity = size0*prev->intensity+fc*(size1*next->intensity-size0*prev->intensity),
							size = accel->flame_width*1*(size0+fc*(size1-size0));

					glWhite(intensity/resolution);
					
					glTexCoord2f(0,0);glVertex3f(p[0]-size,p[1]-size,p[2]);
					glTexCoord2f(1,0);glVertex3f(p[0]+size,p[1]-size,p[2]);
					glTexCoord2f(1,1);glVertex3f(p[0]+size,p[1]+size,p[2]);
					glTexCoord2f(0,1);glVertex3f(p[0]-size,p[1]+size,p[2]);
				}
				prev = next;
			}			
		glEnd();

	}
	*/


	void	Flame::init(FlamePrototype*prototype)
	{
		accel = prototype->accel;
		system = prototype->system->system_link;
	}

	bool 	WheelPrototype::onWheelContact(NxWheelShape* wheelShape, NxVec3& contactPoint,
											NxVec3& contactNormal, NxReal& contactPosition,
											NxReal& normalForce, NxShape* otherShape,
											NxMaterialIndex& otherShapeMaterialIndex,
											NxU32 otherShapeFeatureIndex)
	{
		/*if (normalForce > 70)
			normalForce = 70;*/
		//record contact point for skidmark later on
		return true;
	}

	
	void WheelPrototype::configure(const TEnvironment&env, CGS::Geometry<>&geometry)
	{
		/*NxMaterialDesc materialDesc;
		materialDesc.restitution			= 0.0f;
		materialDesc.dynamicFriction		= Const::transversal_grip*wheel->grip;
		materialDesc.staticFriction			= 2.0f;
		materialDesc.staticFrictionV		= 0.4;
		materialDesc.dynamicFrictionV		= 0.1;
		materialDesc.dirOfAnisotropy.x 		= 0;
		materialDesc.dirOfAnisotropy.y 		= 0;
		materialDesc.dirOfAnisotropy.z 		= 1;
		materialDesc.frictionCombineMode	= NX_CM_MULTIPLY;
		materialDesc.flags					=  NX_MF_ANISOTROPIC;
		material = env.scene->createMaterial(materialDesc);*/
		

		
		desc.radius = wheel->radius;
		desc.suspensionTravel = Vec::length(wheel->contraction)*Const::contraction_extend_factor;	//travel goes down, not up
		desc.suspension.spring = wheel->suspension_strength*600;//15000//5000
		desc.suspension.damper = desc.suspension.spring/2.5;//1006
		desc.suspension.targetValue = desc.suspensionTravel;
		/*desc.longitudalTireForceFunction.extremumValue = 0.1;
		desc.longitudalTireForceFunction.asymptoteValue = 0.01;*/
		desc.lateralTireForceFunction.extremumSlip = 0.3;
		desc.lateralTireForceFunction.extremumValue = Const::transversal_grip*wheel->grip;
		desc.lateralTireForceFunction.asymptoteSlip = 0.6;//desc.lateralTireForceFunction.extremumValue/10;
		desc.lateralTireForceFunction.asymptoteValue = desc.lateralTireForceFunction.extremumValue;//desc.lateralTireForceFunction.extremumValue/10;
		desc.mass = (M_PI*sqr(wheel->radius)*wheel->width*wheel->density);
		desc.group = ActorGroup::Wheel;
		desc.inverseWheelMass = 1.0/desc.mass;
		desc.motorTorque = 0;
		desc.brakeTorque = 0;
		desc.steerAngle = 0;
		desc.wheelContactModify = this;
		
		//desc.materialIndex = material->getMaterialIndex();
		
		TMatrix4<PhFloat> system;
		CGS::SubGeometryA<>*sub = geometry.object_field+path[0];
		Mat::copy(sub->meta.system,system);
		for (index_t i = 1; i+1 < path.length();i++)
		{
			sub = sub->child_field+path[i];
			TMatrix4<PhFloat> sys;
			Mat::transformSystem(system,sub->meta.system,sys);
			system = sys;
		}
		
		TMatrix4<PhFloat>	inner,final;
		TVec3<PhFloat>		p;
		Vec::mad(wheel->position,wheel->contraction,Const::base_contraction_travel - (1.0f- Const::contraction_extend_factor),p);
		//Vec::add(wheel->position,wheel->contraction,p);
		Mat::buildSystem(p,wheel->axis,wheel->contraction,inner);
		Mat::transformSystem(system,inner,final);
		
		
		
		//Vec::div(final+12,env.system_unit_size);
		
		
		
		
		NxMat::convert(final,desc.localPose);
		
		desc.density = wheel->density;
		
		
		ASSERT__(desc.isValid());
		
		//desc.wheelFlags |= NX_WF_AXLE_SPEED_OVERRIDE
	}


	VehiclePrototype::VehiclePrototype()
	{
		config.optimal_speed = 1;
		config.optimal_maneuverability = 1;
		config.zero_maneuverability = 0;
		config.updrift = 0.5;
		config.friction = Vec3<float>(0.6,1,0.1)*1000;
		config.rotation_friction = 0.9;
		config.rotation_speed = 1;
		config.brake_strength = Const::brake;
		config.center_height = Const::center_height;

		float global_drift_factor = 5000.0f;
		config.forward_down_drift = 1.0f*global_drift_factor;
		config.transveral_down_drift = 0.5f*global_drift_factor;
		config.backward_down_drift = 0.25f*global_drift_factor;
		
		if (!engine_type_map.query("wheel_drive",wheel_drive.type))
			FATAL__("Physics engine not properly initialized. Please invoke Physics::init() before loading vehicles");
		wheel_drive.support = 0;
	}
	
	
	

	void 	VehiclePrototype::scanItemsRec(const TEnvironment&env, CGS::SubGeometryA<>&geometry)
	{
		for (index_t i = 0; i < geometry.accelerator_field.length(); i++)
			for (index_t j = 0; j < engine_prototypes.count(); j++)
			{
				EnginePrototype&ept = engine_prototypes[j];
				if (ept.type->zero_efficiency_window[0] <= geometry.accelerator_field[i].zero_efficiency && ept.type->zero_efficiency_window[1] >= geometry.accelerator_field[i].zero_efficiency)
				{
					FlamePrototype&prototype = ept.flames.append();
					prototype.system = &geometry;
					prototype.accel = geometry.accelerator_field+i;
					prototype.engine_type = ept.type;
				}
			}
		count_t current = path.fillLevel();
		for (index_t i = 0; i < geometry.wheel_field.length(); i++)
		{
			path << i;
			logfile << "  Identified wheel in "<<Vec::toStringD(path.pointer(),path.length())<<nl;
			
			WheelPrototype&prototype = wheels.append();
			prototype.system = &geometry;
			prototype.wheel = geometry.wheel_field+i;
			prototype.engine_type = wheel_drive.type;
			path.copyToArray(prototype.path);
			prototype.configure(env,this->geometry);
			path.pop();
		}
		
		for (index_t i = 0; i < geometry.child_field.length(); i++)
		{
			path << i;
			scanItemsRec(env,geometry.child_field[i]);
			path.truncate(current);
		}
	}

	static const count_t MapRes=32;
	static	bool sector_map[MapRes*MapRes*MapRes];

	void	buildHull(const CGS::SubGeometryA<>&child, Buffer<Vec3<PhFloat> >&out, const float3& base, const float3& range, bool sector_map[MapRes*MapRes*MapRes])
	{
		if (child.meta.flags&CGS::AnimationTargetFlag)
			return;
		logfile << "  now processing "<<child.name<<nl;
		if (child.vs_hull_field.length())
		{
			const Mesh<CGS::SubGeometryA<>::VsDef>&vhull = child.vs_hull_field.first();
			for (index_t i = 0; i < vhull.vertex_field.length(); i++)
			{
				const TVec3<CGS::SubGeometryA<>::VsDef::Type>&p = vhull.vertex_field[i].position;
				TVec3<CGS::SubGeometryA<>::VsDef::Type> a;
				Mat::transform(child.path,p,a);
				Vec::sub(a,base);
				if (a.y < 0)
					continue;

				TVec3<index_t> rel = {
									(index_t)((MapRes-1)*clamp((a.x)/(range.x),0,1)),
									(index_t)((MapRes-1)*clamp((a.y)/(range.y),0,1)),
									(index_t)((MapRes-1)*clamp((a.z)/(range.z),0,1))
								};
				index_t index = rel.x*MapRes*MapRes+rel.y*MapRes+rel.z;
				ASSERT2__(index<MapRes*MapRes*MapRes,index,MapRes);
				if (!sector_map[index])
				{
					logfile << "   position not mapped: vertex "<<i<<"/"<<vhull.vertex_field.length()<<" in "<<Vec::toString(rel)<<" ("<<index<<")"<<nl;
					TVec3<double> np = {
									base.x + (range.x)*rel.x/(MapRes-1),
									base.y + (range.y)*rel.y/(MapRes-1),
									base.z + (range.z)*rel.z/(MapRes-1)};
					/*double absolute[3];*/
					logfile << "    including "<<Vec::toString(np)<<nl;
					Vec3<PhFloat>&v = out.append();
					/*v.x = a[0];
					v.y = a[1];
					v.z = a[2];*/
 					v.x = np.x;
					v.y = np.y;
					v.z = np.z;
 
					/*v.x = rel[0]+index/10000;
					v.y = rel[1];
					v.z = rel[2]+index/100000;*/
					
					sector_map[index] = true;
				}
			}
		}
		
		for (index_t i = 0; i < child.child_field.length(); i++)
			buildHull(child.child_field[i],out,base,range,sector_map);
	}
	


	bool		VehiclePrototype::loadFromFile(const TEnvironment&env, const String&filename)
	{
		engine_prototypes.reset();
		wheels.reset();
		wheel_drive.support = 0;
		logfile << "loading geometry from '"<<filename<<"'"<<nl;
		if (!geometry.loadFromFile(filename.c_str()) || !geometry.extractDimensions(dimensions))
		{
			logfile << "failed :("<<nl;
			return false;
		}

		
		//geometry.swapComponents(0,2);
		//geometry.swapComponents(0,2);
		logfile << "extracting radius"<<nl;
		geometry.extractRadius(config.radius);
		config.mass = 4.0/3.0*M_PI*cubic(config.radius)/(env.system_unit_size*env.system_unit_size*env.system_unit_size) * 8;	//material density is 8
		logfile << " radius = "<<config.radius<<nl;
		logfile << " mass = "<<config.mass<<nl;
			
		XML::Container	container;
		logfile << "parsing XML"<<nl;
		container.loadFromString(geometry.xml);
		logfile << "loading XML"<<nl;
		loadFromXML(container);	//load what's there even if xml is partly invalid
		
		logfile << "searching for items"<<nl;
		for (index_t i = 0; i < geometry.object_field.length(); i++)
		{
			path.reset();
			path << i;
			scanItemsRec(env,geometry.object_field[i]);
		}
			
		logfile << "extracting dimensions"<<nl;
		{
			Vec::center(dimensions.min,dimensions.max,center_of_gravity);
			center_of_gravity.y = dimensions.min.y+config.center_height*(dimensions.max.y-dimensions.min.y);
		}
		logfile << " = "<<Vec::toString(dimensions.min)<<" - "<<Vec::toString(dimensions.max)<<nl;
		
		ConvexHullBuilder<PhFloat>	builder;
		
		logfile << "flagging animation targets"<<nl;
		geometry.flagAnimationTargets();
		
		memset(sector_map,false,sizeof(sector_map));
		
		float3 range,base;
		Vec::sub(dimensions.max,dimensions.min,range);
		base = dimensions.min;
		
		if (base.y < range.y/5)	//cut off lower 20% of the height range, to reduce risk of collisions between the vehicle body and uneven ground
		{
			float dif = range.y/5-base.y;
			base.y += dif;
			range.y -= dif;
		}

		
		logfile << "building hull @MapRes = "<<MapRes<<", range = "<<Vec::toString(range)<<nl;
		
		//NxVec3 insPoint = (downAxis * -Cos) + (frontAxis * Sin);
		Buffer<Vec3<PhFloat> >	points;
		
		for (index_t i = 0; i < geometry.object_field.length(); i++)
			buildHull(geometry.object_field[i],points,base,range,sector_map);
			
		/*
		logfile << "including "<<points.count()<<" vertices..."<<nl;
		for (unsigned i = 0; i < points.count(); i++)
		{
			logfile << " including "<<i<<"/"<<points.count()<<" "<<points[i].toString()<<nl;
			builder.include(points[i]);
		}
		
		Mesh<TDef<NxReal> >		hull;			
		
		
		logfile << "exporting hull"<<nl;
		logfile << " vertices = "<<builder.vertices.count()<<nl;
		logfile << " triangles = "<<builder.triangles.count()<<nl;
		builder.exportToObject(hull);
		*/
		
		/*
		logfile << "generating normals"<<nl;
		hull.generateNormals();
		logfile << "generating edges"<<nl;
		hull.generateEdges();
		logfile << " vertices = "<<hull.vertex_field.length()<<nl;
		logfile << " triangles = "<<hull.triangle_field.length()<<nl;
		logfile << " edges = "<<hull.edge_field.length()<<nl;
		logfile << "setting grip"<<nl;
		for (unsigned i = 0; i < hull.vertex_field.length(); i++)
			hull.vertex_field[i].grip = 1;
		for (unsigned i = 0; i < hull.triangle_field.length(); i++)
			hull.triangle_field[i].grip = 1;
		for (unsigned i = 0; i < hull.quad_field.length(); i++)
			hull.quad_field[i].grip = 1;
		
		logfile << "building map"<<nl;
		hull.map = hull.buildMap(O_ALL,3);
		logfile << "prototype loaded"<<nl;*/
		
		if (!hull_material)
		{
			NxMaterialDesc desc;
			desc.dynamicFriction = 0.2;
			desc.staticFriction = 0.4;
			desc.restitution = 0.4;
			desc.frictionCombineMode = NX_CM_MULTIPLY;
			if (!env.scene)
				return false;
			hull_material = env.scene->createMaterial(desc);
		}
			
	
		Array<NxVec3>					hull_vertices;
		//Array<NxU32>					hull_indices;
		
		hull_vertices.setSize(points.count());
		for (index_t i = 0; i < hull_vertices.count(); i++)
			NxVec::convert(points[i],hull_vertices[i]);
		
		/*hull_vertices.resize(hull.vertex_field.length());
		hull_indices.resize(hull.triangle_field.length()*3);
		
		for (unsigned i = 0; i < hull.vertex_field.length(); i++)
			Vec::convert(hull.vertex_field[i].position,hull_vertices[i]);
		for (unsigned i = 0; i < hull.triangle_field.length(); i++)
		{
			hull_indices[i*3] = hull.triangle_field[i].v0-hull.vertex_field;
			hull_indices[i*3+1] = hull.triangle_field[i].v1-hull.vertex_field;
			hull_indices[i*3+2] = hull.triangle_field[i].v2-hull.vertex_field;
		}*/
		
		NxConvexMeshDesc mesh_desc;
		mesh_desc.numVertices = static_cast<NxU32>(hull_vertices.count());
		//mesh_desc.numTriangles = hull.triangle_field.length();
		mesh_desc.pointStrideBytes = sizeof(NxVec3);
		//mesh_desc.triangleStrideBytes = sizeof(NxU32)*3;
		
		
		mesh_desc.points = hull_vertices.pointer();
		//mesh_desc.triangles = hull_indices;
		mesh_desc.flags = NX_CF_COMPUTE_CONVEX;
		
		ASSERT__(mesh_desc.isValid());
		
		
		NxWriteBuffer buf;
		if(CookConvexMesh(mesh_desc, buf)) 
		{
			hull_desc.group = ActorGroup::DynamicMesh;
			hull_desc.meshData = env.scene->getPhysicsSDK().createConvexMesh(NxReader(buf));
			hull_desc.materialIndex = hull_material->getMaterialIndex();
			hull_desc.shapeFlags |= NX_SF_VISUALIZATION;
			if(hull_desc.meshData != NULL) 
				actor_desc.shapes.pushBack(&hull_desc);
			else
				FATAL__("Failed to create convex hull");
		}
		else
		{
			hull_desc.meshData = NULL;
			if (hasCookingLibrary())
				FATAL__("Failed to cook convex hull")
			else
				FATAL__("Cooking library not available");
		}
		
		
		
		body_desc.mass = config.mass = 500;
		body_desc.sleepEnergyThreshold = 0.05;
		actor_desc.group = ActorGroup::DynamicMesh;
		actor_desc.body = &body_desc;
		body_desc.contactReportThreshold = 0;
		
		
		
		for (index_t i = 0; i < wheels.count(); i++)
			actor_desc.shapes.pushBack(&wheels[i].desc);
		
		ASSERT__(actor_desc.isValid());
		
		
		if (!nearingOne(env.system_unit_size))
		{
			geometry.scale(env.system_unit_size);
			
			config.radius *= env.system_unit_size;
		}
		
		
		return true;
	}
	

	void		VehiclePrototype::loadFromXML(const XML::Container&container)
	{
		engine_prototypes.clear(1);
		wheel_drive.support = 0;
		const XML::Node*simulation = container.find("ship/simulation");
		if (!simulation)
			simulation = container.find("vehicle/simulation");
		if (simulation)
		{
			#undef GET
			#undef GETVAR
			
			#define GETVAR(VAR,NAME)\
				{\
					if (const XML::Node*sub = simulation->find(NAME))\
					{\
						if (const XML::TAttribute*attrib = sub->attributes.lookup("value"))\
							convert(attrib->value.c_str(),config.VAR);\
						else\
							ErrMessage("Failed to lookup 'value' attribute of entry "NAME);\
					}\
					else\
						ErrMessage("unable to find xml config segment " NAME);\
				}
			
			#define GET(VAR)			GETVAR(VAR,#VAR)
			GET(optimal_speed);
			GET(optimal_maneuverability);
			GET(zero_maneuverability);
			GET(updrift);
			GET(brake_strength);
			GET(center_height);
			
			GETVAR(rotation_friction[Pitch],"pitch_friction");
			GETVAR(rotation_friction[Roll],"roll_friction");
			GETVAR(rotation_friction[Yaw],"yaw_friction");
			GETVAR(rotation_speed[Pitch],"pitch_speed");
			GETVAR(rotation_speed[Roll],"roll_speed");
			GETVAR(rotation_speed[Yaw],"yaw_speed");
			GETVAR(friction[Directional],"friction");
			GETVAR(friction[Horizontal],"h_friction");
			GETVAR(friction[Vertical],"v_friction");
		
			#undef GET
			#undef GETVAR
			
			
			if (const XML::Node*drive_support = container.find("ship/simulation/drive_support"))
			{
				for (index_t i = 0; i < drive_support->children.count(); i++)
				{
					const String&name = drive_support->children[i].name;
					EngineType*type;
					if (!engine_type_map.query(name,type))
					{
						ErrMessage("Engine type not supported: '"+name+"'");
						continue;
					}
					String value;
					if (!drive_support->children[i].query("value",value))
					{
						ErrMessage("XML node for drive support '"+name+"' doesn't provide a 'value' entry");
						continue;
					}
					float support;
					if (!convert(value.c_str(),support))
					{
						ErrMessage("Unable to convert effective engine support for engine type '"+name+"': '"+value+"' is no valid float");
						continue;
					}
					if (type == wheel_drive.type)
					{
						wheel_drive.support = support;
						continue;
					}
					
					if (support > 0)
					{
						EnginePrototype&prototype = engine_prototypes.append();
						prototype.type = type;
						prototype.support = support;
					}
				}
			}
		}
	}
	

	void Engine::init(EnginePrototype*prototype)
	{
		ASSERT_NOT_NULL__(prototype);
		type = prototype->type;
		//ASSERT__(engine_types(type)>0);
		ASSERT_NOT_NULL__(type);
		support = clamped(prototype->support,0,1);
		heat_up_remaining = type->heat_up;
		status = 0;
		flame_size = 0;
		acceleration = 0;
		fuel_consumption = 0;
		energy_consumption = 0;
		flames.setSize(prototype->flames.count());
		for (index_t i = 0; i < flames.count(); i++)
			flames[i].init(prototype->flames+i);
	}
	
	Entity::Entity():z_is_primary(true),atmosphere_density(1),ground_age(100000),ground_detected(false)
	{
		reset();
	}
	
	void Entity::reset()
	{
		y_axis.set(0,1,0);
		z_axis.set(0,0,-1);
		yaw = 0;
		pitch = 0;
		roll = 0;
		velocity = 0;
		inertia = 0;
		heading.set(0,0,-1);
		Vec::clear(coordinates.remainder);
		Vec::clear(coordinates.sector);
		updateSystem();
	}
	
	
	void Entity::updateSystem()
	{
		Mat::eye(system);
		if (z_is_primary)
		{
			system.z.xyz = z_axis;
			Vec::cross(y_axis,z_axis,system.x.xyz);
			Vec::normalize0(system.x.xyz);
			Vec::cross(z_axis,system.x.xyz,system.y.xyz);
			Vec::normalize0(system.y.xyz);
		}
		else
		{
			system.y.xyz = y_axis;
			//Vec::normalize0(system+4);
			Vec::cross(y_axis,z_axis,system.x.xyz);
			Vec::normalize0(system.x.xyz);
			Vec::cross(system.x.xyz,y_axis,system.z.xyz);
			Vec::normalize0(system.z.xyz);
		}
		//Vec::c3(position,system+12);

		//Vec::mult(system,-1);
		//Vec::mult(system+8,-1);
		Mat::invertSystem(system,invert);
	}
	
	
	void	Wheel::init(WheelPrototype*prototype)
	{
		if (!prototype)
			return;
		system = prototype->system->system_link;
		wheel = prototype->wheel;
		contraction_length = Vec::length(wheel->contraction);
		updateInnerSystem();
	}
	
	void Wheel::updateInnerSystem()
	{
		if (!wheel)
		{
			Mat::eye(inner_system);
			return;
		}
		Vec::copy(wheel->axis,inner_system.x.xyz);
		Vec::cross(wheel->axis,wheel->contraction,inner_system.z.xyz);
		Vec::cross(inner_system.z.xyz,inner_system.x.xyz,inner_system.y.xyz);
		Vec::normalize0(inner_system.z.xyz);
		Vec::normalize0(inner_system.y.xyz);
		Vec::copy(wheel->position,inner_system.w.xyz);
		Mat::resetBottomRow(inner_system);
	}
	

	void	Wheel::reset()
	{
		last_position.reset();
		xgrip = 0;
		grinding = 0;
		smoothed_grinding = 0;
		inertia = 0;
	}
		
	
	void	Wheel::paintTrail(const float3& contact_point, const NxVec3&contact_normal)
	{
		if (active_trail && active_trail->points.count() && Vec::similar(active_trail->points.last().position.remainder,contact_point,wheel->width/5))
			return;
		if (!active_trail)
		{
			active_trail = &trails.push();
			active_trail->points.reset();
			active_trail->first_age = 0;
			active_trail->display_list = 0;
		}
		active_trail->last_age = 0;
		TTrailPoint&point = active_trail->points.append();
		point.position.remainder = contact_point;
		NxVec::convert(contact_normal,point.normal);
		point.position.sector = last_position.sector;
		point.intensity = (grinding-0.1)/0.9;
		
		if (onTrailExpand)
			onTrailExpand(this,*active_trail);
	}
	
	
	void	Wheel::closeTrail()
	{
		if (active_trail)
		{
			if (onTrailFinish)
				onTrailFinish(this,*active_trail);
			active_trail = NULL;
		}
	}
		
	void Wheel::process(const TEnvironment&env, float delta, Vehicle*vehicle, float contact_modifier)
	{
		NxWheelContactData contact;
		
		
		/*
			Variables at this point:
			delta: Time delta (in seconds) since last execution
			vehicle: Pointer to the vehicle that is calling this wheel process method as its own
			contact_modifier: intensity that acceleration, braking and grinding should be multiplied with. this value is to decrease the impact of one or more wheels being in the air.
		
		*/
		
		
		float	acceleration = wheel_status[0],
				braking = fabs(wheel_status[1]);
		
		float adopt = vmin(delta/0.1,1);	//this variable defines a smoother transition between the grip variable calculated during the last frame and the current one. transition should reach completion after at most 1/10 second
		
		
		has_contact = false;
		bool set = false;
		if (shape)
		{
			float axle_speed = -shape->getAxleSpeed();
				
		
			if (NxShape*other = shape->getContact(contact))
			{
				has_contact = true;
				TVec3<PhFloat>		point,
									normal;
				TMatrix4<PhFloat>	out_matrix,
									in_matrix;
				NxVec::mult(contact.contactPoint,(double)env.system_unit_size,point);
				NxVec::convert(contact.contactNormal,normal);
				Vec::normalize0(normal);
				Vec::mad(point,normal,wheel->radius);

				float relative_contact_force = contact.contactForce/vehicle->prototype->body_desc.mass/4;



				Vec::copy(point,last_position.remainder);
				last_position.sector = vehicle->coordinates.sector;
				
				
				Mat::transformSystem(*system,inner_system,out_matrix);			
				Mat::invertSystem(out_matrix,in_matrix);
				
				Mat::transform(in_matrix,point);
				Mat::rotate(in_matrix,normal);
				
				if (fabs(normal.y) > TypeInfo<PhFloat>::error)
					suspension->progress = clamped(Const::base_contraction_travel+(point.y)/contraction_length,0,1);		///normal.y
				
					
				
				
				
				NxMat33	inverse;
				inverse.setTransposed(shape->getGlobalPose().M);
				
				NxVec3 slide_speed;
				inverse.multiply(	vehicle->actor->getPointVelocity(contact.contactPoint)
									-
									other->getActor().getPointVelocity(contact.contactPoint),
									slide_speed);

				slide_speed.z -= shape->getAxleSpeed()*shape->getRadius();	//subtract wheel surface velocity from x (longitudal velocity)
				
				grinding = (Const::drift_slide_factor*fabs(slide_speed.x)+Const::drive_slide_factor*fabs(acceleration));
				
				float	new_xgrip = Const::transversal_grip*wheel->grip/(1.0+grinding);
				xgrip = xgrip*(1-adopt)+new_xgrip*adopt;
				
				NxTireFunctionDesc func = shape->getLateralTireForceFunction();
				func.extremumValue = xgrip*contact_modifier;
				func.asymptoteValue = func.extremumValue;
				shape->setLateralTireForceFunction(func);
				func.extremumValue = 0.02*(1.0f-braking) +xgrip*0.5;
				func.asymptoteValue = func.extremumValue;
				shape->setLongitudalTireForceFunction(func);

				//float max_torque = contact.contactForce/20.0f;
				float torque = -(acceleration)*wheel->power*0.35*contact_modifier;
				//grinding += vmax(fabs(torque) - max_torque,0)*10.0f;	//ok, this doesn't work as well as i would have hoped it would...

				shape->setMotorTorque(torque);
				shape->setBrakeTorque(sqr(inertia*Const::QuadraticRoleFriction)+vabs(inertia*Const::LinearRoleFriction)+Const::StaticRoleFriction+Const::brake_torque*braking);
				
				//-vs[2]/(2*M_PI*wheel->radius);
				
				
				NxMat33	orientation;
				orientation.setColumnMajorStride4(out_matrix.v);
				shape->setGlobalOrientation(orientation);
				
				/*if (wheel->domain->meta.flags&CGS::AnimationTargetFlag)
				{
					if (vehicle->steer_left->progress)
						shape->setSteerAngle(-35*vehicle->steer_left->progress);
					elif (vehicle->steer_right->progress)
						shape->setSteerAngle(35*vehicle->steer_right->progress);
					else
						shape->setSteerAngle(0);
				}*/
				
				was_accelerated |= acceleration!=0;
				
				//if (vabs(wheel_status[0]) > getError<double>())
				{
					//ShowOnce("posting force at "+String(wheel_status[0]));
					//vehicle->actor->addLocalForceAtPos(NxVec3(0,0,100*vabs(wheel_status[0])*wheel->power*delta), contact.contactPoint);
				
				}
				
				
				
				set = true;
				
				
				grinding = grinding/40+braking*fabs(slide_speed.z);
				grinding *= 0.5+0.5*vmin(relative_contact_force,1);
				grinding = linearStep(grinding,0.25,1);
				
				if (grinding > 0.1)
				{
					float3 trail_point;
					NxVec::mult(contact.contactPoint,env.system_unit_size,trail_point);
					paintTrail(trail_point,contact.contactNormal);
				}
				else
					closeTrail();
				
			}
			else
				grinding = 0;
			
			inertia = axle_speed/(2*M_PI);
			if (braking>=0.99)	//full stop
			{
				shape->setAxleSpeed(0);
				inertia = 0;
			}
			
		}
		else
		{
			grinding = 0;
			inertia *= vpow(0.9,delta);
		}

		
		smoothed_grinding = (smoothed_grinding*(1.0f-adopt)+grinding*adopt);
		
		
		if (!set)
		{
			suspension->progress = 0;
			xgrip = 0;
		}
		
		if (grinding<=0.1)
			closeTrail();
		
		for (QueueIterator<TTrail> it = trails.begin(); it != trails.end(); ++it)
		{
			it->first_age += delta;
			it->last_age += delta;
		}
		
		while (trails.length() && trails.oldest().last_age > trail_max_age)
		{
			if (onTrailDelete)
				onTrailDelete(this,trails.oldest());
			trails.pop();
		}
		active_trail = active_trail&&trails.length()?&trails.newest():NULL;

		
		rotation->progress += inertia*delta;
		if (rotation->progress < 0)
			rotation->progress += ceil(-rotation->progress);
		if (rotation->progress > 1)
			rotation->progress -= floor(rotation->progress);
		
		
	}
			
	Vehicle::Vehicle(const TEnvironment&env, VehiclePrototype&prototype_):prototype(&prototype_),engines(prototype_.engine_prototypes.count()),radius(prototype_.config.radius),mass(prototype_.config.mass),current_gear(0),current_motor_speed(0),acceleration(0),fuel_consumption(0),energy_consumption(0),wing_grip(0),control_grip(0),throttle(0),controls_locked(false),parked(false),has_outer_force(false)
	{
		engine_sound_volume = 1.0f;

		state.override = false;

		acceleration = 1;
		VecUnroll<EmissionTypes>::set(signature,0);
		prototype->geometry.adoptInstance(instance);
		instance.link();
		for (index_t i = 0; i < engines.count(); i++)
			engines[i].init(prototype->engine_prototypes + i);
			
		wheels.setSize(prototype->wheels.count());
		for (index_t i = 0; i < wheels.count(); i++)
		{
			wheels[i].init(prototype->wheels+i);
			wheels[i].wheel_status = instance.findStatusOfWheel(prototype->wheels[i].path);
			wheels[i].rotation = instance.findRotationAnimatorOfWheel(prototype->wheels[i].path);
			wheels[i].suspension = instance.findSuspensionAnimatorOfWheel(prototype->wheels[i].path);
			ASSERT_NOT_NULL__(wheels[i].wheel_status);
			ASSERT_NOT_NULL__(wheels[i].rotation);
			ASSERT_NOT_NULL__(wheels[i].suspension);
		}
		
		wheel_engine.init(&prototype->wheel_drive);

		
		steer_left = instance.findAnimator("steer_le");
		if (steer_left == NULL)
			steer_left = instance.findAnimator("steer_left");

		steer_right = instance.findAnimator("steer_ri");
		if (steer_right == NULL)
			steer_right = instance.findAnimator("steer_right");
		accel = instance.findAnimator("accel");
		if (accel == NULL)
			accel = instance.findAnimator("accelerate");
		brake = instance.findAnimator("brake");
		
		center_of_gravity = prototype->center_of_gravity;
		
		scene = env.scene;


		state.angular_velocity.set(0);
		state.linear_velocity.set(0);
		state.orientation = Matrix<>::eye3;
		state.position.set(0);
	}

	void Vehicle::createActor()
	{
		ASSERT_NOT_NULL__(scene);
		ASSERT__(prototype->actor_desc.isValid());
		
		actor = scene->createActor(prototype->actor_desc);
		
		cout << "threshold = "<<actor->getContactReportThreshold()<<endl;
		
		ASSERT_NOT_NULL__(actor);
		ASSERT2__(actor->getNbShapes() == 1+wheels.count(),actor->getNbShapes(),wheels.count());
		NxShape*const *	shapes = actor->getShapes();
		shapes[0]->setFlag(NX_SF_POINT_CONTACT_FORCE,true);
		for (index_t i = 0; i < wheels.count(); i++)
		{
			wheels[i].shape = (NxWheelShape*)shapes[i+1];
			ASSERT__(wheels[i].shape->is(NX_SHAPE_WHEEL));
		}
	}
	
	Vehicle::~Vehicle()
	{
		if (onVehicleErase)
			onVehicleErase(this);
		if (scene && actor)
		{
			cout << "releasing actor 0x"<<actor<<endl;
			scene->releaseActor(*actor);
		}
	
	
	}
	
	/*
	void	Vehicle::applyForce(const Composite::Coordinates&point, const PhFloat dir[3], const PhFloat&intensity)
	{
		PhFloat vert[3],rel[3];
		Composite::Coordinates	abs;
		__transform(instance.matrix,center_of_gravity,abs.position);
		abs.sector = coordinates.sector;
		sub(point,abs,rel);
		Vec::normalize0(rel);
		Vec::cross(rel,dir,vert);
		Vec::mad(momentum,vert,-intensity*100);
		Vec::mad(inertia_change,dir,intensity);
	}
	*/
	void	Vehicle::reset()
	{
		Entity::reset();

		state.linear_velocity.set(0);
		state.angular_velocity.set(0);
		state.position.set(0);
		Mat::eye(state.orientation);
		
		//momentum = 0;
		throttle = 0;
		for (index_t i = 0; i < wheels.count(); i++)
		{
			wheels[i].reset();
		}
		state.captured = timer.now();
		state.override = true;
		
		Mat::eye(instance.matrix);
		Mat::eye(accurate_matrix);
		instance.update();
	}



	
	void Vehicle::process(const TEnvironment&env, float delta)
	{
		if (state.override)
		{
			float age = timer.toSecondsf(timer.now()-state.captured);
			actor->setGlobalPosition(NxVec::convert(state.position + state.linear_velocity * age));
			actor->setLinearVelocity(NxVec::convert(state.linear_velocity));
			actor->setAngularVelocity(NxVec::convert(state.angular_velocity));
			NxMat33	sys;
			NxMat::convert(state.orientation,sys);
			Vec::copy(state.orientation.x,instance.matrix.x.xyz);
			Vec::copy(state.orientation.y,instance.matrix.y.xyz);
			Vec::copy(state.orientation.z,instance.matrix.z.xyz);
			Vec::copy(state.position,instance.matrix.w.xyz);
			//sys.id();
			actor->setGlobalOrientation(sys);
			state.override = false;
		}
		else
		{
			state.position = NxVec::convert(actor->getGlobalPosition());
			state.linear_velocity = NxVec::convert(actor->getLinearVelocity());
			state.angular_velocity = NxVec::convert(actor->getAngularVelocity());
			NxMat33	sys = actor->getGlobalOrientation();
			NxMat::convert(sys,state.orientation);
			state.captured = timer.now();
		}

		
		
		NxVec::convert(actor->getLinearVelocity(),inertia);
		velocity = inertia.length();

		
		//0.045
		//actor->setCMassOffsetLocalPosition(NxVec3(center_of_gravity[0],center_of_gravity[1]/(1.0+velocity*0.035),center_of_gravity[2]));	//lower mass center as the vehicle gets faster.
		actor->setCMassOffsetLocalPosition(NxVec3(center_of_gravity[0],center_of_gravity[1]/10.0,center_of_gravity[2]));

		
		
		//if (actor->isSleeping())
			//actor->wakeUp();
		
		
		//actor->addForceAtLocalPos(const NxVec3& force, const NxVec3& pos, NxForceMode mode = NX_FORCE, bool wakeup = true) = 0;
		
		
		VecUnroll<EmissionTypes>::set(signature,0);
		atmosphere_density = env.getAtmosphereDensity(coordinates);
		float	density = clamp(atmosphere_density,0,1),
				general_grip = sin(M_PI/(velocity/env.system_unit_size/prototype->config.optimal_speed+1))*prototype->config.optimal_maneuverability*density,
				directional = vmax((heading*inertia)/velocity,0.0);

		bool above_water = height_above_water > 0;
		
		wing_grip = general_grip*vmax(directional,0.4);
		
		control_grip = wing_grip*density + (1-density)*prototype->config.zero_maneuverability;
		
		if (velocity > TypeInfo<float>::error)
		{
			float3	local_inertia;
			Mat::rotate(invert,inertia,local_inertia);
		
			float	d = atmosphere_density;
			if (d > 1)
				d = 10;
		
			float	size = (2*M_PI*radius*radius/(env.system_unit_size*env.system_unit_size)),
					f_slow = 6*M_PI*d*radius*velocity,
					f_fast = 0.5*d*size*env.system_unit_size*sqr(velocity/env.system_unit_size),
					f_final = (f_slow + f_fast)*delta;
			
			
			
			NxVec3			friction,
							normalized = NxVec3(local_inertia.v)/velocity;
			friction.x =  -normalized.x* prototype->config.friction.x*f_final;
			friction.y =  -normalized.y* prototype->config.friction.y*f_final;
			friction.z =  -normalized.z* prototype->config.friction.z*f_final;
			friction *= wing_grip;

			NxVec3	air_induced_force = NxVec3(0,0,0);
			
			if (local_inertia.z < 0)
				air_induced_force.y -= prototype->config.forward_down_drift * fabs(local_inertia.z);
			else
				air_induced_force.y -= prototype->config.backward_down_drift * fabs(local_inertia.z);
			air_induced_force.y -= prototype->config.transveral_down_drift * fabs(local_inertia.x);

			NxVec3	center;
			NxVec::convert(center_of_gravity,center);
			actor->addLocalForceAtLocalPos(air_induced_force*delta, center);
									
			/*if (sign(local_inertia.x-friction.x) != sign(local_inertia.x))
				friction.x = local_inertia.x;
			if (sign(local_inertia.y-friction.y) != sign(local_inertia.y))
				friction.y = local_inertia.y;
			if (sign(local_inertia.z-friction.z) != sign(local_inertia.z))
				friction.z = local_inertia.z;*/
			//__rotate(system,friction);
			
			//actor->addLocalForceAtLocalPos(friction, NxVector::null);
		}

		if (has_outer_force)
		{
			NxVec3	center,force;
			NxVec::convert(center_of_gravity,center);
			//center.z -= radius*0.05f;
			center.y -= radius * 0.05f;
			NxVec::convert(outer_force,force);
			actor->addForceAtLocalPos(force*delta,center);
		}
		
		for (index_t i = 0; i < explosions.count(); i++)
		{
			TVec3<>	dir;
			Vec::sub(accurate_matrix.w.xyz,explosions[i].xyz,dir);
			float distance = Vec::length(dir);
			if (distance > 0.01)
			{
				Vec::mult(dir,explosions[i].w / (distance*sqr(1.f+distance)));
			}
			else
				Vec::def(dir,0.f,explosions[i].w,0.f);

			NxVec3	center,force;
			NxVec::convert(dir,force);
			NxVec::convert(center_of_gravity,center);
			actor->addForceAtLocalPos(force,center);
		}
	
		float acceleration = 0;
		for (index_t i = 0; i < engines.count(); i++)
		{
			Engine&engine = engines[i];
			
			
			engine.status = throttle*engine.support;
			
			if (engine.status >= 0.1)
				engine.status *= engine.type->efficiency(atmosphere_density);
			
			
			if (engine.status < 0.1)
			{
				engine.flame_size = engine.fuel_consumption = engine.energy_consumption = engine.signature_level = engine.acceleration = engine.status = 0;
				engine.heat_up_remaining = engine.type->heat_up;
			}
			else
			{
				engine.heat_up_remaining -= delta;
				if (engine.heat_up_remaining < 0)
					engine.heat_up_remaining = 0;
				if (engine.heat_up_remaining>0)
				{
					engine.acceleration = 0.1;
					engine.signature_level = 0.5;
					engine.fuel_consumption = 1;
					engine.energy_consumption = 1;
					engine.flame_size = 0.5*engine.status;
				}
				else
				{
					engine.flame_size = engine.fuel_consumption = engine.energy_consumption = engine.signature_level = engine.acceleration = engine.status;
				}
			}
			acceleration += engine.acceleration * engine.type->base_acceleration;
			
			if (engine.signature_level > TypeInfo<float>::error)
				for (index_t i = 0; i < EmissionTypes; i++)
					if (engine.type->signature[i] > TypeInfo<float>::error)
						signature[i] += vpow(engine.signature_level,engine.type->exponent[i])*engine.type->signature[i];
			fuel_consumption += engine.fuel_consumption * engine.type->fuel_consumption;
			energy_consumption += engine.energy_consumption * engine.type->energy_consumption;
		}
		
		if (wheels.count())
		{
			Engine&engine = wheel_engine;
			
			if (!engine.support)
				engine.support = 1;
			
			float t = throttle*engine.support;
			engine.status = (vpow(2.0,1.0+t)-2)/2;
			
			//engine.status = throttle*engine.support;
			
			if (engine.status >= 0.1)
				engine.status *= engine.type->efficiency(atmosphere_density);
			
			
			if (engine.status < 0.1)
			{
				engine.flame_size = engine.fuel_consumption = engine.energy_consumption = engine.signature_level = engine.acceleration = engine.status = 0;
				engine.heat_up_remaining = engine.type->heat_up;
			}
			else
			{
				engine.heat_up_remaining -= delta;
				if (engine.heat_up_remaining < 0)
					engine.heat_up_remaining = 0;
				if (engine.heat_up_remaining>0)
				{
					engine.acceleration = 0.1;
					engine.signature_level = 0.5;
					engine.fuel_consumption = 1;
					engine.energy_consumption = 1;
					engine.flame_size = 0.5*engine.status;
				}
				else
				{
					engine.flame_size = engine.fuel_consumption = engine.energy_consumption = engine.signature_level = engine.acceleration = engine.status;
				}
			}
			count_t	drives_on_ground=0,
					max_drives=0;
			
			float wheel_speed = 0;
			for (index_t j = 0; j < wheels.count(); j++)
				if (wheels[j].was_accelerated)
				{
					wheel_speed = wheel_speed*max_drives+wheels[j].inertia;
					max_drives ++;
					if (wheels[j].has_contact)
						drives_on_ground++;
				}
			
			if (wheel_engine.status > 0 && current_gear <= 0)
				current_gear = 1;
			elif (wheel_engine.status < 0)
			{
				if (current_gear >= 0)
					current_gear = -1;
			}
			
			if (current_gear)
			{
				float	linear_gear = (float)(current_gear)/(5),	//5 gear drive
						//lower = (vpow(2.0,1.0+vmax(linear_gear-0.6/5.0,0))-2)/2,
						upper = (vpow(2.0,1.0+linear_gear+0.6/5.0)-2)/2,
						current = upper*(1.0f-sqr(1.0f-wheel_engine.status));
						//lower + (upper-lower)
				if (current_gear == 2)
					current *= 0.8;
				if (accel)
					accel->progress = current*3/(0.5+0.5*vabs(current_gear));
				
				if (drives_on_ground)
				{
					current_motor_speed = vabs(wheel_speed/100)/((vpow(2.0,1.0+linear_gear*3.5)-2)/2+1);

					if (current_motor_speed > 0.9)
						current_gear++;
					elif (current_gear > 1 && current_motor_speed < 0.5)
						current_gear--;
				}
				else
				{
					current_motor_speed = wheel_engine.status;
					current_gear = 1;
				}
				
				
				
			}
			
			for (index_t j = 0; j < wheels.count(); j++)
			{
				//for (BYTE k = 0; k < 10; k++)
					wheels[j].process(env,delta,this,this->acceleration*(max_drives?(float)drives_on_ground/(float)max_drives:1));
			}
			
			if (engine.signature_level > TypeInfo<float>::error)
				for (index_t i = 0; i < EmissionTypes; i++)
					if (engine.type->signature[i] > TypeInfo<float>::error)
						signature[i] += vpow(engine.signature_level,engine.type->exponent[i])*engine.type->signature[i];
			fuel_consumption += engine.fuel_consumption * engine.type->fuel_consumption;
			energy_consumption += engine.energy_consumption * engine.type->energy_consumption;
		}
		

	
		//inertia += inertia_change;
		
		//actor->addLocalForceAtLocalPos(NxVec3(0,0,acceleration), NxVector::null);
		//inertia += z_axis*(acceleration*delta);
		
		float3	gravity;
		env.getGravityAcceleration(coordinates,gravity);
		NxVec3 xgrav;
		NxVec::convert(gravity,xgrav);
		//Vec::convert(instance.matrix+12,xpos);
		/*if (atmosphere_density > 1)
			gravity /= atmosphere_density;
		if (!wheels.count())
			gravity*=10;//dunno, don't care, seems better this way :P*/

		float updrift = vmin(directional*velocity*prototype->config.updrift,gravity.length())*delta;
		//actor->addLocalForceAtLocalPos(NxVec3(0,updrift*delta*prototype->body_desc.mass,0), NxVector::null);
		
		//ShowOnce(prototype->body_desc.mass);
		//inertia += y_axis*updrift;	//updrift
		//ShowOnce(String(xgrav.x)+", "+String(xgrav.y)+", "+String(xgrav.z));
		//actor->addForceAtLocalPos(xgrav*(delta*20*prototype->body_desc.mass), NxVector::null);
		
		
		
		//last_frame_movement = inertia*delta;
		
		NxVec3 current_position = actor->getGlobalPosition();
		float3	vec;
		NxVec::mult(current_position,env.system_unit_size,vec);
		Vec::sub(vec,coordinates.remainder);
		
		coordinates.add(vec,env.sector_size);
		
		
		/*
		NxVec3	relative_momentum;
		
		
		__rotate(invert,momentum,relative_momentum);
		float intensity = density+prototype->config.zero_maneuverability/2*(1-density);
		relative_momentum.x *= vpow(1.0f-prototype->config.rotation_friction.x*intensity,delta);
		relative_momentum.y *= vpow(1.0f-prototype->config.rotation_friction.y*intensity,delta);
		relative_momentum.z *= vpow(1.0f-prototype->config.rotation_friction.z*intensity,delta);
		relative_momentum.x += control_grip*pitch*prototype->config.rotation_speed.x*delta*2;
		relative_momentum.y += control_grip*yaw*prototype->config.rotation_speed.y*delta*2;
		relative_momentum.z -= control_grip*roll*prototype->config.rotation_speed.z*delta*2;
		
		__rotate(system,relative_momentum,momentum);
		
		float angle = momentum.length()*delta;
		if (angle > getError<float>())
		{
		
			Vec3<float>	normalized = momentum.normalized();
			float rotation_matrix[3*3];
			__rotationMatrix(angle, normalized, rotation_matrix);
			__mult331(rotation_matrix,z_axis);
			__mult331(rotation_matrix,y_axis);
		}
		
		Vec3<float>	movement_direction = inertia.normalized0(),
						raxis = z_axis | y_axis;
		//Vec::interpolate(z_axis,movement_direction,general_grip*delta*2,z_axis);
		heading = z_axis;
		z_axis.normalize0();
		y_axis = raxis | z_axis;
		y_axis.normalize0();*/
		
		NxMat34 sys = actor->getGlobalPose();
		sys.M.getColumnMajorStride4(system.v);
		
		
		
		Vec::mult(system.w.xyz,env.system_unit_size);
		
		
		//Vec::convert(sys.t,system+12);
		//Vec::c3(system,x_axis);
		y_axis = system.y.xyz;
		z_axis = system.z.xyz;
		
		
		
		
		updateSystem();
		
		Mat::copyOrientation(system,accurate_matrix);
		//instance.update();
		
		
		
		
		/*
		tested_faces.reset();
		tested_sectors.reset();
		
		if (detectGround(env,delta) && height_above_ground < radius*1.5)
		{
			inertia *= vpow(0.5,delta);
			float indent = -ground_normal*inertia;
			if (indent > 0)
			{
				inertia += ground_normal*indent;
				impact(coordinates,env.system_unit_size,indent);
			}
			if (height_above_ground<radius*0.5)
				coordinates += ground_normal * (radius-height_above_ground);
			else
				inertia += ground_normal * (mass/100*env.system_unit_size*sqr(1.0-(height_above_ground/radius-0.5 )))*0.5;
		}

		
		
		
		
		
		
		
		
		velocity = inertia.length();

		
		Vec::c11(system,instance.matrix);
		//instance.update();
		*/
		if (above_water != height_above_water > 0)
		{
			sound(coordinates,env.system_unit_size,velocity/10,SoundInstance::splash_sound);
		}

		
	
		
	}

	void Vehicle::featherAccurateMatrix(float delta)
	{
		float progression = std::min(delta*10.f,1.f);
		float preservation = 1.f - progression;
			//pow(0.00001f,delta);

		Vec::mad(instance.matrix.w.xyz,inertia,delta);

		VecUnroll<16>::interpolate(accurate_matrix.v,instance.matrix.v,preservation,instance.matrix.v);
		//Mat::copy(accurate_matrix,instance.matrix);


		Mat::resetBottomRow(instance.matrix);
		instance.update();
	}

	
	static void linearize(CGS::SubGeometryA<>&sub,Buffer<CGS::SubGeometryA<>*>&objects, bool visible_only)
	{
		if (!visible_only || (sub.vs_hull_field.length() && sub.vs_hull_field.first().vertex_field.length()))
			objects << &sub;
		for (index_t i = 0; i < sub.child_field.length(); i++)
			linearize(sub.child_field[i],objects,visible_only);
	}
	static void linearize(const CGS::SubGeometryA<>&sub,Buffer<const CGS::SubGeometryA<>*>&objects, bool visible_only)
	{
		if (!visible_only || (sub.vs_hull_field.length() && sub.vs_hull_field.first().vertex_field.length()))
			objects << &sub;
		for (index_t i = 0; i < sub.child_field.length(); i++)
			linearize(sub.child_field[i],objects,visible_only);
	}
	

	bool StructurePrototype::loadFromFile(const TEnvironment&env_, const String&filename, bool dynamic)
	{
		if (!geometry.loadFromFile(filename.c_str()))
			return false;
		adopt(env_,geometry,dynamic);
		return true;
	}
	
	void StructurePrototype::adopt(const TEnvironment&env, CGS::Geometry<>&geometry_,bool dynamic)
	{
		this->env = env;
		this->geometry.adoptData(geometry_);
		if (dynamic)
		{
			actor_desc.group = ActorGroup::DynamicMesh;
			actor_desc.body = &body_desc;
		}
		else
			actor_desc.group = ActorGroup::StaticMesh;
		if (!nearingOne(env.system_unit_size))
			geometry.scale(env.system_unit_size);
		updateFromGeometry(geometry,dynamic);
	}

	void StructureDescriptor::updateFromHull(const Mesh<CGS::SubGeometryA<>::PhDef>&hull, bool dynamic)
	{
		logfile << "creating structure from hull"<<nl;
		
		//bool dynamic = actor_desc.body != NULL;
		if (!hull_material[dynamic])
		{
			logfile << "Creating hull material..."<<nl;
			NxMaterialDesc desc;
			desc.dynamicFriction = 0.1+0.4*dynamic;
			desc.staticFriction = 0.2+0.8*dynamic;
			desc.restitution = 0.4;
			desc.frictionCombineMode = NX_CM_MULTIPLY;
			hull_material[dynamic] = env.scene->createMaterial(desc);
		}
		
		for (index_t i = 0; i < hull_desc.count(); i++)
		{
			if (hull_desc[i].meshData)
				env.scene->getPhysicsSDK().releaseTriangleMesh(*hull_desc[i].meshData);
		}
		
		hull_desc.setSize(1);
		
		actor_desc.shapes.clear();
		
		Array<NxVec3>					hull_vertices;
		Array<NxU32>					hull_indices;
		NxWriteBuffer 					buf;
		body_desc.mass = 0;
		{

			count_t num_vertices = 0,
					num_triangles = 0;
			PhFloat	volume=0;

			if (hull.vertex_field.length() > hull_vertices.count())
				hull_vertices.setSize(hull.vertex_field.length());
			count_t indices = hull.triangle_field.length()*3+hull.quad_field.length()*6;
			if (indices > hull_indices.count())
				hull_indices.setSize(indices);
			logfile << " fields adjusted to "<<hull_vertices.count()<<"/"<<hull_indices.count()<<nl;
			
			for (index_t j = 0; j < hull.vertex_field.length(); j++)
				NxVec::div(hull.vertex_field[j].position,env.system_unit_size,hull_vertices[j]);
			logfile << " vertices converted"<<nl;
			NxU32*index = hull_indices.pointer();
			for (index_t j = 0; j < hull.triangle_field.length(); j++)
			{
				(*index++) = static_cast<NxU32>(hull.triangle_field[j].v0-hull.vertex_field);
				(*index++) = static_cast<NxU32>(hull.triangle_field[j].v1-hull.vertex_field);
				(*index++) = static_cast<NxU32>(hull.triangle_field[j].v2-hull.vertex_field);
			}
			for (index_t j = 0; j < hull.quad_field.length(); j++)
			{
				(*index++) = static_cast<NxU32>(hull.quad_field[j].v0-hull.vertex_field);
				(*index++) = static_cast<NxU32>(hull.quad_field[j].v1-hull.vertex_field);
				(*index++) = static_cast<NxU32>(hull.quad_field[j].v2-hull.vertex_field);
				
				(*index++) = static_cast<NxU32>(hull.quad_field[j].v0-hull.vertex_field);
				(*index++) = static_cast<NxU32>(hull.quad_field[j].v2-hull.vertex_field);
				(*index++) = static_cast<NxU32>(hull.quad_field[j].v3-hull.vertex_field);
			}
			
			ASSERT__(index-hull_indices == indices);
			logfile << " indices converted"<<nl;


			num_vertices = hull.vertex_field.length();
			num_triangles = hull.triangle_field.length()+hull.quad_field.length()*2;
			if (dynamic)
				volume = hull.getVolume();

				
			
			NxTriangleMeshDesc mesh_desc;
			mesh_desc.numVertices = static_cast<NxU32>(num_vertices);
			mesh_desc.numTriangles = static_cast<NxU32>(num_triangles);
			mesh_desc.pointStrideBytes = sizeof(NxVec3);
			mesh_desc.triangleStrideBytes = sizeof(NxU32)*3;
			
			mesh_desc.points = hull_vertices.pointer();
			mesh_desc.triangles = hull_indices.pointer();
			mesh_desc.flags = 0;
			
			ASSERT__(mesh_desc.isValid());
			
			NxMat::convert(Matrix<>::eye4,hull_desc[0].localPose);
		
			
			logfile << " pose converted. Now cooking..."<<nl;
			
			buf.reset();
			
			if(CookTriangleMesh(mesh_desc, buf)) 
			{
				logfile << " cooking successful ("<<buf.length()<<" bytes written)"<<nl;
				hull_desc[0].meshData = env.scene->getPhysicsSDK().createTriangleMesh(NxReader(buf));
				hull_desc[0].materialIndex = hull_material[dynamic]->getMaterialIndex();
				logfile << " mesh created (0x"<<hull_desc[0].meshData<< "). appending"<<nl;
				if(hull_desc[0].meshData != NULL) 
					actor_desc.shapes.pushBack(&hull_desc[0]);
				else
					FATAL__("Failed to create hull");
			}
			else
			{
				hull_desc[0].meshData = NULL;
				if (hasCookingLibrary())
					FATAL__("Failed to cook hull")
				else
					FATAL__("Cooking library not available");
			}

			if (dynamic)
			{
				logfile << " Calculating volume..."<<nl;
				PhFloat vol = volume/cubic(env.system_unit_size);
				logfile << " Volume is "<<vol<<nl;
				body_desc.mass += vol;
			}
		}
		
		
		if (dynamic)
		{
			body_desc.mass*=1000;
			body_desc.sleepEnergyThreshold = 0.05;
		}
		
		
		ASSERT__(actor_desc.isValid());
		logfile << "Description is valid. Prototype created..."<<nl;
	}

	void StructureDescriptor::updateFromGeometry(const CGS::Geometry<>&geometry, bool dynamic)
	{
		logfile << "creating structure from geometry"<<nl;
		
		//bool dynamic = actor_desc.body != NULL;
		Buffer<const CGS::SubGeometryA<>*>	objects;
		for (index_t i = 0; i < geometry.object_field.length(); i++)
			linearize(geometry.object_field[i],objects,true);
		logfile << "linearized to "<<objects.count()<<" visual object(s)"<<nl;

		ASSERT__(geometry.object_field.count()>0);
		ASSERT__(objects.count()>0);
		if (!hull_material[dynamic])
		{
			logfile << "Creating hull material..."<<nl;
			NxMaterialDesc desc;
			desc.dynamicFriction = 0.1+0.4*dynamic;
			desc.staticFriction = 0.2+0.8*dynamic;
			desc.restitution = 0.4;
			desc.frictionCombineMode = NX_CM_MULTIPLY;
			hull_material[dynamic] = env.scene->createMaterial(desc);
		}
		
		for (index_t i = 0; i < hull_desc.count(); i++)
		{
			if (hull_desc[i].meshData)
				env.scene->getPhysicsSDK().releaseTriangleMesh(*hull_desc[i].meshData);
		}
		
		hull_desc.setSize(objects.count());
		
		actor_desc.shapes.clear();
		
		Array<NxVec3>					hull_vertices;
		Array<NxU32>					hull_indices;
		NxWriteBuffer 					buf;
		body_desc.mass = 0;
		for (index_t i = 0; i < objects.count(); i++)
		{
			logfile << "processing object #"<<i<<nl;

			count_t num_vertices = 0,
					num_triangles = 0;
			PhFloat	volume=0;

			if (objects[i]->ph_hull.isEmpty())
			{
				logfile << "physical hull is empty - using visual hull (0) "<<nl;
				const Mesh<CGS::SubGeometryA<>::VsDef>&hull = objects[i]->vs_hull_field.first();
				if (hull.vertex_field.length() > hull_vertices.count())
					hull_vertices.setSize(hull.vertex_field.length());
				count_t indices = hull.triangle_field.length()*3+hull.quad_field.length()*6;
				if (indices > hull_indices.count())
					hull_indices.setSize(indices);
				logfile << " fields adjusted to "<<hull_vertices.count()<<"/"<<hull_indices.count()<<nl;
			
				for (index_t j = 0; j < hull.vertex_field.length(); j++)
					NxVec::div(hull.vertex_field[j].position,env.system_unit_size,hull_vertices[j]);
				logfile << " vertices converted"<<nl;
				NxU32*index = hull_indices.pointer();
				for (index_t j = 0; j < hull.triangle_field.length(); j++)
				{
					(*index++) = static_cast<NxU32>(hull.triangle_field[j].v0-hull.vertex_field);
					(*index++) = static_cast<NxU32>(hull.triangle_field[j].v1-hull.vertex_field);
					(*index++) = static_cast<NxU32>(hull.triangle_field[j].v2-hull.vertex_field);
				}
				for (index_t j = 0; j < hull.quad_field.length(); j++)
				{
					(*index++) = static_cast<NxU32>(hull.quad_field[j].v0-hull.vertex_field);
					(*index++) = static_cast<NxU32>(hull.quad_field[j].v1-hull.vertex_field);
					(*index++) = static_cast<NxU32>(hull.quad_field[j].v2-hull.vertex_field);
				
					(*index++) = static_cast<NxU32>(hull.quad_field[j].v0-hull.vertex_field);
					(*index++) = static_cast<NxU32>(hull.quad_field[j].v2-hull.vertex_field);
					(*index++) = static_cast<NxU32>(hull.quad_field[j].v3-hull.vertex_field);
				}
			
				ASSERT__(index-hull_indices == indices);
				logfile << " indices converted"<<nl;
				num_vertices = hull.vertex_field.length();
				num_triangles = hull.triangle_field.length()+hull.quad_field.length()*2;

				if (dynamic)
					volume = hull.getVolume();
			}
			else
			{
				logfile << "physical hull is not empty - using physical hull "<<nl;
				const Mesh<CGS::SubGeometryA<>::PhDef>&hull = objects[i]->ph_hull;
				if (hull.vertex_field.length() > hull_vertices.count())
					hull_vertices.setSize(hull.vertex_field.length());
				count_t indices = hull.triangle_field.length()*3+hull.quad_field.length()*6;
				if (indices > hull_indices.count())
					hull_indices.setSize(indices);
				logfile << " fields adjusted to "<<hull_vertices.count()<<"/"<<hull_indices.count()<<nl;
			
				for (index_t j = 0; j < hull.vertex_field.length(); j++)
					NxVec::div(hull.vertex_field[j].position,env.system_unit_size,hull_vertices[j]);
				logfile << " vertices converted"<<nl;
				NxU32*index = hull_indices.pointer();
				for (index_t j = 0; j < hull.triangle_field.length(); j++)
				{
					(*index++) = static_cast<NxU32>(hull.triangle_field[j].v0-hull.vertex_field);
					(*index++) = static_cast<NxU32>(hull.triangle_field[j].v1-hull.vertex_field);
					(*index++) = static_cast<NxU32>(hull.triangle_field[j].v2-hull.vertex_field);
				}
				for (index_t j = 0; j < hull.quad_field.length(); j++)
				{
					(*index++) = static_cast<NxU32>(hull.quad_field[j].v0-hull.vertex_field);
					(*index++) = static_cast<NxU32>(hull.quad_field[j].v1-hull.vertex_field);
					(*index++) = static_cast<NxU32>(hull.quad_field[j].v2-hull.vertex_field);
				
					(*index++) = static_cast<NxU32>(hull.quad_field[j].v0-hull.vertex_field);
					(*index++) = static_cast<NxU32>(hull.quad_field[j].v2-hull.vertex_field);
					(*index++) = static_cast<NxU32>(hull.quad_field[j].v3-hull.vertex_field);
				}
			
				ASSERT__(index-hull_indices == indices);
				logfile << " indices converted"<<nl;


				num_vertices = hull.vertex_field.length();
				num_triangles = hull.triangle_field.length()+hull.quad_field.length()*2;
				if (dynamic)
					volume = hull.getVolume();

			}
				
			
			NxTriangleMeshDesc mesh_desc;
			mesh_desc.numVertices = static_cast<NxU32>(num_vertices);
			mesh_desc.numTriangles = static_cast<NxU32>(num_triangles);
			mesh_desc.pointStrideBytes = sizeof(NxVec3);
			mesh_desc.triangleStrideBytes = sizeof(NxU32)*3;
			
			mesh_desc.points = hull_vertices.pointer();
			mesh_desc.triangles = hull_indices.pointer();
			mesh_desc.flags = 0;
			
			ASSERT__(mesh_desc.isValid());
			
			NxMat::convert(objects[i]->path,hull_desc[i].localPose);
			
			
			hull_desc[i].localPose.t.x /= env.system_unit_size;
			hull_desc[i].localPose.t.y /= env.system_unit_size;
			hull_desc[i].localPose.t.z /= env.system_unit_size;
			
			
			
			logfile << " pose converted. Now cooking..."<<nl;
			
			buf.reset();
			
			if(CookTriangleMesh(mesh_desc, buf)) 
			{
				logfile << " cooking successful ("<<buf.length()<<" bytes written)"<<nl;
				hull_desc[i].meshData = env.scene->getPhysicsSDK().createTriangleMesh(NxReader(buf));
				hull_desc[i].materialIndex = hull_material[dynamic]->getMaterialIndex();
				logfile << " mesh created (0x"<<hull_desc[i].meshData<< "). appending"<<nl;
				if(hull_desc[i].meshData != NULL) 
					actor_desc.shapes.pushBack(hull_desc+i);
				else
					FATAL__("Failed to create hull");
			}
			else
			{
				hull_desc[i].meshData = NULL;
				if (hasCookingLibrary())
					FATAL__("Failed to cook hull")
				else
					FATAL__("Cooking library not available");
			}

			if (dynamic)
			{
				logfile << " Calculating volume..."<<nl;
				PhFloat vol = volume/cubic(env.system_unit_size);
				logfile << " Volume is "<<vol<<nl;
				body_desc.mass += vol;
			}
		}
		
		
		if (dynamic)
		{
			body_desc.mass*=1000;
			body_desc.sleepEnergyThreshold = 0.05;
		}
		
		
		ASSERT__(actor_desc.isValid());
		logfile << "Description is valid. Prototype created..."<<nl;
	}
	

	void	UniqueStaticStructure::updateFromGeometry(const CGS::Geometry<>&geometry)
	{
		/*Vec::copy(geometry.root_system.matrix.w.xyz,coordinates.remainder);
		Mat::copyOrientation(geometry.root_system.matrix,orientation);*/

		if (env.scene)
		{
			if (actor)
			{
				cout << "releasing actor 0x"<<actor<<endl;
				env.scene->releaseActor(*actor);
			}

			actor = NULL;
			
			StructureDescriptor::updateFromGeometry(geometry,false);
			
			if (installed)
			{
				actor = env.scene->createActor(actor_desc);
				cout << "created actor 0x"<<actor<<endl;
			}

			updatePose();
		}
	}
	
	void	UniqueStaticStructure::updateFromHull(const Mesh<CGS::SubGeometryA<>::PhDef>&hull)
	{
		/*Vec::copy(geometry.root_system.matrix.w.xyz,coordinates.remainder);
		Mat::copyOrientation(geometry.root_system.matrix,orientation);*/

		if (env.scene)
		{
			if (actor)
			{
				cout << "releasing actor 0x"<<actor<<endl;
				env.scene->releaseActor(*actor);
			}

			actor = NULL;
			
			StructureDescriptor::updateFromHull(hull,false);
			
			if (installed)
			{
				actor = env.scene->createActor(actor_desc);
				cout << "created actor 0x"<<actor<<endl;
			}

			updatePose();
		}
	}
	void	TrackSegment::update()
	{
		ASSERT_NOT_NULL__(tile);
		
		
		bend(*tile,bent_geometry);
		
		if (!nearingOne(env.system_unit_size))
			bent_geometry.scale(env.system_unit_size);
		
		bent_geometry.resetSystems();

		UniqueStaticStructure::updateFromGeometry(bent_geometry);
		/*
		if (env.scene)
		{
			if (actor)
				env.scene->releaseActor(*actor);
			actor = NULL;
			
			StructureDescriptor::updateFromGeometry(bent_geometry);
			
			if (installed)
				actor = env.scene->createActor(actor_desc);
		}*/
	}
	
	
	void	UniqueStaticStructure::install()
	{
		if (installed)
			return;
		
		if (!actor && env.scene)
		{
			actor = env.scene->createActor(actor_desc);
		}
		installed = !env.scene || actor;
		if (installed)
		{

			installed_unique_static_structures << this;
		}
	}

	void		UniqueStaticStructure::adoptData(UniqueStaticStructure&other)
	{
		enqueued = other.enqueued;
		if (other.enqueued)
			other.removeFromQueue();
		if (other.installed)
		{
			installed_unique_static_structures.findAndErase(&other);
		}
		ASSERT__(!other.enqueued);
		StructureDescriptor::adoptData(other);
		installed = other.installed;
		coordinates = other.coordinates;
		orientation = other.orientation;
		actor = other.actor;
		sector_translation = other.sector_translation;

		if (enqueued)
			enqueue(true);
		if (installed)
			installed_unique_static_structures << this;

		other.actor = NULL;
		other.installed = false;
	}
			


	
	void	UniqueStaticStructure::uninstall()
	{
		if (!installed)
			return;
		if (actor)
		{
			cout << "releasing actor 0x"<<actor<<endl;
			env.scene->releaseActor(*actor);
		}
		actor = NULL;
		installed_unique_static_structures.findAndErase(this);
		installed = false;
	}


	
	void		Track::exportTo(Array<TrackSegment>&segments,float sector_size, bool adjust_up_vectors)
	{
		count_t cnt = nodes.count()-!closed;
		if (!nodes.count() || cnt<=1)
		{
			segments.free();
			return;
		}
		for (index_t i = 0; i < nodes.count(); i++)
			if ((i&&i+1<nodes.count()) || closed)
			{
				index_t	prev = i?i-1:nodes.count()-1,
						next = (i+1)%nodes.count();
				
				Composite::sub(nodes[next].coordinates,nodes[prev].coordinates,nodes[i].direction,sector_size);
				nodes[i].scale_direction[0] = nodes[next].width_scale-nodes[prev].width_scale;
				nodes[i].scale_direction[1] = nodes[next].height_scale-nodes[prev].height_scale;
				Vec::div(nodes[i].direction,4.5);
				Vec::div(nodes[i].scale_direction,4.5);
				
				
			}
			
		if (!closed)
		{
			Composite::sub(nodes[1].coordinates,nodes[0].coordinates,nodes[0].direction,sector_size);
			nodes[0].scale_direction[0] = nodes[1].width_scale-nodes[0].width_scale;
			nodes[0].scale_direction[1] = nodes[1].height_scale-nodes[0].height_scale;
			Vec::div(nodes[0].direction,4.5);
			Vec::div(nodes[0].scale_direction,4.5);
			
			
			float3 help;
			Vec::cross(nodes[0].direction,nodes[0].up,help);
			Vec::cross(nodes[0].up,help,nodes[0].direction);
			Vec::setLen(nodes[0].direction,Composite::distance(nodes[0].coordinates,nodes[1].coordinates,sector_size)/2.5);
			
			
			//nodes[0].direction = (nodes[0].direction-nodes[1].direction)*2;
		
			Composite::sub(nodes.last().coordinates,nodes[nodes.count()-2].coordinates,nodes.last().direction,sector_size);
			nodes.last().scale_direction[0] = nodes.last().width_scale-nodes[nodes.count()-2].width_scale;
			nodes.last().scale_direction[1] = nodes.last().height_scale-nodes[nodes.count()-2].height_scale;
			Vec::div(nodes.last().direction,4.5);
			Vec::div(nodes.last().scale_direction,4.5);
			
			Vec::cross(nodes.last().direction,nodes.last().up,help);
			Vec::cross(nodes.last().up,help,nodes.last().direction);
			Vec::setLen(nodes.last().direction,Composite::distance(nodes[nodes.count()-2].coordinates,nodes.last().coordinates,sector_size)/2.5);
			
			//nodes.last().direction = (nodes.last().direction-nodes[nodes.count()-2].direction)*2;
		}

		if (adjust_up_vectors)
			for (index_t i = 0; i < nodes.count(); i++)
			{
				float3 help;
				Vec::cross(nodes[i].direction,nodes[i].up,help);
				Vec::cross(help,nodes[i].direction,nodes[i].up);
				Vec::normalize0(nodes[i].up);
			}
	
		segments.setSize(cnt);
		for (index_t i = 0; i < cnt; i++)
		{
			const TNode	&n0 = nodes[i],
						&n1 = nodes[(i+1)%nodes.count()];
			TrackSegment&segment = segments[i];
			
			segment.end_point[0].coordinates = n0.coordinates;
			segment.end_point[1].coordinates = n1.coordinates;
			
			segment.end_point[1].coordinates.relativateToSector(segment.end_point[0].coordinates.sector,sector_size);
			
			
			
			
			segment.end_point[0].scale.x = n0.width_scale;
			segment.end_point[0].scale.y = n0.height_scale;
			segment.end_point[1].scale.x = n1.width_scale;
			segment.end_point[1].scale.y = n1.height_scale;
			
			Vec::add(segment.end_point[0].scale,n0.scale_direction,segment.end_point[0].scale_control);
			Vec::sub(segment.end_point[1].scale,n1.scale_direction,segment.end_point[1].scale_control);
			Vec::add(segment.end_point[0].coordinates.remainder,n0.direction,segment.end_point[0].position_control);
			Vec::sub(segment.end_point[1].coordinates.remainder,n1.direction,segment.end_point[1].position_control);
			segment.end_point[0].up = n0.up;
			segment.end_point[1].up = n1.up;
		}
	}


	StructureDescriptor::~StructureDescriptor()
	{
		for (index_t i = 0; i < hull_desc.count(); i++)
		{
			if (hull_desc[i].meshData)
				env.scene->getPhysicsSDK().releaseTriangleMesh(*hull_desc[i].meshData);
		}
	}

	Structure::Structure():descriptor(NULL),env(NULL),sector_translation(0)
	{}

	Structure::Structure(StructurePrototype&prototype_):descriptor(NULL),env(NULL),sector_translation(0)
	{
		create(prototype_);
	}

	Structure&	Structure::create(StructurePrototype&prototype_)
	{
		if (env && env->scene && actor)
		{
			cout << "releasing actor 0x"<<actor<<endl;
			env->scene->releaseActor(*actor);
		}

		env = &prototype_.env;
		descriptor = &prototype_;

		prototype_.geometry.adoptInstance(instance);
		logfile << "adopted"<<nl;
		ASSERT_NOT_NULL__(env->scene);
		instance.link();
		logfile << "linked. creating actor"<<nl;
		ASSERT__(descriptor->actor_desc.isValid());
		
		actor = env->scene->createActor(descriptor->actor_desc);
		cout << "internal actor creation 0x"<<actor<<endl;
		if (descriptor->actor_desc.body != NULL && actor->getNbShapes())
		{
			actor->getShapes()[0]->setFlag(NX_SF_POINT_CONTACT_FORCE,true);
		}
		ASSERT_NOT_NULL__(actor);
		logfile << "Actor created. All done"<<nl;
		return *this;
	}
	
	Structure::~Structure()
	{
		if (env && env->scene && actor)
		{
			cout << "releasing actor 0x"<<actor<<endl;
			env->scene->releaseActor(*actor);
		}
	}
	
	void			Structure::process()
	{
		if (changed)
		{
			NxMat34	pose;
			NxMat::convert(system,pose);
			
			
			TVec3<PhFloat> sys_translation;
			Vec::add(sector_translation,coordinates.remainder,sys_translation);
			NxVec::convert(sys_translation,pose.t);
			
			pose.t.x /= env->system_unit_size;
			pose.t.y /= env->system_unit_size;
			pose.t.z /= env->system_unit_size;
			
			actor->setGlobalPose(pose);
			changed = false;
		}
		NxVec3 current_position = actor->getGlobalPosition();
		float3	vec;
		NxVec::convert(current_position,vec);
		Vec::sub(vec,coordinates.remainder);
		
		coordinates.add(vec,env->sector_size);
		
		NxMat34 sys = actor->getGlobalPose();
		sys.M.getColumnMajorStride4(system.v);
		//Vec::convert(sys.t,system+12);
		//Vec::c3(system,x_axis);
		y_axis = system.y.xyz;
		z_axis = system.z.xyz;
		
		Vec::mult(system.w.xyz,env->system_unit_size);
		
		updateSystem();
		
		Mat::copyOrientation(system,instance.matrix);
	}
	
	List::ReferenceVector<Structure>			moved_static_structures;
	Buffer<UniqueStaticStructure*>				installed_unique_static_structures;		//!< Linear list of all currently simulated simple structures
	Buffer<UniqueStaticStructure*>				moved_unique_static_structures;		//!< Linear list of all currently simulated simple structures

	
	void 	Structure::update()
	{
		changed = true;
		if (!descriptor->actor_desc.body)	//=> not dynamic
			moved_static_structures.append(this);
	}

	void			UniqueStaticStructure::applyPose()
	{
		ASSERT__(enqueued);
		enqueued = false;
		NxMat34	pose;
		NxMat::convert(orientation,pose.M);
		NxVec::convert(coordinates.remainder,pose.t);
			
			
		TVec3<PhFloat> sys_translation;
		Vec::add(sector_translation,coordinates.remainder,sys_translation);
		NxVec::convert(sys_translation,pose.t);
			
		pose.t.x /= env.system_unit_size;
		pose.t.y /= env.system_unit_size;
		pose.t.z /= env.system_unit_size;
			
		actor->setGlobalPose(pose);
	}

	void 	UniqueStaticStructure::updatePose()
	{
		if (!actor_desc.body && !enqueued)
		{
			enqueue(false);
		}
	}

	void	UniqueStaticStructure::removeFromQueue()
	{
		ASSERT__(enqueued);
		moved_unique_static_structures.findAndErase(this);
		enqueued = false;
	}
	void	UniqueStaticStructure::enqueue(bool ignore_state)
	{
		ASSERT__(ignore_state || !enqueued);
		moved_unique_static_structures.append(this);
		enqueued = true;
	}



	HashTable<EngineType*>				engine_type_map;
	List::Vector<EngineType>			engine_types;
	List::ReferenceVector<Vehicle>		simulated_vehicles,
										simulated_vehicle_waste,
										newly_created_vehicles;
	List::ReferenceVector<Structure>	dynamic_structures,
										static_structures;
	//MappedScenery<OpenGL>						scenery;
	static List::Vector<VehiclePrototype>	local_prototypes;
	HashTable<VehiclePrototype*>		prototypes;
	float								trail_length(1);
	List::ReferenceVector<FirstPerson>	simulated_fps;
	List::Vector<StructurePrototype>	structure_prototypes;
	HashTable<StructurePrototype*>		structure_prototype_map;
	

	
	EngineType*							createEngineType(const String&name)
	{
		EngineType*result = engine_types.append();
		result->name = name;
		engine_type_map.set(name,result);
		return result;
	}
	
	namespace
	{
		bool	query_debug_information = false;
		const NxDebugRenderable*				debug_renderable = NULL;
	}



	const NxDebugRenderable*						getDebugRenderable()
	{
		return debug_renderable;
	}

	NxScene*								init(bool generate_debug_info)
	{
		EngineType*	engine;


		query_debug_information = generate_debug_info;
		
		
		
		engine = createEngineType("ion");
		engine->base_acceleration = 50000;
		Vec::def(engine->optimal_pressure_window,0,0.1);
		Vec::def(engine->minimal_pressure_window,0,0.7);
		Vec::def(engine->zero_efficiency_window,0.3,1);
		Vec::def(engine->flame_color,0,0.5,1);
		engine->heat_up = 0.5;
		engine->energy_consumption = 1;
		engine->fuel_consumption = 0;
		engine->signature[Noise] = 0.1;
		engine->signature[Heat] = 0.1;
		engine->signature[Light] = 1;
		engine->signature[EM] = 1;
		engine->signature[Exhaust] = 0;
		engine->exponent[Noise] = 2;
		engine->exponent[Heat] = 2;
		engine->exponent[Light] = 1;
		engine->exponent[EM] = 2;
		engine->exponent[Exhaust] = 1;
		engine->sound = OpenAL::load("sounds/thruster.wav");
		
		engine = createEngineType("combustion");
		engine->base_acceleration = 2000;
		Vec::def(engine->optimal_pressure_window,0.8,1);
		Vec::def(engine->minimal_pressure_window,0.3,1);
		Vec::def(engine->zero_efficiency_window,0,0.7);
		Vec::def(engine->flame_color,1,0.5,0);
		engine->heat_up = 1.5;
		engine->energy_consumption = 0;
		engine->fuel_consumption = 1;
		engine->signature[Noise] = 1.0;
		engine->signature[Heat] = 1.0;
		engine->signature[Light] = 0.4;
		engine->signature[EM] = 0.1;
		engine->signature[Exhaust] = 0.8;
		engine->exponent[Noise] = 2;
		engine->exponent[Heat] = 2;
		engine->exponent[Light] = 2;
		engine->exponent[EM] = 2;
		engine->exponent[Exhaust] = 2;
		engine->sound = OpenAL::load("sounds/combustion.wav");
		
		engine = createEngineType("crawler");
		engine->base_acceleration = 1000;
		Vec::def(engine->optimal_pressure_window,1,1000);
		Vec::def(engine->minimal_pressure_window,1,100000);
		Vec::def(engine->zero_efficiency_window,-1,-0.01);
		Vec::def(engine->flame_color,0,0,0);
		engine->heat_up = 0.1;
		engine->energy_consumption = 1;
		engine->fuel_consumption = 0;
		engine->signature[Noise] = 1.0;
		engine->signature[Heat] = 0.2;
		engine->signature[Light] = 0;
		engine->signature[EM] = 0.1;
		engine->signature[Exhaust] = 0;
		engine->exponent[Noise] = 2;
		engine->exponent[Heat] = 2;
		engine->exponent[Light] = 2;
		engine->exponent[EM] = 2;
		engine->exponent[Exhaust] = 2;
		engine->sound = OpenAL::load("sounds/crawler.wav");
		
		engine = createEngineType("wheel_drive");
		engine->base_acceleration = 10;	//mostly ignored
		Vec::def(engine->optimal_pressure_window,1,1000);
		Vec::def(engine->minimal_pressure_window,0,100000);
		Vec::def(engine->zero_efficiency_window,1000,1000);	//not accepting thrusters
		Vec::def(engine->flame_color,0,0,0);
		engine->heat_up = 0;			//instantly available
		engine->energy_consumption = 1;	//consumes energy, not fuel
		engine->fuel_consumption = 0;
		engine->signature[Noise] = 1.0;
		engine->signature[Heat] = 0.2;
		engine->signature[Light] = 0;
		engine->signature[EM] = 0.1;
		engine->signature[Exhaust] = 0.1;
		engine->exponent[Noise] = 2;
		engine->exponent[Heat] = 2;
		engine->exponent[Light] = 2;
		engine->exponent[EM] = 2;
		engine->exponent[Exhaust] = 2;
		engine->sound = OpenAL::load("sounds/engine.wav");
		
		
		
		SoundInstance::impact_sound = OpenAL::load("sounds/impact.wav");
		SoundInstance::splash_sound = OpenAL::load("sounds/splash.wav");
		
		/*if (SoundInstance::impact_sound.isEmpty())
			FATAL__("Failed to load physics wave impact.wav");*/
		
		
		
				
		
		NxPhysicsSDKDesc desc;
		NxSDKCreateError errorCode = NXCE_NO_ERROR;
		gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, &error_out, desc, &errorCode);
		if(gPhysicsSDK == NULL) 
		{
			ErrMessage("SDK creation error ("+String(errorCode)+").\nUnable to initialize PhysX SDK.");
			return NULL;
		}

		if (!InitCooking(NULL, NULL))
		{
			ErrMessage("Error: Unable to initialize the cooking library.");
			return NULL;
		}

		gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.035f);//0.025
		//enable visualisation
		gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1.0f);

		/*gPhysicsSDK->setParameter(NX_VISUALIZE_BODY_AXES, 0.0f);
		gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 0.0f);
		gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_AXES, 0.0f);
		gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_POINT, 0.0f);
		gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_NORMAL, 0.0f);

		gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 0.0f);
		gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_WORLD_AXES, 0.0f);
		gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS, 0.0f);*/


		gPhysicsSDK->setParameter(NX_VISUALIZE_BODY_AXES, generate_debug_info);
		gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, generate_debug_info);
		gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_AXES, generate_debug_info);
		gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_POINT, generate_debug_info);
		gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_NORMAL, generate_debug_info);

		gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, generate_debug_info);
		gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_WORLD_AXES, generate_debug_info);
		gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS, generate_debug_info);
		// Don't slow down jointed objects
		//gPhysicsSDK->setParameter(NX_ADAPTIVE_FORCE, 0.0f);
		
		//create some materials -- note that we reuse the same NxMaterial every time,
		//as it gets copied to the SDK with every setMaterial() call, not just referenced.
		
		// Create a scene
		NxSceneDesc sceneDesc;
		sceneDesc.gravity				= NxVec3(0,-9.81,0)*2;
		//sceneDesc.userContactReport		= carContactReport;			//no standard contact report
		NxScene*gScene = gPhysicsSDK->createScene(sceneDesc);
		if(gScene == NULL) 
		{
			ErrMessage("Error: Unable to create a PhysX scene.");
			return NULL;
		}
		
		gScene->setUserContactReport(&global_report);
		//gScene->setActorGroupPairFlags(ActorGroup::StaticMesh,ActorGroup::StaticMesh,NX_NOTIFY_ON_START_TOUCH);
/* 		gScene->setActorGroupPairFlags(ActorGroup::StaticMesh,ActorGroup::DynamicMesh,NX_NOTIFY_ON_START_TOUCH);
		gScene->setActorGroupPairFlags(ActorGroup::DynamicMesh,ActorGroup::StaticMesh,NX_NOTIFY_ON_START_TOUCH);
		gScene->setActorGroupPairFlags(ActorGroup::DynamicMesh,ActorGroup::DynamicMesh,NX_NOTIFY_ON_START_TOUCH);
 */
		gScene->setActorGroupPairFlags(ActorGroup::StaticMesh,ActorGroup::DynamicMesh,NX_NOTIFY_ALL);
		gScene->setActorGroupPairFlags(ActorGroup::DynamicMesh,ActorGroup::StaticMesh,NX_NOTIFY_ALL);
		gScene->setActorGroupPairFlags(ActorGroup::DynamicMesh,ActorGroup::DynamicMesh,NX_NOTIFY_ALL);

		gScene->setTiming(1.0/80.0);
		// Turn on all contact notifications:
		//gScene->setActorGroupPairFlags(0, 0, NX_NOTIFY_ON_TOUCH);

		
		return gScene;
		
		
		
		
	
	
	}
	
	VehiclePrototype*							loadPrototype(const TEnvironment&env, const String&filename)
	{
		String afilename = FileSystem::getAbsolutePath(filename);
		#if SYSTEM==WINDOWS
			afilename.convertToLowerCase();
		#endif
		VehiclePrototype*result;
		if (prototypes.query(afilename,result))
			return result;
		result = SHIELDED(new VehiclePrototype());
		if (!result->loadFromFile(env,afilename))
		{
			DISCARD(result);
			return NULL;
		}
		local_prototypes.append(result);
		prototypes.set(afilename,result);
		return result;
	}
	
	
	Vehicle*								createFromPrototype(const TEnvironment&env, VehiclePrototype*prototype)
	{
		if (!prototype)
			return NULL;
		return newly_created_vehicles.append(SHIELDED(new Vehicle(env,*prototype)));
	}
	
	Vehicle*								createFromFile(const TEnvironment&env, const String&filename)
	{
		if (VehiclePrototype*prototype = loadPrototype(env,filename))
			return createFromPrototype(env,prototype);
		return NULL;
	}
	
	FirstPerson*							createFPS(float height)
	{
		FirstPerson*p = simulated_fps.append();
		p->height = height;
		return p;
	}
	
	
	
	StructurePrototype*					loadStructurePrototype(const TEnvironment&env, const String&filename, bool dynamic)
	{
		StructurePrototype*rs;
		if (structure_prototype_map.query(filename,rs))
			return rs;
		
		rs = SHIELDED(new StructurePrototype());
		if (!rs->loadFromFile(env,filename,dynamic))
		{
			DISCARD(rs);
			return NULL;
		}
		
		structure_prototypes.append(rs);
		structure_prototype_map.set(filename,rs);
		return rs;
	}
	
	Structure*								createFromPrototype(const TEnvironment&env, StructurePrototype*prototype)
	{
		if (!prototype)
			return NULL;
		bool dynamic = prototype->actor_desc.body != NULL;
		
		return dynamic?
				dynamic_structures.append(SHIELDED(new Structure(*prototype))):
				static_structures.append(SHIELDED(new Structure(*prototype)));
	}
	
	StructurePrototype*					createStructurePrototype(const TEnvironment&env, CGS::Geometry<>&geometry, bool dynamic)
	{
		StructurePrototype*p = structure_prototypes.append();
		p->adopt(env,geometry,dynamic);
		return p;
	}
	
	Structure*								createStructure(const TEnvironment&env, CGS::Geometry<>&geometry, bool dynamic)
	{
		StructurePrototype*prototype = createStructurePrototype(env,geometry,dynamic);
		return dynamic?
				dynamic_structures.append(SHIELDED(new Structure(*prototype))):
				static_structures.append(SHIELDED(new Structure(*prototype)));
	}

	void									signalExplosion(const TVec3<>&position, float force)
	{
		explosions << float4(position.x,position.y,position.z,force);
	}

	
	float									simulate(const TEnvironment&env, float delta)
	{
	
		if (delta > 0.05)
			delta = 0.05;

		NxReal step;
		NxU32 max;
		NxTimeStepMethod method;
		NxU32 num;
		env.scene->getTiming(step,max,method,&num);
		
	
		float time_passed = step*num;
		
		env.scene->fetchResults(NX_RIGID_BODY_FINISHED, true);

		/*
		newly_created_vehicles.reset();
		while (Vehicle*v = newly_created_vehicles.each())
		{
			v->instance.matrix.w.x += delta;
			v->instance.update();
		}

		*/
		newly_created_vehicles.reset();
		while (Vehicle*v = newly_created_vehicles.each())
		{
			v->createActor();
			simulated_vehicles.append(v);
		}
		newly_created_vehicles.flush();


		if (query_debug_information)
			debug_renderable = env.scene->getDebugRenderable();
	
		
		for (index_t i = 0; i < moved_static_structures; i++)
			moved_static_structures[i]->process();
		moved_static_structures.flush();
		for (auto it = moved_unique_static_structures.begin(); it != moved_unique_static_structures.end(); ++it)
			(*it)->applyPose();
		moved_unique_static_structures.reset();
		
		if (time_passed > 0)
		{
			for (index_t i = 0; i < dynamic_structures; i++)
			{
				dynamic_structures[i]->process();
			}
			for (index_t i = 0; i < simulated_vehicles; i++)
				simulated_vehicles[i]->process(env,time_passed);
			for (index_t i = 0; i < simulated_fps; i++)
				simulated_fps[i]->process(env,time_passed);
		}
		simulated_vehicle_waste.clear();
		explosions.reset();
		
		if (!was_relativated)
		{
			relativate(env,Vector<Composite::Coordinates::sector_t>::zero);
		}
		was_relativated = false;

		for (index_t i = 0; i < simulated_vehicles; i++)
		{
			simulated_vehicles[i]->featherAccurateMatrix(time_passed);
		}

		
		env.scene->simulate(delta);
		env.scene->flushStream();
			
		
			
		return time_passed;
	}
	
	void									relativate(const TEnvironment&env, const TVec3<Composite::Coordinates::sector_t>&observer_sector)
	{
		was_relativated = true;

		last_observer_sector = observer_sector;
		last_scale = env.system_unit_size;
		
		for (auto it = installed_unique_static_structures.begin(); it != installed_unique_static_structures.end(); ++it)
		{
			UniqueStaticStructure*v = *it;
			//Composite::sub(v->sector,observer_sector,v->geometry.root_system.matrix.w.xyz,env.sector_size);
			//v->geometry.root_system.update();
			TVec3<> new_translation;
			
			Vec::sub(v->coordinates.sector,observer_sector,new_translation);
			Vec::mult(new_translation,env.sector_size);
			//Vec::add(v->last_translation,v->end_point[0].coordinates.remainder,sys_translation);
			if (!Vec::similar(v->sector_translation,new_translation))
			{
				v->sector_translation = new_translation;
				if (v->actor)
				{
					NxVec3 vec;
					NxVec::div(v->coordinates.remainder,env.system_unit_size,vec);
					v->actor->setGlobalPosition(vec);
				}
			}
		}
		
		for (index_t i = 0; i < static_structures; i++)
		{
			Structure*v = static_structures[i];
			Composite::sub(v->coordinates,observer_sector,v->instance.matrix.w.xyz,env.sector_size);
			v->instance.update();
			//PhFloat sys_translation[3];
			Vec::sub(v->coordinates.sector,observer_sector,v->sector_translation);
			Vec::mult(v->sector_translation,env.sector_size);
			//Vec::add(v->last_translation,v->coordinates.remainder,sys_translation);
			if (v->actor)
			{
				NxVec3 vec;
				NxVec::div(v->instance.matrix.w.xyz,(double)env.system_unit_size,vec);
				v->actor->setGlobalPosition(vec);
			}
		}
		
		for (index_t i = 0; i < dynamic_structures; i++)
		{
			Structure*v = dynamic_structures[i];
			Composite::sub(v->coordinates,observer_sector,v->instance.matrix.w.xyz,env.sector_size);
			v->instance.update();
			//PhFloat sys_translation[3];
			Vec::sub(v->coordinates.sector,observer_sector,v->sector_translation);
			Vec::mult(v->sector_translation,env.sector_size);
			//Vec::add(v->last_translation,v->coordinates.remainder,sys_translation);
			if (v->actor)
			{
				NxVec3 vec;
				NxVec::div(v->instance.matrix.w.xyz,(double)env.system_unit_size,vec);
				v->actor->setGlobalPosition(vec);
			}
		}
		
		for (index_t i = 0; i < simulated_vehicles; i++)
		{
			Vehicle*v = simulated_vehicles[i];
			Composite::sub(v->coordinates,observer_sector,v->accurate_matrix.w.xyz,env.sector_size);
			
			TVec3<PhFloat> sys_translation;
			Vec::sub(v->coordinates.sector,observer_sector,sys_translation);
			Vec::mult(sys_translation,env.sector_size);
			//Vec::add(sys_translation,v->coordinates.remainder);
			if (v->actor)
			{
				NxVec3 vec;
				NxVec::div(v->accurate_matrix.w.xyz,(double)env.system_unit_size,vec);
				v->actor->setGlobalPosition(vec);
			}
			
			//Vec::c3(v->instance.matrix+12,v->system+12);
			float density = vmin(v->atmosphere_density,1);
			for (index_t k = 0; k < v->engines.count(); k++)
				{
					Engine&engine = v->engines[k];
					for (index_t j = 0; j < engine.flames.count(); j++)
					{
						Flame&flame = engine.flames[j];
						if (!flame.sound_source.isCreated())
						{
							flame.sound_source.create();
							flame.sound_source.setWaveBuffer(engine.type->sound);
							flame.sound_source.setLooping(true);
							flame.sound_source.setReferenceDistance(10*v->radius);
							flame.sound_source.play();
						}
						float3 p;
						Mat::transform(flame.system,flame.accel->position,p);
						flame.sound_source.locate(p);
						flame.sound_source.setVolume(engine.status*(1.0-engine.heat_up_remaining/engine.type->heat_up));
						flame.sound_source.setPitch(0.5+0.5*engine.status*(1.0-engine.heat_up_remaining/engine.type->heat_up)+0.05*(i+j));
						flame.sound_source.setVelocity((v->inertia/(float)env.system_unit_size));
					}
				}			
			for (index_t j = 0; j < v->wheels.count(); j++)
			{
				Wheel&wheel = v->wheels[j];
				if (!wheel.sound_source.isCreated())
				{
					wheel.sound_source.create();
					wheel.sound_source.setWaveBuffer(Wheel::tire_scream);
					wheel.sound_source.setLooping(true);
					wheel.sound_source.setReferenceDistance(10*v->radius);
					wheel.sound_source.play();
					wheel.sound_muted = false;
					cout << "creating sound source for vehicle "<<i<<" wheel "<<j<<endl;
					if (Wheel::tire_scream.isEmpty())
						cout << "  but sound is empty"<<endl;
				}
				float3 p;
				Composite::sub(wheel.last_position,observer_sector,p,env.sector_size);
				//cout << "relocating sound source for wheel at "<<Vec::toString(p)<<endl;
				wheel.sound_source.locate(p);
				
				float volume = sqr(clamped(wheel.smoothed_grinding,0,1));
				if (volume > 0.01)
				{
					if (wheel.sound_muted)
					{
						wheel.sound_source.play();
						wheel.sound_muted = false;
					}
					wheel.sound_source.setVolume(volume);
					wheel.sound_source.setPitch(0.75+clamped(wheel.smoothed_grinding*0.25,0,0.25));
					wheel.sound_source.setVelocity((v->inertia/(float)env.system_unit_size));
				}
				elif (!wheel.sound_muted)
				{
					wheel.sound_source.stop();
					wheel.sound_muted = true;
				}
			}
			
			if (v->wheels.count() && v->accel && v->engine_sound_volume > 0)
			{
				if (!v->wheel_engine_sound_source0.isCreated())
				{
					v->wheel_engine_sound_source0.create();
					v->wheel_engine_sound_source0.setWaveBuffer(v->prototype->wheel_drive.type->sound);
					v->wheel_engine_sound_source0.setLooping(true);
					v->wheel_engine_sound_source0.setReferenceDistance(10*v->radius);
					v->wheel_engine_sound_source0.play();
					
					v->wheel_engine_sound_source1.create();
					v->wheel_engine_sound_source1.setWaveBuffer(v->prototype->wheel_drive.type->sound);
					v->wheel_engine_sound_source1.setLooping(true);
					v->wheel_engine_sound_source1.setReferenceDistance(10*v->radius);
					v->wheel_engine_sound_source1.play();
					cout << "creating engine sound source for vehicle "<<i<<" wheel"<<endl;
					if (v->prototype->wheel_drive.type->sound.isEmpty())
						cout << "  but sound is empty"<<endl;
				}
				float3 p;
				Composite::sub(v->coordinates,observer_sector,p,env.sector_size);				
				v->wheel_engine_sound_source0.locate(p);
				v->wheel_engine_sound_source0.setVelocity((v->inertia/(float)env.system_unit_size));
				v->wheel_engine_sound_source0.setVolume((0.4+0.6*v->current_motor_speed)*v->engine_sound_volume);
				v->wheel_engine_sound_source0.setPitch(0.1+1.0*v->current_motor_speed);
				
				v->wheel_engine_sound_source1.locate(p);
				v->wheel_engine_sound_source1.setVelocity((v->inertia/(float)env.system_unit_size));
				v->wheel_engine_sound_source1.setVolume((0.4+0.6*v->current_motor_speed)*v->engine_sound_volume);
				v->wheel_engine_sound_source1.setPitch(0.1+0.8*v->current_motor_speed);
			}
		}
		
		
		for (index_t i = 0; i < active_sounds.count(); i++)
		{
			SoundInstance&instance = active_sounds[i];
			float3 p;
			Composite::sub(instance.coordinates,observer_sector,p,env.sector_size);
			instance.locate(p);
			if (!instance.started)
			{
				instance.play();
				instance.started = true;
			}
			else
				if (!instance.isPlaying())
				{
					active_sounds.erase(i);
					i--;
				}
		}
	
	}


	
	void									discard(Vehicle*vehicle)
	{
		simulated_vehicle_waste.append(simulated_vehicles.drop(vehicle));
	}
	
	void									discard(FirstPerson*fps)
	{
		simulated_fps.erase(fps);
	}
	
	void									discard(Structure*structure)
	{
		if (!structure)
			return;
		if (structure->descriptor->actor_desc.body)
			dynamic_structures.erase(structure);
		else
			static_structures.erase(structure);
	}
	
	void									discard(StructurePrototype*prototype)
	{
		structure_prototype_map.unSetEntry(prototype);
		structure_prototypes.erase(prototype);
	}
	

}
