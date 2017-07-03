#ifndef prandomH
#define prandomH

#include <time.h>
#include <math.h>
#include <atomic>
#include "../math/basic.h"
#include "../global_root.h"

/******************************************************************

E:\include\general\random.h

******************************************************************/
namespace DeltaWorks
{

	template <typename T>
		struct TRandomSeed
		{
			const T	value;

			/**/		TRandomSeed(const T&value):value(value)	{}
		};

	template <typename T>
		TRandomSeed<T>	RandomSeed(const T&value)	{return TRandomSeed<T>(value); }


	/**
	Pretty much the simplest possible random number source that still works to an acceptable degree.
	Made to be replacable by std::mt19937 for applications with higher requirements.
	Although SimpleRandomSource can produce only 2^15 different outputs, its internal number of states equals 2^32
	*/
	class SimpleRandomSource
	{
	private:
		UINT32									last;
		static const constexpr	UINT32			Mask = 32767;
		void									Advance();
	public:
		//configuration akin to std::mt19937:

		/**
		Type returned by queries to random numbers
		*/
		typedef	UINT							result_type;

		/**/									SimpleRandomSource() {};					//!< Constructs a new randomized Random object. Does not initialize the internal state
		explicit								SimpleRandomSource(result_type seed):last(seed){};		//!< Constructs a new Random object \param seed Custom seed to start from
		template <typename Sseq>
			explicit							SimpleRandomSource(Sseq&seq) {seq.generate(&last,(&last)+1);}

		/**
		Queries the highest possible value returned by pure random queries (inclusive)
		*/
		static const constexpr result_type		max() {return 32767;}
		/**
		Queries the lowest possible value returned by pure random queries (inclusive). Typically zero
		*/
		static const constexpr result_type		min() {return 0;}
		/**
		Advances the internal state and returns the next random integer
		*/
		result_type								operator()();
		/**
		Discards a number of random values
		*/
		void									discard(count_t);
		/**
		Redefines the internal state to the given seed
		*/
		void									seed(result_type seed)	{last = seed;}
		/**
		Redefines the internal state to the given seed sequence
		*/
		template <typename Sseq>
			void								seed(Sseq&seq)	{seq.generate(&last,(&last)+1);}



		//custom methods:
		/**
		Returns the current seed
		*/
		UINT32									GetCurrentSeed() const {return last;}
		/**
		Advances the internal state and returns the new seed
		*/
		UINT32									NextSeed();
	};


	extern std::atomic<index_t>		randomRandomizationModifier;

	/*!
	\brief Random generator class
	
	The Random class generates a sequence of pseudo random number in the range
	[Random::min(),Random::max()].
	Each Random object will auto randomize using the current time unless a custom
	seed is specified. The returned random sequence is deterministic if the used Random plugin is.

	The Random-template plugin is expected to produce equally distributed random numbers, 
	and must define/implement min(), max(), seed(), type_t and operator()().
	See SimpleRandomSource for details.
	*/
	template <typename Random>
		class RNG : public Random
		{
			typedef Random	Super;
		public:
			typedef typename Super::result_type	type_t;
			typedef typename TypeInfo<type_t>::GreaterType::Type big_type_t;

			/**/			RNG():Super((type_t)(time(NULL)*100 + randomRandomizationModifier++)){};
			explicit		RNG(type_t seed):Super(seed)	{}
			template <typename T>
				explicit	RNG(const TRandomSeed<T>& seed):Super((type_t)seed.value){};		//!< Constructs a new Random object \param seed Custom seed to start from
			template <typename Sseq>
				explicit	RNG(Sseq&seq):Super(seq){}

			/**
			Re-randomizes the internal state using the current time and an incremented global atomic integer
			*/
			void			Randomize()
			{
				Super::seed((type_t)(time(NULL)*100 + randomRandomizationModifier++));
			}
		
			type_t			MaxNext()
			{
				return Next(std::numeric_limits<type_t>::max());
			}

			/**
			Queries the next random value up to the specified max value (inclusive)
			*/
			type_t			Next(type_t max)
			{
				return std::min(max,type_t(big_type_t(Super::operator()()-Super::min()) * big_type_t(max+1) / big_type_t(Super::max() - Super::min())));
			}

			/**
			Queries the next random boolean
			*/
			bool			NextBool()
			{
				return Super::operator()() >= Super::max()/2;
			}

			/**
			Queries a random index up to range-1
			*/
			index_t			NextIndex(count_t range)
			{
				return std::min(range-1,index_t(big_type_t(Super::operator()()-Super::min()) * big_type_t(range) / big_type_t(Super::max() - Super::min())));
			}

			/**
			Queries a random value in the range [min,max]
			*/
			int				Next(int min, int max)
			{
				return min + std::min(max - min,int((big_type_t)(Super::operator()()-Super::min()) * (big_type_t)(max-min+1) / (big_type_t)(Super::max() - Super::min())));
			}
		
			/**
			Queries a random float point value in the range [0,1]
			*/
			template <typename Float>
				Float		NextFloatT()
				{
					return Float(Super::operator()() - Super::min()) / (Super::max() - Super::min());
				}
			/**
			Queries a random float point value in the range [0,max]
			*/
			template <typename Float>
				Float		NextFloatT(Float max)					//!< Query random float value \return Random float value ranging [0.0f,max]
				{
					return NextFloatT<Float>() * max;
				}
			/**
			Queries a random float point value in the range [min,max]
			*/
			template <typename Float>
				Float		NextFloatT(Float min, Float max)					//!< Query random float value \return Random float value ranging [min,max]
				{
					return NextFloatT<Float>() * (max - min) + min;
				}

			/**
			Queries a random float point value in the range [0,1]
			*/
			float			NextFloat()					//!< Query random float value \return Random float value ranging [0.0f,1.0f]
			{
				return NextFloatT<float>();
			}
			/**
			Queries a random float point value in the range [0,max]
			*/
			float			NextFloat(float max)			//!< Query random float value \param max Maximum random value \return Random float value ranging [0,max]
			{
				return NextFloat() * max;
			}
			/**
			Queries a random float point value in the range [min,max]
			*/
			float			NextFloat(float min, float max) //!< Query random float value \param min Minimum random value \param max Maximum random value \return Random float value ranging [min,max]
			{
				return NextFloat() * (max - min) + min;
			}

			/**
			Queries two random gaussian-distributed floating point values
			*/
			template <typename Type>
				void		NextGaussian(Type&y0, Type&y1)
				{
					Type w,x0,x1;
					do
					{
						x0 = Type(2) * NextFloatT<Type>() - Type(1);
						x1 = Type(2) * NextFloatT<Type>() - Type(1);
						w = x0*x0 + x1*x1;
					}
					while (w >= 1);
					w = sqrt((Type(-2) * log(w))/w);
					y0 = x0*w;
					y1 = x1*w;
				}
		};




	typedef RNG<SimpleRandomSource>	Random;

}

#endif
