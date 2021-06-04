using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.IO;
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
			return new Vector4(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f);
		}
	}

	public static class Vector2Exptensions
	{
		public static Vector2 ToVec4(this Microsoft.Xna.Framework.Vector2 v)
		{
			return new Vector2(v.X, v.Y);
		}

		public static Microsoft.Xna.Framework.Vector2 ToVector2(this Vector2 v)
		{
			return new Microsoft.Xna.Framework.Vector2(v.X, v.Y);
		}
	}

	public static class Texture2DExtensions
	{
		public static void SaveAsPng(this Texture2D texture, string fileName)
		{
			using (FileStream s = new FileStream(fileName, FileMode.OpenOrCreate))
			{
				texture.SaveAsPng(s, texture.Width, texture.Height);
			}
		}
	}

	public static class IListExtensions
	{
		public static void AddNotContainet<T>(this IList<T> list, T item) where T: class
		{
			if (!list.Contains(item))
				list.Add(item);
		}
	}

	public static class IEnumerableExtensions
	{
		public static int IndexOf<T>(this IEnumerable<T> e, Func<T,bool>  f) where T : class
		{
			int i = 0;
			foreach(var item in e)
			{
				if (f(item))
					return i;

				++i;
			}
			return -1;
		}

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
