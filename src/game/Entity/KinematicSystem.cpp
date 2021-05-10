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

static EntityManager* e;
static KinematicSystem* s;
static const EntityChangedData* d;

void KinematicSystem::ColliderChangeJob(int start, int end) {
	EntityManager& em = *e;
	const EntityChangedData& data = *d;

	for (int item = start; item < end; ++item) {

		EntityId id = data.entity[item];


		if (em.CollisionArchetype.Archetype.HasEntity(id)) {
			int i = Entity::ToIndex(id);

			auto& c = em.CollisionArchetype.ColliderComponents[i];
			c.SetPosition(data.position[item]);
			s->updateCollider[i] = s->collidersTree.GetCellIdForCollider(c.worldCollider);
		}

	}
}


void KinematicSystem::UpdateCollidersPosition(EntityManager& em, const EntityChangedData& data)
{
	SectionProfiler p2("UpdateCollidersPosition");

	e = &em;
	s = this;
	d = &data;

	JobSystem::RunJob(data.size(), JobSystem::DefaultJobSize, ColliderChangeJob);
}

void KinematicSystem::ApplyCollidersChange( EntityManager& em, const EntityChangedData& data)
{
	SectionProfiler p("ApplyCollidersChange");

	int size = data.size();
	for (int item = 0; item < size; ++item) {

		EntityId id = data.entity[item];

		if (em.CollisionArchetype.Archetype.HasEntity(id)) {
			int i = Entity::ToIndex(id);
			const auto& collider = em.CollisionArchetype.ColliderComponents[i];
			if (entityInTree[i]) {

				collidersTree.UpdateEntityCollider(collider.worldCollider, id, updateCollider[i]);
			}
			else {
				entityInTree[i] = true;
				collidersTree.AddEntity(collider.worldCollider, id, updateCollider[i]);
			}
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
