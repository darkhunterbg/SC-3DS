using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Build
{
	public enum BinaryType
	{
		Int,
		UInt,
		String,
		Bool,
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
			try
			{
				string s = GetObjectString(obj);

				switch (Type)
				{
					case BinaryType.Int:
						{
							return IntSerialize(s);
						}

					case BinaryType.UInt:
						{
							return UIntSerialize(s);
						}
					case BinaryType.String:
						{
							return StringSerialize(s);
						}
				}
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debugger.Break();
				throw;
			}

			return new byte[Size];
		}

		private string GetObjectString(object obj)
		{
			if (obj == null)
				return string.Empty;
			if (obj is string)
				return obj as string;

			if (obj is Enum)
			{
				return ((int)obj).ToString();
			}
			if(obj is bool)
			{
				return (((bool)obj) ? 1 : 0).ToString();
			}

			return obj.ToString();
		}

		private byte[] IntSerialize(string text)
		{
			int i = int.Parse(text);

			switch (Size)
			{
				case 1: return new byte[] { Convert.ToByte((sbyte)i) };
				case 2: return BitConverter.GetBytes((short)i);
				default: return BitConverter.GetBytes(i);
			}
		}
		private byte[] UIntSerialize(string text)
		{
			uint i = uint.Parse(text);

			switch (Size)
			{
				case 1: return new byte[] { Convert.ToByte((byte)i) };
				case 2: return BitConverter.GetBytes((ushort)i);
				default: return BitConverter.GetBytes(i);
			}
		}

		private byte[] StringSerialize(string text)
		{
			byte[] data = new byte[Size];

			var s = Encoding.ASCII.GetBytes(text);
			for (int i = 0; i < text.Length; ++i)
			{
				data[i] = s[i];
			}

			return data;
		}
	}
}
