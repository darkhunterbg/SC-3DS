using DataManager.Panels;
using ImGuiNET;
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

		private List<IEnumerator> coroutines = new List<IEnumerator>();

		public static void RunGuiCoroutine(IEnumerator crt)
		{
			AppGame.Gui.coroutines.Add(crt);
		}

		public AppGui(AppGame game)
		{
			Game = game;
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

			AssetConverter.Update();
			AssetConverter.Draw();
			SpriteAtlasGenerator.Draw();
			ImageEditor.Draw();

			UpdateCoroutines();
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
