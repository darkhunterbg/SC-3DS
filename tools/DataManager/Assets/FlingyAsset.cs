using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{

	public class FlingyAsset : Asset
	{
		private SpriteAsset _sprite;

		[Index(0)]
		[DefaultEditor]
		public string Name { get; set; }

		[Index(1),Name("Sprite")]
		public string SpriteName { get; set; }

		[Ignore]
		[DefaultEditor]
		public SpriteAsset Sprite
		{
			get { return _sprite; }
			set { _sprite = value;
				SpriteName = _sprite?.Name ?? null;
			}
		}

		[Index(2)]
		[DefaultEditor]
		public int TopSpeed { get; set; }

		[Index(3)]
		[DefaultEditor]
		public int Acceleration { get; set; }


		[Index(4)]
		[DefaultEditor]
		public int HalfDistance { get; set; }

		[Index(5)]
		[DefaultEditor]
		public int TurnRadius { get; set; }

		[Index(6)]
		[DefaultEditor]
		public MoveControlType MoveControl { get; set; }

		public override string AssetName => Name;
		public override GuiTexture Preview => Sprite?.Preview;

		public FlingyAsset() : base() { }
		public FlingyAsset(SpriteAsset sprite) : base()
		{
			Sprite = sprite;
		}

		public override void OnAfterDeserialize()
		{
			_sprite = AppGame.AssetManager.GetAssets<SpriteAsset>().FirstOrDefault(s => s.Name == SpriteName);
		}
	}
}
