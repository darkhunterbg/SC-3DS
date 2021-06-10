﻿using DataManager.Assets;
using DataManager.Widgets;
using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using Rectangle = Microsoft.Xna.Framework.Rectangle;
using Color = Microsoft.Xna.Framework.Color;
using Microsoft.Xna.Framework.Graphics;
using DataManager.Gameplay;

namespace DataManager.Panels
{

	public class SpriteEditor : IGuiPanel
	{
		class SpriteAnimData
		{
			public AnimationState State = new AnimationState();
			public bool Flipped;
			public float TimeDelay;
		}

		public string WindowName => "Sprite Editor";

		private RenderTargetImage animPreview = new RenderTargetImage(new Vector2(256, 256));

		private SpriteAnimData animData = new SpriteAnimData();

		private SpriteAsset prevSelection = null;
		private SpriteAsset Selected => treeView.Selected as SpriteAsset;
		private SpriteAnimClipAsset SelectedAnimClip
		{
			get
			{
				if (Selected == null) return null;
				return Selected.Clips.FirstOrDefault(c => c.Type == selectedAnimType);
			}
		}
		private bool selectionChanged = false;
		private AnimationType selectedAnimType = 0;


		private TreeView treeView = new TreeView("tree")
		{
			DataSource = AppGame.AssetManager.GetAssets<SpriteAsset>(),
			ItemName = "Sprite",
			RootName = "Sprites"
		};

		private TablePropertyEditor propertyEditor = new TablePropertyEditor("propertyEditor");

		public SpriteEditor()
		{
			treeView.NewItemAction = NewItem;
			treeView.DeleteItemAction = DeleteItem;
		}

		private ITreeViewItem NewItem(ITreeViewItem item)
		{
			var asset = item as SpriteAsset;
			return AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().New(asset);
		}

		private void DeleteItem(ITreeViewItem item)
		{
			var asset = item as SpriteAsset;
			AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().Delete(asset);
		}

		bool clipModified = false;

		public void Draw(Vector2 client)
		{
			clipModified = false;
			selectionChanged = false;

			ImGui.Columns(4, "spriteeditor");

			ImGui.BeginChild("##items");
			{
				treeView.Draw();

				if (prevSelection != Selected)
				{
					selectionChanged = true;
					prevSelection = Selected;
				}
			}
			ImGui.EndChild();

			ImGui.NextColumn();

			ImGui.BeginChild("##script");
			{
				propertyEditor.EditingItem = treeView.Selected;
				propertyEditor.Draw(new Vector2(0, 100));
				ImGui.Separator();
				DrawScriptEditor();
			}
			ImGui.EndChild();

			ImGui.NextColumn();

			ImGui.BeginChild("##anim");
			{
				DrawAnimationEditor(ImGui.GetColumnWidth());

				ImGui.NewLine();
			}
			ImGui.EndChild();

			ImGui.NextColumn();

			ImGui.BeginChild("assetview");
			{
				DrawSpriteFrames(ImGui.GetColumnWidth());
			}
			ImGui.EndChild();

			if (treeView.ItemModified || propertyEditor.Changed)
			{
				clipModified = true;
				AppGame.AssetManager.GetAssetDatabase<SpriteAsset>().Save();
			}

			if (clipModified)
				AppGame.AssetManager.GetAssetDatabase<SpriteAnimClipAsset>().Save();

		}

		private void DrawSpriteFrames(float width)
		{
			if (Selected != null && Selected.Image.Image != null)
			{
				float x = 0;

				float scale = 1;
				if (Selected.Image.Image.FrameSize.X <= 64 || Selected.Image.Image.FrameSize.Y <= 90)
					scale *= 2;

				if (Selected.Image.Image.FrameSize.X <= 16 || Selected.Image.Image.FrameSize.Y <= 20)
					scale *= 2;

				int i = 0;

				foreach (var frame in Selected.Image.Image.Frames)
				{
					if (Selected.Image.Image.HasUnitColor && Selected.Image.Image.UnitColorOffset <= frame.FrameIndex)
						break;

					++i;
					x += frame.Size.X * scale;
					if (x >= width - 140)
					{
						x = frame.Size.X * scale;
						i = 1;
					}
					else
					{
						if (!Selected.IsRotating || i <= 17)
						{
							ImGui.SameLine();
						}
						else
						{
							i = 1;
							x = frame.Size.X * scale;
						}
					}

					if (frame.FrameIndex == animData.State.GetActualFrameIndex(Selected))
					{
						ImGui.Image(frame.Image.GuiImage, frame.Size * scale - Vector2.One * 2, Vector2.Zero, Vector2.One, Vector4.One, Vector4.One);
					}
					else
						ImGui.Image(frame.Image.GuiImage, frame.Size * scale);
					if (ImGui.IsItemHovered())
						AppGame.Gui.HoverObject = frame;
				}
			}
		}

		private void DrawAnimationEditor(float width)
		{
			DrawAnimation();

			int max = (int)width;// Math.Min((int)width, animPreview.RenderTarget.Width * 2);

			var sb = AppGui.SpriteBatchBegin(new Vector2(max, max), SamplerState.PointClamp);
			sb.Draw(animPreview.RenderTarget, new Rectangle(0, 0, (int)max, (int)max), Color.White);

			AppGui.SpriteBatchEnd();

			if (Selected == null)
				return;

			string[] items = EnumCacheValues.GetValues<AnimationType>();

			//ImGui.SetNextItemWidth(-200);
		
			if(EditorFieldDrawer.Enum("Animation", ref selectedAnimType) || selectionChanged)
			{
				int orientation = animData.State.Orientation;
				animData = new SpriteAnimData();
				animData.State.SetOrientation(orientation);

				SpriteAnimClipAsset clip = SelectedAnimClip;
				buffer = clip == null ? string.Empty : string.Join('\n', clip.Instructions);
				instructions = ParseClipInstructions(buffer);
			}
			//if (ImGui.Combo("Animation", ref animIndex, items, items.Length) || selectionChanged)
			//{
			//	selectedAnimType = (AnimationType)animIndex;
			
			//}

			if (Selected.IsRotating)
			{
				ImGui.SetNextItemWidth(-200);
				int orient = animData.State.Orientation;
				if (EditorFieldDrawer.Int("Orientation", ref orient))
				{
					animData.State.SetOrientation(orient);
				}
			}
		}

		List<string> instructions = new List<string>();
		string buffer = string.Empty;

		private void DrawScriptEditor()
		{
			if (Selected == null)
				return;

			var clip = SelectedAnimClip;

			ImGui.Columns(2, "script");

			if (ImGui.InputTextMultiline(string.Empty, ref buffer, 1024, new Vector2(-1, -1),
				ImGuiInputTextFlags.Multiline))
			{
				if (string.IsNullOrWhiteSpace(buffer))
				{
					if (clip != null)
					{
						clipModified = true;
						AppGame.AssetManager.GetAssetDatabase<SpriteAnimClipAsset>().Assets.Remove(clip);
						Selected.Clips.Remove(clip);
						clip = null;
					}
				}
				else
				{
					instructions = ParseClipInstructions(buffer);
					clipModified = true;

					if (clip == null)
					{
						clip = new SpriteAnimClipAsset(Selected, selectedAnimType);
						AppGame.AssetManager.GetAssetDatabase<SpriteAnimClipAsset>().Assets.Add(clip);
						Selected.Clips.Add(clip);
					}

					clip.SetInstructions(instructions);
				}

			}

			ImGui.NextColumn();

			if (clip != null)
			{
				int i = 0;
				foreach (var instr in instructions)
				{
					ImGui.PushID(++i);
					ImGui.Text(instr);
					ImGui.PopID();
					ImGui.SetCursorPosY(ImGui.GetCursorPosY() - 4);
				}
			}
		}

		private List<string> ParseClipInstructions(string editorText)
		{
			List<string> result = new List<string>();

			var lines = editorText.Split('\n');

			foreach (var line in lines)
			{
				var split = line.Split(' ');
				string instText = split.FirstOrDefault();

				if (instText != null)
				{
					if (AnimClipInstructionDatabase.Instructions.TryGetValue(instText, out var instr))
					{
						if (instr.Parameters.Count > split.Length - 1)
							continue;

						for (int j = 0; j < instr.Parameters.Count; ++j)
						{

							if (!instr.Parameters[j].Validate(split[j + 1]))
								continue;
						}
						// TODO: param validation

						result.Add(line);
						continue;
					}
				}

				result.Add(string.Empty);
			}

			return result;
		}

		private void DrawAnimation()
		{
			SpriteBatch sb = animPreview.StartDrawOn();
			sb.Begin();


			Vector2 pos = new Vector2(animPreview.RenderTarget.Width, animPreview.RenderTarget.Height);
			pos /= 2;

			if (Selected != null && Selected.Image.Image != null)
			{
				var clip = SelectedAnimClip;
				if (clip != null)
				{
					if (animData.TimeDelay > 0)
					{
						animData.TimeDelay -= 0.016f;
					}
					else
					{
						animData.State.ExecuteInstructions(clip);

						int frameDelay = Math.Max(animData.State.FrameDelay, 1);
						animData.TimeDelay = frameDelay / 15.0f;
					}
				}

				int frameIndex = animData.State.GetActualFrameIndex(Selected);

				SpriteEffects eff = animData.State.FlipSprite ? SpriteEffects.FlipHorizontally : SpriteEffects.None;
				var frame = Selected.Image.Image.GetFrame(frameIndex);

				if (frame != null)
				{
					Vector2 offset = frame.GetOffset(animData.State.FlipSprite) + animData.State.Offset;


					sb.Draw(frame.Image.Texture, (pos + offset).ToVector2(), null, Color.White, 0, Vector2.Zero.ToVector2(), 1, eff, 1);
					if (Selected.HasUnitColoring)
					{
						frame = Selected.Image.Image.GetUnitColorFrame(frameIndex);

						if (frame != null)
						{
							offset = frame.GetOffset(animData.State.FlipSprite) + animData.State.Offset;

							sb.Draw(frame.Image.Texture, (pos + offset).ToVector2(), null, Color.Magenta, 0, Vector2.Zero.ToVector2(), 1, eff, 1);
						}
					}
				}
			}

			sb.End();
			animPreview.EndDrawOn();
		}
	}
}
