#ifndef engine_gl_extensionsH
#define engine_gl_extensionsH


#include "../../gl/gl.h"
#include "../../gl/glext.h"

#undef glBlendFuncSeparate
#undef glGenBuffers
#undef glBindBuffer
#undef glBufferData
#undef glDeleteBuffers

#ifndef __CUDACC__

	#include "../../gl/gl_enhancement.h"
	#include <stdio.h>
	#include "../renderer/renderer.h"
	#include "../../container/string_list.h"
	#include "../../string/string_buffer.h"
	//#include "../../container/string_list.h"
	#include "../../string/tokenizer.h"
	#include "../../math/resolution.h"
	#include "../../io/file_stream.h"

	#if SYSTEM==WINDOWS
		#include "../../gl/wglext.h"
	#elif SYSTEM==UNIX
		#ifndef Bool
			typedef bool	Bool;
		#endif
		#include "../../gl/glx.h"
		#include "../../gl/glxext.h"
	#endif


		


	namespace Engine
	{

		#define EXT_MULTITEXTURE_BIT			0x0001
		#define EXT_REGISTER_COMBINERS_BIT		0x0002
		#define EXT_VERTEX_PROGRAM_BIT			0x0004
		#define EXT_VERTEX_BUFFER_OBJECT_BIT	0x0010
		#define EXT_COMPILED_ARRAYS_BIT			0x0020
		#define EXT_SHADER_OBJECTS_BIT			0x0040
		#define EXT_WIN_CONTROL_BIT				0x0080
		#define EXT_PBUFFER_BIT					0x0100
		#define EXT_RENDER_TEXTURE_BIT			0x0200
		#define EXT_OCCLUSION_QUERY_BIT			0x0400
		#define EXT_MAKE_CURRENT_READ_BIT		0x0800
		#define EXT_WINDOW_POS_BIT				0x1000
		#define EXT_FRAME_BUFFER_OBJECT_BIT		0x2000
		#define EXT_BLEND_FUNC_SEPARATE_BIT		0x4000

		

		CONSTRUCT_ENUMERATION4(DepthStorage,Texture,PrivateBuffer,SharedBuffer,NoDepthStencil);

		/*


		#if SYSTEM==WINDOWS
		#define GET_GL_PROC_ADDRESS(name) wglGetProcAddress(name)
		#elif defined(GLX_ARB_get_proc_address)
		#define GET_GL_PROC_ADDRESS(name) glXGetProcAddressARB((GLubyte*) name)
		#endif
		*/

		class ExtCont
		{
		public:
				Array<String,Adopt>	field;

				void					divide(String line);
				bool					lookup(const String&element);
				String					listExtensions(const String&plank="", count_t max_chars_per_line=0);
				String					listColumns(const String&plank="", BYTE columns = 4);
		};

		struct TPBuffer
		{
			#if SYSTEM==WINDOWS && defined(WGL_ARB_pbuffer)
				HDC			device_context;
				HPBUFFERARB buffer;
				HGLRC		buffer_context;
			#elif SYSTEM==UNIX && defined(GLX_SGIX_pbuffer)
				GLXPbuffer	buffer;
				GLXContext	buffer_context;
				Display	*display;
				//to be continued...

			#endif
				GLuint		texture;
				int			width,height;
				bool		enabled;
		};
		
		/**
			@brief Individual color target configuration
		*/
		struct TFBOColorTarget
		{
			GLuint			texture_handle;		//!< Render target texture handle
			GLenum			texture_format;		//!< Internal texture format

							TFBOColorTarget():texture_handle(0),texture_format(0)
							{}
		};

		struct TFBODepthTarget
		{
			GLuint			handle;					//!< Handle of the texture or depth buffer, depending on is_texture
			DepthStorage	storage_type;			//!< Storage type for depth information


							TFBODepthTarget():handle(0)
							{}
		};

		/**
			@brief Multi-target FBO

			Up to four color targets may be addressed simultaneously
		*/
		struct TFBOConfig
		{
			TFBOColorTarget	color_target[4];	//!< Color texture references. References beyond num_color_targets are undefined
			TFBODepthTarget	depth_target;		//!< Points to a depth buffer or depth texture, depending on depth_is_texture
			Resolution		resolution;			//!< Resolution in pixels of all render targets and the depth buffer/texture
			BYTE			num_color_targets;	//!< Number of targets in color_target to actually use
		
		
							TFBOConfig():num_color_targets(0)
							{}
		};
		
		struct TFrameBuffer:public TFBOConfig
		{
			GLuint		frame_buffer;	//!< General rendering target (frame buffer)
						
						TFrameBuffer():frame_buffer(0)
						{}
						TFrameBuffer(GLuint buffer, const TFBOConfig&attachment):frame_buffer(buffer),TFBOConfig(attachment)
						{}
		};


		#ifdef GL_ARB_shader_objects
		

		namespace GLShader
		{

			class	Instance;
		
			class	Variable
			{
			private:
				Instance				*instance;
				GLint					handle;
				static	bool			lock_uninstalled,	//less strict
										assert_is_installed;//strict
				String					name;
				
				friend class GLShader::Instance;
				
				/**/					Variable(Instance*,GLint,const String&name_);
			public:
				/**/					Variable();

				bool					setf(float value);
				inline bool				Set(float value)		{return setf(value);}
				bool					set4f(float x, float y, float z, float w);
				inline bool				Set(float x, float y, float z, float w)		{return set4f(x,y,z,w);}
				bool					set(const TVec2<>&);
				inline bool				Set(const TVec2<>&v)	{return set(v);}
				bool					set(const TVec3<>&);
				inline bool				Set(const TVec3<>&v)	{return set(v);}
				bool					set(const TVec4<>&);
				inline bool				Set(const TVec4<>&v)	{return set(v);}
				bool					set(const TMatrix3<>&);
				inline bool				Set(const TMatrix3<>&m)	{return set(m);}
				bool					set(const TMatrix4<>&);
				inline bool				Set(const TMatrix4<>&m)	{return set(m);}
				bool					seti(int value);
				inline bool				SetInt(int value)		{return seti(value);}
				bool					set2i(int x, int y);
				inline bool				SetInt(int x, int y)		{return set2i(x,y);}
				
				static	void			lockUninstalledShaderVariables()
										{
											lock_uninstalled = true;
										}
				static	void			assertShaderIsInstalled()
										{
											assert_is_installed = true;
										}
				
				bool					exists()	const {return handle != -1;}
				operator				bool()	const {return handle != -1;}
			};

			bool		extractFileContent(const String&filename, String&target, StringBuffer&log_out);
			bool		extractFileContent(const String&filename, String&target);


			class Composition
			{
			public:
				String					shared_source,
										vertex_source,
										fragment_source, 
										geometry_source;

				void					adoptData(Composition&other);
				void					clear();
				bool					isEmpty()	const	{return shared_source.isEmpty() && vertex_source.isEmpty() && fragment_source.isEmpty() && geometry_source.isEmpty();}
				bool					isNotEmpty()const	{return !isEmpty();}
				Composition&			load(const String&object_source);
				bool					loadFromFiles(const String&shared_file, const String&vertex_file, const String&fragment_file);
				bool					loadFromFiles(const String&shared_file, const String&vertex_file, const String&fragment_file, const String&geometry_file);
				bool					loadFromFile(const String&object_file);
			};


			class Template;
			class Instance;

		
				/**
					@brief GLSL Shader precompiler
			
			
					The shader template is a powerful structure to automatically assemble shaders depending on the current opengl light/fog status and user-definable variables.
					Code formatting:<br />
					Similar to the GLShader::Instance class shader code may be provided as individual source blocks, or one block with source segments separated by [vertex], [fragment], and [geometry] respectivly.
					In addition to the normal shader functionality precompiler directives become available in the form #directive much like C/C++ precompiler lines. Supported directives are:<ul>
					<li>#if [condition]<br />
						inserts the code specified in the following lines if the specified condition resolves to true. [condition] supports standard operators + - * / && || ! == != < > <= >= and parenthesis to group expressions.
						apart from said operators integer/boolean constants may be used and/or global integer variables. The following variables are predefined and automatically filled if appropriate:<ul>
						<li>fog: this variable will be filled with 1 if fog is currently enabled and 0 if not</li>
						<li>omni: this variable is valid only if currently inside a light-loop block and set to 1 if the currently processed light is of type omni</li>
						<li>direct: this variable is valid only if currently inside a light-loop block and set to 1 if the currently processed light is of type directional</li>
						<li>spot: this variable is valid only if currently inside a light-loop block and set to 1 if the currently processed light is of type spot</li>
						</ul>
						<br />Other variables may be used as required but must be specified by the client application or will always resolve to 0<br />
						#if blocks must be finalized with an #elif, #else or #endif directive</li>
					<li>#elif [condition]<br />
						similar to #if except only allowed as a followup of #if/#elif<br />
						#elif blocks must be finalized with an #elif, #else or #endif directive</li>
					<li>#else<br />
						specifies code to be included if preceeding conditions evaluate to false<br />
						#else blocks must be finalized with an #endif directive</li>
					<li>#endif<br />
						terminates an #if, #elif, or #else block and returns to the next upper level</li>
					<li>#light \<light\><br />
						Initiates a light loop which will iterate through all enabled lights. The code specified within is repeated once per enabled light.
						Light loops may be recursive. The specified name between \< and \> is initiated as a constant and may be used in the inner code segments.<br />
						Example:<br />
						#light \<i\><br />
							gl_FragColor += shade(\<i&\>,1.0,0.5,0.0);<br />
						#endlight<br />
						Light loops must be terminated using the #endlight directive</li>
					<li>#version [index]<br />
						Passed on to the underlying shader. Not interpreted by the template structure</li>
					<li>#extension [extension] : enable<br />
						Passed on to the underlying shader. Not interpreted by the template structure</li>
					</ul><br /><br />
					<b>Predefined functions</b><br />
					The following functions will be included into the build if called referenced the source. Their names should not be used for other purposes.
					The final parameter 'shadow' should be 0 if the light source is in shadow, and 1 if it is fully lit, or any value inbetween effectively toning diffuse and specular light.
					<ul>
						<li>vec4 omniLight(int index,vec3 position,vec3 normal,vec3 reflected, float shadow);<br />
							Calculates the light color/intensity emitted by the light source with the specified index, assuming it's an omni-directional lightsource</li>
						<li>vec4 directLight(int index,vec3 position,vec3 normal,vec3 reflected, float shadow);<br />
							Calculates the light color/intensity emitted by the light source with the specified index, assuming it's a directional lightsource</li>
						<li>vec4 spotLight(int index,vec3 position,vec3 normal,vec3 reflected, float shadow);<br />
							Calculates the light color/intensity emitted by the light source with the specified index, assuming it's a spot lightsource</li>
						<li>vec4 shade(vec3 position, vec3 normal, vec3 reflected);
							Calculates the summed color/intensity of all enabled lightsources</li>
						<li>vec4 customShade(vec3 position, vec3 normal, vec3 reflected, vec3 ambient, vec3 diffuse, vec3 specular);
							Calculates the summed color/intensity given a custom material </li></ul>
					
					<br />
					The specified parameters are:<ul>
						<li>index: Index of the light in question 0...n</li>
						<li>position: Vertex/fragment position in eye-space</li>
						<li>normal: Normalized vertex/fragment normal in eye-space</li>
						<li>reflected: Normalized reflected viewing direction in eye-space used to calculate specular lighting</li>
					</ul><br />
					Included aspects are the relations between normals and the respective light direction, spot exponent and attenuation if applicable
				*/
				/*
					#light <i>\n\
						#if omni\n\
							gl_FragColor += omniLight(<i>,frag_coord,normal,reflected,fragmentShadow<i>(frag_coord));\n\
						#elif direct\n\
							gl_FragColor += directLight(<i>,frag_coord,normal,reflected,fragmentShadow<i>(frag_coord));\n\
						#elif spot\n\
							gl_FragColor += spotLight(<i>,frag_coord,normal,reflected,fragmentShadow<i>(frag_coord));\n\
						#endif\n\
					#endlight
				*/
			class Template
			{
			public:
				class VariableMap;
				class UserConfiguration;
				class RenderConfiguration;


				enum variable_index_t
				{
					FogVariableIndex = 1,
					LightingVariableIndex = 2
				};


		
				class Expression:public IToString
				{
				public:
					unsigned					level;
		
					virtual						~Expression()	{}
					virtual	int					evaluate(const UserConfiguration&, Light::Type)=0;			//!< Evaluate the value of the local expression
					virtual	bool				adoptLeft(Expression*)		{return false;}	//!< Attempts to adopt the next expression to the left as a sub expression @return true if the expression has been adopted, false otherwise
					virtual	bool				adoptRight(Expression*)	{return false;}	//!< Attempts to adopt the next expression to the right as a sub expression @return true if the expression has been adopted, false otherwise
					virtual	bool				validate()	{return true;}					//!< Check the integrity of the local expression and all its children
				};
		
		
				/**
				@brief Expression that evaluates two sub expressions and combines their respective values

				Sub expressions are deleted automatically on object destruction unless NULL.
				Both a left and a right expression are adopted.
				*/
				class BinaryExpression:public Expression
				{
				public:
					Expression				*first,		//!< First sub expression
											*second;	//!< Second sub expression
				
					BinaryExpression()		:first(NULL),second(NULL)
											{}
					virtual					~BinaryExpression()
											{
												if (first)
													DISCARD(first);
												if (second)
													DISCARD(second);
											}
					virtual	bool			adoptLeft(Expression*exp)
											{
												first = exp;
												return true;
											}
					virtual	bool			adoptRight(Expression*exp)
											{
												second = exp;
												return true;
											}
					virtual	String			toString()	const
											{
												return "<"+(first?first->toString():String("NULL"))+", "+(second?second->toString():String("NULL"))+">";
											}
					virtual	bool			validate()	{return first!=NULL && second!=NULL && first->validate() && second->validate();}
				};

				#undef DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION
				#define DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION(_CLASS_NAME_,_OPERATOR_)\
					class _CLASS_NAME_:public BinaryExpression\
					{\
					public:\
						virtual	int			evaluate(const UserConfiguration&status, Light::Type type)\
											{\
												return first->evaluate(status,type) _OPERATOR_ second->evaluate(status,type);\
											}\
					};

				DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION(ProductExpression,*);
				DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION(DifferenceExpression,-);
				DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION(SumExpression,+);
				DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION(QuotientExpression,/);
				DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION(GreaterExpression,>);
				DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION(GreaterOrEqualExpression,>=);
				DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION(LessExpression,<);
				DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION(LessOrEqualExpression,<=);
				DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION(EqualExpression,==);
				DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION(NotEqualExpression,!=);
				DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION(AndExpression,&&);
				DEFINE_GL_TEMPLATE_OPERATOR_EXPRESSION(OrExpression,||);

		
				/**
				@brief Expression that negates (not) the returned value of the respective inner expression

				Adopts one expression the right
				*/
				class NegationExpression:public Expression
				{
				public:
					Expression				*inner;
				
											NegationExpression():inner(NULL)
											{}
					virtual					~NegationExpression()
											{
												if (inner)
													DISCARD(inner);
											}
					virtual	int				evaluate(const UserConfiguration&status, Light::Type type)
											{
												ASSERT_NOT_NULL__(inner);
												return !inner->evaluate(status,type);
											}
					virtual	bool			adoptRight(Expression*exp)
											{
												inner = exp;
												return true;
											}
					virtual	bool			validate()
											{
												return inner && inner->validate();
											}
					virtual	String			toString()	const
											{
												return "!<"+(inner?inner->toString():String("NULL"))+">";
											}
				};
		
				/**
				@brief Expression that simply returns a constant value
				*/
				class ConstantExpression:public Expression
				{
				public:
					int						value;
				
					virtual	int				evaluate(const UserConfiguration&, Light::Type)	{return value;}
					virtual	String			toString()	const
											{
												return "constant<"+String(value)+">";
											}
				};
		
				/**
				@brief Expression that returns a variable value depending on the current template status
				*/
				class VariableExpression:public Expression
				{
				public:
					index_t					index;	//!< Variable index (0=undefined)
					String					name;	//!< Variable name
				
				
					VariableExpression()	:index(0)	{}
					virtual	int				evaluate(const UserConfiguration&values, Light::Type);
					virtual	String			toString()	const
											{
												return "variable<"+name+"@"+String(index)+">";
											}
				};
		
				/**
				@brief Expression that returns true if the specified light type matches the stored one
				*/
				class LightTypeExpression:public Expression
				{
				public:
					Light::Type				type;	//!< Stored light type
								
					LightTypeExpression()	:type(Light::None){}
					virtual	int				evaluate(const UserConfiguration&, Light::Type light_type)
											{
												return light_type == type;
											}
					virtual	String			toString()	const
											{
												return "light_type<"+String(type)+">";
											}
								
				};
		
				class Block;
				class RootBlock;
		
				class LightShadowAttachment
				{
				public:
					struct SamplerAssignment
					{
						String				sampler_name;
						GLint				sampler_level;

						void				swap(SamplerAssignment&other)
											{
												swp(sampler_level,other.sampler_level);
												sampler_name.swap(other.sampler_name);
											}

					};
					String					shared_attachment,
											vertex_shader_attachment,
											fragment_shadow_code;
					Buffer<SamplerAssignment,0,Swap>
											sampler_assignments;

					void					swap(LightShadowAttachment&other)
											{
												shared_attachment.swap(other.shared_attachment);
												vertex_shader_attachment.swap(other.vertex_shader_attachment);
												fragment_shadow_code.swap(other.fragment_shadow_code);
												sampler_assignments.swap(other.sampler_assignments);
											}
					bool					isEmpty()	const	{return fragment_shadow_code.isEmpty();}

					void					predefineSampler(const char*sampler_name, GLint sampler_level)
											{
												SamplerAssignment&a = sampler_assignments.append();
												a.sampler_name = sampler_name;
												a.sampler_level = sampler_level;
											}
				};

				static	Buffer<LightShadowAttachment,0,Swap>		shadow_attachments;


				/**
				@brief Single code line of a shader block
				*/
				class Line
				{
				public:
					Array<String,Adopt>		segments;		//!< String line segments. Between each two line segments is one light constant. Typically this array only holds one element
				
					void					adoptData(Line&other)
											{
												segments.adoptData(other.segments);
											}
				};
		

		
				/**
				@brief A block of shader code lines
				*/
				class Block
				{
				public:
					enum Type		//! Block type
					{
						Any,				//!< This block is always appended
						Conditional,		//!< This block is appended based on a condition
						ElseConditional,	//!< This block is appended based on a previous and a local condition
						Else,				//!< This block is appended based on a previous condition
						LightLoop			//!< This block iterates through enabled lightsources
					};
			
					Type					type;					//!< Local block type
					Expression				*condition;				//!< Condition expression. May be NULL. The condition expression is automatically deleted on object destruction.
					String					light_loop_constant;	//!< Constant expression used to address the current light loop iteration (typically something like "<i>"). Unless overriden, this expression is inherited from the respective parent block during construction.
			
					Array<Line,Adopt>		inner_lines,	//!< Inner lines preceeding any conditional sub block. Lines following condition blocks are found in the @a trailing_lines member of sub blocks
											trailing_lines;	//!< Lines trailing the local block. They lie, in fact, outside this block on the next parent layer
					List::Vector<Block>		children;		//!< Child blocks
					Block					*parent;		//!< Parent block (if any). may be NULL
				
					Block()					:type(Any),condition(NULL),parent(NULL)	{}
					virtual					~Block()
											{
												if (condition)
													DISCARD(condition);
											}
					void					clear()
											{
												if (condition)
													DISCARD(condition);
												condition = NULL;
												inner_lines.free();
												trailing_lines.free();
												children.clear();
												type = Any;
												light_loop_constant = "";
											}
					void					assemble(StringBuffer&target, const UserConfiguration&values, const RenderConfiguration&render_config, Light::Type type, index_t light_index);
					bool					usesLighting()	const;		//!< Recursive check if lighting functions are used
					void					adoptData(Block&other)
											{
												type = other.type;
												condition = other.condition;
												light_loop_constant.adoptData(other.light_loop_constant);
												inner_lines.adoptData(other.inner_lines);
												trailing_lines.adoptData(other.trailing_lines);
												children.adoptData(other.children);
												parent = other.parent;

												other.type = Any;
												other.condition = NULL;
												other.parent = NULL;
											}
				};
		
				/**
				@brief Root template block
				*/
				class RootBlock:public Block
				{
				private:
					struct Token	//! Expression token identifier
					{
						enum Identifier //! Expression token identifier
						{
							String,	//!< Custom content string
							PUp,	//!< Parenthesis up '(' token
							PDown,	//!< Parenthesis down ')' token
							Plus,	//!< '+'
							Minus,	//!< '-'
							Times,	//!< '*'
							Divide,	//!< '/'
							And,	//!< '&&'
							Or,		//!< '||'
							Equals,	//!< '=='
							Differs,	//!< '!='
							Greater,	//!< '>'
							Less,		//!< '<'
							GreaterOrEqual,	//!< '>='
							LessOrEqual,	//!< '<='
							Not,			//!< '!'
							Space,			//!< Blank
						
							Mask=0x0FFF		//!< Mask to apply to a given token in order to identify its type (eg. 'token&(Token::Mask)')
						};
					};
				
					struct Level		//!< Token level
					{
						enum Identifier //!< Token level
						{
							None=0x0000,
							Zero=0x1000,
							One=0x2000,
							Two=0x3000,
							Three=0x4000,
							Four=0x5000,
							Five=0x6000,
						
							Mask=0xF000
						};
					};
				
					Expression*					parseCondition(const char*condition, VariableMap&map, String&error);
					Expression*					processLayer(TokenList&tokens,VariableMap&map, index_t begin, index_t end, String&error);
					static	void				shadowFunction(index_t level,StringBuffer&buffer);

				public:
					bool 						shade_invoked,				//!< Specifies that the shade() function is called in this block
												shade2_invoked,
												custom_shade_invoked,		//!< Specifies that the customShade() function is called in this block
												spotlight_invoked,			//!< Specifies that the spotLight() function is called in this block
												omnilight_invoked,			//!< Specifies that the omniLight() function is called in this block
												directlight_invoked;		//!< Specifies that the directLight() function is called in this block
				
				
					bool						scan(const String&source, VariableMap&map, StringBuffer&log_out, index_t&line_offset);
					String						assemble(const RenderConfiguration&,const UserConfiguration&, bool is_shared);
					void						assemble(const RenderConfiguration&,const UserConfiguration&,StringBuffer&target, bool is_shared=false);
					bool						usesLighting()	const;		//!< Queries whether or not lighting-relevant constants will be queried during assembly. If this value is false then RenderConfiguration will not be queried
				};
		
				class VariableMap
				{
				protected:
					HashTable<index_t>			variable_map;
					List::ReferenceVector<UserConfiguration>
												attached_configurations;
					bool						changed;

					friend class				RootBlock;
					friend class				Instance;
					friend class				UserConfiguration;

					void						reg(UserConfiguration*config)	{attached_configurations.append(config);}
					void						unreg(UserConfiguration*config){attached_configurations.drop(config);}

					index_t						define(const String&variable_name);
				public:
					VariableMap();
					virtual						~VariableMap();
					index_t						locate(const String&var_name)	const;
					void						clear();		//!< Clears all local variables and redefines standard variables
					void						submitChanges();	//!< Notifies attached user configurations of changes to the map. The method has no effect if no changes have been made
				};
		
				class Configuration;

				class ConfigurationComponent
				{
				private:
					List::ReferenceVector<Configuration>
												linked_configs;
				protected:
					friend class				Configuration;

					index_t						version;

					void						unreg(Configuration*config);	//!< Unlinks a registered configuration
					void						reg(Configuration*config);		//!< Links a configuration
					void						signalHasChanged();				//!< Notifies linked components that local data has changed
				public:
					ConfigurationComponent()	:version(0)	{}
					virtual						~ConfigurationComponent();
				};

				class RenderConfiguration:public ConfigurationComponent	//! Configuration container for pipeline related settings that are relevant for shader assembly
				{
				protected:
					Buffer<Light::Type,8>		lights;		//!< Known light types
					bool						lighting_enabled,
												fog_enabled;
					friend class				UserConfiguration;
					friend class				RootBlock;
					friend class				Block;
					friend class				Instance;
					friend class				Configuration;

				public:
					RenderConfiguration();
					void						redetect();							//!< Updates lights and fog variable
					void						setLights(count_t count,...);		//!< Manually sets light types. The first parameter must specify how many light sources are to be considered enabled. Subsequent parameters must provide at least @a count values of type Light::Type
					void						setLighting(bool lighting);			//!< Globally enables or disables lighting
					void						clear();							//!< Clears all light sources and resets configuration to default
				};
				typedef RenderConfiguration		RenderConfig;

				static RenderConfiguration		global_render_config;		//!< Global configuration that keeps track of shader-relevant pipeline settings

				/**
				@brief Variable values associated with an instance of VariableMap

				These values are required for proper shader assembly
				*/
				class UserConfiguration:public ConfigurationComponent
				{
				protected:
					VariableMap					*map;		//!< Pointer to a variable name field. May be NULL
					Array<int,Primitive>		values;		//!< Variable values
				
					friend class				RootBlock;
					friend class				Block;
					friend class				Template;
					friend class				Configuration;
					friend class				VariableMap;
				
					void						adapt();
					void						signalMapDestruction();
				public:
					/**/						UserConfiguration();
					virtual						~UserConfiguration();
					bool						set(index_t variable, int value);					//!< Sets the value of a specific variable by index. @param variable Index of the variable to update @param value Value to set @return true on success
					bool						setByName(const String&variable_name, int value);	//!< Sets the value of a specific variable by name. The local configuration must be linked with a variable map for this method to succeed. @param variable_name Name of the variable to update @param value Value to set @return true on success
					int							value(index_t variable)	const;						//!< Queries the current value of a variable by index @param variable Variable index starting at 1 @return Current variable value or 0 if the specified index is invalid
					int							valueByName(const String&variable_name) const;		//!< Queries the current value of a variable by namex @param variable_name Name of the variable to query @return Current variable value or 0 if the specified name is invalid
					void						reset();											//!< Resets all local variables to 0.
					void						clear();											//!< Clears the local structure (also unlinks it from any VariableMap instance)
					void						link(VariableMap*map, bool adapt=true);			//!< Updates map link and registration. Potentially updates variable field length. Checks and aborts if @a this is NULL
					VariableMap*				variableMap();										//!< Retrieves the map currently associated with this configuration, if any @return variable map or NULL if no such exists
					const VariableMap*			variableMap() const;								//!< Retrieves the map currently associated with this configuration, if any @return variable map or NULL if no such exists

					void						updateRenderVariables(const RenderConfiguration&);	//!< Updates the local rendering-related variables based on the specified render configuration
					void						updateRenderVariables();							//!< Identical to the above but queries @b global_render_config instead
				};

				typedef UserConfiguration		UserConfig;

				class Configuration:public Array<int,Primitive>
				{
				protected:
					UserConfiguration			*user_config;
					RenderConfiguration			*render_config;

					index_t						user_config_version,
												render_config_version;

					bool						valid,				//!< All configuration components are linked
												registered,			//!< The configurations know of this instance. Initially set to false if components are passed as constructor parameters and @a register_now is false
												structure_changed;	//!< Indicates that at least one configuration pointer has changed since the last update() call

					friend class				Template;
				public:
					Configuration()				:user_config(NULL),render_config(NULL),valid(false),registered(false),structure_changed(false)	{}
					/**/						Configuration(RenderConfiguration&new_render_config,UserConfiguration&new_user_config,bool register_now=true):user_config(&new_user_config),render_config(&new_render_config),valid(true),registered(register_now),structure_changed(false)
												{
													if (register_now)
													{
														user_config->reg(this);
														render_config->reg(this);
													}
												}
												
					virtual						~Configuration();
					bool						update();										//!< Updates the local key if needed
					bool						requiresUpdate()	const;						//!< Checks if the local structure or any of the linked components have changed

					void						unlink();		//!< Unlinks linked configurations
					void						link(RenderConfiguration&render_config, UserConfiguration&user_config);
					void						link(RenderConfiguration&render_config);
					void						link(UserConfiguration&user_config);

					void						updateRenderVariables()
												{
													if (valid)
														user_config->updateRenderVariables(*render_config);	//should trigger the whole changed cascade automatically
												}
					void						signalComponentDestroyed(ConfigurationComponent*component);		//!< Notifies this structure that a linked component was destroyed
					void						signalComponentHasChanged();		//!< Notifies this structure that a linked component has changed
					inline	UserConfiguration*	userConfig()			{return user_config;}
					inline	const UserConfig*	userConfig() const		{return user_config;}
					inline	RenderConfiguration*renderConfig()			{return render_config;}
					inline	const RenderConfig*	renderConfig() const	{return render_config;}
				};

				typedef Configuration			Config;
		
			protected:
				RootBlock					shared_template,
											vertex_template,
											geometry_template,
											fragment_template;
				GLenum 						geometry_type,
											output_type;
				unsigned					max_vertices;
				bool						loaded_,
											uses_lighting;			//!< True if any shading-related functionality is used by the shader
				VariableMap					local_map,				//!< Locally stored variable name map. This would be the standard variable map
											*current_map;			//!< Actually used variable name map. Usually maps to @a local_map. Using the same variable map on different template instances, though, can be useful when working with just one huge user config
				UserConfiguration			local_user_config;		//!< Locally stored user variable config. Contains all user variable values that may be referenced during shader assembly
				Configuration				local_config,
											*current_config;
				StringBuffer				log;
				GenericHashContainer<Array<int,Primitive>,GLShader::Instance>	container;	//!< All assembled shaders mapped to their respective keys
				

				struct TUniformInit	//! Uniform pInherit structure
				{
					enum Type		//! PInherit type
					{
						Int,		//!< Integer type
						Float,		//!< Float type
						Float3		//!< 3d float type
					};
					
					String					name;	//!< Name of the variable to predefine
					union
					{
						int					ival;	//!< Integer value
						float				fval;	//!< Float value
						TVec3<>				f3val;
					};
					Type					type;	//!< Contained type
				};
				
				Buffer<TUniformInit,4>		uniform_init;	//!< Predefined variables
				
			public:
		
				Template()					:geometry_type(GL_TRIANGLES),output_type(GL_TRIANGLE_STRIP),max_vertices(12),loaded_(false),uses_lighting(false),current_map(&local_map),local_config(global_render_config,local_user_config,false),current_config(&local_config)	{}
				void						clear();
				bool						load(const Composition&composition, GLenum geometry_type=GL_TRIANGLES, GLenum output_type=GL_TRIANGLE_STRIP, unsigned max_vertices=12);
				bool						loadComposition(const String&object_source, GLenum geometry_type=GL_TRIANGLES, GLenum output_type=GL_TRIANGLE_STRIP, unsigned max_vertices=12);
				void						loadRequired(const Composition&composition, GLenum geometry_type=GL_TRIANGLES, GLenum output_type=GL_TRIANGLE_STRIP, unsigned max_vertices=12);
				void						loadRequiredComposition(const String&object_source, GLenum geometry_type=GL_TRIANGLES, GLenum output_type=GL_TRIANGLE_STRIP, unsigned max_vertices=12);
				String						report();
				bool						loaded()	const;
				inline	bool				isLoaded()	const	{return this && loaded();}

				inline	VariableMap*		map()				{return current_map;}
				inline	const VariableMap*	map()		const	{return current_map;}
				inline	UserConfiguration*	userConfig()			{return current_config->userConfig();}
				inline	const UserConfig*	userConfig()	const	{return current_config->userConfig();}
				inline	RenderConfiguration*renderConfig()			{return current_config->renderConfig();}
				inline	const RenderConfig*	renderConfig()	const	{return current_config->renderConfig();}
						
				void						setMap(VariableMap*map, bool adjust_user_config=true);	//!< Changes the variable map to use. Should not be changed before loading data into the local object @param 
				void						setConfig(UserConfiguration*config, bool adjust=true);	//!< Changes the user configuration storage object to the specified one. @param config New configuration storage. May be NULL reverting the storage object to default @param adjust Update the length of the storage's variable field to the local map
				void						setConfig(RenderConfiguration*config);
				void						setConfig(Configuration*config);

				void						resetConfig()		{setConfig((Configuration*)NULL);}
				void						resetUserConfig()	{setConfig(NULL,true);}
				void						resetRenderConfig()	{setConfig((RenderConfiguration*)NULL);}
				void						resetMap()			{setMap(NULL);}
				
				inline	index_t				locate(const String&variable_name) const		{return current_map->locate(variable_name);}	//! Locates a template variable. See VariableMap::locate() for details
				inline	bool				setVariable(index_t variable, int value)				{return current_config->valid && current_config->userConfig()->set(variable,value);}	//!< Updates the value of a variable. See UserConfiguration::set() for details
				/**
				@brief Constructs a shader instance based on the current render and user configurations
							
				NULL-pointer sensitive

				@param auto_update_render_variables Auto-update renderpipeline-derived user variables (e.g. 'lighting'). Does not automatically redetect such
				@param is_new [out] Filled with true if a previously undefined shader instance was constructed. Filled with false if an existing matching shader instance was found. May be NULL
				@return Pointer to a new or existing shader instance, or NULL if compilation failed. Use report() to determine a user readable cause if compilation failed for some reason
				*/
				GLShader::Instance*			buildShader(bool auto_update_render_variables=true, bool*is_new=NULL);

				/**
				@brief Assembles a full shader as it could be loaded by an external shader instance
				@param auto_update_render_variables Auto-update renderpipeline-derived user variables (e.g. 'lighting'). Does not automatically redetect such
				@return Shader composition featuring all four sections 'vertex', 'fragment', 'geometry', and 'shared'
				*/
				String						assembleSource(bool auto_update_render_variables=true);
				
				
				Template&					predefineUniformi(const String&name, int value);	//!< Presets the specified uniform variable to the specified value on construction
				Template&					predefineUniformf(const String&name, float value);	//!< Presets the specified uniform variable to the specified value on construction
				Template&					predefineUniform3f(const String&name, const float value[3]);	//!< Presets the specified uniform variable to the specified value on construction
				static String*				locateShaderIncludable(const String&filename);	//!< Attempts to locate or load a block template described by the specified filename. @return Existing or loaded block or NULL if loading failed
				static void					defineShaderIncludable(const String&filename, const String&block_code);	//!< Defines a new or overwrites an existing includable block template
			};

	


			/**
				@brief Compiled GLSL instance

				Contrary to templtes, instances have no switches and hold only ever one shader to install.
			*/
			class	Instance
			{
			private:
					GLhandleARB				program_handle,
											vertex_shader,
											fragment_shader,
											geometry_shader;
					StringBuffer			log;
			static	const Instance			*installed_instance;
			static	bool					warn_on_failure;
					//TCodeLocation		installed_in;
				
					friend class Template;

					bool					extractFileContent(const String&filename, String&target);
					GLhandleARB				loadShader(const String&source, GLenum program_type);
				
											Instance(const Instance&other)
											{}
					void					operator=(const Instance&other)
											{}
					void					resetVariables();
			public:
					Composition				composition;		//!< Shader source code

			static	bool					forget_on_destruct;	//!< If set true the underlying GL objects are not deleted if the destructor of shader objects is invoked
		
											Instance();
			virtual							~Instance();
		
					void					adoptData(Instance&other);

					void					clear();
					bool					load(const Composition&composition, GLenum geometry_type=GL_TRIANGLES, GLenum output_type=GL_TRIANGLE_STRIP, unsigned max_vertices=12);
					bool					loadComposition(const String&object_source, GLenum geometry_type=GL_TRIANGLES, GLenum output_type=GL_TRIANGLE_STRIP, unsigned max_vertices=12);
					void					loadRequired(const Composition&composition, GLenum geometry_type=GL_TRIANGLES, GLenum output_type=GL_TRIANGLE_STRIP, unsigned max_vertices=12);
					void					loadRequiredComposition(const String&object_source, GLenum geometry_type=GL_TRIANGLES, GLenum output_type=GL_TRIANGLE_STRIP, unsigned max_vertices=12);
					String					report();

			static	void					suppressLocationFailureWarning();
			static	void					suppressWarnings();
			static	void					enableFailureWarning();
			static	void					disableFailureWarning();
					Variable				locate(const String&name, bool warn_on_fail=true);
					bool					getContent(GLint name, float*out_field);
					bool					loaded()	const;								//!< Checks whether ot not the local shader is loaded and ready for installation. NULL-pointer sensitive
					bool					isLoaded()	const	{return loaded();}			//!< @overload
					bool					isEmpty()	const	{return !loaded();}			//!< Not isLoaded()


					bool					installed()		const	{return installed_instance == this;}
					bool					isInstalled()	const	{return installed_instance == this;}

			static	bool					anyIsInstalled()	{return installed_instance != NULL;}

					bool					validate();

					bool					install();				//!< Installs this shader. Installation fails if the local shader instance is NULL or not loaded
					bool					install()		const;	//!< const shader installation. Does not write to log
					bool					permissiveInstall();		//!< Installs the local shader, replacing any currently installed shader. Any installed shader will be uninstalled if the local shader instance is NULL or not loaded
					bool					permissiveInstall()	const;	//!< const variant. Does not write to log
					void					deInstall()		const;
					void					uninstall()		const;
			static	void					permissiveUninstall();
			};
		

		
			//typedef Template::Instance	Template;
		
			class Installation	//! Automatic localized shader installation. The class prevents double installation or deinstallation and automatically installs/uninstalls on construction/destruction.
			{
			protected:
					Instance			*object_reference;
					Template			*template_reference;
					bool				installed;

									//SyncLock(const SyncLock&other)	{}
					Installation&		operator=(const Installation&)	{return *this;}
				
									Installation()
									{}
			public:
									Installation(Instance&object):object_reference(&object),template_reference(NULL),installed(false) {install();}
									Installation(Template&templ):object_reference(NULL),template_reference(&templ),installed(false) {install();}

									~Installation()
									{
										uninstall();
									}
					void			install()	//!< Installs the shader object if it hasn't been installed
									{
										if (!installed)
										{
											if (template_reference)
											{
												//template_reference->status()->redetect();
												object_reference = template_reference->buildShader();
												if (!object_reference)
													FATAL__(template_reference->report());
											}
											object_reference->install();
											installed = true;
										}
									}
					void			uninstall()	//!< Uninstalls the shader object if it is installed
									{
										if (installed)
										{
											object_reference->uninstall();
											installed = false;
										}
									}
					operator bool()	{install(); return true;}	//!< Installs if not already done
		
			};
		
		

			#define glsl(shader)		if (Engine::GLShader::Installation __shader_installation__ = shader)
		
		}
		
		#endif

		class Extension
		{
		private:
				String		group;
			#if SYSTEM==WINDOWS
				HDC			hDC;
			#elif SYSTEM==UNIX
				Display	*display;
				int			screen;
			#endif
		static GLuint		Extension::allocateDepthBuffer(const Resolution&res);

		public:
				CONSTRUCT_ENUMERATION3(VRAMQueryMethod,None, NVIDIA,ATI);
				typedef GenericHashTable<Resolution,GLuint,Resolution::Hash>	ResolutionTable;


		static	ResolutionTable	depth_buffer_table;

				VRAMQueryMethod	query_method;

				GLint			max_texture_layers,
								max_texcoord_layers,
								max_lights,
								max_cube_texture_size,
								max_register_combiners;
				GLfloat			max_texture_max_anisotropy;
				ExtCont		gl_ext,sysgl_ext;
				StringBuffer	compile_log;
				size_t			initial_total;

				bool			available(const String&extension);
		template <class C>
		static	void			read(C&target, const String&name);
		template <class C>
		static	void			readSilent(C&target, const String&name);

				bool			init(GLuint);
				GLuint 			loadVertexProgram(const char*source);
				GLuint 			loadFragmentProgram(const char*source);
				GLuint 			loadGeometryProgram(const char*source);
				GLuint 			loadProgram(const char*source, GLenum type);
			#if defined(WGL_ARB_pbuffer) || defined(GLX_SGIX_pbuffer)
				TPBuffer		createPBuffer(unsigned width, unsigned height, ePBufferType type, BYTE fsaa);
				void			destroyPBuffer(const TPBuffer&buffer);
			#endif
				String			renderState();
				
				/**
					@brief Creates a new frame buffer object
					
					@param width Width of the frame buffer object in pixels
					@param height Height of the frame buffer object in pixels
					@param depth_storage Type of depth storage to use for depth components
					@param num_color_targets Number of color targets
					@param format Pointer to a field of internal formats to use for the texture targets
				*/
		static	TFrameBuffer	createFrameBuffer(const Resolution&res, DepthStorage depth_storage, BYTE num_color_targets, const GLenum*format);

				/**
					@brief Frees all associated data and resets all internal variables
					
					The buffer must not currently be bound.
					
					@param buffer Buffer to free
				*/
		static	void			destroyFrameBuffer(const TFrameBuffer&buffer);
		
				/**
					@brief Binds the specified framebuffer as rendering target
					
					@param buffer Buffer to bind
					@return true on success, false otherwise
				*/
		static	bool			bindFrameBuffer(const TFrameBuffer&buffer);
				/**
					@brief Unbinds any currently bound frame buffer object.
					
					The method has no effect if no FBO is currently bound
				*/
		static	void			unbindFrameBuffer();

				/**
					@brief Tests, whether or not the specified format provides an alpha channel
				*/
		static	bool			formatHasAlpha(GLenum format);
				/**
					@brief Attempts to resize the specified buffer to match the new width and height

				*/
		static	bool			resizeFrameBuffer(TFrameBuffer&buffer, const Resolution&res);
				/**
					@brief Copies the content (color and/or depth values) from one frame buffer object to another
					
					Typically the first FBO (from) is a multisampled one, while the second (to) is non-multisampled.
					
					@return true on success
				*/
		static	bool			copyFrameBuffer(const TFrameBuffer&from, const TFrameBuffer&to, bool copy_color = true, bool copy_depth=true);
		static	bool			copyFrameBuffer(const TFrameBuffer&from, const TFrameBuffer&to, const Resolution&res, bool copy_color = true, bool copy_depth=true);
			#if SYSTEM==WINDOWS
				void			initialize(HDC hDC);
			#elif SYSTEM==UNIX
				void			initialize(Display*display,int screen);
			#else
				void			initialize();
			#endif

				bool			deviceMemoryState(size_t&free,size_t&total)	const;
		};

		
		
		/**
			@brief Offscreen renderer to produce one time textures
		*/
		class GLTextureFactory
		{
		protected:
				GLint				previous_frame_buffer,
									viewport[4];
				GLuint				frame_buffer,
									result_texture,
									//color_buffer,
									depth_buffer;
				GLfloat				previous_clear_color[4];
				unsigned			width,height;
				bool				alpha,mipmapping;
		
		
									GLTextureFactory(const GLTextureFactory&)	{}
				GLTextureFactory&	operator=(const GLTextureFactory&)	{return *this;}
		public:
									GLTextureFactory();
									GLTextureFactory(unsigned width, unsigned height, bool alpha, bool mipmapping);	//!< Directly creates the local factory
		virtual						~GLTextureFactory();
		
									/**
										@brief (re)creates the local texture factory to the specified dimensions
										
										The texture factory will initiate buffers and create textures of the specified dimensions
										
										@return true on success
									*/
				bool				create(unsigned width, unsigned height, bool alpha, bool mipmapping);
				bool				checkFormat(unsigned width, unsigned height, bool alpha, bool mipmapping);	//!< Checks if the specified dimensions match the local ones and updates if that should not be the case			bool				checkFormat(unsigned width, unsigned height, bool alpha);	//!< Checks if the specified dimensions match the local ones and updates if that should not be the case
				
				void				clear();	//!< Destroys the local texture factory
		
									/**
										@brief Begins rendering to the off screen source
									
										Any previously bound frame buffer is backuped
										
										@return true on success
									*/
				bool				begin();
				void				test();
									/**
										@brief Ends offscreen rendering and copies the rendered content into a texture
										
										Any previously bound frame buffer is restored
										
										@return Reference to a new OpenGL texture. The returned texture is not further managed. The client application is responsible for its destruction if no longer needed
									*/
				GLuint				end();
				
				void				abort();	//!< Aborts rendering and deletes the created texture
		
				bool				isActive()	const;	//!< Checks if the texture factory is currently bound
				bool				isCreated()	const;	//!< True if this factory has been created
		};
		
		extern Extension gl_extensions;
		
		
		
		void		glTangent3f(GLfloat x, GLfloat y, GLfloat z);
		void		glTangent3fv(const GLfloat*v);
		void		glTangentTextureLayer(GLuint layer=GL_TEXTURE1);
		


	}

#endif

namespace OpenGLExtPointer
{

#ifdef GL_ARB_multitexture

	extern PFNGLMULTITEXCOORD1FARBPROC				glMultiTexCoord1f;
	extern PFNGLMULTITEXCOORD1FVARBPROC				glMultiTexCoord1fv;
	extern PFNGLMULTITEXCOORD2FARBPROC				glMultiTexCoord2f;
	extern PFNGLMULTITEXCOORD2FVARBPROC				glMultiTexCoord2fv;
	extern PFNGLMULTITEXCOORD3FARBPROC				glMultiTexCoord3f;
	extern PFNGLMULTITEXCOORD3FVARBPROC				glMultiTexCoord3fv;
	extern PFNGLMULTITEXCOORD4FARBPROC				glMultiTexCoord4f;
	extern PFNGLMULTITEXCOORD4FVARBPROC				glMultiTexCoord4fv;
	extern PFNGLMULTITEXCOORD1DARBPROC				glMultiTexCoord1d;
	extern PFNGLMULTITEXCOORD1DVARBPROC				glMultiTexCoord1dv;
	extern PFNGLMULTITEXCOORD2DARBPROC				glMultiTexCoord2d;
	extern PFNGLMULTITEXCOORD2DVARBPROC				glMultiTexCoord2dv;
	extern PFNGLMULTITEXCOORD3DARBPROC				glMultiTexCoord3d;
	extern PFNGLMULTITEXCOORD3DVARBPROC				glMultiTexCoord3dv;
	extern PFNGLMULTITEXCOORD4DARBPROC				glMultiTexCoord4d;
	extern PFNGLMULTITEXCOORD4DVARBPROC				glMultiTexCoord4dv;
	extern PFNGLMULTITEXCOORD1IARBPROC				glMultiTexCoord1i;
	extern PFNGLMULTITEXCOORD1IVARBPROC				glMultiTexCoord1iv;
	extern PFNGLMULTITEXCOORD2IARBPROC				glMultiTexCoord2i;
	extern PFNGLMULTITEXCOORD2IVARBPROC				glMultiTexCoord2iv;
	extern PFNGLMULTITEXCOORD3IARBPROC				glMultiTexCoord3i;
	extern PFNGLMULTITEXCOORD3IVARBPROC				glMultiTexCoord3iv;
	extern PFNGLMULTITEXCOORD4IARBPROC				glMultiTexCoord4i;
	extern PFNGLMULTITEXCOORD4IVARBPROC				glMultiTexCoord4iv;
	extern PFNGLACTIVETEXTUREARBPROC				glActiveTexture;
	extern PFNGLCLIENTACTIVETEXTUREARBPROC			glClientActiveTexture;
#endif

#ifdef GL_NV_register_combiners
	extern PFNGLCOMBINERPARAMETERFVNVPROC			glCombinerParameterfv;
	extern PFNGLCOMBINERPARAMETERFNVPROC			glCombinerParameterf;
	extern PFNGLCOMBINERPARAMETERIVNVPROC			glCombinerParameteriv;
	extern PFNGLCOMBINERPARAMETERINVPROC			glCombinerParameteri;
	extern PFNGLCOMBINERINPUTNVPROC					glCombinerInput;
	extern PFNGLCOMBINEROUTPUTNVPROC				glCombinerOutput;
	extern PFNGLFINALCOMBINERINPUTNVPROC			glFinalCombinerInput;
	extern PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC	glGetCombinerInputParameterfv;
	extern PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC	glGetCombinerInputParameteriv;
	extern PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC	glGetCombinerOutputParameterfv;
	extern PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC	glGetCombinerOutputParameteriv;
	extern PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC	glGetFinalCombinerInputParameterfv;
	extern PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC	glGetFinalCombinerInputParameteriv;
#endif
#ifdef GL_ARB_vertex_program
	extern PFNGLVERTEXATTRIB2FARBPROC				glVertexAttrib2f;
	extern PFNGLVERTEXATTRIB2DARBPROC				glVertexAttrib2d;
	extern PFNGLVERTEXATTRIB3FARBPROC				glVertexAttrib3f;
	extern PFNGLVERTEXATTRIB3DARBPROC				glVertexAttrib3d;
	extern PFNGLVERTEXATTRIB4FARBPROC				glVertexAttrib4f;
	extern PFNGLVERTEXATTRIB4DARBPROC				glVertexAttrib4d;
	extern PFNGLVERTEXATTRIB2FVARBPROC				glVertexAttrib2fv;
	extern PFNGLVERTEXATTRIB2DVARBPROC				glVertexAttrib2dv;
	extern PFNGLVERTEXATTRIB3FVARBPROC				glVertexAttrib3fv;
	extern PFNGLVERTEXATTRIB3DVARBPROC				glVertexAttrib3dv;
	extern PFNGLVERTEXATTRIB4IVARBPROC				glVertexAttrib4iv;
	extern PFNGLVERTEXATTRIB4UIVARBPROC				glVertexAttrib4uiv;
	extern PFNGLVERTEXATTRIB4FVARBPROC				glVertexAttrib4fv;
	extern PFNGLVERTEXATTRIB4DVARBPROC				glVertexAttrib4dv;
	extern PFNGLVERTEXATTRIB4NIVARBPROC				glVertexAttrib4Niv;
	extern PFNGLVERTEXATTRIB4NUIVARBPROC			glVertexAttrib4Nuiv;
	extern PFNGLVERTEXATTRIBPOINTERARBPROC			glVertexAttribPointer;
	extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC		glEnableVertexAttribArray;
	extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC		glDisableVertexAttribArray;
	extern PFNGLPROGRAMSTRINGARBPROC				glProgramString;
	extern PFNGLBINDPROGRAMARBPROC					glBindProgram;
	extern PFNGLDELETEPROGRAMSARBPROC				glDeletePrograms;
	extern PFNGLGENPROGRAMSARBPROC					glGenPrograms;
	extern PFNGLPROGRAMENVPARAMETER4DARBPROC		glProgramEnvParameter4d;
	extern PFNGLPROGRAMENVPARAMETER4DVARBPROC		glProgramEnvParameter4dv;
	extern PFNGLPROGRAMENVPARAMETER4FARBPROC		glProgramEnvParameter4f;
	extern PFNGLPROGRAMENVPARAMETER4FVARBPROC		glProgramEnvParameter4fv;
	extern PFNGLPROGRAMLOCALPARAMETER4DARBPROC		glProgramLocalParameter4d;
	extern PFNGLPROGRAMLOCALPARAMETER4DVARBPROC		glProgramLocalParameter4dv;
	extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC		glProgramLocalParameter4f;
	extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC		glProgramLocalParameter4fv;
	extern PFNGLGETPROGRAMENVPARAMETERDVARBPROC		glGetProgramEnvParameterdv;
	extern PFNGLGETPROGRAMENVPARAMETERFVARBPROC		glGetProgramEnvParameterfv;
	extern PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC	glGetProgramLocalParameterdv;
	extern PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC	glGetProgramLocalParameterfv;
	extern PFNGLGETPROGRAMIVARBPROC					glGetProgramiv;
	extern PFNGLGETPROGRAMSTRINGARBPROC				glGetProgramString;
	extern PFNGLGETVERTEXATTRIBDVARBPROC			glGetVertexAttribdv;
	extern PFNGLGETVERTEXATTRIBFVARBPROC			glGetVertexAttribfv;
	extern PFNGLGETVERTEXATTRIBIVARBPROC			glGetVertexAttribiv;
	extern PFNGLGETVERTEXATTRIBPOINTERVARBPROC		glGetVertexAttribPointerv;
	extern PFNGLISPROGRAMARBPROC					glIsProgram;
#endif

#ifdef GL_ARB_vertex_buffer_object
	extern PFNGLBINDBUFFERARBPROC					glBindBuffer;
	extern PFNGLDELETEBUFFERSARBPROC				glDeleteBuffers;
	extern PFNGLGENBUFFERSARBPROC					glGenBuffers;
	extern PFNGLISBUFFERARBPROC						glIsBuffer;
	extern PFNGLBUFFERDATAARBPROC					glBufferData;
	extern PFNGLBUFFERSUBDATAARBPROC				glBufferSubData;
	extern PFNGLGETBUFFERSUBDATAARBPROC				glGetBufferSubData;
	extern PFNGLMAPBUFFERARBPROC					glMapBuffer;
	extern PFNGLUNMAPBUFFERARBPROC					glUnmapBuffer;
	extern PFNGLGETBUFFERPARAMETERIVARBPROC			glGetBufferParameteriv;
	extern PFNGLGETBUFFERPOINTERVARBPROC			glGetBufferPointerv;
#else
	#error extension not available
#endif

#ifdef GL_EXT_compiled_vertex_array
	extern PFNGLLOCKARRAYSEXTPROC					glLockArrays;
	extern PFNGLUNLOCKARRAYSEXTPROC					glUnlockArrays;
#endif

#ifdef GL_ARB_shader_objects
	extern PFNGLDELETEOBJECTARBPROC					glDeleteObject;
	extern PFNGLGETHANDLEARBPROC					glGetHandle;
	extern PFNGLDETACHOBJECTARBPROC					glDetachObject;
	extern PFNGLCREATESHADEROBJECTARBPROC			glCreateShaderObject;
	extern PFNGLSHADERSOURCEARBPROC					glShaderSource;
	extern PFNGLCOMPILESHADERARBPROC				glCompileShader;
	extern PFNGLCREATEPROGRAMOBJECTARBPROC			glCreateProgramObject;
	extern PFNGLATTACHOBJECTARBPROC					glAttachObject;
	extern PFNGLLINKPROGRAMARBPROC					glLinkProgram;
	extern PFNGLUSEPROGRAMOBJECTARBPROC				glUseProgramObject;
	extern PFNGLVALIDATEPROGRAMARBPROC				glValidateProgram;
	extern PFNGLUNIFORM1FARBPROC					glUniform1f;
	extern PFNGLUNIFORM2FARBPROC					glUniform2f;
	extern PFNGLUNIFORM3FARBPROC					glUniform3f;
	extern PFNGLUNIFORM4FARBPROC					glUniform4f;
	extern PFNGLUNIFORM1IARBPROC					glUniform1i;
	extern PFNGLUNIFORM2IARBPROC					glUniform2i;
	extern PFNGLUNIFORM3IARBPROC					glUniform3i;
	extern PFNGLUNIFORM4IARBPROC					glUniform4i;
	extern PFNGLUNIFORM1FVARBPROC					glUniform1fv;
	extern PFNGLUNIFORM2FVARBPROC					glUniform2fv;
	extern PFNGLUNIFORM3FVARBPROC					glUniform3fv;
	extern PFNGLUNIFORM4FVARBPROC					glUniform4fv;
	extern PFNGLUNIFORM1IVARBPROC					glUniform1iv;
	extern PFNGLUNIFORM2IVARBPROC					glUniform2iv;
	extern PFNGLUNIFORM3IVARBPROC					glUniform3iv;
	extern PFNGLUNIFORM4IVARBPROC					glUniform4iv;
	extern PFNGLUNIFORMMATRIX2FVARBPROC				glUniformMatrix2fv;
	extern PFNGLUNIFORMMATRIX3FVARBPROC				glUniformMatrix3fv;
	extern PFNGLUNIFORMMATRIX4FVARBPROC				glUniformMatrix4fv;
	extern PFNGLGETOBJECTPARAMETERFVARBPROC			glGetObjectParameterfv;
	extern PFNGLGETOBJECTPARAMETERIVARBPROC			glGetObjectParameteriv;
	extern PFNGLGETINFOLOGARBPROC					glGetInfoLog;
	extern PFNGLGETATTACHEDOBJECTSARBPROC			glGetAttachedObjects;
	extern PFNGLGETUNIFORMLOCATIONARBPROC			glGetUniformLocation;
	extern PFNGLGETACTIVEUNIFORMARBPROC				glGetActiveUniform;
	extern PFNGLGETUNIFORMFVARBPROC					glGetUniformfv;
	extern PFNGLGETUNIFORMIVARBPROC					glGetUniformiv;
	extern PFNGLGETSHADERSOURCEARBPROC				glGetShaderSource;
#endif

#ifdef WGL_EXT_swap_control
	extern PFNWGLSWAPINTERVALEXTPROC				wglSwapInterval;
	extern PFNWGLGETSWAPINTERVALEXTPROC				wglGetSwapInterval;
#endif

#ifdef WGL_ARB_pixel_format
	extern PFNWGLCHOOSEPIXELFORMATARBPROC			wglChoosePixelFormat;
	extern PFNWGLGETPIXELFORMATATTRIBIVARBPROC		wglGetPixelFormatAttribiv;
	extern PFNWGLGETPIXELFORMATATTRIBFVARBPROC		wglGetPixelFormatAttribfv;
#endif

#ifdef WGL_ARB_extensions_string
	extern PFNWGLGETEXTENSIONSSTRINGARBPROC			wglGetExtensionsString;
#endif

#ifdef WGL_ARB_pbuffer
	extern PFNWGLCREATEPBUFFERARBPROC				wglCreatePbuffer;
	extern PFNWGLGETPBUFFERDCARBPROC				wglGetPbufferDC;
	extern PFNWGLRELEASEPBUFFERDCARBPROC			wglReleasePbufferDC;
	extern PFNWGLDESTROYPBUFFERARBPROC				wglDestroyPbuffer;
	extern PFNWGLQUERYPBUFFERARBPROC				wglQueryPbuffer;
#endif

#if defined(GLX_SGIX_fbconfig) && !defined(GLX_VERSION_1_3)
	extern PFNGLXGETFBCONFIGATTRIBSGIXPROC			glXGetFBConfigAttrib;
	extern PFNGLXCHOOSEFBCONFIGSGIXPROC				glXChooseFBConfig;
	extern PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC	glXCreateGLXPixmapWithConfig;
	extern PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC	glXCreateContextWithConfig;
	extern PFNGLXGETVISUALFROMFBCONFIGSGIXPROC		glXGetVisualFromFBConfig;
	extern PFNGLXGETFBCONFIGFROMVISUALSGIXPROC		glXGetFBConfigFromVisual;
#endif


#ifdef WGL_ARB_render_texture
	extern PFNWGLBINDTEXIMAGEARBPROC				wglBindTexImage;
	extern PFNWGLRELEASETEXIMAGEARBPROC				wglReleaseTexImage;
	extern PFNWGLSETPBUFFERATTRIBARBPROC			wglSetPbufferAttrib;
#endif

#ifdef GL_ARB_occlusion_query
	extern PFNGLGENQUERIESARBPROC					glGenQueries;
	extern PFNGLDELETEQUERIESARBPROC				glDeleteQueries;
	extern PFNGLISQUERYARBPROC						glIsQuery;
	extern PFNGLBEGINQUERYARBPROC					glBeginQuery;
	extern PFNGLENDQUERYARBPROC						glEndQuery;
	extern PFNGLGETQUERYIVARBPROC					glGetQueryiv;
	extern PFNGLGETQUERYOBJECTIVARBPROC				glGetQueryObjectiv;
	extern PFNGLGETQUERYOBJECTUIVARBPROC			glGetQueryObjectuiv;
#endif

#ifdef WGL_ARB_make_current_read
	extern PFNWGLMAKECONTEXTCURRENTARBPROC			wglMakeContextCurrent;
	extern PFNWGLGETCURRENTREADDCARBPROC			wglGetCurrentReadDC;
#endif

#ifdef GL_ARB_window_pos
	extern PFNGLWINDOWPOS2DPROC						glWindowPos2d;
	extern PFNGLWINDOWPOS2DVPROC					glWindowPos2dv;
	extern PFNGLWINDOWPOS2FPROC						glWindowPos2f;
	extern PFNGLWINDOWPOS2FVPROC					glWindowPos2fv;
	extern PFNGLWINDOWPOS2IPROC						glWindowPos2i;
	extern PFNGLWINDOWPOS2IVPROC					glWindowPos2iv;
	extern PFNGLWINDOWPOS2SPROC						glWindowPos2s;
	extern PFNGLWINDOWPOS2SVPROC					glWindowPos2sv;
	extern PFNGLWINDOWPOS3DPROC						glWindowPos3d;
	extern PFNGLWINDOWPOS3DVPROC					glWindowPos3dv;
	extern PFNGLWINDOWPOS3FPROC						glWindowPos3f;
	extern PFNGLWINDOWPOS3FVPROC					glWindowPos3fv;
	extern PFNGLWINDOWPOS3IPROC						glWindowPos3i;
	extern PFNGLWINDOWPOS3IVPROC					glWindowPos3iv;
	extern PFNGLWINDOWPOS3SPROC						glWindowPos3s;
	extern PFNGLWINDOWPOS3SVPROC					glWindowPos3sv;
#endif

#ifdef GL_EXT_geometry_shader4
	extern PFNGLPROGRAMPARAMETERIEXTPROC 			glProgramParameteri;
#endif

#ifdef GL_ARB_framebuffer_object
	extern PFNGLISRENDERBUFFERPROC					glIsRenderbuffer;
	extern PFNGLBINDRENDERBUFFERPROC				glBindRenderbuffer;
	extern PFNGLDELETERENDERBUFFERSPROC				glDeleteRenderbuffers;
	extern PFNGLGENRENDERBUFFERSPROC				glGenRenderbuffers;
	extern PFNGLRENDERBUFFERSTORAGEPROC				glRenderbufferStorage;
	extern PFNGLGETRENDERBUFFERPARAMETERIVPROC		glGetRenderbufferParameteriv;
	extern PFNGLISFRAMEBUFFERPROC					glIsFramebuffer;
	extern PFNGLBINDFRAMEBUFFERPROC					glBindFramebuffer;
	extern PFNGLDELETEFRAMEBUFFERSPROC				glDeleteFramebuffers;
	extern PFNGLGENFRAMEBUFFERSPROC					glGenFramebuffers;
	extern PFNGLCHECKFRAMEBUFFERSTATUSPROC			glCheckFramebufferStatus;
	extern PFNGLFRAMEBUFFERTEXTURE1DPROC			glFramebufferTexture1D;
	extern PFNGLFRAMEBUFFERTEXTURE2DPROC			glFramebufferTexture2D;
	extern PFNGLFRAMEBUFFERTEXTURE3DPROC			glFramebufferTexture3D;
	extern PFNGLFRAMEBUFFERRENDERBUFFERPROC			glFramebufferRenderbuffer;
	extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC	glGetFramebufferAttachmentParameteriv;
	extern PFNGLGENERATEMIPMAPPROC					glGenerateMipmap;
	extern PFNGLBLITFRAMEBUFFERPROC					glBlitFramebuffer;
	extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC	glRenderbufferStorageMultisample;
	extern PFNGLFRAMEBUFFERTEXTURELAYERPROC			glFramebufferTextureLayer;
#endif

#ifdef GL_EXT_blend_func_separate
	extern PFNGLBLENDFUNCSEPARATEEXTPROC			glBlendFuncSeparate;
#endif

#ifdef GL_VERSION_2_0
	extern PFNGLDRAWBUFFERSPROC						glDrawBuffers;
#endif

}

using namespace OpenGLExtPointer;

#ifndef __CUDACC__
	namespace Engine
	{
		template <class C>
		void Extension::read(C&target, const String&name)
		{
			#if SYSTEM==WINDOWS
				target = (C)wglGetProcAddress(name.c_str());
			#elif SYSTEM==UNIX
				target = (C)glXGetProcAddressARB((const GLubyte*)name.c_str());
			#else
				target = NULL;
			#endif
			if (!target)
				ErrMessage("Critical OpenGL extension exception: Pointer not found: "+name);
		}

		template <class C>
		void Extension::readSilent(C&target, const String&name)
		{
			#if SYSTEM==WINDOWS
				target = (C)wglGetProcAddress(name.c_str());
			#elif SYSTEM==UNIX
				target = (C)glXGetProcAddressARB((const GLubyte*)name.c_str());
			#else
				target = NULL;
			#endif
		}
	}
#endif


#endif

