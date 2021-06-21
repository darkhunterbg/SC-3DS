
using CsvHelper;
using CsvHelper.Configuration;
using CsvHelper.Configuration.Attributes;
using CsvHelper.TypeConversion;
using DataManager.Build;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	[BinaryData(DataItemType.Frames)]
	public class ImageFrameAtlasData
	{

		[Index(2), Name("X")]
		[Binary(BinaryType.Int, 2)]
		public int XOffset { get; set; }

		[Index(3), Name("Y")]
		[Binary(BinaryType.Int, 2)]
		public int YOffset { get; set; }

		[Index(2), Name("Width")]
		[Binary(BinaryType.UInt, 2)]
		public int Width { get; set; }

		[Index(3), Name("Height")]
		[Binary(BinaryType.UInt, 2)]
		public int Height { get; set; }

		[Index(5), Name("AtlasX")]
		[Binary(BinaryType.UInt, 2)]
		public int AtlasX { get; set; }

		[Index(6), Name("AtlasY")]
		[Binary(BinaryType.UInt, 2)]
		public int AtlasY { get; set; }

		[Index(4), Name("Atlas")]
		[Binary(BinaryType.UInt, 1)]
		public int AtlasId { get; set; }

		[Ignore]
		public ImageFrame Frame { get; private set; }

		public ImageFrameAtlasData(ImageFrame frame, Vector2 atlasPos)
		{
			Frame = frame;
			XOffset = frame.XOffset;
			YOffset = frame.YOffset;
			Width = (int)frame.Size.X;
			Height = (int)frame.Size.Y;
			AtlasX = (int)atlasPos.X;
			AtlasY = (int)atlasPos.Y;
		}
	}

	public class ImageFrame
	{
		public string fileName;

		public Microsoft.Xna.Framework.Rectangle rect;


		public Vector2 Position => rect.Location.ToVector2().ToVec2();

		public Vector2 Size => rect.Size.ToVector2().ToVec2();

		public int TakenSpace => (rect.Width + 1) * (rect.Height + 1);

		public ImageList ImageList { get; private set; }

		public GuiTexture Image => AppGame.AssetManager.GetImageFrame(ImageList.Key, FrameIndex);

		public string ImageListName => ImageList.Key;

		public int FrameIndex { get; private set; }

		public int XOffset => (int)Position.X;

		public int YOffset => (int)Position.Y;

		public Vector2 GetOffset(bool hFlip = false)
		{
			Vector2 offset = Position - ImageList.FrameSize / 2;
			if (hFlip)
				offset.X = ImageList.FrameSize.X / 2 - Position.X - (int)Size.X;

			return offset;
		}

		public ImageFrame(ImageList list, int i)
		{
			ImageList = list;
			FrameIndex = i;
		}
	}



	public struct ImageFrameRef
	{
		public readonly string Key;

		public readonly ImageFrame Frame;

		public readonly string ImageListName;

		public static readonly ImageFrameRef None = new ImageFrameRef(string.Empty, null);

		public ImageFrameRef(ImageList imageList)
		{
			ImageListName = imageList?.Key ?? string.Empty;
			Frame = null;
			Key = string.Empty;
		}

		public ImageFrameRef(ImageFrame frame)
		{
			ImageListName = frame?.ImageListName ?? string.Empty;
			Frame = frame;
			Key = $"{ImageListName}:{frame.FrameIndex}";
		}

		public ImageFrameRef(string key, ImageFrame frame)
		{
			ImageListName = frame?.ImageListName ?? string.Empty;
			Key = key;
			Frame = frame;
		}
		public class CsvConverter : DefaultTypeConverter
		{
			public override object ConvertFromString(string text, IReaderRow row, MemberMapData memberMapData)
			{
				if (string.IsNullOrEmpty(text))
					return ImageFrameRef.None;

				var s = text.Split(":");

				string key = s[0];
				int index = 0;

				if (s.Length > 1) int.TryParse(s[1], out index);

				var asset = AppGame.AssetManager.ImageLists.FirstOrDefault(f => f.Key == key);
				ImageFrame frame = null;
				if (asset != null)
				{
					frame = asset.Frames.Skip(index).FirstOrDefault();
				}
				return new ImageFrameRef(text, frame);
			}
			public override string ConvertToString(object value, IWriterRow row, MemberMapData memberMapData)
			{
				if (value == null)
					return string.Empty;

				var frame = (ImageFrameRef)value;
				return frame.Key ?? string.Empty;
			}
		}
	}
}
