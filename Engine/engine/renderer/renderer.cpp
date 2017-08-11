#include "renderer.h"

/********************************************************************

Universal visual language-interface and rendering related definitions

********************************************************************/


namespace Engine
{

	TVisualConfig         	default_buffer_config={24,24,0,0,8,0,0,0};
	M::TMatrix4<float>         environment_matrix={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
	bool                    erase_unused_textures(true);


	
	
	
		
	Light::Light(LightScene*scene_,VisualInterface*interface_):origin(scene_->count()),scene(scene_),parent_interface(interface_),moved(false),modified(false),index(InvalidIndex)
	{}


	Light::~Light()
	{}

	void		Light::update()
	{
		if (isShining())
			parent_interface->updateLight(shared_from_this());
		else
			modified = true;
	}


	Light*		Light::setType(Type type_)
	{
		type = type_;
		if (type == Direct)
			M::Vec::normalize0(position);
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
		
	void		Light::Destroy()
	{
		disable();
		parent_interface->discardLight(shared_from_this());
	}

	void		Light::discard()
	{
		Destroy();
	}



	Light*		Light::enable()
	{
		if (isEnabled())
			return this;
		parent_interface->enableLight(shared_from_this());
		return this;
	}

	Light*		Light::disable()
	{
		if (!isEnabled())
			return this;
		parent_interface->disableLight(shared_from_this());
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


	
	Light*			Light::moveTo(float x, float y, float z)
	{
		setPosition(x,y,z);
		return this;
	}
	

	Light*			Light::setPosition(float x, float y, float z)
	{
		M::Vec::def(position,x,y,z);
		if (type == Direct)
			M::Vec::normalize0(position);

		if (isShining())
			parent_interface->updateLightPosition(shared_from_this());
		else
			moved = true;
		return this;
	}


	Light*			Light::SetDirection(float x, float y, float z)
	{
		if (type == Spot)
			setSpotDirection(x,y,z);
		else
			setPosition(-x,-y,-z);
		return this;
	}

	Light*			Light::SetDirection(const M::TVec3<float>&c)
	{
		if (type == Spot)
			setSpotDirection(c);
		else
			setPosition(-c.x,-c.y,-c.z);
		return this;
	}


	Light*			Light::SetDirection(const M::TVec3<double>&c)
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

	Light*			Light::setNegativeDirection(const M::TVec3<float>&c)
	{
		if (type == Spot)
			setSpotDirection(-c.x,-c.y,-c.z);
		else
			setPosition(c);
		return this;
	}


	Light*			Light::setNegativeDirection(const M::TVec3<double>&c)
	{
		if (type == Spot)
			setSpotDirection(-c.x,-c.y,-c.z);
		else
			setPosition(c);
		return this;
	}




	Light*			Light::setSpotDirection(const M::TVec3<float>&c)
	{
		spotDirection = c;
		M::Vec::normalize0(spotDirection);
		update();
		return this;
	}
	
	Light*			Light::setSpotDirection(const M::TVec3<double>&c)
	{
		M::Vec::copy(c,spotDirection);
		M::Vec::normalize0(spotDirection);
		update();
		return this;
	}

	Light*			Light::setSpotDirection(float x, float y, float z)
	{
		M::Vec::def(spotDirection,x,y,z);
		M::Vec::normalize0(spotDirection);
		update();
		return this;
	}


	Light*			Light::setDiffuse(const M::TVec3<float>&c)
	{
		diffuse.rgb = c;
		//diffuse[3] = 1.0f;
		update();
		return this;
	}

	Light*			Light::setDiffuse(const M::TVec3<double>&c)
	{
		M::Vec::copy(c,diffuse.rgb);
		//diffuse[3] = 1.0f;
		update();
		return this;
	}
	Light*			Light::setDiffuse(const M::TVec3<float>&c,float factor)
	{
		M::Vec::mult(c,factor,diffuse.rgb);
		//diffuse[3] = 1.0f;
		update();
		return this;
	}

	Light*			Light::setDiffuse(const M::TVec3<double>&c,double factor)
	{
		M::Vec::mult(c,factor,diffuse.rgb);
		//diffuse[3] = 1.0f;
		update();
		return this;
	}

	Light*			Light::setDiffuse(float r, float g, float b)
	{
		M::Vec::def(diffuse.rgb,r,g,b);
		update();
		return this;
	}

	Light*			Light::setDiffuse(float intensity)
	{
		M::Vec::set(diffuse.rgb,intensity);
		update();
		return this;
	}



	Light*			Light::setAmbient(const M::TVec3<float>&c)
	{
		ambient.rgb = c;
		//ambient[3] = 1.0f;
		update();
		return this;
	}
	
	Light*			Light::setAmbient(const M::TVec3<double>&c)
	{
		M::Vec::copy(c,ambient.rgb);
		//ambient[3] = 1.0f;
		update();
		return this;
	}

	Light*			Light::setAmbient(const M::TVec3<float>&c, float factor)
	{
		M::Vec::mult(c,factor,ambient.rgb);
		//ambient[3] = 1.0f;
		update();
		return this;
	}
	
	Light*			Light::setAmbient(const M::TVec3<double>&c, double factor)
	{
		M::Vec::mult(c,factor,ambient.rgb);
		//ambient[3] = 1.0f;
		update();
		return this;
	}
	
	Light*			Light::setAmbient(float intensity)
	{
		M::Vec::set(ambient.rgb,intensity);
		update();
		return this;
	}

	Light*			Light::setAmbient(float r, float g, float b)
	{
		M::Vec::def(ambient.rgb,r,g,b);
		update();
		return this;
	}


	Light*			Light::setSpecular(const M::TVec3<float>&c)
	{
		specular.rgb = c;
		//specular[3] = 1.0f;
		update();
		return this;
	}
	
	Light*			Light::setSpecular(const M::TVec3<double>&c)
	{
		M::Vec::copy(c,specular.rgb);
		//specular[3] = 1.0f;
		update();
		return this;
	}

	Light*			Light::setSpecular(const M::TVec3<float>&c, float factor)
	{
		M::Vec::mult(c,factor,specular.rgb);
		//specular[3] = 1.0f;
		update();
		return this;
	}
	
	Light*			Light::setSpecular(const M::TVec3<double>&c, double factor)
	{
		M::Vec::mult(c,factor,specular.rgb);
		//specular[3] = 1.0f;
		update();
		return this;
	}

	Light*			Light::setSpecular(float intensity)
	{
		M::Vec::set(specular.rgb,intensity);
		update();
		return this;
	}

	Light*			Light::setSpecular(float r, float g, float b)
	{
		M::Vec::def(specular.rgb,r,g,b);
		update();
		return this;
	}


	Light*			Light::setSpotCutoff(float angle)
	{
		//if (type == Spot)
		{
			spotCutoff = angle;
			if (type == Spot)
				update();
		}
		return this;
	}


	Light*			Light::setSpotExponent(BYTE exponent)
	{
		spotExponent = exponent;
		update();
		return this;
	}
	
	Light*			Light::setExponent(BYTE exponent)
	{
		spotExponent = exponent;
		update();
		return this;
	}

			



	Light*			Light::setAttenuation(float constant, float linear, float quadratic)
	{
		M::Vec::def(attenuation,constant,linear,quadratic);
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

		return OrthographicComparison(offset,other.offset).AddComparison(length,other.length);
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
				.AddComparison(content_type, other.content_type)
				.AddComparison(clamp_x, other.clamp_x)
				.AddComparison(clamp_y, other.clamp_y)
				.AddComparison(clamp_z, other.clamp_z)
				//.AddComparison(reflect, other.reflect)
				.AddComparison(combiner,other.combiner);
		if (system_type == SystemType::Custom)
			comparison.AddComparison(custom_system,other.custom_system);
		return comparison;
	}

	/*virtual override*/	hash_t MaterialColors::ToHash() const
	{
		HashValue val;
		
		val	.AddGeneric(ambient)
			.AddGeneric(diffuse)
			.AddGeneric(specular)
			.AddGeneric(emission)
			.AddGeneric(alpha_test)
			.AddGeneric(fully_reflective)
			.AddGeneric(shininess_exponent);

		if (alpha_test)
			val.AddGeneric(alpha_threshold);

		return val;
	}

	bool	MaterialColors::operator==(const MaterialColors&other) const
	{
		return diffuse == other.diffuse && specular == other.specular && emission == other.emission && alpha_test == other.alpha_test && shininess_exponent == other.shininess_exponent
				&& (!alpha_test || M::similar(alpha_threshold, other.alpha_threshold)) && (fully_reflective == other.fully_reflective);
	}

	char	MaterialColors::compareTo(const MaterialColors&other)	const
	{
		OrthographicComparison rs(ambient.compareTo(other.ambient));
		rs	.AddCompareTo(diffuse,other.diffuse)
			.AddCompareTo(specular,other.specular)
			.AddCompareTo(emission,other.emission)
			.AddComparison(alpha_test,other.alpha_test)
			.AddComparison(fully_reflective,other.fully_reflective)
			.AddComparison(shininess_exponent,other.shininess_exponent);
		if (alpha_test)
			rs.AddComparison(alpha_threshold, other.alpha_threshold);
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

	/*virtual override*/ hash_t VertexBinding::ToHash() const
	{
		HashValue rs;
		rs	.AddGeneric(vertex)
			.AddGeneric(tangent)
			.AddGeneric(normal)
			.AddGeneric(color)
			.AddGeneric(floats_per_vertex)
			;
		for (index_t i = 0; i < texcoords.count(); i++)
			rs.AddGeneric(texcoords[i]);
		return rs;
	}

	char	VertexBinding::compareTo(const VertexBinding&other) const
	{
		return OrthographicComparison(vertex.compareTo(other.vertex))
			.AddCompareTo(tangent,other.tangent)
			.AddCompareTo(normal,other.normal)
			.AddCompareTo(color,other.color)
			.AddCompareTo(texcoords,other.texcoords)
			.AddComparison(floats_per_vertex,other.floats_per_vertex);
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

		texcoords.SetSize(info.layer_field.length());
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
		blend = true;
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
		UINT16 rs = M::vmax(M::vmax(vertex.requiredFloats(),normal.requiredFloats()),M::vmax(color.requiredFloats(),tangent.requiredFloats()));
		for (index_t i = 0; i < texcoords.count(); i++)
			rs = M::vmax(rs,texcoords[i].requiredFloats());
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



	/*virtual override*/ hash_t						MaterialConfiguration::ToHash() const
	{
		HashValue val(MaterialColors::ToHash());
		//val.add(VertexBinding::hashCode());

		for (index_t i = 0; i < layers.count(); i++)
			val.Add(layers[i].ToHash());

		return val;
	}

	void MaterialConfiguration::read(const CGS::MaterialInfo&info, UINT32 flags)
	{
		MaterialColors::read(info);
		//VertexBinding::read(info,floats_per_coordinate,flags);

		layers.SetSize(info.layer_field.length());
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
		return OrthographicComparison(MaterialColors::compareTo(other)).AddCompareTo(layers,other.layers);
	}


	bool				VisualInterface::hasMoved(const PLight&light)
	{
		return light->moved;
	}

	bool				VisualInterface::wasModified(const PLight&light)
	{
		return light->modified;
	}

	void				VisualInterface::setHasMoved(const PLight&light,bool b)
	{
		light->moved = b;
	}

	void				VisualInterface::setWasModified(const PLight&light,bool b)
	{
		light->modified = b;

	}


	index_t			VisualInterface::getIndexOf(const PLight&light)
	{
		return light->index;
	}

	void				VisualInterface::setIndexOf(const PLight&light,index_t index)
	{
		light->index = index;
	}

	LightScene*		VisualInterface::getLightSceneOf(const PLight&light)
	{
		return light->scene;
	}


	VisualInterface::VisualInterface():active_scene_index(0),scene_index_counter(0),lighting_enabled(false)
	{
	    active_scene = scenes.define(scene_index_counter++);
	}

	PLight	VisualInterface::createLight(bool enable)
	{
		PLight light(new Light(active_scene,this));
		active_scene->Append(light);
		
		if (enable)
			light->enable();
		return light;
	}

	void		VisualInterface::discardLight(const PLight&light)
	{
		LightScene*scene = light->scene;
		
	    if (scene->at(light->origin) != light)
	        FATAL__("bad light");
			
	    light->disable();
	    index_t index = light->origin;
	    scene->Erase(light->origin);
	    for (index_t i = index; i < scene->count(); i++)
	        scene->at(i)->origin = i;
	}

	void		VisualInterface::clearLights()
	{
		foreach (*active_scene,light)
			(*light)->disable();
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
			foreach (*exported[i],light)
				(*light)->disable();
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
		foreach (*scene,light)
			(*light)->disable();
		Discard(scene);
	}
	
	index_t		VisualInterface::getLightScene()	const
	{
		return active_scene_index;
	}
	
	void			VisualInterface::getSceneLights(ArrayData<PLight>&array, bool enabled_only)
	{
		size_t	count = 0;
		if (!enabled_only)
			count = active_scene->count();
		else
			for (unsigned i = 0; i < active_scene->count(); i++)
				if (active_scene->at(i)->isEnabled())
					count++;
		array.SetSize(count);
		count = 0;
		for (unsigned i = 0; i < active_scene->count(); i++)
			if (!enabled_only || active_scene->at(i)->isEnabled())
				array[count++] = active_scene->at(i);
	}
	
}
