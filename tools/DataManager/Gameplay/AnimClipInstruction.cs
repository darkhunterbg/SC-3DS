using DataManager.Assets;
using DataManager.Build;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Gameplay
{
	public delegate bool AnimClipInstructionAction(AnimationState state, object[] parameters);

	[BinaryData(DataItemType.AnimInstructions)]
	public class AnimClipInstructionData
	{
		[Binary(BinaryType.UInt, 1)]
		public int InstructionId { get; set; }

		[Binary(BinaryType.Raw, 4)]
		public byte[] Parameters { get; set; } = new byte[4];

		public SpriteAnimClipAsset Clip { get; private set; }

		public AnimClipInstructionData(SpriteAnimClipAsset clip, string rawInstruction)
		{
			Clip = clip;

			var split = rawInstruction.Split(' ');
			string instText = split.FirstOrDefault();

			if (!AnimClipInstructionDatabase.Instructions.TryGetValue(instText, out var instr))
				return;

			InstructionId = AnimClipInstructionDatabase.Instructions.Values.IndexOf(instr);

			if (instr.Parameters.Count != split.Length - 1)
				return;

			for (int i = 0; i < instr.Parameters.Count; ++i) {
				var paramDef = instr.Parameters[i];

				var obj = paramDef.Parse(split[i + 1], out bool valid);

				if (!valid)
					continue;

				var bin = paramDef.BinarySerialize(obj);
				for (int j = 0; j < bin.Length; ++j) {
					Parameters[i * 2 + j] = bin[j];
				}

			}
		}
	}

	public class AnimClipInstruction
	{
		public struct ParamDef
		{
			public string Name;
			public Type Type;

			public override string ToString()
			{
				return Name;
			}

			public ParamDef(string name, Type type)
			{
				Name = name;
				Type = type;
			}

			public object Parse(string input, out bool success)
			{
				success = false;

				if (string.IsNullOrEmpty(input))
					return Type.IsByRef ? null : Activator.CreateInstance(Type);

				if (Type == typeof(ushort))
					if (success = ushort.TryParse(input, out var a))
						return a;

				if (Type == typeof(byte))
					if (success = byte.TryParse(input, out var a))
						return a;

				if (Type.IsSubclassOf(typeof(Asset))) {
					var assets = AppGame.AssetManager.GetAssetDatabase(Type).Assets;
					var asset = assets.FirstOrDefault(s => s.AssetName == input);
					success = true;
					return asset;
				}


				return Type.IsByRef ? null : Activator.CreateInstance(Type);
			}

			public byte[] BinarySerialize(object value)
			{
				return BinaryAttribute.Serialize(value, Type);
			}

			public bool Validate(string input)
			{
				Parse(input, out bool success);
				return success;
			}
		}

		public readonly string Instruction;
		public readonly List<ParamDef> Parameters = new List<ParamDef>();
		public readonly AnimClipInstructionAction ProcessAction;


		public AnimClipInstruction(string instruction, AnimClipInstructionAction action, params ParamDef[] parameters)
		{
			Instruction = instruction;
			ProcessAction = action;
			Parameters.AddRange(parameters);
		}

		public bool Process(AnimationState state, object[] parameters) => ProcessAction(state, parameters);

		public string StringSerialize(object[] parameters)
		{
			List<string> s = new List<string>() { Instruction };
			for (int i = 0; i < parameters.Count(); ++i) {
				s.Add(parameters[i].ToString());
			}

			return string.Join(' ', s);
		}

	}

	public static class AnimClipInstructionDatabase
	{
		public static AnimClipInstruction.ParamDef NewParam<TType>(string name) => new AnimClipInstruction.ParamDef(name, typeof(TType));

		static Random random = new Random();


		public static readonly AnimClipInstruction Frame = new AnimClipInstruction("frame", (state, p) =>
		  {
			  state.SetFrame((ushort)p[0]);
			  return false;
		  }, NewParam<ushort>("frameIndex"));

		public static readonly AnimClipInstruction Wait = new AnimClipInstruction("wait", (state, p) =>
		{
			state.FrameDelay = (ushort)p[0];
			return true;
		}, NewParam<ushort>("frameCount"));

		public static readonly AnimClipInstruction WaitRandom = new AnimClipInstruction("waitrand", (state, p) =>
		{
			int min = (ushort)p[0];
			int max = Math.Max(min, (ushort)p[1]);
			state.FrameDelay = random.Next(min, max);
			return true;
		}, NewParam<ushort>("frameMin"), NewParam<ushort>("frameMax"));

		public static readonly AnimClipInstruction Face = new AnimClipInstruction("face", (state, p) =>
		{
			state.SetOrientation((byte)p[0]);
			return false;
		}, NewParam<byte>("orientation"));

		public static readonly AnimClipInstruction TurnClockWise = new AnimClipInstruction("turncw", (state, p) =>
		{
			state.AddOrientation((byte)p[0]);
			return false;
		}, NewParam<byte>("rotation"));

		public static readonly AnimClipInstruction TurnClocCounterkWise = new AnimClipInstruction("turnccw", (state, p) =>
		{
			state.AddOrientation(-(byte)p[0]);
			return false;
		}, NewParam<byte>("rotation"));

		public static readonly AnimClipInstruction GoTo = new AnimClipInstruction("goto", (state, p) =>
		{
			state.InstructionId = (byte)p[0];
			--state.InstructionId;
			return false;
		}, NewParam<byte>("instruction"));


		public static readonly AnimClipInstruction Attack = new AnimClipInstruction("attack", (state, p) => false);

		public static readonly AnimClipInstruction PlaySound = new AnimClipInstruction("sound", (state, p) =>
		{
			var sound = p[0] as SoundSetAsset;
			if (sound != null) {
				sound.Activate();
			}
			return false;
		},
			NewParam<SoundSetAsset>("sound set"), NewParam<ushort>("priority"));

		public static readonly AnimClipInstruction SpawnSprite = new AnimClipInstruction("sprite", (state, p) => false,
		NewParam<SpriteAsset>("sprite"));

		public static readonly AnimClipInstruction SpawnSpriteBackground = new AnimClipInstruction("spriteback", (state, p) => false,
		NewParam<SpriteAsset>("sprite"));

		public static readonly AnimClipInstruction Destroy = new AnimClipInstruction("destroy", (state, p) => true);


		public static readonly AnimClipInstruction Move = new AnimClipInstruction("move", (state, p) => false, NewParam<ushort>("distance"));

		public static readonly Dictionary<string, AnimClipInstruction> Instructions = new Dictionary<string, AnimClipInstruction>();

		static AnimClipInstructionDatabase()
		{
			var props = typeof(AnimClipInstructionDatabase).GetFields(BindingFlags.Static | BindingFlags.Public).Where(t => t.FieldType == typeof(AnimClipInstruction)).ToArray();

			foreach (var i in props.Select(p => (AnimClipInstruction)p.GetValue(null))) {
				Instructions[i.Instruction] = i;
			}
		}
	}
}
