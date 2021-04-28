using System;
using System.Drawing.Imaging;
using System.IO;

namespace GRPConvert
{
	class Program
	{
		static void Main(string[] args)
		{
			string palettePath = Path.GetFullPath("..\\..\\palettes\\Icons.pal");

			Palette pal = new Palette();
			pal.LoadPalette(palettePath);

			string grpPath = Path.GetFullPath("..\\..\\palettes\\arrow.grp");
			GRPImage img = new GRPImage(grpPath);

			int i = 0;
			foreach(var f in img.Frames) {
				
				string path = Path.GetFullPath("..\\..\\palettes\\");
				path += "arrow_" + i++ + ".png";
				f.ToBitmap(pal).Save(path, ImageFormat.Png);
			}
		}
	}
}
