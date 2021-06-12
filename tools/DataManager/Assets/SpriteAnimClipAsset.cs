using CsvHelper.Configuration.Attributes;
using DataManager.Build;
using DataManager.Gameplay;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	[BinaryData(DataItemType.AnimClips)]
	public class SpriteAnimClipAsset : Asset
	{
		[Binary(BinaryType.String, 32)]
		[Index(0), TypeConverter(typeof(Asset.AssetConverter))]
		public SpriteAsset Sprite { get; set; }

		[Ignore]
		[Binary(BinaryType.UInt, 4)]
		public uint _InstructionStart { get; set; }

		[Ignore]
		[Binary(BinaryType.UInt, 1)]
		public int InstructionCount => Instructions.Count;

		[Index(1)]
		[DefaultEditor]
		[Binary(BinaryType.UInt, 1)]
		public AnimationType Type { get; set; }

		[Index(2), Name("Instructions")]
		public string InstructionsText { get; set; }


		[Ignore]
#warning TODO: Serialize Instructions
		public List<string> Instructions { get; set; } = new List<string>();

		public override string AssetName => $"{Sprite.Name}:{Type}";

		public override void OnAfterDeserialize()
		{
			string[] instructions = InstructionsText.Split(",");
			foreach (var str in instructions)
			{
				Instructions.Add(str);
			}

			Sprite.Clips.Add(this);
		}

		public SpriteAnimClipAsset() : base() { }
		public SpriteAnimClipAsset(SpriteAsset asset, AnimationType type) : this()
		{
			Sprite = asset;
			Type = type;
			Sprite.Clips.Add(this);
		}

		public override void OnBeforeSerizalize()
		{
			InstructionsText = string.Join(',', Instructions);
		}

		public void SetInstructions(IEnumerable<string> instructions)
		{
			Instructions.Clear();
			Instructions.AddRange(instructions.Where(s => !string.IsNullOrEmpty(s)).Select(s => s.Trim()));
			InstructionsText = string.Join(',', instructions);
		}

		public override Asset Clone()
		{
			var clone = base.Clone() as SpriteAnimClipAsset;
			clone.Instructions = Instructions.ToList();
			return clone;
		}

	}
}
