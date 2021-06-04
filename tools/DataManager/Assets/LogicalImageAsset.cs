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
	public class LogicalImageAsset
	{
		[Ignore]
		public SpriteSheetAsset SpriteSheet { get; private set; }


		[Index(0),Name("SpriteSheet")]
		public string SpriteSheetName { get; set; }

		[Index(1)]
		public bool GraphicsTurns { get; set; }

		[Index(2)]
		public bool UnitColor { get; set; }

		public LogicalImageAsset() { }

		public LogicalImageAsset(SpriteSheetAsset asset)
		{
			SpriteSheet = asset;
			SpriteSheetName = asset.SheetName;
		}

		public void OnAfterDeserialized(SpriteSheetAsset asset )
		{
			SpriteSheet = asset;
		}

	}
}
