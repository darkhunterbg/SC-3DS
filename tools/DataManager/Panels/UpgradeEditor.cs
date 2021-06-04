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
	public class UpgradeEditor : IGuiPanel
	{
		private TableEditor<UpgradeAsset> table = new TableEditor<UpgradeAsset>("#ue.table");

		public UpgradeEditor()
		{
			table.DataSource = AppGame.AssetManager.Upgrades;
			table.OnNewItem = (c) =>
			{
				if (c != null)
				{
					return Util.ShallowCopyProperties(c);
				}
				else
				{
					return new UpgradeAsset(AppGame.AssetManager.Icons.FirstOrDefault());
				}
			};
		}

		public void Draw(Vector2 clientSize)
		{
			ImGui.SetNextWindowSize(new Vector2(800, 600), ImGuiCond.FirstUseEver);
			if (!ImGui.Begin("Upgrade Editor##ue"))
				return;

			table.Draw();

			ImGui.End();

			if (table.HasChanges)
				AppGame.AssetManager.SaveUpgrades();
		}
	}
}
