using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MeasurementMerger
{
	class FileGroup
	{
		public readonly List<FileInfo> sources = new List<FileInfo>();
		public readonly string Name;

		public FileGroup(FileInfo f)
		{
			sources.Add(f);
			Name = f.Name;
		}

		public void MergeTo(DirectoryInfo target)
		{
			if (!target.Exists)
				throw new ArgumentException("Out directory "+target.FullName+" does not exist");
			var targetFile = new FileInfo(Path.Combine(target.FullName, Name));
			if (targetFile.Exists)
				throw new ArgumentException("Out file " + targetFile.FullName + " already exists");

			var t = new MetricTable(sources[0]);
			for (int i = 1; i < sources.Count; i++)
			{
				t.Include(new MetricTable(sources[i]));
			}
			t.ExportTo(targetFile);
		}
	};


	class Program
	{
		static void Main(string[] args)
		{
			var input = new DirectoryInfo("input");
			var output = new DirectoryInfo("out");
			if (!input.Exists)
			{
				throw new ArgumentException("Bad working directory. input sub directory must exist and must contain more than one sub directory");
			}
			if (!output.Exists)
			{
				Console.WriteLine("Creating out directory");
				Directory.CreateDirectory(output.FullName);
			}
			else
			{
				var files = output.GetFiles();
				foreach (var f in files)
				{
					Console.WriteLine("Clearing out previous merge result " + f.Name);
					f.Delete();
				}
			}

			var subs = input.GetDirectories();
			if (subs.Length < 2)
			{
				throw new ArgumentException("Bad working directory. input sub directory must exist and must contain more than one sub directory");
			}

			Dictionary<string, FileGroup> groups = new Dictionary<string, FileGroup>();
			{
				MapGroups(groups, subs[0],false, subs[0]);
			}
			for (int i = 1; i < subs.Length; i++)
			{
				MapGroups(groups, subs[i], true, subs[0]);
				foreach (var g in groups.Values)
					if (g.sources.Count != i + 1)
						throw new ArgumentException("File " + g.sources[0].Name + " not found in directory " + subs[i]);
			}


			foreach (var g in groups.Values)
			{
				g.MergeTo(new DirectoryInfo(output.FullName));
			}



			Console.WriteLine("All done");
		}

		private static void MapGroups(Dictionary<string, FileGroup> groups, DirectoryInfo parent, bool existingOnly, DirectoryInfo firstSub)
		{

			var files = parent.GetFiles("*.xml");
			foreach (var f in files)
			{
				if (existingOnly)
				{
					FileGroup group;
					bool success = groups.TryGetValue(f.Name, out group);
					if (!success)
						throw new ArgumentException("File " + f.FullName + " does not also exist in first sub directory " + firstSub.FullName);
					group.sources.Add(f);
				}
				else
					groups.Add(f.Name, new FileGroup(f));
			}
			foreach (var dir in parent.EnumerateDirectories())
				MapGroups(groups, dir, existingOnly, firstSub);
		}
	}
}
