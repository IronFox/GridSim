#include "light.h"
#include "../renderer/opengl.h"

namespace Engine
{
	namespace M = DeltaWorks::M;

	/**/						LightData::LightData():type(Omni),position(M::Vector3<>::zero),diffuse(M::Vector4<>::one),ambient(M::Vector4<>::one),
										specular(M::Vector4<>::one),spotDirection(M::Vector3<>::x_axis),spotCutoff(180),spotExponent(0),size(1),
										constantAttenuation(1),linearAttenuation(0),quadraticAttenuation(0)		{}

	Light::Light(LightScene*scene_,OpenGL*interface_):origin(scene_->Count()),scene(scene_),parent_interface(interface_),moved(false),modified(false),index(InvalidIndex)
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




	Light*			Light::setSpotDirection(const M::TVec3<float>&c)
	{
		spotDirection = c;
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

	Light*			Light::setDiffuse(const M::TVec3<float>&c,float factor)
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

	Light*			Light::setAmbient(const M::TVec3<float>&c, float factor)
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
	
	Light*			Light::setSpecular(const M::TVec3<float>&c, float factor)
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
}


