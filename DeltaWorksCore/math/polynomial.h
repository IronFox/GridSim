#pragma once
#include "matrix.h"

#include "../container/array2d.h"

namespace DeltaWorks
{

	namespace Math
	{

		/**
		N*Mth degree polynomial representation, using negative exponents
		*/
		template <typename T, count_t NumCoefX, count_t NumCoefY>
			struct Polynomial2D
			{
				static const constexpr count_t NumCoefficientsX = NumCoefX;
				static const constexpr count_t NumCoefficientsY = NumCoefY;
				TMatrix<T,NumCoefX,NumCoefY> coefficients;

				T			operator()(const T x, const T y) const
				{
					T z = 0;
					T y1 = 1;
					for (index_t iy = 0; iy < NumCoefY; iy++)
					{
						T x1 = 1;
						for (index_t ix = 0; ix < NumCoefX; ix++)
						{
							z += coefficients.axis[ix].v[iy] * y1 * x1;
							x1 *= x;
						}
						y1 *= y;
					}
					return z;
				}

				/**
				Matches the local polynomial to match the given set of points.
				The X-coordinates of all points must be pairwise different.
				*/
				bool			MatchPoints(const std::array<M::TVec3<T>,NumCoefX*NumCoefY>&points)
				{
					static const count_t TotCoef =  NumCoefX * NumCoefY;
					TMatrix<T,TotCoef,TotCoef>	mX,mInv;
					TVec<T,TotCoef>	vY;

					for (index_t at = 0; at < TotCoef; at++)
					{
						const M::TVec3<T>&p = points[at];
						vY.v[at] = p.z;

						const T fx = p.x;
						const T fy = p.y;
						for (index_t eX = 0; eX < NumCoefX; eX++)
						for (index_t eY = 0; eY < NumCoefY; eY++)
						{
							const index_t eat = eX * NumCoefY + eY;
							T xN = vpow(fx,eX) * vpow(fy,eY);
							mX.axis[eat].v[at] = xN;
						}
					}
					if (!Mat::Invert(mX,mInv))
						return false;
					Mat::Mult(mInv,vY, this->coefficients.asVector);
					return true;
				}
			
				bool		Approximate(const Ctr::ArrayRef<M::TVec3<T> >&points)
				{
					static const count_t TotCoef =  NumCoefX * NumCoefY;

					Array2D<T> XT(TotCoef,points.Count()),X(points.Count(),TotCoef);
					Ctr::Array<T> yv(points.Count());
					for (index_t i = 0; i < points.Count(); i++)
					{
						const M::TVec3<T>&p = points[i];
						yv[i] = p.z;
						for (index_t eX = 0; eX < NumCoefX; eX++)
						for (index_t eY = 0; eY < NumCoefY; eY++)
						{
							const index_t eat = eX * NumCoefY + eY;
							T xN = vpow(p.x,eX) * vpow(p.y,eY);
							X.Get(i,eat) = XT.Get(eat,i) = xN;
						}
					}
					Array2D<T> product{TotCoef,TotCoef},inverted{TotCoef,TotCoef},finalProduct{TotCoef,points.Count()};
					Mat::Raw::MultD(XT.pointer(),X.pointer(),product.pointer(),TotCoef,points.Count(),TotCoef);
					bool success = Mat::Raw::InvertD(product.pointer(),inverted.pointer(),TotCoef);
					if (!success)
						return false;
					Mat::Raw::MultD(inverted.pointer(),XT.pointer(),finalProduct.pointer(),TotCoef,TotCoef,points.Count());
					Mat::Raw::MultD(finalProduct.pointer(),yv.pointer(),coefficients.v,TotCoef,points.Count(),1);
					return true;
				}

			};
	
	
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
				bool			MatchPoints(const std::array<M::TVec2<T>,NumCoef>&points)
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


			
				bool		Approximate(const Ctr::ArrayRef<M::TVec2<T> >&points)
				{
					Array2D<T> XT(NumCoef,points.Count()),X(points.Count(),NumCoef);
					Ctr::Array<T> yv(points.Count());
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
					Array2D<T> product{NumCoef,NumCoef},inverted{NumCoef,NumCoef},finalProduct{NumCoef,points.Count()};
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
				bool			MatchPoints(const std::array<M::TVec2<T>,NumCoef>&points)
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
			
				bool		Approximate(const Ctr::ArrayRef<M::TVec2<T> >&points)
				{
					Array2D<T> XT(NumCoef,points.Count()),X(points.Count(),NumCoef);
					Ctr::Array<T> yv(points.Count());
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
					Array2D<T> product{NumCoef,NumCoef},inverted{NumCoef,NumCoef},finalProduct{NumCoef,points.Count()};
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
		N*Mth degree polynomial representation, using negative exponents
		*/
		template <typename T, count_t NumCoefX, count_t NumCoefY>
			struct NegativePolynomial2D
			{
				static const constexpr count_t NumCoefficientsX = NumCoefX;
				static const constexpr count_t NumCoefficientsY = NumCoefY;
				TMatrix<T,NumCoefX,NumCoefY> coefficients;

				T			operator()(const T x, const T y) const
				{
					T z = 0;
					T y1 = 1;
					for (index_t iy = 0; iy < NumCoefY; iy++)
					{
						T y2 = (T)1 / y1;
						T x1 = 1;
						for (index_t ix = 0; ix < NumCoefX; ix++)
						{
							z += coefficients.axis[ix].v[iy] * y2 / x1;
							x1 *= x;
						}
						y1 *= y;
					}
					return z;
				}

				/**
				Matches the local polynomial to match the given set of points.
				The X-coordinates of all points must be pairwise different.
				*/
				bool			MatchPoints(const std::array<M::TVec3<T>,NumCoefX*NumCoefY>&points)
				{
					static const count_t TotCoef =  NumCoefX * NumCoefY;
					TMatrix<T,TotCoef,TotCoef>	mX,mInv;
					TVec<T,TotCoef>	vY;

					for (index_t at = 0; at < TotCoef; at++)
					{
						const M::TVec3<T>&p = points[at];
						vY.v[at] = p.z;

						const T fx = p.x;
						const T fy = p.y;
						for (index_t eX = 0; eX < NumCoefX; eX++)
						for (index_t eY = 0; eY < NumCoefY; eY++)
						{
							const index_t eat = eX * NumCoefY + eY;
							T xN = (T)1 / vpow(fx,eX) / vpow(fy,eY);
							mX.axis[eat].v[at] = xN;
						}
					}
					if (!Mat::Invert(mX,mInv))
						return false;
					Mat::Mult(mInv,vY, this->coefficients.asVector);
					return true;
				}
			
				bool		Approximate(const Ctr::ArrayRef<M::TVec3<T> >&points)
				{
					static const count_t TotCoef =  NumCoefX * NumCoefY;

					Array2D<T> XT(TotCoef,points.Count()),X(points.Count(),TotCoef);
					Ctr::Array<T> yv(points.Count());
					for (index_t i = 0; i < points.Count(); i++)
					{
						const M::TVec3<T>&p = points[i];
						yv[i] = p.z;
						for (index_t eX = 0; eX < NumCoefX; eX++)
						for (index_t eY = 0; eY < NumCoefY; eY++)
						{
							const index_t eat = eX * NumCoefY + eY;
							T xN = (T)1 / vpow(p.x,eX) / vpow(p.y,eY);
							X.Get(i,eat) = XT.Get(eat,i) = xN;
						}
					}
					Array2D<T> product{TotCoef,TotCoef},inverted{TotCoef,TotCoef},finalProduct{TotCoef,points.Count()};
					Mat::Raw::MultD(XT.pointer(),X.pointer(),product.pointer(),TotCoef,points.Count(),TotCoef);
					bool success = Mat::Raw::InvertD(product.pointer(),inverted.pointer(),TotCoef);
					if (!success)
						return false;
					Mat::Raw::MultD(inverted.pointer(),XT.pointer(),finalProduct.pointer(),TotCoef,TotCoef,points.Count());
					Mat::Raw::MultD(finalProduct.pointer(),yv.pointer(),coefficients.v,TotCoef,points.Count(),1);
					return true;
				}

			};
	}
}
