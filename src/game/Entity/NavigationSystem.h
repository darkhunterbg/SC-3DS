#pragma once

#include "Component.h"
#include <vector>
#include "../Job.h"

typedef std::array<Vector2Int16, 32> MovementVelocityTable;

class EntityManager;

struct AStarAction {
	Vector2Int16 pos;
	int iter;
	int value;
	uint8_t dir;
	uint8_t startDir;
};
class NavigationSystem {



	struct MovementData {
		std::vector<NavigationWorkComponent*> work;
		std::vector<MovementComponent> movement;
		std::vector<OrientationComponent*> orientation;
		std::vector<Vector2Int16*> position;
		std::vector<NavigationComponent> navigation;
		std::vector<EntityChangeComponent*> changed;

		inline void clear() {
			work.clear();
			movement.clear();
			orientation.clear();
			position.clear();
			navigation.clear();
			changed.clear();
		}

		inline size_t size() const {
			return work.size();
		}
	};

	struct MovementAnimData {
		std::vector<MovementComponent> movement;
		std::vector<OrientationComponent*> orientation;
		std::vector<UnitComponent> unit;
		std::vector<AnimationEnableComponent*> animEnabled;
		std::vector<AnimationTrackerComponent*>animTracker;
		std::vector<AnimationComponent*> anim;

		inline void clear() {
			movement.clear();
			orientation.clear();
			unit.clear();
			animEnabled.clear();
			animTracker.clear();
			anim.clear();
		}

		inline size_t size() const {
			return movement.size();
		}
	};

	struct NavigationData {
		std::vector<NavigationComponent*> navigation;
		std::vector<Vector2Int16> position;
		std::vector<uint8_t> velocity;
		std::vector<EntityId> entities;

		inline void clear() {
			navigation.clear();
			position.clear();
			velocity.clear();
			entities.clear();
		}

		inline size_t size() const {
			return navigation.size();
		}
	};
private:
	MovementData movementData;
	NavigationData navigationData;

	std::vector<EntityId> applyNav;

	static void UpdateNavigationJob(int start, int end);

	ThreadLocal<std::vector<AStarAction>>* tlsResults;

public:
	NavigationSystem() {
		tlsResults = new ThreadLocal<std::vector<AStarAction>>();
	}

	void UpdateNavigation(EntityManager& em);

	void ApplyUnitNavigaion(EntityManager& em);
};