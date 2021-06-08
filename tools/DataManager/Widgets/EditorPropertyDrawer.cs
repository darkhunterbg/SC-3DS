using DataManager.Assets;
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
	public delegate bool EditorDrawAction(PropertyInfo prop, EditorAttribute attr, object item);


	public static class EditorPropertyDrawer
	{
		static string text;
		static int number;

		static bool? delayedChangedValue = null;

		public static EditorDrawAction GetPropertyDrawer<TType>(bool readOnly = false)
		{
			return GetPropertyDrawer(typeof(TType), readOnly);
		}
		public static EditorDrawAction GetPropertyDrawer(Type type, bool readOnly = false)
		{
			if (readOnly)
				return ReadOnlyEditor;

			if (type == typeof(string))
				return StringEditor;

			if (type == typeof(int))
				return IntEditor;

			if (type == typeof(bool))
				return BoolEditor;

			if (type.IsEnum)
				return EnumEditor;

			if (type == typeof(IconRef))
				return IconEditor;

			if (type == typeof(ImageListRef))
				return ImageListEditor;

			if (type.IsSubclassOf(typeof(Asset)))
				return AssetEditor;

			return ReadOnlyEditor;
		}
		public static EditorDrawAction GetPropertyDrawer(PropertyInfo prop, EditorAttribute attr)
		{
			if (!prop.CanWrite || attr.ReadOnly)
				return ReadOnlyEditor;

			if (attr is DefaultEditorAttribute)
			{
				return GetPropertyDrawer(prop.PropertyType, false);
			}
			else
			{
				if (attr is CustomEnumEditorAttribute)
					return CustomEnumEditor;

				if (attr is FrameTimeEditorAttribute)
					return FrameTimeEditor;

				if (attr is SupplyEditorAttribute)
					return SupplyEditor;
			}


			return ReadOnlyEditor;
		}

		private static bool DelayedChangedValue()
		{
			if (delayedChangedValue == null)
				return false;

			delayedChangedValue = null;
			return true;
		}
		private static void ModalSelect(PropertyInfo prop, object item)
		{
			EditorModalSelect.SelectItemModal(prop, item, () => delayedChangedValue = true);
		}

		public static bool ReadOnlyEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			ImGui.Text(prop.GetValue(item)?.ToString() ?? string.Empty);
			return false;
		}
		private static bool StringEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			text = prop.GetValue(item)?.ToString() ?? string.Empty;

			AppGui.StrechNextItem();
			if (ImGui.InputText(string.Empty, ref text, 256))
			{
				prop.SetValue(item, text);
				return true;
			}

			return false;

		}
		private static bool IntEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			number = (int)prop.GetValue(item);

			AppGui.StrechNextItem();
			if (ImGui.InputInt(string.Empty, ref number))
			{
				prop.SetValue(item, number);
				return true;
			}
			return false;
		}
		private static bool BoolEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			bool value = (bool)prop.GetValue(item);

			AppGui.StrechNextItem();
			if (ImGui.Checkbox(string.Empty, ref value))
			{
				prop.SetValue(item, value);
				return true;
			}
			return false;
		}
		private static bool EnumEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			number = (int)prop.GetValue(item);
			AppGui.StrechNextItem();
			var enumType = prop.PropertyType;
			var enumValues = EnumCacheValues.GetValues(enumType);
			if (ImGui.Combo(string.Empty, ref number, enumValues, enumValues.Length))
			{
				prop.SetValue(item, number);
				return true;
			}

			return false;
		}

		private static bool CustomEnumEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			var customAttr = attr as CustomEnumEditorAttribute;

			var enumValues = CustomEnumValues.CustomEnums[(int)customAttr.EnumType];

			number = (int)prop.GetValue(item);
			AppGui.StrechNextItem();
			if (ImGui.Combo(string.Empty, ref number, enumValues, enumValues.Length))
			{
				prop.SetValue(item, number);
				return true;
			}
			return false;
		}

		private static bool AssetEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			var asset = prop.GetValue(item) as Asset;

			if (asset?.Preview != null)
			{
				ImGui.Image(asset.Preview.GuiImage, new Vector2(32, 32));
				ImGui.SameLine();
			}

			ImGui.Text(asset?.AssetName ?? string.Empty);

			if (ImGui.IsItemHovered())
			{
				AppGame.Gui.HoverObject = asset;
			}

			ImGui.SameLine();

			if (ImGui.Button($"Change"))
			{
				ModalSelect(prop, item);
			}

			return DelayedChangedValue();
		}

		private static bool IconEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			var icon = (IconRef)prop.GetValue(item);

			if (icon.Image == null)
				return false;

			if (ImGui.ImageButton(icon.Image.Image.GuiImage, icon.Image.ImageList.FrameSize))
			{
				ModalSelect(prop, item);
			}
			return DelayedChangedValue();
		}
		private static bool FrameTimeEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			number = (int)prop.GetValue(item);

			bool changed = false;

			ImGui.SetNextItemWidth(100);
			if (ImGui.InputInt(string.Empty, ref number, 0))
			{
				prop.SetValue(item, number);
				changed = true;
			}
			ImGui.SameLine();
			ImGui.Text($"{(number / 15.0f).ToString("F1")} sec");

			return changed;
		}
		private static bool SupplyEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			number = (int)prop.GetValue(item);

			bool changed = false;

			ImGui.SetNextItemWidth(100);
			if (ImGui.InputInt(string.Empty, ref number, 0))
			{
				prop.SetValue(item, number);
				changed = true;
			}
			ImGui.SameLine();
			ImGui.Text($"{(number / 2.0f)}");

			return changed;
		}
		private static bool ImageListEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			var obj = prop.GetValue(item);

			string text = "None";


			if (obj != null)
			{
				var image = (ImageListRef)obj;

				if (!string.IsNullOrEmpty(image.Key))
					text = image.Key;
			}

			ImGui.Text(text);

			if (ImGui.IsItemHovered())
				AppGame.Gui.HoverObject = obj;

			ImGui.SameLine();

			if (ImGui.Button("Change"))
			{
				ModalSelect(prop, item);
			}
			return DelayedChangedValue();
		}

	}
}
