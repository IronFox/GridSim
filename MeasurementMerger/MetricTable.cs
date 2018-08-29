using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;
using System.Xml.Linq;

namespace MeasurementMerger
{
	internal class MetricTable
	{

		public Dictionary<string, SubTable> chunks = new Dictionary<string, SubTable>();

		public MetricTable(FileInfo fileInfo)
		{
			XElement source = XElement.Load(fileInfo.FullName);

			foreach (XElement node in source.Nodes())
			{
				var sub = new SubTable(node);
				chunks.Add(sub.ID, sub);
			}
		}

		internal void Include(MetricTable other)
		{
			if (chunks.Count != other.chunks.Count)
				throw new ArgumentException("Chunk count mismatch: "+chunks.Count+" != "+other.chunks.Count);
			foreach (var c in chunks)
			{
				var c2 = other.chunks[c.Key];
				c.Value.Include(c2);
			}
		}

		//https://stackoverflow.com/a/43461833
		public static string Beautify(XmlDocument doc)
		{
			string xmlString = null;
			using (MemoryStream ms = new MemoryStream())
			{
				XmlWriterSettings settings = new XmlWriterSettings
				{
					Encoding = new UTF8Encoding(false),
					Indent = true,
					IndentChars = "  ",
					NewLineChars = "\r\n",
					NewLineHandling = NewLineHandling.Replace
				};
				using (XmlWriter writer = XmlWriter.Create(ms, settings))
				{
					doc.Save(writer);
				}
				xmlString = Encoding.UTF8.GetString(ms.ToArray());
			}
			return xmlString;
		}

		internal void ExportTo(FileInfo targetFile)
		{
			XmlDocument doc = new XmlDocument();

			XmlElement root = doc.CreateElement("xml");
			doc.AppendChild(root);
			
			foreach (var c in chunks)
			{
				c.Value.AppendTo(doc,root);
			}
			File.WriteAllText(targetFile.FullName, Beautify(doc));
		}

		public struct Measurement
		{
			public readonly double Sum,
								SquareSum;
			public readonly ulong NumSamples;

			public Measurement(Func<string,string> attributes)
			{
				Sum = double.Parse(attributes("sum"));
				SquareSum = double.Parse(attributes("squareSum"));
				NumSamples = ulong.Parse(attributes("numSamples"));
			}

			public Measurement(double sum, double squareSum, ulong numSamples)
			{
				Sum = sum;
				SquareSum = squareSum;
				NumSamples = numSamples;
			}

			public static Measurement operator +(Measurement a, Measurement b)
			{
				return new Measurement(a.Sum + b.Sum, a.SquareSum + b.SquareSum, a.NumSamples + b.NumSamples);
			}
			public static bool operator ==(Measurement a, Measurement b)
			{
				return a.Sum == b.Sum && a.SquareSum == b.SquareSum && a.NumSamples == b.NumSamples;
			}
			public static bool operator !=(Measurement a, Measurement b)
			{
				return !(a == b);
			}




			internal void WriteTo(XmlElement xm)
			{
				double mean = Sum / NumSamples;
				double sqr = SquareSum / NumSamples;
				double dev = Math.Sqrt(sqr - mean * mean);
				double confInterval = 1.96 * dev / Math.Sqrt(NumSamples);
				xm.SetAttribute("mean", mean.ToString()+" (+/- "+ confInterval.ToString()+")");
				xm.SetAttribute("deviation", dev.ToString());
				xm.SetAttribute("sum", Sum.ToString("R"));
				xm.SetAttribute("squareSum", SquareSum.ToString("R"));
				xm.SetAttribute("numSamples", NumSamples.ToString());

				Measurement back = new Measurement(name => xm.GetAttribute(name));
				if (back != this)
					throw new InvalidCastException("Export != import");
			}

		}

		public class SubTable
		{
			//<C_SpatialDelta mean="0.03935376" deviation="0.07151365" sum="126910.57005142" squareSum="21487.01963334" numSamples="3224865" />

			public readonly Dictionary<string, Measurement> Measurements = new Dictionary<string, Measurement>();
			public readonly string Name, ID;
			public readonly KeyValuePair<string, string>[] Attributes;

			internal void Include(SubTable other)
			{
				var keys = new string[Measurements.Keys.Count];
				Measurements.Keys.CopyTo(keys, 0);
				foreach (var k in keys)
				{
					Measurements[k] += other.Measurements[k];
				}
			}

			public SubTable(XElement node)
			{
				ID = node.Name.LocalName;
				int cnt = 0;
				foreach (var attrib in node.Attributes())
				{
					ID += "_" + attrib.Name + "_" + attrib.Value;
					cnt++;
				}
				Name = node.Name.LocalName;

				Attributes = new KeyValuePair<string, string>[cnt];
				cnt = 0;
				foreach (var attrib in node.Attributes())
				{
					Attributes[cnt] = new KeyValuePair<string, string>(attrib.Name.LocalName , attrib.Value);
					cnt++;
				}
				Parse(node.Nodes());
			}

			public void AppendTo(XmlDocument doc, XmlElement parent)
			{
				
				XmlElement xlocal = doc.CreateElement(Name);
				parent.AppendChild(xlocal);

				foreach (var at in Attributes)
					xlocal.SetAttribute(at.Key, at.Value);

				foreach (var m in Measurements)
				{
					XmlElement xm = doc.CreateElement(m.Key);
					m.Value.WriteTo(xm);
					xlocal.AppendChild(xm);
				}
			}

			internal void Parse(IEnumerable<XNode> table)
			{
				foreach (XElement xm in table)
				{
					Measurements.Add(xm.Name.LocalName, new Measurement(name => xm.Attribute(name).Value));
				}
			}
		}
	}
}