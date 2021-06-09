using DataManager.Assets;
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

namespace DataManager.Panels
{
	class AnimationState
	{
		public Vector2 offset;
		public int instructionId;
		public int frameDelay;
		public int orientation;
		public int frameIndex = 0;

		public bool flip = false;

		public float timeDelay = 0;
	}

	public class SpriteEditor : IGuiPanel
	{
		public string WindowName => "Sprite Editor";

		private RenderTargetImage animPreview = new RenderTargetImage(new Vector2(256, 256));

		private int animTypeIndex = 0;

		private AnimationState animState = new AnimationState();

		private SpriteAsset prevSelection = null;
		private SpriteAsset Selected => treeView.Selected as SpriteAsset;
		private bool selectionChanged = false;

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

		bool first = true;

		bool clipModified = false;

		public void Draw(Vector2 client)
		{
			clipModified = false;
			selectionChanged = false;

			ImGui.Columns(3,"spriteeditor");

			//if (first)
			//{
			//	ImGui.SetColumnWidth(0, -1);
			//	ImGui.SetColumnWidth(1, client.X - 512 - ImGui.GetColumnWidth(0));
			//	first = false;
			//}

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

			ImGui.BeginChild("##settings");
			{
				propertyEditor.EditingItem = treeView.Selected;
				propertyEditor.Draw();
			}
			ImGui.EndChild();

			ImGui.NextColumn();
			ImGui.BeginChild("##anim");
			{
				DrawAnimationEditor(ImGui.GetColumnWidth());
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

		private void DrawAnimationEditor(float width)
		{

			var animType = (AnimationType)animTypeIndex;

			DrawAnimation(animType);

			int max = Math.Min((int)width, animPreview.RenderTarget.Width * 2);

			var sb = AppGui.SpriteBatchBegin(new Vector2(max, max), SamplerState.PointClamp);
			sb.Draw(animPreview.RenderTarget, new Rectangle(0, 0, (int)max, (int)max), Color.White);

			AppGui.SpriteBatchEnd();

			if (Selected == null)
				return;

			string[] items = EnumCacheValues.GetValues<AnimationType>();


			SpriteAnimClipAsset clip = Selected.Clips.FirstOrDefault(c => c.Type == animType);

			ImGui.SetNextItemWidth(-200);
			if (ImGui.Combo("Animation", ref animTypeIndex, items, items.Length) || selectionChanged)
			{
				animType = (AnimationType)animTypeIndex;
				animState = new AnimationState()
				{
					orientation = animState.orientation
				};
				clip = Selected.Clips.FirstOrDefault(c => c.Type == animType);
				buffer = clip == null ? string.Empty : string.Join('\n', clip.Instructions);
				instructions = ParseClipInstructions(buffer);
			}

			if (Selected.IsRotating)
			{
				ImGui.SetNextItemWidth(-200);
				if (ImGui.InputInt("Orientation", ref animState.orientation))
				{
					if (animState.orientation < 0)
						animState.orientation = 32 + (animState.orientation % 32);
					else
						animState.orientation %= 32;
				}
			}


			ImGui.BeginChild("##instructions");

			ImGui.Columns(2,"script");


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
						clip = new SpriteAnimClipAsset(Selected, animType);
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
				}
			}

			ImGui.EndChild();



		}

		List<string> instructions = new List<string>();
		string buffer = string.Empty;

		private List<string> ParseClipInstructions(string editorText)
		{
			List<string> result = new List<string>();

			var lines = editorText.Split('\n');

			foreach(var line in lines)
			{
				var split = line.Split(' ');
				string instruction = split.FirstOrDefault();

				if (instruction != null)
				{
					if(SpriteAnimClipAsset.InstructionDefs.Any(i=>i.Instruction== instruction))
					{
						result.Add(line);
						continue;
					}
				}

				result.Add(string.Empty);
			}

			return result;
		}

		private void DrawAnimation(AnimationType type)
		{
			SpriteBatch sb = animPreview.StartDrawOn();
			sb.Begin();


			Vector2 pos = new Vector2(animPreview.RenderTarget.Width, animPreview.RenderTarget.Height);
			pos /= 2;

			if (Selected != null && Selected.Image.Image != null)
			{
				var clip = Selected.Clips.FirstOrDefault(c => c.Type == type);
				if (clip != null)
				{
					if (animState.timeDelay > 0)
					{
						animState.timeDelay -= 0.016f;
					}
					else
					{
						ClipInstructionExec(animState, clip);

						int frameDelay = Math.Max(animState.frameDelay, 1);
						animState.timeDelay = frameDelay / 15.0f;
					}
				}

				int frameIndex = animState.frameIndex;

				if (Selected.IsRotating)
					frameIndex *= 17;

				if (animState.orientation > 16)
				{
					animState.flip = true;
					frameIndex += 32 - animState.orientation;
				}
				else
				{
					animState.flip = false;
					frameIndex += animState.orientation;
				}

				SpriteEffects eff = animState.flip ? SpriteEffects.FlipHorizontally : SpriteEffects.None;
				var frame = Selected.Image.Image.GetFrame(frameIndex);

				if (frame != null)
				{
					Vector2 offset = frame.GetOffset(animState.flip) + animState.offset;


					sb.Draw(frame.Image.Texture, (pos + offset).ToVector2(), null, Color.White, 0, Vector2.Zero.ToVector2(), 1, eff, 1);
					if (Selected.HasUnitColoring)
					{
						frame = Selected.Image.Image.GetUnitColorFrame(frameIndex);

						if (frame != null)
						{
							offset = frame.GetOffset(animState.flip) + animState.offset;

							sb.Draw(frame.Image.Texture, (pos + offset).ToVector2(), null, Color.Magenta, 0, Vector2.Zero.ToVector2(), 1, eff, 1);
						}
					}
				}
			}

			sb.End();
			animPreview.EndDrawOn();
		}

		private void ClipInstructionExec(AnimationState state, SpriteAnimClipAsset clip)
		{
			bool end = false;

			state.frameDelay = 0;

			while (!end)
			{
				if (state.instructionId >= clip.Instructions.Count)
				{
					state.instructionId = 0;
					return;
				}

				var instr = clip.Instructions[state.instructionId];

				if (!string.IsNullOrEmpty(instr))
				{

					var s = instr.Split(' ');


					switch (s[0])
					{
						case "wait":
							{
								if (s.Length < 2)
									break;
								int.TryParse(s[1], out state.frameDelay); end = true; break;
							}
						case "playfram":
							{
								if (s.Length < 2)
									break;
								int.TryParse(s[1], out int next);
								animState.frameIndex = next;

								break;
							}
					}
				}

				++state.instructionId;
				if (state.instructionId >= clip.Instructions.Count)
				{
					state.instructionId = 0;
					break;
				}
			}
		}
	}
}
