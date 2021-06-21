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

		[DefaultEditor]
		[Optional]
		[Binary(BinaryType.ImageRef, 2)]
		public ImageListRef Portrait { get; set; } = ImageListRef.None;

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
