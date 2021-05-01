#pragma once

#include <vector>
#include "../Camera.h"
#include "Entity.h"
#include "Component.h"
#include "../Assets.h"
#include "../Span.h"


struct RenderComponent : IComponent<0> {
	int depth = 0;
	Rectangle _dst;
	Sprite sprite;
	Vector2Int offset = { 0,0 };
	bool hFlip = false;

	inline void SetSprite(const Sprite& s) {
		_dst.size = s.rect.size;
		sprite = s;
	}
	inline void SetFrame(const SpriteFrame& frame) {
		sprite = frame.sprite;
		Vector2Int oldOffset = offset;
		offset = frame.offset;
		_dst.position += frame.offset - oldOffset;
		_dst.size = frame.sprite.rect.size;
		hFlip = frame.hFlip;
	}

	inline void AddOffset(const Vector2Int& o) {
		_dst.position += o;
		offset += o;
	}
};

class RenderSystem {
	struct Render {
		int order;
		Sprite sprite;
		Rectangle dst;
		bool hFlip;
	};
private:
	std::vector< Render> render;
	static bool RenderSort(const Render& a, const Render& b);
public:
	ComponentCollection<RenderComponent> RenderComponents;
	void Draw(const Camera& camera);
	void UpdateEntities(const Span<Entity> entity);
};