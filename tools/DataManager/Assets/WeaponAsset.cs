using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class WeaponAsset : Asset
	{
		public override string AssetName => Name;
		public override GuiTexture Preview => Icon.Image?.Image;

		[Index(0), TypeConverter(typeof(IconRef.IconConverter)),Name("IconId")]
		[DefaultEditor()]
		public IconRef Icon { get; set; }
		[Index(1)]
		[DefaultEditor]
		public string Name { get; set; }
		[Index(2)]
		[DefaultEditor]
		public int Damage { get; set; }

		[Index(3), Name("Bonus")]
		[DefaultEditor]
		public int UpgradeBonus { get; set; }

		[Index(4)]
		[DefaultEditor]
		public int Attacks { get; set; }

		[Index(5)]
		[FrameTimeEditor]
		public int Cooldown { get; set; }

		

		[Index(6), TypeConverter(typeof(AssetConverter))]
		[DefaultEditor()]
		public UpgradeAsset Upgrade { get; set; }

		public WeaponAsset() : base() { }
		public WeaponAsset(IconRef icon, UpgradeAsset upgrade) : this()
		{
			Icon = icon;
			Upgrade = upgrade;
		}
	}
}
