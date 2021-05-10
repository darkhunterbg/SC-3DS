#include "KinematicSystem.h"
#include "EntityManager.h"
#include "Camera.h"
#include "../Profiler.h"

#include <vector>


KinematicSystem::KinematicSystem() {

	memset(entityInTree.data(), false, Entity::MaxEntities * sizeof(bool));
	memset(updateCollider.data(), 0, Entity::MaxEntities * sizeof(short));
}

void KinematicSystem::SetSize(Vector2Int16 size)
{
	collidersTree.Init({ Vector2Int16{0,0},size }, 8 * 32);
}

static KinematicSystem* s;

void KinematicSystem::ColliderChangeJob(int start, int end) {

	UpdateColliderPosData& data = s->updateColliderPosData;

	for (int i = start; i < end; ++i) {
		int index = Entity::ToIndex(data.entity[i]);

		const auto& collider = data.collider[i];
		const auto& pos = data.position[i];
		auto& wc = data.worldCollider[i];
		wc = collider;
		wc.position += pos;

		s->updateCollider[index] = s->collidersTree.GetCellIdForCollider(wc);
	}
}


void KinematicSystem::UpdateCollidersPosition(EntityManager& em, const EntityChangedData& data)
{
	SectionProfiler p2("UpdateCollidersPosition");

	updateColliderPosData.clear();

	for (int item = 0; item < data.size(); ++item) {

		EntityId id = data.entity[item];

		if (em.CollisionArchetype.Archetype.HasEntity(id)) {
			int i = Entity::ToIndex(id);

			updateColliderPosData.entity.push_back(id);
			updateColliderPosData.collider.push_back(em.CollisionArchetype.ColliderComponents[i].collider);
			updateColliderPosData.position.push_back(data.position[item]);
			updateColliderPosData.worldCollider.push_back({});
		}
	}

	s = this;

	JobSystem::RunJob(data.size(), JobSystem::DefaultJobSize, ColliderChangeJob);
}

void KinematicSystem::ApplyCollidersChange(EntityManager& em)
{
	SectionProfiler p("ApplyCollidersChange");

	int size = updateColliderPosData.size();
	for (int i = 0; i < size; ++i) {

		EntityId id = updateColliderPosData.entity[i];
		int index = Entity::ToIndex(id);
		const auto& wc = updateColliderPosData.worldCollider[i];

		if (entityInTree[i]) {
			collidersTree.UpdateEntityCollider(wc, id, updateCollider[index]);
		}
		else {
			entityInTree[i] = true;
			collidersTree.AddEntity(wc, id, updateCollider[index]);
		}
	}
}


void KinematicSystem::DrawColliders(const Camera& camera) {

	drawColliders.clear();

	Rectangle16 camRect = camera.GetRectangle16();
	Color c = Colors::LightGreen;
	c.a = 0.5f;

	collidersTree.RectCastCollider(camRect, drawColliders);

	for (const Rectangle16& collider : drawColliders) {
		if (camRect.Intersects(collider)) {

			Rectangle dst = { Vector2Int(collider.position), Vector2Int(collider.size) };
			dst.position -= Vector2Int(camRect.position);
			dst.position /= camera.Scale;
			dst.size /= camera.Scale;

			Platform::DrawRectangle(dst, c);
		}
	}

}

bool KinematicSystem::CollidesWithAny(const Rectangle16& collider, EntityId skip)
{
	auto& l = list.Get();
	l.clear();

	collidersTree.RectCastEntity(collider, l);

	for (const auto& c : l) {
		if (c != skip)
			return true;
	}

	return false;
}
