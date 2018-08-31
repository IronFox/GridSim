using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;
using System.Xml.Linq;

namespace MeasurementMerger
{

	public class MetricTable
	{
		private static HashSet<string> excludeFromID =  new HashSet<string>(new string[]{ "endlessRuns","numRuns"});

		public Dictionary<string, Capture> chunks = new Dictionary<string, Capture>();

		public MetricTable(FileInfo fileInfo)
		{
			XElement source = XElement.Load(fileInfo.FullName);

			foreach (XElement node in source.Nodes())
			{
				var sub = new Capture(node);
				chunks.Add(sub.ID, sub);
			}
		}

		public void Include(MetricTable other)
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
		private static string Beautify(XmlDocument doc)
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

		public void ExportTo(FileInfo targetFile)
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

		public CapturePair ExtractPair(string name)
		{
			return new CapturePair(FindByName(name + "Mean"), FindByName(name));
		}

		public Capture FindByName(string name)
		{
			foreach (var c in chunks.Values)
				if (c.Name == name)
					return c;
			return null;
		}

		public Capture FindByID(string id)
		{
			return chunks[id];
		}

		public CapturePair[] ExtractCapturePairs()
		{
			var pairs = new List<CapturePair>();
			foreach (var c in chunks.Values)
			{
				if (c.Name == "capture")
				{
					var siblingID = Capture.MakeID("captureMean", c.IDSource);
					pairs.Add(new CapturePair(FindByID(siblingID), c));
				}
			}
			return pairs.ToArray();
		}

		public struct MeasurementProperties
		{
			public readonly double	Mean,
									Deviation,
									Error;

			public MeasurementProperties(double mean, double deviation, double error)
			{
				Mean = mean;
				Deviation = deviation;
				Error = error;
			}
		};

		public struct Measurement : IEquatable<Measurement>
		{
			public readonly double Sum,
								SquareSum;
			public readonly ulong NumSamples;
			public readonly bool IONumSamplesIsSampleCount;

			public Measurement(Func<string,string> attributes)
			{
				Sum = double.Parse(attributes("sum"));
				SquareSum = double.Parse(attributes("squareSum"));
				try
				{
					NumSamples = ulong.Parse(attributes("numSamples"));
					IONumSamplesIsSampleCount = false;
				}
				catch (Exception ex)
				{
					NumSamples = ulong.Parse(attributes("sampleCount"));
					IONumSamplesIsSampleCount = true;
				}
			}

			public Measurement(double sum, double squareSum, ulong numSamples)
			{
				IONumSamplesIsSampleCount = false;
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

			public MeasurementProperties Properties
			{
				get
				{
					double mean = Sum / NumSamples;
					double sqr = SquareSum / NumSamples;
					double dev = Math.Sqrt(sqr - mean * mean);
					double confInterval = 1.96 * dev / Math.Sqrt(NumSamples);
					return new MeasurementProperties(mean, dev, confInterval);
				}

			}
			

			internal void WriteTo(XmlElement xm)
			{
				var prop = Properties;
				xm.SetAttribute("mean", prop.Mean.ToString()+" (+/- "+ prop.Error.ToString()+")");
				xm.SetAttribute("deviation", prop.Deviation.ToString());
				xm.SetAttribute("sum", Sum.ToString("R"));
				xm.SetAttribute("squareSum", SquareSum.ToString("R"));
				if (IONumSamplesIsSampleCount)
					xm.SetAttribute("sampleCount", NumSamples.ToString());
				else
					xm.SetAttribute("numSamples", NumSamples.ToString());

				Measurement back = new Measurement(name => xm.GetAttribute(name));
				if (back != this)
					throw new InvalidCastException("Export != import");
			}

			public override bool Equals(object obj)
			{
				return obj is Measurement && Equals((Measurement)obj);
			}

			public bool Equals(Measurement other)
			{
				return this == other;
			}

			public override int GetHashCode()
			{
				var hashCode = 70319223;
				hashCode = hashCode * -1521134295 + Sum.GetHashCode();
				hashCode = hashCode * -1521134295 + SquareSum.GetHashCode();
				hashCode = hashCode * -1521134295 + NumSamples.GetHashCode();
				return hashCode;
			}
		}


		public class CapturePair
		{
			public readonly Capture Mean, Full;

			public CapturePair(Capture mean, Capture full)
			{
				Mean = mean ?? throw new ArgumentNullException("mean");
				Full = full ?? throw new ArgumentNullException("full");
			}
		}

		public class Capture
		{
			//<C_SpatialDelta mean="0.03935376" deviation="0.07151365" sum="126910.57005142" squareSum="21487.01963334" numSamples="3224865" />

			public readonly Dictionary<string, Measurement> Measurements = new Dictionary<string, Measurement>();
			public readonly string Name, ID;
			public readonly KeyValuePair<string, string>[] Attributes;
			public readonly XElement ConfigElement, IDSource;

			internal void Include(Capture other)
			{
				var keys = new string[Measurements.Keys.Count];
				Measurements.Keys.CopyTo(keys, 0);
				foreach (var k in keys)
				{
					Measurements[k] += other.Measurements[k];
				}
			}

			public static string MakeID(string baseName, XElement idSource)
			{
				var id = new StringBuilder(baseName);
				foreach (var attrib in idSource.Attributes())
					if (!excludeFromID.Contains(attrib.Name.LocalName))
						id.Append("_").Append(attrib.Name).Append("_").Append(attrib.Value);
				if (!string.IsNullOrWhiteSpace(idSource.Value))
					id.Append(":" + idSource.Value);
				return id.ToString();
			}

			public Capture(XElement node)
			{

				IDSource = node;
				ConfigElement = FindNode(node.Nodes(), "config");
				if (ConfigElement != null)
					IDSource = ConfigElement;

				ID = MakeID(node.Name.LocalName, IDSource);
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