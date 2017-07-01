#ifndef fractal_kernelH
#define fractal_kernelH

#include "../../general/exception.h"
#include "../../global_string.h"

#include "../math/float_composite.h"

#include "../cuda_array.h"
#include "../../engine/gl/texture.h"
#include "../buffer_object.h"
#include "fractal_base.h"
#include "fractal_emulation.h"

#include "../cuda_rasterization.h"

//might be worth attempting to implement it using SSE when running via host emulation. Increased ram consumption though. Worth it?
//use macro defined(__SSE__) to distinguish

#include "debug_channel.h"

namespace CUDA
{


	/**
		@brief Attempts to initialize CUDA
		
		init() attempts to choose the first available device for CUDA calculations.
		
		@param relative_vram_usage Relative amount of vram that CUDA processing should be limited to in the range (0,1].
		@return true on success, false otherwise
	*/
	bool init(float relative_vram_usage=0.5f);

	/**
		@brief Asserts that CUDA is running flawlessly
	*/
	void checkStatus(const TCodeLocation&location);
	


	
	
	/**
		@brief Generates the data vertices  of a surface. The surface's device space vertex array is resized to match if needed
	*/
	void 			generateVertices(THostSurface&surface, THostSurface&parent, const TContext&context,CDeviceArray<TVertexInfo>&device_info_field);
	void 			generateNormals(THostSurface&surface, const TContext&context,CDeviceArray<TVertexInfo>&device_info_field, unsigned max_row);
	
	void			mergeEdges(THostSurface&surface, const TContext&context,CDeviceArray<TVertexInfo>&device_info_field, CDeviceArray<unsigned> borders[3], unsigned edge_length);
	
	void			updateTexture(THostSurface&surface, const TContext&context, CDeviceArray<TVertexInfo>&device_info_field, bool generate_clouds);
	void			updateVBO(THostSurface&surface, const TContext&context, CDeviceArray<TVertexInfo>&device_info_field, CDeviceArray<unsigned>&device_index_field, unsigned range);
	//void			updateCloudVBO(THostSurface&surface, const TContext&context, CDeviceArray<unsigned>&device_cloud_triangles, unsigned range);
	
	void			retrieveCover(THostSurface&surface, const TContext&context, CDeviceArray<unsigned>&device_index_field, CArray<TCoverVertex>&out_cover);
	
	void			getBoundingBox(THostSurface&surface,  float dim[6]);
	float			getBoundingRadius(THostSurface&surface,  const float center[3]);

	/**
		@brief Performs a ray intersection check of the specified surface and ray
		@param surface Surface to check against
		@param b Ray base point relative to the specified surface's sector
		@param d Normalized ray direction vector
		@param triangles Device space index array containing each three indices per triangle, which address the vertices of the specified surface
		@param intersection [out] Intersection data to hold detailed information on the detected intersection. Except for the @a isset member variable, the referenced structure remains unchanged if no intersection was detected
		@return true if an intersection was detected, false otherwise
	*/
	bool			rayCast(THostSurface&surface,const float b[3], const float d[3], CDeviceArray<unsigned>&triangles, TRayIntersection&intersection);
	/**
		@brief Performs a vertical ray intersection check to find a ground data beneath a specific location
		@param surface Surface to check against
		@param b Base point relative to the specified surface's sector
		@param down Normalized down vector to indicate the lookup direction. The factor of this vector and the surface (if any) is directly used to determine the height above ground
		@param triangles Device space index array containing each three indices per triangle, which address the vertices of the specified surface
		@param ground [out] Reference to a ground info struct to hold more detailed information.
		@return true if the specified point is above (or beneath) the specified surface, false if no intersection was detected
	*/
	bool			groundQuery(THostSurface&surface, const float b[3], const float down[3], CDeviceArray<unsigned>&triangles, TGroundInfo&ground);

	
	
	
	

}




#endif
