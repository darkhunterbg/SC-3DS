#pragma once

#include "EntityComponentMap.h"
#include "../Assets.h"
#include "../Color.h"
#include "../Engine/GraphicsPrimitives.h"
#include "IEntitySystem.h"

#include <vector>

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

class ObjectDrawSystem : public IEntitySystem {
	EntityComponentMap<DrawComponent> _drawComponents;
	std::vector<BatchDrawCommand> _drawCmd;

public:
	ObjectDrawSystem();

	void UpdatePositions(EntityManager& em);
	void Draw( const Camera& camera);

	inline void NewComponent(EntityId id) { _drawComponents.NewComponent(id); }
	inline bool HasComponent(EntityId id) const
	{
		return _drawComponents.HasComponent(id);
	}
	inline void RemoveComponent(EntityId id)
	{
		_drawComponents.DeleteComponent(id);
	}
	DrawComponent& GetComponent(EntityId id) { return _drawComponents.GetComponent(id); }

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

	// Inherited via IEntitySystem
	virtual void DeleteEntities(std::vector<EntityId>& entities) override;
	virtual size_t ReportMemoryUsage() override;
};