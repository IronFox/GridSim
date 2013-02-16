#ifndef engine_sceneryTplH
#define engine_sceneryTplH


/******************************************************************

Language- and DefStruct-dependent 3d-environment renderer.
The scenery handles most of the internal settings in order
to provide a simplified interface.

******************************************************************/

namespace Engine
{


	template <typename T0, typename T1>
		inline void	copy2(const T0 from[2], T1 to[2])
		{
			to[0] = from[0];
			to[1] = from[1];
		}
	template <typename T0, typename T1>
		inline void	copy3(const T0 from[3], T1 to[3])
		{
			to[0] = from[0];
			to[1] = from[1];
			to[2] = from[2];
		}

	template <typename T0, typename T1>
		inline void	copy4(const T0 from[4], T1 to[4])
		{
			to[0] = from[0];
			to[1] = from[1];
			to[2] = from[2];
			to[3] = from[3];
		}



	template <class GL>
		bool	TextureTable<GL>::createLink(CGS::TextureA*source, const typename GL::Texture&object)
		{
		    SCENERY_LOG("linking texture");
		    if (!source || object.isEmpty())
		        return false;
		    if (source->signature == this && source->reference)
		    {
		        SCENERY_LOG("previously referenced");
		        ReferencedTexture<GL>*link = reinterpret_cast<ReferencedTexture<GL>*>(source->reference);
		        return link->external == &object;
		    }
		    SCENERY_LOG("not previously referenced - relocating");


			ReferencedTexture<GL>*link = storage.lookup(source);
			if (link)
			{
				SCENERY_LOG("found. linking to host and returning.");

				source->signature = this;
				source->reference = link;
		        return link->object == object;
			}


		    SCENERY_LOG("lookup failed - mapping");

			link = storage.define(source);

			ASSERT_NOT_NULL__(link);


		    link->external = &object;
		    SCENERY_LOG("linking to host");
		    source->signature = this;
		    source->reference = link;
			
			//if (!source->face_field.length())
			{
				if (object.textureDimension()==MaterialLayer::TextureDimension::Cube)
					source->face_field.resize(6);
				else
					source->face_field.resize(1);
			}
			
		    SCENERY_LOG("returning.");
		    return true;
		}

	template <class GL> bool TextureTable<GL>::retrieve(CGS::TextureA*source, typename GL::Texture::Reference&target, Engine::MaterialLayer&layer)
	{
		SCENERY_LOG("attempting to retrieve texture 0x"+PointerToHex(source));
	    if (!source)
		{
			target = typename GL::Texture::Reference();
			return false;
		}
	    SCENERY_LOG("retrieving texture '"+name2str(source->name)+"'");

		if (!source->face_field.length())
		{
			target = typename GL::Texture::Reference();
			return false;
		}

		ReferencedTexture<GL>*link = storage.lookup(source);
		if (link)
		{
	        SCENERY_LOG("found. linking to host and returning.");

	        link->usage++;
			target = link->reference();
			layer.content_type = link->content_type;
	        return true;
	    }

	    SCENERY_LOG("lookup failed - creating");

		link = storage.define(source);
		ASSERT_NOT_NULL__(link);

	    link->usage = 1;

	    BYTE num = BYTE(source->face_field.length());
	    if (num != 6)
	        num = 1;
	    SCENERY_LOG("faces set to "+String(num)+". extracting...");

	    Array<Image*> extracted(num);
		bool failed = false;
	    for (BYTE k = 0; k < num; k++)
		{
			SCENERY_LOG("extracting face "+String(k)+" ("+String(source->face_field[k].size())+" bytes)");
	        extracted[k] = TextureCompression::decompress(source->face_field[k].pointer(),source->face_field[k].size());
			if (extracted[k])
				SCENERY_LOG("extracted ("+String(extracted[k]->getWidth())+"x"+String(extracted[k]->getHeight())+"x"+String(extracted[k]->getChannels())+")")
			else
			{
				SCENERY_LOG("extraction failed ("+TextureCompression::getError()+")");
				failed = true;
			}
		}
	    SCENERY_LOG("extracted. sending data to interface...");
		if (!failed)
		{
			link->content_type = layer.content_type = extracted[0]->getContentType();
			if (num >= 6)
				link->loadCube(*extracted[0],*extracted[1],*extracted[2],*extracted[3],*extracted[4],*extracted[5]);
			else
				link->load(*extracted[0],anisotropy);
		}
		else
			link->clear();

		if (link->isEmpty())
			SCENERY_LOG("failed to load. cleaning up...")
		else
			SCENERY_LOG("loaded (dimension #"+String(link->dimension())+"). cleaning up...");

	    for (BYTE k = 0; k < num; k++)
	        if (extracted[k])
	            DISCARD(extracted[k]);

	    SCENERY_LOG("returning...");

	    target = link->reference();
		return true;
	}

	template <class GL> void TextureTable<GL>::discard(CGS::TextureA*source)
	{
	    if (!source)
	        return;
		ReferencedTexture<GL>*link = storage.lookup(source);
		if (!link)
		{
			if (source->name == 0)	//dummy texture. never loaded
				return;
	        FATAL__("trying to erase non-allocated texture");
			return;
		}

	    link->usage--;
	    if (!link->usage)
	    {
			storage.erase(source);
			//source->signature = this;
			//source->reference = NULL;
	        return;
	    }
	    //source->signature = this;
	    //source->reference = link;
	}


	template <class Def> ObjectEntity<Def>::ObjectEntity(CGS::SubGeometryA<Def>*target, StructureEntity<Def>*super):system(target->system_link),
	                                                                                  visible(false),client_visible(false),added(false),invert_set(false),structure(super),source(target),shortest_edge_length(target->meta.shortest_edge_length),
																					  radius(0),sys_scale(1)
	{
		SCENERY_LOG("extracting geometry information");
	    target->extractDimensions(dim);
		target->extractRadius(radius);
		if (!radius)
			radius = 1;
		SCENERY_LOG(" dim = "+dim.toString());
		SCENERY_LOG(" radius = "+String(radius));
		//target->extractAverageVisualEdgeLength(0,average_edge_length);
	    rescale();
	}




	template <class Def> void ObjectEntity<Def>::rescale()
	{
	    sys_scale = (typename Def::FloatType)(Vec::length(system->x.xyz)+Vec::length(system->y.xyz)+Vec::length(system->z.xyz))/3;
	    updateCage();
	}

	template <class Def> void ObjectEntity<Def>::updateCage()
	{
	    Vec::def(cage[0],dim.x.min,dim.y.min,dim.z.min,1);
	    Vec::def(cage[1],dim.x.min,dim.y.min,dim.z.max,1);
	    Vec::def(cage[2],dim.x.min,dim.y.max,dim.z.min,1);
	    Vec::def(cage[3],dim.x.min,dim.y.max,dim.z.max,1);
	    Vec::def(cage[4],dim.x.max,dim.y.min,dim.z.min,1);
	    Vec::def(cage[5],dim.x.max,dim.y.min,dim.z.max,1);
	    Vec::def(cage[6],dim.x.max,dim.y.max,dim.z.min,1);
	    Vec::def(cage[7],dim.x.max,dim.y.max,dim.z.max,1);
	    /*
	    Vec::multiply(cage[0],sys_scale);
	    Vec::multiply(cage[1],sys_scale);
	    Vec::multiply(cage[2],sys_scale);
	    Vec::multiply(cage[3],sys_scale);
	    Vec::multiply(cage[4],sys_scale);
	    Vec::multiply(cage[5],sys_scale);
	    Vec::multiply(cage[6],sys_scale);
	    Vec::multiply(cage[7],sys_scale);*/
	}


	template <class Def>
		void	StructureEntity<Def>::createEntity(CGS::SubGeometryA<Def>&child)
		{
			object_entities << ObjectEntity<Def>(&child,this);
			object_entity_map.set(object_entities.last().system,object_entities.count()-1);

			for (index_t i = 0; i < child.child_field.length(); i++)
				createEntity(child.child_field[i]);
		}

	template <class Def> StructureEntity<Def>::StructureEntity(CGS::Geometry<Def>*target_,unsigned type):system(target_->system_link),target(target_),detail(0),max_detail(target_->maxDetail()),src_radius(1.0),
	                                              sys_scale(1),visible(false),added(false),radius(1.0),config(type),tint(Vector4<>::zero), shortest_edge_length(std::numeric_limits<typename Def::FloatType>::max())
	{
		for (index_t i = 0; i < target->object_field.length(); i++)
			createEntity(target->object_field[i]);
		object_entities.compact();

		//target->extractAverageVisualEdgeLength(0,average_edge_length);
		for (auto it = object_entities.begin(); it != object_entities.end(); ++it)
			shortest_edge_length = vmin(shortest_edge_length,it->shortest_edge_length);
	    target->extractRadius(src_radius);
		if (!src_radius)
			src_radius = 1.0;
		radius = src_radius;
	    rescale();
	/*
	    if (radius)
	        near_scale = (typename Def::FloatType)target->countVertices()/(4.0/3*M_PI*radius*radius*radius)*GLOBAL_NEAR_SCALE;
	*/
	}

	template <class Def>
		index_t	StructureEntity<Def>::indexOf(CGS::SubGeometryA<Def>*child)
		{
			return object_entity_map.get(child->system_link,InvalidIndex);
		}


	template <class Def> void StructureEntity<Def>::rescale()
	{
	    sys_scale = (typename Def::FloatType)(Vec::length(system->x.xyz)+Vec::length(system->y.xyz)+Vec::length(system->z.xyz))/3;
	    //radius = src_radius*sys_scale;
	}








	#if 0
	template <class Def> VisualObject<Def>::VisualObject(CGS::RenderObject<Def>&obj):ident(obj),global_shutdown(false),offset(obj.detail_layer_field.length())
	{
	    update();
	}

	template <class Def> VisualObject<Def>::~VisualObject()
	{
		if (!global_shutdown)
			ident.reference = NULL;
	}

	#endif

/*
	template <class C> static inline void checkRange(const C*v, unsigned len, StringList*warn_out)
	{
		for (unsigned i = 0; i < len; i++)
		    if (isnan(v[i]))
		    {
		        (*warn_out) << "coord "+String(i)+" is nan: "+_toString(v,len);
		        return;
			}
			else
			    if (v[i] < -10000 || v[i] > 10000)
			    {
		    	    (*warn_out) << "range exceeded("+String(i)+"): "+_toString(v,len);
		    	    return;
				}
				else
					SCENERY_LOG(v[i]);

	}
*/

	#define checkRange(a,b,c)	//disable range check

	#if 0
	template <class Def>
	template <class C0, class C1> void VisualObject<Def>::write(C0*v_out, Index&v_offset, C1*i_out, Index&i_offset, unsigned vsize,StringList&warn_out)
	{
	    SCENERY_LOG("writing render object '"+name2str(ident.target->name)+"' to field at offset "+String(i_offset)+". target vsize is "+String(vsize));
	    if (vsize != ident.vpool.vsize())
	        warn_out << "vsize mismatch (should be "+String(vsize)+" but is "+String(ident.vpool.vsize())+"("+String(ident.vpool.vsize())+"))";

	    for (unsigned i = 0; i < ident.detail_layer_field.length(); i++)
	    {
	        Vec::add(ident.detail_layer_field[i].idata.pointer(),v_offset,i_out + i_offset,ident.detail_layer_field[i].idata.length());
	        offset[i] = i_offset;
	        i_offset+=ident.detail_layer_field[i].idata.length();
	    }
	    
		SCENERY_LOG("vertex-pool does not have additional data");
		Vec::copy(ident.vpool.vdata.pointer(),v_out + v_offset*vsize,ident.vpool.vlen());
		checkRange(v_out+v_offset*vsize,ident.vpool.vlen(),warn_out);

		
	    v_offset += ident.vpool.vcnt;
	    SCENERY_LOG("done writing to field");
	}

	template <class Def> void VisualObject<Def>::update()
	{
	    ilen = 0;
	    for (unsigned i = 0; i < ident.detail_layer_field.length(); i++)
	        ilen += ident.detail_layer_field[i].idata.length();
	}

	#endif





	template <class Def>
		void VisualDetail<Def>::adoptData(VisualDetail<Def>&other)
		{
			objects.swap(other.objects);
			robjects.swap(other.robjects);
			offset.adoptData(other.offset);
			object_index.adoptData(other.object_index);
		}
	
	template <class Def>
		void	RenderGroup<Def>::merge(CGS::MaterialA<Def>&material, StructureEntity<Def>*entity, UINT32&flags)
		{
			
			SCENERY_LOG("merging material '"+material.name+"' ("+String(material.data.object_field.length())+" object(s)) for entity of 0x"+PointerToHex(entity->target));
		    for (index_t i = 0; i < material.data.object_field.length(); i++)
		    {
		        CGS::RenderObjectA<Def>&obj = material.data.object_field[i];
		        SCENERY_LOG("merging render object ("+String(i)+") '"+obj.target->name+"'");
				robjects.append(&obj);
				
				if (obj.detail >= detail_layers.count())
					detail_layers.resizePreserveContent(obj.detail+1);
				
				VisualDetail<Def>*detail = detail_layers+obj.detail;
				detail->robjects.append(&obj);
				detail->object_index << entity->indexOf(obj.target);
				
				flags |= obj.vpool.vflags;
		        SCENERY_LOG("done merging render object '"+obj.target->name+"'");
		    }
			for (index_t i = 0; i < detail_layers.count(); i++)
				detail_layers[i].object_index.compact();
		
		}



	template <class GL, class Def> Material<GL,Def>::Material(Scenery<GL,Def>*parent, GL*renderer_,CGS::MaterialInfo&source,TextureTable<GL>*table):owner(parent), renderer(renderer_),info(source),vertex_flags(0),texture_table(table),locked(false),requires_rebuild(false),initialized(false),additional(0)
	{
	    SCENERY_LOG("creating visual");
	    coord_layers = UINT32(source.countCoordLayers());
		
		SCENERY_LOG("counted "+String(coord_layers)+" coordinate layers");
		//coord_layers = 0;
		
		
	}

	template <class GL, class Def>
		void	Material<GL,Def>::init()
		{
			ASSERT_NOT_NULL__(texture_table);

			material.read(info,vertex_flags);
			binding.read(info,vertex_flags);

			SCENERY_LOG("texture field resized to "+String(material.textures.count()));
			for (index_t i = 0; i < info.layer_field.length(); i++)
			{
				texture_table->retrieve(info.layer_field[i].source,material.textures[i],material.layers[i]);
				SCENERY_LOG("extracted texture is of dimension #"+String(material.textures[i].target() ? String(material.textures[i].target()->dimension()) : String("???")));
			}
			typename GL::Shader::SourceCode	code;
			ASSERT__(GL::Shader::compose(material,code));
			ASSERT1__(shader.create(code),shader.report());
			SCENERY_LOG("visual created");

			initialized = true;
		}

	/*
	template <class GL, class Def> Material<GL,Def>::Material():info(NULL),object(NULL),objects(0),index_object(GL::emptyIndexObject()),
	                                                 vbo(GL::emptyVertexObject()),texture_object(NULL),vertex_band(0),coord_layers(0)
	{}*/

	template <class GL, class Def> Material<GL,Def>::~Material()
	{
		if (application_shutting_down)
			return;
		if (info.attachment.lock())
		{
		    for (index_t i = 0; i < info.layer_field.length(); i++)
			{
				SCENERY_LOG("discarding texture on layer "+String(i));
		        texture_table->discard(info.layer_field[i].source);
			}
		}
		SCENERY_LOG("discarding buffers");
		vbo.clear();
		ibo.clear();

		SCENERY_LOG("visual deleted");
	}

	template <class GL, class Def>
		void	Material<GL,Def>::shutdown()
		{
			info.layer_field.free();
		}



	template <class GL, class Def> void Material<GL,Def>::Rebuild(StringList*warn_out, bool enforce_rebuild)
	{
		if (!enforce_rebuild && !requires_rebuild)
			return;
		if (!renderer)
		{
			renderer = GL::global_instance;
			if (!renderer)
				FATAL__("renderer is NULL");
		}
	    SCENERY_LOG("rebuilding");
		
		count_t	vlen = 0,
				ilen = 0,
				vertex_size = VSIZE(coord_layers,vertex_flags);
		groups.reset();
		while (RenderGroup<Def>*group = groups.each())
		{
			group->robjects.reset();
			while (CGS::RenderObjectA<Def>*robject = group->robjects.each())
			{
				vlen += robject->vpool.vcnt * vertex_size;
				ilen += robject->ipool.idata.length();
			}
		}
		
	    buffer.vertex_field.setSize(vlen);
		buffer.index_field.setSize(ilen);
	    SCENERY_LOG("buffer resized ("+String(vlen)+", "+String(ilen)+")");
	    typename Def::IndexType v_offset(0),i_offset(0);
	    SCENERY_LOG("writing sections");
		count_t		vertex_count = vlen/vertex_size;
		groups.reset();
		while (RenderGroup<Def>*group = groups.each())
		{
				
			for (index_t d = 0; d < group->detail_layers.count(); d++)
			{
				VisualDetail<Def>&detail = group->detail_layers[d];
				SCENERY_LOG("writing detail "+String(d));
				detail.offset.setSize(detail.robjects);
				for (index_t o = 0; o < detail.robjects.count(); o++)
				{
					CGS::RenderObjectA<Def>*robj = detail.robjects[o];
					detail.offset[o] = i_offset;
					
				    SCENERY_LOG("writing render object 0x"+PointerToHex(robj)+" to field at offset "+String(i_offset)+". target vsize is "+String(vertex_size));
				    if (vertex_size != robj->vpool.vsize())
						if (warn_out)
							(*warn_out) << "vsize mismatch (should be "+String(vertex_size)+" but is "+String(robj->vpool.vsize())+"("+String(robj->vpool.vsize())+"))";

				    Vec::addValD(robj->ipool.idata.pointer(),v_offset,buffer.index_field + i_offset,robj->ipool.idata.length());
				    
					if (vertex_size == robj->vpool.vsize())
					{
						SCENERY_LOG("same vertex size detected. copying "+String(robj->vpool.vlen())+" floats");
						Vec::copyD(robj->vpool.vdata.pointer(),buffer.vertex_field + v_offset*vertex_size,robj->vpool.vlen());
					}
					else
					{
						SCENERY_LOG("diversive vertex size detected. copying "+String(robj->vpool.vlen())+" floats");
						unsigned stride = robj->vpool.vsize();
						for (unsigned i = 0; i < robj->vpool.vcnt; i++)
						{
							const typename GL::FloatType	*vfrom = robj->vpool.vdata+i*stride;
							typename GL::FloatType			*vto = buffer.vertex_field + v_offset*vertex_size + i*vertex_size;
							CGS::copy3(vfrom,vto);
							vfrom += 3;
							vto += 3;
							
							if (!!(vertex_flags&CGS::HasNormalFlag) && !!(robj->vpool.vflags&CGS::HasNormalFlag))
							{
								copy3(vfrom,vto);
								vfrom += 3;
								vto += 3;
							}
							elif (!!(vertex_flags&CGS::HasNormalFlag))
								vto += 3;
							if (!!(vertex_flags&CGS::HasTangentFlag) && !!(robj->vpool.vflags&CGS::HasTangentFlag))
							{
								copy3(vfrom,vto);
								vfrom += 3;
								vto += 3;
							}
							elif (!!(vertex_flags&CGS::HasTangentFlag))
								vto += 3;
							if (!!(vertex_flags&CGS::HasColorFlag) && !!(robj->vpool.vflags&CGS::HasColorFlag))
							{
								copy4(vfrom,vto);
								vfrom += 4;
								vto += 4;
							}
							elif (!!(vertex_flags&CGS::HasColorFlag))
								vto += 4;
							unsigned min = vmin(coord_layers,robj->vpool.vlyr);
							for (unsigned i = 0; i < min; i++)
							{
								copy2(vfrom,vto);
								vfrom += 2;
								vto += 2;
							}
							for (unsigned i = min; i < coord_layers; i++)
							{
								copy2(vfrom-2,vto);
								vto += 2;
							}
						}
					
					}
					SCENERY_LOG("checking range...");
					checkRange(buffer.vertex_field+v_offset*vertex_size,robj->vpool.vlen(),warn_out);

				    SCENERY_LOG("done writing to field");

					v_offset += robj->vpool.vcnt;
					i_offset += Def::IndexType(robj->ipool.idata.length());
				}
			}
		}
		ASSERT_EQUAL__(v_offset,vertex_count);
		ASSERT_EQUAL__(i_offset,ilen);

		bool errors=false;
		for (index_t i = 0; i < buffer.index_field.length() && !errors; i++)
	        if (buffer.index_field[i] >= vertex_count)
	        {
				count_t my_count(0);
				groups.reset();
				while (RenderGroup<Def>*group = groups.each())
					for (index_t j = 0; j < group->robjects.count(); j++)
						my_count += group->robjects[j]->vpool.vcnt;
				if (my_count != vertex_count)
					FATAL__("counted "+String(my_count)+" vertices. calculated count was "+String(vertex_count));
					
				errors = true;
				if (warn_out)
					(*warn_out) << "index "+String(i)+"/"+String(buffer.index_field.length())+" ("+String(buffer.index_field[i])+") exceeds vertex limit ("+String(vertex_count)+")\nmore may follow. material disabled.";
			}
		if (errors)
		{
		    SCENERY_LOG("errors encountered. disabling");
			initialized = false;
			vertex_flags = 0;
		    return;
		}
	    SCENERY_LOG("done writing sections");
		vbo.load(buffer.vertex_field);
		ibo.load(buffer.index_field);
		SCENERY_LOG("data buffered ("+String(vertex_count)+", "+String(buffer.index_field.length())+")");
		
	/* 	if (eve_log)
		{
			SCENERY_LOG("vertices:\r\n");
			for (unsigned i = 0; i < vertex_count; i++)
				(*eve_log)<<_toString(buffer.vertex_field+i*vertex_size,vertex_size)<<nl;
			SCENERY_LOG("indices:\r\n");
			for (unsigned i = 0; i < buffer.index_length/3; i++)
				(*eve_log)<<_toString(buffer.index_field+i*3)<<nl;
		}
	 */	
		requires_rebuild = false;

	    SCENERY_LOG("done rebuilding");
	}

	template <class GL, class Def> void Material<GL,Def>::unmap(StructureEntity<Def>*entity, bool rebuild_, StringList&warn_out)
	{
	    SCENERY_LOG("unmapping entity");
		
		RenderGroup<Def>*group;
		if (!group_map.query(entity,group))
		{
			SCENERY_LOG("entity is not mapped in this visual");
			return;
		}
		group->entities.drop(entity);
		group_map.unSet(entity);
		if (!group->entities && !locked)
		{
			groups.erase(group);
			if (rebuild_)
				Rebuild(&warn_out,true);
			else
				requires_rebuild = true;
		}
	    SCENERY_LOG("done unmapping");
	}

	template <class GL, class Def> bool Material<GL,Def>::similar(const CGS::MaterialA<Def>&material)
	{
	    return (material.info) == (info);
	}

	template <class GL, class Def> bool Material<GL,Def>::similar(const Material<GL,Def>&that)
	{
		return this->material == that.material;
	}


	template <class GL, class Def> void Material<GL,Def>::merge(StructureEntity<Def>*entity,CGS::MaterialA<Def>&material, bool rebuild_,StringList&warn_out)
	{
	    if (groups.count() &&!info.similar(material.info))
	        FATAL__("bad merge");
		
		RenderGroup<Def>*group;
		
		if (group_map.query(entity,group))
		{
			SCENERY_LOG("entity already mapped.");
			
			group->merge(material,entity,vertex_flags);
			if (!initialized)
				init();
		    if (rebuild_)
		        Rebuild(&warn_out,true);
			else
				requires_rebuild = true;
			
			return;
		}
		group = groups.lookup(&material);
		if (!group)
		{
			group = groups.add(&material);
			
			group->merge(material,entity,vertex_flags);
			if (!initialized)
				init();
		    if (rebuild_)
		        Rebuild(&warn_out,true);
			else
				requires_rebuild = true;
		}
		
		group->entities.insert(entity);
		group_map.set(entity,group);
		
		
	    SCENERY_LOG("done merging material '"+material.name+"'");

	}

	template <class GL, class Def> void Material<GL,Def>::render(bool ignore_materials /*=false*/)
	{
		if (!renderer)
		{
			renderer = GL::global_instance;
			if (!renderer)
				FATAL__("trying to load texture without renderer");
		}
		if (requires_rebuild)
			Rebuild(NULL);
	    if (!initialized)
	        return;

		if (!ignore_materials)
			renderer->bindMaterial(material,shader);

	    renderer->bindVertices(vbo,binding);
	    renderer->bindIndices(ibo);
		
		groups.reset();
		while (RenderGroup<Def>*group = groups.each())
	    {
			group->entities.reset();
			while (StructureEntity<Def>*entity = group->entities.each())
			{
				renderer->overrideEmission(entity->tint);

	            if (entity->visible && entity->detail < group->detail_layers.count())
	            {
					const VisualDetail<Def>&detail = group->detail_layers[entity->detail];
					
					for (index_t i = 0; i < detail.object_index.fillLevel(); i++)
					{
						const CGS::RenderObjectA<Def>*r = detail.robjects[i];
	                    const CGS::IndexContainerA<Def>&d = r->ipool;
						const ObjectEntity<Def>&oentity = entity->object_entities[detail.object_index[i]];
						if (!oentity.visible)
							continue;
					
		                unsigned    at(detail.offset[i]),
		                            cnt(d.triangles*3);

		                renderer->enterSubSystem(*oentity.system);
		                    renderer->render(at,cnt);
		                    at+=cnt;
							renderer->renderQuads(at,d.quads*4);
							at+=d.quads*4;
		                renderer->exitSubSystem();
					}
				}
			}
		}
	}


	template <class GL, class Def> void Material<GL,Def>::extractRenderPath(List::Vector<typename Def::FloatType>&out)
	{
	/*    unsigned vsize = VSIZE(vertex_band,coord_layers,0);
	    typename Def::IndexType last[3]={UNSIGNED_UNDEF,UNSIGNED_UNDEF,UNSIGNED_UNDEF},now[3];
	    typename Def::FloatType lastp[3],nowp[3];
	    for (unsigned i = 0; i < path_len; i++)
	        for (unsigned j = 0; j < path[i]->section[1]; j+=3)
	        {
	            copy3(&buffer.index_field[j+path[i]->section[0]],now);
	            BYTE matches(0);
	            for (BYTE k = 0; k < 3; k++)
	                for (BYTE l = 0; l < 3; l++)
	                    if (now[k] == last[l])
	                        matches++;
	            copy3(&buffer.vertex_field[now[0]*vsize],nowp);
	            Vec::add(nowp,&buffer.vertex_field[now[1]*vsize]);
	            Vec::add(nowp,&buffer.vertex_field[now[2]*vsize]);
	            Vec::divide(nowp,3);
	            if (matches == 2)
	            {
	                copy3(lastp,out.append(SHIELDED_ARRAY(new typename Def::FloatType[3],3)));
	                copy3(nowp,out.append(SHIELDED_ARRAY(new typename Def::FloatType[3],3)));
	            }
	            copy3(nowp,lastp);
	            copy3(now,last);
	        }*/
	}

	template <class GL, class Def> bool Material<GL,Def>::isOpaque()
	{
		if (info.alpha_test)
			return true;

		for (index_t i = 0; i < info.layer_field.length(); i++)
			if (material.textures[i].isTransparent())
				return false;
		
		return true;

	}

	template <class GL, class Def> String Material<GL,Def>::state()
	{
		ASSERT__(material.isConsistent());
		ASSERT_EQUAL__(material.textures.count(),binding.texcoords.count());

	    String rs = " "+String(material.layers.count())+" layer(s)\n";
	    for (index_t i = 0; i < material.textures.count(); i++)
	    {
			const MaterialLayer&layer = material.layers[i];

	        rs+="  ";
	        if (material.textures[i].isEmpty())
	            rs+="(empty)";
	        else
				rs += TextureDimension::toString(material.textures[i].dimension());
	        rs+="\n";
	    }
	    rs+= " "+String(groups.count())+" group(s)\n";
	    groups.reset();
	    while (RenderGroup<Def>*group = groups.each())
	    {
			count_t		vertices(0),
						indices(0);
			group->robjects.reset();
			while (CGS::RenderObjectA<Def>*robject = group->robjects.each())
			{
				vertices += robject->vpool.vcnt;
				indices += robject->ipool.idata.length();
			}
	        rs+="  "+String(vertices)+" vertices and "+(group->entities.count()==1?String("one entity"):String(group->entities.count())+" entities")+" | ilen="+String(indices)+"\n";
	    }
	    rs+= " compiled: vlen=" +String(buffer.vertex_field.length())+" ilen="+String(buffer.index_field.length())+"\n";
	    return rs;
	}

	/**
		@brief Attemps to find the structure entity that governs the specified sub geometry's system_link

		@param[out] sub_index Index of the respective object entity in the object entity field of the returned structure. The value is undefined if the method returns NULL
		@return Pointer to the governing structure entity, or NULL if no such exists
	*/
	template <class GL, class Def> StructureEntity<Def>* Scenery<GL,Def>::searchForEntityOf(CGS::SubGeometryA<Def>&object, index_t&sub_index)
	{
		structures.reset();
		while (StructureEntity<Def>*structure = structures.each())
		{
			sub_index = structure->indexOf(&object);
			if (sub_index != InvalidIndex)
				return structure;
		}
		return NULL;
	}
	

	/**
		@brief Attemps to find the structure entity that governs the specified sub geometry instance's path

		@param[out] sub_index Index of the respective object entity in the object entity field of the returned structure. The value is undefined if the method returns NULL
		@return Pointer to the governing structure entity, or NULL if no such exists
	*/
	template <class GL, class Def> StructureEntity<Def>* Scenery<GL,Def>::searchForEntityOf(CGS::SubGeometryInstance<Def>&instance, index_t&sub_index)
	{
		structures.reset();
		while (StructureEntity<Def>*structure = structures.each())
		{
			sub_index = structure->object_entity_map.get(&instance.path,InvalidIndex);
			if (sub_index != InvalidIndex)
				return structure;
		}
		return NULL;
	}




	template <class GL, class Def> void Scenery<GL,Def>::Rebuild()
	{
	    if (locked)
	        return;
		if (!renderer)
		{
			setRenderer(GL::global_instance,false);
			if (!renderer)
				FATAL__("trying to rebuild scenery without renderer");
		}
			
	    SCENERY_LOG("rebuilding scenery");
		foreach (all_materials,my_material)
			(*my_material)->Rebuild(&warnings);
	    SCENERY_LOG("done rebuilding scenery");
	}

	template <class GL, class Def> void Scenery<GL,Def>::setConfig(CGS::Geometry<Def>&structure,unsigned config)
	{
	    StructureEntity<Def>*super = structures.lookup(structure.system_link);
	    if (super)
	        super->config = config;
	}

	template <class GL, class Def> void Scenery<GL,Def>::setConfig(CGS::Geometry<Def>*structure,unsigned config)
	{
	    StructureEntity<Def>*super = structures.lookup(structure->system_link);
	    if (super)
	        super->config = config;
	}

	template <class GL, class Def> void Scenery<GL,Def>::setConfig(CGS::StaticInstance<Def>&structure,unsigned config)
	{
	    StructureEntity<Def>*super;
	    if (structure.signature == this && structure.reference)
	        super = (StructureEntity<Def>*)structure.reference;
	    else
	    {
	        super = structures.lookup(&structure.matrix);
	        if (super)
	        {
	            structure.signature = this;
	            structure.reference = super;
	        }
	    }
	    if (super)
	        super->config = config;
	}

	template <class GL, class Def> void Scenery<GL,Def>::setConfig(CGS::StaticInstance<Def>*structure,unsigned config)
	{
	    StructureEntity<Def>*super;
	    if (structure->signature == this && structure->reference)
	        super = (StructureEntity<Def>*)structure->reference;
	    else
	    {
	        super = structures.lookup(&structure->matrix);
	        if (super)
	        {
	            structure->signature = this;
	            structure->reference = super;
	        }
	    }
	    if (super)
	        super->config = config;
	}

	template <class GL, class Def> unsigned Scenery<GL,Def>::getConfig(CGS::Geometry<Def>&structure)
	{
	    unsigned rs = StructureConfig::Invalid;
	    StructureEntity<Def>*super = structures.lookup(structure.system_link);
	    if (super)
	        rs = super->config;
	    return rs;
	}

	template <class GL, class Def> unsigned Scenery<GL,Def>::getConfig(CGS::Geometry<Def>*structure)
	{
	    unsigned rs = StructureConfig::Invalid;
	    StructureEntity<Def>*super = structures.lookup(structure->system_link);
	    if (super)
	        rs = super->config;
	    return rs;
	}

	template <class GL, class Def> unsigned Scenery<GL,Def>::getConfig(CGS::StaticInstance<Def>&structure)
	{
	    unsigned rs = StructureConfig::Invalid;
	    StructureEntity<Def>*super;
	    if (structure.signature == this && structure.reference)
	        rs = ((StructureEntity<Def>*)structure.reference)->config;
	    else
	    {
	        super = structures.lookup(&structure.matrix);
	        if (super)
	        {
	            structure.signature = this;
	            structure.reference = super;
	            rs = super->config;
	        }
	    }
	    return rs;
	}

	template <class GL, class Def> unsigned Scenery<GL,Def>::getConfig(CGS::StaticInstance<Def>*structure)
	{
	    unsigned rs = StructureConfig::Invalid;
	    StructureEntity<Def>*super;
	    if (structure->signature == this && structure->reference)
	        rs = ((StructureEntity<Def>*)structure->reference)->config;
	    else
	    {
	        super = structures.lookup(&structure->matrix);
	        if (super)
	        {
	            structure->signature = this;
	            structure->reference = super;
	            rs = super->config;
	        }
	    }
	    return rs;
	}




	template <class GL, class Def> StructureEntity<Def>* Scenery<GL,Def>::embed(CGS::StaticInstance<Def>&instance,unsigned detail_type)
	{
	    instance.link();
	    StructureEntity<Def>*super = embed(*instance.target,detail_type);
	    return super;
	}

	template <class GL, class Def> StructureEntity<Def>* Scenery<GL,Def>::embed(CGS::StaticInstance<Def>*instance,unsigned detail_type)
	{
	    instance->link();
	    StructureEntity<Def>*super = embed(*instance->target,detail_type);
	    return super;
	}

	template <class GL, class Def> StructureEntity<Def>* Scenery<GL,Def>::embed(CGS::AnimatableInstance<Def>&instance,unsigned detail_type)
	{
	    instance.link();
	    StructureEntity<Def>*super = embed(*instance.target,detail_type);
	    return super;
	}

	template <class GL, class Def> StructureEntity<Def>* Scenery<GL,Def>::embed(CGS::AnimatableInstance<Def>*instance,unsigned detail_type)
	{
	    instance->link();
	    StructureEntity<Def>*super = embed(*instance->target,detail_type);
	    return super;
	}


	template <class GL, class Def> StructureEntity<Def>* Scenery<GL,Def>::embed(CGS::Geometry<Def>&structure, unsigned detail_type)
	{
	        
	    SCENERY_LOG("embedding with locked set "+String(locked));
	    StructureEntity<Def>*super = structures.lookup(structure.system_link);
	    if (!super)
	    {
	        super = SHIELDED(new StructureEntity<Def>(&structure,detail_type));
	        structures.insert(super);
	        SCENERY_LOG("new super-entity created");
	    }
	    SCENERY_LOG("embedding materials");
	    for (index_t i = 0; i < structure.material_field.length(); i++)
	        embed(super,structure.material_field[i]);
	        
	    return super;
	}

	template <class GL, class Def> StructureEntity<Def>*  Scenery<GL,Def>::embed(CGS::Geometry<Def>*structure, unsigned detail_type)
	{
	    return embed(*structure,detail_type);
	}


	template <class GL, class Def> void Scenery<GL,Def>::remove(CGS::AnimatableInstance<Def>&instance)
	{
		{
	        SCENERY_LOG("linking instance");
	        instance.link();
	        remove(*instance.target);
	        if (instance.signature == this)
	            instance.reference = NULL;
		}
	}

	template <class GL, class Def> void Scenery<GL,Def>::remove(CGS::AnimatableInstance<Def>*instance)
	{
	    SCENERY_LOG("linking instance");
	    instance->link();
	    remove(*instance->target);
	    if (instance->signature == this)
	        instance->reference = NULL;
	}

	template <class GL, class Def> void Scenery<GL,Def>::remove(CGS::StaticInstance<Def>&instance)
	{
		{
	        SCENERY_LOG("linking instance");
	        instance.link();
	        remove(*instance.target);
	        if (instance.signature == this)
	            instance.reference = NULL;
		}
	}

	template <class GL, class Def> void Scenery<GL,Def>::remove(CGS::StaticInstance<Def>*instance)
	{
	    SCENERY_LOG("linking instance");
	    instance->link();
	    remove(*instance->target);
	    if (instance->signature == this)
	        instance->reference = NULL;
	}

	template <class GL, class Def>
		StructureEntity<Def>* 	Scenery<GL,Def>::findEntityOf(CGS::StaticInstance<Def>&instance)
		{
			return structures.lookup(&instance.matrix);
		}
		
	template <class GL, class Def>
		StructureEntity<Def>* 	Scenery<GL,Def>::findEntityOf(CGS::Geometry<Def>&geometry)
		{
			return structures.lookup(geometry.system_link);
		}

	template <class GL, class Def> void Scenery<GL,Def>::remove(CGS::Geometry<Def>&structure)
	{
	    SCENERY_LOG("removing structure");
	    StructureEntity<Def>*super = structures.dropByIdent(structure.system_link);
	    if (!super)
	    {
	        SCENERY_LOG("super-entity not found - returning");
	        return;
	    }
	    SCENERY_LOG("unmapping all hooked entities");
	        {
				foreach (all_materials,my_material)
					(*my_material)->unmap(super, false, warnings);
	        }
	    SCENERY_LOG("discarding super-entity");
	    DISCARD(super);
	    SCENERY_LOG("unhooking.");
	    for (index_t i = 0; i < structure.material_field.length(); i++)
		{
			shared_ptr<Material<GL,Def>	> m = dynamic_pointer_cast<Material<GL,Def>,CGS::MaterialObject>(structure.material_field[i].info.attachment.lock());
	        if (m && m->owner == this && !m->groups)
	            structure.material_field[i].info.attachment.reset();
		}

		for (index_t i = 0; i < all_materials.count(); i++)
		{
			auto m = all_materials[i];
	        if (!m->groups && !m->locked && !keep_unused_materials)
	        {
	            SCENERY_LOG("erasing empty material");
				all_materials.erase(i--);
				if (m->isOpaque())
					opaque_materials.findAndErase(m);
				else
					transparent_materials.findAndErase(m);
	        }
	        else
	            if (!locked)
	                m->Rebuild(&warnings);
		}
	    SCENERY_LOG("structure removed");
	}

	template <class GL, class Def> void Scenery<GL,Def>::remove(CGS::Geometry<Def>*structure)
	{
	    remove(*structure);
	}

	template <class GL, class Def> void Scenery<GL,Def>::embedDummyMaterial()
	{
	    SCENERY_LOG("embedding dummy-material");
	    if (!dummy_texture.face_field.length())
	    {
	        Image::THeader head;
	        head.x_exp = 7;
	        head.y_exp = 7;
	        head.channels = 3;
	        head.type = 0;
	     
			dummy_texture.name = 0;
			dummy_texture.face_field.setSize(1);
	        dummy_texture.face_field[0].setSize(sizeof(Image::THeader)+128*128*3);
			Array<BYTE>&fce = dummy_texture.face_field[0];
	        memcpy(fce.pointer(),&head,sizeof(head));
	        UINT32 crc = CRC32::getChecksum(fce.pointer()+4,fce.size()-4);
	        memcpy(fce.pointer(),&crc,4);
			dummy_texture.updateHash();
	    }
	    if (!dummy_info.layer_field.length())
	    {
	        dummy_info.layer_field.setSize(1);
	        dummy_info.layer_field[0].combiner = 0x2100;
	        dummy_info.layer_field[0].source = &dummy_texture;
			shared_ptr<Material<GL,Def> >	dummy(new Material<GL,Def>(this,renderer,dummy_info,textures));
	        opaque_materials.append(dummy);
			all_materials.append(dummy);
	    }
	    SCENERY_LOG("dummy material embedded");
	}


	template <class GL, class Def> void Scenery<GL,Def>::lockMaterials(CGS::Geometry<Def>&structure)
	{
	    lockMaterials(&structure);
	}

	template <class GL, class Def> void Scenery<GL,Def>::lockMaterials(CGS::Geometry<Def>*structure)
	{
	    for (index_t i = 0; i < structure->material_field.length(); i++)
	        lockMaterial(structure->material_field[i]);
	}

	template <class GL, class Def> void Scenery<GL,Def>::lockMaterial(CGS::MaterialA<Def>&material)
	{
	    SCENERY_LOG("locking material");
		shared_ptr<Material<GL,Def> > my_material = dynamic_pointer_cast<Material<GL,Def>,CGS::MaterialObject>(material.info.attachment.lock());
	    if (my_material && my_material->owner == this)
	    {
	        my_material->locked = true;
	        SCENERY_LOG("material locked");
	        return;
	    }
	    SCENERY_LOG("not previously embedded");

		foreach (all_materials,my_material)
	        if ((*my_material)->similar(material))
	        {
	            material.info.attachment = *my_material;
	            (*my_material)->locked = true;
	            SCENERY_LOG("material locked");
	            return;
	        }
	    SCENERY_LOG("creating temporary visual");

	    shared_ptr<Material<GL,Def> > vs = shared_ptr<Material<GL,Def> >(new Material<GL,Def>(this, renderer,material.info,textures));

		foreach (all_materials,my_material)
	        if ((*my_material)->similar(*vs))
	        {
	            (*my_material)->locked = true;
	            material.info.attachment = (*my_material);
	            SCENERY_LOG("discarding temporary visual");
	            vs.reset();
	            SCENERY_LOG("material locked");
	            return;
	        }
	    vs->locked = true;
	    SCENERY_LOG("making temporary visual permanent");
		if (vs->isOpaque())
			opaque_materials.append(vs);
		else
			transparent_materials.append(vs);
		all_materials.append(vs);
	    material.info.attachment = vs;
	    SCENERY_LOG("material locked");
	}



	template <class GL, class Def> shared_ptr<Material<GL,Def> > Scenery<GL,Def>::embed(MyStructureEntity*entity,CGS::MaterialA<Def>&material)
	{
		if (!locked && !renderer)
		{
			setRenderer(GL::global_instance,false);
			if (!renderer)
				FATAL__("trying to embed material without renderer");
		}
		
	    SCENERY_LOG("embedding material '"+material.name+"'");
		shared_ptr<Material<GL,Def> > my_material = dynamic_pointer_cast<Material<GL,Def>,CGS::MaterialObject>(material.info.attachment.lock());
	    if (my_material && my_material->owner == this)
	    {
			SCENERY_LOG("similar material found. merging...");
	        my_material->merge(entity,material,!locked,warnings);
	        SCENERY_LOG("material merged");
	        return my_material;
	    }
	    SCENERY_LOG("not previously embedded");
		
		if (merge_materials)
		{
			foreach (all_materials,my_material)
				if ((*my_material)->similar(material))
		        {
					SCENERY_LOG("similar material found. merging...");
		            (*my_material)->merge(entity,material,!locked,warnings);
		            material.info.attachment = (*my_material);
		            SCENERY_LOG("material merged");
		            return (*my_material);
		        }
		}
		SCENERY_LOG("creating temporary visual.");
		
		shared_ptr<Material<GL,Def> > vs = shared_ptr<Material<GL,Def> >(new Material<GL,Def>(this,renderer,material.info,textures));

		if (merge_materials)
		{
			foreach (all_materials,my_material)
				if ((*my_material)->similar(*vs))
		        {
					SCENERY_LOG("identical material found. merging...");
		            (*my_material)->merge(entity,material,!locked,warnings);
		            material.info.attachment = (*my_material);
		            SCENERY_LOG("discarding temporary visual");
		            vs.reset();
		            SCENERY_LOG("material merged");
		            return (*my_material);
		        }
		}
	    vs->merge(entity,material,!locked,warnings);
	    SCENERY_LOG("making temporary visual permanent");
		if (vs->isOpaque())
			opaque_materials.append(vs);
		else
			transparent_materials.append(vs);
		all_materials.append(vs);
	    material.info.attachment = vs;
	    SCENERY_LOG("material embedded");

	    return vs;
	}



	template <class GL, class Def> bool Scenery<GL,Def>::idle()
	{
	    return structures.isEmpty() && all_materials.isEmpty();
	}


	template <class GL, class Def>
	template <class C0, class C1>
	void Scenery<GL,Def>::resolve(const Aspect<C0>&aspect, const C1&visual_range)
	{
	    TMatrix4<C0>	matrix,path;
		TVec4<C0>		cage[8];
		TVec3<C0>		box0={-1,-1,-1},
						box1={1,1,1},
						center,
						lower,
						upper;
	    Mat::mult(aspect.projection,aspect.view,matrix);

	    structures.reset();
	    while (StructureEntity<Def>*entity = structures.each())
	    {
	        if (entity->config & StructureConfig::Invisible)
	        {
	            entity->visible = false;
	            continue;
	        }
	        Mat::transform(aspect.view,entity->system->w.xyz,center);
	        C0   distance = clamped(Vec::length(center)-entity->radius,0,100000);
	        if (!(entity->config & StructureConfig::AlwaysHighestDetail))
	        {
	            C0  r = entity->radius*entity->sys_scale,
	                relative = distance*entity->shortest_edge_length/r;
	            if (relative < 1)
	                relative = 1;
	            entity->detail = (unsigned)(log(relative)/M_LN2);
	        }
	        else
	            entity->detail = 0;
			entity->visible = true;
	        if (distance > visual_range && !(entity->config & StructureConfig::AlwaysVisible))
				entity->visible = false;
	    
			foreach(entity->object_entities,oentity)
			{
				oentity->visible = entity->visible;
				if (oentity->visible && !(entity->config & StructureConfig::AlwaysVisible))
				{
					Mat::mult(matrix,*oentity->system,path);
		            for (BYTE k = 0; k < 8; k++)
		            {
		                Mat::mult(path,oentity->cage[k],cage[k]);
		                Vec::divide(cage[k].xyz,cage[k].w);
		            }
		            lower.x = vmin(vmin(vmin(cage[0].x,cage[1].x),vmin(cage[2].x,cage[3].x)),vmin(vmin(cage[4].x,cage[5].x),vmin(cage[6].x,cage[7].x)));
		            lower.y = vmin(vmin(vmin(cage[0].y,cage[1].y),vmin(cage[2].y,cage[3].y)),vmin(vmin(cage[4].y,cage[5].y),vmin(cage[6].y,cage[7].y)));
		            lower.z = vmin(vmin(vmin(cage[0].z,cage[1].z),vmin(cage[2].z,cage[3].z)),vmin(vmin(cage[4].z,cage[5].z),vmin(cage[6].z,cage[7].z)));
		            upper.x = vmax(vmax(vmax(cage[0].x,cage[1].x),vmax(cage[2].x,cage[3].x)),vmax(vmax(cage[4].x,cage[5].x),vmax(cage[6].x,cage[7].x)));
		            upper.y = vmax(vmax(vmax(cage[0].y,cage[1].y),vmax(cage[2].y,cage[3].y)),vmax(vmax(cage[4].y,cage[5].y),vmax(cage[6].y,cage[7].y)));
		            upper.z = vmax(vmax(vmax(cage[0].z,cage[1].z),vmax(cage[2].z,cage[3].z)),vmax(vmax(cage[4].z,cage[5].z),vmax(cage[6].z,cage[7].z)));
		            bool out =  (Vec::oneGreater(lower,box1) || Vec::oneLess(upper,box0));
		            oentity->visible = !out;
				}
			}
		}
	}


	template <class GL, class Def>
	template <class C0>
	void Scenery<GL,Def>::resolve(const Aspect<C0>&aspect)
	{
		const Frustum<C0>&volume = aspect.resolveVolume();
		

	    //C0   matrix[16],path[16],cage[8][4],box0[3]={-1,-1,-1},box1[3]={1,1,1},center[3],lower[3],upper[3];
		TVec3<C0> center;

	    structures.reset();
	    while (StructureEntity<Def>*entity = structures.each())
	    {
	        if (entity->config & StructureConfig::Invisible)
	        {
	            entity->visible = false;
	            continue;
	        }
	        Mat::transform(aspect.view,entity->system->w.xyz,center);
	        C0   distance = clamped(Vec::length(center)-entity->radius,0,100000);
	        if (!(entity->config & StructureConfig::AlwaysHighestDetail))
	        {
	            C0  r = entity->radius*entity->sys_scale,
	                relative = distance*entity->shortest_edge_length/r;
	            if (relative < 1)
	                relative = 1;
	            entity->detail = (unsigned)(log(relative)/M_LN2);
	        }
	        else
	            entity->detail = 0;
			if (entity->detail > entity->max_detail)
				entity->detail = entity->max_detail;
			entity->visible = true;
	    
			for (auto it = entity->object_entities.begin(); it != entity->object_entities.end(); ++it)
			{
				it->visible = entity->visible;
				if (it->visible && !(entity->config & StructureConfig::AlwaysVisible))
				{
					it->visible = volume.visible(it->system->w.xyz,it->radius);
				// if (!entity->visible)
				// {
					// SCENERY_LOG("entity '"+name2str(entity->object_name)+"' fails visibility check at "+_toString(entity->name+12)+"/r"+FloatToStr(entity->radius));
					// SCENERY_LOG(volume.toString());
					// SCENERY_LOG(aspect.toString());
				// }
				}
			}
		}
	}

	template <class GL, class Def>
	void Scenery<GL,Def>::renderOpaqueMaterials()
	{
		foreach (opaque_materials,my_material)
			(*my_material)->render();
	}

	template <class GL, class Def>
	void Scenery<GL,Def>::renderTransparentMaterials()
	{
		foreach (transparent_materials,my_material)
			(*my_material)->render();
	}


	template <class GL, class Def>
	template <class C0, class C1>
	void Scenery<GL,Def>::render(const Aspect<C0>&aspect, const C1&visual_range)
	{
		if (!renderer)
		{
			setRenderer(GL::global_instance,false);
			if (!renderer)
				FATAL__("trying to render without renderer");
		}
			resolve(aspect,visual_range);

		foreach (opaque_materials,my_material)
			(*my_material)->render();
		foreach (transparent_materials,my_material)
			(*my_material)->render();
	    renderer->unbindAll();
		postRenderCleanup();
	    

	/*
	    glDisable(GL_TEXTURE_2D);
	    glDisable(GL_DEPTH_TEST);
	    glDisable(GL_LIGHTING);
	    objects.reset();
	    while (ObjectEntity<Def>*entity = objects.each())
	        if (entity->visible)
	        {
	            glPushMatrix();
	                glMultMatrix(entity->name);
	                glBegin(GL_LINES);
	                    glColor3f(1,1,1);
	                    glVertex3f(0,0,0);
	                    glVertex3f(1,0,0);
	                    glVertex3f(0,0,1);
	                    glVertex3f(0,0,0);
	                    glVertex3f(0,0,0);
	                    glColor3f(1,0,0);
	                    glVertex3f(0,1,0);
	                glEnd();
	            glPopMatrix();
	        }
	    glEnable(GL_DEPTH_TEST);
	    glEnable(GL_LIGHTING);*/
	}

	template <class GL, class Def>
	void Scenery<GL,Def>::render(unsigned detail)
	{
		if (!renderer)
		{
			setRenderer(GL::global_instance,false);
			if (!renderer)
				FATAL__("trying to render without renderer");
		}
	
		


	    structures.reset();
	    while (StructureEntity<Def>*entity = structures.each())
	    {
			entity->visible = true;
			entity->detail = detail;
			for (auto it = entity->object_entities.begin(); it != entity->object_entities.end(); ++it)
				it->visible = true;
		}
		
		foreach (opaque_materials,my_material)
			(*my_material)->render();
		foreach (transparent_materials,my_material)
			(*my_material)->render();

	    renderer->unbindAll();
		PostRenderCleanup();


	}

	template <class GL, class Def>
	void Scenery<GL,Def>::RenderIgnoreMaterials(unsigned detail)
	{
		if (!renderer)
		{
			setRenderer(GL::global_instance,false);
			if (!renderer)
				FATAL__("trying to render without renderer");
		}
	
		


	    structures.reset();
	    while (StructureEntity<Def>*entity = structures.each())
	    {
			entity->visible = true;
			entity->detail = detail;
			for (auto it = entity->object_entities.begin(); it != entity->object_entities.end(); ++it)
				it->visible = true;
		}
		
		foreach (opaque_materials,my_material)
			(*my_material)->render(true);
		foreach (transparent_materials,my_material)
			(*my_material)->render(true);

	    renderer->unbindAll();
		PostRenderCleanup();
	}
	
	template <class GL, class Def>
	template <class C0>
	void Scenery<GL,Def>::render(const Aspect<C0>&aspect)
	{
		if (!renderer)
		{
			setRenderer(GL::global_instance,false);
			if (!renderer)
				FATAL__("trying to render without renderer");
		}
			resolve(aspect);

		foreach (opaque_materials,my_material)
			(*my_material)->render();
		foreach (transparent_materials,my_material)
			(*my_material)->render();

	    renderer->unbindAll();
		PostRenderCleanup();

	}

	template <class GL, class Def>
	template <class C0>
	void Scenery<GL,Def>::renderIgnoreMaterials(const Aspect<C0>&aspect)
	{
		if (!renderer)
		{
			setRenderer(GL::global_instance,false);
			if (!renderer)
				FATAL__("trying to render without renderer");
		}
			resolve(aspect);

		foreach (opaque_materials,my_material)
			(*my_material)->render(true);
		foreach (transparent_materials,my_material)
			(*my_material)->render(true);

		PostRenderCleanup();

	}


	template <class GL, class Def> Scenery<GL,Def>::Scenery(TextureTable<GL>*table):locked(0),renderer(GL::global_instance),textures(table?table:&local_textures)
	{
		merge_materials = true;
		keep_unused_materials = false;
	}

	template <class GL, class Def> 
		Scenery<GL,Def>::Scenery(GL*renderer_, TextureTable<GL>*table):locked(0),renderer(renderer_),textures(table?table:&local_textures)
		{
			merge_materials = true;
			keep_unused_materials = false;
		}

	template <class GL, class Def> 
		Scenery<GL,Def>::Scenery(GL&renderer_, TextureTable<GL>*table):locked(0),renderer(&renderer_),textures(table?table:&local_textures)
		{
			merge_materials = true;
			keep_unused_materials = false;
		}

	template <class GL, class Def> 
		void Scenery<GL,Def>::setRenderer(GL*renderer_, bool fail_if_not_idle)
		{
			if (renderer_ == renderer)
				return;
			if (!idle() && fail_if_not_idle)
				FATAL__("Attempting to update renderer (0x"+PointerToHex(renderer)+"=>0x"+PointerToHex(renderer_)+") on a non-idle scenery");
			renderer = renderer_;
			foreach (all_materials,my_material)
				(*my_material)->renderer = renderer_;
		}
		
	template <class GL, class Def> 
		void Scenery<GL,Def>::setRenderer(GL&renderer)
		{
			setRenderer(&renderer);
		}
		
	template <class GL, class Def> 
		GL*	Scenery<GL,Def>::getRenderer()	const
		{
			return renderer;
		}



	template <class GL, class Def> Scenery<GL,Def>::~Scenery()
	{
		//unlink infos - no guarantee these still exist at this point
			
	    clear(true);
	}


	template <class GL, class Def> void Scenery<GL,Def>::clear(bool disconnected)
	{
		if (disconnected)
		{
			foreach (all_materials,my_material)
				(*my_material)->shutdown();
		}
		
	    opaque_materials.reset();
		transparent_materials.reset();
		all_materials.reset();

	    structures.clear();
	    local_textures.clear();
	}




	template <class GL, class Def> String Scenery<GL,Def>::state()
	{
	    unsigned total(0);
	    String rs = String(structures.count())+" structure(s)\n"+String(textures->storage.count())+" texture(s)\n";
	    rs+=String(opaque_materials.count())+" opaque visual(s):\n";

		foreach (transparent_materials,my_material)
	        rs+=(*my_material)->state()+"-----------------------------------------------------------------------\n";
	    rs+=String(transparent_materials.count())+" transparent visual(s):\n";
		foreach (transparent_materials,my_material)
	        rs+=(*my_material)->state()+"-----------------------------------------------------------------------\n";
	    rs+="total faces in scene (detail 0): "+String(total)+"\n";
	    return rs;
	}


	template <class GL, class Def> typename Def::FloatType*    Scenery<GL,Def>::extractRenderPath(count_t&points)
	{
	    List::Vector<typename Def::FloatType>   list;
		foreach(all_materials,my_material)
	        (*my_material)->extractRenderPath(list);
	    typename Def::FloatType*result;
	    alloc(result,list.count()*3);
	    for (index_t i = 0; i < list.count(); i++)
	        copy3(list[i],&result[i*3]);
	    points = list.count();
	    return result;
	}


	template <class GL, class Def> shared_ptr<Material<GL,Def> > Scenery<GL,Def>::largestMaterial()
	{
	    shared_ptr<Material<GL,Def> > result;
	    size_t size(0);
		foreach (all_materials,my_material)
			if ((*my_material)->buffer.index_field.length() > size)
	        {
	            result = (*my_material);
	            size = (*my_material)->buffer.index_field.length();
	        }
	    return result;
	}


	template <class GL, class Def> void Scenery<GL,Def>::lock()
	{
	    locked++;
	}

	template <class GL, class Def> void Scenery<GL,Def>::unlock()
	{
	    if (locked)
	        locked--;
	}



	template <class GL, class Def> count_t					Scenery<GL,Def>::countWarnings()
	{
		return warnings;
	}

	template <class GL, class Def> void						Scenery<GL,Def>::clearWarnings()
	{
		warnings.clear();
	}

	template <class GL, class Def> const String&			Scenery<GL,Def>::getWarning(index_t index)
	{
		return warnings[index];
	}

	template <class GL, class Def> const String&			Scenery<GL,Def>::getWarnings(bool clear_warnings)
	{
		fused = warnings.fuse(0,'\n');
		if (clear_warnings)
		    warnings.clear();
		return fused;
	}











	template <class Def>
		void		SceneryTree<Def>::recursiveRemap(List*source)
		{
			SCENERY_BEGIN
	        elements.flush();
			if (has_children)
			{
				for (BYTE k = 0; k < 8; k++)
					if (child[k])
					{
						DISCARD(child[k]);
						child[k] = NULL;
					}
				has_children = false;
			}
	        dominating = source->first()->structure;
			Vec::clear(split);
	        source->reset();
	        while (Object*section = source->each())
	        {
				Box<Float> box;
				box.setCenter(section->system->w.xyz,section->radius);
				if (!volume.intersects(box))
	                continue;

				Vec::add(split,section->system->w.xyz);
	            elements.insert(section);
	            if (dominating != section->structure)
	                dominating = NULL;
	        }
	        if (!level || elements.count()<2)
			{
				SCENERY_END
	            return;
			}
			Vec::div(split,elements.count());

			BYTE greatest(0),collapsed(0);
			Float greatest_range(0);
			TVec3<Float>	new_split;
			for (BYTE k = 0; k < 3; k++)
			{
				Float delta = (volume.axis[k].max-volume.axis[k].min);
				split.v[k] = clamped(split.v[k],volume.axis[k].min+delta/5,volume.axis[k].max-delta/5);
				Float val = split.v[k];
				
				bool moved;
				
				moved = true;
				while (moved)
				{
					moved = false;
					elements.reset();
					while (Object*object = elements.each())
						if (object->system->w.v[k]-object->radius < val && object->system->w.v[k]+object->radius > val)
						{
							if (val > split.v[k])
								val = object->system->w.v[k]+object->radius*1.01;
							elif (val < split.v[k])
								val = object->system->w.v[k]-object->radius*1.01;
							elif (object->system->w.v[k]>val)
								val = object->system->w.v[k]-object->radius*1.01;
							else
								val = object->system->w.v[k]+object->radius*1.01;
							moved = true;
						}
				}
				
		
				new_split.v[k] = clamped(val,volume.axis[k].min,volume.axis[k].max);
				
				if (new_split.v[k] < volume.axis[k].min+delta/10 || new_split.v[k] > volume.axis[k].max-delta/10)
				{
					if (new_split.v[k] < volume.axis[k].min+delta/10)
						new_split.v[k] = volume.axis[k].min;
					else
						new_split.v[k] = volume.axis[k].max;
					collapsed ++;
				}
				if (delta > greatest_range)
				{
					greatest_range = delta;
					greatest = k;
				}
			}
			
			if (collapsed == 3)
			{
				if (elements > 3)
				{
					Float original = split.v[greatest];
					split = new_split;
					split.v[greatest] = original;
				}
				else
				{
					SCENERY_END
					return;
				}
			}
			else
				split = new_split;
			
			
			BYTE	num_children=0,
					unary_child = 0;
			for (BYTE k = 0; k < 8; k++)
			{
				BYTE p[3] = {k / 4,
							 k % 4 / 2,
							 k % 4 % 2};
				bool collapsed(false);
				TVec3<typename Def::FloatType>	min,max,out_min,out_max;
				volume.getMin(min);
				volume.getMax(max);

				
				for (BYTE j = 0; j < 3; j++)
				{
					out_min.v[j] = p[j]?split.v[j]:min.v[j];
					out_max.v[j] = p[j]?max.v[j]:split.v[j];
					collapsed |= (out_max.v[j]-out_min.v[j]) < (max.v[j]-min.v[j])/20;
				}
				Volume d(out_min,out_max);
				if (!collapsed)
				{
					child[k] = SHIELDED(new Tree(d, level-1));
					num_children++;
					unary_child = k;
				}
				else
					child[k] = NULL;
			}
			has_children = true;
			
			for (BYTE k = 0; k < 8; k++)
				if (child[k])
				{
					child[k]->recursiveRemap(&elements);
					if (!child[k]->elements)
					{
						DISCARD(child[k]);
						child[k] = NULL;
						num_children--;
						if (k == unary_child && num_children == 1)
							for (BYTE k2 = 0; k2 < 8; k2++)
								if (child[k2])
									unary_child = k2;
							
					}
				}
			if (num_children==1 && !child[unary_child]->has_children)
			{
				DISCARD(child[unary_child]);
				child[unary_child] = NULL;
				has_children = false;
			}
				
			SCENERY_END
		}
		
	template <class Def>
		count_t	SceneryTree<Def>::recursiveLookup(const Volume&space, const Structure*exclude, Buffer<Object*>&buffer)
		{
			SCENERY_BEGIN
	        const count_t cnt = elements.count();
			if ((exclude && dominating == exclude) || !volume.intersects(space) || !cnt)
			{
				SCENERY_END
	            return 0;
			}
	        if (!level || 8*level > cnt || !has_children)
	        {
	            count_t c = 0;
	            for (index_t i = 0; i < cnt; i++)
	            {
	                Object*object = elements[i];
					if (object->structure == exclude)
						continue;
					Box<Float> box;
					box.setCenter(object->system->w.xyz,object->radius);
					if (!volume.intersects(box))
	                    continue;
					buffer << object;
	                c++;
	            }
				SCENERY_END
	            return c;
	        }
			
	        index_t at = buffer.fillLevel();
	        for (BYTE k = 0; k < 8; k++)
	            if (child[k] && child[k]->recursiveLookup(space,exclude,buffer) == cnt)
	            {
	                if (buffer.fillLevel()-at > cnt)
	                {
						buffer.truncate(at);
	                    for (index_t i = 0; i < cnt; i++)
	                        buffer << elements[i];
	                }
					SCENERY_END
	                return cnt;
	            }
			SCENERY_END
	        return 0;
		}
		
	template <class Def> template <typename T>
		count_t	SceneryTree<Def>::recursiveLookup(const Frustum<T>&space, Buffer<Object*>&buffer)
		{
			SCENERY_BEGIN
	        const count_t cnt = elements.count();
			
			TVec3<Float> center;
			Vec::center(volume.lower,volume.upper,center);
			
			typename Frustum<T>::Visibility vs = space.checkSphereVisibility(center,_distance(volume.lower,center));
			//space.checkBoxVisibility(volume.lower,volume.upper);
			if (vs == Frustum<T>::NotVisible)
			{
				SCENERY_END
				return 0;
			}
			if (vs == Frustum<T>::FullyVisible || !level || !has_children)
	        {
	            count_t c = 0;
	            for (index_t i = 0; i < cnt; i++)
	            {
	                Object*object = elements[i];
					buffer << object;
	                c++;
	            }
				SCENERY_END
	            return c;
	        }
			
	        index_t at = buffer.fillLevel();
	        for (BYTE k = 0; k < 8; k++)
	            if (child[k] && child[k]->recursiveLookup(space,buffer) == cnt)
	            {
	                if (buffer.fillLevel()-at > cnt)
	                {
						buffer.truncate(at);
	                    for (index_t i = 0; i < cnt; i++)
	                        buffer << elements[i];
	                }
					SCENERY_END
	                return cnt;
	            }
			SCENERY_END
	        return 0;
		}
		
	template <class Def>	
		SceneryTree<Def>::SceneryTree(const Volume&space, unsigned level_):volume(space),level(level_)
	    {
			SCENERY_BEGIN
	        for (BYTE k = 0; k < 8; k++)
	            child[k] = NULL;
			has_children = false;
			SCENERY_END
	    }
		
	template <class Def>
		SceneryTree<Def>::SceneryTree(const Tree&other)
		{
			SCENERY_BEGIN
	        for (index_t i = 0; i < other.elements; i++)
				elements.add(const_cast<Engine::ObjectEntity<Def> *>(other.elements[i]));	//this is not the clean way but i'm feeling rather busy right now
	        dominating = other.dominating;
	        if (level && elements.count())
			{
	            for (BYTE k = 0; k < 8; k++)
	                child[k] = SHIELDED(new Tree(*other.child[k]));
				has_children = true;
			}
	        else
			{
	            for (BYTE k = 0; k < 8; k++)
	                child[k] = NULL; //we will see if we actually need this
				has_children = false;
			}
			SCENERY_END
		}
		
	template <class Def>
		SceneryTree<Def>::SceneryTree(unsigned depth):level(depth)
		{
			SCENERY_BEGIN
		    for (BYTE k = 0; k < 8; k++)
	            child[k] = NULL;
			has_children = false;
			SCENERY_END
		}

	template <class Def>
		SceneryTree<Def>::~SceneryTree()
		{
			SCENERY_BEGIN
	        for (BYTE k = 0; k < 8; k++)
	            if (child[k])
	                DISCARD(child[k]);
			SCENERY_END
		}

	template <class Def>
		void		SceneryTree<Def>::clear()
		{
			SCENERY_BEGIN
	        for (BYTE k = 0; k < 8; k++)
	            if (child[k])
				{
	                DISCARD(child[k]);
					child[k] = NULL;
				}
			elements.flush();
			has_children = false;
			SCENERY_END
		}

	template <class Def>
		void		SceneryTree<Def>::remap(List*source)
		{
			SCENERY_BEGIN
			clear();
	        if (!source->count())
			{
				SCENERY_END
	            return;
			}
			level = (unsigned)log((float)source->count());	//may need to adjust this later
			//ShowMessage(String(source->count())+" => "+String(level));

			{
				Object*object = source->first();
				volume.setCenter(object->system->w.xyz,object->radius);
			}
	        dominating = source->first()->structure;
	        source->reset();
	        while (Object*object = source->each())
			{
				//elements.insert(object);
				//_add(center,object->system+12);
				Box<Float> box;
				box.setCenter(object->system->w.xyz,object->radius);
				volume.include(box);
				/*float absolute[3];
				for (BYTE k = 0; k < 8; k++)
				{
					__transform(object->system,object->cage[k],absolute);
					_oDetDimension(absolute,volume.box);
				}*/
				if (object->structure != dominating)
					dominating = NULL;
			}
			recursiveRemap(source);
			SCENERY_END
		}

	template <class Def>	
		void	SceneryTree<Def>::lookup(const Volume&space, Buffer<Object*>&out, const Structure*exclude)
		{
			SCENERY_BEGIN
			Buffer<Object*>					buffer;
	        recursiveLookup(space,exclude,buffer);
			index_t offset = out.fillLevel();
	        for (index_t i = 0; i < buffer.fillLevel(); i++)
	        {
	            Object*object = buffer[i];
	            if (!object->added && object->structure != exclude)
	            {
					out << object;
	                object->added = true;
	            }
	        }
	        for (index_t i = offset; i < out.fillLevel(); i++)
	            out[i]->added = false;
			SCENERY_END
		}
		
	template <class Def> template <typename T>
		void	SceneryTree<Def>::lookup(const Frustum<T>&volume, Buffer<Structure*>&struct_out, Buffer<Object*>&object_out)
		{
			SCENERY_BEGIN
			Buffer<Object*>					buffer;
	        recursiveLookup(volume,buffer);
			index_t offset = object_out.fillLevel(),
					struct_offset = struct_out.fillLevel();
	        for (index_t i = 0; i < buffer.fillLevel(); i++)
	        {
	            Object*object = buffer[i];
	            if (!object->added)
	            {
					object_out << object;
	                object->added = true;
					if (!object->structure->added)
					{
						struct_out << object->structure;
						object->structure->added = true;
					}
	            }
	        }
	        for (index_t i = offset; i < object_out.fillLevel(); i++)
	            object_out[i]->added = false;
			for (index_t i = struct_offset; i < struct_out.fillLevel(); i++)
				struct_out[i]->added = false;
			SCENERY_END
		}
		
	template <class Def>
		SceneryTree<Def>&	SceneryTree<Def>::operator=(const Tree&other)
		{
			SCENERY_BEGIN
	        for (BYTE k = 0; k < 8; k++)
				if (child[k])
					DISCARD(child[k]);
	        elements.flush();
	        for (index_t i = 0; i < other.elements; i++)
	            elements.add(const_cast<Engine::ObjectEntity<Def> *>(other.elements[i]));
	        dominating = other.dominating;
	        if (level && elements)
	            for (BYTE k = 0; k < 8; k++)
	                child[k] = SHIELDED(new Tree(*other.child[k]));
	        else
	            for (BYTE k = 0; k < 8; k++)
	                child[k] = NULL; //we will see if we actually need this
			SCENERY_END
			return *this;
		}
		
	template <class Def>
		SceneryTree<Def>*						SceneryTree<Def>::getChild(BYTE c)
		{
			if (c >= 8)
				return NULL;
			return child[c];
		}
		
	template <class Def>
		StructureEntity<Def>*					SceneryTree<Def>::getDominating()
		{
			return dominating;
		}
		
	template <class Def>
		bool										SceneryTree<Def>::hasChildren()
		{
			return has_children;
		}
		
		
	template <class Def>
		unsigned									SceneryTree<Def>::getLevel()
		{
			return level;
		}
		
	template <class Def>
		const TVec3<typename Def::FloatType>&						SceneryTree<Def>::getSplitVector()
		{
			return split;
		}
		
	template <class Def>
		const typename SceneryTree<Def>::Volume&					SceneryTree<Def>::getVolume()
		{
			return volume;
		}
		
	template <class Def>
		count_t					SceneryTree<Def>::countElements()
		{
			return elements;
		}
		
	template <class Def>
		void										SceneryTree<Def>::getElements(List&out)
		{
			out.flush();
			out.import(elements);
		}
	
	template <class Def>
		const typename SceneryTree<Def>::List* 	SceneryTree<Def>::getElementList() const
		{
			return &elements;
		}
		


	template <class GL, class Def>
		MappedScenery<GL,Def>::MappedScenery(unsigned tree_depth, TextureTable<GL>*table):Scenery(table),tree(tree_depth)
		{}
		
	template <class GL, class Def>
		MappedScenery<GL,Def>::MappedScenery(unsigned tree_depth, GL*renderer, TextureTable<GL>*table):Scenery(renderer,table),tree(tree_depth)
		{}
		
	template <class GL, class Def>
		MappedScenery<GL,Def>::MappedScenery(unsigned tree_depth, GL&renderer, TextureTable<GL>*table):Scenery(renderer,table),tree(tree_depth)
		{}
		
		
	template <class GL, class Def>
		StructureEntity<Def>*MappedScenery<GL,Def>::embed(CGS::AnimatableInstance<Def>&instance,unsigned detail_type)
		{
			StructureEntity<Def>*rs = Scenery::embed(instance,detail_type);
			if (rs)
			{
				for (index_t i = 0; i < rs->object_entities.count(); i++)
					objects.add(rs->object_entities+i);
				//objects.import(rs->object_entities);
				if (!Scenery::locked)
					remap();
			}
			
			return rs;
		}

	template <class GL, class Def>
		StructureEntity<Def>*MappedScenery<GL,Def>::embed(CGS::AnimatableInstance<Def>*instance,unsigned detail_type)
		{
			StructureEntity<Def>*rs = Scenery::embed(instance,detail_type);
			
			if (rs)
			{
				for (index_t i = 0; i < rs->object_entities.count(); i++)
					objects.add(rs->object_entities+i);
				//objects.import(rs->object_entities);
				if (!Scenery::locked)
					remap();
			}
			return rs;
		}
	
		
	template <class GL, class Def>
		StructureEntity<Def>*MappedScenery<GL,Def>::embed(CGS::StaticInstance<Def>&instance,unsigned detail_type)
		{
			StructureEntity<Def>*rs = Scenery::embed(instance,detail_type);
			if (rs)
			{
				for (index_t i = 0; i < rs->object_entities.count(); i++)
					objects.add(rs->object_entities+i);
				//objects.import(rs->object_entities);
				if (!Scenery::locked)
					remap();
			}
			
			return rs;
		}

	template <class GL, class Def>
		StructureEntity<Def>*MappedScenery<GL,Def>::embed(CGS::StaticInstance<Def>*instance,unsigned detail_type)
		{
			StructureEntity<Def>*rs = Scenery::embed(instance,detail_type);
			
			if (rs)
			{
				for (index_t i = 0; i < rs->object_entities.count(); i++)
					objects.add(rs->object_entities+i);
				//objects.import(rs->object_entities);
				if (!Scenery::locked)
					remap();
			}
			return rs;
		}

	template <class GL, class Def>
		StructureEntity<Def>*MappedScenery<GL,Def>::embed(CGS::Geometry<Def>&structure,unsigned detail_type)
		{
			StructureEntity<Def>*rs = Scenery::embed(structure,detail_type);
			
			if (rs)
			{
				for (index_t i = 0; i < rs->object_entities.count(); i++)
					objects.add(rs->object_entities+i);
				//objects.import(rs->object_entities);
				if (!Scenery::locked)
					remap();
			}
			return rs;
		}
		
	template <class GL, class Def>
		StructureEntity<Def>*MappedScenery<GL,Def>::embed(CGS::Geometry<Def>*structure,unsigned detail_type)
		{
			StructureEntity<Def>*rs = Scenery::embed(structure,detail_type);
			
			
			if (rs)
			{
				for (index_t i = 0; i < rs->object_entities.count(); i++)
					objects.add(rs->object_entities+i);
				//objects.import(rs->object_entities);
				if (!Scenery::locked)
					remap();
			}
			return rs;
		}
		
	template <class GL, class Def>
		void			MappedScenery<GL,Def>::remove(CGS::Geometry<Def>&structure)
		{

			StructureEntity<Def>*entity = Scenery::structures.lookup(structure.system_link);
			if (entity)
			{
				for (index_t i = 0; i < entity->object_entities.count(); i++)
					objects.drop(&entity->object_entities[i]);
				Scenery::remove(structure);
				if (!Scenery::locked)
					remap();
			}
		}
		
	template <class GL, class Def>
		void			MappedScenery<GL,Def>::remove(CGS::Geometry<Def>*structure)
		{
			StructureEntity<Def>*entity = Scenery::structures.lookup(structure->system_link);
			if (!entity)
				return;
			for (index_t i = 0; i < entity->object_entities.count(); i++)
				objects.drop(&entity->object_entities[i]);
			Scenery::remove(*structure);
			if (!Scenery::locked)
				remap();
		}
		
	template <class GL, class Def>
		void			MappedScenery<GL,Def>::remove(CGS::AnimatableInstance<Def>&instance)
		{
			StructureEntity<Def>*entity = Scenery::structures.lookup(&instance.matrix);
			if (entity)
			{
				for (index_t i = 0; i < entity->object_entities.count(); i++)
					objects.drop(&entity->object_entities[i]);

				Scenery::remove(instance);
				if (!Scenery::locked)
					remap();
			}
		}
		
	template <class GL, class Def>
		void			MappedScenery<GL,Def>::remove(CGS::AnimatableInstance<Def>*instance)
		{
			StructureEntity<Def>*entity = Scenery::structures.lookup(&instance->matrix);
			if (!entity)
				return;
			for (index_t i = 0; i < entity->object_entities.count(); i++)
				objects.drop(&entity->object_entities[i]);
			Scenery::remove(*instance);
			if (!Scenery::locked)
				remap();
		}
		
	template <class GL, class Def>
		void			MappedScenery<GL,Def>::remove(CGS::StaticInstance<Def>&instance)
		{
			StructureEntity<Def>*entity = Scenery::structures.lookup(&instance.matrix);
			if (entity)
			{
				for (index_t i = 0; i < entity->object_entities.count(); i++)
					objects.drop(&entity->object_entities[i]);

				Scenery::remove(instance);
				if (!Scenery::locked)
					remap();
			}
		}
		
	template <class GL, class Def>
		void			MappedScenery<GL,Def>::remove(CGS::StaticInstance<Def>*instance)
		{
			StructureEntity<Def>*entity = Scenery::structures.lookup(&instance->matrix);
			if (!entity)
				return;
			for (index_t i = 0; i < entity->object_entities.count(); i++)
				objects.drop(&entity->object_entities[i]);
			Scenery::remove(*instance);
			if (!Scenery::locked)
				remap();
		}
		
	template <class GL, class Def>
		void			MappedScenery<GL,Def>::Rebuild()
		{
			if (Scenery::locked)
				return;
			Scenery::Rebuild();
			remap();
		}
		
	template <class GL, class Def>
		void			MappedScenery<GL,Def>::remap()
		{
			if (!Scenery::locked)
				tree.remap(&objects);
		}
		
	template <class GL, class Def> template <typename T>
		void 			MappedScenery<GL,Def>::lookup(const Box<T>&box, Buffer<ObjectEntity<Def>*>&out, const StructureEntity<Def>*exclude)
		{
			tree.lookup(box, out, exclude);
		}
		
	template <class GL, class Def>
		void			MappedScenery<GL,Def>::clear(bool disconnected)
		{
			tree.clear();
			objects.flush();
		}
		
	template <class GL, class Def> template <class Def2>
		void			MappedScenery<GL,Def>::import(Scenery<GL,Def2>&scenery)
		{
			Scenery::import(scenery);
			if (!Scenery::locked)
				remap();
		}


	template <class GL, class Def> template <class C0>
		void 			MappedScenery<GL,Def>::resolve(const Aspect<C0>&aspect)
		{
		
			if (!Scenery::structures)
				return;
			if (Scenery::structures < 5)	// Don't use expensive tree lookup for few structures
			{
				Scenery::resolve(aspect);
				structure_buffer.reset();
				object_buffer.reset();
				Scenery::structures.reset();
				while (StructureEntity*e = Scenery::structures.each())
					if (e->visible)
					{
						structure_buffer << e;
						e->object_entities.reset();
						while (ObjectEntity<Def>*obj = e->object_entities.each())
							if (obj->visible)
								object_buffer << obj;
					}
				return;
			}
			const Frustum<C0>&volume = aspect.resolveVolume();
			

		    //C0   matrix[16],path[16],cage[8][4],box0[3]={-1,-1,-1},box1[3]={1,1,1},center[3],lower[3],upper[3];
		    //__mult4(aspect.projection,aspect.view,matrix);
			TVec3<C0> center;
		
			structure_buffer.reset();
			object_buffer.reset();
			tree.lookup(volume,structure_buffer,object_buffer);
			for (unsigned i = 0; i < structure_buffer.fillLevel(); i++)
			{
				StructureEntity<Def>*entity = structure_buffer[i];

		        if (entity->config & StructureConfig::Invisible)
		        {
		            entity->visible = false;
		            continue;
		        }
		        __transform(aspect.view,&entity->system[12],center);
		        C0   distance = clamped(_length(center)-entity->radius,0,100000);
		        if (!(entity->config & StructureConfig::AlwaysHighestDetail))
		        {
		            C0  r = entity->radius*entity->sys_scale,
		                relative = distance*entity->shortest_edge_length/r;
		            if (relative < 1)
		                relative = 1;
		            entity->detail = (unsigned)(log(relative)/M_LN2);
		        }
		        else
		            entity->detail = 0;
				if (entity->detail > entity->max_detail)
					entity->detail = entity->max_detail;
				entity->visible = true;
		    
			}
			for (unsigned i = 0; i < object_buffer.fillLevel(); i++)
			{
				ObjectEntity<Def>*entity = object_buffer[i];
				entity->visible = entity->structure->visible;
			}
			

		}
		
	template <class GL, class Def>
		void MappedScenery<GL,Def>::PostRenderCleanup()
		{
			for (index_t i = 0; i < structure_buffer.fillLevel(); i++)
				structure_buffer[i]->visible = false;
			for (index_t i = 0; i < object_buffer.fillLevel(); i++)
				object_buffer[i]->visible = false;
			structure_buffer.reset();
			object_buffer.reset();
			Super::PostRenderCleanup();
		}

	template <class GL, class Def> template <class C0>
		void 			MappedScenery<GL,Def>::render(const Aspect<C0>&aspect)
		{
			if (!Scenery::renderer)
			{
				Scenery::renderer = GL::global_instance;
				if (!Scenery::renderer)
					FATAL__("trying to render without renderer");
			}
				resolve(aspect);

			foreach (Scenery::opaque_materials,my_material)
				(*my_material)->render();
			foreach (Scenery::transparent_materials,my_material)
				(*my_material)->render();

		    Scenery::renderer->unbindAll();
			postRenderCleanup();
		}
		
	template <class GL, class Def> template <class C0, class C1, class C2, class C3>
		ObjectEntity<Def>*	MappedScenery<GL,Def>::lookupClosest(const TVec3<C0>&center, const C1&radius, TVec3<C2>&position_out, TVec3<C3>&normal_out)
		{
			SCENERY_BEGIN
			C0 least_distance(1000000000);
			ObjectEntity<Def>*	closest_entity(NULL);
			
			typename SceneryTree<Def>::Volume volume;
			Vec::subVal(center,radius,volume.lower);
			Vec::addVal(center,radius,volume.upper);
			static Buffer<ObjectEntity<Def>*> obj_buffer;
			obj_buffer.reset();
			tree.lookup(volume,obj_buffer);
				
			for (index_t i = 0; i < obj_buffer.fillLevel(); i++)
			{
				ObjectEntity<Def>*entity = obj_buffer[i];
				CGS::SubGeometryA<Def>*child = entity->source;
				if (!child->vs_hull_field.length() || (entity->structure->config & StructureConfig::NonDetectable))
					continue;
				typedef typename CGS::SubGeometryA<Def>::VsDef	VsDef;
				Mesh<VsDef>&obj = child->vs_hull_field.last();
				if (!obj.map)
				{
					obj.map = obj.buildMap(O_ALL,4);
					
					for (index_t i = 0; i < obj.vertex_field.length(); i++)
						Vec::clear(obj.vertex_field[i].normal);
					for (index_t i = 0; i < obj.triangle_field.length(); i++)
					{
						MeshTriangle<VsDef>&t = obj.triangle_field[i];
						Obj::triangleNormal(t.v0->position,t.v1->position,t.v2->position,t.normal);
						Vec::add(t.v0->normal,t.normal);
						Vec::add(t.v1->normal,t.normal);
						Vec::add(t.v2->normal,t.normal);
						Vec::normalize0(t.normal);
					}
					for (index_t i = 0; i < obj.quad_field.length(); i++)
					{
						MeshQuad<VsDef>&q = obj.quad_field[i];
						Obj::triangleNormal(q.v0->position,q.v1->position,q.v2->position,q.normal[0]);
						Vec::add(q.v0->normal,q.normal[0]);
						Vec::add(q.v1->normal,q.normal[0]);
						Vec::add(q.v2->normal,q.normal[0]);
						Obj::triangleNormal(q.v0->position,q.v2->position,q.v3->position,q.normal[1]);
						Vec::add(q.v0->normal,q.normal[1]);
						Vec::add(q.v2->normal,q.normal[1]);
						Vec::add(q.v3->normal,q.normal[1]);
						Vec::normalize0(q.normal[0]);
						Vec::normalize0(q.normal[1]);
					}
					for (index_t i = 0; i < obj.vertex_field.length(); i++)
						Vec::normalize0(obj.vertex_field[i].normal);
					for (index_t i = 0; i < obj.edge_field.length(); i++)
					{
						MeshEdge<VsDef>&e = obj.edge_field[i];
						Vec::clear(e.normal);
						if (e.n[0])
						{
							if (e.n[0].is_quad)
							{
								Vec::mult(e.n[0].quad->normal[0],0.5,e.normal);
								Vec::mad(e.normal,e.n[0].quad->normal[1],0.5);
							}
							else
								Vec::copy(e.n[0].triangle->normal,e.normal);
						}
						if (e.n[1])
						{
							if (e.n[1].is_quad)
							{
								Vec::mad(e.normal,e.n[1].quad->normal[0],0.5);
								Vec::mad(e.normal,e.n[1].quad->normal[1],0.5);
							}
							else
								Vec::add(e.normal,e.n[1].triangle->normal);
						}
						Vec::normalize0(e.normal);
					}
				}
					
				TVec3<C0> inner_center;
				if (!entity->invert_set)
				{
					Mat::invertSystem(entity->system,entity->invert);
					entity->invert_set = true;
				}
				Mat::transform(entity->invert,center,inner_center);
				Box<typename VsDef::Type> inner_volume;
				inner_volume.setCenter(inner_center,radius);
				//Vec::sub(inner_center,radius,inner_volume.lower);
				//Vec::add(inner_center,radius,inner_volume.upper);
				unsigned cnt = obj.map->lookup(inner_volume);
				if (!cnt)
					continue;
					
				union
				{
					MeshQuad<VsDef>		*qbuffer[1024];
					MeshTriangle<VsDef>	*tbuffer[1024];
					MeshEdge<VsDef>		*ebuffer[1024];
					MeshVertex<VsDef>	*vbuffer[1024];
				};
					
					
				if (unsigned cnt = obj.map->resolveVertices(vbuffer,ARRAYSIZE(vbuffer)))
				{
					for (unsigned i = 0; i < cnt; i++)
					{
						C0 dist = Vec::quadraticDistance(vbuffer[i]->position,inner_center);
						if (dist < least_distance)
						{
							least_distance = dist;
							Mat::transform(entity->system,vbuffer[i]->position,position_out);
							Mat::rotate(entity->system,vbuffer[i]->normal,normal_out);
							closest_entity = entity;
						}
					}
				}
					
				if (unsigned cnt = obj.map->resolveTriangles(tbuffer,ARRAYSIZE(tbuffer)))
				{
					for (unsigned i = 0; i < cnt; i++)
					{
						MeshTriangle<VsDef>*t = tbuffer[i];
						TVec3<C0> factors,e1,e2;
						Vec::mad(inner_center,tbuffer[i]->normal,-radius,e2);
						Vec::mad(inner_center,tbuffer[i]->normal,radius,e1);
						if (!_oDetTriangleEdgeIntersection(t->v0->position,t->v1->position,t->v2->position,e1,e2,factors))
							continue;
						if (!_oValidTriangleEdgeIntersection(factors))
							continue;
						factors.z = factors.z*2-1;
						if (sqr(factors.z*radius) < least_distance)
						{
							least_distance = sqr(factors.z*radius);
							Vec::mad(inner_center,tbuffer[i]->normal,-factors[2]*radius,e2);
							Mat::transform(entity->system,e2,position_out);
							Mat::rotate(entity->system,tbuffer[i]->normal,normal_out);
							closest_entity = entity;
						}
					}
				}
					
				if (unsigned cnt = obj.map->resolveQuads(qbuffer,ARRAYSIZE(qbuffer)))
				{
					for (unsigned i = 0; i < cnt; i++)
					{
						MeshQuad<VsDef>*q = qbuffer[i];
						C0 dist = _oProjectionFactor(q->v0->position,q->normal[0],inner_center);
						TVec3<C0>	collapsed,factors,e1,e2;
							
						Vec::mad(inner_center,q->normal[0],-radius,e2);
						Vec::mad(inner_center,q->normal[0],radius,e1);
						if (_oDetTriangleEdgeIntersection(q->v0->position,q->v1->position,q->v2->position,e1,e2,factors)
							&&
							_oValidTriangleEdgeIntersection(factors))
						{
							C0 factor = factors.z*2.0-1.0;
							if (sqr(factor*radius) < least_distance)
							{
								//ShowMessage("got quad");
								least_distance = sqr(factor*radius);
								Vec::mad(inner_center,q->normal[0],-factor*radius,e2);
								Mat::transform(entity->system,e2,position_out);
								Mat::rotate(entity->system,q->normal[0],normal_out);
								closest_entity = entity;
							}
						}
						
						Vec::mad(inner_center,q->normal[1],-radius,e2);
						Vec::mad(inner_center,q->normal[1],radius,e1);
						if (_oDetTriangleEdgeIntersection(q->v0->position,q->v2->position,q->v3->position,e1,e2,factors)
							&&
							_oValidTriangleEdgeIntersection(factors))
						{
							factors.z = factors.z*2.0-1.0;
							if (sqr(factors.z*radius) < least_distance)
							{
								//ShowMessage("got quad2");
								least_distance = sqr(factors[2]*radius);
								Vec::mad(inner_center,q->normal[1],-factors[2]*radius,e2);
								Mat::transform(entity->system,e2,position_out);
								Mat::rotate(entity->system,q->normal[1],normal_out);
								closest_entity = entity;
							}
						}
					}
				}
					
					
					
				if (unsigned cnt = obj.map->resolveEdges(ebuffer,ARRAYSIZE(ebuffer)))
				{
					for (unsigned i = 0; i < cnt; i++)
					{
						TVec3<C0> d;
						Vec::sub(ebuffer[i]->v1->position,ebuffer[i]->v0->position,d);
						C0 sub = Vec::dot(d);
						if (sub < getError<C0>())
							continue;
						C0 fc = (Vec::dot(inner_center,d)-Vec::dot(ebuffer[i]->v0->position,d))/sub;
						if (fc < -getError<C0>() || fc > 1.0f+getError<C0>())
							continue;
						TVec3<C0> p;
						Vec::mad(ebuffer[i]->v0->position,d,fc,p);
						C0 dist = Vec::quadraticDistance(p,inner_center);
						if (dist < least_distance)
						{
							TVec3<C0> normal;
							least_distance = dist;
							Vec::copy(ebuffer[i]->normal,normal);
							Vec::normalize0(normal);
							Vec::copy(p,position_out);
							Mat::transform(entity->system,p,position_out);
							Mat::rotate(entity->system,normal,normal_out);
							closest_entity = entity;
						}
					}
				}
					
			}
			SCENERY_END
			return 	closest_entity;
		}

	#if 0
	template <class GL, class Def> template <class C0, class C1>
		void 			MappedScenery<GL,Def>::render(const Aspect<C0>&aspect, const C1&max_range)
		{
			/*...*/
		}
		

	#endif
		
		
		
		
		
		
		
		
		
		
		



	/*





















	template <class C, class GL>RenderObject<C,GL>::RenderObject():vdata(NULL),idata(NULL),section(NULL),vcnt(0),vlyr(0),idata.length()(0),sections(0),remote(GL::EmptyHook)
	{}

	template <class C, class GL>
	template <class Def> RenderObject<C,GL>::RenderObject(const CGS::RenderObject<Def>&object):remote(GL::EmptyHook)
	{
	    unsigned v_size = VSIZE(object.vbnd,object.vlyr)*object.vcnt;
	    vdata = v_size?new FloatType[v_size]:NULL;
	    Vec::copy(object.vdata,vdata,v_size);
	    vcnt = object.vcnt;
	    vlyr = object.vlyr;
	    idata.length() = object.idata.length();
	    idata = idata.length()?new IndexType[idata.length()]:NULL;
	    Vec::copy(object.idata,idata,idata.length());
	    sections = object.sdata.length();
	    section = sections?new RenderSection<C>[sections]:NULL;
	    for (unsigned i = 0; i < sections; i++)
	        section[i] = object.sdata[i];
	}

	template <class C, class GL>RenderObject<C,GL>::RenderObject(const RenderObject<C,GL>&other):vlyr(other.vlyr),vcnt(other.vcnt),idata.length()(other.idata.length()),sections(other.sections),vbnd(other.vbnd)
	{
	    unsigned v_size = VSIZE(vbnd,vlyr)*vcnt;
	    vdata = v_size?new FloatType[v_size]:NULL;
	    idata = idata.length()?new IndexType[idata.length()]:NULL;
	    section = sections?new RenderSection<C>[sections]:NULL;
	    memcpy(vdata,other.vdata,vcnt*sizeof(FloatType));
	    memcpy(idata,other.idata,idata.length()*sizeof(IndexType));
	    for (unsigned i = 0; i < sections; i++)
	        section[i] = other.section[i];
	}

	template <class C, class GL>RenderObject<C,GL>::~RenderObject()
	{
	    if (vdata)
	        delete[] vdata;
	    if (idata)
	        delete[] idata;
	    if (section)
	        delete[] section;
	}

	template <class C, class GL>void RenderObject<C,GL>::drop(const C*system)
	{
	    

	}








	template <class C, class GL> class Material
	{
	public:
	        CGS::Material                *source;

	        bool                        masked;
	        TextureLayer<GL>      *layer;
	        UINT32                      layer_field.length();
	        GL::FloatType             colors[17];
	        RenderObject<C,GL>    *object;
	        unsigned                    object_field.length();

	template <class C, class GL>
	template <class Def> Material<C,GL>::Material(CGS::Material<Def>*src, SystemList<C,Def>*translation):source(src),masked(src->masked),layer_field.length()(src->layer_field.length()),object_field.length()(src->object_field.length())
	{
	    layer = layer_field.length()?new TextureLayer<GL>[layer_field.length()]:NULL;
	    Vec::copyV<Def::FloatType,GL::FloatType,17>(source->colors,colors);
	    for (unsigned i = 0; i < layer_field.length(); i++)
	    {
	        layer_field[i].tex_combiner = source->layer_field[i].combiner;
	        layer_field[i].tex_object = buildTexture<GL>(source->layer_field[i].source);
	        layer_field[i].cube_map = source->layer_field[i].source->faces == 6;
	        layer_field[i].normal_map = source->layer_field[i].normal_map;
	    }
	    object = object_field.length()? new RenderObject<C,GL>[object_field.length()]:NULL;
	    for (unsigned i = 0; i < object_field.length(); i++)
	    {

	        unsigned v_cnt = src->object_field[i].vcnt,
	                 v_bnd = src->object_field[i].vbnd,
	                 l_cnt = src->object_field[i].vlyr,
	                 i_cnt = src->object_field[i].idata.length();
	        GL::IndexType*index_field = i_cnt?new GL::IndexType[i_cnt]:NULL;
	        unsigned vsize = VSIZE(v_bnd,l_cnt),
	                 vn = v_cnt*vsize;
	        GL::FloatType*vertex_field = vn?new GL::FloatType[vn]:NULL;
	        Vec::copy(src->object_field[i].idata,index_field,i_cnt);
	        Vec::copy(src->object_field[i].vdata,vertex_field,vn);
	        for (unsigned j = 0; j < src->object_field[i].sdata.length(); j++)
	        {
	            C*system = translation->lookup(src->object_field[i].sdata[j].target)->system;
	            if (!system)
	                continue;
	            GL::FloatType*flocal = &vertex_field[src->object_field[i].sdata[j].voffset];
	            GL::IndexType*ilocal = &index_field[src->object_field[i].sdata[j].ioffset];
	            UINT32 index_crc = CRC32::getChecksum(ilocal,sizeof(GL::IndexType)*src->object_field[i].sdata[j].icount),
	                   vertex_crc = CRC32::getChecksum(flocal,sizeof(GL::FloatType)*vsize*src->object_field[i].sdata[j].vcount);
	            object_field[i].merge(index_crc,vertex_crc,ilocal,vlocal,src->object_field[i].sdata[j].icount,src->object_field[i].sdata[j].vcount,l_cnt,system);
	        }
	        if (index_field)
	            delete[] index_field;
	        if (vertex_field)
	            delete[] vertex_field;
	    }
	}

	template <class GL, class Def>Material<C,GL>::~Material()
	{
	    if (layer)
	        delete[] layer;
	    if (object)
	        delete[] object;
	}


	template <class GL, class Def>bool Material<C,GL>::similar(const Material<C,GL>&other)
	{
	    if (layer_field.length() != other.layer_field.length())
	        return false;
	    for (unsigned i = 0; i < layer_field.length(); i++)
	        if (other.layer_field[i].tex_combiner != layer_field[i].tex_combiner
	            ||
	            other.layer_field[i].tex_object != layer_field[i].tex_object
	            ||
	            (other.layer_field[i].cube_map && other.layer_field[i].mirror_map != layer_field[i].mirror_map))
	            return false;
	    return true;
	}

	template <class GL, class Def>void Material<C,GL>::merge(const Material<C,GL>&other)
	{
	    if (other.object_field.length() > object_field.length())
	    {
	         RenderObject<C,GL>*new_array = new RenderObject<C,GL>[other.object_field.length()];
	         for (unsigned i = 0; i < object_field.length(); i++)
	            new_array[i] = object_field[i];
	         delete[] object;
	         object = new_array;
	         object_field.length() = other.object_field.length();
	    }
	    for (unsigned i = 0; i < other.object_field.length(); i++)
	        object_field[i].merge(other.object_field[i]);
	}

	template <class GL, class Def>void Material<C,GL>::drop(const C*system)
	{
	    for (unsigned i = 0; i < object_field.length(); i++)
	        object_field[i].drop(system);
	}



	template <class GL, class Def>Material<C,GL>*Scenery<C,GL>::instance(const CGS::Material&material, SystemList<C>*translation)
	{
	    Material<C,GL>*fragment = new Material<C,GL>(material,translation);
	    reset();
	    while (Material*visual = each())
	        if (visual->similar(fragment))
	        {
	            visual->merge(fragment);
	            delete fragment;
	            return visual;
	        }
	    return append(fragment);
	}

	template <class GL, class Def>
	template <class C>
	void Scenery<GL,Def>::render(Aspect<C>&aspect)
	{
	        __transform(aspect.view,&system[12],center);
	        float distance = Vec::length(center);
	        int layer = log(distance/structure.data.dimension)/M_LN2;
	        if (layer < 0)
	           layer = 0;
	        if (layer > 4)
	           continue;
	        Vec::add(center,structure.data.radius,upper);
	        Vec::subtract(center,structure.data.radius,lower);
	        upper[2] *= -1;
	        lower[2] *= -1;
	        for (BYTE k = 0; k < 2; k++)
	        {
	            upper[k] = upper[k]<0?upper[k]/upper[2]:upper[k]/lower[2];
	            lower[k] = lower[k]<0?lower[k]/lower[2]:lower[k]/upper[2];
	        }
	        swap(upper[2],lower[2]);
	        if (_oneGreater(lower,visual_upper) || Vec::oneLesser(upper,visual_lower))
	            continue;
	}
	*/
}

#endif
