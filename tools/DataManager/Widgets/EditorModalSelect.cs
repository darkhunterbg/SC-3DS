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
			if (objType == typeof(LogicalImageAsset))
			{
				string text = (selected as LogicalImageAsset)?.SpriteSheet?.SheetName ?? string.Empty;
				ImGui.Text(text);

				ImGui.InputText("##modal.select.filter", ref textFilter, 255);
				return;
			}
			if (objType == typeof(SpriteFrameAsset))
			{
				int id = (selected as SpriteFrameAsset)?.FrameIndex ?? -1;
				ImGui.Text(id != -1 ? id.ToString() : string.Empty);


				ImGui.InputText("##modal.select.filter", ref textFilter, 255);
				
				return;
			}
		}
		private static void Content()
		{
			if (objType == typeof(LogicalImageAsset))
			{
				LogicalImageAssetContent();
				return;
			}
			if (objType == typeof(SpriteFrameAsset))
			{
				IconContent();
				return;
			}
		}


		private static void LogicalImageAssetContent()
		{
			var tmp = selected as LogicalImageAsset;

			var query = ImageEditor.GetImages(textFilter);
			query = query.OrderBy(asset => asset.SpriteSheetName);

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
				ImGui.SameLine();
				ImGui.Text(asset.SpriteSheetName);
				if (ImGui.IsItemHovered())
				{
					AppGame.Gui.HoverObject = asset;

				}

				ImGui.PopID();
			}
		}
		private static void IconContent()
		{
			var image = selected as SpriteFrameAsset;
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
