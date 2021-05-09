#pragma once

#include "Component.h"
#include "Entity.h"
#include "../Span.h"
#include <vector>


//struct ColliderComponent  {
//	Rectangle _worldBox;
//	Rectangle _box;
//
//	void SetBox(const Rectangle& b) {
//		Vector2Int p = _worldBox.position - _box.position;
//		_worldBox = _box = b;
//		_worldBox.position += p;
//	}
//};

class Camera;

class KinematicSystem {
public:
	//ComponentCollection<ColliderComponent> ColliderComponents;

	void UpdateEntities(Span<Entity> entities);
	void DrawColliders(const Camera& camera);

	void PointCast(Vector2Int point, std::vector< EntityId>& outResults);
	EntityId PointCast(Vector2Int point);
	void RectCast(Rectangle rect, std::vector< EntityId>& outResults);
};