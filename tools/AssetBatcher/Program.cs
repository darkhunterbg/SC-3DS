using System;
using System.IO;
using System.Linq;

namespace AssetBatcher
{
	class Program
	{
		static void Main(string[] args)
		{
			string rootDir = Path.GetFullPath("../..");

			string format = "auto-etc1";// "rgba8888";
			string compression =  "auto";
			GenerateT3S(rootDir, format, compression);
		}

		static void GenerateT3S(string rootDir, string format, string compression)
		{
			var dir = Path.Combine(rootDir, "gfx");

			foreach (var f in Directory.GetFiles(dir, "*.t3s", SearchOption.TopDirectoryOnly))
				File.Delete(f);

			var files = Directory.GetFiles(dir, "*.png", SearchOption.AllDirectories);
			var grouped = files.GroupBy(f => Path.GetDirectoryName(f)).ToDictionary(g=>g.Key, g=>g.ToList());

			string header = $"--atlas -f {format} -z {compression} -q high";

			foreach(var g in grouped) {
				var filePath = g.Key.Substring(dir.Length + 1);
				var fileName = filePath.Replace('\\', '_');
				string n = Path.GetFileNameWithoutExtension(g.Value[0]);
				if(g.Value.Count == 1 && !int.TryParse(n,out _)) {
					fileName = filePath.Replace('\\', '_') + "_"+ n;
				}
				fileName = Path.Combine(dir, fileName) + ".t3s";

				using (StreamWriter s = new StreamWriter(fileName)) {
					string h = $"{header}";
					s.WriteLine(header);
					foreach(var p in g.Value) {
						var f = p.Substring(dir.Length + 1);
						s.WriteLine(f);
					}
				}
			}
		}
	}
}
