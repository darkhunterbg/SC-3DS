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
	public class UnitEditor :  IGuiPanel
	{
		private AssetTableEditor<UnitAsset> table = new AssetTableEditor<UnitAsset>("#ue.table");

		public void Draw(Vector2 clientSize)
		{
			if (!ImGui.Begin("Unit Editor##ue"))
				return;

			table.Draw();

			ImGui.End();

			table.SaveChanges();
		}

	}
}
