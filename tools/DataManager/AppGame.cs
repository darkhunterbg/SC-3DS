using ImGuiNET;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using MonoGame.ImGui.Standard;
using System;
using System.Runtime.InteropServices;

namespace DataManager
{
	public class AppGame : Microsoft.Xna.Framework.Game
	{
		private GraphicsDeviceManager _graphics;
		private SpriteBatch _spriteBatch;

		public static ImGUIRenderer GuiRenderer { get; private set; }
		private ImFontPtr font;

		public AppGui Gui { get; private set; }

		public static GraphicsDevice Device { get; private set; }
		public static AssetManager AssetManager { get; private set; }

		[DllImport("SDL2.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern void SDL_MaximizeWindow(IntPtr window);

		public AppGame()
		{
			_graphics = new GraphicsDeviceManager(this);
			Content.RootDirectory = "Content";
			IsMouseVisible = true;
			Window.AllowUserResizing = true;
	
			InactiveSleepTime = TimeSpan.Zero;

			_graphics.GraphicsProfile = GraphicsProfile.HiDef;
		}

		protected override void Initialize()
		{
			GuiRenderer = new ImGUIRenderer(this).Initialize();
			font = ImGui.GetIO().Fonts.AddFontFromFileTTF(@"C:\Windows\Fonts\ARIAL.TTF", 24);
			ImGui.GetIO().ConfigFlags |= ImGuiConfigFlags.DockingEnable;
			ImGui.GetStyle().Alpha = 1.0f;
			ImGui.GetIO().ConfigDockingAlwaysTabBar = true;
			GuiRenderer.RebuildFontAtlas();

			base.Initialize();

			_graphics.PreferredBackBufferWidth = 1920;
			_graphics.PreferredBackBufferHeight = 1080;
			_graphics.ApplyChanges();

			//SDL_MaximizeWindow(Window.Handle);

			Device = _graphics.GraphicsDevice;

			AssetManager = new AssetManager();

			Gui = new AppGui(this);
		}

		protected override void LoadContent()
		{
			_spriteBatch = new SpriteBatch(base.GraphicsDevice);
		}

		protected override void Update(GameTime gameTime)
		{
			if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed || Keyboard.GetState().IsKeyDown(Keys.Escape))
				Exit();

			Gui.Update();

			base.Update(gameTime);
		}

		protected override void Draw(GameTime gameTime)
		{
			base.GraphicsDevice.Clear(Color.Black);

			base.Draw(gameTime);

			GuiRenderer.BeginLayout(gameTime);

			ImGui.PushFont(font);

			var clientSize = new System.Numerics.Vector2(Window.ClientBounds.Width, Window.ClientBounds.Height);

			//ImGui.Begin("Test");

			//ImGui.End();

			Gui.Draw(clientSize);

			ImGui.PopFont();

			GuiRenderer.EndLayout();

			GC.Collect();
		}
	}
}
