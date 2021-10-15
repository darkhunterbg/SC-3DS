using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{

	public class Palette
	{
		public List<Color> Colors { get; private set; } = new List<Color>();

		public string Name;

		public Dictionary<int, Color> Remapped = new Dictionary<int, Color>();
		public List<int> RemappedIndexes = new List<int>();
		public Palette(Color color)
		{
			for (int i = 0; i < 256; ++i)
				Colors.Add(color);
		}

		public void Remap(int c, Color newColor)
		{

			Remapped[c] = newColor;
			Colors[c] = Color.Black;
			RemappedIndexes.Add(c);
		}

		public Palette(byte[] data)
		{
			LoadFromData(data);
		}

		private void LoadFromData(byte[] data)
		{
			if (data.Length != 768 && data.Length != 1024)
				throw new Exception("Invalid palette file!");

			Colors.Clear();


			if (data.Length == 768) {
				for (int i = 0; i < data.Length / 3; ++i) {
					Color c = new Color(data[i * 3], data[i * 3 + 1], data[i * 3 + 2], (byte)255);
					Colors.Add(c);
				}
			} else {
				for (int i = 0; i < data.Length / 4; ++i) {
					Color c = new Color(data[i * 4], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]);
					Colors.Add(c);
				}
			}
		}

		public Palette(string path)
		{
			Name = Path.GetFileNameWithoutExtension(path);

			byte[] data = File.ReadAllBytes(path);
			LoadFromData(data);
		}

		private Palette() { }
		public Palette Clone(string name)
		{
			return new Palette() { Name = name, Colors = Colors.ToList() };
		}
	}
}
