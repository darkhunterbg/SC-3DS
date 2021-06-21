﻿using CsvHelper.Configuration.Attributes;
using DataManager.Build;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	[BinaryData(DataItemType.Units)]
	public class UnitAsset : Asset , ITreeViewItem
	{
		public override string AssetName => Path;
		public override GuiTexture Preview => Icon.Image?.Image;


		[Index(0)]
		[Binary(BinaryType.String, 32)]
		public string Path { get; set; }

		[Optional]
		[Binary(BinaryType.UInt, 4)]
		public AssetId UnitId { get; set; }

		[Section("Text")]
		[DefaultEditor]
		[Binary(BinaryType.String, 32)]
		[Optional]
		public string Name { get; set; } = string.Empty;

		[DefaultEditor]
		[Binary(BinaryType.String, 32)]
		[Optional]
		public string Subtitle { get; set; } = string.Empty;

		[Section("Art")]
		[DefaultEditor]
		[Optional]
		[Binary(BinaryType.UInt, 2)]
		public IconRef Icon { get; set; } = IconRef.None;

		[Optional]
		[DefaultEditor]
		[Binary(BinaryType.AssetRef, 2)]
		[TypeConverter(typeof(AssetConverter))]
		public SpriteAsset Sprite { get; set; }

		[DefaultEditor]
		[Binary(BinaryType.UInt, 2)]
		[Optional]
		public int BarSize { get; set; } = 10;

		[DefaultEditor]
		[Binary(BinaryType.Int, 2)]
		[Optional]
		public int BarOffset { get; set; } = 0;

		[CustomEnumEditor(CustomEnumType.SelectionTypes)]
		[Binary(BinaryType.UInt, 2)]
		[Optional]
		public int SelectonSize { get; set; } = 0;

		[DefaultEditor]
		[Binary(BinaryType.Int, 2)]
		[Optional]
		public int SelectionOffset { get; set; } = 0;

		[DefaultEditor]
		[Binary(BinaryType.ImageRef, 2)]
		[Optional]
		public ImageListRef Shadow { get; set; } = ImageListRef.None;

		[DefaultEditor]
		[Binary(BinaryType.Vector2, 4)]
		[Optional]
		public Vector2 ShadowOffset { get; set; }

		[DefaultEditor]
		[Optional]
		[Binary(BinaryType.ImageRef, 2)]
		public ImageListRef Portrait { get; set; } = ImageListRef.None;

		//[DefaultEditor]
		//[Optional]
		//[Binary(BinaryType.UInt, 2)]
		//public ImageListRef Wireframe { get; set; } = ImageListRef.None;

		[Section("Combat")]
		[DefaultEditor]
		[Binary(BinaryType.UInt, 2)]
		[Optional]
		public int Health { get; set; } = 100;

		public UnitAsset() : base()
		{
			UnitId = AssetId.New();
		}

		public override Asset Clone()
		{
			UnitAsset clone = (UnitAsset) base.Clone();
			clone.UnitId = AssetId.New();
			return clone;
		}
	}

   
}
