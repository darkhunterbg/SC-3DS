using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class SpriteSheetAsset
	{
		[Index(0)]
		public string SheetName { get; set; }
		[Index(1)]
		public string Atlas { get; set; }
		[Index(2)]
		public int SubAtlasId { get; set; }
		[Index(3)]
		public int SubAtlasOffset { get; set; }
		[Index(4)]
		public int Width { get; set; }
		[Index(5)]
		public int Height { get; set; }

		public SpriteSheetAsset() { }
		public SpriteSheetAsset(SpriteSubAtlas subAtlas,int imageListIndex)
		{
			var imageList = subAtlas.ImageLists[imageListIndex];

			SheetName = imageList.RelativePath;
			Atlas = subAtlas.AtlasName;
			SubAtlasId = subAtlas.AtlasIndex;
			SubAtlasOffset = subAtlas.ImageLists.Take(imageListIndex).Sum(s => s.Frames.Count);
			Width = (int)imageList.FrameSize.X;
			Height = (int)imageList.FrameSize.Y;
		}
	}
}
