#include <global_root.h>
#include "statistics.h"
#include "InconsistencyCoverage.h"
#include <string/converter.h>

#include <general/thread_system.h>
#include <global_string.h>
#include <io/string_file.h>
#include <io/xml.h>
#include <container/array2d.h>
#include <io/file_system.h>

#include "experiment.h"

#ifdef D3
	#define FILENAME "result.xml"
#else
	#define FILENAME "result2d.xml"
#endif

void LogMessage(const String&msg);



namespace Statistics
{
	bool		DoCompareEntityConsistency()
	{
		#ifdef D3
			return false;
		#else
			return true;
		#endif
	}


	namespace Details
	{
		struct TMetric
		{
			double	sum=0,
					squareSum = 0;
			count_t	sampleCount=0;

			void	Add(double v)
			{
				if (isnan(v))
					return;
				sum += v;
				squareSum += v*v;
				sampleCount++;
			}

			template <typename T>
			static bool	Get(const XML::Node&n, const String&name, T&val)
			{
				StringRef str;
				if (n.Query(name,str))
				{
					ASSERT__(convert(str.pointer(),str.GetLength(),val));
					return true;
				}
				return false;
			}

			void	Import(const XML::Node&n, const String&baseName, count_t totalRuns)
			{
				if (Get(n,baseName+"_sum",sum))
				{
					Get(n,baseName+"_squareSum",squareSum);
					sampleCount = totalRuns;
				}
				else
				{
					const XML::Node*c = n.Find(baseName);
					if (c)
					{
						Get(*c, "sum",sum);
						Get(*c, "squareSum",squareSum);
						Get(*c, "sampleCount",sampleCount);
					}
				}
			}

			void GetMeanDeviation(double&outMean, double&outDeviation)const
			{
				outMean = sum/double(sampleCount);
				double sqrMean = squareSum/double(sampleCount);
				outDeviation = sqrt(sqrMean - sqr(outMean));
			}

			void	Export(XML::Node&n, const String&baseName) const
			{
				XML::Node&c = n.AddChild(baseName);

				double mean,deviation;
				GetMeanDeviation(mean,deviation);



				c.Set("mean", String(mean,12));
				c.Set("deviation",String(deviation,12));

				c.Set("sum",String(sum,12));
				c.Set("squareSum",String(squareSum,12));
				c.Set("sampleCount",sampleCount);
			}
		};

		struct TRun
		{
			bool imported = false;

			TExperiment setup;

			TMetric	survived,
					deadNodes,	//this should actually be constant
					recoveringNodes,
					revivalDepth,
					recoveryOps,
					recoverCompletedSDS,
					syncTotalCopied,
					syncTotalMerged,
					syncPerfectMerges,
					syncICReducingMerges,
					syncImprovingMerges,
					syncFailedMerges;


			count_t	numRuns=0,
					endlessRuns=0
					;
		};

		Buffer0<TRun>	runs;
		index_t			currentRun=InvalidIndex;

		void AddRun(int reliabilityLevel, count_t recoveryOperations, count_t numLayers, count_t maxSiblingSyncOperations, count_t siblingSyncDelay=1)
		{
			auto&r = runs.Append();
			r.setup.recoveryIterations = (int)recoveryOperations;
			r.setup.reliabilityLevel = (int)reliabilityLevel;
			r.setup.numLayers = (int)numLayers;
			r.setup.maxSiblingSyncOperations = numLayers > 1 ? (int)maxSiblingSyncOperations : 0;
			r.setup.siblingSyncDelay = numLayers > 1 ? (int)siblingSyncDelay : 0;

			#ifndef D3
				#ifndef _DEBUG	
					//r.setup.numEntities = 16*16*4*2*2*8*8;	//16x16 grid, 8x8 R per SD, 4x4 to get to sensor range, x4 => each entity sees 16 others on average
					//r.setup.numEntities = 16*4*256;
					r.setup.numEntities = 16*16*1*1*2*8*8;		//each sees on average 2 other
					//r.setup.numEntities = 16*16*1*2*2*8*8;		//each sees on average 4 other
					//r.setup.numEntities = 16*16*2*2*2*8*8;	//each sees on average _8_ others
				#else
					r.setup.numEntities = 256;
				#endif
			#endif
		}
		
		struct AtomicDouble
		{
			std::atomic<UINT64>	storage;

			/**/		AtomicDouble(double init=0)
			{
				storage = *(const UINT64*)&init;
			}

			void		operator=(double d)
			{
				storage = *(const UINT64*)&d;
			}

			operator double() const
			{
				UINT64 val = storage;
				return *(const double*)&val;
			}

			void	operator+=(double d)
			{
				for (;;)
				{
					double current = operator double();
					if (Replace(current,current+d))
						return;
				}
			}

			bool	Replace(double expected, double replacement)
			{
				UINT64 exp = *(const UINT64*)&expected;
				UINT64 repl = *(const UINT64*)&replacement;
				return storage.compare_exchange_weak(exp,repl);
			}

		};

		


		struct Entry
		{
			std::atomic<count_t>	inconsistentEntities=0,totalEntities=0,samples=0;
			

			void		Include(count_t inconsistentEntities, count_t totalEntities)
			{
				this->inconsistentEntities += inconsistentEntities;
				this->totalEntities += totalEntities;
				this->samples++;
			}

			float		GetMean() const
			{
				if (!totalEntities)
					return 0.f;
				return float(inconsistentEntities) / totalEntities;
			}

			void	ImportMean(double v)
			{
				totalEntities = (count_t)10000;
				inconsistentEntities = count_t(10000 * v);
			}

			static void Decode(const StringRef&str, std::atomic<count_t>&rs)
			{
				count_t val;
				ASSERT1__(convert(str.pointer(),str.GetLength(),val),str);
				rs = val;
			}

			void	Decode(const StringRef&tot, const StringRef&inc, const StringRef&sam)
			{
				Decode(tot,totalEntities);
				Decode(inc, inconsistentEntities);
				Decode(sam,samples);
				ASSERT_LESS_OR_EQUAL__(inconsistentEntities,totalEntities);
			}

		};

		Array2D<Entry>	entries(0x100,0x100);


		struct TExportCategory
		{
			int	syncOps=0;
			int numLayers=0;

			/**/			TExportCategory()	{}
			/**/			TExportCategory(const TExperiment&ex):syncOps(ex.maxSiblingSyncOperations),numLayers(ex.numLayers)	{}

			friend String	ToString(const TExportCategory&cat)
			{
				if (cat.numLayers == 1)
					return "No Replication";
				String rs = String(cat.numLayers-1)+ "x Repl.";
				if (cat.syncOps == 0)
					rs += ", Passive DB";
				elif (cat.syncOps == 1)
					rs += ", DB+Merge";
				else
					rs += ", x"+String(cat.syncOps)+" Merge Synced";
				return rs;
			}

			bool	operator==(const TExportCategory&ex) const
			{
				return ex.syncOps == syncOps && ex.numLayers == numLayers;
			}
			bool	operator!=(const TExportCategory&ex) const
			{
				return !operator==(ex);
			}
		};

		void ExportCSV(StringFile&csv,const TExportCategory&cat, const char*name, const std::function<String(const TRun&)>&f)
		{
			csv << cat<<'_'<<name;
			foreach (runs,r)
				if (cat == r->setup && r->endlessRuns < r->numRuns / 10)
					csv << tab << f(*r);
			csv << nl;
		}

		void ExportTexMeasurement(const char*name, const std::function<double(const TRun&)>&f, const char*ymode="log")
		{
			try
			{
				PathString outPath = "tex";
				FileSystem::CreateDirectory(outPath);
				StringFile texFile;
				String	filename(name);
				filename.replace(' ','_');
				texFile.Create(outPath+PathString("/"+filename+".tex"));

				texFile 
					<< "\\begin{axis}["<<nl
					<< "width=0.8\\linewidth,"<<nl
					<< "xmode = log,"<<nl
					<< "ymode = "<<ymode<<","<<nl
					//<< "x=-1cm,"<<nl
					<< "xlabel=\\emph{Error Probability},"<<nl
					<< "cycle list name=plotcolorlist,"<<nl
					<< "legend style={at={(0.6,0.77)},anchor=west},"<<nl
					<< "ylabel=\\emph{"<<name<<"}]"<<nl;
			
				Buffer0<TExportCategory> settings;
				{
					foreach (runs,r)
					{
						if ((settings.IsEmpty() || settings.Last() != r->setup) && !settings.Contains(r->setup))
						{
							settings << r->setup;
						}
					}
				}
				foreach (settings,s)
				{
					texFile
						<< "\\addplot+[mark=] plot coordinates {"<<nl;
					bool any = false;
					foreach (runs,r)
						if (*s == r->setup && r->endlessRuns < r->numRuns / 10)
						{
							double val = f(*r);
							if (!isnan(val))
							{
								texFile << "("<<(1.f - r->setup.GetReliability())<<","<<val<<")"<<nl;
								any = true;
							}
						}
					texFile
						<< "};"<<nl;
					if (any)
						texFile
						<< "\\addlegendentry{"<<*s<<"}"<<nl;
				}
				texFile << "\\end{axis}"<<nl;
			}
			catch (...)
			{}
		}


		void ExportStackedTexMeasurement(const TExportCategory&cat, std::initializer_list<const char*>names, std::initializer_list<std::function<double(const TRun&)> >f, const char*ymode="log")
		{
			try
			{
				PathString outPath = "tex";
				FileSystem::CreateDirectory(outPath);
				StringFile texFile;
				String	filename(ToString(cat));
				filename.replace(' ','_');
				filename.replace(',','_');
				filename.replace('+','-');
				filename.replace('.','_');
				texFile.Create(outPath+PathString("/"+filename+".tex"));

				texFile 
					<< "\\begin{axis}["<<nl
					<< "width=0.8\\linewidth,"<<nl
					<< "xmode = log,"<<nl;
				if (ymode)
					texFile << "ymode = "<<ymode<<","<<nl;
				texFile 
					//<< "x=-1cm,"<<nl
					<< "xlabel=\\emph{Error Probability},"<<nl
					<< "cycle list name=plotcolorlist,"<<nl
					<< "every axis plot/.append style={fill,fill opacity=0.3},"<<nl
					<< "legend style={at={(0.6,0.77)},anchor=west},"<<nl
					<< "ylabel=\\emph{"<<ToString(cat)<<"}]"<<nl;
			

				ASSERT_EQUAL__(names.size(),f.size());
				for (index_t i = 0; i < names.size(); i++)
				{
					auto func = *(f.begin()+i);
					texFile
						<< "\\addplot+[mark=] plot coordinates {"<<nl;
					bool any = false;
					foreach (runs,r)
						if (cat == r->setup && r->endlessRuns < r->numRuns / 10)
						{
							double val = func(*r);
							if (!isnan(val))
							{
								texFile << "("<<(1.f - r->setup.GetReliability())<<","<<val<<")"<<nl;
								any = true;
							}
						}
					if (i+1 < names.size())
					{
						auto func2 = *(f.begin()+i+1);
						for (index_t k = runs.Count()-1; k < runs.Count(); k--)
						{
							auto r = runs+k;
							if (cat == r->setup && r->endlessRuns < r->numRuns / 10)
							{
								double val = func2(*r);
								if (!isnan(val))
								{
									texFile << "("<<(1.f - r->setup.GetReliability())<<","<<val<<")"<<nl;
									any = true;
								}
							}
						}
					}
					texFile
						<< "}\\closedcycle;"<<nl;
					if (any)
						texFile
						<< "\\addlegendentry{"<<*(names.begin()+i)<<"}"<<nl;
				}
				texFile << "\\end{axis}"<<nl;
			}
			catch (...)
			{}
		}

		void AccumulateSyncStats(const Simulation&sim)
		{


			Shard::TMergeStatistics stat;
			sim.Accumulate(stat);
			const double factor = 1.0 / (sim.GetTopGeneration() * runs[currentRun].setup.maxSiblingSyncOperations * sim.CountSingleLayerShards() );
			runs[currentRun].syncTotalCopied.Add(factor * (stat.copyInCounter + stat.copyOutCounter));
			runs[currentRun].syncTotalMerged.Add(factor * stat.totalMerges);

			const double factor2 = 1.0 / stat.totalMerges;
			runs[currentRun].syncFailedMerges.Add(factor2 * stat.failedMerges);
			runs[currentRun].syncICReducingMerges.Add(factor2 * stat.icSizeReducingMerges);
			runs[currentRun].syncImprovingMerges.Add(factor2 * stat.improvingMerges);
			runs[currentRun].syncPerfectMerges.Add(factor2 * stat.perfectMerges);

		}

		namespace MergeMeasurement
		{
			Sys::SpinLock mergeLock;

			TStateDifference		betterPreMerge,
									worsePreMerge;
									

			struct TMergeCapture
			{
				TStateDifference	postMerge;
			};

			StringTable<TMergeCapture>	mergeCaptures;
			GenericHashTable<TICReductionConfig,TProbabilisticICReduction>	icReductionCaptures;
		}
		TExperiment Next()
		{

			XML::Container doc;
			if (currentRun != InvalidIndex)
				ExportMergeResults(runs[currentRun].setup);

			{
				currentRun = (currentRun+1)%runs.Count();
			}
			//while (currentRun != 0 && ( runs[currentRun].setup.maxSiblingSyncOperations == 0 || runs[currentRun].setup.numLayers == 1));	//skip for now
			if (!runs[currentRun].imported)
			{
				runs[currentRun].imported = true;
				ImportMergeResults(runs[currentRun].setup);
			}


			doc.Clear();
			foreach (runs,r)
			{
				auto&n = doc.root_node.AddChild("run");

				n.Set("reliability",r->setup.GetReliability());

				Array<std::pair<const char*,int> > desc;
				r->setup.Describe(desc);
				foreach (desc,d)
					n.Set(d->first,d->second);



				n.Set("endlessRuns",r->endlessRuns);

				n.Set("numRuns",r->numRuns);
				r->deadNodes.Export(n,"deadNodes");
				r->recoveringNodes.Export(n,"recoveringNodes");
				r->revivalDepth.Export(n,"revivalDepth");
				r->survived.Export(n,"survived");
				r->recoveryOps.Export(n,"recoveryOps");
				r->recoverCompletedSDS.Export(n,"recoverCompletedSDS");

				#undef EXPORT
				#define EXPORT(X) r->X.Export(n,#X);


				EXPORT(syncTotalCopied);
				EXPORT(syncTotalMerged);
				EXPORT(syncFailedMerges);
				EXPORT(syncImprovingMerges);
				EXPORT(syncICReducingMerges);
				EXPORT(syncPerfectMerges);

			}

			doc.SaveToFile(FILENAME);


			ExportTexMeasurement("Time-Steps Survived", [](const TRun&r)
					{
						//if (r.setup.maxSiblingSyncOperations > 0)
							//return (double)NAN;
						double mean,dev;
						auto surv = r.survived;
						for (index_t i = 0; i < r.endlessRuns; i++)
							surv.Add(10000);
						r.survived.GetMeanDeviation(mean,dev);
						return mean;
					});

			ExportTexMeasurement("Total_Merges", [](const TRun&r)
					{
						//if (r.setup.maxSiblingSyncOperations > 0)
							//return (double)NAN;
						double mean,dev;
						r.syncTotalMerged.GetMeanDeviation(mean,dev);
						return mean;
					});

			TExportCategory cat;
			cat.numLayers = 2;
			cat.syncOps = 1;
			ExportStackedTexMeasurement(cat,{"Total Merges","IC-Reduction","Perfect"},{
			
				[](const TRun&r)
				{
					//if (r.setup.maxSiblingSyncOperations > 0)
						//return (double)NAN;
					double mean,dev;
					r.syncTotalMerged.GetMeanDeviation(mean,dev);
					return mean*100;
				},

				[](const TRun&r)
					{
						//if (r.setup.maxSiblingSyncOperations > 0)
							//return (double)NAN;
						double mean,dev,m0,d0,m1,d1;
						r.syncTotalMerged.GetMeanDeviation(m0,d0);
						r.syncPerfectMerges.GetMeanDeviation(m1,d1);
						r.syncICReducingMerges.GetMeanDeviation(mean,dev);
						return (mean+m1)*m0*100;//stacked
					},

				[](const TRun&r)
					{
						//if (r.setup.maxSiblingSyncOperations > 0)
							//return (double)NAN;
						double mean,dev,m0,d0;
						r.syncTotalMerged.GetMeanDeviation(m0,d0);
						r.syncPerfectMerges.GetMeanDeviation(mean,dev);
						return mean*m0*100;
					}
			},nullptr);

			//ExportTexMeasurement("Recovering Nodes", [](const TRun&r)
			//		{
			//			double mean,dev;
			//			r.recoveringNodes.GetMeanDeviation(mean,dev);
			//			return mean;
			//		});


			try
			{
				StringFile csv;
				csv.Create(FILENAME ".csv");



				Buffer0<TExportCategory> settings;
				{
					foreach (runs,r)
					{
						if ((settings.IsEmpty() || settings.Last() != r->setup) && !settings.Contains(r->setup))
						{
							settings << r->setup;
						}
					}
				}
				foreach (settings,s)
				{
					ExportCSV(csv, *s,"Survived",[](const TRun&r)
					{
						double mean,dev;
						r.survived.GetMeanDeviation(mean,dev);
						return mean;
					});

					ExportCSV(csv, *s,"MergeOps",[](const TRun&r)
					{
						double mean,dev;
						r.syncTotalMerged.GetMeanDeviation(mean,dev);
						return mean;
					});
					ExportCSV(csv, *s,"RecoveringNodes",[](const TRun&r)
					{
						double mean,dev;
						r.recoveringNodes.GetMeanDeviation(mean,dev);
						return mean;
					});
					ExportCSV(csv, *s,"RevivalDepth",[](const TRun&r)
					{
						double mean,dev;
						r.revivalDepth.GetMeanDeviation(mean,dev);
						return mean;
					});


				}
				csv.Close();
			}
			catch (...)
			{}

			return runs[currentRun].setup;
		}
	}


	



}

float	Statistics::GetMean(const IC::TSample& textureInconsistency)
{
	using namespace Details;
	return entries.Get(textureInconsistency.blurExtent,textureInconsistency.depth).GetMean();
}



void Statistics::Include(const IC::TSample& textureInconsistency, count_t inconsistentEntities, count_t totalEntities)
{
	using namespace Details;
	ASSERT_LESS_OR_EQUAL__(inconsistentEntities,totalEntities);
	//if (!totalEntities)
	//	return;
	entries.Get(textureInconsistency.blurExtent,textureInconsistency.depth).Include(inconsistentEntities,totalEntities);

}

void	Statistics::ExportToFile(const PathString&file)
{
	using namespace Details;
	try
	{
		StringFile f;
		f.Create(file);
		foreach (entries.Vertical(),v)
		{
			foreach(entries.Horizontal(), h)
			{
				const auto&e = entries.Get(h, v);
				f << tab << e.totalEntities << tab << e.inconsistentEntities << tab << e.samples;
			}
			f << nl;
		}
	}
	catch (const std::exception&ex)
	{
		//...
	}
}

void	Statistics::ImportMean(const PathString&file)
{
	using namespace Details;
	try
	{
		StringFile f;
		f.Open(file);
		index_t line = 0;
		String sline;
		Array<StringRef>	parts;
		while (f >> sline)
		{
			explode(tab,sline,parts);
			ASSERT_EQUAL__(parts.Count(),256*3+1);
			for (index_t i = 0; i*3+3 < parts.Count(); i ++)
			{
				entries.Get(i,line).Decode(parts[i * 3+1], parts[i * 3 + 2], parts[i * 3 + 3]);
			}

			line++;
		}
	}
	catch (...)
	{}

}



TExperiment	Statistics::SignalRecoveryFailed(const Simulation&sim)
{
	using namespace Details;
	runs[currentRun].survived.Add(sim.GetTopGeneration());
	runs[currentRun].deadNodes.Add(sim.GetRelativeNodeMortality());
	runs[currentRun].recoveringNodes.Add(sim.GetRelativeNodeRecovery());
	runs[currentRun].revivalDepth.Add(sim.GetAverageRevivalDepth());
	runs[currentRun].recoveryOps.Add(sim.GetAverageRecoveryOps());
	runs[currentRun].recoverCompletedSDS.Add(sim.GetAverageRecoverCompletedSDS());

	AccumulateSyncStats(sim);


	runs[currentRun].numRuns ++;

	return Next();
}

TExperiment Statistics::SignalSimulationEnd(const Simulation & sim)
{
	using namespace Details;
	runs[currentRun].endlessRuns++;;

	runs[currentRun].deadNodes.Add(sim.GetRelativeNodeMortality());
	runs[currentRun].recoveringNodes.Add(sim.GetRelativeNodeRecovery());
	runs[currentRun].revivalDepth.Add(sim.GetAverageRevivalDepth());
	runs[currentRun].recoveryOps.Add(sim.GetAverageRecoveryOps());
	runs[currentRun].recoverCompletedSDS.Add(sim.GetAverageRecoverCompletedSDS());

	AccumulateSyncStats(sim);



	return Next();
}

template <typename T>
static void Get(const XML::Node&node, const char*attrib, T&v)
{
	StringRef val;
	ASSERT2__(node.Query(attrib,val),node.name,attrib);
	ASSERT3__(convert(val.pointer(),val.GetLength(),v),node.name,attrib,val);
}

template <typename T>
static bool GetOptional(const XML::Node&node, const char*attrib, T&v)
{
	StringRef val;
	if (!node.Query(attrib,val))
		return false;
	ASSERT3__(convert(val.pointer(),val.GetLength(),v),node.name,attrib,val);
	return true;
	//return convert(val.pointer(),val.GetLength(),v);
}

TExperiment Statistics::Begin()
{
	using namespace Details;

	//16 = 0.99, 20=0.9968, 24=0.999

	#ifdef D3
		for (int i = 16; i <= 30; i++)	//7=0.999
			for (int l = 1; l <= 2; l++)
			{
				count_t maxSync = l > 1 ? 1:0;
				for (index_t s = 0; s <= maxSync; s++)
				{
					AddRun(i,2,l,s);
					//AddRun(i,3,l,s);
					//AddRun(i,4,l,s);
					//AddRun(i,6,l,s);
					//AddRun(i,8,l,s);
					//AddRun(i,10,l,s);
					//AddRun(i,12,l,s);
				}
			}
	#else
			//for (int l = 1; l <= 2; l++)
			int l = 2;
			{
				//count_t maxSync = l > 1 ? 1:0;
				index_t s = 1;
				//for (index_t s = 0; s <= maxSync; s++)
				{
					AddRun(20,2,l,s);
					//AddRun(i,3,l,s);
					//AddRun(i,4,l,s);
					//AddRun(i,6,l,s);
					//AddRun(i,8,l,s);
					//AddRun(i,10,l,s);
					//AddRun(i,12,l,s);
				}
			}
	#endif

	try
	{
		XML::Container doc;
		doc.LoadFromFile(FILENAME);

		foreach (doc.root_node.children,c)
			if (c->name == "run")
			{
				TExperiment exp;

				Array<std::pair<const char*,int*> > desc;
				exp.Describe(desc);
				foreach (desc,d)
					GetOptional(*c,d->first,*d->second);

				bool found = false;
				foreach (runs,r)
				{
					if (r->setup == exp)
					{
						ASSERT__(!found);
						found = true;
						Get(*c,"endlessRuns",r->endlessRuns);
						Get(*c,"numRuns",r->numRuns);
						r->deadNodes.Import(*c,"deadNodes",r->numRuns);
						r->recoveringNodes.Import(*c,"recoveringNodes",r->numRuns);
						r->revivalDepth.Import(*c,"revivalDepth",r->numRuns);
						r->recoveryOps.Import(*c,"recoveryOps",r->numRuns);
						r->survived.Import(*c,"survived",r->numRuns);
						r->recoverCompletedSDS.Import(*c,"recoverCompletedSDS",r->numRuns);
						

						#undef IMPORT
						#define IMPORT(X) r->X.Import(*c,#X,r->numRuns);

						

						IMPORT(syncTotalCopied);
						IMPORT(syncFailedMerges);
						IMPORT(syncICReducingMerges);
						IMPORT(syncImprovingMerges);
						IMPORT(syncTotalMerged);
						IMPORT(syncPerfectMerges);

					}
				}
				if (!found)
				{
					StringBuffer nameBuffer;
					foreach (desc,d)
						nameBuffer << ' '<<d->first<<'='<<*d->second;

					LogMessage("Unable to find run:"+nameBuffer.CopyToString());
				}
			}
	}
	catch (...) {}
	


	return Next();
}



namespace Statistics
{


	void	CapturePreMerge(const TStateDifference&preMergeA, const TStateDifference&preMergeB)
	{
		using namespace Details::MergeMeasurement;
		mergeLock.lock();
			worsePreMerge += TStateDifference::Maximum(preMergeA,preMergeB);
			betterPreMerge += TStateDifference::Minimum(preMergeA,preMergeB);
		mergeLock.unlock();
	}

	String ToExt(MergeStrategy strategy)
	{
		switch (strategy)
		{
			case MergeStrategy::Exclusive:
				return "_Ex";
			case MergeStrategy::ExclusiveWithPositionCorrection:
				return "_ExPC";
		};
		return "";
	}

	void	CaptureICTest(const TProbabilisticICReduction&rs)
	{
		using namespace Details::MergeMeasurement;
		using namespace Details;
		mergeLock.lock();
			const auto& run = runs[currentRun];

			auto&entry = icReductionCaptures.Set(rs.config);
			entry.Add(rs);
		mergeLock.unlock();
	}


	void	TProbabilisticICReduction::Import(const XML::Node*n)
	{
		#undef IMPORT
		#define IMPORT(X)		X.Import(n->Find(#X));
		
		IMPORT(totalGuesses);
		IMPORT(consideredConsistent);
		IMPORT(correctGuesses);
		IMPORT(actuallyConsistent);
		IMPORT(shouldHaveConsideredConsistent);
		IMPORT(shouldNotHaveConsideredConsistent);
	}

	void	TProbabilisticICReduction::ToXML(XML::Node&n) const
	{
		#undef EXPORT
		#define EXPORT(X)		X.ToXML(n.AddChild(#X));

		EXPORT(totalGuesses);
		EXPORT(consideredConsistent);
		EXPORT(correctGuesses);
		EXPORT(actuallyConsistent);
		EXPORT(shouldHaveConsideredConsistent);
		EXPORT(shouldNotHaveConsideredConsistent);

		{
			XML::Node&xflags = n.Create("config");
			String sflags = "";
			if (config.flags & ICReductionFlags::RegardEntityEnvironment)
				sflags += " entityEnvironment";
			elif (config.flags & ICReductionFlags::RegardEntityState)
				sflags += " entityState";
			if (config.flags & ICReductionFlags::RegardOriginRange)
				sflags += " originRange";
			if (config.flags & ICReductionFlags::RegardFuzzyOriginRange)
				sflags += " fuzzyOriginRange";
			if (config.flags & ICReductionFlags::RegardOriginBitField)
				sflags += " originBits";
			xflags.inner_content = sflags;

			xflags.Set("minEntityPresence",config.minEntityPresence);
			xflags.Set("overlapTolerance",config.overlapTolerance);
		}
	}


	void	TProbabilisticICReduction::Add(const TProbabilisticICReduction&other)
	{
		config = other.config;
		totalGuesses += other.totalGuesses;
		consideredConsistent += other.consideredConsistent;
		correctGuesses += other.correctGuesses;
		actuallyConsistent += other.actuallyConsistent;
		shouldHaveConsideredConsistent += other.shouldHaveConsideredConsistent;
		shouldNotHaveConsideredConsistent += other.shouldNotHaveConsideredConsistent;
	}



	void CaptureMergeResult(const IC::Comparator&comp, MergeStrategy strategy,const TStateDifference&postMerge)
	{
		using namespace Details::MergeMeasurement;
	
		String ext = ToExt(strategy);
	
		mergeLock.lock();
			auto&entry = mergeCaptures.Set(String(comp.GetName())+ext);
			entry.postMerge.AddMean(postMerge);
		mergeLock.unlock();
	}

	//#define MERGE_FILENAME "mergeMeasurements.xml"
	//#define IC_FILENAME "icMeasurements.xml"


	PathString Filename(const TExperiment&ex, const PathString&coreName, const PathString&ext)
	{
		if (HashProcessGrid::RegardHistory)
			return "measurements/"+coreName+PathString(ex.numEntities)+"."+ext;
		return "measurements/"+coreName+PathString(ex.numEntities)+".NoHistoryState."+ext;
	}

	PathString MergeFilename(const TExperiment&ex)
	{
		return Filename(ex,"mergeMeasurements","xml");
	}

	PathString ICFilename(const TExperiment&ex)
	{
		return Filename(ex,"icMeasurements","xml");
	}



	template <typename T>
	static bool Query(const XML::Node*n, const String&key, T&outValue)
	{
		StringRef str;
		return n->Query(key,str) && Convert(str,outValue);
	}
	template <typename T>
	static bool QueryEnum(const XML::Node*n, const String&key, T&outValue)
	{
		typedef typename std::underlying_type<T>::type V;
		V v;
		if (!Query(n,key,v))
			return false;
		outValue = T(v);
		return true;
	}

	void ImportMergeResults(const TExperiment&ex)
	{
		using namespace Details;
		using namespace MergeMeasurement;
		mergeCaptures.Clear();
		icReductionCaptures.Clear();
		try
		{
			XML::Container doc;
			doc.LoadFromFile(MergeFilename(ex));

			betterPreMerge.Import(doc.root_node.Find("better"));
			worsePreMerge.Import(doc.root_node.Find("worse"));

			String key;
			foreach(doc.root_node.children,n)
				if (n->name == "capture" && n->Query("class",key))
				{
					mergeCaptures.Set(key).postMerge.Import(n);
				}
		}
		catch(...){};
	
		try
		{
			XML::Container doc;
			doc.LoadFromFile(ICFilename(ex));

			count_t numEntities;
			if (Query(&doc.root_node,"entities",numEntities))
				ASSERT_EQUAL__(numEntities,ex.numEntities);

			TICReductionConfig cfg;
			foreach(doc.root_node.children,n)
				if (n->name == "capture" 
					&& 
					QueryEnum(n,"flags",cfg.flags) && 
					Query(n,"overlapTolerance",cfg.overlapTolerance) &&
					Query(n,"minEntityPresence",cfg.minEntityPresence)
					)
				{
					if (cfg.CanCheck())
					{
						auto&ic = icReductionCaptures.Set(cfg);
						ic.config = cfg;
						ic.Import(n);
					}
				}
		}
		catch(...){};
	}


	double ToDisplay(double v)
	{
		int digits = (int)floor(log(fabs(v))/log(10.0)) -1;
		//if (fabs(v) > 1)
		//	digits--;
		double ex = pow(10.0,digits);
		return Round(v /ex) * ex;
	}

	struct ExportChannel
	{
		typedef std::function<TSDSample<double>(const TStateDifference&diff)> FFilter;
		const char*name="";
		FFilter f;
		const char* unit="";

		/**/		ExportChannel()	{};
		/**/		ExportChannel(const char*name, const FFilter&f):name(name),f(f)	{}
		/**/		ExportChannel(const char*name, const char*unit, const FFilter&f):name(name),f(f),unit(unit)	{}

		String		MakeName(const TStateDifference&source) const
		{
			return name;
			//double val = f(source).Get();
			//return String(name)+" \\("+String(ToDisplay(val))+unit+"\\)";
		}

	};

	static void ExportRangeToTex(StringFile&texFile, const Details::MergeMeasurement::TMergeCapture&cap, const ArrayData<ExportChannel>&channels)
	{
		using namespace Details::MergeMeasurement;

		for (index_t index = 0; index < channels.Count(); index++)
		{
			const auto&c = channels[index];
			const float rawMax = std::max(c.f(worsePreMerge).Get(),c.f(cap.postMerge).Get());
			const float max = rawMax*1.2f;
			float h0 = c.f(betterPreMerge).Get()/max;
			float h1 = c.f(worsePreMerge).Get()/max;

			//fill:
			texFile << "\\draw [draw=black, pattern=north west lines, pattern color=black!50] (axis cs:"<<h0<<","<<(index)<<".0) rectangle (axis cs:"<<h1<<","<<index<<".9);"<<nl;

			//describe max:
			texFile << "\\node[anchor=west,align=left] at (axis cs:0.8333,"<<index<<".5) {"<<ToDisplay(rawMax)<<c.unit<<"};"<<nl;

			//describe current:
			float current = c.f(cap.postMerge).Get();
			texFile << "\\node[anchor=west,fill=white,inner sep=0.5pt,align=left] at (axis cs:"<<(current/max+0.01f) <<","<<index<<".45) {"<<ToDisplay(current)<<c.unit<<"};"<<nl;


			//lower:
			texFile << "\\addplot+[draw=black,solid,name path=lower"<<index<<",mark=] plot coordinates {"<<nl
							<< "("<<h0<<","<<index<<".0"<<")"<<nl
							<< "("<<h0<<","<<index<<".9"<<")"<<nl
							<< "};"<<nl;

			//upper:
			texFile << "\\addplot+[draw=black,solid,name path=upper"<<index<<",mark=] plot coordinates {"<<nl
							<< "("<<h1<<","<<index<<".0)"<<nl
							<< "("<<h1<<","<<index<<".9)"<<nl
							<< "};"<<nl;
			//texFile << "\\addplot [thick,color=black,fill=black, fill opacity=0.05]"<<nl
			//		<< tab << "fill between[of=lower"<<index<<" and upper"<<index<<",soft clip={domain=0:100},];"<<nl;
		}
	}


	static void ExportTexFile(const String&name, const Details::MergeMeasurement::TMergeCapture&source)
	{
			try
		{
			using namespace Details::MergeMeasurement;

			StringFile texFile;
			texFile.Create(PathString("tex/"+name+".tex"));

			//areas:
			texFile << "\\begin{axis}["<<nl
					<< tab<< "width=\\linewidth,"<<nl
					<< tab<< "height=0.4\\linewidth,"<<nl
					<< tab<< "xmin=0,"<<nl
					<< tab<< "xmax=1.02,"<<nl
					<< tab<< "ticks=none,"<<nl
					<< tab<<"]"<<nl;

			Array<ExportChannel> channels =			{
				//ExportChannel("IC Vol",[](const TStateDifference&diff)
				//{
				//	return diff.icSize;
				//}),
				ExportChannel("$M$","\\%",[](const TStateDifference&diff)
				{
					return diff.missingEntities / diff.entitiesInInconsistentArea.Get() * 100;
				}),
				ExportChannel("$U$","\\%",[](const TStateDifference&diff)
				{
					return diff.overAccountedEntities / diff.entitiesInInconsistentArea.Get() * 100;
				}),
				ExportChannel("$P$","\\%", [](const TStateDifference&diff)
				{
					return diff.inconsistencyProbability * 100;
				}),
				ExportChannel("$\\Omega$","R",[](const TStateDifference&diff)
				{
					return diff.omega * (1.0/ Entity::MaxInfluenceRadius);
				}),
			};



			ExportRangeToTex(texFile,source ,channels);
			texFile << "\\end{axis}"<<nl
				<< "\\begin{axis}[width=\\linewidth,xmin=0,xmax=1.02,"<<nl
				<< tab<< "height=0.4\\linewidth,"<<nl
				<<tab<<"symbolic y coords={";
			foreach (channels, ch)
			{
				texFile << ch->MakeName(source.postMerge) << ',';
			}

			float enlarge = 0.8f / (channels.Count()-1);
				//0.4f;
				// == 3 ? 0.4f : 0.27f;
			float barWidth = 15/channels.Count();
			

			texFile << "},xticklabels={,,},ytick=data,"<<nl
				<< tab << "enlarge y limits="<<enlarge<<","<<nl
				<< tab << "every axis plot/.append style={fill},"<<nl
				//<< tab << "x tick label style={rotate=-45,anchor=west},"<<nl
				<< tab << "/pgf/bar width="<<barWidth<<"pt,"<<nl
				<< tab << "]"<<nl;
	
			texFile << "\\addplot+[xbar, color=black,mark=] coordinates {"<<nl;
			foreach (channels,ch)
			{
				const float current = ch->f(source.postMerge).Get();
				const float worse = ch->f(worsePreMerge).Get();
				const float max = std::max(worse,current) * 1.2f;
				const float v = current / max;
				texFile << tab<<"("<<v<<","<<ch->MakeName(source.postMerge)<<")"<<nl;
			}
			texFile << "};"<<nl;
			texFile << "\\end{axis}"<<nl;
		}
		catch (...)
		{}
	}

	static const Details::MergeMeasurement::TMergeCapture& ExportTexFile(const String&name)
	{
		const auto&data = Details::MergeMeasurement::mergeCaptures.Require(name);
		ExportTexFile(name,data);
		return data;
	}

	void ExportMergeResults()
	{
		using namespace Details;
		ExportMergeResults(runs[currentRun].setup);
	}

	void ExportMergeResults(const TExperiment&ex)
	{
		using namespace Details;
		{
			XML::Container doc;
			Array<String> keys;
			Array<MergeMeasurement::TMergeCapture> values;
			using namespace MergeMeasurement;

			betterPreMerge.ToXML(doc.root_node.Create("better"));
			worsePreMerge.ToXML(doc.root_node.Create("worse"));

			mergeCaptures.exportTo(keys,values);
			
			for (index_t i = 0; i < keys.Count(); i++)
			{
				XML::Node&node = doc.root_node.Create("capture");

				node.Set("class",keys[i]);
				values[i].postMerge.ToXML(node,betterPreMerge,worsePreMerge);
			}

			doc.SaveToFile(MergeFilename(ex));
		}
		{
			XML::Container doc;
			Array<TProbabilisticICReduction> values;
			using namespace MergeMeasurement;

			icReductionCaptures.exportTo(values);


			if (currentRun < runs.Count())
			{
				auto currentCfg = runs[currentRun];

				doc.root_node.Set("entities",currentCfg.setup.numEntities);
			}
			
			for (index_t i = 0; i < values.Count(); i++)
			{
				const auto&v = values[i];
				XML::Node&node = doc.root_node.Create("capture");

				const double truePositive = (v.consideredConsistent.Get() - values[i].shouldNotHaveConsideredConsistent.Get());
				const double falsePositive = v.shouldNotHaveConsideredConsistent.Get();
				const double falseNegative = v.shouldHaveConsideredConsistent.Get();
				const double trueNegative = ( (v.totalGuesses.Get() - v.consideredConsistent.Get()) - v.shouldHaveConsideredConsistent.Get());
				const double efficiency = truePositive / (values[i].actuallyConsistent.Get());
				const double precision = /*rp / (rp + fp)*/ truePositive / (truePositive + falsePositive);
				const double recall = /*rp / (rp + fn)*/ truePositive / (truePositive + falseNegative);
				const double sensitivity = recall;
				const double specificity = trueNegative / (trueNegative + falsePositive);
				const double fallout = falsePositive / (falsePositive+trueNegative);
				node.Set("sensitivity",  sensitivity );
				node.Set("specificity",  specificity );
				//node.Set("fallout",fallout);
				//node.Set("F0", truePositive / (truePositive + falsePositive));
				node.Set("imformedness", sensitivity + specificity - 1);
				//node.Set("MCC", (truePositive * trueNegative - falsePositive * falseNegative) /
				//	sqrt((truePositive + falsePositive)*(truePositive + falseNegative)*(trueNegative + falsePositive)*(trueNegative+falseNegative)  )
				//	);
				//node.Set("F1-score", 2.0 * precision * recall / (precision + recall));
				//node.Set("falsePositives", falsePositive / values[i].totalGuesses.Get()  );
				//node.Set("correctGuesses", values[i].correctGuesses.Get() / values[i].totalGuesses.Get()  );

				node.Set("flags",index_t(values[i].config.flags));
				node.Set("overlapTolerance",values[i].config.overlapTolerance);
				node.Set("minEntityPresence",values[i].config.minEntityPresence);
				values[i].ToXML(node);
			}

			doc.SaveToFile(ICFilename(ex));

			try
			{
				StringFile csv;
				csv.Create(Filename(ex,"icTable","csv"));

				
				Array2D<String>	table(7*3+3,3);
				index_t at = 3;
				table.Get(at,0) = "zero"; at+=3;
				table.Get(at,0) = "state"; at+=3;
				table.Get(at,0) = "state_env"; at+=3;
				table.Get(at,0) = "originBits"; at+=3;
				table.Get(at,0) = "originBits_env"; at+=3;
				table.Get(at,0) = "originR"; at+=3;
				table.Get(at,0) = "originR_env"; at+=3;
				for (index_t i = 3; i < table.GetWidth(); i+= 3)
				{
					table.Get(i,1) = table.Get(i,0)+" effect.";
					table.Get(i+1,1) = table.Get(i,0)+" false pos.";
					table.Get(i+2,1) = table.Get(i,0)+" score";
				}
				table.Get(0,2) = ex.numEntities;
				table.Get(2,1) = "consistentPercentage";
				table.Get(1,1) = "can see";
				if (values.IsNotEmpty())
					table.Get(2,2) = values.First().actuallyConsistent.Get() / values.First().totalGuesses.Get();
				table.Get(1,2) = double(ex.numEntities) / (16*16)/ sqr(1.0 / (2*Entity::MaxAdvertisementRadius));

				foreach (values,val)
				{
					if (!(val->config.flags & ICReductionFlags::RegardEntityState))
					{
						if (val->config.flags & ICReductionFlags::RegardOriginBitField)
							continue;
						if (val->config.flags & ICReductionFlags::RegardOriginRange)
							continue;
						if (val->config.flags & ICReductionFlags::RegardFuzzyOriginRange)
							continue;
					}
					if (val->config.minEntityPresence > 0 || val->config.overlapTolerance > 0)
						continue;
					if (val->config.flags & ICReductionFlags::RegardFuzzyOriginRange)
						continue;
					const double positive = (val->consideredConsistent.Get() - val->shouldNotHaveConsideredConsistent.Get());
					const double effectivity = positive / (val->actuallyConsistent.Get());
					const double falsePositives = val->shouldNotHaveConsideredConsistent.Get() / (val->totalGuesses.Get()  -val->actuallyConsistent.Get()) ;
//					const float correctGuesses = val->correctGuesses.Get() / val->totalGuesses.Get();

					index_t x = 0;
					if (val->config.flags & ICReductionFlags::RegardEntityState)
						x++;
					if (val->config.flags & ICReductionFlags::RegardOriginBitField)
						x += 2;
					elif (val->config.flags & ICReductionFlags::RegardOriginRange)
						x += 4;
					if (val->config.flags & ICReductionFlags::RegardEntityEnvironment)
						x++;
					x *= 3;
					x += 3;

					index_t y = 2;
					ASSERT__(table.Get(x,y).IsEmpty());
					ASSERT_LESS__(x+2,table.GetWidth());
					ASSERT_LESS__(y,table.GetHeight());
					table.Get(x,y) = String(effectivity,10);
					table.Get(x+1,y) = String(falsePositives,10);
					const double div = val->shouldNotHaveConsideredConsistent.GetPlusOne() / (val->totalGuesses.Get()  -val->actuallyConsistent.Get()) ;
					table.Get(x+2,y) = String(effectivity / div,10);
				}

				for (index_t y = 1; y < table.GetHeight(); y++)
				{
					for (index_t x = 0; x < table.GetWidth(); x++)
						csv << table.Get(x,y)<<',';
					csv << nl;
				}

			}
			catch (...)	{}

		}


		using namespace MergeMeasurement;
		if (mergeCaptures.IsNotEmpty())
		{
			for (int i = 0; i < (int)MergeStrategy::Count; i++)
			{
				ExportTexFile("Binary"+ToExt((MergeStrategy)i));
			}
		}
	}


	bool	TICReductionConfig::CanCheck() const
	{
		count_t set = 0;
		bool canOverlap = false;

		if (!(flags & ICReductionFlags::RegardEntityState))
		{
			if (flags & ICReductionFlags::RegardEntityEnvironment)
				return false;
			if (minEntityPresence != 0)
				return false;
		}

		if (flags & ICReductionFlags::RegardOriginRange)
		{
			canOverlap = true;
			set++;
		}
		if (flags & ICReductionFlags::RegardFuzzyOriginRange)
		{
			canOverlap = true;
			set++;
		}
		
		if (flags & ICReductionFlags::RegardOriginBitField)
			set++;
		
		if (set > 1)
			return false;
		if (!canOverlap && overlapTolerance > 0)
			return false;

		return set <= 1;
		//return true;
		//return !(p & ICReductionFlags::RegardEnvironment) || (p & ICReductionFlags::RegardHistory);	//only have environment if history is available
	}

}

