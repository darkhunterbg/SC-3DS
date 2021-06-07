using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class UpgradeAsset : Asset
	{

		[Index(0), TypeConverter(typeof(IconRef.IconConverter)), Name("IconId")]
		[DefaultEditor()]
		public IconRef Icon { get; set; }

		[DefaultEditor()]
		[Index(1)]
		public string Name { get; set; }

		[Index(2)]
		[DefaultEditor()]
		public int BaseMineralCost { get; set; }

		[Index(3)]
		[DefaultEditor()]
		public int BaseVespeneCost { get; set; }

		[Index(4)]
		[FrameTimeEditor()]
		public int TimeCost { get; set; }

		[Index(5)]
		[DefaultEditor()]
		public int FactorMineralCost { get; set; }

		[Index(6)]
		[DefaultEditor()]
		public int FactorVespeneCost { get; set; }

		[Index(7)]
		[FrameTimeEditor()]
		public int FactorTimeCost { get; set; }

		[Index(8)]
		[DefaultEditor()]
		public int MaxRepeats { get; set; }

		[Index(9)]
		[DefaultEditor()]
		public RaceEnum Race { get; set; }

		public override string AssetName => Name;
		public override GuiTexture Preview => Icon.Image?.Image;

		public UpgradeAsset() : base() { }
		public UpgradeAsset(IconRef icon) : this()
		{
			Icon = icon;
		}
	}
}
