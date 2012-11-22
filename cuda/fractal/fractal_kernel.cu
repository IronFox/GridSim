// includes, system
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
#  define WINDOWS_LEAN_AND_MEAN
#  include <windows.h>
#endif

#include <cuda_gl_interop.h>

//#include <cutil.h>

#include "fractal_kernel.h"
#include "../../math/vector.h"
#include "../math/cuda_vector.h"
#include "../math/cuda_composite.h"
#include "../math/cuda_object.h"

/*#include "../../image/image.h"
#include "../../image/converter/png.h"*/

#include "kernel/config.h"


#define DEVICE_RESOURCE(_TYPE_,_NAME_)	const _TYPE_&_NAME_
//#define DEVICE_RESOURCE(_TYPE_,_NAME_)	_TYPE_ _NAME_

#define __DEVICE_CALL__

__device__	void	setHeight(float vector[3],float current_height, float relative_height, const TDeviceSurface&surface, const TContext&context)
{
	#include "kernel/differential_set_height.function"
}


__device__ float	ageAt(unsigned depth)
{
	#include "kernel/age_at.function"
}

__device__ float	getWeight(float h, float average, float variance, float depth)
{
	#include "kernel/get_weight.function"
}


/**
	@brief Queries a random float value in the range [-1,+1] and advances the seed
	@param [inout] seed value
	@return random value
*/
__device__ float	getRandom(int&seed)
{
	#include "kernel/get_random.function"
}


__device__	float	height(const TChannelConfig&channel)
{
	#include "kernel/height.function"
}


__device__ float	getNoise(float distance, const TVertex&p0, const TVertex&p1, const TDeviceSurface&surface, const TContext&context)
{
	#include "kernel/get_noise.function"
}

/* __device__ float	getSmoothStrength(float distance, const TVertex&p0, const TVertex&p1, const TDeviceSurface&surface, const TContext&context)
{
	#include "kernel/get_smooth_strength.function"
}
 */

__device__ void		generate3(const TVertex&p0, const TVertex&p1, const TVertex&p2, TVertex&result, unsigned seed, TDeviceSurface&surface, const TContext&context)
{
	#include "kernel/generate3.function"
}

__device__ void		generate4(const TVertex&p0, const TVertex&p1, const TVertex&p2, const TVertex&p3, TVertex&result, int seed, TDeviceSurface&surface, const TContext&context)
{
	#include "kernel/generate4.function"
}

__device__ void		generate4edge(const TVertex&p0, const TVertex&p1, const TVertex&p2, const TVertex&p3, TVertex&result, int seed, TDeviceSurface&surface, const TContext&context)
{
	#include "kernel/generate4_edge.function"
}

__device__	unsigned	vertexIndex(unsigned x, unsigned y)
{
	return y*(y+1)/2+x;
}


__global__ void generateVertex(TDeviceSurface surface, TDeviceSurface parent, TContext context,const TVertexInfo*info_field, unsigned vertex_count)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear >= vertex_count)	//rounding error
		return;
	#include "kernel/generate_vertex.function"
}
/*
__global__ void weightVertex(TDeviceSurface surface, TContext context,const TVertexInfo*info_field, unsigned vertex_count)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear >= vertex_count)	//rounding error
		return;
	#include "kernel/weight_vertex.function"
}
*/

__global__	void	generateNormal(TVertex*vertices, const TVertexInfo*info_field, unsigned vertex_count, unsigned max_row)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear >= vertex_count)	//rounding error
		return;
		
	#include "kernel/generate_normal.function"


}


__global__	void	detectBoundingBox0(const TVertex*vertices,float*out, unsigned vertex_count)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear*2 >= vertex_count)	//rounding error
		return;

	const float		*v0 = vertices[linear*2].position,
					*v1 = linear*2+1 >= vertex_count?v0:vertices[linear*2+1].position;
	float	*box = out+linear*6;
	if (v0[0] <  v1[0])
	{
		box[0] = v0[0];
		box[3] = v1[0];
	}
	else
	{
		box[3] = v0[0];
		box[0] = v1[0];
	}
	
	if (v0[1] <  v1[1])
	{
		box[1] = v0[1];
		box[4] = v1[1];
	}
	else
	{
		box[4] = v0[1];
		box[1] = v1[1];
	}

	if (v0[2] <  v1[2])
	{
		box[2] = v0[2];
		box[5] = v1[2];
	}
	else
	{
		box[5] = v0[2];
		box[2] = v1[2];
	}
}

__global__	void	detectBoundingBox1(const float*boxes,float*out, unsigned vertex_count)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear*2 >= vertex_count)	//rounding error
		return;

	const float	*b0 = boxes+linear*12,
				*b1 = linear*2+1 >= vertex_count?b0:b0+6;
	float	*box = out+linear*6;
	if (b0[0] <  b1[0])
		box[0] = b0[0];
	else
		box[0] = b1[0];
	if (b0[1] <  b1[1])
		box[1] = b0[1];
	else
		box[1] = b1[1];
	if (b0[2] <  b1[2])
		box[2] = b0[2];
	else
		box[2] = b1[2];
		
		
	if (b0[3] >  b1[3])
		box[3] = b0[3];
	else
		box[3] = b1[3];
	if (b0[4] >  b1[4])
		box[4] = b0[4];
	else
		box[4] = b1[4];
	if (b0[5] >  b1[5])
		box[5] = b0[5];
	else
		box[5] = b1[5];
}



__global__	void	detectBoundingRadius0(const TVertex*vertices,float*out, unsigned vertex_count, float3 center)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear >= vertex_count)	//rounding error
		return;

	const float	*v0 = vertices[linear].position;
	out[linear] = dbSqr(v0[0]-center.x)+dbSqr(v0[1]-center.y)+dbSqr(v0[2]-center.z);
}

__global__	void	detectBoundingRadius1(const float*radi,float*out, unsigned vertex_count)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear*2 >= vertex_count)	//rounding error
		return;

	const float	r0 = radi[linear*2],
				r1 = linear*2+1 >= vertex_count?r0:radi[linear*2+1];
	out[linear] = fmaxf(r0,r1);

}

__global__	void	mergeEdge(TDeviceSurface this_surface,TDeviceSurface that_surface,TContext context,const TVertexInfo*info_field,const unsigned*edge,const unsigned*neighbor_edge,unsigned edge_length)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	//if (!linear || linear+1 >= edge_length)
	if (linear >= edge_length)
		return;

	#include "kernel/merge_edge.function"

	
	/*this_v.normal[0] = 0;
	this_v.normal[1] = 0;
	this_v.normal[2] = 1;*/
	
	//dvC3(this_v.normal,that_v.normal);
}

__global__	void	copyEdge(TDeviceSurface this_surface,TDeviceSurface that_surface,TContext context,const TVertexInfo*info_field,const unsigned*edge,const unsigned*neighbor_edge,unsigned edge_length, bool copy_to)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	//if (!linear || linear+1 >= edge_length)
	if (linear >= edge_length)
		return;

	#include "kernel/copy_edge.function"

	
	/*this_v.normal[0] = 0;
	this_v.normal[1] = 0;
	this_v.normal[2] = 1;*/
	
	//dvC3(this_v.normal,that_v.normal);
}

__global__	void	generateCover(TCoverVertex*out_vertices,TDeviceSurface surface, TContext context, const unsigned*index_field,unsigned index_count)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear >= index_count)	//rounding error
		return;
		
	#include "kernel/generate_cover.function"
}

__device__	void	resolveDirection(const TVertex&vertex, const TDeviceSurface&surface, const TContext&context, float v[3])
{
	v[0] = vertex.position[0]+context.sector_size*surface.sector[0];
	v[1] = vertex.position[1]+context.sector_size*surface.sector[1];
	v[2] = vertex.position[2]+context.sector_size*surface.sector[2];
	dvNormalize(v);
}


__global__	void	generateVBO(float*vertex_data,TDeviceSurface surface,TContext context, const TVertexInfo*info_field,const unsigned*index_field, unsigned index_count, unsigned vertex_max)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear >= index_count)	//rounding error
		return;

	#include "kernel/generate_vbo.function"

}

__device__	float	cloudThickness(const TVertex&v, const TContext&context)
{
	return 1.0;//v.channel.c0*v.channel.c1;
}

__global__	void	generateCloudVBO(float*vertex_data,TDeviceSurface surface,TContext context, const unsigned*index_field, unsigned index_count, unsigned vertex_max)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear >= index_count/3)	//rounding error
		return;


	float*v = vertex_data+linear*9;
	unsigned	i0 = index_field[linear*3],
				i1 = index_field[linear*3+1],
				i2 = index_field[linear*3+2];
	DEVICE_RESOURCE(TVertex,v0) = surface.vertex[i0];
	DEVICE_RESOURCE(TVertex,v1) = surface.vertex[i1];
	DEVICE_RESOURCE(TVertex,v2) = surface.vertex[i2];
	
	
	float			t0 = cloudThickness(v0,context),
					t1 = cloudThickness(v1,context),
					t2 = cloudThickness(v2,context),
					t = (t0+t1+t2)/3.0f;
	
	dvCenter(v0.position,v1.position,v2.position,v);
	float	h = (v0.height+v1.height+v2.height)/3.0f;
	dvC3(v,v+3);
	setHeight(v,h,1.0f,surface,context);
	setHeight(v+3,h,1.0f+t,surface,context);
	
	float			p0[3],
					p1[3],
					p2[3];
	
	dvC3(v0.position,p0);
	dvC3(v1.position,p1);
	dvC3(v2.position,p2);
	setHeight(p0,v0.height,1.0f+t0,surface,context);
	setHeight(p1,v1.height,1.0f+t1,surface,context);
	setHeight(p2,v2.height,1.0f+t2,surface,context);
	doCalculateTriangleNormal(p0, p1, p2, v+6);
}

__device__	unsigned char*	getTexel3(unsigned char*texel_data, unsigned x, unsigned y, unsigned dimension)
{
	return texel_data+(y*dimension+x)*3;
}

__device__	unsigned char*	getTexel4(unsigned char*texel_data, unsigned x, unsigned y, unsigned dimension)
{
	return texel_data+(y*dimension+x)*4;
}

__device__	unsigned char*	getTexel1(unsigned char*texel_data, unsigned x, unsigned y, unsigned dimension)
{
	return texel_data+(y*dimension+x);
}



__global__ void	generateTexture(unsigned char*texel_data,TDeviceSurface surface,TContext context,const TVertexInfo*info_field,unsigned vertex_count, unsigned dimension)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear >= vertex_count)	//rounding error
		return;

	#include "kernel/generate_texture.function"

}

__global__ void	generateTextureC(unsigned char*texel_data,TDeviceSurface surface,TContext context,const TVertexInfo*info_field,unsigned vertex_count, unsigned dimension)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear >= vertex_count)	//rounding error
		return;

	#include "kernel/generate_texture_c.function"

}

__device__ bool _atomicCAS(int*pntr, int compare, int set)
{
	//return atomicCAS(pntr, compare, set)==set;	//wish i could use this...
	
	if (*pntr != compare)
		return false;
	*pntr = set;
	return true;
}



__global__ void cast(TRayIntersection*intersection,TDeviceSurface surface, float3 b_, float3 d_, unsigned*triangle_indices,unsigned triangles)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear >= triangles)	//rounding error
		return;
		
	float b[3],d[3];
	dvC3(b_,b);
	dvC3(d_,d);
		
		
	#include "kernel/cast.function"


}


__global__ void groundCast(TGroundInfo*ground,TDeviceSurface surface, float3 b_, float3 d_, unsigned*triangle_indices,unsigned triangles)
{
    const unsigned	int	x = blockIdx.x*blockDim.x + threadIdx.x,
						y = blockIdx.y*blockDim.y + threadIdx.y,
						linear = x+y*(blockDim.x*gridDim.x);
	if (linear >= triangles)	//rounding error
		return;

	float b[3],d[3];
	dvC3(b_,b);
	dvC3(d_,d);
	
	#include "kernel/ground_cast.function"

}




namespace CUDA
{
	static const unsigned 	//block_dimension = 14,
							block_x = 128,
							block_y = 1,
							block_size = block_x*block_y;
	
	/**
		Efficiency observations (subjective):
		
		Testing with each 1000 ray lookup operations it turned out that higher thread counts per block are good but the upper limit is unknown.
		One thread per block is definately a bad idea. Device info says 512 threads were fine but resource exceptions are thrown when more than 192 threads are allocated. This correlates to no available specification.
		There may still be some advantage of 192 over 128 threads but the danger that some device may not support that is too great. 128 seems fine for now.
		One dimensional thread blocks seem to be slightly faster than quadratic ones.
	*/

	static	unsigned	ceilHalf(unsigned value)
	{
		unsigned rs = value>>1;
		if (value%2)
			rs++;
		return rs;
	}

	static	unsigned	ceilDiv(unsigned v0, unsigned v1)
	{
		unsigned rs = v0/v1;
		if (v0%v1)
			rs++;
		return rs;
	}


	bool init(float relative_vram_usage)
	{
		if (!Device::initialize(Preference::MostMemory) || !Device::current.totalGlobalMem)
			return false;
		device_channel.memory_limit = (UINT64)((double)Device::current.totalGlobalMem*relative_vram_usage);
		return true;
	}

	void checkStatus(const TCodeLocation&location)
	{
		cudaError_t error = cudaGetLastError();
		if (error != cudaSuccess)
			fatal(location,"Operation failed with error code #"+CString(error)+" ('"+cudaGetErrorString(error)+"')");
	}
	

	void generateVertices(THostSurface&surface, THostSurface&parent, const TContext&context,CDeviceArray<TVertexInfo>&device_info_field)
	{
		bool existed = surface.vertex_field.length() == device_info_field.length();
		surface.vertex_field.resize(device_info_field.length(),!Device::emulation);
		if (Device::emulation)
		{
			Emulation::TProcessParameters	parameter;
			parameter.surface = &surface;
			parameter.parent = &parent;
			parameter.context = &context;
			parameter.info_field = device_info_field.hostPointer();
			//parameter.vertex_count = device_info_field.length();
			
			Emulation::process(emuGenerateVertex,parameter,device_info_field.length());
			//Emulation::process(emuWeightVertex,parameter,device_info_field.length());
			
			surface.vertex_field.signalHostMemoryAltered();
		}
		else
		{
			cudaGetLastError();
			ARRAY_DEBUG_POINT(k,existed)
			TVertex test = surface.vertex_field[0];
			DEBUG_POINT(k)
			unsigned blocks = ceilDiv(device_info_field.length(),block_size);
			
			dim3 block(block_x, block_y, 1);
			dim3 grid(blocks, 1, 1);

			
			TDeviceSurface	ds;
			((TBaseSurface&)ds) = surface;
			TDeviceSurface	dp;
			((TBaseSurface&)dp) = parent;
			
			ASSERT__(parent.vertex_field.length()==device_info_field.length());
			
			ds.vertex = surface.vertex_field.devicePointer();
			dp.vertex = parent.vertex_field.devicePointer();
			
			
			generateVertex<<< grid, block, 0>>>(ds,dp,context,device_info_field.devicePointer(),device_info_field.length());
			checkStatus(CLOCATION);
			
			/*weightVertex<<< grid, block, 0>>>(ds,context,device_info_field.devicePointer(),device_info_field.length());
			checkStatus(CLOCATION);*/
			surface.vertex_field.signalDeviceMemoryAltered();			
			cudaThreadSynchronize();
			//cout << "gen vertices"<<endl;
			
		}
	}	
	
	

	void generateNormals(THostSurface&surface, const TContext&context,CDeviceArray<TVertexInfo>&device_info_field, unsigned max_row)
	{
		if (Device::emulation)
		{
			Emulation::TProcessParameters	parameter;
			parameter.surface = &surface;
			parameter.context = &context;
			parameter.info_field = device_info_field.hostPointer();
			parameter.max_row = max_row;
			
			Emulation::process(emuGenerateNormal,parameter,device_info_field.length());
			surface.vertex_field.signalHostMemoryAltered();
		}
		else
		{
			cudaGetLastError();
			unsigned blocks = ceilDiv(device_info_field.length(),block_size);
			
			dim3 block(block_x, block_y, 1);
			dim3 grid(blocks, 1, 1);

			
			TDeviceSurface	ds;
			((TBaseSurface&)ds) = surface;
			
			//surface.vertex_field.resize(device_info_field.length(),true);
			
			ds.vertex = surface.vertex_field.devicePointer();
			generateNormal<<< grid, block, 0>>>(ds.vertex,device_info_field.devicePointer(),device_info_field.length(),max_row);
			checkStatus(CLOCATION);
			surface.vertex_field.signalDeviceMemoryAltered();
			cudaThreadSynchronize();
			//cout << "gen normals"<<endl;
			
		}
	}	
	


	
	void	getBoundingBox(THostSurface&surface,  float dim[6])
	{
		ASSERT__(!Device::emulation);
		cudaGetLastError();
		static CDeviceArray<float>	box_buffer;
		if (box_buffer.size() < surface.vertex_field.length()*12)
			box_buffer.resize(surface.vertex_field.length()*12,true);	//first iteration will create vertex_count/2 boxes, each 6 floats, next iterations will create each half of that. So x12 should suffice
			
			
		unsigned seg_cnt = surface.vertex_field.count();
		unsigned num_runs = ceilHalf(seg_cnt);
		unsigned blocks = ceilDiv(num_runs,block_size);
		
	    dim3 block(block_x, block_y, 1);
	    dim3 grid(blocks, 1, 1);
		
	    detectBoundingBox0<<< grid, block,0 >>>(surface.vertex_field.devicePointer(),box_buffer.devicePointer(),seg_cnt);
		

		float*at = box_buffer.devicePointer();
		while (seg_cnt > 1)
		{
			seg_cnt = ceilHalf(seg_cnt);
			num_runs = ceilHalf(seg_cnt);
			blocks = ceilDiv(num_runs,block_size);
			dim3 block(block_x, block_y, 1);
		    dim3 grid(blocks, 1, 1);
			float*out = at+seg_cnt*6;
			detectBoundingBox1<<< grid, block,0 >>>(at,out,seg_cnt);
			at=out;
		}
		checkStatus(CLOCATION);
		CUDA_ASSERT(cudaMemcpy(dim,at,sizeof(float)*6,cudaMemcpyDeviceToHost));
		checkStatus(CLOCATION);
	}


	
	float	getBoundingRadius(THostSurface&surface, const float center[3])
	{
		ASSERT__(!Device::emulation);
		cudaGetLastError();
		static CDeviceArray<float>	radius_buffer;
		if (radius_buffer.size() < surface.vertex_field.length()*4)
			radius_buffer.resize(surface.vertex_field.length()*4,true);	//first iteration will create vertex_count radi, each 1 float, next iterations will create each half of that. So x4 should suffice
		
		
			
		unsigned seg_cnt = surface.vertex_field.count();
		unsigned num_runs = seg_cnt;
		unsigned blocks = ceilDiv(num_runs,block_size);
		
		
		float3 c = make_float3(center[0],center[1],center[2]);
	    dim3 block(block_x, block_y, 1);
	    dim3 grid(blocks, 1, 1);
		
	    detectBoundingRadius0<<< grid, block,0 >>>(surface.vertex_field.devicePointer(),radius_buffer.devicePointer(),seg_cnt,c);
		


		float*at = radius_buffer.devicePointer();
		while (seg_cnt > 1)
		{
			num_runs = ceilHalf(seg_cnt);
			blocks = ceilDiv(num_runs,block_size);
		
			dim3 block(block_x, block_y, 1);
		    dim3 grid(blocks, 1, 1);
			float*out = at+seg_cnt;
			detectBoundingRadius1<<< grid, block,0 >>>(at,out,seg_cnt);
			at=out;
			
			seg_cnt = ceilHalf(seg_cnt);
		}
		checkStatus(CLOCATION);
		float radius;
		CUDA_ASSERT(cudaMemcpy(&radius,at,sizeof(float),cudaMemcpyDeviceToHost));
		return sqrt(radius);
	}



	void	mergeEdges(THostSurface&surface, const TContext&context,CDeviceArray<TVertexInfo>&device_info_field, CDeviceArray<unsigned> borders[3], unsigned edge_length)
	{
		//ASSERT__(edge_length>0);
		ASSERT__((surface.flags&(::TBaseSurface::HasData)) != 0);
		if (Device::emulation)
		{
			Emulation::TProcessParameters	parameter;
			parameter.surface = &surface;
			parameter.context = &context;
			parameter.info_field = device_info_field.hostPointer();
			parameter.edge_length = edge_length;
			
			
			for (BYTE k = 0; k < 3; k++)
			{
				ASSERT_EQUAL__(borders[k].length(),edge_length);
				//ASSERT1__(surface.neighbor_link[k].orientation<3,surface.neighbor_link[k].orientation);
				
				if (!surface.neighbor_link[k].surface || !(surface.neighbor_link[k].surface->flags&(::TBaseSurface::HasData)))
				{
					//cout << "warning open edge detected"<<endl;
					continue;
				}
				
				//ASSERT_EQUAL__(&surface, surface.neighbor_link[k].surface->neighbor_link[surface.neighbor_link[k].orientation].surface);
				//ASSERT_EQUAL__(k, surface.neighbor_link[k].surface->neighbor_link[surface.neighbor_link[k].orientation].orientation);
				
				if ((surface.flags&(::TBaseSurface::EdgeMergedOffset<<k)) && (surface.neighbor_link[k].surface->flags&(::TBaseSurface::EdgeMergedOffset<<surface.neighbor_link[k].orientation)))
					continue;	//both adjacent edges have been merged. no need to merge this edge
					
				
				if (!(surface.flags&(::TBaseSurface::EdgeMergedOffset<<k)) && !(surface.neighbor_link[k].surface->flags&(::TBaseSurface::EdgeMergedOffset<<surface.neighbor_link[k].orientation)))
				{
					//DEBUG_POINT(k)
					//cout << "full merge"<<endl;
					//neither this nor the other adjacent edge have been merged. Performing full merge
					parameter.that_surface = surface.neighbor_link[k].surface;
					

					//parameter.copy_to = !(parameter.that_surface.flags & ::TBaseSurface::HasChildren);
					
					parameter.this_border = borders[k].hostPointer();
					parameter.that_border = borders[surface.neighbor_link[k].orientation].hostPointer();
					
					//ASSERT_NOT_NULL__(parameter.surface.vertex);
					//ASSERT_NOT_NULL__(parameter.that_surface.vertex);
				
					/*
						Okay, so far the application mostly (not always) crashed during the following call.
						* all above operations succeeded without problem.
						* surface.neighbor_link[k].orientation is valid.
						* unless something seriously went wrong this_border and that_border are correct
						* with edge_length = 129 Emulation::process will operate linearily, not parallely. Synchronization issues are thus out of the question
						* both segments have data
						* both segments know of each other
						* vertex fields are not NULL
					
					*/
				

					//DEBUG_POINT(k)
					Emulation::process(emuMergeEdge,parameter,edge_length);	//<- crash point
					
				}
				elif (!(surface.flags&(::TBaseSurface::EdgeMergedOffset<<k)))	//this edge has not been merged (and is new compared to the neighboring edge)
				{
					//cout << "left merge"<<endl;
					//DEBUG_POINT(k)
					parameter.that_surface = surface.neighbor_link[k].surface;
					
					parameter.copy_to = false;
					
					parameter.this_border = borders[k].hostPointer();
					parameter.that_border = borders[surface.neighbor_link[k].orientation].hostPointer();
				
					//DEBUG_POINT(k)
					Emulation::process(emuCopyEdge,parameter,edge_length);
				}
				else	// this edge has been merged but not the opposing one
				{
					//cout << "right merge"<<endl;
					//DEBUG_POINT(k)
					parameter.that_surface = surface.neighbor_link[k].surface;
					
					parameter.copy_to = true;
					
					parameter.this_border = borders[k].hostPointer();
					parameter.that_border = borders[surface.neighbor_link[k].orientation].hostPointer();
				
					//DEBUG_POINT(k)
					Emulation::process(emuCopyEdge,parameter,edge_length);
				}

				//DEBUG_POINT(k)
				surface.flags |= (::TBaseSurface::EdgeMergedOffset<<k);
				surface.neighbor_link[k].surface->flags |= (::TBaseSurface::EdgeMergedOffset<<surface.neighbor_link[k].orientation);
				
				
				surface.neighbor_link[k].surface->vertex_field.signalHostMemoryAltered();
				//DEBUG_POINT(k)
			}
			surface.vertex_field.signalHostMemoryAltered();
			//DEBUG_POINT(k)
		}
		else
		{
			cudaGetLastError();
			unsigned blocks = ceilDiv(edge_length,block_size);
		
			dim3 block(block_x, block_y, 1);
			dim3 grid(blocks, 1, 1);

			TDeviceSurface	this_surface;
			((TBaseSurface&)this_surface) = surface;
			
			this_surface.vertex = surface.vertex_field.devicePointer();
			
			bool changed = false;
			
			

			for (BYTE k = 0; k < 3; k++)
			{
				ASSERT_EQUAL__(borders[k].length(),edge_length);
				if (!surface.neighbor_link[k].surface || !(surface.neighbor_link[k].surface->flags&(::TBaseSurface::HasData)))
				{
					//cout << "warning open edge detected"<<endl;
					continue;
				}
				if ((surface.flags&(::TBaseSurface::EdgeMergedOffset<<k)) && (surface.neighbor_link[k].surface->flags&(::TBaseSurface::EdgeMergedOffset<<surface.neighbor_link[k].orientation)))
					continue;	//both adjacent edges have been merged. no need to merge this edge
				
				TDeviceSurface	that_surface;
				((TBaseSurface&)that_surface) = *surface.neighbor_link[k].surface;
				ASSERT_EQUAL__(surface.neighbor_link[k].surface->vertex_field.length(),surface.vertex_field.length());
				that_surface.vertex = surface.neighbor_link[k].surface->vertex_field.devicePointer();
				ASSERT_NOT_NULL__(that_surface.vertex);
				
				
				if (!(surface.flags&(::TBaseSurface::EdgeMergedOffset<<k)) && !(surface.neighbor_link[k].surface->flags&(::TBaseSurface::EdgeMergedOffset<<surface.neighbor_link[k].orientation)))
				{
					//bool copy_to = !(that_surface.flags& ::TBaseSurface::HasChildren);
					
					mergeEdge<<< grid, block,0 >>>(this_surface,that_surface,context,device_info_field.devicePointer(),borders[k].devicePointer(),borders[surface.neighbor_link[k].orientation].devicePointer(),edge_length);
					checkStatus(CLOCATION);
					surface.neighbor_link[k].surface->vertex_field.signalDeviceMemoryAltered();
				}
				elif (!(surface.flags&(::TBaseSurface::EdgeMergedOffset<<k)))	//this edge has not been merged (and is new compared to the neighboring edge)
				{
					copyEdge<<< grid, block,0 >>>(this_surface,that_surface,context,device_info_field.devicePointer(),borders[k].devicePointer(),borders[surface.neighbor_link[k].orientation].devicePointer(),edge_length,false);
					checkStatus(CLOCATION);
				}
				else	// this edge has been merged but not the opposing one
				{
					copyEdge<<< grid, block,0 >>>(this_surface,that_surface,context,device_info_field.devicePointer(),borders[k].devicePointer(),borders[surface.neighbor_link[k].orientation].devicePointer(),edge_length,true);
					checkStatus(CLOCATION);
					surface.neighbor_link[k].surface->vertex_field.signalDeviceMemoryAltered();
				}
				surface.flags |= (::TBaseSurface::EdgeMergedOffset<<k);
				surface.neighbor_link[k].surface->flags |= (::TBaseSurface::EdgeMergedOffset<<surface.neighbor_link[k].orientation);
				changed = true;
			}
			if (changed)
			{
				checkStatus(CLOCATION);
				surface.vertex_field.signalDeviceMemoryAltered();
				cudaThreadSynchronize();
			}
			//cout << "merge edges"<<endl;
			
		}
		
		
	}

	void	updateTexture(THostSurface&surface, const TContext&context, CDeviceArray<TVertexInfo>&device_info_field, bool generate_clouds)
	{
		unsigned	range = context.edge_length,
					image_width = range+1+TEXTURE_SEAM_EXTENSION,	//full range plus 1 for diagonal +1 separation row. should suffice but may have to increase
					num_texels = (image_width) * (image_width),
					num_values = num_texels*(generate_clouds?4:3);
		//cout << image_width<<endl;
		//ASSERT1__(!(image_width%2),image_width);	//at the very least it should divisible by 2 //non-power-of-2 from now on
		size_t		data_size = sizeof(GLubyte)*num_values;
	
		if (Device::emulation)
		{
			Emulation::TProcessParameters	parameter;
			parameter.surface = &surface;
			parameter.context = &context;
			parameter.info_field = device_info_field.hostPointer();
			parameter.range = range;
			parameter.image_width = image_width;
			
			static CArray<BYTE>	write_buffer;
			write_buffer.resize(num_values);
			parameter.out = write_buffer;
			
			if (generate_clouds)
				Emulation::process(emuGenerateTextureC,parameter,device_info_field.length());
			else
				Emulation::process(emuGenerateTexture,parameter,device_info_field.length());
			
			
			/*
			CImage	test(image_width,image_width,3);
			test.read(write_buffer.pointer());
			png.saveToFile(test,"test.png");
			exit(0);
			*/
			
			surface.texture.load(write_buffer.pointer(),image_width,image_width,generate_clouds?4:3,false);
		}
		else
		{
			
			static CBufferObject	pbuffer;
				
			checkStatus(CLOCATION);
			
			
			pbuffer.resize(data_size);
			checkStatus(CLOCATION);
			
			
			
			GLubyte*texel_data;
		
			
			pbuffer.registerAndMap(texel_data);
			checkStatus(CLOCATION);
			
			
			TDeviceSurface	this_surface;
			((TBaseSurface&)this_surface) = surface;
			this_surface.vertex = surface.vertex_field.devicePointer();
			
			
			
			unsigned blocks = ceilDiv(device_info_field.length(),block_size);
		
			dim3 block(block_x, block_y, 1);
			dim3 grid(blocks, 1, 1);
			
			if (generate_clouds)
				generateTextureC<<< grid, block,0 >>>(texel_data,this_surface,context,device_info_field.devicePointer(),device_info_field.length(),image_width);
			else
				generateTexture<<< grid, block,0 >>>(texel_data,this_surface,context,device_info_field.devicePointer(),device_info_field.length(),image_width);
			checkStatus(CLOCATION);
			
			pbuffer.release();
			checkStatus(CLOCATION);
			
			/*surface.texture.resize(image_width,image_width);
			checkStatus(CLOCATION);*/

			surface.texture.load(pbuffer,image_width,image_width,generate_clouds?4:3);
			checkStatus(CLOCATION);
			cudaThreadSynchronize();
			
			//cout << "gen texture"<<endl;
		}
	}
	
	
	void	updateVBO(THostSurface&surface, const TContext&context, CDeviceArray<TVertexInfo>&device_info_field, CDeviceArray<unsigned>&device_index_field, unsigned range)
	{
		
		if (Device::emulation)
		{
			Emulation::TProcessParameters	parameter;
			parameter.surface = &surface;
			parameter.context = &context;
			parameter.info_field = device_info_field.hostPointer();
			parameter.index_field = device_index_field.hostPointer();
			parameter.range = range-1;
			
			static CArray<float>	write_buffer;
			write_buffer.resize(device_index_field.length()*9);
			parameter.fout = write_buffer;
			
			Emulation::process(emuGenerateVBO,parameter,device_index_field.length());
			
			surface.vbo.load(write_buffer.pointer(),write_buffer.contentSize());	//4vtx + 3norm + 2tcoord
		}
		else
		{
			//cudaGetLastError();
			checkStatus(CLOCATION);
			surface.vbo.resize(device_index_field.length()*9*sizeof(float));	//4vtx + 3norm + 2tcoord
			checkStatus(CLOCATION);
			TDeviceSurface	this_surface;
			((TBaseSurface&)this_surface) = surface;
			this_surface.vertex = surface.vertex_field.devicePointer();
			checkStatus(CLOCATION);
			
		
			unsigned blocks = ceilDiv(device_index_field.length(),block_size);
			dim3 block(block_x, block_y, 1);
			dim3 grid(blocks, 1, 1);

			float*vertex_data;
			
			surface.vbo.registerAndMap(vertex_data);
			
			checkStatus(CLOCATION);
			generateVBO<<< grid, block ,0 >>>(vertex_data,this_surface, context,device_info_field.devicePointer(), device_index_field.devicePointer(),device_index_field.length(), range-1);
			checkStatus(CLOCATION);

			surface.vbo.release();
			checkStatus(CLOCATION);
			cudaThreadSynchronize();
			//cout << "update vbo"<<endl;
			
		}
	}
	
	/*
	void	updateCloudVBO(THostSurface&surface, const TContext&context, CDeviceArray<unsigned>&device_cloud_triangles, unsigned range)
	{
		ASSERT1__(device_cloud_triangles.length()>0 && !(device_cloud_triangles.length()%3),device_cloud_triangles.length());
		TDeviceSurface	this_surface;
		((TBaseSurface&)this_surface) = surface;
		this_surface.vertex = surface.vertex_field.pointer();
		
		surface.cloud_vbo.resize(device_cloud_triangles.length()/3*9*sizeof(float));	//3 base + 3 ceiling + 3 norm
	
		unsigned blocks = ceilDiv(device_cloud_triangles.length()/3,block_size);
		dim3 block(block_dimension, block_dimension, 1);
		dim3 grid(blocks, 1, 1);

		float*vertex_data;
		
		surface.cloud_vbo.registerAndMap(vertex_data);
		
		generateCloudVBO<<< grid, block >>>(vertex_data,this_surface, context, device_cloud_triangles.pointer(),device_cloud_triangles.length(), range-1);
	
		surface.cloud_vbo.release();
	}
	*/
	
	void	retrieveCover(THostSurface&surface, const TContext&context, CDeviceArray<unsigned>&device_index_field, CArray<TCoverVertex>&out_cover)
	{
		if (Device::emulation)
		{
			out_cover.resize(device_index_field.length());
			Emulation::TProcessParameters	parameter;
			parameter.surface = &surface;
			parameter.context = &context;
			parameter.index_field = device_index_field.hostPointer();
			parameter.out_cover = out_cover;
			
			Emulation::process(emuGenerateCover,parameter,device_index_field.length());
		}
		else
		{
			//ASSERT__(!Device::emulation);
			static CDeviceArray<TCoverVertex>	out_vertices;
			
			cudaGetLastError();
			out_vertices.resize(device_index_field.length(),true);
			
			TDeviceSurface	this_surface;
			((TBaseSurface&)this_surface) = surface;
			this_surface.vertex = surface.vertex_field.devicePointer();
			
			unsigned blocks = ceilDiv(device_index_field.length(),block_size);
			dim3 block(block_x, block_y, 1);
			dim3 grid(blocks, 1, 1);
			
			generateCover<<< grid, block ,0 >>>(out_vertices.devicePointer(),this_surface, context, device_index_field.devicePointer(),device_index_field.length());
			
			out_vertices.signalDeviceMemoryAltered();
			
			out_vertices.downloadTo(out_cover);
			//cout << "get cover"<<endl;
			
		}
	}
	
	
	bool	rayCast(THostSurface&surface,const float b[3], const float d[3], CDeviceArray<unsigned>&triangles, TRayIntersection&intersection)
	{
		if (Device::emulation)
		{
			Emulation::TProcessParameters	parameter;
			parameter.surface = &surface;
			parameter.b = b;
			parameter.d = d;
			parameter.triangle_indices = triangles.hostPointer();
			parameter.intersection = &intersection;
			Emulation::process(emuCast,parameter,triangles.length()/3);
		}
		else
		{
			static CDeviceArray<TRayIntersection>	device_intersection;
			cudaGetLastError();
			device_intersection.resize(1,true);
			cudaMemset(device_intersection.devicePointer(), 0, sizeof(TRayIntersection));
			
			TDeviceSurface	this_surface;
			((TBaseSurface&)this_surface) = surface;
			this_surface.vertex = surface.vertex_field.devicePointer();
			
			unsigned blocks = ceilDiv(triangles.length()/3,block_size);
			dim3 block(block_x, block_y, 1);
			dim3 grid(blocks, 1, 1);
			
			cast<<< grid, block ,0 >>>(device_intersection.devicePointer(),this_surface, make_float3(b[0],b[1],b[2]), make_float3(d[0],d[1],d[2]), triangles.devicePointer(),triangles.length()/3);
			device_intersection.signalDeviceMemoryAltered();
			
			intersection = device_intersection[0];
		}
		if (intersection.isset)
		{
			lout << "intersection detected:"<<nl;
			lout << "  fc="<<_toString(intersection.fc)<<nl;
			lout << "  v[0]="<<_toString(intersection.vertex[0].position)<<nl;
			lout << "  v[1]="<<_toString(intersection.vertex[1].position)<<nl;
			lout << "  v[2]="<<_toString(intersection.vertex[2].position)<<nl;
		}
		//cout << (int)intersection.isset<<endl;
		return intersection.isset;
	}
	
	bool	groundQuery(THostSurface&surface, const float b[3], const float down[3], CDeviceArray<unsigned>&triangles, TGroundInfo&ground)
	{
	
		if (Device::emulation)
		{
			Emulation::TProcessParameters	parameter;
			parameter.surface = &surface;
			parameter.b = b;
			parameter.d = down;
			parameter.triangle_indices = triangles.hostPointer();
			parameter.ground = &ground;
			ground.isset = false;
			Emulation::process(emuGroundCast,parameter,triangles.length()/3);
		}
		else
		{
			static CDeviceArray<TGroundInfo>	device_ground;
			cudaGetLastError();
			device_ground.resize(1,true);
			cudaMemset(device_ground.devicePointer(), 0, sizeof(TGroundInfo));
			
			TDeviceSurface	this_surface;
			((TBaseSurface&)this_surface) = surface;
			this_surface.vertex = surface.vertex_field.devicePointer();
			
			unsigned blocks = ceilDiv(triangles.length()/3,block_size);
			dim3 block(block_x, block_y, 1);
			dim3 grid(blocks, 1, 1);
			
			groundCast<<< grid, block ,0 >>>(device_ground.devicePointer(),this_surface, make_float3(b[0],b[1],b[2]), make_float3(down[0],down[1],down[2]), triangles.devicePointer(),triangles.length()/3);
			
			device_ground.signalDeviceMemoryAltered();
			ground = device_ground[0];
			//cout << "ground query"<<endl;
			
		}
		return ground.isset;	
	}
	
	
	
	
}
