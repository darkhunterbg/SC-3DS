﻿using DataManager.Assets;
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
	public class ImageEditor
	{
		private bool changed = false;
		private string filter = string.Empty;
		private ImageAsset hoverItem = null;
		private ImageAsset tableSelection = null;

		public void Draw()
		{
			changed = false;
			hoverItem = null;

			ImGui.SetNextWindowSize(new Vector2(800, 600), ImGuiCond.FirstUseEver);
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

			IEnumerable<ImageAsset> query = QueryData(filter);

			int i = 0;

			foreach (var item in query)
			{
				++i;
				ImGui.TableNextRow();
				TableRow(i, item);
			}

			ImGui.EndTable();

			ImGui.End();

			DrawHoverItemTooltip();
			if (changed)
			{
				AppGame.AssetManager.SaveImages();
			}
		}

		private void TableRow(int i, ImageAsset item)
		{

			ImGui.TableNextColumn();
			ImGui.Text(item.SpriteSheetName);
			if (ImGui.IsItemHovered())
			{
				hoverItem = item;
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

		private IEnumerable<ImageAsset> QueryData(string f)
		{
			IEnumerable<ImageAsset> query = GetImages(f);

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

		public static IEnumerable<ImageAsset> GetImages(string f)
		{
			IEnumerable<ImageAsset> query = AppGame.AssetManager.Images;

			if (!string.IsNullOrEmpty(f))
			{
				if (!f.Contains('*') &&
					!f.Contains('?') &&
					!f.Contains('[') &&
					!f.Contains('{'))

					query = query.Where(a => a.SpriteSheetName.Contains(f, StringComparison.InvariantCultureIgnoreCase));
				else
				{
					try
					{
						var g = new Glob(f, GlobOptions.Compiled);
						query = query.Where(a => g.IsMatch(a.SpriteSheetName));
					}
					catch { }
				}

			}

			return query;
		}

		private void DrawHoverItemTooltip()
		{
			if (hoverItem == null)
			{
				return;
			}

			ImGui.BeginTooltip();

			AppGui.DrawSpriteSheetInfo(hoverItem.SpriteSheet);

			ImGui.EndTooltip();
		}
	}
}
