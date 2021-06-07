using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	/// <summary>
	/// Logical data about all sprite in a SpriteSheet(GRP) and how they should be displayed
	/// </summary>
	public class ImageAsset : Asset
	{
		[Index(0)]
		public string Name { get; set; }

		[Index(1), TypeConverter(typeof(ImageListRef.CsvConverter))]
		public ImageListRef SpriteSheet { get; set; }

		[Index(2)]
		public bool GraphicsTurns { get; set; }

		[Index(3)]
		public bool UnitColor { get; set; }

		public override string AssetName => Name;
		public override GuiTexture Preview
		{
			get
			{
				if (SpriteSheet.Image == null)
					return null;

				int f = GraphicsTurns ? 12 : 0;
				if (SpriteSheet.Image.Frames.Count <= f)
					return null;

				return SpriteSheet.Image.Frames[f].Image;
			}
		}

		public ImageAsset() : base() { }

		public ImageAsset(ImageList asset)
		{
			SpriteSheet = new ImageListRef(asset);
			Name = asset.Key;
		}
	}
}
