﻿using DataManager.Assets;
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
		private AssetTableEditor<UpgradeAsset> table = new AssetTableEditor<UpgradeAsset>("#ue.table");

		public void Draw(Vector2 clientSize)
		{
			if (!ImGui.Begin("Upgrade Editor##ue"))
				return;

			table.Draw();

			ImGui.End();

			table.SaveChanges();
		}
	}
}
