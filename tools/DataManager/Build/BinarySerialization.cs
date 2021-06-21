using DataManager.Assets;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Build
{
	public enum BinaryType
	{
		Int,
		UInt,
		String,
		Raw,
		AssetRef,
		Vector2,
		ImageRef,
		Array,
	}

	[AttributeUsage(AttributeTargets.Property, AllowMultiple = false, Inherited = true)]
	public class BinaryAttribute : Attribute
	{
		public readonly BinaryType Type;
		public readonly int Size;

		public BinaryAttribute(BinaryType type, int size)
		{
			Type = type;
			Size = size;
		}



		public byte[] Serialize(object obj)
		{
			return Serialize(obj, Type, Size);
		}
		public static byte[] Serialize(object obj, Type type)
		{
			if (type == typeof(string))
				return Serialize(obj, BinaryType.String, 32);
			if (type == typeof(bool))
				return Serialize(obj, BinaryType.UInt, 1);
			if (type.IsEnum)
				return Serialize(obj, type.GetEnumUnderlyingType());
			if (type == typeof(int))
				return Serialize(obj, BinaryType.Int, 4);
			if (type == typeof(short))
				return Serialize(obj, BinaryType.Int, 2);
			if (type == typeof(sbyte))
				return Serialize(obj, BinaryType.Int, 1);

			if (type == typeof(uint))
				return Serialize(obj, BinaryType.UInt, 4);
			if (type == typeof(ushort))
				return Serialize(obj, BinaryType.UInt, 2);
			if (type == typeof(byte))
				return Serialize(obj, BinaryType.UInt, 1);

			if (type.IsSubclassOf(typeof(Asset)))
				return Serialize(obj, BinaryType.AssetRef, 2);

			if (type == typeof(Vector2))
				return Serialize(obj, BinaryType.Vector2, 2);

			if (type == typeof(ImageListRef))
				return Serialize(obj, BinaryType.ImageRef, 2);

			if (type == typeof(Array))
				return Serialize(obj, BinaryType.Array, ((Array)obj).GetLength(0));

			return null;
		}
		public static byte[] Serialize(object obj, BinaryType type, int size)
		{
			try
			{
				if (type == BinaryType.Array)
				{
					Array array = (Array)obj;
					var arrayType = array.GetValue(0).GetType();
					var props = arrayType.GetProperties(BindingFlags.Public | BindingFlags.Instance).Where(t => t.GetCustomAttribute<BinaryAttribute>() != null).ToList();
					var attr = props.Select(s => s.GetCustomAttribute<BinaryAttribute>()).ToList();
					MemoryStream stream = new MemoryStream(size * attr.Sum(s => s.Size));

					for (int i = 0; i < size; ++i)
					{
						var o = array.GetValue(i);

						for (int j = 0; j < props.Count; ++j)
						{
							var p = props[j];
							var a = attr[j];
							var bin = a.Serialize(p.GetValue(o));
							stream.Write(bin);
						}

					}

					stream.Position = 0;


					return stream.ToArray();
				}

				if (type == BinaryType.Raw)
				{
					return (obj as IEnumerable<byte>).Take(size).ToArray();
				}
				if (type == BinaryType.AssetRef)
				{
					var asset = obj as Asset;
					if (asset == null) return IntSerialize("-1", size);
					int index = AppGame.AssetManager.GetAssetDatabase(obj.GetType()).Assets.IndexOf(asset);
					return IntSerialize(index.ToString(), size);
				}
				if (type == BinaryType.ImageRef)
				{
					var img = obj == null ? ImageListRef.None : (ImageListRef)obj;
					var i = img.Image;
					int index = BuildGenerator.Instance.GeneratedImages.IndexOf(i => i.List == img.Image);
					return IntSerialize(index.ToString(), size);
				}
				if (type == BinaryType.Vector2)
				{
					Vector2 v = (Vector2)obj;
					byte[] data = new byte[size * 2];
					var a = IntSerialize(((int)v.X).ToString(), size);
					var b = IntSerialize(((int)v.Y).ToString(), size);
					for (int i = 0; i < a.Length; ++i)
						data[i] = a[i];
					for (int i = 0; i < b.Length; ++i)
						data[a.Length + i] = b[i];

					return data;
				}

				string s = GetObjectString(obj);

				switch (type)
				{
					case BinaryType.Int:
						{
							return IntSerialize(s, size);
						}

					case BinaryType.UInt:
						{
							return UIntSerialize(s, size);
						}
					case BinaryType.String:
						{
							return StringSerialize(s, size);
						}

				}
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debugger.Break();
				throw;
			}

			return new byte[size];
		}

		private static string GetObjectString(object obj)
		{
			if (obj == null)
				return string.Empty;
			if (obj is string)
				return obj as string;

			if (obj is Enum)
			{
				return ((int)obj).ToString();
			}
			if (obj is bool)
			{
				return (((bool)obj) ? 1 : 0).ToString();
			}

			return obj.ToString();
		}
		private static byte[] IntSerialize(string text, int size)
		{
			int i = -1;
			if (!string.IsNullOrEmpty(text))
				i = int.Parse(text);

			switch (size)
			{
				case 1: return new byte[] { Convert.ToByte((sbyte)i) };
				case 2: return BitConverter.GetBytes((short)i);
				default: return BitConverter.GetBytes(i);
			}
		}
		private static byte[] UIntSerialize(string text, int size)
		{
			uint i = uint.Parse(text);

			switch (size)
			{
				case 1: return new byte[] { Convert.ToByte((byte)i) };
				case 2: return BitConverter.GetBytes((ushort)i);
				default: return BitConverter.GetBytes(i);
			}
		}

		private static byte[] StringSerialize(string text, int size)
		{
			byte[] data = new byte[size];

			var s = Encoding.ASCII.GetBytes(text);
			for (int i = 0; i < text.Length; ++i)
			{
				data[i] = s[i];
			}

			return data;
		}
	}
}
