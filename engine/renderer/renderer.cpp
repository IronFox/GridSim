#include "../../global_root.h"
#include "renderer.h"

/********************************************************************

Universal visual language-interface and rendering related definitions

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

********************************************************************/


namespace Engine
{

	TVisualConfig         	default_buffer_config={24,16,0,0,4,0,0,0};
	TMatrix4<float>         environment_matrix={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
	bool                    erase_unused_textures(true);
	Mutex                  VisualInterface::vs_mutex;


	
	
	


	
	Light::Light(LightScene*scene_,VisualInterface*interface_):origin(scene_->count()),scene(scene_),parent_interface(interface_),moved(false),modified(false),type(Omni)
	{
		index = InvalidIndex;
	    Vec::clear(position);
	    Vec::set(diffuse,1);
	    Vec::set(ambient,1);
	    Vec::set(specular,1);
	    Vec::def(spot_direction,1,0,0);
	    spot_cutoff = 180;
	    spot_exponent = 0;
	    linear_attenuation = 0;
	    quadratic_attenuation = 0;
	    constant_attenuation = 1;
	}


	Light::~Light()
	{}

	void		Light::update()
	{
		if (isShining())
			parent_interface->updateLight(this);
		else
			modified = true;
	}

	Light::Type Light::getType()	const
	{
		return type;
	}

	Light*		Light::setType(Type type_)
	{
		type = type_;
		if (type == Direct)
			Vec::normalize0(position);
		update();
		return this;
	}

	bool		Light::isShining() const
	{
		return isEnabled() && scene == parent_interface->active_scene && parent_interface->lighting_enabled;
	}

	bool		Light::isEnabled()	const
	{
		return index != InvalidIndex;
	}
		
	void		Light::destroy()
	{
		disable();
		parent_interface->discardLight(this);
	}

	void		Light::discard()
	{
		destroy();
	}



	Light*		Light::enable()
	{
		if (isEnabled())
			return this;
		parent_interface->enableLight(this);
		return this;
	}

	Light*		Light::disable()
	{
		if (!isEnabled())
			return this;
		parent_interface->disableLight(this);
		return this;
	}

	Light*		Light::setEnabled(bool b)
	{
		if (b)
			enable();
		else
			disable();
		return this;
	}



	const TVec3<float>&	Light::getPosition()	const
	{
		return position;
	}

	
	Light*			Light::moveTo(float x, float y, float z)
	{
		setPosition(x,y,z);
		return this;
	}
	

	Light*			Light::setPosition(float x, float y, float z)
	{
		Vec::def(position,x,y,z);
		if (type == Direct)
			Vec::normalize0(position);

		if (isShining())
			parent_interface->updateLightPosition(this);
		else
			moved = true;
		return this;
	}


	Light*			Light::setDirection(float x, float y, float z)
	{
		if (type == Spot)
			setSpotDirection(x,y,z);
		else
			setPosition(-x,-y,-z);
		return this;
	}

	Light*			Light::setDirection(const TVec3<float>&c)
	{
		if (type == Spot)
			setSpotDirection(c);
		else
			setPosition(-c.x,-c.y,-c.z);
		return this;
	}


	Light*			Light::setDirection(const TVec3<double>&c)
	{
		if (type == Spot)
			setSpotDirection(c);
		else
			setPosition(-c.x,-c.y,-c.z);
		return this;
	}


	Light*			Light::setNegativeDirection(float x, float y, float z)
	{
		if (type == Spot)
			setSpotDirection(-x,-y,-z);
		else
			setPosition(x,y,z);
		return this;
	}

	Light*			Light::setNegativeDirection(const TVec3<float>&c)
	{
		if (type == Spot)
			setSpotDirection(-c.x,-c.y,-c.z);
		else
			setPosition(c);
		return this;
	}


	Light*			Light::setNegativeDirection(const TVec3<double>&c)
	{
		if (type == Spot)
			setSpotDirection(-c.x,-c.y,-c.z);
		else
			setPosition(c);
		return this;
	}


	const TVec3<float>&	Light::getSpotDirection()	const
	{
		return spot_direction;
	}

	Light*			Light::setSpotDirection(const TVec3<float>&c)
	{
		spot_direction = c;
		Vec::normalize0(spot_direction);
		update();
		return this;
	}
	
	Light*			Light::setSpotDirection(const TVec3<double>&c)
	{
		Vec::copy(c,spot_direction);
		Vec::normalize0(spot_direction);
		update();
		return this;
	}

	Light*			Light::setSpotDirection(float x, float y, float z)
	{
		Vec::def(spot_direction,x,y,z);
		Vec::normalize0(spot_direction);
		update();
		return this;
	}

	const TVec3<float>&	Light::getDiffuse()	const
	{
		return diffuse.rgb;
	}

	Light*			Light::setDiffuse(const TVec3<float>&c)
	{
		diffuse.rgb = c;
		//diffuse[3] = 1.0f;
		update();
		return this;
	}

	Light*			Light::setDiffuse(const TVec3<double>&c)
	{
		Vec::copy(c,diffuse.rgb);
		//diffuse[3] = 1.0f;
		update();
		return this;
	}
	Light*			Light::setDiffuse(const TVec3<float>&c,float factor)
	{
		Vec::mult(c,factor,diffuse.rgb);
		//diffuse[3] = 1.0f;
		update();
		return this;
	}

	Light*			Light::setDiffuse(const TVec3<double>&c,double factor)
	{
		Vec::mult(c,factor,diffuse.rgb);
		//diffuse[3] = 1.0f;
		update();
		return this;
	}

	Light*			Light::setDiffuse(float r, float g, float b)
	{
		Vec::def(diffuse.rgb,r,g,b);
		update();
		return this;
	}

	Light*			Light::setDiffuse(float intensity)
	{
		Vec::set(diffuse.rgb,intensity);
		update();
		return this;
	}


	const TVec3<float>&	Light::getAmbient()	const
	{
		return ambient.rgb;
	}

	Light*			Light::setAmbient(const TVec3<float>&c)
	{
		ambient.rgb = c;
		//ambient[3] = 1.0f;
		update();
		return this;
	}
	
	Light*			Light::setAmbient(const TVec3<double>&c)
	{
		Vec::copy(c,ambient.rgb);
		//ambient[3] = 1.0f;
		update();
		return this;
	}

	Light*			Light::setAmbient(const TVec3<float>&c, float factor)
	{
		Vec::mult(c,factor,ambient.rgb);
		//ambient[3] = 1.0f;
		update();
		return this;
	}
	
	Light*			Light::setAmbient(const TVec3<double>&c, double factor)
	{
		Vec::mult(c,factor,ambient.rgb);
		//ambient[3] = 1.0f;
		update();
		return this;
	}
	
	Light*			Light::setAmbient(float intensity)
	{
		Vec::set(ambient.rgb,intensity);
		update();
		return this;
	}

	Light*			Light::setAmbient(float r, float g, float b)
	{
		Vec::def(ambient.rgb,r,g,b);
		update();
		return this;
	}

	const TVec3<float>&	Light::getSpecular()	const
	{
		return specular.rgb;
	}

	Light*			Light::setSpecular(const TVec3<float>&c)
	{
		specular.rgb = c;
		//specular[3] = 1.0f;
		update();
		return this;
	}
	
	Light*			Light::setSpecular(const TVec3<double>&c)
	{
		Vec::copy(c,specular.rgb);
		//specular[3] = 1.0f;
		update();
		return this;
	}

	Light*			Light::setSpecular(const TVec3<float>&c, float factor)
	{
		Vec::mult(c,factor,specular.rgb);
		//specular[3] = 1.0f;
		update();
		return this;
	}
	
	Light*			Light::setSpecular(const TVec3<double>&c, double factor)
	{
		Vec::mult(c,factor,specular.rgb);
		//specular[3] = 1.0f;
		update();
		return this;
	}

	Light*			Light::setSpecular(float intensity)
	{
		Vec::set(specular.rgb,intensity);
		update();
		return this;
	}

	Light*			Light::setSpecular(float r, float g, float b)
	{
		Vec::def(specular.rgb,r,g,b);
		update();
		return this;
	}

			
	float			Light::getSpotCutoff()	const
	{
		return spot_cutoff;
	}

	Light*			Light::setSpotCutoff(float angle)
	{
		//if (type == Spot)
		{
			spot_cutoff = angle;
			if (type == Spot)
				update();
		}
		return this;
	}

	BYTE			Light::getSpotExponent()	const
	{
		return spot_exponent;
	}

	Light*			Light::setSpotExponent(BYTE exponent)
	{
		spot_exponent = exponent;
		update();
		return this;
	}
	
	Light*			Light::setExponent(BYTE exponent)
	{
		spot_exponent = exponent;
		update();
		return this;
	}

			
	const TVec3<float>&	Light::getAttenuation()	const
	{
		return attenuation;
	}

	float			Light::getConstantAttenuation()	const
	{
		return attenuation.x;
	}

	float			Light::getLinearAttenuation()		const
	{
		return attenuation.y;
	}

	float			Light::getQuadraticAttenuation()	const
	{
		return attenuation.z;
	}


	Light*			Light::setAttenuation(float constant, float linear, float quadratic)
	{
		Vec::def(attenuation,constant,linear,quadratic);
		update();
		return this;
	}

















	bool	TVertexSection::operator==(const TVertexSection&other)	const
	{
		if (!length)
			return !other.length;
		return offset == other.offset && length == other.length;
	}


	char	TVertexSection::compareTo(const TVertexSection&other)	const
	{
		if (!length || !other.length)
			return OrthographicComparison(length,other.length);

		return OrthographicComparison(offset,other.offset).addComparison(length,other.length);
	}



	bool	MaterialLayer::operator==(const MaterialLayer&other)	const
	{
		if (!enabled)
			return !other.enabled;
		return system_type == other.system_type && content_type == other.content_type
			&& clamp_x == other.clamp_x && clamp_y == other.clamp_y && clamp_z == other.clamp_z //&& reflect == other.reflect
			&& combiner == other.combiner 
			&& (system_type == SystemType::Custom?custom_system == other.custom_system:true);
	}


	char	MaterialLayer::compareTo(const MaterialLayer&other)	const
	{
		if (!enabled || !other.enabled)
			return OrthographicComparison(enabled,other.enabled);
		
		OrthographicComparison comparison(system_type, other.system_type);

		comparison
				.addComparison(content_type, other.content_type)
				.addComparison(clamp_x, other.clamp_x)
				.addComparison(clamp_y, other.clamp_y)
				.addComparison(clamp_z, other.clamp_z)
				//.addComparison(reflect, other.reflect)
				.addComparison(combiner,other.combiner);
		if (system_type == SystemType::Custom)
			comparison.addComparison(custom_system,other.custom_system);
		return comparison;
	}

	hash_t MaterialColors::hashCode() const
	{
		HashValue val(ambient.hashCode());

		val	.add(diffuse.hashCode())
			.add(specular.hashCode())
			.add(emission.hashCode())
			.add(alpha_test)
			.add(fully_reflective)
			.add(shininess_exponent);

		if (alpha_test)
			val.addFloat(alpha_threshold);

		return val;
	}

	bool	MaterialColors::operator==(const MaterialColors&other) const
	{
		return diffuse == other.diffuse && specular == other.specular && emission == other.emission && alpha_test == other.alpha_test && shininess_exponent == other.shininess_exponent
				&& (!alpha_test || similar(alpha_threshold, other.alpha_threshold)) && (fully_reflective == other.fully_reflective);
	}

	char	MaterialColors::compareTo(const MaterialColors&other)	const
	{
		OrthographicComparison rs(ambient.compareTo(other.ambient));
		rs	.addCompareTo(diffuse,other.diffuse)
			.addCompareTo(specular,other.specular)
			.addCompareTo(emission,other.emission)
			.addComparison(alpha_test,other.alpha_test)
			.addComparison(fully_reflective,other.fully_reflective)
			.addComparison(shininess_exponent,other.shininess_exponent);
		if (alpha_test)
			rs.addComparison(alpha_threshold, other.alpha_threshold);
		return rs;
	}

	/*
	MaterialColors&			MaterialColors::operator=(const MaterialColors&other)
	{
		this->alpha_test = other.alpha_test;
		this->alpha_threshold = other.alpha_threshold;
		this->ambient = other.ambient;
		this->diffuse = other.diffuse;
		this->emission = other.emission;
		this->shininess_exponent = other.shininess_exponent;
		this->specular = other.specular;

		return *this;
	}*/

	hash_t VertexBinding::hashCode() const
	{
		HashValue rs(vertex.hashCode());
		rs	.add(tangent.hashCode())
			.add(normal.hashCode())
			.add(color.hashCode());
		for (index_t i = 0; i < texcoords.count(); i++)
			rs.add(texcoords[i].hashCode());
		return rs.add(floats_per_vertex);
	}

	char	VertexBinding::compareTo(const VertexBinding&other) const
	{
		return OrthographicComparison(vertex.compareTo(other.vertex))
			.addCompareTo(tangent,other.tangent)
			.addCompareTo(normal,other.normal)
			.addCompareTo(color,other.color)
			.addCompareTo(texcoords,other.texcoords)
			.addComparison(floats_per_vertex,other.floats_per_vertex);
	}

	bool	VertexBinding::operator==(const VertexBinding&other) const
	{
		return vertex == other.vertex && tangent == other.tangent && normal == other.normal && color == other.color && texcoords == other.texcoords && floats_per_vertex == other.floats_per_vertex;
	}


	

	void MaterialColors::read(const CGS::MaterialInfo&info)
	{
		ambient = info.ambient;
		diffuse = info.diffuse;
		specular = info.specular;
		emission = info.emission;
		alpha_threshold = info.alpha_threshold;
		alpha_test = info.alpha_test;
		fully_reflective = info.fully_reflective;
		shininess_exponent = 1+BYTE(info.shininess*127);
	}

	void VertexBinding::read(const CGS::MaterialInfo&info, UINT32 flags)
	{
		vertex.set(0,3);
		floats_per_vertex = 3;
		if (flags&CGS::HasNormalFlag)
		{
			normal.set(floats_per_vertex,3);
			floats_per_vertex += 3;
		}
		else
			normal.set(0,0);

		if (flags&CGS::HasTangentFlag)
		{
			tangent.set(floats_per_vertex,3);
			floats_per_vertex += 3;
		}
		else
			tangent.set(0,0);

		if (flags&CGS::HasColorFlag)
		{
			color.set(floats_per_vertex,4);
			floats_per_vertex += 4;
		}
		else
			color.set(0,0);

		texcoords.setSize(info.layer_field.length());
		for (index_t i = 0; i < texcoords.count(); i++)
		{
			if (CGS::layerRequiresTexCoords(info.layer_field[i]))
			{
				texcoords[i].set(floats_per_vertex,2);
				floats_per_vertex += 2;
				texcoords[i].generate_reflection_coords = false;
			}
			else
			{
				texcoords[i].unset();
				texcoords[i].generate_reflection_coords = info.layer_field[i].mirror_map;
			}
		}
	}

	MaterialColors::MaterialColors()
	{
		resetToDefault();
	}

	VertexBinding::VertexBinding(count_t layers):texcoords(layers)
	{
		resetToDefault(false);
	}

	void MaterialColors::resetToDefault()
	{
		ambient = CGS::ambient_default;
		specular = CGS::specular_default;
		emission = CGS::emission_default;
		diffuse = CGS::diffuse_default;
		shininess_exponent = 16;
		alpha_threshold = 0.5;
		alpha_test = false;
		fully_reflective = false;
	}

	void VertexBinding::resetToDefault(bool erase_layers)
	{
		vertex.set(0,3);
		normal.set(3,3);
		color.set(0,0);
		tangent.set(0,0);
		floats_per_vertex = 6;

		if (erase_layers)
			texcoords.free();
		else
			for (index_t i = 0; i < texcoords.count(); i++)
			{
				texcoords[i].set(floats_per_vertex,2);
				floats_per_vertex += 2;
			}
	}

	UINT16 VertexBinding::minFloatsPerVertex() const
	{
		UINT16 rs = vmax(vmax(vertex.requiredFloats(),normal.requiredFloats()),vmax(color.requiredFloats(),tangent.requiredFloats()));
		for (index_t i = 0; i < texcoords.count(); i++)
			rs = vmax(rs,texcoords[i].requiredFloats());
		return rs;
	}


	void		MaterialLayer::resetToDefault()
	{
		system_type = SystemType::Identity;
		content_type = PixelType::Color;
		enabled = true;
		//reflect = false;
		combiner = 0x2100;
		custom_system = NULL;
		clamp_x = clamp_y = clamp_z = false;
	}



	hash_t						MaterialConfiguration::hashCode() const
	{
		HashValue val(MaterialColors::hashCode());
		//val.add(VertexBinding::hashCode());

		for (index_t i = 0; i < layers.count(); i++)
			val.add(layers[i].hashCode());

		return val;
	}

	void MaterialConfiguration::read(const CGS::MaterialInfo&info, UINT32 flags)
	{
		MaterialColors::read(info);
		//VertexBinding::read(info,floats_per_coordinate,flags);

		layers.setSize(info.layer_field.length());
			//texcoords.count());

		for (index_t i = 0; i < layers.length(); i++)
		{
			const CGS::TLayer&from = info.layer_field[i];
			MaterialLayer&to = layers[i];
			
			to.combiner = from.combiner;
			{
				bool requires_texcoords = CGS::layerRequiresTexCoords(from);


				if (from.source && from.cube_map && !requires_texcoords)
				{
					to.system_type = SystemType::Environment;
					//to.reflect = from.mirror_map;
					to.content_type = PixelType::Color;
					to.clamp_x = to.clamp_y = to.clamp_z = false;
					to.enabled = true;
				}
				else
				{
					if (requires_texcoords)
					{
						to.system_type = SystemType::Identity;
						to.content_type = PixelType::Color;
						to.clamp_x = from.clamp_x;
						to.clamp_y = from.clamp_y;
						to.clamp_z = false;
						//to.reflect = false;
						to.enabled = true;
					}
					else
						to.enabled = false;
				}
			}
		}
	}

	MaterialConfiguration::MaterialConfiguration(count_t layers_):layers(layers_)
	{}

	MaterialConfiguration::MaterialConfiguration(const CGS::MaterialInfo&info, UINT32 flags)
	{
		read(info, flags);
	}

	MaterialConfiguration::MaterialConfiguration(const CGS::MaterialInfo*info, UINT32 flags)
	{
		read(*info,flags);
	}


	void MaterialConfiguration::resetToDefault(bool colors, bool erase_layers)
	{
	    if (colors)
			MaterialColors::resetToDefault();

		if (erase_layers)
		{
			//texcoords.free();
			layers.free();
		}

		/*if (binding)
			VertexBinding::resetToDefault(false);*/
	}





	bool	MaterialConfiguration::operator==(const MaterialConfiguration&other)	const
	{
		return	MaterialColors::operator==(other) && /*VertexBinding::operator==(other) && */layers == other.layers && name == other.name;
	}

	char	MaterialConfiguration::compareTo(const MaterialConfiguration&other)	const
	{
		return OrthographicComparison(MaterialColors::compareTo(other)).addCompareTo(layers,other.layers);
	}


	bool				VisualInterface::hasMoved(Light*light)
	{
		return light->moved;
	}

	bool				VisualInterface::wasModified(Light*light)
	{
		return light->modified;
	}

	void				VisualInterface::setHasMoved(Light*light,bool b)
	{
		light->moved = b;
	}

	void				VisualInterface::setWasModified(Light*light,bool b)
	{
		light->modified = b;

	}


	index_t			VisualInterface::getIndexOf(Light*light)
	{
		return light->index;
	}

	void				VisualInterface::setIndexOf(Light*light,index_t index)
	{
		light->index = index;
	}

	LightScene*		VisualInterface::getLightSceneOf(Light*light)
	{
		return light->scene;
	}


	VisualInterface::VisualInterface():active_scene_index(0),scene_index_counter(0),lighting_enabled(false)
	{
	    active_scene = scenes.define(scene_index_counter++);
	}

	Light*	VisualInterface::createLight(bool enable)
	{
		Light*light = SHIELDED(new Light(active_scene,this));
		active_scene->append(light);
		
		if (enable)
			light->enable();
		return light;
	}

	void		VisualInterface::discardLight(Light*light)
	{
		LightScene*scene = light->scene;
		
	    if (scene->get(light->origin) != light)
	        FATAL__("bad light");
			
	    light->disable();
	    index_t index = light->origin;
	    scene->erase(light->origin);
	    for (index_t i = index; i < scene->count(); i++)
	        scene->get(i)->origin = i;
	}

	void		VisualInterface::clearLights()
	{
		active_scene->reset();
		while (Light*light = active_scene->each())
			light->disable();
		active_scene->clear();
	}

	void		VisualInterface::resetLighting()
	{
		if (application_shutting_down)
			return;
		Array<LightScene*>	exported;
		scenes.exportTo(exported);
		for (index_t i = 0; i < exported.count(); i++)
		{
			exported[i]->reset();
			while (Light*light = exported[i]->each())
				light->disable();
		}
		scenes.clear();
		active_scene_index = 0;
		scene_index_counter = 0;
		active_scene = scenes.define(scene_index_counter++);
		pickLightScene(0);
	}

	index_t  VisualInterface::createLightScene()
	{
		index_t index = scene_index_counter++;
		scenes.define(index);
	    pickLightScene(index);
	    return index;
	}

	bool			VisualInterface::isLightScene(index_t scenario)	const
	{
		return scenes.isSet(scenario);
	}

	void            VisualInterface::discardLightScene(index_t index)
	{
	    if (!index)
			return;
		LightScene*scene = scenes.drop(index);
		if (!scene)
			return;
		if (scene == active_scene)
			pickLightScene(0);
		scene->reset();
		while (Light*light = scene->each())
			light->disable();
		DISCARD(scene);
	}
	
	index_t		VisualInterface::getLightScene()	const
	{
		return active_scene_index;
	}
	
	void			VisualInterface::getSceneLights(ArrayData<Light*>&array, bool enabled_only)
	{
		size_t	count = 0;
		if (!enabled_only)
			count = active_scene->count();
		else
			for (unsigned i = 0; i < active_scene->count(); i++)
				if (active_scene->get(i)->isEnabled())
					count++;
		array.setSize(count);
		count = 0;
		for (unsigned i = 0; i < active_scene->count(); i++)
			if (!enabled_only || active_scene->get(i)->isEnabled())
				array[count++] = active_scene->get(i);
	}
	
}
