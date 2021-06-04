using DataManager.Assets;
using DataManager.Panels;
using ImGuiNET;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager
{
	public class AppGui
	{
		public AppGame Game { get; private set; }

		public AssetConverter AssetConverter { get; private set; } = new AssetConverter();
		public SpriteAtlasGenerator SpriteAtlasGenerator { get; private set; } = new SpriteAtlasGenerator();
		public ImageEditor ImageEditor { get; private set; } = new ImageEditor();
		public SpriteEditor SpriteEditor { get; private set; } = new SpriteEditor();

		public SpriteBatch SpriteBatch { get; private set; }
		public RenderTarget2D BackBuffer { get; private set; }
		private IntPtr guiRenderTarget;


		private List<IEnumerator> coroutines = new List<IEnumerator>();

		public static void RunGuiCoroutine(IEnumerator crt)
		{
			AppGame.Gui.coroutines.Add(crt);
		}

		public AppGui(AppGame game)
		{
			Game = game;
			SpriteBatch = new SpriteBatch(game.GraphicsDevice);
			BackBuffer = new RenderTarget2D(game.GraphicsDevice, 1, 1);
			guiRenderTarget = AppGame.GuiRenderer.BindTexture(BackBuffer);
		}


		private void ResizeRenderTarget(Vector2 size)
		{
			if (BackBuffer.Width != size.X || BackBuffer.Height != size.Y)
			{
				if (size.X > 1 && size.Y > 1)
				{

					AppGame.GuiRenderer.UnbindTexture(guiRenderTarget);
					BackBuffer.Dispose();

					BackBuffer = new RenderTarget2D(Game.GraphicsDevice, (int)size.X, (int)size.Y);
					guiRenderTarget = AppGame.GuiRenderer.BindTexture(BackBuffer);
				}
			}
		}

		public void Draw(Vector2 clientSize)
		{
			ImGui.PushStyleVar(ImGuiStyleVar.WindowPadding, new Vector2(0, 0));

			ImGui.SetNextWindowSize(new Vector2(clientSize.X, clientSize.Y));
			ImGui.SetNextWindowPos(new Vector2(0, 0));
			ImGui.Begin("Root", ImGuiWindowFlags.NoBringToFrontOnFocus | ImGuiWindowFlags.NoCollapse | ImGuiWindowFlags.NoTitleBar
				| ImGuiWindowFlags.DockNodeHost | ImGuiWindowFlags.AlwaysAutoResize);

			ImGui.DockSpace(ImGui.GetID("RootDockspace"));
			ImGui.End();

			ImGui.PopStyleVar(1);


			ResizeRenderTarget(clientSize);

			AppGame.Device.SetRenderTarget(BackBuffer);
			AppGame.Device.Clear(Microsoft.Xna.Framework.Color.CornflowerBlue);

			AssetConverter.Update();
			AssetConverter.Draw();
			SpriteAtlasGenerator.Draw();
			ImageEditor.Draw();
			SpriteEditor.Draw(clientSize);

			UpdateCoroutines();

			AppGame.Device.SetRenderTarget(null);
		}

		public bool UpdateCoroutines()
		{
			for (int i = 0; i < coroutines.Count; ++i)
			{
				var crt = coroutines[i];
				if (!crt.MoveNext())
				{
					coroutines.RemoveAt(i--);
				}
			}

			return coroutines.Count > 0;
		}


		public static void StrechNextItem()
		{
			ImGui.SetNextItemWidth(-float.Epsilon);
		}

		private static Stack<Vector2> sbDrawSize = new Stack<Vector2>();
		private static Stack<Viewport> viewport = new Stack<Viewport>();


		public static SpriteBatch SpriteBatchBegin(Vector2 size, SamplerState? samplerState =
			null, float aspectRatio = 0)
		{
			if (aspectRatio != 0)
			{
				if (size.X > size.Y)
				{
					size.X = size.Y * aspectRatio;
				}
				else if (size.Y > size.X)
				{
					size.Y = size.X * aspectRatio;
				}
			}

			sbDrawSize.Push(size);

			Vector2 pos = ImGui.GetCursorPos();

			viewport.Push(new Viewport((int)pos.X, (int)pos.Y, (int)size.X, (int)size.Y));

			AppGame.Gui.SpriteBatch.Begin(SpriteSortMode.Deferred,null, samplerState);

			AppGame.Device.Viewport = viewport.Peek();

			return AppGame.Gui.SpriteBatch;
		}

		public static void SpriteBatchEnd()
		{
			AppGame.Gui.SpriteBatch.End();

			var size = sbDrawSize.Pop();
			var vp = viewport.Pop();


			var rtSize = new Vector2(AppGame.Gui.BackBuffer.Width, AppGame.Gui.BackBuffer.Height);

			Vector2 uvStart = new Vector2(vp.X, vp.Y) / rtSize;
			Vector2 uvEnd = (new Vector2(vp.X, vp.Y) + size) / rtSize;

			ImGui.Image(AppGame.Gui.guiRenderTarget, size, uvStart, uvEnd);
		}

		public static void DrawSpriteSheetInfo(SpriteSheetAsset ss)
		{
			ImGui.Text($"Dimensions: {ss.Width}x{ss.Height}");
			ImGui.SameLine();


			if (ss.SubAtlas != null)
			{
				ImGui.Text($"Frames: {ss.Frames}");

				for (int i = 0; i < ss.Frames; ++i)
				{
					var tex = AppGame.AssetManager.GetSheetImage(ss.SheetName, i);

					if (i % 16 > 0)
						ImGui.SameLine();

					ImGui.Image(tex.GuiImage, new Vector2(tex.Texture.Width, tex.Texture.Height));

				}

			}
		}

		public static bool ProgressDialog(string text, int count, int total, bool cancelable = false)
		{
			float progress = (float)count / (float)total;

			return ProgressDialog(text, progress, cancelable);
		}
		public static bool ProgressDialog(string text, float progress, bool cancelable = false)
		{
			bool result = true;
			ImGui.OpenPopup("Generic.ProgressBar");
			bool open = true;
			ImGui.BeginPopupModal("Generic.ProgressBar", ref open, ImGuiWindowFlags.AlwaysAutoResize | ImGuiWindowFlags.NoTitleBar);
			ImGui.Text(text);
			ImGui.ProgressBar(progress, new Vector2(400, 40));
			if (cancelable)
			{
				result = !ImGui.Button("Cancel");
			}
			ImGui.EndPopup();

			return result;
		}
	}
}
