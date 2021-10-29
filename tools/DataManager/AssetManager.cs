using CsvHelper;
using CsvHelper.Configuration;
using CsvHelper.Configuration.Attributes;
using DataManager.Assets;
using DataManager.Assets.Raw;
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
		public static readonly string PalettePath = "..\\..\\palettes\\";
		public static readonly string StarcraftAssetDir = "..\\..\\mpq\\";
		public static readonly string AssetsDir = $"..\\..\\assets\\";
		public static readonly string ExtractedAssetsDir = $"..\\..\\assets_extracted\\";
		public static readonly string AudioDir = StarcraftAssetDir;
		public static readonly string VideoDir = StarcraftAssetDir;

		static readonly string[] AudioSearchDirs = new string[] { "sound", "music" };

		public static readonly string GameDataDir = $"{AssetsDir}data\\";

		public static readonly string CookedAssetsRootDir = $"..\\..\\cooked_assets\\";
		public static readonly string CookedAssetsPCDir = $"{CookedAssetsRootDir}\\win32\\";
		public static readonly string Cooked3DSAssetsDir = $"{CookedAssetsRootDir}\\3ds";

		public static readonly string SpriteAtlasOutDir = $"{CookedAssetsPCDir}atlases\\";
		public static readonly string BuildDataFile = $"data.bin";


		public static readonly string SpriteAtlas3DSBuildDir = $"{Cooked3DSAssetsDir}\\atlases\\";
		public static readonly string tex3dsPath = "C:\\devkitPro\\tools\\bin\\tex3ds.exe";
		public static readonly string mkbcfntPath = "C:\\devkitPro\\tools\\bin\\mkbcfnt.exe";

		public static readonly string SpriteAtlasDataPath = $"{GameDataDir}atlases.csv";
		public static readonly string ImagesDataPath = $"{GameDataDir}images.csv";
		public static readonly string FramesDataPath = $"{GameDataDir}frames.csv";


		public Dictionary<string, Palette> Palettes { get; private set; } = new Dictionary<string, Palette>();

		public List<ImageList> ImageLists { get; private set; } = new List<ImageList>();
		public List<AudioClip> AudioClips { get; private set; } = new List<AudioClip>();
		public List<VideoClip> VideoClips { get; private set; } = new List<VideoClip>();
		public List<ImageFrame> Icons { get; private set; } = new List<ImageFrame>();
		public List<ImageFrame> UnitSelection { get; private set; } = new List<ImageFrame>();

		private Dictionary<string, List<GuiTexture>> loadedSheetImages = new Dictionary<string, List<GuiTexture>>();

		public static readonly CsvConfiguration CsvConfig = new CsvConfiguration(CultureInfo.InvariantCulture)
		{
			Delimiter = ",",
		};

		public Dictionary<Type, IAssetDatabase> Assets { get; private set; } = new Dictionary<Type, IAssetDatabase>();

		public event Action OnReloaded;
		public event Action OnAssetsReloaded;

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

			// ORDER MATTERS HERE

			AddNewAssetDatabase<SoundSetAsset>($"{GameDataDir}soundsets.csv");
			AddNewAssetDatabase<SpriteAsset>($"{GameDataDir}sprites.csv");
			AddNewAssetDatabase<SpriteAnimClipAsset>($"{GameDataDir}animclips.csv");
			AddNewAssetDatabase<AbilityAsset>($"{GameDataDir}abilities.csv");
			AddNewAssetDatabase<WeaponAsset>($"{GameDataDir}weapons.csv");
			AddNewAssetDatabase<EffectAsset>($"{GameDataDir}effects.csv");
			AddNewAssetDatabase<UpgradeAsset>($"{GameDataDir}upgrades.csv");
			AddNewAssetDatabase<UnitWireframeAsset>($"{GameDataDir}wireframes.csv");
			AddNewAssetDatabase<UnitPortraitAsset>($"{GameDataDir}unitportraits.csv");
			AddNewAssetDatabase<UnitAsset>($"{GameDataDir}units.csv");
		
		}

		private void AddNewAssetDatabase<TAsset>(string filePath) where TAsset : Asset, new()
		{
			Assets[typeof(TAsset)] = new AssetDatabase<TAsset>(filePath);
		}
		public IAssetDatabase GetAssetDatabase(Type type)
		{
			return Assets[type];
		}
		public AssetDatabase<TAsset> GetAssetDatabase<TAsset>() where TAsset : Asset,
			new()
		{
			return Assets[typeof(TAsset)] as AssetDatabase<TAsset>;
		}

		public IEnumerable<TAsset> GetAssets<TAsset>() where TAsset : Asset, new()
		{
			return (Assets[typeof(TAsset)] as AssetDatabase<TAsset>).Assets;
		}

		public AsyncOperation LoadEverythingAsync()
		{
			return new AsyncOperation(ReloadEverything);
		}

		public void ReloadEverything(AsyncOperation op = null)
		{
			op.Title = "Loading palettes";
			LoadPalettes(op);
			op.Title = "Loading image lists";
			LoadImageLists(op);
			op.Title = "Loading audio clips";
			LoadAudioClips(op);
			op.Title = "Loading video clips";
			LoadVideoClips(op);

			op.Title = "Finalizing";
			op.ItemName = "Reloading assets...";

			ReloadAssets();

			OnReloaded?.Invoke();

			//SaveAllAssets();
		}

		public void ReloadAssets()
		{
			foreach (var db in Assets.Values)
				db.Reload();

			OnAssetsReloaded?.Invoke();
		}

		public void SaveAllAssets()
		{
			foreach (var db in Assets.Values)
				db.Save();
		}

		public void LoadImageLists(AsyncOperation op)
		{
			ImageLists.Clear();

			string[] imageLists = null;
			int count = 0;
			try {

				imageLists = Directory.GetFiles(ExtractedAssetsDir, "info.txt", SearchOption.AllDirectories);
				count = 0;
				foreach (var file in imageLists) {
					op.SetProgress(file, ++count, imageLists.Length);
					ImageLists.Add(ImageList.FromInfoFile(file));
				}

				List<string> ignore = ImageLists.SelectMany(s => s.Frames).Select(s => s.FrameFilePath).ToList();


				imageLists = Directory.GetFiles(ExtractedAssetsDir, "*.png", SearchOption.AllDirectories);
				count = 0;
				foreach (var file in imageLists) {

					op.SetProgress(file, ++count, imageLists.Length);

					if (ignore.Contains(file))
						continue;

					ImageLists.Add(ImageList.FromPng(file, isExtracted: true));
				}
			} catch (SixLabors.ImageSharp.ImageFormatException) {

				op.Title = "Corrupted extracted assets detected!";
				op.ItemName = "Cleaning extracted assets...";
				op.Progress = 0.5f;
				Directory.Delete(ExtractedAssetsDir, true);
				Directory.CreateDirectory(ExtractedAssetsDir);
			}

			imageLists = Directory.GetFiles(AssetsDir, "*.png", SearchOption.AllDirectories);
			count = 0;
			foreach (var file in imageLists) {
				op.SetProgress(file, ++count, imageLists.Length);

				if (file.StartsWith(SpriteAtlasOutDir))
					continue;

				ImageLists.Add(ImageList.FromPng(file));
			}


			Icons.Clear();
			var iconsSheet = ImageLists.FirstOrDefault(s => s.Key == "unit\\cmdbtns\\cmdicons");
			if (iconsSheet != null) {
				op.ItemName = iconsSheet.Key;
				Icons.AddRange(iconsSheet.Frames);
			}


			UnitSelection.Clear();
			foreach (var s in CustomEnumValues.SelectionTypes) {
				string sheet = s;
				sheet = sheet.Substring(0, sheet.Length - 2);
				if (sheet.Length < 3)
					sheet = $"0{sheet}";
				sheet = $"unit\\thingy\\o{sheet}";

				op.ItemName = sheet;

				var ss = AppGame.AssetManager.ImageLists.FirstOrDefault(f => f.Key == sheet);
				if (ss == null)
					continue;
				UnitSelection.Add(ss.Frames[0]);
			}
		}

		public void LoadAudioClips(AsyncOperation op)
		{
			AudioClips.Clear();


			foreach (var subDir in AudioSearchDirs) {
				var files = Directory.GetFiles(Path.Combine(AudioDir, subDir), "*.wav", SearchOption.AllDirectories);
				int count = 0;
				foreach (var file in files) {

					op.SetProgress(file, ++count, files.Length);

					AudioClips.Add(new AudioClip(file));
				}
			}
		}

		public void LoadVideoClips(AsyncOperation op)
		{
			VideoClips.ForEach(v => v.Dispose());
			VideoClips.Clear();

			var files = Directory.GetFiles(VideoDir, "*.smk", SearchOption.AllDirectories);
			int count = 0;
			foreach (var file in files) {

				op.SetProgress(file, ++count, files.Length);

				VideoClips.Add(new VideoClip(file));
			}

		}

		public GuiTexture GetImageFrame(string key, int frameIndex)
		{
			loadedSheetImages.TryGetValue(key, out var images);

			if (images == null) {
				var sheet = ImageLists.FirstOrDefault(s => s.Key == key);

				if (sheet == null)
					return null;

				images = new List<GuiTexture>();
				loadedSheetImages[key] = images;

				for (int i = 0; i < sheet.Frames.Count; ++i) {
					var tex = Texture2D.FromFile(AppGame.Device, sheet.GetFrameFilePath(i));
					images.Add(new GuiTexture(tex));
				}
			}
			return images[frameIndex];
		}

		private void LoadPalettes(AsyncOperation op)
		{
			Palettes.Clear();

			var palettes = Directory.GetFiles(PalettePath, "*.pal");

			int count = 0;
			foreach (var file in palettes) {
				op.SetProgress(file, ++count, palettes.Length);

				var p = new Palette(file);
				Palettes.Add(p.Name, p);
			}

			var cmdIconsPalette = Palettes["Icons"].Clone("CommandIcons");
			Palettes.Add(cmdIconsPalette.Name, cmdIconsPalette);

			int[] t = {
				255, 222, 248, 236, 236,
				187, 153, 146, 104, 76,
				66, 35, 24, 10, 0 };

			for (int i = 0; i < t.Length; ++i) {
				Microsoft.Xna.Framework.Color c = new Microsoft.Xna.Framework.Color(t[i], t[i], t[i], (byte)255);
				cmdIconsPalette.Colors[i + 1] = c;

			}

			for (int i = 0; i < Palettes["Units"].Colors.Count; ++i) {
				var c = Palettes["Units"].Colors[i];
				if (c.R == c.B && c.R > 10 && c.G == 0) {
					Microsoft.Xna.Framework.Color remap = new Microsoft.Xna.Framework.Color(c.R, c.R, c.R, c.A);
					Palettes["Units"].Remap(i, remap);
				}
			}

			Palettes.Add("Shadow", new Palette(Microsoft.Xna.Framework.Color.Black) { Name = "Shadow" });
			Palettes.Add("White", new Palette(Microsoft.Xna.Framework.Color.White) { Name = "White" });
		}

		public void ConvertPCX(RawAssetEntry asset)
		{
			PCXImage img = asset.LoadAsImage() as PCXImage;
			if (img == null) return;


			string f = asset.DisplayName;
			string dst = Path.Combine(ExtractedAssetsDir, Path.GetDirectoryName(f), Path.GetFileNameWithoutExtension(f)) + ".png";

			img.ToTexture().SaveAsPng(dst);
		}
		public void ConvertGRP(RawAssetEntry asset, GRPConvertMode convertMode)
		{
			switch (convertMode) {
				case GRPConvertMode.Wireframe: {
						HandleWireframeGRPConvert(asset);
						break;
					};
				default: {
						var pal = Palettes[convertMode.ToString()];
						HandleGRPConvert(asset, pal);
						break;
					}
			}
		}
		private void HandleGRPConvert(RawAssetEntry asset, Palette pal)
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

			foreach (var fr in img.Frames) {
				string frName = i.ToString("D3") + ".png";
				string s = Path.Combine(dst, frName);
				++i;

				var tex = fr.ToTexture(p);
				tex.SaveAsPng(s);
				tex.Dispose();

				info.Add($"{frName} {fr.XOffset} {fr.YOffset} {fr.Width} {fr.Height}");
			}

			i = 0;
			foreach (var fr in img.Frames) {
				if (p.Name == "Units" && fr.UsesRemappedColors(p)) {
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
		private void HandleWireframeGRPConvert(RawAssetEntry asset)
		{
			string f = asset.DisplayName;
			GRPImage img = new GRPImage(asset.Path);
			string dst = Path.Combine(ExtractedAssetsDir, Path.GetDirectoryName(f), Path.GetFileNameWithoutExtension(f));
			Directory.CreateDirectory(dst);

			var p = Palettes["Units"];

			int i = 0;
			foreach (var fr in img.Frames) {
				List<string> info = new List<string>();
				info.Add($"{img.MaxWidth} {img.MaxHeight}");

				string subDirName = i.ToString("D3");
				string subDir = Path.Combine(dst, subDirName);

				Directory.CreateDirectory(subDir);

				int j = 0;
				foreach (var wf in fr.GenerateWireframeImages(p)) {
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