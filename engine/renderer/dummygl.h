#ifndef engine_renderer_dummyglH
#define engine_renderer_dummyglH

/***************************************************************************************

Dummy GL-interface in compliance with the visual interface-specifications.
The DummyGL interface can be used to immitate the existence of a GL interface to certain
Eve modules in order to use them without such.
Most methods return success if invoked. No objects are ever empty (except textures).

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

***************************************************************************************/


#include "renderer.h"

namespace Engine
{

	#undef T
	#undef T2
	#undef M
	#define T	template <class C>
	#define T2	template <class C0, class C1>
	#define M	template <unsigned Layers>


	class		DummyObject	//! Base DummyGL object
	{
	public:
						DummyObject() {}

	inline	bool		operator==(const DummyObject&other) const
						{
							return this == &other;
						}
	inline	bool		operator!=(const DummyObject&other) const
						{
							return this != &other;
						}
	inline	bool		operator<(const DummyObject&other) const
						{
							return this < &other;
						}
	inline	bool		operator>(const DummyObject&other) const
						{
							return this > &other;
						}

	inline	unsigned	getHandle() const
						{
							return 1;
						}
	inline	bool		isEmpty()	const
						{
							return false;
						}
	inline	PixelType
						getContentType()	const
						{
							return PixelType::Color;
						}
	inline	bool		isTransparent()	const
						{
							return true;
						}
	inline	POINT		size() const
						{
							POINT rs;
							rs.x = 0;
							rs.y = 0;
							return rs;
						}

	};





	/**
		\brief DummyGL Visual Interface

		DummyGL is responsible for binding OpenGL to a Window as well as providing a functional interface for the more common tasks.
	*/
	class DummyGL: public VisualInterface
	{
	protected:
		virtual			void			enableLight(Light*)	{}
		virtual			void			disableLight(Light*)	{}
		virtual			void			updateLight(Light*)	{}
		virtual			void			updateLightPosition(Light*)	{}

	public:

		typedef		float							FloatType;					//!< Float type best used for float fields to achive top render speed
		typedef		unsigned						IndexType;					//!< Index type best used for index fields to achive top render speed
		
		typedef		DummyObject					VBO;			//!< Handle for vertex buffer objects
		typedef		DummyObject					IBO;			//!< Handle for index buffer objects
		typedef		DummyObject					Texture;			//!< Handle for textures
		typedef		DummyObject					Query;			//!< Handle for pixel visibility queries
		typedef		DummyObject					VBO;			//!< Handle for pixel buffer objects
		typedef		DummyObject					Shader;			//!< Handle for pixel buffer objects
		typedef		MaterialComposition<Texture*>	Material;			//!< Material-Container

		static		DummyGL						*global_instance;		//!< Global default instance (set by Eve<DummyGL>)
		
						void			enableLighting()	{}				//!< Globally enables lighting.
						void			disableLighting()	{}				//!< Globally disables lighting.

		virtual			bool			pickLightScene(unsigned  scenario) {return false;}	//!< Picks a light scenario as active scenario (this may be slow at times). \param scenario Index of an existing scenario that should be used from now on (0 = default scenario). \return true on success

						bool			enableGeometryUpload()		{return false;}		//!< Globally enables geometry gmem upload (enabled by default) \return true if geometry upload is possible
						void			disableGeometryUpload()		{}					//!< Globally disables geometry gmem upload (enabled by default)
						bool			queryGeometryUpload()		{return false;}		//!< Queries geometry upload state \return true if geometry is enabled and possible
						void			lockGeometryUpload()		{}				//!< Locks current geometry upload state. Any succeeding enable or disableGeometryUpload() calls will fail until unlockGeometryUpload is called
						void			unlockGeometryUpload()		{}				//!< Unlocks current geometry upload state.
						bool			queryGeometryUploadLock()	{return false;}			//!< Queries whether or not changes to the geometry upload state are currently locked \return true if upload is currently locked
		

						bool			queryBegin(bool depthTest=true)	{return false;}											//!< Set up query environment (disable rendering etc.) \param depthTest if set true then rendered pixels may be occluded by objects \return true if the query-environment could be set up
						void			queryEnd()	{}																//!< Ends querying and returns the original state.
						void			castQuery(const Query&query)	{}										//!< Renders the linked object. To extract the number of pixels on the screen call 'resolveObjectSize(query)' \param query Handle of the respective query object
						void			castPointQuery(const Query&query, const float point[3])	{}				//!< Dynamically renders one point. To extract the number of pixels on the screen call 'resolveObjectSize(query)' \param query Handle of the respective query object \param point Pointer to a location array in R
		
		static inline	void			cullNormal()	{}		//!< Sets face cull mode to normal (back face culling)
		static inline	void			cullInverse()	{}		//!< Sets face cull mode to inverse (front face culling)
		static inline	void			cullAll()		{}			//!< Sets face cull mode to all (no faces will be rendered)
		static inline	void			cullNone()		{}		//!< Sets face cull mode to none (all faces will be rendered)
		static inline	void			depthMask(bool mask)	{}	//!< Sets depth mask
		static inline	void			setDepthTest(eDepthTest depthTest)	{}	//!< Sets depth test to use
		
		static inline	void			setFog(const Fog&fog,bool enabled=true)	{}							//!< Applies given fog configuration \param fog Fog configuration \param enabled Set true to enable fog
		static inline	void			setFog(bool enabled)	{}													//!< Dynamically enables or disables fog \param enabled Set true to enable fog
		static inline	void			setBackgroundColor(float red, float green, float blue, float alpha)	{}	//!< Changes the active clear color \param red Red color component (0.0f - 1.0f) \param green Green color component (0.0f - 1.0f) \param blue Blue color component (0.0f - 1.0f) \param alpha Transparency color component (0.0f - 1.0f)
		static inline	void			setBackgroundColor(const float color[3], float alpha)	{}						//!< Changes the active clear color \param color Pointer to a color vector in R \param alpha Transparency color component (0.0f - 1.0f)
		static inline	void			setBackgroundColor(const float color[4])	{}									//!< Changes the active clear color \param color Pointer to a 4d color vector.
		static inline	void			setClearColor(float red, float green, float blue, float alpha)	{}	//!< Changes the active clear color \param red Red color component (0.0f - 1.0f) \param green Green color component (0.0f - 1.0f) \param blue Blue color component (0.0f - 1.0f) \param alpha Transparency color component (0.0f - 1.0f)
		static inline	void			setClearColor(const float color[3], float alpha)	{}						//!< Changes the active clear color \param color Pointer to a color vector in R \param alpha Transparency color component (0.0f - 1.0f)
		static inline	void			setClearColor(const float color[4])	{}									//!< Changes the active clear color \param color Pointer to a 4d color vector.
		static inline	void			setBackground(float red, float green, float blue, float alpha)	{}	//!< Changes the active clear color \param red Red color component (0.0f - 1.0f) \param green Green color component (0.0f - 1.0f) \param blue Blue color component (0.0f - 1.0f) \param alpha Transparency color component (0.0f - 1.0f)
		static inline	void			setBackground(const float color[3], float alpha)	{}						//!< Changes the active clear color \param color Pointer to a color vector in R \param alpha Transparency color component (0.0f - 1.0f)
		static inline	void			setBackground(const float color[4])	{}									//!< Changes the active clear color \param color Pointer to a 4d color vector.

						bool			useTexture(const Texture&object, bool clamp = false, bool override_safety = false)	{return true;}
						bool			useTexture(const Texture*object, bool clamp = false, bool override_safety = false)	{return true;}
						
		
		static inline	void			setDefaultBlendFunc()	{};	//!< Sets default alpha blend function
		static inline	void			setFlareBlendFunc()		{};	//!< Sets additive alpha weighted blend function

						void			bindMaterial(CGS::MaterialInfo*info,unsigned coord_dimensions, UINT32 vertex_flags, const Texture*const *list,bool treat_empty_as_planar=false)	{}
						void			bindMaterial(const MaterialConfiguration&config, const Texture *const * list)					{}
						void			bindMaterial(const Material&material)														{}
	T					void			bindVertices(C*vdata, const VBO&vobj, unsigned band, const TCodeLocation&location)		{}
	T					void			bindIndices(C*idata, const IBO&iobj)													{}
	T	inline			void			renderSub(const C system[4*4], unsigned vertex_offset, unsigned vertex_count)					{}
	T	inline			void			renderQuadsSub(const C system[4*4], unsigned vertex_offset, unsigned vertex_count)				{}
		inline			void			render(unsigned vertex_offset, unsigned vertex_count)											{}
		inline			void			renderQuads(unsigned vertex_offset, unsigned vertex_count)										{}
	T	inline			void			renderStrip(const C system[4*4], unsigned vertex_offset, unsigned vertex_count)					{}
	T	inline			void			renderQuadStrip(const C system[4*4], unsigned vertex_offset, unsigned vertex_count)				{}
		inline			void			renderStrip(unsigned vertex_offset, unsigned vertex_count)										{}
		inline			void			renderQuadStrip(unsigned vertex_offset, unsigned vertex_count)									{}
		inline			void			renderVertexStrip(unsigned vertex_offset, unsigned vertex_count)								{}
						void			render(const SimpleGeometry&structure)		{}
		static			void			face(const tTVertex&v0, const tTVertex&v1, const tTVertex&v2, const tTVertex&v3)	{}		//!< Renders a quad using the four specified texture-vertices
		static			void			face(const tTVertex&v0, const tTVertex&v1, const tTVertex&v2)	{}							//!< Renders a triangle using the three specified texture-vertices
		static			void			face(const tCTVertex&v0, const tCTVertex&v1, const tCTVertex&v2, const tCTVertex&v3)	{}	//!< Renders a quad using the four specified color-texture-vertices
		static			void			face(const tCTVertex&v0, const tCTVertex&v1, const tCTVertex&v2)	{}						//!< Renders a triangle using the three specified color-texture-vertices
		static			void			face(const tNTVertex&v0, const tNTVertex&v1, const tNTVertex&v2, const tNTVertex&v3)	{}	//!< Renders a quad using the four specified normal-texture-vertices
		static			void			face(const tNTVertex&v0, const tNTVertex&v1, const tNTVertex&v2)	{}						//!< Renders a triangle using the three specified normal-texture-vertices
		static			void			face(const tNVertex&v0, const tNVertex&v1, const tNVertex&v2, const tNVertex&v3)	{}		//!< Renders a quad using the four specified normal-vertices
		static			void			face(const tNVertex&v0, const tNVertex&v1, const tNVertex&v2)	{}							//!< Renders a triangle using the three specified normal-vertices
		static			void			face(const tCVertex&v0, const tCVertex&v1, const tCVertex&v2, const tCVertex&v3)	{}		//!< Renders a quad using the four specified color-vertices
		static			void			face(const tCVertex&v0, const tCVertex&v1, const tCVertex&v2)	{}							//!< Renders a triangle using the three specified color-vertices
		static			void			face(const TVertex&v0, const TVertex&v1, const TVertex&v2, const TVertex&v3)	{}			//!< Renders a quad using the four specified vertices
		static			void			face(const TVertex&v0, const TVertex&v1, const TVertex&v2)	{}								//!< Renders a triangle using the three specified vertices
		static			void			segment(const tCVertex&v0, const tCVertex&v1)	{}												//!< Renders a line segment using the two specified color-vertices
		static			void			segment(const TVertex&v0, const TVertex&v1)	{}												//!< Renders a line segment using the two specified vertices
	T	static inline	void			enterSubSystem(const C system[4*4])	{}																	//!< Enters a sub-system affecting all following render-calls \param system Pointer to a 4x4 system matrix
		static inline	void			exitSubSystem()	{}																				//!< Returns from a sub-system to the next higher system
						void			unbindAll()	{}																					//!< Unbinds the currently bound material, all textures and all vertex/index objects
	T					void			loadModelview(const C matrix[4*4])	{}															//!< Loads the specified matrix as modelview matrix \param matrix Pointer to a 4x4 (system) matrix
	T					void			loadProjection(const C matrix[4*4])	{}															//!< Loads the specified matrix as projection matrix \param matrix Pointer to a 4x4 (projection) matrix
	T					void			replaceModelview(const C matrix[4*4]) {}															//!< Pushes the current modelview matrix to the stack and loads the specified one. restoreModelview() must be called when done working with the replacement \param matrix Pointer to a 4x4 (system) matrix to replace the current one
	T					void			replaceProjection(const C matrix[4*4]) {}															//!< Pushes the current projection matrix to the stack and loads the specified one. restoreProjection() must be called when done working with the replacement \param matrix Pointer to a 4x4 (projection) matrix to replace the current one
		inline			void			restoreModelview() {}																				//!< Restores the modelview matrix from the stack overwriting the currently loaded modelview matrix
		inline			void			restoreProjection() {}																			//!< Restores the projection matrix from the stack overwriting the currently loaded projection matrix
						void			nextFrameNoClr()	{}																				//!< Swaps buffers
						void			nextFrame()	{}																					//!< Swaps buffers and clears the back buffer

		static			String			status()	{return "OK";}																						//!< Queries the current DummyGL status \return String containing the current interface status
		static			String			renderState()	{return "DummyGL";}																					//!< Queries the current render state \return String containing the current OpenGL render state

	#if SYSTEM==WINDOWS
						bool			createContext(HWND hWnd, TVisualConfig&config)	{return false;}
	#elif SYSTEM==UNIX
						bool			createContext(Display*connection, TVisualConfig&config, TWindowAttributes&out_attributes)	{return false;}
						bool			bindContext(Window window)	{return false;}
	#endif

						void			setVerbose(bool)	{}
						void			setRegion(const RECT&region)	{}																//!< Sets a new rendering region	\param region Rectangle specifying the new rendering region in the active window
		static			const char*		getErrorStr()		{return "DummyGL does not provide any functionality";}																					//!< Returns a string-representation of the last occured error \return String-representation of the last occured error
		static			int				getErrorCode()		{return ERR_GENERAL_FAULT;}																				//!< Returns an index representation of the last occured error \return Error code of the last occured error
						const char*		name()				{return "DummyGL";}																						//!< Returns identity \return "OpenGL"
	};

	#undef T
	#undef T2
	#undef M



	/*! \fn	bool DummyGL::queryObject(QueryObject&target, const float lower_corner[3], const float upper_corner[3])
	 *	\brief Creates a new query object
	 *	\param target Target query object (will be reused if not empty)
	 *	\param lower_corner Pointer to an R vector defining the lower corner of the bounding box.
	 *	\param upper_corner Pointer to an R vector defining the upper corner of the bounding box.
	 *	\return true if the query object could be created, false otherwise.

	 Creates a query object from a bounding box. When testing the box all faces will be rendered without affecting any of the buffers.
	 If the resulting pixel count is above 10 then the specified object is considered visible.


	 *	\fn	QueryObject DummyGL::queryObject(const float lower_corner[3], const float upper_corner[3])
	 *	\brief Creates a new query object
	 *	\param lower_corner Pointer to an R vector defining the lower corner of the bounding box.
	 *	\param upper_corner Pointer to an R vector defining the upper corner of the bounding box.
	 *	\return Handle of a new query object (which will be empty if the query object could not be created)

	 Creates a query object from a bounding box. When testing the box all faces will be rendered without affecting any of the buffers.
	 If the resulting pixel count is above 10 then the specified object is considered visible.
	 

	 *	\fn	bool	DummyGL::queryObject(QueryObject&target, const float point[3])
	 *	\brief Creates a new query object
	 *	\param target Target query object (will be reused if not empty)
	 *	\param point Pointer to an R vector defining the 3d position of the point to test.
	 *	\return true if the query object could be created, false otherwise.

	 Creates a query object from a point. When testing the point one single pixel will be rendered without affecting any of the buffers.
	 If the resulting pixel count is 1 then the specified pixel is visible.
	 

	 *	\fn	QueryObject	DummyGL::queryObject(const float point[3])
	 *	\brief Creates a new query object
	 *	\param point Pointer to an R vector defining the 3d position of the point to test.
	 *	\return Handle of a new query object (which will be empty if the query object could not be created)

	 Creates a query object from a point. When testing the point one single pixel will be rendered without affecting any of the buffers.
	 If the resulting pixel count is 1 then the specified pixel is visible.
	 

	 *	\fn	QueryObject	DummyGL::queryObject()
	 *	\brief Creates a new query object
	 *	\return Handle of a new query object (which will be empty if the query object could not be created)

	 Creates a query object from an empty object. This is for manually testing pixels for visibility.

	 
	 *	\fn	FrameBufferObject	DummyGL::frameBufferObject(unsigned width, unsigned height, ePBufferType type, BYTE fsaa)
	 *	\brief Creates a new pixel buffer object
	 *	\param width Width of the new pixel buffer object in pixels
	 *	\param height Height of the new pixel buffer object in pixels
	 *	\param type Pixel buffer type
	 *	\param fsaa Full scene anti-aliasing samples per pixel. Note that usually pixel buffers do not support fsaa.
	 *	\return Handle of a new pixel buffer object (which will be empty if the pbuffer object could not be created)

	 Creates a new pixel buffer object. Pixel buffer objects are still somewhat experimental and have not been tested under linux.
	 Binding is currently disabled.

	 
	 *	\fn	bool	DummyGL::frameBufferObject(FrameBufferObject&target,unsigned width, unsigned height, ePBufferType type, BYTE fsaa)
	 *	\brief Creates a new pixel buffer object
	 *	\param target Target pbuffer object (will be reused if not empty)
	 *	\param width Width of the new pixel buffer object in pixels
	 *	\param height Height of the new pixel buffer object in pixels
	 *	\param type Pixel buffer type
	 *	\param fsaa Full scene anti-aliasing samples per pixel. Note that usually pixel buffers do not support fsaa.
	 *	\return true if the pixel buffer object could be created, false otherwise

	 Creates a new pixel buffer object. Pixel buffer objects are still somewhat experimental and have not been tested under linux.
	 Binding is currently disabled.

	 *	\fn	bool	DummyGL::useTexture(const TextureObject&object, bool clamp = false)
	 *	\brief Binds the specifed texture object to OpenGL
	 *	\param object Handle of the respective texture object
	 *	\param clamp Set true to clamp texture-coordinates to [0,1]
	 *	\return true if the specified texture could be bound, false otherwise

	 This function simply binds the specified texture to the GL-context and will affect all following render commands.
	 Be sure not to use this function if a material is currently bound or the application will terminate.
	 Calling \a useTexture(TextureObject()) will unbind any bound texture.


	 *	\fn	void DummyGL::bindMaterial(Structure::MaterialInfo<C>*info,TextureObject*list,bool treat_empty_as_planar=false)
	 *	\brief Binds the specifed material object to OpenGL for subsequent render calls
	 *	\param info Pointer to a CVStructure material info structure
	 *	\param list Pointer to an array of texture objects (one for each layer specified in the info struct)
	 *	\param treat_empty_as_planar If set true then DummyGL will assign 2d-texture coordinates to empty texture layers. Not recommended.

	 This function binds a Structure::MaterialInfo<C> structure using a list of textures to OpenGL for subsequent rendering calls.
	 Note that the bound material specifies the number of floats that must be specified per vertex during later render calls.
	 
	 
	 *	\fn	void DummyGL::bindMaterial(const MaterialConfiguration&config, TextureObject*list)
	 *	\brief Binds the specifed material object to OpenGL for subsequent render calls
	 *	\param config Reference to an eve material configuration structure
	 *	\param list Pointer to an array of texture objects (one for each layer specified in the config struct)

	 This function binds an MaterialConfiguration structure using a list of textures to OpenGL for subsequent rendering calls.
	 Note that the bound material specifies the number of floats that must be specified per vertex during later render calls.
	 
	 
	 *	\fn	void DummyGL::bindMaterial(const MaterialObject&material)
	 *	\brief Binds the specifed material object to OpenGL for subsequent render calls
	 *	\param material Reference to an eve material object

	 This function binds a material object structure to OpenGL for subsequent rendering calls.
	 Note that the bound material specifies the number of floats that must be specified per vertex during later render calls.
	 
	 
	 *	\fn	void DummyGL::bindVertices(C*vdata, const VertexBufferObject&vobj, unsigned band, const TCodeLocation&location)
	 *	\brief Binds vertex data for rendering
	 *	\param vdata Pointer to an array containing the vertex data in local memory
	 *	\param vobj Handle of a vertex buffer object containing the data in vram
	 *	\param band Number of float values per vertex for verification
	 *	\param location Provide CLOCATION as location parameter

	 A material must be bound before invoking this method.
	 Both local and remote vertex data must be specified to insure the binding.
	 If available the remote vertex field will be used. Otherwise the local field is used. Due to this double binding the client application
	 does not need to know whether or not the vertex buffer extension is available or the data has been uploaded at all.
	 The method throws an error if the specified \a band parameter is less than the bound material's constrains.

	 
	 *	\fn	void DummyGL::bindIndices(C*idata, const IndexBufferObject&iobj)
	 *	\brief Binds index data for rendering
	 *	\param vdata Pointer to an array containing the index data in local memory
	 *	\param vobj Handle of an index buffer object containing the data in vram

	 A material must be bound before invoking this method.
	 Both local and remote index data must be specified to insure the binding.
	 If available the remote index field will be used. Otherwise the local field is used. Due to this double binding the client application
	 does not need to know whether or not the vertex buffer extension is available or the data has been uploaded at all.


	 *	\fn	void DummyGL::renderSub(C*system, unsigned vertex_offset, unsigned vertex_count)
	 *	\brief Renders a triangle portion of the bound vertex/index data in the specified sub-system
	 *	\param system Pointer to a 4x4 system matrix
	 *	\param vertex_offset First vertex to render with 0 indicating the first vertex in the field
	 *	\param vertex_count Number of vertices to render (3 needed per triangle)

	 The method enters the specified sub-system, renders the specified number of triangles and exits the system again.
	 Both vertex and index fields must be bound in order to use this method.

	 
	 *	\fn	void DummyGL::render(unsigned vertex_offset, unsigned vertex_count)
	 *	\brief Renders a triangle portion of the bound vertex/index data
	 *	\param vertex_offset First vertex to render with 0 indicating the first vertex in the field
	 *	\param vertex_count Number of vertices to render (3 needed per triangle)
	 
	 The method renders the specified number of triangles.
	 Both vertex and index fields must be bound in order to use this method.

	 
	 *	\fn	void DummyGL::renderStrip(C*system, unsigned vertex_offset, unsigned vertex_count)
	 *	\brief Renders a triangle-strip in the specified sub-system
	 *	\param system Pointer to a 4x4 system matrix
	 *	\param vertex_offset First vertex to render with 0 indicating the first vertex in the field
	 *	\param vertex_count Number of vertices to render (3 needed for the first triangle, 1 for each following)
	 
	 The method enters the specified sub-system, renders the specified triangle strip and exits the system again.
	 Both vertex and index fields must be bound in order to use this method.

	 
	 *	\fn	void DummyGL::renderStrip(unsigned vertex_offset, unsigned vertex_count)
	 *	\brief Renders a triangle-strip
	 *	\param vertex_offset First vertex to render with 0 indicating the first vertex in the field
	 *	\param vertex_count Number of vertices to render (3 needed for the first triangle, 1 for each following)
	 
	 The method renders the specified triangle strip.
	 Both vertex and index fields must be bound in order to use this method.

	 
	 *	\fn	void DummyGL::renderVertexStrip(unsigned vertex_offset, unsigned vertex_count)
	 *	\brief Renders a triangles without use of the index field
	 *	\param vertex_offset First vertex to render with 0 indicating the first vertex in the field
	 *	\param vertex_count Number of vertices to render (3 needed per triangle)
	 
	 The method renders the specified number of triangles.
	 A vertex field must be bound in order to use this method.

	 
	 *	\fn	void DummyGL::render(const SimpleGeometry&structure)
	 *	\brief Renders the specified structure
	 *	\param structure Reference to the structure to render


	*/


}


#endif
