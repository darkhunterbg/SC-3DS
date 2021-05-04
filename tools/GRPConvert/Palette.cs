using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GRPConvert
{

	public class Palette
	{
		public List<Color> Colors { get; private set; } = new List<Color>();

		public string Name;

		public Palette(Color color)
		{
			for (int i = 0; i < 256; ++i)
				Colors.Add(color);
		}

		public Palette(string path)
		{
			Name = Path.GetFileNameWithoutExtension(path);

			byte[] data = File.ReadAllBytes(path);
			if (data.Length != 768 && data.Length != 1024)
				throw new Exception("Invalid palette file!");

			Colors.Clear();

			for (int i = 0; i < data.Length / 3; ++i) {
				Color c = Color.FromArgb(255, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
				Colors.Add(c);
			}
		}
	}
}
