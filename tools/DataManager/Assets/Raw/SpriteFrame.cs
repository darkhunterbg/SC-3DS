using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class SpriteFrame 
	{
		[Ignore]
		public SpriteSheetAsset SpriteSheet { get; private set; }

		[Index(0), Name("SpriteSheet")]
		public string SpriteSheetName { get; set; }

		[Index(1), Name("Frame")]
		public int FrameIndex { get; set; }

		[Index(2)]
		public int X { get; set; }
		[Index(3)]
		public int Y { get; set; }

		public Vector2 GetOffset(bool hFlip = false)
		{
			Vector2 offset = new Vector2(X, Y) - SpriteSheet.ImageAsset.FrameSize / 2;
			if (hFlip)
				offset.X = SpriteSheet.ImageAsset.FrameSize.X / 2 - X - (int)GetFrameData().rect.Width;

			return offset;
		}

		public SpriteFrame(SpriteSheetAsset asset, int frame)
		{
			SpriteSheet = asset;
			SpriteSheetName = asset.SheetName;
			FrameIndex = frame;

			Vector2 offset = new Vector2(GetFrameData().rect.X, GetFrameData().rect.Y);

			X = (int)offset.X;
			Y = (int)offset.Y;
		}

		public ImageList.FrameData GetFrameData()
		{
			return SpriteSheet.ImageAsset.Frames[FrameIndex];
		}

		public GuiTexture Image => AppGame.AssetManager.GetSheetImage(SpriteSheet.SheetName, FrameIndex);
	}
}
