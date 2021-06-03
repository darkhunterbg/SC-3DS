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
	public class ImageEditor
	{
		private bool changed = false;
		private string filter = string.Empty;
		private GameImageAsset hoverItem = null;

		public void Draw()
		{
			changed = false;
			hoverItem = null;

			ImGui.SetNextWindowSize(new Vector2(800, 600), ImGuiCond.FirstUseEver);
			ImGui.Begin("Image Editor##ie");


			ImGui.InputText("##ie.filter", ref filter, 255);


			if (!ImGui.BeginTable("##ie.items", 3, ImGuiTableFlags.Sortable | ImGuiTableFlags.BordersInnerH
					| ImGuiTableFlags.ScrollY | ImGuiTableFlags.RowBg | ImGuiTableFlags.Resizable))
				return;

			ImGui.TableSetupScrollFreeze(0, 1);
			ImGui.TableSetupColumn("Sheet");
			ImGui.TableSetupColumn("Graphics Turn");
			ImGui.TableSetupColumn("Unit Color");
			ImGui.TableHeadersRow();

			IEnumerable<GameImageAsset> query = QueryData();

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

		private void TableRow(int i, GameImageAsset item)
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
		}

		private IEnumerable<GameImageAsset> QueryData()
		{
			IEnumerable<GameImageAsset> query = AppGame.AssetManager.Images;

			if (!string.IsNullOrEmpty(filter))
			{
				if (!filter.Contains('*') &&
					!filter.Contains('?') &&
					!filter.Contains('[') &&
					!filter.Contains('{'))

					query = query.Where(a => a.SpriteSheetName.Contains(filter, StringComparison.InvariantCultureIgnoreCase));
				else
				{
					try
					{
						var g = new Glob(filter, GlobOptions.Compiled);
						query = query.Where(a => g.IsMatch(a.SpriteSheetName));
					}
					catch { }
				}

			}


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

		private void DrawHoverItemTooltip()
		{
			if (hoverItem == null)
			{
				return;
			}

			ImGui.BeginTooltip();

			ImGui.Text($"Dimensions: {hoverItem.SpriteSheet.Width}x{hoverItem.SpriteSheet.Height}");
			ImGui.SameLine();


			if (hoverItem.SpriteSheet.SubAtlas != null)
			{
				ImGui.Text($"Frames: {hoverItem.SpriteSheet.Frames}");

				for (int i = 0; i < hoverItem.SpriteSheet.Frames; ++i)
				{
					var tex = AppGame.AssetManager.GetSheetImage(hoverItem.SpriteSheetName, i);

					if (i % 16 > 0)
						ImGui.SameLine();

					ImGui.Image(tex.GuiImage, new Vector2(tex.Texture.Width, tex.Texture.Height));
					
				}

			}

			//if (loaded != null)
			//{
			//	ImGui.Text($"Dimensions: {loaded.MaxWidth}x{loaded.MaxHeight}");
			//	ImGui.Text($"Frames: {loaded.NumberOfFrames}");

			//	Vector2 uv = new Vector2(loaded.Preview.Width, loaded.Preview.Height);
			//	Vector2 size = new Vector2(loaded.MaxWidth, loaded.MaxHeight);
			//	int multiplier = 1;
			//	if (size.X <= 128 && size.Y <= 128)
			//		multiplier = 2;
			//	if (size.X <= 64 && size.Y <= 64)
			//		multiplier = 4;
			//	//if (size.X <= 32 && size.Y <= 32)
			//	//	multiplier = 8;

			//	ImGui.Image(previewTexture.GuiImage, uv * multiplier, Vector2.Zero, uv / previewTexture.Texture.Width);
			//}
			//else
			//{
			//	ImGui.TextColored(Microsoft.Xna.Framework.Color.Red.ToVec4(), "Failed to load image data.");
			//}

			ImGui.EndTooltip();
		}
	}
}
