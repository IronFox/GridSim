#include "../../global_root.h"
#include "extensions.h"

/******************************************************************

OpenGL-Extension-manager. The listed bits specify extensions
whose functions are derived in groups.


******************************************************************/

	bool						Engine::GLShader::Instance::forget_on_destruct=false;
	const Engine::GLShader::Instance	*Engine::GLShader::Instance::installed_instance(NULL);
	Engine::GLShader::Template::RenderConfiguration		Engine::GLShader::Template::global_render_config;

	Buffer<Engine::GLShader::Template::LightShadowAttachment,0,Swap>		Engine::GLShader::Template::shadow_attachments;


	#define EXT_CONTEXT(name)					{group	= #name;}
	#define EXT_GET_EXTENSION(var)				Extension::read(var,String(#var)+group)
	#define EXT_GET_EXTENSION_NO_CHECK(var)		Extension::readSilent(var,String(#var)+group)

	#undef glBlendFuncSeparate
	#undef glGenBuffers
	#undef glBindBuffer
	#undef glBufferData
	#undef glDeleteBuffers

	namespace OpenGLExtPointer
	{
	#ifdef GL_VERSION_2_0
		PFNGLDRAWBUFFERSPROC			glDrawBuffers = NULL;
	#endif

	#ifdef GL_ARB_multitexture
		PFNGLMULTITEXCOORD1FARBPROC		glMultiTexCoord1f	= NULL;
		PFNGLMULTITEXCOORD1FVARBPROC	glMultiTexCoord1fv	= NULL;
		PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2f	= NULL;
		PFNGLMULTITEXCOORD2FVARBPROC	glMultiTexCoord2fv	= NULL;
		PFNGLMULTITEXCOORD3FARBPROC		glMultiTexCoord3f	= NULL;
		PFNGLMULTITEXCOORD3FVARBPROC	glMultiTexCoord3fv	= NULL;
		PFNGLMULTITEXCOORD4FARBPROC		glMultiTexCoord4f	= NULL;
		PFNGLMULTITEXCOORD4FVARBPROC	glMultiTexCoord4fv	= NULL;
		PFNGLMULTITEXCOORD1DARBPROC		glMultiTexCoord1d	= NULL;
		PFNGLMULTITEXCOORD1DVARBPROC	glMultiTexCoord1dv	= NULL;
		PFNGLMULTITEXCOORD2DARBPROC		glMultiTexCoord2d	= NULL;
		PFNGLMULTITEXCOORD2DVARBPROC	glMultiTexCoord2dv	= NULL;
		PFNGLMULTITEXCOORD3DARBPROC		glMultiTexCoord3d	= NULL;
		PFNGLMULTITEXCOORD3DVARBPROC	glMultiTexCoord3dv	= NULL;
		PFNGLMULTITEXCOORD4DARBPROC		glMultiTexCoord4d	= NULL;
		PFNGLMULTITEXCOORD4DVARBPROC	glMultiTexCoord4dv	= NULL;
		PFNGLMULTITEXCOORD1IARBPROC		glMultiTexCoord1i	= NULL;
		PFNGLMULTITEXCOORD1IVARBPROC	glMultiTexCoord1iv	= NULL;
		PFNGLMULTITEXCOORD2IARBPROC		glMultiTexCoord2i	= NULL;
		PFNGLMULTITEXCOORD2IVARBPROC	glMultiTexCoord2iv	= NULL;
		PFNGLMULTITEXCOORD3IARBPROC		glMultiTexCoord3i	= NULL;
		PFNGLMULTITEXCOORD3IVARBPROC	glMultiTexCoord3iv	= NULL;
		PFNGLMULTITEXCOORD4IARBPROC		glMultiTexCoord4i	= NULL;
		PFNGLMULTITEXCOORD4IVARBPROC	glMultiTexCoord4iv	= NULL;
		PFNGLACTIVETEXTUREARBPROC		glActiveTexture	= NULL;
		PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTexture	= NULL;
	#endif

	#ifdef GL_NV_register_combiners
		PFNGLCOMBINERPARAMETERFVNVPROC			glCombinerParameterfv	= NULL;
		PFNGLCOMBINERPARAMETERFNVPROC			glCombinerParameterf	= NULL;
		PFNGLCOMBINERPARAMETERIVNVPROC			glCombinerParameteriv	= NULL;
		PFNGLCOMBINERPARAMETERINVPROC			glCombinerParameteri	= NULL;
		PFNGLCOMBINERINPUTNVPROC				glCombinerInput		= NULL;
		PFNGLCOMBINEROUTPUTNVPROC				glCombinerOutput		= NULL;
		PFNGLFINALCOMBINERINPUTNVPROC			glFinalCombinerInput	= NULL;
		PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC	glGetCombinerInputParameterfv	= NULL;
		PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC	glGetCombinerInputParameteriv	= NULL;
		PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC	glGetCombinerOutputParameterfv	= NULL;
		PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC	glGetCombinerOutputParameteriv	= NULL;
		PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC	glGetFinalCombinerInputParameterfv	= NULL;
		PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC	glGetFinalCombinerInputParameteriv	= NULL;
	#endif


	#ifdef GL_ARB_vertex_program
		PFNGLVERTEXATTRIB2FARBPROC				glVertexAttrib2f		= NULL;
		PFNGLVERTEXATTRIB2DARBPROC				glVertexAttrib2d		= NULL;
		PFNGLVERTEXATTRIB3FARBPROC				glVertexAttrib3f		= NULL;
		PFNGLVERTEXATTRIB3DARBPROC				glVertexAttrib3d		= NULL;
		PFNGLVERTEXATTRIB4FARBPROC				glVertexAttrib4f		= NULL;
		PFNGLVERTEXATTRIB4DARBPROC				glVertexAttrib4d		= NULL;
		PFNGLVERTEXATTRIB2FVARBPROC				glVertexAttrib2fv		= NULL;
		PFNGLVERTEXATTRIB2DVARBPROC				glVertexAttrib2dv		= NULL;
		PFNGLVERTEXATTRIB3FVARBPROC				glVertexAttrib3fv		= NULL;
		PFNGLVERTEXATTRIB3DVARBPROC				glVertexAttrib3dv		= NULL;
		PFNGLVERTEXATTRIB4IVARBPROC				glVertexAttrib4iv		= NULL;
		PFNGLVERTEXATTRIB4UIVARBPROC			glVertexAttrib4uiv		= NULL;
		PFNGLVERTEXATTRIB4FVARBPROC				glVertexAttrib4fv		= NULL;
		PFNGLVERTEXATTRIB4DVARBPROC				glVertexAttrib4dv		= NULL;
		PFNGLVERTEXATTRIB4NIVARBPROC			glVertexAttrib4Niv		= NULL;
		PFNGLVERTEXATTRIB4NUIVARBPROC			glVertexAttrib4Nuiv		= NULL;
		PFNGLVERTEXATTRIBPOINTERARBPROC			glVertexAttribPointer		= NULL;
		PFNGLENABLEVERTEXATTRIBARRAYARBPROC		glEnableVertexAttribArray	= NULL;
		PFNGLDISABLEVERTEXATTRIBARRAYARBPROC	glDisableVertexAttribArray	= NULL;
		PFNGLPROGRAMSTRINGARBPROC				glProgramString				= NULL;
		PFNGLBINDPROGRAMARBPROC					glBindProgram				= NULL;
		PFNGLDELETEPROGRAMSARBPROC				glDeletePrograms			= NULL;
		PFNGLGENPROGRAMSARBPROC					glGenPrograms				= NULL;
		PFNGLPROGRAMENVPARAMETER4DARBPROC		glProgramEnvParameter4d		= NULL;
		PFNGLPROGRAMENVPARAMETER4DVARBPROC		glProgramEnvParameter4dv	= NULL;
		PFNGLPROGRAMENVPARAMETER4FARBPROC		glProgramEnvParameter4f		= NULL;
		PFNGLPROGRAMENVPARAMETER4FVARBPROC		glProgramEnvParameter4fv	= NULL;
		PFNGLPROGRAMLOCALPARAMETER4DARBPROC		glProgramLocalParameter4d	= NULL;
		PFNGLPROGRAMLOCALPARAMETER4DVARBPROC	glProgramLocalParameter4dv	= NULL;
		PFNGLPROGRAMLOCALPARAMETER4FARBPROC		glProgramLocalParameter4f	= NULL;
		PFNGLPROGRAMLOCALPARAMETER4FVARBPROC	glProgramLocalParameter4fv	= NULL;
		PFNGLGETPROGRAMENVPARAMETERDVARBPROC	glGetProgramEnvParameterdv	= NULL;
		PFNGLGETPROGRAMENVPARAMETERFVARBPROC	glGetProgramEnvParameterfv	= NULL;
		PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC	glGetProgramLocalParameterdv	= NULL;
		PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC	glGetProgramLocalParameterfv	= NULL;
		PFNGLGETPROGRAMIVARBPROC				glGetProgramiv				= NULL;
		PFNGLGETPROGRAMSTRINGARBPROC			glGetProgramString			= NULL;
		PFNGLGETVERTEXATTRIBDVARBPROC			glGetVertexAttribdv			= NULL;
		PFNGLGETVERTEXATTRIBFVARBPROC			glGetVertexAttribfv			= NULL;
		PFNGLGETVERTEXATTRIBIVARBPROC			glGetVertexAttribiv			= NULL;
		PFNGLGETVERTEXATTRIBPOINTERVARBPROC		glGetVertexAttribPointerv	= NULL;
		PFNGLISPROGRAMARBPROC					glIsProgram					= NULL;
	#endif


	#ifdef GL_ARB_vertex_buffer_object
		PFNGLBINDBUFFERARBPROC					glBindBuffer			= NULL;
		PFNGLDELETEBUFFERSARBPROC				glDeleteBuffers			= NULL;
		PFNGLGENBUFFERSARBPROC					glGenBuffers			= NULL;
		PFNGLISBUFFERARBPROC					glIsBuffer				= NULL;
		PFNGLBUFFERDATAARBPROC					glBufferData			= NULL;
		PFNGLBUFFERSUBDATAARBPROC				glBufferSubData			= NULL;
		PFNGLGETBUFFERSUBDATAARBPROC			glGetBufferSubData		= NULL;
		PFNGLMAPBUFFERARBPROC					glMapBuffer				= NULL;
		PFNGLUNMAPBUFFERARBPROC					glUnmapBuffer			= NULL;
		PFNGLGETBUFFERPARAMETERIVARBPROC		glGetBufferParameteriv	= NULL;
		PFNGLGETBUFFERPOINTERVARBPROC			glGetBufferPointerv		= NULL;
	#endif

	#ifdef GL_EXT_compiled_vertex_array
		PFNGLLOCKARRAYSEXTPROC					glLockArrays			= NULL;
		PFNGLUNLOCKARRAYSEXTPROC				glUnlockArrays			= NULL;
	#endif

	#ifdef GL_ARB_shader_objects
		PFNGLDELETEOBJECTARBPROC				glDeleteObject			= NULL;
		PFNGLGETHANDLEARBPROC					glGetHandle				= NULL;
		PFNGLDETACHOBJECTARBPROC				glDetachObject			= NULL;
		PFNGLCREATESHADEROBJECTARBPROC			glCreateShaderObject	= NULL;
		PFNGLSHADERSOURCEARBPROC				glShaderSource			= NULL;
		PFNGLCOMPILESHADERARBPROC				glCompileShader			= NULL;
		PFNGLCREATEPROGRAMOBJECTARBPROC			glCreateProgramObject	= NULL;
		PFNGLATTACHOBJECTARBPROC				glAttachObject			= NULL;
		PFNGLLINKPROGRAMARBPROC					glLinkProgram			= NULL;
		PFNGLUSEPROGRAMOBJECTARBPROC			glUseProgramObject		= NULL;
		PFNGLVALIDATEPROGRAMARBPROC				glValidateProgram		= NULL;
		PFNGLUNIFORM1FARBPROC					glUniform1f			= NULL;
		PFNGLUNIFORM2FARBPROC					glUniform2f			= NULL;
		PFNGLUNIFORM3FARBPROC					glUniform3f			= NULL;
		PFNGLUNIFORM4FARBPROC					glUniform4f			= NULL;
		PFNGLUNIFORM1IARBPROC					glUniform1i			= NULL;
		PFNGLUNIFORM2IARBPROC					glUniform2i			= NULL;
		PFNGLUNIFORM3IARBPROC					glUniform3i			= NULL;
		PFNGLUNIFORM4IARBPROC					glUniform4i			= NULL;
		PFNGLUNIFORM1FVARBPROC					glUniform1fv			= NULL;
		PFNGLUNIFORM2FVARBPROC					glUniform2fv			= NULL;
		PFNGLUNIFORM3FVARBPROC					glUniform3fv			= NULL;
		PFNGLUNIFORM4FVARBPROC					glUniform4fv			= NULL;
		PFNGLUNIFORM1IVARBPROC					glUniform1iv			= NULL;
		PFNGLUNIFORM2IVARBPROC					glUniform2iv			= NULL;
		PFNGLUNIFORM3IVARBPROC					glUniform3iv			= NULL;
		PFNGLUNIFORM4IVARBPROC					glUniform4iv			= NULL;
		PFNGLUNIFORMMATRIX2FVARBPROC			glUniformMatrix2fv		= NULL;
		PFNGLUNIFORMMATRIX3FVARBPROC			glUniformMatrix3fv		= NULL;
		PFNGLUNIFORMMATRIX4FVARBPROC			glUniformMatrix4fv		= NULL;
		PFNGLGETOBJECTPARAMETERFVARBPROC		glGetObjectParameterfv	= NULL;
		PFNGLGETOBJECTPARAMETERIVARBPROC		glGetObjectParameteriv	= NULL;
		PFNGLGETINFOLOGARBPROC					glGetInfoLog			= NULL;
		PFNGLGETATTACHEDOBJECTSARBPROC			glGetAttachedObjects	= NULL;
		PFNGLGETUNIFORMLOCATIONARBPROC			glGetUniformLocation	= NULL;
		PFNGLGETACTIVEUNIFORMARBPROC			glGetActiveUniform		= NULL;
		PFNGLGETUNIFORMFVARBPROC				glGetUniformfv			= NULL;
		PFNGLGETUNIFORMIVARBPROC				glGetUniformiv			= NULL;
		PFNGLGETSHADERSOURCEARBPROC				glGetShaderSource		= NULL;
	#endif


	#ifdef WGL_EXT_swap_control
		PFNWGLSWAPINTERVALEXTPROC				wglSwapInterval			= NULL;
		PFNWGLGETSWAPINTERVALEXTPROC			wglGetSwapInterval		= NULL;
	#endif

	#ifdef WGL_ARB_pixel_format
		PFNWGLCHOOSEPIXELFORMATARBPROC			wglChoosePixelFormat		= NULL;
		PFNWGLGETPIXELFORMATATTRIBIVARBPROC		wglGetPixelFormatAttribiv	= NULL;
		PFNWGLGETPIXELFORMATATTRIBFVARBPROC		wglGetPixelFormatAttribfv	= NULL;
	#endif

	#ifdef WGL_ARB_extensions_string
		PFNWGLGETEXTENSIONSSTRINGARBPROC		wglGetExtensionsString	= NULL;
	#endif

	#ifdef WGL_ARB_pbuffer
		PFNWGLCREATEPBUFFERARBPROC				wglCreatePbuffer		= NULL;
		PFNWGLGETPBUFFERDCARBPROC				wglGetPbufferDC			= NULL;
		PFNWGLRELEASEPBUFFERDCARBPROC			wglReleasePbufferDC		= NULL;
		PFNWGLDESTROYPBUFFERARBPROC				wglDestroyPbuffer		= NULL;
		PFNWGLQUERYPBUFFERARBPROC				wglQueryPbuffer			= NULL;
	#endif

	#if defined(GLX_SGIX_fbconfig) && !defined(GLX_VERSION_1_3)
		PFNGLXGETFBCONFIGATTRIBSGIXPROC			glXGetFBConfigAttrib	= NULL;
		PFNGLXCHOOSEFBCONFIGSGIXPROC			glXChooseFBConfig		= NULL;
		PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC	glXCreateGLXPixmapWithConfig	= NULL;
		PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC	glXCreateContextWithConfig		= NULL;
		PFNGLXGETVISUALFROMFBCONFIGSGIXPROC		glXGetVisualFromFBConfig		= NULL;
		PFNGLXGETFBCONFIGFROMVISUALSGIXPROC		glXGetFBConfigFromVisual		= NULL;
	#endif


	#ifdef WGL_ARB_render_texture
		PFNWGLBINDTEXIMAGEARBPROC				wglBindTexImage			= NULL;
		PFNWGLRELEASETEXIMAGEARBPROC			wglReleaseTexImage		= NULL;
		PFNWGLSETPBUFFERATTRIBARBPROC			wglSetPbufferAttrib		= NULL;
	#endif

	#ifdef GL_ARB_occlusion_query
		PFNGLGENQUERIESARBPROC					glGenQueries			= NULL;
		PFNGLDELETEQUERIESARBPROC				glDeleteQueries			= NULL;
		PFNGLISQUERYARBPROC						glIsQuery				= NULL;
		PFNGLBEGINQUERYARBPROC					glBeginQuery			= NULL;
		PFNGLENDQUERYARBPROC					glEndQuery				= NULL;
		PFNGLGETQUERYIVARBPROC					glGetQueryiv			= NULL;
		PFNGLGETQUERYOBJECTIVARBPROC			glGetQueryObjectiv		= NULL;
		PFNGLGETQUERYOBJECTUIVARBPROC			glGetQueryObjectuiv		= NULL;
	#endif

	#ifdef WGL_ARB_make_current_read
		PFNWGLMAKECONTEXTCURRENTARBPROC			wglMakeContextCurrent	= NULL;
		PFNWGLGETCURRENTREADDCARBPROC			wglGetCurrentReadDC		= NULL;
	#endif

	#ifdef GL_ARB_window_pos
		PFNGLWINDOWPOS2DPROC					glWindowPos2d			= NULL;
		PFNGLWINDOWPOS2DVPROC					glWindowPos2dv			= NULL;
		PFNGLWINDOWPOS2FPROC					glWindowPos2f			= NULL;
		PFNGLWINDOWPOS2FVPROC					glWindowPos2fv			= NULL;
		PFNGLWINDOWPOS2IPROC					glWindowPos2i			= NULL;
		PFNGLWINDOWPOS2IVPROC					glWindowPos2iv			= NULL;
		PFNGLWINDOWPOS2SPROC					glWindowPos2s			= NULL;
		PFNGLWINDOWPOS2SVPROC					glWindowPos2sv			= NULL;
		PFNGLWINDOWPOS3DPROC					glWindowPos3d			= NULL;
		PFNGLWINDOWPOS3DVPROC					glWindowPos3dv			= NULL;
		PFNGLWINDOWPOS3FPROC					glWindowPos3f			= NULL;
		PFNGLWINDOWPOS3FVPROC					glWindowPos3fv			= NULL;
		PFNGLWINDOWPOS3IPROC					glWindowPos3i			= NULL;
		PFNGLWINDOWPOS3IVPROC					glWindowPos3iv			= NULL;
		PFNGLWINDOWPOS3SPROC					glWindowPos3s			= NULL;
		PFNGLWINDOWPOS3SVPROC					glWindowPos3sv			= NULL;
	#endif

	#ifdef GL_EXT_geometry_shader4
		PFNGLPROGRAMPARAMETERIEXTPROC 			glProgramParameteri		= NULL;
	#endif

	#ifdef GL_ARB_framebuffer_object
		PFNGLISRENDERBUFFERPROC					glIsRenderbuffer		= NULL;
		PFNGLBINDRENDERBUFFERPROC				glBindRenderbuffer		= NULL;
		PFNGLDELETERENDERBUFFERSPROC			glDeleteRenderbuffers	= NULL;
		PFNGLGENRENDERBUFFERSPROC				glGenRenderbuffers		= NULL;
		PFNGLRENDERBUFFERSTORAGEPROC			glRenderbufferStorage	= NULL;
		PFNGLGETRENDERBUFFERPARAMETERIVPROC		glGetRenderbufferParameteriv	= NULL;
		PFNGLISFRAMEBUFFERPROC					glIsFramebuffer			= NULL;
		PFNGLBINDFRAMEBUFFERPROC				glBindFramebuffer		= NULL;
		PFNGLDELETEFRAMEBUFFERSPROC				glDeleteFramebuffers	= NULL;
		PFNGLGENRENDERBUFFERSPROC				glGenFramebuffers		= NULL;
		PFNGLCHECKFRAMEBUFFERSTATUSPROC			glCheckFramebufferStatus	= NULL;
		PFNGLFRAMEBUFFERTEXTURE1DPROC			glFramebufferTexture1D	= NULL;
		PFNGLFRAMEBUFFERTEXTURE2DPROC			glFramebufferTexture2D	= NULL;
		PFNGLFRAMEBUFFERTEXTURE3DPROC			glFramebufferTexture3D	= NULL;
		PFNGLFRAMEBUFFERRENDERBUFFERPROC		glFramebufferRenderbuffer = NULL;
		PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC	glGetFramebufferAttachmentParameteriv = NULL;
		PFNGLGENERATEMIPMAPPROC					glGenerateMipmap		= NULL;
		PFNGLBLITFRAMEBUFFERPROC				glBlitFramebuffer		= NULL;
		PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC	glRenderbufferStorageMultisample = NULL;
		PFNGLFRAMEBUFFERTEXTURELAYERPROC		glFramebufferTextureLayer = NULL;
	#endif
	
	#ifdef GL_EXT_blend_func_separate
		PFNGLBLENDFUNCSEPARATEEXTPROC			glBlendFuncSeparate		= NULL;
	#endif
	
	
	}


namespace Engine
{
	#ifdef GL_ARB_multitexture
		static GLuint	tangent_layer=GL_TEXTURE1;
	#endif


	void		glTangent3f(GLfloat x, GLfloat y, GLfloat z)
	{
		#ifdef GL_ARB_multitexture
			if (glMultiTexCoord3f)
				glMultiTexCoord3f(tangent_layer,x,y,z);
		#endif
	}
	
	void		glTangent3fv(const GLfloat*v)
	{
		#ifdef GL_ARB_multitexture
			if (glMultiTexCoord3fv)
				glMultiTexCoord3fv(tangent_layer,v);
		#endif
	}
	
	void		glTangentTextureLayer(GLuint layer)
	{
		#ifdef GL_ARB_multitexture
			tangent_layer = layer;
		#endif
	}



	GLTextureFactory::GLTextureFactory():frame_buffer(0),result_texture(0),/*color_buffer(0),*/depth_buffer(0),width(0),height(0),alpha(false),mipmapping(false)
	{}
	
	GLTextureFactory::GLTextureFactory(unsigned width_, unsigned height_,bool alpha_,bool mipmapping_):frame_buffer(0),result_texture(0),/*color_buffer(0),*/depth_buffer(0),width(0),height(0),alpha(false),mipmapping(false)
	{
		create(width_,height_,alpha_,mipmapping_);
	}
	
	GLTextureFactory::~GLTextureFactory()
	{
		clear();
	}
	
	void	GLTextureFactory::clear()
	{
		#ifdef GL_ARB_framebuffer_object
			if (glDeleteFramebuffers)
			{
				if (frame_buffer)
					glDeleteFramebuffers(1, &frame_buffer);
				if (depth_buffer)
					glDeleteRenderbuffers(1, &depth_buffer);
				/*if (color_buffer)
					glDeleteRenderbuffers(1, &color_buffer);*/
			}
		#endif
		frame_buffer = 0;
		depth_buffer = 0;
/* 		color_buffer = 0; */
	}
	
	
	
	
	
	bool				GLTextureFactory::checkFormat(unsigned width_, unsigned height_, bool alpha_, bool mipmapping_)
	{
		if (width == width_ && height == height_ && alpha == alpha_ && mipmapping == mipmapping_)
			return true;
		return create(width_,height_,alpha_,mipmapping_);
	}
	
	bool				GLTextureFactory::create(unsigned width_, unsigned height_,bool alpha_, bool mipmapping_)
	{
		width = width_;
		height = height_;
		alpha = alpha_;
		mipmapping = mipmapping_;
		//samples = samples_;
		
		clear();
		
		#ifdef GL_ARB_framebuffer_object
			if (glGenFramebuffers)
			{
					glGenFramebuffers( 1, &frame_buffer );
					glGenRenderbuffers( 1, &depth_buffer );
					//glGenRenderbuffers( 1, &color_buffer );

					glBindRenderbuffer( GL_RENDERBUFFER, depth_buffer );
					//if (!samples)
						glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height );
					/*else
						glRenderbufferStorageMultisample(GL_RENDERBUFFER,samples, GL_DEPTH_COMPONENT24, width, height);*/
/* 
					glBindRenderbuffer( GL_RENDERBUFFER, color_buffer );
					glRenderbufferStorage( GL_RENDERBUFFER, alpha?GL_RGBA8:GL_RGB8, width, height ); */


					GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
					if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
					{
						clear();
						return false;
					}

					
				
					glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);


					glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer );
					//glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_buffer );
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
					
					return true;
			}
		#endif
		
		return false;
	}
	
	bool				GLTextureFactory::begin()
	{
		#ifdef GL_ARB_framebuffer_object
			if (glBindFramebuffer && frame_buffer)
			{
				glGetError();//flush errors
				glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT,&previous_frame_buffer);		
				glGetFloatv(GL_COLOR_CLEAR_VALUE,previous_clear_color);
				glGetIntegerv(GL_VIEWPORT, viewport); 
								
				glGenTextures( 1, &result_texture );

				glBindTexture( GL_TEXTURE_2D, result_texture );

				glTexImage2D( GL_TEXTURE_2D, 0, alpha?GL_RGBA:GL_RGB, 
						    width, height, 
						    0, alpha?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, 0 );

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
				glBindTexture(GL_TEXTURE_2D,0);
				
				glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result_texture, 0 );
				if (!glGetError()==GL_NO_ERROR)
				{
					glDeleteTextures(1,&result_texture);
					result_texture = 0;
					return false;
				}
				glViewport(0,0,width,height);
				glClearColor(0,0,0,0);
				glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

				return true;
			}
		#endif
		return false;
	}
	

	
	
	GLuint				GLTextureFactory::end()
	{
		#ifdef GL_ARB_framebuffer_object
			if (glBindFramebuffer && frame_buffer)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, previous_frame_buffer);
				glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);				
				glClearColor(previous_clear_color[0],previous_clear_color[1],previous_clear_color[2],previous_clear_color[3]);
				/*glBindTexture(GL_TEXTURE_2D,result_texture);
				glGenerateMipmap(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,0);*/
				GLuint result = result_texture;

				if (mipmapping)
				{
					ASSERT_NOT_NULL__(glGenerateMipmap);
					glBindTexture(GL_TEXTURE_2D,result_texture);
						glGenerateMipmap(GL_TEXTURE_2D);
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

					glBindTexture(GL_TEXTURE_2D,0);
				}

				result_texture = 0;
				return result;
			}
			
		#endif
		return 0;
	}
	
	void				GLTextureFactory::test()
	{
		#ifdef GL_ARB_framebuffer_object
			
			glGetError();//flush errors
			static GLuint test_texture = 0;
			if (!test_texture)
			{
				glGenTextures( 1, &test_texture );

				glBindTexture( GL_TEXTURE_2D, test_texture );

				glTexImage2D( GL_TEXTURE_2D, 0, alpha?GL_RGBA:GL_RGB, 
						    width, height, 
						    0, alpha?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, 0 );

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			}

			
			if (glBindFramebuffer && frame_buffer)
			{
				glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT,&previous_frame_buffer);		
				glGetFloatv(GL_COLOR_CLEAR_VALUE,previous_clear_color);
				glGetIntegerv(GL_VIEWPORT, viewport); 
								
				
				glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, test_texture, 0 );
				if (!glGetError()==GL_NO_ERROR)
					return;
				glViewport(0,0,width,height);
				glClearColor(0,0,0,0);
				glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
				
				
				
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
				glClearColor(previous_clear_color[0],previous_clear_color[1],previous_clear_color[2],previous_clear_color[3]);
				result_texture = 0;
			}
			
		#endif
	}
	
	void				GLTextureFactory::abort()
	{
		#ifdef GL_ARB_framebuffer_object
			if (glBindFramebuffer && frame_buffer)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, previous_frame_buffer);
				glDeleteTextures(1,&result_texture);
				result_texture = 0;
				glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);				
				glClearColor(previous_clear_color[0],previous_clear_color[1],previous_clear_color[2],previous_clear_color[3]);
			}
		#endif
	}

	bool				GLTextureFactory::isActive()	const
	{
		return result_texture != 0;
	}
	
	bool				GLTextureFactory::isCreated()	const
	{
		return frame_buffer != 0;
	}




	void ExtCont::divide(String line)
	{
		unsigned elements	= 0;
		for (unsigned i	= 1; i < line.length(); i++)
			if (line.get(i) == ' ' && line.get(i-1) != ' ')
				elements++;
		if (line.length() && line.lastChar() != ' ')
			elements++;
		field.setSize(elements);
		for (index_t i	= 0; i < elements; i++)
		{
			index_t p	= line.indexOf(' ');
			if (!p)
				field[i] = line;
			else
			{
				field[i] = line.subString(0,p-1);
				index_t j	= i-1;
				while (j < i && field[j+1] < field[j])
				{
					swp(field[j],field[j+1]);
					j--;
				}
				line.erase(0,p);
				line = line.trimLeft();
			}
		}
	}

	bool ExtCont::lookup(const String&element)
	{
		index_t		lower	= 0,
					upper	= field.length();
		while (lower< upper && upper <= field.length())
		{
			index_t el	= (lower+upper)/2;
			if (field[el] > element)
				upper	= el;
			else
				if (field[el] < element)
					lower	= el+1;
				else
					return true;
		}
		return false;
	}


	GLuint Extension::loadProgram(const char*source, GLenum type)
	{
		#ifdef GL_ARB_vertex_program
			GLuint	pname;
			glGenPrograms(1,&pname);
			glBindProgram(type, pname);
			glProgramString(type, GL_PROGRAM_FORMAT_ASCII_ARB,
								(GLsizei)strlen(source), source);
			int errl;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB,&errl);
			if (errl >= 0)
			{
				unsigned end	=errl+1;
				while (source[end] != ',' && source[end] != ';' && source[end] != ' ' && source[end] != '\n' && source[end])
					end++;
				String source2	= source;
				source2.insert(end,'<');
				source2.insert(errl,'>');
				String line = String(source).subString(errl-1,end-errl-1);
				compile_log	<<	(char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB)<<": \""<<line<<"\"\n---------------------------------------\n"<<source2<<nl;
				glDeletePrograms(1,&pname);
				return 0;
			}
			return pname;
		#else
			compile_log << "Error: Headers missing"<<nl;
			return 0;
		#endif
	}

	GLuint Extension::loadVertexProgram(const char*source)
	{
		return loadProgram(source,GL_VERTEX_PROGRAM_ARB);
	}

	GLuint Extension::loadFragmentProgram(const char*source)
	{
		#ifdef GL_ARB_fragment_program
			return loadProgram(source,GL_FRAGMENT_PROGRAM_ARB);
		#else
			compile_log << "Error: Headers missing"<<nl;
			return 0;
		#endif
	}

	GLuint Extension::loadGeometryProgram(const char*source)
	{
		#ifdef GL_EXT_geometry_shader4
			return loadProgram(source,GL_GEOMETRY_SHADER_EXT);
		#else
			compile_log << "Error: Headers missing"<<nl;
			return 0;
		#endif


	}

	#ifdef GL_ARB_shader_objects
	
	namespace GLShader
	{
	
		Variable::Variable(Instance*parent,GLint handle_, const String&name_):instance(parent),handle(handle_),name(name_)
		{}
	
		Variable::Variable():instance(NULL),handle(-1)
		{}

		bool			Variable::setf(float value)
		{
			if (handle == -1)
				return false;
			bool was_installed = instance->installed();
			if (!was_installed && assert_is_installed)
				FATAL__("trying to update variable '"+name+"' while shader is NOT installed");
			if (!was_installed && (lock_uninstalled || !instance->install()))
				return false;
			
			glGetError();//flush errors
			glUniform1f(handle, value);
		
			if (!was_installed)
				instance->uninstall();
		
			return glGetError() == GL_NO_ERROR;
		}

		bool					Variable::set(const TVec3<>&vector)
		{
			if (handle == -1)
				return false;
			bool was_installed = instance->installed();
			if (!was_installed && assert_is_installed)
				FATAL__("trying to update variable '"+name+"' while shader is NOT installed");
			if (!was_installed && (lock_uninstalled || !instance->install()))
				return false;

			glGetError();//flush errors
			glUniform3f(handle, vector.x,vector.y,vector.z);
		
			if (!was_installed)
				instance->uninstall();
		
			return glGetError() == GL_NO_ERROR;

		}
	
		bool					Variable::set(const TVec2<>&vector)
		{
			if (handle == -1)
				return false;
			bool was_installed = instance->installed();
			if (!was_installed && assert_is_installed)
				FATAL__("trying to update variable '"+name+"' while shader is NOT installed");
			if (!was_installed && (lock_uninstalled || !instance->install()))
				return false;

			glGetError();//flush errors
			glUniform2f(handle, vector.x,vector.y);
		
			if (!was_installed)
				instance->uninstall();
		
			return glGetError() == GL_NO_ERROR;
		}
	

		bool			Variable::set4f(float x, float y, float z, float w)
		{
			if (handle == -1)
				return false;
			bool was_installed = instance->installed();
			if (!was_installed && assert_is_installed)
				FATAL__("trying to update variable '"+name+"' while shader is NOT installed");
			if (!was_installed && (lock_uninstalled || !instance->install()))
				return false;
			
			glGetError();//flush errors
			glUniform4f(handle, x,y,z,w);

			if (!was_installed)
				instance->uninstall();
			return glGetError() == GL_NO_ERROR;
		}


		bool			Variable::set(const TVec4<>&vector)
		{
			if (handle == -1)
				return false;
			bool was_installed = instance->installed();
			if (!was_installed && assert_is_installed)
				FATAL__("trying to update variable '"+name+"' while shader is NOT installed");
			if (!was_installed && (lock_uninstalled || !instance->install()))
				return false;
			
			glGetError();//flush errors
			glUniform4f(handle, vector.x,vector.y,vector.z,vector.w);

			if (!was_installed)
				instance->uninstall();
			return glGetError() == GL_NO_ERROR;
		}
	
		bool					Variable::set(const TMatrix3<>&matrix)
		{
			if (handle == -1)
				return false;
			bool was_installed = instance->installed();
			if (!was_installed && assert_is_installed)
				FATAL__("trying to update variable '"+name+"' while shader is NOT installed");
			if (!was_installed && (lock_uninstalled || !instance->install()))
				return false;
			glGetError();//flush errors
			glUniformMatrix3fv(handle,1,false,matrix.v);
			if (!was_installed)
				instance->uninstall();
			return glGetError() == GL_NO_ERROR;
		}
	
		bool					Variable::set(const TMatrix4<>&matrix)
		{
			if (handle == -1)
				return false;
			bool was_installed = instance->installed();
			if (!was_installed && assert_is_installed)
				FATAL__("trying to update variable '"+name+"' while shader is NOT installed");
			if (!was_installed && (lock_uninstalled || !instance->install()))
				return false;
			glGetError();//flush errors
			glUniformMatrix4fv(handle,1,false,matrix.v);
			if (!was_installed)
				instance->uninstall();
			return glGetError() == GL_NO_ERROR;
		}
	
		bool			Variable::seti(int value)
		{
			if (handle == -1)
				return false;
			bool was_installed = instance->installed();
			if (!was_installed && assert_is_installed)
				FATAL__("trying to update variable '"+name+"' while shader is NOT installed");
			if (!was_installed && (lock_uninstalled || !instance->install()))
				return false;
		
			glGetError();//flush errors
			glUniform1i(handle, value);
		
			if (!was_installed)
				instance->uninstall();
		
			return glGetError() == GL_NO_ERROR;
		}
		bool			Variable::set2i(int x, int y)
		{
			if (handle == -1)
				return false;
			bool was_installed = instance->installed();
			if (!was_installed && assert_is_installed)
				FATAL__("trying to update variable '"+name+"' while shader is NOT installed");
			if (!was_installed && (lock_uninstalled || !instance->install()))
				return false;
		
			glGetError();//flush errors
			glUniform2i(handle, x,y);
		
			if (!was_installed)
				instance->uninstall();
		
			return glGetError() == GL_NO_ERROR;
		}
	
		bool Variable::lock_uninstalled(false);
		bool Variable::assert_is_installed(false);
		bool Instance::warn_on_failure(true);
	
		Instance::Instance():program_handle(0),vertex_shader(0),fragment_shader(0),geometry_shader(0)
		{}

		Instance::~Instance()
		{
			if (!application_shutting_down && !forget_on_destruct)
				clear();
		}

		bool	Instance::loaded()	const
		{
			return this != NULL && program_handle != 0;
		}


		void	Instance::clear()
		{
			if (program_handle && glGetHandle(GL_PROGRAM_OBJECT_ARB) == program_handle)
				deInstall();
			if (vertex_shader)
				glDeleteObject(vertex_shader);
			if (fragment_shader)
				glDeleteObject(fragment_shader);
			if (geometry_shader)
				glDeleteObject(geometry_shader);
			if (program_handle)
				glDeleteObject(program_handle);
			vertex_shader	= 0;
			fragment_shader	= 0;
			geometry_shader = 0;
			program_handle	= 0;
			composition.clear();
			//log.reset();
		}
	
		void	Instance::adoptData(Instance&other)
		{
			bool reinstall = other.isInstalled();
			if (reinstall)
				other.uninstall();
			elif (installed())
				uninstall();

			clear();
			composition.adoptData(other.composition);
			vertex_shader = other.vertex_shader;
			fragment_shader = other.fragment_shader;
			geometry_shader = other.geometry_shader;
			program_handle = other.program_handle;
			log.adoptData(other.log);
			other.vertex_shader = 0;
			other.fragment_shader = 0;
			other.geometry_shader = 0;
			other.program_handle = 0;

			if (reinstall)
				install();
		}
	


		bool		extractFileContent(const String&filename, String&target, StringBuffer&log_out)
		{
			FileStream	file(filename.c_str(),FileStream::StandardRead);
			if (!file.isOpen())
			{
				log_out << "Unable to read from '"<<filename<<"'\n";
				target = "";
				return false;
			}
			uint64_t size = file.size();
			target.resize(static_cast<size_t>(size));
			if (!file.read(target.mutablePointer(),size))
			{
				target = "";
				return false;
			}
			return true;
		}

		bool		extractFileContent(const String&filename, String&target)
		{
			FileStream	file(filename.c_str(),FileStream::StandardRead);
			if (!file.isOpen())
			{
				target = "";
				return false;
			}
			uint64_t size = file.size();
			target.resize(static_cast<size_t>(size));
			if (!file.read(target.mutablePointer(),size))
			{
				target = "";
				return false;
			}
			return true;
		}



		void				Composition::clear()
		{
			sharedSource = "";
			vertexSource = "";
			fragmentSource = "";
			geometrySource = "";
		}

		void				Composition::adoptData(Composition&other)
		{
			sharedSource.adoptData(other.sharedSource);
			vertexSource.adoptData(other.vertexSource);
			fragmentSource.adoptData(other.fragmentSource);
			geometrySource.adoptData(other.geometrySource);
		}


		Composition&			Composition::Load(const String&source)
		{
			if (source.contains("[vertex]"))
			{
				static const String type[5]={"[object]","[shared]","[fragment]","[vertex]","[geometry]"};

				const String		*current(NULL);
					

				const char	*at = source.c_str(),
							*end = at+source.length();
				while (at < end)
				{
					const char*	min = NULL;
					const String*item	= NULL;
					
					for (unsigned k	= 0; k < ARRAYSIZE(type); k++)
					{
						const char*my_at = ::Template::strstr(at,type[k].c_str());
						if (!my_at)
							continue;
						if (!min || my_at < min)
						{
							min	= my_at;
							item = type+k;
						}
					}
					ReferenceExpression<char>	segment = item?ReferenceExpression<char>(at,min-at):ReferenceExpression<char>(at,end-at);
					if (item)
						at = min+item->length();
					else
						at = end;
					switch (current-type)
					{
						case 1:
							sharedSource = segment;
						break;
						case 2:
							fragmentSource	= segment;
						break;
						case 3:
							vertexSource = segment;
						break;
						case 4:
							geometrySource = segment;
						break;
					}
					current	= item;
				}
			}
			else
			{
				static Tokenizer::Configuration config;
				if (!config.recursion_break)
				{
					config.recursion_up = "{";
					config.recursion_down = "}";
					config.recursion_break = true;
					config.trim_characters.set('\n');
					config.trim_characters.set('\r');
				}
				static StringList tokens;
				Tokenizer::tokenize(source, config, tokens);	//!< @overload

				for (index_t i = 0; i+1 < tokens.count();)
				{
					String*source_target = NULL;
					const String&group = tokens[i];
					{
						if (group == "shared")
							source_target = &sharedSource;
						elif (group == "vertex")
							source_target = &vertexSource;
						elif (group == "fragment")
							source_target = &fragmentSource;
						elif (group == "geometry")
							source_target = &geometrySource;
					}
					i++;
					if (!source_target)
					{
						if (group.trimRef().length() > 0)
							ErrMessage("Warning: unexpected code token(s) encountered: '"+group+"'");
						continue;
					}
					for (; i < tokens.count(); )
					{
						ReferenceExpression<char>	source = tokens[i++].trimRef();
						if (source.length() == 0)
							continue;
						
						if (source.pointer()[0] != '{' || source.pointer()[source.length()-1] != '}')
							continue;
						
						*source_target = ReferenceExpression<char>(source.pointer() + 1, source.length()-2);
						source_target = NULL;
						break;
					}
					if (source_target != NULL)
						ErrMessage("Warning: Unable to find body for group '"+group+"'");
				}

			}
			return *this;
		}

		bool				Composition::LoadFromFiles(const String&shared_file, const String&vertex_file, const String&fragment_file)
		{
			geometrySource = "";
			return	Engine::GLShader::extractFileContent(shared_file,sharedSource)
					&&
					Engine::GLShader::extractFileContent(vertex_file,vertexSource)
					&&
					Engine::GLShader::extractFileContent(fragment_file,fragmentSource);
		}

		bool				Composition::LoadFromFiles(const String&shared_file, const String&vertex_file, const String&fragment_file, const String&geometry_file)
		{
			return	Engine::GLShader::extractFileContent(shared_file,sharedSource)
					&&
					Engine::GLShader::extractFileContent(vertex_file,vertexSource)
					&&
					Engine::GLShader::extractFileContent(fragment_file,fragmentSource)
					&&
					Engine::GLShader::extractFileContent(geometry_file,geometrySource);
		}

		bool				Composition::LoadFromFile(const String&object_file)
		{
			String content;
			if (!Engine::GLShader::extractFileContent(object_file,content))
				return false;
			Load(content);
			return true;
		}

	
		bool	Instance::extractFileContent(const String&filename, String&target)
		{
			return Engine::GLShader::extractFileContent(filename,target,log);

		}

		static String	glProgramTypeName(GLenum program_type)
		{
			#undef ecase
			#define ecase(type)	case type:	return #type;
		
			switch (program_type)
			{
				ecase(GL_VERTEX_SHADER_ARB)
				ecase(GL_FRAGMENT_SHADER_ARB)
				ecase(GL_GEOMETRY_SHADER_EXT)
			}
			return "Unkown shader type ("+String(program_type)+")";
		}

		GLhandleARB Instance::loadShader(const String&source, GLenum program_type)
		{
			if (!glCreateShaderObject)
			{
				log<<"extension not loaded!\n";
				return 0;
			}
			glGetError();//flush previous errors
			const char*str	= source.c_str();
			GLhandleARB shader	= glCreateShaderObject(program_type);
			glShaderSource(shader,1,&str, NULL);
			glCompileShader(shader);
			GLenum glErr	= glGetError();
			while (glErr != GL_NO_ERROR)
			{
				log<<"error while compiling shader("<<glProgramTypeName(program_type)<<"): \""<<glGetErrorName(glErr)<<"\"\n";
				glDeleteObject(shader);
				return 0;
			}
			int compiled;
			glGetObjectParameteriv(shader,GL_OBJECT_COMPILE_STATUS_ARB,&compiled);

			int log_len	= 0;
			glGetObjectParameteriv(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &log_len);
			if (log_len > 1)
			{
				Array<char,Primitive>	log_str(log_len);
				int		written(0);
				glGetInfoLog(shader, log_len, &written, log_str.pointer());
				if (strstr(log_str.pointer(),"was successfully compiled to run on hardware") && log_len < 64)//maybe some spaces and newlines and stuff
				{
					log_len = 0;
				}
				else
				{
					log<<"\n-- shader("<<glProgramTypeName(program_type)<<") construction --\n";
					log<<log_str.pointer();
				}
			}
			if (!compiled)
			{
				log<<"internal error: shader did not compile\n";

				glDeleteObject(shader);
				shader = 0;
			}
			if (log_len > 1)
			{
				log<<"\noriginal source was:\n--------------------------------";
				String copy	= source;
				unsigned line(1);
				index_t at;
				while (copy.length())
				{
					log<<"\n("+IntToStr(line++)+") ";
					if (at = copy.indexOf('\n'))
					{
						log<<copy.subString(0,at-1);
						copy.erase(0,at);
					}
					else
					{
						log<<copy;
						copy	= "";
					}
				}
				log<<"\n--------------------------------\n";
			}
		
			return shader;
		}

		void		Instance::loadRequired(const Composition&composition, GLenum geometry_type, GLenum output_type, unsigned max_vertices)
		{
			if (!load(composition,geometry_type,output_type,max_vertices))
				FATAL__(report());
		}


		bool		Instance::load(const Composition&composition, GLenum geometry_type, GLenum output_type, unsigned max_vertices)
		{
			clear();
			this->composition = composition;
			glGetError();//flush errors
			vertex_shader	= loadShader(composition.sharedSource+composition.vertexSource,GL_VERTEX_SHADER_ARB);
			fragment_shader	= loadShader(composition.sharedSource+composition.fragmentSource,GL_FRAGMENT_SHADER_ARB);
			#ifdef GL_GEOMETRY_SHADER_EXT
				bool has_geometry_shader = composition.geometrySource.indexOf("main")!=0;
				if (has_geometry_shader)
				{
					String full_source;
					index_t p = composition.geometrySource.indexOf('#');
					if (!p)
						full_source = 
						"#version 120 \n"
						"#extension GL_EXT_geometry_shader4 : enable	\n"
						+composition.sharedSource+composition.geometrySource;
					else
					{
						const char*c = composition.geometrySource.c_str()+p;
						while (*c && *c != '\n')
							c++;
						full_source = composition.geometrySource.subString(0,c-composition.geometrySource.c_str()+1)+composition.sharedSource+(c+1);
					}
				
				
					geometry_shader = loadShader(full_source,GL_GEOMETRY_SHADER_EXT);
					if (geometry_shader && !glProgramParameteri)
					{
						String group;
						EXT_CONTEXT(EXT);
						EXT_GET_EXTENSION_NO_CHECK(glProgramParameteri);
						if (!glProgramParameteri)
						{
							log << "(glProgramParameteri not available)\n";
							clear();
							return false;
						}
					}
					if (has_geometry_shader && !geometry_shader)
					{
						log<<"(geometry shader not set)\n";
						clear();
						return false;
					}
				}
				else
					geometry_shader = 0;
			#endif
			if (!vertex_shader || !fragment_shader)
			{
				log<<"(shader-fragment(s) not set)\n";
				clear();
				return false;
			}
			program_handle	= glCreateProgramObject();
			glAttachObject(program_handle, vertex_shader);
			glAttachObject(program_handle, fragment_shader);
			if (geometry_shader)
			{
				glAttachObject(program_handle, geometry_shader);

				glProgramParameteri(program_handle, GL_GEOMETRY_INPUT_TYPE_EXT, geometry_type);
				glProgramParameteri(program_handle, GL_GEOMETRY_OUTPUT_TYPE_EXT, output_type);
				glProgramParameteri(program_handle, GL_GEOMETRY_VERTICES_OUT_EXT, max_vertices);
			}


			glLinkProgram(program_handle);

			GLenum glErr	= glGetError();
			while (glErr != GL_NO_ERROR)
			{
				log<<"error while linking program: \""<<glGetErrorName(glErr)<<"\"\n";
				clear();
				return false;
			}
			int linked;
			glGetObjectParameteriv(program_handle,GL_OBJECT_LINK_STATUS_ARB, &linked);
			int log_len	= 0;
			glGetObjectParameteriv(program_handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &log_len);
			if (log_len > 0)
			{
				char	*log_str	= SHIELDED_ARRAY(new char[log_len],log_len);
				int		written(0);
				glGetInfoLog(program_handle, log_len, &written, log_str);
				log<<"\n-- program construction --\n";
				log<<log_str;
				log<<nl;
				DISCARD_ARRAY(log_str);
			}
			if (!linked)
			{
				log<<"program link failed\n";
				clear();
				return false;
			}
#if 0
			glValidateProgram(program_handle);
			log_len	= 0;
			glGetObjectParameteriv(program_handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &log_len);
			if (log_len > 0)
			{
				char	*log_str	= SHIELDED_ARRAY(new char[log_len],log_len);
				int		written(0);
				glGetInfoLog(program_handle, log_len, &written, log_str);
				log<<"\n-- program validation --\n";
				log<<log_str;
				log<<nl;
				DISCARD_ARRAY(log_str);
			}

			int validated;
			glGetObjectParameteriv(program_handle, GL_OBJECT_VALIDATE_STATUS_ARB,&validated);
			if (!validated)
			{
				log << "validation failed\n";
				clear();
				return false;
			}
#endif /*9*/
			return true;
		}

		bool			Instance::validate()
		{
			log	<< "validating program...\n";
			if (!program_handle)
			{
				log << "no program present\n";
				return false;
			}
			glGetError();//flush errors
			glValidateProgram(program_handle);
			if (GLenum err	= glGetError())
				if (err != GL_NO_ERROR)
				{
					log << "GL: "<<glGetErrorName(err);
					return false;
				}
			int log_len	= 0;
			glGetObjectParameteriv(program_handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &log_len);
			if (log_len > 0)
			{
				char	*log_str	= SHIELDED_ARRAY(new char[log_len],log_len);
				int		written(0);
				glGetInfoLog(program_handle, log_len, &written, log_str);
				log<<"\n-- program validation --\n";
				log<<log_str;
				log<<nl;
				DISCARD_ARRAY(log_str);
			}

			int validated;
			glGetObjectParameteriv(program_handle, GL_OBJECT_VALIDATE_STATUS_ARB,&validated);
			if (!validated)
			{
				log << "validation failed\n";
				return false;
			}
			return true;
		}

		void		Instance::loadRequiredComposition(const String&object_source, GLenum input_type, GLenum output_type, unsigned max_vertices)
		{
			if (!loadComposition(object_source,input_type,output_type,max_vertices))
				FATAL__(report());
		}

		bool		Instance::loadComposition(const String&object_source, GLenum input_type, GLenum output_type, unsigned max_vertices)
		{
			return load(Composition().Load(object_source),input_type,output_type,max_vertices);
		}



		String	Instance::report()
		{
			String rs	= log.toString();
			log.reset();
			return rs;
		}

		void			Instance::enableFailureWarning()
		{
			warn_on_failure	= true;
		}

		void			Instance::disableFailureWarning()
		{
			warn_on_failure	= false;
		}

		void			Instance::suppressLocationFailureWarning()
		{
			warn_on_failure	= false;
		}
	
		void			Instance::suppressWarnings()
		{
			warn_on_failure	= false;
		}

		bool			Instance::getContent(GLint name, float*out_field)
		{
			if (!program_handle)
			{
				log	<< "Program not created"<<nl;
				return false;
			}
			if (name	== -1)
			{
				log	<< "The requested variable does not exist"<<nl;
				return false;
			}
			glGetUniformfv(program_handle,name,out_field);
			return true;
		}


		Variable Instance::locate(const String&name, bool warn_on_fail)
		{
			if (!program_handle)
			{
				log	<< "Program not created"<<nl;
				if (warn_on_failure && warn_on_fail)
					ErrMessage("unable to locate uniform variable '"+name+"' - no object present.");
				return Variable();
			}
			GLint result	= glGetUniformLocation(program_handle,name.c_str());
			if (result	== -1)
				if (warn_on_failure && warn_on_fail)
					ErrMessage("unable to locate uniform variable '"+name+"'");
				else
					log << nl<<"Unable to locate uniform variable '"<<name<<"'"<<nl;
			return Variable(this,result,name);
		}

		bool			Instance::install(/*const TCodeLocation&location*/)
		{
			if (!loaded())
				return false;

			glGetError();	//flush previous errors
			if (installed_instance)
			{
				FATAL__("overriding previously installed shader object 0x"+PointerToHex(installed_instance)/*+". Remote shader was installed in:\n"+String(installed_object->installed_in.toString())*/);
				installed_instance->uninstall();
			}
			//installed_in = location;
			glUseProgramObject(program_handle);
			GLenum error	= glGetError();
			bool is_installed = error	== GL_NO_ERROR;
			if (!is_installed)
			{
				log.reset();
				log << "Shader installation of object #"<<program_handle<<" failed"<<nl;
				log	<< glGetErrorName(error)<<nl;
				if (glGetHandle(GL_PROGRAM_OBJECT_ARB) != 0)
				{
					log << "OpenGL reports error but shader is installed. Uninstalling shader and returning false."<<nl;
					glUseProgramObject(0);
				}
			}

			if (is_installed)
				installed_instance = this;
			else
				installed_instance = NULL;

			return is_installed;
		}

		bool			Instance::install(/*const TCodeLocation&location*/)	const
		{
			if (!this)
				return true;
			if (!program_handle)
				return false;
			glGetError();	//flush previous errors
			if (installed_instance)
			{
				FATAL__("overriding previously installed shader object 0x"+PointerToHex(installed_instance)/*+". Remote shader was installed in:\n"+String(installed_object->installed_in.toString())*/);
				installed_instance->uninstall();
			}
			//installed_in = location;
			glUseProgramObject(program_handle);
			GLenum error	= glGetError();
			bool is_installed = error	== GL_NO_ERROR;
			if (!is_installed)
				glUseProgramObject(0);

			if (is_installed)
				installed_instance = this;
			else
				installed_instance = NULL;
			return is_installed;
		}



		bool			Instance::permissiveInstall(/*const TCodeLocation&location*/)
		{
			if (!loaded())
			{
				if (installed_instance)
				{
					installed_instance->uninstall();
					installed_instance = NULL;
				}
				return true;
			}
			if (installed_instance == this)
				return true;


			if (installed_instance)
				installed_instance->uninstall();

			glGetError();	//flush previous errors
			//installed_in = location;

			glUseProgramObject(program_handle);
			GLenum error	= glGetError();

			bool is_installed = (error == GL_NO_ERROR);
			if (!is_installed)
			{
				log.reset();
				log << "Shader installation of object #"<<program_handle<<" failed"<<nl;
				log	<< glGetErrorName(error)<<nl;
				if (glGetHandle(GL_PROGRAM_OBJECT_ARB) != 0)
				{
					log << "OpenGL reports error but shader is installed. Uninstalling shader and returning false."<<nl;
					glUseProgramObject(0);
				}
			}
			if (is_installed)
				installed_instance = this;
			else
				installed_instance = NULL;
			return is_installed;
		}



		bool			Instance::permissiveInstall(/*const TCodeLocation&location*/)	const
		{
			if (!loaded())
			{
				if (installed_instance)
				{
					installed_instance->uninstall();
					installed_instance = NULL;
				}
				return true;
			}
			if (installed_instance == this)
				return true;


			if (installed_instance)
				installed_instance->uninstall();

			glGetError();	//flush previous errors
			//installed_in = location;

			glUseProgramObject(program_handle);
			GLenum error	= glGetError();

			bool is_installed = (error == GL_NO_ERROR);
			if (!is_installed)
				glUseProgramObject(0);

			if (is_installed)
				installed_instance = this;
			else
				installed_instance = NULL;
			return is_installed;
		}






		void			Instance::deInstall()	const
		{
			uninstall();
		}

		void			Instance::uninstall()	const
		{
			if (!this)
				return;
			if (installed_instance != this)
				FATAL__("Uninstalling but installed pointer is not this");
			if (glGetHandle(GL_PROGRAM_OBJECT_ARB) != program_handle)
				FATAL__("UnInstalling but installed handle is not mine");
			installed_instance = NULL;
			if (glUseProgramObject)
				glUseProgramObject(0);
		}

		/*static*/	void	Instance::permissiveUninstall()
		{
			installed_instance = NULL;
			if (glUseProgramObject)
				glUseProgramObject(0);
		}

	
	
	
	
	
	
	
			int Template::VariableExpression::evaluate(const UserConfiguration&status, Light::Type)
			{
				return status.value(index);
			}

			bool	Template::Block::usesLighting() const
			{
				if (type == LightLoop)
					return true;
				for (index_t i = 0; i < children; i++)
					if (children[i]->usesLighting())
						return true;
				return false;
			}

			void	Template::Block::assemble(StringBuffer&target, const UserConfiguration&user_config, const RenderConfiguration&render_config, Light::Type type, index_t light_index)
			{
				for (index_t i = 0; i < inner_lines.count(); i++)
					if (inner_lines[i].segments.count())
					{
						const Array<String,Adopt>&segments = inner_lines[i].segments;
						for (index_t j = 0; j < segments.count()-1; j++)
						{
							target << segments[j] << light_index;
						}
						target << segments.last()<<nl;
						/*if (inner_lines[i].includes)
							inner_lines[i].includes->Block::assemble(target,user_config,render_config,type,light_index);*/
					}
				bool is_else = false;
				for (index_t i = 0; i < children; i++)
				{
					Block*child = children[i];
					bool do_assemble=false;
					switch (child->type)
					{
						case Any:
							do_assemble = true;
							is_else = false;
						break;
						case Conditional:
							do_assemble = child->condition && child->condition->evaluate(user_config,type);
							is_else = !do_assemble;
						break;
						case ElseConditional:
							do_assemble = is_else && child->condition && child->condition->evaluate(user_config,type);
							is_else = is_else && !do_assemble;
						break;
						case Else:
							do_assemble = is_else;
							is_else = false;
						break;
						case LightLoop:
							do_assemble = true;
							is_else = false;
						break;
					}
					if (do_assemble)
					{
						if (child->type == LightLoop)
						{
							for (index_t j = 0; j < render_config.lights.fillLevel(); j++)
								if (render_config.lights[j] != Light::None)
									child->assemble(target,user_config,render_config,render_config.lights[j],j);
						}
						else
							child->assemble(target,user_config,render_config,type,light_index);
					}
					for (index_t k = 0; k < child->trailing_lines.count(); k++)
						if (child->trailing_lines[k].segments.count())
						{
							const Array<String,Adopt>&segments = child->trailing_lines[k].segments;
							for (index_t j = 0; j < segments.count()-1; j++)
							{
								target << segments[j] << light_index;
							}
							target << segments.last()<<nl;
						}
				}
			}
	
	
			Template::Expression*	Template::RootBlock::processLayer(TokenList&tokens,VariableMap&map, index_t begin, index_t end, String&error)
			{
				index_t level = 0,
						block_begin;

		/* 		cout << "symbol chain is";
				for (unsigned i = begin; i < end; i++)
					cout << ' '<<tokens[i]->content<<"("<<tokens[i]->ident<<")";
				cout << endl;
		 */		
		
				List::Vector<Expression>	expressions;
				for (index_t i = begin; i < end; i++)
				{
					const TToken&token = tokens[i];
					Expression*expression=NULL;
			
					switch (token.ident&Token::Mask)
					{
						case Token::PUp:
							if (!level)
								block_begin = i+1;
							level++;
						break;
						case Token::PDown:
							level--;
							if (!level)
							{
								expression = processLayer(tokens,map,block_begin,i,error);
								if (!expression)
									return NULL;
							}
						break;
					}
					if (!level && !expression)
						switch (token.ident&Token::Mask)
						{
							case Token::And:
								expression = SHIELDED(new AndExpression());
							break;
							case Token::Or:
								expression = SHIELDED(new OrExpression());
							break;
							case Token::Not:
								expression = SHIELDED(new NegationExpression());
							break;
							case Token::String:
							{
								int val;
								bool bval;
								bool convertable = true;
								if (convert(token.content.c_str(),bval))
									val = bval;
								else
									if (!convert(token.content.c_str(),val))
										convertable = false;
								if (convertable)
								{
									expression = SHIELDED(new ConstantExpression());
									((ConstantExpression*)expression)->value = val;
								}
								else
								{
									if (token.content == "omni")
									{
										expression = SHIELDED(new LightTypeExpression());
										((LightTypeExpression*)expression)->type = Light::Omni;
									}
									elif (token.content == "direct")
									{
										expression = SHIELDED(new LightTypeExpression());
										((LightTypeExpression*)expression)->type = Light::Direct;
									}
									elif (token.content == "spot")
									{
										expression = SHIELDED(new LightTypeExpression());
										((LightTypeExpression*)expression)->type = Light::Spot;
									}
									else
									{
										expression = SHIELDED(new VariableExpression());
										((VariableExpression*)expression)->name = token.content;
										((VariableExpression*)expression)->index = map.define(token.content);
									}
								}
							}
							break;
							case Token::Plus:
								expression = SHIELDED(new SumExpression());
							break;
							case Token::Minus:
								expression = SHIELDED(new DifferenceExpression());
							break;
							case Token::Times:
								expression = SHIELDED(new ProductExpression());
							break;
							case Token::Divide:
								expression = SHIELDED(new QuotientExpression());
							break;
							case Token::Equals:
								expression = SHIELDED(new EqualExpression());
							break;
							case Token::Differs:
								expression = SHIELDED(new NotEqualExpression());
							break;
							case Token::Greater:
								expression = SHIELDED(new GreaterExpression());
							break;
							case Token::Less:
								expression = SHIELDED(new LessExpression());
							break;
							case Token::GreaterOrEqual:
								expression = SHIELDED(new GreaterOrEqualExpression());
							break;
							case Token::LessOrEqual:
								expression = SHIELDED(new LessOrEqualExpression());
							break;
						}
					if (expression)
					{
						expression->level = token.ident&Level::Mask;
						expressions.append(expression);
					}
				}
				for (unsigned level = 1; level <= 5; level++)
				{
					for (unsigned i = 0; i < expressions; i++)
					{
						Expression*expression = expressions[i];
						if (expression->level == level*0x1000)
						{
							if (expression->adoptRight(expressions[i+1]))
								expressions.drop(i+1);
							if (expression->adoptLeft(expressions[i-1]))
							{
								if (expressions.drop(i-1))
									i--;
							}
						}
					}
				}
				if (expressions != 1)
				{
					error = "Expression parsing failed";
					return NULL;
				}
				return expressions.drop(TypeInfo<index_t>::zero);
			}
	
			Template::Expression*	Template::RootBlock::parseCondition(const char*condition, VariableMap&map, String&error)
			{
				static StringTokenizer	tokenizer;
				static bool				initialized = false;
		

				if (!initialized)
				{
		
					tokenizer.setStringIdent(Token::String);
					tokenizer.reg("(",Token::PUp);
					tokenizer.reg(")",Token::PDown);
					tokenizer.reg("+",Token::Plus|Level::Five);
					tokenizer.reg("-",Token::Minus|Level::Five);
					tokenizer.reg("*",Token::Times|Level::Four);
					tokenizer.reg("/",Token::Divide|Level::Four);
					tokenizer.reg("&&",Token::And|Level::One);
					tokenizer.reg("||",Token::Or|Level::One);
					tokenizer.reg("==",Token::Equals|Level::Two);
					tokenizer.reg("!=",Token::Differs|Level::Two);
					tokenizer.reg(">",Token::Greater|Level::Three);
					tokenizer.reg("<",Token::Less|Level::Three);
					tokenizer.reg(">=",Token::GreaterOrEqual|Level::Three);
					tokenizer.reg("<=",Token::LessOrEqual|Level::Three);
					tokenizer.reg("!",Token::Not|Level::Zero);

					tokenizer.reg(" ",Token::Space);
					tokenizer.reg("  ",Token::Space);
					tokenizer.reg("   ",Token::Space);
					tokenizer.reg("\t",Token::Space);
					tokenizer.finalizeInitialization();

			
			
					initialized = true;
				}
		
				TokenList	tokens;
		
				tokenizer.parse(condition,tokens);
		
				Expression*result = processLayer(tokens,map,0,tokens.count(),error);
				if (result && !result->validate())
				{
					error = "Expression '"+result->toString()+"' failed to validate";
					DISCARD(result);
					return NULL;
				}
		/* 		if (result)
					cout << result->toString()<<endl; */
				return result;
			}
	
			static void stripComments(String&line, bool&in_comment)
			{
				const char	*c = line.c_str(),
							*comment_start = line.c_str();

				unsigned count = 0;
				while (*c)
				{
					if (in_comment)
					{
						c = ::Template::strstr(c,"*/");
						if (c)
						{
							unsigned index = comment_start-line.c_str();
							line.erase(comment_start-line.c_str(),c-comment_start+2);
							c = line.c_str();
							c+=index;
							in_comment = false;
						}
						else
						{
							line.erase(comment_start-line.c_str());
							return;
						}
					}
			
			
					c = ::Template::strchr(c,'/');
					if (!c)
						return;
					c++;
					if (*c == '/')	//line comment
					{
						line.erase(c-line.c_str()-1);
						return;
					}
					if (*c == '*')
					{
						comment_start = c-1;
						in_comment = true;
					}
				}
			}
	
			static void		logLine(const Array<String,Adopt>&lines,index_t index, StringBuffer&log_out)
			{
				index_t begin = index >= 2?index-2:0,
						end = index+3 < lines.count()?index+3:lines.count();
				//log_out << "-------- context --------"<<nl;
				log_out << nl;
				if (begin)
					log_out << "     ..."<<nl;
				for (index_t i = begin; i < end; i++)
				{
					if (i == index)
						log_out << " >>> ";
					else
						log_out << "     ";
					log_out << lines[i];
			
					log_out<<nl;
				}
				if (end < lines.count())
					log_out << "     ..."<<nl;
			}
	
			bool			Template::RootBlock::scan(const String&source, VariableMap&map, StringBuffer&log_out, index_t&line)
			{
				shade_invoked = source.findWord("shade")!=0;
				shade2_invoked = source.findWord("shade2")!=0;
				custom_shade_invoked = source.findWord("customShade")!=0;
				spotlight_invoked = source.findWord("spotLight")!=0;
				omnilight_invoked = source.findWord("omniLight")!=0;
				directlight_invoked = source.findWord("directLight")!=0;

				clear();
				Block*current = this;
				index_t block_begin = 0;
				Array<String,Adopt>	lines;
		
				bool in_comment = false;
				explode('\n',source,lines);
		
		
		
				for (index_t i = 0; i < lines.count(); i++)
				{
					stripComments(lines[i],in_comment);
					lines[i].trimThis();

					if (lines[i].beginsWith("#include"))
					{
						String file = lines[i].subString(8).trimThis();
						if (!file.length())
						{
							log_out << "#include directive broken in line "<<(i+1)<<nl;
							logLine(lines,i,log_out);					
							return false;
						}
						if ((file.firstChar() == '"' && file.lastChar() == '"')
							||
							(file.firstChar() == '<' && file.lastChar() == '>'))
							file = file.subString(1,file.length()-2).trimThis();
						String*include = locateShaderIncludable(file);
						if (!include)
						{
							log_out << "Unable to find '"<<file<<"' for inclusion in line "<<(i+1)<<nl;
							logLine(lines,i,log_out);					
							return false;
						}
						Array<String,AdoptStrategy>	temp;
						explode('\n',*include,temp);
						lines.erase(i);
						lines.insertImport(i,temp);
					}
				}
				for (index_t i = 0; i < lines.count(); i++)
				{
					const char*c = lines[i].c_str();
					while (*c && isWhitespace(*c))
						c++;
					if (*c != '#')
						continue;
			

					c++;
					while (*c && isWhitespace(*c))
						c++;
					const char*w = c;
					while (*c && !isWhitespace(*c))
						c++;
					String	word(w,c-w);
			
					if (word == "version" || word == "extension" || word == "include")
						continue;
			
			
			
					Array<Line,Adopt>&target = current->children
												? current->children.last()->trailing_lines
												: current->inner_lines;
			
		/* 			if (current->children)
						cout << "appending "<<(i-block_begin)<<" line(s):"<<endl; */
					target.setSize(i-block_begin);
					for (index_t j = 0; j < target.size(); j++)
					{
						explode(current->light_loop_constant,lines[block_begin+j],target[j].segments);
		/* 				if (current->children)
							cout << "  "<<lines[block_begin+j]<<endl; */
					}
			
					block_begin = i+1;
			

					if (word=="endlight")
					{
						if (current->parent && (current->type == LightLoop))
							current = current->parent;
						else
						{
							log_out << "misplaced #"<<word<<" in line "<<(line+i+1)<<nl;
							if (current->parent && (current->type == Conditional || current->type == ElseConditional || current->type == Else))
								log_out << "    (expected #endif)"<<nl;
							logLine(lines,i,log_out);
							return false;
						}
					}
					elif (word=="endif")
					{
						if (current->parent && (current->type == Conditional || current->type == ElseConditional || current->type == Else))
							current = current->parent;
						else
						{
							log_out << "misplaced #"<<word<<" in line "<<(line+i+1)<<nl;
							if (current->parent && (current->type == LightLoop))
								log_out << "    (expected #endlight)"<<nl;
							logLine(lines,i,log_out);
							return false;
						}
					}
					elif (word == "if")
					{
						if (*c)
							c++;
						/*...*/
						Block*parent = current;
						current = current->children.append();
						current->parent = parent;
						current->light_loop_constant = parent->light_loop_constant;
						current->type = Conditional;
						String error;
						current->condition = parseCondition(c,map,error);
						if (!current->condition)
						{
							log_out << "Condition parse error '"<<error <<"' in line "<< (line+i+1)<<nl;
							logLine(lines,i,log_out);
							return false;
						}
			
					}
					elif (word == "elif")
					{
						if (*c)
							c++;
						if (!current->parent)
						{
							log_out << "misplaced #"<<word<<" in line "<<(line+i+1)<<nl;
							logLine(lines,i,log_out);
							return false;
						}
				
						{
							Block*predecessor = current->parent->children.last();
							if (!predecessor || (predecessor->type != Conditional && predecessor->type != ElseConditional))
							{
								log_out << "misplaced #"<<word<<" in line "<<(line+i+1)<<nl;
								logLine(lines,i,log_out);
								return false;
							}
						}
						Block*parent = current->parent;
						current = current->parent->children.append();
						current->parent = parent;
						current->light_loop_constant = parent->light_loop_constant;
						current->type = ElseConditional;
						String  error;
						current->condition = parseCondition(c,map,error);
						if (!current->condition)
						{
							log_out << "Condition parse error '"<<error <<"' in line "<< (line+i+1)<<nl;
							logLine(lines,i,log_out);
							return false;
						}
							
					}
					elif (word == "else")
					{
						if (!current->parent)
						{
							log_out << "misplaced #"<<word<<" in line "<<(line+i+1)<<nl;
							logLine(lines,i,log_out);
							return false;
						}
				
						{
							Block*predecessor = current->parent->children.last();
							if (!predecessor || (predecessor->type != Conditional && predecessor->type != ElseConditional))
							{
								log_out << "misplaced #"<<word<<" in line "<<(line+i+1)<<nl;
								logLine(lines,i,log_out);
								return false;
							}
						}
						Block*parent = current->parent;
						current = current->parent->children.append();
						current->parent = parent;
						current->light_loop_constant = parent->light_loop_constant;
						current->type = Else;
					}
					elif (word.beginsWith("light"))
					{
						if (*c)
							c++;
						Block*parent = current;
						current = current->children.append();
						current->parent = parent;
						current->type = LightLoop;
						current->light_loop_constant = c;
						current->light_loop_constant.trimThis();
						if (current->light_loop_constant.firstChar() != '<' || current->light_loop_constant.lastChar() != '>')
						{
							log_out << "illformatted lightloop variable '"<<current->light_loop_constant<<"' found in line "<<(line+i+1)<<". Variables must be formatted '<NAME>'."<<nl;
							logLine(lines,i,log_out);
							return false;
						}
					}
					else
					{
						log_out << "unsupported precompiler directive '#"<<word<<"' found in line "<<(line+i+1)<<nl;
						logLine(lines,i,log_out);
						return false;
					}
				}
				if (current != this)
				{
					log_out << "missing #endif directive at line "<<lines.count()<<nl;
					logLine(lines,lines.count()-1,log_out);
					return false;
				}
		
				{
					Array<Line,Adopt>&target = current->children
												? current->children.last()->trailing_lines
												: current->inner_lines;
			
					target.setSize(lines.count()-block_begin);
					for (unsigned j = 0; j < target.size(); j++)
					{
						explode(current->light_loop_constant,lines[block_begin+j],target[j].segments);
					}
				}
		
				line += lines.count()-1;
				return true;
			}
	
	
	
	
			String			Template::RootBlock::assemble(const RenderConfiguration&render_config,const UserConfiguration&user_config, bool is_shared)
			{
				static StringBuffer	buffer;
				buffer.reset();
				assemble(render_config, user_config,buffer,is_shared);
				return buffer.toString();
			}
	

			bool			Template::RootBlock::usesLighting()	const
			{
				if (shade_invoked || shade2_invoked || custom_shade_invoked)
					return true;
				return Block::usesLighting();
			}
		
			/*static*/ void		Template::RootBlock::shadowFunction(index_t level,StringBuffer&buffer)
			{
				if (level >= shadow_attachments.size() || shadow_attachments[level].isEmpty())
					buffer << "1.0";
				else
					buffer << "fragmentShadow"<<level<<"(position)";
			}

			void			Template::RootBlock::assemble(const RenderConfiguration&render_config, const UserConfiguration&user_config,StringBuffer&buffer, bool is_shared/*=false*/)
			{
				if (is_shared)
				{
					for (index_t i = 0; i < shadow_attachments.count(); i++)
						if (!shadow_attachments[i].isEmpty())
						{
							buffer << nl;
							if (shadow_attachments[i].shared_attachment.isNotEmpty())
								buffer << shadow_attachments[i].shared_attachment<<nl;
							buffer << "float fragmentShadow"<<i<<"(vec3 position)"<<nl
									<< '{'<<nl
									<< shadow_attachments[i].fragment_shadow_code
									<< '}'<<nl;
						}
					for (index_t i = shadow_attachments.count(); i < render_config.lights.fillLevel(); i++)
							buffer << "float fragmentShadow"<<i<<"(vec3 position)"<<nl
									<< '{'<<nl
									<< "return 1.0;"<<nl
									<< '}'<<nl;

					buffer << nl << "void vertexShadow()\n{\n";
					for (index_t i = 0; i < shadow_attachments.size(); i++)
						if (!shadow_attachments[i].isEmpty())
						{
							if (shadow_attachments[i].vertex_shader_attachment.isNotEmpty())
								buffer << shadow_attachments[i].vertex_shader_attachment<<nl;
						}
					buffer << "}\n";
				}
				if (shade_invoked)
				{
					buffer << nl <<
					"vec4 shade(vec3 position, vec3 normal, vec3 reflected)\n"
					"{\n";
					if (render_config.lighting_enabled)
					{
						buffer <<
						"	vec3 result = vec3(0.0);\n"
						"	float fresnel = 1.0-abs(normal.z)*0.5;\n";
						for (index_t i = 0; i < render_config.lights.fillLevel(); i++)
							switch (render_config.lights[i])
							{
								case Light::Omni:
									buffer << 
									"	{\n"
									"		vec3 ldir = (gl_LightSource["<<i<<"].position.xyz-position);\n"
									"		float distance = length(ldir);\n"
									"		ldir /= distance;\n"
									"		float shadow = ";
									shadowFunction(i,buffer);
									buffer <<";\n"
									"		float attenuation = 1.0 / (gl_LightSource["<<i<<"].constantAttenuation + gl_LightSource["<<i<<"].linearAttenuation * distance + gl_LightSource["<<i<<"].quadraticAttenuation * distance*distance);\n"
									"		result += max(dot(normal,ldir),0.0)*gl_FrontLightProduct["<<i<<"].diffuse.rgb*attenuation*shadow;\n"
									"		if (gl_FrontMaterial.shininess > 0.0)\n"
									"			result += pow(max(dot(reflected,ldir),0.0),gl_FrontMaterial.shininess*fresnel)*gl_FrontLightProduct["<<i<<"].specular.rgb*attenuation*fresnel*shadow;\n"
									"		result += gl_FrontLightProduct["<<i<<"].ambient.rgb*attenuation;\n"
									"	}\n";
								break;
								case Light::Direct:
									buffer << 
									"	{\n"
									"		vec3 ldir = gl_LightSource["<<i<<"].position.xyz;\n"
									"		float shadow = ";
													shadowFunction(i,buffer);
													buffer <<";\n"
									"		result += max(dot(normal,ldir),0.0)*gl_FrontLightProduct["<<i<<"].diffuse.rgb*shadow;\n"
									"		if (gl_FrontMaterial.shininess > 0.0)\n"
									"			result += pow(max(dot(reflected,ldir),0.0),gl_FrontMaterial.shininess*fresnel)*gl_FrontLightProduct["<<i<<"].specular.rgb*fresnel*shadow;\n"
									"		result += gl_FrontLightProduct["<<i<<"].ambient.rgb;\n"
									"	}\n";
								break;
								case Light::Spot:
									buffer <<
									"	{\n"
									"		vec3 ldir = (gl_LightSource["<<i<<"].position.xyz-position);\n"
									"		float general_intensity = -dot(gl_LightSource["<<i<<"].spotDirection,ldir);\n"
									"		if (general_intensity > 0.0)\n"
									"		{\n"
									"			float diffuse = dot(normal,ldir);\n"
									"			if (diffuse > 0.0)\n"
									"			{\n"
									"				float distance = length(ldir);\n"
									"				ldir /= distance;\n"
									"				general_intensity /= distance;\n"
									"				diffuse /= distance;\n"
									"				float shadow = ";
														shadowFunction(i,buffer);
														buffer <<";\n"
									"				float attenuation = 1.0 / (gl_LightSource["<<i<<"].constantAttenuation + gl_LightSource["<<i<<"].linearAttenuation * distance + gl_LightSource["<<i<<"].quadraticAttenuation * distance*distance);\n"
									"				float cone_intensity = pow(general_intensity,gl_LightSource["<<i<<"].spotExponent)*attenuation;\n"
									"				diffuse *= cone_intensity;\n"
									"				if (gl_FrontMaterial.shininess > 0.0)\n"
									"				{\n"
									"					float specular = pow(max(dot(reflected,ldir),0.0),gl_FrontMaterial.shininess*fresnel)*cone_intensity*fresnel*shadow;\n"
									"					result += specular * gl_FrontLightProduct["<<i<<"].specular.rgb;\n"
									"				}\n"
									"				result += diffuse * gl_FrontLightProduct["<<i<<"].diffuse.rgb*shadow;\n"
									"				result += attenuation * gl_FrontLightProduct["<<i<<"].ambient.rgb;\n"
									"			}\n"
									"		}\n"
									"	}\n";				
								break;
							}
						buffer << "	return vec4(result,1.0);\n";
					}
					else
						buffer << " return vec4(1.0);\n";
					buffer << "}\n\n";
				}


				if (shade2_invoked)
				{
					buffer << nl <<
					"vec4 shade2(vec3 position, vec3 eye_direction, vec3 normal, vec3 reflected)\n"
					"{\n";
					if (render_config.lighting_enabled)
					{
						buffer <<
						"	vec3 result = vec3(0.0);\n"
						"	float fresnel = 1.0-abs(dot(normal,eye_direction))*0.5;\n";
						for (index_t i = 0; i < render_config.lights.fillLevel(); i++)
							switch (render_config.lights[i])
							{
								case Light::Omni:
									buffer << 
									"	{\n"
									"		vec3 ldir = (gl_LightSource["<<i<<"].position.xyz-position);\n"
									"		float distance = length(ldir);\n"
									"		ldir /= distance;\n"
									"		float shadow = ";
									shadowFunction(i,buffer);
									buffer <<";\n"
									"		float attenuation = 1.0 / (gl_LightSource["<<i<<"].constantAttenuation + gl_LightSource["<<i<<"].linearAttenuation * distance + gl_LightSource["<<i<<"].quadraticAttenuation * distance*distance);\n"
									"		result += max(dot(normal,ldir),0.0)*gl_FrontLightProduct["<<i<<"].diffuse.rgb*attenuation*shadow;\n"
									"		if (gl_FrontMaterial.shininess > 0.0)\n"
									"			result += pow(max(dot(reflected,ldir),0.0),gl_FrontMaterial.shininess*fresnel)*gl_FrontLightProduct["<<i<<"].specular.rgb*attenuation*fresnel*shadow;\n"
									"		result += gl_FrontLightProduct["<<i<<"].ambient.rgb*attenuation;\n"
									"	}\n";
								break;
								case Light::Direct:
									buffer << 
									"	{\n"
									"		vec3 ldir = gl_LightSource["<<i<<"].position.xyz;\n"
									"		float shadow = ";
													shadowFunction(i,buffer);
													buffer <<";\n"
									"		result += max(dot(normal,ldir),0.0)*gl_FrontLightProduct["<<i<<"].diffuse.rgb*shadow;\n"
									"		if (gl_FrontMaterial.shininess > 0.0)\n"
									"			result += pow(max(dot(reflected,ldir),0.0),gl_FrontMaterial.shininess*fresnel)*gl_FrontLightProduct["<<i<<"].specular.rgb*fresnel*shadow;\n"
									"		result += gl_FrontLightProduct["<<i<<"].ambient.rgb;\n"
									"	}\n";
								break;
								case Light::Spot:
									buffer <<
									"	{\n"
									"		vec3 ldir = (gl_LightSource["<<i<<"].position.xyz-position);\n"
									"		float general_intensity = -dot(gl_LightSource["<<i<<"].spotDirection,ldir);\n"
									"		if (general_intensity > 0.0)\n"
									"		{\n"
									"			float diffuse = dot(normal,ldir);\n"
									"			if (diffuse > 0.0)\n"
									"			{\n"
									"				float distance = length(ldir);\n"
									"				ldir /= distance;\n"
									"				general_intensity /= distance;\n"
									"				diffuse /= distance;\n"
									"				float shadow = ";
														shadowFunction(i,buffer);
														buffer <<";\n"
									"				float attenuation = 1.0 / (gl_LightSource["<<i<<"].constantAttenuation + gl_LightSource["<<i<<"].linearAttenuation * distance + gl_LightSource["<<i<<"].quadraticAttenuation * distance*distance);\n"
									"				float cone_intensity = pow(general_intensity,gl_LightSource["<<i<<"].spotExponent)*attenuation;\n"
									"				diffuse *= cone_intensity;\n"
									"				if (gl_FrontMaterial.shininess > 0.0)\n"
									"				{\n"
									"					float specular = pow(max(dot(reflected,ldir),0.0),gl_FrontMaterial.shininess*fresnel)*cone_intensity*fresnel*shadow;\n"
									"					result += specular * gl_FrontLightProduct["<<i<<"].specular.rgb;\n"
									"				}\n"
									"				result += diffuse * gl_FrontLightProduct["<<i<<"].diffuse.rgb*shadow;\n"
									"				result += attenuation * gl_FrontLightProduct["<<i<<"].ambient.rgb;\n"
									"			}\n"
									"		}\n"
									"	}\n";				
								break;
							}
						buffer << "	return vec4(result,1.0);\n";
					}
					else
						buffer << " return vec4(1.0);\n";
					buffer << "}\n\n";
				}
		
		
				if (custom_shade_invoked)
				{
					buffer << nl <<
					"vec4 customShade(vec3 position, vec3 normal, vec3 reflected, vec3 ambient, vec3 diffuse, vec3 specular)\n"
					"{\n";
					if (render_config.lighting_enabled)
					{
						buffer <<
						"	vec3 result = vec3(0.0);\n"
						"	float fresnel = 1.0-abs(normal.z)*0.5;\n";
						for (index_t i = 0; i < render_config.lights.fillLevel(); i++)
							switch (render_config.lights[i])
							{
								case Light::Omni:
									buffer << 
									"	{\n"
									"		vec3 ldir = (gl_LightSource["<<i<<"].position.xyz-position);\n"
									"		float distance = length(ldir);\n"
									"		ldir /= distance;\n"
									"		float shadow = ";
													shadowFunction(i,buffer);
													buffer <<";\n"
									"		float attenuation = 1.0 / (gl_LightSource["<<i<<"].constantAttenuation + gl_LightSource["<<i<<"].linearAttenuation * distance + gl_LightSource["<<i<<"].quadraticAttenuation * distance*distance);\n"
									"		result += max(dot(normal,ldir),0.0)*gl_LightSource["<<i<<"].diffuse.rgb*diffuse*attenuation*shadow;\n"
									"		if (gl_FrontMaterial.shininess > 0.0)\n"
									"			result += pow(max(dot(reflected,ldir),0.0),gl_FrontMaterial.shininess*fresnel)*gl_LightSource["<<i<<"].specular.rgb*specular*attenuation*fresnel*shadow;\n"
									"		result += gl_LightSource["<<i<<"].ambient.rgb*ambient*attenuation;\n"
									"	}\n";
								break;
								case Light::Direct:
									buffer << 
									"	{\n"
									"		vec3 ldir = gl_LightSource["<<i<<"].position.xyz;\n"
									"		float shadow = ";
													shadowFunction(i,buffer);
													buffer <<";\n"
									"		result += max(dot(normal,ldir),0.0)*gl_LightSource["<<i<<"].diffuse.rgb*diffuse*shadow;\n"
									"		if (gl_FrontMaterial.shininess > 0.0)\n"
									"			result += pow(max(dot(reflected,ldir),0.0),gl_FrontMaterial.shininess*fresnel)*gl_LightSource["<<i<<"].specular.rgb*specular*fresnel*shadow;\n"
									"		result += gl_LightSource["<<i<<"].ambient.rgb*ambient;\n"
									"	}\n";
								break;
								case Light::Spot:
									buffer <<
									"	{\n"
									"		vec3 ldir = (gl_LightSource["<<i<<"].position.xyz-position);\n"
									"		float general_intensity = -dot(gl_LightSource["<<i<<"].spotDirection,ldir);\n"
									"		if (general_intensity > 0.0)\n"
									"		{\n"
									"			float shadow = ";
													shadowFunction(i,buffer);
													buffer <<";\n"
									"			float diff = dot(normal,ldir);\n"
									"			if (diff > 0.0)\n"
									"			{\n"
									"				float distance = length(ldir);\n"
									"				ldir /= distance;\n"
									"				general_intensity /= distance;\n"
									"				diff /= distance;\n"
									"				float attenuation = 1.0 / (gl_LightSource["<<i<<"].constantAttenuation + gl_LightSource["<<i<<"].linearAttenuation * distance + gl_LightSource["<<i<<"].quadraticAttenuation * distance*distance);\n"
									"				float cone_intensity = pow(general_intensity,gl_LightSource["<<i<<"].spotExponent)*attenuation;\n"
									"				diff *= cone_intensity;\n"
									"				if (gl_FrontMaterial.shininess > 0.0)\n"
									"				{\n"
									"					float spec = pow(max(dot(reflected,ldir),0.0),gl_FrontMaterial.shininess*fresnel)*cone_intensity*fresnel*shadow;\n"
									"					result += spec * gl_LightSource["<<i<<"].specular.rgb*specular;\n"
									"				}\n"
									"				result += diff * gl_LightSource["<<i<<"].diffuse.rgb*diffuse*shadow;\n"
									"				result += attenuation * gl_LightSource["<<i<<"].ambient.rgb*ambient;\n"
									"			}\n"
									"		}\n"
									"	}\n";				
								break;
							}
						buffer << "	return vec4(result,1.0);\n";
					}
					else
						buffer << " return vec4(1.0);\n";
					buffer << "}\n\n";
				}
		
		
				if (spotlight_invoked)
				buffer <<
					"vec4 spotLight(int index, vec3 position, vec3 normal, vec3 reflected, float shadow)\n"
					"{\n"
					"	float fresnel = 1.0-abs(normal.z)*0.5;\n"
					"	vec4 result = vec4(0.0);\n"
					"	vec3 ldir = (gl_LightSource[index].position.xyz-position);\n"
					"	float general_intensity = -dot(gl_LightSource[index].spotDirection,ldir);\n"
					"	if (general_intensity > 0.0)\n"
					"	{\n"
					"		float diffuse = dot(normal,ldir);\n"
					"		if (diffuse > 0.0)\n"
					"		{\n"
					"			float distance = length(ldir);\n"
					"			ldir /= distance;\n"
					"			general_intensity /= distance;\n"
					"			diffuse /= distance;\n"
					"			float attenuation = 1.0 / (gl_LightSource[index].constantAttenuation + gl_LightSource[index].linearAttenuation * distance + gl_LightSource[index].quadraticAttenuation * distance*distance);\n"
					"			float cone_intensity = pow(general_intensity,gl_LightSource[index].spotExponent)*attenuation;\n"
					"			diffuse *= cone_intensity;\n"
					"			if (gl_FrontMaterial.shininess > 0.0)\n"
					"			{\n"
					"				float specular = pow(max(dot(reflected,ldir),0.0),gl_FrontMaterial.shininess*fresnel)*cone_intensity*fresnel*shadow;\n"
					"				result += specular * gl_FrontLightProduct[index].specular;\n"
					"			}\n"
					"			result += diffuse * gl_FrontLightProduct[index].diffuse*shadow;\n"
					"			result += attenuation * gl_FrontLightProduct[index].ambient;\n"
					"		}\n"
					"	}\n"
					"	return result;\n"
					"}\n\n";				

				if (directlight_invoked)
				buffer <<
					"vec4 directLight(int index, vec3 position, vec3 normal, vec3 reflected, float shadow)\n"
					"{\n"
					"	float fresnel = 1.0-abs(normal.z)*0.5;\n"
					"	vec4 result = vec4(0.0);\n"
					"	vec3 ldir = gl_LightSource[index].position.xyz;\n"
					"	result += max(dot(normal,ldir),0.0)*gl_FrontLightProduct[index].diffuse*shadow;\n"
					"	if (gl_FrontMaterial.shininess > 0.0)\n"
					"		result += pow(max(dot(reflected,ldir),0.0),gl_FrontMaterial.shininess*fresnel)*gl_FrontLightProduct[index].specular*fresnel*shadow;\n"
					"	result += gl_FrontLightProduct[index].ambient;\n"
					"	return result;\n"
					"}\n\n";				

				if (omnilight_invoked)
				buffer <<
					"vec4 omniLight(int index, vec3 position, vec3 normal, vec3 reflected, float shadow)\n"
					"{\n"
					"	float fresnel = 1.0-abs(normal.z)*0.5;\n"
					"	vec4 result = vec4(0.0);\n"
					"	vec3 ldir = (gl_LightSource[index].position.xyz-position);\n"
					"	float distance = length(ldir);\n"
					"	ldir /= distance;\n"
					"	float attenuation = 1.0 / (gl_LightSource[index].constantAttenuation + gl_LightSource[index].linearAttenuation * distance + gl_LightSource[index].quadraticAttenuation * distance*distance);\n"
					"	result += max(dot(normal,ldir),0.0)*gl_FrontLightProduct[index].diffuse*attenuation*shadow;\n"
					"	if (gl_FrontMaterial.shininess > 0.0)\n"
					"		result += pow(max(dot(reflected,ldir),0.0),gl_FrontMaterial.shininess*fresnel)*gl_FrontLightProduct[index].specular*attenuation*fresnel*shadow;\n"
					"	result += gl_FrontLightProduct[index].ambient*attenuation;\n"
					"	return result;\n"
					"}\n\n";				

	
	
				Block::assemble(buffer,user_config,render_config,Light::None,0);
			}
	
	
	
	
			Template::VariableMap::VariableMap():changed(true)
			{
				clear();
			}
			
			void					Template::VariableMap::clear()
			{
				variable_map.clear();
				variable_map.set("fog",FogVariableIndex);
				variable_map.set("lighting",LightingVariableIndex);
				changed = true;
			}

			Template::VariableMap::~VariableMap()
			{
				if (!application_shutting_down)
				{
					for (index_t i = 0; i < attached_configurations.count(); i++)
						attached_configurations[i]->signalMapDestruction();
				}

			}


	
	
	
	/*
			void		Configuration::toArray(Array<int>&target)	const
			{
				target.resize(lights.fillLevel()+values.count());
				for (unsigned i = 0; i < lights.fillLevel(); i++)
					target[i] = lights[i];
				for (unsigned i = 0; i < values.count(); i++)
					target[i+lights.fillLevel()] = values[i];
			}*/
	
			void		Template::RenderConfiguration::setLights(count_t count,...)
			{
				va_list vl;
				va_start(vl,count);
				lights.reset();
				for (index_t i = 0; i < count; i++)
				{
					int val=va_arg(vl,int);
					lights << (Light::Type)val;
				}
				va_end(vl);
				signalHasChanged();
			}
	
			void		Template::RenderConfiguration::setLighting(bool lighting)
			{
				if (lighting_enabled != lighting)
				{
					lighting_enabled = lighting;
					signalHasChanged();
				}
			}


			Template::RenderConfiguration::RenderConfiguration():lighting_enabled(false),fog_enabled(false)
			{}
	
			Template::UserConfiguration::UserConfiguration():map(NULL)
			{
				reset();
			}

			void Template::RenderConfiguration::clear()
			{
				if (lights.count()!=0 || lighting_enabled || fog_enabled)
				{
					lights.reset();
					lighting_enabled = false;
					fog_enabled = false;
					signalHasChanged();
				}
			}

	
			void	Template::UserConfiguration::clear()
			{
				if (map)
					map->unreg(this);
				map = NULL;
				if (values.count() != 2 || values[0] != 0 || values[1] != 0)
				{
					values.setSize(2);
					values[0] = 0;
					values[1] = 0;
					signalHasChanged();
				}
			}

			void		Template::UserConfiguration::signalMapDestruction()
			{
				map = NULL;
			}

	
			void		Template::RenderConfiguration::redetect()
			{
				lights.reset();
				lighting_enabled = glIsEnabled(GL_LIGHTING)!=0;
				fog_enabled = glIsEnabled(GL_FOG)!=0;
				if (lighting_enabled)
					for (GLint i = 0; i < gl_extensions.max_lights; i++)
					{
						GLint index = GL_LIGHT0+i;
						if (glIsEnabled(index))
						{
							float position[4];
							glGetLightfv(index,GL_POSITION,position);
							if (nearingZero(position[3]))
								lights << Light::Direct;
							else
							{
								float cutoff;
								glGetLightfv(index,GL_SPOT_CUTOFF,&cutoff);
								if (cutoff < 180-getError<float>())
									lights << Light::Spot;
								else
									lights << Light::Omni;
							}
						}
						else
							lights << Light::None;
					}
				signalHasChanged();
			}

			void	Template::UserConfiguration::updateRenderVariables(const RenderConfiguration&config)
			{
				if (values.isEmpty())
					return;
				if (values[0] != (int)config.fog_enabled || (values.count() > 1 && values[1] != (int)config.lighting_enabled))
				{
					values[FogVariableIndex-1] = config.fog_enabled;
					if (values.count() > 1)
						values[LightingVariableIndex-1] = config.lighting_enabled;
					signalHasChanged();
				}
			}

			void	Template::UserConfiguration::updateRenderVariables()
			{
				updateRenderVariables(global_render_config);
			}

			void	Template::VariableMap::submitChanges()
			{
				if (changed)
				{
					for (index_t i = 0; i < attached_configurations; i++)
						attached_configurations[i]->adapt();
					changed = false;
				}
			}
	
			index_t	Template::VariableMap::locate(const String&var_name)	const
			{
				index_t result;
				if (variable_map.query(var_name,result))
					return result;
				return 0;
			}
	
			index_t	Template::VariableMap::define(const String&variable_name)
			{
				index_t result;
				if (variable_map.query(variable_name,result))
					return result;
				result = variable_map.count()+1;
				variable_map.set(variable_name,result);
				changed = true;
				return result;
			}

	
			bool		Template::UserConfiguration::set(index_t variable, int value)
			{
				index_t index = variable-1;
				if (index >= values.count())
					return false;
				if (values[index] != value)
				{
					values[index] = value;
					signalHasChanged();
				}
				return true;
			}
		
			bool		Template::UserConfiguration::setByName(const String&variable_name, int value)
			{
				if (!map)
					return false;
				index_t index = map->locate(variable_name)-1;
				if (index >= values.count())
					return false;
				if (values[index] != value)
				{
					values[index] = value;
					signalHasChanged();
				}
				return true;
			}
	
			int			Template::UserConfiguration::value(index_t variable)	const
			{
				index_t index = variable-1;
				if (index >= values.count())
					return 0;
				return values[index];
			}

			int			Template::UserConfiguration::valueByName(const String&variable_name)	const
			{
				if (!map)
					return 0;
				index_t index = map->locate(variable_name)-1;
				if (index >= values.count())
					return 0;
				return values[index];
			}
	

	
			void		Template::UserConfiguration::reset()
			{
				values.fill(0);
			}



			Template::UserConfiguration::~UserConfiguration()
			{
				if (!application_shutting_down && map)
					map->unreg(this);
			}


			Template::VariableMap*	Template::UserConfiguration::variableMap()
			{
				return map;
			}
		
			const Template::VariableMap*		Template::UserConfiguration::variableMap() const								//!< Retrieves the map currently associated with this configuration, if any @return variable map or NULL if no such exists
			{
				return map;
			}

			void					Template::UserConfiguration::adapt()
			{
				if (!map)
				{
					clear();
					return;
				}

				count_t old = values.count();
				values.resizePreserveContent(map->variable_map.count());
				for (index_t i = old; i < values.count(); i++)
					values[i] = 0;
				if (old != values.count())
					signalHasChanged();
			}

			void					Template::UserConfiguration::link(VariableMap*new_map, bool adapt)
			{
				if (!this)
					return;
				if (!new_map)
				{
					clear();
					return;
				}
				if (map == new_map)
				{
					if (adapt)
					{
						count_t old = values.count();
						values.resizePreserveContent(map->variable_map.count());
						for (index_t i = old; i < values.count(); i++)
							values[i] = 0;
						if (old != values.count())
							signalHasChanged();
					}

					return;
				}
				if (map)
					map->unreg(this);
				map = new_map;
				if (map)
					map->reg(this);
				values.setSize(map->variable_map.count());
				values.fill(0);

				signalHasChanged();
			}

	
			static HashContainer<String>	global_shader_includables;



			Template::ConfigurationComponent::~ConfigurationComponent()
			{
				if (!application_shutting_down)
					for (index_t i = 0; i < linked_configs; i++)
						linked_configs[i]->signalComponentDestroyed(this);
			}

			void	Template::ConfigurationComponent::unreg(Configuration*config)
			{
				linked_configs.drop(config);
			}

			void	Template::ConfigurationComponent::reg(Configuration*config)
			{
				linked_configs.append(config);
			}

			void	Template::ConfigurationComponent::signalHasChanged()
			{
				version++;
			}


			bool	Template::Configuration::requiresUpdate()	const
			{
				return structure_changed || (user_config!=NULL && user_config_version != user_config->version) || (render_config!=NULL && render_config_version != render_config->version);
			}

			bool	Template::Configuration::update()
			{

				if (valid && requiresUpdate())
				{
					if (!registered)
					{
						user_config->reg(this);
						render_config->reg(this);
						registered = true;
					}
					setSize(render_config->lights.fillLevel()+user_config->values.count());
					for (unsigned i = 0; i < render_config->lights.fillLevel(); i++)
						data[i] = render_config->lights[i];
					for (unsigned i = 0; i < user_config->values.count(); i++)
						data[i+render_config->lights.fillLevel()] = user_config->values[i];
					structure_changed = false;
					user_config_version = user_config->version;
					render_config_version = render_config->version;
					return true;
				}
				return false;

			}

			void	Template::Configuration::signalComponentDestroyed(ConfigurationComponent*component)
			{
				if (component == user_config)
					user_config = NULL;
				elif (component == render_config)
					render_config = NULL;
				unlink();
			}

			Template::Configuration::~Configuration()
			{
				if (!application_shutting_down)
					unlink();
			}

			void Template::Configuration::unlink()
			{
				if (user_config)
				{
					if (registered)
						user_config->unreg(this);
					user_config = NULL;
				}
				if (render_config)
				{
					if (registered)
						render_config->unreg(this);
					render_config = NULL;
				}
				valid = false;
				structure_changed = true;
			}

			void	Template::Configuration::link(RenderConfiguration&new_render_config, UserConfiguration&new_user_config)
			{
				if (&new_render_config == render_config && &new_user_config == user_config)
					return;
				unlink();
				user_config = &new_user_config;
				render_config = &new_render_config;
				user_config->reg(this);
				render_config->reg(this);
				valid = true;
				structure_changed = true;
				user_config_version = user_config->version;
				render_config_version = render_config->version;
				registered = true;
			}

			void	Template::Configuration::link(RenderConfiguration&new_render_config)
			{
				if (render_config == &new_render_config)
					return;
				if (registered && render_config!=NULL)
					render_config->unreg(this);
				render_config = &new_render_config;
				if (registered)
					render_config->reg(this);
				valid = user_config != NULL;
				render_config_version = render_config->version;
				structure_changed = true;
			}

			void	Template::Configuration::link(UserConfiguration&new_user_config)
			{
				if (user_config == &new_user_config)
					return;
				if (registered && user_config!=NULL)
					user_config->unreg(this);
				user_config = &new_user_config;
				if (registered)
					user_config->reg(this);
				valid = render_config != NULL;
				user_config_version = user_config->version;
				structure_changed = true;
			}

				
			/*static*/ String*		Template::locateShaderIncludable(const String&filename)
			{
				String*result = global_shader_includables.lookup(filename);
				if (result)
					return result;
				static StringBuffer	dummy;
				dummy.reset();
				String content;
				if (!extractFileContent(filename,content,dummy))
					return NULL;
				result = global_shader_includables.define(filename);
				(*result) = content;
				return result;
			}
	
			/*static*/ void		Template::defineShaderIncludable(const String&filename, const String&block_code)
			{
				(*global_shader_includables.define(filename)) = block_code;
			}
	
			void				Template::clear()
			{
				local_map.clear();
				local_user_config.link(current_map);
				uses_lighting = false;
				

				shared_template.clear();
				vertex_template.clear();
				fragment_template.clear();
				geometry_template.clear();
				container.clear();
				loaded_ = false;
				uniform_init.reset();
			}
	

	
			Template&				Template::predefineUniformi(const String&name, int value)
			{
				TUniformInit&init = uniform_init.append();
				init.name = name;
				init.type = TUniformInit::Int;
				init.ival = value;
				return *this;
			}
	
			Template&				Template::predefineUniformf(const String&name, float value)
			{
				TUniformInit&init = uniform_init.append();
				init.name = name;
				init.type = TUniformInit::Float;
				init.fval = value;
				return *this;
			}

			Template&				Template::predefineUniform3f(const String&name, const float value[3])
			{
				TUniformInit&init = uniform_init.append();
				init.name = name;
				init.type = TUniformInit::Float3;
				init.f3val = Vec::ref3(value);
				return *this;
			}

			void				Template::loadRequired(const Composition&composition, GLenum geometry_type_, GLenum output_type_, unsigned max_vertices_)
			{
				if (!load(composition,geometry_type_,output_type_,max_vertices_))
					FATAL__(report());
			}
	
			bool				Template::load(const Composition&composition, GLenum geometry_type_, GLenum output_type_, unsigned max_vertices_)
			{
				clear();
				loaded_ = true;
				index_t line = 0;
				if (loaded_)
				{
					log << "-------- scanning shared source --------"<<nl;
					loaded_ = shared_template.scan(composition.sharedSource,*current_map,log,line);
					if (!loaded_)
						log << "  failed.\n";
				}
				if (loaded_)
				{
					log << "-------- scanning vertex shader --------"<<nl;
					loaded_ = vertex_template.scan(composition.vertexSource,*current_map,log,line);
					if (!loaded_)
						log << "  failed.\n";
				}
				if (loaded_)
				{
					log << "-------- scanning fragment shader --------"<<nl;
					loaded_ = fragment_template.scan(composition.fragmentSource,*current_map,log,line);
					if (!loaded_)
						log << "  failed.\n";
				}
				if (loaded_)
				{
					log << "-------- scanning geometry shader --------"<<nl;
					loaded_ = geometry_template.scan(composition.geometrySource,*current_map,log,line);
					if (!loaded_)
						log << "  failed.\n";
				}
				current_config->userConfig()->link(current_map,false);
				current_map->submitChanges();	//this indirectly triggers an adapt() call. since the above link() call makes sure that our user config is linked to current_map, this will update our user config as well (just as any other linked user config)
				geometry_type = geometry_type_;
				output_type = output_type_;
				max_vertices = max_vertices_;

				uses_lighting = shared_template.usesLighting() || vertex_template.usesLighting() || fragment_template.usesLighting() || geometry_template.usesLighting();
				if (loaded_)
					log << "  loading process finished gracefully.\n";
				return loaded_;
			}
	
			void				Template::loadRequiredComposition(const String&object_source, GLenum geometry_type_, GLenum output_type_, unsigned max_vertices_)
			{
				if (!loadComposition(object_source,geometry_type_,output_type_,max_vertices_))
					FATAL__(report());
			}

			bool				Template::loadComposition(const String&object_source, GLenum geometry_type_, GLenum output_type_, unsigned max_vertices_)
			{
				return load(Composition().Load(object_source),geometry_type_,output_type_,max_vertices_);
			}

			void		Template::setConfig(UserConfig*config, bool update)
			{
				if (config)
					current_config->link(*config);
				else
					current_config->link(local_user_config);
				if (update)
					current_config->userConfig()->link(current_map);
			}

			void		Template::setConfig(RenderConfig*config)
			{
				if (config)
					current_config->link(*config);
				else
					current_config->link(local_user_config);
			}

			void		Template::setConfig(Configuration*config)
			{
				if (config)
					current_config = config;
				else
					current_config = &local_config;
			}


			void		Template::setMap(VariableMap*new_map, bool adjust)
			{
				if (new_map)
					current_map = new_map;
				else
					current_map = &local_map;
				if (adjust)
					current_config->userConfig()->link(current_map);
			}


	
	
	
			String		Template::report()
			{
				String rs	= log.toString();
				log.reset();
				return rs;
			}
	
			bool		Template::loaded()	const
			{
				return this!=NULL && loaded_;
			}
	
	
			GLShader::Instance*	Template::buildShader(bool auto_update_render_variables,bool*is_new)
			{
				if (!this)
					return NULL;
				if (auto_update_render_variables /*&& uses_lighting*/)
					current_config->updateRenderVariables();
				log.reset();
				if (is_new)
					*is_new = false;
				GLShader::Instance*result;

				const Array<int,Primitive>*key;
				if (uses_lighting)
				{
					current_config->update();
					key = current_config;
				}
				else
					key = &current_config->userConfig()->values;

				if (container.query(*key,result))
				{
					//cout << result->composition_code<<endl;
					return result;
				}
				GLShader::Instance	local;
				Composition	composition;

				const RenderConfig&render_config = *current_config->renderConfig();
				const UserConfig&user_config = *current_config->userConfig();

				composition.sharedSource = shared_template.assemble(render_config, user_config,true);
				composition.vertexSource = vertex_template.assemble(render_config, user_config,false);
				composition.fragmentSource = fragment_template.assemble(render_config, user_config,false);
				composition.geometrySource = geometry_template.assemble(render_config, user_config,false);
				if (local.load(composition,geometry_type,output_type,max_vertices))
				{
					if (is_new)
						*is_new = true;
					result = container.define(*key);
					result->adoptData(local);
					log << local.report();
			
					GLuint current_program = glGetHandle(GL_PROGRAM_OBJECT_ARB);
					glUseProgramObject(result->program_handle);
			
					for (index_t i = 0; i < uniform_init.fillLevel(); i++)
					{
			
						GLint var	= glGetUniformLocation(result->program_handle,uniform_init[i].name.c_str());
						if (var == -1)
							continue;

						switch (uniform_init[i].type)
						{
							case TUniformInit::Int:
								glUniform1i(var,uniform_init[i].ival);
							break;
							case TUniformInit::Float:
								glUniform1f(var,uniform_init[i].fval);
							break;
							case TUniformInit::Float3:
								glUniform3fv(var,1,uniform_init[i].f3val.v);
							break;
						}
					}
					foreach (shadow_attachments,attachment)
						foreach (attachment->sampler_assignments, a)
						{
							GLint var	= glGetUniformLocation(result->program_handle,a->sampler_name.c_str());
							glUniform1i(var,a->sampler_level);
						}
					glUseProgramObject(current_program);
					return result;
				}
				log << local.report();
				return NULL;
			}
	
			String				Template::assembleSource(bool auto_update_render_variables)
			{
				if (auto_update_render_variables && uses_lighting)
					current_config->updateRenderVariables();

				const RenderConfig&render_config = *current_config->renderConfig();
				const UserConfig&user_config = *current_config->userConfig();

				static StringBuffer	out_buffer;
				out_buffer.reset();
				out_buffer << "[shared]";
				shared_template.assemble(render_config, user_config,out_buffer,true);
				out_buffer << "[vertex]";
				vertex_template.assemble(render_config, user_config,out_buffer);
				out_buffer << nl<<"[geometry]";
				geometry_template.assemble(render_config, user_config,out_buffer);
				out_buffer << nl<<"[fragment]";
				fragment_template.assemble(render_config, user_config,out_buffer);
				return out_buffer.toString();
			}

		
	}
	
	
	
	
	/*static*/	Extension::ResolutionTable	Extension::depth_buffer_table;

	/*static*/ GLuint		Extension::allocateDepthBuffer(const Resolution&res)
	{
		GLuint	result;
		if (!depth_buffer_table.query(res,result))
		{
			glGenRenderbuffers( 1, &result );
			glBindRenderbuffer( GL_RENDERBUFFER, result );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, res.width, res.height );
			depth_buffer_table.set(res,result);
		}
		return result;
	}

	
	
	
	
	
	
	
	
	
	
	#endif



	

	/*static*/	TFrameBuffer	Extension::createFrameBuffer(const Resolution&res, DepthStorage depth_storage, BYTE num_color_targets, const GLenum*format)
	{
		glGetError();
		TFrameBuffer result;
		result.depth_target.storage_type = depth_storage;
		result.num_color_targets = num_color_targets;
		ASSERT_LESS__(num_color_targets,ARRAYSIZE(result.color_target));
		if (depth_storage != DepthStorage::Texture)
			ASSERT_GREATER__(num_color_targets,0);
		for (BYTE k = 0; k < num_color_targets; k++)
			result.color_target[k].texture_handle = 0;
		result.resolution = res;
		#ifdef GL_ARB_framebuffer_object
			GLenum status;
			
			if (glGenFramebuffers)
			{
					glGenFramebuffers( 1, &result.frame_buffer );

					// Initialize the render-buffer for usage as a depth buffer.
					// We don't really need this to render things into the frame-buffer object,
					// but without it the geometry will not be sorted properly.

					switch (depth_storage)
					{
						case DepthStorage::SharedBuffer:
							result.depth_target.handle = allocateDepthBuffer(res);
						break;
						case DepthStorage::PrivateBuffer:
							glGenRenderbuffers( 1, &result.depth_target.handle );
							glBindRenderbuffer( GL_RENDERBUFFER, result.depth_target.handle );
							glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, res.width, res.height );
						break;
						case DepthStorage::Texture:
						{
							glGenTextures( 1, &result.depth_target.handle );

							glBindTexture( GL_TEXTURE_2D, result.depth_target.handle );
							glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
							glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );

							glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
											  res.width, res.height, 
											  0, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );

							glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
							glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
							glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
							glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
							glBindTexture( GL_TEXTURE_2D, 0);
						}
						break;
						case DepthStorage::NoDepthStencil:
						break;
					}
					
					ASSERT__(depth_storage == DepthStorage::NoDepthStencil || result.depth_target.handle != 0);

					glBindFramebuffer(GL_FRAMEBUFFER, result.frame_buffer);
					GLenum	draw_buffers[GL_MAX_COLOR_ATTACHMENTS];
					for (BYTE k = 0; k < num_color_targets; k++)
					{
						draw_buffers[k] = GL_COLOR_ATTACHMENT0+k;
					}
					if (!num_color_targets)
						glDrawBuffers(0,draw_buffers);
					else
						glDrawBuffers(num_color_targets,draw_buffers);
					//glReadBuffer(GL_NONE);
					
					if (depth_storage != DepthStorage::NoDepthStencil)
						if (depth_storage != DepthStorage::Texture)
							glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, result.depth_target.handle );
						else
							glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, result.depth_target.handle, 0 );						


					for (BYTE k = 0; k < num_color_targets; k++)
					{
						glGenTextures( 1, &result.color_target[k].texture_handle );

						glBindTexture( GL_TEXTURE_2D, result.color_target[k].texture_handle );
						result.color_target[k].texture_format = format[k];

						glTexImage2D( GL_TEXTURE_2D, 0, format[k], 
									  res.width, res.height, 
									  0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );

						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
					
						glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+k, GL_TEXTURE_2D, result.color_target[k].texture_handle, 0 );
						glBindTexture( GL_TEXTURE_2D, 0);
	
					}

					
					status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
					if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
					{
						const char* error = "Undescribed and/or unsupported error";
						switch (status)
						{
							case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
								error = "Not all framebuffer attachment points are framebuffer attachment complete.";
							break;
							case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
								error = "Not all attached images have the same width and height.";
							break;
							case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
								error = "Draw buffer is incomplete.";
							break;
							case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
								error = "Read buffer is incomplete.";
							break;
							case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
								error = "No images are attached to the framebuffer.";
							break;
							case GL_FRAMEBUFFER_UNSUPPORTED:
								error = "The combination of internal formats of the attached images violates an implementation-dependent set of restrictions.";
							break;
                    
							default:
							break;
						}
						FATAL__("Frame buffer construction failed: OpenGL reports: \""+String(error)+"\"");
						destroyFrameBuffer(result);
						return result;
					}					
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
					
			}
		#endif
		return result;
	}
	
	/*static*/	bool			Extension::copyFrameBuffer(const TFrameBuffer&from, const TFrameBuffer&to, const Resolution&res, bool copy_color /*= true*/, bool copy_depth/*=true*/)
	{
		if (!copy_color && !copy_depth)
			return true;
		GLenum mask = 0;
		if (copy_color)
			mask |= GL_COLOR_BUFFER_BIT;
		if (copy_depth)
			mask |= GL_DEPTH_BUFFER_BIT;
		if (!from.frame_buffer || !to.frame_buffer)
			return false;
		glGetError();
        glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, from.frame_buffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, to.frame_buffer);
        glBlitFramebuffer(0, 0, res.width, res.height,
                          0, 0, res.width, res.height,
                          mask,
                          GL_NEAREST);

        glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, 0);
		return glGetError() == GL_NO_ERROR;
	}

	/*static*/	bool			Extension::copyFrameBuffer(const TFrameBuffer&from, const TFrameBuffer&to, bool copy_color, bool copy_depth)
	{
		if (from.resolution != to.resolution)
			return false;
		return copyFrameBuffer(from,to,from.resolution,copy_color,copy_depth);
	}
	
	
	
	bool			Extension::resizeFrameBuffer(TFrameBuffer&buffer,const Resolution&res)
	{
		if (buffer.resolution == res)
			return true;
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame_buffer);
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );
			
		for (BYTE k = 0; k < buffer.num_color_targets; k++)
		{
			if (!buffer.color_target[k].texture_handle)
				glGenTextures( 1, &buffer.color_target[k].texture_handle );
		
			glBindTexture(GL_TEXTURE_2D,buffer.color_target[k].texture_handle);
			glTexImage2D( GL_TEXTURE_2D, 0, buffer.color_target[k].texture_format,
				res.width, res.height, 
				0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
			glBindTexture(GL_TEXTURE_2D,0);
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+k, GL_TEXTURE_2D, buffer.color_target[k].texture_handle, 0 );
		}

		switch (buffer.depth_target.storage_type)
		{
			case DepthStorage::SharedBuffer:
				buffer.depth_target.handle = allocateDepthBuffer(res);
				glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer.depth_target.handle );
			break;
			case DepthStorage::PrivateBuffer:
				if (!buffer.depth_target.handle)
					glGenRenderbuffers( 1, &buffer.depth_target.handle );
				
				glBindRenderbuffer( GL_RENDERBUFFER, buffer.depth_target.handle );
				glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, res.width, res.height );
			
				glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer.depth_target.handle );
			break;
			case DepthStorage::Texture:
				glBindTexture(GL_TEXTURE_2D,buffer.depth_target.handle);
				glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
					res.width, res.height, 
					0, GL_LUMINANCE, GL_FLOAT, 0 );
				glBindTexture(GL_TEXTURE_2D,0);
				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer.depth_target.handle, 0 );
			break;
			case DepthStorage::NoDepthStencil:
			break;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		buffer.resolution = res;
		
		
		//destroyFrameBuffer(buffer);
		//buffer = createFrameBuffer(width,height,buffer.alpha);
		return true;
	}


	/*static*/	bool			Extension::formatHasAlpha(GLenum format)
	{
		switch (format)
		{
			case GL_ALPHA8:
			case GL_LUMINANCE8_ALPHA8:
			case GL_RGBA8:
			case GL_ALPHA16:
			case GL_LUMINANCE16_ALPHA16:
			case GL_RGBA16:
			case GL_ALPHA32I_EXT:
			case GL_RGBA32I:
			case GL_ALPHA16F_ARB:
			case GL_ALPHA32F_ARB:
			case GL_LUMINANCE_ALPHA16F_ARB:
			case GL_LUMINANCE_ALPHA32F_ARB:
			case GL_RGBA16F_ARB:
			case GL_RGBA32F_ARB:
				return true;
			default:
				return false;
		};
	}

	
	
	bool			Extension::bindFrameBuffer(const TFrameBuffer&buffer)
	{
		if (!buffer.frame_buffer)
			return false;
		#ifdef GL_ARB_framebuffer_object
			if (glBindFramebuffer)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame_buffer);
				glViewport(0,0,buffer.resolution.width,buffer.resolution.height);
				//glBindRenderbufferEXT( GL_RENDERBUFFER, g_depthRenderBuffer );
				//glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.texture, 0 );
				//glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer.depth_buffer );
				
				
				return true;
			}
			else
				return false;
		#endif
		return false;
	}
	
	/*static*/ void			Extension::unbindFrameBuffer()
	{
		#ifdef GL_ARB_framebuffer_object
			if (glBindFramebuffer)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		#endif
	}
	
	/*static*/ void			Extension::destroyFrameBuffer(const TFrameBuffer&buffer)
	{
		#ifdef GL_ARB_framebuffer_object
			if (glDeleteFramebuffers)
			{
				if (buffer.frame_buffer)
					glDeleteFramebuffers(1, &buffer.frame_buffer);

				if (buffer.depth_target.storage_type == DepthStorage::PrivateBuffer && buffer.depth_target.handle)
					glDeleteRenderbuffers(1, &buffer.depth_target.handle);
			}
		#endif
		for (BYTE k = 0; k < buffer.num_color_targets; k++)
			if (buffer.color_target[k].texture_handle)
				glDeleteTextures(1,&buffer.color_target[k].texture_handle);
		if (buffer.depth_target.storage_type == DepthStorage::Texture && buffer.depth_target.handle)
			glDeleteTextures(1,&buffer.depth_target.handle);
	}

	
	
	#ifdef WGL_ARB_pbuffer

	TPBuffer Extension::createPBuffer(unsigned width, unsigned height, ePBufferType type, BYTE fsaa)
	{

		TPBuffer	result;
		result.enabled	= false;
		if (!wglChoosePixelFormat||!wglQueryPbuffer)
			return result;
		int bind_type(0),
			alpha_bits(0);

		switch (type)
		{
			#ifdef WGL_DEPTH_TEXTURE_FORMAT_NV
			case PBT_DEPTH:
				bind_type	= WGL_BIND_TO_TEXTURE_DEPTH_NV;
				alpha_bits	= 0;
			break;
			#endif
			case PBT_RGB_CUBE:
			case PBT_RGB:
				bind_type	= WGL_BIND_TO_TEXTURE_RGB_ARB;
				alpha_bits	= 0;
			break;
			case PBT_RGBA_CUBE:
			case PBT_RGBA:
				bind_type	= WGL_BIND_TO_TEXTURE_RGBA_ARB;
				alpha_bits	= 8;
			break;
		}


		int attribs[] = {
				WGL_DRAW_TO_PBUFFER_ARB, GL_TRUE,
				bind_type, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_FALSE,
				WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
				WGL_STEREO_ARB, GL_FALSE,
				WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
				WGL_RED_BITS_ARB, 8,
				WGL_GREEN_BITS_ARB, 8,
				WGL_BLUE_BITS_ARB, 8,
				WGL_ALPHA_BITS_ARB, alpha_bits,
				WGL_DEPTH_BITS_ARB, 16,
				WGL_STENCIL_BITS_ARB, 8,
				fsaa?WGL_SAMPLES_ARB:0, fsaa, //zero terminate here if no samples set
				WGL_SAMPLE_BUFFERS_ARB,fsaa?GL_TRUE:GL_FALSE,
				0,0
			};
		GLuint PixelFormat;
		unsigned nof;
		result.enabled	= wglChoosePixelFormat(hDC,attribs,NULL,1,(int*)&PixelFormat,&nof) && nof;	//			<<<<<<<<<<<<<<<<<<<<<
		if (!result.enabled && fsaa)
		{
			attribs[23] = 0;
			result.enabled	= wglChoosePixelFormat(hDC,attribs,NULL,1,(int*)&PixelFormat,&nof) && nof;
		}
		if (!result.enabled)
			return result;

		int pattribs[] = {
				WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_RGBA_ARB,
				WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_2D_ARB,
				0,0
			};
		switch (type)
		{
			case PBT_RGBA_CUBE:
				pattribs[3] = WGL_TEXTURE_CUBE_MAP_ARB;
			break;
			case PBT_RGB_CUBE:
				pattribs[3] = WGL_TEXTURE_CUBE_MAP_ARB;
			case PBT_RGB:
				pattribs[1] = WGL_TEXTURE_RGB_ARB;
			break;
			#ifdef WGL_DEPTH_TEXTURE_FORMAT_NV
			case PBT_DEPTH:
				pattribs[1] = WGL_DEPTH_TEXTURE_FORMAT_NV;
			break;
			#endif
			default:
			break;
		}
		result.buffer	= wglCreatePbuffer(hDC, PixelFormat, width, height, pattribs);
		result.device_context	= wglGetPbufferDC(result.buffer);
		result.buffer_context	= wglCreateContext(result.device_context);
		wglQueryPbuffer(result.buffer,WGL_PBUFFER_WIDTH_ARB,&result.width);
		wglQueryPbuffer(result.buffer,WGL_PBUFFER_HEIGHT_ARB,&result.height);
		
		wglShareLists(wglGetCurrentContext(),result.buffer_context);

		glGenTextures(1, &result.texture);
		glBindTexture(GL_TEXTURE_2D, result.texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		return result;
	}

	void	Extension::destroyPBuffer(const TPBuffer&buffer)
	{
		if (!buffer.enabled)
			return;
		wglDeleteContext(buffer.buffer_context);
		wglReleasePbufferDC(buffer.buffer,buffer.device_context);
		wglDestroyPbuffer(buffer.buffer);
		glDeleteTextures(1,&buffer.texture);
	}

	#elif defined(GLX_VERSION_1_3)


	TPBuffer	Extension::createPBuffer(unsigned width, unsigned height, ePBufferType type, BYTE fsaa)
	{


		TPBuffer	result;
		result.enabled	= false;
		int bind_type,
			alpha_bits;

		switch (type)
		{
			#ifdef WGL_DEPTH_TEXTURE_FORMAT_NV
			case PBT_DEPTH:
				//bind_type	= WGL_BIND_TO_TEXTURE_DEPTH_NV;
				alpha_bits	= 0;
			break;
			#endif
			case PBT_RGB_CUBE:
			case PBT_RGB:
				//bind_type	= WGL_BIND_TO_TEXTURE_RGB_ARB;
				alpha_bits	= 0;
			break;
			case PBT_RGBA_CUBE:
			case PBT_RGBA:
				//bind_type	= WGL_BIND_TO_TEXTURE_RGBA_ARB;
				alpha_bits	= 8;
			break;
		}


		int attribs[] = {
			GLX_DOUBLEBUFFER,	False,
			GLX_STEREO,			False,
			GLX_RED_SIZE,		8,
			GLX_GREEN_SIZE,		8,
			GLX_BLUE_SIZE,		8,
			GLX_ALPHA_SIZE,		alpha_bits,
			GLX_DEPTH_SIZE,		16,
			GLX_STENCIL_SIZE,	8,
			GLX_RENDER_TYPE,	GLX_RGBA_BIT,
			0,					0};


		int pattribs[] = {
				GLX_PBUFFER_WIDTH, width,
				GLX_PBUFFER_HEIGHT, height,
				0,0
			};
		int nof;
		GLXFBConfig*config	= glXChooseFBConfig(display, screen,attribs,&nof);
		result.enabled	= config && nof;
		if (!result.enabled)
			return result;
		result.buffer	= glXCreatePbuffer(display, *config, pattribs);
		result.buffer_context	= glXCreateNewContext(display, *config, GLX_RGBA, glXGetCurrentContext(), True);
		result.display	= display;

		glGenTextures(1, &result.texture);
		glBindTexture(GL_TEXTURE_2D, result.texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		

		
		return result;
	}

	void		Extension::destroyPBuffer(const TPBuffer&buffer)
	{
		if (!buffer.enabled)
			return;
		glXDestroyContext(display,buffer.buffer_context);
		glXDestroyPbuffer(display,buffer.buffer);

	}
	#endif



	static bool resolveArray(GLenum main_, GLenum size_, GLenum stride_, GLenum type_, GLenum pntr_, String&result)
	{
		if (!glIsEnabled(main_))
		{
			result = "disabled";
			return false;
		}
		GLint	components,
				stride,
				type;
		GLvoid* pntr;

		if (size_)
			glGetIntegerv(size_,&components);
		else
			components	= 3;
		glGetIntegerv(stride_,&stride);
		glGetIntegerv(type_,&type);
		glGetPointerv(pntr_,&pntr);

		result = "(num: "+IntToStr(components)+"; stride: "+IntToStr(stride)+"; type: "+getTypeName(type)+"; pntr: 0x"+PointerToHex(pntr)+")";
		return true;
	}
	static String resolveArray(GLenum main_, GLenum size_, GLenum stride_, GLenum type_, GLenum pntr_)
	{
		String rs;
		resolveArray(main_,size_,stride_,type_,pntr_,rs);
		return rs;
	}
	
	static const char*	filterToString(GLint filter)
	{
		#undef ECASE
		#define ECASE(_CASE_)	case _CASE_: return #_CASE_;
		switch (filter)
		{
			ECASE(GL_LINEAR)
			ECASE(GL_NEAREST)
			ECASE(GL_NEAREST_MIPMAP_NEAREST)
			ECASE(GL_LINEAR_MIPMAP_NEAREST)
			ECASE(GL_NEAREST_MIPMAP_LINEAR)
			ECASE(GL_LINEAR_MIPMAP_LINEAR)
		}
		return "unknown";
	}
		
	static const char*	depthTextureModeToString(GLint filter)
	{
		#undef ECASE
		#define ECASE(_CASE_)	case _CASE_: return #_CASE_;
		switch (filter)
		{
			ECASE(GL_INTENSITY)
			ECASE(GL_ALPHA)
			ECASE(GL_LUMINANCE)
		}
		return "unknown";
	}		
	static const char*	textureCompareModeToString(GLint filter)
	{
		#undef ECASE
		#define ECASE(_CASE_)	case _CASE_: return #_CASE_;
		switch (filter)
		{
			ECASE(GL_COMPARE_R_TO_TEXTURE)
			ECASE(GL_NONE)
		}
		return "unknown";
	}
	
	static String	formatToString(GLint filter)
	{
		#undef ECASE
		#define ECASE(_CASE_)	case _CASE_: return #_CASE_;
		switch (filter)
		{
			case 1: return "GL_LUMINANCE8(1)";
			case 2: return "GL_LUMINANCE8_ALPHA8(2)";
			case 3: return "GL_RGB8(3)";
			case 4: return "GL_RGBA8(4)";
			ECASE(GL_R3_G3_B2)
			ECASE(GL_ALPHA4)
			ECASE(GL_ALPHA8)
			ECASE(GL_ALPHA12)
			ECASE(GL_ALPHA16)
			ECASE(GL_LUMINANCE4)
			ECASE(GL_LUMINANCE8)
			ECASE(GL_LUMINANCE12)
			ECASE(GL_LUMINANCE16)
			ECASE(GL_LUMINANCE4_ALPHA4)
			ECASE(GL_LUMINANCE6_ALPHA2)
			ECASE(GL_LUMINANCE8_ALPHA8)
			ECASE(GL_LUMINANCE12_ALPHA4)
			ECASE(GL_LUMINANCE12_ALPHA12)
			ECASE(GL_LUMINANCE16_ALPHA16)
			ECASE(GL_INTENSITY)
			ECASE(GL_INTENSITY4)
			ECASE(GL_INTENSITY8)
			ECASE(GL_INTENSITY12)
			ECASE(GL_INTENSITY16)
			ECASE(GL_RGB)
			ECASE(GL_RGB4)
			ECASE(GL_RGB5)
			ECASE(GL_RGB8)
			ECASE(GL_RGB10)
			ECASE(GL_RGB12)
			ECASE(GL_RGB16)
			ECASE(GL_RGBA)
			ECASE(GL_RGBA2)
			ECASE(GL_RGBA4)
			ECASE(GL_RGB5_A1)
			ECASE(GL_RGBA8)
			ECASE(GL_RGB10_A2)
			ECASE(GL_RGBA12)
			ECASE(GL_RGBA16)

			ECASE(GL_DEPTH_COMPONENT)
			ECASE(GL_DEPTH_COMPONENT16)
			ECASE(GL_DEPTH_COMPONENT24)
			ECASE(GL_DEPTH_COMPONENT32)

			ECASE(GL_RGBA16F)
			ECASE(GL_RGB16F)
			ECASE(GL_RGBA32F)
			ECASE(GL_RGB32F)
			ECASE(GL_R16F)
			ECASE(GL_R32F)
			ECASE(GL_RG16F)
			ECASE(GL_RG32F)
			ECASE(GL_ALPHA16F_ARB)
			ECASE(GL_INTENSITY16F_ARB)
			ECASE(GL_LUMINANCE16F_ARB)
			ECASE(GL_LUMINANCE_ALPHA16F_ARB)

			
		}
		return "unknown ("+String(filter)+")";
	}

	String	Extension::renderState()
	{
		#undef BSTR
		#define BSTR(b)		String(b?"enabled":"disabled")
		GLfloat	projection[16], modelview[16], raster_distance;
		TVec4<GLfloat>	raster, raster_color, raster_texcoords,color,coords;
		TVec3<GLfloat>	normal;

		GLuint		program=0;
		GLint		array_buffer_binding = 0,
					element_array_buffer_binding = 0,
					pixel_pack_buffer_binding = 0,
					pixel_unpack_buffer_binding = 0,
					active_texture = -1,
					client_active_texture = -1;
		GLboolean	raster_valid;
			glGetFloatv(GL_PROJECTION_MATRIX,projection);
			glGetFloatv(GL_MODELVIEW_MATRIX,modelview);
			glGetFloatv(GL_CURRENT_RASTER_POSITION,raster.v);
			glGetFloatv(GL_CURRENT_RASTER_COLOR,raster_color.v);
			glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID,&raster_valid);
			glGetFloatv(GL_CURRENT_RASTER_DISTANCE,&raster_distance);
			glGetFloatv(GL_CURRENT_RASTER_TEXTURE_COORDS,raster_texcoords.v);
			glGetFloatv(GL_CURRENT_COLOR,color.v);
			glGetFloatv(GL_CURRENT_NORMAL,normal.v);
		
		#ifdef GL_ACTIVE_TEXTURE_ARB
			glGetIntegerv(GL_ACTIVE_TEXTURE_ARB,&active_texture);
		#endif
		
		#ifdef GL_CLIENT_ACTIVE_TEXTURE_ARB
			glGetIntegerv(GL_CLIENT_ACTIVE_TEXTURE_ARB,&client_active_texture);
		#endif
			
			
		#ifdef GL_ARB_shader_objects
			if (glGetHandle)
				program = (GLuint)glGetHandle(GL_PROGRAM_OBJECT_ARB);
		#endif
		#ifdef GL_ARB_vertex_buffer_object
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING,&array_buffer_binding);
			glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,&element_array_buffer_binding);
			glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING,&pixel_pack_buffer_binding);
			glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING,&pixel_unpack_buffer_binding);
		#endif

		

		String rs	= "OpenGL state:\n\
  Projection:\n"+__toString(projection)+"\n\
  Modelview:\n"+__toString(modelview)+"\n\
  Program: "+String(program)+"\n\
  Active texture layer: "+String(active_texture!=-1?active_texture-GL_TEXTURE0:-1)+"\n\
  Client active texture layer: "+String(client_active_texture!=-1?client_active_texture-GL_TEXTURE0:-1)+"\n\
  Color: "+Vec::toString(color)+"\n\
  Normal: "+Vec::toString(normal)+"\n\
  RasterPos: "+Vec::toString(raster)+"\n\
  RasterColor: "+Vec::toString(raster_color)+"\n\
  RasterDistance: "+FloatToStr(raster_distance)+"\n\
  RasterCoords: "+Vec::toString(raster_texcoords)+"\n\
  RasterValid: "+(raster_valid?"true":"false")+"\n\
  AlphaTest: ";
  
  if (glIsEnabled(GL_ALPHA_TEST))
  {
	rs += "enabled (";
	GLfloat   alpha_ref;
    GLint     alpha_func;

    glGetFloatv(GL_ALPHA_TEST_REF,&alpha_ref);
    glGetIntegerv(GL_ALPHA_TEST_FUNC,&alpha_func);
	switch (alpha_func)
	{
		case GL_LESS:
			rs += "<";
		break;
		case GL_LEQUAL:
			rs += "<=";
		break;
		case GL_EQUAL:
			rs += "=";
		break;
		case GL_GREATER:
			rs += ">";
		break;
		case GL_GEQUAL:
			rs += ">=";
		break;
		case GL_NOTEQUAL:
			rs += "!=";
		break;
	}
	rs += " "+String(alpha_ref)+")";
  
  }
  else
	rs += "disabled";
  rs += "\n\
  DepthTest: ";
  if (glIsEnabled(GL_DEPTH_TEST))
  {
	  GLint val;
	  glGetIntegerv(GL_DEPTH_FUNC,&val);
	  switch (val)
	  {
		case GL_LESS:
			rs += "GL_LESS";
		break;
		case GL_LEQUAL:
			rs += "GL_LEQUAL";
		break;
		case GL_EQUAL:
			rs += "! GL_EQUAL !";
		break;
		case GL_GREATER:
			rs += "GL_GREATER";
		break;
		case GL_GEQUAL:
			rs += "GL_GEQUAL";
		break;
		case GL_NOTEQUAL:
			rs += "! GL_NOTEQUAL !";
		break;
	  }
  }
  else
	  rs += "Disabled\n";

  rs += "\n\
  FaceCull: "+BSTR(glIsEnabled(GL_CULL_FACE))+"\n\
  Lighting: "+BSTR(glIsEnabled(GL_LIGHTING))+"\n\
  Blend: "+BSTR(glIsEnabled(GL_BLEND))+"\n\
  Array Buffer Binding: "+String(array_buffer_binding)+"\n\
  Element Array Buffer Binding: "+String(element_array_buffer_binding)+"\n\
  Pixel Pack Buffer Binding: "+String(pixel_pack_buffer_binding)+"\n\
  Pixel Unpack Buffer Binding: "+String(pixel_unpack_buffer_binding)+"\n\
  VertexArray: "+resolveArray(GL_VERTEX_ARRAY,GL_VERTEX_ARRAY_SIZE,GL_VERTEX_ARRAY_STRIDE,GL_VERTEX_ARRAY_TYPE,GL_VERTEX_ARRAY_POINTER)+"\n\
  ColorArray: "+resolveArray(GL_COLOR_ARRAY,GL_COLOR_ARRAY_SIZE,GL_COLOR_ARRAY_STRIDE,GL_COLOR_ARRAY_TYPE,GL_COLOR_ARRAY_POINTER)+"\n\
  NormalArray: "+resolveArray(GL_NORMAL_ARRAY,0,GL_NORMAL_ARRAY_STRIDE,GL_NORMAL_ARRAY_TYPE,GL_NORMAL_ARRAY_POINTER)+"\n";
		if (glActiveTexture)
		{
			for (unsigned i	= 0; i < (unsigned)max_texcoord_layers; i++)
			{
				glActiveTexture(GL_TEXTURE0+i);
				glClientActiveTexture(GL_TEXTURE0+i);
				glGetFloatv(GL_TEXTURE_MATRIX,modelview);
				glGetFloatv(GL_CURRENT_TEXTURE_COORDS,coords.v);
				
				String texture,array,texture_info;
				bool has_array = resolveArray(GL_TEXTURE_COORD_ARRAY,GL_TEXTURE_COORD_ARRAY_SIZE,GL_TEXTURE_COORD_ARRAY_STRIDE,GL_TEXTURE_COORD_ARRAY_TYPE,GL_TEXTURE_COORD_ARRAY_POINTER,array);
				GLint	value;
				GLenum target,face_target;
				if (glIsEnabled(GL_TEXTURE_1D))
				{
					texture = "1D";
					glGetIntegerv(GL_TEXTURE_BINDING_1D,&value);
					texture += " ("+String(value)+")";
					face_target = target = GL_TEXTURE_1D;
				}
				elif (glIsEnabled(GL_TEXTURE_2D))
				{
					texture = "2D";
					glGetIntegerv(GL_TEXTURE_BINDING_2D,&value);
					texture += " ("+String(value)+")";
					face_target = target = GL_TEXTURE_2D;
				}
				elif (glIsEnabled(GL_TEXTURE_3D))
				{
					texture = "3D";
					glGetIntegerv(GL_TEXTURE_BINDING_3D,&value);
					texture += " ("+String(value)+")";
					face_target = target = GL_TEXTURE_3D;
				}
				elif (glIsEnabled(GL_TEXTURE_CUBE_MAP))
				{
					texture = "CUBE";
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP,&value);
					texture += " ("+String(value)+")";
					target = GL_TEXTURE_CUBE_MAP;
					face_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
				}
				else
				{
					texture = "disabled";
					if (!has_array)
						continue;
				}
				
				if (texture != "disabled")
				{
					{
						glGetTexParameteriv(target,GL_TEXTURE_MIN_FILTER,&value);
						texture_info += "   Min-Filter: "+String(filterToString(value))+"\n";
						glGetTexParameteriv(target,GL_TEXTURE_MAG_FILTER,&value);
						texture_info += "   Mag-Filter: "+String(filterToString(value))+"\n";
						glGetError();
						glGetTexLevelParameteriv(face_target,0,GL_TEXTURE_INTERNAL_FORMAT,&value);
						glThrowError();
						texture_info += "   Format: "+formatToString(value)+"\n";
						glGetTexLevelParameteriv(face_target,0,GL_TEXTURE_WIDTH,&value);
						texture_info += "   Width: "+String(value)+"\n";
						glGetTexLevelParameteriv(face_target,0,GL_TEXTURE_HEIGHT,&value);
						texture_info += "   Height: "+String(value)+"\n";
						glGetTexParameteriv(target,GL_TEXTURE_COMPARE_MODE,&value);
						texture_info += "   Compare mode: "+String(textureCompareModeToString(value))+"\n";
						glGetTexParameteriv(target,GL_DEPTH_TEXTURE_MODE,&value);
						texture_info += "   Depth texture mode: "+String(depthTextureModeToString(value))+"\n";
					}
					
				}
				
				
				rs += "Layer "+IntToStr(i)+"/"+String(max_texcoord_layers) +"\n";
				rs += "  Texture: "+texture+"\n"+texture_info;
				rs += "  CoordArray: "+array+"\n";
				rs += "  Coords: "+Vec::toString(coords)+"\n";
				rs += "  Matrix "+Vec::toString(Vec::ref4(modelview))+"; "+Vec::toString(Vec::ref4(modelview+4))+"; "+Vec::toString(Vec::ref4(modelview+8))+"; "+Vec::toString(Vec::ref4(modelview+12))+"\n";
			}
			glActiveTexture(active_texture);
			glClientActiveTexture(client_active_texture);
		}
		// glMatrixMode(GL_PROJECTION);
			// glLoadMatrixf(projection);
		// glMatrixMode(GL_MODELVIEW);
			// glLoadMatrixf(modelview);

		return rs;
		#undef BSTR
	}

	bool Extension::init(GLuint index)
	{
		#define set(group_name)	EXT_CONTEXT(group_name)
		#define get(var)			EXT_GET_EXTENSION(var)
		#define silent(var)		EXT_GET_EXTENSION_NO_CHECK(var)

		bool one_failed	= false;

		#ifdef GL_VERSION_2_0
			if (!glDrawBuffers)
			{
				group = "";
				get(glDrawBuffers);
			}
		#endif

		if (index&EXT_MULTITEXTURE_BIT)
		{
			#ifdef GL_ARB_multitexture
				if (available("GL_ARB_multitexture"))
				{
					set(ARB);
					get(glMultiTexCoord1f);
					get(glMultiTexCoord1fv);
					get(glMultiTexCoord2f);
					get(glMultiTexCoord2fv);
					get(glMultiTexCoord3f);
					get(glMultiTexCoord3fv);
					get(glMultiTexCoord4f);
					get(glMultiTexCoord4fv);
					get(glMultiTexCoord1d);
					get(glMultiTexCoord1dv);
					get(glMultiTexCoord2d);
					get(glMultiTexCoord2dv);
					get(glMultiTexCoord3d);
					get(glMultiTexCoord3dv);
					get(glMultiTexCoord4d);
					get(glMultiTexCoord4dv);
					get(glMultiTexCoord1i);
					get(glMultiTexCoord1iv);
					get(glMultiTexCoord2i);
					get(glMultiTexCoord2iv);
					get(glMultiTexCoord3i);
					get(glMultiTexCoord3iv);
					get(glMultiTexCoord4i);
					get(glMultiTexCoord4iv);
					get(glActiveTexture);
					get(glClientActiveTexture);
				}
				else
					one_failed	= true;
			#else
				one_failed	= true;
			#endif
		}
		if (index&EXT_REGISTER_COMBINERS_BIT)
		{
			#ifdef GL_NV_register_combiners
				if (available("GL_NV_register_combiners"))
				{
					set(NV);
					get(glCombinerParameterfv);
					get(glCombinerParameterf);
					get(glCombinerParameteriv);
					get(glCombinerParameteri);
					get(glCombinerInput);
					get(glCombinerOutput);
					get(glFinalCombinerInput);
					get(glGetCombinerInputParameterfv);
					get(glGetCombinerInputParameteriv);
					get(glGetCombinerOutputParameterfv);
					get(glGetCombinerOutputParameteriv);
					get(glGetFinalCombinerInputParameterfv);
					get(glGetFinalCombinerInputParameteriv);
				}
				else
					one_failed	= true;
			#else
				one_failed	= true;
			#endif
		}
		if (index&EXT_VERTEX_PROGRAM_BIT)
		{
			#ifdef GL_ARB_vertex_program
				if (available("GL_ARB_vertex_program"))
				{
					set(ARB);
					get(glVertexAttrib2f);
					get(glVertexAttrib2d);
					get(glVertexAttrib3f);
					get(glVertexAttrib3d);
					get(glVertexAttrib4f);
					get(glVertexAttrib4d);
					get(glVertexAttrib2fv);
					get(glVertexAttrib2dv);
					get(glVertexAttrib3fv);
					get(glVertexAttrib3dv);
					get(glVertexAttrib4iv);
					get(glVertexAttrib4uiv);
					get(glVertexAttrib4fv);
					get(glVertexAttrib4dv);
					get(glVertexAttrib4Niv);
					get(glVertexAttrib4Nuiv);
					get(glVertexAttribPointer);
					get(glEnableVertexAttribArray);
					get(glDisableVertexAttribArray);
					get(glProgramString);
					get(glBindProgram);
					get(glDeletePrograms);
					get(glGenPrograms);
					get(glProgramEnvParameter4d);
					get(glProgramEnvParameter4dv);
					get(glProgramEnvParameter4f);
					get(glProgramEnvParameter4fv);
					get(glProgramLocalParameter4d);
					get(glProgramLocalParameter4dv);
					get(glProgramLocalParameter4f);
					get(glProgramLocalParameter4fv);
					get(glGetProgramEnvParameterdv);
					get(glGetProgramEnvParameterfv);
					get(glGetProgramLocalParameterdv);
					get(glGetProgramLocalParameterfv);
					get(glGetProgramiv);
					get(glGetProgramString);
					get(glGetVertexAttribdv);
					get(glGetVertexAttribfv);
					get(glGetVertexAttribiv);
					get(glGetVertexAttribPointerv);
					get(glIsProgram);
				}
				else
					one_failed	= true;
			#else
				one_failed	= true;
			#endif
		}
		if (index&EXT_VERTEX_BUFFER_OBJECT_BIT)
		{
			#ifdef GL_ARB_vertex_buffer_object
				if (available("GL_ARB_vertex_buffer_object"))
				{
					set(ARB);
					get(glBindBuffer);
					get(glDeleteBuffers);
					get(glGenBuffers);
					get(glIsBuffer);
					get(glBufferData);
					get(glBufferSubData);
					get(glGetBufferSubData);
					get(glMapBuffer);
					get(glUnmapBuffer);
					get(glGetBufferParameteriv);
					get(glGetBufferPointerv);
				}
				else
					one_failed	= true;
			#else
				one_failed	= true;
			#endif
		}
		if (index&EXT_COMPILED_ARRAYS_BIT)
		{
			#ifdef GL_EXT_compiled_vertex_array
				if (available("GL_EXT_compiled_vertex_array"))
				{
					set(EXT);
					get(glLockArrays);
					get(glUnlockArrays);
				}
				else
					one_failed	= true;
			#else
				one_failed	= true;
			#endif
		}
		if (index&EXT_SHADER_OBJECTS_BIT)
		{
			#ifdef GL_ARB_shader_objects
				if (available("GL_ARB_shader_objects"))
				{
					set(ARB);
					get(glDeleteObject);
					get(glGetHandle);
					get(glDetachObject);
					get(glCreateShaderObject);
					get(glShaderSource);
					get(glCompileShader);
					get(glCreateProgramObject);
					get(glAttachObject);
					get(glLinkProgram);
					get(glUseProgramObject);
					get(glValidateProgram);
					get(glUniform1f);
					get(glUniform2f);
					get(glUniform3f);
					get(glUniform4f);
					get(glUniform1i);
					get(glUniform2i);
					get(glUniform3i);
					get(glUniform4i);
					get(glUniform1fv);
					get(glUniform2fv);
					get(glUniform3fv);
					get(glUniform4fv);
					get(glUniform1iv);
					get(glUniform2iv);
					get(glUniform3iv);
					get(glUniform4iv);
					get(glUniformMatrix2fv);
					get(glUniformMatrix3fv);
					get(glUniformMatrix4fv);
					get(glGetObjectParameterfv);
					get(glGetObjectParameteriv);
					get(glGetInfoLog);
					get(glGetAttachedObjects);
					get(glGetUniformLocation);
					get(glGetActiveUniform);
					get(glGetUniformfv);
					get(glGetUniformiv);
					get(glGetShaderSource);
				}
				else
					one_failed	= true;
			#else
				one_failed	= true;
			#endif
		}
		if (index&EXT_WIN_CONTROL_BIT)
		{
			#ifdef WGL_EXT_swap_control
				set(EXT);
				silent(wglSwapInterval);
				silent(wglGetSwapInterval);
			#endif
			#ifdef WGL_ARB_pixel_format
				set(ARB);
				silent(wglChoosePixelFormat);
				silent(wglGetPixelFormatAttribiv);
				silent(wglGetPixelFormatAttribfv);
			#endif
			#if defined(GLX_SGIX_fbconfig) && !defined(GLX_VERSION_1_3)
				set(SGIX);
				silent(glXGetFBConfigAttrib);
				silent(glXChooseFBConfig);
				silent(glXCreateGLXPixmapWithConfig);
				silent(glXCreateContextWithConfig);
				silent(glXGetVisualFromFBConfig);
				silent(glXGetFBConfigFromVisual);
			#endif
		}
		if (index&EXT_PBUFFER_BIT)
		{
			#if SYSTEM	==WINDOWS
				#ifdef WGL_ARB_pbuffer
					set(ARB);
					if (available("WGL_ARB_pbuffer"))
					{
						get(wglCreatePbuffer);
						get(wglGetPbufferDC);
						get(wglReleasePbufferDC);
						get(wglDestroyPbuffer);
						get(wglQueryPbuffer);
					}
					else
						one_failed	= true;
				#else
					one_failed	= true;
				#endif
			#endif
		}
		if (index&EXT_RENDER_TEXTURE_BIT)
		{
			#ifdef WGL_ARB_render_texture
				if (available("WGL_ARB_render_texture"))
				{
					set(ARB);
					get(wglBindTexImage);
					get(wglReleaseTexImage);
					get(wglSetPbufferAttrib);
				}
				else
					one_failed	= true;
			#else
				one_failed	= true;
			#endif
		}

		if (index&EXT_OCCLUSION_QUERY_BIT)
		{
			#ifdef GL_ARB_occlusion_query
				if (available("GL_ARB_occlusion_query"))
				{
					set(ARB);
					get(glGenQueries);
					get(glDeleteQueries);
					get(glIsQuery);
					get(glBeginQuery);
					get(glEndQuery);
					get(glGetQueryiv);
					get(glGetQueryObjectiv);
					get(glGetQueryObjectuiv);
				}
				else
					one_failed	= true;
			#else
				one_failed	= true;
			#endif
		}

		if (index&EXT_MAKE_CURRENT_READ_BIT)
		{
			#ifdef WGL_ARB_make_current_read
				if (available("WGL_ARB_make_current_read"))
				{
					set(ARB);
					get(wglMakeContextCurrent);
					get(wglGetCurrentReadDC);
				}
				else
					one_failed	= true;
			#else
				one_failed	= true;
			#endif
		}

		if (index&EXT_WINDOW_POS_BIT)
		{
			#ifdef GL_ARB_window_pos
				if (available("GL_ARB_window_pos"))
				{
					set(ARB);
					get(glWindowPos2d);
					get(glWindowPos2dv);
					get(glWindowPos2f);
					get(glWindowPos2fv);
					get(glWindowPos2i);
					get(glWindowPos2iv);
					get(glWindowPos2s);
					get(glWindowPos2sv);
					get(glWindowPos3d);
					get(glWindowPos3dv);
					get(glWindowPos3f);
					get(glWindowPos3fv);
					get(glWindowPos3i);
					get(glWindowPos3iv);
					get(glWindowPos3s);
					get(glWindowPos3sv);
				}
				else
					one_failed	= true;
			#else
				one_failed	= true;
			#endif
		}
		if (index&EXT_FRAME_BUFFER_OBJECT_BIT)
		{
			#ifdef GL_ARB_framebuffer_object
				if (available("GL_ARB_framebuffer_object"))
				{
					//set(ARB);
					group = "";
					get(glIsRenderbuffer);
					get(glBindRenderbuffer);
					get(glDeleteRenderbuffers);
					get(glGenRenderbuffers);
					get(glRenderbufferStorage);
					get(glGetRenderbufferParameteriv);
					get(glIsFramebuffer);
					get(glBindFramebuffer);
					get(glDeleteFramebuffers);
					get(glGenFramebuffers);
					get(glCheckFramebufferStatus);
					get(glFramebufferTexture1D);
					get(glFramebufferTexture2D);
					get(glFramebufferTexture3D);
					get(glFramebufferRenderbuffer);
					get(glGetFramebufferAttachmentParameteriv);
					get(glGenerateMipmap);
					get(glBlitFramebuffer);
					get(glRenderbufferStorageMultisample);
					get(glFramebufferTextureLayer);
				}
				else
					one_failed = true;
			#else
				one_failed = true;
			#endif
		}
		
		if (index&EXT_BLEND_FUNC_SEPARATE_BIT)
		{
			#ifdef GL_EXT_blend_func_separate
				if (available("GL_EXT_blend_func_separate"))
				{
					set(EXT);
					get(glBlendFuncSeparate);
				}
				else
					one_failed = true;
			#else
				one_failed = true;
			#endif
		}
		
	
	




		return !one_failed;
		#undef silent
		#undef get
		#undef set
	}

	bool Extension::available(const String&extension_name)
	{
		return gl_ext.lookup(extension_name) || sysgl_ext.lookup(extension_name);
	}


	String ExtCont::listColumns(const String&plank, BYTE columns)
	{
		String rs;
		size_t height	= field.length() / columns;
		if (field.length() % columns)
			height++;
		BYTE	width[256];
		for (BYTE k	= 0; k < columns; k++)
		{
			width[k] = 0;
			for (index_t i	= 0; i < height; i++)
				if (height*k+i < field.length() && field[height*k+i].length() > width[k])
					width[k] = BYTE(field[height*k+i].length());
			width[k]+= 2;
		}
		for (index_t i	= 0; i < height; i++)
		{
			rs += plank;
			for (BYTE k	= 0; k < columns; k++)
				if (height*k+i < field.length())
				{
					rs+= field[height*k+i];
					for (BYTE j	= 0; j < width[k] - field[height*k+i].length(); j++)
						rs+=' ';
				}
			rs+='\n';
		}
		return rs;
	}


	String ExtCont::listExtensions(const String&plank, count_t max_chars_per_line)
	{
		count_t c	= 0;
		String rs(plank);
		for (index_t i	= 0; i < field.length(); i++)
		{
			if (max_chars_per_line && c + field[i].length()+2 > max_chars_per_line)
			{
				rs+="\n"+plank;
				c	= 0;
			}
			rs+=field[i];
			if (i < field.length()-1)
				rs+= ", ";
			c+= field[i].length()+2;
		}
		return rs;
	}

	void Extension::initialize(
	#if SYSTEM	==WINDOWS
		HDC hDC
	#elif SYSTEM	==UNIX
		Display*display,int screen
	#endif
		)
	{
		String sysgl_str;
		#if SYSTEM	==WINDOWS && defined(WGL_ARB_extensions_string)
			this->hDC	= hDC;
			EXT_CONTEXT(ARB);
			EXT_GET_EXTENSION_NO_CHECK(wglGetExtensionsString);
			if (wglGetExtensionsString)
				sysgl_str	= wglGetExtensionsString(hDC);
		#elif SYSTEM	==UNIX
			this->display	= display;
			this->screen	= screen;
			#ifdef __glx_h__
				sysgl_str	=	glXQueryExtensionsString(display,screen);
			#endif
		#endif

		sysgl_ext.divide(sysgl_str);
		String line	= (char*)glGetString(GL_EXTENSIONS);
		gl_ext.divide(line);


		max_lights = 8;
		glGetIntegerv(GL_MAX_LIGHTS,&max_lights);
		max_texture_max_anisotropy = 2.0;
		#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&max_texture_max_anisotropy);
		#endif
		max_texture_layers	= 0;
		max_texcoord_layers	= 0;
		#ifdef GL_MAX_TEXTURE_UNITS_ARB
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB,&max_texture_layers);
			glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB,&max_texcoord_layers);
		#endif
		max_register_combiners	= 0;
		#ifdef GL_MAX_GENERAL_COMBINERS_NV
			glGetIntegerv(GL_MAX_GENERAL_COMBINERS_NV,&max_register_combiners);
		#endif
		max_cube_texture_size	= 0;
		#ifdef GL_MAX_CUBE_MAP_TEXTURE_SIZE
			glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE,&max_cube_texture_size);
		#endif

		initial_total = 1;
		if (available("GL_NVX_gpu_memory_info"))
		{
			cout<<"NVIDIA VRAM info available"<<endl;
			query_method = VRAMQueryMethod::NVIDIA;
		}
		elif (available("GL_ATI_meminfo"))
		{
			cout<<"ATI VRAM info available"<<endl;
			query_method = VRAMQueryMethod::ATI;
			{
				GLint	val[4];
				glGetError();
				glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI,val);
				initial_total = size_t(val[0])*1024;
				glThrowError();
			}
		}
		else
		{
			cout << "No supported VRAM info available!"<<endl;
			query_method = VRAMQueryMethod::None;
		}


		

	}

	bool			Extension::deviceMemoryState(size_t&free,size_t&total)	const
	{
		switch (query_method)
		{
			case VRAMQueryMethod::NVIDIA:
			{
				#ifndef GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 
					#define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 0x9047
					#define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX 0x9048
					#define GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049
					#define GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX 0x904A
					#define GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX 0x904B
				#endif
				GLint	val;
				glGetError();
				glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,&val);
				total = size_t(val)*1024;
				glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,&val);
				free = size_t(val)*1024;
				return glGetError() == GL_NO_ERROR;
			}
			break;
			case VRAMQueryMethod::ATI:
			{
				GLint	val[4];
				glGetError();
				total = initial_total;
				glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI,val);
				free = size_t(val[0])*1024;
				return glGetError() != GL_NO_ERROR;
			}
			break;
		}
		return false;
	}



	Extension gl_extensions;
}
