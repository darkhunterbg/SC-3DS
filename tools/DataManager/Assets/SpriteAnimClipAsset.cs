using CsvHelper.Configuration.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Assets
{
	public class AnimClipInstructionDef
	{
		public struct ParamDef
		{
			public string Name;
			public Type Type;

			public ParamDef(string name, Type type)
			{
				Name = name;
				Type = type;
			}
		}

		public readonly string Instruction;
		public List<ParamDef> Parameters = new List<ParamDef>();

		public AnimClipInstructionDef(string instruction)
		{
			Instruction = instruction;
		}

		public string Serialize(params object[] parameters)
		{
			List<string> s = new List<string>() { Instruction };
			for(int i = 0; i < parameters.Count(); ++i)
			{
				s.Add(parameters[i].ToString());
			}

			return string.Join(',', s);
		}

		public static AnimClipInstructionDef New(string i) => new AnimClipInstructionDef(i);
		public static AnimClipInstructionDef New<T1>(string i, string p1) => new AnimClipInstructionDef(i) { Parameters = new List<ParamDef>() { new ParamDef("p1", typeof(T1)) } };

		public static AnimClipInstructionDef New<T1, T2>(string i, string p1, string p2) => new AnimClipInstructionDef(i)
		{
			Parameters = new List<ParamDef>() {
			new ParamDef("p1", typeof(T1)),         new ParamDef("p2", typeof(T2)),
		}
		};

		public static List<AnimClipInstructionDef> Instructions = new List<AnimClipInstructionDef>()
		{
			New<int>("playfram","frameIndex"),
			New<int>("wait","frameCount"),
			New<int>("turncwise", "directionAnount"),
			New<int>("turnccwise","directionAnount"),
		};

	}

	public class SpriteAnimClipAsset : Asset
	{
		//public class Instruction
		//{
		//	AnimClipInstructionDef Def;
		//	List<object> Params = new List<object>();
		//}

		[Index(0), TypeConverter(typeof(Asset.AssetConverter))]
		public SpriteAsset Sprite { get; set; }

		[Index(1)]
		[DefaultEditor]
		public AnimationType Type { get; set; }

		[Index(2), Name("Instructions")]
		public string InstructionsText { get; set; }

		[Ignore]
		public List<string> Instructions { get; set; } = new List<string>();

		public override string AssetName => $"{Sprite.Name}:{Type}";

		[Ignore]
		public static IEnumerable<AnimClipInstructionDef> InstructionDefs => AnimClipInstructionDef.Instructions;

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
		public SpriteAnimClipAsset(SpriteAsset asset, AnimationType type) : this() {
			Sprite = asset;
			Type = type;
			Sprite.Clips.Add(this);
		}

		public override void OnBeforeSerizalize()
		{
			InstructionsText = string.Join(',', Instructions);
		}
	}
}
