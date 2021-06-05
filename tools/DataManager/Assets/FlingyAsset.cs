using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{

	public class FlingyAsset
	{
		private LogicalSpriteAsset _sprite;

		[Index(0)]
		[DefaultEditor]
		public string Name { get; set; }

		[Index(1),Name("Sprite")]
		public string SpriteName { get; set; }

		[Ignore]
		[SpriteEditor]
		public LogicalSpriteAsset Sprite
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

		static int id = 0;

		[UniqueKey]
		public readonly int Id = 0;

		public FlingyAsset() {
			Id = ++id;
		}

		public FlingyAsset(LogicalSpriteAsset sprite) : base()
		{
			Sprite = sprite;
		}

		public void OnAfterDeserialize()
		{
			_sprite = AppGame.AssetManager.Sprites.FirstOrDefault(s => s.Name == SpriteName);
		}
	}
}
