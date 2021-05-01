#include "KinematicSystem.h"

void KinematicSystem::UpdateEntities(Span<Entity> entities)
{
	for (const Entity& entity : entities) {
		if (!entity.HasComponent<ColliderComponent>())
			continue;
		ColliderComponent& component = ColliderComponents.GetComponent(entity.id);
		component._worldBox = component._box;
		component._worldBox.position += entity.position;
	}
}


void KinematicSystem::PointCast(Vector2Int point, std::vector< EntityId>& outResults) {

	auto components = ColliderComponents.GetComponents();
	unsigned size = components.Size();
	for (unsigned i = 0; i < size; ++i) {
		ColliderComponent& cmp = components.At(i);
		if (cmp._worldBox.Contains(point))
			outResults.push_back({ ColliderComponents.GetEntityIdForComponent(i)});
	}
}

EntityId KinematicSystem::PointCast(Vector2Int point)
{
	auto components = ColliderComponents.GetComponents();
	unsigned size = components.Size();
	for (unsigned i = 0; i < size; ++i) {
		ColliderComponent& cmp = components.At(i);
		if (cmp._worldBox.Contains(point))
			return ColliderComponents.GetEntityIdForComponent(i);
	}

	return Entity::None;
}

void KinematicSystem::RectCast(Rectangle rect, std::vector< EntityId>& outResults) {
	auto components = ColliderComponents.GetComponents();
	unsigned size = components.Size();
	for (unsigned i = 0; i < size; ++i) {
		ColliderComponent& cmp = components.At(i);
		if (cmp._worldBox.Intersects(rect))
			outResults.push_back({ ColliderComponents.GetEntityIdForComponent(i) });
	}

}