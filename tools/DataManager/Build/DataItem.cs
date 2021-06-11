using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Build
{
	public enum DataItemType : byte
	{
		None = 0,
		Atlases = 1,
		Images = 2,
		Frames = 3,
		Sprites = 4,
		AnimClips = 5,
	}

	[AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct)]
	public class BinaryDataAttribute : Attribute
	{
		public readonly DataItemType Type;

		public BinaryDataAttribute(DataItemType type)
		{
			Type = type;
		}
	}

	[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
	struct DataItemHeader
	{
		public DataItemType Type;
		public uint Count;
		public uint Offset;


		public static int Size => Marshal.SizeOf<DataItemHeader>();

		public byte[] Serialize()
		{
			IntPtr bufferPtr = Marshal.AllocHGlobal(Size);

			Marshal.StructureToPtr(this, bufferPtr, false);
			byte[] bytes = new byte[Size];
			Marshal.Copy(bufferPtr, bytes, 0, Size);


			Marshal.FreeHGlobal(bufferPtr);

			return bytes;

		}
	}

	class DataItem
	{
		public IEnumerable<object> Data;
		public string Name;

		public Type DataItemType;
		public int ElementSize;
		public DataItemType Type;


		public MemoryStream MemorySerialize()
		{
			if (DataItemType == null)
				return new MemoryStream();

			var props = DataItemType.GetProperties(BindingFlags.Public| BindingFlags.Instance).Where(t => t.GetCustomAttribute<BinaryAttribute>() != null).ToList();

			var attr = props.Select(s => s.GetCustomAttribute<BinaryAttribute>()).ToList();

			MemoryStream stream = new MemoryStream(Data.Count() * attr.Sum(s => s.Size));

			foreach (var item in Data)
			{
				for (int i = 0; i < props.Count; ++i)
				{
					var p = props[i];
					var a = attr[i];
					var bin = a.Serialize(p.GetValue(item));
					stream.Write(bin);
				}
			}

			stream.Position = 0;

			return stream;
		}

		public void Init()
		{
			DataItemType = Data.FirstOrDefault()?.GetType();

			if (DataItemType != null)
			{
				var a = DataItemType.GetCustomAttribute<BinaryDataAttribute>();
				if (a != null)
				{
					Type = a.Type;
				}
			}
			else
			{
				return;
			}

			var props = DataItemType.GetProperties(BindingFlags.Public | BindingFlags.Instance).Where(t => t.GetCustomAttribute<BinaryAttribute>() != null).ToList();

			var attr = props.Select(s => s.GetCustomAttribute<BinaryAttribute>()).ToList();

			ElementSize = attr.Sum(s => s.Size);
		}

		public DataItemHeader GetHeader()
		{
			return new DataItemHeader()
			{
				Type = Type,
				Count = (uint)Data.Count(),
				Offset = 0
			};
		}
	}
}
