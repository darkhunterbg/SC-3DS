using CsvHelper.Configuration.Attributes;
using DataManager.Build;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public enum UnitAIType
	{
		None = 0,
		Aggressive = 1,
	}

	public class UnitAttack
	{
		[DefaultEditor]
		[Binary(BinaryType.AssetRef, 2)]
		[Optional, TypeConverter(typeof(Asset.AssetConverter))]
		public WeaponAsset Weapon { get; set; }

		[DefaultEditor]
		[Binary(BinaryType.Vector2, 2)]
		[Optional]
		public Vector2 Range { get; set; }


		[FrameTimeEditor]
		[Binary(BinaryType.UInt, 1)]
		[Optional]
		public int Cooldown { get; set; } = 1;

		[Binary(BinaryType.UInt, 1)]
		[Optional]
		public int _Padding { get; set; }

		[DefaultEditor]
		[Binary(BinaryType.UInt, 2)]
		[Optional]
		public int Damage { get; set; }
	}

	[BinaryData(DataItemType.Units)]
	public class UnitAsset : Asset, ITreeViewItem
	{
		public override string AssetName => Path;
		public override GuiTexture Preview => Icon.Image?.Image;


		[Index(0)]
		[Binary(BinaryType.String, 32)]
		public string Path { get; set; }

		[Optional]
		[Binary(BinaryType.UInt, 4)]
		public AssetId UnitId { get; set; }

		#region Text
		[Section("Text")]
		[DefaultEditor]
		[Binary(BinaryType.String, 32)]
		[Optional]
		public string Name { get; set; } = string.Empty;

		[DefaultEditor]
		[Binary(BinaryType.String, 32)]
		[Optional]
		public string Subtitle { get; set; } = string.Empty;
		#endregion

		#region Art
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
		[Binary(BinaryType.Vector2, 2)]
		[Optional]
		public Vector2 ShadowOffset { get; set; }


		[Optional]
		[DefaultEditor]
		[Binary(BinaryType.AssetRef, 2)]
		[TypeConverter(typeof(AssetConverter))]
		public UnitPortraitAsset Portrait { get; set; }

		[Optional]
		[DefaultEditor]
		[Binary(BinaryType.AssetRef, 2)]
		[TypeConverter(typeof(AssetConverter))]
		public UnitWireframeAsset Wireframe { get; set; }
		#endregion

		#region Combat
		[Section("Combat")]
		[DefaultEditor]
		[Binary(BinaryType.UInt, 2)]
		[Optional]
		public int Armor { get; set; } = 0;

		[ArrayEditor(2, Name = "Attack")]
		[Binary(BinaryType.Array, 2)]
		public UnitAttack[] Attacks { get; set; } = new UnitAttack[2];

		[Optional, HeaderPrefix("Attack1.")]
		public UnitAttack Attack1 { get { return Attacks[0]; } set { Attacks[0] = value; } }
		[Optional, HeaderPrefix("Attack2.")]
		public UnitAttack Attack2 { get { return Attacks[1]; } set { Attacks[1] = value; } }
		#endregion

		#region Stats
		[Section("Stats")]
		[DefaultEditor]
		[Binary(BinaryType.UInt, 2)]
		[Optional]
		public int Health { get; set; } = 100;

		[DefaultEditor]
		[Binary(BinaryType.UInt, 2)]
		[Optional]
		public int Shield { get; set; } = 0;

		[DefaultEditor]
		[Binary(BinaryType.UInt, 2)]
		[Optional]
		public int Vision { get; set; } = 7;

		[SupplyEditor]
		[Binary(BinaryType.UInt, 2)]
		[Optional]
		public int UseSupply { get; set; } = 0;

		[SupplyEditor]
		[Binary(BinaryType.UInt, 2)]
		[Optional]
		public int ProvideSupply { get; set; } = 0;
		#endregion

		#region Movement
		[Section("Movement")]
		[DefaultEditor]
		[Binary(BinaryType.UInt, 2)]
		[Optional]
		public int MaxVelocity { get; set; } = 0;

		[DefaultEditor]
		[Binary(BinaryType.UInt, 2)]
		[Optional]
		public int RotationSpeed { get; set; } = 0;
		#endregion

		#region Tech Tree
		[Section("Tech Tree")]
		[DefaultEditor]
		[Binary(BinaryType.AssetRef, 2)]
		[Optional, TypeConverter(typeof(AssetConverter))]
		public UpgradeAsset ArmorType { get; set; }

		[DefaultEditor]
		[Binary(BinaryType.AssetRef, 2)]
		[Optional, TypeConverter(typeof(AssetConverter))]
		public UpgradeAsset ShieldType { get; set; }
		#endregion

		#region Sounds
		[Section("Sounds")]
		[DefaultEditor]
		[Binary(BinaryType.AssetRef, 2)]
		[Optional, TypeConverter(typeof(AssetConverter))]
		public SoundSetAsset ReadySound { get; set; }

		[DefaultEditor]
		[Binary(BinaryType.AssetRef, 2)]
		[Optional, TypeConverter(typeof(AssetConverter))]
		public SoundSetAsset YesSound { get; set; }

		[DefaultEditor]
		[Binary(BinaryType.AssetRef, 2)]
		[Optional, TypeConverter(typeof(AssetConverter))]
		public SoundSetAsset WhatSound { get; set; }

		[DefaultEditor]
		[Binary(BinaryType.AssetRef, 2)]
		[Optional, TypeConverter(typeof(AssetConverter))]
		public SoundSetAsset AnnoyedSound { get; set; }
		#endregion

		#region AI
		[Section("Unit AI")]
		[DefaultEditor]
		[Binary(BinaryType.UInt, 1)]
		[Optional]
		public UnitAIType AIType { get; set; }
		#endregion

		[Binary(BinaryType.UInt, 1)]
		[Ignore]
		public int _padding { get; set; }

		public UnitAsset() : base()
		{
			UnitId = AssetId.New();

		}

		public override Asset Clone()
		{
			UnitAsset clone = (UnitAsset)base.Clone();
			clone.UnitId = AssetId.New();
			clone.Attacks = clone.Attacks.Select(s => Util.ShallowCopyProperties(s)).ToArray();
			return clone;
		}
	}


}
