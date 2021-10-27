using DataManager.Assets;
using DataManager.Panels;
using DataManager.Widgets;
using ImGuiNET;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
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
	public class GuiCoroutine
	{
		public IEnumerator crt;
		public volatile bool done = false;

		// Danger, can cause deadlock if used on GUI thread
		public void Wait()
		{
			if (!System.Threading.Thread.CurrentThread.IsBackground)
				throw new Exception("Tried to wait GuiCoroutine on main thread!");

			while (!done) System.Threading.Thread.Sleep(1);
		}

	}

	public class AppGuiWindow
	{
		public string Name;
		public IGuiPanel Panel;
	}

	public class AppGui
	{
		public AppGame Game { get; private set; }

		public SpriteBatch SpriteBatch { get; private set; }
		public RenderTarget2D BackBuffer { get; private set; }
		private IntPtr guiRenderTarget;

		private List<AppGuiWindow> windows = new List<AppGuiWindow>();

		private List<GuiCoroutine> coroutines = new List<GuiCoroutine>();

		public object HoverObject;

		public bool IsConfirmPressed { get; private set; }
		public bool IsCancelPressed { get; private set; }

		private KeyboardState keyboardState, prevKeyboardState;

		private GuiTexture _genericPreviewTexture;

		public static GuiCoroutine RunGuiCoroutine(IEnumerator crt)
		{
			lock (AppGame.Gui.coroutines) {
				GuiCoroutine c = new GuiCoroutine()
				{
					crt = crt
				};
				AppGame.Gui.coroutines.Add(c);
				return c;
			}
		}

		static IEnumerator RunOnUICrt(Action action)
		{
			yield return null;
			action();
		}

		public static GuiCoroutine RunGuiCoroutine(Action action)
		{
			return RunGuiCoroutine(RunOnUICrt(action));
		}

		private bool _ready = false;

		public AppGui(AppGame game)
		{
			Game = game;
			SpriteBatch = new SpriteBatch(game.GraphicsDevice);
			BackBuffer = new RenderTarget2D(game.GraphicsDevice, 1, 1);
			guiRenderTarget = AppGame.GuiRenderer.BindTexture(BackBuffer);

			Texture2D t = new Texture2D(game.GraphicsDevice, 512, 512);
			_genericPreviewTexture = new GuiTexture(t);
		}



		private void ResizeRenderTarget(Vector2 size)
		{
			if (BackBuffer.Width != size.X || BackBuffer.Height != size.Y) {
				if (size.X > 1 && size.Y > 1) {

					AppGame.GuiRenderer.UnbindTexture(guiRenderTarget);
					BackBuffer.Dispose();

					BackBuffer = new RenderTarget2D(Game.GraphicsDevice, (int)size.X, (int)size.Y);
					guiRenderTarget = AppGame.GuiRenderer.BindTexture(BackBuffer);
				}
			}
		}

		public void Draw(Vector2 clientSize)
		{
			HoverObject = null;

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

			UpdateInput();

			EditorFieldDrawer.ResetIds();

			EditorModalSelect.DrawSelectItemModal();

			if (Ready()) {

				foreach (var win in windows) {
					ImGui.SetNextWindowSize(new Vector2(800, 600), ImGuiCond.FirstUseEver);

					if (ImGui.Begin(win.Name)) {
						win.Panel.Draw(clientSize);
						ImGui.End();
					}
				}
			}
			TooltipForObject(HoverObject);

			UpdateCoroutines();

			AppGame.Device.SetRenderTarget(null);
		}

		private bool Ready()
		{
			if (_ready)
				return true;

			if (Game.StartupOperation != null && !Game.StartupOperation.Completed) {
				ProgressDialog(Game.StartupOperation);
			} else {

				if (!_ready) {
					var types = GetType().Assembly.GetTypes().Where(t => !t.IsAbstract && !t.IsInterface && t.IsAssignableTo(typeof(IGuiPanel)))
	.ToList();

					foreach (var t in types) {
						var p = Activator.CreateInstance(t) as IGuiPanel;
						var window = new AppGuiWindow()
						{
							Name = p.WindowName,
							Panel = p,
						};

						windows.Add(window);
					}
					_ready = true;
				}
			}

			return _ready;
		}

		private void UpdateInput()
		{
			prevKeyboardState = keyboardState;
			keyboardState = Keyboard.GetState();
			IsCancelPressed = IsButtonPressed(Keys.Escape);
			IsConfirmPressed = IsButtonPressed(Keys.Enter);
		}

		public bool IsButtonPressed(Keys key)
		{
			return prevKeyboardState.IsKeyDown(key) && keyboardState.IsKeyUp(key);
		}

		public bool UpdateCoroutines()
		{
			for (int i = 0; i < coroutines.Count; ++i) {
				var crt = coroutines[i];
				if (!crt.crt.MoveNext()) {
					crt.done = true;
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
			if (aspectRatio != 0) {
				if (size.X > size.Y) {
					size.X = size.Y * aspectRatio;
				} else if (size.Y > size.X) {
					size.Y = size.X * aspectRatio;
				}
			}

			sbDrawSize.Push(size);

			Vector2 pos = ImGui.GetCursorPos();

			viewport.Push(new Viewport((int)pos.X, (int)pos.Y, (int)size.X, (int)size.Y));

			AppGame.Gui.SpriteBatch.Begin(SpriteSortMode.Deferred, null, samplerState);

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

		private void TooltipForObject(object obj)
		{
			if (obj == null)
				return;


			if (obj is ImageList imgList) {
				ImGui.BeginTooltip();

				DrawImageListInfo(imgList);

				ImGui.EndTooltip();
				return;
			}

			if (obj is ImageListRef imgRef) {
				if (imgRef.Image != null) {
					ImGui.BeginTooltip();

					DrawImageListInfo(imgRef.Image);

					ImGui.EndTooltip();
				}
				return;
			}

			if (obj is ImageFrame frame) {
				ImGui.BeginTooltip();
				DrawImageFrameInfo(frame);
				ImGui.EndTooltip();
				return;
			}


			if (obj is ImageFrameRef imgFrameRef) {
				if (imgFrameRef.Frame != null) {
					ImGui.BeginTooltip();

					DrawImageFrameInfo(imgFrameRef.Frame);

					ImGui.EndTooltip();
				}
				return;
			}

			if (obj is VideoClip videoClip) {

				ImGui.BeginTooltip();
				videoClip.GUIPreview();
				ImGui.EndTooltip();

				return;
			}

			if (obj is VideoClipRef videoClipRef) {
				if (videoClipRef.Clip != null) {
					ImGui.BeginTooltip();
					videoClipRef.Clip.GUIPreview();
					ImGui.EndTooltip();
				}
				return;
			}

			if(obj is Asset asset) {
				if (asset.HasTooltip) {
					ImGui.BeginTooltip();
					ImGui.Text(asset.AssetName);
					ImGui.Separator();
					asset.DrawTooltip();
					ImGui.EndTooltip();
				}
			}
		}


		public static void DrawImageListInfo(ImageList list)
		{
			ImGui.Text($"Dimensions: {list.FrameSize.X}x{list.FrameSize.Y}");
			ImGui.SameLine();
			ImGui.Text($"Frames: {list.Frames.Count}");
			if (list.HasUnitColor) {
				ImGui.SameLine();
				ImGui.Text($"With unit coloring.");
			}

			float scale = 1;
			if (list.FrameSize.X < 16 || list.FrameSize.Y < 16)
				scale = 2;
			else if (list.FrameSize.X > 200 || list.FrameSize.Y > 200)
				scale = 0.5f;

			for (int i = 0; i < list.Frames.Count; ++i) {
				var tex = AppGame.AssetManager.GetImageFrame(list.Key, i);

				if (i % 16 > 0)
					ImGui.SameLine();

				ImGui.Image(tex.GuiImage, new Vector2(tex.Texture.Width, tex.Texture.Height) * scale);

			}
		}
		public static void DrawImageFrameInfo(ImageFrame frame)
		{
			ImGui.Text($"{frame.ImageListName}:{frame.FrameIndex}");
			ImGui.Text($"Index: {frame.FrameIndex / 17} rotated, {frame.FrameIndex % 17} orientation");
			ImGui.Text($"Frame: {frame.ImageList.FrameSize.X}x{frame.ImageList.FrameSize.Y}");
			ImGui.Text($"Size: {frame.Size.X}x{frame.Size.Y}");
			ImGui.Text($"Offset: {frame.XOffset}x{frame.YOffset}");
		}
		public static bool ProgressDialog(string text, int count, int total, bool cancelable = false)
		{
			float progress = (float)count / (float)total;

			return ProgressDialog(text, progress, cancelable);
		}
		public static bool ProgressDialog(string text, float progress, bool cancelable = false)
		{
			return ProgressDialog(null, text, progress, cancelable);
		}
		public static bool ProgressDialog(string title, string text, float progress, bool cancelable = false)
		{
			bool result = true;
			ImGui.OpenPopup("Generic.ProgressBar");
			bool open = true;
			ImGui.BeginPopupModal("Generic.ProgressBar", ref open, ImGuiWindowFlags.AlwaysAutoResize | ImGuiWindowFlags.NoTitleBar);
			if (!string.IsNullOrEmpty(title))
				ImGui.Text(title);
			if (!string.IsNullOrEmpty(text))
				ImGui.Text(text);
			ImGui.ProgressBar(progress, new Vector2(600, 40));
			if (cancelable) {
				result = !ImGui.Button("Cancel");
			}
			ImGui.EndPopup();

			return result;
		}
		public static bool ProgressDialog(AsyncOperation op, bool cancelable = false)
		{
			bool running = ProgressDialog(op.Title + $" ({op.ElaspedTime:hh\\:mm\\:ss})", op.ItemName, op.Progress, cancelable);
			if (!running)
				op.Cancel();

			return running;
		}
	}
}
