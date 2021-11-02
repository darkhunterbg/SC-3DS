﻿using DataManager.Assets;
using DataManager.Assets.Raw;
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
		static int dialogId = 0;
		static object dialogResult = null;
		static int dialogResultFor = 0;

		private static void DrawName(string text)
		{
			if (!string.IsNullOrEmpty(text))
			{
				ImGui.Text(text);
				ImGui.SameLine();
			}
		}
		public static void ResetIds()
		{
			dialogId = 0;
			dialogResultFor = 0;
			dialogResult = null;
		}

		public static void ReadOnly(string name, object obj)
		{
			//DrawName(name);
			ImGui.Text(obj?.ToString() ?? string.Empty);
		}

		public static object Object(string name, object obj, Type type, out bool changed)
		{
			if (type == typeof(string))
			{
				return String(name, obj as string, out changed);
			}
			if (type == typeof(int))
			{
				int i = obj == null ? default : (int)obj;
				return Int(name, i, out changed);
			}
			if (type == typeof(bool))
			{
				bool b = obj == null ? default : (bool)obj;
				return Bool(name, b, out changed);
			}
			if (type.IsEnum)
			{
				Enum i = obj == null ? default : (Enum)obj;
				return Enum(name, i, out changed);
			}
			if (type.IsSubclassOf(typeof(Asset)))
			{
				Asset a = obj as Asset;
				return Asset(name, a, type, out changed);
			}
			if (type == typeof(IconRef))
			{
				IconRef b = obj == null ? IconRef.None : (IconRef)obj;
				return Icon(name, b, out changed);
			}
			if (type == typeof(ImageListRef))
			{
				ImageListRef b = obj == null ? ImageListRef.None : (ImageListRef)obj;
				return ImageList(name, b, out changed);
			}
			if (type == typeof(ImageFrameRef))
			{
				ImageFrameRef b = obj == null ? ImageFrameRef.None : (ImageFrameRef)obj;
				return ImageFrame(name, b, out changed);
			}
			if (type == typeof(Vector2))
			{
				Vector2 b = obj == null ? System.Numerics.Vector2.Zero : (Vector2)obj;
				return Vector2(name, b, out changed);
			}
			if (type == typeof(AudioClipRef))
			{
				AudioClipRef b = obj == null ? AudioClipRef.None : (AudioClipRef)obj;
				return AudioClip(name, b, out changed);
			}
			if (type == typeof(VideoClipRef)) {
				VideoClipRef b = obj == null ? VideoClipRef.None : (VideoClipRef)obj;
				return VideoClip(name, b, out changed);
			}


			changed = false;
			ReadOnly(name, obj);
			return obj;
		}
		public static object Object(string name, object obj, out bool changed)
		{
			if (obj is string s)
			{
				return String(name, s, out changed);
			}
			if (obj is int i)
			{
				return Int(name, i, out changed);

			}
			if (obj is bool b)
			{
				return Bool(name, b, out changed);
			}
			if (obj is Enum e)
			{
				return Enum(name, e, out changed);
			}
			if (obj is Asset a)
			{
				return Asset(name, a, obj.GetType(), out changed);
			}
			if (obj is IconRef ic)
			{
				return Icon(name, ic, out changed);
			}
			if (obj is ImageListRef il)
			{
				return ImageList(name, il, out changed);
			}
			if (obj is ImageFrameRef imf)
			{
				return ImageFrame(name, imf, out changed);
			}
			if (obj is Vector2 v2)
			{
				return Vector2(name, v2, out changed);
			}
			if (obj is AudioClipRef ac)
			{
				return AudioClip(name, ac, out changed);
			}

			changed = false;
			ReadOnly(name, obj);
			return obj;
		}

		public static bool String(string name, ref string text)
		{
			//DrawName(name);
			return ImGui.InputText(name, ref text, 1024);
		}
		public static string String(string name, string text, out bool changed)
		{
			changed = String(name, ref text);
			return text;
		}

		public static bool Int(string name, ref int number)
		{
			//DrawName(name);
			return ImGui.InputInt(name, ref number);
		}
		public static int Int(string name, int number, out bool changed)
		{
			changed = Int(name, ref number);
			return number;
		}

		public static bool Bool(string name, ref bool b)
		{
			//DrawName(name);
			return ImGui.Checkbox(name, ref b);
		}
		public static bool Bool(string name, bool b, out bool changed)
		{
			changed = Bool(name, ref b);
			return b;
		}

		public static bool Enum<TType>(string name, ref TType value) where TType : struct, Enum
		{
			//DrawName(name);
			var values = EnumCacheValues.GetValues<TType>();
			int editNumber = (int)(object)(value);
			if (ImGui.Combo(name, ref editNumber, values, values.Length))
			{
				value = (TType)(object)editNumber;
				return true;
			}

			return false;
		}
		public static TType Enum<TType>(string name, TType value, out bool changed) where TType : struct, Enum
		{
			changed = Enum(name, ref value);
			return value;
		}
		public static bool Enum(string name, ref Enum value)
		{
			//DrawName(name);
			var values = EnumCacheValues.GetValues(value.GetType());
			int editNumber = (int)(object)(value);
			if (ImGui.Combo(name, ref editNumber, values, values.Length))
			{
				value = (Enum)System.Enum.GetValues(value.GetType()).GetValue(editNumber);
				return true;
			}
			return false;
		}
		public static Enum Enum(string name, Enum value, out bool changed)
		{
			changed = Enum(name, ref value);
			return value;
		}

		public static bool CustomEnum<TType>(string name, CustomEnumType type, ref TType value) where TType : struct, Enum
		{
			//DrawName(name);
			var values = CustomEnumValues.CustomEnums[(int)type];

			int editNumber = (int)(object)(value);
			if (ImGui.Combo(name, ref editNumber, values, values.Length))
			{
				value = (TType)(object)editNumber;
				return true;
			}

			return false;
		}
		public static TType Enum<TType>(string name, CustomEnumType type, TType value, out bool changed) where TType : struct, Enum
		{
			changed = CustomEnum(name, type, ref value);
			return value;
		}

		public static bool CustomEnum(string name, CustomEnumType type, ref int value)
		{
			//DrawName(name);
			var values = CustomEnumValues.CustomEnums[(int)type];
			int editNumber = (int)(object)(value);
			if (ImGui.Combo(name, ref editNumber, values, values.Length))
			{
				value = (int)((object)editNumber);
				return true;
			}
			return false;
		}
		public static int CustomEnum(string name, CustomEnumType type, int value, out bool changed)
		{
			changed = CustomEnum(name, type, ref value);
			return value;
		}

		static int[] vector2 = new int[2];

		public static bool Vector2(string name, ref Vector2 value)
		{
			vector2[0] = (int)value.X;
			vector2[1] = (int)value.Y;
			bool changed = ImGui.DragInt2(name, ref vector2[0], 0.025f);
			if (changed)
			{
				value.X = vector2[0];
				value.Y = vector2[1];
			}
			return changed;
		}

		public static Vector2 Vector2(string name, Vector2 value, out bool result)
		{
			result = Vector2(name, ref value);
			return value;
		}

		public static void AssetReadonly(string name, Asset asset)
		{
			if (asset?.Preview != null) {
				ImGui.Image(asset.Preview.GuiImage, new Vector2(32, 32));
				ImGui.SameLine();
			}

			if (asset?.ActionButtonText != null) {
				if (ImGui.Button(asset.ActionButtonText))
					asset.Activate();
				ImGui.SameLine();
			}

			ImGui.Text(asset?.AssetName ?? "None");

			if (ImGui.IsItemHovered()) {
				AppGame.Gui.HoverObject = asset;
			}

		}
		public static Asset Asset(string name, Asset asset, Type assetType, out bool changed)
		{
			int id = ++dialogId;
			//DrawName(name);
			if (asset?.Preview != null)
			{
				ImGui.Image(asset.Preview.GuiImage, new Vector2(32, 32));
				ImGui.SameLine();
			}

			if (asset?.ActionButtonText != null)
			{
				if (ImGui.Button(asset.ActionButtonText))
					asset.Activate();
				ImGui.SameLine();
			}

			ImGui.Text(asset?.AssetName ?? "None");

			if (ImGui.IsItemHovered())
			{
				AppGame.Gui.HoverObject = asset;
			}

			ImGui.SameLine();

			if (ImGui.Button($"Change"))
			{
				EditorModalSelect.SelectItemModal(assetType, asset, (o) =>
				{
					dialogResultFor = id;
					dialogResult = o;
				});
			}

			changed = dialogResultFor == dialogId;
			return changed ? (Asset)dialogResult : asset;
		}

		public static IconRef Icon(string name, IconRef icon, out bool changed)
		{
			int id = ++dialogId;
			//DrawName(name);
			changed = false;
			if (icon.Image == null)
			{
				if (ImGui.Button("None"))
				{
					EditorModalSelect.SelectItemModal(typeof(IconRef), icon, (o) =>
					{
						dialogResultFor = id;
						dialogResult = o;
					});
				}
			}
			else
			{
				if (ImGui.ImageButton(icon.Image.Image.GuiImage, new Vector2(24)))
				{
					EditorModalSelect.SelectItemModal(typeof(IconRef), icon, (o) =>
					{
						dialogResultFor = id;
						dialogResult = o;
					});
				}
			}

			changed = dialogResultFor == dialogId;
			return changed ? (IconRef)dialogResult : icon;
		}

		public static AudioClipRef AudioClip(string name, AudioClipRef clip, out bool changed)
		{
			int id = ++dialogId;
			//DrawName(name);
			changed = false;
			string text = string.IsNullOrEmpty(clip.Id) ? "None" : clip.Id;
			if (clip.Clip != null)
			{
				if (!clip.Clip.IsPlaying)
				{
					if (ImGui.Button("Play"))
						clip.Clip.Play();
				}
				else
				{
					if (ImGui.Button("Stop"))
						clip.Clip.Stop();
				}

				ImGui.SameLine();
			}

			ImGui.Text(text);
			ImGui.SameLine();
			if (ImGui.Button("Change"))
			{
				EditorModalSelect.SelectItemModal(typeof(AudioClipRef), clip, (o) =>
				{
					dialogResultFor = id;
					dialogResult = o;
				});
			}

			changed = dialogResultFor == dialogId;
			return changed ? (AudioClipRef)dialogResult : clip;
		}

		public static VideoClipRef VideoClip(string name, VideoClipRef clip, out bool changed)
		{
			int id = ++dialogId;
			//DrawName(name);
			changed = false;
			if (clip.Clip == null) {
				if (ImGui.Button("None")) {
					EditorModalSelect.SelectItemModal(typeof(VideoClipRef), clip, (o) =>
					{
						dialogResultFor = id;
						dialogResult = o;
					});
				}
			} else {
				if (ImGui.Button(clip.Clip.RelativePath)) {
					EditorModalSelect.SelectItemModal(typeof(VideoClipRef), clip, (o) =>
					{
						dialogResultFor = id;
						dialogResult = o;
					});
				}

				if (ImGui.IsItemHovered()) {
					AppGame.Gui.HoverObject = clip;
				}
			}

			changed = dialogResultFor == dialogId;
			return changed ? (VideoClipRef)dialogResult : clip;
		}

		public static ImageListRef ImageList(string name, ImageListRef image, out bool changed)
		{
			int id = ++dialogId;

			string text = "None";

			//DrawName(name);

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
					dialogResultFor = id;
					dialogResult = o;
				});
			}

			changed = dialogResultFor == dialogId;
			return changed ? (ImageListRef)dialogResult : image;
		}

		public static ImageFrameRef ImageFrame(string name, ImageFrameRef frame, out bool changed)
		{
			int id = ++dialogId;
			string text = "None";

			//DrawName(name);

			if (!string.IsNullOrEmpty(frame.Key))
				text = frame.Key;

			if (frame.Frame == null)
			{
				if (ImGui.Button(text))
				{
					EditorModalSelect.SelectItemModal(typeof(ImageFrameRef), frame, (o) =>
					{
						dialogResultFor = id;
						dialogResult = o;
					});
				}
			}
			else
			{

				if (ImGui.IsItemHovered())
					AppGame.Gui.HoverObject = frame;

				if (ImGui.ImageButton(frame.Frame.Image.GuiImage, new Vector2(24, 24)))
				{
					EditorModalSelect.SelectItemModal(typeof(ImageFrameRef), frame, (o) =>
					{
						dialogResultFor = id;
						dialogResult = o;
					});
				}
			}

			changed = dialogResultFor == dialogId;
			return changed ? (ImageFrameRef)dialogResult : frame;
		}

		public static int FrameTime(string name, int number, out bool changed)
		{
			//DrawName(name);

			ImGui.SetNextItemWidth(100);
			changed = ImGui.InputInt(name, ref number, 0);
			ImGui.SameLine();
			ImGui.Text($"{(number / 15.0f).ToString("F1")} sec");
			return number;

		}
		public static int Supply(string name, int number, out bool changed)
		{
			//DrawName(name);

			ImGui.SetNextItemWidth(100);
			changed = ImGui.InputInt(name, ref number, 0);
			ImGui.SameLine();
			ImGui.Text($"{(number / 2.0f)}");
			return number;
		}
	}
}
