﻿using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class GameImageAsset
	{
		[Ignore]
		public SpriteSheetAsset SpriteSheet { get; set; }

		[Index(0),Name("SpriteSheet")]
		public string SpriteSheetName { get; set; }

		[Index(1)]
		public bool GraphicsTurns { get; set; }

		[Index(2)]
		public bool UnitColor { get; set; }

		public GameImageAsset() { }

		public GameImageAsset(SpriteSheetAsset asset)
		{
			SpriteSheet = asset;
			SpriteSheetName = asset.SheetName;
		}

	}
}
