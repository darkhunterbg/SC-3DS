using DataManager.Assets;
using DataManager.Widgets;
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

		private AssetTableEditor<SpriteAsset> table = new AssetTableEditor<SpriteAsset>("table");

		private bool changed = false;

		private TreeView treeView = new TreeView("tree")
		{
			DataSource = AppGame.AssetManager.GetAssets<SpriteAsset>(),

		};

		public SpriteEditor()
		{
			treeView.NewItemAction = NewItem;
			treeView.DeleteItemAction = DeleteItem;
		}

		private ITreeViewItem NewItem(ITreeViewItem item)
		{
			var asset = item as SpriteAsset;
			changed = true;
			return AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().New(asset);
		}

		private void DeleteItem(ITreeViewItem item)
		{
			var asset = item as SpriteAsset;
			changed = true;
			AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().Delete(asset);
		}

		public void Draw(Vector2 client)
		{
			changed = false;

			treeView.Draw();

			if (changed)
				AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().Save();
			//table.Draw();
			//table.SaveChanges();
		}
	}
}
