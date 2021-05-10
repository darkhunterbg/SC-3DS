#pragma once

#include "Component.h"
#include "Entity.h"
#include "../Span.h"
#include <vector>
#include "../Job.h"



class EntityManager;

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

struct Collider {
	Rectangle16 rect;
	EntityId id;
};

class KinematicSystem {
private:
	ThreadLocal<std::vector< Collider>> list;
public:

	KinematicSystem();

	void RefreshColliders(const EntityManager& em);

	//void UpdateEntities(Span<Entity> entities);
	void DrawColliders(const Camera& camera ) ;

	bool CollidesWithAny(const Rectangle16& rect, EntityId skip) ;

	//void PointCast(Vector2Int point, std::vector< EntityId>& outResults);
	//EntityId PointCast(Vector2Int point);
	//void RectCast(Rectangle rect, std::vector< EntityId>& outResults);
};