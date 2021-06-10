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
		public int InstructionId;
		public int FrameDelay;
		public int Orientation { get; private set; }

		public int FrameIndex = 0;

		public bool FlipSprite => Orientation > 16;


		public int GetActualFrameIndex(SpriteAsset sprite)
		{
			int frameIndex = FrameIndex;

			if (sprite.IsRotating)
				frameIndex *= 17;

			if (FlipSprite)
			{
				frameIndex += 32 - Orientation;
			}
			else
			{
				frameIndex += Orientation;
			}
			return frameIndex;
		}

		public void SetOrientation(int orientation)
		{
			Orientation = orientation;

			if (Orientation < 0)
				Orientation = 32 + (Orientation % 32);
			else
				Orientation %= 32;
		}



		public void ExecuteInstructions(SpriteAnimClipAsset clip)
		{
			bool end = false;

			FrameDelay = 0;

			while (!end)
			{
				if (InstructionId >= clip.Instructions.Count)
				{
					InstructionId = 0;
					return;
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
							for(int i=0;i<parsed.Length;++i)
							{
								parsed[i] = instruction.Parameters[i].Parse(s[i + 1], out bool success);
								if (!success)
									continue;
							}
							end = instruction.Process(this, parsed);
						}
					}
				}

				++InstructionId;
				if (InstructionId >= clip.Instructions.Count)
				{
					InstructionId = 0;
					break;
				}
			}
		}
	}
}
