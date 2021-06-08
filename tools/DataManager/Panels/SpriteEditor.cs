using DataManager.Assets;
using DataManager.Widgets;
using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
	public class SpriteEditor : IGuiPanel
	{
		public string WindowName => "Sprite Editor";

		private TreeView treeView = new TreeView("tree")
		{
			DataSource = AppGame.AssetManager.GetAssets<SpriteAsset>(),
			ItemName = "Sprite",
			RootName = "Sprites"
		};

		public SpriteEditor()
		{
			treeView.NewItemAction = NewItem;
			treeView.DeleteItemAction = DeleteItem;
		}

		private ITreeViewItem NewItem(ITreeViewItem item)
		{
			var asset = item as SpriteAsset;
			return AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().New(asset);
		}

		private void DeleteItem(ITreeViewItem item)
		{
			var asset = item as SpriteAsset;
			AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().Delete(asset);
		}

		bool first = true;

		public void Draw(Vector2 client)
		{

			ImGui.Columns(2);

			if (first)
			{
				ImGui.SetColumnOffset(0, -300);
				first = false;
			}

			ImGui.BeginChild("##items");

			treeView.Draw();

			ImGui.EndChild();

			ImGui.BeginChild("##settings");

			ImGui.NextColumn();

			ImGui.EndChild();

			if (treeView.ItemModified)
				AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().Save();
		}
	}
}
