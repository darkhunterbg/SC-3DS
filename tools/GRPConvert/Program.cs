using System;
using System.Collections.Generic;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;

namespace GRPConvert
{
	class Program
	{
		static Dictionary<string, Palette> palettes = new Dictionary<string, Palette>();

		static void Main(string[] args)
		{
			foreach (var file in Directory.EnumerateFiles("..\\..\\palettes\\", "*.pal", SearchOption.AllDirectories)) {
				palettes.Add(Path.GetFileNameWithoutExtension(file), new Palette(file));
			}

			string[] lines = File.ReadAllLines("input.txt");

			Palette pal = null;

			string dataDir = Path.GetFullPath("..\\..\\data\\");
			string dataOutDir = Path.GetFullPath("..\\..\\data_out\\");

			if (Directory.Exists(dataOutDir))
				Directory.Delete(dataOutDir, true);

			foreach (var l in lines) {
				if (l.StartsWith("#"))
					continue;
				if (string.IsNullOrWhiteSpace(l))
					continue;

				if (l.StartsWith("palette")) {
					pal = palettes[l.Trim().Split(' ').Last()];
					Console.WriteLine($"Using palette '{pal.Name}'");
					continue;
				}

				List<string> files = new List<string>();

				if (l.EndsWith("\\*.grp")) {
					string s = Path.Combine(dataDir, l);

					files = Directory.EnumerateFiles(s.Substring(0, s.Length - 6), "*.grp", SearchOption.AllDirectories)
						.Select(s => s.Substring(dataDir.Length)).ToList();
				} else {
					files.Add(l) ;
				}

				foreach (var f in files) {
					GRPImage img = new GRPImage(Path.Combine(dataDir, f));
					int i = 0;
					string dst = Path.Combine(dataOutDir, Path.GetDirectoryName(f), Path.GetFileNameWithoutExtension(f));
					Directory.CreateDirectory(dst);

					string name = Path.GetFileNameWithoutExtension(f);

					foreach (var fr in img.Frames) {

						string s = Path.Combine(dst, i.ToString("D2") + ".png");
						++i;
						fr.ToBitmap(pal).Save(s, ImageFormat.Png);
					}
					Console.WriteLine(f);
				}
			}
		}
	}
}
