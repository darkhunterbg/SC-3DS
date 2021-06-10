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
		public static EditorDrawAction GetPropertyDrawer(PropertyInfo prop, EditorAttribute attr)
		{
			if (!prop.CanWrite || attr.ReadOnly)
				return ReadOnlyEditor;

			if (attr is DefaultEditorAttribute)
			{
				return GenerirPropertyDrawer;
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




		private static bool ReadOnlyEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			AppGui.StrechNextItem();
			EditorFieldDrawer.ReadOnly(prop.GetValue(item));
			return false;
		}
		private static bool GenerirPropertyDrawer(PropertyInfo p, EditorAttribute a, object i)
		{
			var v = p.GetValue(i);
			AppGui.StrechNextItem();
			v = EditorFieldDrawer.Object(v, out bool changed);
			if (changed)
				p.SetValue(i, v);

			return changed;
		}

		private static bool CustomEnumEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			var customAttr = attr as CustomEnumEditorAttribute;

			Enum val = (Enum)prop.GetValue(item);
			AppGui.StrechNextItem();
			val = EditorFieldDrawer.CustomEnum(customAttr.EnumType, val, out bool changed);
			if (changed)
			{
				prop.SetValue(item, val);
				return true;
			}
			return changed;
		}

	
		private static bool FrameTimeEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			int number = (int)prop.GetValue(item);

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
			int number = (int)prop.GetValue(item);

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


	}
}
