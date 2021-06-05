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
		private AssetTableEditor<FlingyAsset> table = new AssetTableEditor<FlingyAsset>("##fe.table");

		public void Draw(Vector2 clientSize)
		{
			if (!ImGui.Begin("Flingy Editor##fe"))
				return;

			table.Draw();

			ImGui.End();

			table.SaveChanges();
		}
	}
}
