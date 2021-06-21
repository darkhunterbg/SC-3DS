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

				if (attr is ArrayEditorAttribute)
				{
					return ArrayEditor;
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

		private static bool ArrayEditor(PropertyInfo prop, EditorAttribute attr, object item)
		{
			ArrayEditorAttribute a = attr as ArrayEditorAttribute;

			var arrayElemType = prop.PropertyType.GetElementType();

			var editorProperties = arrayElemType.GetProperties().Where(t => t.GetCustomAttribute<EditorAttribute>() != null).ToList();
			var array = prop.GetValue(item) as Array;
			bool changed = false;

			for (int i = 0; i < a.ArraySize; ++i)
			{
				if (array.GetValue(i) == null)
					array.SetValue(Activator.CreateInstance(arrayElemType), i);


				foreach (var p in editorProperties)
				{
					var pa = p.GetCustomAttribute<EditorAttribute>();
					changed = GetPropertyDrawer(p, pa)(p, pa, array.GetValue(i));
				}
			}

			return changed;
		}
	}
}
