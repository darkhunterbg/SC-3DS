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

		private SpriteFrame _icon;

		[Index(0)]
		public int IconId { get; set; }

		[Ignore]
		[IconEditorAttribute()]
		public SpriteFrame Icon
		{
			get { return _icon; }
			set
			{
				_icon = value;
				IconId = _icon?.FrameIndex ?? 0;
			}
		}

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
		public override GuiTexture Preview => Icon?.Image;

		public UpgradeAsset() : base() { }
		public UpgradeAsset(SpriteFrame asset) : this()
		{
			Icon = asset;
		}

		public override void OnAfterDeserialize()
		{
			_icon = AppGame.AssetManager.Icons.Skip(IconId).FirstOrDefault();
		}
	}
}
