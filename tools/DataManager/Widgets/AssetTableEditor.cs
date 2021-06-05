using DataManager.Assets;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Widgets
{
	public class AssetTableEditor<TAsset>: TableEditor<TAsset> where TAsset : Asset
	{
		public AssetTableEditor(string id) : base(id)
		{
			DataSource = AppGame.AssetManager.GetAssetDatabase<TAsset>().Assets;
			OnNewItem = Asset.New;
		}

		public void SaveChanges()
		{
			if(HasChanges)
				AppGame.AssetManager.GetAssetDatabase<TAsset>().Save();
		}
	}
}
