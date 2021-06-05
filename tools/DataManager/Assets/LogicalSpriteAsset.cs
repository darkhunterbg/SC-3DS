﻿using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class LogicalSpriteAsset : Asset
	{
		[Index(0)]
		[DefaultEditor()]
		public string Name { get; set; } = string.Empty;

		private LogicalImageAsset _img;
		[Ignore]
		[DefaultEditor]
		public LogicalImageAsset Image
		{
			get
			{
				return _img;
			}
			set
			{
				_img = value;
				ImageName = _img?.SpriteSheetName ?? string.Empty;
			}
		}

		[Index(1), Name("Image")]
		public string ImageName { get; set; } = string.Empty;

		[Index(2)]
		[DefaultEditor]
		public int BarSize { get; set; }

		[Index(3)]
		[Name("BarY")]
		[DefaultEditor]
		public int BarOffset { get; set; }

		[Index(4)]
		[Name("SelType")]
		[CustomEnumEditor(CustomEnumType.SelectionTypes)]
		public int SelectionType { get; set; }

		[Index(5)]
		[Name("SelY")]
		[DefaultEditor]
		public int SelectionOffset { get; set; }

		public override string AssetName => Name;
		public override GuiTexture Preview => Image?.Preview;

		public LogicalSpriteAsset() : base() { }


		public LogicalSpriteAsset(LogicalImageAsset asset) : this()
		{
			Image = asset;
		}
		public override void  OnAfterDeserialize()
		{
			_img = AppGame.AssetManager.Images.FirstOrDefault(s => s.SpriteSheetName == ImageName);
		}
	}
}
