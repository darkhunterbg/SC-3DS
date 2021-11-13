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
		GoTo = 2,
		GoToAttack = 3,
		Patrol = 4,
		HoldPosition = 5,
		Follow = 6,

		Nothing = 0xFF,
	}
}
