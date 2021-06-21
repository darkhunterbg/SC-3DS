using DataManager.Assets;
using DataManager.Gameplay;
using ImGuiNET;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using Color = Microsoft.Xna.Framework.Color;
using Rectangle = Microsoft.Xna.Framework.Rectangle;

namespace DataManager.Widgets
{
	public class SpriteView
	{
		static readonly Color SCGreen = new Color(0xff249824);

		public class SpriteAnimData
		{
			public AnimationState State = new AnimationState();
			public float TimeDelay;
			public int BreakpointAt = int.MinValue;
		}


		private int gameSpeed = 15;
		private RenderTargetImage animPreview = new RenderTargetImage(new Vector2(256, 256));

		private SpriteAsset _sprite;
		public SpriteAsset Sprite
		{
			get { return _sprite; }
			set
			{
				if (_sprite == value)
					return;
				_sprite = value;
				SelectionChanged = true;
			}
		}
		public SpriteAnimClipAsset Clip
		{
			get
			{
				if (Sprite == null) return null;
				return Sprite.Clips.FirstOrDefault(c => c.Type == selectedAnimType);
			}
		}
		private AnimationType selectedAnimType;
		public AnimationType SelectedAnimType => selectedAnimType;

		public int SelectionType = -1;
		public int SelectionOffset;
		public bool ShowSelection = false;

		public int BarSize = 0;
		public int BarOffset = 0;
		public bool ShowBars = false;

		public bool ShowShadows = false;
		public ImageListRef ShadowImage;
		public Vector2 ShadowOffset;

		public bool ShowCollider = false;

		public bool SelectionChanged { get; private set; } = false;
		public bool ClipChanged { get; private set; } = false;
		public SpriteAnimData AnimData { get; private set; } = new SpriteAnimData();

		public void Draw(float size)
		{
			ClipChanged = false;

			SpriteBatch sb = animPreview.StartDrawOn();
			sb.Begin();

			Vector2 pos = new Vector2(animPreview.RenderTarget.Width, animPreview.RenderTarget.Height);
			pos /= 2;

			if (ShowSelection)
				DrawSelection(pos, sb);

			if (ShowShadows)
				DrawShadow(pos, sb);

			DrawSprite(pos, sb);

			if (ShowBars)
				DrawBars(pos, sb);

			if (ShowCollider)
				DrawCollider(pos, sb);

			sb.End();

			animPreview.EndDrawOn();
			DrawControls(size);

			SelectionChanged = false;
		}

		private void DrawControls(float width)
		{
			int max = (int)width;// Math.Min((int)width, animPreview.RenderTarget.Width * 2);

			var sb = AppGui.SpriteBatchBegin(new Vector2(max, max), SamplerState.PointClamp);
			sb.Draw(animPreview.RenderTarget, new Rectangle(0, 0, (int)max, (int)max), Color.White);

			AppGui.SpriteBatchEnd();

			if (Sprite == null)
				return;

			//ImGui.SetNextItemWidth(-200);

			if (EditorFieldDrawer.Enum("Animation", ref selectedAnimType) || SelectionChanged)
			{
				int orientation = AnimData.State.Orientation;
				AnimData = new SpriteAnimData();
				if (Sprite.IsRotating)
					AnimData.State.SetOrientation(orientation);
				ClipChanged = true;
			}

			ImGui.DragInt("Animation Speed", ref gameSpeed, 0.05f, 6, 24);

			if (Sprite.IsRotating)
			{
				ImGui.SetNextItemWidth(-200);
				int orient = AnimData.State.Orientation;
				if (EditorFieldDrawer.Int("Orientation", ref orient))
				{
					AnimData.State.SetOrientation(orient);
				}
			}

			EditorFieldDrawer.Bool("Show Collider", ref ShowCollider);
		}

		private void DrawSprite(Vector2 pos, SpriteBatch sb)
		{
			if (Sprite != null && Sprite.Image.Image != null)
			{
				if (Clip != null)
				{
					if (AnimData.State.InstructionId != AnimData.BreakpointAt)
					{
						if (AnimData.TimeDelay > 0)
							AnimData.TimeDelay -= (gameSpeed / 60.0f);
						else
						{
							AnimData.State.FrameDelay = 0;

							while (AnimData.State.ExecuteInstruction(Clip))
							{
								if (AnimData.State.InstructionId == AnimData.BreakpointAt)
									break;
							}

							int frameDelay = AnimData.State.FrameDelay;
							AnimData.TimeDelay = frameDelay;
						}
					}
				}

				int frameIndex = AnimData.State.FrameIndex;

				SpriteEffects eff = AnimData.State.FlipSprite ? SpriteEffects.FlipHorizontally : SpriteEffects.None;
				var frame = Sprite.Image.Image.GetFrame(frameIndex);

				if (frame != null)
				{
					Vector2 offset = frame.GetOffset(AnimData.State.FlipSprite) + AnimData.State.Offset;


					sb.Draw(frame.Image.Texture, (pos + offset).ToVector2(), null, Color.White, 0, Vector2.Zero.ToVector2(), 1, eff, 1);
					if (Sprite.HasUnitColoring)
					{
						frame = Sprite.Image.Image.GetUnitColorFrame(frameIndex);

						if (frame != null)
						{
							offset = frame.GetOffset(AnimData.State.FlipSprite) + AnimData.State.Offset;

							sb.Draw(frame.Image.Texture, (pos + offset).ToVector2(), null, Color.Magenta, 0, Vector2.Zero.ToVector2(), 1, eff, 1);
						}
					}
				}
			}
		}

		private void DrawShadow(Vector2 pos, SpriteBatch sb)
		{
			if (ShadowImage.Image != null)
			{
				int frameIndex = AnimData.State.FrameIndex;

				SpriteEffects eff = AnimData.State.FlipSprite ? SpriteEffects.FlipHorizontally : SpriteEffects.None;
				var frame = ShadowImage.Image.GetFrame(frameIndex);

				if (frame != null)
				{
					Vector2 offset = frame.GetOffset(AnimData.State.FlipSprite);// + AnimData.State.Offset;
					offset += ShadowOffset;

					Color shadowColor = Color.Black;
					shadowColor.A = 127;

					sb.Draw(frame.Image.Texture, (pos + offset).ToVector2(), null, shadowColor, 0, Vector2.Zero.ToVector2(), 1, eff, 1);
				}
			}
		}

		private void DrawSelection(Vector2 pos, SpriteBatch sb)
		{
			if (SelectionType < 0 || SelectionType >= AppGame.AssetManager.UnitSelection.Count)
				return;

			var selectionImage = AppGame.AssetManager.UnitSelection[SelectionType];

			pos += selectionImage.GetOffset();
			pos.Y += SelectionOffset;

			sb.Draw(selectionImage.Image.Texture, pos.ToVector2(), Color.LightGreen);
		}

		private void DrawBars(Vector2 pos, SpriteBatch sb)
		{
			if (BarSize < 1)
				return;

			Vector2 bar = new Vector2(BarSize * 3 + 1, 5);
			pos.X -= (int)(bar.X / 2);
			pos.Y += BarOffset;

			sb.DrawRectangle(pos, bar, Color.Black);

			pos += Vector2.One;

			for (int i = 0; i < BarSize; ++i)
			{
				sb.DrawRectangle(pos, new Vector2(2, 3), SCGreen);
				pos.X += 3;
			}
		}

		private void DrawCollider(Vector2 pos, SpriteBatch sb)
		{
			if (Sprite == null)
				return;

			if (Sprite.ColliderSize == Vector2.Zero)
				return;

			pos += Sprite.ColliderPosition;


			Color c = Color.Green;
			c.A = 65;

			sb.DrawRectangle(pos, Sprite.ColliderSize, c);
		}
	}
}
