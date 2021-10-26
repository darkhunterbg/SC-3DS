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
	public class AssetConverter : IGuiPanel
	{
		class AssetConverEntry
		{
			[JsonProperty]
			public string Path { get; private set; } = string.Empty;
			[JsonIgnore]
			private Glob glob = new Glob(string.Empty);
			[JsonProperty]
			public GRPConvertMode Mode;
			[JsonProperty]
			public bool IgnorePCXPalette = true;


			public void SetPath(string path)
			{
				Path = path;
				try {
					glob = new Glob(path, GlobOptions.Compiled);
				} catch {
					glob = new Glob(string.Empty);
				}
			}

			public IEnumerable<RawAssetEntry> GetMatches(IEnumerable<RawAssetEntry> assets)
			{
				var result = assets.Where(a => glob.IsMatch(a.DisplayName));

				return result;
			}

			public bool IsMatch(RawAssetEntry asset)
			{
				return glob.IsMatch(asset.DisplayName);
			}

			public void Init()
			{
				SetPath(Path);
			}
		}


		public string WindowName => "Asset Converter";

		private List<RawAssetEntry> rawAssets;
		private List<AssetConverEntry> convertEntries = new List<AssetConverEntry>();
		private IRawImage loaded;
		private RawAssetEntry hovered;
		private GuiTexture previewTexture;
		private string assetFilter = string.Empty;

		private GRPConvertMode[] modes;
		private string[] modeNames;
		private bool failed = false;

		static string SettingsFileName = "convert_assets";

		public AssetConverter()
		{
			rawAssets = Directory.GetFiles(AssetManager.StarcraftAssetDir, "*.grp", SearchOption.AllDirectories)
				.Select(s => new RawAssetEntry(s)).ToList();

			rawAssets.AddRange(Directory.GetFiles(AssetManager.StarcraftAssetDir, "*.pcx", SearchOption.AllDirectories)
				.Select(s => new RawAssetEntry(s)));

			rawAssets = rawAssets.OrderByDescending(r => r.Path).ToList();

			Texture2D tex = new Texture2D(AppGame.Device, 1024, 1024, false, SurfaceFormat.Color);

			previewTexture = new GuiTexture(tex);

			modes = Enum.GetValues<GRPConvertMode>();
			modeNames = Enum.GetNames<GRPConvertMode>();

			convertEntries = AppSettings.Load<List<AssetConverEntry>>(SettingsFileName);
			if (convertEntries == null)
				convertEntries = new List<AssetConverEntry>();

			foreach (var e in convertEntries)
				e.Init();
		}

		private void Update()
		{
			if (hovered == null) {
				if (loaded != null) {
					loaded.Dispose();
					loaded = null;

				}

				failed = false;
			} else {
				if ((loaded == null || loaded.Path == hovered.Path) && failed)
					return;

				if (loaded == null || loaded.Path != hovered.Path) {
					loaded?.Dispose();
					try {
						loaded = hovered.LoadAsImage();
						if (loaded == null) {
							failed = true;
						}
					} catch (Exception ex) {
						loaded = null;
						failed = true;
					}
				}
			}

		}

		public void Draw(Vector2 clientSize)
		{
			Update();

			ImGui.Columns(2);
			DrawConvertSettings();

			ImGui.NextColumn();
			DrawRawAssets();
		}

		private void DrawRawAssets()
		{
			ImGui.BeginChild("##AssetConverter.Assets");
			ImGui.Text("Assets");
			ImGui.Separator();

			ImGui.InputText("##AssetConverter.Filter", ref assetFilter, 256);

			ImGui.Separator();

			hovered = null;

			IEnumerable<RawAssetEntry> query = Util.TextFilter(rawAssets, assetFilter, a => a.DisplayName);

			ImGui.BeginChild("##AssetConverter.RawAssetsTable");
			foreach (var asset in query) {
				if (convertEntries.Any(t => t.IsMatch(asset))) {
					ImGui.Text(asset.DisplayName);
				} else {
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

			if (loaded != null) {
				loaded.GUIPreview(previewTexture);
			} else {
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

			if (ImGui.Button("Convert all##AssetConverter.ConvertSettings.ConvertAll")) {
				if (Directory.Exists(AssetManager.ExtractedAssetsDir))
					Directory.Delete(AssetManager.ExtractedAssetsDir, true);
				AppGui.RunGuiCoroutine(ConvertCrt(convertEntries));
			}
			ImGui.SameLine();
			ImGui.Text($"{total}/{rawAssets.Count}");

			ImGui.Separator();
			AssetConverEntry hover = null;
			string text = string.Empty;

			bool changed = false;

			for (int i = 0; i < convertEntries.Count; ++i) {
				var entry = convertEntries[i];
				text = entry.Path;

				ImGui.SetNextItemWidth(400);
				if (ImGui.InputText($"##AssetConverter.ConvertSettings.path.{i}", ref text, 256)) {
					entry.SetPath(text);
					changed = true;
				}

				ImGui.SameLine();
				int mode = (int)entry.Mode;
				ImGui.SetNextItemWidth(200);
				if (ImGui.Combo($"##AssetConverter.ConvertSettings.mode.{i}", ref mode, modeNames, modeNames.Length)) {
					entry.Mode = modes[mode];
					changed = true;
				}

				ImGui.SameLine();
				if (ImGui.Checkbox($"Ignore PCX Palette##AssetConverter.ConvertSettings.ignorepal.{i}", ref entry.IgnorePCXPalette)) {
					changed = true;
				}


				ImGui.SameLine();
				ImGui.Text($"[{entry.GetMatches(rawAssets).Count()}]");
				if (ImGui.IsItemHovered())
					hover = entry;


				ImGui.SameLine();
				if (ImGui.Button($"Convert##AssetConverter.ConvertSettings.convert.{i}")) {
					AppGui.RunGuiCoroutine(ConvertCrt(new List<AssetConverEntry>() { entry }));
				}

				ImGui.SameLine();
				if (ImGui.Button($"Delete##AssetConverter.ConvertSettings.del.{i}")) {
					convertEntries.RemoveAt(i);
					--i;
					changed = true;
				}

				if (i > 0) {
					ImGui.SameLine();
					if (ImGui.ArrowButton($"##AssetConverter.ConvertSettings.up.{i}", ImGuiDir.Up)) {
						convertEntries.RemoveAt(i);
						convertEntries.Insert(i - 1, entry);
						changed = true;
					}
				}
				if (i < convertEntries.Count - 1) {
					ImGui.SameLine();
					if (ImGui.ArrowButton($"##AssetConverter.ConvertSettings.down.{i}", ImGuiDir.Down)) {
						convertEntries.RemoveAt(i);
						convertEntries.Insert(i + 1, entry);
						changed = true;
					}
				}
			}

			if (ImGui.Button("New Entry##AssetConverter.ConvertSettings.NewEntry")) {
				convertEntries.Add(new AssetConverEntry());
			}

			if (hover != null) {
				ShowConvertEntryHover(hover);
			}

			if (changed) {
				AppSettings.Save(SettingsFileName, convertEntries);
			}

			ImGui.EndChild();
		}

		private void ShowConvertEntryHover(AssetConverEntry entry)
		{
			var matches = entry.GetMatches(rawAssets).ToList();

			if (matches.Count == 0)
				return;

			ImGui.BeginTooltip();

			foreach (var m in matches) {
				ImGui.Text(m.DisplayName);
			}


			ImGui.EndTooltip();
		}

		private IEnumerator ConvertCrt(List<AssetConverEntry> entries)
		{
			AsyncOperation op = new AsyncOperation((op) =>
			{
				HashSet<Tuple<RawAssetEntry, GRPConvertMode>> convert = new HashSet<Tuple<RawAssetEntry, GRPConvertMode>>();

				int count = 0;
				op.Title = "Analyzing...";

				foreach (var entry in entries) {
					if (op.Cancelled) return;

					var matches = entry.GetMatches(rawAssets);

					op.SetProgress(entry.Path, ++count, entries.Count);

					foreach (var match in matches) {

						if (entry.IgnorePCXPalette) {
							if (match.IsPCX)
								try {

									var img = match.LoadAsImage() as PCXImage;

									if (img.IsPaletteFormat)
										continue;

								} catch (Exception ex) { }
						}

						var item = new Tuple<RawAssetEntry, GRPConvertMode>(match, entry.Mode);

						if (!convert.Contains(item)) {
							convert.Add(item);
						}
					}
				}

				count = 0;

				op.Title = "Converting...";
				foreach (var item in convert) {
					if (op.Cancelled) return;

					op.SetProgress(item.Item1.DisplayName, ++count, convert.Count);

					var crt = AppGui.RunGuiCoroutine(() =>
					{
						if (item.Item1.IsGRP)
							AppGame.AssetManager.ConvertGRP(item.Item1, item.Item2);
						else
							if (item.Item1.IsPCX)
							AppGame.AssetManager.ConvertPCX(item.Item1);

					});

					crt.Wait();
				}

				op.Title = "Loading images...";

				if (op.Cancelled) return;

				AppGame.AssetManager.ReloadEverything(op);
			});


			while (!op.Completed) {
				AppGui.ProgressDialog(op, true);
				yield return null;
			}
		}
	}
}
