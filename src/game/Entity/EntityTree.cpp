#include "EntityTree.h"
#include "../Debug.h"
#include <cstring>

void EntityTree::Init(Rectangle16 region, short minRegionSize, short cellItemReserve) {
	cells.clear();

	cells.reserve(10000);

	memset(entityToCellMap.data(), 0, sizeof(EntityTreeCellId) * entityToCellMap.size());

	std::vector<EntityTreeCellId> subdivide;
	subdivide.reserve(1000);
	subdivide.push_back(0);

	cells.push_back({ region,0 });

	while (subdivide.size() > 0) {
		auto i = *subdivide.begin();
		region = cells[i].region;
		cells[i].colliders.reserve(cellItemReserve);
		cells[i].entities.reserve(cellItemReserve);

		subdivide.erase(subdivide.begin());

		if (region.size.x <= minRegionSize ||
			region.size.y <= minRegionSize)
			continue;

		cells[i].leafStart = cells.size();

		EntityTreeCellId x = cells[i].leafStart;

		subdivide.push_back(x++);
		subdivide.push_back(x++);
		subdivide.push_back(x++);
		subdivide.push_back(x);

		region.size /= 2;

		cells.push_back({ region });
		region.position.x += region.size.x;
		cells.push_back({ region });
		region.position.x -= region.size.x;
		region.position.y += region.size.y;
		cells.push_back({ region });
		region.position.x += region.size.x;
		cells.push_back({ region });
	}

}

void EntityTree::UpdateEntityCollider(const Rectangle16& collider, EntityId id, EntityTreeCellId next) {
	short current = entityToCellMap[Entity::ToIndex(id)];
	auto& cell = GetCell(current);
	int size = cell.colliders.size();

	if (current != next) {

		int i = entityToCellPositionMap[Entity::ToIndex(id)];

		cell.colliders.erase(cell.colliders.begin() + i);
		cell.entities.erase(cell.entities.begin() + i);
		--size;
		for (; i < size; ++i) {
			EntityId id = cell.entities[i];
			entityToCellPositionMap[Entity::ToIndex(id)]--;
		}

		auto& newCell = GetCell(next);

		newCell.colliders.push_back(collider);
		newCell.entities.push_back(id);
		entityToCellMap[Entity::ToIndex(id)] = next;
		entityToCellPositionMap[Entity::ToIndex(id)] = newCell.entities.size() - 1;
	}
	else {

		int i = entityToCellPositionMap[Entity::ToIndex(id)];
		cell.colliders[i] = collider;
	}
}

void EntityTree::AddEntity(const Rectangle16& collider, EntityId id, EntityTreeCellId cellId) {
	auto& cell = GetCell(cellId);
	cell.colliders.push_back(collider);
	cell.entities.push_back(id);
	entityToCellMap[Entity::ToIndex(id)] = cellId;
	entityToCellPositionMap[Entity::ToIndex(id)] = cell.entities.size() - 1;
}

EntityTreeCellId EntityTree::GetCellIdForCollider(const Rectangle16& collider, EntityTreeCellId cellId) const {
	const Cell& cell = GetCell(cellId);
	if (!cell.leafStart)
		return cellId;

	EntityTreeCellId lastIntersection = -1;
	int intersections = 0;

	for (int i = cell.leafStart; i < cell.leafStart + 4; ++i) {
		const Cell& subCell = GetCell(i);
		if (subCell.region.Intersects(collider)) {
			++intersections;
			lastIntersection = i;
		}

	}

	if (intersections > 1) {
		return cellId;
	}
	else {

		if (lastIntersection < 0)
			return 0;

		return GetCellIdForCollider(collider, lastIntersection);
	}
}

void EntityTree::RectCastEntity(const Rectangle16& region, EntityTreeCellId cellId, std::vector<EntityId>& result) const {
	auto& cell = GetCell(cellId);

	if (region.Intersects(cell.region)) {
		auto end = cell.colliders.size();

		for (unsigned i = 0; i < end; ++i) {
			if (cell.colliders[i].Intersects(region)) {
				result.push_back(cell.entities[i]);
			}
		}

		if (cell.leafStart)
		{
			for (int i = 0; i < 4; ++i) {
				RectCastEntity(region, cell.leafStart + i, result);
			}
		}
	}
}
void EntityTree::RectCastCollider(const Rectangle16& region, EntityTreeCellId cellId, std::vector<Rectangle16>& result) const {
	auto& cell = GetCell(cellId);

	if (region.Intersects(cell.region)) {
		auto end = cell.colliders.size();

		for (unsigned i = 0; i < end; ++i) {
			if (cell.colliders[i].Intersects(region)) {
				result.push_back(cell.colliders[i]);
			}
		}

		if (cell.leafStart)
		{
			for (int i = 0; i < 4; ++i) {
				RectCastCollider(region, cell.leafStart + i, result);
			}
		}
	}
}

EntityId EntityTree::PointCastEntity(Vector2Int16 point, EntityTreeCellId cellId) const
{
	auto& cell = GetCell(cellId);

	if (cell.region.Contains(point)) {
		auto end = cell.colliders.size();

		for (unsigned i = 0; i < end; ++i) {
			if (cell.colliders[i].Contains(point)) {
				return cell.entities[i];
			}
		}

		if (cell.leafStart)
		{
			for (int i = 0; i < 4; ++i) {
				EntityId id = PointCastEntity(point, cell.leafStart + i);
				if (id != Entity::None)
					return id;
			}
		}
	}

	return Entity::None;
}
