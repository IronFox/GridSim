#include "interface.h"
#include "../io/log.h"
#include "../general/thread_system.h"

#undef EXCEPTION
#define EXCEPTION(message)	FRAC_FATAL(message)

//#define DEBUG_POINT		cout << "\rf"<<__LINE__;
/*#define BEGIN
#define END
#define LBEGIN*/


namespace DeltaWorks
{


	static unsigned __log_depth = 0;
	char*__space_field=NULL;

	const char*spaces(unsigned length)
	{
		if (!__space_field)
		{
			__space_field = new char[256];
		}
		memset(__space_field,' ',length);
		__space_field[length] = 0;
		return __space_field;
	}

	#undef LBEGIN
	#undef BEGIN
	#undef END
	#ifndef BEGIN
		#define BEGIN	//cout<<spaces(__log_depth++)<<__func__<<"()"<<endl;
		//#define BEGIN	//logfile<<"entering: "+String(__func__)+"\n";
	#endif
	#ifndef END
		#define END		//cout<<spaces(--__log_depth)<<__func__<<"() done\n";
		//#define END		//logfile<<"exiting: "+String(__func__)+"\n";
	#endif


	namespace Fractal
	{
		#include "kernel/config.h"
	
	
	

	

		SynchronizedLogFile								logfile("fractal.log",true);
		unsigned										num_sectors=0,
														max_sectors=2000,
														build_operations=1;
		static const float								ttl_loss_per_level = 0.8f;
		Timer::Time									build_time = 1000000;
	
		bool											mipmapping = true,
														texture_compression = false,
														background_subdiv = true;

		//::TContext										default_context = {10,1,10000,32};

	
	
	
	
	
	

		namespace BackgroundSubDiv
		{
			volatile bool								active = false;
			bool										signaled = false;
											

			System::BlockingPipe						begin_tickets,
														end_tickets;


			class Process:public System::ThreadMainObject
			{
			public:
			virtual	void								ThreadMain();

			};

			Process										process;

			System::Thread								thread(process,true);

			void										begin();
			void										end();

			//void										signalEnd();

		};
	
	
		//ReferenceVector<TSurfaceSegment>					surface_list;

	
	
		#define INVOKING_DISCARD(ADDR)		//logfile << ">>Discard(0x"<<IntToHex((int)(ADDR),8)<<")"<<nl;
		#define INVOKED_DISCARD(ADDR)		//logfile << "<<Discard(0x"<<IntToHex((int)(ADDR),8)<<")"<<nl;
		#define INVOKING_VIRTUAL(ADDR,FUNC)	//logfile << ">>"<<#FUNC<<"(0x"<<IntToHex((int)(ADDR),8)<<")"<<nl;
		#define INVOKED_VIRTUAL(ADDR,FUNC)	//logfile << "<<"<<#FUNC<<"(0x"<<IntToHex((int)(ADDR),8)<<")"<<nl;
	
	
		void				reportFatalException(const TCodeLocation&location, const String&message)
		{
			Except::fatal(location,message);
		}
			

		void	setSeed(Random&random, const Seed64&seed)
		{
			random.seed(seed.toInt());
		}
			
		void	setSeed(Random&random, const DynamicSeed&seed)
		{
			random.seed(seed.toInt());
		}
			

	
		String	randomWord()
		{
			static const char alpha[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_./\\";
			Random random;
			BYTE len = random.Next(3,32);
			String result;
			result.resize(len);
			for (BYTE k = 0; k < len; k++)
				result.set(k,alpha[random.NextIndex(strlen(alpha))]);
			return result;
		}
	
		DynamicSeed::DynamicSeed()
		{
			randomize();
		}
	
		DynamicSeed::DynamicSeed(const String&name)
		{
			hash(name);
		}
		
		UINT32				DynamicSeed::toInt()	const
		{
			return CRC32::Sequence().Append(value.pointer(),value.length()).Finish();
		}
	
		String			DynamicSeed::toHex()	const
		{
			return binaryToHex(value.pointer(),value.GetContentSize());
		}
	
		DynamicSeed&			DynamicSeed::hash(const String&string)
		{
			value.SetSize(string.length());
			value.copyFrom(string.c_str(),string.length());
			as_int = toInt();
			return *this;
		}
	
		DynamicSeed&			DynamicSeed::operator=(const ValueType&array)
		{
			this->value = array;
			as_int = toInt();
			return *this;
		}
	
		void			DynamicSeed::implant(const DynamicSeed&seed, BYTE child_id)
		{
			value.SetSize(seed.value.length()+1);
			value.copyFrom(seed.value.pointer(),seed.value.length());
			value.last() = child_id;
			as_int = toInt();
			//cout << seed.array.length() <<" -> "<<array.length()<<endl;
		}
	
		void			DynamicSeed::rotate(unsigned depth, BYTE orientation)
		{
			BYTE step = (BYTE)depth;
			for (unsigned i = 0;
					i < value.length();
					i++)
			{
				value[i]+=step;
				step += value[i]>0
						?orientation%value[i]
						:orientation;
			}
			as_int = toInt();
		}
	
		void			DynamicSeed::randomize()
		{
			Random random;
			unsigned len = random.Next(3,32);
			value.SetSize(len);
			for (unsigned i = 0; i < len; i++)
				value[i] = random.Next(255);
			as_int = toInt();
		}


	
	
		Seed64::Seed64()
		{
			randomize();
		}
	
		Seed64::Seed64(const String&name)
		{
			hash(name);
		}
		
		UINT32				Seed64::toInt()	const
		{
			//algorithm by Thomas Wang, Jan 1997
			UINT64 temp = (~value) + (value << 18); // key = (key << 18) - key - 1;
			temp = temp ^ (temp >> 31);
			temp = temp * 21; // key = (key + (key << 2)) + (key << 4);
			temp = temp ^ (temp >> 11);
			temp = temp + (temp << 6);
			temp = temp ^ (temp >> 22);
			return (UINT32) temp;
		}
	
		String			Seed64::toHex()	const
		{
			return IntToHex((__int64)value,16);
		}
	
		Hash64				Seed64::hashFunction;
	
		Seed64&			Seed64::hash(const String&string)
		{
			value = hashFunction(string.c_str(),string.length());
			as_int = toInt();
			return *this;
		}
	
		Seed64&			Seed64::operator=(const UINT64&seed)
		{
			this->value = seed;
			as_int = toInt();
			return *this;
		}
	
		void			Seed64::implant(const Seed64&seed, BYTE child_id)
		{
			this->value = (seed.value << 2) | child_id;
			as_int = toInt();
			//cout << seed.array.length() <<" -> "<<array.length()<<endl;
		}
	
		void			Seed64::rotate(unsigned depth, BYTE orientation)
		{
			BYTE step = (BYTE)depth;
			BYTE*bytes = (BYTE*)&value;
			for (unsigned i = 0; i < 8; i++)
			{
				bytes[i]+=step;
				step += bytes[i]>0 ? orientation%bytes[i] : orientation;
			}
			as_int = toInt();
		}
	
		void			Seed64::randomize()
		{
			Random random;
			BYTE*bytes = (BYTE*)&value;
			for (unsigned i = 0; i < 8; i++)
				bytes[i] = random.Next(255);
			as_int = toInt();
		}



	

		void				makeHaloGeometry(Geometry&geometry)
		{
			static const unsigned resolution = 20;
	
			geometry.vertexField.SetSize(resolution*2);
			geometry.triangleField.SetSize(resolution*6);
			unsigned *f = geometry.triangleField.pointer();
			#undef t
			#define t(i0,i1,i2)	{(*f++) = i0; (*f++) = i1; (*f++) = i2;}
				for (unsigned i = 0; i < resolution; i++)
				{
					t(i,(i+1)%resolution,i+resolution);
					t((i+1)%resolution,(i+1)%resolution+resolution,i+resolution);
				}
			#undef t
			ASSERT_CONCLUSION(geometry.triangleField,f);

			float step = 360.0f/resolution;
			Geometry::Vertex*v = geometry.vertexField.pointer();
			for (unsigned i = 0; i < resolution; i++)
				Vec::sphereCoords(step*i,-step/2,1,(*v++));
			for (unsigned i = 0; i < resolution; i++)
				Vec::sphereCoords(step*i+step/2,step/2,1,(*v++));
		
			ASSERT_CONCLUSION(geometry.vertexField, v);
		}
	
		Geometry			makeHaloGeometry()
		{
			Geometry result;
			makeHaloGeometry(result);
			return result;
		}
	
		Geometry			makeSphereGeometry()
		{
			Geometry result;
			makeSphereGeometry(result);
			return result;
		}

	
		void	makeSphereGeometry(Geometry&geometry)
		{
			//4 sided:
			/*
			geometry.vertex_field.resize(4);
			geometry.index_field.resize(12);
			unsigned *f = geometry.index_field;
			#undef t
			#define t(i0,i1,i2)	{(*f++) = i0; (*f++) = i1; (*f++) = i2;}
				t(1,3,2);
				t(0,2,3);
				t(1,0,3);
				t(2,0,1);
			#undef t
			ASSERT_CONCLUSION(geometry.index_field,f);

			_v3(geometry.vertex_field[0].position,-1,-1,-1);
			_v3(geometry.vertex_field[1].position,1,-1,1);
			_v3(geometry.vertex_field[2].position,-1,1,1);
			_v3(geometry.vertex_field[3].position,1,1,-1);
			*/
		
			//8 sided:
			/*
			geometry.vertex_field.resize(6);
			geometry.index_field.resize(24);
			unsigned *f = geometry.index_field;
			#undef t
			#define t(i0,i1,i2)	{(*f++) = i0; (*f++) = i1; (*f++) = i2;}
				t(0,1,2);
				t(0,2,3);
				t(0,3,4);
				t(0,4,1);
			
				t(5,2,1);
				t(5,3,2);
				t(5,4,3);
				t(5,1,4);
			#undef t
			ASSERT_CONCLUSION(geometry.index_field,f);

			_v3(geometry.vertex_field[0].position,0,1,0);
			_v3(geometry.vertex_field[1].position,-1,0,0);
			_v3(geometry.vertex_field[2].position,0,0,1);
			_v3(geometry.vertex_field[3].position,1,0,0);
			_v3(geometry.vertex_field[4].position,0,0,-1);
			_v3(geometry.vertex_field[5].position,0,-1,0);
			*/
	
			//20 sided:
		
			geometry.vertexField.SetSize(12);
			geometry.triangleField.SetSize(60);
			geometry.edgeField.SetSize(60);
			UINT32 *f = geometry.triangleField.pointer();
			#undef t
			#define t(i0,i1,i2)	{(*f++) = i0; (*f++) = i1; (*f++) = i2;}
				t(0,1,2);
				t(0,2,3);
				t(0,3,4);
				t(0,4,5);
				t(0,5,1);

				t(2,1,6);
				t(3,2,7);
				t(4,3,8);
				t(5,4,9);
				t(1,5,10);

				t(2,6,7);
				t(3,7,8);
				t(4,8,9);
				t(5,9,10);
				t(1,10,6);

				t(7,6,11);
				t(8,7,11);
				t(9,8,11);
				t(10,9,11);
				t(6,10,11);
			#undef t
			ASSERT_CONCLUSION(geometry.triangleField,f);

			UINT32 *e = geometry.edgeField.pointer();
			#undef E
			#define E(i0,i1)	{(*e++) = i0; (*e++) = i1; }
				E(0,1);
				E(0,2);
				E(0,3);
				E(0,4);
				E(0,5);

				E(5, 1);
				E(1, 2);
				E(2, 3);
				E(3, 4);
				E(4, 5);

				E(1, 10);
				E(1, 6);
				E(2, 6);
				E(2, 7);
				E(3, 7);
				E(3, 8);
				E(4, 8);
				E(4, 9);
				E(5, 9);
				E(5, 10);

				E(10, 6);
				E(6, 7);
				E(7, 8);
				E(8, 9);
				E(9, 10);

				E(6, 11);
				E(7, 11);
				E(8, 11);
				E(9, 11);
				E(10, 11);
			#undef E
			ASSERT_CONCLUSION(geometry.edgeField,e);


			Vec::sphereCoords(0,90,1,geometry.vertexField[0]);
	    
			Vec::sphereCoords(0, 30, 1, geometry.vertexField[1]);
			Vec::sphereCoords(-72, 30, 1, geometry.vertexField[2]);
			Vec::sphereCoords(-144, 30, 1, geometry.vertexField[3]);
			Vec::sphereCoords(-216, 30, 1, geometry.vertexField[4]);
			Vec::sphereCoords(-288, 30, 1, geometry.vertexField[5]);

			Vec::sphereCoords(-36, -30, 1, geometry.vertexField[6]);
			Vec::sphereCoords(-108, -30, 1, geometry.vertexField[7]);
			Vec::sphereCoords(-180, -30, 1, geometry.vertexField[8]);
			Vec::sphereCoords(-252, -30, 1, geometry.vertexField[9]);
			Vec::sphereCoords(-324, -30, 1, geometry.vertexField[10]);

			Vec::sphereCoords(0, -90, 1, geometry.vertexField[11]);
		
		}
	
		RollSpace::RollSpace(double modal_range_):modal_range(modal_range_)
		{
			Vec::clear(texture_root);
			Vec::def(texture_up,0,1,0);
			Vec::def(texture_x,1,0,0);
			Vec::def(texture_y,0,0,1);
			Mat::Eye(invert);
		}
	
		void	RollSpace::roll(const TVec3<>&up, const TVec3<>&location)
		{
			TVec3<double>	up_ = {up.x,up.y,up.z},
							location_ = {location.x,location.y,location.z};
			roll(up_,location_);
		}
	
		void	RollSpace::roll(const TVec3<>&up, const TVec3<double>&location)
		{
			TVec3<double> up_ = {up.x,up.y,up.z};
			roll(up_,location);
		}

		void	RollSpace::mod(double&val)	const
		{
			int64_t step = (int64_t)(val/modal_range);
			if (step < 0)
				step--;
			val -= modal_range*step;
		}
	
		void	RollSpace::roll(const TVec3<double>& up, const TVec3<double>&delta)
		{
			TMatrix4<double> sys;
			TVec3<double>	projection,temp;
			double translation = Vec::dot(delta,up);
			//_mad(texture_root,up,-translation);
			//_add(texture_root,delta);
		
	
			Vec::add(up,texture_up,projection);
			Vec::mad(texture_x,projection,-Vec::dot(texture_x,up)/Vec::dot(projection,up));
			Vec::copy(up,texture_up);
			Vec::cross(texture_x,up,texture_y);
			Vec::normalize0(texture_up);
			Vec::normalize0(texture_x);
			Vec::normalize0(texture_y);
		
			//texture_root[0] = fmod(texture_root[0],modal_range);
			//texture_root[1] = fmod(texture_root[1],modal_range);
			//texture_root[2] = fmod(texture_root[2],modal_range);

			//mod(texture_root[0]);
			//mod(texture_root[1]);
			//mod(texture_root[2]);

		
			//shiver me timbers. the seas have got me - yar.
			//project root to plane:
			Vec::mad(texture_root,texture_x,-Vec::dot(texture_x,delta));
			Vec::mad(texture_root,texture_y,-Vec::dot(texture_y,delta));
			Vec::mad(texture_root,texture_up,-Vec::dot(texture_up,texture_root));
		
			//modulate root:
			{
				double	x = Vec::dot(texture_root,texture_x),
						y = Vec::dot(texture_root,texture_y);
				mod(x);
				mod(y);
				Vec::mult(texture_x,x,texture_root);
				Vec::mad(texture_root,texture_y,y);
			}
			/*
			mod(texture_root[0]);
			mod(texture_root[1]);
			mod(texture_root[2]);*/
		
			/*int mod[3];
			_div(texture_root,modal_range,mod);
			_mad(texture_root,mod,-modal_range);*/
		

			Vec::copy(texture_x,sys.x.xyz);
			Vec::copy(texture_y,sys.y.xyz);
			Vec::copy(texture_up,sys.z.xyz);
			Vec::copy(texture_root,sys.w.xyz);
			Mat::resetBottomRow(sys);
			Mat::invertSystem(sys,invert);
		}


		void	RollSpace::exportTo(TMatrix4<>&system, double range, bool include_translation)	const
		{
			Vec::div(invert.x.xyz,range,system.x.xyz);
			Vec::div(invert.y.xyz,range,system.y.xyz);
			Vec::div(invert.z.xyz,range,system.z.xyz);
			if (include_translation)
				Vec::div(invert.w.xyz,range,system.w.xyz);
			else
				Vec::clear(system.w.xyz);
			Mat::resetBottomRow(system);
		}



	
		String	getConfiguration()
		{
			return			 	"Fractal kernel " FRACTAL_VERSION "\n"
								"Secondary parent weight="+String(SECONDARY_PARENT_WEIGHT*100)+"%\n"
								"\nVertex size="+String(sizeof(TVertex))+
								"\nWater gain down="+String(WATER_GAIN_DOWN)+
								"\nWater gain up="+String(WATER_GAIN_UP)+
								"\nWater noise="+String(WATER_NOISE);
		}
	
		float		relativeSeamWidth(unsigned exponent)
		{
			return (1.0f+(float)TEXTURE_SEAM_EXTENSION)/(float)((1<<exponent)+1+1+TEXTURE_SEAM_EXTENSION);
		}
		float		relativeTexelSize(unsigned exponent)
		{
			return (1.0f)/(float)((1<<exponent)+1+1+TEXTURE_SEAM_EXTENSION);
		}
	
	
	}
}
