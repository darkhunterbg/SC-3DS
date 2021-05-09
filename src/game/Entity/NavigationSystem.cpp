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
		const auto& movement = data.movement[i];
		auto& orientation = *data.orientation[i];
		const auto& nav = data.navigation[i];

		if (nav.targetHeading != orientation.orientation) {

			int8_t diff = nav.targetHeading - orientation.orientation;

			if (diff != 0) {
				if (std::abs(diff) > movement.rotationSpeed) {
					int sign = diff > 0 ? 1 : -1;
					if (std::abs(diff) > 15)
						sign = -sign;

					orientation.orientation += sign * movement.rotationSpeed;
					orientation.orientation = (orientation.orientation + 32) % 32;
				}
				else {
					orientation.orientation = nav.targetHeading;
				}
			}

			orientation.changed = true;

			if (orientation.orientation != nav.targetHeading)
				continue;
		}

		auto& position = *data.position[i];

		Vector2Int distance = Vector2Int(nav.target - position);

		if (distance.LengthSquaredInt() < movement.velocity * movement.velocity) {
			position = nav.target;
			work.work = false;

		/*	auto& anim = *data.animEnabled[i];
			anim.pause = true;*/
		}
		else {

			Vector2Int16 move = Vector2Int16(movementTable32[orientation.orientation] * movement.velocity);
			position += move;
		}

		data.changed[i]->changed = true;
	}
}
void NavigationSystem::SetMovementAnimJob(int start, int end) {
	 auto& data = s->movementAnimData;

	 for (int i = start; i < end; ++i) {
		 const auto& unit = data.unit[i];
		 const auto& movement = data.movement[i];
		 auto& orientation = *data.orientation[i];
		 auto& anim = *data.anim[i];
		 auto& animEnable = *data.animEnabled[i];
		 auto& animTracker = *data.animTracker[i];

		 orientation.changed = false;
		 anim.clip = &unit.def->MovementAnimations[orientation.orientation];
		 anim.shadowClip = &unit.def->MovementAnimationsShadow[orientation.orientation];
		 anim.unitColorClip = &unit.def->MovementAnimationsTeamColor[orientation.orientation];
		 animTracker.PlayClip(anim.clip);
		 animEnable.pause = false;
	 }
 }
void NavigationSystem::MoveEntities(EntityManager& em) {

	SectionProfiler p("MoveEntities");

	movementData.clear();

	for (EntityId id : em.NavigationArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);
		if (em.NavigationArchetype.WorkComponents[i].work) {
			movementData.work.push_back(&em.NavigationArchetype.WorkComponents[i]);
			movementData.movement.push_back(em.NavigationArchetype.MovementComponents[i]);
			movementData.orientation.push_back(&em.NavigationArchetype.OrientationComponents[i]);
			movementData.position.push_back(&em.PositionComponents[i]);
			movementData.navigation.push_back(em.NavigationArchetype.NavigationComponents[i]);
			movementData.changed.push_back(&em.EntityChangeComponents[i]);
		}
	}

	s = this;
	JobSystem::RunJob(movementData.size(), JobSystem::DefaultJobSize, MoveEntitiesJob);

	movementAnimData.clear();

	for (EntityId id : em.NavigationArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);

		if (em.NavigationArchetype.OrientationComponents[i].changed &&
			em.AnimationArchetype.Archetype.HasEntity(id))
		{
			movementAnimData.movement.push_back(em.NavigationArchetype.MovementComponents[i]);
			movementAnimData.orientation.push_back(&em.NavigationArchetype.OrientationComponents[i]);
			movementAnimData.unit.push_back(em.UnitComponents[i]);
			movementAnimData.anim.push_back(&em.AnimationArchetype.AnimationComponents[i]);
			movementAnimData.animEnabled.push_back(&em.AnimationArchetype.EnableComponents[i]);
			movementAnimData.animTracker.push_back(&em.AnimationArchetype.TrackerComponents[i]);
		}
	}

	JobSystem::RunJob(movementAnimData.size(), JobSystem::DefaultJobSize, SetMovementAnimJob);

	for (EntityId id : em.NavigationArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);

		if (!em.NavigationArchetype.WorkComponents[i].work && 
			em.AnimationArchetype.Archetype.HasEntity(id)){

			// TODO: Fix this, extra archetypes
			// TODO: movement flag has to be different from navigation enabled flag 
			// (maybe movement system differ from nav system)

			em.AnimationArchetype.EnableComponents[i].pause = true;
		}
	}

	p.Submit();
}

void NavigationSystem::UpdateNavigationJob(int start, int end) {
	NavigationData& data = s->navigationData;


	for (int i = start; i < end; ++i) {
		auto& nav = *data.navigation[i];
		const auto& movement = data.movement[i];
		const auto& position = data.position[i];

		int h = std::numeric_limits<int>::max();
		uint8_t heading = nav.currentHeading;
		for (int i = 0; i < 8; i += 1) {

			Vector2Int16 move = Vector2Int16(movementTable8[i] * movement.velocity);
			Vector2Int16 pos = position + move;
			int dist = (nav.target - pos).LengthSquaredInt();
			dist -= i * 4 == nav.currentHeading ? (movement.velocity * movement.velocity + 1) : 0;
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
			em.NavigationArchetype.NavigationComponents[i].currentHeading = em.NavigationArchetype.OrientationComponents[i].orientation;
			navigationData.navigation.push_back(&em.NavigationArchetype.NavigationComponents[i]);
		}
	}

	s = this;
	JobSystem::RunJob(navigationData.size(), JobSystem::DefaultJobSize, UpdateNavigationJob);
}
