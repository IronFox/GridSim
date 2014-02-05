#ifndef math_resolutionH
#define math_resolutionH

#include "../interface/hashable.h"


struct Resolution3D;

struct Resolution
{
public:
		struct Hash
		{
		static	hash_t				hash(const Resolution&res);
		};


		UINT32						width,
									height;


									Resolution():width(0),height(0)
									{}
									Resolution(UINT width_,UINT height_):width(width_),height(height_)
									{}
		explicit					Resolution(const Resolution3D&);
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
										return Resolution(std::max<UINT32>(1,width/div),std::max<UINT32>(1,height/div));
									}
		Resolution&					operator/=(int div)
									{
										width = std::max<UINT32>(1,width/div);
										height = std::max<UINT32>(1,height/div);
										return *this;
									}
		Resolution					operator*(int factor) const
									{
										return Resolution(std::max<UINT32>(1,width*factor),std::max<UINT32>(1,height*factor));
									}
		Resolution&					operator*=(int factor)
									{
										width = std::max<UINT32>(1,width*factor);
										height = std::max<UINT32>(1,height*factor);
										return *this;
									}
};

struct Resolution3D
{
public:
		struct Hash
		{
		static	hash_t				hash(const Resolution3D&res);
		};


		UINT32						width,
									height,
									depth;


									Resolution3D():width(0),height(0),depth(0)
									{}
									Resolution3D(UINT width,UINT height, UINT depth):width(width),height(height),depth(depth)
									{}
									Resolution3D(const Resolution&res, UINT depth):width(res.width),height(res.height),depth(depth)
									{}
		void						Set(UINT width_, UINT height_, UINT depth_)
									{
										width = width_;
										height = height_;
										depth = depth_;
									}

		bool						operator==(const Resolution3D&other)	const
									{
										return width == other.width && height == other.height && depth == other.depth;
									}
		bool						operator!=(const Resolution3D&other)	const
									{
										return width != other.width || height != other.height || depth != other.depth;
									}
		Resolution3D				operator/(int div) const
									{
										return Resolution3D(std::max<UINT32>(1,width/div),std::max<UINT32>(1,height/div),std::max<UINT32>(1,depth/div));
									}
		Resolution3D&				operator/=(int div)
									{
										width = std::max<UINT32>(1,width/div);
										height = std::max<UINT32>(1,height/div);
										depth = std::max<UINT32>(1,depth/div);
										return *this;
									}
		Resolution3D				operator*(int factor) const
									{
										return Resolution3D(
												std::max<UINT32>(1,width*factor),
												std::max<UINT32>(1,height*factor),
												std::max<UINT32>(1,depth*factor)
												);
									}
		Resolution3D&				operator*=(int factor)
									{
										width = std::max<UINT32>(1,width*factor);
										height = std::max<UINT32>(1,height*factor);
										depth = std::max<UINT32>(1,depth*factor);
										return *this;
									}
};



#endif
