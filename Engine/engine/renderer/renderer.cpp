#include "renderer.h"

/********************************************************************

Universal visual language-interface and rendering related definitions

********************************************************************/


namespace Engine
{

	TVisualConfig         	default_buffer_config={24,24,0,0,8,0,0,0};
	M::TMatrix4<float>         environment_matrix={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
	bool                    erase_unused_textures(true);


	
	










	bool	TVertexSection::operator==(const TVertexSection&other)	const
	{
		if (!length)
			return !other.length;
		return offset == other.offset && length == other.length;
	}


	char	TVertexSection::CompareTo(const TVertexSection&other)	const
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


	char	MaterialLayer::CompareTo(const MaterialLayer&other)	const
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
				&& (!alpha_test || M::Similar(alpha_threshold, other.alpha_threshold)) && (fully_reflective == other.fully_reflective);
	}

	char	MaterialColors::CompareTo(const MaterialColors&other)	const
	{
		OrthographicComparison rs(ambient.CompareTo(other.ambient));
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
		for (index_t i = 0; i < texcoords.Count(); i++)
			rs.AddGeneric(texcoords[i]);
		return rs;
	}

	char	VertexBinding::CompareTo(const VertexBinding&other) const
	{
		return OrthographicComparison(vertex.CompareTo(other.vertex))
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
		vertex.Set(0,3);
		floats_per_vertex = 3;
		if (flags&CGS::HasNormalFlag)
		{
			normal.Set(floats_per_vertex,3);
			floats_per_vertex += 3;
		}
		else
			normal.Set(0,0);

		if (flags&CGS::HasTangentFlag)
		{
			tangent.Set(floats_per_vertex,3);
			floats_per_vertex += 3;
		}
		else
			tangent.Set(0,0);

		if (flags&CGS::HasColorFlag)
		{
			color.Set(floats_per_vertex,4);
			floats_per_vertex += 4;
		}
		else
			color.Set(0,0);

		texcoords.SetSize(info.layer_field.GetLength());
		for (index_t i = 0; i < texcoords.Count(); i++)
		{
			if (CGS::layerRequiresTexCoords(info.layer_field[i]))
			{
				texcoords[i].Set(floats_per_vertex,2);
				floats_per_vertex += 2;
				texcoords[i].generate_reflection_coords = false;
			}
			else
			{
				texcoords[i].Unset();
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
		vertex.Set(0,3);
		normal.Set(3,3);
		color.Set(0,0);
		tangent.Set(0,0);
		floats_per_vertex = 6;

		if (erase_layers)
			texcoords.free();
		else
			for (index_t i = 0; i < texcoords.Count(); i++)
			{
				texcoords[i].Set(floats_per_vertex,2);
				floats_per_vertex += 2;
			}
	}

	UINT16 VertexBinding::minFloatsPerVertex() const
	{
		UINT16 rs = M::vmax(M::vmax(vertex.requiredFloats(),normal.requiredFloats()),M::vmax(color.requiredFloats(),tangent.requiredFloats()));
		for (index_t i = 0; i < texcoords.Count(); i++)
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

		for (index_t i = 0; i < layers.Count(); i++)
			val.Add(layers[i].ToHash());

		return val;
	}

	void MaterialConfiguration::read(const CGS::MaterialInfo&info, UINT32 flags)
	{
		MaterialColors::read(info);
		//VertexBinding::read(info,floats_per_coordinate,flags);

		layers.SetSize(info.layer_field.GetLength());
			//texcoords.Count());

		for (index_t i = 0; i < layers.GetLength(); i++)
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

	char	MaterialConfiguration::CompareTo(const MaterialConfiguration&other)	const
	{
		return OrthographicComparison(MaterialColors::CompareTo(other)).AddCompareTo(layers,other.layers);
	}


	
}
