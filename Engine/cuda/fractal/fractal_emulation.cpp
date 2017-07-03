#include "../../global_root.h"
#include "fractal_emulation.h"
#include "../../math/vector.h"
#include "../../math/object.h"
#include "../math/float_composite.h"
#include "../../general/thread_system.h"

#include "debug_channel.h"

//#define KERNEL_ALLOW_DEBUG



namespace CUDA
{
	namespace Emulation
	{

		static System::CKernel		kernel;
		//static System::CThreadPool		kernel(1);
		
		
		static CMutex					mutex;
		
		static TProcessParameters		*current_parameter;
		static	pMethod					current_method;
		static	unsigned				current_iterations;
		static const unsigned			job_size=128;
		static CThreadPoolContext		context;
		
		class CJob:public CThreadMain
		{
		public:
				unsigned		begin;
		
		
								CJob(unsigned index):begin(index*job_size)
								{}
								
				void			ThreadMain()
				{
					for (unsigned i = begin; i < begin+job_size && i < current_iterations; i++)
						current_method(i,*current_parameter);
				};
		
		
		};
		
		void	process(pMethod method, TProcessParameters&parameter, unsigned iterations)
		{
			//cout << "begin emulation of job "<<method<<" in "<<iterations<<" iterations"<<endl;
			if (iterations < job_size*4)
				for (unsigned i = 0; i < iterations; i++)
					method(i,parameter);
			else
			{
				ASSERT__(!kernel.busy(&context));
			//mutex.lock();
				//cout << "attempting parallel processing"<<endl;
					current_parameter = &parameter;
					current_method = method;
					current_iterations = iterations;
					static CArray<CThreadMain*>	field;
					unsigned num_jobs = iterations/job_size;
					if (iterations%job_size)
						num_jobs++;
					
					if (num_jobs > field.length())
					{
						unsigned old = field.length();
						field.resizeCopy(num_jobs);
						for (unsigned i = old; i < num_jobs; i++)
							field[i] = new CJob(i);
					}
					kernel.split(field,num_jobs,NULL,&context);
					kernel.waitUntilIdle(&context);
				//mutex.release();
			}
			//cout << "job completed"<<endl;
			//cout << "end operation"<<endl;
		}	
		
		TProcessParameters global_parameter;
		static void	begin(pMethod method, unsigned iterations)
		{
			ASSERT__(!kernel.busy(&context));
			//cout << "begin emulation of job "<<method<<" in "<<iterations<<" iterations"<<endl;
			current_parameter = &global_parameter;
			current_method = method;
			current_iterations = iterations;
			static CArray<CThreadMain*>	field;
			unsigned num_jobs = iterations/job_size;
			if (iterations%job_size)
				num_jobs++;
					
			if (num_jobs > field.length())
			{
				unsigned old = field.length();
				field.resizeCopy(num_jobs);
				for (unsigned i = old; i < num_jobs; i++)
					field[i] = new CJob(i);
			}
			kernel.split(field,num_jobs,NULL,&context);
		}
		
		static THostSurface*signal_surface	= NULL;
		
		void beginGenerateNormals(THostSurface&surface, const TContext&context,CDeviceArray<TVertexInfo>&device_info_field, unsigned max_row)
		{
			global_parameter.surface = &surface;
			global_parameter.context = &context;
			global_parameter.info_field = device_info_field.hostPointer();
			global_parameter.max_row = max_row;
			
			signal_surface = &surface;
			
			begin(emuGenerateNormal,device_info_field.length());
		}
		
		void beginGenerateVertices(THostSurface&surface, THostSurface&parent, const TContext&context,CDeviceArray<TVertexInfo>&device_info_field)
		{
			surface.vertex_field.resize(device_info_field.length(),false);
			global_parameter.surface = &surface;
			global_parameter.parent = &parent;
			global_parameter.context = &context;
			global_parameter.info_field = device_info_field.hostPointer();
			
			signal_surface = &surface;
			begin(emuGenerateVertex,device_info_field.length());
		}
		
		static CArray<BYTE>	image_write_buffer;
		static unsigned		image_width;
		static CArray<float>	vbo_write_buffer;
		
		void	beginUpdateTexture(THostSurface&surface, const TContext&context, CDeviceArray<TVertexInfo>&device_info_field, unsigned range)
		{
			image_width = range-1;
			unsigned	num_texels = image_width * image_width,
						num_values = num_texels*3;
			ASSERT1__(!(image_width%2),image_width);	//at the very least it should divisible by 2
			size_t		data_size = sizeof(GLubyte)*num_values;
		
			global_parameter.surface = &surface;
			global_parameter.context = &context;
			global_parameter.info_field = device_info_field.hostPointer();
			global_parameter.range = range;
			global_parameter.image_width = image_width;
			
			image_write_buffer.resize(num_values);
			global_parameter.out = image_write_buffer;
			
			signal_surface = &surface;
			
			begin(emuGenerateTexture,device_info_field.length());
		}
		
		void	finishUpdateTexture()
		{
			kernel.waitUntilIdle(&context);

			signal_surface->texture.load(image_write_buffer.pointer(),image_width,image_width,3,false);
		}
		
		void	beginUpdateVBO(THostSurface&surface, const TContext&context, CDeviceArray<TVertexInfo>&device_info_field, CDeviceArray<unsigned>&device_index_field, unsigned range)
		{
			global_parameter.surface = &surface;
			global_parameter.context = &context;
			global_parameter.info_field = device_info_field.hostPointer();
			global_parameter.index_field = device_index_field.hostPointer();
			global_parameter.range = range-1;
			
			vbo_write_buffer.resize(device_index_field.length()*9);
			global_parameter.fout = vbo_write_buffer;
			
			signal_surface = &surface;
			
			begin(emuGenerateVBO,device_index_field.length());
		}
		
		void	finishUpdateVBO()
		{
			kernel.waitUntilIdle(&context);
			
			signal_surface->vbo.load(vbo_write_buffer.pointer(),vbo_write_buffer.contentSize());	//4vtx + 3norm + 2tcoord
		}
		
		
		
		
		void awaitCompletion()
		{
			kernel.waitUntilIdle(&context);
			if (signal_surface)
				signal_surface->vertex_field.signalHostMemoryAltered();
		}
		
	}
}









#define dbClamped	clamped
#define fminf		vmin
#define fmaxf		vmax
#define dbSign		sign
#define dbCosStep	cosStep
#define dbCubicStep	cubicStep
#define dbSqr		sqr
#define cSub		Composite::sub
#define dvMad		_mad
#define dvNormalize	_normalize
#define dvDot		_dot
#define dvDistance	_distance
#define dvMult		_mult
#define dvCenter	_center
#define dvAdd		_add
#define dvSub		_sub
#define dvLength	_length
#define dvClear		_clear
#define dvC3		_c3
#define dvC4		_c4
#define dvCross		_cross
#define dbLinearStep	linearStep
#define doAddTriangleNormal	_oAddTriangleNormal
#define __powf		powf
#define DEVICE_RESOURCE(_TYPE_,_NAME_)	const _TYPE_&_NAME_

#include "kernel/config.h"



	static inline float	ageAt(unsigned depth)
	{
		#include "kernel/age_at.function"
	}
	
	static inline float	getWeight(float h, float average, float variance, float depth)
	{
		#include "kernel/get_weight.function"
	}
	
	static inline float	getRandom(int&seed)
	{
		#include "kernel/get_random.function"
	}

	static inline float	height(const TChannelConfig&channel)
	{
		#include "kernel/height.function"
	}

	
	static inline float		getNoise(float distance, const TVertex&p0, const TVertex&p1, const TDeviceSurface&surface, const TContext&context)
	{
		#include "kernel/get_noise.function"
	}

/*	static inline float		getSmoothStrength(float distance, const TVertex&p0, const TVertex&p1, const TDeviceSurface&surface, const TContext&context)
	{
		#include "kernel/get_smooth_strength.function"
	}*/

	static inline void		generate3(const TVertex&p0, const TVertex&p1, const TVertex&p2, TVertex&result, unsigned seed, TDeviceSurface&surface, const TContext&context)
	{
		#include "kernel/generate3.function"
	}

	static inline void		generate4(const TVertex&p0, const TVertex&p1, const TVertex&p2, const TVertex&p3, TVertex&result, int seed, TDeviceSurface&surface, const TContext&context)
	{
		#include "kernel/generate4.function"
	}

	static inline void		generate4edge(const TVertex&p0, const TVertex&p1, const TVertex&p2, const TVertex&p3, TVertex&result, int seed, TDeviceSurface&surface, const TContext&context)
	{
		#include "kernel/generate4_edge.function"
	}

	
	static inline void		setHeight(float vector[3],float relative_height,const TDeviceSurface&surface,const ::TContext&context)
	{
		#include "kernel/set_height.function"
	}
	
	static inline void		setHeight(float vector[3],float current_height, float relative_height,const TDeviceSurface&surface,const ::TContext&context)
	{
		#include "kernel/differential_set_height.function"
	}

	static inline unsigned	vertexIndex(unsigned x, unsigned y)
	{
		return y*(y+1)/2+x;
	}

	static inline unsigned char*	getTexel3(unsigned char*texel_data, unsigned x, unsigned y, unsigned dimension)
	{
		return texel_data+(y*dimension+x)*3;
	}
	static inline unsigned char*	getTexel4(unsigned char*texel_data, unsigned x, unsigned y, unsigned dimension)
	{
		return texel_data+(y*dimension+x)*4;
	}
	
	static inline unsigned char*	getTexel1(unsigned char*texel_data, unsigned x, unsigned y, unsigned dimension)
	{
		return texel_data+(y*dimension+x);
	}
	
	static inline	void	resolveDirection(const TVertex&vertex, const TDeviceSurface&surface, const TContext&context, float v[3])
	{
		v[0] = vertex.position[0]+context.sector_size*surface.sector[0];
		v[1] = vertex.position[1]+context.sector_size*surface.sector[1];
		v[2] = vertex.position[2]+context.sector_size*surface.sector[2];
		dvNormalize(v);
	}	
	
	static inline bool _atomicCAS(int*pntr, int compare, int set)
	{
		//return atomicCAS(pntr, compare, set)==set;	//wish i could use this...
		
		if (*pntr != compare)
			return false;
		*pntr = set;
		return true;
	}




void emuGenerateVertex(unsigned linear, CUDA::Emulation::TProcessParameters&parameter)
{
	//cout << "begin"<<endl;
	const TVertexInfo*info_field = parameter.info_field;
	TDeviceSurface		&surface = parameter.surface;
	const TDeviceSurface&parent = parameter.parent;
	const TContext&context = *parameter.context;
	//cout << "context "<<parameter.context<<endl;
	
	//cout << "forward"<<endl;
	#include "kernel/generate_vertex.function"

	
}

/*
void emuWeightVertex(unsigned linear, CUDA::Emulation::TProcessParameters&parameter)
{
	//cout << "begin"<<endl;
	const TVertexInfo*info_field = parameter.info_field;
	TDeviceSurface		&surface = parameter.surface;
	const TContext&context = *parameter.context;
	//cout << "context "<<parameter.context<<endl;
	
	//cout << "forward"<<endl;
	#include "kernel/weight_vertex.function"

	
}*/

void emuGenerateNormal(unsigned linear, CUDA::Emulation::TProcessParameters&parameter)
{
	const TVertexInfo*info_field = parameter.info_field;
	TVertex*vertices = parameter.surface.vertex;
	unsigned max_row = parameter.max_row;
	
	#include "kernel/generate_normal.function"
}


void emuCopyEdge(unsigned linear, CUDA::Emulation::TProcessParameters&parameter)
{
	const TVertexInfo*info_field = parameter.info_field;
	TDeviceSurface		&this_surface = parameter.surface,
						&that_surface = parameter.that_surface;
	const TContext&context = *parameter.context;
	bool copy_to = parameter.copy_to;
	unsigned edge_length = parameter.edge_length;
	
	const unsigned	*edge = parameter.this_border,
					*neighbor_edge = parameter.that_border;

	#include "kernel/copy_edge.function"
}


void emuMergeEdge(unsigned linear, CUDA::Emulation::TProcessParameters&parameter)
{
	const TVertexInfo*info_field = parameter.info_field;
	TDeviceSurface		&this_surface = parameter.surface,
						&that_surface = parameter.that_surface;
	//ASSERT_NOT_NULL__(this_surface.vertex);
	//ASSERT_NOT_NULL__(that_surface.vertex);
	const TContext&context = *parameter.context;
	//bool copy_to = parameter.copy_to;
	unsigned edge_length = parameter.edge_length;
	
	const unsigned	*edge = parameter.this_border,
					*neighbor_edge = parameter.that_border;

	#include "kernel/merge_edge.function"
}

void emuGenerateTexture(unsigned linear, CUDA::Emulation::TProcessParameters&parameter)
{
	const TVertexInfo*info_field = parameter.info_field;
	unsigned dimension = parameter.image_width;
	const TContext&context = *parameter.context;
	BYTE*texel_data = parameter.out;
	TDeviceSurface		&surface = parameter.surface;
	
	//unsigned x,y;

	#include "kernel/generate_texture.function"
}


void emuGenerateTextureC(unsigned linear, CUDA::Emulation::TProcessParameters&parameter)
{
	const TVertexInfo*info_field = parameter.info_field;
	unsigned dimension = parameter.image_width;
	const TContext&context = *parameter.context;
	BYTE*texel_data = parameter.out;
	TDeviceSurface		&surface = parameter.surface;
	
	//unsigned x,y;

	#include "kernel/generate_texture_c.function"
}



void emuGenerateVBO(unsigned linear, CUDA::Emulation::TProcessParameters&parameter)
{
	float*vertex_data = parameter.fout;
	const unsigned*index_field = parameter.index_field;
	TDeviceSurface		&surface = parameter.surface;
	unsigned vertex_max = parameter.range;
	const TVertexInfo*info_field = parameter.info_field;
	const TContext&context = *parameter.context;
	
	#include "kernel/generate_vbo.function"
}


void emuCast(unsigned linear, CUDA::Emulation::TProcessParameters&parameter)
{
	TDeviceSurface		&surface = parameter.surface;
	const unsigned*triangle_indices = parameter.triangle_indices;
	const float*b = parameter.b;
	const float*d = parameter.d;
	TRayIntersection*intersection = parameter.intersection;
	
	#include "kernel/cast.function"
}


void emuGroundCast(unsigned linear, CUDA::Emulation::TProcessParameters&parameter)
{
	TDeviceSurface		&surface = parameter.surface;
	const unsigned*triangle_indices = parameter.triangle_indices;
	const float*b = parameter.b;
	const float*d = parameter.d;
	TGroundInfo*ground = parameter.ground;
	
	#include "kernel/ground_cast.function"
}

void emuGenerateCover(unsigned linear, CUDA::Emulation::TProcessParameters&parameter)
{
	TDeviceSurface		&surface = parameter.surface;
	TCoverVertex*out_vertices = parameter.out_cover;
	const unsigned*index_field = parameter.index_field;
	const TContext&context = *parameter.context;
	

	#include "kernel/generate_cover.function"
}

