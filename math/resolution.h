#ifndef math_resolutionH
#define math_resolutionH

#include "../interface/hashable.h"

struct Resolution
{
public:
		struct Hash
		{
		static	hash_t				hash(const Resolution&res);
		};


		UINT						width,
									height;


									Resolution():width(0),height(0)
									{}
									Resolution(UINT width_,UINT height_):width(width_),height(height_)
									{}
		void						set(UINT width_, UINT height_)
									{
										width = width_;
										height = height_;
									}
		float						aspect()	const
									{
										return static_cast<float>(width) / static_cast<float>(height);
									}
		bool						operator==(const Resolution&other)	const
									{
										return width == other.width && height == other.height;
									}
		bool						operator!=(const Resolution&other)	const
									{
										return width != other.width || height != other.height;
									}
		Resolution					operator/(int div) const
									{
										return Resolution(std::max<UINT>(1,width/div),std::max<UINT>(1,height/div));
									}

};




#endif
