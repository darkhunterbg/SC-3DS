using DataManager.Assets;
using DataManager.Assets.Raw;
using DataManager.Build;
using DataManager.Widgets;
using GlobExpressions;
using ImGuiNET;
using Newtonsoft.Json;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{

	public class SpriteAtlasGenerator : IGuiPanel
	{
		private List<SpriteAtlasEntry> entries = new List<SpriteAtlasEntry>();

		public static readonly string SettingsFileName = "sprite_atlases";

		bool changed = false;

		SpriteAtlasEntry selectItemsFor = null;
		int selectEditDirIndex = -1;

		string assetFilter = string.Empty;

		public string WindowName => "Atlas Generator";

		public SpriteAtlasGenerator()
		{
			entries = AppSettings.Load<List<SpriteAtlasEntry>>(SettingsFileName) ??
				new List<SpriteAtlasEntry>();

			foreach (var e in entries)
				e.ReloadAssets();

		}

		public void Draw(Vector2 clientSize)
		{
			changed = false;

			ImGui.Columns(2);

			DrawEntries();

			ImGui.NextColumn();

			DrawImageListAssets();

			if (selectItemsFor != null)
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


			ImGui.SameLine();
			if (ImGui.Button("Build All##sag.entries.buildall"))
			{
				AppGame.RunCoroutine(BuildCrt(true));
			}
			ImGui.SameLine();
			if (ImGui.Button("Build PC##sag.entries.buildall"))
			{
				AppGame.RunCoroutine(BuildCrt(false));
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
				ImGui.SetNextItemWidth(300);
				if (ImGui.InputText($"##sag.entries.path.{entry.Id}", ref text, 256))
				{
					entry.OutputName = text;
					changed = true;
				}

				ImGui.SameLine();
				int number = (int)entry.PackStrategy;
				var enumValues = EnumCacheValues.GetValues(typeof(SpriteAtlasPackStrategy));
				ImGui.SetNextItemWidth(200);
				if (ImGui.Combo($"##sag.entries.packstrat.{entry.Id}", ref number, enumValues, enumValues.Length))
				{
					entry.PackStrategy = (SpriteAtlasPackStrategy)number;
					changed = true;
				}

				ImGui.SameLine();
				number = (int)entry.FilterStrategy;
				enumValues = EnumCacheValues.GetValues(typeof(SpriteAtlasFilterStrategy));
				ImGui.SetNextItemWidth(200);
				if (ImGui.Combo($"##sag.entries.filterStart.{entry.Id}", ref number, enumValues, enumValues.Length))
				{
					entry.FilterStrategy = (SpriteAtlasFilterStrategy)number;
					changed = true;
				}


				ImGui.SameLine();
				ImGui.Text($"Atlas Size {(int)(entry.Usage * 100)}%% ");

				ImGui.SameLine();

				ImGui.SameLine();
				if (ImGui.Button($"Delete##sag.entries.del.{entry.Id}"))
				{
					entries.RemoveAt(i--);
					changed = true;
					continue;
				}


				if (expand)
				{
					DrawEntryExtendedItem(entry);

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

		private void DrawEntryExtendedItem(SpriteAtlasEntry entry)
		{
			int i = 0;

			for (int j = 0; j < entry.Directories.Count; ++j)
			{
				var path = entry.Directories[j];

				ImGui.PushID(++i);

				ImGui.Text(path);
				ImGui.SameLine();

				ImGui.SameLine();
				if (ImGui.Button($"Edit##sag.entries.edit.{entry.Id}"))
				{
					selectEditDirIndex = j;
					selectItemsFor = entry;
					modalSearchText = selectItemsFor.Directories[selectEditDirIndex];
				}

				ImGui.SameLine();
				if (ImGui.Button($"Delete##sag.entries.delete.{entry.Id}"))
				{
					entry.Directories.Remove(path);
				}

				ImGui.PopID();
			}

			if (ImGui.Button($"New Entry##sag.entries.add.{entry.Id}"))
			{
				selectEditDirIndex = -1;
				selectItemsFor = entry;
			}


		}

		private void DrawImageAssetListItem(ImageList file, bool enabled = true)
		{
			string text = $"[{file.Frames.Count}] [{file.FrameSize.X}x{file.FrameSize.Y}] {file.Key}";

			if (enabled)
				ImGui.Text(text);
			else
				ImGui.TextDisabled(text);

			if (ImGui.IsItemHovered())
				AppGame.Gui.HoverObject = file;

		}

		private IEnumerable<ImageList> DrawImageListAssetsFilter(string id)
		{
			IEnumerable<ImageList> query = Util.TextFilter(AppGame.AssetManager.ImageLists, assetFilter, a => a.Key);

			ImGui.InputText($"##{id}", ref assetFilter, 256);

			return query;

		}

		string modalSearchText = string.Empty;
		private void DrawSelectItemsModal()
		{
			ImGui.OpenPopup("sag.entries.edit");
			bool opened = selectItemsFor != null;
			ImGui.BeginPopupModal("sag.entries.edit", ref opened, ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.AlwaysAutoResize);


			ImGui.InputText($"##sag.entries.edit.filter", ref modalSearchText, 256);

			IEnumerable<ImageList> query = Util.TextFilter(AppGame.AssetManager.ImageLists, modalSearchText, a => a.Key, false);

			ImGui.BeginChild("sag.entries.edit.items", new Vector2(600, 600));

			ImGui.Text($"{query.Count()} entries.");

			foreach (var asset in query)
			{
				DrawImageAssetListItem(asset);
			}

			ImGui.EndChild();

			if (ImGui.Button("Cancel##sag.entries.edit.cancel"))
			{
				selectItemsFor = null;
				modalSearchText = string.Empty;
			}

			ImGui.SameLine();

			if (!string.IsNullOrEmpty(modalSearchText))
			{
				if (ImGui.Button("Ok##sag.entries.edit.ok"))
				{

					if (selectEditDirIndex != -1)
						selectItemsFor.Directories.Remove(selectItemsFor.Directories[selectEditDirIndex]);
					selectItemsFor.Directories.Add(modalSearchText);
					selectItemsFor.Directories = selectItemsFor.Directories.OrderBy(t => t).ToList();
					selectItemsFor.ReloadAssets();
					//selectItemsFor.SetAssets(modalSelectedAssets);
					modalSearchText = string.Empty;
					changed = true;
					selectItemsFor = null;

				}
			}
			ImGui.EndPopup();
		}

		private IEnumerator BuildCrt(bool build3DS)
		{
			Stopwatch timer = new Stopwatch();
			timer.Start();

			BuildGenerator build = new BuildGenerator() { Build3DS = build3DS };


			foreach (var file in Directory.GetFiles(AssetManager.SpriteAtlas3DSBuildDir))
				File.Delete(file);


			var op = build.BuildAtlases(entries);


			while (!op.Completed)
			{
				var elapsed = TimeSpan.FromSeconds((int)timer.Elapsed.TotalSeconds);

				if (!AppGui.ProgressDialog($"Building: {op.ItemName} ({elapsed.ToString("c")}) ", op.Progress, true))
				{
					op.Cancel();
					break;

				}
				yield return null;
			}

			build.Dispose();

		}
	}

}
