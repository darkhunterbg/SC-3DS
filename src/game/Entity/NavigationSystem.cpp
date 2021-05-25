#include "NavigationSystem.h"
#include "EntityManager.h"
#include "AnimationSystem.h"
#include "Job.h"
#include "EntityUtil.h"

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
static const Vector2Int16 movementTableNavGrid[]{
		{0,-1}, {1,-1},{1,0}, {1,1},{0,1}, {-1,1}, {-1,0},{-1,-1}
};

static NavigationSystem* s;
static EntityManager* e;

static  int Evaluate(uint8_t d, uint8_t v, Vector2Int16 pos, const Rectangle16& collider, const NavigationComponent& nav, EntityId entity) {
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
void NavigationSystem::SetSize(Vector2Int16 size)
{
	gridSize = size / 32;
	//memset(navGrid.data(), 0, gridSize.x * gridSize.y);
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

		uint8_t dir = 255;
		int val = std::numeric_limits<int>::max();

		//if ((nav.target - position).LengthSquaredInt() < 64 * 64)
		{
			for (uint8_t d = 0; d < 8; d++) {

				int eval = Evaluate(d, velocity, position, collider, nav, entity);
				if (eval < val) {
					val = eval;
					dir = d << 2;
				}
			}
		}
		//else {

		//	for (uint8_t d = 0; d < 8; d++) {

		//		int eval = EvaluateNavGrid(d,  position,  nav.target);
		//		if (eval < val) {
		//			val = eval;
		//			dir = d << 2;
		//		}
		//	}
		//}

		nav.targetHeading = dir;
	}
}
void NavigationSystem::UpdateNavigation(EntityManager& em)
{
	//SectionProfiler p("UpdateNavigation");

	navigationData.clear();

	applyNav.clear();

	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {
		if (em.NavigationArchetype.Archetype.HasEntity(id)) {
			int i = Entity::ToIndex(id);

			if (em.FlagComponents[i].test(ComponentFlags::NavigationWork)) {
				applyNav.push_back(id);

				auto& nav = em.NavigationArchetype.NavigationComponents[i];

				if (nav.targetHeading == 255 ||
					em.OrientationComponents[i] ==
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

	s = this;
	e = &em;

	JobSystem::RunJob(navigationData.size(), JobSystem::DefaultJobSize, UpdateNavigationJob);
}

void NavigationSystem::ApplyUnitNavigationJob(int start, int end) {
	EntityManager& em = *e;

	for (int index = start; index < end; ++index) {
		EntityId id = s->applyNav[index];
		int i = Entity::ToIndex(id);

		const auto& nav = em.NavigationArchetype.NavigationComponents[i];
		auto& orientation = em.OrientationComponents[i];
		const auto& unitMove = em.UnitArchetype.MovementComponents[i];;

		auto& flags = em.FlagComponents[i];
		auto& movement = em.MovementArchetype.MovementComponents[i];

		int8_t diff = nav.targetHeading - orientation;

		const auto& unit = em.UnitArchetype.UnitComponents.GetComponent(id);

		if (diff != 0) {

			if (nav.targetHeading != 255) {

				uint8_t rotation = unitMove.rotationSpeed;

				if (std::abs(diff) > rotation) {
					int s = sign(diff);
					if (std::abs(diff) > 16)
						s = -s;

					orientation += s * rotation;
					orientation = (orientation + 32) % 32;
				}
				else {
					orientation = nav.targetHeading;
				}

				flags.set(ComponentFlags::OrientationChanged);
				flags.set(ComponentFlags::UnitStateChanged);
				em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::Turning;
			}

		}
		else {
			const auto& position = em.PositionComponents[i];

			int velocity = unitMove.movementSpeed;

			Vector2Int distance = Vector2Int(nav.target - position);

			if (distance.LengthSquaredInt() < velocity * velocity) {

				flags.set(ComponentFlags::UnitStateChanged);
				em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::Idle;
			}
			else {
				movement.velocity = Vector2Int8(movementTable32[orientation] * velocity);

				if (em.UnitArchetype.StateComponents.GetComponent(id) != UnitState::Movement)
				{
					flags.set(ComponentFlags::UnitStateChanged);
					em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::Movement;
				}
			}
		}
	}
}

void NavigationSystem::ApplyUnitNavigaion(EntityManager& em)
{
	//SectionProfiler p("ApplyNavigation");

	s = this;
	e = &em;

	JobSystem::RunJob(applyNav.size(), JobSystem::DefaultJobSize, ApplyUnitNavigationJob);
}

void NavigationSystem::UpdateNavGrid(EntityManager& em)
{
	SectionProfiler p("UpdateNavGrid");

	memset(navGrid.data(), 0, gridSize.x * gridSize.y);

	for (EntityId id : em.CollisionArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);

		//Rectangle16 collider = em.CollisionArchetype.ColliderComponents[i].collider;
		//collider.position += em.PositionComponents[i];

		Vector2Int16 center = em.PositionComponents[i];

		center.x = center.x >> 5;
		center.y = center.y >> 5;

		//collider.position.x = collider.position.x >> 3;
		//collider.position.y =collider.position.y >> 3;
		//collider.size.x = collider.size.x >> 3;
		//collider.size.y = collider.size.y >> 3;

		SetUnpassable(center.x, center.y);

		//Vector2Int max = Vector2Int(collider.GetMax());
	/*	for (int y = collider.position.y; y < max.x; ++y) {
			for (int x = collider.position.x; x < max.y; ++x) {
				SetPassable( x,y );
			}
		}*/
	}
}
