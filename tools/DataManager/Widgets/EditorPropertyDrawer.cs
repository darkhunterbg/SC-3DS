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
		static  int number;

		static bool? delayedChangedValue = null;

		public static EditorDrawAction GetPropertyDrawer(PropertyInfo prop, EditorAttribute attr)
		{
			if (!prop.CanWrite || attr.ReadOnly)
				return ReadOnlyEditor;

			if (attr is DefaultEditorAttribute)
			{
				var type = prop.PropertyType;

				if (type == typeof(string))
					return StringEditor;

				if (type == typeof(int))
					return IntEditor;

				if (type.IsEnum)
					return EnumEditor;
			}
			else
			{
				if (attr is CustomEnumEditorAttribute)
					return CustomEnumEditor;

				if (attr is ImageEditorAttribute)
					return LogicalImageEditor;

				if (attr is IconEditorAttribute)
					return IconEditor;

				if (attr is FrameTimeEditorAttribute)
					return FrameTimeEditor;

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
			if (ImGui.InputInt(string.Empty ,ref number))
			{
				prop.SetValue(item, number);
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
		private static bool LogicalImageEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			var image = prop.GetValue(item) as LogicalImageAsset;

			ImGui.Text(image.SpriteSheetName ?? string.Empty);

			if (ImGui.IsItemHovered())
			{
				AppGame.Gui.HoverObject = image;
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
			var icon = prop.GetValue(item) as SpriteFrameAsset;

			if (ImGui.ImageButton(icon.Image.GuiImage, icon.SpriteSheet.FrameSize, Vector2.Zero,
				Vector2.One, 0,
				Vector4.Zero,
				Microsoft.Xna.Framework.Color.Yellow.ToVec4()))
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
	}
}
