#pragma once
#include "matrix.h"


namespace Math
{
	
	
	/**
	Nth degree polynomial representation
	*/
	template <typename T, count_t NumCoef>
		struct Polynomial
		{
			static const constexpr count_t NumCoefficients = NumCoef;
			TVec<T,NumCoef>	coefficients;

			T			operator()(const T x) const
			{
				T y = 0;
				for (index_t i = 0; i < NumCoef; i++)
				{
					y = coefficients.v[NumCoef -i-1] + y * x;
				}
				return y;
			}

			/**
			Matches the local polynomial to match the given set of points.
			The X-coordinates of all points must be pairwise different.
			*/
			bool			MatchPoints(const std::array<TVec2<T>,NumCoef>&points)
			{
				TMatrix<T,NumCoef,NumCoef>	mX,mInv;
				TVec<T,NumCoef>	vY;

				const index_t lastSample = points.size()-1;

				for (index_t at = 0; at < NumCoef; at++)
				{

					vY.v[at] = points[at].y;

					T fx = points[at].x;
					for (index_t e = 0; e < NumCoef; e++)
					{
						T xN = vpow(fx,e);
						mX.axis[e].v[at] = xN;
					}
				}
				if (!Mat::Invert(mX,mInv))
					return false;
				Mat::Mult(mInv,vY,this->coefficients);
				return true;
			}


			
			bool		Approximate(const ArrayData<TVec2<T> >&points)
			{
				Array2D<double> XT(NumCoef,points.Count()),X(points.Count(),NumCoef);
				Array<double> yv(points.Count());
				for (index_t i = 0; i < points.Count(); i++)
				{
					float x = points[i].x;
					float y = points[i].y;
					yv[i] = y;
					for (index_t e = 0; e < NumCoef; e++)
					{
						X.Get(i,e) = XT.Get(e,i) = pow(x,e);
					}
				}
				Array2D<double> product{NumCoef,NumCoef},inverted{NumCoef,NumCoef},finalProduct{NumCoef,points.Count()};
				Mat::Raw::MultD(XT.pointer(),X.pointer(),product.pointer(),NumCoef,points.Count(),NumCoef);
				bool success = Mat::Raw::InvertD(product.pointer(),inverted.pointer(),NumCoef);
				if (!success)
					return false;
				Mat::Raw::MultD(inverted.pointer(),XT.pointer(),finalProduct.pointer(),NumCoef,NumCoef,points.Count());
				Mat::Raw::MultD(finalProduct.pointer(),yv.pointer(),coefficients.v,NumCoef,points.Count(),1);
				return true;
			}
		};
	

	/**
	Nth degree polynomial representation, using negative exponents
	*/
	template <typename T, count_t NumCoef>
		struct NegativePolynomial
		{
			static const constexpr count_t NumCoefficients = NumCoef;
			TVec<T,NumCoef>	coefficients;

			T			operator()(const T x) const
			{
				T y = 0;
				T x1 = 1;
				for (index_t i = 0; i < NumCoef; i++)
				{
					y += coefficients.v[i] / x1;
					x1 *= x;
				}
				return y;
			}

			/**
			Matches the local polynomial to match the given set of points.
			The X-coordinates of all points must be pairwise different.
			*/
			bool			MatchPoints(const std::array<TVec2<T>,NumCoef>&points)
			{
				TMatrix<T,NumCoef,NumCoef>	mX,mInv;
				TVec<T,NumCoef>	vY;

				const index_t lastSample = points.size()-1;

				for (index_t at = 0; at < NumCoef; at++)
				{

					vY.v[at] = points[at].y;

					T fx = points[at].x;
					for (int e = 0; e < (int)NumCoef; e++)
					{
						T xN = vpow(fx,-e);
						mX.axis[e].v[at] = xN;
					}
				}
				if (!Mat::Invert(mX,mInv))
					return false;
				Mat::Mult(mInv,vY,this->coefficients);
				return true;
			}
			
			bool		Approximate(const ArrayData<TVec2<T> >&points)
			{
				Array2D<double> XT(NumCoef,points.Count()),X(points.Count(),NumCoef);
				Array<double> yv(points.Count());
				for (index_t i = 0; i < points.Count(); i++)
				{
					float x = points[i].x;
					float y = points[i].y;
					yv[i] = y;
					for (index_t e = 0; e < NumCoef; e++)
					{
						X.Get(i,e) = XT.Get(e,i) = pow(x,-(int)e);
					}
				}
				Array2D<double> product{NumCoef,NumCoef},inverted{NumCoef,NumCoef},finalProduct{NumCoef,points.Count()};
				Mat::Raw::MultD(XT.pointer(),X.pointer(),product.pointer(),NumCoef,points.Count(),NumCoef);
				bool success = Mat::Raw::InvertD(product.pointer(),inverted.pointer(),NumCoef);
				if (!success)
					return false;
				Mat::Raw::MultD(inverted.pointer(),XT.pointer(),finalProduct.pointer(),NumCoef,NumCoef,points.Count());
				Mat::Raw::MultD(finalProduct.pointer(),yv.pointer(),coefficients.v,NumCoef,points.Count(),1);
				return true;
			}

		};
	
	
}
