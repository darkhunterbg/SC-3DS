using DataManager.Assets;
using ImGuiNET;
using System;
using System.Collections;
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
				return GenericPropertyDrawer;
			}
			else
			{
				if (attr is CustomEnumEditorAttribute)
					return CustomEnumEditor;

				if (attr is FrameTimeEditorAttribute)
					return FrameTimeEditor;

				if (attr is SupplyEditorAttribute)
					return SupplyEditor;

				if (attr is ImageFrameEditorAttribute)
					return ImageFrameEditor;

				if (attr is ListEditorAttribute)
				{
					return ListEditor;
				}
			}


			return ReadOnlyEditor;
		}

		private static bool ReadOnlyEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			AppGui.StrechNextItem();
			EditorFieldDrawer.ReadOnly(string.Empty, prop.GetValue(item));
			return false;
		}
		private static bool GenericPropertyDrawer(PropertyInfo p, EditorAttribute a, object i)
		{
			var v = p.GetValue(i);
			AppGui.StrechNextItem();
			v = EditorFieldDrawer.Object(string.Empty, v, p.PropertyType, out bool changed);
			if (changed)
				p.SetValue(i, v);

			return changed;
		}

		private static bool CustomEnumEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			var customAttr = attr as CustomEnumEditorAttribute;

			int val = (int)prop.GetValue(item);
			AppGui.StrechNextItem();
			val = EditorFieldDrawer.CustomEnum(string.Empty, customAttr.EnumType, val, out bool changed);
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

			EditorFieldDrawer.FrameTime(string.Empty, number, out bool changed);
			if (changed)
			{
				prop.SetValue(item, number);
			}

			return changed;
		}
		private static bool SupplyEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			int number = (int)prop.GetValue(item);

			EditorFieldDrawer.Supply(string.Empty, number, out bool changed);
			if (changed)
			{
				prop.SetValue(item, number);
			}

			return changed;
		}

		private static bool ImageFrameEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			var v = prop.GetValue(item);
			var frame = v == null ? ImageFrameRef.None : (ImageFrameRef)v;

			var a = attr as ImageFrameEditorAttribute;

			if (frame.ImageListName == string.Empty)
				frame = new ImageFrameRef(AppGame.AssetManager.ImageLists.FirstOrDefault(f => f.Key == a.ImagePath));

			AppGui.StrechNextItem();
			frame = EditorFieldDrawer.ImageFrame(string.Empty, frame, out bool changed);
			if (changed)
				prop.SetValue(item, frame);

			return changed;
		}

		private static bool ListEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			var listElemType = prop.PropertyType.GetGenericArguments()[0];

			var list = prop.GetValue(item) as IList;
			bool changed = false;


			int j = 0;
			for (int i = 0; i < list.Count; ++i)
			{
				ImGui.PushID(++j);
				object o = EditorFieldDrawer.Object(string.Empty, list[i], listElemType, out bool c);
				if (c)
				{
					changed = true;
					list[i] = o;
				}
				ImGui.SameLine();
				ImGui.SetCursorPosX(ImGui.GetCursorPosX() + 8);
				if (ImGui.Button("Delete"))
				{
					list.RemoveAt(i);
					--i;
					changed = true;
				}
				ImGui.PopID();
			}

			if (ImGui.Button("Add"))
			{
				list.Add(Activator.CreateInstance(listElemType));
				changed = true;
			}
			ImGui.SameLine();

			if (ImGui.Button("Clear"))
			{
				list.Clear();
				changed = true;
			}

			ImGui.Separator();

			return changed;
		}
	}
}
