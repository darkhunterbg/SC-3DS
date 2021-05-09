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
		std::vector<MovementComponent> movement;

		inline void clear() {
			navigation.clear();
			position.clear();
			movement.clear();
		}

		inline size_t size() const {
			return navigation.size();
		}
	};
private:
	MovementData movementData;
	MovementAnimData movementAnimData;
	NavigationData navigationData;

	static void UpdateNavigationJob(int start, int end);
	static void MoveEntitiesJob(int start, int end);
	static void SetMovementAnimJob(int start, int end);
public:
	void UpdateNavigation(EntityManager& em);
	void MoveEntities(EntityManager& em);
};