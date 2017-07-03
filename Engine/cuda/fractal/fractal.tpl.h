#ifndef CudaFractal_mapTplH
#define CudaFractal_mapTplH

/******************************************************************

CudaFractal-Core for generation of CudaFractal landscapes.
Use in combination with EveCudaFractalModule.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/




 
template <unsigned Exponent>
	CudaFractal::CTemplateMap<Exponent>::CTemplateMap():CMap(info,range,vertices,child_vertices,inner_vertices,center_vertices,parent_vertices,border[0],border[1],border[2],inner_border[0],inner_border[1],inner_border[2],inner,child_vertex,center,parent_vertex)
	{}


template <unsigned Exponent>
	CudaFractal::CTemplateSurface<Exponent>::CTemplateSurface():CDataSurface(vertex,Exponent,vertices)
	{}
	
template <unsigned Exponent>
	CudaFractal::CTemplateSurface<Exponent>::CTemplateSurface(CBody*super_):CDataSurface(vertex,Exponent,vertices)
	{
		super = super_;
	}

template <unsigned Exponent>
	CudaFractal::CTemplateSurface<Exponent>::CTemplateSurface(CBody*super_,CTemplateSurface<Exponent>*parent_,BYTE child_index, const CTimer::Time&time):CDataSurface(vertex,Exponent,vertices)
	{
		super = super_;
		parent = parent_;
		this->child_index = child_index;
		referenced = time;
	}
	
	
template <unsigned Exponent>
	CudaFractal::CTemplateSurface<Exponent>::CTemplateSurface(CBody*super_,CTemplateSurface<Exponent>*parent_,BYTE child_index, const THostSurfaceLink&n0_, const THostSurfaceLink&n1_, const THostSurfaceLink&n2_, const CTimer::Time&time):CDataSurface(vertex,Exponent,vertices)
	{
		referenced = time;
		super = super_;
		parent = parent_;
		this->child_index = child_index;
		neighbor_link[0] = n0_;
		neighbor_link[1] = n1_;
		neighbor_link[2] = n2_;
		//logfile << "created 0x"<<IntToHex((int)this,8)<<" with super 0x"<<IntToHex((int)super,8)<<nl;
		reinit();
	}
	




	


template <unsigned Exponent> 
	void			CudaFractal::CIndexPatterns<Exponent>::createCorePattern()
	{
	    BEGIN
	    UINT32*p = core_pattern;

	    for (unsigned y = 3; y < range-1; y++)
	    {
	        for (unsigned x = 1; x < y-2; x++)
	        {
	            (*p++) = map.getIndex(x,y);
	            (*p++) = map.getIndex(x+1,y);
	            (*p++) = map.getIndex(x,y-1);
	            (*p++) = map.getIndex(x+1,y);
	            (*p++) = map.getIndex(x+1,y-1);
	            (*p++) = map.getIndex(x,y-1);
	        }
	        (*p++) = map.getIndex(y-2,y);
	        (*p++) = map.getIndex(y-1,y);
	        (*p++) = map.getIndex(y-2,y-1);
	    }

	    if (p != core_pattern + ARRAYSIZE(core_pattern))
	        ERROR_THROW__("bad core-pattern. local: "+IntToStr(p-core_pattern)+" global: "+IntToStr(ARRAYSIZE(core_pattern)));

	    current = pattern;
	    ocurrent = pattern_offset;
	    END
	}
	
template <unsigned Exponent>
	void CudaFractal::CIndexPatterns<Exponent>::makePattern(unsigned r0, unsigned r1, unsigned r2)
	{
	    BEGIN
	    (*ocurrent++) = current - pattern;

	    unsigned r[3] = {r0,r1,r2};	//resolution, number of elements along the edge
	    for (BYTE k = 0; k < 3; k++)
	    {
	        unsigned overload = (range-1) / r[k];
			ASSERT2__(overload > 0, range,r[k]);
	        unsigned*outer = map.border[k],
	                *inner = map.inner_border[k];

			#if 0	//traditional, ugly
				(*current++) = outer[0];
				(*current++) = outer[overload];
				(*current++) = inner[1];
				(*current++) = outer[overload];
				(*current++) = outer[overload*2];
				(*current++) = inner[1];

				unsigned last = overload*2;	//last outer vertex
				for (unsigned i = 1; i < range-2; i++)
				{

					if (i >= last+overload/2)
					{
						(*current++) = inner[i];
						(*current++) = outer[last];
						(*current++) = outer[last+overload];
						last += overload;
					}
					if (i < range-3)
					{
						(*current++) = inner[i+1];
						(*current++) = inner[i];
						(*current++) = outer[last];
					}
				}

				if (range-3 > last+overload/2)
				{
					(*current++) = inner[range-3];
					(*current++) = outer[last];
					(*current++) = outer[last+overload];
					last += overload;
				}
				if (last != range-1)
				{
					(*current++) = inner[range-3];
					(*current++) = outer[last];
					(*current++) = outer[range-1];
					last += overload;
				}
			#else
				//begin
				(*current++) = outer[0];
				(*current++) = outer[overload];
				(*current++) = inner[1];
				
				unsigned iat = 1,
						oat = overload;
				
				//end
				if (r[k]>1)
				{
					(*current++) = outer[range-1-overload];
					(*current++) = outer[range-1];
					(*current++) = inner[range-3];


					
					//segments:
					//for (unsigned i = 0; i+1 < r[k]; i++)	//go 4
					//unsigned i = 0;
					while (oat+overload < range)
					{
						unsigned end = oat+overload*2 < range?oat+overload/2:oat+overload-1;
					
						while (iat < end && iat+1<range-2)
						{
							ASSERT__(iat > 0);
							(*current++) = outer[oat];
							(*current++) = inner[iat+1];
							(*current++) = inner[iat];
							iat++;
						}
						if (oat+overload*2 < range)
						{
							(*current++) = outer[oat];
							(*current++) = outer[oat+overload];
							(*current++) = inner[iat];
						}
						oat += overload;
					}
				}
				else
				{
					while (iat+3<range)
					{
						(*current++) = outer[oat];
						(*current++) = inner[iat+1];
						(*current++) = inner[iat];
						iat++;
					}
				}

			
			
			#endif
			
	    }

	    memcpy(current,core_pattern,sizeof(core_pattern));
	    current+=ARRAYSIZE(core_pattern);
	    END
}
	
	 
template <unsigned Exponent> 
	CudaFractal::CIndexPatterns<Exponent>::CIndexPatterns():CAbstractIndexPatterns(pattern,pattern_offset,core_pattern,ARRAYSIZE(pattern_offset)-1,Exponent)
	{}
	
template <unsigned Exponent>
	void CudaFractal::CIndexPatterns<Exponent>::create()
	{
		if (created)
			return;
		BEGIN
		created = true;
		createCorePattern();
		unsigned range0 = range-1;
		while (range0 > 0)
		{
			unsigned range1 = range-1;
			while (range1 > 0)
			{
				unsigned range2 = range-1;
				while (range2 > 0)
				{
					makePattern(range0,range1,range2);
					range2>>=1;
				}
				range1>>=1;
			}
			range0>>=1;
		}
		(*ocurrent++) = current - pattern;

		if (current-pattern > ARRAYSIZE(pattern))
			ERROR_THROW__("bad pattern-creation");

		if (ocurrent-pattern_offset > ARRAYSIZE(pattern_offset))
			ERROR_THROW__("bad pattern-creation ("+IntToStr(ocurrent-pattern_offset)+">"+IntToStr(ARRAYSIZE(pattern_offset))+")");

		END
	}
	
template <unsigned Exponent, unsigned ExponentReduction>
	CudaFractal::CTemplateBody<Exponent,ExponentReduction>::CTemplateBody(float sector_size, unsigned max_recursion_level):
		CBody("[body]",sector_size,Exponent,max_recursion_level,&device_info_field,&device_vertex_index_field,device_border_field,&device_triangles, &full_map,&reduced_map)
		{}
		

template <unsigned Exponent, unsigned ExponentReduction>
	CudaFractal::CTemplateBody<Exponent,ExponentReduction>::CTemplateBody(const CString&name, float sector_size, unsigned max_recursion_level):
		CBody(name,sector_size,Exponent,max_recursion_level,&device_info_field,&device_vertex_index_field,device_border_field,&device_triangles, &full_map,&reduced_map)
		{}

template <unsigned Exponent, unsigned ExponentReduction>
	CudaFractal::CDataSurface*		CudaFractal::CTemplateBody<Exponent,ExponentReduction>::makeFace()
	{
		return SHIELDED(new CTemplateSurface<Exponent>());
	}
	

	
	
	
template <unsigned Exponent, unsigned ExponentReduction>
	void		CudaFractal::CTemplateBody<Exponent,ExponentReduction>::assembleDeviceMaps(unsigned lookup_reduction)
	{
		if (device_info_field.length())
			return;
		
		unsigned	forward_map[full_map.vertices][4];
		bool		grid_map[full_map.vertices];
		memset(grid_map,false,sizeof(grid_map));
		
		for (unsigned i = 0; i < full_map.parent_vertices; i++)
		{
			grid_map[full_map.parent_vertex[i].index] = true;
			_c4(full_map.parent_vertex[i].origin,forward_map[full_map.parent_vertex[i].index]);
		}
		
		CArray<unsigned>  local_vertex_index(reduced_map.vertices);
		unsigned at_index = 0;
		
		CArray< ::TVertexInfo>	local_vertex_info(full_map.vertices);
		for (unsigned i = 0; i < full_map.vertices; i++)
		{
			//cout << " "<<i;
			TVertexInfo&info = local_vertex_info[i];
			const TMapVertex&v = full_map.info[i];
			info.grid_vertex = grid_map[i];
			info.edge_index = v.edge_index;
			for (BYTE k = 0; k < 4; k++)
			{
				if (info.grid_vertex)
					info.parent_space[k].match = forward_map[i][k];
				else
				{
					ASSERT2__(v.parent0<full_map.vertices,v.parent0,full_map.vertices);
					ASSERT2__(v.parent1<full_map.vertices,v.parent1,full_map.vertices);
					ASSERT2__(v.parent2<full_map.vertices,v.parent2,full_map.vertices);
					info.parent_space[k].parent0 = forward_map[v.parent0][k];
					info.parent_space[k].parent1 = forward_map[v.parent1][k];
					info.parent_space[k].parent2 = forward_map[v.parent2][k];
					if (info.edge_index == -1)
					{
						ASSERT2__(v.parent3<full_map.vertices,v.parent3,full_map.vertices);
						info.parent_space[k].parent3 = forward_map[v.parent3][k];
					}
				}
			}
			info.x = v.x;
			info.y = v.y;
			info.parent0 = v.parent0;
			info.parent1 = v.parent1;
			info.parent2 = v.parent2;
			info.parent3 = v.parent3;
			
			
			
			if (!(v.x%(1<<ExponentReduction)) && !(v.y%(1<<ExponentReduction)))
			{
				ASSERT2__(at_index < local_vertex_index.length(),at_index,local_vertex_index.length());
				local_vertex_index[at_index++] = i;
			}
		}
		ASSERT__(at_index == reduced_map.vertices);
		
		device_vertex_index_field = local_vertex_index;
		device_info_field = local_vertex_info;
		for (BYTE k = 0; k < 3; k++)
		{
			device_border_field[k].resize(full_map.range,!CUDA::Device::emulation);
			device_border_field[k].copyFrom(full_map.border[k]);
			//cout << "border field "<<(int)k<<" now "<<device_border_field[k].length()<<endl;
		}
		
		
		CBuffer<unsigned>	vertex_indices;
		unsigned step = 1<<lookup_reduction;
		for (unsigned y = step; y < full_map.range; y+=step)
			for (unsigned x = 0; x < y; x+=step)
			{
				vertex_indices << full_map.getIndex(x,y);
				vertex_indices << full_map.getIndex(x+step,y);
				vertex_indices << full_map.getIndex(x,y-step);
				if (x+step < y)
				{
					vertex_indices << full_map.getIndex(x,y-step);
					vertex_indices << full_map.getIndex(x+step,y);
					vertex_indices << full_map.getIndex(x+step,y-step);
				}
			}
		
		device_triangles.resize(vertex_indices.fillState(),!CUDA::Device::emulation);
		device_triangles.copyFrom(vertex_indices.pointer());
	}
	
	

	
template <unsigned Exponent> 
	UINT32			CudaFractal::CIndexPatterns<Exponent>::pattern[(Exponent+1)*(Exponent+1)*(Exponent+1)*range*range*3];
template <unsigned Exponent> 
	UINT32			CudaFractal::CIndexPatterns<Exponent>::pattern_offset[(Exponent+1)*(Exponent+1)*(Exponent+1)+1];
template <unsigned Exponent> 
	UINT32			CudaFractal::CIndexPatterns<Exponent>::core_pattern[(range-4)*(range-4)*3];
template <unsigned Exponent> 
	UINT32			*CudaFractal::CIndexPatterns<Exponent>::current = CudaFractal::CIndexPatterns<Exponent>::pattern;
template <unsigned Exponent> 
	UINT32			*CudaFractal::CIndexPatterns<Exponent>::ocurrent = CudaFractal::CIndexPatterns<Exponent>::pattern_offset;
					

template <unsigned Exponent> 
	CudaFractal::CTemplateMap<Exponent>	CudaFractal::CIndexPatterns<Exponent>::map;

template <unsigned Exponent> 
	bool			CudaFractal::CIndexPatterns<Exponent>::created(false);

	
template <unsigned Exponent, unsigned ExponentReduction>
	CudaFractal::CTemplateMap<Exponent>		CudaFractal::CTemplateBody<Exponent,ExponentReduction>::full_map;
template <unsigned Exponent, unsigned ExponentReduction>
	CudaFractal::CTemplateMap<Exponent-ExponentReduction>	CudaFractal::CTemplateBody<Exponent,ExponentReduction>::reduced_map;
template <unsigned Exponent, unsigned ExponentReduction>
	CUDA::CDeviceArray< ::TVertexInfo>			CudaFractal::CTemplateBody<Exponent,ExponentReduction>::device_info_field;
template <unsigned Exponent, unsigned ExponentReduction>
	CUDA::CDeviceArray<unsigned>				CudaFractal::CTemplateBody<Exponent,ExponentReduction>::device_vertex_index_field;
template <unsigned Exponent, unsigned ExponentReduction>
	CUDA::CDeviceArray<unsigned>				CudaFractal::CTemplateBody<Exponent,ExponentReduction>::device_border_field[3];
template <unsigned Exponent, unsigned ExponentReduction>
	CUDA::CDeviceArray<unsigned>				CudaFractal::CTemplateBody<Exponent,ExponentReduction>::device_triangles;










#endif
