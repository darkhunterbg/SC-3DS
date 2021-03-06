using DataManager.Widgets;
using ImGuiNET;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using MonoGame.ImGui;
using System;
using System.Collections;
using System.Runtime.InteropServices;

namespace DataManager
{
	public class AppGame : Game
	{
		private GraphicsDeviceManager _graphics;

		public static ImGUIRenderer GuiRenderer { get; private set; }
		public static GuiTexture White { get; private set; }
		private ImFontPtr font;

		public static AppGui Gui { get; private set; }

		public static GraphicsDevice Device { get; private set; }
		public static AssetManager AssetManager { get; private set; }

		public static Random Rand = new Random();

		private SpriteBatch sb;

		public AsyncOperation StartupOperation { get; private set; }

		[DllImport("SDL2.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern void SDL_MaximizeWindow(IntPtr window);

		public static GuiCoroutine RunCoroutine(IEnumerator crt)
		{
			return AppGui.RunGuiCoroutine(crt);
		}

		public AppGame()
		{
			_graphics = new GraphicsDeviceManager(this);
			Content.RootDirectory = "Content";
			IsMouseVisible = true;
			Window.AllowUserResizing = true;
			InactiveSleepTime = TimeSpan.Zero;

			_graphics.GraphicsProfile = GraphicsProfile.HiDef;

			SoundEffect.MasterVolume = 0.5f;
		}

		protected override void Initialize()
		{
			base.Initialize();

			_graphics.PreferredBackBufferWidth = 1920;
			_graphics.PreferredBackBufferHeight = 1080;
			_graphics.ApplyChanges();

			SDL_MaximizeWindow(Window.Handle);

			GuiRenderer = new ImGUIRenderer(this).Initialize();

			font = ImGui.GetIO().Fonts.AddFontFromFileTTF(@"C:\Windows\Fonts\ARIAL.TTF", 24);
			ImGui.GetIO().ConfigFlags |= ImGuiConfigFlags.DockingEnable;
			//ImGui.StyleColorsDark();
			ImGui.GetStyle().Alpha = 1.0f;
			ImGui.GetIO().ConfigDockingAlwaysTabBar = true;
			GuiRenderer.RebuildFontAtlas();

			Texture2D white = new Texture2D(GraphicsDevice, 1, 1);
			white.SetData<Color>(new Color[] { Color.White });
			White = new GuiTexture(white);


			sb = new SpriteBatch(GraphicsDevice);

			Device = _graphics.GraphicsDevice;

			AssetManager = new AssetManager();
			this.StartupOperation = AssetManager.LoadEverythingAsync();
			//AssetManager.LoadEverything();
			Gui = new AppGui(this);

			var version = ImGui.GetVersion();
		}

		protected override void Update(GameTime gameTime)
		{
			//if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed || Keyboard.GetState().IsKeyDown(Keys.Escape))
			//	Exit();

			base.Update(gameTime);
		}

		protected override void Draw(GameTime gameTime)
		{

			base.Draw(gameTime);
			GraphicsDevice.Clear(Color.Black);

			GuiRenderer.BeginLayout(gameTime);

			ImGui.PushFont(font);

			var clientSize = new System.Numerics.Vector2(Window.ClientBounds.Width, Window.ClientBounds.Height);

			// CRITICAL! for fixing bug with ImgGui/MonoGame when texture is generated
			sb.Begin();
			sb.End();

			Gui.Draw(clientSize);

			//ImGui.ShowDemoWindow();

			ImGui.PopFont();

			GuiRenderer.EndLayout();

			GC.Collect();
		}
	}
}
