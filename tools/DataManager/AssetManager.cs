using CsvHelper;
using CsvHelper.Configuration;
using DataManager.Assets;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;

namespace DataManager
{
	public class GuiTexture : IDisposable
	{
		public Texture2D Texture { get; private set; }
		public IntPtr GuiImage { get; private set; }


		public GuiTexture(Texture2D texture)
		{
			Texture = texture;
			GuiImage = AppGame.GuiRenderer.BindTexture(Texture);
		}

		public void Dispose()
		{
			Texture.Dispose();
			AppGame.GuiRenderer.UnbindTexture(GuiImage);
		}
	}


	public enum GRPConvertMode
	{
		Units = 0,
		Icons = 1,
		CommandIcons = 2,
		Wireframe = 3,
		Orange = 4,
		White = 5,
		Shadow = 6,
		Blue = 7,
		Blue2 = 8,
		Green = 9,
		Terrain = 10,
	}


	public class AssetManager
	{
		public static readonly string PalettePath = "../../palettes/";
		public static readonly string RawAssetDir = "../../mpq/";
		public static readonly string ConvertedAssetDir = "../../data_out/";
		public static readonly string ConvertedAssetDirRelative = "data_out/";
		public static readonly string GameDataDir = "../../data/";
		public static readonly string SpriteAtlasDir = "../../data/atlases/";
		public static readonly string SpriteBuildDir = "../../gfxbuild/atlases/";
		public static readonly string tex3dsPath = "C:\\devkitPro\\tools\\bin\\tex3ds.exe";

		public static readonly string SpriteAtlasDataPath = $"{GameDataDir}atlases.csv";
		public static readonly string SpriteSheetDataPath = $"{GameDataDir}spritesheets.csv";

		public Dictionary<string, Palette> Palettes { get; private set; } = new Dictionary<string, Palette>();

		public List<ImageListAsset> ImageListAssets { get; private set; } = new List<ImageListAsset>();
		public List<SpriteAtlasAsset> SpriteAtlasAssets { get; private set; } = new List<SpriteAtlasAsset>();
		public List<SpriteSheetAsset> SpriteSheets { get; private set; } = new List<SpriteSheetAsset>();

		private CsvConfiguration csvConfig = new CsvConfiguration(CultureInfo.InvariantCulture)
		{
			Delimiter = ",",
		};

		public AssetManager()
		{
			if (!Directory.Exists(ConvertedAssetDir))
				Directory.CreateDirectory(ConvertedAssetDir);

			if (!Directory.Exists(SpriteAtlasDir))
				Directory.CreateDirectory(SpriteAtlasDir);

			if (!Directory.Exists(SpriteBuildDir))
				Directory.CreateDirectory(SpriteBuildDir);

			LoadPalettes();
			ReloadImageListAssets();
			ReloadSpriteAtlasAssets();
		}

		public void ReloadImageListAssets()
		{
			ImageListAssets.Clear();

			foreach (var file in Directory.GetFiles(ConvertedAssetDir, "info.txt", SearchOption.AllDirectories))
			{
				ImageListAssets.Add(new ImageListAsset(file));
			}
		}

		public void ReloadSpriteAtlasAssets()
		{
			SpriteAtlasAssets.Clear();
		
			List<IGrouping<string, SpriteSubAtlas>> atlasRecords = null;

			using (var csv = new CsvReader(new StreamReader(SpriteAtlasDataPath), csvConfig))
			{
				atlasRecords = csv.GetRecords<SpriteSubAtlas>().GroupBy(g => g.AtlasName)
					.ToList();
			}

			var sheets = Directory.GetFileSystemEntries($"{SpriteAtlasDir}", "*.t3s");
			Dictionary<string, List<string>> sheetsInfoFiles = new Dictionary<string, List<string>>();

			foreach (var sheet in sheets)
			{
				var info = File.ReadAllLines(sheet).Skip(1).ToList();
				string sheetName = Path.GetFileNameWithoutExtension(sheet);
				sheetsInfoFiles[sheetName] = info
					.Select(f => Path.GetDirectoryName(f))
					.Distinct()
					.Select(i => i.Substring(ConvertedAssetDir.Length) + "\\info.txt").ToList();

			}


			foreach (var r in atlasRecords.SelectMany(s => s))
			{
				var atlasInfoFiles = sheetsInfoFiles[r.FullName];
				var images = ImageListAssets.Where(s => atlasInfoFiles.Contains(s.InfoFilePath)).ToList();
				r.ImageLists = images;
			}

			foreach (var group in atlasRecords)
			{
				var s = new SpriteAtlasAsset(group.Key);
				SpriteAtlasAssets.Add(s);

				s.SetSubAtlases(group);
			}

			SpriteSheets.Clear();

			using (var csv = new CsvReader(new StreamReader(SpriteSheetDataPath), csvConfig))
			{
				SpriteSheets.AddRange(csv.GetRecords<SpriteSheetAsset>());
			}

		}

		private void LoadPalettes()
		{
			foreach (var file in Directory.GetFiles(PalettePath, "*.pal"))
			{
				var p = new Palette(file);
				Palettes.Add(p.Name, p);
			}

			var cmdIconsPalette = Palettes["Icons"].Clone("CommandIcons");
			Palettes.Add(cmdIconsPalette.Name, cmdIconsPalette);

			int[] t = {
				255, 222, 248, 236, 236,
				187, 153, 146, 104, 76,
				66, 35, 24, 10, 0 };

			for (int i = 0; i < t.Length; ++i)
			{
				Color c = new Color(t[i], t[i], t[i], (byte)255);
				cmdIconsPalette.Colors[i + 1] = c;

			}

			for (int i = 0; i < Palettes["Units"].Colors.Count; ++i)
			{
				var c = Palettes["Units"].Colors[i];
				if (c.R == c.B && c.R > 10 && c.G == 0)
				{
					Color remap = new Color(c.R, c.R, c.R, c.A);
					Palettes["Units"].Remap(i, remap);
				}
			}

			Palettes.Add("Shadow", new Palette(Color.Black) { Name = "Shadow" });
			Palettes.Add("White", new Palette(Color.White) { Name = "White" });
		}

		public void ConvertGRP(GRPAsset asset, GRPConvertMode convertMode)
		{
			switch (convertMode)
			{
				case GRPConvertMode.Wireframe:
					{
						HandleWireframeGRPConvert(asset);
						break;
					};
				default:
					{
						var pal = Palettes[convertMode.ToString()];
						HandleGRPConvert(asset, pal);

						break;
					}
			}
		}

		private void HandleGRPConvert(GRPAsset asset, Palette pal)
		{
			string f = asset.DisplayName;

			List<string> info = new List<string>();
			GRPImage img = new GRPImage(asset.Path);
			int i = 0;
			string dst = Path.Combine(ConvertedAssetDir, Path.GetDirectoryName(f), Path.GetFileNameWithoutExtension(f));
			Directory.CreateDirectory(dst);
			info.Add($"{img.MaxWidth} {img.MaxHeight}");

			string name = Path.GetFileNameWithoutExtension(f);

			var p = pal;

			foreach (var fr in img.Frames)
			{
				string frName = i.ToString("D3") + ".png";
				string s = Path.Combine(dst, frName);
				++i;

				var tex = fr.ToTexture(p);
				tex.SaveAsPng(s);
				tex.Dispose();

				info.Add($"{frName} {fr.XOffset} {fr.YOffset} {fr.Width} {fr.Height}");
			}

			i = 0;
			foreach (var fr in img.Frames)
			{
				if (p.Name == "Units" && fr.UsesRemappedColors(p))
				{
					string frName = "cm_" + i.ToString("D3") + ".png";
					string s = Path.Combine(dst, frName);

					var tex = fr.GenerateMapOfRemappedColors(p);
					tex.SaveAsPng(s);
					tex.Dispose();

					info.Add($"{frName} {fr.XOffset} {fr.YOffset} {fr.Width} {fr.Height}");
				}

				++i;
			}

			File.WriteAllLines(Path.Combine(dst, $"info.txt"), info);
		}

		private void HandleWireframeGRPConvert(GRPAsset asset)
		{
			string f = asset.DisplayName;
			GRPImage img = new GRPImage(asset.Path);
			string dst = Path.Combine(ConvertedAssetDir, Path.GetDirectoryName(f), Path.GetFileNameWithoutExtension(f));
			Directory.CreateDirectory(dst);

			var p = Palettes["Units"];

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

					wf.SaveAsPng(outWf);

					++j;

					info.Add($"{wfName} {fr.XOffset} {fr.YOffset} {fr.Width} {fr.Height}");

					wf.Dispose();
				}

				++i;

				File.WriteAllLines(Path.Combine(subDir, $"info.txt"), info);
			}
		}

		public void ExportAtlas(IEnumerable<ImageListAsset> assets, string atlasName)
		{
			List<ImageListAsset> group = new List<ImageListAsset>();

			SpriteAtlasAsset spriteAtlas = new SpriteAtlasAsset(atlasName);

			int freeSpace = (1024 * 1024 * 90) / 100;

			var r = new SpriteSubAtlas();

			foreach (var asset in assets)
			{
				if (group.Sum(s => s.TakenSpace) + asset.TakenSpace < freeSpace)
				{
					group.Add(asset);
				}
				else
				{
					spriteAtlas.AddSubAtlas(group);
					group.Clear();
				}
			}

			if (group.Count > 0)
			{
				spriteAtlas.AddSubAtlas(group);
			}

			foreach (var atlas in spriteAtlas.SubAtlases)
			{

				string file = atlas.InfoFilePath;
				string outDir = $"../../{ConvertedAssetDirRelative}";

				using (StreamWriter sw = new StreamWriter(file))
				{
					sw.WriteLine($"--atlas -f auto-etc1 -z auto -q high");
					foreach (var asset in atlas.ImageLists)
					{
						foreach (var frame in asset.Frames)
						{
							sw.WriteLine($"{outDir}{asset.RelativePath}/{frame.fileName}".Replace("\\", "/"));
						}
					}

				}
			}


			SpriteAtlasAssets.RemoveAll(t => t.Name == atlasName);
			SpriteAtlasAssets.Add(spriteAtlas);

			SpriteSheets.RemoveAll(t => t.Atlas == atlasName);
			SpriteSheets.AddRange( spriteAtlas.SubAtlases.SelectMany(a => a.GenerateSpriteSheets()));

			var records = SpriteAtlasAssets.SelectMany(s => s.SubAtlases).OrderBy(t => t.AtlasName).ThenBy(t => t.AtlasIndex);


			using (var csv = new CsvWriter(new StreamWriter(SpriteAtlasDataPath), csvConfig))
			{
				csv.WriteRecords(records);
			}

			using (var csv = new CsvWriter(new StreamWriter(SpriteSheetDataPath), csvConfig))
			{
				csv.WriteRecords(SpriteSheets);
			}
		}

		public AsyncOperation BuildAtlas(SpriteAtlasAsset asset)
		{
			Process p = null;
			bool done = false;

			return new AsyncOperation((Action<float> report) =>
			{
				int i = 0;
				foreach (var subatlas in asset.SubAtlases)
				{
					++i;
					if (done)
						break;

					string infoFile = subatlas.InfoFilePath;
					string outAtlas = Path.GetFullPath(Path.Combine(SpriteBuildDir, subatlas.FullName)) + ".t3x";

					string args = $"-i {infoFile} -o {outAtlas}";
					var process = new ProcessStartInfo(tex3dsPath, args);
					process.UseShellExecute = false;
					process.CreateNoWindow = true;
					process.RedirectStandardOutput = true;
					process.RedirectStandardError = true;
					p = new Process()
					{
						StartInfo = process
					};


					p.Start();

					p.WaitForExit();

					string error = p.StandardError.ReadToEnd();
					if (!string.IsNullOrEmpty(error))
						throw new Exception($"Failed to build subatlas {subatlas.FullName}: {error}");

					report((float)i / (float)asset.SubAtlases.Count());
				}
			}, () =>
			{
				p.Kill();
			});

		}
	}
}
