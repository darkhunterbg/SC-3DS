using DataManager.Assets;
using GlobExpressions;
using ImGuiNET;
using Microsoft.Xna.Framework.Graphics;
using Newtonsoft.Json;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;


namespace DataManager.Panels
{
	public class AssetConverter
	{
		class GRPConverEntry
		{
			[JsonProperty]
			public string Path { get; private set; } = string.Empty;
			[JsonIgnore]
			private Glob glob = new Glob(string.Empty);
			[JsonProperty]
			public GRPConvertMode Mode;

			public void SetPath(string path)
			{
				Path = path;
				try
				{
					glob = new Glob(path, GlobOptions.Compiled);
				}
				catch
				{
					glob = new Glob(string.Empty);
				}
			}

			public IEnumerable<GRPAsset> GetMatches(IEnumerable<GRPAsset> assets)
			{

				return assets.Where(a => glob.IsMatch(a.DisplayName));

			}

			public bool IsMatch(GRPAsset asset)
			{
				return glob.IsMatch(asset.DisplayName);
			}

			public void Init()
			{
				SetPath(Path);
			}
		}

		private List<GRPAsset> rawAssets;
		private List<GRPConverEntry> convertEntries = new List<GRPConverEntry>();
		private GRPImage loaded;
		private GRPAsset hovered;
		private GuiTexture previewTexture;
		private string assetFilter = string.Empty;

		private GRPConvertMode[] modes;
		private string[] modeNames;
		private bool failed = false;

		static string SettingsFileName = "convert_assets";

		public AssetConverter()
		{
			rawAssets = Directory.GetFiles(AssetManager.RawAssetDir, "*.grp", SearchOption.AllDirectories)
				.Select(s => new GRPAsset(s)).ToList();

			Texture2D tex = new Texture2D(AppGame.Device, 1024, 1024, false, SurfaceFormat.Color);

			previewTexture = new GuiTexture(tex);

			modes = Enum.GetValues<GRPConvertMode>();
			modeNames = Enum.GetNames<GRPConvertMode>();

			convertEntries = AppSettings.Load<List<GRPConverEntry>>(SettingsFileName);
			if (convertEntries == null)
				convertEntries = new List<GRPConverEntry>();

			foreach (var e in convertEntries)
				e.Init();
		}

		public void Update()
		{
			if (hovered == null)
			{
				if (loaded != null)
				{
					loaded.Dispose();
					loaded = null;

				}

				failed = false;
			}
			else
			{
				if ((loaded == null || loaded.Path == hovered.Path) && failed)
					return;

				if (loaded == null || loaded.Path != hovered.Path)
				{
					loaded?.Dispose();
					try
					{
						loaded = new GRPImage(hovered.Path);
						loaded.LoadPreview(previewTexture.Texture);
						failed = false;
					}
					catch
					{
						loaded = null;
						failed = true;
					}
				}
			}

		}

		public void Draw()
		{
			ImGui.SetNextWindowSize(new Vector2(800, 600), ImGuiCond.FirstUseEver);

			ImGui.Begin("Asset Converter##AssetConverter");

			ImGui.Columns(2);
			DrawConvertSettings();

			ImGui.NextColumn();
			DrawRawAssets();

			ImGui.End();
		}

		private void DrawRawAssets()
		{
			ImGui.BeginChild("##AssetConverter.Assets");
			ImGui.Text("Assets");
			ImGui.Separator();

			ImGui.InputText("##AssetConverter.Filter", ref assetFilter, 256);

			ImGui.Separator();

			hovered = null;

			IEnumerable<GRPAsset> query = rawAssets;

			if (!string.IsNullOrEmpty(assetFilter))
			{
				if (!assetFilter.Contains('*') &&
					!assetFilter.Contains('?') &&
					!assetFilter.Contains('[') &&
					!assetFilter.Contains('{'))

					query = query.Where(a => a.DisplayName.Contains(assetFilter, StringComparison.InvariantCultureIgnoreCase));
				else
				{
					try
					{
						var g = new Glob(assetFilter, GlobOptions.Compiled);
						query = query.Where(a => g.IsMatch(a.DisplayName));
					}
					catch { }
				}

			}


			ImGui.BeginChild("##AssetConverter.RawAssetsTable");
			foreach (var asset in query)
			{
				if (convertEntries.Any(t => t.IsMatch(asset)))
				{
					ImGui.Text(asset.DisplayName);
				}
				else
				{
					ImGui.TextDisabled(asset.DisplayName);
				}

				if (ImGui.IsItemHovered())
					hovered = asset;
			}
			ImGui.EndChild();

			ImGui.EndChild();


			if (hovered != null)
				DrawRawAssetTooltip();

		}

		private void DrawRawAssetTooltip()
		{
			ImGui.BeginTooltip();

			if (loaded != null)
			{
				ImGui.Text($"Dimensions: {loaded.MaxWidth}x{loaded.MaxHeight}");
				ImGui.Text($"Frames: {loaded.NumberOfFrames}");

				Vector2 uv = new Vector2(loaded.Preview.Width, loaded.Preview.Height);
				Vector2 size = new Vector2(loaded.MaxWidth, loaded.MaxHeight);
				int multiplier = 1;
				if (size.X <= 128 && size.Y <= 128)
					multiplier = 2;
				if (size.X <= 64 && size.Y <= 64)
					multiplier = 4;
				//if (size.X <= 32 && size.Y <= 32)
				//	multiplier = 8;

				ImGui.Image(previewTexture.GuiImage, uv * multiplier, Vector2.Zero, uv / previewTexture.Texture.Width);
			}
			else
			{
				ImGui.TextColored(Microsoft.Xna.Framework.Color.Red.ToVec4(), "Failed to load image data.");
			}

			ImGui.EndTooltip();
		}

		private void DrawConvertSettings()
		{
			ImGui.BeginChild("##AssetConverter.ConvertSettings");
			ImGui.Text("Convert Settings");
			ImGui.Separator();

			int total = convertEntries.SelectMany(entry => entry.GetMatches(rawAssets)).Distinct().Count();

			if (ImGui.Button("Convert all##AssetConverter.ConvertSettings.ConvertAll"))
			{
				if (Directory.Exists(AssetManager.ConvertedAssetDir))
					Directory.Delete(AssetManager.ConvertedAssetDir, true);
				AppGui.RunGuiCoroutine(ConvertCrt(convertEntries));
			}
			ImGui.SameLine();
			ImGui.Text($"{total}/{rawAssets.Count}");

			ImGui.Separator();
			GRPConverEntry hover = null;
			string text = string.Empty;

			bool changed = false;

			for (int i = 0; i < convertEntries.Count; ++i)
			{
				var entry = convertEntries[i];
				text = entry.Path;

				ImGui.SetNextItemWidth(400);
				if (ImGui.InputText($"##AssetConverter.ConvertSettings.path.{i}", ref text, 256))
				{
					entry.SetPath(text);
					changed = true;
				}

				ImGui.SameLine();
				int mode = (int)entry.Mode;
				ImGui.SetNextItemWidth(200);
				if (ImGui.Combo($"##AssetConverter.ConvertSettings.mode.{i}", ref mode, modeNames, modeNames.Length))
				{
					entry.Mode = modes[mode];
					changed = true;
				}

				ImGui.SameLine();
				ImGui.Text($"[{entry.GetMatches(rawAssets).Count()}]");
				if (ImGui.IsItemHovered())
					hover = entry;


				ImGui.SameLine();
				if (ImGui.Button($"Convert##AssetConverter.ConvertSettings.convert.{i}"))
				{
					AppGui.RunGuiCoroutine(ConvertCrt(new List<GRPConverEntry>() { entry }));
				}

				ImGui.SameLine();
				if (ImGui.Button($"Delete##AssetConverter.ConvertSettings.del.{i}"))
				{
					convertEntries.RemoveAt(i);
					--i;
					changed = true;
				}

				if (i > 0)
				{
					ImGui.SameLine();
					if (ImGui.ArrowButton($"##AssetConverter.ConvertSettings.up.{i}", ImGuiDir.Up))
					{
						convertEntries.RemoveAt(i);
						convertEntries.Insert(i - 1, entry);
						changed = true;
					}
				}
				if (i < convertEntries.Count - 1)
				{
					ImGui.SameLine();
					if (ImGui.ArrowButton($"##AssetConverter.ConvertSettings.down.{i}", ImGuiDir.Down))
					{
						convertEntries.RemoveAt(i);
						convertEntries.Insert(i + 1, entry);
						changed = true;
					}
				}
			}

			if (ImGui.Button("New Entry##AssetConverter.ConvertSettings.NewEntry"))
			{
				convertEntries.Add(new GRPConverEntry());
			}

			if (hover != null)
			{
				ShowConvertEntryHover(hover);
			}

			if (changed)
			{
				AppSettings.Save(SettingsFileName, convertEntries);
			}

			ImGui.EndChild();
		}

		private void ShowConvertEntryHover(GRPConverEntry entry)
		{
			var matches = entry.GetMatches(rawAssets).ToList();

			if (matches.Count == 0)
				return;

			ImGui.BeginTooltip();

			foreach (var m in matches)
			{
				ImGui.Text(m.DisplayName);
			}


			ImGui.EndTooltip();
		}

		struct ConvertItem
		{
			GRPAsset asset;
			GRPConvertMode mode;
		}

		private IEnumerator ConvertCrt(List<GRPConverEntry> entries)
		{
			HashSet<Tuple<GRPAsset,GRPConvertMode>> convert = new HashSet<Tuple<GRPAsset, GRPConvertMode>>();

			foreach (var entry in entries)
			{
				var matches = entry.GetMatches(rawAssets);

				foreach (var match in matches)
				{
					var item = new Tuple<GRPAsset, GRPConvertMode> (match, entry.Mode);

					if (!convert.Contains(item))
					{
						convert.Add(item);
					}
		

				
				}
			}

			int i = 0;

			foreach (var item in convert)
			{
				AppGame.AssetManager.ConvertGRP(item.Item1, item.Item2);

				if (!AppGui.ProgressDialog($"Converting {i++}/{convert.Count}", i, convert.Count, true))
					yield break;

				yield return null;
			}
		}
	}
}
