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
				throw new ArgumentException("input directory does not contain exactly one xml file, but "+files.Length);

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
					Console.WriteLine("Erasing old output " + f+"...");
					File.Delete(f);
				}
			}

			{
				using (StreamWriter file = new StreamWriter(@"output\statistics.tex"))
				{
					/*
						\begin{tabular}{lll}
							Property & Mean & Standard Deviation\\
							$\Omega$ & $1.3_{(\pm 2.8\mathrm{e}{-3})} R$ & $1.7 R$\\
							$\Delta p(e)$ & $0.16_{(\pm 7.4\mathrm{e}{-4})} R$& $0.44 R$\\
							$\left| IS \right| \over \left| SD \right|$ & $12_{(\pm 2.5\mathrm{e}{-2})} \%$ & $15\%$\\
							$\left| IS_{\text{merged}} \right| \over \left| SD \right|$ &
							$4.4_{(\pm 3.1\mathrm{e}{\text{-}2})} \%$ & $13 \%$\\
							$P_I$ & $79_{(\pm 7.2\mathrm{e}{-2})} \%$ & $43\%$\\
							$P_M$ & $16_{(\pm 6\mathrm{e}{-2})} \%$ & $36\%$ \\
							$P_U$ & $8.5_{(\pm 4.2\mathrm{e}{-2})}\%$ & $24\%$\\
						\end{tabular}
					 */

					file.WriteLine(@"\begin{tabular}{lllll}");
					{
						file.WriteLine(@"Property & Mean(O) & Deviation(O) & Mean(P) & Deviation(P)\\");
						WriteProperty(file, @"\Omega", "I_Omega", general,8.0,"R");
						WriteProperty(file, @"\Delta p(e)", "C_SpatialDelta", general,8.0,"R");
						WriteProperty(file, @"\left| IS \right| \over \left| SD \right|", "ICSize", general,100.0 / 64, @"\%");
						WriteProperty(file, @"\left| IS_{\text{merged}} \right| \over \left| SD \right|", "ICSize", captures[0],100.0 / 64, @"\%");
						WriteProperty(file, @"P_I", "C_InconsistencyProbability", general,100.0, @"\%");
						WriteProperty(file, @"P_M", "C_MissingProbability", general, 100.0, @"\%");
						WriteProperty(file, @"P_U", "I_UnwantedProbability", general, 100.0, @"\%");
					}
					file.WriteLine(@"\end{tabular}");
				}
			}



			bool brk = true;
		}

		private static void WriteProperty(StreamWriter file, string caption, string key, MetricTable.CapturePair general, double scale, string unit)
		{
			file.Write("$");
			file.Write(caption);
			file.Write("$");
			var mean = general.Mean.Measurements[key];
			var full = general.Full.Measurements[key];
			WriteProperties(file, mean.Properties,scale,unit);
			WriteProperties(file, full.Properties, scale, unit);
			file.WriteLine(@"\\");
		}

		private static void WriteProperties(StreamWriter file, MetricTable.MeasurementProperties properties, double scale, string unit)
		{
			file.Write("& $");
			WriteTexNumber(file, properties.Mean * scale, unit);
			file.Write(@"_{\pm ");
			WriteTexNumber(file, properties.Error * scale, "");
			file.Write("}$ & $");
			WriteTexNumber(file, properties.Deviation * scale, unit);
			file.Write("$");
		}

		private static void WriteTexNumber(StreamWriter file, double v, string unit)
		{
			int digits = (int)Math.Floor(Math.Log10(Math.Abs(v))) - 1;
			double ex = Math.Pow(10.0, digits);
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
