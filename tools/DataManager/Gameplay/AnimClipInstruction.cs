using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Gameplay
{
	public delegate bool AnimClipInstructionAction(AnimationState state, string[] parameters);

	public class AnimClipInstruction
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

			public bool Validate(string input)
			{
				if (string.IsNullOrEmpty(input))
					return false;

				if (Type == typeof(int))
					return int.TryParse(input,out _);

				return false;
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

		public bool Process(AnimationState state, string[] parameters) => ProcessAction(state, parameters);

		public string Serialize(params object[] parameters)
		{
			List<string> s = new List<string>() { Instruction };
			for (int i = 0; i < parameters.Count(); ++i)
			{
				s.Add(parameters[i].ToString());
			}

			return string.Join(',', s);
		}


	}

	public static class AnimClipInstructionDatabase
	{
		public static AnimClipInstruction.ParamDef NewParam<TType>(string name) => new AnimClipInstruction.ParamDef(name, typeof(TType));

		public static readonly AnimClipInstruction PlayFrame = new AnimClipInstruction("playfram", (state, p) =>
		  {
			  int.TryParse(p[0], out int f);
			  state.FrameIndex = f;
			  return false;
		  }, NewParam<int>("frameIndex"));

		public static readonly AnimClipInstruction Wait = new AnimClipInstruction("wait", (state, p) =>
		{
			int.TryParse(p[0], out int f);
			state.FrameDelay = f;
			return true;
		}, NewParam<int>("frameCount"));


		public static readonly Dictionary<string, AnimClipInstruction> Instructions = new Dictionary<string, AnimClipInstruction>();

		static AnimClipInstructionDatabase()
		{
			var props = typeof(AnimClipInstructionDatabase).GetFields(BindingFlags.Static| BindingFlags.Public).Where(t => t.FieldType == typeof(AnimClipInstruction)).ToArray();

			foreach(var i in props.Select(p => (AnimClipInstruction)p.GetValue(null)))
			{
				Instructions[i.Instruction] = i;
			}
		}
	}
}
