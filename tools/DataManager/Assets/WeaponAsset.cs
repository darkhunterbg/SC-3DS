using CsvHelper.Configuration.Attributes;
using DataManager.Build;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	[BinaryData(DataItemType.Weapons)]
	public class WeaponAsset : Asset, ITreeViewItem
	{
		public override string AssetName => Path;
		public override GuiTexture Preview => Icon.Image?.Image;

		[Index(0)]
		public string Path { get; set; }

		[Section("Art")]

		[DefaultEditor]
		[Optional]
		[Binary(BinaryType.UInt, 2)]
		public IconRef Icon { get; set; } = IconRef.None;

		[Section("Text")]

		[DefaultEditor]
		[Optional]
		[Binary(BinaryType.String, 32)]
		public string Name { get; set; } = string.Empty;

		[Section("Sounds")]
		[DefaultEditor]
		[Binary(BinaryType.AssetRef, 2)]
		[Optional, TypeConverter(typeof(AssetConverter))]
		public SoundSetAsset SpawnSound { get; set; }
	}
}
