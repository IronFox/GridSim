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
#include "simulationSetup.h"

bool			windowFocused = true;


/**
The following DWORD causes Nvidia Optimus to select the high performance GPU.
Don't change the name, value, or dllexport-declaration ("C", so it shows up as that name in the export and not some mangled thing)
*/
extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}


Engine::Display<Engine::OpenGL>	display;
Engine::Textout<Engine::GLTextureFont2>	textout;
Buffer0<String>	messageLog;
Sys::SpinLock	messageLogLock;



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
//					float h = float(px[0])/M::Max(1,px[1])*(LayerHeight-BarHeight)*0.9f;
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


	if (SimulationActive())
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
		const float sceneExt = Vec::length(ToVector( testSimulation.GetGridSize() ));
		float base = M::Max(0.f,Vec::length(Aspect::scene.GetAbsoluteLocation()) - sceneExt);
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
			#ifdef CAPTURE_MESSAGE_VOLUME
				<< "In messages (MB): "<< MessageData::totalMessageVolume/1024/1024<<" reachable "<< testSimulation.GetReachableMessageVolume()/1024/1024 <<nl 
			#endif

			#if 0
			<< /*testSimulation.CountCurrentEntities()*/accumulation<<"/"<< simAt << nl 
			#endif /*0*/
			//<< (connectionSamples?double(connectedSamples) / connectionSamples : 0) << " ("<<connectionSamples<<")"<<nl
			<< 1.f / Engine::timing.delta << nl;
		textout.PopState();


		display.useTexture(nullptr);
		M::Rect<> rect(0.7f,0.05f,0.95f,0.3f);
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



int main( int argc, const char* argv[])
{
	








	Random random;
	//direction test:
	for (int i = 0; i < 10000; i++)
	{
		Metric::Direction2D dir(random);
		M::TVec2<> vector = dir.ToVector();
		Metric::Direction2D dir2(vector);
		ASSERT3__(M::Similar(dir.value,dir2.value),dir.value,dir2.value,Vec::toString(vector));
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




