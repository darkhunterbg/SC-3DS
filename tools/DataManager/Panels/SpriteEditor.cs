using DataManager.Assets;
using GlobExpressions;
using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Panels
{
	public class SpriteEditor
	{
		private bool changed = false;
		private string filter = string.Empty;
		private string modalFilter = string.Empty;
		private SpriteAsset hoverItem = null;

		private SpriteAsset changeItemImage = null;
		private ImageAsset tmp = null;
		private SpriteAsset tableSelection = null;

		private string[] SelectionTypes =
		{
			"22px","32px","46px", "62px","72px","94px","110px","122px","146px","224px",
		};

		public void Draw(Vector2 clientSize)
		{
			changed = false;
			hoverItem = null;

			ImGui.SetNextWindowSize(new Vector2(800, 600), ImGuiCond.FirstUseEver);
			ImGui.Begin("Sprite Editor##se");

			ImGui.InputText("##se.filter", ref filter, 255);

			if (ImGui.Button("New Sprite##se.new"))
			{
				CreateNewSprite();
			}

			if (!ImGui.BeginTable("##se.items", 6, ImGuiTableFlags.Sortable | ImGuiTableFlags.BordersInnerH
					| ImGuiTableFlags.ScrollY | ImGuiTableFlags.RowBg | ImGuiTableFlags.Resizable
				))
				return;

			ImGui.TableSetupScrollFreeze(0, 1);
			ImGui.TableSetupColumn("Name");
			ImGui.TableSetupColumn("Image");
			ImGui.TableSetupColumn("Bar Size");
			ImGui.TableSetupColumn("Selection");
			ImGui.TableSetupColumn("Selection Y");
			ImGui.TableHeadersRow();

			IEnumerable<SpriteAsset> query = QueryData();

			int i = 0;

			foreach (var item in query.ToList())
			{
				++i;
				ImGui.TableNextRow();

				TableRow(i, item);

			}

			ImGui.EndTable();

			ImGui.SameLine();

			ImGui.End();

			DrawHoverItemTooltip();
			DrawChangeImageModal();


			if (changed)
			{
				AppGame.AssetManager.SaveSprites();
			}
		}

		private void CreateNewSprite()
		{
			var sprite = new SpriteAsset()
			{
				Name = "New Sprite"

			};
			sprite.OnAfterDeserialize(AppGame.AssetManager.Images.FirstOrDefault());

			AppGame.AssetManager.Sprites.Add(sprite);
			changed = true;
		}

		private void TableRow(int i, SpriteAsset item)
		{
			string text = string.Empty;
			int number = 0;



			ImGui.TableNextColumn();
			text = item.Name;

			AppGui.StrechNextItem();
			if (ImGui.InputText($"##se.items.{i}.name", ref text, 256))
			{
				item.Name = text;
				changed = true;
			}
			ImGui.TableNextColumn();


			ImGui.Text(item.ImageName);

			if (ImGui.IsItemHovered())
			{
				hoverItem = item;
			}
			ImGui.SameLine();

			if (ImGui.Button($"Change##se.items.{i}.changeimage"))
			{
				changeItemImage = item;
				tmp = changeItemImage.Image;
			}

			ImGui.TableNextColumn();
			number = item.BarSize;

			AppGui.StrechNextItem();
			if (ImGui.InputInt($"##se.items.{i}.barsize", ref number))
			{
				item.BarSize = number;
				changed = true;
			}

			ImGui.TableNextColumn();
			number = item.SelectionType;

			AppGui.StrechNextItem();

			if (ImGui.Combo($"##se.items.{i}.seltype", ref number, SelectionTypes, SelectionTypes.Length))
			{
				item.SelectionType = number;
				changed = true;
			}

			ImGui.TableNextColumn();
			number = item.SelectionY;
			AppGui.StrechNextItem();
			if (ImGui.InputInt($"##se.items.{i}.sely", ref number))
			{
				item.SelectionY = number;
				changed = true;
			}

			ImGui.TableNextColumn();
			if (ImGui.Button($"Delete##se.items.{i}.del"))
			{
				AppGame.AssetManager.Sprites.Remove(item);
				changed = true;
			}

			bool selected = tableSelection == item;

			ImGui.SameLine();

			if (ImGui.Selectable($"##se.items.{i}.selected", ref selected, ImGuiSelectableFlags.SpanAllColumns))
			{
				if (selected)
					tableSelection = item;
				else
					tableSelection = null;
			}
		}

		private IEnumerable<SpriteAsset> QueryData()
		{
			IEnumerable<SpriteAsset> query = AppGame.AssetManager.Sprites;

			if (!string.IsNullOrEmpty(filter))
			{
				if (!filter.Contains('*') &&
					!filter.Contains('?') &&
					!filter.Contains('[') &&
					!filter.Contains('{'))

					query = query.Where(a => a.Name.Contains(filter, StringComparison.InvariantCultureIgnoreCase));
				else
				{
					try
					{
						var g = new Glob(filter, GlobOptions.Compiled);
						query = query.Where(a => g.IsMatch(a.Name));
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
							query.OrderBy(q => q.Name) : query.OrderByDescending(q => q.Name);
						break;
					}
				case 1:
					{
						query = sort.Specs.SortDirection == ImGuiSortDirection.Ascending ?
							query.OrderBy(q => q.ImageName) : query.OrderByDescending(q => q.ImageName);
						break;
					}

				case 2:
					{
						query = sort.Specs.SortDirection == ImGuiSortDirection.Ascending ?
							query.OrderBy(q => q.BarSize) : query.OrderByDescending(q => q.BarSize);
						break;
					}
				case 3:
					{
						query = sort.Specs.SortDirection == ImGuiSortDirection.Ascending ?
							query.OrderBy(q => q.SelectionType) : query.OrderByDescending(q => q.SelectionType);
						break;
					}
				case 4:
					{
						query = sort.Specs.SortDirection == ImGuiSortDirection.Ascending ?
							query.OrderBy(q => q.SelectionY) : query.OrderByDescending(q => q.SelectionY);
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

			if (hoverItem.Image != null)
				AppGui.DrawSpriteSheetInfo(hoverItem.Image.SpriteSheet);

			ImGui.EndTooltip();
		}

		private void DrawChangeImageModal()
		{
			bool opened = changeItemImage != null;

			if (!opened)
				return;

			ImGui.OpenPopup("se.image.change");

			if (!ImGui.BeginPopupModal("se.image.change", ref opened, ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.AlwaysAutoResize))
				return;

			ImGui.Text(tmp?.SpriteSheetName ?? string.Empty);

			ImGui.InputText("##se.image.change.filter", ref modalFilter, 255);

			var query = ImageEditor.GetImages(modalFilter);

			query = query.OrderBy(asset => asset.SpriteSheetName);

			int i = 0;

			ImGui.BeginChild("se.image.change.items", new Vector2(600, 600));

			foreach (var asset in query)
			{
				bool selected = tmp == asset;

				if (ImGui.Selectable($"##sag.select.{i++}", selected))
				{
					if (selected)
						tmp = null;
					else
						tmp = asset;

				}
				ImGui.SameLine();
				ImGui.Text(asset.SpriteSheetName);
				if (ImGui.IsItemHovered())
				{
					ImGui.BeginTooltip();

					AppGui.DrawSpriteSheetInfo(asset.SpriteSheet);

					ImGui.EndTooltip();
				}
			}

			ImGui.EndChild();

			if (ImGui.Button("Cancel##se.image.change.items.cancel"))
			{
				tmp = null;
				changeItemImage = null;
				modalFilter = string.Empty;
			}

			ImGui.SameLine();

			if (tmp != null)
			{
				if (ImGui.Button("Ok##se.image.change.items.ok"))
				{
					changeItemImage.Image = tmp;
					tmp = null;
					changeItemImage = null;
					changed = true;
					modalFilter = string.Empty;
				}
			}
			ImGui.EndPopup();
		}
	}
}
