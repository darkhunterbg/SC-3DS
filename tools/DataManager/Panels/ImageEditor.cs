using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
	public class ImageEditor
	{
		public void Draw()
		{
			ImGui.SetNextWindowSize(new Vector2(800, 600), ImGuiCond.FirstUseEver);
			ImGui.Begin("Image Editor##ie");


			ImGui.BeginTable("##ie.items", 3);
			ImGui.TableSetupColumn("Name");
			ImGui.TableSetupColumn("Id");
			ImGui.TableHeadersRow();

			ImGui.EndTable();

			ImGui.End();
		}
	}
}
