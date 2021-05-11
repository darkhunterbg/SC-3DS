#pragma once

#include "Component.h"
#include <vector>

class EntityManager;

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

	struct NavigationData {
		std::vector<NavigationComponent*> navigation;
		std::vector<Vector2Int16> position;
		std::vector<uint8_t> velocity;
		std::vector<EntityId> entity;

		inline void clear() {
			navigation.clear();
			position.clear();
			velocity.clear();
			entity.clear();
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
	static void ApplyUnitNavigationJob(int start, int end);
public:
	void UpdateNavigation(EntityManager& em);

	void ApplyUnitNavigaion(EntityManager& em);
};