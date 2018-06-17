#ifndef gl_lightH
#define gl_lightH

#include <math/vector.h>
#include <container/buffer.h>

namespace Engine
{
	namespace M = DeltaWorks::M;

	class Light;
	typedef std::shared_ptr<Light>		PLight;

	/**
		\brief Light setting

		Container for a light setting
	*/
	typedef DeltaWorks::Buffer0<PLight>			LightScene;

	class OpenGL;

	class LightData
	{
	public:
		enum Type	//! Light type
		{
			None,					//!< Undetermined light type
			Direct,					//!< Directional (parallel) light.
			Omni,					//!< Omni (radial) light.
			Spot					//!< Spot light (radial, limited to cone).
		};

	protected:
		friend class VisualInterface;
			
		Type						type;		//!< Type of this light.
			
		M::TVec3<float>				position,		//!< Position of this light in R3
									spotDirection;	//!< Spot direction in R3. Needed if light type is not Omni.
		M::TVec4<float>				diffuse,		//!< Diffuse light vector - the 4th component is supposed to be 1.0f.
									ambient,		//!< Ambient light vector - the 4th component is supposed to be 1.0f.
									specular;		//!< Specular light vector - the 4th component is supposed to be 1.0f.
		float						spotCutoff;		//!< Cutoff angle (0.0f - 180.0f). Only needed if light type is Spot.
		BYTE						spotExponent;	//!< Exponent of the light Cone. Higher values mean a thinner cone.
		float						size;			//!< Geometrical size of the light. Ignored by renderers. Initialized to 1.
		union
		{
			M::TVec3<float>			attenuation;
			struct
			{
				float				constantAttenuation,	//!< Constant component of an exponential light source. 1 (linear) by default.
									linearAttenuation,		//!< Linear component of an exponential light source. 0 by default.
									quadraticAttenuation;	//!< Quadratic component of an exponential light source. 0 by default.
			};
		};

		/**/						LightData():type(Omni),position(M::Vector3<>::zero),diffuse(M::Vector4<>::one),ambient(M::Vector4<>::one),
										specular(M::Vector4<>::one),spotDirection(M::Vector3<>::x_axis),spotCutoff(180),spotExponent(0),size(1),
										constantAttenuation(1),linearAttenuation(0),quadraticAttenuation(0)		{}
	public:
		Type						GetType()			const	/** Retrieves light type */			{return type;}
		const M::TVec3<float>&		GetPosition()		const	/** Get light position {x,y,z} */	{return position;}
		const M::TVec3<float>&		GetSpotDirection()	const	/** Get light spot direction */		{return spotDirection;}
		const M::TVec3<float>&		GetDiffuse()		const	/** Get diffuse color (rgb)*/		{return diffuse.rgb;}
		const M::TVec3<float>&		GetAmbient()		const	/** Get ambient color (rgb)*/		{return ambient.rgb;}
		const M::TVec3<float>&		GetSpecular()		const	/** Get specular color (rgb)*/		{return specular.rgb;}
		float						GetSpotCutoff()		const	/** Get spot cutoff angle (0-90)*/	{return spotCutoff;}
		BYTE						GetSpotExponent()	const	/** Get spot exponent (0-128)*/		{return spotExponent;}
		const M::TVec3<float>&		GetAttenuation()	const	/** Get attenuation {constant, linear, quadratic} */	{return attenuation;}
		float						GetConstantAttenuation()	const	/** Get constant attenuation */		{return constantAttenuation;}
		float						GetLinearAttenuation()		const	/** Get linear attenuation */		{return linearAttenuation;}
		float						GetQuadraticAttenuation()	const	/** Get quadratic attenuation */	{return quadraticAttenuation;}
		float						GetSize()			const	/** Retrieves the geometrical light size*/	{return size;}
	};


	/**
		\brief Light configuration
		
		Configuration structure for a single light.
	*/
	class Light : public LightData, public std::enable_shared_from_this<Light>
	{
	public:

	protected:
		friend class OpenGL;
			
		index_t						index,		//!< Index of this light. Assigned by the visual interface. (light is disabled if index == UNSIGNED_UNDEF (-1))
									origin;		//!< Index of this light in the light list. Assigned by the visual interface.
		LightScene					*scene;		//!< Home light scene
		OpenGL						*parent_interface;	//!< Parent visual interface
			
		bool						moved,				//!< Signales the parent visual interface to update the light position when lighting is enabled the next time (set only if lighting is currently disabled)
									modified;			//!< Signales the parent visual interface to update the light configuration (and position) when lighting is enabled the next time (set only if lighting is currently disabled)
		
										
									Light(LightScene*scene_,OpenGL*interface_);
		void						update();

	public:

		virtual						~Light();

		Light*						setType(Type type);		//!< Set light type @return this
			
		bool						isShining() const;		//!< Checks if local light is enabled, its light scene the active scene and lighting enabled
		bool						isEnabled()	const;		//!< Checks if this light is enabled
		Light*						enable();				//!< Enable light @return this
		Light*						disable();				//!< Disable light @return this
		Light*						setEnabled(bool);		//!< Set light enabled status @return this
			
		void						Destroy();				//!< Disables and discards (deletes) the local object.
		void						discard();				//!< Disables and discards (deletes) the local object.
			

		Light*						setPosition(float x, float y, float z);		//!< Set light position. Auto normalized if type is Direct @return this
	template <typename T>
		Light*						setPosition(const M::TVec3<T>&p)				//!< Set light position {x,y,z}. Auto normalized if type is Direct @return this
									{
										return setPosition((float)p.x,(float)p.y,(float)p.z);
									}
		Light*						moveTo(float x, float y, float z);			//!< Identical to setPosition() @return this
	template <typename T>
		Light*						moveTo(const M::TVec3<T>&p)						//!< Identical to setPosition() @return this
									{
										return setPosition((float)p.x,(float)p.y,(float)p.z);
									}
			
		Light*						SetDirection(float x, float y, float z);			//!< Set spot direction if spot or negative position otherwise (auto normalized) @return this
		Light*						SetDirection(const M::TVec3<float>&c);				//!< Set spot direction if spot or negative position otherwise (auto normalized) @return this
		Light*						SetDirection(const M::TVec3<double>&c);				//!< Set spot direction if spot or negative position otherwise (auto normalized) @return this
		Light*						setNegativeDirection(float x, float y, float z);	//!< Set negative spot direction if spot or positive position otherwise (auto normalized) @return this
		Light*						setNegativeDirection(const M::TVec3<float>&c);		//!< Set negative spot direction if spot or positive position otherwise (auto normalized) @return this
		Light*						setNegativeDirection(const M::TVec3<double>&c);		//!< Set negative spot direction if spot or positive position otherwise (auto normalized) @return this
			
		Light*						setSpotDirection(const M::TVec3<float>&c);	//!< Set spot direction {x, y, z} (auto normalized) @return this
		Light*						setSpotDirection(const M::TVec3<double>&c);	//!< Set spot direction {x, y, z} (auto normalized) @return this
		Light*						setSpotDirection(float x, float y, float z);//!< Set spot direction (auto normalized) @return this

		Light*						setDiffuse(const M::TVec3<float>&c);		//!< Set diffuse color component {r, g, b} @return this
		Light*						setDiffuse(const M::TVec3<double>&c);		//!< Set diffuse color component {r, g, b} @return this
		Light*						setDiffuse(const M::TVec3<float>&c,float factor);		//!< Set diffuse color component {r, g, b} @return this
		Light*						setDiffuse(const M::TVec3<double>&c,double factor);		//!< Set diffuse color component {r, g, b} @return this
		Light*						setDiffuse(float r, float g, float b);		//!< Set diffuse color component @return this
		Light*						setDiffuse(float intensity);				//!< Set diffuse color component (r, g, b to \b intensity ) @return this
		Light*						setAmbient(const M::TVec3<float>&c);		//!< Set ambient color component {r, g, b} @return this
		Light*						setAmbient(const M::TVec3<double>&c);		//!< Set ambient color component {r, g, b} @return this
		Light*						setAmbient(const M::TVec3<float>&c,float factor);		//!< Set ambient color component {r, g, b} @return this
		Light*						setAmbient(const M::TVec3<double>&c,double factor);		//!< Set ambient color component {r, g, b} @return this
		Light*						setAmbient(float r, float g, float b);		//!< Set ambient color component @return this
		Light*						setAmbient(float intensity);				//!< Set ambient color component (r, g, b to \b intensity ) @return this
		Light*						setSpecular(const M::TVec3<float>&c);		//!< Set specular color component {r,g,b} @return this
		Light*						setSpecular(const M::TVec3<double>&c);		//!< Set specular color component {r,g,b} @return this
		Light*						setSpecular(const M::TVec3<float>&c,float factor);		//!< Set specular color component {r,g,b} @return this
		Light*						setSpecular(const M::TVec3<double>&c,double factor);		//!< Set specular color component {r,g,b} @return this
		Light*						setSpecular(float r, float g, float b);		//!< Set specular color component @return this
		Light*						setSpecular(float intensity);				//!< Set specular color component (r, g, b to \b intensity ) @return this
			
		Light*						setSpotCutoff(float angle);					//!< Set spot cutoff angle (0-90) @return this
		Light*						setSpotExponent(BYTE exponent);				//!< Set spot exponent (0-128) @return this
		Light*						setExponent(BYTE exponent);					//!< Set spot exponent (0-128)
			
		Light*						setAttenuation(float constant, float linear, float quadratic);	//!< Set attenuation @return this
	};
}


#endif
