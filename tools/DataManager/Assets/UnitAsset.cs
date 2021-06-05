using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class UnitAsset : Asset
	{
		private FlingyAsset _graphics;

		public override string AssetName => Name;
		public override GuiTexture Preview => Graphics.Preview;

		[Index(0)]
		[DefaultEditor]
		public string Name { get; set; }

		[Index(1)]
		[DefaultEditor]
		public int HitPoints { get; set; }

		[Index(2)]
		[DefaultEditor]
		public int Armor { get; set; }

		[Index(3)]
		[DefaultEditor]
		public int MineralCost { get; set; }

		[Index(4)]
		[DefaultEditor]
		public int VespeneCost { get; set; }

		[Index(5)]
		[FrameTimeEditor]
		public int TimeCost { get; set; }

		[Index(6)]
		[SupplyEditor]
		public int SupplyTake { get; set; }

		[Index(7)]
		[SupplyEditor]
		public int SupplyGive { get; set; }

		[Index(8), TypeConverter(typeof(AssetConverter))]
		[DefaultEditor]
		public FlingyAsset Graphics { get; set; }


		public UnitAsset() : base() { }

	}
}
