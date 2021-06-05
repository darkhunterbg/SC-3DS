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
		public override string AssetName => throw new NotImplementedException();
		public override GuiTexture Preview => Icon?.Image;

		private SpriteFrame _icon;
		private UpgradeAsset _upgrade;

		[Index(0)]
		public int IconId { get; set; }

		[Ignore]
		[IconEditor()]
		public SpriteFrame Icon
		{
			get { return _icon; }
			set
			{
				_icon = value;
				IconId = _icon?.FrameIndex ?? 0;
			}
		}

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

		[Index(6) , Name("Upgrade")]
		public string UpgradeName { get; set; }


		[Ignore]
		[DefaultEditor()]
		public UpgradeAsset Upgrade
		{
			get { return _upgrade; }
			set
			{
				_upgrade = value;
				UpgradeName = _upgrade?.Name ?? null;
			}
		}

		public WeaponAsset() : base() { }
		public WeaponAsset(SpriteFrame asset, UpgradeAsset upgrade) : this()
		{
			Icon = asset;
			Upgrade = upgrade;
		}

		public override void OnAfterDeserialize()
		{
			_icon = AppGame.AssetManager.Icons.Skip(IconId).FirstOrDefault();
			_upgrade = AppGame.AssetManager.GetAssets<UpgradeAsset>().FirstOrDefault(a=>a.Name == UpgradeName);
		}
	}
}
