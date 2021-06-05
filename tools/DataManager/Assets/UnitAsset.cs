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
		public int HitPoints { get; set; }

		[Index(2)]
		public int Armor { get; set; }

		[Index(3)]
		public int MineralCost { get; set; }

		[Index(4)]
		public int VespeneCost { get; set; }

		[Index(5)]
		public int TimeCost { get; set; }

		[Index(6)]
		public int SupplyTake { get; set; }

		[Index(7)]
		public int SupplyGive { get; set; }

		[Index(8)]
		public string GraphicsName { get; set; }

		[Ignore]
		[DefaultEditor]
		public FlingyAsset Graphics
		{
			get { return _graphics; }
			set
			{
				_graphics = value;
				GraphicsName = _graphics?.AssetName;
			}
		}

		public UnitAsset() : base() { }

	}
}
