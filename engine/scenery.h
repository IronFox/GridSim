#ifndef engine_sceneryH
#define engine_sceneryH

/******************************************************************

Language- and DefStruct-dependent 3d-environment renderer.
The scenery handles most of the internal settings in order
to provide a simplified interface.

******************************************************************/

#include "display.h"
#include "renderer/renderer.h"
#include "aspect.h"
#include "../container/lvector.h"
#include "../container/sortedlist.h"
#include "../container/binarytree.h"
#include "../container/linkedlist.h"
#include "../container/buffer.h"
#include "../container/incremental_hash_list.h"
#include "../geometry/cgs.h"


#ifdef DEBUG
	#include "../container/lists.h"
#endif


namespace Engine
{
	//extern float global_shortest_edge_length;


	#define SCENERY_LOG(msg) {if (::Engine::log_file) (*::Engine::log_file)<<__FILE__+String("(")+(String)__LINE__+") <"+__func__+">: "+String(msg)+"\r\n"; }	//define void to disable logging
	#define SCENERY_BEGIN		SCENERY_LOG("begin")
	#define SCENERY_END			SCENERY_LOG("end")

	template <class> struct TTextureLink;

	template <class> class TextureTable;		//holds a list of TextureLink for sequential texture-load
	//template <class> class ObjectField;
	template <class> class StructureField;
	template <class> class ObjectEntity;
	template <class> class StructureEntity;
	template <class> class RenderBuffer;
	template <class> class VisualObject;
	template <class> class VisualContainer;
	template <class, class> class Material;
	template <class, class> class Scenery;


	/* incremental-process

	1) check non-extracted texture-data
	2) check texture-object
	3) append or merge visual

	main characteristic
	- visual does not hold system-tree
	- materials may be discarded on empty system-list-occurance

	*/

	MAKE_SORT_CLASS(SystemSort, system)


	namespace StructureConfig	//! Structure flags
	{
		enum StructureFlags		//! Flags to use for StructureEntity::Config
		{
			Invalid				= 0x00,	//!< Structure entity is invalid (invalid lookup result)
			Default				= 0x01,	//!< Default structure entity. Visible, dynamic detail, detectable
			Invisible			= 0x02,	//!< Entity is permanently invisible (but potentially detectable)
			AlwaysVisible		= 0x04,	//!< Entity is always rendered (even if visibility checks return invisible)
			AlwaysHighestDetail	= 0x08,	//!< If this entity is rendered it always uses the highest available detail
			NonDetectable		= 0x10,	//!< Entity is not detectable (by lookupClosest()).
			NotDetectable		= NonDetectable	//!< Identical to NonDetectable
		};
	}



	/**
		@brief Texture that counts referencing and unloads if appropriate
	*/
	template <class GL>
		class ReferencedTexture:public GL::Texture, public CGS::TextureObject
		{
		public:
				count_t								usage;		//!< Reference counter
				PixelType							content_type;

				const typename GL::Texture			*external;

													ReferencedTexture():usage(0),external(NULL)
													{}

													ReferencedTexture(Nothing):usage(0),external(NULL)
													{}


				typename GL::Texture::Reference		reference()	const
													{
														if (external)
															return external;
														return this;
													}
		};

	/*!
		\brief Texture lookup table
	*/
	template <class GL>
		class TextureTable /*: public Signature*/
		{
		public:
				PointerContainer<ReferencedTexture<GL> >	storage;
				float										anisotropy;
							
															TextureTable():anisotropy(1.0f)
															{}

				bool								retrieve(CGS::TextureA*source, typename GL::Texture::Reference&target, Engine::MaterialLayer&layer);	//!< Loads a texture to the respective rendering context (if the respective texture source has not already been loaded) \return true on success, false otherwise. If the method returns false then the specified reference will be reset to empty
				void								discard(CGS::TextureA*source);	//!< Discards a loaded texture
				bool								createLink(CGS::TextureA*from, const typename GL::Texture&object);	//!< Creates a texture link from the specified texture to the externally loaded texture object. The referenced CGS texture must not already be loaded. \return true on success
				void								clear()	{storage.clear();}
		};


	/*template <class Def=CGS::StdDef> class ObjectField:public Sorted<List::Vector<ObjectEntity<Def> >,SystemSort >
	{};*/

	template <class Def=CGS::StdDef> class StructureField:public Sorted<List::Vector<StructureEntity<Def> >,SystemSort >
	{};

	/*!
		\brief Link between a texture resource and a loaded texture reference
	*/
	template <class GL> struct TTextureLink:public CGS::TextureObject
	{
			typename GL::Texture			object;	//!< Loaded texture object
			CGS::TextureA					*name;	//!< Texture resource
			unsigned						usage;	//!< Reference counter
	};



	template <class Def>
		class OrderedMatrixReference
		{
		public:
				TMatrix4<typename Def::SystemType>	*reference;
				index_t								order_index;

													OrderedMatrixReference():reference(NULL),order_index(InvalidIndex)
													{}
													OrderedMatrixReference(TMatrix4<typename Def::SystemType>*matrix, index_t index):reference(matrix),order_index(index)
													{}
		inline	bool								operator<(const OrderedMatrixReference<Def>&other)	const
													{
														return order_index < other.order_index || (order_index == other.order_index && reference < other.reference);
													}
		inline	bool								operator>(const OrderedMatrixReference<Def>&other)	const
													{
														return order_index > other.order_index || (order_index == other.order_index && reference > other.reference);
													}
		inline	bool								operator==(const OrderedMatrixReference<Def>&other)	const
													{
														return reference == other.reference;
													}
		inline	bool								operator!=(const OrderedMatrixReference<Def>&other)	const
													{
														return reference != other.reference;
													}

		};

	/*!
		\brief System to CGS::SubGeometry mapping
		
		An ObjectEntity maps an instance sub system to an embedded CGS::SubGeometry. Since the same CGS::SubGeometry instance may be embedded multiple times using different systems, multiple ObjectEntity instances
		may exist for the same CGS::SubGeometry instance.
	*/
	template <class Def=CGS::StdDef> class ObjectEntity
	{
	private:
			void						updateCage();
	public:
			class Attachment
			{};
			
			StructureEntity<Def>				*structure;		//!< Parent structure instance (not NULL)
			TMatrix4<typename Def::SystemType>	*system;			//!< Pointer to a system matrix (4x4) (possibly to a CGS::SubGeometryInstance::path) (not NULL)
			CGS::SubGeometryA<Def>				*source;		//!< Actual geometrical source pointer (not NULL). Multiple ObjectEntity instances may point to the same CGS::SubGeometry instance.
			bool								visible,		//!< True if this object entity passed the last visibility check
												client_visible,	//!< Reserved for custom client application usage
												added,			//!< Should be false at all times. Used to distinctly map object entities to the lookup buffer by SceneryTree
												invert_set;		//!< True if the provided system invert is calculated (false by default. only makes sense if the local entity is mostly stationary)
			Box<typename Def::FloatType>		src_dim,			//!< System relative bounding box of the local object (updated when the local entity is created)
												scaled_dim;			//!< Scaled dimensions
			TVec4<typename Def::FloatType>		cage[8];		//!< System relative cage for determining visibility (created using dim)
			typename Def::FloatType				shortest_edge_length,	//!< Average edge length on maximum detail
												src_radius,			//!< Radius of the local entity (distance between the furthest vertex and the local object center)
												radius, 			//!< Scaled radius
												sys_scale;		//!< Local system scale. Should be 1.0
			TMatrix4<typename Def::FloatType>	invert; 	//!< Invert of the specified system matrix.
			Reference<Attachment>				attachment;		//!< Custom data attachment

										ObjectEntity(CGS::SubGeometryA<Def>*target, StructureEntity<Def>*structure);
			void						rescale();		//!< Updates the local system scale variable based on the current system
	};

	/*!
		\brief Mapping for one instance of a CGS::Geometry
		
		An StructureEntity maps an instance system to an embedded CGS::Geometry. Since the same CGS::Geometry instance may be embedded multiple times using different systems, multiple StructureEntity instances
		may exist for the same CGS::Geometry instance.
	*/
	template <class Def=CGS::StdDef> class StructureEntity:public CGS::TInstanceObject
	{
	private:
			void						createEntity(CGS::SubGeometryA<Def>&child);
	public:
			class Attachment
			{};
			

			
			

			TMatrix4<typename Def::SystemType>	*system;								//!< Pointer to a system matrix (4x4) (possibly to a GeometryInstance::system) (not NULL)
			CGS::Geometry<Def>					*target;								//!< Actual geometrical source pointer (not NULL). Multiple StructureEntity instances may point to the same CGS::Geometry instance.

			Buffer<ObjectEntity<Def>,8>	object_entities;						//!< List of object entities
			PointerTable<index_t>		object_entity_map;						//!< System-pointer to index map

			unsigned					detail,									//!< Calculated detail of this structure entity
										max_detail,								//!< Maximum detail level provided by the respective structure
										config;									//!< Structure configuration (any combination of StructureConfig::StructureFlags ). This value affects all ObjectEntity instances linked to this instance.
			TVec4<>						tint;									//!< Tint to apply to this structure. Applied as emission color. Black by default
			bool						visible,								//!< True if this structure passed the last visibility check.
										added;									//!< Should be false at all times. Used to distinctly map structure entities to the lookup buffer by SceneryTree
			typename Def::FloatType		src_radius,								//!< Unscaled geometrical radius as extracted from the linked Geometry instance.
										radius, 								//!< Scaled radius
										shortest_edge_length,					//!< Average edge length on maximum detail
										sys_scale; 								//!< Extracted system-scale (1 if system is normalized)
			Reference<Attachment>		attachment;								//!< Custom data attachment

										StructureEntity(CGS::Geometry<Def>*target, unsigned type);
			void						rescale();								//!< Updates the local system scale variable based on the current system
			index_t						indexOf(CGS::SubGeometryA<Def>*child);		//!< Retrieves the index of the specified sub geometry. \return Index of the specified sub geometry with 0 being the first sub geometry or InvalidIndex if the specified child could not be found
	};

	/*!
		\brief Buffer to store rendering data for upload to the rendering context
	*/
	template <class GL> class RenderBuffer
	{
	public:
			Array<typename GL::FloatType>	vertex_field;	//!< Vertex (float) field
			Array<typename GL::IndexType>	index_field;	//!< Index (uint) field
	};



	/*!
		\brief Rendering structure, mapping one visual detail
	*/
	template <class Def=CGS::StdDef>
		class VisualDetail
		{
		public:
				List::ReferenceVector<List::ReferenceVector<ObjectEntity<Def> > >		objects;	//!< One list instance per structure entity. Each object in the inner list corresponds with the respective entry in robjects.
				List::ReferenceVector<CGS::RenderObjectA<Def> >							robjects;	//!< List of render objects in this detail layer
				Array<unsigned>															offset;		//!< Index offset of the respective render object in the index buffer
				Buffer<index_t>															object_index;	//!< Per render object. Points to the respective render objects.
				
				void																	adoptData(VisualDetail<Def>&other);
		};
		
		
		
	template <class Def=CGS::StdDef>
		class RenderGroup	//! Groups all render objects of the same geometry
		{
		public:
				Sorted<List::ReferenceVector<StructureEntity<Def> >,PointerSort>	entities;	//!< Structure entities created from this the local geometry
				CGS::MaterialA<Def>													*ident;		//!< Pointer to the respective material. This pointer should be unique throughout the scenery
				
				List::ReferenceVector<CGS::RenderObjectA<Def> >						robjects;	//!< List of render objects stored in the material
				
				Array<VisualDetail<Def>,Adopt>										detail_layers;	//!< Array of detail groups

																					RenderGroup():ident(NULL)
																					{}
				
				void																merge(CGS::MaterialA<Def>&material, StructureEntity<Def>*entity, UINT32&flags);	//!< Merges the specified material's data into the local rendering group (only if of the same geometry) @param flags [out] Flag out. Any occured flags will be or'ed to the current content 
		};


	/*!
		\brief Material mapping
		
		An Material<GL,Def> instance groups mostly identical materials into one mapping, reducing the amount of times the gpu has to load a new material per frame.
		A material keeps vertex and index data compact in two large arrays, thus reducing bind overhead. This behavior does, however, increase load time.
	*/
	template <class GL, class Def=CGS::StdDef> 
		class Material:public CGS::MaterialObject
		{
		protected:
			void													init();		//!< Updates the local material and constructs the local shader. @a texture_table must not be NULL
			bool													initialized;
		public:
		
			Sorted<List::Vector<RenderGroup<Def> >,IdentSort>		groups;		//!< Lists all groups, loaded to this visual. Ideally this would be one group per merged geometry material.
			PointerTable<RenderGroup<Def>*>							group_map;	//!< Maps StructureEntities to render groups for unmapping purposes
			
			Scenery<GL,Def>											*const owner;
			GL														*renderer;					//!< Pointer to the used rendering context (not NULL)
			CGS::MaterialInfo										info;						//!< Copy of the original material info. Only the first merged material information is used for rendering.
			UINT16													coord_layers,				//!< Number of layers requiring 2 component texture coordinates
																	additional;					//!< Number of additional non-used floats per vertex
			UINT32													vertexFlags;				//!< Vertex flags (has normal, has tangent, etc)
			RenderBuffer<GL>										buffer;						//!< Vertex/index data upload buffer
			bool													locked,			//!< Set true if this visual should not be erased if empty. Non-locked materials will be erased when they do no longer host any rendering data.
																	requires_rebuild;	//!< Set true if materials were merged or structure entities unmapped with false passed as \b rebuild
			TextureTable<GL>										*texture_table;		//!< Texture resource pointer (not NULL)
			
			typename GL::Material									material;
			typename Engine::VertexBinding							binding;
			typename GL::Shader										shader;			//!< Shader used to render this material
			typename GL::IBO										ibo;			//!< Uploaded index data
			typename GL::VBO										vbo;			//!< Uploaded vertex data

												Material(Scenery<GL,Def>*parent, GL*,CGS::MaterialInfo&,TextureTable<GL>*);
			virtual								~Material();
			bool								similar(const CGS::MaterialA<Def>&material);	//!< Queries whether or not the local visual is similar to the specified CGS material. This comparison is less accurate than comparing two loaded materials but never produces false positives \param material CGS material to compare to \return true if the specified material is similar to the local visual and may be merged with it
			bool								similar(const Material<GL,Def>&other);			//!< Queries whether ot not the local visual is similar to the specified remote visual. This comparison compares both material parameters and texture data. The method may return true where the similar() method for CGS materials returned false. \param other Visual to compare to \return true if the specified visual is similar to the local visual and may be merged with it
			void								merge(StructureEntity<Def>*entity, CGS::MaterialA<Def>&material, bool rebuild,StringList&warn_out);	//!< Merges a CGS material with the local visual \param entity Super entity associated with the material data \param material Material to merge \param rebuild Rebuild the local visual when done merging. Passing false here will require explicit rebuilding later. \param warn_out Out string list to store merging warnings in
			void								unmap(StructureEntity<Def>*entity, bool rebuild, StringList&warn_out);	//!< Unmaps the specified structure entity \param entity Structure entity to unmap \param rebuild Rebuild the local visual when done unmapping. Passing false here will require explicit rebuilding later. \param warn_out Out string list to store unmapping warnings in
			void								Rebuild(StringList*warn_out, bool enforce=false);							//!< Explicitly rebuilds the local visual \param warn_out Out warning string list or NULL to not log warnings \param enforce Set true to enforce rebuild even if the local visual does not require rebuilding
			void								render(bool ignore_material=false, bool ignoreShader = false);						//!< Renders the local visual. render() automatically rebuilds the visual if \b requires_update is set true
			String								state();												//!< Queries a state string representation
			bool								isOpaque();												//!< Returns true if the local visual fully visible (not transparent)
			void								extractRenderPath(List::Vector<typename Def::FloatType>&);	//!< Deprecated
			
			void								shutdown();			//!< Unlink info and set global_shutdown of objects. Evoked by ~Scenery()
		};

	/*!
		\brief Dynamic mapped scenery
		
		The Scenery class maps CGS geometries to a rendering context for realtime rendering. The standard scenery supports material merging, visibility checks, dynamic detail selection, and separation in opaque and transparent materials.
	*/
	template <class GL, class Def=CGS::StdDef>
		class Scenery:public Signature
		{
		protected:
			CGS::TextureA				dummy_texture;	//!< Non empty dummy texture (for embedDummyMaterial())
			CGS::MaterialInfo			dummy_info;		//!< Non empty dummy material info (for embedDummyMaterial())
			unsigned					locked;			//!< Lock level (0 = not locked, 1+ = locked). Lock prevents the scenery from automatically rebuilding materials.
				
			String						fused;
			StringList					warnings;		//!< Scenery warning list

			GL							*renderer;		//!< Link to the active renderer. The scenery will attempt to automatically retrieve a renderer once if this pointer is NULL. If renderer remains NULL, then Scenery will throw an exception.

			template <typename F0, typename F1, typename F2, typename F3>
				unsigned				EstimateLOD(const Aspect<F0>&aspect, const F1&distance, const F2&scaledEdgeLength, const F3&resolutionModifier);

		public:
			typedef StructureEntity<Def>MyStructureEntity;
				
			/**/						Scenery(TextureTable<GL>*table=NULL);				//!< Base constructor. Implies automatical renderer retrieval. \param table Pointer to a compatible external texture table or NULL to use the internal texture table
			/**/						Scenery(GL*renderer, TextureTable<GL>*table=NULL);	//!< Constructor for a specific renderer. \param renderer Pointer to the renderer to use for geometry/texture upload and rendering \param table Pointer to a compatible external texture table or NULL to use the internal texture table
			/**/						Scenery(GL&renderer, TextureTable<GL>*table=NULL);	//!< Constructor for a specific renderer. \param renderer Reference to the renderer to use for geometry/texture upload and rendering \param table Pointer to a compatible external texture table or NULL to use the internal texture table
			virtual						~Scenery();												//!< The destructor calls clear() with \b disconnected set true, assuming any referenced geometrical data does no longer exist or will be erased shortly after.

			#ifdef DEBUG
				StringList				debug;
			#endif
			bool						merge_materials;											//!< True by default. Allows merging of similar materials. Setting this value to false will prevent the scenery from automatically merging materials. Disabling material merging can prove useful in scenarios where material color attributes may dynamically change.
			bool						keep_unused_materials;										//!< False by default. Allows materials to remain loaded, even though they are not actually needed by any loaded data

			StructureField<Def>			structures;													//!< List of embedded structure entities (object entities are managed by the respective structure entity)
			TextureTable<GL>			local_textures,												//!< Local texture table to upload/retrieve texture references of CGS::Texture instances.
										*textures;													//!< Effective texture table. By default this pointer points to \b local_textures.
			Buffer<std::shared_ptr<Material<GL,Def> >,0,Swap>	opaque_materials,												//!< List of fully opaque materials. This also includes masked materials since those do not provide blended transparency.
										transparent_materials,										//!< List of transparent materials that require blending.
										all_materials;
				
			count_t						countWarnings();						//!< Queries the number of warnings occured during previous embed(), remove() or rebuild() calls.
			inline count_t				CountWarnings()	{return countWarnings();}
			void						clearWarnings();						//!< Clear warnings generated during preceeding embed(), remove(), or rebuild() calls.
			inline void					ClearWarnings()	{clearWarnings();}
			const String&				getWarning(index_t index);				//!< Queries a specified warning \param index Index of the warning to retrieve ranging [0,countWarnings()-1] \return const String reference to the respective warning
			inline const String&		GetWarning(index_t index)	{return getWarning(index);}
			const String&				getWarnings(bool clear_warnings=true);	//!< Queries a merged warning string using newline as separator \param clear_warnings Clear local warnings after merging \return const String reference to the respective merged warning list
			inline const String&		GetWarnings(bool clearWarnings=true)	{return getWarnings(clearWarnings);}
				
			MyStructureEntity*			embed(CGS::StaticInstance<Def>&instance,unsigned detail_type=StructureConfig::Default);	//!< Embeds a CGS::StaticInstance instance creating a new StructureEntity instance (if not already embedded). Subsequent embed() calls on the same geometrical instance reuse loaded data. \param instance Instance to embed \param detail_type Detail type to use for this particular instance \return Newly embedded or already mapped StructureEntity
			inline MyStructureEntity*	Embed(CGS::StaticInstance<Def>&instance,unsigned detailType=StructureConfig::Default)	{return embed(instance,detailType);}
			MyStructureEntity*			embed(CGS::StaticInstance<Def>*instance,unsigned detail_type=StructureConfig::Default);	//!< Embeds a CGS::StaticInstance instance creating a new StructureEntity instance (if not already embedded). Subsequent embed() calls on the same geometrical instance reuse loaded data. \param instance Instance to embed \param detail_type Detail type to use for this particular instance \return Newly embedded or already mapped StructureEntity
			inline MyStructureEntity*	Embed(CGS::StaticInstance<Def>*instance,unsigned detailType=StructureConfig::Default)	{return embed(instance,detailType);}
			MyStructureEntity*			embed(CGS::AnimatableInstance<Def>&instance,unsigned detail_type=StructureConfig::Default);	//!< Embeds a CGS::AnimatableInstance instance creating a new StructureEntity instance (if not already embedded). Subsequent embed() calls on the same geometrical instance reuse loaded data. \param instance Instance to embed \param detail_type Detail type to use for this particular instance \return Newly embedded or already mapped StructureEntity
			inline MyStructureEntity*	Embed(CGS::AnimatableInstance<Def>&instance,unsigned detailType=StructureConfig::Default){return embed(instance,detailType);}
			MyStructureEntity*			embed(CGS::AnimatableInstance<Def>*instance,unsigned detail_type=StructureConfig::Default);	//!< Embeds a CGS::AnimatableInstance instance creating a new StructureEntity instance (if not already embedded). Subsequent embed() calls on the same geometrical instance reuse loaded data. \param instance Instance to embed \param detail_type Detail type to use for this particular instance \return Newly embedded or already mapped StructureEntity
			inline MyStructureEntity*	Embed(CGS::AnimatableInstance<Def>*instance,unsigned detailType=StructureConfig::Default)	{return embed(instance,detailType);}
			MyStructureEntity*			embed(CGS::Geometry<Def>&structure,unsigned detail_type=StructureConfig::Default);	//!< Embeds the currently linked system pointers of the specified geometry creating a new StructureEntity instance (if not already embedded). Subsequent embed() calls on the same geometry reuse loaded data. \param instance Instance to embed \param detail_type Detail type to use for this particular instance \return Newly embedded or already mapped StructureEntity
			inline MyStructureEntity*	Embed(CGS::Geometry<Def>&structure,unsigned detailType=StructureConfig::Default)			{return embed(structure,detailType);}
			MyStructureEntity*			embed(CGS::Geometry<Def>*structure,unsigned detail_type=StructureConfig::Default);	//!< Embeds the currently linked system pointers of the specified geometry creating a new StructureEntity instance (if not already embedded). Subsequent embed() calls on the same geometry reuse loaded data. \param instance Instance to embed \param detail_type Detail type to use for this particular instance \return Newly embedded or already mapped StructureEntity
			inline MyStructureEntity*	Embed(CGS::Geometry<Def>*structure,unsigned detailType=StructureConfig::Default)			{return embed(structure,detailType);}
			std::shared_ptr<Material<GL,Def> >embed(MyStructureEntity*entity,CGS::MaterialA<Def>&material);	//!< Embeds a CGS material \param entity Parent structure entity (must not be NULL) \param material Material to embed
			inline std::shared_ptr<Material<GL,Def> >Embed(MyStructureEntity*entity,CGS::MaterialA<Def>&material)						{return embed(entity,material);}
			void						embedDummyMaterial();												//!< Embeds an empty dummy material. When rendering large texture-less geometries this method may greatly increase rendering speed (due to whatever odd gpu related reasons)
			inline void					EmbedDummyMaterial()	{embedDummyMaterial();}
			void						remove(CGS::Geometry<Def>&structure);		//!< Removes the structure entity (and children) refering the currently linked structure system pointers of the specified structure. Non-locked materials refering to this structure that become empty as a result of this operation are automatically deleted 
			inline void					Remove(CGS::Geometry<Def>&structure)	{remove(structure);}
			void						remove(CGS::Geometry<Def>*structure);								//!< Removes the structure entity (and children) refering the currently linked structure system pointers of the specified structure. Non-locked materials refering to this structure that become empty as a result of this operation are automatically deleted
			inline void					Remove(CGS::Geometry<Def>*structure)	{remove(structure);}
			void						remove(CGS::StaticInstance<Def>&instance);	//!< Removes the structure entity (and children)  refering the specified structure instance system pointers. Non-locked materials refering to this structure that become empty as a result of this operation are automatically deleted
			inline void					Remove(CGS::StaticInstance<Def>&instance)	{remove(instance);}
			void						remove(CGS::StaticInstance<Def>*instance);							//!< Removes the structure entity (and children)  refering the specified structure instance system pointers. Non-locked materials refering to this structure that become empty as a result of this operation are automatically deleted
			inline void					Remove(CGS::StaticInstance<Def>*instance)	{remove(instance);}
			void						remove(CGS::AnimatableInstance<Def>&instance);	//!< Removes the structure entity (and children)  refering the specified structure instance system pointers. Non-locked materials refering to this structure that become empty as a result of this operation are automatically deleted
			inline void					Remove(CGS::AnimatableInstance<Def>&instance)	{remove(instance);}
			void						remove(CGS::AnimatableInstance<Def>*instance);							//!< Removes the structure entity (and children)  refering the specified structure instance system pointers. Non-locked materials refering to this structure that become empty as a result of this operation are automatically deleted
			inline void					Remove(CGS::AnimatableInstance<Def>*instance)	{remove(instance);}
			void						lockMaterials(CGS::Geometry<Def>&structure);							//!< Loads (unless already loaded) the CGS materials of the specified CGS geometry to materials and locks them. Locked materials are not automatically erased when they become empty thus retaining their order.
			inline void					LockMaterials(CGS::Geometry<Def>&structure)	{lockMaterials(structure);}
			void						lockMaterials(CGS::Geometry<Def>*structure);							//!< Loads (unless already loaded) the CGS materials of the specified CGS geometry to materials and locks them. Locked materials are not automatically erased when they become empty thus retaining their order.
			inline void					LockMaterials(CGS::Geometry<Def>*structure)	{lockMaterials(structure);}
			void						lockMaterial(CGS::MaterialA<Def>&material);		//!< Loads (unless already loaded) a specific CGS material and locks the associated visual. Locked materials are not automatically erased when they become empty thus retaining their order.
			inline void					LockMaterial(CGS::MaterialA<Def>&material)	{lockMaterial(material);}
				
			void						setRenderer(GL*renderer,bool fail_if_not_idle=true);			//!< Sets the renderer to use for subsequent geometry/texture uploads and rendering. Setting the active renderer on a non-idle scenery causes and exception (unless \b fail_if_not_idle is false).
			inline void					SetRenderer(GL*renderer,bool failIfNotIdle=true)	{setRenderer(renderer,failIfNotIdle);}
			void						setRenderer(GL&renderer);			//!< Sets the renderer to use for subsequent geometry/texture uploads and rendering. Setting the active renderer on a non-idle scenery causes and exception.
			inline void					SetRenderer(GL&renderer)	{setRenderer(renderer);}
			GL*							getRenderer()	const;				//!< Retrieves the currently used renderer
			inline GL*					GetRenderer()	const	{return getRenderer();}
				
			void						setConfig(CGS::Geometry<Def>&structure,unsigned status);	//!< Updates the config/detail behavior of the structure entity associated with the currently linked geometry system matrices. The method fails if no such structure entity exists. \param structure Geometry to associate the new status with \param status New status
			inline void					SetConfig(CGS::Geometry<Def>&structure,unsigned status)	{setConfig(structure,status);}
			void						setConfig(CGS::Geometry<Def>*structure,unsigned status);		//!< Updates the config/detail behavior of the structure entity associated with the currently linked geometry system matrices. The method fails if no such structure entity exists. \param structure Geometry to associate the new status with \param status New status
			inline void					SetConfig(CGS::Geometry<Def>*structure,unsigned status)	{setConfig(structure,status);}
			void						setConfig(CGS::StaticInstance<Def>&structure,unsigned status);	//!< Updates the config/detail behavior of the structure entity associated with the geometry instance system matrices. The method fails if no such structure entity exists. \param structure Geometry to associate the new status with \param status New status
			inline void					SetConfig(CGS::StaticInstance<Def>&structure,unsigned status)	{setConfig(structure,status);}
			void						setConfig(CGS::StaticInstance<Def>*structure,unsigned status);	//!< Updates the config/detail behavior of the structure entity associated with the geometry instance system matrices. The method fails if no such structure entity exists. \param structure Geometry to associate the new status with \param status New status
			inline void					SetConfig(CGS::StaticInstance<Def>*structure,unsigned status)	{setConfig(structure,status);}

			unsigned					getConfig(CGS::Geometry<Def>&structure);			//!< Attempts to retrieve the config/detail behavior of the structure entity associated  with the currently linked geometry system matrices. \param structure Geometry to identify the respective structure entity  \return Current config/detail behavior of the associated structure entity or StructureEntity::Invalid if no structure entity could be found
			inline unsigned				GetConfig(CGS::Geometry<Def>&structure)		{return getConfig(structure);}
			unsigned					getConfig(CGS::Geometry<Def>*structure);			//!< Attempts to retrieve the config/detail behavior of the structure entity associated  with the currently linked geometry system matrices. \param structure Geometry to identify the respective structure entity \return Current config/detail behavior of the associated structure entity or StructureEntity::Invalid if no structure entity could be found
			inline unsigned				GetConfig(CGS::Geometry<Def>*structure)		{return getConfig(structure);}
			unsigned					getConfig(CGS::StaticInstance<Def>&structure);			//!< Attempts to retrieve the config/detail behavior of the structure entity associated  with the geometry instance system matrices. \param structure Geometry instance to identify the respective structure entity \return Current config/detail behavior of the associated structure entity or StructureEntity::Invalid if no structure entity could be found
			inline unsigned				GetConfig(CGS::StaticInstance<Def>&structure)		{return getConfig(structure);}
			unsigned					getConfig(CGS::StaticInstance<Def>*structure);			//!< Attempts to retrieve the config/detail behavior of the structure entity associated  with the geometry instance system matrices. \param structure Geometry instance to identify the respective structure entity \return Current config/detail behavior of the associated structure entity or StructureEntity::Invalid if no structure entity could be found
			inline unsigned				GetConfig(CGS::StaticInstance<Def>*structure)		{return getConfig(structure);}
				
			MyStructureEntity* 			searchForEntityOf(CGS::SubGeometryA<Def>&child, index_t&sub_index);
			inline MyStructureEntity* 	SearchForEntityOf(CGS::SubGeometryA<Def>&child, index_t&subIndex)	{return searchForEntityOf(child,subIndex);}
			MyStructureEntity* 			searchForEntityOf(CGS::SubGeometryInstance<Def>&instance, index_t&sub_index);
			inline MyStructureEntity* 	SearchForEntityOf(CGS::SubGeometryInstance<Def>&instance, index_t&subIndex)	{return searchForEntityOf(instance,subIndex);}
			MyStructureEntity* 			findEntityOf(CGS::Geometry<Def>&geometry);					//!< Attempts to find the structure entity associated with the currently bound system matrix of the specified geometry. Operates in O(log n). \return Pointer to the respective structure entity or NULL if no structure entity could be found
			inline MyStructureEntity* 	FindEntityOf(CGS::Geometry<Def>&geometry)			{return findEntityOf(geometry);}
			MyStructureEntity* 			findEntityOf(CGS::StaticInstance<Def>&instance);			//!< Attempts to find the structure entity associated with the system matrix of the specified geometry instance. Operates in O(log n). \return Pointer to the respective structure entity or NULL if no structure entity could be found
			inline MyStructureEntity* 	FindEntityOf(CGS::StaticInstance<Def>&instance)		{return findEntityOf(instance);}

			virtual	void				Rebuild();													//!< Rebuilds materials of the local scenery that require a rebuild
			void						lock();														//!< Increases the local lock variable by one, preventing automatic visual rebuilds (except when rendering) due to changes in the local scenery.
			inline void					Lock()		{lock();}
			void						unlock();													//!< Decreases the local lock variable by one. Subsequent automatic rebuilding is again allowed if the lock variable is reduced to zero by this operation.
			inline void					Unlock()	{unlock();}
				
			virtual	void				clear(bool disconnected=false);		//!< Clears the local scenery. When erasing materials Scenery assumes that the linked CGS::Geometry instances still exist. \param disconnected Pass true if referenced CGS::Geometry/CGS::SubGeometry instances do no longer exist.
			inline void					Clear(bool disconnected=false)	{clear(disconnected);}
			bool						idle();								//!< Queries whether or not data is currently loaded into the scenery \return true if \b no data is currently loaded into the scenery, false otherwise
			inline bool					IsIdle()	{return idle();}
				
			String						state();							//!< Retrieves a string representation of the current scenery state
			inline String				GetState()	{return state();}
				
			typename Def::FloatType*	extractRenderPath(count_t&points);
			inline typename Def::FloatType*	ExtractRenderPath(count_t&points)	{return extractRenderPath(points);}
			std::shared_ptr<Material<GL,Def> > largestMaterial();
			inline std::shared_ptr<Material<GL,Def> > GetLargestMaterial()	{return largestMaterial();}
				
			template <class Def2>
				void					import(Scenery<GL,Def2>&scenery);	//!< Imports rendering data from another scenery (experimental)
			template <class Def2>
				inline void				Import(Scenery<GL,Def2>&scenery)	{import(scenery);}
			/*!
			\brief Determines the detail of all structure entities and the visibility of their respective object entities using cage projection
					
			The determined structure detail is stored in the StructureEntity::detail member variables, object visibility in the ObjectEntity::visible member variables.
			Structures or objects exceeding the specified maximum visual range are classified invisible.
			\param aspect Camera to test visibily against
			*/
			template <class C0, class C1>
				void 					Resolve(const Aspect<C0>&aspect, const C1&resolutionModifier);
		
			template <class C0, class C1>
				void					Render(const Aspect<C0>&aspect, const C1&resolutionModifier);		//!< Invokes Resolve() using the specified parameters and renders the scenery. Invokes postRenderCleanup() after render \param aspect Camera that is currently loaded the rendering context (The scenery does not load this camera)
			template <class C0, class C1>
				void					RenderIgnoreMaterials(const Aspect<C0>&aspect, const C1&resolutionModifier);				//!< Invokes Resolve() using the specified parameters and renders the scenery. Does not bind any shaders. Invokes postRenderCleanup() after render \param aspect Camera that is currently loaded the rendering context (The scenery does not load this camera)
			template <class C0, class C1>
				void					RenderIgnoreShaders(const Aspect<C0>&aspect, const C1&resolutionModifier);				//!< Invokes Resolve() using the specified parameters and renders the scenery. Does not bind any shaders. Invokes postRenderCleanup() after render \param aspect Camera that is currently loaded the rendering context (The scenery does not load this camera)
			void						Render(unsigned detail=0);									//!< Renders the entire scenery in the specified detail disregarding of visibility
			inline void					RenderIgnoreMaterials(unsigned detail=0);
			inline void					RenderIgnoreShaders(unsigned detail=0);
			void						RenderOpaqueMaterials();										//!< Renders non-transparent materials. Required to be embedded between Resolve() and postRenderCleanup()
			void						RenderTransparentMaterials();									//!< Renders transparent materials. Required to be embedded between Resolve() and postRenderCleanup()
		
		protected:
			virtual void				PostRenderCleanup()	{};										//!< Automatically invoked by render(). Derivative classes such as MappedScenery<> require post rendering cleanups.
		};

	/*!
		\brief Recursive octree/octree-node for ObjectEntity instances
		
		The SceneryTree recursivly maps object entities into the tree. Lookup is done by passing a volume (box) to the tree.
		Each SceneryTree instance has one parent (or NULL) and can have up to 8 children.
	*/
	template <class Def=CGS::StdDef>
		class SceneryTree
		{
		public:
				typedef ObjectEntity<Def>			Object;
				typedef StructureEntity<Def>		Structure;
				typedef List::ReferenceVector<Object> 	ReferenceList;
				typedef Sorted<List::ReferenceVector<Object>,PointerSort> 			List;
				typedef typename Def::FloatType		Float;
				typedef SceneryTree<Def>			Tree;
				
				typedef Box<Float>					Volume;
			
		protected:
				List								elements;		//!< (Reference)list of all object entities that are mapped in the local tree node
				Volume								volume;			//!< Effective volume of the local tree node
				TVec3<Float>						split;		//!< Composite used to split the local volume into up to 8 child volumes.
				Tree								*child[8];		//!< Pointer to the respective children (up to 8). Any of these may be NULL.
				Structure							*dominating;	//!< Set non-NULL if the local tree node exclusivly contains objects of the specified structure entity.
				unsigned							level;			//!< Recursive level with 0 being the bottom-most level.
				bool								has_children;	//!< True if at least one SceneryTree::child pointer is not NULL

				void								recursiveRemap(List*source);		//!< Recursivly maps the elements of the specified list to the local tree node overwriting any existing mapped object entities.
				count_t								recursiveLookup(const Volume&space, const Structure*exclude, Buffer<Object*>&buffer);	//!< Volume based recursive lookup call \param space Volume to lookup \param exclude Structure entity to exclude the objects of during lookup or NULL to not exclude any object entities \param buffer Outbuffer for found object entities. The same object entity may be listed multiple times
			template <typename T>
				count_t								recursiveLookup(const Frustum<T>&volume, Buffer<Object*>&buffer);	//!< Visual volume based recursive lookup call \param volume Visual volume to lookup \param buffer Outbuffer for found object entities. The same object entity may be listed multiple times
		public:

													SceneryTree(const Volume&space, unsigned level);	//!< Child node constructor \param space Volume this node should occupy. \param level Recursion level. A node on level 0 may not have child nodes.
													SceneryTree(const Tree&other);					//!< Copy constructor
													SceneryTree(unsigned depth);						//!< Root node constructor \param depth Max recursive tree depth
		virtual										~SceneryTree();
				void								remap(List*source);									//!< Clears the local map and maps the object entities of the specified source \param source List of object entities to map
				void								clear();											//!< Clears the local map
				
				void 								lookup(const Volume&space, Buffer<Object*>&out, const Structure*exclude=NULL);	//!< Volume based lookup call. Determines all object entities that lie inside or on the border of the specified volume \param space Volume to lookup \param out Outbuffer for found object entities. The out buffer will NOT contain the same entry multiple times \param exclude Structure entity to exclude the object entities of or NULL to not exclude any object entities
			template <typename T>
				void								lookup(const Frustum<T>&volume, Buffer<Structure*>&struct_out, Buffer<Object*>&object_out);	//!< Visual volume based lookup call. Determines all object entities of sectors inside or on the borders of the specified visual volume \param volume Visual volume to lookup \param struct_out Outbuffer for (distinct) structure entities \param object_out Outbuffer for (distinct) object entities
				Tree&								operator=(const Tree&other);				//!< Copy operator
				
				Tree*								getChild(BYTE index);			//!< Retrieves the tree child associated with the respective index, \param index Index of the child to retrieve in the range [0,7]. \return Pointer to the respective child or NULL if the child does not exist or the specified index is greater than 7
				Structure*							getDominating();				//!< Retrieves the dominating structure entity of the local tree node.(if any)
				bool								hasChildren();					//!< Returns true if the node has at least one child, false otherwise
				unsigned							getLevel();						//!< Retrieves the recursive level of the local tree node (0 = bottom most node/leaf).
				const TVec3<Float>&					getSplitVector();				//!< Retrieves the split point (3 components) used to determine the volumes of the local children. The returned vector is undefined if the local node has no children.
				const Volume&						getVolume();					//!< Retrieves the actual volume of the local tree node
				count_t								countElements();				//!< Retrieves the number of elements mapped in this node
				void								getElements(List&out);			//!< Copies the pointers of all locally mapped object entities to the target list.
				const List*							getElementList()	const;		//!< Retrieves a link to the local element list containing all mapped entities
		};


	/*!
		\brief Octree mapped version of Scenery

		MappedScenery keeps an octree to map contained object entities, which is updated whenever geometries are mapped
		or unmapped. If objects are changed outside the scenery MappedScenery::remap() needs to be invoked.<br />
		While Scenery is best for fast-changing sceneries with little or no physical interaction,
		MappedScenery can be used to faster render large scale scenarios and lookup potentially interacting objects in O(log n) rather than O(n).

	*/
	template <class GL, class Def=CGS::StdDef>
		class MappedScenery:public Scenery<GL,Def>
		{
		public:
			typedef	Engine::StructureEntity<Def>	MyStructureEntity;
			typedef Scenery<GL,Def>					Super;
				
		protected:
				Buffer<MyStructureEntity*>			structure_buffer;	//!< Buffer used to store structure lookup results
				Buffer<ObjectEntity<Def>*>			object_buffer;		//!< Buffer used to store object lookup results

		public:
				SceneryTree<Def>					tree;				//!< Recursive octree containing the local scenery
				Sorted<List::ReferenceVector<ObjectEntity<Def> >,PointerSort>	objects;	//!< List of currently mapped object entities
		
		public:
				//typedef Scenery<GL,Def>				Scenery;
		
													MappedScenery(unsigned tree_depth=4, TextureTable<GL>*table=NULL);
													MappedScenery(unsigned tree_depth, GL*renderer, TextureTable<GL>*table=NULL);
													MappedScenery(unsigned tree_depth, GL&renderer, TextureTable<GL>*table=NULL);
		
		
				MyStructureEntity*					embed(CGS::StaticInstance<Def>&instance,unsigned detail_type=StructureConfig::Default);
				MyStructureEntity*					embed(CGS::StaticInstance<Def>*instance,unsigned detail_type=StructureConfig::Default);
				MyStructureEntity*					embed(CGS::AnimatableInstance<Def>&instance,unsigned detail_type=StructureConfig::Default);
				MyStructureEntity*					embed(CGS::AnimatableInstance<Def>*instance,unsigned detail_type=StructureConfig::Default);
				MyStructureEntity*					embed(CGS::Geometry<Def>&structure,unsigned detail_type=StructureConfig::Default);
				MyStructureEntity*					embed(CGS::Geometry<Def>*structure,unsigned detail_type=StructureConfig::Default);
				void								remove(CGS::Geometry<Def>&structure);
				void								remove(CGS::Geometry<Def>*structure);
				void								remove(CGS::StaticInstance<Def>&instance);
				void								remove(CGS::StaticInstance<Def>*instance);
				void								remove(CGS::AnimatableInstance<Def>&instance);
				void								remove(CGS::AnimatableInstance<Def>*instance);
				
				virtual void						Rebuild()	override;	//!< Rebuilds modified materials (Scenery::rebuild()) and remaps the local scenery
				void								remap();	//!< Remaps the local scenery. Invoke this whenever objects have been moved. If the local scenery is static then this method need not be called manually 
				
			template <typename T>
				void 								lookup(const Box<T>&vol, Buffer<ObjectEntity<Def>*>&out, const MyStructureEntity*exclude=NULL);	//!< Performs a recursive lookup on the local tree. For this method to work correctly the local tree is required to be up to date. \param lower Lower corner of the lookup volume \param upper Upper corner of the lookup volume \param out Outbuffer for (distinct) found object entities \param exclude Structure entity to exclude the object entities of or NULL to not exclude any object entities
				
				void								clear(bool disconnected=false) override; 		//!< Clears the local scenery. When erasing materials Scenery assumes that the linked CGS::Geometry instances still exist. \param disconnected Pass true if referenced CGS::Geometry/SubGeometry instances do no longer exist-
				
			template <class Def2>
				void								import(Scenery<GL,Def2>&scenery);		//!< Imports the content of the specified other scenery
				/*!
					\brief Determines the detail of all structure entities and the visibility of their respective child object entities using visual volume extraction and recursive tree lookups. 
					
					The determined structure detail is stored in the StructureEntity::detail member variables, the respective object visibility in the ObjectEntity::visible member variables.
					Object entities whoes sphere does not at least intersect the visual volume of \b aspect are classified invisible.
					The method may produce undesired results if the local tree mapping is outdated. Use remap() if some or all local entities have moved.
					
					\param aspect Camera to test visibily against
				*/
			template <class C0, class C1>
				void 								Resolve(const Aspect<C0>&aspect, const C1&resolutionModifier);
	

			template <class C0, class C1, class C2, class C3>
				ObjectEntity<Def>*					lookupClosest(const TVec3<C0>&center, const C1&radius, TVec3<C2>&position_out, TVec3<C3>&normal_out);	//!< Determines the closest hull point approximately inside the specified sphere. Invisible entities are ignored. The method does not miss points that should be inside but may return a point that lies outside the specified radius. \param center Sphere center \param radius Sphere radius, \param position_out 3 component out vector for the closest hull point \param normal_out 3 component out vector for the normal of the closest hull point \return Pointer to the closest object entity or NULL if no object is close enough to the sphere center.
		
		protected:
			virtual void						PostRenderCleanup() override;						//!< Perform post render cleanup
		};


}

/*
template <class GL>	class Renderable //what were these for...?
{};


template <class GL>	class RenderContext
{};

*/


#include "scenery.tpl.h"

#endif
