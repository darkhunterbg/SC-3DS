#pragma once

#include "Entity.h"
#include "../MathLib.h"

typedef short EntityTreeCellId;

class EntityTree {

	struct Cell {
		Rectangle16 region;
		EntityTreeCellId leafStart = 0;
		uint8_t bitShiftSize = 0;
		std::vector<Rectangle16> colliders;
		std::vector<EntityId> entities;
	};

	std::vector<Cell> cells;
	std::array<EntityTreeCellId, Entity::MaxEntities> entityToCellMap;
	std::array<unsigned short, Entity::MaxEntities> entityToCellPositionMap;

private:
	EntityTreeCellId GetCellIdForCollider(const Rectangle16& collider, EntityTreeCellId cellId) const;
	void RectCastEntity(const Rectangle16& region, EntityTreeCellId cellId, std::vector<EntityId>& result) const;
	void RectCastCollider(const Rectangle16& region, EntityTreeCellId cellId, std::vector<Rectangle16>& result) const;
	void CircleCastEntity(const Circle16& circle, EntityTreeCellId cellId, std::vector<EntityId>& result) const;

	EntityId PointCastEntity(Vector2Int16 point, EntityTreeCellId cellId) const;

	bool CollidesWithAny(const Rectangle16& region, EntityId ignore, EntityTreeCellId cellId) const;
public:
	void Init(Rectangle16 region, short minRegionSize, short cellItemReserve = 32);

	void UpdateEntityCollider(const Rectangle16& collider, EntityId id, EntityTreeCellId next);
	void AddEntity(const Rectangle16& collider, EntityId id, EntityTreeCellId cellId);
	void RemoveEntity(EntityId id);
	inline void RectCastEntity(const Rectangle16& rect, std::vector<EntityId>& result) const {
		RectCastEntity(rect, 0, result);
	}
	inline void RectCastCollider(const Rectangle16& rect, std::vector<Rectangle16>& result)  const {
		RectCastCollider(rect, 0, result);
	}
	inline void CircleCastEntity(const Circle16& circle, std::vector<EntityId>& result) const {
		CircleCastEntity(circle, 0, result);
	}
	inline EntityId PointCastEntity(Vector2Int16 point) const {
		return PointCastEntity(point, 0);
	}
	inline bool CollidesWithAny(const Rectangle16& region, EntityId ignore) const {
		return CollidesWithAny(region, ignore, 0);
	}
	inline EntityTreeCellId GetCellIdForCollider(const Rectangle16& collider) const {
		if (GetCell(0).region.Contains(collider.position))
			return GetCellIdForCollider(collider, 0);
		else
			return 0;
	}
	inline const Cell& GetCell(EntityTreeCellId cellId) const {
		return cells[cellId];
	}
	inline Cell& GetCell(EntityTreeCellId cellId) {
		return cells[cellId];
	}
};