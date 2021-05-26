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
		static Palette cmdIconsPalette;

		static string dataDir = Path.GetFullPath("..\\..\\mpq\\");
		static string dataOutDir = Path.GetFullPath("..\\..\\data_out\\");

		static void Main(string[] args)
		{
			shadowPalette = new Palette(Color.Black) { Name = "Shadows" };

			palettes.Add("White", new Palette(Color.White) { Name = "White" });

			foreach (var file in Directory.EnumerateFiles("..\\..\\palettes\\", "*.pal", SearchOption.AllDirectories))
			{
				palettes.Add(Path.GetFileNameWithoutExtension(file), new Palette(file));
			}

			cmdIconsPalette = palettes["Icons"].Clone("CmdIcons");
			palettes.Add(cmdIconsPalette.Name, cmdIconsPalette);

			for (int i = 0; i < palettes["Units"].Colors.Count; ++i)
			{
				var c = palettes["Units"].Colors[i];
				if (c.R == c.B && c.R > 10 && c.G == 0)
				{
					Color remap = Color.FromArgb(c.A, c.R, c.R, c.R);
					palettes["Units"].Remap(i, remap);
				}
			}

			int[] t = { 
				255, 222, 248, 236, 236,
				187, 153, 146, 104, 76,
				66, 35, 24, 10, 0 };

			for(int i = 0; i < t.Length; ++i)
			{
				Color c = Color.FromArgb(255, t[i], t[i], t[i]);
				cmdIconsPalette.Colors[i + 1] = c;

			}

			string[] lines = File.ReadAllLines("input.txt");

			Palette pal = null;


			foreach (var l in lines)
			{
				if (l.StartsWith("#"))
					continue;
				if (string.IsNullOrWhiteSpace(l))
					continue;

				if (l.Equals("clear"))
				{
					if (Directory.Exists(dataOutDir))
						Directory.Delete(dataOutDir, true);
					continue;
				}

				if (l.Equals("done"))
				{
					break;
				}

				if (l.StartsWith("palette"))
				{
					pal = palettes[l.Trim().Split(' ').Last()];
					Console.WriteLine($"Using palette '{pal.Name}'");
					continue;
				}

				if (l.StartsWith("wireframe "))
				{
					HandleWireframe(l.Split(" ").Last());
					continue;
				}

				List<string> files = new List<string>();

				if (l.EndsWith("\\*.grp"))
				{
					string s = Path.Combine(dataDir, l);

					files = Directory.EnumerateFiles(s.Substring(0, s.Length - 6), "*.grp", SearchOption.AllDirectories)
						.Select(s => s.Substring(dataDir.Length)).ToList();
				}
				else
				{
					files.Add(l);
				}

				foreach (var f in files)
				{

					List<string> info = new List<string>();
					GRPImage img = new GRPImage(Path.Combine(dataDir, f));
					int i = 0;
					string dst = Path.Combine(dataOutDir, Path.GetDirectoryName(f), Path.GetFileNameWithoutExtension(f));
					Directory.CreateDirectory(dst);
					info.Add($"{img.MaxWidth} {img.MaxHeight}");

					string name = Path.GetFileNameWithoutExtension(f);

					var p = f.EndsWith("shad.grp") ? shadowPalette : pal;

					foreach (var fr in img.Frames)
					{
						string frName = i.ToString("D3") + ".png";
						string s = Path.Combine(dst, frName);
						++i;

						fr.ToBitmap(p).Save(s, ImageFormat.Png);

						info.Add($"{frName} {fr.XOffset} {fr.YOffset} {fr.Width} {fr.Height}");
					}

					i = 0;
					foreach (var fr in img.Frames)
					{
						if (p.Name == "Units" && fr.UsesRemappedColors(p))
						{
							string frName = "cm_" + i.ToString("D3") + ".png";
							string s = Path.Combine(dst, frName);

							fr.GenerateMapOfRemappedColors(p).Save(s, ImageFormat.Png);

							info.Add($"{frName} {fr.XOffset} {fr.YOffset} {fr.Width} {fr.Height}");
						}

						++i;
					}

					File.WriteAllLines(Path.Combine(dst, $"info.txt"), info);
					Console.WriteLine(f);
				}
			}
		}

		static void HandleWireframe(string f)
		{
			GRPImage img = new GRPImage(Path.Combine(dataDir, f));


			string dst = Path.Combine(dataOutDir, Path.GetDirectoryName(f), Path.GetFileNameWithoutExtension(f));
			Directory.CreateDirectory(dst);

			var p = palettes["Units"];

			int i = 0;
			foreach (var fr in img.Frames)
			{
				List<string> info = new List<string>();
				info.Add($"{img.MaxWidth} {img.MaxHeight}");

				string subDirName = i.ToString("D3");
				string subDir = Path.Combine(dst, subDirName);

				Directory.CreateDirectory(subDir);

				int j = 0;
				foreach (var wf in fr.GenerateWireframeImages(p))
				{
					string wfName = j.ToString() + ".png";

					string outWf = Path.Combine(subDir, wfName);

					wf.Save(outWf);

					++j;

					info.Add($"{wfName} {fr.XOffset} {fr.YOffset} {fr.Width} {fr.Height}");
				}

				++i;

				File.WriteAllLines(Path.Combine(subDir, $"info.txt"), info);
			}


			Console.WriteLine(f);
		}
	}
}
