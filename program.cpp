//#include <math/polynomial.h>
#include <engine/display.h>
#include <engine/auto_display_configure.h>
#include <engine/aspect.h>
#include <engine/renderer/opengl.h>
#include <gl/gl_generators.h>

#include <math/vclasses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <general/thread_system.h>
#include <string/string_converter.h>
#include <general/random.h>
#include <image/converter/magic.h>
#include <engine/gl/texture_font2.h>
#include <io/config.h>
#include <container/string_list.h>
#include <io/file_system.h>

//#include <geometry/bvh.h>


#include <io/svg.h>
#include "rc/Resource.h"
#include "types.h"
#include "shard.h"
#include "grid.h"
#include "scene.h"
#include "simulation.h"
#include "statistics.h"
#include "metric.h"
//#include <math/perlinNoise.h>
//#include <math/SimplexNoise.h>
#include "experiment.h"


bool			windowFocused = true;
TExperiment		currentSetup;

Random			random;

bool			stopSimulation = false;
/**
The following DWORD causes Nvidia Optimus to select the high performance GPU.
Don't change the name, value, or dllexport-declaration ("C", so it shows up as that name in the export and not some mangled thing)
*/
extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

void SetupScene();
namespace EntityFlags
{
	enum flag_t
	{
		None=0,
		IsFish=1,
		IsFocus=2,
		IsWall=4,
	};

}

Engine::Display<Engine::OpenGL>	display;
Engine::Textout<Engine::GLTextureFont2>	textout;
Buffer0<String>	messageLog;
Sys::SpinLock	messageLogLock;

count_t		connectionSamples=0,connectedSamples=0;

count_t		numResets = 0;	//!< Number of times SetupScene() has been called
Simulation	testSimulation;
bool		simulate = false;

GUID		avatarID;
bool		hasAvatar=false;

bool	blockCenter = false;
bool	blockRight = false;


void LogMessage(const String&msg)
{
	messageLogLock.lock();
	messageLog << msg;
	messageLogLock.unlock();
}

namespace Aspect
{
	#ifdef D3
		float zoom = 0.4f;
	#else
		float zoom = 0.17f;
	#endif

	Engine::OrthographicAspect<>	hud,topDown;
	Engine::AngularCamera<>			scene;
}

struct TInputVector
{
	int	directionChange = 0;
	int	speedChange = 0;
};

Timer::Time applicationStart;

void EvolveV(const TInputVector&);
//
//template<typename T>
//struct DataPoint
//{
//	count_t	numSamples=0;
//	T		accumulated = 0;
//	TFloatRange<T>	range = MaxInvalidRange<T>();
//
//	void	Include(const T&sample)
//	{
//		accumulated += sample;
//		range.Include(sample);
//		numSamples ++;
//	}
//
//	double	GetAverage() const
//	{
//		return double(accumulated)/double(numSamples);
//	}
//};
//
//
//template<typename T>
//class DataRange : public Buffer0<DataPoint<T> >
//{
//typedef Buffer0<DataPoint<T> >	Super;
//public:
//	String name;
//	void	swap(DataRange&other)
//	{
//		Super::swap(other);
//		name.swap(other.name);
//	}
//
//	DataPoint<T>&	Get(index_t at)
//	{
//		while (Super::Count() <= at)
//			Append();
//		return Super::at(at);
//	}
//};

void Evolve()
{
	TInputVector vec;
	vec.directionChange = -(int)Engine::input.pressed[Key::RightArrow];
	if (Engine::input.pressed[Key::LeftArrow])
		vec.directionChange++;
	vec.speedChange = Engine::input.pressed[Key::UpArrow];
	if (Engine::input.pressed[Key::DownArrow])
		vec.speedChange--;
	EvolveV(vec);
}


//Buffer0<DataRange<count_t>,Swap>	dataRanges;
//DataRange<count_t>					errors;

static const count_t NumSamples = 100;
bool	BeginNextRange();
static count_t accumulation = 0;



//class SceneAccumulation
//{
//public:
//
//	struct Shard
//	{
//		Image16					badEntityDensity;
//		InconsistencyCoverage	ic;
//		int2					coords;
//		bool					isSet = false;
//	};
//
//	struct Generation
//	{
//		Array2D<Shard>	shards;
//		index_t			index;
//	};
//
//	struct ShardTraffic
//	{
//		DataSize		sent,
//						confirmed;
//	};
//	struct ShardTrafficData
//	{
//		FixedArray<ShardTraffic,8>	traffic;
//	};
//
//	Array2D<ShardTrafficData>	shardTraffic;
//
//
//	Buffer0<index_t>	splits;
//	count_t				stackCount=0;
//	bool				latexOutput = false;
//	bool				noDensityGraph = false;
//	bool				showTransferredRCSCount = false;
//	float3				faultColor = float3(0.5f,0,0);	//color used for fault bars, and hatched inconsistency texture
//
//	Array<Generation>	generations;
//
//	void	Accumulate(const Simulation&sim, index_t layer)
//	{
//		if (generations.IsEmpty())
//		{
//			if (blockCenter)
//				splits << sim.GetGridWidth()/2-1;
//			if (blockRight)
//				splits << sim.GetGridWidth()*3/4-1;
//			shardTraffic.SetSize(sim.GetGridWidth(),sim.GetGridHeight());
//
//			generations.SetSize(sim.GetTopGeneration() - sim.GetOldestGeneration()+1);
//			foreach (generations,gen)
//			{
//				gen->index = (gen - generations.pointer()) + sim.GetOldestGeneration();
//				gen->shards.SetSize(sim.GetGridWidth(),sim.GetGridHeight());
//				for (index_t x = 0; x <gen->shards.GetWidth(); x++)
//					for (index_t y = 0; y < gen->shards.GetHeight(); y++)
//					{
//						auto&s = gen->shards.Get(x,y);
//						s.coords = int2(x,y);
//						if (sim.GetSDSCoverage(s.coords,gen->index,layer,s.ic))
//						{
//							s.badEntityDensity.SetSize(32,1,2);
//							s.badEntityDensity.SetChannel(0,0);
//							s.badEntityDensity.SetChannel(1,0);
//							s.isSet = true;
//						}
//					}
//			}
//		}
//
//		foreach (generations,gen)
//			foreach (gen->shards,s)
//				if (s->isSet)
//				{
//					sim.AccumulateInconsistentEntities(s->coords, gen->index, layer, s->badEntityDensity);
//				}
//		for (index_t x = 0; x < shardTraffic.GetWidth(); x++)
//			for (index_t y = 0; y < shardTraffic.GetHeight(); y++)
//			{
//				auto&t = shardTraffic.Get(x,y);
//				for (index_t i = 0; i < t.traffic.Count(); i++)
//					sim.AccumulateTraffic(int2(x,y),layer,i,t.traffic[i].sent,t.traffic[i].confirmed);
//			}
//		stackCount++;
//	}
//
//	static constexpr	float hExtra = 0.2f;
//	static constexpr	float hScale = (1.f + hExtra);
//	static constexpr	float LayerHeight = 0.5f;
//	static constexpr	float TextHeight = LayerHeight*0.75f;
//
//	String Latex(String raw) const
//	{
//		if (!latexOutput)
//			return raw;
//		#ifdef _DEBUG
//			return raw;
//		#endif
//		raw.replace('#',"\\#");
//		return "{\\Large "+raw+"}";
//	}
//	String LatexIt(String raw) const
//	{
//		if (!latexOutput)
//			return raw;
//		#ifdef _DEBUG
//			return raw;
//		#endif
//		raw.replace('#',"\\#");
//		return "{\\textit{\\Large "+raw+"}}";
//	}
//
//	void	BuildGeneration(const Generation&gen, index_t firstGeneration, index_t lastGeneration,SVG::Element&node, const PathString&imageFolder)	const
//	{
//	
//		float at = LayerHeight*(lastGeneration - gen.index);
//		SVG::Element ctx = node.CreateGroup().SetFontSize(TextHeight);
//		ctx.Translate(float2(0,at));
//		foreach (splits,split)
//			if (gen.shards.Get(*split,0).isSet && gen.shards.Get(1+*split,0).isSet)
//			{
//				ctx.CreateLine(float2((*split)*hScale+1.f+hExtra/2.f,LayerHeight*0.05f),float2((*split)*hScale+1.f+hExtra/2.f,LayerHeight)).SetStrokeWidth(0.1f).Stroke(faultColor);
//
//			}
//		ctx.CreateText(float2(gen.shards.GetWidth()*hScale,LayerHeight),Latex(gen.index));	
//		foreach (gen.shards,sh)
//		{
//			if (!sh->isSet)
//				continue;
//
//			if (sh->coords.y != 0)	//1d only ... for now
//				continue;
//			
//			float2 offset = sh->coords;
//			offset *= hScale;
//			//float2 p2 = offset+float2(1,0);
//
//			const float BarHeight = LayerHeight * (noDensityGraph ? 0.5f : 0.25f);
//			float base = LayerHeight-BarHeight-0.03f;
//			Buffer0<float2>	coords;
//			//coords << offset+float2(0,base);
//			//for (UINT32 i = 0; i < sh->badEntityDensity.GetWidth(); i++)
//			//{
//			//	float h = float(sh->badEntityDensity.Get(i,0)[1])/20.f/stackCount;
//			//	coords << offset+float2((float(i)+0.5f)/sh->badEntityDensity.GetWidth(),base-h);
//			//}
//			//coords << offset+float2(1,base);
//			//ctx.CreatePolygon(coords.ToRef()).NoFill().Stroke(float4(0,0,0,0.25f)).Fill(float4(0,0,0,0.125f)).NoStroke();
//
//			if (!noDensityGraph)
//			{
//				coords.Clear();
//				coords << offset+float2(0,base);
//				for (UINT32 i = 0; i < sh->badEntityDensity.GetWidth(); i++)
//				{
//					const USHORT*px = sh->badEntityDensity.Get(i,0);
//					float h = float(px[0])/vmax(1,px[1])*(LayerHeight-BarHeight)*0.9f;
//							///20.f/stackCount;
//					coords << offset+float2((float(i))/(sh->badEntityDensity.GetWidth()-1),base-h);
//				}
//				coords << offset+float2(1,base);
//				ctx.CreatePolygon(coords.ToRef()).Fill(float3(0.5f,0,0)).NoStroke();
//			}
//
//			//ctx.CreatePolygon(Rect<>(offset+float2(0,LayerHeight-BarHeight),offset+float2(1,LayerHeight))).NoFill();
//			const index_t w = sh->ic.ic.GetWidth();
//			for (index_t i = 0; i < w; i++)
//			{
//				if (!sh->ic.ic.Get(i,0,0).IsConsistent())
//				{
//					ctx.CreatePolygon(Rect<>(offset.x+float(i)/w,offset.y+LayerHeight ,offset.x+(float(i+1))/w,offset.y+LayerHeight-BarHeight)).FillPattern("inconsistency").NoStroke();
//
//				}
//			}
//			ctx.CreateLine(offset+float2(0,LayerHeight-BarHeight),offset+float2(1,LayerHeight-BarHeight)).NoFill().SetStrokeWidth(0.03f);
//
//			//ctx.CreateLine(offset,p2).SetStroke(float3());
//		}
//	}
//
//
//
//	static const String Round(float v, int digits)
//	{
//		char temp[0x100];
//		sprintf_s(temp,("%."+String(digits)+"f").c_str(),v);
//		StringRef ref(temp);
//		if (digits > 0)
//		{
//			while (ref.GetLastChar()=='0')
//				ref.DropLastChar();
//			if (ref.GetLastChar()=='.')
//				ref.DropLastChar();
//		}
//		return ref;
//	}
//
//	void	BuildSVG(SVG::Element&node, const PathString&imageFolder) const
//	{
//		if (generations.IsEmpty())
//			return;
//		foreach (generations,gen)
//			BuildGeneration(*gen,generations.First().index,generations.Last().index,node,imageFolder);
//	}
//
//	static constexpr float TextRow0 = -TextHeight*2;
//	static constexpr float TextRow1 = -TextHeight;
//	static constexpr float TextRow2 = 0;
//
//
//	void	Export(String baseName) const
//	{
//		//PathString imageFolder = PathString(".\\");
//		//FileSystem::Folder f(imageFolder);
//		//if (f.IsValidLocation())
//		//	FileSystem::RemoveFolderContents(f.GetLocation());
//		//else
//		//	ASSERT__(FileSystem::CreateDirectory(imageFolder));
//		SVG::Document doc;
//		{
//			constexpr UINT32 Size = 32;
//			constexpr float s = (float)Size / 100.f;
//			//if (!FileSystem::IsFile("pattern.png"))
//			{
//				Image image(Size,Size,4);
//				image.Fill(faultColor.r*255,faultColor.g*255,faultColor.b*255);
//				foreach (image.Horizontal(),x)
//					foreach (image.Vertical(),y)
//					{
//						BYTE&px = *(image.Get(x,y)+3);
//						UINT32 at = (Size+*x - *y)%16;
//						if (at < 7)
//							px = 0;
//						elif (at < 9)
//							px = (at - 6)*255/3;
//						elif (at < 14)
//							px = 255;
//						else
//							px = (16 - at)*255/3;
//						px = 255-px;
//					}
//				png.SaveToFile(image,"pattern.png");
//			}
//			SVG::Element pattern=doc.CreatePattern("inconsistency",Rect<>(0.f,0.f,s,s));
//			//pattern.Rotate(45.f,float2(s/2.f));
//			float h = 0.35f;
//			float d = 0.005f;
//			pattern.CreateImage(Rect<>(0.f,0.f,s,s),"pattern.png");
//			//pattern.CreatePolygon(Rect<>(0-d,0-d,s+d,s*(h))).Fill(float3(0,0,0));
//			//pattern.CreatePolygon(Rect<>(0-d,(1.f-h)*s,s+d,s+d)).Fill(float3(0,0,0));
//		}
//		
//		SVG::Element root = doc.CreateGroup().SetStrokeWidth(0.0125f).Stroke(float3());
//		{
//			if (showTransferredRCSCount)
//				root.CreateText(float2(-hScale/2,TextRow2),LatexIt("#RCS")).SetFontSize(TextHeight);
//
//
//			SVG::Element numbers = root.CreateGroup().SetTextAnchor("middle").SetFontSize(TextHeight);
//			foreach(generations.First().shards,s)
//				if (s->coords.y==0)
//				{
//					numbers.CreateText(float2(hScale*(0.5f+s->coords.x),TextRow1),Latex("S"+String(s->coords.x)));
//				}
//		}
//		BuildSVG(root,"");
//		if (showTransferredRCSCount)
//		{
//
//			SVG::Element numbers = root.CreateGroup().SetTextAnchor("middle").SetFontSize(TextHeight);
//			for (index_t x = 0; x < shardTraffic.GetWidth()-1; x++)
//			{
//				DataSize traffic = shardTraffic.Get(x,0).traffic[6].confirmed + shardTraffic.Get(x+1,0).traffic[1].confirmed;
//				numbers.CreateText(float2(hScale*(x)+hScale-hExtra/2,TextRow2),LatexIt(String(round(float(traffic.numIC)/stackCount))));
//			}
//		}
//
//		root.Scale(float2(50));
//		//root.CreateCircle(float2(1),1).FillPattern("inconsistency");
//		Rect<> bounds = doc.GetBoundingBox();
//		root.Translate(-bounds.min());
//		doc.DefineCustomDocumentSize(bounds.GetExtent());
//		doc.SaveToFile(PathString(baseName+".svg"));
//	}
//
//};
//
//FixedArray<SceneAccumulation,4>	snapshot;

#if 0
	//generate svg evolution graphic:
	static index_t simAt = 0;

	void	RunIteration()
	{

		if (simAt != InvalidIndex)
		{
			constexpr count_t	Fault0 = 18,
								Recovery = 9,
								Fault1Capture0 = 3,
								Fault1Capture1 = 2;

			constexpr index_t	Stage0 = Fault0,	//initial fault ends
								Stage1 = Stage0+Recovery,	//secondary fault begins
								Stage2 = Stage1+Fault1Capture0,	//secondary fault continues
								Stage3 = Stage2+Fault1Capture1;	//simulation ends

			simAt++;
			if (simAt < Stage0)
			{
				blockCenter = true;
				blockRight = false;
				Evolve();
			}
			elif (simAt < Stage1)
			{
				if (simAt == Stage0)
				{
					snapshot[0].Accumulate(testSimulation,0);
				}
				blockCenter = false;
				blockRight= false;
				Evolve();
			}
			elif (simAt < Stage2)
			{
				if (simAt == Stage1)
					snapshot[1].Accumulate(testSimulation,0);
				blockCenter = false;
				blockRight = true;
				Evolve();
			}
			elif (simAt < Stage3)
			{
				if (simAt == Stage2)
					snapshot[2].Accumulate(testSimulation,0);
				blockCenter = false;
				blockRight = true;
				Evolve();
			}
			else
			//{
			//	blockCenter = false;
			//	blockRight = false;
			//	Evolve();
			//}
			{
				snapshot[3].Accumulate(testSimulation,0);
				simAt = 0;
				accumulation++;

				#ifdef _DEBUG
					constexpr count_t End = 2;
				#else
					constexpr count_t End = 200;
				#endif

				if (accumulation == End)
				{
					snapshot[0].Export("snapshot0");
					snapshot[1].Export("snapshot1");
					snapshot[2].Export("snapshot2");
					snapshot[3].Export("snapshot3");
					simAt = InvalidIndex;
					application_shutting_down = true;
				}
				SetupScene();
			}
		}
	
	}
#endif /*0*/


bool	Loop()
{
	#if 0
		for (index_t i = 0; i < 10; i++)
			RunIteration();
	#endif


	if (simulate)
	{
		Evolve();
		//dataRanges.Last().Get(testSimulation.CountIterations()-1).Include(testSimulation.CountInconsistentSDS());
		//errors.Get(testSimulation.CountIterations()-1).Include(testSimulation.CountUnconfirmedTopLevelUpdates()/2);
		//if (testSimulation.CountIterations() >= NumSamples)
		//{
		//	if (!BeginNextRange())
		//	{
		//		simulate = false;
		//		StringFile file;
		//		file.Create("values.txt");
		//		file << "Errors (min)"<<tab << "Errors (max)";
		//		foreach (dataRanges,r)
		//			file << tab << r->name;
		//		file << nl;
		//		for (index_t i = 0; i < NumSamples; i++)
		//		{
		//			file << errors[i].range.min << tab << errors[i].range.max;
		//			foreach (dataRanges,r)
		//				file << tab << r->at(i).GetAverage();
		//			file << nl;
		//		}
		//	}
		//}
		if (!windowFocused)
		{
			for (index_t i = 0; i < 10; i++)
			{
				Sleep(10);
				Evolve();
			}
			return true;
		}
	}
	if (!testSimulation.IsAsync())
	{
		#ifdef _DEBUG
			testSimulation.RebuildIfOudatedFor(1.f);
		#else
			testSimulation.RebuildIfOudatedFor(0.05f);
		#endif
	}

	glEnable(GL_BLEND);
	display.setDefaultBlendFunc();

	if (Engine::mouse.buttons.pressed)
	{
		float2 delta = (Engine::mouse.location.windowRelative - Engine::mouse.previous_location.windowRelative) *300;
		Aspect::scene.AlterAngles(-delta.y,0,-delta.x);


	}


	display.Pick(Aspect::scene);
	#ifndef D3
		Scene::Render(Aspect::scene);
	#else
		const float sceneExt = TEntityCoords(testSimulation.GetGridSize()).Length();
		float base = vmax(0.f,Vec::length(Aspect::scene.GetAbsoluteLocation()) - sceneExt);
		display.setFog(Engine::Fog(base,base+sceneExt*1.5f, float3()));
		display.flareBlendFunc();
	
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 1);
		glEnable(GL_STENCIL_TEST);
		Scene3D::Render();
		display.setFog(nullptr);

	#endif

	display.Pick(Aspect::hud);
		#ifdef D3
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			glStencilFunc(GL_NOTEQUAL, 1, 1);
			glDisable(GL_BLEND);
			glBegin(GL_QUADS);
				glColor4f(1, 1, 1, 0);
				glVertex2f(0, 0);
				glVertex2f(1, 0);
				glVertex2f(1, 1);
				glVertex2f(0, 1);
			glEnd();
			glEnable(GL_BLEND);
			display.defaultBlendFunc();
			glDisable(GL_STENCIL_TEST);
		#endif

		textout.PushState();
		messageLogLock.lock();
		{
			textout.MoveTo(0.f,0.f + textout.GetScaledHeight() * (10 + messageLog.Count()));
			foreach (messageLog,msg)
				textout << *msg << nl;
		}
		messageLogLock.unlock();
		textout 
			<< testSimulation.GetTopGeneration()
			<< (testSimulation.IsAsync() ? " (working)":"")
			<< nl 
			<< "DD: "<< (testSimulation.GetTopGeneration() - testSimulation.GetOldestGeneration()) << nl
			<< "Recovery: "<< (testSimulation.LastRecoveryOps())<<"/"<< testSimulation.LastRecoverCompletedSDS() << nl
			//<< "Scene# "<< numResets << nl
			<< "Reliability "<<currentSetup.GetReliability()<<nl
			<< "Recovery Ops "<<currentSetup.recoveryIterations << nl
			<< "Recoverying or dead nodes: "<<testSimulation.CountNonFullyAvailableNodes() << nl
			<< "Layers "<<currentSetup.numLayers << nl
			<< "SyncOps "<<currentSetup.maxSiblingSyncOperations << nl
			<< "SyncDelay "<<currentSetup.siblingSyncDelay << nl
			<< "Memory allocated (MB): "<<	System::getMemoryConsumption()/1024/1024<<"/"<<(System::getPhysicalMemory()>>20)<<nl

			#if 0
			<< /*testSimulation.CountCurrentEntities()*/accumulation<<"/"<< simAt << nl 
			#endif /*0*/
			//<< (connectionSamples?double(connectedSamples) / connectionSamples : 0) << " ("<<connectionSamples<<")"<<nl
			<< 1.f / Engine::timing.delta << nl;
		textout.PopState();


		display.useTexture(nullptr);
		Rect<> rect(0.7f,0.05f,0.95f,0.3f);
		glColor4f(0,0,0,1);
		glBegin(GL_LINE_STRIP);
			glVertex2f(rect.x.max,rect.y.min);
			glVertex2f(rect.x.max,rect.y.max);
			glVertex2f(rect.x.min,rect.y.max);
			glVertex2f(rect.x.min,rect.y.min);

			for (index_t i = 0; i < 256; i++)
			{
				float relative = Statistics::GetMean(IC::TSample(1,(IC::content_t)i));
				float x,y;
				rect.Derelativate(float(i)/255,relative,x,y);
				glVertex2f(x,y);
			}
		glEnd();


	return !application_shutting_down;
}


void	onWindowResize(const Resolution&res, UINT32)
{
	
	textout.SetScale(1.f / display.GetClientWidth(), 1.f / display.GetClientHeight());

	Aspect::topDown.UpdateProjection(res.GetAspect(),Aspect::zoom,-1,1);
	Aspect::scene.UpdateProjection(res.GetAspect(),0.1,1000);

}

namespace Logic
{


	void RandomMotion(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&, MessageDispatcher&)
	{
		float r = random.NextFloat(0.f,Entity::MaxMotionDistance);
		Metric::Direction(random).ToVector(inOutShape.velocity);
		inOutShape.velocity *= r;
	}

	void AggressiveRandomMotion(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&, const MessageReceiver&, MessageDispatcher&)
	{
		CRC32::Sequence seq;
		foreach (e.neighborhood,n)
		{
			seq.AppendPOD(n->guid);
			seq.AppendPOD(n->coordinates);
		}
		seq.AppendPOD(e.guid);
		seq.AppendPOD(e.coordinates);
		if (serialState.GetLength()	== 4)
		{
			seq.AppendPOD(*(const UINT32*)serialState.GetPointer());
		}
		Random random(seq.Finish());

		float r = Entity::MaxMotionDistance;
	//	random.getFloat(0.f,Entity::MaxOperationalRadius);
		TEntityCoords motion;
		Metric::Direction(random).ToVector(motion);
		motion *= r;

		serialState.SetSize(4);
		(*(UINT32*)serialState.GetPointer()) = random.NextSeed();

		inOutShape.velocity += motion;
	}

	void AggressiveRandomMotionX(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape, Random&, const MessageReceiver&, MessageDispatcher&)
	{
		CRC32::Sequence seq;
		foreach(e.neighborhood, n)
		{
			seq.AppendPOD(n->guid);
			seq.AppendPOD(n->coordinates);
		}
		seq.AppendPOD(e.guid);
		seq.AppendPOD(e.coordinates);
		if (serialState.GetLength() == 4)
		{
			seq.AppendPOD(*(const UINT32*)serialState.GetPointer());
		}
		Random random(seq.Finish());

		float r = Entity::MaxMotionDistance;
		//	random.getFloat(0.f,Entity::MaxOperationalRadius);
		TEntityCoords motion;
		motion.x = random.NextBool() ? -r : r;
		//Metric::Direction(random).ToVector(motion);
		//motion *= r;

		serialState.SetSize(4);
		(*(UINT32*)serialState.GetPointer()) = random.NextSeed();

		inOutShape.velocity += motion;
	}

	void MinDistance(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&, MessageDispatcher&)
	{
		TEntityCoords delta;
		foreach (e.neighborhood,n)
		{
			TEntityCoords dn = e.coordinates - n->coordinates;
			Metric::Distance d2(dn);
			static const float minDist = 0.1f;
			if (d2 < minDist)
			{
				if (nearingZero(*d2))
				{
					float moveBy = (minDist) / 2;
					TEntityCoords pdelta;
					Metric::Direction(random).ToVector(pdelta);
					pdelta *= moveBy;
					delta += pdelta;
				}
				else
				{
					float d = *d2;
					float moveBy = (minDist - d) / 2;
					delta += dn / d * moveBy;
				}
			}
		}
		inOutShape.velocity +=  delta;
	}

	void HorizontalMinDistance(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape, Random&random, const MessageReceiver&, MessageDispatcher&)
	{
		TEntityCoords delta;
		foreach(e.neighborhood, n)
		{
			TEntityCoords dn = e.coordinates - n->coordinates;
			Metric::Distance d2(dn);
			static const float minDist = 0.1f;
			if (d2 < minDist)
			{
				if (nearingZero(*d2))
				{
					float moveBy = (minDist) / 2;
					float pdelta = random.NextBool() ? -moveBy : moveBy;
					delta.x += pdelta;
				}
				else
				{
					float d = *d2;
					float moveBy = (minDist - d) / 2;
					delta.x += dn.x / d * moveBy;
				}
			}
		}
		inOutShape.velocity += delta;
	}

	namespace Flock
	{
		namespace Helper
		{
			float AngularDistance (const Metric::Direction2D& a0, const Metric::Direction2D& a1)
			{
				return a0.MinimalDifferenceTo(a1);
			}

			float AngularDistance (const Metric::Direction3D& a0, const Metric::Direction3D& a1)
			{
				TVec3<> v0,v1;
				a0.ToVector(v0);
				a1.ToVector(v1);
				return Vec::quadraticDistance(v0,v1);
			}
		}
		static const float MinSpeed = Entity::MaxMotionDistance * 0.2f * 0.05f;
		static const float MaxSpeed = Entity::MaxMotionDistance * 0.4f * 0.05f;

		void MaintainVelocity(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&, MessageDispatcher&)
		{
			inOutShape.velocity = e.velocity;
			float speed = e.velocity.Length();
			float wantSpeed = random.NextFloat(MinSpeed,MaxSpeed);
			TEntityCoords wantDirection;
			e.GetDirection(wantDirection);
			wantDirection *= wantSpeed;
			Vec::interpolate(inOutShape.velocity,wantDirection,0.5f,inOutShape.velocity);
		}

		void Focus(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&recv, MessageDispatcher&disp)
		{
			struct TFocusLocation
			{
				TEntityCoords loc,velocity;
				count_t gen;
			};
			if (serialState.size() != sizeof(TFocusLocation))
			{
				serialState.SetSize(sizeof(TFocusLocation));
				TFocusLocation init = {TEntityCoords(),TEntityCoords(),0};
				(*(TFocusLocation*)serialState.pointer()) = init;
			}

			TFocusLocation&best = (*(TFocusLocation*)serialState.pointer());

			foreach (e.neighborhood,n)
				if (n->userFlags & EntityFlags::IsFocus)
				{
					best.loc = n->coordinates;
					best.velocity = n->velocity;
					best.gen = generation;
					break;
				}
			if (best.gen != generation)
			{
				foreach (recv,msg)
				{
					if (msg->senderProcess == &Focus && msg->data.size() == sizeof(TFocusLocation))
					{
						const TFocusLocation&f = *(const TFocusLocation*)msg->data.pointer();
						if (f.gen > best.gen)
						{
							best = f;
						}
					}
					else
					{
						bool brk = true;
					}
				}
			}

			if (best.gen > 0)
			{
				if (e.neighborhood.IsNotEmpty())
				{
					index_t a = random.NextIndex(e.neighborhood.Count()-1);
					{
						MessageData dat(sizeof(TFocusLocation));
						memcpy(dat.pointer(),&best,sizeof(best));
						disp.DispatchFleeting(e.neighborhood[a],&Focus,std::move(dat));
					}
					//if (e.neighborhood.Count() > 1)
					//{
					//	index_t b = a;
					//	while (b == a)
					//		b = random.getIndex(e.neighborhood.Count()-1);
					//	{
					//		MessageData dat(sizeof(TFocusLocation));
					//		memcpy(dat.pointer(),&best,sizeof(best));
					//		disp.DispatchFleeting(e.neighborhood[b],&Focus,std::move(dat));
					//	}
					//}
					

				}
	/*			foreach (e.neighborhood,n)
				{
					MessageData dat(sizeof(TFocusLocation));
					memcpy(dat.pointer(),&best,sizeof(best));
					disp.DispatchFleeting(*n,&Focus,std::move(dat));
				}
*/
				TEntityCoords delta = best.loc + best.velocity * (generation - best.gen) - e.coordinates;
				float pull = *Metric::Distance(delta);
				float finalPull = vmin(pull,MinSpeed/10.f);
				delta *= finalPull / pull;
				inOutShape.velocity += delta;
				inOutShape.orientation = Metric::Direction(inOutShape.velocity);
			}
		}


		void AvoidCollision(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape, Random&random, const MessageReceiver&, MessageDispatcher&)
		{
			TEntityCoords motion = inOutShape.velocity;
			const float orginalSpeed = Vec::length(motion);
			float desiredSpeed = orginalSpeed;
			const float lookahead = 20.f;
			if (orginalSpeed > 0)
			{
//				NeighborhoodCollider collider( e.neighborhood);
				NeighborhoodCollider wallCollider( e.neighborhood,[](const EntityAppearance&app){return (app.userFlags & EntityFlags::IsWall) != 0;} );
				if (wallCollider.TestEdge(e.coordinates,e.coordinates+motion*lookahead,true))
				{
					float bestDistance = 0;
					float bestAngleDifference = std::numeric_limits<float>::max();
					Metric::Direction bestAngle;
					bool any = false;
					NeighborhoodCollider wallCollider( e.neighborhood,[](const EntityAppearance&app){return (app.userFlags & EntityFlags::IsWall) != 0;} );

					for (int i = 0; i < 128; i++)
					{
						Metric::Direction a (random);
						TEntityCoords dir;
						a.ToVector(dir);
						float d;
						//if (wallCollider.TestEdge(e.coordinates,e.coordinates+dir*orginalSpeed*2.f,false))
						//	continue;
						wallCollider.TestEdge(e.coordinates,e.coordinates+dir*orginalSpeed*lookahead,true,&d);

						float angleDifference = Helper::AngularDistance(a,e.orientation);
						if (d > bestDistance || (d == bestDistance && angleDifference < bestAngleDifference))
						{
							bestDistance = d;
							bestAngleDifference = angleDifference;
							any = true;
							bestAngle = a;
						}
						else
						{
							if (!any)
							bool b= true;
						}
					}
					ASSERT__ (any);

					if (bestDistance*0.5f < desiredSpeed)
					{
						desiredSpeed = bestDistance *0.5f;
					}
					inOutShape.orientation = bestAngle;
					TEntityCoords newDirection;
					inOutShape.orientation.ToVector(newDirection);
					motion = desiredSpeed * newDirection;
					
				}
			}
			inOutShape.velocity = motion;
		}

		void Align(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&, MessageDispatcher&)
		{

			constexpr float DistanceCutOff = 0.05f;
			constexpr float DistanceCutOff2 = DistanceCutOff*DistanceCutOff;

			float load = 2;
			TEntityCoords speed = inOutShape.velocity * load;

			foreach (e.neighborhood,n)
			{
				{
					speed += n->velocity;
					load++;
				}
			}
			inOutShape.velocity = speed / load;
			inOutShape.orientation = Metric::Direction(inOutShape.velocity);
		}

		constexpr float DistanceCutOff = 0.025f;
		constexpr float DistanceCutOff2 = DistanceCutOff*DistanceCutOff;

		void Center(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&, MessageDispatcher&)
		{
			if (e.neighborhood.IsEmpty())
				return;
			TEntityCoords motion = inOutShape.velocity;
			const float orginalSpeed = Vec::length(motion);
			float desiredSpeed = orginalSpeed;

			TEntityCoords center = e.coordinates;
			count_t cnt = 1;
			foreach (e.neighborhood,n)
			{
				if (n->shape != Entity::Shape::None)
					continue;
				float w = Vec::intensity(n->coordinates - e.coordinates,e.velocity)*0.5f + 0.5f;
				if (w > 0.35f)
				{
					center += n->coordinates;// + n->velocity;
					cnt ++;
				}
			}
			if (!cnt)
				return;
			center /= cnt;
			TEntityCoords wantOutTo = center - e.coordinates;
			float total = wantOutTo.Length();
			//if (total < DistanceCutOff*0.5f)
				//return;
			static const float Max = MaxSpeed/10.f;
			if (total > Max)
				wantOutTo *= Max / total;
			inOutShape.velocity += wantOutTo;
			inOutShape.orientation = Metric::Direction(inOutShape.velocity);
		}

		void MaintainMinimumDistance(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&, MessageDispatcher&)
		{
			TEntityCoords motion = inOutShape.velocity;
			const float orginalSpeed = Vec::length(motion);
			float desiredSpeed = orginalSpeed;

			static const constexpr Metric::Distance CutOff = Metric::Distance(DistanceCutOff);
			TEntityCoords wantOutTo;

			foreach (e.neighborhood,n)
			{
				float w = Vec::intensity(n->coordinates - e.coordinates,e.velocity)*0.5f + 0.5f;
				if (w > 0.35f)
				{

					//float d2 = Vec::quadraticDistance(e.coordinates,n->coordinates);
					//if (d2 < DistanceCutOff2)
					Metric::Distance	d(e.coordinates,n->coordinates);
					if (d < CutOff)
					{
						float dist =*d;
						float wantOut =  (CutOff.Squared() / d.Squared()  - 1.f);
						TEntityCoords to = wantOut * (e.coordinates - n->coordinates) / dist;
						wantOutTo += to;
					}
				}
			}
			Metric::Distance total(wantOutTo);
			static const float Max = MaxSpeed/5.f;
			if (total > Max)
				wantOutTo *= Max / *total;
			inOutShape.velocity += wantOutTo;
			inOutShape.orientation = Metric::Direction(inOutShape.velocity);
		}
	}

	void UserAvatar(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape,Random&random, const MessageReceiver&recv, MessageDispatcher&)
	{
		if (serialState.size() != sizeof(float))
		{
			serialState.SetSize(sizeof(float));
			(*(float*)serialState.pointer()) = 0;
		}
		float&speed = (*(float*)serialState.pointer());

		float speedGain = 0;
		Metric::Direction::Angle dirGain = Metric::Direction::Angle(0);
		static const float GainPerIteration = Flock::MaxSpeed/10.f;

		foreach (recv,msg)
			if (!msg->senderProcess && msg->data.size() == sizeof(TInputVector))
			{
				const TInputVector	&vec = (const TInputVector	&)*(msg->data.pointer());
				speedGain += GainPerIteration * vec.speedChange;
				dirGain += Metric::Direction::MaxValue/40.f * vec.directionChange;
			}


		speed = clamp(speed + speedGain,0,Entity::MaxMotionDistance);
		inOutShape.orientation += dirGain;
		inOutShape.orientation.ToVector(inOutShape.velocity);
		inOutShape.velocity *= speed;

		#ifdef D3
			Aspect::scene.location = e.coordinates+inOutShape.velocity;
		#else
			Aspect::scene.location = float3(e.coordinates+inOutShape.velocity,0.f);
		#endif
		Aspect::scene.UpdateView();
	}

	static const float WaveSpacing = Entity::MaxAdvertisementRadius * 0.9f;
	static const float WavePlaneHeight = 20.f;
	constexpr float g = 9.806f;
	constexpr float EPSILON = std::numeric_limits<float>::epsilon();

	float3 HorizontalPotential(float3 gp)
	{
		float h = vmax(gp.x, 0.0f);
		float uh = gp.y;
		float vh = gp.z;

		float h4 = h * h * h * h;
		float u = sqrtf(2.0f) * h * uh / (sqrtf(h4 + vmax(h4, EPSILON)));

		float3 F;
		F.x = u * h;
		F.y = uh * u + g * h * h;
		F.z = vh * u;
		return F;
	}

	float3 VerticalPotential(float3 gp)
	{
		float h = vmax(gp.x, 0.0f);
		float uh = gp.y;
		float vh = gp.z;

		float h4 = h * h * h * h;
		float v = sqrtf(2.0f) * h * vh / (sqrtf(h4 + vmax(h4, EPSILON)));

		float3 G;
		G.x = v * h;
		G.y = uh * v;
		G.z = vh * v + g * h * h;
		return G;
	}

	float3 SlopeForce(float3 c, float3 n, float3 e, float3 s, float3 w)
	{
		float h = vmax(c.x, 0.0f);

		float3 H;
		H.x = 0.0f;
		H.y = 0;//-g * h * (e.w - w.w);
		H.z = 0;//-g * h * (s.w - n.w);
		return H;
	}


	
	void Wave(Array<BYTE>&serialState, count_t generation, const Entity&e, EntityShape&inOutShape, Random&random, const MessageReceiver&recv, MessageDispatcher&send)
	{
		if (serialState.size() != sizeof(float3))
		{
			serialState.SetSize(sizeof(float3));
			(*(float3*)serialState.pointer()) = float3(WavePlaneHeight,0,0);
		}

		float3		&mine = (*(float3*)serialState.pointer());
		float3		left,right,top,bottom;
		bool		haveLeft=false,haveRight=false,haveTop=false,haveBottom=false;

		foreach (recv,msg)
		{
			if (msg->senderProcess != &Wave)
				continue;
			ASSERT_EQUAL__(msg->data.size(),sizeof(float3));
			const float3&src = *(const float3*)msg->data.pointer();
			if (msg->sender.coordinates.y == e.coordinates.y)
			{
				//horizontal
				if (msg->sender.coordinates.x < e.coordinates.x)
				{
					haveLeft = true;
					left = src;
				}
				else
				{
					haveRight = true;
					right = src;
				}
			}
			else
			{
				//vertical
				if (msg->sender.coordinates.y < e.coordinates.y)
				{
					haveBottom = true;
					bottom = src;
				}
				else
				{
					haveTop = true;
					top = src;
				}
			}
		}
		if (!haveTop)
			top = mine;
		if (!haveBottom)
			bottom = mine;
		if (!haveRight)
			right = mine;
		if (!haveLeft)
			left = mine;


		constexpr float dt = 0.1f;
		static const float dx = WaveSpacing*100.f;
		static const float dy = WaveSpacing*100.f;







        //int gridx = x + 1;
        //int gridy = y + 1;


        //fixShore(west, center, east);
        //fixShore(north, center, south);
		//if (haveLeft)
		{

			float3 u_south = 0.5f * ( bottom + mine ) - dt/(2*dy) * ( VerticalPotential(mine) - VerticalPotential(bottom));
			float3 u_north = 0.5f * ( top + mine ) - dt/(2*dy) * ( VerticalPotential(top) - VerticalPotential(mine));
			float3 u_west = 0.5f * ( left + mine ) - dt/(2*dx) * ( HorizontalPotential(mine) - HorizontalPotential(left) );
			float3 u_east = 0.5f * ( right + mine ) - dt/(2*dx) * ( HorizontalPotential(right) - HorizontalPotential(mine) );

			float3 u_center = mine /*+ dt * SlopeForce(mine, top, right, bottom, left)*/ - dt/dx * ( HorizontalPotential(u_east) - HorizontalPotential(u_west) ) - dt/dy * ( VerticalPotential(u_north) - VerticalPotential(u_south) );
			u_center.x = vmax(0.0f, u_center.x);

			mine = u_center;


		}

		foreach (e.neighborhood,n)
			if (n->userFlags == EntityFlags::IsFocus)
			{
				float x = 1.f - Metric::Distance(n->coordinates,e.coordinates).Squared() / sqr(Entity::MaxAdvertisementRadius);
//				Vec::quadraticDistance(n->coordinates,e.coordinates)/sqr(Entity::MaxAdvertisementRadius);
				//mine.x = x * (WavePlaneHeight +sin(generation*0.1f)* 0.1f) + (1.f - x) * mine.x;
				#ifdef D3
					mine.yz += n->velocity.xy * x *  WavePlaneHeight * 10.f;
				#else
					mine.yz += n->velocity * x *  WavePlaneHeight * 10.f;
				#endif
			}

		//if (!haveLeft)
		//	mine.x = WavePlaneHeight +sin(generation*dt)* 0.1f;


		if (e.coordinates.x >= 0.45f && e.coordinates.x < 0.6f
			&&	e.coordinates.y >= 0.45f && e.coordinates.y < 0.6f
			)
			{
				mine.x = WavePlaneHeight +sin(generation*dt*1.5f)* 0.2f;
				mine.yz = Vector2<>::zero;
			}
		if (isnan(mine.x))
			mine = float3(WavePlaneHeight,0,0);


		foreach (e.neighborhood,n)
		{
			send.DispatchPOD(*n,&Wave, mine);
		}
		float scale = (mine.x-WavePlaneHeight)/10.f;
		inOutShape.zOffset = scale;
		inOutShape.color = float3(scale*20.f+0.5f);
	}


}


void OnMouseWheel(short delta)
{
	Aspect::zoom *= pow(0.9f,-delta);
	Aspect::topDown.UpdateProjection(display.GetPixelAspect(),Aspect::zoom,-1,1);
	Aspect::scene.retraction = float3(0,0,-4.f/Aspect::zoom);
	Aspect::scene.UpdateView();
}

void ZoomIn()
{
	OnMouseWheel(3);
}

void ZoomOut()
{
	OnMouseWheel(-3);
}



void VerifyScene()
{
//	testSimulation.Verify();
}


void	SetupScene()
{
	numResets++;

	{

		const GridSize size(16);

		Array<Entity> entities(currentSetup.numEntities);
		{

			for (int i = 0; i < currentSetup.numEntities; i++)
			{
				Entity&e = entities[i];
				#ifdef D3
					e.coordinates = float3(random.NextFloat(0, size.width), random.NextFloat(0, size.height), random.NextFloat(0, size.depth));
				#else
					e.coordinates = float2(random.NextFloat(0, size.width), random.NextFloat(0, size.height));
				#endif
				//e.coordinates = float2(random.NextFloat(0,testSimulation.GetGridWidth()),0.5f);//don't care about rest
				e.shape = Entity::Shape::Sphere;
				e.scale.x = 0.05f *random.NextFloat(0.25f,1);
				e.scale.y = 0.05f *random.NextFloat(0.25f,1);
				e.orientation = Metric::Direction(random);
				//e.GenerateID();
				memset(&e.guid,0,sizeof(e.guid));
				e.guid.Data1 = (unsigned long)i;	//sufficient, more meaningful

				e.AddLogic(Logic::AggressiveRandomMotion);
				//e.AddLogic(Logic::RandomMotion);
				//e.AddLogic(Logic::HorizontalMinDistance);
			}
		}


		UINT32 seed = random.NextSeed();
		#ifdef RECOVERY_TEST
			seed = 0;
			#ifdef _DEBUG
				size = GridSize(5);
			#endif
		#endif
		testSimulation.Reset(size,Statistics::DoCompareEntityConsistency(),seed,currentSetup.numLayers,entities);
	}
	//testSimulation.Reset(2,2,true,2);

//	Random random;//(1024);

//	#ifndef _DEBUG //random hostile:



	#if 0
		//flock:
		for (index_t i = 0; i < 16; i++)
		{
			Entity e;
			{
				e.coordinates = float2(0.125f + 0.05f*i,0.9f);
				e.shape = Entity::Shape::LineCube;
				e.userFlags = EntityFlags::IsWall;
				e.scale.x = 0.025f;
				e.scale.y = 0.0125f;
				e.orientation = 0;
				e.GenerateID();

				//e.AddLogic(Logic::AggressiveRandomMotion);
				//e.AddLogic(Logic::MinDistance);
	
				testSimulation.CreateEntity(e);
			}
			{
				e.coordinates = float2(0.125f + 0.05f*i,0.1f);
				e.shape = Entity::Shape::LineCube;
				e.userFlags = EntityFlags::IsWall;
				e.scale.x = 0.025f;
				e.scale.y = 0.0125f;
				e.orientation = 0;
				e.GenerateID();

				//e.AddLogic(Logic::AggressiveRandomMotion);
				//e.AddLogic(Logic::MinDistance);
	
				testSimulation.CreateEntity(e);
			}
			{
				e.coordinates = float2(0.1f,0.125f + 0.05f*i);
				e.shape = Entity::Shape::LineCube;
				e.userFlags = EntityFlags::IsWall;
				e.scale.x = 0.025f;
				e.scale.y = 0.0125f;
				e.orientation = M_PIF/2.f;
				e.GenerateID();

				//e.AddLogic(Logic::AggressiveRandomMotion);
				//e.AddLogic(Logic::MinDistance);
	
				testSimulation.CreateEntity(e);
			}
			{
				e.coordinates = float2(0.9f,0.125f + 0.05f*i);
				e.shape = Entity::Shape::LineCube;
				e.userFlags = EntityFlags::IsWall;

				e.scale.x = 0.025f;
				e.scale.y = 0.0125f;
				e.orientation = M_PIF/2.f;
				e.GenerateID();

				//e.AddLogic(Logic::AggressiveRandomMotion);
				//e.AddLogic(Logic::MinDistance);
	
				testSimulation.CreateEntity(e);
			}



		}
		{
			Entity e;
			e.coordinates = float2(0.6f,0.6f);
			e.shape = Entity::Shape::LineCube;
			e.userFlags = EntityFlags::IsWall;

			e.scale.x = 0.025f;
			e.scale.y = 0.0125f;
			e.orientation = M_PIF/2.f;
			e.GenerateID();

			//e.AddLogic(Logic::AggressiveRandomMotion);
			//e.AddLogic(Logic::MinDistance);
	
			testSimulation.CreateEntity(e);
		}
		{
			Entity e;
			e.coordinates = float2(0.6f,0.65f);
			e.shape = Entity::Shape::LineCube;
			e.userFlags = EntityFlags::IsWall;

			e.scale.x = 0.025f;
			e.scale.y = 0.0125f;
			e.orientation = M_PIF/2.f;
			e.GenerateID();

			//e.AddLogic(Logic::AggressiveRandomMotion);
			//e.AddLogic(Logic::MinDistance);
	
			testSimulation.CreateEntity(e);
		}
		for (int i = 0; i < 30; i++)
		{
			Entity e;
			e.coordinates = float2(random.getFloat(0.2f,0.8f),random.getFloat(0.2f,0.8f));
			e.orientation = random.getFloat(M_PIF*2);
			e.userFlags = EntityFlags::IsFish;
			//M_PIF*0.25f;
			e.GenerateID();

			e.AddLogic(Logic::Flock::Align);
			e.AddLogic(Logic::Flock::MaintainVelocity);
			e.AddLogic(Logic::Flock::Focus);
			//e.AddLogic(Logic::Flock::Center);
			e.AddLogic(Logic::Flock::MaintainMinimumDistance);
			e.AddLogic(Logic::Flock::AvoidCollision);
		
			testSimulation.CreateEntity(e);
		}
	#endif /*0*/


	#if 0
		//waves:
		for (float x = 0; x < 1.f; x+= Logic::WaveSpacing)
		//for (float x = 0; x < Logic::WaveSpacing*0.5f; x+= Logic::WaveSpacing)
		for (float y = 0; y < 1.f; y+= Logic::WaveSpacing)
		//for (float y = 0; y < Logic::WaveSpacing*2.5f; y+= Logic::WaveSpacing)
		//for (float y = 0; y < Logic::WaveSpacing*0.5f; y+= Logic::WaveSpacing)
		{
			Entity e;
			e.coordinates = float2(x,y);
			e.orientation = 0;
			e.shape = Entity::Shape::SolidCube;
			e.scale = float2(Logic::WaveSpacing/4);
			//e.userFlags = EntityFlags::IsWave;
			//M_PIF*0.25f;
			e.GenerateID();

			e.AddLogic(Logic::Wave);
		
			testSimulation.CreateEntity(e);
			
		}
	#endif /*0*/


	//{
	//	Entity e;
	//	e.coordinates = float2(0.5f,0.5f);
	//	e.shape = Entity::Shape::None;
	//	e.userFlags = EntityFlags::IsFocus;
	//	e.scale.x = 0.025f;
	//	e.scale.y = 0.0125f;
	//	e.orientation = M_PIF/2.f;
	//	e.GenerateID();
	//	avatarID = e.guid;
	//	hasAvatar = true;

	//	e.AddLogic(Logic::UserAvatar);

	//	testSimulation.CreateEntity(e);
	//}
}



void	StartNext(bool failed)
{
	ASSERT__(!testSimulation.IsAsync());
	if (testSimulation.GetTopGeneration() > 0)
	{
		#ifndef RECOVERY_TEST
			if (failed)
				currentSetup = Statistics::SignalRecoveryFailed( testSimulation);
			else
				currentSetup = Statistics::SignalSimulationEnd( testSimulation);
		#endif
	}
	SetupScene();
	//dataRanges.Append().name = String(s.ToString())+"["+String(overload)+"]";

}



void EvolveV(const TInputVector&vec)
{
	if (hasAvatar && !testSimulation.IsAsync())
		testSimulation.DispatchUserMessage(avatarID, Logic::UserAvatar, &vec, sizeof(vec));

	//if (testSimulation.GetTopGeneration() >= 1000)
	//{
	//	SetupScene();

	//	Statistics::ExportToFile("capture.txt");
	//}


	const size_t physical = System::getPhysicalMemory();
	static const size_t window = 2000000000;
	const size_t allowance = physical > window ? physical - window : window;


	auto success = testSimulation.RunIterationAsync(currentSetup,true);
	if (success == Simulation::Result::Delayed)
		return;

	bool forceReset = false;

	if (System::getMemoryConsumption() >= allowance)
	{
		LogMessage("Max RAM consumption ("+String(allowance)+") reached. Forcing reset.");
		forceReset = true;
	}

	#if defined RECOVERY_TEST && defined DBG_SHARD_HISTORY
		if (testSimulation.GetTopGeneration() >= 50)
			currentSetup.reliabilityLevel = INT_MAX;
	#endif



	if (success == Simulation::Result::Failed)
	{
		#ifndef RECOVERY_TEST
			StartNext(true);
		#endif
	}
	else
		if (testSimulation.GetTopGeneration() > 10000 || forceReset)
			StartNext(false);

	if (stopSimulation)
	{
		simulate = false;
		SetThreadExecutionState(ES_CONTINUOUS);
		stopSimulation = false;
		return;
	}

}

void Recover()
{
}

float GetFloat(std::mt19937&random, float min, float max)
{
	return float(random()) / random.max() * (max - min) + min;
}

//
//void ExportSVG(const String&baseName)
//{
//	XML::Container c;
//	c.root_node.name = "svg";
//	c.root_node.SetMore("xmlns","http://www.w3.org/2000/svg").SetMore("xmlns:xlink","http://www.w3.org/1999/xlink").SetMore("version","1.1");
//	PathString imageFolder = PathString(".\\"+baseName+"Images\\");
//	FileSystem::Folder f(imageFolder);
//	if (f.IsValidLocation())
//		FileSystem::RemoveFolderContents(f.GetLocation());
//	else
//		ASSERT__(FileSystem::CreateDirectory(imageFolder));
//	float2 size;
//	#ifndef D3
//	Scene::BuildSVG(c.root_node,imageFolder,size);
//	#endif
//	c.root_node.SetMore("width",size.x);
//	c.root_node.SetMore("height",size.y);
//	c.SaveToFile(PathString(baseName+".svg"));
//}
//
//void MakeSVG()
//{
//	ExportSVG("out");
//}


void StartSimulation()
{
	simulate = true;
	if (SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED) == NULL)
	   SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
}

void StopSimulation()
{
	if (!testSimulation.IsAsync())
	{
		simulate = false;
		SetThreadExecutionState(ES_CONTINUOUS);
	}
	else
		stopSimulation = true;
}


void Pause()
{
	if (simulate)
		StopSimulation();
	else
		StartSimulation();
}

void VerifyIntegrity()
{
	testSimulation.AssertIsRecovering();
}


void MakeErrorFree()
{
	currentSetup.reliabilityLevel = INT_MAX;

}

void RecoverOnly()
{
	testSimulation.RecoverOne(currentSetup,false);
	Database::ProcessDistribution();

}

int main( int argc, const char* argv[])
{
	









	//direction test:
	for (int i = 0; i < 10000; i++)
	{
		Metric::Direction2D dir(random);
		TVec2<> vector = dir.ToVector();
		Metric::Direction2D dir2(vector);
		ASSERT3__(similar(dir.value,dir2.value),dir.value,dir2.value,Vec::toString(vector));
	}


	//random test:
	/*FixedArray<count_t,20>	tests,tests2;
	tests.Fill(0);
	tests2.Fill(0);
	for (int i = 0; i < 100000; i++)
	{
		tests[random.NextIndex(tests.Count())] ++;
		tests2[random.Next((UINT)tests.Count()-1)]++;
	}
	TFloatRange<count_t>	range = TFloatRange<count_t>::Invalid;
	foreach (tests2,t)
		range.Include(*t);
*/



	Vec::assertConsistency<float>();


	bool brk = true;


	//static const count_t Total = 1000000;
	//float seconds0,seconds1;
	//{
	//	std::mt19937	random((long)1024);
	//	Timer::Time t = timer.Now();
	//	for (index_t i = 0; i < Total; i++)
	//	{
	//		TVec3<>	p = {GetFloat(random,-1000,1000),GetFloat(random,-1000,1000),GetFloat(random,-1000,1000)};
	//		volatile double v = noise.Get(p.x,p.y,p.z);
	//	}
	//	seconds0 = timer.GetSecondsSince(t);
	//}
	//{
	//	std::mt19937	random((long)1024);
	//	Timer::Time t = timer.Now();
	//	for (index_t i = 0; i < Total; i++)
	//	{
	//		TVec3<>	p = {GetFloat(random,-1000,1000),GetFloat(random,-1000,1000),GetFloat(random,-1000,1000)};
	//		volatile double v = perlinNoise.Get(p.x,p.y,p.z);
	//	}
	//	seconds1 = timer.GetSecondsSince(t);
	//}

	//ShowMessage("simplex: "+String(seconds0)+", perlin: "+String(seconds1));


	try
	{

		display.config.fsaa_samples = 4;
		display.config.vertical_sync = 1;
		Engine::CreateDisplay(display,"./config","Grid Simulation"
			#ifdef _DEBUG
				" (DEBUG)"
			#endif
			,Resolution(1280,800),onWindowResize
			,Engine::DisplayConfig::Icon(IDI_CLIENT)
			);
		display.RegisterFocusCallbacks([](){windowFocused = false;},[](){windowFocused = true;});

		textout.GetActiveFont()->LoadFromFile("Font/font.xml");


//			Engine::mouse.bindWheel(Camera::Event::WheelAction);

		textout.SetScale(1.f/display.GetClientWidth(),1.f/display.GetClientHeight());

		Engine::GlobalAspectConfiguration::worldZIsUp = true;
		Aspect::hud.UpdateProjection(0,0,1,1,-1,1);

		Aspect::scene.retraction = float3(0,0,-4.f/Aspect::zoom);
		Aspect::scene.UpdateProjection(display.GetPixelAspect(),0.1,1000);
		#ifdef D3
		Aspect::scene.angle.z = 45;
		Aspect::scene.angle.x = 45;
		#else
			//Aspect::scene.angle.z = 45;
			//Aspect::scene.angle.x = 45;

		#endif
		Aspect::scene.UpdateView();
		Aspect::scene.depthTest = Engine::NoDepthTest;

		#ifdef D3
			display.SetBackbufferClearColor(0,0,0,0);
		#else
			display.SetBackbufferClearColor(1,1,1,0);
		#endif

		Engine::mouse.BindWheel(OnMouseWheel);
		Engine::input.Bind(Key::Space,Pause);
		Engine::input.Bind(Key::Return,VerifyIntegrity);
		Engine::input.Bind(Key::Enter,VerifyIntegrity);
		//Engine::input.Bind(Key::N1,[](){blockCenter = !blockCenter;});
		//Engine::input.Bind(Key::N2,[](){blockRight = !blockRight;});
		Engine::input.Bind(Key::Right,StartSimulation,StopSimulation);
		Engine::input.Bind(Key::E,MakeErrorFree);
		Engine::input.Bind(Key::R,RecoverOnly);
		Engine::input.Bind(Key::Up,ZoomIn);
		Engine::input.Bind(Key::Down,ZoomOut);
		
		#ifndef D3
		Scene::Create();
		#endif


		//BeginNextRange();

		//Statistics::ImportMean("capture.txt");
		//StartSimulation();

		#ifndef RECOVERY_TEST
			currentSetup = Statistics::Begin();
		#else
			currentSetup.reliabilityLevel = 30;
		#endif
		SetupScene();
		testSimulation.Rebuild();

		display.Assign(Loop);
		StartSimulation();
		display.Execute();

		display.Destroy();
		application_shutting_down = true;

		TerminateProcess(GetCurrentProcess(),0);	//self-terminate

		return 0;
	}
	catch (const Except::Exception&exception)
	{
		FATAL__("Caught unhandled exception of type "+String(exception.GetType())+":\n"+exception.what());
	}
	catch (const std::exception&exception)
	{
		FATAL__("Caught unhandled exception of type "+String(typeid(exception).name())+":\n"+String(exception.what()));
	}
	catch (...)
	{
		FATAL__("Caught something...");
	}
	return -1;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	return main(0,NULL);
}




