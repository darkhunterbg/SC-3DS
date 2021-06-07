﻿using DataManager.Assets;
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
	public class SpriteEditor  :IGuiPanel
	{
		private bool first = true;
		private bool showSelectionMarker = true;
		private bool showBars = true;

		private SpriteAsset itemPreview = null;

		private Vector4 previewBgColor = Microsoft.Xna.Framework.Color.CornflowerBlue.ToVec4();

		private AssetTableEditor<SpriteAsset> table = new AssetTableEditor<SpriteAsset>("##se.table");

		private Microsoft.Xna.Framework.Color BarColor = new Microsoft.Xna.Framework.Color(0xff249824);
		private RenderTargetImage spritePreview = new RenderTargetImage(new Vector2(256, 256));

		public SpriteEditor()
		{
		

		}

		public void Draw(Vector2 clientSize)
		{
			if (!ImGui.Begin("Sprite Editor##se"))
				return;

			ImGui.Columns(2, "se.columns");

			if (first)
				ImGui.SetColumnWidth(0, clientSize.X - 512);

			first = false;

			table.Draw();

			ImGui.NextColumn();

			float width = ImGui.GetColumnWidth();

			DrawSpritePreview((int)width);

			ImGui.SetNextItemWidth(ImGui.GetColumnWidth() - 200);
			ImGui.ColorPicker4("Background Color##se.bgcolor", ref previewBgColor, ImGuiColorEditFlags.NoInputs | ImGuiColorEditFlags.NoAlpha);
			ImGui.Checkbox("Selection Marker##se.selectionmarker", ref showSelectionMarker);
			ImGui.Checkbox("Bars##se.bars", ref showBars);

			ImGui.End();

			table.SaveChanges();
		}

		private void DrawSpritePreview(int width)
		{
			itemPreview = table.SelectedItem ?? table.HoverItem;

			var sb = spritePreview.BeginDraw(previewBgColor.ToColor());
			sb.Begin(SpriteSortMode.Deferred, null, SamplerState.PointClamp);
			Vector2 center = spritePreview.Size / 2;
			Vector2 pos = center;

			if (itemPreview?.Image != null)
			{
				int frameIndex = 0;
				if (itemPreview.Image.GraphicsTurns)
					frameIndex += 12;

				var frame = itemPreview.Image.SpriteSheet.Image.Frames[frameIndex];


				if (showSelectionMarker && itemPreview.SelectionType >= 0)
				{
					if (AppGame.AssetManager.UnitSelection.Count > itemPreview.SelectionType)
					{
						var selection = AppGame.AssetManager.UnitSelection[itemPreview.SelectionType];
						pos = center + selection.GetOffset();
						pos.Y += itemPreview.SelectionOffset;
						sb.Draw(selection.Image.Texture, pos.ToVector2(), Microsoft.Xna.Framework.Color.LightGreen);
					}
				}

				pos = center + frame.GetOffset();

				sb.Draw(frame.Image.Texture, pos.ToVector2(), Microsoft.Xna.Framework.Color.White);

				if (itemPreview.Image.UnitColor)
				{
					frameIndex = itemPreview.Image.SpriteSheet.Image.GetUnitColorFrameIndex(frameIndex);

					frame = itemPreview.Image.SpriteSheet.Image.Frames[frameIndex];
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
	}
}
