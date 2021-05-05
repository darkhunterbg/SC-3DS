#pragma once

#include "Entity.h"
#include "Component.h"
#include <vector>

struct RenderTest : IComponent<0> {
	Vector2Int _dst;
	Vector2Int offset = { 0,0 };

	Vector2Int _shadowDst;
	Vector2Int shadowOffset = { 0,0 };

	inline void SetFrame(const SpriteFrame& frame) {
		Vector2Int oldOffset = offset;
		offset = frame.offset;
		_dst += frame.offset - oldOffset;
	}

	inline void SetShadowFrame(const SpriteFrame& frame) {
		Vector2Int oldOffset = shadowOffset;
		shadowOffset = frame.offset;
		_shadowDst += frame.offset - oldOffset;
	}
};




//struct RenderArchetype {
//	EntityId entityId[Entity::MaxEntities];
//	TransformComponent transform[Entity::MaxEntities];
//	RenderTest render[Entity::MaxEntities];
//};

struct RenderPositionArchetype {
	std::vector<Vector2Int> position;
	std::vector<
};