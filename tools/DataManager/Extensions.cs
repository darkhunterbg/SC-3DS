using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager
{
	public static class ColorExtensions
	{
		public static Vector4 ToVec4(this Microsoft.Xna.Framework.Color color)
		{
			var v = color.ToVector4();
			return new Vector4(v.X, v.Y, v.Z, v.W);
		}

		public static Vector4 ToVec4(this System.Drawing.Color color)
		{
			return new Vector4(color.R/255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f);
		}
	}

	public static class IEnumerableExtensions
	{
		public static int IndexOf<T>(this IEnumerable<T> col, T element) where T : class
		{
			int i = 0;
			IEnumerator<T> e = col.GetEnumerator();

			try
			{
				while (e.MoveNext())
				{
					if (e.Current != null && e.Current == element)
					{
						return i;
					}

					i++;
				}
			}
			finally
			{
				e.Dispose();
			}

			return -1;
		}
	}
}
