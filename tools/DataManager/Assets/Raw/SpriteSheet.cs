using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{

	/// <summary>
	/// Hold info about what frames are in single Sprite Sheet (.grp)
	/// </summary>
	public class SpriteSheet 
	{
		[Index(0)]
		public string SheetName { get; set; }
		[Index(1)]
		public string Atlas { get; set; }
		[Index(2)]
		public int SubAtlasId { get; set; }
		[Index(3)]
		public int SubAtlasOffset { get; set; }
		[Index(4), Name("Frames")]
		public int TotalFrames { get; set; }
		[Index(5)]
		public int UnitColorOffset { get; set; }
		[Index(6)]
		public int Width { get; set; }
		[Index(7)]
		public int Height { get; set; }

		[Ignore]
		public SpriteSubAtlas SubAtlas { get; private set; }

		[Ignore]
		public ImageList ImageAsset { get; private set; }

		[Ignore]
		public Vector2 FrameSize => ImageAsset.FrameSize;

		[Ignore]
		public List<SpriteFrame> Frames { get; private set; } = new List<SpriteFrame>();


		public SpriteSheet() { }
		public SpriteSheet(SpriteSubAtlas subAtlas, int imageListIndex)
		{
			var imageList = subAtlas.ImageLists[imageListIndex];

			SheetName = imageList.RelativePath;
			Atlas = subAtlas.AtlasName;
			SubAtlasId = subAtlas.AtlasIndex;
			SubAtlasOffset = subAtlas.ImageLists.Take(imageListIndex).Sum(s => s.Frames.Count);
			TotalFrames = subAtlas.ImageLists[imageListIndex].Frames.Count();
			Width = (int)imageList.FrameSize.X;
			Height = (int)imageList.FrameSize.Y;

			ImageAsset = subAtlas.ImageLists[imageListIndex];

			UnitColorOffset = ImageAsset.Frames.IndexOf(f => f.fileName.StartsWith("cm_"));
			if (UnitColorOffset < 0)
				UnitColorOffset = 0;

			SubAtlas = subAtlas;

			for (int i = 0; i < TotalFrames; ++i)
			{
				Frames.Add(new SpriteFrame(this, i));
			}
		}

		public void OnAfterDeserialize()
		{
			SubAtlas = AppGame.AssetManager.SpriteAtlases.FirstOrDefault(a => a.Name == Atlas)
				.SubAtlases.FirstOrDefault(s => s.AtlasIndex == SubAtlasId);
			ImageAsset = SubAtlas.GetImageListAtOffset(SubAtlasOffset);
			for (int i = 0; i < TotalFrames; ++i)
			{
				Frames.Add(new SpriteFrame(this, i));
			}

		}


		public int GetUnitColorFrameIndex(int frame)
		{
			return frame + UnitColorOffset;
		}
	}
}
