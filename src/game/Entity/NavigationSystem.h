#pragma once

#include "Component.h"
#include <vector>

class EntityManager;

class NavigationSystem {

	struct NavigationData {
		std::vector<NavigationComponent*> navigation;
		std::vector<Vector2Int16> position;
		std::vector<uint8_t> velocity;
		std::vector<Rectangle16> collider;
		std::vector<EntityId> entity;

		inline void clear() {
			navigation.clear();
			position.clear();
			velocity.clear();
			collider.clear();
			entity.clear();
		}

		inline size_t size() const {
			return navigation.size();
		}
	};
private:
	NavigationData navigationData;

	std::array<uint32_t, (256 * 256) / 8> navGrid;
	Vector2Int16 gridSize;

	std::vector<EntityId> applyNav;

	static void UpdateNavigationJob(int start, int end);
	static void ApplyUnitNavigationJob(int start, int end);


public:

	inline bool IsPassable(int x, int y) {
		if (x < 0 || x > gridSize.x || y < 0 || y> gridSize.y)
			return false;

		int i = ((x >> 3) + (y * gridSize.x)) >> 3;
		uint32_t var = navGrid[i];

		bool result = var & (1 << (x >> 3));
		return !result;
	}

	inline void SetPassable(int x, int y) {
		int i = ((x >> 3) + (y * gridSize.x)) >> 3;
		uint32_t& var = navGrid[i];
		var |= (1 << (x >> 3));
	}
	inline void SetUnpassable(int x, int y) {
		int i = ((x >> 3) + (y * gridSize.x)) >> 3;
		uint32_t& var = navGrid[i];
		var &= ~(1 << (x >> 3));
	}


	void SetSize(Vector2Int16 size);

	void UpdateNavigation(EntityManager& em);

	void ApplyUnitNavigaion(EntityManager& em);

	void UpdateNavGrid(EntityManager& em);

};