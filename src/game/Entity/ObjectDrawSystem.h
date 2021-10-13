#pragma once

#include "Entity.h"
#include "../Assets.h"
#include "../Color.h"
#include "../Engine/GraphicsPrimitives.h"
#include <vector>
#include <array>

class Camera;
class EntityManager;

struct DrawComponent
{
	ImageFrame sprite;
	const ImageFrame* colorMask = nullptr;
	Color32 color = 0xFFFFFFFF;
	int8_t depth = 0;
	bool hFlip = false;
	Vector2Int16 position;
	Vector2Int16 offset;
	Rectangle16 boundingBox;
	const ImageFrame* shadow = nullptr;
	Vector2Int16 shadowOffset = { 0,0 };
	bool visible = true;
};

class ObjectDrawSystem {
	std::vector<DrawComponent> _drawComponents;
	std::vector<BatchDrawCommand> _drawCmd;
	std::array<short, Entity::MaxEntities> _entityToIndexMap;
	EntityId _maxEntity = Entity::None;

	DrawComponent* TryGetComponent(EntityId id);
public:
	ObjectDrawSystem();

	void Draw(EntityManager& em, const Camera& camera);

	void NewComponent(EntityId id);
	inline bool HasComponent(EntityId id) const { return _entityToIndexMap[id] != -1; }

	void RemoveComponent(EntityId id);

	inline void InitFromImage(EntityId id, const Image& image)
	{
		auto& draw = GetComponent(id);
		draw.boundingBox = { Vector2Int16(0,0), image.GetSize() };
	}

	inline void SetSprite(EntityId id, const Image& img, int frame, bool flipped)
	{
		auto& draw = GetComponent(id);
		draw.sprite = img.GetFrame(frame);
		draw.hFlip = flipped;
		draw.offset = img.GetImageFrameOffset(frame, flipped);
	}
	inline void SetSpriteWithColor(EntityId id, const Image& img, int frame, bool flipped)
	{
		auto& draw = GetComponent(id);
		draw.sprite = img.GetFrame(frame);
		draw.hFlip = flipped;
		draw.offset = img.GetImageFrameOffset(frame, flipped);
		draw.colorMask = img.GetColorMaskFrame(frame);
	}
	inline void SetSpriteWithColorShadow(EntityId id, const Image& img, const Image& shadowImg, int frame, bool flipped)
	{
		auto& draw = GetComponent(id);
		draw.sprite = img.GetFrame(frame);
		draw.hFlip = flipped;
		draw.offset = img.GetImageFrameOffset(frame, flipped);
		draw.colorMask = img.GetColorMaskFrame(frame);
		draw.shadow = &shadowImg.GetFrame(frame);
		draw.shadowOffset = shadowImg.GetImageFrameOffset(frame, flipped);
	}

	DrawComponent& GetComponent(EntityId id);
};