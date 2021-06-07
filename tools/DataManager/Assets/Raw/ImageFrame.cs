﻿
using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class ImageFrameAtlasData
	{
		[Index(0), Name("Image")]
		public string ImageListName { get; set; }

		[Index(1), Name("Frame")]
		public int FrameIndex { get;  set; }

		[Index(2), Name("X")]
		public int XOffset { get; set; }

		[Index(3), Name("Y")]
		public int YOffset { get; set; }

		[Index(2), Name("Width")]
		public int Width { get; set; }

		[Index(3), Name("Height")]
		public int Height { get; set; }

		[Index(4), Name("Atlas")]
		public string AtlasName { get; set; }

		[Index(5), Name("AtlasX")]
		public int AtlasX { get; set; }

		[Index(6), Name("AtlasY")]
		public int AtlasY { get; set; }

		public ImageFrameAtlasData(ImageFrame frame, Vector2 atlasPos)
		{
			ImageListName = frame.ImageListName;
			FrameIndex = frame.FrameIndex;
			XOffset = XOffset;
			YOffset = YOffset;
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

		public int YOffset => (int)Position.X;

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
}