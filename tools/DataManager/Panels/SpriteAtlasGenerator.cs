using DataManager.Assets;
using GlobExpressions;
using ImGuiNET;
using Newtonsoft.Json;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
	public class SpriteAtlasEntry
	{
		[JsonProperty]
		public string OutputName = string.Empty;
		[JsonProperty(PropertyName = "ImageList")]
		private List<string> imageList = new List<string>();

		[JsonIgnore]
		public List<ImageListAsset> Assets = new List<ImageListAsset>();
		[JsonIgnore]
		public readonly int Id = -1;
		[JsonIgnore]
		public float Used { get; private set; }

		static int id = 0;

		public SpriteAtlasEntry()
		{
			Id = ++id;
		}

		public void Init()
		{
			Assets.Clear();

			foreach (var l in imageList)
			{

				var item = AppGame.AssetManager.ImageListAssets.FirstOrDefault(a => a.RelativePath == l);
				if (item == null || Assets.Contains(item))
					continue;

				Assets.Add(item);
			}

			Assets = Assets.OrderBy(a => a.RelativePath).ToList();
			imageList.Sort();

			RecalculateUsed();
		}

		public void SetAssets(IEnumerable<ImageListAsset> assets)
		{
			imageList.Clear();
			Assets.Clear();


			Assets.AddRange(assets.Distinct());
			Assets = Assets.OrderBy(a => a.RelativePath).ToList();

			imageList.AddRange(Assets.Select(s => s.RelativePath));

			imageList.Sort();

			RecalculateUsed();
		}

		public void RemoveAsset(ImageListAsset asset)
		{
			Assets.Remove(asset);
			imageList.Remove(asset.RelativePath);
			RecalculateUsed();
		}

		public static float CalculateUsage(ImageListAsset asset)
		{
			int size = 1024 * 1024;
			int used = 0;

			foreach (var frame in asset.Frames)
			{
				used += frame.rect.Size.X * frame.rect.Size.Y + 2;
			}


			return (float)used / (float)size;
		}

		private void RecalculateUsed()
		{
			int size = 1024 * 1024;
			int used = 0;
			foreach (var asset in Assets)
			{
				foreach (var frame in asset.Frames)
				{
					used += frame.rect.Size.X * frame.rect.Size.Y + 2;
				}
			}

			Used = (float)used / (float)size;
		}
	}

	public class SpriteAtlasGenerator
	{
		private List<SpriteAtlasEntry> entries = new List<SpriteAtlasEntry>();

		static readonly string SettingsFileName = "sprite_atlases";

		bool changed = false;
		bool selectItemsModal = false;
		SpriteAtlasEntry selectItemsFor = null;
		List<ImageListAsset> modalSelectedAssets = new List<ImageListAsset>();
		string assetFilter = string.Empty;

		public SpriteAtlasGenerator()
		{
			entries = AppSettings.Load<List<SpriteAtlasEntry>>(SettingsFileName) ??
				new List<SpriteAtlasEntry>();

			foreach (var e in entries)
				e.Init();
		}

		public void Draw()
		{
			changed = false;

			ImGui.SetNextWindowSize(new Vector2(800, 600), ImGuiCond.FirstUseEver);

			ImGui.Begin("Sprite Atlas Generator##sag");

			ImGui.Columns(2);

			DrawEntries();

			ImGui.NextColumn();

			DrawImageListAssets();

			ImGui.End();

			if (selectItemsModal)
			{
				DrawSelectItemsModal();
			}

			if (changed)
			{
				AppSettings.Save(SettingsFileName, entries);
			}
		}

		private void DrawImageListAssets()
		{
			ImGui.BeginChild("##sag.assets");

			var query = DrawImageListAssetsFilter("##sag.assets.search");

			ImGui.BeginChild("##sag.assets.list");

			foreach (var asset in query)
			{
				bool enabled = entries.Any(e => e.Assets.Contains(asset));
				DrawImageAssetListItem(asset, enabled);

			}

			ImGui.EndChild();

			ImGui.EndChild();
		}

		private void DrawEntries()
		{

			ImGui.BeginChild("##sag.entries");

			if (ImGui.Button("Normalize##sag.entries.normalize"))
			{
				NormalizeEntries();
			}
			ImGui.SameLine();
			if (ImGui.Button("Generate##sag.entries.generate"))
			{
				AppGame.RunCoroutine(GenerateCrt());
			}
			ImGui.SameLine();
			if (ImGui.Button("Build##sag.entries.build"))
			{
				AppGame.RunCoroutine(BuildCrt());
			}

			ImGui.BeginChild("##sag.entries.items");

			string text = string.Empty;

			for (int i = 0; i < entries.Count; ++i)
			{
				var entry = entries[i];
				text = entry.OutputName;

				int count = entry.Assets.Sum(s => s.Frames.Count);
				bool expand = ImGui.TreeNodeEx($"[{count}]##sag.entries.node.{entry.Id}");

				ImGui.SameLine();
				ImGui.SetNextItemWidth(400);
				if (ImGui.InputText($"##sag.entries.path.{entry.Id}", ref text, 256))
				{
					entry.OutputName = text;
					changed = true;
				}

				ImGui.SameLine();
				ImGui.Text($"{(int)(entry.Used * 100)}%% used");

				ImGui.SameLine();
				if (ImGui.Button($"Edit List##sag.entries.edit.{entry.Id}"))
				{
					selectItemsModal = true;
					selectItemsFor = entry;

					modalSelectedAssets.Clear();
					modalSelectedAssets.AddRange(entry.Assets);
				}

				ImGui.SameLine();
				if (ImGui.Button($"Delete##sag.entries.del.{entry.Id}"))
				{
					entries.RemoveAt(i--);
					changed = true;
					continue;
				}


				if (expand)
				{
					DrawEntryAssetList(entry);

					ImGui.TreePop();
				}

			}

			ImGui.Separator();

			if (ImGui.Button("New Entry##sag.entries.new"))
			{
				entries.Add(new SpriteAtlasEntry());

				changed = true;
			}
			ImGui.EndChild();

			ImGui.EndChild();
		}

		private void DrawEntryAssetList(SpriteAtlasEntry entry)
		{
			for (int j = 0; j < entry.Assets.Count; ++j)
			{
				var asset = entry.Assets[j];
				DrawImageAssetListItem(asset);
			}
		}

		private void DrawImageAssetListItem(ImageListAsset file, bool enabled = true)
		{
			float usage = SpriteAtlasEntry.CalculateUsage(file) * 100;

			string text = $"[{file.Frames.Count}] [{file.FrameSize.X}x{file.FrameSize.Y}] {file.RelativePath} [{usage.ToString("F1")}%%]";

			if (enabled)
				ImGui.Text(text);
			else
				ImGui.TextDisabled(text);

		}

		private IEnumerable<ImageListAsset> DrawImageListAssetsFilter(string id)
		{
			IEnumerable<ImageListAsset> query = AppGame.AssetManager.ImageListAssets;

			ImGui.InputText($"##{id}", ref assetFilter, 256);

			if (!string.IsNullOrEmpty(assetFilter))
			{
				if (!assetFilter.Contains('*') &&
					!assetFilter.Contains('?') &&
					!assetFilter.Contains('[') &&
					!assetFilter.Contains('{'))

					query = query.Where(a => a.RelativePath.Contains(assetFilter, StringComparison.InvariantCultureIgnoreCase));
				else
				{
					try
					{
						var g = new Glob(assetFilter, GlobOptions.Compiled);
						query = query.Where(a => g.IsMatch(a.RelativePath));
					}
					catch { }
				}

			}


			return query;

		}

		private void DrawSelectItemsModal()
		{
			ImGui.OpenPopup("sag.entries.add");

			ImGui.BeginPopupModal("sag.entries.add", ref selectItemsModal, ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.AlwaysAutoResize);

			float total = modalSelectedAssets.Sum(s => SpriteAtlasEntry.CalculateUsage(s)) * 100;

			ImGui.Text($"{total.ToString("F1")}%%");
			ImGui.SameLine();

			var query = DrawImageListAssetsFilter("sag.entries.add.filter");

			ImGui.SameLine();

			if (ImGui.Button("Toggle Selection##sag.entries.add.toggle"))
			{
				foreach (var asset in query)
				{
					if (modalSelectedAssets.Contains(asset))
						modalSelectedAssets.Remove(asset);
					else
						modalSelectedAssets.Add(asset);
				}
			}

			query = query.OrderByDescending(asset => modalSelectedAssets.Contains(asset));

			int i = 0;

			ImGui.BeginChild("sag.entries.add.items", new Vector2(600, 600));

			foreach (var asset in query)
			{
				bool selected = modalSelectedAssets.Contains(asset);

				if (ImGui.Selectable($"##sag.select.{i++}", selected))
				{
					if (selected)
					{
						modalSelectedAssets.Remove(asset);
					}
					else
					{
						modalSelectedAssets.Add(asset);
					}
				}
				ImGui.SameLine();
				DrawImageAssetListItem(asset);
			}

			ImGui.EndChild();

			if (ImGui.Button("Cancel##sag.entries.add.cancel"))
			{
				selectItemsModal = false;
				modalSelectedAssets.Clear();
			}

			ImGui.SameLine();

			if (ImGui.Button("Ok##sag.entries.add.ok"))
			{
				selectItemsModal = false;
				selectItemsFor.SetAssets(modalSelectedAssets);
				modalSelectedAssets.Clear();
				changed = true;
			}

			ImGui.EndPopup();
		}

		private void NormalizeEntries()
		{
			List<ImageListAsset> assets = new List<ImageListAsset>();

			foreach (var entry in entries)
			{
				foreach (var a in entry.Assets.ToArray())
				{
					if (assets.Contains(a))
					{
						entry.RemoveAsset(a);
						changed = true;
					}
					else
					{
						assets.Add(a);
					}
				}
			}


		}

		private IEnumerator GenerateCrt()
		{
			foreach (var file in Directory.GetFiles(AssetManager.SpriteAtlasDir))
				File.Delete(file);

			int i = 0;
			foreach (var entry in entries)
			{
				if (!AppGui.ProgressDialog("Generating atlases", ++i, entries.Count, true))
					yield break;

				AppGame.AssetManager.ExportAtlas(entry.Assets, entry.OutputName);


				yield return null;
			}

			AppGame.AssetManager.ReloadSpriteAtlasAssets();
		}

		private IEnumerator BuildCrt()
		{
			foreach(var file in Directory.GetFiles(AssetManager.SpriteBuildDir))
				File.Delete(file);

			int i = 0;

			foreach (var atlas in AppGame.AssetManager.SpriteAtlasAssets)
			{
				var op = AppGame.AssetManager.BuildAtlas(atlas);

				while (!op.Completed)
				{
					if (!AppGui.ProgressDialog($"Building atlas: {atlas.Name}", i, entries.Count, true))
					{
						op.Cancel();
						yield break;

					}

					yield return null;
				}

				++i;
			}
		}
	}

}
