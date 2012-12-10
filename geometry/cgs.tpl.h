#ifndef cgsTplH
#define cgsTplH




#define CGS_POINT		{}//{if (CGS::log) (*CGS::log)<<shortenFile(__FILE__)<<"("<<__LINE__<<") <"<<__func__<<">"<<nl; }   //define void to disable logging
#define CGS_MSG(msg)	{}//{if (CGS::log) (*CGS::log)<<shortenFile(__FILE__)<<"("<<__LINE__<<") <"<<__func__<<">: "<<String(msg)<<nl; }   //define void to disable logging

/*
template <class C> String vstructArrayCompare(const String&name,unsigned cnt0,C*pntr0,unsigned cnt1,C*pntr1, const String&intend="")
{
	String rs;
	if (cnt0 != cnt1)
		rs+="\n"+intend+name+": "+String(cnt0)+" != "+String(cnt1);
	if ((pntr0 == pntr1) && pntr0)
		rs+="\n"+intend+name+": pointers equal";
	else
		for (index_t i = 0; i < cnt0 && i < cnt1; i++)
			rs+="\n "+intend+"("+String(i)+") "+pntr0[i].difference(pntr1[i],"	"+intend);
	return rs;
}*/



template <class Def>
	void			Tracks<Def>::adoptData(Tracks<Def>&other)
	{
		if (this == &other)
			return;
		domain = other.domain;
		name_field.adoptData(other.name_field);
		entry_field.adoptData(other.entry_field);
		rotation.adoptData(other.rotation);
		flags = other.flags;
	}



template <class Def>
template <class Def0> MaterialData<Def>& MaterialData<Def>::operator=(const MaterialData<Def0>&origin)
{
	coord_layers = origin.coord_layers;
	object_field = origin.object_field;
	return *this;
}


template <class Def> void MaterialData<Def>::postCopyLink(Geometry<Def>*domain)
{
	for (index_t i = 0; i < object_field.length(); i++)
		object_field[i].target = domain->lookupObject(object_field[i].tname);
}

template <class Def> void MaterialA<Def>::storeTargetNames()
{
	for (index_t i = 0; i < data.object_field.length(); i++)
		data.object_field[i].tname = data.object_field[i].target->name;
		
	for (index_t i = 0; i < info.layer_field.length(); i++)
		if (info.layer_field[i].source)
			info.layer_field[i].source_name = info.layer_field[i].source->name;
}

template <class Def>	template <class C>
	void	MaterialA<Def>::scale(const C&factor)
	{
		for (index_t i = 0; i < data.object_field.length(); i++)
			data.object_field[i].vpool.scale(factor);
	}



template <class Def> void MaterialA<Def>::adoptData(MaterialA<Def>&other)
{
	if (this == &other)
		return;
	info.adoptData(other.info);
	data.adoptData(other.data);
	name.adoptData(other.name);
}

template <class Def>
count_t	MaterialA<Def>::countTriangles()	const
{
	count_t rs = 0;
	for (index_t i = 0; i < data.object_field.length(); i++)
		rs += data.object_field[i].countTriangles();
	return rs;
}

template <class Def>
count_t	MaterialA<Def>::countQuads()	const
{
	count_t rs = 0;
	for (index_t i = 0; i < data.object_field.length(); i++)
		rs += data.object_field[i].countQuads();
	return rs;
}


template <class Def>
count_t	MaterialA<Def>::countFaces()	const
{
	count_t rs = 0;
	for (index_t i = 0; i < data.object_field.length(); i++)
		rs += data.object_field[i].countFaces();
	return rs;
}

template <class Def>
count_t	MaterialA<Def>::countTriangles(unsigned detail_layer)	const
{
	count_t rs = 0;
	for (index_t i = 0; i < data.object_field.length(); i++)
		rs += data.object_field[i].countTriangles(detail_layer);
	return rs;
}

template <class Def>
count_t	MaterialA<Def>::countQuads(unsigned detail_layer)	const
{
	count_t rs = 0;
	for (index_t i = 0; i < data.object_field.length(); i++)
		rs += data.object_field[i].countQuads(detail_layer);
	return rs;
}


template <class Def>
count_t	MaterialA<Def>::countFaces(unsigned detail_layer)	const
{
	count_t rs = 0;
	for (index_t i = 0; i < data.object_field.length(); i++)
		rs += data.object_field[i].countFaces(detail_layer);
	return rs;
}


template <class Def>
count_t	MaterialA<Def>::countVertices()	const
{
	count_t rs = 0;
	for (index_t i = 0; i < data.object_field.length(); i++)
		rs += data.object_field[i].vpool.vcnt;
	return rs;
}



template <class Def>
template <class Def0> MaterialA<Def>& MaterialA<Def>::operator=(const MaterialA<Def0>&origin)
{
	data = origin.data;
	info = origin.info;
	return *this;
}

template <class Def> void	MaterialA<Def>::postCopyLink(Geometry<Def>*domain, TextureResource*resource)
{
	data.postCopyLink(domain);
	info.postCopyLink(resource);
}

template <class Def> String MaterialA<Def>::toString(const String&intend)	const
{
	String rs = "Material\n"+intend;
	rs+="Layers: "+String(info.layer_field.length())+"\n"+intend;
	rs+="Objects: "+String(data.object_field.length())+"\n";
	for (index_t i = 0; i < data.object_field.length(); i++)
		rs+=intend+" ("+String(i)+") "+data.object_field[i].toString(intend+"	")+"\n";
	return rs;
}



/*
template <class Def> String MaterialA<Def>::difference(MaterialA&other, const String&intend)
{
	String rs;
	if (info.layers != other.info.layers)
		rs+="\n"+intend+"info.layers "+String(info.layers)+" != "+String(other.info.layers);
	if ((info.layer == other.info.layer) && info.layer)
		rs+="\n"+intend+"info.layers: pointers equal";
	if (!(info == other.info))
		rs+="\n"+intend+"info reports difference";
	rs+=vstructArrayCompare("objects",objects,object,other.objects,other.object,intend+" ");
	return rs;
}*/

template <class Def> void MaterialA<Def>::linkTextures(TextureResource*resource)
{
	if (!resource)
		FATAL__("passing NULL is invalid here");
	if (!this)
		FATAL__("THIS MISSING");
	for (index_t i = 0; i < info.layer_field.length(); i++)
	{
		TextureA*tex = resource->retrieve(info.layer_field[i].source_name);
		info.layer_field[i].source = tex;
		info.layer_field[i].cube_map = info.layer_field[i].source && info.layer_field[i].source->face_field.length() == 6;
	}
}

template <class Def> void MaterialA<Def>::setSize(count_t layer_count, count_t object_count)
{
	info.layer_field.setSize(layer_count);
	data.setSize(object_count);
}


template <class Def> VertexContainerA<Def>::VertexContainerA():vcnt(0),vlyr(0),vflags(0) {}


template <class Def> template <class C>
	void		VertexContainerA<Def>::scale(const C&factor)
	{
		UINT32 stride = vsize();
		typename Def::FloatType*at = vdata.pointer();
		Concurrency::parallel_for(UINT32(0),vcnt,[at,stride,factor](UINT32 i)
		{
			Vec::mult(Vec::ref3(at+i*stride),factor);
		});
	}



template <class Def> bool VertexContainerA<Def>::operator>(const VertexContainerA<Def>&other) const
{
	equal = false;
	if (vertex_crc > other.vertex_crc)
		return true;
	else
		if (vertex_crc < other.vertex_crc)
			return false;

	UINT32	vlen0 = VSIZE(vlyr,vflags)*vcnt,
			vlen1 = VSIZE(other.vlyr,other.vflags)*other.vcnt;
	if (vlen0 > vlen1)
		return true;
	else
		if (vlen0 < vlen1)
			return false;
	#ifndef FUZZY_COMPARE
	char rs = fastCompare(vdata.pointer(),other.vdata.pointer(),sizeof(typename Def::FloatType)*vlen0);
	if (rs > 0)
		return true;
	if (rs < 0)
		return false;
	#endif

	equal = true;
	return false;
}

template <class Def> bool VertexContainerA<Def>::operator<(const VertexContainerA<Def>&other) const
{
	return !equal;
}

template <class Def>
template <class Def0> VertexContainerA<Def>&	VertexContainerA<Def>::operator=(const VertexContainerA<Def0>&origin)
{
	vcnt = origin.vcnt;
	vlyr = origin.vlyr;
	vflags = origin.vflags;
	vdata = origin.vdata;
	return *this;
}





template <class Def> void VertexContainerA<Def>::stretch(UINT16 layers, UINT32 flags)
{
	if (layers == UNSIGNED_UNDEF)
		layers = vlyr;
	unsigned	step0 = VSIZE(vlyr,vflags),
				step1 = VSIZE(layers,flags);
	Array<typename Def::FloatType> new_field(vcnt*step1);

	typename Def::FloatType	*from = vdata.pointer(),
							*to = new_field.pointer();
	unsigned	lyr = vmin(vlyr,layers);
	bool		tan = !!(vflags&HasTangentFlag) && !!(flags&HasTangentFlag),
				norm = !!(vflags&HasNormalFlag) && !!(flags&HasNormalFlag),
				col = !!(vflags&HasColorFlag) && !!(flags&HasColorFlag);
	
	for (UINT32 i = 0; i < vcnt; i++)
	{
		copy3(from,to);
		to += 3;
		from += 3;
		if (norm)
		{
			copy3(from,to);
			from+=3;
			to+=3;
		}
		elif (flags&HasNormalFlag)
			to += 3;
		if (tan)
		{
			copy3(from,to);
			from+=3;
			to+=3;
		}
		elif (flags&HasTangentFlag)
			to+=3;
		if (col)
		{
			copy4(from,to);
			from+=4;
			to+=4;
		}
		elif (flags&HasColorFlag)
			to+=4;
		Vec::copyD(from,to,lyr*2);
		from+=vlyr*2;
		to+=layers*2;
	}
	vdata.adoptData(new_field);
	vlyr = layers;
	vflags = flags;
}


template <class Def> void VertexContainerA<Def>::setSize(count_t vertex_count, UINT16 layers, UINT32 flags)
{
	vdata.setSize(vertex_count * VSIZE(layers,flags));
	vcnt = UINT32(vertex_count);
	vlyr = UINT32(layers);
	vflags = flags;
}

template <class Def> void VertexContainerA<Def>::adoptData(VertexContainerA<Def>&other)
{
	if (this == &other)
		return;
	vdata.adoptData(other.vdata);
	vcnt = other.vcnt;
	vlyr = other.vlyr;
	vflags = other.vflags;
}

template <class Def> void VertexContainerA<Def>::updateCRC()
{
	vertex_crc = CRC32::getChecksum(vdata);
}

template <class Def> unsigned VertexContainerA<Def>::vsize() const
{
	return VSIZE(vlyr,vflags);
}

template <class Def> count_t VertexContainerA<Def>::vlen() const
{
	return vdata.length();
}






template <class Def > bool IndexContainerA<Def>::operator>(const IndexContainerA<Def>&other) const
{
	equal = false;
	if (index_crc > other.index_crc)
		return true;
	else
		if (index_crc < other.index_crc)
			return false;
	if (idata.length() > other.idata.length())
		return true;
	else
		if (idata.length() < other.idata.length())
			return false;
	if (triangles > other.triangles)
		return true;
	else
		if (triangles < other.triangles)
			return false;
	if (quads < other.quads)
		return true;
	else
		if (quads < other.quads)
			return false;
	#ifndef FUZZY_COMPARE
	char rs = fastCompare(idata,other.idata,idata.contentSize());
	if (rs > 0)
		return true;
	if (rs < 0)
		return false;
	#endif
	equal = true;
	return false;
}

template <class Def> bool IndexContainerA<Def>::operator<(const IndexContainerA<Def>&other) const
{
	return !equal;
}


template <class Def> void IndexContainerA<Def>::setSize(UINT32 triangles_, UINT32 quads_)
{
	idata.setSize(triangles_*3+quads_*4);
	triangles = triangles_;
	quads = quads_;
}

template <class Def>
template <class Def0> IndexContainerA<Def>& IndexContainerA<Def>::operator=(const IndexContainerA<Def0>&origin)
{
	idata = origin.idata;
	triangles = origin.triangles;
	quads = origin.quads;
	index_crc = origin.index_crc;
	return *this;
}

template <class Def> void IndexContainerA<Def>::adoptData(IndexContainerA<Def>&other)
{
	if (this == &other)
		return;
	idata.adoptData(other.idata);
	triangles = other.triangles;
	quads = other.quads;
	other.triangles = 0;
	other.quads = 0;
	index_crc = other.index_crc;
}

template <class Def> count_t	IndexContainerA<Def>::countFaces()					const
{
	return triangles+quads;
}

template <class Def> count_t	IndexContainerA<Def>::countTriangles()				const
{
	return triangles+quads*2;
}


template <class Def> count_t	IndexContainerA<Def>::countQuads()					const
{
	return quads;
}

/*
template <class Def> String IndexContainer<Def>::difference(SubGeometryChunk&other, const String&intend)
{
	String rs;
	#define COMPARE(token) if (token != other.token) \
							rs+="\n"+intend+#token+" "+String(token)+" != "+String(other.token)
	COMPARE(vcnt);
	COMPARE(vlyr);
	COMPARE(vbnd);
	COMPARE(icnt);
	COMPARE(ecnt);
	COMPARE(vertex_crc);
	COMPARE(index_crc);
	
	#undef COMPARE
	#define COMPARE(token, elements) {\
		unsigned difference = 0;\
		if ((token == other.token) && token)\
			rs+="\n"+intend+#token+": pointers equal";\
		else\
		{\
			for (index_t i = 0; i < element_field.length(); i++)\
				if (token[i] != other.token[i])\
				{\
					if (!difference)\
						rs+="\n"+intend+" ("+String(i)+") {";\
					if (difference < 5)\
						rs+=FloatToStr(token[i])+" != "+FloatToStr(other.token[i])+", ";\
					difference++;\
				}\
			if (difference >= 5)\
				rs+="...";\
			if (difference)\
				rs+="}";\
		}}

	unsigned cnt = vmin(vcnt*(3+vbnd+2*vlyr),other.vcnt*(3+other.vbnd+2*other.vlyr));
	COMPARE(vdata,cnt);
	COMPARE(idata,vmin(icnt,other.icnt));
	#undef COMPARE
	return rs;
}*/



template <class Def> RenderObjectA<Def>::RenderObjectA():target(NULL),detail(0),particle_config(0)
{}


template <class Def> template <class C>
	void	RenderObjectA<Def>::scale(const C&factor)
	{
		vpool.scale(factor);
	}


template <class Def> String RenderObjectA<Def>::toString(const String&intend)	const
{
	String result = "Object (Vertices: "+String(vpool.vcnt)+"; Detail: "+String(detail)+"; ";
	result += "Indices("+String(detail)+"): "+String(ipool.idata.length())+"; ";

	return result+")";
}

template <class Def>
	void		RenderObjectA<Def>::invert()
	{
		//for (index_t i = 0; i < detail_layer_field.length(); i++)
		{
			IndexContainerA<Def>&chunk = ipool;
			typename Def::IndexType*p = chunk.idata.pointer();
			for (UINT32 j = 0; j < chunk.triangles; j++)
			{
				swp(p[1],p[2]);
				p+=3;
			}
			for (UINT32 j = 0; j < chunk.quads; j++)
			{
				swp(p[0],p[3]);
				swp(p[1],p[2]);
				p+=4;
			}
			ASSERT_CONCLUSION(chunk.idata,p);
		}
	}


template <class Def> count_t RenderObjectA<Def>::countFaces()	const
{
	count_t rs = 0;
	//for (index_t j = 0; j < detail_layer_field.length(); j++)
		rs+=ipool.countFaces();
	return rs;
}

template <class Def> count_t RenderObjectA<Def>::countTriangles()	const
{
	count_t rs = 0;
	//for (index_t j = 0; j < detail_layer_field.length(); j++)
		rs += ipool.countTriangles();
	return rs;
}

template <class Def> count_t RenderObjectA<Def>::countQuads()	const
{
	count_t rs = 0;
	//for (index_t j = 0; j < detail_layer_field.length(); j++)
		rs += ipool.countQuads();
	return rs;
}


template <class Def> count_t RenderObjectA<Def>::countFaces(unsigned detail_layer)	const
{
	if (detail_layer == detail)
		return ipool.countFaces();
	return 0;
}

template <class Def> count_t RenderObjectA<Def>::countTriangles(unsigned detail_layer)	const
{
	if (detail_layer == detail)
		return ipool.countTriangles();
	return 0;
}

template <class Def> count_t RenderObjectA<Def>::countQuads(unsigned detail_layer)	const
{
	if (detail_layer == detail)
		return ipool.countQuads();
	return 0;
}


template <class Def> bool RenderObjectA<Def>::operator>(const RenderObjectA<Def>&other) const
{
	if (vpool>other.vpool)
		return true;
	if (vpool<other.vpool)
	{
		equal = false;
		return false;
	}
	if (detail > other.detail)
		return true;
	if (detail < other.detail)
	{
		equal = false;
		return false;
	}
	{
		if (ipool > ipool)
			return true;
		if (ipool < ipool)
		{
			equal = false;
			return false;
		}
	}
	equal = true;
	return false;
}

template <class Def> bool RenderObjectA<Def>::operator<(const RenderObjectA<Def>&other) const
{
	return !equal;
}

template <class Def> void RenderObjectA<Def>::adoptData(RenderObjectA<Def>&other)
{
	if (this == &other)
		return;
	ipool.adoptData(other.ipool);
	vpool.adoptData(other.vpool);
	particle_field.adoptData(other.particle_field);
	particle_config = other.particle_config;
	target = other.target;
	tname = other.tname;
	detail = other.detail;
}


template <class Def> count_t RenderObjectA<Def>::countIndices()	const
{
	count_t result(0);
		result+=ipool.idata.length();
	return result;
}

template <class Def>
template <class Def0> RenderObjectA<Def>& RenderObjectA<Def>::operator=(const RenderObjectA<Def0>&origin)
{
	ipool = origin.ipool;
	detail = origin.detail;
	vpool = origin.vpool;
	tname = origin.tname;
	target = NULL;
	return *this;
}


template <class Def> bool RenderObjectA<Def>::validIndices()
{
	for (index_t i = 0; i < ipool.idata.length(); i++)
		if (ipool.idata[i] >= vpool.vcnt)
			return false;
	return ipool.triangles*3+ipool.quads*4 == ipool.idata.length();
}


template <class Def> bool MaterialData<Def>::operator>(const MaterialData<Def>&other) const
{
	equal = false;
	if (object_field.length() > other.object_field.length())
		return true;
	if (object_field.length() < other.object_field.length())
	{
		equal = false;
		return false;
	}
	for (index_t i = 0; i < object_field.length(); i++)
	{
		if (object_field[i].vpool > other.object_field[i].vpool)
			return true;
		if (object_field[i].vpool < other.object_field[i].vpool)
		{
			equal = false;
			return false;
		}
		if (object_field[i].detail > other.object_field[i].detail)
			return true;
		if (object_field[i].detail < other.object_field[i].detail)
		{
			equal = false;
			return false;
		}
		//for (index_t j = 0; j < object_field[i].detail_layer_field.length(); j++)
		{
			if (object_field[i].ipool > other.object_field[i].ipool)
				return true;
			if (object_field[i].ipool < other.object_field[i].ipool)
				return false;
		}
	}
	equal = true;
	return false;
}

template <class Def> void MaterialData<Def>::adoptData(MaterialData<Def>&other)
{
	if (this == &other)
		return;
	object_field.adoptData(other.object_field);
	coord_layers = other.coord_layers;
}

template <class Def> bool MaterialData<Def>::operator<(const MaterialData<Def>&other) const
{
	return !equal;
}





/*
template <class Def> String RenderObjectA<Def>::difference(RenderObjectA&other, const String&intend)
{
	return vstructArrayCompare("chunks",chunks,chunk,other.chunks,other.chunk,intend+" ");
}*/

template <typename T>
	System<T>::System(Callback*callback_):callback(callback_)
	{
		loadIdentity(false);
	}
	
template <typename T>
	System<T>::System(const System<T>&other):matrix(other.matrix),callback(NULL)
	{}
	
template <typename T> template <typename T2>
	System<T>::System(const System<T2>&other):callback(NULL)
	{
		Mat::copy(other.matrix,matrix);
	}
	
template <typename T>
	System<T>&				System<T>::operator=(const System<T>&other)
	{
		matrix = other.matrix;
		return *this;
	}
	
template <typename T> template <typename T2>
	System<T>&				System<T>::operator=(const System<T2>&other)
	{
		Mat::copy(other.matrix,matrix);
		return *this;
	}
	
	
template <typename T>
	inline void	System<T>::loadIdentity(bool do_callback)
	{
		Mat::eye(matrix);
		if (do_callback && callback)
			callback->onSystemChange();
	}
	
template <typename T>
	inline void	System<T>::setPosition(const T&x, const T&y, const T&z)
	{
		matrix.w.x = x;
		matrix.w.y = y;
		matrix.w.z = z;
		if (callback)
			callback->onSystemChange();
	}
	
template <typename T>
	inline void	System<T>::setPositionNoCallback(const T&x, const T&y, const T&z)
	{
		matrix.w.x = x;
		matrix.w.y = y;
		matrix.w.z = z;
	}
	
template <typename T> template <typename T2>
	inline void	System<T>::setPosition(const TVec3<T2>&p)
	{
		Vec::copy(p,matrix.w.xyz);
		if (callback)
			callback->onSystemChange();
	}
	
template <typename T> template <typename T2>
	inline void	System<T>::setPositionNoCallback(const TVec3<T2>&p)
	{
		Vec::copy(p,matrix.w.xyz);
	}
	
template <typename T>
	inline void	System<T>::moveTo(const T&x, const T&y, const T&z)
	{
		Vec::def(matrix.w.xyz,x,y,z);
		if (callback)
			callback->onSystemChange();
	}
	
template <typename T>
	inline void	System<T>::moveToNoCallback(const T&x, const T&y, const T&z)
	{
		Vec::def(matrix.w.xyz,x,y,z);
	}
	
template <typename T> template <typename T2>
	inline void	System<T>::moveTo(const TVec3<T2>&p)
	{
		Vec::copy(p,matrix.w);
		if (callback)
			callback->onSystemChange();
	}
	
template <typename T> template <typename T2>
	inline void	System<T>::moveToNoCallback(const TVec3<T2>&p)
	{
		Vec::copy(p,matrix.w);
	}
	
	
	
template <typename T>			
	inline void	System<T>::translate(const T&x, const T&y, const T&z)
	{
		matrix[12] += x;
		matrix[13] += y;
		matrix[14] += z;
		if (callback)
			callback->onSystemChange();
	}
	
template <typename T>
	inline void	System<T>::translateBy(const T&x, const T&y, const T&z)
	{
		matrix[12] += x;
		matrix[13] += y;
		matrix[14] += z;
		if (callback)
			callback->onSystemChange();
	}
	
template <typename T> template <typename T2>
	inline void	System<T>::translate(const TVec3<T2>&vector)
	{
		Vec::add(matrix.w.xyz,vector);
		if (callback)
			callback->onSystemChange();
	}
	
template <typename T> template <typename T2>
	inline void	System<T>::translate(const TVec3<T2>&vector, const T&factor)
	{
		Vec::mad(matrix.w.xyz,vector,factor);
		if (callback)
			callback->onSystemChange();
	}
	
template <typename T>
	void	System<T>::rotate(const T&angle, const T&axis_x,const T&axis_y, const T&axis_z)
	{
		TMatrix3<T>	rotation_matrix;

		Mat::rotationMatrix(angle,axis_x,axis_y,axis_z,rotation_matrix);

		Mat::mult(rotation_matrix,matrix.x.xyz);
		Mat::mult(rotation_matrix,matrix.y.xyz);
		Mat::mult(rotation_matrix,matrix.z.xyz);
		
		if (callback)
			callback->onSystemChange();
	}
	
template <typename T> template <typename T2>
	void	System<T>::rotate(const T&angle, const TVec3<T2>&axis)
	{
		TMatrix3<T>	rotation_matrix;
		Mat::rotationMatrix(angle,axis,rotation_matrix);

		Mat::mult(rotation_matrix,matrix.x.xyz);
		Mat::mult(rotation_matrix,matrix.y.xyz);
		Mat::mult(rotation_matrix,matrix.z.xyz);
	
		if (callback)
			callback->onSystemChange();
	}

template <typename T>			
	inline void	System<T>::translateNoCallback(const T&x, const T&y, const T&z)
	{
		matrix.w.x += x;
		matrix.w.y += y;
		matrix.w.z += z;
	}
	
	
template <typename T> template <typename T2>
	inline void	System<T>::translateNoCallback(const TVec3<T2>&vector)
	{
		Vec::add(matrix.w.xyz,vector);
	}
	
template <typename T> template <typename T2>
	inline void	System<T>::translateNoCallback(const TVec3<T2>&vector, const T&factor)
	{
		Vec::mad(matrix.w.xyz,vector,factor);
	}
	
template <typename T>
	void	System<T>::rotateNoCallback(const T&angle, const T&axis_x,const T&axis_y, const T&axis_z)
	{
		TMatrix3<T>	rotation_matrix;

		Mat::rotationMatrix(angle,axis_x,axis_y,axis_z,rotation_matrix);

		Mat::mult(rotation_matrix,matrix.x.xyz);
		Mat::mult(rotation_matrix,matrix.y.xyz);
		Mat::mult(rotation_matrix,matrix.z.xyz);
	}
	
template <typename T> template <typename T2>
	void	System<T>::rotateNoCallback(const T&angle, const TVec3<T2>&axis)
	{
		TMatrix3<T>	rotation_matrix;
		Mat::rotationMatrix(angle,axis,rotation_matrix);

		Mat::mult(rotation_matrix,matrix.x.xyz);
		Mat::mult(rotation_matrix,matrix.y.xyz);
		Mat::mult(rotation_matrix,matrix.z.xyz);
	}


	
template <class Def>
	Iterator<Def>::Iterator(Geometry<Def>*target):super(target),context(&target->object_field),current(target->object_field.pointer())
	{}
template <class Def>
	Iterator<Def>::Iterator(Geometry<Def>&target):super(&target),context(&target.object_field),current(target.object_field.pointer())
	{}

template <class Def>	
	SubGeometryA<Def>&		Iterator<Def>::operator*()		const
	{
		return *current;
	}
	
template <class Def>
	SubGeometryA<Def>*		Iterator<Def>::operator->()	const
	{
		return current;
	}
	
template <class Def>	
	bool					Iterator<Def>::operator++()
	{
		if (!current)
			return false;
		//check inner content (walk up by one)
		if (current->child_field.length())
		{
			context_stack.append(context);
			entry_stack.append(current);
			context = &current->child_field;
			current = context->pointer();
			return true;
		}
		current++;
		//check termination (walk down all the way):
		while (context && context->concludedBy(current))
		{
			context = context_stack.drop(context_stack.count()-1);
			current = entry_stack.drop(entry_stack.count()-1);
			if (current)
				current++;
		}
		return current != NULL;
		
		
		/*
		current++;
		//check termination (walk down by one):
		if (context && context->concludedBy(current))
		{
			context = context_stack.drop(context_stack.count()-1);
			current = entry_stack.drop(entry_stack.count()-1);
			return current != NULL;
		}
		if (!context)
			return false;
		
		//check inner content (walk up all the way):
		while (current->child_field.length())
		{
			context_stack.append(context);
			entry_stack.append(current);
			context = &current->child_field;
			current = context->pointer();
		}
		return true;
		*/
	}
	
template <class Def>
	bool				Iterator<Def>::operator++(int)
	{
		return operator++();
	}
	
template <class Def>
	Iterator<Def>::operator bool()	const
	{
		return current != NULL;
	}

	

template <class Def> SubGeometryA<Def>::SubGeometryA():system_link(&path),name(0)
{
	meta.flags = 0;
	meta.radius = 0;
	meta.volume = 0;
	meta.density = 1;
	meta.shortest_edge_length = 0.1f;
	Mat::eye(meta.system);
}


template <class Def> void SubGeometryA<Def>::clear()
{
	child_field.free();
	ph_hull.clear();
	accelerator_field.free();
	mounting_field.free();
	wheel_field.free();
	tracks_field.free();
	vs_hull_field.free();
	meta.radius = 0;
	meta.volume = 0;
	meta.density = 1;
	meta.shortest_edge_length = 0.1f;
	meta.flags = 0;
	Mat::eye(meta.system);
	system_link = &path;
}

template <class Def>
	void	SubGeometryA<Def>::clearFlags(UINT32 clear_mask, bool geometries, bool wheels, bool accelerators, bool constructs)
	{
		UINT32 mod = (0xFFFFFFFF^clear_mask);
		if (geometries)
			meta.flags &= mod;
		if (wheels)
			for (index_t i = 0; i < wheel_field.length(); i++)
				wheel_field[i].flags &= mod;
		if (accelerators)
			for (index_t i = 0; i < accelerator_field.length(); i++)
				accelerator_field[i].flags &= mod;
		if (constructs)
			for (index_t i = 0; i < tracks_field.length(); i++)
				tracks_field[i].flags &= mod;
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].clearFlags(clear_mask,geometries,wheels,accelerators,constructs);
	}

template <class Def>
	void	SubGeometryA<Def>::flagAnimationTargets(UINT32 flag)
	{
		for (index_t i = 0; i < wheel_field.length(); i++)
		{
			{
				AnimatorA<Def>&a = wheel_field[i].rotation;
				for (index_t j = 0; j < a.obj_trace_field.length(); j++)
					a.obj_trace_field[j].target->meta.flags |= flag;
				for (index_t j = 0; j < a.whl_trace_field.length(); j++)
					a.whl_trace_field[j].target->flags |= flag;
				for (index_t j = 0; j < a.acc_trace_field.length(); j++)
					a.acc_trace_field[j].target->flags |= flag;
			}
			{
				AnimatorA<Def>&a = wheel_field[i].suspension;
				for (index_t j = 0; j < a.obj_trace_field.length(); j++)
					a.obj_trace_field[j].target->meta.flags |= flag;
				for (index_t j = 0; j < a.whl_trace_field.length(); j++)
					a.whl_trace_field[j].target->flags |= flag;
				for (index_t j = 0; j < a.acc_trace_field.length(); j++)
					a.acc_trace_field[j].target->flags |= flag;
			}
		}
		/*
		for (index_t i = 0; i < tracks_field.length(); i++)
		{
			{

				AnimatorA<Def>&a = tracks_field[i].rotation;
				for (index_t j = 0; j < a.obj_trace_field.length(); j++)
					a.obj_trace_field[j].target->meta.flags |= flag;
				for (index_t j = 0; j < a.whl_trace_field.length(); j++)
					a.whl_trace_field[j].target->flags |= flag;
				for (index_t j = 0; j < a.acc_trace_field.length(); j++)
					a.acc_trace_field[j].target->flags |= flag;
			}
		}*/
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].flagAnimationTargets(flag);
	}

template <class Def>
	void	SubGeometryA<Def>::walkChildrenRec(void (*childCallback)(SubGeometryA<Def>*))
	{
		childCallback(this);
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].walkChildrenRec(childCallback);
	}

template <class Def>
	void	SubGeometryA<Def>::walkChildrenRec(SubGeometryCallback<Def>*callback)
	{
		callback->geometryCallback(this);
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].walkChildrenRec(callback);
	}
	

template <class Def>
	void	SubGeometryA<Def>::createVisualMapsRec(unsigned detail, unsigned max_depth)
	{
		if (detail < vs_hull_field.count() && !vs_hull_field[detail].getMap())
		{
			typedef ObjectMath::Mesh<VsDef>	VsMesh;
			VsMesh&obj = vs_hull_field[detail];
			obj.buildMap(O_ALL,max_depth);
		
			for (index_t i = 0; i < obj.vertex_field.length(); i++)
				Vec::clear(obj.vertex_field[i].normal);
			for (index_t i = 0; i < obj.triangle_field.length(); i++)
			{
				VsMesh::Triangle&t = obj.triangle_field[i];
				_oTriangleNormal(t.v0->position,t.v1->position,t.v2->position,t.normal);
				Vec::add(t.v0->normal,t.normal);
				Vec::add(t.v1->normal,t.normal);
				Vec::add(t.v2->normal,t.normal);
				Vec::normalize0(t.normal);
			}
			for (index_t i = 0; i < obj.quad_field.length(); i++)
			{
				VsMesh::Quad&q = obj.quad_field[i];
				_oTriangleNormal(q.v0->position,q.v1->position,q.v2->position,q.normal);
				_oAddTriangleNormal(q.v0->position,q.v2->position,q.v3->position,q.normal);
				Vec::add(q.v0->normal,q.normal);
				Vec::add(q.v1->normal,q.normal);
				Vec::add(q.v2->normal,q.normal);
				Vec::add(q.v3->normal,q.normal);
				Vec::normalize0(q.normal);
			}
			for (index_t i = 0; i < obj.vertex_field.length(); i++)
				Vec::normalize0(obj.vertex_field[i].normal);
			for (index_t i = 0; i < obj.edge_field.length(); i++)
			{
				VsMesh::Edge&e = obj.edge_field[i];
				Vec::clear(e.normal);
				if (e.n[0])
				{
					if (e.n[0].is_quad)
					{
						Vec::copy(e.n[0].quad->normal,e.normal);
					}
					else
						Vec::copy(e.n[0].triangle->normal,e.normal);
				}
				if (e.n[1])
				{
					if (e.n[1].is_quad)
					{
						Vec::add(e.normal,e.n[1].quad->normal);
					}
					else
						Vec::add(e.normal,e.n[1].triangle->normal);
				}
				Vec::normalize0(e.normal);
			}
		}
		
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].createVisualMapsRec(detail,max_depth);
	}
	
template <class Def>	template <class C>
	void	SubGeometryA<Def>::scale(const C&factor)
	{
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].scale(factor);
		
		ph_hull.scale(factor);
		for (index_t i = 0; i < vs_hull_field.length(); i++)
			vs_hull_field[i].scale(factor);
		
		for (index_t i = 0; i < wheel_field.length(); i++)
		{
			Vec::mult(wheel_field[i].position,factor);
			Vec::mult(wheel_field[i].contraction,factor);
			wheel_field[i].radius*=factor;
			wheel_field[i].width*=factor;
			wheel_field[i].suspension.scale(factor);
			wheel_field[i].rotation.scale(factor);
		}
			
		for (index_t i = 0; i < accelerator_field.length(); i++)
			Vec::mult(accelerator_field[i].position,factor);
		
		for (index_t i = 0; i < mounting_field.length(); i++)
			Vec::mult(mounting_field[i].position,factor);
		
		Vec::mult(meta.system.w.xyz,factor);
		Vec::mult(meta.center,factor);
		meta.radius *= factor;
		meta.volume *= factor*factor*factor;
		meta.shortest_edge_length *= factor;
	}


template <class Def> String SubGeometryA<Def>::toString(const String&intend)	const
{

	String rs = "<"+name+">\n"+intend;
	rs+="Wheels: "+String(wheel_field.length())+"\n"+intend;
	rs+="Accelerators: "+String(accelerator_field.length())+"\n"+intend;
	rs+="Mountings: "+String(mounting_field.length())+"\n"+intend;
	rs+="Hull: "+ph_hull.toString()+"\n"+intend;
	for (index_t i = 0; i < vs_hull_field.length(); i++)
		rs+="VHull("+String(i)+"): "+vs_hull_field[i].toString()+"\n"+intend;
	rs+="Children: "+String(child_field.length())+"\n";
	for (index_t i = 0; i < child_field.length(); i++)
		rs+=intend+" ("+String(i)+") "+child_field[i].toString(intend+"	")+"\n";

	return rs;
}

/*
template <class Def> String SubGeometryA<Def>::difference(SubGeometryA&other, const String&intend)
{
	String rs;
	if (vs_hull_field.length() != other.vs_hull_field.length())
		rs+="\n"+intend+"objects "+String(vs_hull_field.length())+" != "+String(other.vs_hull_field.length());
	if ((vs_hull == other.vs_hull) && vs_hull)
		rs+="\n"+intend+"vs_hull: pointers equal";
	else
		for (index_t i = 0; i < vs_hull_field.length() && i < other.vs_hull_field.length(); i++)
			rs+="\n"+intend+"vs_hull("+String(i)+") "+vs_hull[i].difference(other.vs_hull[i],intend+" ");
	rs+="\n"+intend+"ph_hull "+ph_hull.difference(other.ph_hull,intend+" ");

	if (constructs != other.constructs)
		rs+="\n"+intend+"constructs "+String(constructs)+" != "+String(other.constructs);
	if ((construct == other.construct) && construct)
		rs+="\n"+intend+"constructs: pointers equal";
	if (wheels != other.wheels)
		rs+="\n"+intend+"wheels "+String(wheels)+" != "+String(other.wheels);
	if ((wheel == other.wheel) && wheel)
		rs+="\n"+intend+"wheels: pointers equal";
	if (accelerators != other.accelerators)
		rs+="\n"+intend+"accelerators "+String(accelerators)+" != "+String(other.accelerators);
	if ((accelerator == other.accelerator) && accelerator)
		rs+="\n"+intend+"accelerators: pointers equal";
	rs+=vstructArrayCompare("child_field.length()",child_field.length(),child,other.child_field.length(),other.child,intend+" ");
	return rs;
}*/

template <class Def> void SubGeometryA<Def>::saveToRiff(RiffChunk*riff)	const
{
	riff->appendBlock("OBJ ",name.c_str(),name.length());

	RiffChunk*inner = riff->appendBlock(RIFF_LIST);
		{
			ByteStream	m;
			//typename Def::SystemType	field[22];
			m.push(meta.system.v,16);
			m.push(meta.center.v,3);
			m << meta.radius
				<< meta.volume
				<< meta.density
				<< meta.shortest_edge_length;
			//field[19] = meta.radius;
			//ield[20] = meta.volume;
			//field[21] = meta.density;
			inner->appendBlock("META",m.data(),m.fillLevel());
		}
		
		for (index_t i = 0; i < vs_hull_field.length(); i++)
		{
			typedef Mesh<VsDef>	VsMesh;
			const VsMesh&hull = vs_hull_field[i];
			{
				Array<typename Def::FloatType> out(hull.vertex_field.length()*3);
				for (index_t j = 0; j < hull.vertex_field.length(); j++)
					Vec::copy(hull.vertex_field[j].position,Vec::ref3(out + j*3));
				inner->appendBlock("VVX3",out);
    		}
			{
				Array<UINT32>	out(hull.triangle_field.length()*3);
				for (index_t j = 0; j < hull.triangle_field.length(); j++)
				{
					const VsMesh::Triangle	&tri = hull.triangle_field[j];
					out[j*3 + 0] = static_cast<UINT32>(tri.vertex[0]-hull.vertex_field);
					out[j*3 + 1] = static_cast<UINT32>(tri.vertex[1]-hull.vertex_field);
					out[j*3 + 2] = static_cast<UINT32>(tri.vertex[2]-hull.vertex_field);
				}
				inner->appendBlock("VFC3",out);
			}
			{
				Array<UINT32>	out(hull.quad_field.length()*4);
				for (index_t j = 0; j < hull.quad_field.length(); j++)
				{
					const VsMesh::Quad	&quad = hull.quad_field[j];
					out[j*4 + 0] = static_cast<UINT32>(quad.vertex[0]-hull.vertex_field);
					out[j*4 + 1] = static_cast<UINT32>(quad.vertex[1]-hull.vertex_field);
					out[j*4 + 2] = static_cast<UINT32>(quad.vertex[2]-hull.vertex_field);
					out[j*4 + 3] = static_cast<UINT32>(quad.vertex[3]-hull.vertex_field);
				}
				inner->appendBlock("VFC4",out);
			}			
			{
				Array<INT32>out(hull.edge_field.length()*4);	//2*vertex. 2*face. positive: triangle, negative: quad+1
				for (index_t j = 0; j < hull.edge_field.length(); j++)
				{
					const VsMesh::Edge	&edge = hull.edge_field[j];
					
					out[j*4 + 0] = static_cast<UINT32>(edge.vertex[0]-hull.vertex_field);
					out[j*4 + 1] = static_cast<UINT32>(edge.vertex[1]-hull.vertex_field);
					out[j*4 + 2] = static_cast<UINT32>(hull.linkToIndex(edge.n[0]));
					out[j*4 + 3] = static_cast<UINT32>(hull.linkToIndex(edge.n[1]));
				}
				inner->appendBlock("VEDG",out);
			}
		}

		if (!ph_hull.isEmpty())
		{
			{
				Array<typename Def::PhHullFloatType> out(ph_hull.vertex_field.length()*3);
				for (index_t j = 0; j < ph_hull.vertex_field.length(); j++)
				{
					Vec::copy(ph_hull.vertex_field[j].position,Vec::ref3(out + j*3));
					//ASSERT_IS_CONSTRAINED3__(out + j*3,-30,30);

				}
				inner->appendBlock("HVX3",out);
    		}
		
			ByteStream	out;	//3*vindex, group, name, grip, updrift
			for (index_t i = 0; i < ph_hull.triangle_field.length(); i++)
			{
				const Mesh<PhDef>::Triangle	&triangle = ph_hull.triangle_field[i];
				out << (UINT32)(triangle.vertex[0]-ph_hull.vertex_field)
					<< (UINT32)(triangle.vertex[1]-ph_hull.vertex_field)
					<< (UINT32)(triangle.vertex[2]-ph_hull.vertex_field);
			}
			inner->appendBlock("HTRI",out.data(),out.fillLevel());
		
			out.reset();
			for (index_t i = 0; i < ph_hull.quad_field.length(); i++)
			{
				const Mesh<PhDef>::Quad	&quad = ph_hull.quad_field[i];
				out << (UINT32)(quad.vertex[0]-ph_hull.vertex_field)
					<< (UINT32)(quad.vertex[1]-ph_hull.vertex_field)
					<< (UINT32)(quad.vertex[2]-ph_hull.vertex_field)
					<< (UINT32)(quad.vertex[3]-ph_hull.vertex_field);
			}
			inner->appendBlock("HQAD",out.data(),out.fillLevel());

			{
				Array<UINT32>out(ph_hull.edge_field.length()*4);	//2*vertex. 2*face
				for (index_t j = 0; j < ph_hull.edge_field.length(); j++)
				{
					const Mesh<PhDef>::Edge	&edge = ph_hull.edge_field[j];
						
					out[j*4 + 0] = static_cast<UINT32>(edge.vertex[0]-ph_hull.vertex_field);
					out[j*4 + 1] = static_cast<UINT32>(edge.vertex[1]-ph_hull.vertex_field);
					out[j*4 + 2] = static_cast<UINT32>(ph_hull.linkToIndex(edge.n[0]));
					out[j*4 + 3] = static_cast<UINT32>(ph_hull.linkToIndex(edge.n[1]));
				}
				inner->appendBlock("HEDG",out);
			}
		}
		
		if (accelerator_field.length())
		{
			Buffer<BYTE>	write_buffer;
			ByteStream	a;
			for (index_t i = 0; i < accelerator_field.length(); i++)
			{
				a.reset();
				a.push(accelerator_field[i].position.v,3);
				a.push(accelerator_field[i].direction.v,3);
				a << accelerator_field[i].power << accelerator_field[i].zero_efficiency;
				a << accelerator_field[i].flame_length << accelerator_field[i].flame_width;
				a.push(accelerator_field[i].flame_color.v,3);
				a.push(accelerator_field[i].name.c_str(),accelerator_field[i].name.length());
				inner->appendBlock("ACC2", a.data(),a.fillLevel());
			}
		}

		if (mounting_field.length())
		{
			
		
			ByteStream	a(sizeof(UINT32)+13*sizeof(typename Def::FloatType));
			for (index_t i = 0; i < mounting_field.length(); i++)
			{
				a.reset();
				a.push(mounting_field[i].flags);
				a.push(mounting_field[i].position.v,3);
				a.push(mounting_field[i].direction.v,3);
				a.push(mounting_field[i].vertical.v,3);
				a.push(mounting_field[i].dimension.v,3);
				a.push(mounting_field[i].max_payload);
				inner->appendBlock("MNT ", a.data(),a.fillLevel());
			}
		}

		if (wheel_field.length())
		{
			ByteStream	w;
			for (index_t i = 0; i < wheel_field.length(); i++)
			{
				w.reset();
				//w.push(wheel_field[i].name);
				//w.push(wheel_field[i].next?wheel_field[i].next->name:(tName)0);
				//w.push(wheel_field[i].prev?wheel_field[i].prev->name:(tName)0);
				w.push(wheel_field[i].position.v,3);
				w.push(wheel_field[i].contraction.v,3);
				w.push(wheel_field[i].axis.v,3);
				w.push(wheel_field[i].radius);
				w.push(wheel_field[i].width);
				w.push(wheel_field[i].parameter,ARRAYSIZE(wheel_field[i].parameter));
				w.push(wheel_field[i].name.c_str(),wheel_field[i].name.length());
				inner->appendBlock("WHL2",w.data(),w.fillLevel());
				wheel_field[i].suspension.saveToRiff(inner);
				wheel_field[i].rotation.saveToRiff(inner);
			}
		}
		if (tracks_field.isNotEmpty())
		{
			ByteStream	c;
			for (index_t i = 0; i < tracks_field.length(); i++)
			{
				const Tracks<Def>&t = tracks_field[i];
				c.reset();
				c << (UINT32)t.member_field.length();

				for (index_t k = 0; k < t.member_field.length(); k++)
				{
					c << t.member_field[k].name.length();
					c.push(t.member_field[k].name.c_str(),t.member_field[k].name.length());
				}

				//for (index_t k = 0; k < tracks_field[i].entry_field.length(); k++)
				//	c.push(tracks_field[i].entry_field[k]->name);
				inner->appendBlock("TRK2",c.data(),c.fillLevel());
				//tracks_field[i].rotation.saveToRiff(inner);
			}
		}
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].saveToRiff(inner);
}

template <class Def> void SubGeometryA<Def>::loadFromRiff(RiffFile&riff) //assume we're alread in the object-list, have name, smooth_groups, layers and materials set
{
	clear();

	if (riff.findFirst("META") /*&& riff.getSize() == sizeof(typename Def::SystemType)*22*/)
	{
		riff.openStream();
			riff.stream(meta.system.v,16);
			riff.stream(meta.center.v,3);
			riff.stream(meta.radius);
			riff.stream(meta.volume);
			riff.stream(meta.density);
			if (!riff.stream(meta.shortest_edge_length))
				meta.shortest_edge_length = 0.1f;
		riff.closeStream();
	}

	count_t objects(0);
	if (riff.findFirst("VVTX"))
		do
			if (riff.multipleOf(sizeof(typename Def::FloatType)*4))
				objects++;
		while (riff.findNext("VVTX"));
	
	if (riff.findFirst("VVX3"))
		do
			if (riff.multipleOf(sizeof(typename Def::FloatType)*3))
				objects++;
		while (riff.findNext("VVX3"));

	vs_hull_field.setSize(objects);
	count_t c(0);
	
	if (riff.findFirst("VVTX"))
		do
			if (riff.multipleOf(sizeof(typename Def::FloatType)*4))
			{
				typedef Mesh<VsDef>	VsMesh;
				VsMesh&hull = vs_hull_field[c];
				hull.clear();
				{
					Array<typename Def::FloatType>	array;
					riff.get(array);
					hull.vertex_field.setSize(array.length()/4);
					for (index_t i = 0; i < hull.vertex_field.length(); i++)
					{
						hull.vertex_field[i].position = Vec::ref3(array + i*4);
						hull.vertex_field[i].index = i;
						hull.vertex_field[i].marked = false;
					}
				}
				if (riff.findNext("VFCE") && riff.multipleOf(sizeof(UINT32)*4))
				{
					Array<UINT32>	array;
					riff.get(array);
					hull.triangle_field.setSize(array.length()/4);
					for (index_t i = 0; i < hull.triangle_field.length(); i++)
					{
						VsMesh::Triangle	&face = hull.triangle_field[i];
						face.vertex[0] = hull.vertex_field+array[i*4];
						face.vertex[1] = hull.vertex_field+array[i*4+1];
						face.vertex[2] = hull.vertex_field+array[i*4+2];
						face.index = i;
						face.marked = false;
					}
				}
				if (riff.findNext("VEDG") && riff.multipleOf(sizeof(UINT32)*4))
				{
					Array<UINT32>	array;
					riff.get(array);
					hull.edge_field.setSize(array.length()/4);
					for (index_t i = 0; i < hull.edge_field.length(); i++)
					{
						VsMesh::Edge	&edge = hull.edge_field[i];
						edge.vertex[0] = hull.vertex_field+array[i*4];
						edge.vertex[1] = hull.vertex_field+array[i*4+1];
						hull.indexToLink(array[i*4+2],edge.n[0]);
						hull.indexToLink(array[i*4+3],edge.n[1]);
						edge.index = i;
						edge.marked = false;
					}
				}
				hull.correct();
				c++;
			}
		while (riff.findNext("VVTX"));

	if (riff.findFirst("VVX3"))
		do
			if (riff.multipleOf(sizeof(typename Def::FloatType)*3))
			{
				typedef Mesh<VsDef>	VsMesh;
				VsMesh	&hull = vs_hull_field[c];
				hull.clear();
				{
					Array<typename Def::FloatType>	array;
					riff.get(array);
					hull.vertex_field.setSize(array.length()/3);
					for (index_t i = 0; i < hull.vertex_field.length(); i++)
					{
						hull.vertex_field[i].position = Vec::ref3(array + i*3);
						hull.vertex_field[i].index = i;
						hull.vertex_field[i].marked = false;
					}
				}
				if (riff.findNext("VFC3") && riff.multipleOf(sizeof(UINT32)*3))
				{
					Array<UINT32>	array;
					riff.get(array);
					hull.triangle_field.setSize(array.length()/3);
					for (index_t i = 0; i < hull.triangle_field.length(); i++)
					{
						VsMesh::Triangle	&face = hull.triangle_field[i];
						face.vertex[0] = hull.vertex_field+array[i*3];
						face.vertex[1] = hull.vertex_field+array[i*3+1];
						face.vertex[2] = hull.vertex_field+array[i*3+2];
						face.index = i;
						face.marked = false;
					}
				}
				if (riff.findNext("VFC4") && riff.multipleOf(sizeof(UINT32)*4))
				{
					Array<UINT32>	array;
					riff.get(array);
					hull.quad_field.setSize(array.length()/4);
					for (index_t i = 0; i < hull.quad_field.length(); i++)
					{
						VsMesh::Quad	&face = hull.quad_field[i];
						face.vertex[0] = hull.vertex_field+array[i*4];
						face.vertex[1] = hull.vertex_field+array[i*4+1];
						face.vertex[2] = hull.vertex_field+array[i*4+2];
						face.vertex[3] = hull.vertex_field+array[i*4+3];
						face.index = i;
						face.marked = false;
					}
				}
				if (riff.findNext("VEDG") && riff.multipleOf(sizeof(UINT32)*4))
				{
					Array<INT32>	array;
					riff.get(array);
					hull.edge_field.setSize(array.length()/4);
					for (index_t i = 0; i < hull.edge_field.length(); i++)
					{
						VsMesh::Edge	&edge = hull.edge_field[i];
						edge.vertex[0] = hull.vertex_field+array[i*4];
						edge.vertex[1] = hull.vertex_field+array[i*4+1];
						hull.indexToLink(array[i*4+2],edge.n[0]);
						hull.indexToLink(array[i*4+3],edge.n[1]);
						edge.index = i;
						edge.marked = false;
					}
				}
				hull.correct();
				c++;
			}
		while (riff.findNext("VVX3"));
		

	ph_hull.clear();
	if (riff.findFirst("HVTX") && riff.multipleOf(sizeof(typename Def::PhHullFloatType)*4))
	{
		{
			Array<typename Def::PhHullFloatType>	array;
			riff.get(array);
			ph_hull.vertex_field.setSize(array.length()/4);
			for (index_t i = 0; i < ph_hull.vertex_field.length(); i++)
			{
				Mesh<PhDef>::Vertex	&vertex = ph_hull.vertex_field[i];
				/*vertex.p0 = vertex.p1 = */
				vertex.position = Vec::ref3(array + i*4);
				Vec::clear(vertex.normal);
				//vertex.grip = 0;
				vertex.index = i;
				vertex.marked = false;
			}
		}
		
		static const size_t ph_face_size = sizeof(UINT32)*5+sizeof(typename Def::PhFloatType)*2+sizeof(uint64_t);
		if (riff.findNext("HFCE") && riff.multipleOf(ph_face_size))
		{
			
			unsigned cnt = riff.getSize()/ph_face_size;
			ph_hull.triangle_field.setSize(cnt);
			
			riff.openStream();
			for (unsigned i = 0; i < cnt; i++)
			{
				Mesh<PhDef>::Triangle	&face = ph_hull.triangle_field[i];
				UINT32	index[4];
				riff.stream(index,4);
				face.vertex[0] = ph_hull.vertex_field+index[0];
				face.vertex[1] = ph_hull.vertex_field+index[1];
				face.vertex[2] = ph_hull.vertex_field+index[2];
				
				riff.skip(sizeof(typename Def::PhFloatType)*2 + sizeof(name64_t) + sizeof(UINT32));
				/*riff.stream(dummyf);
				riff.stream(dummyf);
				riff.stream(dummy_name);
				riff.stream(dummy32);*/
				riff.skip(4);
			}
			riff.closeStream();
		}
		
		if (riff.findNext("HEDG") && riff.multipleOf(sizeof(UINT32)*4))
		{
			Array<UINT32>	array;
			riff.get(array);
			ph_hull.edge_field.setSize(array.length()/4);
			for (index_t i = 0; i < ph_hull.edge_field.length(); i++)
			{
				Mesh<PhDef>::Edge	&edge = ph_hull.edge_field[i];
				edge.vertex[0] = ph_hull.vertex_field+array[i*4];
				edge.vertex[1] = ph_hull.vertex_field+array[i*4+1];
				ph_hull.indexToLink(array[i*4+2],edge.n[0]);
				ph_hull.indexToLink(array[i*4+3],edge.n[1]);
				edge.index = i;
				edge.marked = false;
			}
		}
	}
	else
		if (riff.findFirst("HVX3") && riff.multipleOf(sizeof(typename Def::PhHullFloatType)*3))
		{
			{
				Array<typename Def::PhHullFloatType>	array;
				riff.get(array);
				ph_hull.vertex_field.setSize(array.length()/3);
				for (index_t i = 0; i < ph_hull.vertex_field.length(); i++)
				{
					Mesh<PhDef>::Vertex	&vertex = ph_hull.vertex_field[i];
					//vertex.p0 = vertex.p1 = 
					vertex.position = Vec::ref3(array + i*3);
					Vec::clear(vertex.normal);
					//ASSERT_IS_CONSTRAINED3__(array+i*3,-30,30);
					//vertex.grip = 0;
					vertex.index = i;
					vertex.marked = false;
				}
			}
			
			static const size_t ph_face_size3 = sizeof(UINT32)*3,
								ph_face_size4 = sizeof(UINT32)*4;
			if (riff.findNext("HTRI"))
			{
				
				if ( riff.multipleOf(ph_face_size3))
				{
					count_t cnt = riff.getSize()/ph_face_size3;
					ph_hull.triangle_field.setSize(cnt);
				
					riff.openStream();
					for (index_t i = 0; i < cnt; i++)
					{
						Mesh<PhDef>::Triangle	&face = ph_hull.triangle_field[i];
						UINT32	index[3];
						riff.stream(index,3);
						face.vertex[0] = ph_hull.vertex_field+index[0];
						face.vertex[1] = ph_hull.vertex_field+index[1];
						face.vertex[2] = ph_hull.vertex_field+index[2];
					}
					riff.closeStream();
				}
			}
			if (riff.findNext("HQAD"))
			{
				if (riff.multipleOf(ph_face_size4))
				{
					count_t cnt = riff.getSize()/ph_face_size4;
					ph_hull.quad_field.setSize(cnt);
				
					riff.openStream();
					for (index_t i = 0; i < cnt; i++)
					{
						Mesh<PhDef>::Quad	&face = ph_hull.quad_field[i];
						UINT32	index[4];
						riff.stream(index,4);
						face.vertex[0] = ph_hull.vertex_field+index[0];
						face.vertex[1] = ph_hull.vertex_field+index[1];
						face.vertex[2] = ph_hull.vertex_field+index[2];
						face.vertex[3] = ph_hull.vertex_field+index[3];
					}
					riff.closeStream();
				}
			}
						
			
			if (riff.findNext("HEDG") && riff.multipleOf(sizeof(UINT32)*4))
			{
				Array<UINT32>	array;
				riff.get(array);
				ph_hull.edge_field.setSize(array.length()/4);
				for (index_t i = 0; i < ph_hull.edge_field.length(); i++)
				{
					Mesh<PhDef>::Edge	&edge = ph_hull.edge_field[i];
					edge.vertex[0] = ph_hull.vertex_field+array[i*4];
					edge.vertex[1] = ph_hull.vertex_field+array[i*4+1];
					ph_hull.indexToLink(array[i*4+2],edge.n[0]);
					ph_hull.indexToLink(array[i*4+3],edge.n[1]);

					edge.index = i;
					edge.marked = false;
				}
			}
		}
	ph_hull.verifyIntegrity();
	ph_hull.correct();

	/*
	for (index_t i = 0; i < ph_hull.vertex_field.length(); i++)
		ph_hull.vertex_field[i].index = 0;*/
	
	for (index_t i = 0; i < ph_hull.triangle_field.length(); i++)
	{
		Mesh<PhDef>::Triangle	&face = ph_hull.triangle_field[i];
		Obj::triangleNormal(face.vertex[0]->position,face.vertex[1]->position,face.vertex[2]->position,face.normal);
		Vec::add(face.vertex[0]->normal,face.normal);
		Vec::add(face.vertex[1]->normal,face.normal);
		Vec::add(face.vertex[2]->normal,face.normal);
		/*face.vertex[0]->grip += face.grip;
		face.vertex[1]->grip += face.grip;
		face.vertex[2]->grip += face.grip;*/
//		_normalize(face.normal);
		/*face.vertex[0]->index++;
		face.vertex[1]->index++;
		face.vertex[2]->index++;*/
		//Vec::center(face.vertex[0]->position,face.vertex[1]->position,face.vertex[2]->position,face.center);
		//Vec::copy(face.center,face.gcenter);
	}
	
	for (index_t i = 0; i < ph_hull.quad_field.length(); i++)
	{
		Mesh<PhDef>::Quad	&face = ph_hull.quad_field[i];
		_oTriangleNormal(face.vertex[0]->position,face.vertex[1]->position,face.vertex[2]->position,face.normal);
		_oAddTriangleNormal(face.vertex[0]->position,face.vertex[2]->position,face.vertex[3]->position,face.normal);
		Vec::add(face.vertex[0]->normal,face.normal);
		Vec::add(face.vertex[1]->normal,face.normal);
		Vec::add(face.vertex[2]->normal,face.normal);
		Vec::add(face.vertex[3]->normal,face.normal);
		/*face.vertex[0]->grip += face.grip;
		face.vertex[1]->grip += face.grip;
		face.vertex[2]->grip += face.grip;
		face.vertex[3]->grip += face.grip;*/
//		_normalize(face.normal);
		/*face.vertex[0]->index++;
		face.vertex[1]->index++;
		face.vertex[2]->index++;
		face.vertex[3]->index++;*/
		/*Vec::copy(face.vertex[0]->position,face.center);
		Vec::add(face.center,face.vertex[1]->position);
		Vec::add(face.center,face.vertex[2]->position);
		Vec::add(face.center,face.vertex[3]->position);
		Vec::div(face.center,4.0);
		Vec::copy(face.center,face.gcenter);*/
	}
	
	
	for (index_t i = 0; i < ph_hull.vertex_field.length(); i++)
	{
		if (ph_hull.vertex_field[i].index)
		{
			Vec::normalize0(ph_hull.vertex_field[i].normal);
			//ph_hull.vertex_field[i].grip /= ph_hull.vertex_field[i].index;
		}
		//ph_hull.vertex_field[i].index = i;
	}
	
	for (index_t i = 0; i < ph_hull.edge_field.length(); i++)
	{
		Mesh<PhDef>::Edge	&edge = ph_hull.edge_field[i];
		Vec::clear(edge.normal);
		
		if (edge.n[0].is_quad)
		{
			Vec::copy(edge.n[0].quad->normal,edge.normal);
			//edge.grip = edge.n[0].quad->grip;
		}
		else
		{
			Vec::copy(edge.n[0].triangle->normal,edge.normal);
			//edge.grip = edge.n[0].triangle->grip;
		}

		if (edge.n[1].triangle)
			if (edge.n[1].is_quad)
			{
				Vec::add(edge.normal,edge.n[1].quad->normal);
				//edge.grip = (edge.grip+edge.n[1].quad->grip)/2;
			}
			else
			{
				Vec::add(edge.normal,edge.n[1].triangle->normal);
				//edge.grip = (edge.grip+edge.n[1].triangle->grip)/2;
			}
	}
	
	for (index_t i = 0; i < ph_hull.triangle_field.length(); i++)
		Vec::normalize(ph_hull.triangle_field[i].normal);
	for (index_t i = 0; i < ph_hull.quad_field.length(); i++)
		Vec::normalize(ph_hull.quad_field[i].normal);
	
	
	static const size_t AcceleratorSize = sizeof(tName)+8*sizeof(typename Def::PhFloatType),
						ExtAcceleratorSize = AcceleratorSize+5*sizeof(typename Def::FloatType),
						MinAcceleratorSize = 8*sizeof(typename Def::PhFloatType) + 5*sizeof(typename Def::FloatType),
						MountingSize = sizeof(UINT32)+sizeof(typename Def::FloatType)*13;

	unsigned mountings = 0;
	if (riff.findFirst("MNT "))
		do
		{
			if (riff.getSize() == MountingSize)
				mountings++;
		}
		while (riff.findNext("MNT "));
	
	mounting_field.setSize(mountings);
	mountings = 0;
	
	if (mounting_field.length() && riff.findFirst("MNT "))
		do
		{
			if (riff.getSize() == MountingSize)
			{
				riff.openStream();
					riff.stream(mounting_field[mountings].flags);
					riff.stream(mounting_field[mountings].position.v,3);
					riff.stream(mounting_field[mountings].direction.v,3);
					riff.stream(mounting_field[mountings].vertical.v,3);
					riff.stream(mounting_field[mountings].dimension.v,3);
					riff.stream(mounting_field[mountings].max_payload);
				riff.closeStream();
				mountings++;
			}
		}
		while (riff.findNext("MNT "));
	
	count_t accelerators = 0;
	index_t acc_version = 0;
	const char*acc_key = "ACCL";
	if (riff.findFirst("ACCL"))
	{
		do
		{
			if (riff.getSize() == AcceleratorSize || riff.getSize() == ExtAcceleratorSize)
				accelerators++;
		}
		while (riff.findNext("ACCL"));
	}
	else
		if (riff.findFirst("ACC2"))
		{
			acc_version = 1;
			acc_key = "ACC2";
			do
			{
				if (riff.getSize() > MinAcceleratorSize)
					accelerators++;
			}
			while (riff.findNext("ACC2"));
		}
	accelerator_field.setSize(accelerators);
	accelerators = 0;
	if (accelerator_field.isNotEmpty() && riff.findFirst(acc_key))
		do
		{

			switch (acc_version)
			{
				case 0:
					if (riff.getSize() == AcceleratorSize)
					{

						riff.openStream();
							name64_t	name;
							riff.stream(name);
							accelerator_field[accelerators].name = name2str(name);

							riff.stream(accelerator_field[accelerators].position.v,3);
							riff.stream(accelerator_field[accelerators].direction.v,3);
							riff.stream(accelerator_field[accelerators].power);
							riff.stream(accelerator_field[accelerators].zero_efficiency);
						riff.closeStream();

						accelerator_field[accelerators].flame_length = 0;
						accelerator_field[accelerators].flame_width = 0;
						Vec::def(accelerator_field[accelerators].flame_color,1,0.5,0);

					}
					elif (riff.getSize() == ExtAcceleratorSize)
					{
						riff.openStream();
							name64_t name;
							riff.stream(name);
							accelerator_field[accelerators].name = name2str(name);

							riff.stream(accelerator_field[accelerators].position.v,3);
							riff.stream(accelerator_field[accelerators].direction.v,3);
							riff.stream(accelerator_field[accelerators].power);
							riff.stream(accelerator_field[accelerators].zero_efficiency);
							riff.stream(accelerator_field[accelerators].flame_length);
							riff.stream(accelerator_field[accelerators].flame_width);
							riff.stream(accelerator_field[accelerators].flame_color.v,3);
						riff.closeStream();
					}
					else
						continue;
				break;
				case 1:
					if (riff.getSize() > MinAcceleratorSize)
					{
						size_t name_length = riff.getSize() - MinAcceleratorSize;
						accelerator_field[accelerators].name.setLength(name_length);
						riff.openStream();
							riff.stream(accelerator_field[accelerators].position.v,3);
							riff.stream(accelerator_field[accelerators].direction.v,3);
							riff.stream(accelerator_field[accelerators].power);
							riff.stream(accelerator_field[accelerators].zero_efficiency);
							riff.stream(accelerator_field[accelerators].flame_length);
							riff.stream(accelerator_field[accelerators].flame_width);
							riff.stream(accelerator_field[accelerators].flame_color.v,3);

							riff.stream(accelerator_field[accelerators].name.mutablePointer(),name_length);
						riff.closeStream();
					}
					else
						continue;
				break;
				default:
					continue;
			}

			accelerators++;
		}
		while (riff.findNext(acc_key));

	count_t wheels = 0;
	const char*whl_key = "WHEL";
	index_t	whl_version = 0;
		bool whl_begin = false;
	if (riff.findFirst("WHEL"))
	{
		whl_begin= true;
	}	//default
	else if (riff.findFirst("WHL2"))
	{
		whl_key = "WHL2";
		whl_version = 1;
		whl_begin = true;
	}
	if (whl_begin)
		do
		{
			wheels++;
		}
		while (riff.findNext(whl_key));
	

	wheel_field.setSize(wheels);
	wheels = 0;
	if (wheel_field.isNotEmpty() && riff.findFirst(whl_key))
		do
		{
			switch (whl_version)
			{
				case 0:
				{
					riff.openStream();
						name64_t name;
						riff.stream(name);
						wheel_field[wheels].name = name2str(name);
						riff.stream(name);	//ignore
						riff.stream(name);	//ignore
						riff.stream(wheel_field[wheels].position.v,3);
						riff.stream(wheel_field[wheels].contraction.v,3);
						riff.stream(wheel_field[wheels].axis.v,3);
						riff.stream(wheel_field[wheels].radius);
						riff.stream(wheel_field[wheels].width);
						riff.stream(wheel_field[wheels].parameter,ARRAYSIZE(wheel_field[wheels].parameter));
					riff.closeStream();
				}
				break;
				case 1:
				{
					riff.openStream();
						riff.stream(wheel_field[wheels].position.v,3);
						riff.stream(wheel_field[wheels].contraction.v,3);
						riff.stream(wheel_field[wheels].axis.v,3);
						riff.stream(wheel_field[wheels].radius);
						riff.stream(wheel_field[wheels].width);
						riff.stream(wheel_field[wheels].parameter,ARRAYSIZE(wheel_field[wheels].parameter));

						size_t name_length = riff.getSize() - riff.streamTell();
						wheel_field[wheels].name.setLength(name_length);
						riff.stream(wheel_field[wheels].name.mutablePointer(),name_length);
					riff.closeStream();
				}
				break;
				default:
					continue;
			}
			wheel_field[wheels].suspension.loadFromRiff(riff,1);	//must start at 1, because otherwise the animator uses findFirst(...) and that will cause issues
			wheel_field[wheels].rotation.loadFromRiff(riff,2);
			wheels++;
		}
		while (riff.findNext(whl_key));

	/*for (index_t i = 0; i < wheel_field.length(); i++)
	{
		wheel_field[i].next = findWheel(wheel_field[i].nname);
		wheel_field[i].prev = findWheel(wheel_field[i].pname);
	}*/
	
	count_t tracks = 0;

	const char* trk_key = "CNST";
	index_t		trk_version = 0;
	bool		trk_begin = false;

	if (riff.findFirst("CNST"))
	{
		trk_begin = true;
	}
	else
		if (riff.findFirst("TRK2"))
		{
			trk_key = "TRK2";
			trk_version = 1;
			trk_begin = true;
		}
	if (trk_begin)
		do
		{
			tracks++;
		}
		while (riff.findNext(trk_key));
	tracks_field.setSize(tracks);
	tracks = 0;
	if (tracks_field.isNotEmpty() && riff.findFirst(trk_key))
		do
		{
			switch (trk_version)
			{
				case 0:
					if (riff.getSize() && riff.getSize() && !(riff.getSize()%8))
					{
						tracks_field[tracks].member_field.setSize(riff.getSize()/8);
						name64_t	name;
						riff.openStream();
							for (index_t k = 0; k < tracks_field[tracks].member_field.length(); k++)
								if (riff.stream(name))
								{
									tracks_field[tracks].member_field[k].wheel = NULL;
									tracks_field[tracks].member_field[k].name = name2str(name);
								}
						riff.closeStream();
					}
					else
						continue;
				break;
				case 1:
					UINT32	member_count;
					riff.openStream();
						if (!riff.stream(member_count))
						{
							riff.closeStream();
							continue;
						}
						tracks_field[tracks].member_field.setSize(member_count);
						for (UINT32 k = 0; k < member_count; k++)
						{
							UINT32	name_length;
							if (!riff.stream(name_length) || !riff.canStreamBytes(name_length))
							{
								riff.closeStream();
								tracks_field[tracks].member_field.free();
								continue;
							}
							tracks_field[tracks].member_field[k].name.setLength(name_length);
							riff.stream(tracks_field[tracks].member_field[k].name.mutablePointer(),name_length);
						}
					riff.closeStream();

				break;
			}
			//tracks_field[tracks].rotation.loadFromRiff(riff);
			tracks++;
		}
		while (riff.findNext(trk_key));

	count_t children = 0;
	if (riff.findFirst("OBJ "))
		do
		{
			if (riff.findNext("LIST"))
				children++;
		}
		while (riff.findNext("OBJ "));
	child_field.setSize(children);
	children = 0;
	if (child_field.length() && riff.findFirst("OBJ "))
		do
		{
			child_field[children].name.setLength(riff.getSize());
			riff.get(child_field[children].name.mutablePointer());
			child_field[children].name.trimThis();

			//if (riff.getSize() == sizeof(name))
			{
				//riff.get(&child_field[children].name);
				if (riff.findNext("LIST"))
				{
					riff.enter();
						child_field[children].loadFromRiff(riff);
					riff.dropBack();
					children++;
				}
			}
		}
		while (riff.findNext("OBJ "));
		
}

template <class Def> bool SubGeometryA<Def>::isMember(SubGeometryA<Def>*obj) const
{
	if ((unsigned)(obj-child_field) < child_field.length())
		return true;
	for (index_t i = 0; i < child_field.length(); i++)
		if (child_field[i].isMember(obj))
			return true;
	return false;
}

template <class Def> bool SubGeometryA<Def>::isMember(TWheel<Def>*whl) const
{
	if ((unsigned)(whl-wheel_field) < wheel_field.length())
		return true;
	for (index_t i = 0; i < child_field.length(); i++)
		if (child_field[i].isMember(whl))
			return true;
	return false;
}

template <class Def> bool SubGeometryA<Def>::isMember(TAccelerator<Def>*acc) const
{
	if ((unsigned)(acc-accelerator_field) < accelerator_field.length())
		return true;
	for (index_t i = 0; i < child_field.length(); i++)
		if (child_field[i].isMember(acc))
			return true;
	return false;
}



template <class Def> void SubGeometryA<Def>::linkAnimators(Geometry<Def>*domain)
{
	for (index_t i = 0; i < accelerator_field.length(); i++)
	{
		accelerator_field[i].domain = this;
	}
	for (index_t i = 0; i < wheel_field.length(); i++)
	{
		wheel_field[i].domain = this;
		wheel_field[i].rotation.link(domain);
		wheel_field[i].suspension.link(domain);
	}
	for (index_t i = 0; i < tracks_field.length(); i++)
	{
		//tracks_field[i].rotation.link(domain);
		tracks_field[i].domain = this;
	}
	for (index_t i = 0; i < child_field.length(); i++)
	{
		child_field[i].linkAnimators(domain);
		//child_field[i].domain = this;	//maybe later
	}
}

template <class Def> void SubGeometryA<Def>::postCopyLink(Geometry<Def>*domain)
{
	system_link = &path;
	for (index_t i = 0; i < wheel_field.length(); i++)
	{
		wheel_field[i].rotation.link(domain);
		wheel_field[i].suspension.link(domain);
		/*wheel_field[i].next = findWheel(wheel_field[i].nname);
		if (!wheel_field[i].next)
			wheel_field[i].next = wheel_field+i;
		wheel_field[i].prev = findWheel(wheel_field[i].pname);
		if (!wheel_field[i].prev)
			wheel_field[i].prev = wheel_field+i;*/
	}
	for (index_t i = 0; i < tracks_field.length(); i++)
	{
		for (index_t j = 0; j < tracks_field[i].member_field.length(); j++)
			tracks_field[i].member_field[j].wheel = domain->lookupWheel(tracks_field[i].member_field[j].name);	//localize
		//tracks_field[i].rotation.link(domain);
	}
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].postCopyLink(domain);
}


template <class Def> void SubGeometryA<Def>::storeTargetNames()
{
	for (index_t i = 0; i < wheel_field.length(); i++)
	{
		wheel_field[i].rotation.storeTargetNames();
		wheel_field[i].suspension.storeTargetNames();
		//wheel_field[i].nname = wheel_field[i].next->name;
		//wheel_field[i].pname = wheel_field[i].prev->name;
	}
	for (index_t i = 0; i < tracks_field.length(); i++)
	{
		for (index_t j = 0; j < tracks_field[i].member_field.length(); j++)
			tracks_field[i].member_field[j].name = tracks_field[i].member_field[j].wheel->name;
		//tracks_field[i].rotation.storeTargetNames();
	}
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].storeTargetNames();
}


template <class Def> void SubGeometryA<Def>::checkLinkage()
{
	/*for (index_t i = 0; i < wheel_field.length(); i++)
	{
		if ((wheel_field[i].prev && !wheel_field.owns(wheel_field[i].prev))
			||
			(wheel_field[i].next && !wheel_field.owns(wheel_field[i].next)))
			FATAL__("WheelLinkage of "+name2str(name)+" broken");
	}
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].checkLinkage();
	*/
}


template <class Def>
template <class C> bool SubGeometryA<Def>::extractDimensions(const TMatrix4<typename Def::SystemType>&system, DimensionParser<C>&parser) const
{
	bool def;
	TMatrix4<typename Def::SystemType> lsystem;
	Mat::transformSystem(system,meta.system,lsystem);
	if (vs_hull_field.length() && vs_hull_field[0].vertex_field.length())
	{
		def = true;
		TVec3<C>	p;
		TBox<C>		local;

		Mat::transform(lsystem,vs_hull_field[0].vertex_field[0].position,p);
		local.lower = local.upper = p;

		for (index_t i = 1; i < vs_hull_field[0].vertex_field.length(); i++)
		{
			Mat::transform(lsystem,vs_hull_field[0].vertex_field[i].position,p);
			_oDetDimension(p,local);
		}
		parser.parse(local.lower);
		parser.parse(local.upper);
	}
	else
		def = false;
	for (index_t i = 0; i < child_field.length(); i++)
		def = child_field[i].extractDimensions(lsystem,parser)||def;
	return def;
}

template <class Def>
template <class C> bool SubGeometryA<Def>::extractDimensions(TBox<C>&dim) const
{
	bool def;
	if (vs_hull_field.length()&&vs_hull_field[0].vertex_field.length())
	{
		def = true;
		dim.lower = dim.upper = vs_hull_field[0].vertex_field[0].position;

		for (index_t i = 1; i < vs_hull_field[0].vertex_field.length(); i++)
			_oDetDimension(vs_hull_field[0].vertex_field[i].position,dim);
	}
	else
	{
		dim.lower = dim.upper = Vector<C>::zero;
		def = false;
	}
	return def;
}

template <class Def>
template <class C> void SubGeometryA<Def>::extractRadius(C&radius) const
{
	C vr = radius*radius;
	if (vs_hull_field.length())
		for (index_t i = 0; i < vs_hull_field[0].vertex_field.length(); i++)
		{
			C r = Vec::dot(vs_hull_field[0].vertex_field[i].position);
			if (r > vr)
				vr = r;
		}
	radius = vsqrt(vr);
}

template <class Def>
template <class C> void SubGeometryA<Def>::extractAverageVisualEdgeLength(index_t layer, C&length) const
{
	length = 0;
	if (layer < vs_hull_field.length())
	{
		for (index_t i = 0; i < vs_hull_field[layer].edge_field.length(); i++)
		{
			length += (C)Vec::quadraticDistance(vs_hull_field[layer].edge_field[i].vertex[0]->position,vs_hull_field[layer].edge_field[i].vertex[1]->position);
		}
		length = vsqrt(length)/(C)vs_hull_field[layer].edge_field.length();
	}
}



template <class Def>
	template <typename T>
		void SubGeometryA<Def>::extractShortestVisualEdgeLength(index_t layer, T&length)	const
		{
			length = numeric_limits<T>::max();
			if (layer < vs_hull_field.length())
			{
				const Mesh<VsDef>&mesh = vs_hull_field[layer];
				if (mesh.edge_field.isNotEmpty())
				{
					for (index_t i = 0; i < mesh.edge_field.length(); i++)
					{
						const typename Mesh<VsDef>::Edge&edge = mesh.edge_field[i];
						length = vmin(length,Vec::quadraticDistance(edge.v0->position,edge.v1->position));
					}
				}
				else
				{
					for (index_t i = 0; i < mesh.triangle_field.length(); i++)
					{
						const typename Mesh<VsDef>::Triangle&t = mesh.triangle_field[i];
						length = vmin(
									vmin(length,
										Vec::quadraticDistance(t.v0->position,t.v1->position)),
									vmin(Vec::quadraticDistance(t.v1->position,t.v2->position),
										Vec::quadraticDistance(t.v2->position,t.v0->position))
								);
					}

					for (index_t i = 0; i < mesh.quad_field.length(); i++)
					{
						const typename Mesh<VsDef>::Quad&q = mesh.quad_field[i];
						length = vmin(length,
									vmin(
										vmin(Vec::quadraticDistance(q.v0->position,q.v1->position),
											Vec::quadraticDistance(q.v1->position,q.v2->position)),
										vmin(Vec::quadraticDistance(q.v2->position,q.v3->position),
											Vec::quadraticDistance(q.v3->position,q.v0->position))
										)
								);
					}
				}

			}
	
			length = vsqrt(length);
		}


template <class Def>
template <class C> void SubGeometryA<Def>::extractAbsoluteRadius(C&radius) const
{
	if (vs_hull_field.length() && vs_hull_field[0].vertex_field.length())
	{
		C vr = radius*radius;
		for (index_t i = 0; i < vs_hull_field[0].vertex_field.length(); i++)
		{
			C r;
			TVec3<C>	p;
			Mat::transform(path,vs_hull_field[0].vertex_field[i].position,p);
			r = Vec::dot(p);
			if (r > vr)
				vr = r;
		}
		radius = vsqrt(vr);
	}
}

template <class Def>
template <class C> void SubGeometryA<Def>::extractAbsoluteRadius(const TVec3<C>&center, C&radius) const
{
	C vr = radius*radius;
	if (vs_hull_field.length())
		for (index_t i = 0; i < vs_hull_field[0].vertex_field.length(); i++)
		{
			C r;
			TVec3<C>	p;
			Mat::transform(path,vs_hull_field[0].vertex_field[i].position,p);
			Vec::sub(p,center);
			r = Vec::dot(p);
			if (r > vr)
				vr = r;
		}
	radius = vsqrt(vr);
}

template <class Def>
template <class C> void SubGeometryA<Def>::addWeight(C&weight)		const
{
	weight += meta.volume*meta.density;
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].addWeight(weight);
}



template <class Def> void SubGeometryA<Def>::defLinkageStr(String&rs, const String&intend)
{
	rs+=intend+"(0x"+IntToHex((int)system_link,8)+")\n";
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].defLinkageStr(rs,intend+"	");
}


template <class Def>
template <class C> void SubGeometryA<Def>::extractRadiusRec(C&radius) const
{
	extractAbsoluteRadius(radius);
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].extractRadiusRec(radius);
}

template <class Def>
template <class C> void SubGeometryA<Def>::extractRadiusRec(const TVec3<C>&center, C&radius) const
{
	
	extractAbsoluteRadius(center,radius);
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].extractRadiusRec(center,radius);
}


template <class Def> void SubGeometryA<Def>::setSystemRec(const TMatrix4<typename Def::SystemType>&system)
{
	Mat::transformSystem(system,meta.system,path);
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].setSystemRec(path);
}

template <class Def> void SubGeometryA<Def>::resetLinkageRec()
{
	system_link = &path;
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].resetLinkageRec();
}


template <class Def> void SubGeometryA<Def>::toInstance(StaticSubInstanceA<Def>&tree)
{
	tree.target = this;
	tree.child_field.setSize(child_field.length());
	tree.system = meta.system;
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].toInstance(tree.child_field[i]);
}

template <class Def> void SubGeometryA<Def>::toInstance(AnimatableSubInstanceA<Def>&tree)
{
	tree.target = this;
	
	Vec::clear(tree.animation_status.translation);
	Vec::clear(tree.animation_status.rotation);

	tree.wheel_rotation_animators.setSize(wheel_field.length());
	tree.wheel_suspension_animators.setSize(wheel_field.length());
	//tree.construct_rotation_animators.setSize(tracks_field.length());
	for (index_t i = 0; i < wheel_field.length(); i++)
	{
		tree.wheel_rotation_animators[i].build(&wheel_field[i].rotation);
		tree.wheel_suspension_animators[i].build(&wheel_field[i].suspension);
	}
	/*for (index_t i = 0; i < tracks_field.length(); i++)
		tree.construct_rotation_animators[i].build(&tracks_field[i].rotation);*/
	tree.wheel_status.setSize(wheel_field.length()*2);
	tree.accelerator_status.setSize(accelerator_field.length());
	tree.wheel_status.fill(0);
	tree.accelerator_status.fill(0);
	
	tree.child_field.setSize(child_field.length());
	tree.system = meta.system;
	
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].toInstance(tree.child_field[i]);
}

template <class Def> count_t SubGeometryA<Def>::countVertices(unsigned detail)
{
	count_t rs = detail<vs_hull_field.length()?vs_hull_field[detail].vertex_field.length():0;
	for (index_t i = 0; i < child_field.length(); i++)
		rs+=child_field[i].countVertices(detail);
	return rs;
}


template <class Def> bool SubGeometryA<Def>::checkIntegrity(Geometry<Def>*super, const String&path)
{
	error = "obj_err:";
	bool rs = true;
	String local = path+"/"+name;
	if (!ph_hull.valid())
	{
		error += "\nhull-error in "+local+": "+ph_hull.errorStr();
		rs = false;
	}
	for (index_t i = 0; i < vs_hull_field.length(); i++)
		if (!vs_hull_field[i].valid())
		{
			error += "\nvs_hull("+String(i)+")-error in "+local+": "+vs_hull_field[i].errorStr();
			rs = false;
		}
	for (index_t i = 0; i < accelerator_field.length(); i++)
	{
		if (accelerator_field[i].domain != this)
		{
			error += "\ndomain link of accelerator "+String(i)+" of "+local+" is broken";
			rs = false;
		}
	}
	
	for (index_t i = 0; i < wheel_field.length(); i++)
	{
		if (wheel_field[i].domain != this)
		{
			error += "\ndomain link of wheel "+String(i)+" of "+local+" is broken";
			rs = false;
		}
		if (!wheel_field[i].rotation.valid(super))
		{
			error += "\nrotation-animation of wheel "+String(i)+" ("+wheel_field[i].name+") of "+local+" is broken";
			rs = false;
		}
		if (!wheel_field[i].suspension.valid(super))
		{
			error += "\nsuspension-animation of wheel "+String(i)+" ("+wheel_field[i].name+") of "+local+" is broken";
			rs = false;
		}
	}
	for (index_t i = 0; i < tracks_field.length(); i++)
	{
		if (tracks_field[i].domain != this)
		{
			error += "\ndomain link of construct "+String(i)+" of "+local+" is broken";
			rs = false;
		}
		/*
		if (!tracks_field[i].rotation.valid(super))
		{
			error += "\nconstruct-animation of "+String(i)+" of "+local+" is broken";
			rs = false;
		}
		*/
		for (index_t j = 0; j < tracks_field[i].member_field.length(); j++)
			if (!isMember(tracks_field[i].member_field[j].wheel))
			{
				error += "\nentry "+String(j)+" ('"+tracks_field[i].member_field[j].name+"') of construct "+String(i)+" is no member of "+local;
				rs = false;
			}
	}
	for (index_t i = 0; i < child_field.length(); i++)
	{
		if (!child_field[i].checkIntegrity(super,local))
		{
			error += "\n"+child_field[i].error;
			rs = false;
		}
	}
	return rs;
}

template <class Def> void SubGeometryA<Def>::verifyIntegrity(Geometry<Def>*super, const String&path)
{
	if (!checkIntegrity(super,path))
		FATAL__(error);
}




template <class Def>
template <class Def0>
SubGeometryA<Def>::SubGeometryA(const SubGeometryA<Def0>&other)
{
	operator=(other);
}
	

template <class Def>
SubGeometryA<Def>::SubGeometryA(const SubGeometryA<Def>&other)
{
	operator=(other);
}

template <class Def>
	void	SubGeometryA<Def>::adoptData(SubGeometryA<Def>&other)
	{
		if (this == &other)
			return;
		child_field.adoptData(other.child_field);
		ph_hull.adoptData(other.ph_hull);
		vs_hull_field.adoptData(other.vs_hull_field);
		
		tracks_field.adoptData(other.tracks_field);
		accelerator_field.adoptData(other.accelerator_field);
		wheel_field.adoptData(other.wheel_field);



		
		for (index_t i = 0; i < tracks_field.length(); i++)
			tracks_field[i].domain = this;
		for (index_t i = 0; i < accelerator_field.length(); i++)
			accelerator_field[i].domain = this;
		for (index_t i = 0; i < wheel_field.length(); i++)
			wheel_field[i].domain = this;
			
		
		
		name.adoptData(other.name);
		meta = other.meta;
		path = other.path;
		system_link = &path;
	}

template <class Def>
SubGeometryA<Def>& SubGeometryA<Def>::operator=(const SubGeometryA<Def>&other)
{
	child_field = other.child_field;	//self adapts
	ph_hull = other.ph_hull;			//self adapts
	vs_hull_field = other.vs_hull_field;	//self adapts
	
	tracks_field = other.tracks_field;	//needs post linking
	accelerator_field = other.accelerator_field;	//needs post linking
	wheel_field = other.wheel_field;			//needs post linking
	name = other.name;
	meta = other.meta;
	path = other.path;
	system_link = &path;
	
	return *this;
}
	
template <class Def>
template <class T>
SubGeometryA<Def>& SubGeometryA<Def>::operator=(const SubGeometryA<T>&other)
{
	child_field = other.child_field;	//self adapts
	ph_hull = other.ph_hull;			//self adapts
	vs_hull_field = other.vs_hull_field;	//self adapts
	
	tracks_field = other.tracks_field;	//needs post linking
	accelerator_field = other.accelerator_field;	//needs post linking
	wheel_field = other.wheel_field;			//needs post linking
	name = other.name;
	meta = other.meta;
	path = other.path;
	system_link = &path;
	
	return *this;
}


template <class C> DimensionParser<C>::DimensionParser():set(false)
{}

template <class C>
template <class C0> void DimensionParser<C>::parse(const C0 point[3])
{
	if (set)
		_oDetDimension(Vec::ref3(point),dimension);
	else
	{
		dimension.lower = dimension.upper = Vec::ref3(point);
		set = true;
	}
}

template <class C>
template <class C0> void DimensionParser<C>::parse(const TVec3<C0>&point)
{
	if (set)
		_oDetDimension(point,dimension);
	else
	{
		dimension.lower = dimension.upper = point;
		set = true;
	}
}



template <class Def>
	void			Geometry<Def>::onSystemChange()
	{
		for (index_t i = 0; i < object_field.length(); i++)
			object_field[i].setSystemRec(root_system.matrix);
	}

	
template <class Def>
	void	Geometry<Def>::clearFlags(UINT32 clear_mask, bool geometries, bool wheels, bool accelerators, bool constructs)
	{
		for (index_t i = 0; i < object_field.length(); i++)
			object_field[i].clearFlags(clear_mask,geometries,wheels,accelerators,constructs);
	}

template <class Def>
	void	Geometry<Def>::flagAnimationTargets(UINT32 flag)
	{
		for (index_t i = 0; i < animator_field.length(); i++)
		{
			AnimatorA<Def>&a = animator_field[i];
			for (index_t j = 0; j < a.obj_trace_field.length(); j++)
				a.obj_trace_field[j].target->meta.flags |= flag;
			for (index_t j = 0; j < a.whl_trace_field.length(); j++)
				a.whl_trace_field[j].target->flags |= flag;
			for (index_t j = 0; j < a.acc_trace_field.length(); j++)
				a.acc_trace_field[j].target->flags |= flag;
		}
		for (index_t i = 0; i < object_field.length(); i++)
			object_field[i].flagAnimationTargets(flag);
	}


template <class Def> Geometry<Def>::Geometry():root_system(this),system_link(&root_system.matrix),texture_resource(&local_textures)
{}

template <class Def> Geometry<Def>::Geometry(RiffFile&source):root_system(this),system_link(&root_system.matrix),texture_resource(&local_textures)
{
	loadFromRiff(source);
}

template <class Def> Geometry<Def>::Geometry(const Geometry<Def>&other):root_system(this),system_link(&root_system.matrix),texture_resource(&local_textures)
{
	copy(other);
}

template <class Def>
template <class Def0> Geometry<Def>::Geometry(const Geometry<Def0>&other):root_system(this),system_link(&root_system.matrix),texture_resource(&local_textures)
{
	copy(other);
}



template <class Def>
	void	Geometry<Def>::walkChildren(void (*childCallback)(SubGeometryA<Def>*))
	{
		for (index_t i = 0; i < object_field.length(); i++)
			object_field[i].walkChildrenRec(childCallback);
	}

template <class Def>
	void	Geometry<Def>::walkChildren(SubGeometryCallback<Def>*callback)
	{
		for (index_t i = 0; i < object_field.length(); i++)
			object_field[i].walkChildrenRec(callback);
	}

template <class Def>
	void	Geometry<Def>::createVisualMaps(unsigned detail, unsigned max_depth)
	{
		for (index_t i = 0; i < object_field.length(); i++)
			object_field[i].createVisualMapsRec(detail,max_depth);
	}

template <class Def>template <class C>
	void	Geometry<Def>::scale(const C&factor)
	{
		radius *= factor;
		for (index_t i = 0; i < material_field.length(); i++)
			material_field[i].scale(factor);
		for (index_t i = 0; i < object_field.length(); i++)
			object_field[i].scale(factor);
		for (index_t i = 0; i < animator_field.length(); i++)
			animator_field[i].scale(factor);
		for (index_t i = 0; i < connector_field.length(); i++)
		{
			Vec::mult(connector_field[i].p0,factor);
			Vec::mult(connector_field[i].p1,factor);
		}
		Vec::mult(root_system.matrix.w.xyz,factor);
		for (index_t i = 0; i < object_field.length(); i++)
			object_field[i].setSystemRec(root_system.matrix);		
	}
	
template <class Def>
	template <class C>
		void	Geometry<Def>::translate(const TVec3<C>&delta)
		{
			for (index_t i = 0; i < object_field.length(); i++)
			{
				Vec::add(object_field[i].meta.system.w.xyz,delta);
				object_field[i].setSystemRec(root_system.matrix);		
			}
		}
	
	
template <class Def>
	void	Geometry<Def>::swapComponents(BYTE c0, BYTE c1)
	{
		if (c0 > 2 || c1 > 2 || c0 == c1)
			return;
		for (index_t i = 0; i < object_field.length(); i++)
		{
			typename Def::SystemType*system = object_field[i].meta.system.v;
			swp(system[0+c0],system[0+c1]);
			swp(system[4+c0],system[4+c1]);
			swp(system[8+c0],system[8+c1]);
			swp(system[12+c0],system[12+c1]);
			system[0+c0]*=-1;
			system[4+c0]*=-1;
			system[8+c0]*=-1;
			system[12+c0]*=-1;
			//_mult(system+4*c0,-1);
		}
		for (index_t i = 0; i < animator_field.length(); i++)
		{
			for (index_t j = 0; j < animator_field[i].obj_trace_field.length(); j++)
				for (index_t k = 0; k < animator_field[i].obj_trace_field[j].step_field.length(); k++)
				{
					swp(animator_field[i].obj_trace_field[j].step_field[k].range[c0],animator_field[i].obj_trace_field[j].step_field[k].range[c1]);
					swp(animator_field[i].obj_trace_field[j].step_field[k].range[3+c0],animator_field[i].obj_trace_field[j].step_field[k].range[3+c1]);
					animator_field[i].obj_trace_field[j].step_field[k].range[c0]*=-1;
				}
		}
		for (index_t i = 0; i < connector_field.length(); i++)
		{
			swp(connector_field[i].p0.v[c0],connector_field[i].p0.v[c1]);
			swp(connector_field[i].p1.v[c0],connector_field[i].p1.v[c1]);
			swp(connector_field[i].direction.v[c0],connector_field[i].direction.v[c1]);
			connector_field[i].p0.v[c0]*=-1;
			connector_field[i].p1.v[c0]*=-1;
			connector_field[i].direction.v[c0]*=-1;
		}
		
		for (index_t i = 0; i < object_field.length(); i++)
			object_field[i].setSystemRec(root_system.matrix);		
	}
	


template <class Def> count_t Geometry<Def>::countFaces()	const
{
	count_t total = 0;
	for (index_t i = 0; i < material_field.length(); i++)
		total += material_field[i].countFaces();
	return total;
}

template <class Def> count_t Geometry<Def>::countTriangles()	const
{
	count_t total = 0;
	for (index_t i = 0; i < material_field.length(); i++)
		total += material_field[i].countTriangles();
	return total;
}

template <class Def> count_t Geometry<Def>::countQuads()	const
{
	count_t total = 0;
	for (index_t i = 0; i < material_field.length(); i++)
		total += material_field[i].countQuads();
	return total;
}


template <class Def> count_t Geometry<Def>::countFaces(unsigned detail_layer)	const
{
	count_t total = 0;
	for (index_t i = 0; i < material_field.length(); i++)
		total += material_field[i].countFaces(detail_layer);
	return total;
}

template <class Def> count_t Geometry<Def>::countTriangles(unsigned detail_layer)	const
{
	count_t total = 0;
	for (index_t i = 0; i < material_field.length(); i++)
		total += material_field[i].countTriangles(detail_layer);
	return total;
}

template <class Def> count_t Geometry<Def>::countQuads(unsigned detail_layer)	const
{
	count_t total = 0;
	for (index_t i = 0; i < material_field.length(); i++)
		total += material_field[i].countQuads(detail_layer);
	return total;
}


template <class Def> count_t Geometry<Def>::countVertices()	const
{
	count_t total = 0;
	for (index_t i = 0; i < material_field.length(); i++)
		total += material_field[i].countVertices();
	return total;
}

template <class Def>
	unsigned		Geometry<Def>::maxDetail()								const
	{
		unsigned result(0);
		
		for (index_t i = 0; i < material_field.length(); i++)
			for (index_t j = 0; j < material_field[i].data.object_field.length(); j++)
				if (material_field[i].data.object_field[j].detail > result)
					result = material_field[i].data.object_field[j].detail;
		return result;
	}



template <class Def> String Geometry<Def>::toString(bool details)	const
{
	String rs = "Geometry\n";
	rs+=" Vertices: "+String(countVertices())+"\n";
	rs+=" Faces: "+String(countFaces())+" ("+String(countFaces(0))+")\n";
	rs+=" Objects: "+String(object_field.length())+"\n";
	if (details)
		for (index_t i = 0; i < object_field.length(); i++)
			rs+="	("+String(i)+") "+object_field[i].toString("	");
	rs+=" Materials: "+String(material_field.length())+"\n";
	if (details)
		for (index_t i = 0; i < material_field.length(); i++)
			rs+="	("+String(i)+") "+material_field[i].toString("	");
	rs+=" Animators: "+String(animator_field.length())+"\n";
	if (details)
		for (index_t i = 0; i < animator_field.length(); i++)
			rs+="	("+String(i)+") "+animator_field[i].toString("	");
	rs+=" Connectors: "+String(connector_field.length())+"\n";
	if (details)
		for (index_t i = 0; i < connector_field.length(); i++)
			rs+="	("+String(i)+") "+Vec::toString(connector_field[i].p0)+" - "+Vec::toString(connector_field[i].p1)+" -> "+Vec::toString(connector_field[i].direction)+"\n";
	rs+=" "+info;
	return rs;
}


template <class Def>
template <class Def0>Geometry<Def>& Geometry<Def>::operator=(const Geometry<Def0>&other)
{
	copy(other);
	return *this;
}

template <class Def>Geometry<Def>& Geometry<Def>::operator=(const Geometry<Def>&other)
{
	copy(other);
	return *this;
}
/*
template <class Def>void	Geometry<Def>::relink()
{
	for (auto it = animator_field.begin(); it != animator_field.end(); ++it)
		it->link(this);
	for (auto it = object_field.begin(); it != object_field.end(); ++it)
		it->postCopyLink(this);
	for (auto it = material_field.begin(); it != material_field.end(); ++it)
		it->postCopyLink(this,textures);
}*/


template <class Def>void	Geometry<Def>::adoptData(Geometry<Def>&other)
{
	CGS_POINT
	if (this == &other)
		return;
	storeTargetNames();

	object_field.adoptData(other.object_field);
	material_field.adoptData(other.material_field);
	animator_field.adoptData(other.animator_field);
	connector_field.adoptData(other.connector_field);
	local_textures.entry_field.adoptData(other.local_textures.entry_field);
	if (other.texture_resource == &other.local_textures)
		texture_resource = &local_textures;
	else
		texture_resource = other.texture_resource;

	object_table.adoptData(other.object_table);
	wheel_table.adoptData(other.wheel_table);
	accelerator_table.adoptData(other.accelerator_table);
	animator_table.adoptData(other.animator_table);

	root_system.matrix = other.root_system.matrix;
	radius = other.radius;
	info = other.info;
	xml.adoptData(other.xml);

	relinkEverything();

	CGS_POINT
}


template <class Def>
template <class Def0>
	void	Geometry<Def>::copy(const Geometry<Def0>&other)
{
	object_field = other.object_field;
	material_field = other.material_field;
	animator_field = other.animator_field;
	connector_field = other.connector_field;
	
	this->local_textures = other.local_textures;
	if (other.texture_resource == &other.local_textures)
		this->texture_resource = &this->local_textures;
	else
		this->texture_resource = other.texture_resource;

	object_table = other.object_table;
	wheel_table = other.wheel_table;
	accelerator_table = other.accelerator_table;
	animator_table = other.animator_table;

	root_system.matrix = other.root_system.matrix;
	radius = other.radius;
	info = other.info;
	xml = other.xml;
	
	system_link = &root_system.matrix;


	for (index_t i = 0; i < animator_field.length(); i++)
		animator_field[i].link(this);
	for (index_t i = 0; i < object_field.length(); i++)
		object_field[i].postCopyLink(this);
	for (index_t i = 0; i < material_field.length(); i++)
		material_field[i].postCopyLink(this,texture_resource);
}



template <class Def>
	void Geometry<Def>::storeTargetNames()
	{
		for (index_t i = 0; i < animator_field.length(); i++)
			animator_field[i].storeTargetNames();
		for (index_t i = 0; i < object_field.length(); i++)
			object_field[i].storeTargetNames();
		for (index_t i = 0; i < material_field.length(); i++)
			material_field[i].storeTargetNames();
	}
	
template <class Def>
	void	Geometry<Def>::relinkEverything()
	{
		for (index_t i = 0; i < animator_field.length(); i++)
			animator_field[i].link(this);
		for (index_t i = 0; i < object_field.length(); i++)
			object_field[i].postCopyLink(this);
		for (index_t i = 0; i < material_field.length(); i++)
			material_field[i].postCopyLink(this,texture_resource == &local_textures ? &local_textures : NULL);	//don't relink external textures
	}





template <class Def> void Geometry<Def>::clear()
{
	object_field.free();
	material_field.free();
	animator_field.free();
	connector_field.free();
	texture_resource = &local_textures;

	info = "";
	xml = "";
	system_link = &root_system.matrix;
}

template <class Def> bool Geometry<Def>::saveToFile(const String&filename)	const
{
	return saveToFile(filename.c_str());
}

template <class Def> bool Geometry<Def>::saveToFile(const char*filename)	const
{
	RiffChunk	riff;
	UINT32 version = CGS_FILE_VERSION;
	riff.appendBlock("VERS",&version,sizeof(version));
	saveEmbedded(riff,true);
	return riff.saveToFile(filename);
}

template <class Def> bool Geometry<Def>::loadFromFile(const String&filename,TextureResource*resource/*=NULL*/)
{
	return loadFromFile(filename.c_str(),resource);
}

template <class Def> bool Geometry<Def>::loadFromFile(const char*filename,TextureResource*resource/*=NULL*/)//, bool post_strip, unsigned min_strip)
{
	RiffFile riff;
	if (!riff.open(filename))
		return false;
	unsigned long version = 0;
	if (riff.findFirst("VERS") && riff.getSize() == sizeof(version))
		riff.get(&version);
	if (version > CGS_FILE_VERSION)
	{
		riff.close();
		return false;
	}
	loadFromRiff(riff,resource);//, post_strip,min_strip);
	CGS_MSG("loaded '"+String(filename)+"'");
	
	return true;
}

/*
template <class Def> String Geometry<Def>::difference(Geometry*other)
{
	String rs="dif_test";
	
	#define COMPARE(cnt,obj)	rs+= vstructArrayCompare(#cnt,cnt,obj,other->cnt,other->obj)
	
		COMPARE(materials,material);
		COMPARE(objects,object);
//		COMPARE(animators,animator);
//		COMPARE(connectors,connector);

	#undef COMPARE
	return rs;
}*/


template <class Def> void Geometry<Def>::saveEmbedded(RiffChunk&riff, bool embed_textures)	const
{

	if (info.length())
		riff.appendBlock("INFO",info.c_str(),info.length());
	if (xml.length())
	{
		Array<BYTE> buffer(xml.length()+4);
		(*(UINT32*)buffer.pointer()) = (UINT32)xml.length();
		size_t compressed = BZ2::compress(xml.c_str(),xml.length(),buffer.pointer()+4,buffer.contentSize()-4);
		if (compressed)
			riff.appendBlock("CXML",buffer.pointer(),compressed+4);
	}
	if (embed_textures)
	{
		RiffChunk*tlist = openBlock(riff,DATA_TEXTURES);
			for (index_t i = 0; i < local_textures.entry_field.length(); i++)
			{
				tlist->appendBlock("ID  ",&local_textures.entry_field[i].name,sizeof(tName));
				for (BYTE k = 0; k < local_textures.entry_field[i].face_field.length(); k++)
					tlist->appendBlock("FACE",local_textures.entry_field[i].face_field[k]);
			}
	}
	ByteStream	buffer;
	
	RiffChunk*mlist = openBlock(riff,DATA_MATERIAL);
		for (index_t i = 0; i < material_field.length(); i++)
		{
			RiffChunk*mout = mlist->appendBlock(RIFF_LIST);
			

			buffer.reset();
			buffer << (UINT32)AlphaTestFlag*material_field[i].info.alpha_test;
			buffer.push(material_field[i].info.ambient.v,4);
			buffer.push(material_field[i].info.diffuse.v,4);
			buffer.push(material_field[i].info.emission.v,4);
			buffer.push(material_field[i].info.specular.v,4);
			buffer << material_field[i].info.shininess;
			buffer << material_field[i].info.alpha_threshold;
			

			mout->appendBlock("MATR",buffer.pointer(),buffer.fillLevel());
			mout->appendBlock("NAME",material_field[i].name.c_str(),material_field[i].name.length()+1);
			for (index_t j = 0; j < material_field[i].info.layer_field.length(); j++)
			{
				buffer.reset();
				buffer << (UINT32)material_field[i].info.layer_field[j].combiner;
				UINT32 flags = 0;
				if (material_field[i].info.layer_field[j].mirror_map)
					flags |= MirrorFlag;
				if (material_field[i].info.layer_field[j].clamp_x)
					flags |= ClampXFlag;
				if (material_field[i].info.layer_field[j].clamp_y)
					flags |= ClampYFlag;

				buffer << flags;
				buffer << (INT64)(material_field[i].info.layer_field[j].source?material_field[i].info.layer_field[j].source->name:0);
				mout->appendBlock("TLYR",buffer.pointer(),buffer.fillLevel());
			}

			const MaterialData<Def>&d = material_field[i].data;
			mout->appendBlock("VCRD",&d.coord_layers,sizeof(UINT16));
			
			for (index_t j = 0; j < d.object_field.length(); j++)
			{
				RiffChunk*obj = mout->appendBlock(RIFF_LIST);

				ASSERT_EQUAL1__(d.object_field[j].vpool.vlyr,d.coord_layers,j);
				
				buffer.reset();
				buffer << (UINT32)d.object_field[j].vpool.vcnt;
				buffer << (UINT32)d.object_field[j].vpool.vflags;
				buffer << (UINT32)d.object_field[j].detail;
				buffer.push(d.object_field[j].target->name.c_str(),d.object_field[j].target->name.length());
				
				obj->appendBlock("ROB3",buffer.pointer(),buffer.fillLevel());

				obj->appendBlock("RVTX",d.object_field[j].vpool.vdata);

				
				//for (index_t k = 0; k < d.object_field[j].detail_layer_field.length(); k++)
				{
					const IndexContainerA<Def>&chunk = d.object_field[j].ipool;
					obj->appendBlock("IOBJ",chunk.idata);
					obj->appendBlock("ICMP",chunk.composition,8);
					
					//obj->appendBlock("SOBJ",chunk.sdata,chunk.sdata.contentSize());
					//obj->appendBlock("QOBJ",chunk.qdata,chunk.qdata.contentSize());
				}
			}
		}
	RiffChunk*olist = openBlock(riff,DATA_OBJECTS);
 	for (index_t i = 0; i < object_field.length(); i++)
			object_field[i].saveToRiff(olist);
	RiffChunk*alist = openBlock(riff,DATA_ANIMATOR);
		for (index_t i = 0; i < animator_field.length(); i++)
			animator_field[i].saveToRiff(alist);
	RiffChunk*clist = openBlock(riff,DATA_CONNECTR);
		clist->appendBlock("CNCT",connector_field);
}


template <class Def> void Geometry<Def>::loadMaterials(RiffFile&riff)//, bool post_strip, unsigned min_strip)
{
	Buffer<MaterialA<Def>,0,AdoptStrategy>		mbuffer;
	Buffer<RenderObjectA<Def>,0,AdoptStrategy>	obuffer;
	//Vector<IndexContainer<Def> >		cbuffer;
	Buffer<TLayer,0>							lbuffer;

	//TFMaterialInfo<Def> info;
	//TFLayer				layr;
	TRenderObjectInfoIO	rinf;

	count_t total_indices(0);
	

	if (riff.findFirst(RIFF_LIST))
		do
		{
			riff.enter();
				if (riff.findFirst("MATR"))
				{
					MaterialA<Def>&material = mbuffer.append();
					
					riff.openStream();
					UINT32 flags;
					bool success = true;
					success = success && riff.stream(flags);
					success = success && riff.stream(material.info.ambient.v,4);
					success = success && riff.stream(material.info.diffuse.v,4);
					success = success && riff.stream(material.info.emission.v,4);
					success = success && riff.stream(material.info.specular.v,4);
					success = success && riff.stream(material.info.shininess);
					success = success && riff.stream(material.info.alpha_threshold);
					riff.closeStream();
					if (!success)
					{
						mbuffer.eraseLast();
						CGS_MSG("Stream errors while streaming material");
						riff.exit();
						continue;
					}
					
					material.info.alpha_test = flags&AlphaTestFlag;
					
						
					if (riff.findFirst("NAME"))
					{
						Array<char> field(riff.getSize()+1);
						riff.get(field.pointer());
						field[field.size()-1] = 0;
						material.name = field;
					}
					
					CGS_MSG("new material ("+material.name+")");
					
					

					if (riff.findFirst("TLYR"))
						do
						{
							TLayerIO	layr;
							riff.openStream();
							
							if (!(riff.stream(layr.combiner))
								||
								!(riff.stream(layr.flags))
								||
								!(riff.stream(layr.sname)))
							{
								riff.closeStream();
								continue;
							}
							riff.closeStream();
							
							TLayer&layer = lbuffer.append();
							layer.source_name = layr.sname;
							layer.cube_map = false;
							layer.mirror_map = !!(layr.flags&MirrorFlag);
							layer.clamp_x = !!(layr.flags&ClampXFlag);
							layer.clamp_y = !!(layr.flags&ClampYFlag);
							layer.combiner = layr.combiner;
						}
						while (riff.findNext("TLYR"));


					bool valid = false;
					if (riff.findFirst("VINF") && riff.getSize() == sizeof(UINT32)*2)
					{
						UINT32 d;
						riff.openStream();
						riff.stream(d);	//ignored
						riff.stream(d);	material.data.coord_layers = (UINT16)d;
						riff.closeStream();
						valid = true;
					}
					elif (riff.findFirst("VCRD") && riff.getSize() == sizeof(UINT16))
					{
						riff.get(&material.data.coord_layers);
						valid = true;
					}

					if (valid)
					{
						if (riff.findFirst(RIFF_LIST))
							do
							{
								riff.enter();
								{
									RenderObjectA<Def>*object;
									if (riff.findFirst("ROBJ"))
									{
										if (riff.getSize() == sizeof(name64_t)+sizeof(rinf.vcnt)+sizeof(UINT32))
										{
											riff.openStream();
												name64_t name;
												riff.stream(name);
												rinf.tname = name2str(name);
												riff.stream(rinf.vcnt);
												UINT32 dummy;
												riff.stream(dummy); //deprectaed additional unformated content
											riff.closeStream();
											rinf.detail = 0;
											rinf.vflags = HasNormalFlag;
										}
										elif (riff.getSize() == sizeof(name64_t)+sizeof(rinf.vcnt)+sizeof(UINT32)+sizeof(rinf.detail))
										{
											riff.openStream();
												name64_t name;
												riff.stream(name);
												rinf.tname = name2str(name);
												riff.stream(rinf.vcnt);
												UINT32 dummy;
												riff.stream(dummy);	//deprectaed additional unformated content
												riff.stream(rinf.detail);
												rinf.vflags = HasNormalFlag;
											riff.closeStream();
										}
										else
										{
											CGS_MSG("ROBJ chunk is of unexpected size: "+String(riff.getSize()));
												ShowMessage(riff.getSize());
											riff.exit();
											continue;
										}
									}
									elif (riff.findFirst("ROB2"))
									{
										if (riff.getSize() == sizeof(name64_t)+sizeof(rinf.vcnt)+sizeof(rinf.vflags)+sizeof(rinf.detail))
										{
											riff.openStream();
												name64_t	name;
												riff.stream(name);
												rinf.tname = name2str(name);
												riff.stream(rinf.vcnt);
												riff.stream(rinf.vflags);
												riff.stream(rinf.detail);
											riff.closeStream();
										}
										else
										{
											CGS_MSG("ROB2 chunk is of unexpected size: "+String(riff.getSize()));
		//										ShowMessage(sizeof(rinf));
											riff.exit();
											continue;
										}
									}
									elif (riff.findFirst("ROB3"))
									{
										if (riff.getSize() > sizeof(rinf.vcnt)+sizeof(rinf.vflags)+sizeof(rinf.detail))
										{
											riff.openStream();
												riff.stream(rinf.vcnt);
												riff.stream(rinf.vflags);
												riff.stream(rinf.detail);

												size_t string_length = riff.getSize() - riff.streamTell();
												rinf.tname.setLength(string_length);
												riff.stream(rinf.tname.mutablePointer(),string_length);
											riff.closeStream();
										}
										else
										{
											CGS_MSG("ROB3 chunk is of unexpected size: "+String(riff.getSize()));
		//										ShowMessage(sizeof(rinf));
											riff.exit();
											continue;
										}
									}
									else
									{
										CGS_MSG("unable to find ROBJ, ROB2, or ROB3");
	//										ShowMessage(sizeof(rinf));
										riff.exit();
										continue;
									}
									if (riff.findFirst("RVTX"))
									{
										if (riff.getSize() == rinf.vcnt*VSIZE(material.data.coord_layers,rinf.vflags)*sizeof(typename Def::FloatType))
										{
											object = &obuffer.append();
											object->tname = rinf.tname;
											object->detail = rinf.detail;
											object->vpool.setSize(rinf.vcnt,material.data.coord_layers,rinf.vflags);
											riff.get(object->vpool.vdata);
											CGS_MSG("loaded "+String(object->vpool.vdata.length())+" floats for "+rinf.tname+". loading indices...");
											object->vpool.updateCRC();
										}
										else
										{
											CGS_MSG("RVTX of wrong size ("+String(riff.getSize())+" != "+String(rinf.vcnt)+"*(3+3+2*"+String(material.data.coord_layers)+"+"+String(rinf.vflags)+")*"+String(sizeof(typename Def::FloatType))+" (="+String(rinf.vcnt*VSIZE(material.data.coord_layers,rinf.vflags)*sizeof(typename Def::FloatType))+"))");
											riff.exit();
											continue;
										}
									}
									else
									{
										CGS_MSG("RVTX not found");
										riff.exit();
										continue;
									}
									if (riff.findFirst("IOBJ"))
										//do
										{
											if (riff.multipleOf(sizeof(typename Def::IndexType)))
											{
												IndexContainerA<Def>*chunk = &object->ipool;
												chunk->idata.setSize(riff.getSize()/sizeof(typename Def::IndexType));
												riff.get(chunk->idata);
												total_indices += chunk->idata.length();
												CGS_MSG("loaded indices: "+String(chunk->idata.length())+" to a total of "+String(total_indices)+" indices");
												if (riff.next())
												{
													if (riff.isID("ICMP") && riff.getSize()==8)
													{
														riff.get(chunk->composition);
													}
													elif (riff.isID("SOBJ") && riff.multipleOf(sizeof(typename Def::IndexType)))
													{
														typename Def::IndexType indices;
														riff.openStream();
															riff.stream(indices);
															chunk->triangles = indices/3;
														riff.closeStream();
														
														if (riff.next() && riff.isID("QOBJ") && riff.multipleOf(sizeof(typename Def::IndexType)))
														{
															typename Def::IndexType indices;
															riff.openStream();
																riff.stream(indices);
																chunk->quads = indices/4;
															riff.closeStream();
															//CGS_MSG("got strip data: "+_toString(chunk->sdata.pointer(),chunk->sdata.length()));
														}
														else
															chunk->quads = 0;
														//CGS_MSG("got strip data: "+_toString(chunk->sdata.pointer(),chunk->sdata.length()));
													}
													else
													{
														//CGS_MSG("generated strip data");
														chunk->triangles = UINT32(chunk->idata.length()/3);
														chunk->quads = 0;
													}
												}
												else
												{
													//CGS_MSG("generated strip data");
													chunk->quads = 0;
												}
											}
											else
												CGS_MSG("odd shaped IOBJ");
										}
										//while (riff.findNext("IOBJ"));
								}
								riff.exit();
							}
							while (riff.findNext(RIFF_LIST));
						obuffer.moveToArray(material.data.object_field);
						//material.data.object_field.adoptData(obuffer);
						/*material->data.object_field.setSize(obuffer.count());
						for (index_t i = 0; i < material->data.object_field.length(); i++)
						{
							// CGS_MSG("casting object("+String(i)+"): "+String(obuffer[i]->vpool.vcnt)+"/"+String(obuffer[i]->countIndices()));
							material->data.object_field[i].adoptData(*obuffer[i]);
							// CGS_MSG("  -> "+String(material->data.object_field[i].vpool.vcnt)+" / "+String(material->data.object_field[i].countIndices()));
						}
						obuffer.clear();*/
					}
					lbuffer.moveToArray(material.info.layer_field);
					//material.info.layer_field.adoptData(lbuffer);
						/*.setSize(lbuffer.count());
					for (index_t i = 0; i < material->info.layer_field.length(); i++)
						material->info.layer_field[i] = *lbuffer[i];
					lbuffer.clear();*/
				}
			riff.exit();
		}
		while (riff.findNext(RIFF_LIST));
	mbuffer.moveToArray(material_field);
	//material_field.adoptData(mbuffer);
	/*material_field.setSize(mbuffer.count());
	for (index_t i = 0; i < material_field.length(); i++)
	{
		// CGS_MSG("casting material("+String(i)+"): "+String(mbuffer[i]->data.countChunks()));
		material_field[i].adoptData(*mbuffer[i]);
		// CGS_MSG("  -> "+String(material_field[i].data.countChunks()));
	}
	mbuffer.clear();*/
	
	CGS_MSG("materials loaded: "+String(mbuffer.count()));
	for (index_t i = 0; i < material_field.length(); i++)
		CGS_MSG("material "+String(i)+" '"+material_field[i].name+"' has "+String(material_field[i].countFaces())+"("+String(material_field[i].countFaces(0))+") face(s) and "+String(material_field[i].countVertices())+" vertices in "+String(material_field[i].data.object_field.length())+" object(s)");
}

template <class Def> void Geometry<Def>::loadObjects(RiffFile&riff)
{
	count_t object_count = 0;
	if (riff.findFirst("OBJ "))
		do
		{
			if (riff.next() && riff.isID(RIFF_LIST))
				object_count++;
		}
		while (riff.findNext("OBJ "));
	object_field.setSize(object_count);
	index_t at = 0;
	//Buffer<index_t>	path;
	if (riff.findFirst("OBJ "))
		do
		{
			if (at < object_count)
			{
				object_field[at].name.setLength(riff.getSize());
				riff.get(object_field[at].name.mutablePointer());
				object_field[at].name.trimThis();
				//riff.get(&object_field[at].name);
				if (riff.next() && riff.isID(RIFF_LIST))
				{
					//path << at;
					riff.enter();
						object_field[at++].loadFromRiff(riff);
					riff.dropBack();
					//path.eraseLast();
				}
			}
		}
		while (riff.findNext("OBJ "));
}

template <class Def> void Geometry<Def>::loadAnimators(RiffFile&riff)
{
	count_t count = 0;
	if (riff.findFirst("ANIM"))
	do
		count++;
	while (riff.findNext("ANIM"));
	animator_field.setSize(count);

	Buffer<index_t>	path;	//empty path

	for (index_t i = 0; i < count; i++)
		animator_field[i].loadFromRiff(riff,i);
}

template <class Def> void Geometry<Def>::loadConnectors(RiffFile&riff)
{
	if (riff.findFirst("CNCT"))
	{
		count_t count = riff.getSize()/sizeof(TConnector<Def>);
		connector_field.setSize(count);
		riff.get(connector_field);
	}
}

template <class Def> void Geometry<Def>::loadTextures(RiffFile&riff)
{
	CGS_POINT
	count_t count = 0;
	if (riff.findFirst(RIFF_ID))
		do
			if (riff.getSize() == 8)
				count++;
		while (riff.findNext(RIFF_ID));
	CGS_POINT
	local_textures.entry_field.setSize(count);
	CGS_POINT
	TextureA*texture = NULL;
	index_t at = 0;
	if (riff.first())
		do
		{
			switch (riff.getChunk().info.id)
			{
				case RIFF_ID:
					if (riff.getSize() == 8)
					{
						CGS_POINT
						texture = &local_textures.entry_field[at++];
						riff.get(&texture->name);
						CGS_POINT
					}
				break;
				case RIFF_FACE:
					if (texture)
					{
						CGS_POINT
						index_t index = texture->face_field.length();
						texture->face_field.resizePreserveContent(index+1);	//this has got to be the second most inefficient way to do this :S
						texture->face_field[index].setSize(riff.getSize());
						riff.get(texture->face_field[index].pointer());
						CGS_POINT
					}
				break;
			}
		}
		while (riff.next());

	for (index_t i = 0; i < local_textures.entry_field.length(); i++)
		local_textures.entry_field[i].updateHash();
	CGS_POINT
}

template <class Def> void Geometry<Def>::loadEmbedded(RiffFile&riff, TextureResource*resource)//, bool post_strip, unsigned min_strip)
{
	clear();
	
	CGS_POINT
	
	if (riff.findFirst("CXML") && riff.getSize())
	{
		Array<BYTE> buffer(riff.getSize());
		riff.get(buffer.pointer());
		UINT32 extracted = (*(UINT32*)buffer.pointer());
		Array<char> extracted_buffer(extracted+1);
		size_t bz_result = BZ2::decompress(buffer.pointer()+4,buffer.contentSize()-4,extracted_buffer.pointer(),extracted_buffer.contentSize());
		if (bz_result==(size_t)extracted)
		{
			extracted_buffer[extracted] = 0;
			xml = extracted_buffer;
		}
		else
			if (bz_result)
				CGS_MSG("Unable to extract XML content. Stored size is "+String(extracted)+". BZ2 reports "+String((int)bz_result))
			else
				CGS_MSG("Unable to extract XML content: "+String(BZ2::errorStr()));
	}
	
	if (riff.findFirst("INFO") && riff.getSize())
	{
		RIFF_SIZE size = riff.getSize();
		char*buffer = SHIELDED_ARRAY(new char[size+1],size+1);
		riff.get(buffer);
		buffer[size] = 0;
		for (RIFF_SIZE i = 0; i < size; i++)
			if (!buffer[i])
				buffer[i] = '\n';
		info = buffer;
		DISCARD_ARRAY(buffer);
	}
	else
		if (riff.findFirst("IDNT") && riff.getSize())
		{
			RIFF_SIZE size = riff.getSize();
			char*buffer = SHIELDED_ARRAY(new char[size+1],size+1),
				*first(buffer),*name(NULL),*cont(NULL);
			riff.get(buffer);
			buffer[size] = 0;
			for (RIFF_SIZE i = 0; i < size; i++)
				if (!buffer[i])
				{
					if (!name)
						name = first;
					else
						cont = first;
					first = buffer+i+1;
				}
			if (name)
				info = name;
			if (cont)
			{
				if (name)
					info+="\n";
				info += cont;
			}
			DISCARD_ARRAY(buffer);
		}
	
	CGS_POINT
	
	if (riff.findFirst(RIFF_DATA))
		do
			if (riff.getSize() == sizeof(tName))
			{
				tName name;
				riff.get(&name);
				if (!riff.next() || !riff.isID(RIFF_LIST))
					continue;
				riff.enter();
					switch (name)
					{
						case DATA_MATERIAL:
							CGS_POINT
							loadMaterials(riff);//,post_strip, min_strip);
							CGS_POINT
						break;
						case DATA_OBJECTS:
							CGS_POINT
							loadObjects(riff);
							CGS_POINT
						break;
						case DATA_TEXTURES:
							CGS_POINT
							loadTextures(riff);
							CGS_POINT
						break;
						case DATA_ANIMATOR:
							CGS_POINT
							loadAnimators(riff);
							CGS_POINT
						break;
						case DATA_CONNECTR:
							CGS_POINT
							loadConnectors(riff);
							CGS_POINT
						break;
					}
				riff.dropBack();
			}
		while (riff.findNext(RIFF_DATA));

	remap(~RelinkBit);

	if (!resource)
		texture_resource = &local_textures;
	else
		texture_resource = resource;
	CGS_POINT



	
	for (index_t i = 0; i < material_field.length(); i++)
	{
		material_field[i].linkTextures(texture_resource);
		count_t vcnt(0);
		for (index_t j = 0; j < material_field[i].data.object_field.length(); j++)
		{
			RenderObjectA<Def>&obj = material_field[i].data.object_field[j];

			obj.target = lookupObject(obj.tname);
			if (obj.target)
				vcnt ++;
				//#ifdef DEBUG
			else
				FATAL__("unable to find object \""+obj.tname+"\" in object "+String(j)+" material "+String(i));
				//#endif
		}
		if (vcnt < material_field[i].data.object_field.length())
		{
			Array<RenderObjectA<Def>,Adopt>	npntr(vcnt);
			vcnt = 0;
			for (index_t j = 0; j < material_field[i].data.object_field.length(); j++)
				if (material_field[i].data.object_field[j].target)
				{
					npntr[vcnt++].adoptData(material_field[i].data.object_field[j]);
					//material_field[i].data.object_field[j].abandon();
				}
			material_field[i].data.object_field.adoptData(npntr);
		}
	}
	CGS_POINT

	
	for (index_t i = 0; i < animator_field.length(); i++)
		animator_field[i].link(this);
	root_system.loadIdentity(false);
	CGS_POINT
	for (index_t i = 0; i < object_field.length(); i++)
	{
		object_field[i].linkAnimators(this);
		object_field[i].setSystemRec(root_system.matrix);
	}
	CGS_POINT
	updateRadius();
	CGS_MSG("finished");
}


template <class Def> void Geometry<Def>::loadFromRiff(RiffFile&riff,TextureResource*resource/*=NULL*/)//, bool post_strip, unsigned min_strip)
{
	loadEmbedded(riff,resource);
}

template <class Def>
	const SubGeometryA<Def>*	Geometry<Def>::lookupChild(const index_t*path_begin, index_t path_length)	const
	{
		ASSERT__(path_length > 0);

		ASSERT_LESS__(*path_begin, object_field.length());

		const SubGeometryA<Def>*current_obj = object_field + *path_begin;

		const index_t	*current = path_begin+1,
						*end = path_begin + path_length;

		while (current != end)
		{
			if (*current >= current_obj->child_field.length())
			{
				FATAL__("Path entry "+String(current-path_begin)+" points to an invalid child of sub geometry '"+current_obj->name+"'");
				return NULL;
			}
			current_obj = current_obj->child_field + *current;
			current++;
		}

		return current_obj;
	}


template <class Def>
	SubGeometryA<Def>*	Geometry<Def>::lookupChild(const index_t*path_begin, index_t path_length)
	{
		ASSERT__(path_length > 0);
		ASSERT__(path_length < (count_t)-10);

		ASSERT_LESS__(*path_begin, object_field.length());

		SubGeometryA<Def>*current_obj = object_field + *path_begin;

		const index_t	*current = path_begin+1,
						*end = path_begin + path_length;

		while (current != end)
		{
			if (*current >= current_obj->child_field.length())
			{
				FATAL__("Path entry "+String(current-path_begin)+" points to an invalid child of sub geometry '"+current_obj->name+"'");
				return NULL;
			}
			current_obj = current_obj->child_field + *current;
			current++;
		}

		return current_obj;
	}

template <class Def> SubGeometryA<Def>*Geometry<Def>::lookupObject(const String&name)
{
	const Path*p = object_table.queryPointer(name);

	return p ? lookupChild(p->pointer(),p->count()) : NULL;
}

template <class Def> const SubGeometryA<Def>*Geometry<Def>::lookupObject(const String&name) const
{
	const Path*p = object_table.queryPointer(name);

	return p ? lookupChild(p->pointer(),p->count()) : NULL;
}



template <class Def>
	void		SubGeometryA<Def>::remap(BuildPath&path, Geometry<Def>&geometry, UINT32 bits)
	{
		if (bits & ObjectBit)
			geometry.object_table.set(name,path.copyToArray());

		if (bits & (WheelBit | AnimatorBit))
			for (index_t i = 0; i < wheel_field.length(); i++)
			{
				path << i;
					if (bits & WheelBit)
						geometry.wheel_table.set(wheel_field[i].name,path.copyToArray());

					if (bits & AnimatorBit)
					{
						path << 1;
							geometry.animator_table.set(wheel_field[i].name+'.'+wheel_field[i].suspension.name,path.copyToArray());
						path.eraseLast();
						path << 2;
							geometry.animator_table.set(wheel_field[i].name+'.'+wheel_field[i].rotation.name,path.copyToArray());
						path.eraseLast();
					}

				path.eraseLast();
			}
		if (bits & AcceleratorBit)
			for (index_t i = 0; i < accelerator_field.length(); i++)
			{
				path << i;
					geometry.accelerator_table.set(accelerator_field[i].name,path.copyToArray());
				path.eraseLast();
			}

		for (index_t i = 0; i < child_field.length(); i++)
		{
			path << i;
			child_field[i].remap(path,geometry,bits);
			path.eraseLast();
		}
	}



template <class Def>
	void		Geometry<Def>::remap(UINT32 bits)
	{
		if (bits & ObjectBit)
			object_table.clear();
		if (bits & AcceleratorBit)
			accelerator_table.clear();
		if (bits & AnimatorBit)
			animator_table.clear();
		if (bits & WheelBit)
			wheel_table.clear();


		Buffer<index_t,4>	path;
		for (index_t i = 0; i < object_field.length(); i++)
		{
			path << i;

				object_field[i].remap(path,*this, bits);

			path.eraseLast();
			ASSERT__(path.isEmpty());
		}
		if (bits & AnimatorBit)
			for (index_t i = 0; i < animator_field.length(); i++)
			{
				path << i;
				
				animator_table.set(animator_field[i].name,path.copyToArray());

				path.eraseLast();
				ASSERT__(path.isEmpty());
			}
		if (bits & RelinkBit)
			relinkEverything();
	}



template <class Def> TWheel<Def>* Geometry<Def>::lookupWheel(const String&name)
{
	const Path*p = wheel_table.queryPointer(name);
	if (!p)
		return NULL;
	SubGeometryA<Def>*context = lookupChild(p->pointer(),p->count()-1);

	const index_t*current = p->pointer() + p->count()-1;
	if (*current >= context->wheel_field.length())
	{
		FATAL__("Path entry "+String(current-p->pointer())+" points to an invalid wheel of sub geometry '"+context->name+"'");
		return NULL;
	}

	return context->wheel_field + *current;
}

template <class Def> const TWheel<Def>* Geometry<Def>::lookupWheel(const String&name) const
{
	const Path*p = wheel_table.queryPointer(name);
	if (!p)
		return NULL;
	const SubGeometryA<Def>*context = lookupChild(p->pointer(),p->count()-1);

	const index_t*current = p->pointer() + p->count()-1;
	if (*current >= context->wheel_field.length())
	{
		FATAL__("Path entry "+String(current-p->pointer())+" points to an invalid wheel of sub geometry '"+context->name+"'");
		return NULL;
	}

	return context->wheel_field + *current;
}

template <class Def> TAccelerator<Def>*Geometry<Def>::lookupAccelerator(const String&name)
{
	const Path*p = accelerator_table.queryPointer(name);
	if (!p)
		return NULL;
	SubGeometryA<Def>*context = lookupChild(p->pointer(),p->count()-1);

	const index_t*current = p->pointer() + p->count()-1;
	if (*current >= context->accelerator_field.length())
	{
		FATAL__("Path entry "+String(current-p->pointer())+" points to an invalid accelerator of sub geometry '"+context->name+"'");
		return NULL;
	}

	return context->accelerator_field + *current;
}

template <class Def> const TAccelerator<Def>*Geometry<Def>::lookupAccelerator(const String&name) const
{
	const Path*p = accelerator_table.queryPointer(name);
	if (!p)
		return NULL;
	const SubGeometryA<Def>*context = lookupChild(p->pointer(),p->count()-1);

	const index_t*current = p->pointer() + p->count()-1;
	if (*current >= context->accelerator_field.length())
	{
		FATAL__("Path entry "+String(current-p->pointer())+" points to an invalid accelerator of sub geometry '"+context->name+"'");
		return NULL;
	}

	return context->accelerator_field + *current;
}

template <class Def> AnimatorA<Def>*Geometry<Def>::lookupAnimator(const String&name)
{
	const Path*p = animator_table.queryPointer(name);
	if (!p)
		return NULL;

	if (p->length() == 1)
	{
		ASSERT_LESS__(p->first(),animator_field.length());
		return animator_field + p->first();
	}

	SubGeometryA<Def>*context = lookupChild(p->pointer(),p->count()-2);

	const index_t*current = p->pointer() + p->count()-2;
	if (*current >= context->wheel_field.length())
	{
		FATAL__("Path entry "+String(current-p->pointer())+" points to an invalid wheel of sub geometry '"+context->name+"'");
		return NULL;
	}
	TWheel<Def>*wheel = context->wheel_field+*current;
	current++;
	ASSERT_LESS__(current,p->pointer()+p->count());	//MARKED TRANSITORY
	switch (*current)
	{
		case 1:
			return &wheel->suspension;
		break;
		case 2:
			return &wheel->rotation;
		break;
	}

	FATAL__("Unexpected final index in path ("+String(current)+"). Expected 1 or 2.");
	return NULL;
}

template <class Def> const AnimatorA<Def>*Geometry<Def>::lookupAnimator(const String&name) const
{
	const Path*p = animator_table.queryPointer(name);
	if (!p)
		return NULL;

	if (p->length() == 1)
	{
		ASSERT_LESS__(p->first(),animator_field.length());
		return animator_field + p->first();
	}

	const SubGeometryA<Def>*context = lookupChild(p->pointer(),p->count()-2);

	const index_t*current = p->pointer() + p->count()-2;
	if (*current >= context->wheel_field.length())
	{
		FATAL__("Path entry "+String(current-p->pointer())+" points to an invalid wheel of sub geometry '"+context->name+"'");
		return NULL;
	}
	const TWheel<Def>*wheel = context->wheel_field+*current;
	current++;
	ASSERT_LESS__(current,p->pointer()+p->count());	//MARKED TRANSITORY
	switch (*current)
	{
		case 1:
			return &wheel->suspension;
		break;
		case 2:
			return &wheel->rotation;
		break;
	}

	FATAL__("Unexpected final index in path ("+String(current)+"). Expected 1 or 2.");
	return NULL;
}


template <class Def>
template <class C> bool Geometry<Def>::extractDimensions(TBox<C>&dim) const
{
	if (!object_field.length())
		return false;
	DimensionParser<C> parser;
	TMatrix4<typename Def::SystemType> matrix = Matrix<typename Def::SystemType>::eye4;
	bool def = false;
	for (index_t i = 0; i < object_field.length(); i++)
		def = object_field[i].extractDimensions(matrix,parser)||def;
	dim = parser.dimension;
	return def;
}

template <class Def> void Geometry<Def>::checkLinkage()
{
	for (index_t i = 0; i < object_field.length(); i++)
		object_field[i].checkLinkage();
}

template <class Def>
template <class C> void Geometry<Def>::extractRadius(C&radius) const
{
	radius = 0;
	for (index_t i = 0; i < object_field.length(); i++)
		object_field[i].extractRadiusRec(radius);
}

template <class Def>
template <class C>
void	Geometry<Def>::extractAverageVisualEdgeLength(unsigned layer, C&length)	const
{
	length = 0;
	count_t count = 0;
	for (index_t i = 0; i < object_field.length(); i++)
		object_field[i].addVisualEdgeLengthSqrRec(layer,length,count);
	length = vsqrt(length);
	if (count)
		length /= (C)count;
}


template <class Def>
template <class C> C Geometry<Def>::extractRadiusAround(const TVec3<C>&center) const
{
	C radius = 0;
	for (index_t i = 0; i < object_field.length(); i++)
		object_field[i].extractRadiusRec(center,radius);
	return radius;
}

template <class Def>
template <class C> void Geometry<Def>::resolveWeight(C&weight)					const
{
	weight = 0;
	for (index_t i = 0; i < object_field.length(); i++)
		object_field[i].addWeight(weight);
}



template <class Def> void Geometry<Def>::updateRadius()
{
	extractRadius(radius);
}

template <class Def> void Geometry<Def>::setSize(count_t objects_, count_t materials_, count_t animators_, count_t connectors_)
{
	object_field.setSize(objects_);
	material_field.setSize(materials_);
	animator_field.setSize(animators_);
	connector_field.setSize(connectors_);
}

template <class Def> bool Geometry<Def>::isMember(SubGeometryA<Def>*obj) const
{
	if (object_field.owns(obj))
		return true;
	for (index_t i = 0; i < object_field.length(); i++)
		if (object_field[i].isMember(obj))
			return true;
	return false;
}

template <class Def> bool Geometry<Def>::isMember(TWheel<Def>*whl) const
{
	for (index_t i = 0; i < object_field.length(); i++)
		if (object_field[i].isMember(whl))
			return true;
	return false;
}

template <class Def> bool Geometry<Def>::isMember(TAccelerator<Def>*acc) const
{
	for (index_t i = 0; i < object_field.length(); i++)
		if (object_field[i].isMember(acc))
			return true;
	return false;
}

template <class Def> bool Geometry<Def>::checkIntegrity()
{
	error = "errors:";
	bool rs = true;
	for (index_t i = 0; i < material_field.length(); i++)
		for (index_t j = 0; j < material_field[i].data.object_field.length(); j++)
		{
			if (!isMember(material_field[i].data.object_field[j].target))
			{
				error += "\ntarget-link of object "+String(j)+" (material "+String(i)+") is broken";
				rs = false;
			}
			if (!material_field[i].data.object_field[j].validIndices())
			{
				error += "\nindices broken in object "+String(j)+" (material "+String(i)+")";
				rs = false;
			}
		}
	for (index_t i = 0; i < animator_field.length(); i++)
		if (!animator_field[i].valid(this))
		{
			error += "\nanimator "+String(i)+" is broken";
			rs = false;
		}
	for (index_t i = 0; i < object_field.length(); i++)
	{
		if (!object_field[i].checkIntegrity(this))
		{
			error += "\n"+object_field[i].error;
			rs = false;
		}
	}
	return rs;
}

template <class Def> const String& Geometry<Def>::errorStr()
{
	return error;
}


template <class Def> void Geometry<Def>::verifyIntegrity()
{
	if (!checkIntegrity())
		FATAL__(errorStr());
}

template <class Def> String Geometry<Def>::getLinkageString()
{
	String rs = "(0x"+IntToHex((int)system_link,8)+")\n ";
	for (index_t i = 0; i < object_field.length(); i++)
		object_field[i].defLinkageStr(rs," ");
	return rs;
}

template <class Def> void Geometry<Def>::makeSimpleObject(const Float*vertex,count_t vertices,const Index*index,count_t indices)
{
	makeSimpleObject(vertex,vertices,0,index,indices);
}

template <class Def> void Geometry<Def>::makeSimpleObject(const Float*vertex,count_t vertices,count_t layers, const Index*index,count_t indices)
{
	clear();
	setSize(1,1,0,0);
	material_field[0].data.object_field.setSize(1);
	material_field[0].data.coord_layers = (unsigned)(layers);
	RenderObjectA<Def>&robj = material_field[0].data.object_field[0];
	ASSERT__(object_field.length()==1);
	robj.target = object_field.pointer();
	ASSERT_LESS__(layers,0x10000);
	robj.vpool.setSize(vertices,(UINT16)layers);
	
	robj.vpool.vdata.copyFrom(vertex);
	robj.detail=0;
	robj.ipool.idata.setSize(indices);
	robj.ipool.triangles = unsigned(indices/3);
	robj.ipool.idata.copyFrom(index);
	object_field[0].vs_hull_field.setSize(1);
	typedef Mesh<typename SubGeometryA<Def>::VsDef>	VsMesh;
	VsMesh	&obj = object_field[0].vs_hull_field[0];
	obj.vertex_field.setSize(vertices);
	obj.triangle_field.setSize(indices/3);
	obj.edge_field.free();
	obj.quad_field.free();
	UINT32 band = UINT32(3+3+layers*2);
	for (index_t i = 0; i < vertices; i++)
	{
		Vec::clear(obj.vertex_field[i].position);
		copy3(vertex+i*band,obj.vertex_field[i].position.v);
		obj.vertex_field[i].index = i;
		obj.vertex_field[i].marked = false;
	}
	for (index_t i = 0; i < indices/3; i++)
	{
		obj.triangle_field[i].vertex[0] = obj.vertex_field+index[i*3];
		obj.triangle_field[i].vertex[1] = obj.vertex_field+index[i*3+1];
		obj.triangle_field[i].vertex[2] = obj.vertex_field+index[i*3+2];
		obj.triangle_field[i].index = i;
		obj.triangle_field[i].marked = false;
	}
	ASSERT1__(obj.valid(),obj.errorStr());
	material_field[0].info.layer_field.setSize(layers);
	for (index_t i = 0; i < layers; i++)
	{
		material_field[0].info.layer_field[i].combiner = 0x2100;
		material_field[0].info.layer_field[i].source = NULL;
	}
	
	
	object_field[0].name = str2name("child");
	Mat::eye(object_field[0].meta.system);
	Vec::clear(object_field[0].meta.center);
	object_field[0].meta.radius = 1;
	object_field[0].meta.volume = 1;
	object_field[0].meta.density = 1;
	object_field[0].extractShortestVisualEdgeLength(0,object_field[0].meta.shortest_edge_length);
	Mat::eye(object_field[0].path);
	object_field[0].system_link = &object_field[0].path;
	robj.tname = object_field[0].name;
	
	//verifyIntegrity();
}

template <class Def>
	void	Geometry<Def>::makeFromConstructor(const Constructor<Def>&ctr)
	{
		clear();
		setSize(ctr.countObjects(),1,0,0);
		ASSERT__(object_field.length()==ctr.countObjects());
		material_field[0].data.object_field.setSize(ctr.countObjects());
		material_field[0].data.coord_layers = (unsigned)(ctr.countTextureLayers());
		for (index_t i = 0; i < ctr.countObjects(); i++)
		{
			RenderObjectA<Def>&robj = material_field[0].data.object_field[i];
			robj.target = object_field.pointer() + i;
			ASSERT_LESS__(ctr.countTextureLayers(),0x10000);
			const typename Constructor<Def>::Object&cobj = ctr.getObject(i);
			robj.vpool.setSize(ctr.countVertices(),(UINT16)ctr.countTextureLayers(),ctr.getVertexFlags());
	
			robj.vpool.vdata.copyFrom(cobj.getVertices());
			robj.detail=0;
			robj.ipool.idata.setSize(cobj.countIndices());
			robj.ipool.triangles = unsigned(cobj.countTriangles());
			robj.ipool.idata.copyFrom(cobj.getIndices());
			object_field[i].vs_hull_field.setSize(1);
			typedef Mesh<typename SubGeometryA<Def>::VsDef>	VsMesh;
			VsMesh	&obj = object_field[i].vs_hull_field[0];
			obj.vertex_field.setSize(cobj.countVertices());
			obj.triangle_field.setSize(cobj.countIndices()/3);
			obj.edge_field.free();
			obj.quad_field.free();
			UINT32 band = cobj.getVertexSize();
			const count_t vertices = cobj.countVertices();
			const Float*vertex = cobj.getVertices();
			for (index_t i = 0; i < vertices; i++)
			{
				Vec::clear(obj.vertex_field[i].position);
				copy3(vertex+i*band,obj.vertex_field[i].position.v);
				obj.vertex_field[i].index = i;
				obj.vertex_field[i].marked = false;
			}
			const count_t triangles = cobj.countTriangles();
			const Index*index = cobj.getIndices();
			for (index_t i = 0; i < triangles; i++)
			{
				obj.triangle_field[i].vertex[0] = obj.vertex_field+index[i*3];
				obj.triangle_field[i].vertex[1] = obj.vertex_field+index[i*3+1];
				obj.triangle_field[i].vertex[2] = obj.vertex_field+index[i*3+2];
				obj.triangle_field[i].index = i;
				obj.triangle_field[i].marked = false;
			}
			ASSERT1__(obj.valid(),obj.errorStr());

			object_field[i].name = str2name("child"+String(i));
			Mat::eye(object_field[i].meta.system);
			Vec::clear(object_field[i].meta.center);
			object_field[i].meta.radius = 1;
			object_field[i].meta.volume = 1;
			object_field[i].meta.density = 1;
			object_field[i].extractShortestVisualEdgeLength(0,object_field[i].meta.shortest_edge_length);
			Mat::eye(object_field[i].path);
			object_field[i].system_link = &object_field[i].path;
			robj.tname = object_field[i].name;
		}
		material_field[0].info.layer_field.setSize(ctr.countTextureLayers());
		for (index_t i = 0; i < ctr.countTextureLayers(); i++)
		{
			material_field[0].info.layer_field[i].combiner = 0x2100;
			material_field[0].info.layer_field[i].source = NULL;
		}
	
	

	}



template <class Def> template <typename T0>
	void	Geometry<Def>::makeBox(const TBox<T0>&b)
	{
		const T0*l = b.lower.v,
				*u = b.upper.v;
		Float	vertex_path[] = {
								l[0],l[1],l[2],	-1,0,0,
								l[0],l[1],u[2],	-1,0,0,
								l[0],u[1],u[2],	-1,0,0,
								l[0],u[1],l[2],	-1,0,0,
								
								u[0],l[1],l[2],	0,0,-1,
								l[0],l[1],l[2],	0,0,-1,
								l[0],u[1],l[2],	0,0,-1,
								u[0],u[1],l[2],	0,0,-1,
	
								u[0],l[1],u[2], 1,0,0,
								u[0],l[1],l[2],	1,0,0,
								u[0],u[1],l[2],	1,0,0,
								u[0],u[1],u[2],	1,0,0,

								l[0],l[1],u[2],	0,0,1,
								u[0],l[1],u[2],	0,0,1,
								u[0],u[1],u[2],	0,0,1,
								l[0],u[1],u[2],	0,0,1,

								l[0],l[1],u[2],	0,-1,0,
								l[0],l[1],l[2],	0,-1,0,
								u[0],l[1],l[2],	0,-1,0,
								u[0],l[1],u[2],	0,-1,0,
								
								u[0],u[1],u[2],	0,1,0,
								u[0],u[1],l[2],	0,1,0,
								l[0],u[1],l[2],	0,1,0,
								l[0],u[1],u[2],	0,1,0};
		UINT32 index_path[36];
		for (BYTE k = 0; k < 6; k++)
		{
			index_path[k*2*3] = k*4;
			index_path[k*2*3+1] = k*4+1;
			index_path[k*2*3+2] = k*4+2;
			index_path[(k*2+1)*3] = k*4;
			index_path[(k*2+1)*3+1] = k*4+2;
			index_path[(k*2+1)*3+2] = k*4+3;
		}
		makeSimpleObject(vertex_path,ARRAYSIZE(vertex_path)/6,index_path,ARRAYSIZE(index_path));
		/*float r;
		extractRadius(r);
		if (r > 10000)
			ErrMessage("bad box");*/
					
	}

template <class Def>
	void	Geometry<Def>::makePlane(Float width, Float depth, count_t x_segments, count_t z_segments)
	{
		if (!x_segments || !z_segments)
			return;
		Array<Float>	vertex_path((x_segments+1)*(z_segments+1)*6);
		for (index_t x = 0; x < x_segments+1; x++)
			for (index_t z = 0; z < z_segments+1; z++)
			{
				Float*vtx = vertex_path+((x_segments+1)*z+x)*6;
				Vec::def(Vec::ref3(vtx),((float)x/x_segments-0.5)*width,0,((float)z/z_segments-0.5)*depth);
				Vec::def(Vec::ref3(vtx+3),0,1,0);
			}
		
		Array<UINT32>	index_path((x_segments)*(z_segments)*6);
		UINT32*at = index_path.pointer();
		for (index_t x = 0; x < x_segments; x++)
			for (index_t z = 0; z < z_segments; z++)
			{
				*at++ = UINT32(z*(x_segments+1)+x+1);
				*at++ = UINT32(z*(x_segments+1)+x);
				*at++ = UINT32((z+1)*(x_segments+1)+x+1);
			
				*at++ = UINT32((z+1)*(x_segments+1)+x+1);
				*at++ = UINT32(z*(x_segments+1)+x);
				*at++ = UINT32((z+1)*(x_segments+1)+x);
			}
		ASSERT_CONCLUSION(index_path,at);
		
		makeSimpleObject(vertex_path.pointer(),vertex_path.length()/6,index_path.pointer(),index_path.length());
	}

template <class Def>	
	void	Geometry<Def>::makeBox(Float lower_x, Float lower_y, Float lower_z, Float upper_x, Float upper_y, Float upper_z)
	{
		TBox<Float> box = { lower_x, lower_y, lower_z,
							upper_x, upper_y, upper_z};
		makeBox(box);
	}

template <class Def> 
	void	Geometry<Def>::makeSphere(Float radius, count_t iterations)
	{
									
		TriangleMesh<float>	tobj;
		_oSphere(tobj,radius,iterations);

		Sorted<List::Vector<NormalPoint<float> >,OperatorSort>	point_field;
		Array<NormalPoint<float>*> map(tobj.vertices);
		for (index_t i = 0; i < tobj.vertices; i++)
		{
			NormalPoint<float> np(Vec::ref3(tobj.data+i*tobj.band),Vec::ref3(tobj.data+i*tobj.band+3));
			map[i] = point_field.add(np);
		}
		Array<float>	vfield(point_field.count()*6);
		for (index_t i = 0; i < point_field.count(); i++)
		{
			point_field[i]->index = i;
			Vec::ref3(vfield+i*6) = *point_field[i];
			Vec::ref3(vfield+i*6+3) = point_field[i]->normal;
		}
		
		Array<UINT32>	ifield(tobj.vertices);
		for (index_t i = 0; i < tobj.vertices; i++)
			ifield[i] = UINT32(map[i]->index);
		
		makeSimpleObject(vfield.pointer(),point_field.count(),ifield.pointer(),ifield.length());
	}







template <class Def>
	void		Geometry<Def>::adoptInstance(AnimatableInstance<Def>&instance,bool copy_root_system/*=false*/)
	{
		instance.target = this;
		if (copy_root_system)
			instance.matrix = root_system.matrix;
		else
			instance.loadIdentity(false);
		PointerTable<double*>	map;
		PointerTable<TAnimationStatus*>	map2;
		instance.child_field.setSize(object_field.length());
		for (index_t i = 0; i < object_field.length(); i++)
		{
			object_field[i].toInstance(instance.child_field[i]);
			instance.child_field[i].mapEntities(map,map2);
		}
		for (index_t i = 0; i < object_field.length(); i++)
			instance.child_field[i].linkEntities(map,map2);
		
		instance.animator_field.setSize(animator_field.length());
		for (index_t i = 0; i < animator_field.length(); i++)
		{
			instance.animator_field[i].build(animator_field+i);
			instance.animator_field[i].link(map,map2);
		}
		clearFlags();
		flagAnimationTargets();
		instance.updateAnimationMap();
		
		instance.update();
		CGS_MSG(__toString(instance.child_field.last().path.v));
	}




template <class Def>
	void		Geometry<Def>::adoptInstance(StaticInstance<Def>&instance,bool copy_root_system/*=false*/)
	{
		instance.target = this;
		if (copy_root_system)
			instance.matrix = root_system.matrix;
		else
			instance.loadIdentity(false);
		instance.child_field.setSize(object_field.length());
		for (index_t i = 0; i < object_field.length(); i++)
			object_field[i].toInstance(instance.child_field[i]);
		instance.update();
	}



template <class Def> shared_ptr<StaticInstance<Def> > Geometry<Def>::createInstance(bool copy_root_system/*=false*/)
{
	shared_ptr<StaticInstance<Def> > result(new StaticInstance<Def>());
	adoptInstance(*result, copy_root_system);
	return result;
}

template <class Def> shared_ptr<AnimatableInstance<Def> > Geometry<Def>::createAnimatableInstance(bool copy_root_system/*=false*/)
{
	shared_ptr<AnimatableInstance<Def> > result(new AnimatableInstance<Def>());
	adoptInstance(*result,copy_root_system);
	return result;
}


template <class Def> void		Geometry<Def>::resetLinkage()
{
	system_link = &root_system.matrix;
	for (index_t i = 0; i < object_field.length(); i++)
		object_field[i].resetLinkageRec();
}

template <class Def> void		Geometry<Def>::resetSystems()
{
	Mat::eye(root_system.matrix);
	for (index_t i = 0; i < object_field.length(); i++)
	{
		object_field[i].resetLinkageRec();
		object_field[i].setSystemRec(root_system.matrix);
	}
}


template <class Def> count_t Geometry<Def>::countVertices(unsigned detail)
{
	count_t rs = 0;
	for (index_t i = 0; i < object_field.length(); i++)
		rs+=object_field[i].countVertices(detail);
	return rs;
}

template <class Def> GeometryInstance<Def>::GeometryInstance():System<typename Def::SystemType>(this),target(NULL)
{}


template <class Def>
	void			StaticInstance<Def>::onSystemChange()
	{
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].updatePath(Super::matrix);
	}




template <class Def> StaticInstance<Def>::StaticInstance()
{}


template <class Def> template <typename T0, typename T1, typename T2>
	bool	StaticInstance<Def>::detectOpticalIntersection(const TVec3<T0>&b, const TVec3<T1>&d, T2&distance)	const
	{
		for (index_t i = 0; i < child_field.length(); i++)
			if (child_field[i].detectOpticalIntersection(b,d,distance))
				return true;
		return false;
	}


template <class Def> template <typename T0, typename T1, typename T2>
	bool	StaticSubInstanceA<Def>::detectOpticalIntersection(const TVec3<T0>&b_, const TVec3<T1>&d_, T2&distance)	const
	{
		if (!SubGeometryInstance<Def>::target)
			return false;
		if (SubGeometryInstance<Def>::target->vs_hull_field.length())
		{
			TMatrix4<typename Def::FloatType>	inverse;
			TVec3<typename Def::FloatType>		d,b;
			Mat::invertSystem(SubGeometryInstance<Def>::path,inverse);
			Mat::transform(inverse,b_,b);
			Mat::rotate(inverse,d_,d);
			
			typedef Mesh<typename SubGeometryA<Def>::VsDef>	VsMesh;
			const VsMesh	&hull = SubGeometryInstance<Def>::target->vs_hull_field.first();
			for (index_t i = 0; i < hull.triangle_field.length(); i++)
				if (Obj::detectOpticalIntersection(hull.triangle_field[i].v0->position,hull.triangle_field[i].v1->position,hull.triangle_field[i].v2->position,b,d,distance))
					return true;
			for (index_t i = 0; i < hull.quad_field.length(); i++)
				if (Obj::detectOpticalIntersection(hull.quad_field[i].v0->position,hull.quad_field[i].v1->position,hull.quad_field[i].v2->position,b,d,distance)
					||
					Obj::detectOpticalIntersection(hull.quad_field[i].v0->position,hull.quad_field[i].v2->position,hull.quad_field[i].v3->position,b,d,distance))
					return true;
		}
		for (index_t i = 0; i < child_field.length(); i++)
			if (child_field[i].detectOpticalIntersection(b_,d_,distance))
				return true;
		return false;
	}


template <class Def> void StaticInstance<Def>::update()
{
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].updatePath(Super::matrix);
}

template <class Def> void StaticInstance<Def>::restoreFromGeometry()
{
	if (GeometryInstance<Def>::target)
	{
		Super::matrix = GeometryInstance<Def>::target->root_system.matrix;
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].restoreFromGeometry();
		update();
	}
}


template <class Def> void StaticInstance<Def>::link()
{
	ASSERT_NOT_NULL__(GeometryInstance<Def>::target);
	TMatrix4<typename Def::SystemType>&m = System::matrix;
	GeometryInstance<Def>::target->system_link = &m;
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].linkObjects();
}



template <class Def>
	AnimatorInstanceA<Def>::AnimatorInstanceA():target(NULL),progress(0),active(false)
	{}
	
template <class Def>
	void	AnimatorInstanceA<Def>::adoptData(AnimatorInstanceA<Def>&other)
	{
		if (this == &other)
			return;
		target = other.target;
		progress = other.progress;
		active = other.active;
		obj_animator_field.adoptData(other.obj_animator_field);
		whl_animator_field.adoptData(other.whl_animator_field);
		acc_animator_field.adoptData(other.acc_animator_field);
	}

template <class Def>
	void	AnimatorInstanceA<Def>::build(AnimatorA<Def>*target_)
	{
		target = target_;
		if (!target)
		{
			obj_animator_field.free();
			whl_animator_field.free();
			acc_animator_field.free();
			return;
		}
		obj_animator_field.setSize(target->obj_trace_field.length());
		whl_animator_field.setSize(target->whl_trace_field.length());
		acc_animator_field.setSize(target->acc_trace_field.length());
		
		for (index_t i = 0; i < obj_animator_field.length(); i++)
		{
			obj_animator_field[i].target = target->obj_trace_field[i].target;
			obj_animator_field[i].target_trace = target->obj_trace_field+i;
			Vec::clear(obj_animator_field[i].current.translation);
			Vec::clear(obj_animator_field[i].current.rotation);
			obj_animator_field[i].out_target = NULL;
		}
		for (index_t i = 0; i < whl_animator_field.length(); i++)
		{
			whl_animator_field[i].target = target->whl_trace_field[i].target;
			whl_animator_field[i].target_trace = target->whl_trace_field+i;
			whl_animator_field[i].current_acceleration = 0;
			whl_animator_field[i].current_brake_status = 0;
			whl_animator_field[i].out_target = NULL;
		}
	
		for (index_t i = 0; i < acc_animator_field.length(); i++)
		{
			acc_animator_field[i].target_trace = target->acc_trace_field+i;
			acc_animator_field[i].target = target->acc_trace_field[i].target;
			acc_animator_field[i].current_acceleration = 0;
			acc_animator_field[i].out_target = NULL;
		}
	
	
	}
	
template <class Def>
	void	AnimatorInstanceA<Def>::link(const PointerTable<double*>&map,const PointerTable<TAnimationStatus*>&map2)
	{
		for (index_t i = 0; i < obj_animator_field.length(); i++)
		{
			ASSERT__(map2.query(obj_animator_field[i].target,obj_animator_field[i].out_target));
		}
		for (index_t i = 0; i < whl_animator_field.length(); i++)
		{
			ASSERT__(map.query(whl_animator_field[i].target,whl_animator_field[i].out_target));
		}
		for (index_t i = 0; i < acc_animator_field.length(); i++)
		{
			ASSERT__(map.query(acc_animator_field[i].target,acc_animator_field[i].out_target));
		}
	}
	
template <class Def>
	void	AnimatorInstanceA<Def>::update()
	{
		for (index_t i = 0; i < obj_animator_field.length(); i++)
		{
			double field[6];
			obj_animator_field[i].target_trace->current(progress,field);
			obj_animator_field[i].current.translation = Vec::ref3(field);
			obj_animator_field[i].current.rotation = Vec::ref3(field+3);
			//if (_dot(obj_animator_field[i].current+3))
				//CGS_MSG("Adding "+_toString(obj_animator_field[i].current+3)+" to target, which is "+_toString(obj_animator_field[i].out_target+3));
			Vec::add(obj_animator_field[i].out_target->translation,obj_animator_field[i].current.translation);
			Vec::add(obj_animator_field[i].out_target->rotation,obj_animator_field[i].current.rotation);
		}
		for (index_t i = 0; i < whl_animator_field.length(); i++)
		{
			whl_animator_field[i].target_trace->current(progress,whl_animator_field[i].current);
			Vec::add(Vec::ref2(whl_animator_field[i].out_target),Vec::ref2(whl_animator_field[i].current));
		}
		for (index_t i = 0; i < acc_animator_field.length(); i++)
		{
			acc_animator_field[i].target_trace->current(progress,&acc_animator_field[i].current_acceleration);
			(*acc_animator_field[i].out_target) += acc_animator_field[i].current_acceleration;
		}
	}



template <class Def>
	void			AnimatableInstance<Def>::onSystemChange()
	{
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].updatePath(Super::matrix);
	}
	
template <class Def>
	void			AnimatableInstance<Def>::updateAnimationMap()
	{
		animation_map.sub_animator_field.reset();
		animation_map.animated_sub_field.reset();
		animation_map.animated_wheel_status_field.reset();
		animation_map.animated_accel_status_field.reset();
		
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].fillAnimationMap(animation_map);
		
		animation_map.sub_animator_field.compact();
		animation_map.animated_sub_field.compact();
		animation_map.animated_wheel_status_field.compact();
		animation_map.animated_accel_status_field.compact();
		
	}
	
	

	
template <class Def>
	AnimatorInstanceA<Def>*				AnimatableInstance<Def>::findAnimator(const String&name)
	{
		const Path*p = target->animator_table.queryPointer(name);
		if (!p)
			return NULL;
		ASSERT_EQUAL__(p->size(),1);
		ASSERT_LESS__(p->first(),animator_field.length());
		return animator_field + p->first();
	}

template <class Def>
	AnimatorInstanceA<Def>*				AnimatableInstance<Def>::findInstanceOf(AnimatorA<Def>*animator)
	{
		if (!GeometryInstance<Def>::target || !animator)
			return NULL;
		if (GeometryInstance<Def>::target->animator_field.owns(animator))
			return animator_field+GeometryInstance<Def>::target->animator_field.indexOf(animator);
		for (index_t i = 0; i < child_field.length(); i++)
			if (AnimatorInstanceA<Def>*rs = child_field[i].findInstanceOf(animator))
				return rs;
		return NULL;
	}


template <class Def> AnimatableInstance<Def>::AnimatableInstance()
{}	

template <class Def>
	void		AnimatableInstance<Def>::update()
	{
		if (!GeometryInstance<Def>::target)
			return;
		for (index_t i = 0; i < animation_map.animated_sub_field.length(); i++)
		{
			Vec::clear(animation_map.animated_sub_field[i]->animation_status.translation);
			Vec::clear(animation_map.animated_sub_field[i]->animation_status.rotation);
		}
		for (index_t i = 0; i < animation_map.animated_wheel_status_field.length(); i++)
			Vec::clear(Vec::ref2(animation_map.animated_wheel_status_field[i]));
		for (index_t i = 0; i < animation_map.animated_accel_status_field.length(); i++)
			(*animation_map.animated_accel_status_field[i])=0;
		
		for (index_t i = 0; i < animator_field.length(); i++)
			animator_field[i].update();
		for (index_t i = 0; i < animation_map.sub_animator_field.length(); i++)
			animation_map.sub_animator_field[i]->update();

		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].updatePath(Super::matrix);
	}
	
template <class Def>
	void		AnimatableInstance<Def>::link()
	{
		ASSERT_NOT_NULL__(GeometryInstance<Def>::target);
		GeometryInstance<Def>::target->system_link = Super::getMatrix();
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].linkObjects();
	}
	
template <class Def>
	double*		AnimatableInstance<Def>::findStatusOfWheel(const ArrayData<index_t>&path)
	{
		if (!path.length() || path.first() >= child_field.length())
			return NULL;
		AnimatableSubInstanceA<Def>*child = child_field+path.first();
		for (index_t i = 1; i+1 < path.length(); i++)
			if (path[i] >= child->child_field.length())
				return NULL;
			else
				child = child->child_field+path[i];
		if (path.last()*2 >= child->wheel_status.length())
			return NULL;
		return child->wheel_status+path.last()*2;
	}
	
template <class Def>
	double*		AnimatableInstance<Def>::findStatusOfAccelerator(const ArrayData<index_t>&path)
	{
		if (!path.length() || path.first() >= child_field.length())
			return NULL;
		AnimatableSubInstanceA<Def>*child = child_field+path.first();
		for (index_t i = 1; i+1 < path.length(); i++)
			if (path[i] >= child->child_field.length())
				return NULL;
			else
				child = child->child_field+path[i];
		if (path.last() >= child->accelerator_status.length())
			return NULL;
		return child->accelerator_status+path.last();
	}

template <class Def>
	AnimatorInstanceA<Def>*		AnimatableInstance<Def>::findRotationAnimatorOfWheel(const ArrayData<index_t>&path)
	{
		if (!path.length() || path.first() >= child_field.length())
			return NULL;
		AnimatableSubInstanceA<Def>*child = child_field+path.first();
		for (index_t i = 1; i+1 < path.length(); i++)
			if (path[i] >= child->child_field.length())
				return NULL;
			else
				child = child->child_field+path[i];
		if (path.last() >= child->wheel_rotation_animators.count())
			return NULL;
		return child->wheel_rotation_animators+path.last();
	}

template <class Def>
	AnimatorInstanceA<Def>*		AnimatableInstance<Def>::findSuspensionAnimatorOfWheel(const ArrayData<index_t>&path)
	{
		if (!path.length() || path.first() >= child_field.length())
			return NULL;
		AnimatableSubInstanceA<Def>*child = child_field+path.first();
		for (index_t i = 1; i+1 < path.length(); i++)
			if (path[i] >= child->child_field.length())
				return NULL;
			else
				child = child->child_field+path[i];
		if (path.last() >= child->wheel_suspension_animators.count())
			return NULL;
		return child->wheel_suspension_animators+path.last();
	}

template <class Def>
	double*		AnimatableInstance<Def>::findStatusOf(TWheel<Def>*wheel)
	{
		if (!wheel)
			return NULL;
		for (index_t i = 0; i < child_field.length(); i++)
		{
			if (double*rs = child_field[i].findStatusOf(wheel))
				return rs;
		}
		return NULL;
	}
	
template <class Def>
	void		AnimatableInstance<Def>::restoreFromGeometry(bool reset_animations)
	{
		if (GeometryInstance<Def>::target)
		{
			if (reset_animations)
			{
				for (index_t i = 0; i < animator_field.length(); i++)
					animator_field[i].progress = 0;
			}
			Mat::copy(GeometryInstance<Def>::target->root_system.matrix,Super::matrix);
			for (index_t i = 0; i < child_field.length(); i++)
				child_field[i].restoreFromGeometry(reset_animations);
			update();
		}
	}


	
	

template <class Def> SubGeometryInstance<Def>::SubGeometryInstance():target(NULL),system(Matrix<typename Def::SystemType>::eye4),path(Matrix<typename Def::SystemType>::eye4)
{}


template <class Def> StaticSubInstanceA<Def>::StaticSubInstanceA()
{}



template <class Def> void StaticSubInstanceA<Def>::linkObjects()
{
	if (SubGeometryInstance<Def>::target)
	{
		TMatrix4<typename Def::SystemType>&m = SubGeometryInstance<Def>::path;
		SubGeometryInstance<Def>::target->system_link = &m;
	}
	else
		FATAL__("target not found");
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].linkObjects();
}

template <class Def> void StaticSubInstanceA<Def>::updatePath(const TMatrix4<typename Def::SystemType>&parent)
{
	Mat::transformSystem(parent,SubGeometryInstance<Def>::system,SubGeometryInstance<Def>::path);
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].updatePath(SubGeometryInstance<Def>::path);
}

template <class Def> void StaticSubInstanceA<Def>::restoreFromGeometry()
{
	if (SubGeometryInstance<Def>::target)
		system = SubGeometryInstance<Def>::target->meta.system;
	else
		FATAL__("target expected");
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].restoreFromGeometry();
}


template <class Def> AnimatableSubInstanceA<Def>::AnimatableSubInstanceA()
{}


template <class Def>
	AnimatorInstanceA<Def>*				AnimatableSubInstanceA<Def>::findInstanceOf(AnimatorA<Def>*animator)
	{
		for (index_t i = 0; i < wheel_rotation_animators.count(); i++)
			if (wheel_rotation_animators[i].target == animator)
				return wheel_rotation_animators+i;
		/*for (index_t i = 0; i < construct_rotation_animators.count(); i++)
			if (construct_rotation_animators[i].target == animator)
				return construct_rotation_animators+i;*/
		for (index_t i = 0; i < wheel_suspension_animators.count(); i++)
			if (wheel_suspension_animators[i].target == animator)
				return wheel_suspension_animators+i;
	
	
		for (index_t i = 0; i < child_field.length(); i++)
			if (AnimatorInstanceA<Def>*rs = child_field[i].findInstanceOf(animator))
				return rs;
		return NULL;
	}
	
template <class Def>
	double*		AnimatableSubInstanceA<Def>::findStatusOf(TWheel<Def>*wheel)
	{
		if (SubGeometryInstance<Def>::target->wheel_field.owns(wheel))
		{
			return wheel_status+2*SubGeometryInstance<Def>::target->wheel_field.indexOf(wheel);
		}
		for (index_t i = 0; i < child_field.length(); i++)
		{
			if (double*rs = child_field[i].findStatusOf(wheel))
				return rs;
		}
		return NULL;
	}


template <class Def> void AnimatableSubInstanceA<Def>::linkObjects()
{
	if (SubGeometryInstance<Def>::target)
		SubGeometryInstance<Def>::target->system_link = Super::getPath();
	else
		FATAL__("target not found");
	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].linkObjects();
}

#ifndef __CUDACC__

template <class Def> void AnimatableSubInstanceA<Def>::updatePath(const TMatrix4<typename Def::SystemType>&parent)
{
	TVec3<>	vector;
	float	angle;
	TMatrix3<>	rotation_matrix;
	TMatrix4<>	final_matrix;

	angle = Vec::length(animation_status.rotation);

	if (vabs(angle)>getError<double>())
	{
		ASSERT__(target->meta.flags&AnimationTargetFlag);
		Vec::div(animation_status.rotation,angle,vector);
		Mat::rotationMatrix(angle,vector,rotation_matrix);
		Mat::mult(rotation_matrix,system.x.xyz,animated_system.x.xyz);
		Mat::mult(rotation_matrix,system.y.xyz,animated_system.y.xyz);
		Mat::mult(rotation_matrix,system.z.xyz,animated_system.z.xyz);
		//CGS_MSG("Angle is "+String(angle)+". vector is "+_toString(vector)+". base vector was "+_toString(animation_status+3));
	}
	else
		Mat::copyOrientation(system,animated_system);
	Vec::add(system.w.xyz,animation_status.translation,animated_system.w.xyz);	//translation

	Mat::resetBottomRow(animated_system);
	//copy3(animation_status,final_matrix+12);
	/*if (_dot(animation_status))
		CGS_MSG("Translation is "+_toString(final_matrix+12));*/
	
	Mat::transformSystem(parent,animated_system,path);

	for (index_t i = 0; i < child_field.length(); i++)
		child_field[i].updatePath(path);
}

template <class Def>
	void AnimatableSubInstanceA<Def>::restoreFromGeometry(bool reset_animations)
	{
		if (reset_animations)
		{
			for (index_t i = 0; i < wheel_rotation_animators.length(); i++)
				wheel_rotation_animators[i].progress = 0;
			for (index_t i = 0; i < wheel_suspension_animators.length(); i++)
				wheel_suspension_animators[i].progress = 0;
			/*for (index_t i = 0; i < construct_rotation_animators.length(); i++)
				construct_rotation_animators[i].progress = 0;*/
		}
		if (target)
			system = target->meta.system;
		else
			FATAL__("target expected");
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].restoreFromGeometry(reset_animations);
	}
#endif

template <class Def>
	void AnimatableSubInstanceA<Def>::mapEntities(PointerTable<double*>&map, PointerTable<TAnimationStatus*>&map2)
	{
		map2.set(target,&animation_status);
		for (index_t i = 0; i < target->wheel_field.length(); i++)
			map.set(target->wheel_field+i,wheel_status+2*i);
		for (index_t i = 0; i < target->accelerator_field.length(); i++)
			map.set(target->accelerator_field+i,accelerator_status+i);
	
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].mapEntities(map,map2);
	}
	
template <class Def>
	void AnimatableSubInstanceA<Def>::linkEntities(const PointerTable<double*>&map, const PointerTable<TAnimationStatus*>&map2)
	{
		for (index_t i = 0; i < wheel_rotation_animators.length(); i++)
			wheel_rotation_animators[i].link(map,map2);
		for (index_t i = 0; i < wheel_suspension_animators.length(); i++)
			wheel_suspension_animators[i].link(map,map2);
		/*for (index_t i = 0; i < construct_rotation_animators.length(); i++)
			construct_rotation_animators[i].link(map,map2);*/

		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].linkEntities(map,map2);
	}

#ifndef __CUDACC__

template <class Def>
	void AnimatableSubInstanceA<Def>::	fillAnimationMap(typename AnimatableInstance<Def>::TAnimationMap&map)
	{
		if (target->meta.flags & AnimationTargetFlag)
			map.animated_sub_field << this;
		map.sub_animator_field.appendAddresses(wheel_rotation_animators);
		map.sub_animator_field.appendAddresses(wheel_suspension_animators);
		//map.sub_animator_field.appendAddresses(construct_rotation_animators);
		
		for (index_t i = 0; i < target->wheel_field.length(); i++)
			if (target->wheel_field[i].flags & AnimationTargetFlag)
				map.animated_wheel_status_field << wheel_status+2*i;
		for (index_t i = 0; i < target->accelerator_field.length(); i++)
			if (target->accelerator_field[i].flags & AnimationTargetFlag)
				map.animated_accel_status_field << wheel_status+i;
		
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].fillAnimationMap(map);
	}
#endif
	
	
template <class Def>
	void AnimatableSubInstanceA<Def>::resetAnimationStatus()
	{
		_clear6(animation_status);
		wheel_status.fill(0);
		accelerator_status.fill(0);
	
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].resetAnimationStatus();
	}	
template <class Def>
	void AnimatableSubInstanceA<Def>::updateAnimations()
	{
		for (index_t i = 0; i < wheel_rotation_animators.length(); i++)
			wheel_rotation_animators[i].update();
		for (index_t i = 0; i < wheel_suspension_animators.length(); i++)
			wheel_suspension_animators[i].update();
		for (index_t i = 0; i < construct_rotation_animators.length(); i++)
			construct_rotation_animators[i].update();
	
		for (index_t i = 0; i < child_field.length(); i++)
			child_field[i].updateAnimations();
	}



template <class Def> AnimatorA<Def>::AnimatorA():name(0)
{}

template <class Def> 
	void	AnimatorA<Def>::revert()
	{
		for (index_t i = 0; i < obj_trace_field.length(); i++)
			obj_trace_field[i].revert();
		for (index_t i = 0; i < acc_trace_field.length(); i++)
			acc_trace_field[i].revert();
		for (index_t i = 0; i < whl_trace_field.length(); i++)
			whl_trace_field[i].revert();
	}


template <class Def> 
	double	AnimatorA<Def>::length()	const
	{
		double rs = 0;
		for (index_t i = 0; i < obj_trace_field.count(); i++)
			if (obj_trace_field[i].step_field.length())
				rs = vmax(rs,obj_trace_field[i].step_field.last().end);
		for (index_t i = 0; i < acc_trace_field.count(); i++)
			if (acc_trace_field[i].step_field.length())
				rs = vmax(rs,acc_trace_field[i].step_field.last().end);
		for (index_t i = 0; i < whl_trace_field.count(); i++)
			if (whl_trace_field[i].step_field.length())
				rs = vmax(rs,whl_trace_field[i].step_field.last().end);
		return rs;
	}

template <class Def>	template <class C>
	void	AnimatorA<Def>::scale(const C&factor)
	{
		for (index_t i = 0; i < obj_trace_field.count(); i++)
			for (index_t j = 0; j < obj_trace_field[i].step_field.length(); j++)
			{
				Vec::mult(Vec::ref3(obj_trace_field[i].step_field[j].range),factor);	//only affect translaction
				Vec::mult(Vec::ref3(obj_trace_field[i].step_field[j].slope0),factor);
				Vec::mult(Vec::ref3(obj_trace_field[i].step_field[j].slope1),factor);
			}
	}


template <class Def> template <class Obj, unsigned Band>
	void AnimatorA<Def>::storeTargetNames(ArrayData<TraceA<Obj,Band> >&field)
	{
		for (index_t i = 0; i < field.length(); i++)
			field[i].tname = field[i].target->name;
	}

template <class Def> void AnimatorA<Def>::storeTargetNames()
{
	storeTargetNames(obj_trace_field);
	storeTargetNames(acc_trace_field);
	storeTargetNames(whl_trace_field);
}

template <class Def> bool AnimatorA<Def>::valid(Geometry<Def>*structure)
{
	for (index_t i = 0; i < obj_trace_field.length(); i++)
		if (!structure->isMember(obj_trace_field[i].target))
			return false;
	for (index_t i = 0; i < acc_trace_field.length(); i++)
		if (!structure->isMember(acc_trace_field[i].target))
			return false;
	for (index_t i = 0; i < whl_trace_field.length(); i++)
		if (!structure->isMember(whl_trace_field[i].target))
			return false;
	return true;
}

template <class Def> String AnimatorA<Def>::toString(const String&intend)	const
{
	return "Animator (ObjecCTraces: "+String(obj_trace_field.length())+"; AcceleratorTraces: "+String(acc_trace_field.length())+"; WheelTraces: "+String(whl_trace_field.length())+")";
}

template <class Def> void AnimatorA<Def>::setSize(count_t obj_count, count_t acc_count, count_t whl_count)
{
	obj_trace_field.setSize(obj_count);
	acc_trace_field.setSize(acc_count);
	whl_trace_field.setSize(whl_count);
}


template <class Def>
template <class Def0> AnimatorA<Def>& AnimatorA<Def>::operator=(const AnimatorA<Def0>&source)
{
	name = source.name;
	obj_trace_field = source.obj_trace_field;
	acc_trace_field = source.acc_trace_field;
	whl_trace_field = source.whl_trace_field;
	return *this;
}

template <class Def> void AnimatorA<Def>::adoptData(AnimatorA<Def>&other)
{
	if (this == &other)
		return;
	name.adoptData(other.name);
	obj_trace_field.adoptData(other.obj_trace_field);
	acc_trace_field.adoptData(other.acc_trace_field);
	whl_trace_field.adoptData(other.whl_trace_field);
}


template <class T, unsigned B> TraceA<T,B>::TraceA():target(NULL)
{}

template <class T, unsigned B>
	void	TraceA<T,B>::revert()
	{
		step_field.revert();
		for (index_t i = 0; i < step_field.length(); i++)
		{
			VecUnroll<B>::mult(step_field[i].range,-1);
			VecUnroll<B>::mult(step_field[i].slope0,-1);
			VecUnroll<B>::mult(step_field[i].slope1,-1);
		}
	}
	
template <class T, unsigned B>
	void	TraceA<T,B>::adoptData(TraceA<T,B>&other)
	{
		if (this == &other)
			return;
		tname.adoptData(other.tname);
		target = other.target;
		step_field.adoptData(other.step_field);
	}
	
template <class T, unsigned B>
	void	TraceA<T,B>::calculateSlopes()
	{
		if (!step_field.length())
			return;
		//CGS_MSG("Auto generating slopes for trace of length "+String(step_field.length()));
		VecUnroll<B>::div(step_field.first().range,step_field.first().end-step_field.first().start,step_field.first().slope0);
		//CGS_MSG("Initial slope calculated at "+_toString(step_field.first().slope0,B));
		for (index_t i = 0; i < step_field.length(); i++)
		{
			double v0[B],v1[B],v2[B];
			if (i)
				VecUnroll<B>::div(step_field[i-1].range,step_field[i-1].end-step_field[i-1].start,v0);
			VecUnroll<B>::div(step_field[i].range,step_field[i].end-step_field[i].start,v1);
			if (i+1 < step_field.length())
				VecUnroll<B>::div(step_field[i+1].range,step_field[i+1].end-step_field[i+1].start,v2);
			if (i)
				VecUnroll<B>::center(v0,v1,step_field[i].slope0);
			if (i+1 < step_field.length())
				VecUnroll<B>::center(v1,v2,step_field[i].slope1);
		}
		VecUnroll<B>::div(step_field.last().range,step_field.last().end-step_field.last().start,step_field.last().slope1);
		//CGS_MSG("Final slope calculated at "+_toString(step_field.last().slope1,B));
	}


template <class T, unsigned B>
	void	TraceA<T,B>::current(const double&time, double out[B])	const
	{
		VecUnroll<B>::clear(out);
		if (!step_field.length() || time <= step_field.first().start)
			return;
	
		for (index_t i = 0; i < step_field.length(); i++)
		{
			//CGS_MSG("Processing step "+String(i)+"/"+String(step_field.length()));
			const TStep<B>&step = step_field[i];
			if (time < step.start)
				return;
			if (time > step.end)
			{
				//CGS_MSG("Exceeded. Skipping to next step (if any)");
				VecUnroll<B>::add(out,step.range);
				continue;
			}
			
			double	len = step.end-step.start,
					fc = (time-step.start)/(len),
					a[B],
					b[B],
					c[B];
					//s0[B],
					//s1[B];
/* 			CGS_MSG("start="+String(step.start));
			CGS_MSG("end="+String(step.end));
			CGS_MSG("fc="+String(fc)+" (time="+String(time)+")");
			CGS_MSG("slope0="+_toString(step.slope0,B));
			CGS_MSG("slope1="+_toString(step.slope1,B));
			CGS_MSG("range="+_toString(step.range,B)); */
			//_divV<double,double,double,B>(step.slope0,len,s0);
			//_divV<double,double,double,B>(step.slope1,len,s1);

			VecUnroll<B>::add(step.slope0,step.slope1,a);
			VecUnroll<B>::mad(a,step.range,-2);	//a = s0+s1-2*delta
			
			VecUnroll<B>::mult(step.range,3,b);
			VecUnroll<B>::sub(b,step.slope1);
			VecUnroll<B>::mad(b,step.slope0,-2);	//b = 3*delta-s1-2*s0
			
			VecUnroll<B>::copy(step.slope0,c);	//c = s0
		/* 	
			CGS_MSG("a="+_toString(a,B));
			CGS_MSG("b="+_toString(b,B));
			CGS_MSG("c="+_toString(c,B)); */
			
			
			VecUnroll<B>::mad(out,a,fc*fc*fc);
			VecUnroll<B>::mad(out,b,fc*fc);
			VecUnroll<B>::mad(out,c,fc);
			return;
		}
	}
	

template <class Def>
template <class T, unsigned B>
/*static*/	void AnimatorA<Def>::saveTraces(const ArrayData<TraceA<T,B> >&traces,RiffChunk*riff,const char*riff_name)
{
	for (index_t i = 0; i < traces.count(); i++)
	{
		ByteStream trace;
		trace << (UINT32)traces[i].target->name.length();
		trace.push(traces[i].target->name.c_str(),traces[i].target->name.length());
		double delta;
		for (index_t j = 0; j < traces[i].step_field.length(); j++)
		{
			delta = traces[i].step_field[j].end - traces[i].step_field[j].start;
			trace.push(delta);
			trace.push(traces[i].step_field[j].range,B);
			trace.push(traces[i].step_field[j].slope0,B);
			trace.push(traces[i].step_field[j].slope1,B);
		}
		riff->appendBlock(riff_name,trace.data(),trace.fillLevel());
	}
}



template <class Def>
template <class T, unsigned B>
void AnimatorA<Def>::loadTraces(ArrayData<TraceA<T,B> >&array,RiffFile&riff,const char*riff_name0, const char*riff_name1)
{
	index_t format = 0;
	const char* riff_name = riff_name0;
	bool begin = false;
	count_t cnt = 0;
	if (riff.findFirst(riff_name))
	{
		begin = true;
	}
	else
		if (riff.findFirst(riff_name1))
		{
			format = 1;
			begin = true;
			riff_name = riff_name1;
		}
	if (begin)
		do
		{
			cnt++;
		}
		while (riff.findNext(riff_name));
	array.setSize(cnt);
	index_t at = 0;
	if (begin)
	if (riff.findFirst(riff_name))
		do
		{
			TraceA<T,B>&trace = array[at++];
			switch (format)
			{
				case 0:
				{
					if (riff.getSize() < sizeof(name64_t))
						continue;
					trace.step_field.setSize((riff.getSize()-sizeof(name64_t))/((1+B*3)*sizeof(double)));
					riff.openStream();
						name64_t	name64;
						riff.stream(name64);
						trace.tname = name2str(name64);
						trace.tname.trimThis();

						double time(0),len;
						for (index_t i = 0; i < trace.step_field.length(); i++)
						{
							trace.step_field[i].start = time;
							riff.stream(len);
							time+=len;
							trace.step_field[i].end = time;
							riff.stream(trace.step_field[i].range,B);
							riff.stream(trace.step_field[i].slope0,B);
							riff.stream(trace.step_field[i].slope1,B);
						}
					riff.closeStream();
				}
				break;
				case 1:
				{
					riff.openStream();
						UINT32 name_length;
						if (!riff.stream(name_length) || !riff.canStreamBytes(name_length))
						{
							riff.closeStream();
							continue;
						}
						trace.tname.setLength(name_length);
						riff.stream(trace.tname.mutablePointer(),name_length);
						trace.step_field.setSize((riff.getSize()-riff.streamTell())/((1+B*3)*sizeof(double)));
						double time(0),len;
						for (index_t i = 0; i < trace.step_field.length(); i++)
						{
							trace.step_field[i].start = time;
							riff.stream(len);
							time+=len;
							trace.step_field[i].end = time;
							riff.stream(trace.step_field[i].range,B);
							riff.stream(trace.step_field[i].slope0,B);
							riff.stream(trace.step_field[i].slope1,B);
						}
					riff.closeStream();
				}
				break;
			}
		}
		while (riff.findNext(riff_name));
}


template <class Def> void AnimatorA<Def>::loadFromRiff(RiffFile&riff, index_t index_in_context)
{
	bool first = index_in_context == 0;

	if ((first && !riff.findFirst("ANIM")) || (!first && !riff.findNext("ANIM")))
		return;
	size_t name_length = riff.getSize();
	name.setLength(name_length);
	riff.get(name.mutablePointer());
	name.trimThis();


	if (!riff.findNext("LIST"))
		return;
	riff.enter();
		loadTraces(obj_trace_field,riff,"OTRC","OTR2");
		loadTraces(acc_trace_field,riff,"ATRC","ATR2");
		loadTraces(whl_trace_field,riff,"WTRC","WTR2");
	riff.dropBack();
}

template <class Def> double AnimatorA<Def>::executionTime()	const
{
	double len = 0;
			
	for (index_t i = 0; i < obj_trace_field.length(); i++)
		for (index_t j = 0; j < obj_trace_field[i].step_field.length(); j++)
			if (obj_trace_field[i].step_field[j].end > len)
				len = obj_trace_field[i].step_field[j].end;
	for (index_t i = 0; i < acc_trace_field.length(); i++)
		for (index_t j = 0; j < acc_trace_field[i].step_field.length(); j++)
			if (acc_trace_field[i].step_field[j].end > len)
				len = acc_trace_field[i].step_field[j].end;
	for (index_t i = 0; i < whl_trace_field.length(); i++)
		for (index_t j = 0; j < whl_trace_field[i].step_field.length(); j++)
			if (whl_trace_field[i].step_field[j].end > len)
				len = whl_trace_field[i].step_field[j].end;
	return len;
}



template <class Def> void AnimatorA<Def>::link(Geometry<Def>*domain)
{
	count_t invalid;
	
	invalid = 0;
	for (index_t i = 0; i < obj_trace_field.length(); i++)
	{
		obj_trace_field[i].target = domain->lookupObject(obj_trace_field[i].tname);
		if (!obj_trace_field[i].target)
			invalid ++;
	}
	if (invalid)
	{
		Array<TraceA<SubGeometryA<Def>,6>,Adopt>	new_field(obj_trace_field.length()-invalid);
		index_t at = 0;
		for (index_t i = 0; i < obj_trace_field.length(); i++)
		{
			if (obj_trace_field[i].target)
				new_field[at++] = obj_trace_field[i];
		}
		obj_trace_field.adoptData(new_field);
	}
	
	
	invalid = 0;
	for (index_t i = 0; i < acc_trace_field.length(); i++)
	{
		acc_trace_field[i].target = domain->lookupAccelerator(acc_trace_field[i].tname);
		if (!acc_trace_field[i].target)
			invalid++;
	}
	if (invalid)
	{
		Array<TraceA<TAccelerator<Def>,1>,Adopt>	new_field(acc_trace_field.length()-invalid);
		index_t at = 0;
		for (index_t i = 0; i < acc_trace_field.length(); i++)
		{
			if (acc_trace_field[i].target)
				new_field[at++] = acc_trace_field[i];
		}
		acc_trace_field.adoptData(new_field);
	}
	
	invalid = 0;
	for (index_t i = 0; i < whl_trace_field.length(); i++)
	{
		whl_trace_field[i].target = domain->lookupWheel(whl_trace_field[i].tname);
		if (!whl_trace_field[i].target)
			invalid++;
	}
	if (invalid)
	{
		Array<TraceA<TWheel<Def>,2>,Adopt>		new_field(whl_trace_field.length()-invalid);
		index_t at = 0;
		for (index_t i = 0; i < whl_trace_field.length(); i++)
		{
			if (whl_trace_field[i].target)
				new_field[at++] = whl_trace_field[i];
		}
		whl_trace_field.adoptData(new_field);
	}
}

template <class Def> void AnimatorA<Def>::saveToRiff(RiffChunk*riff)	const
{
	riff->appendBlock("ANIM",name.c_str(),name.length());
	RiffChunk*list = riff->appendBlock("LIST");
		saveTraces(obj_trace_field,list,"OTR2");
		saveTraces(acc_trace_field,list,"ATR2");
		saveTraces(whl_trace_field,list,"WTR2");
}

template <class Def> bool VertexContainerA<Def>::equal;
template <class Def> bool MaterialData<Def>::equal;
template <class Def> bool IndexContainerA<Def>::equal;
template <class Def> bool RenderObjectA<Def>::equal;

template <class Def> String Geometry<Def>::error;
template <class Def> String SubGeometryA<Def>::error;
#endif
