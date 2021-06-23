using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Gameplay
{
	public enum UnitAIState
	{
		Idle = 0,
		AttackTarget = 1,
		GoToPosition = 2,
		GoToAttack = 3,
		HoldPosition = 4,
		Patrol = 5,
		Follow = 6,
		GatherResource = 7,
		ReturnCargo = 8,

		Nothing = 0xFF,
	}
}
