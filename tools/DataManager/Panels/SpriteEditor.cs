using DataManager.Assets;
using DataManager.Widgets;
using ImGuiNET;
using Microsoft.Xna.Framework.Graphics;
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
		private SpriteAsset itemPreview = null;

		private bool first = true;

		private string[] SelectionTypes =
		{
			"22px","32px","46px", "62px","72px","94px","110px","122px","146px","224px",
		};

		private Microsoft.Xna.Framework.Color BarColor = new Microsoft.Xna.Framework.Color(0xff249824);
		private RenderTargetImage spritePreview = new RenderTargetImage(new Vector2(256, 256));

		public void Draw(Vector2 clientSize)
		{
			changed = false;
			hoverItem = null;

			ImGui.SetNextWindowSize(new Vector2(800, 600), ImGuiCond.FirstUseEver);
			if (!ImGui.Begin("Sprite Editor##se"))
				return;


			ImGui.Columns(2, "se.columns");

			if (first)
				ImGui.SetColumnWidth(0, clientSize.X - 512);

			first = false;

			ImGui.InputText("##se.filter", ref filter, 255);

			if (ImGui.Button("New Sprite##se.new"))
			{
				CreateNewSprite();
			}

			DrawTable();


			ImGui.NextColumn();


			float width = ImGui.GetColumnWidth();

			DrawSpritePreview((int)width);

			ImGui.End();

			DrawHoverItemTooltip();
			DrawChangeImageModal();


			if (changed)
			{
				AppGame.AssetManager.SaveSprites();
			}
		}

		private void DrawSpritePreview(int width)
		{
			var sb = spritePreview.BeginDraw();
			sb.Begin(SpriteSortMode.Deferred, null, SamplerState.PointClamp);
			Vector2 center = spritePreview.Size / 2;
			Vector2 pos = center;

			if (itemPreview != null)
			{
				int frame = 0;
				if (itemPreview.Image.GraphicsTurns)
					frame += 12;

				var sprite = AppGame.AssetManager.GetSheetImage(itemPreview.Image.SpriteSheetName, frame);

				if (itemPreview.SelectionType >= 0)
				{
					string sheet = SelectionTypes[itemPreview.SelectionType];
					sheet = sheet.Substring(0, sheet.Length - 2);
					if (sheet.Length < 3)
						sheet = $"0{sheet}";
					sheet = $"unit\\thingy\\o{sheet}";
					var selection = AppGame.AssetManager.GetSheetImage(sheet, 0);

					pos = center - selection.TextureSize / 2;
					pos.Y += itemPreview.SelectionY;
					sb.Draw(selection.Texture, pos.ToVector2(), Microsoft.Xna.Framework.Color.LightGreen);
				}

				pos = center - sprite.TextureSize / 2;

				sb.Draw(sprite.Texture, pos.ToVector2(), Microsoft.Xna.Framework.Color.White);

				if (itemPreview.Image.UnitColor)
				{
					frame = itemPreview.Image.SpriteSheet.GetUnitColorFrameIndex(frame);
					sprite = AppGame.AssetManager.GetSheetImage(itemPreview.Image.SpriteSheetName, frame);
					sb.Draw(sprite.Texture, pos.ToVector2(), Microsoft.Xna.Framework.Color.Magenta);
				}


				if (itemPreview.BarSize > 0)
				{
					int barWidth = itemPreview.BarSize * 3 + 1;

					Vector2 size = new Vector2(barWidth, 5);
					pos = center - size / 2;
					pos.Y += sprite.TextureSize.Y / 2 + itemPreview.SelectionY;

					sb.DrawRectangle(pos, size, Microsoft.Xna.Framework.Color.Black);

					pos += new Vector2(1, 1);

					for (int i = 0; i < itemPreview.BarSize; ++i)
					{
						sb.DrawRectangle(pos, new Vector2(2, 3), BarColor);
						pos.X += 3;
					}
				}
			}


			sb.End();
			spritePreview.EndDraw();

			sb = AppGui.SpriteBatchBegin(new Vector2(width, width), SamplerState.PointClamp);

			sb.Draw(spritePreview.RenderTarget, new Microsoft.Xna.Framework.Rectangle(0, 0, width, width),
				 Microsoft.Xna.Framework.Color.White);

			AppGui.SpriteBatchEnd();
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

		private void DrawTable()
		{
			itemPreview = tableSelection;

			if (!ImGui.BeginTable("##se.items", 6, ImGuiTableFlags.BordersInnerH
		| ImGuiTableFlags.ScrollY | ImGuiTableFlags.RowBg | ImGuiTableFlags.Resizable
	))
				return;

			ImGui.TableSetupScrollFreeze(0, 1);
			ImGui.TableSetupColumn("Name");
			ImGui.TableSetupColumn("Image");
			ImGui.TableSetupColumn("Bar Size");
			ImGui.TableSetupColumn("Selection");
			ImGui.TableSetupColumn("Selection Y");
			ImGui.TableSetupColumn(string.Empty, ImGuiTableColumnFlags.WidthFixed, 100);
			ImGui.TableHeadersRow();

			IEnumerable<SpriteAsset> query = QueryData(); ;

			foreach (var item in query.ToList())
			{
				ImGui.TableNextRow();

				TableRow(item);

			}

			ImGui.EndTable();
		}

		private void TableRow(SpriteAsset item)
		{
			int i = item.Id;

			string text = string.Empty;
			int number = 0;



			ImGui.TableNextColumn();
			text = item.Name;

			//AppGui.StrechNextItem();
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


			if (ImGui.IsItemHovered())
			{
				if (tableSelection != null)
				{
					if (selected)
						itemPreview = tableSelection;
				}
				else
					itemPreview = item;
			}

		}

		private IEnumerable<SpriteAsset> QueryData()
		{
			IEnumerable<SpriteAsset> query = Util.TextFilter(AppGame.AssetManager.Sprites, filter, a => a.Name);

			var sort = ImGui.TableGetSortSpecs();

			unsafe
			{
				if (sort.NativePtr == null)
					return query;
			}

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
