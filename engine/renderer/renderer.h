#ifndef engine_rendererH
#define engine_rendererH


/********************************************************************

Universal visual language-interface and rendering related definitions

********************************************************************/

#include "../../math/matrix.h"
#include "../../image/image.h"
#include "../../geometry/simple_geometry.h"
#include "../../geometry/cgs.h"
#include "../enumerations.h"
#include "../../general/system.h"
#include "../../general/thread_system.h"
#include "../../container/hashtable.h"
#include "../objects/texture.h"
#include "../../interface/hashable.h"
#include "../../general/orthographic_comparator.h"

#if SYSTEM==UNIX

	#include <X11/Xlib.h>
#endif

namespace Engine
{





	//errors
	#define		ERR_NO_ERROR		0
	#define		ERR_RETRY			1
	#define		ERR_GENERAL_FAULT	(-1)


	#define DO_VERIFY_VERTEX_BINDING
	//#define DO_VERIFY_BUFFERED_DATA



	#if SYSTEM==UNIX

		/**
			\brief Window attributes
			
			Characterizes Window attributes
		*/

		struct TWindowAttributes
		{
				int							depth,
											screen_number;
				Visual						*visual;
		};

	#endif


	class VisualInterface;


	/**
		\brief Visual interface attributes
		
		Specifies attributes to be applied during the interface creation process
	*/

	struct TVisualConfig
	{
			BYTE						color_buffer_bits,			//!< Color buffer bits per pixel. Should be 16, 24 or 32
										depth_buffer_bits,			//!< Color buffer bits per pixel. Should be 16 or 24 (32 may cause trouble with FSAA)
										alpha_buffer_bits,			//!< Bits used to store transparency in the buffer. Should be 0, 4 or 8 (0 does not mean no transparency).
										accumulation_buffer_bits,	//!< Accumulation buffer bits per pixel. 0 by default.
										stencil_buffer_bits,		//!< Stencil buffer bits per pixel. Should be 0, 2, 4, or 8
										auxiliary_buffer_bits,		//!< Auxiliary buffer bits per pixel. 0 by default.
										fsaa_samples,				//!< Full screen anti aliasing samples that should be taken per pixel. (0, 2, 4, 6, 8, ...) Depending on local hardware.
										vertical_sync;				//!< 0 or 1. Set 1 to enable vertical synchronization (eats frames).
	};

	class Light;

	/**
		\brief Light setting

		Container for a light setting
	*/
	typedef List::Vector<Light> LightScene;

	/**
		\brief Light configuration
		
		Configuration structure for a single light.
	*/
	class Light
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
			
			index_t						index,		//!< Index of this light. Assigned by the visual interface. (light is disabled if index == UNSIGNED_UNDEF (-1))
										origin;		//!< Index of this light in the light list. Assigned by the visual interface.
			LightScene					*scene;		//!< Home light scene
			VisualInterface			*parent_interface;	//!< Parent visual interface
			
			bool						moved,				//!< Signales the parent visual interface to update the light position when lighting is enabled the next time (set only if lighting is currently disabled)
										modified;			//!< Signales the parent visual interface to update the light configuration (and position) when lighting is enabled the next time (set only if lighting is currently disabled)

										
			Type						type;		//!< Type of this light.
			
			TVec3<float>				position,		//!< Position of this light in R3
										spot_direction;	//!< Spot direction in R3. Needed if light type is not Omni.
			TVec4<float>				diffuse,		//!< Diffuse light vector - the 4th component is supposed to be 1.0f.
										ambient,		//!< Ambient light vector - the 4th component is supposed to be 1.0f.
										specular;		//!< Specular light vector - the 4th component is supposed to be 1.0f.
			float						spot_cutoff;		//!< Cutoff angle (0.0f - 180.0f). Only needed if light type is Spot.
			BYTE						spot_exponent;		//!< Exponent of the light Cone. Higher values mean a thinner cone.
			union
			{
				TVec3<float>			attenuation;
				struct
				{
					float				constant_attenuation,	//!< Constant component of an exponential light source. 1 (linear) by default.
										linear_attenuation,		//!< Linear component of an exponential light source. 0 by default.
										quadratic_attenuation;	//!< Quadratic component of an exponential light source. 0 by default.
				};
			};
										
										Light(LightScene*scene_,VisualInterface*interface_);
			void						update();

	public:

	virtual								~Light();

			Type						getType()	const;				//!< Retrieve light type
			Light*						setType(Type type);		//!< Set light type @return this
			
			bool						isShining() const;		//!< Checks if local light is enabled, its light scene the active scene and lighting enabled
			bool						isEnabled()	const;		//!< Checks if this light is enabled
			Light*						enable();				//!< Enable light @return this
			Light*						disable();				//!< Disable light @return this
			Light*						setEnabled(bool);		//!< Set light enabled status @return this
			
			void						destroy();				//!< Disables and discards (deletes) the local object.
			void						discard();				//!< Disables and discards (deletes) the local object.
			

			const TVec3<float>&			getPosition()	const;			//!< Get light position {x,y,z}
			Light*						setPosition(float x, float y, float z);		//!< Set light position. Auto normalized if type is Direct @return this
		template <typename T>
			Light*						setPosition(const TVec3<T>&p)				//!< Set light position {x,y,z}. Auto normalized if type is Direct @return this
										{
											return setPosition((float)p.x,(float)p.y,(float)p.z);
										}
			Light*						moveTo(float x, float y, float z);			//!< Identical to setPosition() @return this
		template <typename T>
			Light*						moveTo(const TVec3<T>&p)						//!< Identical to setPosition() @return this
										{
											return setPosition((float)p.x,(float)p.y,(float)p.z);
										}
			
			Light*						setDirection(float x, float y, float z);			//!< Set spot direction if spot or negative position otherwise (auto normalized) @return this
			Light*						setDirection(const TVec3<float>&c);				//!< Set spot direction if spot or negative position otherwise (auto normalized) @return this
			Light*						setDirection(const TVec3<double>&c);				//!< Set spot direction if spot or negative position otherwise (auto normalized) @return this
			Light*						setNegativeDirection(float x, float y, float z);	//!< Set negative spot direction if spot or positive position otherwise (auto normalized) @return this
			Light*						setNegativeDirection(const TVec3<float>&c);		//!< Set negative spot direction if spot or positive position otherwise (auto normalized) @return this
			Light*						setNegativeDirection(const TVec3<double>&c);		//!< Set negative spot direction if spot or positive position otherwise (auto normalized) @return this
			
			const TVec3<float>&			getSpotDirection()	const;					//!< Get spot direction
			Light*						setSpotDirection(const TVec3<float>&c);	//!< Set spot direction {x, y, z} (auto normalized) @return this
			Light*						setSpotDirection(const TVec3<double>&c);	//!< Set spot direction {x, y, z} (auto normalized) @return this
			Light*						setSpotDirection(float x, float y, float z);//!< Set spot direction (auto normalized) @return this

			const TVec3<float>&			getDiffuse()	const;						//!< Get diffuse color component {r,g,b}
			Light*						setDiffuse(const TVec3<float>&c);		//!< Set diffuse color component {r, g, b} @return this
			Light*						setDiffuse(const TVec3<double>&c);		//!< Set diffuse color component {r, g, b} @return this
			Light*						setDiffuse(const TVec3<float>&c,float factor);		//!< Set diffuse color component {r, g, b} @return this
			Light*						setDiffuse(const TVec3<double>&c,double factor);		//!< Set diffuse color component {r, g, b} @return this
			Light*						setDiffuse(float r, float g, float b);		//!< Set diffuse color component @return this
			Light*						setDiffuse(float intensity);				//!< Set diffuse color component (r, g, b to \b intensity ) @return this
			const TVec3<float>&			getAmbient()	const;						//!< Get ambient color component {r,g,b}
			Light*						setAmbient(const TVec3<float>&c);		//!< Set ambient color component {r, g, b} @return this
			Light*						setAmbient(const TVec3<double>&c);		//!< Set ambient color component {r, g, b} @return this
			Light*						setAmbient(const TVec3<float>&c,float factor);		//!< Set ambient color component {r, g, b} @return this
			Light*						setAmbient(const TVec3<double>&c,double factor);		//!< Set ambient color component {r, g, b} @return this
			Light*						setAmbient(float r, float g, float b);		//!< Set ambient color component @return this
			Light*						setAmbient(float intensity);				//!< Set ambient color component (r, g, b to \b intensity ) @return this
			const TVec3<float>&			getSpecular()	const;						//!< Get specular color component {r,g,b}
			Light*						setSpecular(const TVec3<float>&c);		//!< Set specular color component {r,g,b} @return this
			Light*						setSpecular(const TVec3<double>&c);		//!< Set specular color component {r,g,b} @return this
			Light*						setSpecular(const TVec3<float>&c,float factor);		//!< Set specular color component {r,g,b} @return this
			Light*						setSpecular(const TVec3<double>&c,double factor);		//!< Set specular color component {r,g,b} @return this
			Light*						setSpecular(float r, float g, float b);		//!< Set specular color component @return this
			Light*						setSpecular(float intensity);				//!< Set specular color component (r, g, b to \b intensity ) @return this
			
			float						getSpotCutoff()	const;						//!< Get spot cutoff angle (0-90)
			Light*						setSpotCutoff(float angle);					//!< Set spot cutoff angle (0-90) @return this
			BYTE						getSpotExponent()	const;					//!< Get spot exponent (0-128)
			Light*						setSpotExponent(BYTE exponent);				//!< Set spot exponent (0-128) @return this
			Light*						setExponent(BYTE exponent);					//!< Set spot exponent (0-128)
			
			const TVec3<float>&			getAttenuation()		const;				//!< Get attenuation {constant, linear, quadratic}
			Light*						setAttenuation(float constant, float linear, float quadratic);	//!< Set attenuation @return this
			float						getConstantAttenuation()	const;			//!< Get constant attenuation
			float						getLinearAttenuation()		const;			//!< Get linear attenuation
			float						getQuadraticAttenuation()	const;			//!< Get quadratic attenuation
	};


	/**
		\brief Fog configuration
		
		Configuration structure for the fog environment.
	*/
	class Fog
	{
	public:
			float						near_range, //!< Fog start (should be small compared to the scene)
										far_range,	//!< Fog end (should be large compared to the scene)
										density;	//!< Fog density
			TVec4<float>				color;	//!< Fog color - the 4th component should be 1.0f
			enum
			{
				Linear,					//!< Linear fog (commonly used)
				Exp,					//!< Exponential fog
				Exp2					//!< Exp2 fog
			}							type;		//!< Fog type
			
										Fog():near_range(0.1),far_range(100),density(1.0),type(Linear)
										{
											Vec::set(color,1);
										}
										Fog(float range, float red=1, float green=1, float blue=1):near_range(0.1),far_range(range),density(1.0),type(Linear)
										{
											Vec::def(color,red,green,blue,1);
										}
										Fog(float near_range_,float range, float red, float green, float blue):near_range(near_range_),far_range(range),density(1.0),type(Linear)
										{
											Vec::def(color,red,green,blue,1);
										}
										Fog(float range, const TVec3<>&fog_color):near_range(0.1),far_range(range),density(1.0),type(Linear)
										{
											color.rgb = fog_color;
											color.alpha = 1;
										}
										Fog(float near_range_, float range, const TVec3<>&fog_color):near_range(near_range_),far_range(range),density(1.0),type(Linear)
										{
											color.rgb = fog_color;
											color.alpha = 1;
										}
	};


	/**
		\brief Vertex float field section
		
		Struct used to specify the location of a section within the float field
	*/
	struct TVertexSection
	{
			UINT16						offset, //!< First float used for the active component with 0 indicating the first float
										length;	//!< Number of floats used for the active component. Must be 3 for normals or 0 if deactivated.

	inline								TVertexSection():length(0)
										{}
										TVertexSection(UINT16 offset_, UINT16 length_):offset(offset_),length(length_)
										{}
										
			void						set(UINT16 offset_,UINT16 length_)
										{
											offset = offset_;
											length = length_;
										}
			void						unset()
										{
											length = 0;
										}
			bool						isset() const
										{
											return length > 0;
										}
			bool						isEmpty() const
										{
											return !length;
										}

			char						compareTo(const TVertexSection&)	const;

			bool						operator==(const TVertexSection&)	const;
			bool						operator!=(const TVertexSection&other)	const		{return !operator==(other);}
			bool						operator>(const TVertexSection&other)	const		{return compareTo(other) > 0;}
			bool						operator<(const TVertexSection&other)	const		{return compareTo(other) < 0;}

			hash_t						hashCode()	const
										{
											return (hash_t(offset)<<16) | hash_t(length);
										}
			UINT16						requiredFloats()	const	//! Queries the minimum number of floats per vertex this section requires
										{
											return length>0?offset+length:0;
										}
	};

	struct TTexVertexSection : public TVertexSection
	{
			bool						generate_reflection_coords;			//!< Reflect texture coordinates. Most commonly only applied when mapping cubes. Effective only if @a TVertexSection::length is 0
			
										TTexVertexSection():generate_reflection_coords(false)
										{}
	};

	/**
		\brief Material layer configuration
		
		Structure holding basic material layer information
	*/
	class MaterialLayer:public IHashable
	{
	public:
			typedef PixelType				content_t;


			SystemType						system_type;			//!< Texcoord system type. Shaders may ignore this setting. Defaults to Identity
			content_t						content_type;			//!< Type of contained texel. Shaders may ignore this setting. Defaults to Color
			

			bool						clamp_x,			//!< Clamp texture coordinates along x axis. Disabled by default
										clamp_y,			//!< Clamp texture coordinates along y axis. Disabled by default
										clamp_z,			//!< Clamp texture coordinates along z axis. Disabled by default
										enabled;			//!< Layer is enabled. Other values are not reliable and may be filled with garbage if this value is set false. Defaults to true
			UINT32						combiner;			//!< GL combiner (usually GL_MODULATE or GL_DECAL). Somewhat deprecated.
			const TMatrix4<>			*custom_system;		//!< Remote texcoord 4x4 system matrix. Must be set non-NULL if system_type is SystemType::Custom. This pointer may be invalid if system_type is NOT SystemType::Custom.
			
										MaterialLayer()
										{
											resetToDefault();
										}

			void						resetToDefault();

			void						setClamp(bool do_clamp)
										{
											clamp_x = clamp_y = clamp_z = do_clamp;
										}
			bool						isNormalMap() const
										{
											return content_type != PixelType::Color;
										}
			bool						isValid() const
										{
											return enabled
													&& system_type != SystemType::None
													&& content_type != PixelType::None
													&& (system_type != SystemType::Custom || custom_system != NULL);
										}
										
			char						compareTo(const MaterialLayer&)		const;

			bool						operator==(const MaterialLayer&)		const;
			bool						operator!=(const MaterialLayer&other)	const		{return !operator==(other);}
			bool						operator>(const MaterialLayer&other)	const		{return compareTo(other) > 0;}
			bool						operator<(const MaterialLayer&other)	const		{return compareTo(other) < 0;}

	virtual	hash_t						hashCode()	const
										{
											return enabled? 
													HashValue(system_type)
													.add(content_type)
													.add( (UINT32(clamp_x)<<24) | (UINT32(clamp_y)<<16) | (UINT32(clamp_z)<<8)  /*| UINT32(reflect)*/)
													.add(combiner)
													.add(system_type == SystemType::Custom && custom_system != NULL ? stdMemHash(custom_system,16*sizeof(float)) : 0)
													:
													0;
										}
	};


	/**
		@brief Color definition
	*/
	struct TColor:public TVec4<>
	{
			typedef TVec4<>	Super;

			void						set(float intensity)	//! Sets the intensity of the local color. red, green, and blue are set to the specified value, alpha is set to 1.0
										{
											red = green = blue = intensity;
											alpha = 1.0f;
										}
			void						set(float new_red, float new_green, float new_blue, float new_alpha = 1.0f)	//!< Updates the local color vector to the specified color values
										{
											red = new_red;
											green = new_green;
											blue = new_blue;
											alpha = new_alpha;
										}
			void						set(const TVec3<float>&channel_values)	//!< Updates the local color vector. The alpha channel is set to 1.0
										{
											rgb = channel_values;
											alpha = 1.0f;
										}

			hash_t						hashCode()	const
										{
											return HashValue().addFloat(red).addFloat(green).addFloat(blue).addFloat(alpha);
										}

			bool						operator==(const TColor&other) const
										{
											return Vec::similar(*this,other);
										}

			char						compareTo(const TColor&other) const
										{
											return Vec::compare(*this,other);
										}

			bool						operator!=(const TColor&other)	const		{return !operator==(other);}
			bool						operator>(const TColor&other)	const		{return compareTo(other) > 0;}
			bool						operator<(const TColor&other)	const		{return compareTo(other) < 0;}


			TColor&						operator=(const TVec4<>&other)	{Super::operator=(other); return*this;}
	};

	/**
		@brief Material color configuration
	*/
	class MaterialColors : public IHashable
	{
	public:
			TColor						ambient,		//!< Ambient light intensity vector
										diffuse,		//!< Diffuse light intensity vector
										specular,		//!< Specular light intensity vector
										emission;		//!< Emission light intensity vector
			float						alpha_threshold;	//!< @deprecated{should implement this feature using shaders} Minimum alpha value for a fragment to be visible. Effective only if @a alpha_test is set true
			bool						alpha_test,			//!< @deprecated{should implement this feature using shaders}
										blend,				//!< Indicates that alpha blending should be used for this material (true by default)
										fully_reflective;	//!< Configures the shader to apply any reflection map everywhere, thus overriding any other material parameters (safe for normal mapping)
			BYTE						shininess_exponent;	//!< Shininess exponent in the range 1-128. The higher, the sharper the reflected cone will be. Defaults to 16

										MaterialColors();
	virtual								~MaterialColors()	{};
	virtual	hash_t						hashCode() const;

			void 						read(const CGS::MaterialInfo&info);	//!< Fills all local variables in accordance with the specified CGS material

			char						compareTo(const MaterialColors&)	const;

			bool						operator==(const MaterialColors&)	const;
			bool						operator!=(const MaterialColors&other)	const		{return !operator==(other);}
			bool						operator>(const MaterialColors&other)	const		{return compareTo(other) > 0;}
			bool						operator<(const MaterialColors&other)	const		{return compareTo(other) < 0;}

			void						resetToDefault();
	};

	/**
		@brief Basic vertex binding

		Contains sections for vertex coordinates, normal, tangent, and color
	*/
	class VertexBinding	: public IHashable
	{
	public:
		TVertexSection					vertex,				//!< Section of the float field used to specify the position of a vertex
										tangent,			//!< Section of the float field used to specify the tangent of a vertex. This field is required if the material features tangen space normal maps. OpenGL maps this section one past the last used texcoord field
										normal,				//!< Section of the float field used to specify the normal of a vertex
										color;				//!< Section of the float field used to specify the color of a vertex (if lighting is disabled)
		Array<TTexVertexSection>		texcoords;			//!< Texcoord binding
		UINT16							floats_per_vertex;	//!< Vertex size in elements. This value is used to calculate the vertex stride value and must include all active vertex sections

		/**/							VertexBinding(count_t layers=0);
		virtual							~VertexBinding()	{};
		virtual hash_t					hashCode() const override;

		void							swap(VertexBinding&other)
										{
											swp(vertex,other.vertex);
											swp(tangent,other.tangent);
											swp(normal,other.normal);
											swp(color,other.color);
											texcoords.swap(other.texcoords);
											swp(floats_per_vertex,other.floats_per_vertex);
										}

		char							compareTo(const VertexBinding&)	const;

		bool							operator==(const VertexBinding&)		const;
		bool							operator!=(const VertexBinding&other)	const	{return !operator==(other);}
		bool							operator>(const VertexBinding&other)	const	{return compareTo(other) > 0;}
		bool							operator<(const VertexBinding&other)	const	{return compareTo(other) < 0;}

		UINT16							minFloatsPerVertex()	const;	//!< Calculates the minimal number of floats per vertex that this binding requires. A valid vertex binding complies with minFloatsPerVertex() <= floats_per_vertex

		virtual	void 					read(const CGS::MaterialInfo&info, UINT32 flags);	//!< Fills all local variables in accordance with the specified CGS material, floats per coordinate and flags

		void							resetToDefault(bool erase_layers=true);		//!< Restores default layer binding @param erase_layers Set true to resize the number of texcoord layers to 0. If set false then the existing layers will be set to default values
	};

	typedef VertexBinding	VertexDefinition,VertexDeclaration;


	/**
		\brief Material configuration
		
		Structure holding material finding information
	*/


	class MaterialConfiguration:public MaterialColors
	{
	public:
			Array<MaterialLayer>		layers;
			String						name;
			
										MaterialConfiguration(count_t layers=0); //!< Constructs a new material \param layers Number of texture layers to use
										MaterialConfiguration(const CGS::MaterialInfo&, UINT32 flags);
										MaterialConfiguration(const CGS::MaterialInfo*, UINT32 flags);

			void						resetToDefault(bool colors=true, bool erase_layers=false); //!< Loads default values into the structure. \param colors If set true then default colors will be loaded. @param erase_layers If set true then the number of layers will be reduced to zero

	virtual	void						setLayers(count_t num_layers)	{layers.resizePreserveContent(num_layers);/*texcoords.resizePreserveContent(num_layers);*/};	//!< Changes the number of layers. Copies the content of the existing layer array. \param layers New number of layers.

	virtual	void 						read(const CGS::MaterialInfo&info, UINT32 flags);	//!< Fills all local variables in accordance with the specified CGS material, floats per coordinate and flags

			char						compareTo(const MaterialConfiguration&)		const;

			bool						operator==(const MaterialConfiguration&)		const;
			bool						operator!=(const MaterialConfiguration&other)	const	{return !operator==(other);}
			bool						operator>(const MaterialConfiguration&other)	const	{return compareTo(other) > 0;}
			bool						operator<(const MaterialConfiguration&other)	const	{return compareTo(other) < 0;}

			void						adoptData(MaterialConfiguration&other)
										{
											MaterialColors::operator=(other);
											layers.adoptData(other.layers);
											name.adoptData(other.name);
										}
			void						swap(MaterialConfiguration&other)
										{
											swp(((MaterialColors&)*this),((MaterialColors&)other));
											layers.swap(other.layers);
											name.swap(other.name);
										}
	

	virtual	hash_t						hashCode() const;

	//virtual	bool						isConsistent() const	{return VertexBinding::texcoords.count() == layers.count();}
	};



	typedef MaterialConfiguration	MaterialConfig;

	extern TVisualConfig				default_buffer_config;
	extern TMatrix4<float>				environment_matrix;
	extern bool							erase_unused_textures;


	namespace VisualEnum
	{
		
		enum eLayerFlags //! Material layer configuration flags
		{
			MirrorFlag			= CGS::MirrorFlag,	//!< Derived from CGS::MirrorFlag. Causes generated 3d texture coordinates to immitate the behavior of reflected light-rays. Effective only in combination with CubeMapFlag
			ClampXFlag			= CGS::ClampXFlag,	//!< Derived from CGS::ClampXFlag. Causes non-generated texture coordinates to be clamped to the range [0,1] along the X/U-axis
			ClampYFlag			= CGS::ClampYFlag,	//!< Derived from CGS::ClampYFlag. Causes non-generated texture coordinates to be clamped to the range [0,1] along the Y/V-axis
			ClampZFlag			= CGS::ClampZFlag,	//!< Derived from CGS::ClampZFlag. Causes non-generated texture coordinates to be clamped to the range [0,1] along the Z/W-axis
			CustomSystemFlag	= 0x0200,			//!< Specifies that a custom or the environment matrix should be loaded as texcoord transformation matrix of this layer
			EnvSystemFlag		= 0x0400,			//!< Specifies that the environment matrix should be loaded as texcoord transformation matrix of this layer. Effective only in combination with CustomSystemFlag
			CubeMapFlag			= 0x0800,			//!< Specifies that the texture bound to this layer should be a cube map. Required for MirrorFlag
			LayerDisabledFlag	= 0x1000,			//!< Specifies that this layer should not be rendered
			BlendLayerFlag		= 0x2000,			//!< Non implemented stub.
			NormalMapFlag		= 0x4000,			//!< Specifies that this layer's texture should be interpreted as a normal map
			TangentNormalFlag	= 0x8000			//!< Specifies that the normals provided by this layer should be interpreted as tangent space, rather than object space. Effective only in combination with @a NormalMapFlag
		};
		
	}



	/* ** objects **

	objects must:

		- auto-initialize to empty
		- be copyable via operator= and constructor
		- be comparable via operator== and operator!=
		- be comparable via operator< and operator>
		- NOT auto-delete on destruction (except local data)
		- be distinguishable so ambiguities cannot occur on any method other than discardObject(...) and isEmpty(...)
		- NOT keep references to client-data
		- provide a const isEmpty()-method to determine whether not not content is available.
		
	*/


	template <typename Texture>
		class MaterialComposition:public MaterialConfiguration //! Eve material container (including textures)
		{
		public:
				Array<Texture>		textures;


		inline	bool				operator==(const MaterialComposition<Texture>&other) const
									{
										return MaterialConfiguration::operator==(other) && textures == other.textures;
									}

		inline	char				compareTo(const MaterialComposition<Texture>&other)	const
		{
			return OrthographicComparison(MaterialConfiguration::compareTo(other)).addComparison(textures,other.textures);
		}

		inline	bool				operator!=(const MaterialComposition<Texture>&other)	const	{return !operator==(other);}
		inline	bool				operator>(const MaterialComposition<Texture>&other)	const	{return compareTo(other) > 0;}
		inline	bool				operator<(const MaterialComposition<Texture>&other)	const	{return compareTo(other) < 0;}

						
		virtual	void				setLayers(count_t num_layers)
									{
										MaterialConfiguration::setLayers(num_layers);
										textures.resizePreserveContent(num_layers);
									}

		virtual	void 				read(const CGS::MaterialInfo&info, UINT32 flags)
									{
										MaterialConfiguration::read(info,flags);
										textures.setSize(MaterialConfiguration::layers.count());
									}

				void				adoptData(MaterialComposition&other)
									{
										MaterialConfiguration::adoptData(other);
										textures.adoptData(other.textures);
									}
				void				swap(MaterialComposition&other)
									{
										MaterialConfiguration::swap(other);
										textures.swap(other.textures);
									}

		virtual	hash_t				hashCode() const
									{
										HashValue hash(MaterialConfiguration::hashCode());
										hash.add(stdMemHash(textures.pointer(),textures.contentSize()));

										return hash;
									}

		virtual	bool				isConsistent() const	{return MaterialConfiguration::layers.count() == textures.count();}

		};
	
	
	
	struct	TExtShaderConfiguration		//! Additional shader configuration
	{
			bool				requires_tangents;
	
	};
	
	class	ShaderSourceCode:public TExtShaderConfiguration	//! Defined shader source code includig configuration
	{
	public:
			String				code;
			count_t				num_samplers;
			bool				sky_lighting;

								ShaderSourceCode():num_samplers(0),sky_lighting(false)
								{}
			
	
	};
	
	
	/*template <class GL>
		void	convertMaterial(const MaterialConfiguration&material, const typename GL::Texture**list, CGS::GraphMaterial&target);

	template <class GL>
		void	convertMaterial(const Material<GL>&material, CGS::GraphMaterial&target)
		{
			convertMaterial(material,material.textures(),target);
		}
	*/
	
	
	
	
	
	
	
	
	
	

	/**
		\brief Visual interface super class

		All interfaces (currently only OpenGL) are supposed to comply with the specifications in this file

	*/
	class VisualInterface
	{
	protected:
			index_t							active_scene_index,		//!< Current light scenario index
											scene_index_counter;
			IndexContainer<LightScene>		scenes;				//!< Light scenario container (index mapped)
			LightScene						*active_scene;		//!< Currently active light scenario
			bool							lighting_enabled;	//!< Lighting is currently enabled (some rendering processes behave differently when lighting is enabled)
		
			friend class Light;


	virtual void							enableLight(Light*)=0;
	virtual	void							disableLight(Light*)=0;
	virtual	void							updateLight(Light*)=0;
	virtual void							updateLightPosition(Light*)=0;

	static	index_t							getIndexOf(Light*);
	static	void							setIndexOf(Light*,index_t);
	static	LightScene*						getLightSceneOf(Light*);
	static	bool							hasMoved(Light*);
	static	bool							wasModified(Light*);
	static	void							setHasMoved(Light*,bool);
	static	void							setWasModified(Light*,bool);
			
			

		
	public:

		/**
			Interface mutex to shield the underlieing visual language (i.e. OpenGL or Direct3D) from multiple parallel calls.
		*/
	static	Mutex							vs_mutex;

											VisualInterface();
		
	virtual	Light*							createLight(bool enable=true);			//!< Creates a new light. \param enable Directly enable created light (if possible). \return New light in the currently active light scenario
	virtual	void							discardLight(Light*light);			//!< Discards a light. \param light Light to discard. The light is not required to belong to the currently active light scenario.
	virtual	void							clearLights();							//!< Discards all lights in the active light scenery
	virtual	void							resetLighting();						//!< Discards all light scenarios except the primary and clears the primary light scenario
		
			index_t							createLightScene();						//!< Creates a new light scenario (collection of lights). \return Index of a new light scenario.
			void							discardLightScene(index_t scenario);	//!< Erases a given light scenario. \param scenario Index of an existing scenario that should be erased.
			bool							isLightScene(index_t scenario)	const;	//!< Queries if the specified light scenario exists. \param scenario Index of the light scenario that should be queried.
	virtual	bool							pickLightScene(index_t  scenario)=0;	//!< Picks a light scenario as active scenario (may be slow). \param scenario Index of an existing scenario that should be used from this point on (0 = default scenario). \return true on success
			index_t							getLightScene()	const;
			void							getSceneLights(ArrayData<Light*>&array, bool enabled_only);	//!< Retrieves pointers to all lights in the active light scene

	/*

		typedef			<>				FloatType;			//used for vertex-data
		typedef			<>				IndexType;			//used for index-data
		typedef			<>				VBOReference;		//reference to uploaded vertex data
		typedef			<>				IBOReference;		//reference to uploaded index data
		typedef			<>				TextureReference;	//reference to uploaded texture data
		typedef			<>				ShaderReference;	//reference to uploaded shader data


		static			bool			textureObject(TextureObject&target,const Image*image, unsigned faces = 1, unsigned texture_mode=TM_TRILINEAR, bool patternize=true);
		static			TextureObject	textureObject(const Image*image, unsigned faces = 1, unsigned texture_mode=TM_TRILINEAR, bool patternize=true);
		static			bool			textureObject(TextureObject&target,const Image**image,unsigned faces = 1, unsigned texture_mode=TM_TRILINEAR, bool patternize=true);
		static			TextureObject	textureObject(const Image**image, unsigned faces = 1, unsigned texture_mode=TM_TRILINEAR, bool patternize=true);
		static			bool			vertexObject(VertexObject&target,FloatType*data,unsigned length);
		static			VertexObject	vertexObject(FloatType*data, unsigned length);
		static			bool			indexObject(IndexObject&target,IndexType*data,unsigned length);
		static			IndexObject		indexObject(GLuint*data, unsigned length);
		static inline	TextureObject	emptyTextureObject();
		static inline	VertexObject	emptyVertexObject();
		static inline	IndexObject		emptyIndexObject();
		static			void			duplicateTexture(const TextureObject&object);
		static			void			discardTexture(const TextureObject&object);
		static			void			discardVertices(const VertexObject&object);
		static			void			discardIndices(const IndexObject&object);
		static			void			alterBinding(TextureObject*list);
		static			void			alterBinding(unsigned layer, const TextureObject&object);
	T	static			void			bindMaterial(CVMaterialInfo<C>*info,TextureObject*list);
	T	static			void			bindVertexObject(C*vdata, const VertexObject&vobj, unsigned coord_band=3, unsigned tlyr=0);
		static			void			bindIndexObject(const IndexObject&iobj);
	T2	static inline	void			render(C0*system, C1*idata, unsigned vertex_offset, unsigned vertex_count);
	T	static inline	void			render(C*idata, unsigned vertex_offset, unsigned vertex_count);
	T2	static inline	void			renderStrip(C0*system, C1*idata, unsigned vertex_offset, unsigned vertex_count);
	T	static inline	void			renderStrip(C*idata, unsigned vertex_offset, unsigned vertex_count);
		static			void			render(const Structure&structure);
	T	static inline	void			enterSubSystem(C*system);
		static inline	void			exitSubSystem();
		static			void			unbindAll();
	T	static			void			loadModelview(const C*matrix);
	T	static			void			loadProjection(const C*matrix);
						void			nextFrameNoClr();
						void			nextFrame();

		static			String			status();


	#if SYSTEM==WINDOWS
						bool			createContext(HINSTANCE hInstance, HWND hWnd, VisualConfig&config);
	#elif SYSTEM==UNIX
						XVisualInfo*	createContext(Display*connection, VisualConfig&config);
						bool			bindContext(Window window);
	#endif
						void			destroyContext();
						void			setRegion(const RECT&region);
						const char*		getErrorStr();
						int				getErrorCode();
						const char*		name();
	#if SYSTEM==WINDOWS
						HDC				getDC();
	#elif SYSTEM==UNIX
						XVisual*		getVisual();
	#endif
	*/
	};
}

#include "renderer.tpl.h"

#endif
