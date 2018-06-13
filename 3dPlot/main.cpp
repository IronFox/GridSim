#include <global_root.h>
#include "main.h"
#include <ppl.h>
#include <math/vclasses.h>
#include <general/random.h>
#include <math/polynomial.h>
#include <io/cryptographic_hash.h>
#include "sample.h"
#include "rendering.h"
#include "hypothesis.h"
#include "table.h"
#include <io/delta_crc32.h>
#include <string/converter.h>

using namespace DeltaWorks;

float l[3] = {0,1,0};



bool trainStatic = false;
static bool	radial = false;

SampleType	currentSampleType = SampleType::DivergenceDepth;

float h0 = 0;

float fontScale = 1.f;
//2.f;




extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}





//struct TVertex
//{
//	float3	coordinates,
//			normal;
//	float	absoluteHeight;
//};
//
//class Geometry
//{
//public:
//	Buffer0<TVertex>	vertices;
//
//};
//



const char* Shorten(const SampleType&t)
{
	switch (t)
	{
		case SampleType::DivergenceDepth:
			return "DD";
		case SampleType::Inconsistency:
			return "IP";
		case SampleType::InconsistencyDelta:
			return "IP'";
		case SampleType::MissingEntities:
			return "ME";
	}
	return t.ToString();
}


void UpdatePlotGeometry()
{
	UpdateTableRanges(currentSampleType);

	float vRange = GetVerticalRange(currentSampleType);
	range.z.max = vRange * 1.1f;
	arrowLabel.z = Shorten(currentSampleType);
	markerSteps.z = vRange/5.0;
	if (tables.IsNotEmpty())
	{
		//rangeTable.UpdatePlotGeometry(currentSampleType);

		foreach (tables,t)
			if (t == tables.begin())
				t->UpdatePlotGeometry(currentSampleType,false);
	}


}


String CreateFilename(const Table::TMetrics&m)
{
	String name = String(m.dimensions)+"D_"+String(m.entityDensity);
	if (m.entityDeathAllowed)
		name += "d";
	if (m.motionProfile == Table::TMetrics::Linear)
		name += "l";
	elif (m.motionProfile != Table::TMetrics::Binary)
		name += "e";
	name += "_"+String(m.totalSimulationRuns);
	name += String("_")+currentSampleType.ToString();
	return name;
}

PathString GetCaptureFilename(const String&baseName, const String&extIncludingDot)
{
	FileSystem::CreateDirectory("captures");
	index_t counter = 0;
	while (FileSystem::IsFile(PathString("captures/"+baseName+String(counter)+extIncludingDot)))
		counter++;
	return PathString("captures/"+baseName+String(counter)+extIncludingDot);
}



void PreviousFile();
void NextFile();

bool takeScreenshot = false;
bool takeTransparentScreenshot = false;

bool loop()		//main loop function
{
	if (mouse.buttons.pressed)		//rotate the camera if any mouse button is currently pressed
	{
		M::TVec2<> d = mouse.location.windowRelative - mouse.previous_location.windowRelative;
		camera.AlterAngles(-d.y*100.0f,0,-d.x*100.0f);						//rotate the camera (also rebuild the camera)
	}
	static Timer::Time lastLoad = timer.Now();
	if (timer.GetSecondsSince(lastLoad) > 0.25f)
	{
		if (input.pressed[Key::Left])
		{
			PreviousFile();
			lastLoad = timer.Now();
		}
		elif (input.pressed[Key::Right])
		{
			NextFile();
			lastLoad = timer.Now();
		}
	}


	RenderShadows();

	display.Pick(camera);
		//display.renderToken();
		

		glDisable(GL_CULL_FACE);

		
		RenderExtendedAxes();

		
		RenderPlot();
		
		


		glBlack();



		if (!radial)
		{

			foreach (tables,t)
				t->RenderLines();

//			glDepthRange (0.0, 0.9999f);
//			glLineWidth(2.f);
//			const auto&yRange = range.y;
//			const auto&yMarkerSteps = markerSteps.y;
//			const float yExtent = 2;
//
//			int ybegin = (int)ceil(yRange.min / yMarkerSteps);
//			int yend = (int)ceil(yRange.max / yMarkerSteps);
//			int xbegin = (int)ceil(range.x.min / markerSteps.x);
//			int xend = (int)ceil(range.x.max / markerSteps.x);
//
//	/*		for (int y = ybegin; y < yend; y++)
//			{
//				float fy = y * yMarkerSteps;
//				float relative = yRange.Relativate(fy);
//				glColor3f(0.25f,0.25,0.25f);
//				foreach (tables,t)
//				{
//					glBegin(GL_LINE_STRIP);
//						for (int i = 0; i < Table::res; i++)
//						{
//							float fi = float(i)/(Table::res-1);
//							float h = t->HeightFunction(currentSampleType, range.x.Derelativate( fi ), fy);
//							glVertex3f(fi * 2.f - 1.f, relative*2.f - 1.f, h);
//						}
//					glEnd();
//				}
//			}
//*/
//			//for (int x = xbegin; x < xend; x++)
//			//{
//			//	float fx = x * markerSteps.x;
//			//	float relative = range.x.Relativate(fx);
//			//	glBegin(GL_LINE_STRIP);
//			//		for (int i = 0; i < res; i++)
//			//		{
//			//			float fi = float(i)/(res-1);
//			//			float h = HeightFunction(fx, range.y.Derelativate( fi ));
//			//			glVertex3f(relative * 2.f - 1.f, fi*2.f - 1.f, h);
//			//		}
//			//	glEnd();
//			//}
//
//
//			glDepthRange (0.0, 1.f);
//
//
//			glDisable(GL_DEPTH_TEST);
//			glColor4f(1,0,0,0.5f);

			//if (Engine::input.pressed[Key::Space])
			//{
			//	static index_t attribute = 0;
			//	foreach (hypotheses,h)
			//		h->OptimizeAttribute((Hypothesis::Attribute::name_t)attribute,0.1f);
			//	attribute++;
			//	attribute %= Hypothesis::Attribute::N;
			//}

			index_t selected = 1;
		}

		glLineWidth(2);
	RenderExtendedAxesLabels();


		if (takeScreenshot)
		{
			takeScreenshot = false;
			Image image;
			display.Capture(image);
			image.TruncateToOpaque();
			if (takeTransparentScreenshot)
			{
				foreach (image.Horizontal(),x)
					foreach (image.Vertical(),y)
					{
						BYTE*pixel = image.Get(x,y);
						if (pixel[3])
						{
							float a = float(Vec::max(Vec::ref4(pixel))) / 255.f;
								//float(pixel[3])/255.f;
							float3 c0 = float3(pixel)/255.f;
						
							float3 c = c0 / a;
							Vec::clamp(c,0,1);
							Vec::mult(c,255.f,Vec::ref3(pixel));
						}

					}
			}
			else
				image.SetChannels(3);

			//String currentFile = "";
			//foreach (tables,t)
			//	currentFile += t->currentFile;
			try
			{
				const Table&t = tables.First();
				const PathString path = GetCaptureFilename(CreateFilename(t.metrics),".png");
				png.SaveToFileQ(image,path);
			}
			catch (...)
			{}
			if (takeTransparentScreenshot)
				glClearColor(1,1,1,0);

		}

	display.Pick(hud);
		textout.MoveTo(0,1);
		foreach (tables,t)
		{
			textout.SetColor(t->color);
			textout << nl << t->sourceFile;
			textout << nl << "  density "<<t->metrics.entityDensity;
			textout << nl << "  motionRange "<<t->metrics.relativeMovementRange;
			textout << nl << "  "<<t->metrics.entitiesPerSimulation<< " entities per simulations";
			textout << nl << "  "<<t->metrics.totalSimulationRuns << " simulations";
		}
		textout.SetColor(1,1,1);
		if (input.pressed[Key::V])
			ShowMessage(display.renderState());
		textout << nl << currentSampleType.ToString();
		textout << nl << camera.angle;

	return !input.pressed[Key::Escape];			//continue as long as the escape key is not pressed
}


void wheelAction(short param)					//handler for the mouse wheel
{
	camera.retraction.z /= (1+0.05*param);	//alter camera retraction depending on the mouse wheel delta value
	camera.UpdateView();								//and rebuild the camera
}


void ZoomOut()								//handler for the down arrow key
{
	camera.retraction.z /= (1-0.025);			//increase retraction slightly
	camera.UpdateView();								//and rebuild the camera
}

void ZoomIn()								//handler for the up arrow key
{
	camera.retraction.z /= (1+0.025);			//decrease retraction slightly
	camera.UpdateView();								//and rebuild the camera
}

void	OnResize(const Resolution&res, UINT32 flags)
{
	camera.UpdateProjection(res.GetAspect(),
					0.1,
					100,
					45);

	textout.SetScale(fontScale/res.width,fontScale/res.height);

}



void Convert(StringRef&_str, float&result)
{
	String str = _str;
	str.FindAndReplace(',','.');
	ASSERT1__(convert(str.c_str(),str.GetLength(),result),str);
}

void LoadParameters(const FileSystem::File&f)
{
	hypothesis.Deactivate();
	StringFile file;
	index_t cnt = 0;
	if (!f.DoesExist())
		return;
	for (;;)
	{
		try
		{
			file.Open(f.GetLocation());
			break;
		}
		catch (...)
		{}
		Sleep(200);
		cnt++;
		if (cnt > 5)
			return;
	}

	String line;
	Array<StringRef>	parts;
	bool hasLocationErrorSum = false;
	bool hasMissingEntites = false;
	file >> line;	//headers
	ASSERT__(line == "SampleCount	Error	Offset	Scale	HeightScale	Alpha	Beta");
	Buffer0<float>	alpha,beta,height;
	while (file >> line)
	{
		explode(tab,line,parts);
		ASSERT_EQUAL__(parts.Count(),7);
		Convert(parts[4],height.Append());
		Convert(parts[5],alpha.Append());
		Convert(parts[6],beta.Append());
	}
	alpha.CopyToArray(hypothesis.sigma);
	beta.CopyToArray(hypothesis.mu);
	height.CopyToArray(hypothesis.height);

	file.Close();
	//currentFile = String(f.GetInnerName());
	hypothesis.Activate();
}





struct Source
{
	FileSystem::File	sampleFile,
						parameterFile;


	void				Setup(const FileSystem::File&sampleAt)
	{
		sampleFile = sampleAt;
		FileSystem::Folder().FindFile(  "../BatchNormFinder/results/v6_"+sampleAt.GetName(),parameterFile,false );
	}

	void				Load(const Source&successor)
	{
		LoadTables({sampleFile},true);

		//tables.Append();
		//tables.Last().SetTransformed(tables.GetFromEnd(1),
		//	[](TSample s)->TSample
		//	{
		//		s.locationError = pow(s.Get(SampleType::DivergenceDepth),0.3) * s.numEntities * 4;
		//		return s;
		//	}, 
		//	float4(1,0,0,0.5));
		//tables.Append().TrainHypothesis(0,currentSampleType,trainStatic);

		#if 0
		float3 plane = tables.Append().TrainFlatOverestimation(0,SampleType::DivergenceDepth,0.001f);


		if (false)
		{
			//let's try to create a mapping
			//range is now used DD range. we can now split it into sub-ranges
			struct TSubRange
			{
				count_t				numSamples = 0;
				double				sum = 0;
				TFloatRange<double> valueRange = MaxInvalidRange<double>();
			};
			StringFile csv;
			csv.Create("Err_Mapping.csv");

			for (index_t i = 0; i < SampleType::N; i++)
			{
				const SampleType st = SampleType::Reinterpret(i);
				Array<TSubRange> subRanges(100);
				for (index_t t = 1; t < tables.First().samples.Count(); t++)
				{
					const auto&sampled = tables.First().samples[t];
					for (index_t s = 0; s < sampled.Count(); s++)
					{
						const double y = sampled[s].Get(st);
						const double x = tables.Last().Get(s,t,st);
						ASSERT__(x >= 0);
						ASSERT__(x <= 1);
						const index_t subIndex = Math::Min(subRanges.Count()-1,index_t( x * subRanges.Count()));
						subRanges[subIndex].numSamples++;
						subRanges[subIndex].valueRange.Include(y);
						subRanges[subIndex].sum += y;
					}
				}
				csv << st.ToString()<< "_high";
				foreach (subRanges,r)
					csv << ","<<r->valueRange.max;
				csv << nl;
				csv << st.ToString()<< "_mean";
				foreach (subRanges,r)
					csv << ","<<(r->sum/r->numSamples);
				csv << nl;
				csv << st.ToString()<< "_low";
				foreach (subRanges,r)
					csv << ","<<r->valueRange.min;
				csv << nl;
			}
			csv.Close();
		}

		#endif /*0*/

		UpdateColors();
	}

};

Buffer0<Source>			sources;
index_t currentSourceIndex = InvalidIndex;

void	LoadFile(index_t at)
{
	if (sources.IsNotEmpty())
	{
		currentSourceIndex = at;

		sources[at].Load(sources[(at+1)%sources.Count()]);
	}

	UpdatePlotGeometry();
}


void NextFile()
{
	if (sources.IsEmpty())
		return;
	currentSourceIndex ++;
	currentSourceIndex %= sources.Count();
	LoadFile(currentSourceIndex);
}

void PreviousFile()
{
	if (sources.IsEmpty())
		return;
	if (currentSourceIndex)
		currentSourceIndex --;
	else
		currentSourceIndex = sources.Count() -1;
	LoadFile(currentSourceIndex);
}



void IterateSampleMode()
{
	index_t next = (currentSampleType+1)%SampleType::N;
	if (input.pressed[Key::Shift])
	{
		if (currentSampleType > 0)
			next = currentSampleType -1;
		else
			next = SampleType::N-1;
	}
	currentSampleType.Load(next);
	UpdatePlotGeometry();
}

void MakeScreenshot()
{
	takeScreenshot = true;
	takeTransparentScreenshot = Engine::input.pressed[Key::Shift];
	if (takeTransparentScreenshot)
		glClearColor(0,0,0,0);
}
//
//String ColorOf(const Table::TMetrics&m)
//{
//	Random random( CRC32::getChecksum(&m,sizeof(m)) );
//
//	return 
//}

void ExportExtremePlotP(const PathString&path)
{
	count_t extreme = 32;
	StringFile file;
	try
	{
		file.Create(path);
	
		float max = 0;
		
		for (const auto&t:tables)
			for (index_t i = 1; i < t.samples.Count() && i <= extreme; i++)
			{
				const auto&row = t.samples[i];
				for (index_t j = 0; j < row.Count() && j <= extreme*t.metrics.spatialSupersampling; j++)
				{
					float f = row[j].Get(currentSampleType);
					max = std::max(max,f);
				}
			}

		float min = -max / 20.f;
		max *= 1.f + 1.f / 20.f;

		String yLabel = Shorten(currentSampleType);
		file << "\\begin{tikzpicture}"<<nl
			<< "\\input{resources/plotstyle.tex}"<<nl
			<< "\\begin{axis}["<<nl
			<< "width=0.6\\linewidth,"<<nl
			//xmin=0,
			<< "xmax="<<extreme<<","<<nl
			<< "ymin="<<min<<",ymax="<<max<<","<<nl
			<< "xlabel=T \\small(S{=}1),"<<nl
			//<< "cycle list/RdGy-6,"<<nl
			<< "cycle list name=plotcolorlist,"<<nl
			<< "ylabel=$"<<yLabel<<"$]"<<nl;
		for (const auto&t:tables)
		{
			//,style={ultra thick}
			file << "\\addplot+[mark=] plot coordinates {"<<nl;
				for (index_t i = 1; i < t.samples.Count() && i <= extreme; i++)
				{
					float f = t.GetSample(1,i).Get(currentSampleType);
					file << tab << "("<<i<<","<<f<<")"<<nl;
				}
			file << "};"<<nl;
		}
			//<<"\\addlegendentry{Temporal segment}"<<nl
		file <<"\\end{axis}"<<nl
			<< "\\begin{axis}["<<nl
			<< "at={(0.42\\linewidth,0)},"<<nl
			<< "width=0.6\\linewidth,"<<nl
			<< "xmin=1,"<<nl//xmax=128,
			<< "ymin="<<min<<",ymax="<<max<<","<<nl
			<< "xlabel=S \\small(T{=}"<<extreme<<"),"<<nl
			//<< "cycle list/RdGy-6,"<<nl
			<< "yticklabels={,,},"<<nl
			<< "cycle list name=plotcolorlist"<<nl
			<< "]"<<nl;

		for (const auto&t:tables)
		{
			file <<"\\addplot+[mark=] plot coordinates {"<<nl;
			{
				const auto&row = t.samples[extreme];
				for (index_t i = 0; i < row.Count() && i <= extreme*t.metrics.spatialSupersampling; i++)
				{
					float f = row[i].Get(currentSampleType);
					file << tab << "("<<float(i+1) / t.metrics.spatialSupersampling<<","<<f<<")"<<nl;
				}
			}
			file << "};"<<nl
				<<"\\addlegendentry{"<<t.metrics.entityDensity<<"}"<<nl
				;
		}
		file <<"\\end{axis}"<<nl
			<<"\\end{tikzpicture}"<<nl;
	}
	catch (...)
	{
		std::cerr << "Unable to create plot file at "<<String(path)<<std::endl;
	}

}


void ExportExtremePlot()
{
	String name = "plot";
	ExportExtremePlotP( GetCaptureFilename(name,".tex") );

}


void ExportPlot(SampleType t)
{
	currentSampleType = t;
	ExportExtremePlotP(PathString("D:\\paper\\technites\\active_redundancy\\resources\\"+String(t.ToString())+".tex"));
}


namespace ResultTable
{

	struct TCell
	{
		float3	linearPlaneFunc[3];	//for 1,2, or 3 dimensions
		bool	isSet[3] = {false,false,false};
	};


	struct TResultAxis
	{
		struct TStop
		{
			bool				isComplete=false;
			float				at=0;

			bool				operator<(float x) const
			{
				return at < x;
			}
			bool				operator>(float x) const
			{
				return at > x;
			}

		};


		Array<TStop>			stops;

		bool					FindOrInsert(float stop, index_t&closestBefore)
		{
			if (Find(stop,closestBefore))
				return true;

			stops.Insert(closestBefore).at = stop;

			return false;
		}

		bool					Find(float stop, index_t&closestBefore) const
		{
			if (stops.IsEmpty())
			{
				closestBefore = 0;
				return false;
			}
			if (stops.First() > stop)
			{
				closestBefore = 0;
				return false;
			}
			if (stops.Last() < stop)
			{
				closestBefore = stops.Count();	//one past last
				return false;
			}
			index_t begin = 0;
			index_t end = stops.Count();
		
			while (begin < end)
			{
				index_t middle = (begin+end)/2;
				float middleStop = stops[middle].at;
				if (middleStop == stop)
				{
					closestBefore = middle;
					return true;
				}
				if (middleStop < stop)
					begin = middle+1;
				else
					end = middle;
			}
			/*
				index_t begin = 0;
		index_t end = resultRows.Count();
		while (begin < end)
		{
			index_t middle = (begin+end)/2;
			float mDensity = resultRows[middle].density;
			if (mDensity > density)
				end = middle;
			elif (mDensity < density)
				begin = middle+1;
			else
			{
				return resultRows[middle];
			}
		}
	*/


			closestBefore = begin;
			return false;
		}
	};

	Array2D<TCell>		resultCells;
	TResultAxis					densities,relativeMovementRanges;


	count_t CountDensityEntries(index_t dimension)
	{
		count_t numEntries = 0;
		foreach (ResultTable::resultCells,cell)
			if (cell->isSet[dimension])
				numEntries++;
		return numEntries;

	}

	count_t CountDensityEntriesForRelativeMovementRange(index_t r, index_t dimension)
	{
		count_t numEntries = 0;
		for (index_t d = 0; d < ResultTable::densities.stops.Count(); d++)
			if (ResultTable::resultCells.Get(d,r).isSet[dimension])
				numEntries++;
		return numEntries;
	}

	void	EraseRelativeMovementRange(index_t r)
	{
		relativeMovementRanges.stops.Erase(r);
		resultCells.EraseRow(r);
	}


	TCell& AllocateCell(float density, float relativeMovementRange)
	{
		index_t x,y;
		const bool d = densities.FindOrInsert(density,x);
		const bool r = relativeMovementRanges.FindOrInsert(relativeMovementRange,y); 
		if (!d)
			resultCells.InsertColumnBefore(x);
		if (!r)
			resultCells.InsertRowBefore(y);
		ASSERT_EQUAL__(densities.stops.Count(),resultCells.GetWidth());
		ASSERT_EQUAL__(relativeMovementRanges.stops.Count(),resultCells.GetHeight());

		return resultCells.Get(x,y);
	}
}


float GetMotionProportion(const String&filename)
{
	index_t at = filename.Find("_m");
	ASSERT1__(at != InvalidIndex,filename);

	index_t dot = filename.FindLast('.');
	index_t us = filename.FindLast('_');
	if (us > at && us < dot)
		dot = us;

	StringRef number = filename.SubStringRef(at+2,dot-at-2);

	float rs;
	ASSERT1__(convert(number.pointer(),number.GetLength(),rs),number);
	return rs;
}


void GetFilesIn(FileSystem::Folder sources, BasicBuffer<FileSystem::File>&rsOut, const std::function<bool(const PathString&)>&filter = std::function<bool(const PathString&)>())
{
	sources.Rewind();
	FileSystem::File file;
	while (sources.NextFile(".txt",file))
	{
		if (!filter || filter(file.GetLocation()))
			rsOut << file;
	}
	sources.Rewind();
	while (sources.NextFolder(file))
		GetFilesIn(file,rsOut,filter);
}


template <typename T, count_t NumCoefficientsX, count_t NumCoefficientsY>
	void PrintTableToCSV(StringFile&csv, const char*name, const M::TMatrix<T,NumCoefficientsX,NumCoefficientsY>&p)
	{
		csv << name<<','<<NumCoefficientsX<<','<<NumCoefficientsY;
		csv << nl;
		for (index_t y = 0; y < NumCoefficientsY; y++)
		{
			for (index_t x = 0; x < NumCoefficientsX; x++)
				csv << ','<<p.axis[x].v[y];
			csv << nl;
		}
	}

int main()	//main entry point
{
	try
	{
		//{
		//	float4 x = float4(0.25f,0.5f,0.75f,1.f);
		//	float4 y = float4(0.037,	0.035,	0.03,	0.025);
		//	TMatrix4<> mat,inv;
		//	mat.x = Vector4<>::one;

		//	mat.y = float4(x.x,x.y,x.z,x.w);
		//	mat.z = mat.y & mat.y;
		//	mat.w = mat.z & mat.y;

		//	bool rs = Mat::invert(mat,inv);

		//	float4 results;

		//	Mat::Mult(inv,y,results);

		//	System::copyToClipboard(nullptr,(String(results.x)+"\t"+String(results.y)+"\t"+String(results.z)+"\t"+String(results.w)).c_str());
		//	ShowMessage(results.ToString()+" "+String(rs));
		//}




		Buffer0<FileSystem::File>	files;
		//GetFilesIn(FileSystem::Folder("../SimpleSimulation/results/compare"),files);

		files << *FileSystem::Folder("./data").FindFile("icProfile262144.NoHistoryState.csv");

		#if 0
		GetFilesIn(FileSystem::Folder("./data"),files,[](const PathString&path)->bool
		{
			const index_t dDat = path.GetIndexOf("data");
			index_t dD = path.FindFrom(dDat,"D" FOLDER_SLASH_STR);
			if (!dD)
				return false;
			dD-=3;
			const index_t dI = path.FindFrom(dD,FOLDER_SLASH_STR"d");
			if (!dI)
				return false;
			const index_t sI = path.FindFrom(dI,FOLDER_SLASH);
			if (!sI)
				return false;

			count_t dimensions;
			const String sdim = String(path.subStringRef(dD+1,dI-dD-3));
			if (!Convert(sdim,dimensions))
				return false;
			if (dimensions != 2)
				return false;

			float density;
			const String sdensity = String(path.subStringRef(dI+1,sI-dI-2));
			if (!convert(sdensity.c_str(),sdensity.length(),density))
				return false;
			const int idensity = (int) density;
			if (density != idensity)
				return false;
			//if (idensity%5)
			//	return false;
			return path.contains("m0.5_");	//half motion/sensor
		});
		#endif /*0*/

#if 0
		foreach (files,f)
		{
			tables.Append().LoadSamples(*f);
		}
		//currentSampleType = SampleType::LocationError;
		ExportPlot(SampleType::Inconsistency);
		ExportPlot(SampleType::DivergenceDepth);
		ExportPlot(SampleType::LocationError);
		return 0;
#endif
		
#if 0

		foreach (files,f)
		{
			std::cout << String(f->GetLocation())<<". ";
			tables.Clear();
			if (!tables.Append().LoadSamples(*f,[](const Table::TMetrics&m)->bool
			{
				return m.motionProfile == Table::TMetrics::Binary 
					&& !m.entityDeathAllowed 
					&& !m.unaffectedRandomMotion 
					&& int(m.entityDensity) == m.entityDensity
					//&& int(m.relativeMovementRange*8) == m.relativeMovementRange*8
					;
			}))
			{
				std::cout << "Skipping..."<<std::endl;
				continue;	//don't have all densities across all dimensions
			}
			const Table&t = tables.Last();
			std::cout << "Processing. ";
			Table hypothesis(false,true);
			hypothesis.TrainHypothesis(0,SampleType::DivergenceDepth);
			std::cout << hypothesis.hypothesis.age.linearPlaneFunc.ToString()<<std::endl;
			ResultTable::TCell&r = ResultTable::AllocateCell(t.metrics.entityDensity, t.metrics.relativeMovementRange);
			r.linearPlaneFunc[t.metrics.dimensions-1] = hypothesis.hypothesis.age.linearPlaneFunc;
			r.isSet[t.metrics.dimensions-1] = true;
		}
		

		{


			//(X^T X)^-1 X^T y
			//csv << "Dimensions,Axis, C0,C1,C2,C3"<<nl;
			//static const count_t Degree = 6;//0.01
			static const count_t Degree = 6;


			for (index_t r = 0; r < ResultTable::relativeMovementRanges.stops.Count(); r++)
			{
				if (ResultTable::CountDensityEntriesForRelativeMovementRange(r,2) <= Degree)
				{
					ResultTable::EraseRelativeMovementRange(r);
					r--;
				}

			}

			typedef TStaticAgeHypothesis::PFloat PFloat;
			auto&pBase = THypothesisData::staticAgeHypothesis.pBase;
			auto&pX = THypothesisData::staticAgeHypothesis.pX;

			//Array2D<NegativePolynomial<double,Degree+1>	> polynomial(ResultTable::relativeMovementRanges.stops.Count(),2);

			//static const count_t numCoef = count_t(degree+1);

			//TVec<double,numCoef>	results[3*2];

			for (int axis = 0; axis < 3; axis++)
				if (axis != 1)	//y axis seems to be fixed at 1
				{
					const count_t numEntries = ResultTable::CountDensityEntries(2);

					Array<TVec3<PFloat> > yv(numEntries);
					index_t at = 0;
					for (index_t r = 0; r < ResultTable::relativeMovementRanges.stops.Count(); r++)
					for (index_t d = 0; d < ResultTable::densities.stops.Count(); d++)
					{
						const auto&cell = ResultTable::resultCells.Get(d,r);
						if (cell.isSet[2])
						{
							yv[at].x = ResultTable::densities.stops[d].at;
							yv[at].y = 1.f - ResultTable::relativeMovementRanges.stops[r].at;	//must invert these
							yv[at].z = cell.linearPlaneFunc[2].v[axis];
							at++;
						}
					}
					ASSERT_EQUAL__(at,numEntries);

					bool success = axis!=0 ? pBase.Approximate(yv) : pX.Approximate(yv);
					ASSERT__(success);
				}
			std::cout << "Writing results to csv..."<<std::endl;
			StringFile csv;
			csv.Create("planes.csv");
			if (csv.IsActive())
			{

				PrintTableToCSV(csv, "b",pBase.coefficients);
				PrintTableToCSV(csv, "x", pX.coefficients);
				csv.Close();
			}
			else
				std::cout << "Failed to create planes.csv"<<std::endl;

			#if 1
				tables.Clear();
				Table&t = tables.Append();
				count_t resolution = 128;
				float maxRelativeError = 0;
				float maxAbsoluteError = 0;
				float maxAbsoluteValue = 0;
				for (index_t d = 0; d < ResultTable::densities.stops.Count(); d++)
				{
					//static const index_t Axis = 0;
					auto&row = t.samples.Append();
					const count_t rRange = ResultTable::relativeMovementRanges.stops.Count();
					for (index_t r = rRange-1; r < rRange; r--)
					{
						auto&sample = row.Append();
						const double p = pBase(ResultTable::densities.stops[d].at,1.f - ResultTable::relativeMovementRanges.stops[r].at);
						const double t = ResultTable::resultCells.Get(d,r).linearPlaneFunc[2].v[2];
						sample.numEntities = 10000;
						sample.inconsistencyAge = vmax(0.0,-p * sample.numEntities);
						sample.locationError = -t * sample.numEntities;
						if (ResultTable::resultCells.Get(d,r).isSet[2])
						{
							maxAbsoluteValue = vmax(maxAbsoluteValue,fabs(t));
							float error = fabs(t - p);
							maxAbsoluteError = vmax(maxAbsoluteError,error);
							maxRelativeError = vmax(maxRelativeError,error/vmax(fabs(t),0.1));
						}
						bool brk = true;
					}
				}
				std::cout << pBase.NumCoefficientsX << ","<<pBase.NumCoefficientsY<<","<<maxAbsoluteError/maxAbsoluteValue<<","<<maxRelativeError<<std::endl;
				bool brk = false;
			#endif /*0*/
		}
		//csv << "Density";
		//for (int i = 1; i <= 3; i++)
		//{
		//	csv<<",D"<<i<<"_Px";
		//	csv<<",D"<<i<<"_Py";
		//	csv<<",D"<<i<<"_Pb";
		//}
		//csv << nl;
		//foreach (resultRows,row)
		//{
		//	csv << row->density;
		//	for (int i = 0; i < 3; i++)
		//		for (int v = 0; v < 3; v++)
		//			csv << ',' << row->linearPlaneFunc[i].v[v];
		//	csv << nl;
		//}

		//__debugbreak();
		trainStatic = true;
#endif


		tables.Clear();

		foreach (files,f)
			::sources.Append().Setup(*f);
		


		if (false)
		{
			::sources.Clear();
			Sorting::ByComparator::QuickSort(files,[](const FileSystem::File&a, const FileSystem::File&b)->int
			{
				float mA = GetMotionProportion(String(a.GetName()));
				float mB = GetMotionProportion(String(b.GetName()));
				if (mA < mB)
					return -1;
				if (mA > mB)
					return 1;
				return 0;
			});


			Table&accum = tables.Append();
			//accum.samples.Append();
			foreach (files,f)
			{
				Table t;
				t.LoadSamples(*f);
				accum.samples.MoveAppend(t.samples.Last());
			}

			range.x.max = accum.samples.Count();
			markerSteps.x = float(accum.samples.Count())/4;
			markerLabelFactor.x = 1.f / accum.samples.Count();
			arrowLabel.x = "M";
		}


		//LoadTables(files);



		Engine::GlobalAspectConfiguration::worldZIsUp = true;

		display.config.fsaa_samples = 16;
		display.config.vertical_sync = 1;
		display.config.alpha_buffer_bits = 8;

		Engine::CreateDisplay(display, "./config", "3d Plot", Resolution(1000,1000), OnResize);
		glClearColor(1,1,1,0);
		
		
		//Mat::makeAxisSystem(Vector<>::zero, light_direction0,2,shadow_aspect0.view);
		//Mat::invertSystem(shadow_aspect0.view,shadow_aspect0.viewInvert);
		//Mat::makeAxisSystem(Vector<>::zero, light_direction1,2,shadow_aspect1.view);
		//Mat::invertSystem(shadow_aspect1.view,shadow_aspect1.viewInvert);
		
		

		glEnable(GL_DEPTH_TEST);	//enable depth testing for all following render operations (also all in loop unless stated otherwise)
		glEnable(GL_CULL_FACE);		//enable backface culling for all following render operations

		#if SYSTEM==WINDOWS
			textout.GetFont().LoadFromFile("../Font/font.xml");// display.getDC(), "Arial", 32, BoldStyle);	//create default font
		#else
			textout.getFont().make(0);	//create default font
		#endif
		textout.SetScale(fontScale/display.GetClientWidth(),fontScale/display.GetClientHeight());	//set texture scale to the inverse pixel width and height


		camera.UpdateProjection(display.GetPixelAspect(),	//create the perspective camera projection matrix using the current window pixel_aspect
					0.1,				//0.1 as near z plane
					100,				//100 as far zplane
					45);				//45 degrees as vertical field of view
		camera.angle.z = 220.f;
		camera.angle.x = 35.f;

		//camera.angle.z = 55.f;
		//camera.angle.x = 0.f;

		camera.retraction.z = -3.6;
		camera.UpdateView();

		hud.UpdateProjection(	0,0,		//create the internal overlay (hud) projection matrix using 0,0 as lower left corner
					1,1,		//1,1 as upper right corner
					-1,			//-1 as near z plane
					1);			//1 as far z plane
		hud.UpdateView();			//rebuild internal modelview matrix

		input.Bind(Key::Up,ZoomIn);			//bind the up-arrow key to the zoomIn event handler
		input.Bind(Key::Down,ZoomOut);		//bind the down-arrow key to the zoomOut event handler
		mouse.BindWheel(wheelAction);		//bind the mouse wheel to the wheelAction event handler
		input.Bind(Key::M,IterateSampleMode);
		input.Bind(Key::S,MakeScreenshot);
		input.Bind(Key::P,ExportExtremePlot);

		
		SetupRenderer();

		Profiler::TextoutManager<GLTextureFont2>::Set(&textout);	//set textout pointer to profiler font object so we don't have to create a second one
		
		fps_graph.position.Set(0.65,0.6,1,1);	//local graph
		fps_graph.install();					//and install graph
		
		//for (index_t i = 0; i < 9; i++)
		//{
		//	float fi = range.x.Derelativate( float(i)/9 );
		//	hypotheses << Hypothesis((index_t)round(fi));
		//}

		LoadFile(0);


		display.Assign(loop);		//assign loop as next frame target
		display.Execute();			//and execute. this method returns if the frame function returned false or the window has been closed
		display.Terminate();		//clean up

	}
	catch (const std::exception&e)
	{
		ErrMessage(e.what());
	}

	application_shutting_down = true;
	return 0;				//and return ok
}


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	return main();
}
