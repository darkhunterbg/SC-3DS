#include "KinematicSystem.h"
#include "EntityManager.h"
#include "Camera.h"
#include "../Profiler.h"

#include <vector>



struct CollisionTreeCell {
	Rectangle16 region;
	short leafStart = 0;
	std::vector<Collider> colliders;
};

class CollisionTree {
	std::vector<CollisionTreeCell> cells;
	std::array<short, Entity::MaxEntities> entityCell;

	short GetGetCellForCollider(const Rectangle16& collider, short cellId) {
		auto& cell = GetCell(cellId);
		if (!cell.leafStart)
			return cellId;


		short lastIntersection = -1;
		int intersections = 0;

		for (short i = cell.leafStart; i < cell.leafStart + 4; ++i) {
			CollisionTreeCell& cell = GetCell(i);
			if (cell.region.Intersects(collider)) {
				++intersections;
				lastIntersection = i;
			}
		}

		if (intersections > 1) {
			return cellId;
		}
		else {
			return GetGetCellForCollider(collider, lastIntersection);
		}
	}

	short GetGetCellForCollider(const Rectangle16& collider) {
		return GetGetCellForCollider(collider, 0);
	}

	void AddColliderToCell(const Rectangle16& collider, EntityId id, short cellId) {

		auto addToCellId = GetGetCellForCollider(collider);
		entityCell[Entity::ToIndex(id)] = addToCellId;
		GetCell(addToCellId).colliders.push_back({ collider, id });
	}

	void RectCastCell(const Rectangle16& region, const CollisionTreeCell& cell, std::vector<Collider>& result) {
		if (region.Intersects(cell.region)) {
			auto end = cell.colliders.end();
			for (auto i = cell.colliders.begin(); i != end; ++i)
			{
				if (i->rect.Intersects(region))
					result.push_back(*i);
			}

			if (cell.leafStart)
			{
				for (int i = 0; i < 4; ++i) {
					RectCastCell(region, GetCell(cell.leafStart + i), result);
				}
			}
		}
	}
public:
	void Init(Rectangle16 region, short minRegionSize) {
		cells.clear();

		std::vector<short> subdivide = { 0 };

		cells.push_back({ region,0 });

		while (subdivide.size() > 0) {
			auto i = *subdivide.begin();
			region = cells[i].region;
			cells[i].colliders.reserve(32);

			subdivide.erase(subdivide.begin());

			if (region.size.x <= minRegionSize ||
				region.size.y <= minRegionSize)
				continue;


			cells[i].leafStart = cells.size();
			int x = 0;
			for (int x = 0; x < 4; ++x) {
				subdivide.push_back(cells[i].leafStart + x);
			}
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

	inline CollisionTreeCell& GetCell(short cellId) {
		return cells[cellId];
	}

	void AddCollider(const Rectangle16& collider, EntityId id) {
		auto& cell = GetCell(0);
		if (cell.region.Intersects(collider)) {
			AddColliderToCell(collider, id, 0);
		}
		else {
			EXCEPTION("Collider %i %i %i %i is out of bounds!",
				collider.GetMin().x, collider.GetMin().y, collider.GetMax().x, collider.GetMax().y);
		}
	}

	void UpdateCollider(const Rectangle16& collider, EntityId id) {
		short current = entityCell[Entity::ToIndex(id)];
		short next = GetGetCellForCollider(collider);

		if (current != next) {
			auto& cell = GetCell(current);
			for (auto i = cell.colliders.begin(); i != cell.colliders.end(); ++i) {
				if (i->id == id) {
					cell.colliders.erase(i);
					break;
				}
			}

			GetCell(next).colliders.push_back({ collider, id });
			entityCell[Entity::ToIndex(id)] = next;
		}
		else {
			auto& cell = GetCell(current);
			for (auto i = cell.colliders.begin(); i != cell.colliders.end(); ++i) {
				if (i->id == id) {
					i->rect = collider;
					break;
				}
			}
		}
	}

	void RectCast(const Rectangle16& rect, std::vector<Collider>& result) {
		RectCastCell(rect, GetCell(0), result);
	}
};

static CollisionTree tree;

static std::array<bool, Entity::MaxEntities> inTree;

KinematicSystem::KinematicSystem() {
	tree.Init({ {0,0},{256 * 32,256 * 32} }, 2 * 32);
	memset(inTree.data(), false, Entity::MaxEntities * sizeof(bool));
}

void KinematicSystem::RefreshColliders(const EntityManager& em)
{
	SectionProfiler p("ColliderRefresh");

	for (EntityId id : em.CollisionArchetype.Archetype.GetEntities()) {
		int  i = Entity::ToIndex(id);
		if (inTree[i]) {
			if (em.EntityChangeComponents[i].changed) {
				tree.UpdateCollider(em.CollisionArchetype.ColliderComponents[i].worldCollider, id);
			}
		}
		else {
			inTree[i] = true;
			tree.AddCollider(em.CollisionArchetype.ColliderComponents[i].worldCollider, id);
		}
	}
}


void KinematicSystem::DrawColliders(const Camera& camera) {

	auto& l = list.Get();
	l.clear();

	Rectangle16 camRect = camera.GetRectangle16();
	Color c = Colors::LightGreen;
	c.a = 0.5f;

	tree.RectCast(camRect, l);

	for (const Collider& collider : l) {
		if (camRect.Intersects(collider.rect)) {

			Rectangle dst = { Vector2Int(collider.rect.position), Vector2Int(collider.rect.size) };
			dst.position -= Vector2Int(camRect.position);
			dst.position /= camera.Scale;
			dst.size /= camera.Scale;

			Platform::DrawRectangle(dst, c);
		}
	}

}

bool KinematicSystem::CollidesWithAny(const Rectangle16& collider, EntityId skip)
{
	//return tree.CollidesWithAny(collider, skip);
	auto& l = list.Get();
	l.clear();


	tree.RectCast(collider, l);

	for (const auto& c : l) {
		if (c.id != skip)
			return true;
	}

	return false;
}
