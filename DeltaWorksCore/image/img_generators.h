#ifndef img_generatorsH
#define img_generatorsH

/******************************************************************

Predefined image generators.

******************************************************************/


#include "image.h"
#include "../math/vector.h"

namespace DeltaWorks
{

	class CGColor:public M::TVec4<>	//! Image generator color structure. A color structure holds one typename Nature::float_type_t for each red, green, blue, and alpha component.
	{
	public:
					CGColor(float red=1, float green=1, float blue=1, float alpha=1);	//! Constructor. Default color is opaque white.
					CGColor(const M::TVec4<>&v)	:M::TVec4<>(v)	{}
					CGColor(const M::TVec3<>&v, float alpha=1) {this->xyz = v; a = alpha;}
	};


	template <class Nature>
		void        igRadial(GenericImage<Nature>&target, const CGColor&inner, const CGColor&outer, bool use_cos=true);	//!< Same as the above for an existing image. The image is not resized but rather its existing dimensions used.

	template <class Nature>
		void        igExtRadial(GenericImage<Nature>&target, const CGColor&inner, const CGColor&outer, bool smooth=true);	//!< Same as the above for an existing image. The image is not resized but rather its existing dimensions used.

	template <class Nature>
		void		igNormalSphere(GenericImage<Nature> targets[6], UINT32 resolution);									//!< Generates all 6 sides of a normal sphere cube map. The normalized direction vectors are encoded in each texel

	 /*!
		\brief Generates a ring.
	
		igRing() generates a ring using 4 different color states:<ul>
		<li>The outer most 2% are filled using the outer color</li>
		<li>The next inner 3% are used to blend from the outer color to the outer ring</li>
		<li>The outer ring then blends to the inner ring in the specified width</li>
		<li>The inner ring border blends to the inner in the next 3%</li>
		<li>The inner circle is filled with the inner color</li>
		</ul><br>
		This function is useful for generating lens flare rings of different colors.
		It's recommended to assign alpha=0 to both the inner and outer colors.
	
		\param size Target dimension to use for both width and height of the resulting image
		\param width Width of the ring (0-0.95) starting at 95% (100%=image radius)
		\param outer Color to use for the outer most 2% and the corners
		\param outer_ring Color to use for the outer ring border
		\param inner_ring Color to use for the inner ring border
		\param inner Color to Fill the inner circle with
		\param use_cos Set true to use cosinus smoothing for the interpolation
		\return Pointer to a new Image object. The client application is responsible for discarding the returned object.
	*/
	template <class Nature>
		void        igRing(GenericImage<Nature>&target, typename Nature::float_type_t width, const CGColor&outer, const CGColor&outer_ring, const CGColor&inner_ring, const CGColor&inner, bool use_cos=true);	//!< Same as the above for an existing image. The image is not resized but rather its existing dimensions used.
	template <class Nature>
		void        igVertical(GenericImage<Nature>&target, const CGColor&top, const CGColor&center, const CGColor&bottom, bool use_cos=true);	//!< \overload
	template <class Nature>
		void        igHorizontal(GenericImage<Nature>&target, const CGColor&top, const CGColor&center, const CGColor&bottom, bool use_cos=true);	//!< \overload
	template <class Nature>
		void        igHorizontalCubic(GenericImage<Nature>&target, const CGColor&bottom, const CGColor&top);	//!< \overload
	template <class Nature>
		void        igHorizontalCubic(GenericImage<Nature>&target, const CGColor&bottom, const CGColor&top, UINT begin, UINT width);	//!< \overload
	template <class Nature>
		void        igVerticalSinus(GenericImage<Nature>&target, const CGColor&bottom, const CGColor&top);	//!< \overload
	template <class Nature>
		void		igCubic(GenericImage<Nature>&target, typename Nature::float_type_t frame_width, const CGColor&outer, const CGColor&inner, bool use_cos=true);	//!< \overload

	/**
		@brief draws a singular sun-flare-like stripe across the target image
	
		@param target Target image. The resulting image may appear stretched if the target image is not quadratic
		@param center_radius Radius of the sun on this image
		@param axis_x X-axis of the stripe axis vector. The axis vector (axis_x, axis_y) is required to be of length 1
		@param axis_y Y-axis of the stripe axis vector. The axis vector (axis_x, axis_y) is required to be of length 1
		@param angle Stripe angle. If this angle is zero then the result will be a bar of the size of the sun in the middle
		@param intensity Opacity of the stripe. 1 by default. If greater values are passed then the shape well tend more towards the inner color
		@param inner Inner (ie. highlighted) color of the stripe
		@param outer Outer (ie. background) color of the stripe texture
		@param smoothing Smoothing of the final stripe. 0.02 by default (optimized for 512x512).
	*/
	template <class Nature>
		void		igSolarStripe(GenericImage<Nature>&target, typename Nature::float_type_t center_radius, typename Nature::float_type_t axis_x, typename Nature::float_type_t axis_y, typename Nature::float_type_t angle, typename Nature::float_type_t intensity, const CGColor&inner, const CGColor&outer, typename Nature::float_type_t smoothing=0.02f);
	template <class Nature>
		void		igHexagon(GenericImage<Nature>&target, typename Nature::float_type_t smoothing, const CGColor&inner,const CGColor&border,const CGColor&outer);

	#include "img_generators.tpl.h"

}

#endif
