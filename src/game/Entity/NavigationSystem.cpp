#include "NavigationSystem.h"
#include "EntityManager.h"
#include "AnimationSystem.h"
#include "Job.h"
#include "../Profiler.h"

#include "../Assets.h"

#include <algorithm>

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
static EntityManager* e;

/*
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

			//	auto& anim = *data.animEnabled[i];
			//	anim.pause = true;
		}
		else {

			Vector2Int16 move = Vector2Int16(movementTable32[orientation.orientation] * movement.velocity);
			position += move;
		}

		data.changed[i]->changed = true;
	}
}*/

/*void NavigationSystem::MoveEntities(EntityManager& em) {

	//SectionProfiler p("MoveEntities");

	movementData.clear();

	for (EntityId id : em.NavigationArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);
		if (em.NavigationArchetype.WorkComponents[i].work) {
			movementData.work.push_back(&em.NavigationArchetype.WorkComponents[i]);
			movementData.movement.push_back(em.MovementArchetype.MovementComponents[i]);
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
			movementAnimData.movement.push_back(em.MovementArchetype.MovementComponents[i]);
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
			em.AnimationArchetype.Archetype.HasEntity(id)) {

			// TODO: Fix this, extra archetypes
			// TODO: movement flag has to be different from navigation enabled flag 
			// (maybe movement system differ from nav system)

			em.AnimationArchetype.EnableComponents[i].pause = true;
		}
	}
}

*/

static bool ActionSort(const AStarAction& a, const AStarAction& b)
{
	return a.value > b.value;
}

static int Evaluate(uint8_t d, uint8_t v, Vector2Int16& pos, const NavigationComponent& nav, EntityId entity) {
	Vector2Int16 move = Vector2Int16(movementTable8[d] * v);
	pos += move;

	if (pos.x < 0 || pos.y < 0)
		return  std::numeric_limits<int>::max();

	Rectangle16 c = nav.collider;
	c.position += pos;

	if (!e->CollidesWithAny(c, entity))
	{
		//action.dir = d * 4;
		int dist = ((nav.target - pos)).LengthSquaredInt();
		//dist -= currentDir == action.dir ? (1) : 0;
		//action.value = dist;
		if (dist <= v * v)
			return 0;

		return dist;

		/*if (dist < vSquared)
		{
			action.iter = maxIter;
			action.value = 0;
			found = true;

		}*/
	}

	return  std::numeric_limits<int>::max();
}

void NavigationSystem::UpdateNavigationJob(int start, int end) {
	NavigationData& data = s->navigationData;
	EntityManager& em = *e;

	auto& results = s->tlsResults->Get();

	for (int i = start; i < end; ++i) {
		auto& nav = *data.navigation[i];
		const auto& movement = data.movement[i];
		const auto& position = data.position[i];
		EntityId entity = data.entities[i];

		static constexpr const int maxIter = 2;

		uint8_t v = movement.velocity.Length();

		results.clear();

		for (int i = 0; i < 8; i++) {
			Vector2Int16 p = position;

			int eval = Evaluate(i , v, p, nav, entity);
			if (eval != std::numeric_limits<int>::max())
				results.push_back({ p , 1, eval , (uint8_t)(i * 4) , (uint8_t)(i * 4) });
		}


		std::sort(results.begin(), results.end(), ActionSort);

		bool found = false;

		while (results.size() > 0) {

			bool done = true;
			AStarAction action;

			int x = results.size();
			for (auto i = results.rbegin(); i != results.rend(); ++i) {
				--x;
				if (i->iter == maxIter)
					continue;

				action = *i;
				done = false;

				results.erase(results.begin() + x);

				break;
			}

			if (done)
				break;


			uint8_t currentDir = action.dir;
			Vector2Int16 currentPos = action.pos;
			++action.iter;


			for (int p = 0; p <= 4; ++p) {

				int d = (p + currentDir + 6) % 8;

				Vector2Int16 move = Vector2Int16(movementTable8[d] * v);
				action.pos = currentPos + move;

				if (action.pos.x < 0 || action.pos.y < 0)
					continue;

				Rectangle16 c = nav.collider;
				c.position += action.pos;

				if (!e->CollidesWithAny(c, entity))
				{
					action.dir = d * 4;
					int dist = ((nav.target - action.pos)).LengthSquaredInt();
					dist -= nav.targetHeading == action.startDir ? (1) : 0;
					action.value = dist;

					results.push_back(action);

					if (dist < movement.velocity.LengthSquaredInt())
					{
						action.iter = maxIter;
						//action.value = 0;
						found = true;

					}
				}
			}


			std::sort(results.begin(), results.end(), ActionSort);

			if (found)
				break;
		}

		if (results.size() > 0) {
			const auto& move = *results.crbegin();
			nav.targetHeading = move.startDir;
		}

	}
}
void NavigationSystem::UpdateNavigation(EntityManager& em)
{
	//SectionProfiler p("UpdateNavigation");

	navigationData.clear();

	for (EntityId id : em.NavigationArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);

		if (em.NavigationArchetype.WorkComponents[i].work &&
			em.NavigationArchetype.OrientationComponents[i].orientation ==
			em.NavigationArchetype.NavigationComponents[i].targetHeading) {

			navigationData.movement.push_back(em.MovementArchetype.MovementComponents[i]);
			navigationData.position.push_back(em.PositionComponents[i]);
			em.NavigationArchetype.NavigationComponents[i].currentHeading = em.NavigationArchetype.OrientationComponents[i].orientation;
			em.NavigationArchetype.NavigationComponents[i].collider =  em.CollisionArchetype.ColliderComponents[i].collider;
			navigationData.navigation.push_back(&em.NavigationArchetype.NavigationComponents[i]);
			navigationData.entities.push_back(id);
		}
	}

	s = this;
	e = &em;
	JobSystem::RunJob(navigationData.size(), JobSystem::DefaultJobSize, UpdateNavigationJob);
}
