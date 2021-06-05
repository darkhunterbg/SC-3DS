using DataManager.Assets;
using GlobExpressions;
using ImGuiNET;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
	public class ImageEditor : IGuiPanel
	{
		private bool changed = false;
		private string filter = string.Empty;
		private LogicalImageAsset tableSelection = null;

		public void Draw(Vector2 clientSize)
		{
			changed = false;
	
			if (!ImGui.Begin("Image Editor##ie"))
				return;


			ImGui.InputText("##ie.filter", ref filter, 255);


			if (!ImGui.BeginTable("##ie.items", 3, ImGuiTableFlags.Sortable | ImGuiTableFlags.PreciseWidths
				| ImGuiTableFlags.SizingStretchProp
					| ImGuiTableFlags.ScrollY | ImGuiTableFlags.RowBg | ImGuiTableFlags.Resizable))
				return;

			ImGui.TableSetupScrollFreeze(0, 1);
			ImGui.TableSetupColumn("Sheet");
			ImGui.TableSetupColumn("Graphics Turn");
			ImGui.TableSetupColumn("Unit Color");
			ImGui.TableHeadersRow();

			IEnumerable<LogicalImageAsset> query = QueryData(filter);

			int i = 0;

			foreach (var item in query)
			{
				++i;
				ImGui.TableNextRow();
				TableRow(i, item);
			}

			ImGui.EndTable();

			ImGui.End();

			if (changed)
			{
				AppGame.AssetManager.SaveImages();
			}
		}

		private void TableRow(int i, LogicalImageAsset item)
		{

			ImGui.TableNextColumn();
			ImGui.Text(item.SpriteSheetName);
			if (ImGui.IsItemHovered())
			{
				AppGame.Gui.HoverObject = item;
			}

			ImGui.TableNextColumn();
			bool value = item.GraphicsTurns;
			if (ImGui.Checkbox($"##ie.items.{i}.graphicsturn", ref value))
			{
				item.GraphicsTurns = value;
				changed = true;
			}


			ImGui.TableNextColumn();
			value = item.UnitColor;
			if (ImGui.Checkbox($"##ie.items.{i}.unitcolor", ref value))
			{
				item.UnitColor = value;
				changed = true;
			}

			bool selected = tableSelection == item;

			ImGui.SameLine();

			if (ImGui.Selectable($"##ie.items.{i}.selected", ref selected, ImGuiSelectableFlags.SpanAllColumns))
			{
				if (selected)
					tableSelection = item;
				else
					tableSelection = null;
			}
		}

		private IEnumerable<LogicalImageAsset> QueryData(string f)
		{
			IEnumerable<LogicalImageAsset> query = GetImages(f);

			var sort = ImGui.TableGetSortSpecs();

			switch (sort.Specs.ColumnIndex)
			{
				case 0:
					{
						query = sort.Specs.SortDirection == ImGuiSortDirection.Ascending ?
							query.OrderBy(q => q.SpriteSheetName) : query.OrderByDescending(q => q.SpriteSheetName);
						break;
					}
				case 1:
					{
						query = sort.Specs.SortDirection == ImGuiSortDirection.Ascending ?
							query.OrderBy(q => q.GraphicsTurns) : query.OrderByDescending(q => q.GraphicsTurns);
						break;
					}

				case 2:
					{
						query = sort.Specs.SortDirection == ImGuiSortDirection.Ascending ?
							query.OrderBy(q => q.UnitColor) : query.OrderByDescending(q => q.UnitColor);
						break;
					}

			}

			return query;
		}

		public static IEnumerable<LogicalImageAsset> GetImages(string f)
		{
			return Util.TextFilter(AppGame.AssetManager.Images, f, a => a.SpriteSheetName);
		}

	}
}
