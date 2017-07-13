//#include <global_root.h>
//#include <global_string.h>
//#include <container/buffer.h>
//#include <general/random.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <array>
#include <iostream>
#include <fstream>
#include <locale>
#include <sstream>
#include <future>
#include <algorithm>
#include <iterator>

#include <stdint.h>

#ifdef _WIN32
	#include <windows.h>
	#undef min
	#undef max
	#include <ppl.h>
#else
	#include <tbb/parallel_for.h>
	#include <sched.h>
	namespace Concurrency = tbb;
#endif

//#include <global_root.h>
//#include <io/delta_crc32.h>

typedef double density_t;


typedef	uint64_t	UINT64;
typedef	uint32_t	UINT32;
typedef	uint16_t	UINT16;
typedef uint8_t		BYTE;

typedef std::size_t		count_t, index_t;
typedef std::ptrdiff_t	sindex_t;
static const constexpr index_t InvalidIndex = std::numeric_limits<index_t>::max();

#ifndef DIMENSIONS
	#define DIMENSIONS 2
#endif

static const constexpr count_t Dimensions=DIMENSIONS;
typedef std::array<float,Dimensions>	TLocation;
typedef UINT32							TAppearance;

typedef std::mt19937	BaseRandom;


#ifndef _WIN32
inline void __debugbreak()
{

}
#endif


template <typename Traits>
	std::basic_ostream<char,Traits>& operator<<(std::basic_ostream<char,Traits> &stream, const TLocation&loc)
	{
		stream << loc[0];
		for (index_t i = 1; i < Dimensions; i++)
			stream << ','<<loc[i];
		return stream;
	}



#define LOOP_OTHER_AXES

namespace Helper
{


	template <typename T=float>
		struct TFloatRange
		{
			T			minimum,
						maximum;



			static const TFloatRange<T> Invalid;

			void		Loop(T&val) const
			{
				const T ext = GetExtent();
				if (val > maximum)
					val -=  ceil((val-maximum)/ext) * ext;
				else
					if (val < minimum)
						val += ceil((minimum - val)/ext)*ext;
			}

			bool		Contains(const T&val) const
			{
				return val >= minimum && val <= maximum;
			}

			void		Clamp(T&val) const
			{
				using std::min;
				using std::max;
				val = min(max(minimum,val),maximum);
			}

			void		Include(const T&val)
			{
				using std::min;
				using std::max;
				minimum = min(val,minimum);
				maximum = max(val,maximum);
			}

			T			GetExtent() const
			{
				return maximum - minimum;
			}
		};

	template <typename T>
		/*static*/ const TFloatRange<T> TFloatRange<T>::Invalid = TFloatRange<T>{std::numeric_limits<T>::max(),std::numeric_limits<T>::lowest()};



	float Cube(float x)
	{
		return x*x*x;
	}

	int Sign(float x)
	{
		return x < 0 ? -1 : (x > 0 ? 1 : 0);
	}

	float Square(float x)
	{
		return x*x;
	}

	template <count_t Length>
		float Square(const std::array<float,Length>&v)
		{
			float rs=0;
			for (index_t i = 0; i < Length; i++)
				rs += Square(v[i]);
			return rs;
		}


	template <count_t Length>
		float QuadraticDistance(const std::array<float,Length>&u, const std::array<float,Length>&v)
		{
			float rs=0;
			for (index_t i = 0; i < Length; i++)
				rs += Square(u[i]-v[i]);
			return rs;
		}

	template <count_t Length>
		float QuadraticDistanceD(const std::array<float,Length>&u, std::array<float,Length>&v, float corridorExtent, index_t d)
		{
			float original = v[d];
			float dist = original - u[d];
			if (dist > corridorExtent/2)
				v[d] -= corridorExtent;
			else if (dist < -corridorExtent/2)
				v[d] += corridorExtent;
			float rs = d+1 < Dimensions ? QuadraticDistanceD(u,v,corridorExtent,d+1) : QuadraticDistance(u,v);

			v[d] = original;
			return rs;
		}

	template <count_t Length>
		float QuadraticDistance(const std::array<float,Length>&u, std::array<float,Length>&v, float corridorExtent)
		{
			#ifdef LOOP_OTHER_AXES
				if (Dimensions > 1)
					return QuadraticDistanceD(u, v, corridorExtent,1);
			#endif
			return QuadraticDistance(u,v);
		}


	template <count_t Length>
		float Difference(const std::array<float,Length>&u, const std::array<float,Length>&v)
		{
			return sqrt(QuadraticDistance(u,v));
		}

	float Difference(float a, float b)
	{
		return fabs(a-b);
	}

	template <count_t Length>
		void operator*=(std::array<float,Length>&v, const float fc)
		{
			for (index_t i = 0; i < Length; i++)
				v[i] *= fc;
		}

	template <count_t Length>
		void operator/=(std::array<float,Length>&v, const float div)
		{
			v *= (1.f / div);
		}

		template <count_t Length>
		void operator+=(std::array<float,Length>&v, const std::array<float,Length>&w)
		{
			for (index_t i = 0; i < Length; i++)
				v[i] += w[i];
		}



	template <typename T>
		void	ClampRadially(T&v)
		{
			float qLength = Square(v);
			if (qLength <= 1.f)
				return;
			v /= sqrt(qLength);
		}




	/**
	\brief CRC32 Checksum calculation

	Calculates a sequenced CRC32 checksum. 

	*/

	namespace CRC32
	{
		UINT32 table[0x100];

		UINT32 Reflect(UINT32 ref, BYTE ch)
		{
			UINT32 value(0);
			for (UINT16 i = 1; i < (UINT16)(ch+1); i++)
			{
				if(ref & 1)
					value |= 1 << (ch-i);
				ref>>=1;
			}
			return value;
		}


		void Init()
		{
			UINT32 polynomial = 0x04C11DB7;
			for (unsigned i = 0; i <= 0xFF; i++)
			{
				table[i] = Reflect(i,8) << 24;
				for (BYTE j = 0; j < 8; j++)
					table[i] = (table[i]<<1) ^ (table[i] & (1<<31)?polynomial:0);
				table[i] = Reflect(table[i],32);
			}
		}




 		class	Sequence
		{
		private:
			UINT32		status;
		public:
			typedef UINT32	HashContainer;	//for interchangability

			/**/		Sequence()
			{
				status = 0xFFFFFFFF;
			}

			inline void	Append(const void*source, size_t size)
			{
				const BYTE*data = (const BYTE*)source;
				for (size_t i = 0; i < size; i++)
					status = (status >> 8) ^ table[(status&0xFF) ^ data[i]];
			}

			template<typename T>
				void	AppendPOD(const T&pod)					{Append(&pod,sizeof(T));}

			UINT32		Finish()	const
			{
				return status ^ 0xFFFFFFFF;
			}
			void		Finish(UINT32&rs) const {rs = Finish();}
		};
	}



	template <typename T>
		class ArrayRef
		{
		private:
			T			*_begin=nullptr,
						*_end=nullptr;
#ifdef _DEBUG
			count_t		_length=0;
#endif
		public:
			/**/		ArrayRef(T*begin, T*end):_begin(begin),_end(end)
			#ifdef _DEBUG
				,_length(end-begin)
			#endif
			{}
			/**/		ArrayRef(T*begin, count_t length):_begin(begin),_end(begin+length)
			#ifdef _DEBUG
				,_length(length)
			#endif
			{}


			T*			begin() const {return _begin;}
			T*			end() const {return _end;}
			count_t		size() const {return _end - _begin;}
			bool		empty() const {return _end == _begin;}

			T*			operator+(index_t at) const
			{
				#ifdef _DEBUG
					if (at >= size())
						throw std::out_of_range("ArrayRef<>[] index out of bounds");
				#endif
				return (_begin+at);
			}
			T&			operator[](index_t at) const
			{
				#ifdef _DEBUG
					if (at >= size())
						throw std::out_of_range("ArrayRef<>[] index out of bounds");
				#endif
				return *(_begin+at);
			}

			bool		Contains(const T&el) const
			{
				const T*at = _begin;
				for (;at != _end; ++at)
					if (*at == el)
						return true;
				return false;
			}

			void		Export(std::vector<T>&out)const
			{
				out.reserve(size());
				out.insert(out.end(), begin(), end());
			}

			ArrayRef<T>	SubRef(index_t begin, count_t length) const
			{
				begin = std::min(begin,size());
				length = std::min(length,size()-begin);
				return ArrayRef<T>(this->begin()+begin,this->begin()+begin+length);
			}
			ArrayRef<T>	SubRef(index_t begin) const
			{
				begin = std::min(begin,size());
				return ArrayRef<T>(this->begin()+begin,this->end());
			}

			T&			First() const
			{
				#ifdef _DEBUG
					if (empty())
						throw std::out_of_range("ArrayRef<>[] first element not available");
				#endif
				return *_begin;
			}
			T&			Last() const
			{
				#ifdef _DEBUG
					if (empty())
						throw std::out_of_range("ArrayRef<>[] last element not available");
				#endif
				return *(_end-1);
			}
		};


	void ConvertNumeric(const std::string&val, std::string&rs)
	{
		rs = val;
	}

	void ConvertNumeric(const std::string&val, float&rs)
	{
		rs = std::stof(val);
	}
	void ConvertNumeric(const std::string&val, double&rs)
	{
		rs = std::stod(val);
	}
	void ConvertNumeric(const std::string&val, long double&rs)
	{
		rs = std::stold(val);
	}

	void ConvertNumeric(const std::string&val, int32_t&rs)
	{
		rs = (int32_t)std::stoi(val);
	}
	void ConvertNumeric(const std::string&val, uint32_t&rs)
	{
		rs = (uint32_t)std::stoul(val);
	}
	void ConvertNumeric(const std::string&val, uint64_t&rs)
	{
		rs = std::stoull(val);
	}
	void ConvertNumeric(const std::string&val, int64_t&rs)
	{
		rs = std::stoll(val);
	}

	//antibus http://stackoverflow.com/questions/13686482/c11-stdto-stringdouble-no-trailing-zeros
	template <typename T>
		std::string FloatToString(T f)
		{
			std::string str = std::to_string(f);
			int offset{1};
			if (str.find_last_not_of('0') == str.find('.'))
				offset = 0;
			str.erase(str.find_last_not_of('0') + offset, std::string::npos);
			return str;
		}
}

using namespace Helper;

struct TSample
{
	UINT64		numEntities = 0,
				inconsistentEntities = 0,
				newlyInconsistentEntities = 0,
				missingEntities = 0,
				inconsistencyAgeSum = 0
				;
	double		positionErrorSum = 0;



	void		Decode(	const std::string&numEntities, const std::string&inconsistentEntities, const std::string&newlyInconsistentEntities, 
						const std::string&positionErrorSum, const std::string&missingEntities, const std::string&inconsistencyAgeSum)
	{
		this->numEntities = std::stoull(numEntities);
		this->inconsistentEntities = std::stoull(inconsistentEntities);
		this->newlyInconsistentEntities = std::stoull(newlyInconsistentEntities);
		this->positionErrorSum = std::stod(positionErrorSum);
		this->missingEntities = std::stoull(missingEntities);
		this->inconsistencyAgeSum = std::stoull(inconsistencyAgeSum);
		
		if (this->inconsistentEntities > this->numEntities)
			throw std::invalid_argument("Got more inconsistent entities than total entities");
	}
};





struct TEntity
{
	TLocation	location;
	TAppearance	appearance=0;
	UINT32	inconsistencyAge = 0;

	bool	operator==(const TEntity&other) const
	{
		return location == other.location && appearance == other.appearance;
	}

	bool	operator!=(const TEntity&other) const
	{
		return !operator==(other);
	}

	bool	FlagInconsistent()
	{
		if (inconsistencyAge)
			return false;
		inconsistencyAge = 1;
		return true;
	}

	

	int	GetIsolationSector() const
	{
		return GetXLocation() < 0 ? -1 : 1;
	}

	void	Die()
	{
		SetXLocation(-10000);
	}

	bool	IsDead() const
	{
		return GetXLocation() <= -10000;	//not possible under normal circumstances
	}

	bool	CanSee(const TEntity&other, float sensorRange, float corridorExtent) const
	{
		TLocation copy = other.location;
		return QuadraticDistance(location,copy,corridorExtent) <= sensorRange*sensorRange;
	}

	bool	CanSee(const TEntity&other, float sensorRange) const
	{
		TLocation copy = other.location;
		return QuadraticDistance(location,copy) <= sensorRange*sensorRange;
	}

	float	GetXLocation() const
	{
		return location[0];
	}



	void	SetXLocation(float x)
	{
		location[0] = x;
	}
};




class LookupTable
{
private:
	std::vector<std::vector<const TEntity*> >	buckets;

	TFloatRange<>xRange,otherRange;
	float		sensorRange,bucketExtent;


	index_t		GetBucketOf(const TEntity&e) const
	{
		return std::min(buckets.size()-1,index_t((e.location[0]-xRange.minimum)/xRange.GetExtent() * (buckets.size())));
	}


	count_t		FilterBucket(const TEntity&pivot, index_t bucketIndex, const ArrayRef<const TEntity*> &resultField) const
	{
		const TEntity**begin = resultField.begin(),
			**const end = resultField.end();
		const float ext = otherRange.GetExtent();
		for (const auto&e : buckets[bucketIndex])
		{
			if (pivot.CanSee(*e,sensorRange,ext))
			{
				if (begin == end)
					throw std::out_of_range("Insufficient result capacity");
				(*begin) = e;
				begin++;
			}
		};
		return begin - resultField.begin();
	}

public:
	void	Construct(const std::vector<TEntity>&population, float sensorRange, const TFloatRange<>&xRange, const TFloatRange<>&otherRange)
	{
		this->xRange = xRange;
		this->otherRange = otherRange;
		bucketExtent = sensorRange;
		this->sensorRange = sensorRange;
		size_t numBuckets = (size_t)ceil(xRange.GetExtent()/bucketExtent);
		buckets.resize(0);
		buckets.resize(numBuckets);
		for (const auto&p : population)
		{
			index_t bucket = GetBucketOf(p);
			buckets[bucket].push_back(&p);
		}
	}

	count_t FindNeighbors(const TEntity&pivot, const ArrayRef<const TEntity*> &resultField) const
	{
		index_t bucket = GetBucketOf(pivot);
		count_t count = FilterBucket(pivot,bucket, resultField);
		if (bucket > 0)
			count += FilterBucket(pivot,bucket-1,resultField.SubRef(count));
		if (bucket +1 < buckets.size())
			count += FilterBucket(pivot,bucket+1,resultField.SubRef(count));
		return count;
	}



};




template <count_t Dimensions>
	struct Sphere
	{
		static double	GetVolume(double r)
		{
			return 0;
		}
	};
template <>
	struct Sphere<1>
	{
		static double	GetVolume(double r)
		{
			return r*2;
		}
	};
template <>
	struct Sphere<2>
	{
		static double	GetVolume(double r)
		{
			return M_PI*r*r;
		}
	};
template <>
	struct Sphere<3>
	{
		static double	GetVolume(double r)
		{
			return 4.0/3.0 * M_PI*r*r*r;
		}
	};





class Random : public BaseRandom
{
	typedef BaseRandom Super;
public:
	/**/	Random(){}
	/**/	Random(long seed):Super(seed)	{}
	auto	Next()
	{
		return Super::operator()();
	}

	float	NextFloat(float min, float max)
	{
		return float(Next()-Super::min())/float(Super::max()-Super::min()) * (max-min)+min;
	}
	float	NextFloat(float max)
	{
		return float(Next()-Super::min())/float(Super::max()-Super::min()) * max;
	}
	float	NextFloat(const TFloatRange<>&range)
	{
		return NextFloat(range.minimum,range.maximum);
	}
	bool	NextBool()
	{
		return Next() >= (Super::max()+Super::min())/2;
	}


};




struct State
{
	static count_t	parallelIterationsBefore, 
					parallelIterationsAfter;


	static constexpr const count_t NumSectors = 16;
	static const TFloatRange<>	MainAxisRange,OtherAxisRange;
	//static const constexpr count_t MapResolution = 16;
	
	static constexpr const float InfluenceRadius = 1.f / 8;
//	static constexpr const float SensorRange = InfluenceRadius * 0.9f;
		//InfluenceRadius / 2.f;
	//static constexpr const float MovementRange = InfluenceRadius - SensorRange;
	std::vector<TEntity>		entities,
								next;
	Random						random;

	LookupTable					table;

	struct RangeConfig
	{
		float					relativeMovementRange = 0.5f,
								movementRange = InfluenceRadius*0.5f,
								sensorRange= InfluenceRadius*0.5f;

		/**/					RangeConfig(){};
		/**/					RangeConfig(float relativeMovementRange):relativeMovementRange(relativeMovementRange),
																		movementRange(relativeMovementRange*InfluenceRadius),
																		sensorRange((1.f - relativeMovementRange)*InfluenceRadius)
		{}

	};

	typedef void (*FMotion)(Random&rand, TLocation&motionVector);
	typedef std::pair<FMotion,std::string> TMotion;

	struct Config
	{
		TMotion			motion;
		bool			unaffectedRandomMotion=false;
		bool			allowEntityDeath=false;
		density_t		entitiesPerInfluenceVolume=1;
		count_t			spatialSupersampling=1;
		RangeConfig		range;
	};


	/**/	State()	{}
	/**/	State(int seed, density_t entitiesPerInfluenceVolume) : random(long(seed))
	{
		count_t numEntities = count_t(
									ceil(MainAxisRange.GetExtent() * pow(OtherAxisRange.GetExtent(),Dimensions-1)	//total area
									* entitiesPerInfluenceVolume
									/ Sphere<Dimensions>::GetVolume(InfluenceRadius))
								);
		entities.resize(numEntities);

		for (auto&e : entities)
		{
			e.location[0] = random.NextFloat(MainAxisRange);
			for (index_t i = 1; i < Dimensions; i++)
				e.location[i] = random.NextFloat(OtherAxisRange);
		}

		//count_t sum = 0;
		//for (auto&e : entities)
		//for (auto&e1 : entities)
		//{
		//	if (&e != &e1 && e.CanSee(e1,InfluenceRadius))
		//		sum++;
		//}
		//volatile float avg = float(sum) / entities.size();
		//__debugbreak();
	}



	static void BinaryMotion(Random&rand, TLocation&motionVector)
	{
		switch (Dimensions)
		{
			case 1:
				motionVector[0] = rand.NextBool() ? 1.f : -1.f;
			break;
			case 2:
			{
				float angle = rand.NextFloat(float(M_PI*2.f));
				motionVector[0] = cos(angle);
				motionVector[1] = sin(angle);
			}
			break;
			case 3:
			{
				float sqrLength = 0;
				do
				{
					motionVector[0] = rand.NextFloat(-1,1);
					motionVector[1] = rand.NextFloat(-1,1);
					motionVector[2] = rand.NextFloat(-1,1);
					sqrLength = Square(motionVector);
				}
				while (sqrLength > 1 || sqrLength==0);
				
				float fc = 1.f/sqrt(sqrLength);
				motionVector[0] *= fc;
				motionVector[1] *= fc;
				motionVector[2] *= fc;
			}
			break;
		}
	}

	//static float NearBinaryMotion(Random&rand)
	//{
	//	float direction = rand.NextBool() ? 1 : -1;
	//	return direction * rand.NextFloat(0.99f,1.f);
	//}

	static void LinearMotion(Random&rand, TLocation&motionVector)
	{
		do
		{
			for (index_t i = 0; i < Dimensions; i++)
				motionVector[i] = rand.NextFloat(-1,1);
		}
		while (Square(motionVector) > 1);
	}


	typedef CRC32::Sequence	Hasher;

	void	SimulateKD(bool isolateSectors, index_t generation, const Config&cfg)
	{
		//typedef SHA1 Hasher;
		table.Construct(entities,cfg.range.sensorRange,MainAxisRange,OtherAxisRange);
		next = entities;

		parallelIterationsBefore += next.size();
		int generationSeed = random();
		Concurrency::parallel_for(index_t(0),next.size(),[=](index_t i)
		{
			try
			{
				auto&current = next[i];
				if (current.IsDead())
					return;	
				Hasher hash;
				hash.AppendPOD(generationSeed);
				hash.AppendPOD(i);
				Hasher::HashContainer rs;
				long positionSeed;
				if (cfg.unaffectedRandomMotion)
				{
					Hasher copy = hash;
					copy.Finish(rs);
					positionSeed = rs;
				}

				static const count_t BufferSize = 1024;
				BYTE field[sizeof(TEntity*) * BufferSize];
				ArrayRef<const TEntity*> resultField((const TEntity**)field,BufferSize);

				count_t found = table.FindNeighbors(current, resultField);
				resultField = resultField.SubRef(0,found);

#if 0
				for (auto e = entities.begin(); e != entities.end(); ++e)
				{
					
					float dif = Difference(e->location,current.location);
					if (dif < SensorRange)
					{
						if (!resultField.Contains(&*e)/* && e->GetXLocation()*/ )
						{
							std::cerr << "Remote entity at "<<e->location<<" visible from "<<current.location<<" (distance "<<dif<<"), but not in result from table ("<<resultField.size()<<" result(s))" <<std::endl;
							__debugbreak();
						}
					}
				}
				for (auto e = resultField.begin(); e != resultField.end(); ++e)
					if (!current.CanSee(**e,SensorRange,OtherAxisRange.GetExtent()))  //(Difference((*e)->location,current.location) > SensorRange)
					{
						__debugbreak();
					}
#endif /*0*/


				for (index_t k = 0; k < found; k++)
				{
					const TEntity* e = resultField[k];
					if (!isolateSectors || (e->GetIsolationSector() == current.GetIsolationSector()))
					{
						hash.AppendPOD(e->location);
						hash.AppendPOD(e->appearance);
					}
				}


				hash.Finish(rs);
				long environmentSeed = rs;
				if (!cfg.unaffectedRandomMotion)
				{
					positionSeed = environmentSeed+271590781;
				}

				next[i].appearance = environmentSeed;

				Random rand(positionSeed);
				const auto old = current.location;
				const int previousSector = current.GetIsolationSector();
				
				TLocation m;
				cfg.motion.first(rand,m);
				ClampRadially(m);
				m *= cfg.range.movementRange;
				current.location += m;
				MainAxisRange.Clamp(current.location[0]);
				for (index_t i = 1; i < Dimensions; i++)
				{
					#ifdef LOOP_OTHER_AXES
						OtherAxisRange.Loop(current.location[i]);
						//if (!OtherAxisRange.Contains(current.location[i]))
						//	throw std::runtime_error("Invalid looped location");
					#else
						OtherAxisRange.Clamp(current.location[i]);
					#endif
				}

				if (isolateSectors && previousSector != current.GetIsolationSector())
				{
					if (cfg.allowEntityDeath)
						current.Die();
					else
					{
						//ASSERT__(!wasIsolated);
						current.location = old;
						//current.SetXLocation((rand.NextFloat(0.01f,cfg.range.movementRange)) * previousSector);
					}
					//*n = (int)old == 0 ? 0 : 1;
				}
			}
			catch (const std::exception&ex)
			{
				std::cerr<< __func__<<" "<<__LINE__<<": "<<ex.what() << std::endl;
				__debugbreak();
			}
		});
		entities.swap(next);
		parallelIterationsAfter = parallelIterationsBefore;
	}

	TSample& GetSampleOf(std::vector<TSample>&line, const TEntity&e, const count_t samplesPerSector)
	{
		if (e.IsDead())
			throw std::out_of_range("Given entity is dead");
		const index_t rawSampleID = index_t(fabs(e.GetXLocation()) * samplesPerSector);
		if (rawSampleID > line.size()+1)
		{
			throw std::out_of_range("Sector out of range: "+std::to_string(rawSampleID)+"/"+std::to_string(line.size())+", derived from entity location "+std::to_string(e.GetXLocation()));
		}
		return line[std::min(rawSampleID,line.size()-1)];
	}

	void Accumulate(const State&correct, index_t iteration, const Config&cfg, std::vector<std::vector<TSample> >	&samples)
	{
		if (iteration >= samples.size())
			samples.resize(iteration+1);
		std::vector<TSample>&line = samples[iteration];

		const count_t samplesPerSector = count_t(cfg.spatialSupersampling / InfluenceRadius);
		line.resize((NumSectors) *samplesPerSector);
		for (index_t i = 0; i < entities.size(); i++)
		{
			TEntity&inconsistentEntity = entities[i];
			const TEntity&consistentEntity = correct.entities[i];

			TSample&s1 = GetSampleOf(line,consistentEntity,samplesPerSector);

			if (!inconsistentEntity.IsDead())
			{
				TSample&s0 = GetSampleOf(line,inconsistentEntity,samplesPerSector);
				s0.numEntities ++;
				s1.numEntities ++;

				if (inconsistentEntity.inconsistencyAge)
					inconsistentEntity.inconsistencyAge++;
				else 
					if (inconsistentEntity != consistentEntity && inconsistentEntity.FlagInconsistent())
					{
						s0.newlyInconsistentEntities ++;
						s1.newlyInconsistentEntities ++;
					}
				double error = Difference(inconsistentEntity.location, consistentEntity.location);
				if (cfg.range.movementRange != 0)
					error /= cfg.range.movementRange;

				if (inconsistentEntity.inconsistencyAge)
				{
					s0.inconsistentEntities ++;
					s0.positionErrorSum += error;
					s0.inconsistencyAgeSum += inconsistentEntity.inconsistencyAge;
					s1.inconsistentEntities ++;
					s1.positionErrorSum += error;
					s1.inconsistencyAgeSum += inconsistentEntity.inconsistencyAge;
				}
			}
			else
			{
				s1.numEntities += 2;	//count double (the inconsistent half won't be accounted for)
				s1.missingEntities+=2;
			}
		}

	}


};

/*static*/ const TFloatRange<>	State::MainAxisRange = TFloatRange<float>{-(float)State::NumSectors,(float)State::NumSectors},
				State::OtherAxisRange = TFloatRange<float>{-1,1};

count_t	State::parallelIterationsBefore = 0, 
		State::parallelIterationsAfter = 0;

count_t RunSimulation(int seed, const State::Config&cfg, std::vector<std::vector<TSample> >&samples)
{
	State	correct(seed,cfg.entitiesPerInfluenceVolume),
			approx;
	approx = correct;
	count_t rs = correct.entities.size();

	for (int iteration = 0; iteration < 128; iteration++)
	{
		correct.SimulateKD(false,iteration,cfg);
		approx.SimulateKD(true, iteration, cfg);

		approx.Accumulate(correct,iteration,cfg, samples);
	}

	return rs;
}


char timestamp[0x500] = "",
	startedTimestamp[0x500] = "";

void UpdateTimestamp(char*buffer, size_t bufferSize)
{
	time_t t = time(nullptr);
	tm tm;
#ifdef _WIN32
	localtime_s(&tm,&t);

	strftime(buffer,bufferSize,"%d.%m.%y %T",&tm);
#else
	strftime(buffer,bufferSize,"%d.%m.%y %T",localtime(&t));
#endif
}


void UpdateTimestamp()
{
	UpdateTimestamp(timestamp,sizeof(timestamp));

	std::string consoleTitle = "Simulation started "+std::string(startedTimestamp)+". Dimensions="+std::to_string(Dimensions)+". Entities simulated: "+std::to_string(State::parallelIterationsAfter);
	#ifdef _WIN32
		SetConsoleTitleA(consoleTitle.c_str());
	#endif

}


bool CheckIfOutdatedAndUpdate(std::chrono::high_resolution_clock::time_point&last, float seconds)
{
	auto now = std::chrono::high_resolution_clock::now();
	auto age = std::chrono::duration_cast<std::chrono::duration<float> >( now - last);
	if (age.count() > seconds)
	{
		last = now;
		return true;
	}
	return false;
}

void PrintProgress(index_t at, index_t max, bool forceUpdate=false)
{
	static auto lastUpdate = std::chrono::high_resolution_clock::now();
	if (!forceUpdate && !CheckIfOutdatedAndUpdate(lastUpdate,0.25f))
		return;
	std::cout << "\r"<<timestamp<<" [";
	static const constexpr count_t Length = 40;
	for (index_t i = 0; i < Length; i++)
		std::cout << ' ';
	std::cout << "]\r"<<timestamp<<" [";
	for (index_t i = 0; i < at*Length/max; i++)
		std::cout << '=';
	if (at < max)
		std::cout << '>';
	std::cout.flush();
}


std::mt19937	randomSource;

namespace std
{
	const std::string&	to_string(const std::string&str)
	{
		return str;
	}
}


class Metric
{
public:
	const std::string	name;

	/**/				Metric(const std::string&name):name(name)	{}
	virtual bool		DecodeAndCompare(const std::string&value, const State::Config&cfg)=0;
	virtual std::string	ValueToString(const State::Config&cfg) const=0;
};


template <typename T>
	class NativeMetric : public Metric
	{
		typedef Metric Super;
	public:
		typedef std::function<T(const State::Config&cfg)> FGet;
		FGet			get;

		/**/			NativeMetric(const std::string&name,const FGet&get):Super(name), get(get)	{}

		virtual bool	DecodeAndCompare(const std::string&svalue, const State::Config&cfg) override
		{
			try
			{
				T primitive;
				ConvertNumeric(svalue,primitive);
				T shouldBe = get(cfg);
				if (shouldBe != primitive)
				{
					std::cout << "Expected "<<name<<" "<<shouldBe<<", but got "<<primitive<<". Starting fresh."<<std::endl;
					return false;
				}
				return true;
			}
			catch (...)
			{
				std::cout << "File contains malformatted "<<name<<" value '"<<svalue<<"'."<<std::endl;
			}
			return false;
		}

		virtual std::string	ValueToString(const State::Config&cfg) const override
		{
			return std::to_string(get(cfg));
		}
	};


typedef std::shared_ptr<Metric>	PMetric;
std::vector<PMetric>	metrics;



void SimulateDensity(const State::Config&cfg)
{
	//static constexpr const count_t OverResolution = State::MapResolution * State::InfluenceRadius;

	const std::string dim = Dimensions > 1 ? std::to_string(Dimensions)+"D_":"";


	std::string path = "results/";
	path += std::to_string(Dimensions)+"D/";
	path += "d"+std::to_string(FloatToString(std::round(cfg.entitiesPerInfluenceVolume*100)/100))+"/";

	#ifdef WIN32
		index_t c;
		while ((c = path.find('/',0)) != std::string::npos)
			path.replace(c,1,1,'\\');
		const std::string mkdirCommand = ("mkdir "+path);
	#else
		const std::string mkdirCommand = ("mkdir -p "+path);
	#endif

	const int mkdirRc = system(mkdirCommand.c_str());
		
	std::string name = path + "table"
							+"_m"+FloatToString(cfg.range.relativeMovementRange)
							+"_"+std::to_string(cfg.spatialSupersampling)
							+(cfg.motion.second.empty() ? "":"_"+(cfg.motion.second))
							+(cfg.unaffectedRandomMotion ? "_unaffected":"")
							+(cfg.allowEntityDeath ? "_d":"")
							+".txt";

	std::vector<std::vector<TSample> >	samples;

	count_t expectEntitiesPerSimulation = 0;
	count_t totalSimulationRuns = 0;
	std::cout << "Simulating data for '"<<name<<"'" << std::endl;

	{
		std::ifstream file(name);
		if (file.good())
		{
			std::cout << "Attempting to import existing file..." << std::endl;

			std::string line;
			std::vector<std::string>	parts;

			bool hasLocationErrorSum0 = false,
				hasLocationErrorSum1 = true,
				hasMissingEntites = false,
				hasInconsistencyAge = false;
		
			bool	hasHeader = false;

			while (std::getline(file,line))
			{
				if (line.empty())
					continue;
				
				if (isalpha((int)(unsigned char)(line[0])))
				{
					//attribute or metric
					index_t eqAt = line.find('=');
					if (eqAt == std::string::npos)
					{
						std::cout << "File contains malformatted metric '"<<line<<"'. Ignoring."<<std::endl;
						continue;
					}
					const std::string svalue = line.substr(eqAt+1);

					std::string metric = line.substr(0,eqAt);

					if (metric == "TotalSimulations")
					{
						try
						{
							ConvertNumeric(svalue,totalSimulationRuns);
						}
						catch (...)
						{
							std::cout << "File contains malformatted TotalSimulations value '"<<svalue<<"'. Ignoring."<<std::endl;
						}
						continue;
					}
					if (metric == "EntitiesPerSimulation")
					{
						try
						{
							int value;
							ConvertNumeric(svalue,value);
							expectEntitiesPerSimulation = value;
						}
						catch (...)
						{
							std::cout << "File contains malformatted EntitiesPerSimulation value '"<<line<<"'. Ignoring."<<std::endl;
						}
						continue;
					}
					if (metric == "LoopOtherAxes")
					{
						try
						{
							int value;
							ConvertNumeric(svalue,value);
							expectEntitiesPerSimulation = value;
						}
						catch (...)
						{
							std::cout << "File contains malformatted EntitiesPerSimulation value '"<<line<<"'. Ignoring."<<std::endl;
						}
						continue;
					}



					bool found = false;
					bool abort = false;
					for (const auto&m : metrics)
						if (m->name == metric)
						{
							found = true;
							if (!m->DecodeAndCompare(svalue,cfg))
							{
								abort = true;
								break;
							}
						}
					if (abort)
						break;
					if (found)
						continue;

					std::cout << "File contains unknown metric '"<<metric<<"' in '"<<line<<"'. Ignoring."<<std::endl;
					continue;
				}

				if (*line.begin() == '[' && *line.rbegin() == ']')	//flags
				{
					std::stringstream ss(line.substr(1,line.length()-2));
					std::istream_iterator<std::string> begin(ss);
					std::istream_iterator<std::string> end;
					std::vector<std::string> parts(begin, end);

					for (const auto&part : parts)
					{
						if (part == "LocationError")
							hasLocationErrorSum0 = true;
						else if (part == "LocationError1" || part == "PositionErrorSum")
							hasLocationErrorSum1 = true;
						else if (part == "MissingEntities")
							hasMissingEntites = true;
						else if (part == "InconsistencyAge" || part == "InconsistencyAgeSum")
							hasInconsistencyAge = true;
					}

					if (!hasLocationErrorSum1 || hasLocationErrorSum0)
					{
						std::cout << "File cannot be continued (lacking or incompatible location error sum). Starting fresh."<<std::endl;
						break;
					}
					if (!hasInconsistencyAge)
					{
						std::cout << "File cannot be continued (lacking inconsistency age). Starting fresh."<<std::endl;
						break;
					}
					if (!hasMissingEntites)
					{
						std::cout << "File cannot be continued (lacking missing entities). Starting fresh."<<std::endl;
						break;
					}
					hasHeader = true;
					continue;
				}
				if (!hasHeader)
				{
					std::cout << "File cannot be continued (lacking header). Starting fresh."<<std::endl;
					break;
				}
				{
					std::stringstream ss(line.substr(1,line.length()-2));
					std::istream_iterator<std::string> begin(ss);
					std::istream_iterator<std::string> end;
					std::vector<std::string> parts(begin, end);

					samples.push_back(std::vector<TSample>());
					std::vector<TSample>&line = *samples.rbegin();

					count_t numParts = 3 + hasLocationErrorSum1 + hasMissingEntites + hasInconsistencyAge;

					line.resize((parts.size()-1) / numParts);

					for (index_t i = 0; i*numParts+numParts < parts.size(); i++)
					{
						const auto&total = parts[i*numParts];
						const auto&inc = parts[i * numParts + 1];
						const auto&newlyInc = parts[i*numParts+2];
						const auto&incAge = parts[i*numParts+5];
						line[i].Decode(total,inc,newlyInc, parts[i*numParts+3], parts[i*numParts+4], incAge);
					}
				}
			}


		}
		else
			if (mkdirRc != 0)
				std::cerr << "Warning: Source file does not exist, and '"<<mkdirCommand<<"' returned an error. Chances are that no output file will be written."<<std::endl;
	}
	
	if (!samples.empty())
	{
		std::cout << "Continuing simulation at run "<<totalSimulationRuns<<std::endl;
	}

	const count_t batchSize = count_t(5000 * pow(10,-(int)Dimensions));
	const count_t numIterations = (count_t)ceil(20000.0 / cfg.entitiesPerInfluenceVolume / 500);

	std::cout << "Simulating density "<<cfg.entitiesPerInfluenceVolume<<" in "<<numIterations<<" batch(es) of "<<batchSize<<" simulation(s)..." << std::endl;
	for (index_t r = 0; r < numIterations; r++)
	{
		UpdateTimestamp();
		auto started = std::chrono::high_resolution_clock::now();
		count_t numEntities = 0;
		for (index_t i = 0; i < batchSize; i++)
		{
			PrintProgress(i,batchSize,i==0);
			numEntities = RunSimulation(randomSource(),cfg, samples);
			if (expectEntitiesPerSimulation != 0 && expectEntitiesPerSimulation != numEntities)
			{
//				throw std::runtime_error("Unexpected entity count. Imported entity count is "+std::to_string(expectEntitiesPerSimulation)+", but local simulation says "+std::to_string(numEntities));
				//let's ignore this one
			}
			totalSimulationRuns++;
		}
		float seconds = std::chrono::duration_cast<std::chrono::duration<float> >( (std::chrono::high_resolution_clock::now() - started)).count();

		std::ofstream file(name,std::ofstream::out | std::ofstream::trunc);

		if (file.is_open())
		{
			file << "TotalSimulations="<<totalSimulationRuns<<std::endl;
			file << "EntitiesPerSimulation="<<numEntities<<std::endl;
			for (const auto&m : metrics)
				file << m->name<<'='<< m->ValueToString(cfg) << std::endl;
			//file << "EntityDensity="<<density<<std::endl;
			//file << "EntityMotionProfile="<<motion.second<<std::endl;
			//file << "EnvironmentImpact="<<(cfg.unaffectedRandomMotion?"AppearanceOnly":"AppearanceAndMotion")<<std::endl;
			//file << "EntityDeath="<<(allowEntityDeath?"Allowed":"Avoided")<<std::endl;
			//file << "Dimensions="<<std::to_string( Dimensions)<<std::endl;
			//file << "SpatialSupersampling="<<spatialSupersampling<<std::endl;
			file << "[NumEntities InconsistentEntities NewlyInconsistentEntities PositionErrorSum MissingEntities InconsistencyAgeSum]"<<std::endl;
			for (const auto&line : samples)
			{
				for (auto sample = line.begin(); sample != line.end(); ++sample)
				{
					file << '\t'<<sample->numEntities 
							<< '\t'<<sample->inconsistentEntities 
							<< '\t'<<sample->newlyInconsistentEntities 
							<< '\t'<<sample->positionErrorSum
							<< '\t'<<sample->missingEntities
							<< '\t'<<sample->inconsistencyAgeSum
					;
				}
				file << std::endl;
			}
		}
		else
			std::cerr<<"Warning: unable to open output file '"<<name<<"' for output. Simulation result is lost"<<std::endl;
		PrintProgress(batchSize,batchSize,true);
		std::cout << "] #"<<r<<" (d"<<cfg.entitiesPerInfluenceVolume<<"/"<<cfg.motion.second<<") done after "<<seconds<<std::endl;
	}


}


namespace Motion
{
	

	const State::TMotion	binary = std::make_pair(State::BinaryMotion,"");
	//const std::pair<State::FMotion,String>	nearBinary = std::make_pair(State::NearBinaryMotion,"nb");
	const State::TMotion	linear = std::make_pair(State::LinearMotion,"linear");
//	const std::pair<State::FMotion,String>	sinus = std::make_pair(State::SinusMotion,"sinus");
//	const std::pair<State::FMotion,String>	sqrtSinus = std::make_pair(State::SqrtSinusMotion,"sqrtSinus");
	
}


int main( int argc, const char* argv[])
{
#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), PROCESS_MODE_BACKGROUND_BEGIN);
#else
	sched_param sp;
	sp.sched_priority = 0;
	//pthread_setschedparam(pthread_self(), SCHED_IDLE, &sp);
	int rc = sched_setscheduler(0, SCHED_IDLE, &sp);
	if (rc != 0)
		std::cerr <<"Warning: Failed to set idle priority class. Application will run with normal priority"<<std::endl;
#endif

	CRC32::Init();

	UpdateTimestamp(startedTimestamp,sizeof(startedTimestamp));



	{
		metrics.push_back(PMetric(new NativeMetric<density_t>("EntityDensity",[](const State::Config&cfg){return cfg.entitiesPerInfluenceVolume;})));
		metrics.push_back(PMetric(new NativeMetric<float>("RelativeMovementRange",[](const State::Config&cfg){return cfg.range.relativeMovementRange;})));
		metrics.push_back(PMetric(new NativeMetric<std::string>("EntityMotionProfile",[](const State::Config&cfg){return cfg.motion.second;})));
		metrics.push_back(PMetric(new NativeMetric<std::string>("EnvironmentImpact",[](const State::Config&cfg){return cfg.unaffectedRandomMotion ? "AppearanceOnly" : "AppearanceAndMotion";})));
		metrics.push_back(PMetric(new NativeMetric<std::string>("EntityDeath",[](const State::Config&cfg){return cfg.allowEntityDeath ? "Allowed" : "Avoided";})));
		metrics.push_back(PMetric(new NativeMetric<count_t>("Dimensions",[](const State::Config&cfg){return Dimensions;})));
		metrics.push_back(PMetric(new NativeMetric<count_t>("SpatialSupersampling",[](const State::Config&cfg){return cfg.spatialSupersampling;})));
	}





	State::Config cfg;

	cfg.range = State::RangeConfig(0.5f);
	cfg.motion = Motion::binary;
	cfg.spatialSupersampling = 2;

	count_t mvStepCount = 1;
	float mvStepWidth = 0.1f;

	bool endless = false;
	{

		enum class Expect
		{
			Density,
			MovementRange,
			MovementRangeStepWidth,
			MovementRangeStepCount
		};

		Expect expect = Expect::Density;
		for (int i = 1; i < argc; i++)
		{
			std::string a{argv[i]};
		
			if (a == "--help")
			{
				std::cout << "Usage: "<<argv[0]<<" <Density> [options]"<<std::endl;
				std::cout << "\tDensity: float, >0"<<std::endl;
				std::cout << "\t-m <f>: Set relative movement range (0<= f <1). Default is 0.5"<<std::endl;
				std::cout << "\t-mStp <f>: Set relative movement range step (0< f <1). Default is 0.1"<<std::endl;
				std::cout << "\t-mCnt <u>: Set number of steps to take in relative movement range,\n\t each additional incrementing by mStp. Default is 1"<<std::endl;
				std::cout << "\t-l/--linear: force linear entity motion (instead of binary)"<<std::endl;
				std::cout << "\t-d/--death: Allow entity death"<<std::endl;
				std::cout << "\t-e/--endless: Repeat simulation indefinitely"<<std::endl;
				return 0;
			}
		
			if (a.length() == 0)
				continue;
			if (a[0] == '-')
			{
				expect = Expect::Density;
				if (a == "-l" || a == "--linear")
					cfg.motion = Motion::linear;
				else if (a == "-d" || a == "--death")
					cfg.allowEntityDeath = true;
				else if (a == "-e" || a == "--endless")
					endless = true;
				else if (a == "-m")
					expect = Expect::MovementRange;
				else if (a == "-mStp")
					expect = Expect::MovementRangeStepWidth;
				else if (a == "-mCnt")
					expect = Expect::MovementRangeStepCount;
				else
					std::cerr<<"Unsupported option: "<<a<<std::endl;
			}
			else
			{
				try
				{
					switch (expect)
					{
						case Expect::MovementRangeStepCount:
							mvStepCount = std::stoul(a);
							break;
						case Expect::MovementRangeStepWidth:
							mvStepWidth = std::stof(a);
							break;
						case Expect::MovementRange:
							cfg.range = State::RangeConfig( std::stof(a) );
							break;
						case Expect::Density:
							cfg.entitiesPerInfluenceVolume = std::stof(a);
							break;
					}
				}
				catch (...)
				{
					std::cerr<<"Warning: Unsupported or non-parsable parameter: "<<a<<std::endl;
				}
				expect = Expect::Density;
			}
	
		}
	}
	try
	{
		const float mv0 = cfg.range.relativeMovementRange;
		for (;;)
		{
			for (index_t i = 0; i < mvStepCount; i++)
			{
				cfg.range = State::RangeConfig(mv0 + mvStepWidth * i);
				SimulateDensity(cfg);
			}
			cfg.range = State::RangeConfig(mv0);
			if (!endless)
				break;
		}
		return 0;
	}
	catch (const std::exception&ex)
	{
		#ifdef WIN32
			OutputDebugStringA(typeid( ex ).name());
			OutputDebugStringA(": ");
			OutputDebugStringA(ex.what());
			OutputDebugStringA("\n");
		#endif
		std::cerr << ex.what()<<std::endl;
	}
	catch (...)
	{
		std::cerr << "Caught unknown exception"<<std::endl;
	}
	return -1;
}

