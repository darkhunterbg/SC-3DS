using DataManager.Assets;
using DataManager.Panels;
using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Widgets
{
	public static class EditorModalSelect
	{
		private static Type objType;
		private static object selected;
		private static Action<object> callback;
		private static string textFilter = string.Empty;

		public static void SelectItemModal(PropertyInfo info, object obj, Action onChanged)
		{
			SelectItemModal(
				info.PropertyType,
				info.GetValue(obj),
				(o) => { info.SetValue(obj, o); onChanged(); });
		}

		public static void SelectItemModal(Type objType, object selected, Action<object> selectionCompleted)
		{
			EditorModalSelect.objType = objType;
			EditorModalSelect.selected = selected;
			callback = selectionCompleted;
		}

		public static void DrawSelectItemModal()
		{
			bool opened = objType != null;

			if (!opened)
				return;
			ImGui.OpenPopup("modal.select");

			if (!ImGui.BeginPopupModal("modal.select", ref opened, ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.AlwaysAutoResize))
				return;


			Header();
			ImGui.InputText("##modal.select.filter", ref textFilter, 255);
			ImGui.BeginChild("modal.select.items", new Vector2(800, 800));

			Content();

			ImGui.EndChild();

			if (ImGui.Button("Cancel##modal.select.cancel"))
			{
				objType = null;
				selected = null;
				textFilter = string.Empty;

			}

			ImGui.SameLine();

			if (selected != null)
			{
				if (ImGui.Button("Ok##modal.select.ok"))
				{
					callback(selected);

					objType = null;
					selected = null;
					textFilter = string.Empty;
				}
			}
			ImGui.EndPopup();
		}

		private static void Header()
		{
			if (objType.IsSubclassOf(typeof(Asset)))
			{
				var asset = selected as Asset;

				if (asset?.Preview != null)
				{
				
					ImGui.Image(asset.Preview.GuiImage, new Vector2(32, 32));

				}
				ImGui.SameLine();
				ImGui.Text(asset?.AssetName ?? string.Empty);

			}
			if (objType == typeof(SpriteFrame))
			{
				int id = (selected as SpriteFrame)?.FrameIndex ?? -1;
				string text = id != -1 ? id.ToString() : string.Empty;
				ImGui.Text(text);
			}
		}
		private static void Content()
		{
			if (objType.IsSubclassOf(typeof(Asset)))
			{
				AssetContent();
				return;
			}
			
			if (objType == typeof(SpriteFrame))
			{
				IconContent();
				return;
			}
		}


		private static void AssetContent()
		{
			var tmp = selected as Asset;

			var query = AppGame.AssetManager.Assets[objType];
			query = Util.TextFilter(query, textFilter, t => t.AssetName);
			query = query.OrderBy(asset => asset.AssetName);

			int i = 0;

			foreach (var asset in query)
			{
				ImGui.PushID(i++);

				bool isSelectedItem = tmp == asset;

				if (ImGui.Selectable(string.Empty, isSelectedItem))
				{
					if (isSelectedItem)
						selected = null;
					else
						selected = asset;

				}
				if (asset.Preview != null)
				{
					ImGui.SameLine();
					ImGui.Image(asset.Preview.GuiImage, new Vector2(32, 32));
				
				}

				ImGui.SameLine();
				ImGui.Text(asset.AssetName);
				if (ImGui.IsItemHovered())
				{
					AppGame.Gui.HoverObject = asset;

				}

				ImGui.PopID();
			}
		}

		private static void IconContent()
		{
			var image = selected as SpriteFrame;
			var query = AppGame.AssetManager.Icons;

			int i = 0;

			int? filter = null;

			if (int.TryParse(textFilter, out int x))
				filter = x;

			foreach (var asset in query)
			{
				if (filter.HasValue)
				{
					if (asset.FrameIndex != filter)
						continue;
				}

				i++;
				bool isSelectedItem = image == asset;

				Vector4 color = isSelectedItem ? Vector4.One : Vector4.Zero;

				if (ImGui.ImageButton(asset.Image.GuiImage, new Vector2(64, 64), Vector2.Zero,
			Vector2.One, 2,
			color,
			Microsoft.Xna.Framework.Color.Yellow.ToVec4()))
				{
					if (isSelectedItem)
						selected = null;
					else
						selected = asset;
				}


				if (i % 10 != 0)
					ImGui.SameLine();
			}
		}
	}
}
