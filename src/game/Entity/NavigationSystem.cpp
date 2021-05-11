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

static bool ActionSort(const AStarAction& a, const AStarAction& b)
{
	return a.value > b.value;
}

static int Evaluate(uint8_t d, uint8_t v, Vector2Int16& pos, const NavigationComponent& nav, EntityId entity) {
	Vector2Int16 move = Vector2Int16(movementTable8[d] * v);
	pos += move;

	if (pos.x < 0 || pos.y < 0)
		return  std::numeric_limits<int>::max();

	//Rectangle16 c = nav.collider;
	//c.position += pos;

	//if (!e->CollidesWithAny(c, entity))
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
		uint8_t velocity = data.velocity[i];
		const auto& position = data.position[i];
		EntityId entity = data.entities[i];

		static constexpr const int maxIter = 1;

		uint8_t v = velocity;

		results.clear();

		for (int i = 0; i < 8; i++) {
			Vector2Int16 p = position;

			int eval = Evaluate(i, v, p, nav, entity);
			if (eval != std::numeric_limits<int>::max())
				results.push_back({ p , 1, eval , (uint8_t)(i * 4) , (uint8_t)(i * 4) });
		}


		std::sort(results.begin(), results.end(), ActionSort);

		bool found = false;

		/*
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
		}*/

		if (results.size() > 0) {
			const auto& move = *results.crbegin();
			nav.targetHeading = move.startDir;
		}
		else {
			nav.targetHeading = 255;
		}

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

			if (em.NavigationArchetype.WorkComponents[i].work)
				applyNav.push_back(id);

			em.NavigationArchetype.NavigationComponents[i].targetHeading = 16;


			if (em.UnitArchetype.OrientationComponents[i].orientation ==
				em.NavigationArchetype.NavigationComponents[i].targetHeading) {

				navigationData.velocity.push_back(em.UnitArchetype.UnitComponents[i].def->MovementSpeed);
				navigationData.position.push_back(em.PositionComponents[i]);
				em.NavigationArchetype.NavigationComponents[i].currentHeading = em.UnitArchetype.OrientationComponents[i].orientation;
				//em.NavigationArchetype.NavigationComponents[i].collider =  em.CollisionArchetype.ColliderComponents[i].collider;
				navigationData.navigation.push_back(&em.NavigationArchetype.NavigationComponents[i]);
				navigationData.entities.push_back(id);
			}
		}
	}

	s = this;
	e = &em;
	//JobSystem::RunJob(navigationData.size(), JobSystem::DefaultJobSize, UpdateNavigationJob);
}



void NavigationSystem::ApplyUnitNavigaion(EntityManager& em)
{
	SectionProfiler p("ApplyNavigation");

	uint8_t rotation = 1;
	uint8_t velocity = 4;

	for (EntityId id : applyNav) {
		int i = Entity::ToIndex(id);
		const auto& nav = em.NavigationArchetype.NavigationComponents[i];
		auto& orientation = em.UnitArchetype.OrientationComponents[i];
		//const auto& unitDef = *em.UnitArchetype.UnitComponents[i].def;

		auto& work = em.NavigationArchetype.WorkComponents[i];
		auto& movement = em.MovementArchetype.MovementComponents[i];

		//if (!work.work)
		//	continue;

		//if (nav.targetHeading == 255) {
		//	movement.velocity = { 0,0 };
		//	continue;
		//}

		int8_t diff = nav.targetHeading - orientation.orientation;

		if (diff != 0) {

			if (nav.targetHeading != 255 && std::abs(diff) > rotation) {
				int s = sign(diff);
				if (std::abs(diff) > 16) 
					s = -s;
				
				orientation.orientation += s * rotation;
				orientation.orientation = (orientation.orientation + 32) % 32;
			}
			else {
				orientation.orientation = nav.targetHeading;
			}

			orientation.changed = true;
			movement.velocity = { 0,0 };
			/*	if (orientation.orientation != nav.targetHeading)
					continue;*/
		}
		else {

			const auto& position = em.PositionComponents[i];

			Vector2Int distance = Vector2Int(nav.target - position);

			if (distance.LengthSquaredInt() < velocity* velocity) {
				movement.velocity = { 0,0 };
				em.NavigationArchetype.WorkComponents[i].work = false;

				//	auto& anim = *data.animEnabled[i];
				//	anim.pause = true;
			}
			else {
				movement.velocity = Vector2Int16(movementTable32[orientation.orientation] * velocity);
			}
			 
		}
	}
}
