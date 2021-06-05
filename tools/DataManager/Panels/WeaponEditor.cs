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
	public class WeaponEditor : IGuiPanel
	{
		private AssetTableEditor<WeaponAsset> table = new AssetTableEditor<WeaponAsset>("#we.table");

		public void Draw(Vector2 clientSize)
		{
			if (!ImGui.Begin("Weapon Editor##we"))
				return;

			table.Draw();

			ImGui.End();

			table.SaveChanges();
		}
	}
}
