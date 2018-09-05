using MeasurementMerger;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TexUpdater
{
	class Program
	{

		static void Main(string[] args)
		{
			var input = new DirectoryInfo("input");
			if (!input.Exists)
				throw new ArgumentException("No input directory found");
			var files = input.GetFiles("mergeMeasurements*.xml");
			if (files.Length != 1)
				throw new ArgumentException("input directory does not contain exactly one xml file, but " + files.Length);

			MetricTable table = new MetricTable(files[0]);

			var better = table.ExtractPair("better");
			var worse = table.ExtractPair("worse");
			var average = table.ExtractPair("average");
			var general = table.ExtractPair("general");

			var captures = table.ExtractCapturePairs();

			if (!Directory.Exists("output"))
				Directory.CreateDirectory("output");
			else
			{
				var _f = Directory.GetFiles("output");
				foreach (var f in _f)
				{
					Console.WriteLine("Erasing old output " + f + "...");
					File.Delete(f);
				}
			}

			{
				using (StreamWriter file = new StreamWriter(@"output\statistics.tex"))
				{
					file.WriteLine(@"\begin{tabular}{lllll}");
					file.WriteLine(@"& Operational Mean & $\leftarrow \cap IS_M$ & Population Mean & $\leftarrow \cap IS_M$\\");
					{
						var data = Array(general, average);
						WriteProperty(file, @"\Omega", "I_Omega", data, 8.0, "R");
						WriteProperty(file, @"\Delta p(e)", "C_SpatialDelta", data, 8.0, "R");
						WriteProperty(file, @"P_I", "C_InconsistencyProbability", data, 100.0, @"\%");
						WriteProperty(file, @"P_M", "C_MissingProbability", data, 100.0, @"\%");
						WriteProperty(file, @"P_U", "I_UnwantedProbability", data, 100.0, @"\%");
					}
					file.WriteLine(@"\end{tabular}");
				}
			}
			{
				using (StreamWriter file = new StreamWriter(@"output\ic_statistics.tex"))
				{
					file.WriteLine(@"\begin{tabular}{ll}");
					file.WriteLine(@"& Mean\\");
					//file.WriteLine(@"$\left| IS \right| \over \left| SD \right|$ &  $\left| IS_M \right| \over \left| SD \right|$  \\");
					WriteProperty(file, @"\left| IS \right| \over \left| SD \right|", "ICSize", Array(general), 100.0 / 64, @"\%",true,false);
					WriteProperty(file, @"\left| IS_M \right| \over \left| SD \right|", "ICSize", Array(captures[0]), 100.0 / 64, @"\%",true,false);
					file.WriteLine(@"\end{tabular}");
				}
			}
			{
				using (StreamWriter file = new StreamWriter(@"output\better_worse_statistics.tex"))
				{
					file.WriteLine(@"\begin{tabular}{lllll}");
					file.WriteLine(@"& Mean Better(O) & Mean Worse(O) & Mean Better(P) & Mean Worse(P) \\");
					{
						var data = Array(better, worse);
						WriteProperty(file, @"\Omega", "I_Omega", data, 8.0, "R");
						WriteProperty(file, @"\Delta p(e)", "C_SpatialDelta", data, 8.0, "R");
						WriteProperty(file, @"P_I", "C_InconsistencyProbability", data, 100.0, @"\%");
						WriteProperty(file, @"P_M", "C_MissingProbability", data, 100.0, @"\%");
						WriteProperty(file, @"P_U", "I_UnwantedProbability", data, 100.0, @"\%");
					}
					file.WriteLine(@"\end{tabular}");
				}
			}

			bool brk = true;
		}

		private static T[] Array<T>(T element)
		{
			return new T[] { element };
		}

		private static T[] Array<T>(T a, T b)
		{
			return new T[] { a, b };
		}

		private static T[] ArrayN<T>(T first, bool appendNull) where T : class
		{
			return appendNull ? new T[] { first, null } : new T[] { first };
		}

		static bool putAnd = false;//put & before next cell

		private static void WriteProperty(StreamWriter file, string caption, string key, IEnumerable<MetricTable.CapturePair> sources, double scale, string unit, bool mean = true, bool full = true)
		{
			if (caption != null)
			{
				file.Write("$");
				file.Write(caption);
				file.Write("$");
				putAnd = true;
			}
			else
				putAnd = false;
			if (full)
				foreach (var s in sources)
					if (s != null)
						WriteProperties(file, s.Full.Measurements[key].Properties, scale, unit);
					else
						WriteEmpty(file);
			if (mean)
				foreach (var s in sources)
					if (s != null)
						WriteProperties(file, s.Mean.Measurements[key].Properties, scale, unit);
					else
						WriteEmpty(file);

			file.WriteLine(@"\\");
		}

		private static void WriteEmpty(StreamWriter file)
		{
			file.Write("&");
			//if (writeDeviation)
			//	file.Write("&");
		}

		private static void WriteProperties(StreamWriter file, MetricTable.MeasurementProperties properties, double scale, string unit)
		{
			if (putAnd)
				file.Write("&");
			file.Write(" $");
			WriteTexNumber(file, properties.Mean * scale, unit);
			file.Write(@"_{\:\pm ");
			WriteTexNumber(file, properties.Error * scale, "");
			file.Write("}$");
			//if (writeDeviation)
			//{
			//	file.Write("& $");
			//	WriteTexNumber(file, properties.Deviation * scale, unit);
			//	file.Write("$");
			//}

			putAnd = true;
		}

		private static void WriteTexNumber(StreamWriter file, double v, string unit)
		{
			int digits = (int)Math.Floor(Math.Log10(Math.Abs(v)));
			int firstDigit = (int)Math.Round(v / Math.Pow(10.0, digits));

			int zeroDigits = digits - 1;
			if (firstDigit < 5)
				zeroDigits--;
			double ex = Math.Pow(10.0, zeroDigits);
			v = (Math.Round(v / ex) * ex);
			string s;
			if (v < 0.001 || v > 1000)
				s = v.ToString("#.###E-0");
			else
				s = v.ToString();
			s = s.Replace("-", "\\text{-}");
			file.Write(s);
			file.Write(unit);
		}
	}
}
