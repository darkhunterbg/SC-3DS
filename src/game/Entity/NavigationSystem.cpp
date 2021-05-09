#include "NavigationSystem.h"
#include "EntityManager.h"
#include "AnimationSystem.h"
#include "Job.h"
#include "../Profiler.h"

#include "../Assets.h"

static const Vector2 movementTable32[]{
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
static const Vector2 movementTable8[]{
		{0,-1}, {0.7,-0.7},{1,0}, {0.7,0.7},{0,1}, {-0.7,0.7}, {-1,0},{-0.7,-0.7}
};
static NavigationSystem* s;

 void NavigationSystem::MoveEntitiesJob(int start, int end) {
	 MovementData& data = s->movementData;

	for (int i = start; i < end; ++i) {
		auto& work = *data.work[i];
		auto& movement = *data.movement[i];
		const auto& nav = data.navigation[i];

		if (nav.targetHeading != movement.orientation) {

			uint8_t diff = nav.targetHeading - movement.orientation;

			if (diff != 0) {
				if (std::abs(diff) > movement.rotationSpeed) {
					int sign = diff > 0 ? 1 : -1;
					if (std::abs(diff) > 15)
						sign = -sign;

					movement.orientation += sign * movement.rotationSpeed;
					movement.orientation = (movement.orientation + 32) % 32;
				}
				else {
					movement.orientation = nav.targetHeading;
				}
			}

			const auto& unit = data.unit[i];
			auto& anim = *data.anim[i];
			auto& animEnable = *data.animEnabled[i];
			auto& animTracker = *data.animTracker[i];

			anim.clip  = &unit.def->MovementAnimations[movement.orientation];
			anim.shadowClip = &unit.def->MovementAnimationsShadow[movement.orientation];
			anim.unitColorClip = &unit.def->MovementAnimationsTeamColor[movement.orientation];
			animTracker.PlayClip(anim.clip);
			animEnable.pause = false;

			if (movement.orientation != nav.targetHeading)
				continue;
		}

		auto& position = *data.position[i];

		Vector2Int distance = Vector2Int(nav.target - position);

		if (distance.LengthSquaredInt() < movement.velocity * movement.velocity) {
			position = nav.target;
			work.work = false;

			auto& anim = *data.animEnabled[i];
			anim.pause = true;
		}
		else {

			Vector2Int16 move = Vector2Int16(movementTable32[movement.orientation] * movement.velocity);
			position += move;
		}

		data.changed[i]->changed = true;
	}
}

void NavigationSystem::MoveEntities(EntityManager& em) {

	SectionProfiler p("MoveEntities");

	movementData.clear();

	for (EntityId id : em.NavigationArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);
		if (em.NavigationArchetype.WorkComponents[i].work) {
			movementData.work.push_back(&em.NavigationArchetype.WorkComponents[i]);
			movementData.movement.push_back(&em.NavigationArchetype.MovementComponents[i]);
			movementData.position.push_back(&em.PositionComponents[i]);
			movementData.navigation.push_back(em.NavigationArchetype.NavigationComponents[i]);
			movementData.changed.push_back(&em.EntityChangeComponents[i]);
			movementData.unit.push_back(em.UnitComponents[i]);

			if (em.AnimationArchetype.Archetype.HasEntity(id)) {
				// TODO: animation update goes to a different update
				movementData.anim.push_back(&em.AnimationArchetype.AnimationComponents[i]);
				movementData.animEnabled.push_back(&em.AnimationArchetype.EnableComponents[i]);
				movementData.animTracker.push_back(&em.AnimationArchetype.TrackerComponents[i]);
			}
		}
	}

	s = this;
	JobSystem::RunJob(movementData.size(), JobSystem::DefaultJobSize, MoveEntitiesJob);

	p.Submit();
}

void NavigationSystem::UpdateNavigationJob(int start, int end) {
	NavigationData& data = s->navigationData;


	for (int i = start; i < end; ++i) {
		auto& nav = *data.navigation[i];
		const auto& movement = data.movement[i];
		const auto& position = data.position[i];


		int h = std::numeric_limits<int>::max();
		uint8_t heading = movement.orientation;
		for (int i = 0; i < 8; i += 1) {

			Vector2Int16 move = Vector2Int16(movementTable8[i] * movement.velocity);
			Vector2Int16 pos = position + move;
			int dist = (nav.target - pos).LengthSquaredInt();
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

void NavigationSystem::UpdateNavigation(EntityManager& em)
{
	SectionProfiler p("UpdateNavigation");

	navigationData.clear();

	for (EntityId id : em.NavigationArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);

		if (em.NavigationArchetype.WorkComponents[i].work) {
			navigationData.movement.push_back(em.NavigationArchetype.MovementComponents[i]);
			navigationData.position.push_back(em.PositionComponents[i]);
			navigationData.navigation.push_back(&em.NavigationArchetype.NavigationComponents[i]);
		}
	}

	s = this;
	JobSystem::RunJob(navigationData.size(), JobSystem::DefaultJobSize, UpdateNavigationJob);
}
