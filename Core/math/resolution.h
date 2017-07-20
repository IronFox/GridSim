#ifndef math_resolutionH
#define math_resolutionH

#include "../interface/hashable.h"
#include <algorithm>
#include "sizeND.h"

namespace DeltaWorks
{
	struct Resolution3D;

	struct Resolution : public Size2D<UINT32>
	{
	public:
		typedef Size2D<UINT32>	Super;

		/**/		Resolution() {}
		/**/		Resolution(UINT width,UINT height):Super(width,height){}
		explicit	Resolution(const Resolution3D&);
		float		GetPixelAspect()	const
		{
			return static_cast<float>(Super::width) / static_cast<float>(Super::height);
		}
		float		GetAspect()	const	{return GetPixelAspect();}
		count_t		CountPixels()	const	{return (count_t)Super::width*(count_t)Super::height;}

		bool		operator==(const Resolution&other)	const {return Super::operator==(other);}
		bool		operator!=(const Resolution&other)	const {return Super::operator!=(other);}
		Resolution	operator/(int div) const
		{
			return Resolution(std::max<UINT32>(1,Super::width/div),std::max<UINT32>(1,Super::height/div));
		}
		Resolution&	operator/=(int div)
		{
			Super::width = std::max<UINT32>(1,Super::width/div);
			Super::height = std::max<UINT32>(1,Super::height/div);
			return *this;
		}
		Resolution	operator*(int factor) const
		{
			return Resolution(std::max<UINT32>(1,Super::width*factor),std::max<UINT32>(1,Super::height*factor));
		}
		Resolution&	operator*=(int factor)
		{
			Super::width = std::max<UINT32>(1,Super::width*factor);
			Super::height = std::max<UINT32>(1,Super::height*factor);
			return *this;
		}

		friend hash_t	Hash(const Resolution&res)
		{
			return HashValue() << res.width << res.height;
		}

	};

	struct Resolution3D : public Size3D<UINT32>
	{
	public:
		typedef Size3D<UINT32>		Super;
		typedef Resolution3D		Self;


		/**/		Resolution3D(){}
		/**/		Resolution3D(UINT width,UINT height, UINT depth):Super(width,height,depth)	{}
		/**/		Resolution3D(const Resolution&res, UINT depth):Super(res.width,res.height,depth){}
		explicit	Resolution3D(UINT diagonalSize):Super(diagonalSize)	{}
		count_t		CountPixels()	const	{return (count_t)Super::width*(count_t)Super::height*(count_t)Super::depth;}
		float		GetFlatAspect()	const
		{
			return static_cast<float>(width) / static_cast<float>(height);
		}
		bool		operator==(const Resolution3D&other)	const	{return Super::operator==(other);}
		bool		operator!=(const Resolution3D&other)	const	{return Super::operator!=(other);}
		Self		operator/(int div) const
		{
			return Self(std::max<UINT32>(1,Super::width/div),std::max<UINT32>(1,Super::height/div),std::max<UINT32>(1,Super::depth/div));
		}
		Self&		operator/=(int div)
		{
			Super::width = std::max<UINT32>(1,Super::width/div);
			Super::height = std::max<UINT32>(1,Super::height/div);
			Super::depth = std::max<UINT32>(1,Super::depth/div);
			return *this;
		}
		Self		operator*(int factor) const
		{
			return Self(
					std::max<UINT32>(1,Super::width*factor),
					std::max<UINT32>(1,Super::height*factor),
					std::max<UINT32>(1,Super::depth*factor)
					);
		}
		Self&		operator*=(int factor)
		{
			Super::width = std::max<UINT32>(1,Super::width*factor);
			Super::height = std::max<UINT32>(1,Super::height*factor);
			Super::depth = std::max<UINT32>(1,Super::depth*factor);
			return *this;
		}

		friend hash_t	Hash(const Self&res)
		{
			return HashValue() << res.width << res.height << res.depth;
		}
	};

}

#endif
