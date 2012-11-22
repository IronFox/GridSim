#ifndef cuda_fractal_emulationH
#define cuda_fractal_emulationH

#include "fractal_base.h"
#include "../cuda_array.h"

namespace CUDA
{

	namespace Emulation
	{
		struct TProcessParameters;
	}
}


void emuGenerateVertex(unsigned linear, CUDA::Emulation::TProcessParameters&parameter);
void emuWeightVertex(unsigned linear, CUDA::Emulation::TProcessParameters&parameter);
void emuGenerateNormal(unsigned linear, CUDA::Emulation::TProcessParameters&parameter);
void emuMergeEdge(unsigned linear, CUDA::Emulation::TProcessParameters&parameter);
void emuCopyEdge(unsigned linear, CUDA::Emulation::TProcessParameters&parameter);
void emuGenerateTexture(unsigned linear, CUDA::Emulation::TProcessParameters&parameter);
void emuGenerateTextureC(unsigned linear, CUDA::Emulation::TProcessParameters&parameter);
void emuGenerateVBO(unsigned linear, CUDA::Emulation::TProcessParameters&parameter);
void emuCast(unsigned linear, CUDA::Emulation::TProcessParameters&parameter);
void emuGroundCast(unsigned linear, CUDA::Emulation::TProcessParameters&parameter);
void emuGenerateCover(unsigned linear, CUDA::Emulation::TProcessParameters&parameter);



namespace CUDA
{
	namespace Emulation
	{
	
		/**
			@brief Support surface structure immitating the access structure of a TDeviceSurface in host memory
		*/
		struct TEmulationSurface:public TDeviceSurface
		{
				TEmulationSurface()
				{
					vertex = NULL;
				}
				
				TEmulationSurface(THostSurface*surface)
				{
					((TBaseSurface&)*this) = *surface;
					vertex = surface->vertex_field.hostPointer();
					ASSERT_NOT_NULL__(vertex);
				}
				
				void					operator=(THostSurface*surface)
				{
					((TBaseSurface&)*this) = *surface;
					vertex = surface->vertex_field.hostPointer();
					ASSERT_NOT_NULL__(vertex);
				}
		};	
	
		/**
			@brief Joint parameter construct
		*/
		struct TProcessParameters
		{
			TEmulationSurface					surface,
												that_surface,
												parent;
			const ::TContext					*context;
			::TVertexInfo						*info_field;
			union
			{
				unsigned						max_row;
				unsigned						edge_length;
				unsigned						range;
				unsigned						vertex_count;
			};
			unsigned							image_width;
			bool								copy_to;
			union
			{
				struct
				{
					unsigned					*this_border,
												*that_border;
				};
				unsigned						*index_field;
				unsigned						*triangle_indices;
			};
			union
			{
				BYTE							*out;
				float							*fout;
				TCoverVertex					*out_cover;
				struct
				{
					const float					*b,
												*d;
					union
					{
						TRayIntersection		*intersection;
						TGroundInfo				*ground;
					};
				};
			};
		};

		
		
		typedef void (*pMethod)(unsigned linear, TProcessParameters&parameter);
		
		/**
			@brief Begins parallel processing of the specified method
			@param method Method to execute each iteration
			@param parameter Parameter to pass to each method invocation
			@param iterations Number of iterations to process
		*/
		void									process(pMethod method, TProcessParameters&parameter, unsigned iterations);

		/**
			@brief Starts an asynchronous normal generation process on the internal thread kernel
			Completion may be awaited via awaitCompletion()
		*/
		void 									beginGenerateNormals(THostSurface&surface, const TContext&context,CDeviceArray<TVertexInfo>&device_info_field, unsigned max_row);
		/**
			@brief Starts an asynchronous vertex generation process on the internal thread kernel
			Completion may be awaited via awaitCompletion()
		*/
		void 									beginGenerateVertices(THostSurface&surface, THostSurface&parent, const TContext&context,CDeviceArray<TVertexInfo>&device_info_field);
		
		/**
			@brief Awaits completion of the last started begingenerateNormals() or beginGenerateVertices() process
			Not usable for any other process.
		*/
		void									awaitCompletion();
		
		/**
			@brief Starts an asynchronous texture generation process on the internal thread kernel
			Use finishUpdateTexture() to await completion.
		*/
		void									beginUpdateTexture(THostSurface&surface, const TContext&context, CDeviceArray<TVertexInfo>&device_info_field, unsigned range);
		void									finishUpdateTexture();
		
		/**
			@brief Starts an asynchronous VBO generation process on the internal thread kernel
			Use finishUpdateVBO() to await completion.
		*/
		void									beginUpdateVBO(THostSurface&surface, const TContext&context, CDeviceArray<TVertexInfo>&device_info_field, CDeviceArray<unsigned>&device_index_field, unsigned range);
		void									finishUpdateVBO();



	}

}







#endif
