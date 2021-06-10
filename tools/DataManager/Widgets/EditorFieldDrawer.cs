using DataManager.Assets;
using ImGuiNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Widgets
{
	public static class EditorFieldDrawer
	{
		static bool? delayedChangedValue = null;
		private static bool DelayedChangedValue()
		{
			if (delayedChangedValue == null)
				return false;

			delayedChangedValue = null;
			return true;
		}

		public static void ReadOnly(object obj)
		{
			ImGui.Text(obj.ToString() ?? string.Empty);
		}

		static string editText = string.Empty;

		public static object Object(object obj, Type type, out bool changed)
		{
			if (type == typeof(string))
			{
				return String(obj as string, out changed);
			}
			if (type == typeof(int))
			{
				int i = obj == null ? default : (int)obj;
				return Int(i, out changed);
			}
			if (type == typeof(bool))
			{
				bool b = obj == null ? default : (bool)obj;
				return Bool(b, out changed);
			}
			if (type.IsEnum)
			{
				int i = obj == null ? default : (int)obj;
				return Enum((Enum)(object)i, out changed);
			}
			if (type.IsSubclassOf(typeof(Asset)))
			{
				Asset a = obj as Asset;
				return Asset(a, out changed);
			}
			if (type == typeof(IconRef))
			{
				IconRef b = obj == null ? IconRef.None : (IconRef)obj;
				return Icon(b, out changed);
			}
			if (type == typeof(ImageListRef))
			{
				ImageListRef b = obj == null ? ImageListRef.None : (ImageListRef)obj;
				return ImageList(b, out changed);
			}

			changed = false;
			ReadOnly(obj);
			return obj;
		}
		public static object Object(object obj, out bool changed)
		{
			if (obj is string s)
			{
				return String(s, out changed);
			}
			if (obj is int i)
			{
				return Int(i, out changed);

			}
			if (obj is bool b)
			{
				return Bool(b, out changed);
			}
			if (obj is Enum e)
			{
				return Enum(e, out changed);
			}
			if (obj is Asset a)
			{
				return Asset(a, out changed);
			}
			if (obj is IconRef ic)
			{
				return Icon(ic, out changed);
			}
			if(obj is ImageListRef il)
			{
				return ImageList(il, out changed);
			}

			changed = false;
			ReadOnly(obj);
			return obj;
		}

		public static bool String(ref string text)
		{
			return ImGui.InputText(string.Empty, ref text, 1024);
		}
		public static string String(string text, out bool changed)
		{
			editText = text;
			changed = String(ref editText);
			return editText;
		}

		public static bool Int(ref int number)
		{
			return ImGui.InputInt(string.Empty, ref number);
		}
		public static int Int(int number, out bool changed)
		{
			changed = Int(ref number);
			return number;
		}

		public static bool Bool(ref bool b)
		{
			return ImGui.Checkbox(string.Empty, ref b);
		}
		public static bool Bool(bool b, out bool changed)
		{
			changed = Bool(ref b);
			return b;
		}

		public static bool Enum<TType>(ref TType value) where TType : struct, Enum
		{
			var values = EnumCacheValues.GetValues<TType>();
			int editNumber = (int)(object)(value);
			if (ImGui.Combo(string.Empty, ref editNumber, values, values.Length))
			{
				value = (TType)(object)editNumber;
				return true;
			}

			return false;
		}
		public static TType Enum<TType>(TType value, out bool changed) where TType : struct, Enum
		{
			changed = Enum(ref value);
			return value;
		}
		public static bool Enum(ref Enum value)
		{
			var values = EnumCacheValues.GetValues(value.GetType());
			int editNumber = (int)(object)(value);
			if (ImGui.Combo(string.Empty, ref editNumber, values, values.Length))
			{
				value = (Enum)((object)editNumber);
				return true;
			}
			return false;
		}
		public static Enum Enum(Enum value, out bool changed)
		{
			changed = Enum(ref value);
			return value;
		}

		public static bool CustomEnum<TType>(CustomEnumType type, ref TType value) where TType : struct, Enum
		{
			var values = CustomEnumValues.CustomEnums[(int)type];

			int editNumber = (int)(object)(value);
			if (ImGui.Combo(string.Empty, ref editNumber, values, values.Length))
			{
				value = (TType)(object)editNumber;
				return true;
			}

			return false;
		}
		public static TType Enum<TType>(CustomEnumType type, TType value, out bool changed) where TType : struct, Enum
		{
			changed = CustomEnum(type, ref value);
			return value;
		}

		public static bool CustomEnum(CustomEnumType type, ref Enum value)
		{
			var values = CustomEnumValues.CustomEnums[(int)type];
			int editNumber = (int)(object)(value);
			if (ImGui.Combo(string.Empty, ref editNumber, values, values.Length))
			{
				value = (Enum)((object)editNumber);
				return true;
			}
			return false;
		}
		public static Enum CustomEnum(CustomEnumType type, Enum value, out bool changed)
		{
			changed = CustomEnum(type, ref value);
			return value;
		}

		public static Asset Asset(Asset asset, out bool changed)
		{
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
				EditorModalSelect.SelectItemModal(typeof(Asset), asset, (o) =>
				{
					asset = o as Asset; delayedChangedValue = true;
				});
			}

			changed = DelayedChangedValue();
			return asset;
		}
		private static IconRef Icon(IconRef icon, out bool changed)
		{
			changed = false;
			if (icon.Image == null)
				return icon;

			if (ImGui.ImageButton(icon.Image.Image.GuiImage, icon.Image.ImageList.FrameSize))
			{
				EditorModalSelect.SelectItemModal(typeof(IconRef), icon, (o) =>
				{
					icon = (IconRef)o; delayedChangedValue = true;
				});
			}
			changed = DelayedChangedValue();
			return icon;
		}

		private static ImageListRef ImageList(ImageListRef image, out bool changed)
		{
			string text = "None";


			if (!string.IsNullOrEmpty(image.Key))
				text = image.Key;


			ImGui.Text(text);

			if (ImGui.IsItemHovered())
				AppGame.Gui.HoverObject = image;

			ImGui.SameLine();

			if (ImGui.Button("Change"))
			{
				EditorModalSelect.SelectItemModal(typeof(ImageListRef), image, (o) =>
				{
					image = (ImageListRef)o; delayedChangedValue = true;
				});
			}
			changed = DelayedChangedValue();
			return image;
		}
	}
}
