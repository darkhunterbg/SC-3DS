#include "KinematicSystem.h"
#include "EntityManager.h"
#include "../Camera.h"
#include "../Engine/GraphicsRenderer.h"

void KinematicSystem::NewCollider(EntityId id, const Rectangle16& collider)
{
	_colliderComponents.NewComponent(id);
	_colliderComponents.GetComponent(id).collider = collider;

	// TODO: sync add with _worldColliders;
}

void KinematicSystem::RemoveCollider(EntityId id)
{
	_colliderComponents.DeleteComponent(id);

	// TODO: sync delete with _worldColliders;
}

void KinematicSystem::UpdateColliders(EntityManager& em)
{
	_worldColliders.clear();

	auto& entites = _colliderComponents.GetEntities();
	auto& components = _colliderComponents.GetComponents();
	for (int i = 0; i < entites.size(); ++i)
	{
		EntityId e = entites[i];
		const ColliderComponent& collider = components[i];
		Rectangle16 worldCollider = collider.collider;
		worldCollider.position += em.GetPosition(e);

		_worldColliders.push_back(worldCollider);
	}
}

void KinematicSystem::DeleteEntities(std::vector<EntityId>& entities)
{
	_colliderComponents.DeleteComponents(entities);

	// TODO: sync delete with _worldColliders;
}

size_t KinematicSystem::ReportMemoryUsage()
{
	size_t mem =_colliderComponents.GetMemoryUsage();

	mem += _worldColliders.capacity() * sizeof(Rectangle16);
	return mem;
}

void KinematicSystem::DrawColliders(const Camera& camera)
{
	if (!ShowColliders) return;

	Rectangle16 camRect = camera.GetRectangle16();
	Color c = Colors::LightGreen;
	c.a = 0.5f;

	for (Rectangle16& collider : _worldColliders)
	{
		if (camRect.Intersects(collider))
		{

			Rectangle dst = { Vector2Int(collider.position), Vector2Int(collider.size) };
			dst.position -= Vector2Int(camRect.position);
			dst.position /= camera.Scale;
			dst.size /= camera.Scale;

			GraphicsRenderer::DrawRectangle(dst, Color32(c));
		}
	}
}

EntityId KinematicSystem::PointCast(Vector2Int16 point) const
{
	int i = 0;
	const auto& entities = _colliderComponents.GetEntities();

	for (const Rectangle16& rect : _worldColliders)
	{
		if (rect.Contains(point)) return entities[i];
		++i;
	}

	return Entity::None;
}

void KinematicSystem::RectCast(const Rectangle16& collider, std::vector<EntityId>& result) const
{
	int i = 0;
	const auto& entities = _colliderComponents.GetEntities();

	for (const Rectangle16& rect : _worldColliders)
	{
		if (rect.Intersects(collider)) result.push_back(entities[i]);
		++i;
	}
}

void KinematicSystem::CircleCast(const Circle16& circle, std::vector<EntityId>& result) const
{
	int i = 0;
	const auto& entities = _colliderComponents.GetEntities();

	for (const Rectangle16& rect : _worldColliders)
	{
		if (rect.Intersects(circle)) result.push_back(entities[i]);
		++i;
	}
}
