#pragma once

#include <vector>
#include "../Camera.h"
#include "Entity.h"
#include "Component.h"
#include "../Assets.h"
#include "../Span.h"
#include "../Color.h"
#include "../Platform.h"

struct RenderComponent : IComponent<0> {
	int depth = 0;
	Vector2Int _dst;
	Image sprite;
	Vector2Int offset = { 0,0 };
	bool hFlip = false;

	Vector2Int _shadowDst;
	Image shadowSprite;
	Vector2Int shadowOffset = { 0,0 };

	Color4 unitColor;
	Image colorSprite;

	inline void SetSprite(const Sprite& s) {
		sprite = s.image;
	}
	inline void SetFrame(const SpriteFrame& frame) {
		sprite = frame.sprite.image;
		Vector2Int oldOffset = offset;
		offset = frame.offset;
		_dst += frame.offset - oldOffset;
		hFlip = frame.hFlip;
	}

	inline void SetShadowFrame(const SpriteFrame& frame) {
		shadowSprite = frame.sprite.image;
		Vector2Int oldOffset = shadowOffset;
		shadowOffset = frame.offset;
		_shadowDst += frame.offset - oldOffset;
	}
};


class RenderSystem {

private:
	std::vector< BatchDrawCommand> render;

	static bool RenderSort(const BatchDrawCommand& a, const BatchDrawCommand& b);
public:
	ComponentCollection<RenderComponent> RenderComponents;
	void Draw(const Camera& camera);
	void UpdateEntities(const Span<Entity> entity);
};