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
		private LogicalSpriteAsset hoverItem = null;

		private LogicalSpriteAsset changeItemImage = null;
		private LogicalImageAsset tmp = null;
		private LogicalSpriteAsset tableSelection = null;
		private LogicalSpriteAsset itemPreview = null;

		private bool first = true;
		private bool showSelectionMarker = true;
		private bool showBars = true;

		private Vector4 previewBgColor = Microsoft.Xna.Framework.Color.CornflowerBlue.ToVec4();


		private string[] SelectionTypes =
		{
			"22px","32px","48px", "62px","72px","94px","110px","122px","146px","224px",
		};

		private List<SpriteFrameAsset> selectionFrames = new List<SpriteFrameAsset>();

		private Microsoft.Xna.Framework.Color BarColor = new Microsoft.Xna.Framework.Color(0xff249824);
		private RenderTargetImage spritePreview = new RenderTargetImage(new Vector2(256, 256));

		public SpriteEditor()
		{
			foreach (var s in SelectionTypes)
			{
				string sheet = s;
				sheet = sheet.Substring(0, sheet.Length - 2);
				if (sheet.Length < 3)
					sheet = $"0{sheet}";
				sheet = $"unit\\thingy\\o{sheet}";

				var ss = AppGame.AssetManager.SpriteSheets.FirstOrDefault(f => f.SheetName == sheet);
				if (ss == null)
					continue;
				selectionFrames.Add(ss.Frames[0]);
			}
		}

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

			DrawControlHeader();

			DrawTable();

			ImGui.NextColumn();

			float width = ImGui.GetColumnWidth();

			DrawSpritePreview((int)width);

			ImGui.SetNextItemWidth(ImGui.GetColumnWidth() - 200);
			ImGui.ColorPicker4("Background Color##se.bgcolor", ref previewBgColor, ImGuiColorEditFlags.NoInputs | ImGuiColorEditFlags.NoAlpha);
			ImGui.Checkbox("Selection Marker##se.selectionmarker", ref showSelectionMarker);
			ImGui.Checkbox("Bars##se.bars", ref showBars);


			ImGui.End();

			DrawHoverItemTooltip();
			DrawChangeImageModal();


			if (changed)
			{
				AppGame.AssetManager.SaveSprites();
			}
		}

		private void DrawControlHeader()
		{
			ImGui.InputText("##se.filter", ref filter, 255);

			if (ImGui.Button("New Sprite##se.new"))
			{
				CreateNewSprite();
			}

			ImGui.SameLine();

			if (ImGui.Button("Duplicate Selected##se.copy"))
			{
				if (tableSelection != null)
				{
					int index = AppGame.AssetManager.Sprites.IndexOf(tableSelection);
					var newS = new LogicalSpriteAsset(tableSelection)
					{
						Name = $"{tableSelection.Name} copy"
					};
					AppGame.AssetManager.Sprites.Insert(index + 1,newS);
					tableSelection = newS;
					changed = true;
				}
			}

			ImGui.SameLine();

			if (ImGui.Button("Delete Selected##se.del"))
			{
				if (tableSelection != null)
				{
					AppGame.AssetManager.Sprites.Remove(tableSelection);
					changed = true;
					tableSelection = null;
				}
				
			}
		}

		private void DrawSpritePreview(int width)
		{
			var sb = spritePreview.BeginDraw(previewBgColor.ToColor());
			sb.Begin(SpriteSortMode.Deferred, null, SamplerState.PointClamp);
			Vector2 center = spritePreview.Size / 2;
			Vector2 pos = center;

			if (itemPreview != null)
			{
				int frameIndex = 0;
				if (itemPreview.Image.GraphicsTurns)
					frameIndex += 12;

				var frame = itemPreview.Image.SpriteSheet.Frames[frameIndex];


				if (showSelectionMarker && itemPreview.SelectionType >= 0)
				{
					if (selectionFrames.Count > itemPreview.SelectionType)
					{
						var selection = selectionFrames[itemPreview.SelectionType];
						pos = center + selection.GetOffset();
						pos.Y += itemPreview.SelectionOffset;
						sb.Draw(selection.Image.Texture, pos.ToVector2(), Microsoft.Xna.Framework.Color.LightGreen);
					}
				}

				pos = center + frame.GetOffset();

				sb.Draw(frame.Image.Texture, pos.ToVector2(), Microsoft.Xna.Framework.Color.White);

				if (itemPreview.Image.UnitColor)
				{
					frameIndex = itemPreview.Image.SpriteSheet.GetUnitColorFrameIndex(frameIndex);

					frame = itemPreview.Image.SpriteSheet.Frames[frameIndex];
					sb.Draw(frame.Image.Texture, pos.ToVector2(), Microsoft.Xna.Framework.Color.Magenta);
				}


				if (showBars && itemPreview.BarSize > 0)
				{
					int barWidth = itemPreview.BarSize * 3 + 1;

					Vector2 size = new Vector2(barWidth, 5);
					pos = center;
					pos.X -= size.X / 2;

					pos.Y += itemPreview.BarOffset;

					pos.X = (int)pos.X;

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
			var sprite = new LogicalSpriteAsset()
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

			if (!ImGui.BeginTable("##se.items", 7, ImGuiTableFlags.BordersInnerH
		| ImGuiTableFlags.ScrollY | ImGuiTableFlags.RowBg | ImGuiTableFlags.Resizable
	))
				return;

			ImGui.TableSetupScrollFreeze(0, 1);
			ImGui.TableSetupColumn("Name");
			ImGui.TableSetupColumn("Image");
			ImGui.TableSetupColumn("Bar Size");
			ImGui.TableSetupColumn("Bar Y");
			ImGui.TableSetupColumn("Selection");
			ImGui.TableSetupColumn("Selection Y");
			ImGui.TableSetupColumn(string.Empty, ImGuiTableColumnFlags.WidthFixed, 100);
			ImGui.TableHeadersRow();

			IEnumerable<LogicalSpriteAsset> query = QueryData(); ;

			foreach (var item in query.ToList())
			{
				ImGui.TableNextRow();

				TableRow(item);

			}

			ImGui.EndTable();
		}

		private void TableRow(LogicalSpriteAsset item)
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
			number = item.BarOffset;
			AppGui.StrechNextItem();
			if (ImGui.InputInt($"##se.items.{i}.bary", ref number))
			{
				item.BarOffset = number;
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
			number = item.SelectionOffset;
			AppGui.StrechNextItem();
			if (ImGui.InputInt($"##se.items.{i}.sely", ref number))
			{
				item.SelectionOffset = number;
				changed = true;
			}

			ImGui.TableNextColumn();
			if (ImGui.Button($"Duplicate##se.items.{i}.copy"))
			{
				int index = AppGame.AssetManager.Sprites.IndexOf(item);
				AppGame.AssetManager.Sprites.Insert(index + 1, new LogicalSpriteAsset(item)
				{
					Name = $"{item.Name} copy"
				});
				changed = true;
			}
			ImGui.SameLine();
			if (ImGui.Button($"Delete##se.items.{i}.del"))
			{
				AppGame.AssetManager.Sprites.Remove(item);
				changed = true;
			}

			ImGui.SameLine();
			bool selected = tableSelection == item;

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

		private IEnumerable<LogicalSpriteAsset> QueryData()
		{
			IEnumerable<LogicalSpriteAsset> query = Util.TextFilter(AppGame.AssetManager.Sprites, filter, a => a.Name);

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
