using System;
using System.IO;
using System.Linq;

namespace AssetBatcher
{
	class Program
	{
		static string format = "auto-etc1";// "rgba8888";
		static string compression = "auto";

		static void Main(string[] args)
		{
			string rootDir = Path.GetFullPath("../..");
			string path = Path.Combine(rootDir, "data_out", "unit", "terran");
			var dir = Path.Combine(rootDir, "gfx");

			T3SFolder("unit/cmdbtns/cmdicons", "cmdicons");
		}

		static void T3SFolder(string folder, string name)
		{
			string path = Path.GetFullPath($"../../gfx/{folder}");
			string root = Path.GetFullPath("../../gfx");

			string header = $"--atlas -f {format} -z {compression} -q high";

			string fileName = Path.GetFullPath($"../../gfx/{name}.t3s");

			using (StreamWriter s = new StreamWriter(fileName)) {
				s.WriteLine(header);
				foreach (var f in Directory.GetFiles(path, "*.png", SearchOption.AllDirectories)) {
					s.WriteLine(f.Substring(root.Length + 1));
				}
			}
		}

		static void GenerateT3S(string dir)
		{

			foreach (var f in Directory.GetFiles(dir, "*.t3s", SearchOption.TopDirectoryOnly))
				File.Delete(f);

			var files = Directory.GetFiles(dir, "*.png", SearchOption.AllDirectories);
			var grouped = files.GroupBy(f => Path.GetDirectoryName(f)).ToDictionary(g => g.Key, g => g.ToList());

			string header = $"--atlas -f {format} -z {compression} -q high";

			foreach (var g in grouped) {
				string filePath = Path.GetFileNameWithoutExtension(g.Key);
				if (g.Key != dir)
					filePath = g.Key.Substring(dir.Length + 1);
				var fileName = filePath.Replace('\\', '_');
				string n = Path.GetFileNameWithoutExtension(g.Value[0]);
				if (g.Value.Count == 1 && !int.TryParse(n, out _)) {
					fileName = filePath.Replace('\\', '_') + "_" + n;
				}
				fileName = Path.Combine(dir, fileName) + ".t3s";

				using (StreamWriter s = new StreamWriter(fileName)) {
					string h = $"{header}";
					s.WriteLine(header);
					foreach (var p in g.Value) {
						var f = p.Substring(dir.Length + 1);
						s.WriteLine(f);
					}
				}
			}
		}
	}
}
