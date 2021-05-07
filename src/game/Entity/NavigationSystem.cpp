#include "NavigationSystem.h"
#include "EntityManager.h"
#include "AnimationSystem.h"
#include "Job.h"

#include "../Assets.h"
#include "../Platform.h"

static Vector2 movementTable[]{
	{0,-1}, {0,-1},
	{0.7,-0.7},{0.7,-0.7},{0.7,-0.7},{0.7,-0.7},
	{1,0}, {1,0}, {1,0}, {1,0},
	{0.7,0.7},{0.7,0.7},{0.7,0.7},{0.7,0.7},
	{0,1},{0,1},{0,1},{0,1},
	{-0.7,0.7},{-0.7,0.7},{-0.7,0.7},{-0.7,0.7},
	{-1,0}, {-1,0}, {-1,0}, {-1,0},
	{-0.7,-0.7},{-0.7,-0.7},{-0.7,-0.7},{-0.7,-0.7},
	{0,-1},{0,-1}

};
static Vector2 movementTable8[]{
			{0,-1}, {0.7,-0.7},{1,0}, {0.7,0.7},{0,1}, {-0.7,0.7}, {-1,0},{-0.7,-0.7}
};
static MovementArchetype* ma;
static NavigationArchetype* na;

static void Move(int start, int end) {
	MovementArchetype& archetype = *ma;

	for (int i = start; i < end; ++i) {
		auto& work = *archetype.work[i];
		auto& movement = *archetype.movement[i];
		const auto& nav = archetype.navigation[i];


		if (nav.targetHeading != movement.orientation) {

			int diff = nav.targetHeading - movement.orientation;

			if (diff != 0) {
				if (std::abs(diff) > movement.rotationSpeed) {
					int sign = diff > 0 ? 1 : -1;
					if (std::abs(diff) > 15)
					{
						sign = -sign;
					}

					movement.orientation += sign * movement.rotationSpeed;
					movement.orientation = (movement.orientation + 32) % 32;
				}
				else {
					movement.orientation = nav.targetHeading;
				}
			}

			const auto& unit = archetype.unit[i];
			auto& anim = *archetype.anim[i];

			anim.PlayClip(&unit.def->MovementAnimations[movement.orientation]);
			anim.shadowClip = &unit.def->MovementAnimationsShadow[movement.orientation];
			anim.unitColorClip = &unit.def->MovementAnimationsTeamColor[movement.orientation];
		
			if (movement.orientation != nav.targetHeading)
				continue;
		}

		auto& position = *archetype.position[i];

		Vector2Int distance = nav.target - position;

		if (distance.LengthSquared() < movement.velocity * movement.velocity) {
			position = nav.target;
			work.work = false;

			auto& anim = *archetype.anim[i];
			anim.pause = true;
			anim.clipFrame = 0;
		}
		else {

			Vector2Int move = movementTable[movement.orientation] * movement.velocity;
			position += move;
		}

		archetype.changed[i]->changed = true;
	}
}

void NavigationSystem::MoveEntities(MovementArchetype& archetype) {
	ma = &archetype;
	int end = archetype.size();

	JobSystem::RunJob(end, JobSystem::DefaultJobSize, Move);
}



static void UpdateNav(int start, int end) {
	NavigationArchetype& archetype = *na;

	for (int i = start; i < end; ++i) {
		auto& nav = *archetype.navigation[i];
		const auto& movement = archetype.movement[i];
		const auto& position = archetype.position[i];


		int h = 1'000'000;
		int heading = movement.orientation;
		for (int i = 0; i < 8; i += 1) {

			Vector2Int move = movementTable8[i] * movement.velocity;
			Vector2Int pos = position + move;
			int dist = (nav.target - pos).LengthSquared();
			dist -= i * 4 == movement.orientation ? (movement.velocity * movement.velocity + 1) : 0;
			if (dist < h)
			{
				h = dist;
				heading = i * 4;
			}
		}

		nav.targetHeading = heading;
	}
}

void NavigationSystem::UpdateNavigation(NavigationArchetype& archetype)
{
	na = &archetype;
	int end = archetype.size();
	//int start = 0;
	JobSystem::RunJob(end, JobSystem::DefaultJobSize, UpdateNav);
}
