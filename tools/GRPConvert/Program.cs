using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;

namespace GRPConvert
{
	class Program
	{
		static Dictionary<string, Palette> palettes = new Dictionary<string, Palette>();

		static Palette shadowPalette;

		static void Main(string[] args)
		{
			shadowPalette = new Palette(Color.Black);

			foreach (var file in Directory.EnumerateFiles("..\\..\\palettes\\", "*.pal", SearchOption.AllDirectories)) {
				palettes.Add(Path.GetFileNameWithoutExtension(file), new Palette(file));
			}

			string[] lines = File.ReadAllLines("input.txt");

			Palette pal = null;

			string dataDir = Path.GetFullPath("..\\..\\mpq\\");
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

					List<string> info = new List<string>();
					GRPImage img = new GRPImage(Path.Combine(dataDir, f));
					int i = 0;
					string dst = Path.Combine(dataOutDir, Path.GetDirectoryName(f), Path.GetFileNameWithoutExtension(f));
					Directory.CreateDirectory(dst);
					info.Add($"{img.MaxWidth} {img.MaxHeight}");

					string name = Path.GetFileNameWithoutExtension(f);

					var p = f.EndsWith("shad.grp") ? shadowPalette : pal;

					foreach (var fr in img.Frames) {

						string frName = i.ToString("D3") + ".png";
						string s = Path.Combine(dst, frName);
						++i;
						fr.ToBitmap(p).Save(s, ImageFormat.Png);
						info.Add($"{frName} {fr.XOffset} {fr.YOffset} {fr.Width} {fr.Height}");
					}
					File.WriteAllLines(Path.Combine(dst,$"info.txt"),info);
					Console.WriteLine(f);
				}
			}
		}
	}
}
