using DataManager.Assets;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Gameplay
{
	public delegate bool AnimClipInstructionAction(AnimationState state, object[] parameters);

	public class AnimClipInstruction
	{
		public struct ParamDef
		{
			public string Name;
			public Type Type;

			public override string ToString()
			{
				return $"[{Type.Name}] {Name}";
			}

			public ParamDef(string name, Type type)
			{
				Name = name;
				Type = type;
			}

			public object Parse(string input, SpriteAsset sprite, out bool success)
			{
				success = false;

				if (string.IsNullOrEmpty(input))
					return Type.IsByRef ? null : Activator.CreateInstance(Type);

				if (Type == typeof(int))
					if (success = int.TryParse(input, out var a))
						return a;

				if (Type == typeof(ImageFrameRef))
				{
					if (success = int.TryParse(input, out var a))
					{
						var image = sprite.Image.Image;
						var frame = image.GetFrame(a);
						if (frame == null)
							success = false;
						else
							return new ImageFrameRef(frame);
					}
				}


				return Type.IsByRef ? null : Activator.CreateInstance(Type);
			}

			public bool Validate(string input, SpriteAsset sprite)
			{
				Parse(input, sprite, out bool success);
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

		public string Serialize(object[] parameters)
		{
			List<string> s = new List<string>() { Instruction };
			for (int i = 0; i < parameters.Count(); ++i)
			{
				s.Add(parameters[i].ToString());
			}

			return string.Join(' ', s);
		}
	}

	public static class AnimClipInstructionDatabase
	{
		public static AnimClipInstruction.ParamDef NewParam<TType>(string name) => new AnimClipInstruction.ParamDef(name, typeof(TType));

		public static readonly AnimClipInstruction Frame = new AnimClipInstruction("frame", (state, p) =>
		  {
			  state.FrameIndex = ((int)p[0]);
			  return false;
		  }, NewParam<int>("frameIndex"));

		public static readonly AnimClipInstruction FrameOrientated = new AnimClipInstruction("frameo", (state, p) =>
		{
			state.SetFrameOrientated(((int)p[0]));
			return false;
		}, NewParam<int>("frameIndex"));

		public static readonly AnimClipInstruction Wait = new AnimClipInstruction("wait", (state, p) =>
		{
			state.FrameDelay = (int)p[0];
			return true;
		}, NewParam<int>("frameCount"));


		public static readonly Dictionary<string, AnimClipInstruction> Instructions = new Dictionary<string, AnimClipInstruction>();

		static AnimClipInstructionDatabase()
		{
			var props = typeof(AnimClipInstructionDatabase).GetFields(BindingFlags.Static | BindingFlags.Public).Where(t => t.FieldType == typeof(AnimClipInstruction)).ToArray();

			foreach (var i in props.Select(p => (AnimClipInstruction)p.GetValue(null)))
			{
				Instructions[i.Instruction] = i;
			}
		}
	}
}
