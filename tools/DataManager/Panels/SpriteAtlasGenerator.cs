using ImGuiNET;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
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

		[JsonProperty]
		public List<string> ImageList = new List<string>();

		[JsonIgnore]
		public List<ImageListAsset> Assets = new List<ImageListAsset>();

		[JsonIgnore]
		public readonly int Id = -1;

		static int id = 0;

		public SpriteAtlasEntry()
		{
			Id = ++id;
		}

		public void Init()
		{
			
			Assets.Clear();

			foreach (var l in ImageList)
			{

				var item = AppGame.AssetManager.ImageListAssets.FirstOrDefault(a => a.RelativePath == l);
				if (item == null || Assets.Contains(item))
					continue;

				Assets.Add(item);
			}

			Assets = Assets.OrderBy(a => a.RelativePath).ToList();
			ImageList.Sort();
		}

		public void SetAssets(IEnumerable<ImageListAsset> assets)
		{
			ImageList.Clear();
			Assets.Clear();


			Assets.AddRange(assets.Distinct());
			Assets = Assets.OrderBy(a => a.RelativePath).ToList();

			ImageList.AddRange(Assets.Select(s => s.RelativePath));

			ImageList.Sort();
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

			foreach (var asset in AppGame.AssetManager.ImageListAssets)
			{
				bool enabled = entries.Any(e => e.Assets.Contains(asset));
				DrawImageAssetListItem(asset, enabled);

			}

			ImGui.EndChild();
		}

		private void DrawEntries()
		{
			ImGui.BeginChild("##sag.entries");

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
					DrawEntryAssetList( entry);

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
		}

		private void DrawEntryAssetList( SpriteAtlasEntry entry)
		{
			for (int j = 0; j < entry.Assets.Count; ++j)
			{
				var asset = entry.Assets[j];
				DrawImageAssetListItem(asset);
			}
		}

		private void DrawImageAssetListItem(ImageListAsset file, bool enabled = true)
		{
			if (enabled)
				ImGui.Text($"[{file.Frames.Count}]");
			else
				ImGui.TextDisabled($"[{file.Frames.Count}]");
			ImGui.SameLine();
			if (enabled)
				ImGui.Text(file.RelativePath);
			else
				ImGui.TextDisabled(file.RelativePath);
		}

		private void DrawSelectItemsModal()
		{
			ImGui.OpenPopup("sag.entries.add");


			ImGui.BeginPopupModal("sag.entries.add", ref selectItemsModal, ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.AlwaysAutoResize);


			int i = 0;

			ImGui.BeginChild("sag.entries.add.items", new Vector2(600, 600));

			foreach (var asset in AppGame.AssetManager.ImageListAssets)
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

			if (ImGui.Button("Cancel"))
			{
				selectItemsModal = false;
				modalSelectedAssets.Clear();
			}

			ImGui.SameLine();

			if (ImGui.Button("Ok"))
			{
				selectItemsModal = false;
				selectItemsFor.SetAssets(modalSelectedAssets);
				modalSelectedAssets.Clear();
				changed = true;
			}

			ImGui.EndPopup();
		}

	}

}
