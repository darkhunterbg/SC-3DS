#include "NavigationSystem.h"
#include "EntityManager.h"
#include "AnimationSystem.h"

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

void NavigationSystem::UpdateNavigation(Entity* entities, AnimationSystem& animationSystem)
{
	int i = 0;
	for (NavigationComponent& cmp : NavigationComponents.GetComponents()) {
		int cid = i++;
		if (!cmp.work)
			continue;

		EntityId id = NavigationComponents.GetEntityIdForComponent(cid);
		Entity& entity = entities[EntityIdToIndex(id)];

		Vector2Int distance = cmp.target - entity.position;
		float angle = atan2f(distance.y, distance.x) + PI / 2.0f;
		int heading = ((((int)((angle * 16) / PI) + 32) % 32) / 8) * 8;


		if (cmp.newNav || heading != entity.orientation) {


			int diff = heading - entity.orientation;

			if (diff != 0) {
				if (std::abs(diff) > cmp.turnSpeed) {
					int sign = diff > 0 ? 1 : -1;
					if (std::abs(diff) > 15)
					{
						sign = -sign;
					}

					entity.orientation += sign * cmp.turnSpeed;
					entity.orientation = (entity.orientation + 32) % 32;

				}
				else {
					entity.orientation = heading;
				}
			}

			cmp.newNav = false;


			if (entity.HasComponent<AnimationComponent>()) {
				animationSystem.AnimationComponents.GetComponent(entity.id)
					.PlayClip(cmp.clips[entity.orientation]);
			}
			printf("Heading changed to %i\n", entity.orientation);

			if (entity.orientation != heading)
				continue;
		}



		if (distance.LengthSquared() < cmp.velocity * cmp.velocity) {
			entity.position = cmp.target;
			cmp.work = false;

			if (entity.HasComponent<AnimationComponent>()) {
				animationSystem.AnimationComponents.GetComponent(entity.id)
					.pause = true;
			}
		}
		else {
			Vector2Int move = movementTable[heading] * cmp.velocity;
			entity.position += move;
		}
		entity.changed = true;
	}

}
