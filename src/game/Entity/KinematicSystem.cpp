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
	updateColliderPosData.clear();

	int size = data.size();
	for (int item = 0; item < size; ++item) {

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

	JobSystem::RunJob(updateColliderPosData.size(), JobSystem::DefaultJobSize, ColliderChangeJob);
}

void KinematicSystem::ApplyCollidersChange(EntityManager& em)
{
	for (EntityId id : em.CollisionArchetype.Archetype.RemovedEntities()) {
		int i = Entity::ToIndex(id);
		if (entityInTree[i]) {
			entityInTree[i] = false;
			collidersTree.RemoveEntity(id);
		}
	}

	int size = updateColliderPosData.size();
	for (int i = 0; i < size; ++i) {

		EntityId id = updateColliderPosData.entity[i];
		int index = Entity::ToIndex(id);
		const auto& wc = updateColliderPosData.worldCollider[i];

		if (entityInTree[index]) {
			collidersTree.UpdateEntityCollider(wc, id, updateCollider[index]);
		}
		else {
			entityInTree[index] = true;
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

			Platform::DrawRectangle(dst, Color32(c));
		}
	}
}

void KinematicSystem::MoveEntities(EntityManager& em)
{
	static const Vector2Int8 zero = { 0,0 };

	for (EntityId id : em.MovementArchetype.Archetype.GetEntities()) {
		int i = Entity::ToIndex(id);

		Vector2Int8 movement = em.MovementArchetype.MovementComponents[i].velocity;
		if (movement == zero)
			continue;

		auto& pos = em.PositionComponents[i];
		pos += Vector2Int16(em.MovementArchetype.MovementComponents[i].velocity);

		em.FlagComponents[i].set(ComponentFlags::PositionChanged, true);
	}
}

