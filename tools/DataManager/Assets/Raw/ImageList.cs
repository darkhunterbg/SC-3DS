
using CsvHelper;
using CsvHelper.Configuration;
using CsvHelper.TypeConversion;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class ImageList
	{
		//public string InfoFilePath { get; private set; }
		private string RelativePath;
		public string Key => RelativePath;
		public string AssetDir { get; private set; }

		public Vector2 FrameSize { get; private set; }

		public List<ImageFrame> Frames { get; private set; } = new List<ImageFrame>();

		public int TakenSpace => Frames.Sum(f => f.TakenSpace);

		public int UnitColorOffset { get; private set; }

		private ImageList() { }

		public static ImageList FromPng(string pngFile)
		{
			var i = new ImageList();
			i.AssetDir = Path.GetDirectoryName(pngFile);
			i.RelativePath = pngFile.Substring(AssetManager.AssetsDir.Length);
			i.RelativePath = i.RelativePath.Substring(0, i.RelativePath.Length - 4);


			using (var image = SixLabors.ImageSharp.Image.Load(pngFile))
			{
				i.FrameSize = new Vector2(image.Width, image.Height);

				i.Frames.Add(new ImageFrame(i, 0)
				{
					fileName = Path.GetFileName(pngFile),
					rect = new Microsoft.Xna.Framework.Rectangle(0, 0, image.Width, image.Height)
				});
			}

			return i;
		}

		public static ImageList FromInfoFile(string infoFile)
		{
			var i = new ImageList();

			i.AssetDir = Path.GetDirectoryName(infoFile);
			i.RelativePath = i.AssetDir.Substring(AssetManager.ExtractedAssetsDir.Length);
			//InfoFilePath = infoFile.Substring(AssetManager.AssetsDir.Length);

			string[] info = File.ReadAllLines(infoFile);
			var spl = info[0].Split(' ');
			i.FrameSize = new Vector2(int.Parse(spl[0]), int.Parse(spl[1]));

			foreach (var e in info.Skip(1))
			{
				spl = e.Split(' ');

				Microsoft.Xna.Framework.Rectangle rect = new Microsoft.Xna.Framework.Rectangle();
				rect.X = int.Parse(spl[1]);
				rect.Y = int.Parse(spl[2]);
				rect.Width = int.Parse(spl[3]);
				rect.Height = int.Parse(spl[4]);

				string name = spl[0];

				i.Frames.Add(new ImageFrame(i, i.Frames.Count)
				{
					fileName = name,
					rect = rect,
				});
			}

			i.UnitColorOffset = i.Frames.IndexOf(f => f.fileName.StartsWith("cm_"));
			if (i.UnitColorOffset < 0)
				i.UnitColorOffset = 0;

			return i;
		}

		public int GetUnitColorFrameIndex(int frame)
		{
			return frame + UnitColorOffset;
		}

		public string GetFrameFilePath(int i)
		{
			return $"{AssetDir}\\{Frames[i].fileName}";
		}

	}

	public struct ImageListRef
	{
		public readonly string Key;

		public readonly ImageList Image;

		public static readonly ImageListRef None = new ImageListRef(string.Empty,null);

		public ImageListRef( ImageList image)
		{
			Key = image.Key;
			Image = image;
		}

		public ImageListRef(string key, ImageList image)
		{
			Key = key;
			Image = image;
		}

		public class CsvConverter : DefaultTypeConverter
		{
			public override object ConvertFromString(string text, IReaderRow row, MemberMapData memberMapData)
			{
				if (string.IsNullOrEmpty(text))
					return ImageListRef.None;

				var i = AppGame.AssetManager.ImageLists.FirstOrDefault(f => f.Key == text);

				return new ImageListRef(text, i);
			}
			public override string ConvertToString(object value, IWriterRow row, MemberMapData memberMapData)
			{
				if (value == null)
					return string.Empty;

				return ((ImageListRef)value).Key ?? string.Empty;
			}
		}
	}
}
