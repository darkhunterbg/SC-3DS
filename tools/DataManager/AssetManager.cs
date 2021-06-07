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
using Rectangle = Microsoft.Xna.Framework.Rectangle;

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
		public static readonly string StarcraftAssetDir = "../../mpq/";
		public static readonly string AssetsDir = $"../../assets/";
		public static readonly string ExtractedAssetsDir = $"../../assets_extracted/";

		public static readonly string GameDataDir = $"{AssetsDir}data/";
		public static readonly string SpriteAtlasOutDir = $"{AssetsDir}atlases/";
		public static readonly string SpriteAtlas3DSBuildDir = "../../gfxbuild/atlases/";
		public static readonly string tex3dsPath = "C:\\devkitPro\\tools\\bin\\tex3ds.exe";

		public static readonly string SpriteAtlasDataPath = $"{GameDataDir}atlases.csv";
		public static readonly string FramesDataPath = $"{GameDataDir}frames.csv";

		public static readonly string ImagesDataPath = $"{GameDataDir}images.csv";
		public static readonly string SpritesDataPath = $"{GameDataDir}sprites.csv";
		public static readonly string UpgradesDataPath = $"{GameDataDir}upgrades.csv";
		public static readonly string FlingyDataPath = $"{GameDataDir}flingy.csv";
		public static readonly string WeaponsDataPath = $"{GameDataDir}weapons.csv";
		public static readonly string UnitsDataPath = $"{GameDataDir}units.csv";

		public Dictionary<string, Palette> Palettes { get; private set; } = new Dictionary<string, Palette>();

		public List<ImageList> ImageLists { get; private set; } = new List<ImageList>();



		public List<ImageFrame> Icons { get; private set; } = new List<ImageFrame>();
		public List<ImageFrame> UnitSelection { get; private set; } = new List<ImageFrame>();

		private Dictionary<string, List<GuiTexture>> loadedSheetImages = new Dictionary<string, List<GuiTexture>>();

		public static readonly CsvConfiguration CsvConfig = new CsvConfiguration(CultureInfo.InvariantCulture)
		{
			Delimiter = ",",
		};

		public Dictionary<Type, IAssetDatabase> Assets { get; private set; } = new Dictionary<Type, IAssetDatabase>();

		public AssetManager()
		{

			if (!Directory.Exists(AssetsDir))
				Directory.CreateDirectory(AssetsDir);

			if (!Directory.Exists(ExtractedAssetsDir))
				Directory.CreateDirectory(ExtractedAssetsDir);

			if (!Directory.Exists(SpriteAtlasOutDir))
				Directory.CreateDirectory(SpriteAtlasOutDir);

			if (!Directory.Exists(SpriteAtlas3DSBuildDir))
				Directory.CreateDirectory(SpriteAtlas3DSBuildDir);


			AddNewAssetDatabase<ImageAsset>(ImagesDataPath);
			AddNewAssetDatabase<SpriteAsset>(SpritesDataPath);
			AddNewAssetDatabase<UpgradeAsset>(UpgradesDataPath);
			AddNewAssetDatabase<FlingyAsset>(FlingyDataPath);
			AddNewAssetDatabase<WeaponAsset>(WeaponsDataPath);
			AddNewAssetDatabase<UnitAsset>(UnitsDataPath);

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


			var iconsSheet = ImageLists.FirstOrDefault(s => s.Key == "unit\\cmdbtns\\cmdicons");
			if (iconsSheet != null)
			{
				Icons.AddRange(iconsSheet.Frames);
			}


			foreach (var s in CustomEnumValues.SelectionTypes)
			{
				string sheet = s;
				sheet = sheet.Substring(0, sheet.Length - 2);
				if (sheet.Length < 3)
					sheet = $"0{sheet}";
				sheet = $"unit\\thingy\\o{sheet}";

				var ss = AppGame.AssetManager.ImageLists.FirstOrDefault(f => f.Key == sheet);
				if (ss == null)
					continue;
				UnitSelection.Add(ss.Frames[0]);
			}

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

			foreach (var file in Directory.GetFiles(ExtractedAssetsDir, "info.txt", SearchOption.AllDirectories))
			{
				ImageLists.Add(ImageList.FromInfoFile(file));
			}

			foreach (var file in Directory.GetFiles(AssetsDir, "*.png", SearchOption.AllDirectories))
			{
				ImageLists.Add(ImageList.FromPng(file));
			}

		}

		public GuiTexture GetImageFrame(string key, int frameIndex)
		{
			loadedSheetImages.TryGetValue(key, out var images);

			if (images == null)
			{
				var sheet = ImageLists.FirstOrDefault(s => s.Key == key);

				if (sheet == null)
					return null;

				images = new List<GuiTexture>();
				loadedSheetImages[key] = images;

				for (int i = 0; i < sheet.Frames.Count; ++i)
				{
					var tex = Texture2D.FromFile(AppGame.Device, sheet.GetFrameFilePath(i));
					images.Add(new GuiTexture(tex));
				}
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
			string dst = Path.Combine(ExtractedAssetsDir, Path.GetDirectoryName(f), Path.GetFileNameWithoutExtension(f));
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
			string dst = Path.Combine(ExtractedAssetsDir, Path.GetDirectoryName(f), Path.GetFileNameWithoutExtension(f));
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
	}
}