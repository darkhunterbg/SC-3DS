using DataManager.Assets;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.IO;

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

	public class GRPAsset
	{
		public string Path;
		public string DisplayName;

		public GRPAsset(string file)
		{
			Path = file;
			DisplayName = file.Substring(AssetManager.RawAssetDir.Length);
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

		public Dictionary<string, Palette> Palettes { get; private set; } = new Dictionary<string, Palette>();

		public AssetManager()
		{
			if (!Directory.Exists(ConvertedAssetDir))
				Directory.CreateDirectory(ConvertedAssetDir);

			LoadPalettes();
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
	}
}
