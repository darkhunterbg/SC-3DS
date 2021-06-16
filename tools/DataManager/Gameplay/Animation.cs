using DataManager.Assets;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Gameplay
{
	public enum AnimationType
	{
		Init,
		Death,
		GroundAttackInit,
		AirAttackInit,
		SpecialAbility1,
		GroundAttackRepeat,
		AirAttackRepeat,
		SpecialAbility2,
		GroundAttackToIdle,
		AirAttackToIdle,
		SpecialAbility3,
		Walking,
		Other,
		BurrowInit,
		ConstructHarvest,
		IsWorking,
		Landing,
		LiftOff,
		Burrow,
		UnBorrow,
	}

	public class AnimationState
	{
		public Vector2 Offset;
		public int InstructionId = -1;
		public int FrameDelay;
		public int Orientation { get; private set; }

		private int localFrame = 0;
		public int OrientationOffset = 0;

		public bool FlipSprite => Orientation > 16;

		public int FrameIndex => localFrame + OrientationOffset;

		public void SetInstruction(int id)
		{
			InstructionId = id;
			FrameDelay = 0;
		}

		public void SetFrame(int frame)
		{
			localFrame = frame;
		}

		public void AddOrientation(int orientation)
		{
			Orientation += orientation;
			SetOrientation(Orientation);
		}

		public void SetOrientation(int orientation)
		{
			Orientation = orientation;

			if (Orientation < 0)
				Orientation = 32 + (Orientation % 32);
			else
				Orientation %= 32;

			if (FlipSprite)
				OrientationOffset = 32 - Orientation;
			else
				OrientationOffset = Orientation;
		}


		public bool ExecuteInstruction(SpriteAnimClipAsset clip)
		{
			++InstructionId;
			if (InstructionId >= clip.Instructions.Count)
			{
				InstructionId = -1;
				return false;
			}

			var instr = clip.Instructions[InstructionId];

			if (!string.IsNullOrEmpty(instr))
			{
				var s = instr.Split(' ');

				if (AnimClipInstructionDatabase.Instructions.TryGetValue(s[0], out var instruction))
				{
					if (instruction.Parameters.Count < s.Length)
					{
						var parsed = new object[instruction.Parameters.Count];
						for (int i = 0; i < parsed.Length; ++i)
						{
							parsed[i] = instruction.Parameters[i].Parse(s[i + 1], out bool success);
							if (!success)
								return false;
						}
						return !instruction.Process(this, parsed);
					}
				}
			}

			return true;
		}


	}
}
