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
		[Ignore]
		public SpriteSheet SpriteSheet { get; private set; }

		[Index(0),Name("SpriteSheet")]
		public string SpriteSheetName { get; set; }

		[Index(1)]
		public bool GraphicsTurns { get; set; }

		[Index(2)]
		public bool UnitColor { get; set; }

		public override string AssetName => SpriteSheetName;
		public override GuiTexture Preview
		{
			get
			{
				if (SpriteSheet == null)
					return null;

				int f = GraphicsTurns ? 12 : 0;
				if (SpriteSheet.Frames.Count <= f)
					return null;

				return SpriteSheet.Frames[f].Image;
			}
		}

		public ImageAsset() : base() { }

		public ImageAsset(SpriteSheet asset)
		{
			SpriteSheet = asset;
			SpriteSheetName = asset.SheetName;
		}

		public override void OnAfterDeserialize()
		{
			SpriteSheet = AppGame.AssetManager.SpriteSheets.FirstOrDefault(s => s.SheetName == SpriteSheetName);
		}
	}
}
