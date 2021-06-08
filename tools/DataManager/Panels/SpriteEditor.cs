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

		private TreeView treeView = new TreeView("tree")
		{

			DataSource = AppGame.AssetManager.GetAssets<SpriteAsset>()
		};



		public void Draw(Vector2 client)
		{
			treeView.Draw();

			//table.Draw();
			//table.SaveChanges();
		}
	}
}
