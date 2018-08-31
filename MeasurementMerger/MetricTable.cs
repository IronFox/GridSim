using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Linq;

namespace MeasurementMerger
{
	internal class MetricTable
	{
		private static HashSet<string> excludeFromID =  new HashSet<string>(new string[]{ "endlessRuns","numRuns"});

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
			public readonly bool IsSampleCount;

			public Measurement(Func<string,string> attributes)
			{
				Sum = double.Parse(attributes("sum"));
				SquareSum = double.Parse(attributes("squareSum"));
				try
				{
					NumSamples = ulong.Parse(attributes("numSamples"));
					IsSampleCount = false;
				}
				catch (Exception ex)
				{
					NumSamples = ulong.Parse(attributes("sampleCount"));
					IsSampleCount = true;
				}
			}

			public Measurement(double sum, double squareSum, ulong numSamples)
			{
				IsSampleCount = false;
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
				if (IsSampleCount)
					xm.SetAttribute("sampleCount", NumSamples.ToString());
				else
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
			public readonly XElement ConfigElement;

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

				XElement idSource = node;
				ConfigElement = FindNode(node.Nodes(), "config");
				if (ConfigElement != null)
					idSource = ConfigElement;


				foreach (var attrib in idSource.Attributes())
					if (!excludeFromID.Contains(attrib.Name.LocalName))
						ID += "_" + attrib.Name + "_" + attrib.Value;
				if (!string.IsNullOrWhiteSpace( idSource.Value))
					ID += "_" + idSource.Value;
				Name = node.Name.LocalName;


				Attributes = node.Attributes().Select(xattrib => new KeyValuePair<string, string>(xattrib.Name.LocalName, xattrib.Value)).ToArray();
				Parse(node.Nodes());
			}

			private static XElement FindNode(IEnumerable<XNode> enumerable, string v)
			{
				foreach (XElement el in enumerable)
					if (el.Name.LocalName == v)
						return el;
				return null;
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
				if (ConfigElement != null)
				{
					XmlElement xcfg = doc.CreateElement(ConfigElement.Name.LocalName);
					xlocal.AppendChild(xcfg);
					foreach (var attrib in ConfigElement.Attributes())
						xcfg.SetAttribute(attrib.Name.LocalName, attrib.Value);
					if (!string.IsNullOrWhiteSpace(ConfigElement.Value))
						xcfg.InnerText = ConfigElement.Value;
				}
			}

			internal void Parse(IEnumerable<XNode> table)
			{
				foreach (XElement xm in table)
				{
					if (xm == ConfigElement)
						continue;
					Measurements.Add(xm.Name.LocalName, new Measurement(name => xm.Attribute(name).Value));
				}
			}
		}
	}
}