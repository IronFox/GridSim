#include "../../global_root.h"
#include "fractal.h"


/******************************************************************

E:\include\structure\fractal_map.cpp

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

#undef EXCEPTION
#define EXCEPTION(message)	FRAC_FATAL(message)

//#define DEBUG_POINT		cout << "\rf"<<__LINE__;
/*#define BEGIN
#define END
#define LBEGIN*/

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
	//#define BEGIN	//logfile<<"entering: "+CString(__func__)+"\n";
#endif
#ifndef END
	#define END		//cout<<spaces(--__log_depth)<<__func__<<"() done\n";
	//#define END		//logfile<<"exiting: "+CString(__func__)+"\n";
#endif


namespace CudaFractal
{
	#include "kernel/config.h"

	CMutexLog	logfile("fractal.log",true);
	CSurfaceSegment									*CBody::production(NULL);
	unsigned										num_sectors=0,
													max_sectors=20000,
													build_operations=1;
	static const float								ttl_loss_per_level = 0.8f;
	CTimer::Time									build_time = 1000000;
	bool											use_cuda=true;
	//::TContext										default_context = {10,1,10000,32};
	CTimer::Time									CBody::trace_time;		//!< Global time when tracing began
	CBuffer<CSurfaceSegment*>						CBody::need_children;

	static CPointerSet								array_table,
													segment_table;
	
	
	static inline void	registerArray(CUDA::CDeviceArray< ::TVertex>&array)
	{
		ASSERT__(array.onHost());
		
		array_table.set(array.hostPointer());
	}
	
	static inline void	unregisterArray(CUDA::CDeviceArray< ::TVertex>&array)
	{
		ASSERT__(array.onHost());
	
		array_table.unset(array.hostPointer());
	}
	
	static inline bool	arrayExists(CUDA::CDeviceArray< ::TVertex>&array)
	{
		ASSERT__(array.onHost());
	
		return array_table.isset(array.hostPointer());
	}
	
	static inline void	registerSegment(::THostSurface*segment)
	{
		segment_table.set(segment);
	}
	
	static inline void	unregisterSegment(::THostSurface*segment)
	{
		segment_table.unset(segment);
	}
	
	static inline bool	segmentExists(::THostSurface*segment)
	{
		return segment_table.isset(segment);
	}
	
	
	
	

	class CBackboneInit:public CThreadMain
	{
	public:
	static	Engine::OpenGL				*opengl;
			Engine::OpenGL::context_t	context;
			void						ThreadMain()
			{
				if (!CUDA::Device::emulation)
					CUDA_ASSERT(cudaSetDevice(CUDA::Device::index));
			
				context = opengl->linkContextClone();
			}
	};
	
	class CBackboneProcess:public CThreadMain
	{
	public:
			bool				do_finish;	//!< Set true if the process is supposed to return at the earliest possibility
			CTimer::Time		processing_time;	//!< Time spent in kernel processing
			float 				min_time_frame, max_time_frame;
			void				ThreadMain();
	};
	
	Engine::OpenGL						*CBackboneInit::opengl;
	
	static CBackboneInit				backbone_init;	//!< CThreadMain object to initialize a new background thread to operate on both opengl and potentially CUDA
	static CBackboneProcess				backbone_process;	//!< Actually working CThreadMain

	static CThreadPool					backbone;	//!< Single thread background processor
	
	//CReferenceVector<THostSurface>					surface_list;
	
	
	namespace	Adaption
	{
		#define dbClamped	clamped
		#define fminf		vmin
		#define fmaxf		vmax
		#define dbSign		sign
		#define dbCosStep	cosStep
		#define dbCubicStep	cubicStep
		#define dbSqr		sqr
		#define cSub		Composite::sub
		#define dvMad		_mad
		#define dvNormalize	_normalize
		#define dvDot		_dot
		#define dvDistance	_distance
		#define dvMult		_mult
		#define dvAdd		_add
		#define dvSub		_sub
		#define dvLength	_length
		#define __powf		powf
		
		inline float	ageAt(unsigned depth)
		{
			#include "kernel/age_at.function"
		}
		
		inline float	getWeight(float h, float average, float variance, float depth)
		{
			#include "kernel/get_weight.function"
		}
	
		inline float	getRandom(int&seed)
		{
			#include "kernel/get_random.function"
		}

		inline float	height(const TChannelConfig&channel)
		{
			#include "kernel/height.function"
		}

		
		inline float		getNoise(float distance, const TVertex&p0, const TVertex&p1, const THostSurface&surface, const TContext&context)
		{
			#include "kernel/get_noise.function"
		}
		
/* 		inline float	getSmoothStrength(float distance, const TVertex&p0, const TVertex&p1, const THostSurface&surface, const TContext&context)
		{
			#include "kernel/get_smooth_strength.function"
		}
 */		
		
	
		inline void		generate3(const TVertex&p0, const TVertex&p1, const TVertex&p2, TVertex&result, unsigned seed, THostSurface&surface, const TContext&context)
		{
			_center(p0.position,p1.position,result.position);
			#include "kernel/generate3.function"
		}

		inline void		generate4(const TVertex&p0, const TVertex&p1, const TVertex&p2, const TVertex&p3, TVertex&result, int seed, THostSurface&surface, const TContext&context)
		{
			_center(p0.position,p1.position,result.position);
			#include "kernel/generate4.function"
		}

		
		inline void		setHeight(float vector[3],float relative_height,const THostSurface&surface,const ::TContext&context)
		{
			#include "kernel/set_height.function"
		}
		
		inline void		setHeight(float vector[3],float current_height, float relative_height,const THostSurface&surface,const ::TContext&context)
		{
			#include "kernel/differential_set_height.function"
		}
	}
	
	
	
	#define INVOKING_DISCARD(ADDR)		//logfile << ">>DISCARD(0x"<<IntToHex((int)(ADDR),8)<<")"<<nl;
	#define INVOKED_DISCARD(ADDR)		//logfile << "<<DISCARD(0x"<<IntToHex((int)(ADDR),8)<<")"<<nl;
	#define INVOKING_VIRTUAL(ADDR,FUNC)	//logfile << ">>"<<#FUNC<<"(0x"<<IntToHex((int)(ADDR),8)<<")"<<nl;
	#define INVOKED_VIRTUAL(ADDR,FUNC)	//logfile << "<<"<<#FUNC<<"(0x"<<IntToHex((int)(ADDR),8)<<")"<<nl;
	
	
	void				reportFatalException(const TCodeLocation&location, const CString&message)
	{
		fatal(location,message);
	}
			

	void	setSeed(CRandom&random, const CSeed&seed)
	{
		random.setSeed(seed.toInt());
	}
			
	THostSurfaceLink link(CSurfaceSegment*sector, BYTE orientation)
	{
		THostSurfaceLink rs;
		rs.surface = sector;
		rs.orientation = orientation;
		return rs;
	}
	
	void	setRadialHeight(float v[3],double height, const CSurfaceSegment*context)
	{
		/*if (!_zero(context->sector.v))
		{
			v[0] += (double)context->sector.x*(double)context->super->sector_size;
			v[1] += (double)context->sector.y*(double)context->super->sector_size;
			v[2] += (double)context->sector.z*(double)context->super->sector_size;
			return;
		}*/
		double absolute_vector[3],delta_vector[3],len,delta;
		context->convertToAbsolute(v,absolute_vector);
		
		len = _length(absolute_vector);
		delta = height-len;
		if (vabs(len) < getError<double>())
			return;
		_mult(absolute_vector,delta/len,delta_vector);
		_add(v,delta_vector);
	}
	
	
	CString	randomWord()
	{
		static const char alpha[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_./\\";
		CRandom random;
		BYTE len = random.get(3,32);
		CString result;
		result.resize(len);
		for (BYTE k = 0; k < len; k++)
			result.set(k,alpha[random.get(0,strlen(alpha)-1)]);
		return result;
	}
	
	CSeed::CSeed()
	{
		randomize();
	}
	
	CSeed::CSeed(const CString&name)
	{
		operator=(name);
	}
		
	int				CSeed::toInt()	const
	{
		return CRC32::getChecksum(array.pointer(),array.length());
	}
	
	CString			CSeed::toString()	const
	{
		CString rs;
		for (unsigned i = 0; i < array.length(); i++)
		{
			if (isalnum((char)array[i]))
				rs += (char)array[i];
			else
				rs += "\\"+IntToHex((int)array[i],2);
		}
		return rs;
	}
	
	CSeed&			CSeed::operator=(const CString&string)
	{
		array.resize(string.length());
		array.copyFrom(string.c_str(),string.length());
		as_int = toInt();
		return *this;
	}
	
	void			CSeed::expand(const CSeed&seed, BYTE child_id)
	{
		array.resize(seed.array.length()+1);
		array.readFrom(seed.array,seed.array.length());
		array.last() = child_id;
		as_int = toInt();
	}
	
	void			CSeed::rotate(unsigned depth, BYTE orientation)
	{
		BYTE step = (BYTE)depth;
		for (unsigned i = 0;
				i < array.length();
				i++)
		{
			array[i]+=step;
			step += array[i]>0
					?orientation%array[i]
					:orientation;
		}
		as_int = toInt();
	}
	
	void			CSeed::randomize()
	{
		CRandom random;
		unsigned len = random.get(3,32);
		array.resize(len);
		for (unsigned i = 0; i < len; i++)
			array[i] = random.get(255);
		as_int = toInt();
	}



	CMap::CMap(TMapVertex *const vp,unsigned vr, unsigned vc, unsigned cvc, unsigned ivc, unsigned ctvc, unsigned pvc, unsigned* bp0, unsigned* bp1,unsigned* bp2,unsigned* ibp0, unsigned* ibp1, unsigned* ibp2,unsigned*const ivp, unsigned*const cvp, unsigned*const ctvp, TParentInfo*const pvp):
					vertex_info(vp),vertex_range(vr), vertex_maximum(vr-1),vertex_count(vc), child_vertex_count(cvc), inner_vertex_count(ivc), center_vertex_count(ctvc), parent_vertex_count(pvc),
					inner_vertex_index(ivp), child_vertex_index(cvp),	center_vertex_index(ctvp), parent_vertex_info(pvp)
	{
		BEGIN
		_v3(border_index,bp0,bp1,bp2);
		_v3(inner_border_index,ibp0,ibp1,ibp2);
		for (unsigned y = 0; y < vertex_range; y++)
			for (unsigned x = 0; x <= y; x++)
			{
				unsigned index = getIndex(x,y);
	
				TMapVertex&inf = vertex_info[index];
	
				inf.x = x;
				inf.y = y;
				inf.edge_index = -1;
				inf.border_index = 0;
				inf.index = index;
				
				inf.grid_vertex = !(x%2) && !(y%2);
				
				if (!x)
				{
					inf.edge_index = 0;
					inf.border_index = y;
				}
				else
					if (y == vertex_range-1)
					{
						inf.edge_index = 1;
						inf.border_index = x;
					}
					else
						if (x == y)
						{
							inf.edge_index = 2;
							inf.border_index = vertex_range-x-1;
						}
	
				if (!(y%2))
					if (!(x%2))
					{
						inf.dir = 255;
						inf.parent0 = inf.parent1 = inf.parent2 = inf.parent3 = inf.sibling0 = inf.sibling1 = inf.sibling2 = inf.sibling3 = UNSIGNED_UNDEF;
					}
					else
					{
						inf.dir = 1;
						inf.parent0 = getIndex(x-1,y);
						inf.parent1 = getIndex(x+1,y);

						inf.parent2 = getIndex(x-1,y-2);
						if (y < vertex_range-1)
						{
							inf.sibling0 = getIndex(x,y+1);
							inf.sibling1 = getIndex(x+1,y+1);
							inf.parent3 = getIndex(x+1,y+2);
						}
						else
						{
							inf.sibling0 = inf.sibling1 = UNSIGNED_UNDEF;
							inf.parent3 = UNSIGNED_UNDEF;
						}
						inf.sibling3 = getIndex(x-1,y-1);
						inf.sibling2 = getIndex(x,y-1);
					}
				else
					if (!(x%2))
					{
						inf.dir = 0;
						inf.parent0 = getIndex(x,y-1);
						inf.parent1 = getIndex(x,y+1);
						inf.parent2 = getIndex(x+2,y+1);
						if (x)
						{
							inf.sibling0 = getIndex(x-1,y-1);
							inf.sibling1 = getIndex(x-1,y);
							inf.parent3 = getIndex(x-2,y-1);
						}
						else
						{
							inf.sibling0 = inf.sibling1 = UNSIGNED_UNDEF;
							inf.parent3 = UNSIGNED_UNDEF;
						}
	
						inf.sibling2 = getIndex(x+1,y+1);
						inf.sibling3 = getIndex(x+1,y);
					}
					else
					{
						inf.dir = 2;
						inf.parent0 = getIndex(x+1,y+1);
						inf.parent1 = getIndex(x-1,y-1);
						inf.parent2 = getIndex(x-1,y+1);
	
						
						if (x != y)
						{
							inf.sibling0 = getIndex(x+1,y);
							inf.sibling1 = getIndex(x,y-1);
							inf.parent3 = getIndex(x+1,y-1);
						}
						else
						{
							inf.sibling0 = inf.sibling1 = UNSIGNED_UNDEF;
							inf.parent3 = UNSIGNED_UNDEF;
						}
					}                    
			}
	
	
	
		
		for (unsigned i = 0; i < vertex_range; i++)
		{
			border_index[0][i] = getIndex(0,i);
			border_index[1][i] = getIndex(i,vertex_range-1);
			border_index[2][vertex_range-i-1]  = getIndex(i,i);
		}
	
		for (unsigned i = 1; i < vertex_range; i++)
		{
			inner_border_index[0][i-1] = getIndex(1,i);
			inner_border_index[1][i-1] = getIndex(i-1,vertex_range-2);
			inner_border_index[2][vertex_range-i-1] = getIndex(i-1,i);
		}
	
	
	
	
		unsigned*current = child_vertex_index;
		
		for (BYTE k = 0; k < 3; k++)
		{
			unsigned*b = border_index[k];
			for (unsigned i = 1; i < vertex_range; i+=2)
			{
				(*current++) = b[i];
			}
		}
		
		for (unsigned y = 1; y < vertex_range; y+=2)
		{
			for (unsigned x = 2; x <= y; x+=2)
				(*current++) = getIndex(x,y);
			for (unsigned x = 1; x < y; x+=2)
				(*current++) = getIndex(x,y);
		}
		for (unsigned y = 2; y < vertex_range-1; y+=2)
			for (unsigned x = 1; x <=y; x+=2)
				(*current++) = getIndex(x,y);
				
		if (current != child_vertex_index+child_vertex_count)
		{
			ErrMessage("Generator Exception: expected "+IntToStr(child_vertex_count)+" (of "+IntToStr(vertex_count)+" total) generated vertices but got "+IntToStr(current-child_vertex_index));
			EXCEPTION("expected "+IntToStr(child_vertex_count)+" generated vertices but got "+IntToStr(current-child_vertex_index));
		}
		
		
		
		current = inner_vertex_index;
		
		for (unsigned y = 1; y < vertex_range; y+=2)
		{
			for (unsigned x = 2; x <= y; x+=2)
				(*current++) = getIndex(x,y);
			for (unsigned x = 1; x < y; x+=2)
				(*current++) = getIndex(x,y);
		}
		for (unsigned y = 2; y < vertex_range-1; y+=2)
			for (unsigned x = 1; x <=y; x+=2)
				(*current++) = getIndex(x,y);
				
		if (current != inner_vertex_index+inner_vertex_count)
		{
			ErrMessage("Generator Exception: expected "+IntToStr(inner_vertex_count)+" (of "+IntToStr(vertex_count)+" total) generated vertices but got "+IntToStr(current-inner_vertex_index));
			EXCEPTION("expected "+IntToStr(inner_vertex_count)+" generated vertices but got "+IntToStr(current-inner_vertex_index));
		}
		
		
		
		
		TParentInfo*pvi = parent_vertex_info;
		for (unsigned y = 0; y <= vertex_range/2; y++)
			for (unsigned x = 0; x <= y; x++)
				(pvi++)->index = getIndex(x*2,y*2);
	
		if (pvi != parent_vertex_info+parent_vertex_count)
		{
			ErrMessage("Generator Exception: expected "+IntToStr(parent_vertex_count)+" (of "+IntToStr(vertex_count)+" total) generated vertices but got "+IntToStr(pvi-parent_vertex_info));
			EXCEPTION("expected "+IntToStr(parent_vertex_count)+" generated vertices but got "+IntToStr(pvi-parent_vertex_info));
		}
	
		for (BYTE k = 0; k < 3; k++)
		{
			pvi = parent_vertex_info;
			unsigned	yoff = k?vertex_range/2:0,
						xoff = k==2?vertex_range/2:0;
	
			for (unsigned y = 0; y <= vertex_range/2; y++)
				for (unsigned x = 0; x <= y; x++)
					(pvi++)->origin[k] = getIndex(xoff+x,yoff+y);
		}
		pvi = parent_vertex_info;
	
		for (unsigned y = 0; y <= vertex_range/2; y++)
			for (unsigned x = 0; x <= y; x++)
				(pvi++)->origin[3] = getIndex(vertex_range/2-x, vertex_range-y-1);
	
	
		CArray<bool>	test_map(vertex_count);
		memset(test_map,false,vertex_count);
		
		for (unsigned i = 0; i < parent_vertex_count; i++)
			if (!test_map[parent_vertex_info[i].index])
				test_map[parent_vertex_info[i].index] = true;
			else
				EXCEPTION("map test failed for parent element "+IntToStr(i));
	
		for (unsigned i = 0; i < child_vertex_count; i++)
		{
			if (!test_map[child_vertex_index[i]])
				test_map[child_vertex_index[i]] = true;
			else
				EXCEPTION("map test failed for child element "+IntToStr(i));
			if (vertex_info[child_vertex_index[i]].parent0 == UNSIGNED_UNDEF)
				EXCEPTION("map test failed for parent0 of child element "+IntToStr(i));
			if (vertex_info[child_vertex_index[i]].parent1 == UNSIGNED_UNDEF)
				EXCEPTION("map test failed for parent1 of child element "+IntToStr(i));
	
		}
		END
	}
	

	CSurfaceSegment::CSurfaceSegment(unsigned exponent_, unsigned vertex_count_, CBody*super_, CSurfaceSegment*parent_,BYTE child_index_,const CTimer::Time now):
						exponent(exponent_),vertex_range((1<<exponent_)+1),vertex_count(vertex_count_),parent(parent_),super(super_),
						c0(NULL),c1(NULL),c2(NULL),c3(NULL),priority(0),
						attachment(NULL)
	{
		
		//registerSegment(this);
		
	
		if (parent)
			depth = parent->depth-1;
		else
			depth = 0;
		child_index = child_index_;
		if (parent)
			_c3(parent->sector,sector);
		else
			_clear(sector);
		//central_height = Composite::build(0,0);
		flags = RequiresUpdate;
		num_sectors++;
		//surface_list.append(this);
	}

	CSurfaceSegment::CSurfaceSegment(unsigned exponent_, unsigned vertex_count_):
						exponent(exponent_),vertex_range((1<<exponent_)+1),vertex_count(vertex_count_),parent(NULL),
						c0(NULL),c1(NULL),c2(NULL),c3(NULL),priority(0),
						attachment(NULL)
	{
		//registerSegment(this);
		
		depth = 0;
		child_index = 0;
		_clear(sector);
		//central_height = Composite::build(0,0);
		flags = RequiresUpdate;
		num_sectors++;
		//surface_list.append(this);
	}
	
	void CSurfaceSegment::finishBuild()
	{
		/*ASSERT__(vertex_field.length() > 0);
		if (vertex_field.onDevice())
			ASSERT_NOT_NULL__(vertex_field.devicePointer())
		else
			ASSERT_NOT_NULL__(vertex_field.hostPointer());*/
			
		//ARRAY_DEBUG_POINT(f,super->full_map->border_index[0][0]);
		CSurfaceSegment::corner[0] = vertex_field[super->full_map->border_index[0][0]].position;
		//ARRAY_DEBUG_POINT(f,super->full_map->border_index[1][0]);
		CSurfaceSegment::corner[1] = vertex_field[super->full_map->border_index[1][0]].position;
		//ARRAY_DEBUG_POINT(f,super->full_map->border_index[2][0]);
		CSurfaceSegment::corner[2] = vertex_field[super->full_map->border_index[2][0]].position;
		
		unsigned	middle_x = (super->full_map->vertex_range)/3,
					middle_y = (2*super->full_map->vertex_range)/3;
		::TVertex middle = vertex_field[super->full_map->getIndex(middle_x,middle_y)];
		CSurfaceSegment::water_center = middle.position;
		float dir[3];
		{
			dir[0] = middle.position[0]+super->sector_size*sector[0];
			dir[1] = middle.position[1]+super->sector_size*sector[1];
			dir[2] = middle.position[2]+super->sector_size*sector[2];
			_normalize(dir);
		}	
		_mad(water_center.v,dir,-super->variance*middle.height);
		//Adaption::setHeight(CSurfaceSegment::water_center.v,0, middle.height,*this,*super);		

		//DEBUG_POINT(f)
		updateSphere();
		
		for (BYTE k = 0; k < 3; k++)
		{
			if (neighbor_link[k].surface)
			{
				neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation].surface = this;
				neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation].orientation = k;
				neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation].primary = !orderOfEdge(k);
				neighbor_link[k].primary = orderOfEdge(k);
			}
		}
		
		flags |= RequiresUpdate;
	}
	

	struct TRiverNode
	{
		unsigned	index;
		::TVertex	vertex;
	};
	
	void CSurfaceSegment::build()
	{
		ASSERT_NOT_NULL__(parent);
		ASSERT_NOT_NULL__(super);
		
		#ifdef FRACTAL_BUILD_STATISTIC
			CTimer::Time t = timer.now();
		#endif
		
		CUDA::generateVertices(*this,*parent,*super,*super->device_info_field);
		
		#ifdef RIVERS
			if (depth >= super->min_river_layer && depth <= super->max_river_layer)
			{
				//vertex_field.suspend();	//localize vertices for rivers. this isn't much of a parallel 
				CRandom random(inner_seed.as_int);
				unsigned spawns = random.get(0,40);
				//cout << "tracing "<<spawns<<" river(s) at level "<<depth<<endl;
				for (unsigned i = 0; i < spawns; i++)
				{
					unsigned	y = random.get(2,super->edge_length-2),
								x = random.get(1,y-1),
								river_depth = 1,
								direction = -1;
					//cout << "river";
					static CArray<bool>	visited;
					if (visited.length() < vertex_field.length())
					{
						visited.resize(vertex_field.length());
						visited.fill(false);
					}
					static CBuffer<TRiverNode>	nodes;
					nodes.reset();
					while (true)
					{
						unsigned index = y*(y+1)/2+x;
						::TVertex vtx = vertex_field[index];
						ASSERT1__(!visited[index],index);
						
						float f_depth = 0.1f+vmin(0.1f* river_depth++,0.9f);
						//vmin(50.0f*(1<<this->depth)/(1<<super->recursive_depth)* depth,1.0);
						if (vtx.channel.river_depth >= f_depth || (super->has_ocean && vtx.height < 0))
							break;
						visited[index] = true;
						
						float next_height = vtx.height;
						
						vtx.channel.river_depth = f_depth;
						vtx.height = Adaption::height(vtx.channel);
						Adaption::setHeight(vtx.position,next_height, vtx.height,*this,*super);
						
						TRiverNode&node = nodes.append();
						node.index = index;
						node.vertex = vtx;
						
						
						//cout << "->("<<x<<", "<<y<<"/"<<index<<")";
						
						unsigned	next_x,
									next_y,
									next_direction;
						bool		found = false;
						
						#undef RIVER_TEST
						#define RIVER_TEST(_X_,_Y_,_DIRECTION_,_TOLERATES0_,_TOLERATES1_)	\
								{\
									unsigned 	x_ = (_X_),\
												y_ = (_Y_);\
									if (x_ && y_ && x_ != y_ && y_+1<super->edge_length)\
									{\
										unsigned index_ = y_*(y_+1)/2+x_;\
										if (!visited[index_])\
										{\
											float h = vertex_field[index_].height-0.05f-random.getFloat(0.05f)-0.03f*(direction==-1 || _DIRECTION_==direction||_TOLERATES0_==direction || _TOLERATES1_==direction);\
											if (h<next_height)\
											{\
												found = true;\
												next_height = h;\
												next_x = x_;\
												next_y = y_;\
												next_direction = _DIRECTION_;\
											}\
										}\
									}\
								}
						
						RIVER_TEST(x-1,y-1,	0,1,2)
						RIVER_TEST(x,y-1,	1,0,4)
						RIVER_TEST(x-1,y,	2,0,3)
						RIVER_TEST(x+1,y,	3,5,2)
						RIVER_TEST(x,y+1,	4,1,5)
						RIVER_TEST(x+1,y+1,	5,4,3)
						
						if (!found)
							break;
						x = next_x;
						y = next_y;
						direction = next_direction;
					}
					//cout << endl;
					if (nodes.count() > 3)
					{
						cout << "river ("<<nodes.count()<<")"<<endl;
						for (unsigned i = 0; i < nodes.count(); i++)
							vertex_field.set(nodes[i].index,nodes[i].vertex);
					}
					for (unsigned i = 0; i < nodes.count(); i++)
						visited[nodes[i].index] = false;
				}
			}	
			
		
		
		#endif
		
		
		CUDA::generateNormals(*this,*super,*super->device_info_field,super->full_map->vertex_range-1);
		
		
		
		
		flags |= HasData;
		finishBuild();
		
		#ifdef FRACTAL_BUILD_STATISTIC
			build_time += timer.now()-t;
			build_operations++;
		#endif
	}
	
	void	CSurfaceSegment::mergeEdges()
	{
		/*ASSERT__(segmentExists(this));
		ASSERT__(arrayExists(vertex_field));
		
		for (BYTE k = 0; k < 3; k++)
			if (neighbor_link[k].surface)
			{
				ASSERT1__(segmentExists(neighbor_link[k].surface),k);
				ASSERT1__(arrayExists(neighbor_link[k].surface->vertex_field),k);
			}
		*/
		CUDA::mergeEdges(*this,*super,*super->device_info_field,super->device_border_field,super->full_map->vertex_range);
		//DEBUG_POINT
	}
	
	/*void	CSurfaceSegment::updateClouds()
	{
		CUDA::updateCloudVBO(*this, *super, *super->device_cloud_triangles, 0);
	}*/
	
	void	CSurfaceSegment::update()
	{
		
		if (flags & (RequiresUpdate | RequiresTextureUpdate))
		{
			if (!(flags & HasData))
				build();
			#ifdef FRACTAL_BUILD_STATISTIC
				CTimer::Time t = timer.now();
			#endif
			

			CUDA::updateTexture(*this,*super,*super->device_info_field,super->fractal_clouds);
			flags &= ~RequiresTextureUpdate;
			if (flags & RequiresUpdate)
			{
				CUDA::updateVBO(*this,*super,*super->device_info_field,*super->device_vertex_index_field,super->full_map->vertex_range);
				flags &= ~RequiresUpdate;

				#ifdef FRACTAL_BUILD_STATISTIC
					build_time += timer.now()-t;
					build_operations++;
				#endif
				if (onUpdate)
					onUpdate(this);
			}
			#ifdef FRACTAL_BUILD_STATISTIC
			else
			{

				build_time += timer.now()-t;
				build_operations++;
			}
			#endif

			
		}
	}
	
	void	CSurfaceSegment::getCoverVertices(CArray< ::TCoverVertex>&cover_vertices)
	{
		CUDA::retrieveCover(*this,*super,*super->device_vertex_index_field,cover_vertices);
	}

	
	bool CSurfaceSegment::orderOfEdge(BYTE edge)	const
	{
		return corner[(edge+1)%3].x>corner[edge].x;
	}
		
	void CSurfaceSegment::createChildren(const CTimer::Time&time)
	{
		if (!(flags & HasData))
			reportFatalException(CLOCATION,"children requested but data not yet generated");
	
		if (flags&HasChildren)
			return;
			
		if (!( ((flags & Edge0Merged) || (flags & Edge0Open))
			&& ((flags & Edge1Merged) || (flags & Edge1Open))
			&& ((flags & Edge2Merged) || (flags & Edge2Open))))
			reportFatalException(CLOCATION,"edges not all merged but creating children (flags are "+IntToHex((int)flags,8)+")");
			
		
		//logfile << "(creating children)\n";
		
	
		//static CMutex   mutex;
		//mutex.lock();	//To ensure fixed child creation order (no children of neighbors created this very instance) - no longer necessary since children are generated sequentially
		
			c0 = new CSurfaceSegment(exponent, vertex_count, super,this,0,time);
				_v3(c0->neighbor_link,resolveChild(neighbor_link[0],1),THostSurfaceLink(),resolveChild(neighbor_link[2],0));
				c0->outer_seed[0].expand(outer_seed[0],orderOfEdge(0));
				c0->outer_seed[1].expand(inner_seed,0);
				c0->outer_seed[2].expand(outer_seed[2],!orderOfEdge(2));
				c0->inner_seed.expand(outer_seed[1],0);
				c0->reinit();
			c1 = new CSurfaceSegment(exponent, vertex_count, super,this,1,time);
				_v3(c1->neighbor_link,resolveChild(neighbor_link[0],0),resolveChild(neighbor_link[1],1),THostSurfaceLink());
				c1->outer_seed[0].expand(outer_seed[0],!orderOfEdge(0));
				c1->outer_seed[1].expand(outer_seed[1],orderOfEdge(1));
				c1->outer_seed[2].expand(inner_seed,1);
				c1->inner_seed.expand(outer_seed[2],1);
				c1->reinit();
			c2 = new CSurfaceSegment(exponent, vertex_count, super,this,2,time);
				_v3(c2->neighbor_link,THostSurfaceLink(),resolveChild(neighbor_link[1],0),resolveChild(neighbor_link[2],1));
				c2->outer_seed[0].expand(inner_seed,2);
				c2->outer_seed[1].expand(outer_seed[1],!orderOfEdge(1));
				c2->outer_seed[2].expand(outer_seed[2],orderOfEdge(2));
				c2->inner_seed.expand(outer_seed[0],2);
				c2->reinit();
			c3 = new CSurfaceSegment(exponent, vertex_count, super,this,3,time);
				_v3(c3->neighbor_link,link(c2,0),link(c0,1),link(c1,2));
				c3->outer_seed[0] = c2->outer_seed[0];
				c3->outer_seed[1] = c0->outer_seed[1];
				c3->outer_seed[2] = c1->outer_seed[2];
				c3->inner_seed.expand(inner_seed,3);
				c3->reinit();
		
		flags |= HasChildren;
	}

	
	CSurfaceSegment* CSurfaceSegment::createChild(const CTimer::Time&time)
	{
		if (!(flags&HasData))
			reportFatalException(CLOCATION,"children requested but data not yet generated");
	
		if (flags&HasChildren)
			return NULL;
		
		if (!c0)
		{
			c0 = new CSurfaceSegment(exponent, vertex_count, super,this,0,time);
				//_v3(c0->neighbor_link,resolveChild(neighbor_link[0],1),THostSurfaceLink(),resolveChild(neighbor_link[2],0));
				//_set(c0->neighbor_link,THostSurfaceLink());
				c0->outer_seed[0].expand(outer_seed[0],orderOfEdge(0));
				c0->outer_seed[1].expand(inner_seed,0);
				c0->outer_seed[2].expand(outer_seed[2],!orderOfEdge(2));
				c0->inner_seed.expand(outer_seed[1],0);
				c0->reinit();
			return c0;
		}
		if (!c1)
		{
			c1 = new CSurfaceSegment(exponent, vertex_count, super,this,1,time);
				//_v3(c1->neighbor_link,resolveChild(neighbor_link[0],0),resolveChild(neighbor_link[1],1),THostSurfaceLink());
				//_set(c1->neighbor_link,THostSurfaceLink());
				c1->outer_seed[0].expand(outer_seed[0],!orderOfEdge(0));
				c1->outer_seed[1].expand(outer_seed[1],orderOfEdge(1));
				c1->outer_seed[2].expand(inner_seed,1);
				c1->inner_seed.expand(outer_seed[2],1);
				c1->reinit();
			return c1;
		}
		if (!c2)
		{
			c2 = new CSurfaceSegment(exponent, vertex_count, super,this,2,time);
				//_v3(c2->neighbor_link,THostSurfaceLink(),resolveChild(neighbor_link[1],0),resolveChild(neighbor_link[2],1));
				//_set(c2->neighbor_link,THostSurfaceLink());
				c2->outer_seed[0].expand(inner_seed,2);
				c2->outer_seed[1].expand(outer_seed[1],!orderOfEdge(1));
				c2->outer_seed[2].expand(outer_seed[2],orderOfEdge(2));
				c2->inner_seed.expand(outer_seed[0],2);
				c2->reinit();
			return c2;
		}
		flags |= HasChildren;
		ASSERT__(!c3);
		

		c0->flags |= RequiresUpdate;
		c1->flags |= RequiresUpdate;
		c2->flags |= RequiresUpdate;
		
		c3 = new CSurfaceSegment(exponent, vertex_count, super,this,3,time);
			_v3(c3->neighbor_link,link(c2,0),link(c0,1),link(c1,2));
			c3->outer_seed[0] = c2->outer_seed[0];
			c3->outer_seed[1] = c0->outer_seed[1];
			c3->outer_seed[2] = c1->outer_seed[2];
			c3->inner_seed.expand(inner_seed,3);
			c3->reinit();
			
		c0->setNeighbors(resolveChild(neighbor_link[0],1),	link(NULL,1),						resolveChild(neighbor_link[2],0));
		c1->setNeighbors(resolveChild(neighbor_link[0],0),	resolveChild(neighbor_link[1],1),	link(NULL,2));
		c2->setNeighbors(link(NULL,0),						resolveChild(neighbor_link[1],0),	resolveChild(neighbor_link[2],1));
		

		
		return c3;
	}

	
	
	CDataSurface::CDataSurface(TVertex*vertices, unsigned exponent_, unsigned vertex_count_):
						CSurfaceSegment(exponent_,vertex_count_),	vertex_pointer(vertices)
						
	{}


	
	void			CSurfaceSegment::reinit()
	{
		if (parent)
		{
			depth = parent->depth-1;
			
			_c3(parent->sector,sector);
			
			unsigned	i0 = super->full_map->parent_vertex_info[0].origin[child_index],
						i1 = super->full_map->parent_vertex_info[super->full_map->getIndex(0,(super->full_map->vertex_range-1)/2)].origin[child_index],
						i2 = super->full_map->parent_vertex_info[super->full_map->getIndex((super->full_map->vertex_range-1)/2,(super->full_map->vertex_range-1)/2)].origin[child_index];
			
			float center[3];
			_center(parent->vertex_field[i0].position,parent->vertex_field[i1].position,parent->vertex_field[i2].position,center);

			sector_t sdelta[3];
			
			_div(center,super->sector_size,sdelta);
			_add(sector,sdelta);
			
			/*
				doesn't fire
			ASSERT__(segmentExists(this));
			
			for (BYTE k = 0; k < 3; k++)
				if (neighbor_link[k].surface)
					ASSERT1__(segmentExists(neighbor_link[k].surface),k);
			*/
			
			for (BYTE k = 0; k < 3; k++)
			{
				if (neighbor_link[k].surface)
				{
					neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation].surface = this;
					neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation].orientation = k;
					neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation].primary = !orderOfEdge(k);
					neighbor_link[k].primary = orderOfEdge(k);
				}
			}		
			
			
			//lout << "sector central sector set to "<<_toString(sector)<<nl;
		}
		else
		{
			_clear(sector);
			if (super)
				depth = super->recursive_depth-1;
			else
				depth = 0;
		}
		
			
		
		
		double absolute[3];
		_c3(sector,absolute);
		double sector_height = _length(absolute);
		//central_height.offset = (int)sector_height;
		//central_height.remainder = (float)(sector_height-(double)central_height.offset)*super->sector_size;
		
		inner_seed.rotate(depth,child_index);
		
		
		::THostSurface::inner_seed = inner_seed.as_int;
		::THostSurface::outer_seed[0] = outer_seed[0].as_int;
		::THostSurface::outer_seed[1] = outer_seed[1].as_int;
		::THostSurface::outer_seed[2] = outer_seed[2].as_int;
		
		//lout << "central height is now <"<<central_height.offset<<", "<<central_height.remainder<<"> ("<<Composite::toDouble(central_height,super->sector_size)<<")"<<nl;
		

		/*outer_seed[0].expand(seed,0);
		outer_seed[1].expand(seed,1);
		outer_seed[2].expand(seed,3);
		inner_seed.expand(seed,4);*/
	}
	
	void			CSurfaceSegment::modifySphere(const CAbstractSphere<float>&child_sphere)
	{
		global_sphere.include(child_sphere);
		if (parent)
			parent->modifySphere(global_sphere);
	}
	
	THostSurfaceLink			CSurfaceSegment::resolveChild(const THostSurfaceLink&ref, BYTE index)
	{
		if (!ref.surface || !(ref.surface->flags&HasChildren))
			return THostSurfaceLink();
		return link(((CSurfaceSegment*)ref.surface)->child[(index+ref.orientation)%3],ref.orientation);
	}
	
	THostSurfaceLink			CSurfaceSegment::resolveChildRec(const THostSurfaceLink&ref, BYTE index)
	{
		if (!ref.surface || /*!((CSurfaceSegment*)ref.surface)->child[(index+ref.orientation)%3]*/!(ref.surface->flags&HasChildren) || ((CSurfaceSegment*)ref.surface)->priority <= 0)
			return ref;
		return link(((CSurfaceSegment*)ref.surface)->child[(index+ref.orientation)%3],ref.orientation);
	}
	
			
	CSurfaceSegment::~CSurfaceSegment()
	{
		BEGIN
		/*if (vertex_field.length())
			unregisterArray(vertex_field);
		unregisterSegment(this);*/
		if (CBody::production == this)
			CBody::production = NULL;
		if (attachment)
			DISCARD(attachment);
		/*lout << "destructor invocation on "<<this<<". unhooking..."<<nl;
		flush(lout);*/
		//logfile << "unhooking\r\n";
		if (!application_shutting_down)
			unhook();	//cheap cheat, but whatever, if it works ;P
		//logfile << "unhooked\r\n";
		if (c0)
			delete c0;
		if (c1)
			delete c1;
		if (c2)
			delete c2;
		if (c3)
			delete c3;

		num_sectors--;
		/*lout << "destructor finished"<<nl;
		flush(lout);*/
		
		//surface_list.drop(this);

		END
	}
	
			
	void		CSurfaceSegment::setNeighbors(const THostSurfaceLink&link0, const THostSurfaceLink&link1, const THostSurfaceLink&link2)
	{
		BEGIN
		neighbor_link[0] = link0;
		neighbor_link[1] = link1;
		neighbor_link[2] = link2;
		for (BYTE k = 0; k < 3; k++)
			if (neighbor_link[k].surface)
			{
				THostSurfaceLink&n = neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation];
				n.orientation = k;
				n.surface = this;
				n.primary = !orderOfEdge(k);
				neighbor_link[k].primary = orderOfEdge(k);
			}
		END
	}
	
	void		CDataSurface::copyEdge(BYTE k)
	{
		BEGIN
		THostSurfaceLink&n = neighbor_link[k];
		if (!n.surface)
			return;
		unsigned*border0 = super->full_map->border_index[k],
				*border1 = super->full_map->border_index[n.orientation];
		for (unsigned i = 0; i < vertex_range; i++)
			vertex_pointer[border0[i]] = ((CDataSurface*)n.surface)->vertex_pointer[border1[vertex_range-1-i]];
		END
	}
	
	void		CDataSurface::fillAllNormals()
	{
		BEGIN
		for (unsigned i = 0; i < vertex_count; i++)
		#if NORMAL_GENERATION==DOUBLE_NORMAL_STORAGE
			_clear(vertex_pointer[i].base_normal);
		#else
			_clear(vertex_pointer[i].normal);
		#endif
		
		for (unsigned y = 1; y < vertex_range; y++)
		{
			for (unsigned x = 0; x < y; x++)
			{
				TVertex	&v0 = getVertex(x,y),
						&v1 = getVertex(x+1,y),
						&v2 = getVertex(x,y-1);
	
				float	normal[3];
				_oTriangleNormal(v0.position,v1.position,v2.position,normal);
				#if NORMAL_GENERATION==DOUBLE_NORMAL_STORAGE
					_add(v0.base_normal,normal);
					_add(v1.base_normal,normal);
					_add(v2.base_normal,normal);
				#else
					_add(v0.normal,normal);
					_add(v1.normal,normal);
					_add(v2.normal,normal);
				#endif
			}
			for (unsigned x = 0; x < y-1; x++)
			{
				TVertex	&v0 = getVertex(x+1,y),
						&v1 = getVertex(x+1,y-1),
						&v2 = getVertex(x,y-1);
	
				float	normal[3];
				_oTriangleNormal(v0.position,v1.position,v2.position,normal);
				#if NORMAL_GENERATION==DOUBLE_NORMAL_STORAGE
					_add(v0.base_normal,normal);
					_add(v1.base_normal,normal);
					_add(v2.base_normal,normal);
				#else
					_add(v0.normal,normal);
					_add(v1.normal,normal);
					_add(v2.normal,normal);
				#endif
			}
		}
		#if NORMAL_GENERATION==DOUBLE_NORMAL_STORAGE
			for (unsigned i = 0; i < vertex_count; i++)
				_c3(vertex_pointer[i].base_normal,vertex_pointer[i].normal);
		#endif
		END
	}
	

	

	
	
	
	void		CSurfaceSegment::resetLinkage()
	{
		unhook();
		neighbor_link[0] = THostSurfaceLink();
		neighbor_link[1] = THostSurfaceLink();
		neighbor_link[2] = THostSurfaceLink();
	}
	
	void		CSurfaceSegment::unhook()
	{
		for (BYTE k = 0; k < 3; k++)
			if (neighbor_link[k].surface)
			{
				//ASSERT__(surface_list(neighbor_link[k].surface)>0);
				/*lout << "attempting to unset neighbor link (orientation "<<(int)neighbor_link[k].orientation<<") on neighbor "<<(int)k<<" ("<<neighbor_link[k].surface<<")"<<nl;
				flush(lout);*/
				neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation] = THostSurfaceLink();
				neighbor_link[k] = THostSurfaceLink();
			}
		/*lout << "unhooked"<<nl;
		flush(lout);*/
	}
	

	void	CDataSurface::update()
	{
		if (flags & RequiresUpdate)
		{
			vertex_field.resize(vertex_count,!CUDA::Device::emulation);
			vertex_field.copyFrom(vertex_pointer);
			CUDA::generateNormals(*this,*super,*super->device_info_field,super->full_map->vertex_range-1);
			flags |= HasData;
			//CSurfaceSegment::update();
		}
		/*
		for (BYTE k = 0; k < 3; k++)
		{
			if (neighbor_link[k].surface)
			{
				neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation].surface = this;
				neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation].orientation = k;
				neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation].primary = !orderOfEdge(k);
				neighbor_link[k].primary = orderOfEdge(k);
			}
		}*/
	}


	/*
		Old, complicated, deprecated
	*/
	void		CSurfaceSegment::resolve(const THostSurfaceLink&l0, const THostSurfaceLink&l1, const THostSurfaceLink&l2)
	{
	//    logfile << "visaddr("<<PointerToHex(this,8)<<"): "<<IntToHex((int)l0.link,8)<<" "<<IntToHex((int)l1.link,8)<<" "<<IntToHex((int)l2.link)<<nl;
		surface.delta0 = l0.surface?l0.surface->depth:this->depth;
		surface.delta1 = l1.surface?l1.surface->depth:this->depth;
		surface.delta2 = l2.surface?l2.surface->depth:this->depth;
		
	//    logfile << "vis: "<<vis0 << " "<<vis1 << " "<<vis2 << nl;
	
		if (priority > 0 && (flags&HasChildren))
		{
			c0->resolve(resolveChildRec(l0,1), link(c3,1),resolveChildRec(l2,0));
			c1->resolve(resolveChildRec(l0,0),resolveChildRec(l1,1),link(c3,2));
			c2->resolve(link(c3,0),resolveChildRec(l1,0),resolveChildRec(l2,1));
			c3->resolve();
		}
    }

	/*
		New, simpler, faster
	*/
	void		CSurfaceSegment::resolve(unsigned d0, unsigned d1, unsigned d2)
	{
	//    logfile << "visaddr("<<PointerToHex(this,8)<<"): "<<IntToHex((int)l0.link,8)<<" "<<IntToHex((int)l1.link,8)<<" "<<IntToHex((int)l2.link)<<nl;

		surface.delta0 = d0;
		surface.delta1 = d1;
		surface.delta2 = d2;
		

			
	//    logfile << "vis: "<<vis0 << " "<<vis1 << " "<<vis2 << nl;
	
		if (priority > 0 && (flags&HasChildren))
		{
			if (d0 == this->depth && neighbor_link[0].surface &&  !!(neighbor_link[0].surface->flags&HasChildren) && ((CSurfaceSegment*)neighbor_link[0].surface)->priority)	//advance
				d0--;
			if (d1 == this->depth && neighbor_link[1].surface &&  !!(neighbor_link[1].surface->flags&HasChildren) && ((CSurfaceSegment*)neighbor_link[1].surface)->priority)	//advance
				d1--;
			if (d2 == this->depth && neighbor_link[2].surface &&  !!(neighbor_link[2].surface->flags&HasChildren) && ((CSurfaceSegment*)neighbor_link[2].surface)->priority)	//advance
				d2--;
			
			c0->resolve(d0, this->depth-1, d2);
			c1->resolve(d0, d1,	this->depth-1);
			c2->resolve(this->depth-1,	d1,	d2);
			c3->resolve();
		}

    }

	
	void		CSurfaceSegment::resolve()
	{
		//resolve(neighbor_link[0],neighbor_link[1],neighbor_link[2]);
		resolve(this->depth,this->depth,this->depth);
	}
	

	/*
		Old, complicated, deprecated
	*/
	void		CSurfaceSegment::resolveReflection(const THostSurfaceLink&l0, const THostSurfaceLink&l1, const THostSurfaceLink&l2)
	{

	//    logfile << "visaddr("<<PointerToHex(this,8)<<"): "<<IntToHex((int)l0.link,8)<<" "<<IntToHex((int)l1.link,8)<<" "<<IntToHex((int)l2.link)<<nl;
		reflection.delta0 = l0.surface?l0.surface->depth:this->depth;
		reflection.delta1 = l1.surface?l1.surface->depth:this->depth;
		reflection.delta2 = l2.surface?l2.surface->depth:this->depth;
		
	//    logfile << "vis: "<<vis0 << " "<<vis1 << " "<<vis2 << nl;
	
		if ((flags & SubdivReflection) && (flags&HasChildren))
		{
			c0->resolveReflection(resolveChildRec(l0,1), link(c3,1),resolveChildRec(l2,0));
			c1->resolveReflection(resolveChildRec(l0,0),resolveChildRec(l1,1),link(c3,2));
			c2->resolveReflection(link(c3,0),resolveChildRec(l1,0),resolveChildRec(l2,1));
			c3->resolveReflection();
		}

    }
	
	/*
		New, simpler, faster
	*/
	void		CSurfaceSegment::resolveReflection(unsigned d0, unsigned d1, unsigned d2)
	{

		reflection.delta0 = d0;
		reflection.delta1 = d1;
		reflection.delta2 = d2;
		if ((flags & SubdivReflection) && (flags&HasChildren))
		{
			if (d0 == this->depth && neighbor_link[0].surface &&  (neighbor_link[0].surface->flags&HasChildren) && (neighbor_link[0].surface->flags&SubdivReflection))	//advance
				d0--;
			if (d1 == this->depth && neighbor_link[1].surface &&  (neighbor_link[1].surface->flags&HasChildren) && (neighbor_link[1].surface->flags&SubdivReflection))	//advance
				d1--;
			if (d2 == this->depth && neighbor_link[2].surface &&  (neighbor_link[2].surface->flags&HasChildren) && (neighbor_link[2].surface->flags&SubdivReflection))	//advance
				d2--;
			
			c0->resolveReflection(d0, this->depth-1, d2);
			c1->resolveReflection(d0, d1,	this->depth-1);
			c2->resolveReflection(this->depth-1,	d1,	d2);
			c3->resolveReflection();
		}

    }	
	
	void		CSurfaceSegment::resolveReflection()
	{
		//resolveReflection(neighbor_link[0],neighbor_link[1],neighbor_link[2]);
		resolveReflection(this->depth,this->depth,this->depth);
	}
	
	void		CSurfaceSegment::resolveWater(unsigned d0, unsigned d1, unsigned d2)
	{

		water.delta0 = d0;
		water.delta1 = d1;
		water.delta2 = d2;
		if ((flags & SubdivWater) && (flags&HasChildren))
		{
			if (d0 == this->depth && neighbor_link[0].surface &&  (neighbor_link[0].surface->flags&HasChildren) && (neighbor_link[0].surface->flags&SubdivWater))	//advance
				d0--;
			if (d1 == this->depth && neighbor_link[1].surface &&  (neighbor_link[1].surface->flags&HasChildren) && (neighbor_link[1].surface->flags&SubdivWater))	//advance
				d1--;
			if (d2 == this->depth && neighbor_link[2].surface &&  (neighbor_link[2].surface->flags&HasChildren) && (neighbor_link[2].surface->flags&SubdivWater))	//advance
				d2--;
			
			unsigned dn = this->depth-1;
			c0->resolveWater(d0, dn, d2);
			c1->resolveWater(d0, d1, dn);
			c2->resolveWater(dn, d1, d2);
			c3->resolveWater(dn, dn, dn);
		}

    }	
	
	void		CSurfaceSegment::resolveWater()
	{
		//resolveReflection(neighbor_link[0],neighbor_link[1],neighbor_link[2]);
		resolveWater(this->depth,this->depth,this->depth);
	}
	

	void		CSurfaceSegment::dropChildren()
	{
		if (c0)
			delete c0;
		if (c1)
			delete c1;
		if (c2)
			delete c2;
		if (c3)
			delete c3;
		c0 = NULL;
		c1 = NULL;
		c2 = NULL;
		c3 = NULL;
		
		if (CBody::production == this)	//can't continue production on this segment with the children gone
			CBody::production = NULL;
		
		flags &= ~HasChildren;
		updateSphere();
	}
	
	int			CSurfaceSegment::setPriority(const CView&view,bool override_only,const CTimer::Time&time)
	{
		ASSERT__(flags & HasData);
		
		bool has_override = !!(visibility & Visibility::Override);
		visibility = Visibility::Invisible;
		if (!has_override)
		{
			priority = 0;
			
			if (override_only)
				return 0;
		}
		
		//const CView&view = *project_parameters.view;
		
		float center[3],axis[3],normal[3];
		getTranslation(view.coordinates,super->center,this->sector,translation,super->sector_size);
		_add(translation,global_sphere.center.vector,center);
		
		_sub(center,view.aspect.absoluteLocation(),axis);
		_oTriangleNormal(corner[0].v,corner[1].v,corner[2].v,normal);
		_normalize(normal);
		
		float	d = distance_square = _dot(axis),
				r = sqr(global_sphere.radius);
		float fc = (float)(depth+1)/(float)super->recursive_depth;
		
		_normalize(axis);
		float	effective_distance = d,
				focus_barrier = r
								
								*sqr(2*(0.35+0.65*fc)*view.lod*(fc+(1.0f-fc)*vmax(0.0f,-_dot(axis,normal))));
		/*if (effective_distance > 2*focus_barrier && depth < super->recursive_depth-1)
			return 0;*/
		//visibility = Visibility::Range;
		


		
		if (!override_only && view.frustrum.isVisible(center,global_sphere.radius))
			visibility |= Visibility::Primary;
		if ((visibility & (Visibility::Primary)) || has_override)
		{
			if (effective_distance <= focus_barrier && depth > view.min_layer)
			{
				if (visibility & (Visibility::Primary))
					priority = (int)((focus_barrier/effective_distance)*100)+1;
				
				if (   ((flags & Edge0Merged) || (flags & Edge0Open))
					&& ((flags & Edge1Merged) || (flags & Edge1Open))
					&& ((flags & Edge2Merged) || (flags & Edge2Open))
					)
				{
					/*else
						leave priority as assigned*/
				}
				else
				{
					//want to subdivide but neighbors aren't available...
					
					if (!parent)	//this might happen if the base mesh wasn't closed
					{
						//flags |= Edge0Merged | Edge1Merged | Edge2Merged;
						FATAL__("Trying to determine priority of un-merged parent-less surface segment. Such a segment should not exist.");
					}
					else
						for (BYTE k = 0; k < 3; k++)
							if (!(flags & (Edge0Merged << k)) && !(flags & (Edge0Open << k)))
							{
								//edge k not merged. we need all
								//cout << "setPriority(...): invoking set flag method"<<endl;
								parent->triggerOverrideFlag(child_index,k,time,priority);
								//cout << "setPriority(...): done"<<endl;
							}
					priority = 0;
				}
			}
			else
				priority = 0;
		}
		else
			priority = 0;	//this should be implied
		
		

		/*
		if (view.require_reflection)
		{
			double abs[3];
			_mad(global_sphere.center.vector,sector,super->sector_size,abs);
			double h = _length(abs)-super->base_heightf;
			if (h > 0 && h < super->variance/3)
			{
				float reflected_center[3];
				_c3(global_sphere.center.vector,reflected_center);
				
				Adaption::setHeight(reflected_center,-h,*this,*super);
				_add(reflected_center,translation);
				if (view.frustrum.isVisible(reflected_center,global_sphere.radius))
				{
					visibility |= Visibility::Reflection;
				}
			}
		}
		*/

	
		return priority;
	}
	
	bool			CSurfaceSegment::triggerOverrideFlag(const CTimer::Time&now, unsigned priority_to_set)
	{
		//cout << "triggerOverrideFlag(): setting flag"<<endl;
		BEGIN
		if (!this || !(flags&HasData) || (parent && !(parent->flags&HasChildren)))
		{
			END
			return false;
		}
		visibility |= Visibility::Override;
		referenced = now;
		
		bool can_subdivide =
			(	((flags & Edge0Merged) || (flags & Edge0Open))
			&&	((flags & Edge1Merged) || (flags & Edge1Open))
			&&	((flags & Edge2Merged) || (flags & Edge2Open))
			);
		if (can_subdivide)
			priority += priority_to_set;
		else
		{
			//cout << "triggerOverrideFlag():  cannot subdivide"<<endl;
			for (BYTE k = 0; k < 3; k++)
				if (!(flags & (Edge0Merged << k)) && !(flags & (Edge0Open << k)))
				{
					//edge k not merged. we need all
					//cout << "triggerOverrideFlag():  invoking override flag set"<<endl;
					parent->triggerOverrideFlag(child_index,k,now, priority_to_set);
				}
		}
		//cout << "triggerOverrideFlag():  setting flag for parents"<<endl;
		CSurfaceSegment*p = parent;
		while (p)
		{
			//cout << "triggerOverrideFlag():  setting flag for "<<p<<endl;
			p->visibility |= Visibility::Override;
			p->referenced = now;
			p->priority += priority_to_set;
			p = p->parent;
		}
		END
		return true;
	}
	
	void			CSurfaceSegment::triggerOverrideFlag(BYTE child_index, BYTE neighbor_index,const CTimer::Time&now, unsigned priority_to_set)
	{
		BEGIN
		ASSERT__(!!(flags&HasChildren));	//how on earth could this thing fire...???
		
		ASSERT1__(child_index <= 3, child_index);
		//cout << "triggerOverrideFlag(...): invoked for "<<(int)child_index<<", "<<(int)neighbor_index<<endl;
		if (child_index == 3)
		{
			BYTE down = (neighbor_index+2)%3;
			//cout << "triggerOverrideFlag(...):  invoking nested for child "<<down<<endl;
			child[down]->triggerOverrideFlag(now,priority_to_set);
		}
		else
			if (((child_index+1)%3) == neighbor_index)
			{
				//cout << "triggerOverrideFlag(...):  invoking nested for central child"<<endl;
				c3->triggerOverrideFlag(now,priority_to_set);
			}
			else
				if (!((CSurfaceSegment*)neighbor_link[neighbor_index].surface)->triggerOverrideFlag(now,priority_to_set))	//NULL-pointer sensitive
					if (parent)
					{
						//cout << "triggerOverrideFlag(...):  invoking recursive"<<endl;
						parent->triggerOverrideFlag(this->child_index,neighbor_index,now,priority_to_set);
					}
					else
						FATAL__("Parent expected");
		END
	}
	
	void			CSurfaceSegment::setReflectionPriority(const CView&view)
	{
		ASSERT__(flags&HasData);
		
		flags &= ~SubdivReflection;
		
		
		//const CView&view = *project_parameters.view;
		
		float center[3],axis[3],normal[3];
		getTranslation(view.coordinates,super->center,this->sector,translation,super->sector_size);
		_add(translation,global_sphere.center.vector,center);
		
		_sub(center,view.aspect.absoluteLocation(),axis);
		_oTriangleNormal(corner[0].v,corner[1].v,corner[2].v,normal);
		_normalize(normal);
		

		
		float	d = distance_square = _dot(axis),
				r = sqr(global_sphere.radius);
		float fc = (float)(depth+1)/(float)super->recursive_depth;
		
		_normalize(axis);

				
		double abs[3];
		_c3(sector,abs);
		_mult(abs,super->sector_size);
		_add(abs,global_sphere.center.vector);
		double h = _length(abs)-super->base_heightf;
		if (h+global_sphere.radius > 0 /*&& h-global_sphere.radius < super->variance/4*/)
		{
			float	effective_distance = d,
					focus_barrier = r*10.0f*64.0f/(1<<super->exponent) *sqr((0.35+0.65*fc)*(view.lod*0.8))*(fc+(1.0f-fc)*vmax(0.0f,-_dot(axis,normal)));
				
			float reflected_center[3];
			_c3(global_sphere.center.vector,reflected_center);
			
			Adaption::setHeight(reflected_center,-h,*this,*super);
			_add(reflected_center,translation);
			if (view.frustrum.isVisible(reflected_center,global_sphere.radius))
			{
				visibility |= Visibility::Reflection;
				if (effective_distance <= focus_barrier && depth > view.min_layer /*&& depth > super->recursive_depth/2*/)
					flags |= SubdivReflection;					
			}
		}
	}
	
	void			CSurfaceSegment::setWaterPriority(const CView&view)
	{
		ASSERT__(flags&HasData);
		
		flags &= ~SubdivWater;
		
		
		//const CView&view = *project_parameters.view;
		
		float center[3],axis[3],normal[3];
		
		double abs[3];
		_c3(sector,abs);
		_mult(abs,super->sector_size);
		_add(abs,global_sphere.center.vector);
		double h = _length(abs);
		_div(abs,h,normal);
		h-=super->base_heightf;
		
		
		
		getTranslation(view.coordinates,super->center,this->sector,translation,super->sector_size);
		
		_add(translation,global_sphere.center.vector,center);
		_sub(center,view.aspect.absoluteLocation(),axis);
		
		

		
		float	d = distance_square = _dot(axis),
				r = sqr(global_sphere.radius);
		float fc = (float)(depth+1)/(float)super->recursive_depth;
		
		_normalize(axis);

				

		if (h-global_sphere.radius < 0)
		{
			float	effective_distance = d,
					focus_barrier = r*10.0f*64.0f/(1<<super->exponent) *sqr((0.35+0.65*fc)*(view.lod*0.9))*(fc+(1.0f-fc)*vmax(0.0f,-_dot(axis,normal)));
		
			float water_center[3];
			_add(this->water_center.v,translation,water_center);

			if (view.frustrum.isVisible(water_center,global_sphere.radius))
			{
				visibility |= Visibility::Water;
				if (effective_distance <= focus_barrier && depth > view.min_layer && h+global_sphere.radius*0.5 > 0)
					flags |= SubdivWater;
			}
		}
	}
	
	void		CSurfaceSegment::project(const CView&view, float ttl, const CTimer::Time&time, bool override_only)
	{
		//LBEGIN

		if (priority > 0 && !depth)
		{
			//lout<< "depth is 0. setting priority 0"<<nl;
			priority = 0;
		}
		
		if ((visibility&Visibility::Primary) && !(flags&AvailableEventTriggered))
		{
			if (onSegmentAvailable)
				onSegmentAvailable(this);
			flags |= AvailableEventTriggered;
		}
		if (!override_only && (visibility&Visibility::Primary) && attachment)
			super->attachment_buffer << this;
			
			
		if (priority > 0)
		{
			if (!(flags&HasChildren))
			{
				CBody::need_children << this;
				//lout << "inserting"<<nl;
			}
			else
				if ((c0->flags&HasData) && (c1->flags&HasData) && (c2->flags&HasData) && (c3->flags&HasData))
				{
					//lout << "focusing on children on level "<<depth<<nl;
					bool override_only_ = override_only || !(visibility&Visibility::Primary);
					c0->setPriority(view,override_only_,time);
					c1->setPriority(view,override_only_,time);
					c2->setPriority(view,override_only_,time);
					c3->setPriority(view,override_only_,time);
					
					c0->project(view,ttl*ttl_loss_per_level,time,override_only_);
					c1->project(view,ttl*ttl_loss_per_level,time,override_only_);
					c2->project(view,ttl*ttl_loss_per_level,time,override_only_);
					c3->project(view,ttl*ttl_loss_per_level,time,override_only_);
						

					referenced = time;
					//END
					return;
				}
				//else
					//lout << "cannot go on. not all children have data"<<nl;
		}
			
		if (visibility&Visibility::Primary)
		{
			update();
			super->face_buffer << this;
			if (depth < super->least_traced_level)
				super->least_traced_level = depth;
		}
		if (visibility != Visibility::Invisible)
			referenced = time;
		if (depth)
			checkTimeouts(time,ttl);

	
		//END
    }
	
	void		CSurfaceSegment::projectReflection(const CView&view)
	{
		//LBEGIN

		if ((flags&SubdivReflection) && (!depth/* || !priority*/))
		{
			//lout<< "depth is 0. setting priority 0"<<nl;
			flags &= ~SubdivReflection;
		}
		
		if (flags&SubdivReflection)
		{
			if (!(flags&HasChildren))
			{
				//don't request water surfaces
				flags &= ~SubdivReflection;
			}
			else
				if ((c0->flags&HasData) && (c1->flags&HasData) && (c2->flags&HasData) && (c3->flags&HasData))
				{
					//lout << "focusing on children on level "<<depth<<nl;
				
					c0->setReflectionPriority(view);
					c1->setReflectionPriority(view);
					c2->setReflectionPriority(view);
					c3->setReflectionPriority(view);
					
					c0->projectReflection(view);
					c1->projectReflection(view);
					c2->projectReflection(view);
					c3->projectReflection(view);

					//referenced = time;
					//END
					return;
				}
				//else
					//lout << "cannot go on. not all children have data"<<nl;
		}
			
		if (visibility&Visibility::Reflection)
		{
			//update();
			super->reflection_buffer << this;
		}
		/*if (visibility != Visibility::Invisible)
			referenced = time;*/
	
		//END
    }
	
	void		CSurfaceSegment::projectWater(const CView&view)
	{
		//LBEGIN

		if ((flags&SubdivWater) && (!depth || !priority))
		{
			//lout<< "depth is 0. setting priority 0"<<nl;
			flags &= ~SubdivWater;
		}
		
		if (flags&SubdivWater)
		{
			if (!(flags&HasChildren))
			{
				//don't request water surfaces
				flags &= ~SubdivWater;
			}
			else
				if ((c0->flags&HasData) && (c1->flags&HasData) && (c2->flags&HasData) && (c3->flags&HasData))
				{
					//lout << "focusing on children on level "<<depth<<nl;
				
					c0->setWaterPriority(view);
					c1->setWaterPriority(view);
					c2->setWaterPriority(view);
					c3->setWaterPriority(view);
					
					c0->projectWater(view);
					c1->projectWater(view);
					c2->projectWater(view);
					c3->projectWater(view);

					//referenced = time;
					//END
					return;
				}
				//else
					//lout << "cannot go on. not all children have data"<<nl;
		}
			
		if (visibility&Visibility::Water)
		{
			//update();
			super->water_buffer << this;
		}
		/*if (visibility != Visibility::Invisible)
			referenced = time;*/
	
		//END
    }
	
	void		CSurfaceSegment::checkTimeouts(const CTimer::Time&time, float ttl)
	{
		float delta = timer.toSecondsf(time-referenced);
		if ((delta > ttl || delta < 0) && !(visibility & Visibility::Override))
		{
			dropChildren();
		}
		else
		{
			if ((flags&HasChildren))
			{
				c0->checkTimeouts(time,ttl*ttl_loss_per_level);
				c1->checkTimeouts(time,ttl*ttl_loss_per_level);
				c2->checkTimeouts(time,ttl*ttl_loss_per_level);
				c3->checkTimeouts(time,ttl*ttl_loss_per_level);
			}
		}
	}
	
	void		CSurfaceSegment::include(CSurfaceSegment*child)
	{
		float delta[3],center[3];
		super->getTranslation(child->sector,this->sector,delta);
		_sub(child->global_sphere.center.vector,delta,center);
		global_sphere.include(center,child->global_sphere.radius);
	}
	
	void		CSurfaceSegment::defaultDefineSphere(CAbstractSphere<float>&sphere)
	{
		_center(corner[0].v,corner[1].v,corner[2].v,sphere.center.vector);
		sphere.radius = sqrt(vmax(vmax(_distanceSquare(corner[0].v,sphere.center.vector),_distanceSquare(corner[1].v,sphere.center.vector)),_distanceSquare(corner[2].v,sphere.center.vector)));
	}
	
	bool		CSurfaceSegment::rayCast(const CSpaceCoordinates&b_, const float d[3], TRayIntersection&intersection)
	{
		intersection.isset = false;
		if (!(flags&HasData))
			return false;
		float b[3];
		_sub(b_.sector,sector,b);
		_mult(b,super->sector_size);
		_add(b,b_.remainder);
		//lout << "inner base ("<<_toString(b_.sector)<<"relative to "<<_toString(sector)<<") is "<<_toString(b)<<nl;
		if (!_oDetectOpticalSphereIntersection(global_sphere.center.vector, global_sphere.radius, b, d))
			return false;
		
		if ((flags&HasChildren))
		{
			bool changed = false;
			TRayIntersection	my_intersection;
			if (c0->rayCast(b_,d,my_intersection) && (!intersection.isset || my_intersection.fc[2] < intersection.fc[2]))
			{
				intersection = my_intersection;
				changed = true;
			}
			if (c1->rayCast(b_,d,my_intersection) && (!intersection.isset || my_intersection.fc[2] < intersection.fc[2]))
			{
				intersection = my_intersection;
				changed = true;
			}
			if (c2->rayCast(b_,d,my_intersection) && (!intersection.isset || my_intersection.fc[2] < intersection.fc[2]))
			{
				intersection = my_intersection;
				changed = true;
			}
			if (c3->rayCast(b_,d,my_intersection) && (!intersection.isset || my_intersection.fc[2] < intersection.fc[2]))
			{
				intersection = my_intersection;
				changed = true;
			}
			//if (changed)
				//lout << "returning true"<<nl;
			return changed;
		}
		if (CUDA::rayCast(*this,b,d,*super->device_triangles,intersection))
		{
			intersection.surface = this;
			return true;
		}
		return false;
	}

	bool		CSurfaceSegment::groundQuery(const CSpaceCoordinates&b_, const float d[3], unsigned min_layer, TGroundInfo&ground)
	{
		if (!(flags&HasData))
			return false;
		float b[3];
		_sub(b_.sector,sector,b);
		_mult(b,super->sector_size);
		_add(b,b_.remainder);
		float b2 = _dot(b);
		if (b2 > sqr(super->base_heightf)*1.5)
		//if (b2 > sqr(global_sphere.radius)*1.5)
		{
			return false;
		}
		

		
		//lout << "inner base ("<<_toString(b_.sector)<<"relative to "<<_toString(sector)<<") is "<<_toString(b)<<nl;
		
		float	d1[3],r[3];
		_subtract(b,global_sphere.center.vector,d1);
		if (_dot(d1) > sqr(super->base_heightf)*1.5)
		//if (b2 > sqr(global_sphere.radius)*1.5)
		{
			return false;
		}
		
		_cross(d,d1,r);
		float dist2 = _dot(r);
		if (dist2 > sqr(global_sphere.radius))
			return false;
			
		float fc[3];
		if (!_oDetTriangleRayIntersection(corner[0].v, corner[1].v, corner[2].v, b, d, fc))
			return false;
		if (fc[0] < -0.01 || fc[1] < -0.01 || fc[0]+fc[1]>1.01)
			return false;

		
					
		
		if ((flags&HasChildren) && depth > min_layer)
		{
			if (c0->groundQuery(b_,d,min_layer,ground))
				return true;
			if (c1->groundQuery(b_,d,min_layer,ground))
				return true;
			if (c2->groundQuery(b_,d,min_layer,ground))
				return true;
			if (c3->groundQuery(b_,d,min_layer,ground))
				return true;
			return false;
		}
		if (depth > 2 || _distanceSquare(global_sphere.center.vector,b) > sqr(global_sphere.radius*2.5))
		{
			ground.surface = this;
			ground.isset = true;
			ground.height_above_ground = fc[2];
			ground.ground_height = 0; //erm...
			_mad(b,d,fc[2],ground.position);
			_oTriangleNormal(corner[0].v,corner[1].v,corner[2].v,ground.normal);
			return true;
		}
		else
			if (CUDA::groundQuery(*this,b,d,*super->device_triangles,ground))
			{
				ground.surface = this;
				return true;
			}
		return false;
	}

	
	void		CSurfaceSegment::updateSphere()
	{
		BEGIN
		defaultDefineSphere(local_sphere);
		
		

		global_sphere = local_sphere;
	
		if ((flags&HasChildren))
		{
			include(c0);
			include(c1);
			include(c2);
			include(c3);
		}
	
	
		if (parent)
			parent->modifySphere(global_sphere);
		END
	}
	
	void		CSurfaceSegment::checkLinkage()
	{
		BEGIN
		if (parent && parent->child[child_index] != this)
			EXCEPTION("parent-orientation broken");
		for (BYTE k = 0; k < 3; k++)
			if (neighbor_link[k].surface)
			{
				if (neighbor_link[k].surface == this)
					EXCEPTION("self referential error");
				if (neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation].surface != this || neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation].orientation != k)
					EXCEPTION("neighbor-link "+IntToStr(k)+" broken");
				if (neighbor_link[k].primary == neighbor_link[k].surface->neighbor_link[neighbor_link[k].orientation].primary)
					EXCEPTION("primary flag identical");
					
				if (parent && ((CSurfaceSegment*)neighbor_link[k].surface)->parent != parent)
				{
					bool found = false;
					for (BYTE i = 0; i < 3; i++)
						if (parent->neighbor_link[i].surface == ((CSurfaceSegment*)neighbor_link[k].surface)->parent)
							found = true;
					if (!found)
						EXCEPTION("parents don't know each other");
				}
				
				
				/*if (!_similar(neighbor_link[k].surface->corner[neighbor_link[k].orientation].position,corner[(k+1)%3].position)
					||
					!_similar(neighbor_link[k].surface->corner[(neighbor_link[k].orientation+1)%3].position,corner[k].position))
					EXCEPTION("corners differ");*/
			}
		if ((flags&HasChildren))
			for (BYTE k = 0; k < 4; k++)
				if (child[k])
					child[k]->checkLinkage();
	
		END
	}


	void		CDataSurface::syncGeneratedBorders(unsigned level)
	{
		CMap*root_map = this->super->full_map;
		unsigned step = 1<<level;
		
		unsigned*rmap[3];
		for (BYTE k = 0; k < 3; k++)
		{
			THostSurfaceLink&n = neighbor_link[k];
			rmap[k] = root_map->border_index[n.orientation];
		}
		
		for (BYTE k = 0; k < 3; k++)
			if (neighbor_link[k].surface && neighbor_link[k].primary)
			{
				for (unsigned i = step; i < root_map->vertex_range; i+=2*step)
					((CDataSurface*)neighbor_link[k].surface)->vertex_pointer[rmap[k][vertex_range-1-i]] = vertex_pointer[root_map->border_index[k][i]];
			}
				
		#if 0
		//logfile << __LINE__<<nl;
		
		for (BYTE k = 0; k < 3; k++)
			if (neighbor_link[k].surface && neighbor_link[k].primary)
			{
				/*neighbor[k].link->primary[neighbor[k].orientation] = false;
				neighbor[k].link->neighbor[neighbor[k].orientation] = link(this,k);*/
				
				for (unsigned i = step; i < map->vertex_range; i+=2*step)
				{
					::TChannelConfig	&vthis = vertex_pointer[map->border_index[k][i]].channel,
										&vthat = ((CDataSurface*)neighbor_link[k].surface)->vertex_pointer[rmap[k][vertex_range-1-i]].channel;
					vthis.c0 = (vthis.c0+vthat.c0)/2;
					vthis.c1 = (vthis.c1+vthat.c1)/2;
					vthis.c2 = (vthis.c2+vthat.c2)/2;
					#if FRACTAL_KERNEL!=MINIMAL_KERNEL
						vthis.c3 = (vthis.c3+vthat.c3)/2;
					#endif
					#if FRACTAL_KERNEL==FULL_KERNEL
						vthis.c4 = (vthis.c4+vthat.c4)/2;
					#endif
					vthis.oceanic = (vthis.oceanic+vthat.oceanic)/2;
					vthis.age = (vthis.age+vthat.age)/2;
					vthis.water = (vthis.water+vthat.water)/2;
					vthat = vthis;
				}
			}
			
		#endif
		//logfile << __LINE__<<nl;
	}
	
	void		CDataSurface::generateLayer(unsigned level)
	{
		BEGIN
		unsigned step = 1<<level;
		
		CMap*root_map = super->full_map;
		
		CRandom	random(inner_seed.toInt());

		unsigned linear;
		int seed;
		for (unsigned y = step; y < vertex_range; y+=2*step)
		{
			{
				linear = root_map->getIndex(0,y);
				TVertex&v = vertex_pointer[linear];
				seed = outer_seed[0].as_int+(((linear*15+3)*(linear*11+2)));				
				Adaption::generate3(vertex_pointer[root_map->getIndex(0,y-step)],vertex_pointer[root_map->getIndex(0,y+step)],
									vertex_pointer[root_map->getIndex(2*step,y+step)],
									v,
									seed,*this,*super);
				Adaption::setHeight(v.position,v.height*super->variance,*this,*super);
			}
			for (unsigned x = 2*step; x <= y; x+=2*step)
			{
				linear = root_map->getIndex(x,y);
				TVertex&v = vertex_pointer[linear];
				seed = inner_seed.as_int+(((linear*15+3)*(linear*11+2)));				
				Adaption::generate4(vertex_pointer[root_map->getIndex(x,y-step)],vertex_pointer[root_map->getIndex(x,y+step)],
									vertex_pointer[root_map->getIndex(x-2*step,y-step)],vertex_pointer[root_map->getIndex(x+2*step,y+step)],
									v,
									seed,*this,*super);
				Adaption::setHeight(v.position,v.height*super->variance,*this,*super);
			}
			
			{
				linear = root_map->getIndex(y,y);
				TVertex&v = vertex_pointer[linear];
				
				seed = outer_seed[2].as_int+(((linear*15+3)*(linear*11+2)));				
				Adaption::generate3(vertex_pointer[root_map->getIndex(y-step,y-step)],vertex_pointer[root_map->getIndex(y+step,y+step)],
									vertex_pointer[root_map->getIndex(y-step,y+step)],
									v,
									seed,*this,*super);
				
				Adaption::setHeight(v.position,v.height*super->variance,*this,*super);
			}
			for (unsigned x = step; x < y; x+=2*step)
			{
				linear = root_map->getIndex(x,y);
				TVertex&v = vertex_pointer[linear];
				seed = inner_seed.as_int+(((linear*15+3)*(linear*11+2)));				
				Adaption::generate4(vertex_pointer[root_map->getIndex(x-step,y-step)],vertex_pointer[root_map->getIndex(x+step,y+step)],
									vertex_pointer[root_map->getIndex(x-step,y+step)],vertex_pointer[root_map->getIndex(x+step,y-step)],
									v,
									seed,*this,*super);
				Adaption::setHeight(v.position,v.height*super->variance,*this,*super);
			}
		}
		for (unsigned y = 2*step; y < vertex_range-1; y+=2*step)
			for (unsigned x = step; x <=y; x+=2*step)
			{
				linear = root_map->getIndex(x,y);
				TVertex&v = vertex_pointer[linear];
				seed = inner_seed.as_int+(((linear*15+3)*(linear*11+2)));				
				Adaption::generate4(vertex_pointer[root_map->getIndex(x-step,y)],vertex_pointer[root_map->getIndex(x+step,y)],
									vertex_pointer[root_map->getIndex(x-step,y-2*step)],vertex_pointer[root_map->getIndex(x+step,y+2*step)],
									v,
									seed,*this,*super);
				Adaption::setHeight(v.position,v.height*super->variance,*this,*super);
			}
									
		for (unsigned x = step; x <=vertex_range-1; x+=2*step)
		{
			linear = root_map->getIndex(x,vertex_range-1);
			TVertex&v = vertex_pointer[linear];
			seed = outer_seed[1].as_int+(((linear*15+3)*(linear*11+2)));				
			Adaption::generate3(vertex_pointer[root_map->getIndex(x-step,vertex_range-1)],vertex_pointer[root_map->getIndex(x+step,vertex_range-1)],
								vertex_pointer[root_map->getIndex(x-step,vertex_range-1-2*step)],
								v,
								seed,*this,*super);
			Adaption::setHeight(v.position,v.height*super->variance,*this,*super);
		}

		flags |= HasData;
		END
    }
		
	void		CDataSurface::generateLayer(unsigned level,void(*updateHeight)(::TVertex&vertex))
	{
		BEGIN
		unsigned step = 1<<level;
		
		CMap*root_map = super->full_map;
		
		CRandom random(inner_seed.toInt());

		unsigned linear;
		int seed;
		for (unsigned y = step; y < vertex_range; y+=2*step)
		{
			{
				linear = root_map->getIndex(0,y);
				TVertex&v = vertex_pointer[linear];
				seed = outer_seed[0].as_int+(((linear*15+3)*(linear*11+2)));				
				Adaption::generate3(vertex_pointer[root_map->getIndex(0,y-step)],vertex_pointer[root_map->getIndex(0,y+step)],
									vertex_pointer[root_map->getIndex(2*step,y+step)],
									v,
									seed,*this,*super);
				updateHeight(v);
			}
			for (unsigned x = 2*step; x <= y; x+=2*step)
			{
				linear = root_map->getIndex(x,y);
				TVertex&v = vertex_pointer[linear];
				seed = inner_seed.as_int+(((linear*15+3)*(linear*11+2)));				
				Adaption::generate4(vertex_pointer[root_map->getIndex(x,y-step)],vertex_pointer[root_map->getIndex(x,y+step)],
									vertex_pointer[root_map->getIndex(x-2*step,y-step)],vertex_pointer[root_map->getIndex(x+2*step,y+step)],
									v,
									seed,*this,*super);
				updateHeight(v);
			}
			
			{
				linear = root_map->getIndex(y,y);
				TVertex&v = vertex_pointer[linear];
				
				seed = outer_seed[2].as_int+(((linear*15+3)*(linear*11+2)));				
				Adaption::generate3(vertex_pointer[root_map->getIndex(y-step,y-step)],vertex_pointer[root_map->getIndex(y+step,y+step)],
									vertex_pointer[root_map->getIndex(y-step,y+step)],
									v,
									seed,*this,*super);
				
				updateHeight(v);
			}
			for (unsigned x = step; x < y; x+=2*step)
			{
				linear = root_map->getIndex(x,y);
				TVertex&v = vertex_pointer[linear];
				seed = inner_seed.as_int+(((linear*15+3)*(linear*11+2)));				
				Adaption::generate4(vertex_pointer[root_map->getIndex(x-step,y-step)],vertex_pointer[root_map->getIndex(x+step,y+step)],
									vertex_pointer[root_map->getIndex(x-step,y+step)],vertex_pointer[root_map->getIndex(x+step,y-step)],
									v,
									seed,*this,*super);
				updateHeight(v);
			}
		}
		for (unsigned y = 2*step; y < vertex_range-1; y+=2*step)
			for (unsigned x = step; x <=y; x+=2*step)
			{
				linear = root_map->getIndex(x,y);
				TVertex&v = vertex_pointer[linear];
				seed = inner_seed.as_int+(((linear*15+3)*(linear*11+2)));				
				Adaption::generate4(vertex_pointer[root_map->getIndex(x-step,y)],vertex_pointer[root_map->getIndex(x+step,y)],
									vertex_pointer[root_map->getIndex(x-step,y-2*step)],vertex_pointer[root_map->getIndex(x+step,y+2*step)],
									v,
									seed,*this,*super);
				updateHeight(v);
			}
									
		for (unsigned x = step; x <=vertex_range-1; x+=2*step)
		{
			linear = root_map->getIndex(x,vertex_range-1);
			TVertex&v = vertex_pointer[linear];
			seed = outer_seed[1].as_int+(((linear*15+3)*(linear*11+2)));				
			Adaption::generate3(vertex_pointer[root_map->getIndex(x-step,vertex_range-1)],vertex_pointer[root_map->getIndex(x+step,vertex_range-1)],
								vertex_pointer[root_map->getIndex(x-step,vertex_range-1-2*step)],
								v,
								seed,*this,*super);
			updateHeight(v);
		}

		flags |= HasData;
		END
    }
	
	

	
	void		CDataSurface::mergeBorderNormals(bool update)
	{
		BEGIN
		CMap*root_map = super->full_map;
		for (BYTE k = 0; k < 3; k++)
			if (neighbor_link[k].surface)
			{
				for (unsigned i = 1; i < vertex_range-1; i++)
				{
					TVertex	&local = vertex_pointer[root_map->border_index[k][i]],
							&remote = ((CDataSurface*)neighbor_link[k].surface)->vertex_pointer[root_map->border_index[neighbor_link[k].orientation][vertex_range-i-1]];
					_add(local.normal,remote.normal);
					_normalize(local.normal);
					_c3(local.normal,remote.normal);
				}
				if (update)
					((CDataSurface*)neighbor_link[k].surface)->flags |= RequiresUpdate;
			}
		END
	}
	



	CBody::CBody(const CString&name, float sector_size_, unsigned exponent_, unsigned recursion_depth,CUDA::CDeviceArray< ::TVertexInfo>*device_info_field_, CUDA::CDeviceArray<unsigned>*device_vertex_index_field_,CUDA::CDeviceArray<unsigned>*device_border_field_, CUDA::CDeviceArray<unsigned>*device_triangles_,CMap*full_map_,CMap*reduced_map_):
								exponent(exponent_),
								min_river_layer(recursive_depth>>2),
								max_river_layer(recursive_depth>>1),
								full_map(full_map_),reduced_map(reduced_map_),
								device_info_field(device_info_field_),
								device_vertex_index_field(device_vertex_index_field_),
								device_border_field(device_border_field_),
								device_triangles(device_triangles_),
								fractal_clouds(false),
								least_traced_level(recursion_depth-1), range(0)
	{
		::TContext::sector_size = sector_size_;
		::TContext::recursive_depth = recursion_depth;
		::TContext::edge_length = full_map->vertex_range;
		
		::TContext::fertility = 1.0f;
		::TContext::temperature = 1.0f;
		::TContext::has_ocean = true;
		
	}
	

	
	void		CBody::clearFaces()
	{
		for (unsigned i = 0; i < faces.count(); i++)
		{
			INVOKING_DISCARD(faces[i]);
			DISCARD(faces[i]);
			INVOKED_DISCARD(faces[i]);
		}
		faces.resize(0);
	}
	
	void		CBody::purgeSegments()
	{
		for (unsigned i = 0; i < faces.count(); i++)
			faces[i]->dropChildren();
	}
	
	bool		CBody::rayCast(const CSpaceCoordinates&b_, const float d[3], TRayIntersection&intersection, bool is_planet_space)
	{
		CSpaceCoordinates	b = b_;
		if (!is_planet_space)
		{
			_sub(b.sector,center.sector);
			_sub(b.remainder,center.remainder);
			b.wrap(sector_size);
		}
		
		intersection.isset = false;
		TRayIntersection my_intersection;
		for (unsigned i = 0; i < faces.count(); i++)
		{
			if (faces[i]->rayCast(b,d,my_intersection) && (!intersection.isset || my_intersection.fc[2] < intersection.fc[2]))
				intersection = my_intersection;
		}
		return intersection.isset;
	}
	
	bool		CBody::groundQuery(const CSpaceCoordinates&b_, unsigned min_layer, TGroundInfo&ground,bool is_planet_space)
	{
		CSpaceCoordinates	b = b_;
		if (!is_planet_space)
		{
			_sub(b.sector,center.sector);
			_sub(b.remainder,center.remainder);
			b.wrap(sector_size);
		}
		float d[3];
		b.convertToAbsolute(d,sector_size);
		_normalize0(d);
		_mult(d,-1);
		for (unsigned i = 0; i < faces.count(); i++)
			if (faces[i]->groundQuery(b,d,min_layer,ground))
				return true;
		return false;
	}
	
	void		CBody::defaultInit(::TChannelConfig&vertex, CRandom&random, const CBody*context)
	{
		vertex.c0 = random.getFloat(0,1);	//general height, riffs
		vertex.c1 = random.getFloat(0,1);	//general height, riffs
		vertex.c2 = random.getFloat(0,1);	//general height
		#if FRACTAL_KERNEL!=MINIMAL_KERNEL
			vertex.c3 = random.getFloat(0,1);	//general height, height offset (1/2)
		#endif
		#if FRACTAL_KERNEL==FULL_KERNEL
			vertex.c4 = random.getFloat(0,1);	//general height, height offset (2/2)
		#endif
		vertex.oceanic = random.getFloat(0,1);	//global scale height offset (low noise)
		#ifdef RIVERS
			vertex.river_depth = 0.0f;
		#endif
		

		vertex.water = random.getFloat(0.0,0.5*context->fertility);	//water density
		vertex.age = Adaption::ageAt(context->recursive_depth);
		
	}
	
	
	void		CBody::rebuildFromGeometry(const CGeometry&geometry, pInit init_function)
	{
		lout << "building shape from geometry"<<nl;
		lout << " vertices="<<geometry.vertex_field.length()<<nl;
		lout << " triangles="<<geometry.index_field.length()/3<<nl;
		
		clearFaces();
		faces.resize(geometry.index_field.length()/3);
		for (unsigned i = 0; i < faces.count(); i++)
			faces[i] = makeFace();
		lout << "applying seed"<<nl;
		//parameter_buffer.resize(faces.count());
		
		CRandom	random(root_seed.toInt());
		//logfile << __LINE__<<nl;
		lout << "creating vertex field"<<nl;
		
		CArray<TVertex>		vertex_field(geometry.vertex_field.length());
		for (unsigned i = 0; i < geometry.vertex_field.length(); i++)
		{
			init_function(vertex_field[i].channel,random,this);
			vertex_field[i].height = Adaption::height(vertex_field[i].channel);
		}
		
		//link neighbors:
		
		CMesh<TFaceGraphDef<TDef<float> > >	graph;
		
		//logfile << __LINE__<<nl;
		//lout << "creating graph"<<nl;
		
		_oMakeGraph(graph,geometry.index_field,CArray<unsigned>());

		//for (unsigned i = 0; i < sector_count; i++)
			//logfile << "local map of "<<i<<" is 0x"<<PointerToHex(faces[i]->root_map)<<nl;
		
		//lout << "assigning seeds"<<nl;
		for (unsigned i = 0; i < faces.count(); i++)
		{
			CSurfaceSegment*f = faces[i];
			f->inner_seed.expand(root_seed,i);
			f->inner_seed.rotate(recursive_depth-1,i);
			for (BYTE k = 0; k < 3; k++)
				f->outer_seed[k].expand(f->inner_seed,k);
		}
		
		
		//lout << "building faces"<<nl;
		//logfile << __LINE__<<nl;
		for (unsigned i = 0; i < faces.count(); i++)
		{
			//logfile << __LINE__<<nl;
			CDataSurface*f = faces[i];
			f->child_index = i;
			f->super = this;
			
			//logfile << __LINE__<<nl;
			for (BYTE k = 0; k < 3; k++)
			{
				//logfile << __LINE__<<nl;
				if (graph.triangle_field[i].n[k])
				{
					f->neighbor_link[k] =
							link(
									faces[(int)(graph.triangle_field[i].n[k].triangle-graph.triangle_field)],
		 							graph.triangle_field[i].n[k].indexOf(graph.triangle_field[i].vertex[(k+1)%3])
								);
					//f->neighbor_link[k].surface->neighbor_link[f->neighbor_link[k].orientation].primary = false;
					f->outer_seed[k] = f->neighbor_link[k].surface->outer_seed[f->neighbor_link[k].orientation];
				}
				else
				{
					f->neighbor_link[k] = THostSurfaceLink();
				}
				//logfile << __LINE__<<nl;
				
				if (geometry.index_field[i*3+k]>=geometry.vertex_field.length())
					EXCEPTION("index exception");
				//logfile << "0x"<<PointerToHex(faces[i]->root_map)<<nl;
				//logfile << i<<"/"<<sector_count<<", "<<k<<nl;
				::TVertex&corner = f->vertex_pointer[full_map->border_index[k][0]];
				const CGeometry::Vertex&vertex = geometry.vertex_field[geometry.index_field[i*3+k]];
				corner = vertex_field[geometry.index_field[i*3+k]];
				_c3(vertex.position,corner.position);
				
				Adaption::setHeight(corner.position,variance*corner.height,*faces[i],*this);
				
				faces[i]->corner[k] = corner.position;

			}
			for (BYTE k = 0; k < 3; k++)
				faces[i]->neighbor_link[k].primary = faces[i]->orderOfEdge(k);
			/*
			float	dx[3],
					dy[3];
			_sub(f->corner[0].v,f->corner[1].v,dy);
			_sub(f->corner[2].v,f->corner[1].v,dx);
			for (unsigned j = 0; j < full_map->vertex_count; j++)
			{
				_mad(f->corner[1].v,dx,(float)full_map->vertex_info[j].x/(float)full_map->vertex_maximum,f->vertex_pointer[j].position);
				_mad(f->vertex_pointer[j].position,dy,1.0f-(float)full_map->vertex_info[j].y/(float)full_map->vertex_maximum);
			}*/
			f->flags |= ::TBaseSurface::HasData;
			
			//logfile << __LINE__<<nl;
			f->reinit();
			//logfile << __LINE__<<nl;
			/*for (BYTE k = 0; k < 3; k++)
			{
				TVertex&corner = faces[i]->vertex_pointer[faces[i]->root_map->border_index[k][0]];
				Adaption::setHeight(corner.uniform_position,faces[i]->uniform_height,*faces[i],sector_size);
				//_c3(faces[i]->getCorner(k).uniform_position,faces[i]->getCorner(k).position);
			}*/
			//logfile << __LINE__<<nl;
		}
		
		//logfile << __LINE__<<nl;
		//lout << "generating layers"<<nl;
		
		for (unsigned j = exponent-1; j < exponent; j--)
		{
			for (unsigned i = 0; i < faces.count(); i++)
				faces[i]->generateLayer(j);
			for (unsigned i = 0; i < faces.count(); i++)
				faces[i]->syncGeneratedBorders(j);
		}
		//logfile << __LINE__<<nl;

		/*lout << "calculating normals"<<nl;
		
		for (unsigned i = 0; i < faces.count(); i++)
			faces[i]->fillAllNormals();*/
		//lout << "finalizing"<<nl;
		//logfile << __LINE__<<nl;
		for (unsigned i = 0; i < faces.count(); i++)
		{
			//faces[i]->mergeBorderNormals();
			faces[i]->flags |= ::TBaseSurface::RequiresUpdate;
			faces[i]->update();
			faces[i]->updateSphere();
			
			faces[i]->flags |= TBaseSurface::Edge0Merged | TBaseSurface::Edge1Merged | TBaseSurface::Edge2Merged;

			//faces[i]->requires_update = true;
		}
		for (unsigned i = 0; i < faces.count(); i++)
			faces[i]->CSurfaceSegment::update();
		//logfile << __LINE__<<nl;
	}
	
	

	void		CBody::reset()
	{
		face_buffer.reset();
		reflection_buffer.reset();
		water_buffer.reset();
		attachment_buffer.reset();
		//cloud_buffer.reset();
		least_traced_level = recursive_depth-1;
	}
	
	/*
	void		CBody::clearChildren(unsigned depth)
	{
		least_traced_level = depth;
		for (unsigned i = 0; i < faces.count(); i++)
        	faces[i]->dropChildren();
	}
	*/
	
	void	CBody::backgroundSubdivide(float min_time_frame, float max_time_frame)
	{
		if (!need_children.count() && !production)
			return;
		//lout << "processing "<<need_children.length()<<" entries"<<nl;
		if (need_children.count())
			CSort<PrioritySort>::quickSortField(need_children.pointer(),need_children.length());
		backbone_process.min_time_frame = min_time_frame;
		backbone_process.max_time_frame = max_time_frame;
		backbone_process.do_finish = false;
		backbone << &backbone_process;

	}
	
	

	void		CBackboneProcess::ThreadMain()
	{
		CTimer::Time begin = timer.now();
		//lout << "subdivide ("<<time_frame<<") step "<<step<<" production 0x"<<production;
		float current = 0;
		do
		{
			//cout << "step"<<endl;
			if (!CBody::processStep())
			{
				processing_time = timer.now()-begin;
				//cout << "abort"<<endl;
				return;
			}
			//cout << "done"<<endl;
			current = timer.toSecondsf(timer.now()-begin);
		}
		while ((!do_finish || current < min_time_frame) && current < max_time_frame);
		processing_time = timer.now()-begin;
		//cout << "return"<<endl;
	}

	bool	CBody::processStep()
	{
		BEGIN
		//DEBUG_POINT
		static unsigned step;
		if (!production)	//surface deleted
			step = 0;
		else
		{
			ASSERT_NOT_NULL__(production->c0);
			ASSERT_NOT_NULL__(production->c1);
			ASSERT_NOT_NULL__(production->c2);
			ASSERT_NOT_NULL__(production->c3);
		}
		
		#if SYSTEM==WINDOWS
			ASSERT__(wglGetCurrentContext() != 0);
		#endif
		
		switch (step)
		{
			case 0:
				//DEBUG_POINT
				if (!need_children.count())
					return false;
				if (CSurfaceSegment::max_load && num_sectors+4 > CSurfaceSegment::max_load)
				{
					need_children.reset();
					return false;
				}
				production = need_children.pop();
				if (production->flags&::TBaseSurface::HasChildren)
				{
					//need_children.reset();
					//this case happens if a segment that just finished constructing has re-requested children during the last frame (which, in fact, is quite likely)
					production = NULL;
					return need_children.count()>0;
				}

				production->createChildren(trace_time);
				production->flags &= ~::TBaseSurface::HasChildren;	//clear children flag to prevent sub division
			break;
			case 1:
				production->c0->build();
			break;
			case 2:
				production->c1->build();
			break;
			case 3:
				production->c2->build();
			break;
			case 4:
				production->c3->build();
			break;
			case 5:
				//DEBUG_POINT
				//ASSERT_NOT_NULL__(production);
				//ASSERT_NOT_NULL__(production->c0);
				//DEBUG_POINT
				production->c0->mergeEdges();
				//DEBUG_POINT
			break;
			case 6:
				//DEBUG_POINT
				//ASSERT_NOT_NULL__(production);
				//ASSERT_NOT_NULL__(production->c1);
				//DEBUG_POINT
				production->c1->mergeEdges();
				//DEBUG_POINT
			break;
			case 7:
				//DEBUG_POINT
				//ASSERT_NOT_NULL__(production);
				//ASSERT_NOT_NULL__(production->c2);
				//DEBUG_POINT
				production->c2->mergeEdges();
				//DEBUG_POINT
			break;
			case 8:
				//DEBUG_POINT
				//ASSERT_NOT_NULL__(production);
				//ASSERT_NOT_NULL__(production->c3);
				//DEBUG_POINT
				production->c3->mergeEdges();
				//DEBUG_POINT
			break;
			case 9:
				production->c0->update();
			break;
			case 10:
				production->c1->update();
			break;
			case 11:
				production->c2->update();
			break;
			case 12:
				production->c3->update();
				production->flags |= ::TBaseSurface::HasChildren;
			break;
		}
		step++;
		if (step > 12)
			step = 0;
			
		//DEBUG_POINT
		END

		return true;

	}
	
	void	CBody::subdivide(float time_frame)
	{
		BEGIN
		if (!need_children.count() && !production)
		{
			END
			return;
		}
		
		//lout << "processing "<<need_children.length()<<" entries"<<nl;
		if (need_children.count())
			CSort<PrioritySort>::quickSortField(need_children.pointer(),need_children.length());

		
		CTimer::Time started = timer.now();
		do
		{
			if (!processStep())
			{
				END
				return;
			}
		}
		while (timer.toSecondsf(timer.now()-started) < time_frame /*&& production*/);
		
		
		
		need_children.reset();
		END
	}

	CTimer::Time		CBody::backgroundNext()
	{
		backbone.waitUntilIdle();
		ASSERT__(!backbone.busy());
		need_children.reset();
		return backbone_process.processing_time;
	}
	
	void		CBody::process(const CTimer::Time&time, float top_level_ttl, const CView&view)
	{
		BEGIN
		trace_time = time;

		reset();
		//cout << "CBody::process(...): projecting"<<endl;
	
		for (unsigned i = 0; i < faces.count(); i++)
		{
			/*faces[i]->project_parameters.time = &trace_time;
			faces[i]->project_parameters.ttl = top_level_ttl;
			faces[i]->project_parameters.view = &view;*/
			faces[i]->setPriority(view, false, time);
			faces[i]->project(view,top_level_ttl,time,false);
			if (view.require_reflection)
			{
				faces[i]->setReflectionPriority(view);
				faces[i]->projectReflection(view);
			}
			faces[i]->setWaterPriority(view);
			faces[i]->projectWater(view);
			
			/*if (view.clouds)
				faces[i]->projectClouds(true);*/
		}
		//cout << "CBody::process(...): projected. resolving"<<endl;
		
		//logfile << "-clearing waste pipe\n";
		CSurfaceSegment*sector;
		
		//logfile << "-writing project map\n";
		/*for (unsigned i = 0; i < face_buffer.fillState(); i++)
			face_buffer[i]->update();*/
		
	
		for (unsigned i = 0; i < faces.count(); i++)
		{
			faces[i]->resolve();
			faces[i]->resolveReflection();
			faces[i]->resolveWater();
		}
		

		//cout << "CBody::process(...): done"<<endl;
		
		
		END
	}
	

	
	float		CBody::currentOverload()
	{
		float rs = 0;
		for (unsigned i = 0; i < need_children.count(); i++)
			rs += need_children[i]->priority;
		return rs;
	}
	
	void		CBody::checkLinkage()
	{
		for (unsigned i = 0; i < faces.count(); i++)
			faces[i]->checkLinkage();
	}
	/*
	CSurfaceSegment*		CBody::detectVisualPathCollision(const double p0[3],const double p1[3],const TSector&sector, float&scalar_out, float point_out[3],float normal_out[3], TSector&sector_out)
	{
		CVisualPathCollisionCallback	callback(p0,p1,sector,scalar_out,point_out,normal_out,sector_out);
		return recursiveLookup(callback);
	}
	CSurfaceSegment*		CBody::detectVisualCollision(const double p0[3],const double p1[3],const TSector&sector, float&scalar_out, float point_out[3],float normal_out[3], TSector&sector_out)
	{
		CVisualCollisionCallback	callback(p0,p1,sector,scalar_out,point_out,normal_out,sector_out);
		return recursiveLookup(callback);
	}
	CSurfaceSegment*		CBody::detectCollision(const double p0[3],const double p1[3],const TSector&sector, float&scalar_out, float point_out[3],float normal_out[3], TSector&sector_out)
	{
		CSurfaceSegment*rs(NULL);
		if (rs==detectVisualCollision(p0,p1,sector,scalar_out,point_out,normal_out,sector_out))
			return rs;
		if (rs==detectVisualPathCollision(p0,p1,sector,scalar_out,point_out,normal_out,sector_out))
			return rs;
		CCollisionCallback	callback(p0,p1,sector,scalar_out,point_out,normal_out,sector_out);
		return recursiveLookup(callback);
	}
	
	CSurfaceSegment*		CBody::recursiveLookup(CCallback&callback, unsigned depth)
	{
		CSurfaceSegment*rs;
		if (last_match)
		{
			if (rs = last_match->recursiveLookup(callback,depth))
			{
				last_match = rs;
				return rs;
			}
			if (last_match->n0.surface && (rs = last_match->n0.surface->recursiveLookup(callback,depth)))
			{
				last_match = rs;
				return rs;
			}
			if (last_match->n1.surface && (rs = last_match->n1.surface->recursiveLookup(callback,depth)))
			{
				last_match = rs;
				return rs;
			}
			if (last_match->n2.surface && (rs = last_match->n2.surface->recursiveLookup(callback,depth)))
			{
				last_match = rs;
				return rs;
			}
		}
		for (unsigned i = 0; i < faces.count(); i++)
			if (rs = faces[i]->recursiveLookup(callback,depth))
			{
				last_match = rs;
				return rs;
			}
		return NULL;
	}
*/

	CAbstractIndexPatterns::CAbstractIndexPatterns(UINT32*const pattern_, UINT32*const pattern_offset_, UINT32*const core_pattern_, UINT32 num_patterns_, UINT32 exponent_):
						pattern(pattern_),pattern_offset(pattern_offset_),core_pattern(core_pattern_),num_patterns(num_patterns_),exponent(exponent_),
						step0((exponent_+1)*(exponent_+1)),step1(exponent_+1)
	{}
	
	unsigned	CAbstractIndexPatterns::getPatternIndexFor(const CSurfaceSegment*seg, const CSurfaceSegment::TNeighborDepthConfig&config)	const
	{
		//logfile << "attempting to get pattern index for 0x"<<PointerToHex(sector)<<nl;
        unsigned	d0 = config.delta0 - seg->depth,
					d1 = config.delta1 - seg->depth,
					d2 = config.delta2 - seg->depth;
		//logfile << d0<<", "<<d1<<", "<<d2<<nl;
        if (d0 > exponent)
            d0 = exponent;
        if (d1 > exponent)
            d1 = exponent;
        if (d2 > exponent)
            d2 = exponent;
		//logfile << "clamped: "<<d0<<", "<<d1<<", "<<d2<<nl;
        return  d0*step0 + d1*step1 + d2;
	}
	
	unsigned	CAbstractIndexPatterns::getPatternIndexFor(const CSurfaceSegment::TNeighborDepthConfig&config,const CSurfaceSegment*seg)	const
	{
		//logfile << "attempting to get pattern index for 0x"<<PointerToHex(sector)<<nl;
        unsigned	d0 = config.delta0 - seg->depth,
					d1 = config.delta1 - seg->depth,
					d2 = config.delta2 - seg->depth;
		//logfile << d0<<", "<<d1<<", "<<d2<<nl;
        if (d0 > exponent)
            d0 = exponent;
        if (d1 > exponent)
            d1 = exponent;
        if (d2 > exponent)
            d2 = exponent;
		//logfile << "clamped: "<<d0<<", "<<d1<<", "<<d2<<nl;
        return  d0*step0 + d1*step1 + d2;
	}
	

	void				makeHaloGeometry(CGeometry&geometry)
	{
		static const unsigned resolution = 20;
	
		geometry.vertex_field.resize(resolution*2);
		geometry.index_field.resize(resolution*6);
		unsigned *f = geometry.index_field;
		#undef t
		#define t(i0,i1,i2)	{(*f++) = i0; (*f++) = i1; (*f++) = i2;}
			for (unsigned i = 0; i < resolution; i++)
			{
				t(i,(i+1)%resolution,i+resolution);
				t((i+1)%resolution,(i+1)%resolution+resolution,i+resolution);
			}
		#undef t
		ASSERT_CONCLUSION(geometry.index_field,f);

		float step = 360.0f/resolution;
		CGeometry::Vertex*v = geometry.vertex_field;
		for (unsigned i = 0; i < resolution; i++)
			_sphereCoords(step*i,-step/2,1,(*v++).position);
		for (unsigned i = 0; i < resolution; i++)
			_sphereCoords(step*i+step/2,step/2,1,(*v++).position);
		
		ASSERT_CONCLUSION(geometry.vertex_field,v);
	}
	
	CGeometry			makeHaloGeometry()
	{
		CGeometry result;
		makeHaloGeometry(result);
		return result;
	}
	
	CGeometry			makeSphereGeometry()
	{
		CGeometry result;
		makeSphereGeometry(result);
		return result;
	}

	
	void	makeSphereGeometry(CGeometry&geometry)
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
		
		geometry.vertex_field.resize(12);
		geometry.index_field.resize(60);
		unsigned *f = geometry.index_field;
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
		ASSERT_CONCLUSION(geometry.index_field,f);

		_sphereCoords(0,90,1,geometry.vertex_field[0].position);
	    
		_sphereCoords(0,30,1,geometry.vertex_field[1].position);
		_sphereCoords(-72,30,1,geometry.vertex_field[2].position);
		_sphereCoords(-144,30,1,geometry.vertex_field[3].position);
		_sphereCoords(-216,30,1,geometry.vertex_field[4].position);
		_sphereCoords(-288,30,1,geometry.vertex_field[5].position);

		_sphereCoords(-36,-30,1,geometry.vertex_field[6].position);
		_sphereCoords(-108,-30,1,geometry.vertex_field[7].position);
		_sphereCoords(-180,-30,1,geometry.vertex_field[8].position);
		_sphereCoords(-252,-30,1,geometry.vertex_field[9].position);
		_sphereCoords(-324,-30,1,geometry.vertex_field[10].position);

		_sphereCoords(0,-90,1,geometry.vertex_field[11].position);
		
	}
	
	CRollSpace::CRollSpace(double modal_range_):modal_range(modal_range_)
	{
		_clear(texture_root);
		_v3(texture_up,0,1,0);
		_v3(texture_x,1,0,0);
		_v3(texture_y,0,0,1);
		__eye4(invert);
	}
	
	void	CRollSpace::roll(const float up[3], const float location[3])
	{
		double up_[3] = {up[0],up[1],up[2]},
				location_[3] = {location[0],location[1],location[2]};
		roll(up_,location_);
	}
	
	void	CRollSpace::roll(const float up[3], const double location[3])
	{
		double up_[3] = {up[0],up[1],up[2]};
		roll(up_,location);
	}

	void	CRollSpace::mod(double&val)	const
	{
		int64_t step = (int64_t)(val/modal_range);
		if (step < 0)
			step--;
		val -= modal_range*step;
	}
	
	void	CRollSpace::roll(const double up[3], const double delta[3])
	{
	    double sys[16],projection[3],temp[3];
		double translation = _dot(delta,up);
		//_mad(texture_root,up,-translation);
		//_add(texture_root,delta);
		
	
	    _add(up,texture_up,projection);
	    _mad(texture_x,projection,-_dot(texture_x,up)/_dot(projection,up));
	    _c3(up,texture_up);
	    _cross(texture_x,up,texture_y);
	    _normalize0(texture_up);
	    _normalize0(texture_x);
	    _normalize0(texture_y);
		
		//texture_root[0] = fmod(texture_root[0],modal_range);
		//texture_root[1] = fmod(texture_root[1],modal_range);
		//texture_root[2] = fmod(texture_root[2],modal_range);

		//mod(texture_root[0]);
		//mod(texture_root[1]);
		//mod(texture_root[2]);

		
	    //shiver me timbers. the seas have got me - yar.
		//project root to plane:
		_mad(texture_root,texture_x,-_dot(texture_x,delta));
		_mad(texture_root,texture_y,-_dot(texture_y,delta));
		_mad(texture_root,texture_up,-_dot(texture_up,texture_root));
		
		//modulate root:
		{
			double	x = _dot(texture_root,texture_x),
					y = _dot(texture_root,texture_y);
			mod(x);
			mod(y);
			_mult(texture_x,x,texture_root);
			_mad(texture_root,texture_y,y);
		}
		/*
		mod(texture_root[0]);
		mod(texture_root[1]);
		mod(texture_root[2]);*/
		
		/*int mod[3];
		_div(texture_root,modal_range,mod);
		_mad(texture_root,mod,-modal_range);*/
		

	    _c3(texture_x,sys);
	    _c3(texture_y,sys+4);
	    _c3(texture_up,sys+8);
	    _c3(texture_root,sys+12);
	    sys[3] = sys[7] = sys[11] = 0;
	    sys[15] = 1;
	    __invertSystem(sys,invert);

	    
	}


	void	CRollSpace::exportTo(float system[16], double range, bool include_translation)	const
	{
	    _div(invert,range,system);
	    _div(invert+4,range,system+4);
	    _div(invert+8,range,system+8);
		if (include_translation)
			_div(invert+12,range,system+12);
		else
			_clear(system+12);
		//_c3(invert+12,system+12);
		//_mult(system+12,-10000);
		/*system[12] = -fmod(invert[12]/range*modal_range,1);
		system[13] = -fmod(invert[13]/range*modal_range,1);
		system[14] = -fmod(invert[14]/range*modal_range,1);*/
		system[3] = system[7] = system[11] = 0;
		system[15] = 1;
	}



	
	CString	getConfiguration()
	{
		return			 	"Fractal kernel " FRACTAL_VERSION "\n"
							"Selective weighting="+CString(SELECTIVE_WEIGHTING)+"\n"
							"Secondary parent weight="+CString(SECONDARY_PARENT_WEIGHT*100)+"%\n"
							"Complexity="
		#if FRACTAL_KERNEL==FULL_KERNEL
							"Full"
		#elif FRACTAL_KERNEL==REDUCED_KERNEL
							"Reduced"
		#else
							"Minimal"
		#endif
							"\nNormal-strategy="
		#if NORMAL_GENERATION==DOUBLE_NORMAL_STORAGE		
							"Double normal storage"
		#elif NORMAL_GENERATION==SIMPLE_NORMAL_MERGE
							"Flagged copy"
		#else
							"Unknown"
		#endif
	
							"\nStrategy="
		#if WEIGHTING==FIXED_WEIGHT
							"Fixed weighting"
		#elif WEIGHTING==HALF_AGE_SENSITIVE
							"Half age sensitive weighting"
		#elif WEIGHTING==BALANCED_AGE_SENSITIVE
							"Balanced age sensitive weighting"
		#elif WEIGHTING==FULL_AGE_SENSITIVE
							"Age sensitive weighting"
		#elif WEIGHTING==SELECTIVE_AGE_SENSITIVE
							"Selective age sensitive weighting"
		#else
							"Unknown"
		#endif
							"\nVertex size="+CString(sizeof(::TVertex))+
							"\nAge base="+CString(AGE_BASE)+
							"\nWater gain down="+CString(WATER_GAIN_DOWN)+
							"\nWater gain up="+CString(WATER_GAIN_UP)+
							"\nWater noise="+CString(WATER_NOISE);
	}
	
	void	backgroundInit(Engine::OpenGL*opengl)
	{
		ASSERT_NOT_NULL__(opengl);
		CBackboneInit::opengl = opengl;
		backbone.build(1,&backbone_init);
	}
	
	
	
	void	(*CSurfaceSegment::onUpdate)(CSurfaceSegment*surface)=NULL;
	void	(*CSurfaceSegment::onSegmentAvailable)(CSurfaceSegment*surface)=NULL;
	unsigned		CSurfaceSegment::max_load=0;
	
}
