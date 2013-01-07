#include "../global_root.h"
#include "cgs.h"


template CGS::Geometry<CGS::StdDef>;
template CGS::AnimatableInstance<CGS::StdDef>;
template CGS::StaticInstance<CGS::StdDef>;
template CGS::VertexContainerA<CGS::StdDef>;
template CGS::Constructor<CGS::StdDef>;


const char* shortenFile(const char*full_file)
{
	const char*c = full_file+strlen(full_file)-1;
	while (c >= full_file && *c!='/' && *c!='\\')
		c--;
	return c+1;
}


namespace CGS
{

TVec4<float>	ambient_default = {0.2,0.2,0.2,1},
				diffuse_default = {0.8,0.8,0.8,1},
				specular_default = {0,0,0,1},
				emission_default = {0,0,0,1};
float			shininess_default = 0;

LogFile		*log(NULL);

template class Geometry<StdDef>;
template class StaticInstance<StdDef>;
template class AnimatorInstanceA<StdDef>;


MaterialInfo::MaterialInfo():alpha_test(false),fully_reflective(false)
{
	ambient = ambient_default;
	diffuse = diffuse_default;
	specular = specular_default;
	emission = emission_default;
	shininess = 0.5;
	alpha_threshold = 0.5;
}


bool		layerRequiresTexCoords(const TLayer&layer)
{
	if (layer.source)
		return layer.source->face_field.length() == 1;
	return EMPTY_TEXTURES_ARE_PLANAR;
}


count_t MaterialInfo::countCoordLayers()	const
{
	count_t cnt = 0;
	for (count_t i = 0; i < layer_field.length(); i++)
		cnt += layerRequiresTexCoords(layer_field[i]);
	return cnt;
}

void MaterialInfo::postCopyLink(TextureResource*textures)
{
	if (!textures)
		return;
	for (index_t i = 0; i < layer_field.length(); i++)
		layer_field[i].source = textures->retrieve(layer_field[i].source_name);

}


bool MaterialInfo::similar(const MaterialInfo&other) const
{
	if (layer_field.length() != other.layer_field.length())
		return false;
	if (alpha_test != other.alpha_test)
		return false;
	if (fully_reflective != other.fully_reflective)
		return false;
	typedef float Float;
	if (!Vec::similar(ambient,other.ambient))
		return false;
	if (!Vec::similar(diffuse,other.diffuse))
		return false;
	if (!Vec::similar(specular,other.specular))
		return false;
	if (!Vec::similar(emission,other.emission))
		return false;
	if (shininess != other.shininess)
		return false;
	if (alpha_threshold != other.alpha_threshold)
		return false;
	for (unsigned i = 0; i < layer_field.length(); i++)
	{
		if (layer_field[i].combiner != other.layer_field[i].combiner)
			return false;
		bool cube = layer_field[i].source && layer_field[i].source->face_field.length() == 6;
		if (cube && layer_field[i].mirror_map != other.layer_field[i].mirror_map)
			return false;
	}
	return true;
}


bool MaterialInfo::operator==(const MaterialInfo&other) const
{
	if (!similar(other))
		return false;
	for (unsigned i = 0; i < layer_field.length(); i++)
	{
		bool	cube0 = layer_field[i].source&&layer_field[i].source->face_field.length() == 6,
				cube1 = other.layer_field[i].source&&other.layer_field[i].source->face_field.length() == 6;
		if (cube0 != cube1 || !layer_field[i].source || !other.layer_field[i].source || !layer_field[i].source->face_field.length() || !other.layer_field[i].source->face_field.length())
			return false;
		if (!layer_field[i].source->isSimilar(*other.layer_field[i].source))
			return false;
	}
	return true;
}

void MaterialInfo::adoptData(MaterialInfo&other)
{
	fully_reflective = other.fully_reflective;
	alpha_test = other.alpha_test;
	layer_field.adoptData(other.layer_field);
	ambient = other.ambient;
	diffuse = other.diffuse;
	specular = other.specular;
	emission = other.emission;
	shininess = other.shininess;
	alpha_threshold = other.alpha_threshold;
	attachment.swap(other.attachment);

}




TextureA* cloneTexture(TextureA*source)
{
	return SHIELDED(new TextureA(*source));
}


RiffChunk*openBlock(RiffChunk&riff,__int64 name)
{
    riff.appendBlock(RIFF_DATA,&name,sizeof(name));
    return riff.appendBlock(RIFF_LIST);
}

void closeBlock(RiffFile&riff)
{
    riff.dropBack();
}



TextureA::TextureA():name(0),data_hash(0)
{}

void	TextureA::updateHash()
{
	data_hash = StdHash::hash(face_field);
}

bool	TextureA::isSimilar(const TextureA&other)	const
{
	if (this == &other)
		return true;
	if (face_field.length() != other.face_field.length())
		return false;
	for (index_t i = 0; i < face_field.length(); i++)
		if (face_field[i].length() != other.face_field[i].length())
			return false;
	return data_hash == other.data_hash;
}



/*
Texture::type_t	Texture::type()	const
{
	if (!face_field.count())
		return NoTexture;
	if (face_field.count() >= 6)
		return TextureCube;
	Image::THeader header = *(Image::THeader*)face_field.first().pointer();
	if (!header.y_exp || !header.x_exp)
		return Texture1D;
	return Texture2D;
}
*/


bool		TextureA::isEmpty()	const
{
	if (!this || !face_field.length())
		return true;
	for (index_t i = 0; i < face_field.length(); i++)
		if (face_field[i].contentSize() > sizeof(Image::THeader))
			return false;
	return true;
}

void		TextureA::adoptData(TextureA&other)
{
	name = other.name;
	face_field.adoptData(other.face_field);
	data_hash = other.data_hash;
}

void		TextureA::downSample(BYTE modifier, String*error_out)
{
	Image extracted;
	for (index_t i = 0; i < face_field.length(); i++)
	{
		if (face_field[i].contentSize() < sizeof(Image::THeader))
		{
			if (error_out)
				(*error_out) += "Face #"+String(i)+": insufficient size ("+String((unsigned)face_field[i].contentSize())+" byte(s))";
			continue;
		}
		Image::THeader header = *(Image::THeader*)face_field[i].pointer();
		if (header.x_exp == 1 && header.y_exp == 1)
		{
			if (error_out)
				(*error_out) += "Face #"+String(i)+": already too small ("+String(1<<header.x_exp)+"x"+String(1<<header.y_exp)+")";
			continue;
		}
		if (!TextureCompression::decompress(face_field[i],extracted))
		{
			if (error_out)
				(*error_out) += "Face #"+String(i)+": extraction failed ("+TextureCompression::getError()+")";
			continue;
		}
		if (header.x_exp > 1+modifier)
			header.x_exp -= modifier;
		else
			header.x_exp = 1;
		if (header.y_exp > 1+modifier)
			header.y_exp -= modifier;
		else
			header.y_exp = 1;
		
		extracted.scaleTo(1<<header.x_exp,1<<header.y_exp);
		TextureCompression::compress(extracted,face_field[i]);
	}
	updateHash();
}


void	TextureA::limitSizeExponent(BYTE max_exponent, String*error_out)
{
	if (!max_exponent)
		return;
	Image extracted;
	for (unsigned i = 0; i < face_field.length(); i++)
	{
		if (face_field[i].contentSize() < sizeof(Image::THeader))
		{
			if (error_out)
				(*error_out) += "Face #"+String(i)+": insufficient size ("+String((unsigned)face_field[i].contentSize())+" byte(s))";
			continue;
		}
		Image::THeader header = *(Image::THeader*)face_field[i].pointer();
		if (header.x_exp <= max_exponent && header.y_exp <= max_exponent)
			continue;
		if (!TextureCompression::decompress(face_field[i],extracted))
		{
			if (error_out)
				(*error_out) += "Face #"+String(i)+": extraction failed ("+TextureCompression::getError()+")";
			continue;
		}
		if (header.x_exp > max_exponent)
			header.x_exp = max_exponent;
		if (header.y_exp > max_exponent)
			header.y_exp = max_exponent;
		
		extracted.scaleTo(1<<header.x_exp,1<<header.y_exp);
		TextureCompression::compress(extracted,face_field[i]);
	}
	updateHash();


}


count_t TextureResource::countEntries()
{
    return 0;
}

TextureA*TextureResource::retrieve(const __int64&)
{
    return NULL;
}

TextureA*TextureResource::entry(index_t)
{
    return NULL;
}

void	TextureResource::downSampleAll(BYTE exponent_modifier)
{
	for (index_t i = 0; i < countEntries(); i++)
		entry(i)->downSample(exponent_modifier);
}





TextureA*LocalTexResource::retrieve(const __int64&name)
{
    for (unsigned i = 0; i < entry_field.length(); i++)
        if (entry_field[i].name == name)
            return &entry_field[i];
    return NULL;
}

TextureA*LocalTexResource::entry(index_t index)
{
    return &entry_field[index];
}

count_t LocalTexResource::countEntries()
{
    return entry_field.length();
}


}
