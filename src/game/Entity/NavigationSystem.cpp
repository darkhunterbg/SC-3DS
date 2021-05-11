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
		{0,-1}, {0.8,-0.8},{1,0}, {0.8,0.8},{0,1}, {-0.8,0.8}, {-1,0},{-0.8,-0.8}
};
static NavigationSystem* s;
static EntityManager* e;

static inline int Evaluate(uint8_t d, uint8_t v, Vector2Int16 pos, const Rectangle16& collider, const NavigationComponent& nav, EntityId entity) {
	Vector2Int16 move = Vector2Int16(movementTable8[d] * (v));
	pos += move;

	Rectangle16 c = collider;
	c.position += pos;

	if (!e->CollidesWithAny(c, entity))
	{
		int dist = ((nav.target - pos)).LengthSquaredInt();

		if (dist > 64)
			dist -= d << 2 == nav.targetHeading ? 64 : 0;
		else
			dist -= d << 2 == nav.targetHeading ? v << 2 : 0;

		return dist;
	}


	return  std::numeric_limits<int>::max();
}

void NavigationSystem::UpdateNavigationJob(int start, int end) {
	NavigationData& data = s->navigationData;
	EntityManager& em = *e;


	for (int i = start; i < end; ++i) {
		auto& nav = *data.navigation[i];
		uint8_t velocity = data.velocity[i];
		const auto& position = data.position[i];
		const auto& collider = data.collider[i];
		EntityId entity = data.entity[i];

		int val = std::numeric_limits<int>::max();
		uint8_t dir = 255;

		for (uint8_t d = 0; d < 8; d++) {

			int eval = Evaluate(d, velocity, position, collider, nav, entity);
			if (eval < val) {
				val = eval;
				dir = d << 2;
			}
		}

		nav.targetHeading = dir;
	}
}
void NavigationSystem::UpdateNavigation(EntityManager& em)
{
	SectionProfiler p("UpdateNavigation");

	navigationData.clear();

	applyNav.clear();

	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {
		if (em.NavigationArchetype.Archetype.HasEntity(id)) {
			int i = Entity::ToIndex(id);

			if (em.NavigationArchetype.WorkComponents[i].work) {
				applyNav.push_back(id);

				auto& nav = em.NavigationArchetype.NavigationComponents[i];

				if (nav.targetHeading == 255 ||
					em.UnitArchetype.OrientationComponents[i].orientation ==
					nav.targetHeading) {

		
					navigationData.velocity.push_back(em.UnitArchetype.MovementComponents[i].movementSpeed);
					navigationData.position.push_back(em.PositionComponents[i]);
					navigationData.collider.push_back(em.CollisionArchetype.ColliderComponents[i].collider);
					//em.NavigationArchetype.NavigationComponents[i].collider =  em.CollisionArchetype.ColliderComponents[i].collider;
					navigationData.navigation.push_back(&nav);
					navigationData.entity.push_back(id);
				}
			}
		}
	}
	p.Submit();
	s = this;
	e = &em;

	SectionProfiler p2("UpdateNavigationJob");
	JobSystem::RunJob(navigationData.size(), JobSystem::DefaultJobSize, UpdateNavigationJob);
}

void NavigationSystem::ApplyUnitNavigationJob(int start, int end) {
	EntityManager& em = *e;

	for (int index = start; index < end; ++index) {
		EntityId id = s->applyNav[index];
		int i = Entity::ToIndex(id);

		const auto& nav = em.NavigationArchetype.NavigationComponents[i];
		auto& orientation = em.UnitArchetype.OrientationComponents[i];
		const auto& unitMove = em.UnitArchetype.MovementComponents[i];;

		auto& work = em.NavigationArchetype.WorkComponents[i];
		auto& movement = em.MovementArchetype.MovementComponents[i];

		int8_t diff = nav.targetHeading - orientation.orientation;

		if (diff != 0) {

			if (nav.targetHeading != 255) {

				uint8_t rotation = unitMove.rotationSpeed;;

				if (std::abs(diff) > rotation) {
					int s = sign(diff);
					if (std::abs(diff) > 16)
						s = -s;

					orientation.orientation += s * rotation;
					orientation.orientation = (orientation.orientation + 32) % 32;
				}
				else {
					orientation.orientation = nav.targetHeading;
				}
			}
			orientation.changed = true;
			movement.velocity = { 0,0 };
		}
		else {
			const auto& position = em.PositionComponents[i];

			int velocity = unitMove.movementSpeed;

			Vector2Int distance = Vector2Int(nav.target - position);

			if (distance.LengthSquaredInt() < velocity * velocity) {
				movement.velocity = { 0,0 };
				em.NavigationArchetype.WorkComponents[i].work = false;
			}
			else {
				movement.velocity = Vector2Int8(movementTable32[orientation.orientation] * velocity);
			}
		}
	}
}

void NavigationSystem::ApplyUnitNavigaion(EntityManager& em)
{
	SectionProfiler p("ApplyNavigation");

	NavigationSystem* s = this;
	EntityManager* e = &em;

	JobSystem::RunJob(applyNav.size(), JobSystem::DefaultJobSize, ApplyUnitNavigationJob);
}
