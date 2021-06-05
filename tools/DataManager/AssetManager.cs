using CsvHelper;
using CsvHelper.Configuration;
using CsvHelper.Configuration.Attributes;
using DataManager.Assets;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Numerics;

namespace DataManager
{
	public class GuiTexture : IDisposable
	{
		public Texture2D Texture { get; private set; }
		public IntPtr GuiImage { get; private set; }
		public Vector2 TextureSize => new Vector2(Texture.Width, Texture.Height);


		public GuiTexture(Texture2D texture)
		{
			Texture = texture;
			GuiImage = AppGame.GuiRenderer.BindTexture(Texture);
		}

		public void Dispose()
		{
			if (Texture != null)
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
		public static readonly string ImagesDataPath = $"{GameDataDir}images.csv";
		public static readonly string SpritesDataPath = $"{GameDataDir}sprites.csv";
		public static readonly string FramesDataPath = $"{GameDataDir}frames.csv";
		public static readonly string UpgradesDataPath = $"{GameDataDir}upgrades.csv";
		public static readonly string FlingyDataPath = $"{GameDataDir}flingy.csv";
		public static readonly string WeaponsDataPath = $"{GameDataDir}weapons.csv";

		public Dictionary<string, Palette> Palettes { get; private set; } = new Dictionary<string, Palette>();

		public List<ImageList> ImageLists { get; private set; } = new List<ImageList>();
		public List<SpriteAtlas> SpriteAtlases { get; private set; } = new List<SpriteAtlas>();
		public List<SpriteSheet> SpriteSheets { get; private set; } = new List<SpriteSheet>();
		//public List<LogicalImageAsset> Images { get; private set; } = new List<LogicalImageAsset>();
		//public List<LogicalSpriteAsset> Sprites { get; private set; } = new List<LogicalSpriteAsset>();
		//public List<UpgradeAsset> Upgrades { get; private set; } = new List<UpgradeAsset>();
		//public List<FlingyAsset> Flingy { get; private set; } = new List<FlingyAsset>();

		public List<SpriteFrame> Icons { get; private set; } = new List<SpriteFrame>();

		private Dictionary<string, List<GuiTexture>> loadedSheetImages = new Dictionary<string, List<GuiTexture>>();

		public static readonly CsvConfiguration CsvConfig = new CsvConfiguration(CultureInfo.InvariantCulture)
		{
			Delimiter = ",",
			
		};

		public Dictionary<Type, IAssetDatabase> Assets { get; private set; } = new Dictionary<Type, IAssetDatabase>();

		public AssetManager()
		{
			if (!Directory.Exists(ConvertedAssetDir))
				Directory.CreateDirectory(ConvertedAssetDir);

			if (!Directory.Exists(SpriteAtlasDir))
				Directory.CreateDirectory(SpriteAtlasDir);

			if (!Directory.Exists(SpriteBuildDir))
				Directory.CreateDirectory(SpriteBuildDir);


			AddNewAssetDatabase<LogicalImageAsset>(ImagesDataPath);
			AddNewAssetDatabase<LogicalSpriteAsset>(SpritesDataPath);
			AddNewAssetDatabase<UpgradeAsset>(UpgradesDataPath);
			AddNewAssetDatabase<FlingyAsset>(FlingyDataPath);
			AddNewAssetDatabase<WeaponAsset>(WeaponsDataPath);

		}

		private void AddNewAssetDatabase<TAsset>(string filePath) where TAsset : Asset
		{
			Assets[typeof(TAsset)] = new AssetDatabase<TAsset>(filePath);
		}

		public AssetDatabase<TAsset> GetAssetDatabase<TAsset>() where TAsset : Asset
		{
			return Assets[typeof(TAsset)] as AssetDatabase<TAsset>;
		}

		public List<TAsset> GetAssets<TAsset>() where TAsset : Asset
		{
			return (Assets[typeof(TAsset)] as AssetDatabase<TAsset>).Assets;
		}


		public void LoadEverything()
		{
			LoadPalettes();
			LoadImageLists();
			LoadSpriteAtlases();
			ReloadAssets();
		}

		public void ReloadAssets()
		{
			foreach (var db in Assets.Values)
				db.Reload();
		}

		public void LoadImageLists()
		{
			ImageLists.Clear();

			foreach (var file in Directory.GetFiles(ConvertedAssetDir, "info.txt", SearchOption.AllDirectories))
			{
				ImageLists.Add(new ImageList(file));
			}

		}

		public void LoadSpriteAtlases()
		{
			foreach (var tex in loadedSheetImages.SelectMany(s => s.Value))
				tex.Dispose();

			loadedSheetImages.Clear();

			SpriteAtlases.Clear();
			SpriteSheets.Clear();
			Icons.Clear();

			if (!File.Exists(SpriteAtlasDataPath) || !File.Exists(SpriteSheetDataPath))
				return;

			List<IGrouping<string, SpriteSubAtlas>> atlasRecords = null;

			using (var csv = new CsvReader(new StreamReader(SpriteAtlasDataPath), CsvConfig))
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
				var images = ImageLists.Where(s => atlasInfoFiles.Contains(s.InfoFilePath)).ToList();
				r.ImageLists = images;
			}

			foreach (var group in atlasRecords)
			{
				var s = new SpriteAtlas(group.Key);
				SpriteAtlases.Add(s);

				s.SetSubAtlases(group);
			}


			using (var csv = new CsvReader(new StreamReader(SpriteSheetDataPath), CsvConfig))
			{
				SpriteSheets.AddRange(csv.GetRecords<SpriteSheet>());
			}


			foreach (var sheet in SpriteSheets)
			{
				sheet.OnAfterDeserialize();
			}

			var iconsSheet = SpriteSheets.FirstOrDefault(s => s.SheetName == "unit\\cmdbtns\\cmdicons");
			if (iconsSheet != null)
			{
				Icons.AddRange(iconsSheet.Frames);
			}
		}


		public GuiTexture GetSheetImage(string sheetName, int frameIndex)
		{
			loadedSheetImages.TryGetValue(sheetName, out var images);

			if (images == null)
			{
				var sheet = SpriteSheets.FirstOrDefault(s => s.SheetName == sheetName);

				if (sheet == null)
					return null;

				images = new List<GuiTexture>();
				loadedSheetImages[sheetName] = images;

				for (int i = 0; i < sheet.TotalFrames; ++i)
				{
					var tex = Texture2D.FromFile(AppGame.Device, sheet.ImageAsset.GetFrameFilePath(i));
					images.Add(new GuiTexture(tex));
				}
				//var list = sheet.SubAtlas.ImageLists[sheet.]
			}
			return images[frameIndex];
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
				Microsoft.Xna.Framework.Color c = new Microsoft.Xna.Framework.Color(t[i], t[i], t[i], (byte)255);
				cmdIconsPalette.Colors[i + 1] = c;

			}

			for (int i = 0; i < Palettes["Units"].Colors.Count; ++i)
			{
				var c = Palettes["Units"].Colors[i];
				if (c.R == c.B && c.R > 10 && c.G == 0)
				{
					Microsoft.Xna.Framework.Color remap = new Microsoft.Xna.Framework.Color(c.R, c.R, c.R, c.A);
					Palettes["Units"].Remap(i, remap);
				}
			}

			Palettes.Add("Shadow", new Palette(Microsoft.Xna.Framework.Color.Black) { Name = "Shadow" });
			Palettes.Add("White", new Palette(Microsoft.Xna.Framework.Color.White) { Name = "White" });
		}

		public void ConvertGRP(GRPEntry asset, GRPConvertMode convertMode)
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

		private void HandleGRPConvert(GRPEntry asset, Palette pal)
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

		private void HandleWireframeGRPConvert(GRPEntry asset)
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

		public void ExportAtlas(IEnumerable<ImageList> assets, string atlasName)
		{
			List<ImageList> group = new List<ImageList>();

			SpriteAtlas spriteAtlas = new SpriteAtlas(atlasName);

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


			SpriteAtlases.RemoveAll(t => t.Name == atlasName);
			SpriteAtlases.Add(spriteAtlas);

			SpriteSheets.RemoveAll(t => t.Atlas == atlasName);
			SpriteSheets.AddRange(spriteAtlas.SubAtlases.SelectMany(a => a.GenerateSpriteSheets()));

			var records = SpriteAtlases.SelectMany(s => s.SubAtlases).OrderBy(t => t.AtlasName).ThenBy(t => t.AtlasIndex);

			var frames = SpriteSheets.SelectMany(s => s.Frames).OrderBy(f => f.SpriteSheetName);

			using (var csv = new CsvWriter(new StreamWriter(SpriteAtlasDataPath), CsvConfig))
			{
				csv.WriteRecords(records);
			}

			using (var csv = new CsvWriter(new StreamWriter(SpriteSheetDataPath), CsvConfig))
			{
				csv.WriteRecords(SpriteSheets);
			}

			using (var csv = new CsvWriter(new StreamWriter(FramesDataPath), CsvConfig))
			{
				csv.WriteRecords(frames);
			}
		}

		public AsyncOperation BuildAtlas(SpriteAtlas asset)
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