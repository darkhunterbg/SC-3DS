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
	public class FlingyEditor : IGuiPanel
	{
		private TableEditor<FlingyAsset> table = new TableEditor<FlingyAsset>("##fe.table");

		public FlingyEditor()
		{
			table.DataSource = AppGame.AssetManager.Flingy ;
			table.OnNewItem = (c) =>
			{
				if (c != null)
				{
					return Util.ShallowCopyProperties(c);
				}
				else
				{
					return new FlingyAsset(AppGame.AssetManager.Sprites.FirstOrDefault());
				}
			};
		}

		public void Draw(Vector2 clientSize)
		{
			if (!ImGui.Begin("Flingy Editor##fe"))
				return;

			table.Draw();

			ImGui.End();

			if (table.HasChanges)
				AppGame.AssetManager.SaveFlingy();
		}
	}
}
