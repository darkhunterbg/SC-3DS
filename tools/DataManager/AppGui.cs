﻿using DataManager.Assets;
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

		public static GuiCoroutine RunGuiCoroutine(IEnumerator crt)
		{
			lock (AppGame.Gui.coroutines)
			{
				GuiCoroutine c = new GuiCoroutine()
				{
					crt = crt
				};
				AppGame.Gui.coroutines.Add(c);
				return c;
			}
		}

		public AppGui(AppGame game)
		{
			Game = game;
			SpriteBatch = new SpriteBatch(game.GraphicsDevice);
			BackBuffer = new RenderTarget2D(game.GraphicsDevice, 1, 1);
			guiRenderTarget = AppGame.GuiRenderer.BindTexture(BackBuffer);

			var types = GetType().Assembly.GetTypes().Where(t => !t.IsAbstract && !t.IsInterface && t.IsAssignableTo(typeof(IGuiPanel)))
				.ToList();

			foreach (var t in types)
			{
				var p = Activator.CreateInstance(t) as IGuiPanel;
				var window = new AppGuiWindow()
				{
					Name = p.WindowName,
					Panel = p,
				};

				windows.Add(window);
			}
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

			foreach (var win in windows)
			{
				ImGui.SetNextWindowSize(new Vector2(800, 600), ImGuiCond.FirstUseEver);

				if (ImGui.Begin(win.Name))
				{
					win.Panel.Draw(clientSize);
					ImGui.End();
				}
			}


			EditorModalSelect.DrawSelectItemModal();

			TooltipForObject(HoverObject);

			UpdateCoroutines();

			AppGame.Device.SetRenderTarget(null);
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
			for (int i = 0; i < coroutines.Count; ++i)
			{
				var crt = coroutines[i];
				if (!crt.crt.MoveNext())
				{
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


			if (obj is ImageList imgList)
			{
				ImGui.BeginTooltip();

				DrawImageListInfo(imgList);

				ImGui.EndTooltip();
				return;
			}

			if (obj is ImageListRef imgRef)
			{
				if (imgRef.Image != null)
				{
					ImGui.BeginTooltip();

					DrawImageListInfo(imgRef.Image);

					ImGui.EndTooltip();
				}
				return;
			}
		}


		public static void DrawImageListInfo(ImageList list)
		{
			ImGui.Text($"Dimensions: {list.FrameSize.X}x{list.FrameSize.Y}");
			ImGui.SameLine();

			ImGui.Text($"Frames: {list.Frames.Count}");

			float scale = 1;
			if (list.FrameSize.X < 16 || list.FrameSize.Y < 16)
				scale = 2;
			else if (list.FrameSize.X > 200 || list.FrameSize.Y > 200)
				scale = 0.5f;

			for (int i = 0; i < list.Frames.Count; ++i)
			{
				var tex = AppGame.AssetManager.GetImageFrame(list.Key, i);

				if (i % 16 > 0)
					ImGui.SameLine();

				ImGui.Image(tex.GuiImage, new Vector2(tex.Texture.Width, tex.Texture.Height) * scale);

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
