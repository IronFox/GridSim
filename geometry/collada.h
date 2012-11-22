#ifndef colladaH
#define colladaH

#include "../io/xml.h"
#include "../list/lvector.h"
#include "../list/hashlist.h"

/******************************************************************

E:\include\structure\collada.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

namespace Collada
{
	template <class C> class List:public Vector<C>{};
	template <class C> class Map:public StringMappedList<C>{};


	class Node
	{
	public:
			String						id, name;
			Asset						asset;
	};
	
	
	class Accessor
	{
	public:
			unsigned					count,
										offset,
										stride;
			String						source;
	};

	class Source:public Node
	{
	public:
			enum Type{IDREF,Name,Bool,Float,Int};
			
			union
			{
				Field<String>			*idref_field;
				Field<String>			*name_field;
				Field<bool>			*bool_field;
				Field<float>			*float_field;
				Field<int>				*int_field;
			};
			
			Type						type;
			Accessor					common_accessor;	//only technique_common accepted here
	};
	
	class Input
	{
	public:
			unsigned					offset, set;
			String						semantic,
										source;
			Source						*source_ref;
	};

	class Image:public Node
	{
	public:
	static const String				xml_parent="library_images",
										xml_child="image";
			String						format, init_from, data;
			unsigned					width,height,depth;
			//List<Extra>					extras;
	};
	
	
	class Sampler
	{
	public:
	const String						input_semantics="INPUT, INTERPOLATION, IN_TANGENT, OUT_TANGENT, OUTPUT";
			String						id;
			List<Input>				inputs;
	};
	
	class Channel
	{
	public:
			String						sampler,target;
			Sampler					*sampler_ref;
	};
	


	class Animation:public Node
	{
	public:
	static const String				xml_parent="library_animations",
										xml_child="animation";
	
			Map<Animation>			sub_animations;
			Map<Source>				sources;
			List<Sampler>				samplers;
			List<Channel>				channels;
			//List<Extra>				extras;
	};
	
	class InstanceAnimation:public Node
	{
	public:
			String						url;
			Animation					*animation_ref;
			//List<Extra>				extras;
	};

	class AnimationClip:public Node
	{
	public:
	static const String				xml_parent="library_animation_clips",
										xml_child="animation_clip";

			double						start,end;
			Map<InstanceAnimation>	instances;
			//List<Extra>				extras;
	};
	
	
	class Optics
	{
	public:
			float						xmag,ymag,	//orthographic only
										xfov,yfov,	//perspective only
										aspect_ratio,
										znear,zfar;
			bool						is_perspective;
			//List<Extra>				extras;
	};
	
	
	class Camera:public Node
	{
	public:
	static const String				xml_parent="library_cameras",
										xml_child="camera";
										
			Optics						optics;
			//Imager						imager;
			//List<Extra>				extras;
	};
	
	class Joints
	{
	public:
			Input						name_field;
			List<Input>				other_inputs;
			//List<Extra>				extras;
	};
	
	class VertexWeight
	{
	public:
			unsigned					count;	//number of vertices
			Array<unsigned>			vcount;	//number of bones per vertex
			Input						joint_input;
			List<Input>				other_inputs;
			Array<float>				v;	//whatever
			//List<Extra>				extras;
	};
	
	class Skin
	{
	public:
			String						source;
			float						bind_shape_matrix[4*4];
			Source						joints,weights,inv_bind_mats;
			List<Source>				other_sources;
			Joints						joints;
			VertexWeights				vertex_weights;
			//List<Extra>				extras;
	};
	
	class Targets
	{
	public:
			Input						target_input,
										weight_input;
			List<Input>				other_inputs;
			//List<Extra>				extras;
	};
	
	class Morph
	{
	public:
			enum Method{Normalized,Relative};
			
			Method						method;
			String						source;
			Source						target_source,
										weight_source;
			List<Source>				other_sources;
			Targets					targets;
			//List<Extra>				extras;
	};
	
	class Controller:public Node
	{
	public:
	static const String				xml_parent="library_controllers",
										xml_child="controller";
			union
			{
				Skin					*skin;
				Morph					*morph;
			};
			bool						is_morph;
			//Map<Extra>				extras;
	};
	
	class CFXTechnique
	{
	public:
			String						id, sid,
										method;		//"constant", "lambert", "phong", or "blinn"
			float						emission[3],
										ambient[3],
										diffuse[3],
										specular[3],
										shininess,
										reflective[3],
										reflectivity,
										transparent[3],
										transparency,
										index_of_refraction;
	};
	
	
	class Parameter
	{
	public:
			String						sid, 		//variable/parameter name
										semantic,
										modifier,	//'CONST', 'UNIFORM', 'VARYING', 'STATIC', 'VOLATILE', 'EXTERN', 'SHARED'
										type,		//'int', 'float', 'string', <class-name>
										type_ref;
			List<Annotate>			annotations;
			List<String>				init_values;
	};
	
	class SetParameter
	{
	public:
			String						ref,
										program,
										type,		//'int', 'float', 'string', <class-name>
										type_ref;
			List<Annotate>			annotations;
			List<String>				init_values;
	};

	class Code
	{
	public:
			String						sid,
										content;
	};
	
	class Include
	{
	public:
			String						sid,
										url;
	};
	
	class Profile
	{
	public:
			String						id,
										plattform,
										profile;	//CG, GLSL, COMMON
			Asset						asset;
			List<Code>				code;
			List<Include>				includes;
			List<Image>				images;
			List<Parameter>			parameters;
			CFXTechnique				technique;
			//List<Extra>				extras;
	};
	
	class Effect:public Node
	{
	public:
	static const String				xml_parent="library_effects",
										xml_child="effect";
										
			List<Annotate>			annotations;
			List<Image>				images;
			List<Parameter>			parameters;
			List<Profile>				profiles;
			//List<Extra>				extras;
	};
	
	class ForceField:public Node
	{
	public:
	static const String				xml_parent="library_force_fields",
										xml_child="force_field";
	};
	
	
	class Mesh
	{
	public:
			String						convex_hull_of;
			List<Source>				vertex_sources;
			Vertices					vertices;
			List<Lines>				lines;
			List<LineStrips>			line_strips;
			List<Polygons>			polygons;
			List<PolyLists>			polylists;
			List<Triangles>			triangles;
			List<TriangleFans>		triangle_fans;
			List<TriangleStrips>		triangle_strips;
			//List<Extra>					extras;
	};
	
	
	class Spline
	{
	public:
			bool						closed;
			List<Source>				sources;
			Input						control_position_input;
			List<Input>				other_control_inputs;
			//List<Extra>					extras;
	};
	
	class Geometry:public Node
	{
	public:
	static const String				xml_parent="library_geometries",
										xml_child="geometry";
										
			enum Type{Mesh,ConvexMesh,Spline};
			union
			{
				Mesh					*mesh;
				Spline					*spline;
			};
			Type						type;
	
			//List<Extra>					extras;
	};
	
	class Light:public Node
	{
	public:
	static const String				xml_parent="library_lights",
										xml_child="light";
			enum Type{Ambient,Directional,Point,Spot};
			Type						type;
			float						color[3],
										constant, linear, quadratic, 		//attenuation
										falloff_angle, falloff_exponent;	//spot only
			
			//List<Extra>					extras;
	};
	
	class TechniqueHint
	{
	public:
			String						platform,
										ref,
										profile;
	};
	
	class MaterialResource		//instance_effect
	{
	public:
			String						sid, name, url;
			List<TechniqueHint>		hints;
			List<SetParameter>		parameters;	//"AMBIENT", "DIFFUSE", "SPECULAR", "SHININESS", ...
			//List<Extra>					extras;
	};
	
	class Material:public Node
	{
	public:
	static const String				xml_parent="library_materials",
										xml_child="material";
			MaterialResource			instance_effect;
			//List<Extra>					extras;
	};
	

	template <class C> class Library:public Node
	{
	public:
			Map<C>						entries;
			//List<Extra>				extras;
	};
	

	class Structure
	{
	public:
			String						version, xml_namespace, base;
			
			Asset						asset;
			Library<Animation>		animations;
			Library<AnimationClip>	animation_clips;
			Library<Camera>			cameras;
			Library<Controller>		controllers;
			Library<Effect>			effects
			Library<ForceField>		force_fields;
			Library<Geometry>			geometries;
			Library<Image>			images;
			Library<Light>			lights;
			Library<Material>			materials;
			Library<Node>				nodes;
			Library<PhysicsMaterial>	physics_materials;
			Library<PhysicsModel>		physics_models;
			Library<PhysicsScene>		physics_scenes;
			Library<VisualScene>		visual_scenes;
			Scene						scene;
			//List<Extra>					extras;
	};


	





}











#endif
